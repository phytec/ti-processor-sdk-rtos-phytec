/*
*
* Copyright (c) {2015 - 2020} Texas Instruments Incorporated
*
* All rights reserved not granted herein.
*
* Limited License.
*
* Texas Instruments Incorporated grants a world-wide, royalty-free, non-exclusive
* license under copyrights and patents it now or hereafter owns or controls to make,
* have made, use, import, offer to sell and sell ("Utilize") this software subject to the
* terms herein.  With respect to the foregoing patent license, such license is granted
* solely to the extent that any such patent is necessary to Utilize the software alone.
* The patent license shall not apply to any combinations which include this software,
* other than combinations with devices manufactured by or for TI ("TI Devices").
* No hardware patent is licensed hereunder.
*
* Redistributions must preserve existing copyright notices and reproduce this license
* (including the above copyright notice and the disclaimer and (if applicable) source
* code license limitations below) in the documentation and/or other materials provided
* with the distribution
*
* Redistribution and use in binary form, without modification, are permitted provided
* that the following conditions are met:
*
* *       No reverse engineering, decompilation, or disassembly of this software is
* permitted with respect to any software provided in binary form.
*
* *       any redistribution and use are licensed by TI for use only with TI Devices.
*
* *       Nothing shall obligate TI to provide you with source code for the software
* licensed and provided to you in object code.
*
* If software source code is provided to you, modification and redistribution of the
* source code are permitted provided that the following conditions are met:
*
* *       any redistribution and use of the source code, including any resulting derivative
* works, are licensed by TI for use only with TI Devices.
*
* *       any redistribution and use of any object code compiled from the source code
* and any resulting derivative works, are licensed by TI for use only with TI Devices.
*
* Neither the name of Texas Instruments Incorporated nor the names of its suppliers
*
* may be used to endorse or promote products derived from this software without
* specific prior written permission.
*
* DISCLAIMER.
*
* THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY EXPRESS
* OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL TI AND TI'S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
* OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
* OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <cmath>
#include <stdlib.h>
#include <stdarg.h>
#include <string>
#include <list>
#include <vector>

#include "ti_dl.h"
#include "tidl_import_api.h"
#include "tidl_import_config.h"
#include "tidl_import_common.h"
#include "tidl_deviceInfo.h"
#include "tidl_import_common_tidlrt_osrt.h"

#define TIDL_SIMD_WIDTH (64)
#define MODEL_CHECK_CONV_SUGGESTIONS 0
static bool is16bit = false;
extern "C"{
std::vector<std::string> diagsInfo;
}
// This class represents a warning or error diagnostic. It can apply to a single layer
// or the whole model.
class TIDL_ModelDiagnostic
{
public:
  enum Kind
  {
    DK_Supported,     // Fully supported
    DK_Info,          // Fully supported, informational message
    DK_NotPerformant, // Supported, but may not perform well
    DK_PCOnly,        // Supported only for PC host emulation
    DK_NotVerified,   // Supported, but not verified
    DK_NotSupported,  // Not supported (error)
  };

  // Construct a diagnostic, with printf-style formatting
  TIDL_ModelDiagnostic(Kind k, const char* fmt, ...);
  // Emit diagnostic to stdout
  void emit() const;
  // True if diagnostic represents a fatal error (model will not run on TIDL)
  bool isError() const;
  // True if node should part of subgraph offloaded to TIDL
  bool isOKToOffload() const;
  //Extract message from the diagnostic
  std::pair<const char *, std::string> extractDiagMsg();
private:
   const char* prefix() const;
   Kind kind;
   std::string msg;
};

using DiagList_t = std::list<TIDL_ModelDiagnostic>;

void tidlModelCheckLayer(const sTIDL_LayerPC_t &layerPC, DiagList_t &diags);

// Construct a diagnostic using printf-style formatting
TIDL_ModelDiagnostic::TIDL_ModelDiagnostic(TIDL_ModelDiagnostic::Kind k, const char* fmt, ...) : kind(k)
{
  va_list args;
  va_start(args, fmt);
  size_t len = vsnprintf(nullptr, 0, fmt, args);
  va_end(args);

  va_start(args, fmt);
  std::vector<char> buf(len+1);
  len = vsnprintf(buf.data(), buf.size(), fmt, args);
  va_end(args);
  msg = std::string(buf.data(), len);
}

// Emit diagnostic to stdout
void TIDL_ModelDiagnostic::emit() const
{
  if (kind == DK_Supported)
    return;
  printf("%s: ", prefix());
  printf("%s\n", msg.c_str());
}

const char* TIDL_ModelDiagnostic::prefix() const
{
  switch (kind)
  {
    default:               return "UNKNOWN ERROR";
    case DK_Supported:     return "";
    case DK_NotSupported:  return "TIDL ALLOWLISTING LAYER CHECK";
    case DK_PCOnly:
    case DK_NotVerified:   return "WARNING";
    case DK_NotPerformant: return "SUGGESTION";
    case DK_Info:          return "INFORMATION";
  }
}

std::pair<const char *, std::string> TIDL_ModelDiagnostic::extractDiagMsg()
{
  return std::make_pair(prefix(), msg);
}

// Return true if this diagnostic represents a condition that will prevent the network
// from operating properly on TIDL.
bool TIDL_ModelDiagnostic::isError() const
{
  return kind == DK_NotSupported;
}

// Return true if this condition should not prevent offloading this node to TIDL as
// part of a subgraph.
bool TIDL_ModelDiagnostic::isOKToOffload() const
{
  switch (kind)
  {
    default:
    case DK_NotSupported:
    case DK_PCOnly:
    case DK_NotVerified:   return false;

    case DK_Supported:
    case DK_NotPerformant:
    case DK_Info:          return true;
  }
}

/** Common across tflite RT and ONNX RT
 * ONNX RT Layer -- Add / Sum
 * Tflite RT Layer -- kTfLiteBuiltinAdd
 */
int32_t checkDimConstTensor (sTIDL_LayerPC_t &layer)
{
  //this function checks if the constant input is a vector or not
  sTIDL_allowlistingMetaData md = layer.allowlistingMetaData;
  int32_t nonSingletonDimNum = 0, constDim = 0;
  if (md.numConstInputs == 0) return 0;
  if (md.constTensorsDims[0].size() == 0) return 1;

  for(int i = 0; i < md.constTensorsDims[0].size(); i++)
  {
    if(md.constTensorsDims[0][i] != 1)
    {
      constDim = i;
      nonSingletonDimNum++;
    }
  }

  if (nonSingletonDimNum > 1) return 0;

  //check if the add/sub broadcast is across channels
  int32_t matchingDim=-1;
  if (nonSingletonDimNum == 1)
  {
    int32_t numDim = md.varTensorsDims[0].size();
    if (numDim != 0)
    {
      for (int32_t i=numDim-1;i>=0;i--) {
        if (md.varTensorsDims[0][i] == md.constTensorsDims[0][constDim])
        {
          matchingDim = i;
          break;
        }
      }
      /*Convert matchingDim to 6D:*/
      matchingDim += TIDL_DIM_MAX - numDim;
      if (matchingDim != TIDL_DIM_NUMCH)
        return 0;
    }
    else
    {
      printf("WARNING: Cannot read dims for variable input of Add/Mul operator, shape inference should be done on the model\n" \
              "assuming 1D const input is channel broadcast, operator maps to BatchNorm Layer ");
      return 1;
    }

  }

  return 1;
}

int32_t checkShapeInferenceforOnnx(sTIDL_allowlistingMetaData md)
{
  /** Make sure all the required tensor dimensions are populated, else use open source runtimes offering to run this model
   * or run shape inference on this model before executing import **/
  std::vector<std::vector<int32_t>> vartensordims   = md.varTensorsDims;
  std::vector<std::vector<int32_t>> consttensordims = md.constTensorsDims;
  std::vector<std::vector<int32_t>> outtensordims   = md.outputTensorDims;
  for (int i=0; i<md.varTensorIndices.size();i++)
  {
    if (vartensordims[i].size() == 0)
    {
      return -1;
    }
  }
  for (int i=0; i<md.constTensorIndices.size();i++)
  {
    if (consttensordims[i].size() == 0)
    {
      return -1;
    }
  }
  for (int i=0; i<outtensordims.size();i++)
  {
    if (outtensordims[i].size() == 0)
    {
      return -1;
    }
  }
  return 0;
}

int32_t TIDL_checkAddMulSubDivTensorProperties(const sTIDL_LayerPC_t &layerPC, TIDL_base_operator_t oper)
{
  sTIDL_allowlistingMetaData md = layerPC.allowlistingMetaData;
  std::string diagMsg;
  /** Conditions for allowlisting:
   * Necessary attributes : No attributes
   * Number of inputs : 2
   * Input type : both variable, "input 1" variable and "input 2" constant
   * Number of dimensions : Variable input - TIDL_DIM_MAX, Constant input - any number of dimensions but only one dimension can be non-1,
   * and non-1 dim size should match number of input channels of variable tensor
   */
  if (gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_ONNX)
  {
    int32_t status = checkShapeInferenceforOnnx(md);
    if (status == -1)
    {
      if (gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_ONNX_RT)
      {
        printf("*** ERROR : Add/Mul/Sub/Div with constant tensor requires input dimensions of that layer to be present as part of the network.\
          Please use open source runtimes offering to run this model or run shape inference on this model before executing import  *** \n");
        return -1;
      }
    }
  }
  if(md.numInputs != 2)
  {
    diagMsg = "ALLOWLISTING : ADD/MUL/SUB layer : Only 2 inputs supported  --  file info - " + std::string(__FILE__) + " , " \
    + std::string(__func__) + " , " + std::to_string(__LINE__);
    diagsInfo.push_back(diagMsg);
    return TIDL_ALLOWLISTING_LAYER_CHECK_FAILED;
  }
  if(gParams.modelType != TIDL_IMPORT_MODEL_FORMAT_ONNX)
  {
    #if 0
    for(auto & varDims : md.varTensorsDims)
    {
      if(varDims.size() != 4)
      {
        diagMsg = "ALLOWLISTING : ADD/MUL/SUB layer : The variable inputs of layer must have 4 dimensions  --  file info - " \
        + std::string(__FILE__) + " , " + std::string(__func__) + " , " + std::to_string(__LINE__);
        diagsInfo.push_back(diagMsg);
        return TIDL_ALLOWLISTING_LAYER_CHECK_FAILED;
      }
    }
    #endif
  }

  if(md.varTensorIndices.size() == 2) //2 variable inputs
  {

  }
  else //addition with constant
  {
      if(md.constTensorsDims[0].size() != 0) //addition with vector, not constant number
      {
        /* Need to do addition with vector, so only one of the dimensions can be other than 1 */
        //find non singleton dimensions in tensor
        int32_t nonSingletonDimNum = 0;
        int32_t nonSingletonDimIdx = 0;
        for(int i = 0; i < md.constTensorsDims[0].size(); i++)
        {
          if(md.constTensorsDims[0][i] != 1)
          {
            nonSingletonDimNum++;
            nonSingletonDimIdx = i;
          }
        }
        //checks
        if(nonSingletonDimNum > 1)
        {
          #if 0
            if (oper == BASE_OP_MUL || oper == BASE_OP_ADD)
            {
              bool isEltwise = false;
              if(md.constTensorsDims.size() == 1 && md.varTensorIndices.size() == 1) //eltwise mul between a variable tensor and constant tensor
              {
                  isEltwise = ((md.varTensorsDims[0][0] == md.constTensorsDims[0][0]) &&
                                    (md.varTensorsDims[0][1] == md.constTensorsDims[0][1]) &&
                                    (md.varTensorsDims[0][2] == md.constTensorsDims[0][2]) &&
                                    (md.varTensorsDims[0][3] == md.constTensorsDims[0][3]) );
              }
              if(!isEltwise)
              {
                diagMsg = "ALLOWLISTING : ADD/MUL layer : Eltwise operator is supported only with operands of similar dimensions  --  file info - " \
                + std::string(__FILE__) + " , " + std::string(__func__) + " , " + std::to_string(__LINE__);
                diagsInfo.push_back(diagMsg);
                return TIDL_ALLOWLISTING_LAYER_CHECK_FAILED;
              }
            }
            else
            {
              diagMsg = "ALLOWLISTING : ADD layer : Constant tensor must be a number or vector; only one dimension can be > 1  --  file info - " + \
              std::string(__FILE__) + " , "  + std::string(__func__) + " , " + std::to_string(__LINE__);
              diagsInfo.push_back(diagMsg);
              return TIDL_ALLOWLISTING_LAYER_CHECK_FAILED;
            }
            #endif
        }
        else
        {
          if(gParams.modelType != TIDL_IMPORT_MODEL_FORMAT_ONNX)
          {
            #if 0
            /** Avoiding use of TIDL_DIM_NUMCH in varTensorDims array as TIDL_DIM_NUMCH maps to 6D, but varTensorDims can be of any dimensions*/
            if(md.constTensorsDims[0][nonSingletonDimIdx] != md.varTensorsDims[0][1])
            {
              diagMsg = "ALLOWLISTING : ADD layer : Number of channels of input do not match number of elements to be added  --  file info - " + \
              std::string(__FILE__) + " , "  + std::string(__func__) + " , " + std::to_string(__LINE__);
              diagsInfo.push_back(diagMsg);
              return TIDL_ALLOWLISTING_LAYER_CHECK_FAILED;
            }
            #endif
          }
        }
      }
    }
  return 0;
}

/** Common across tflite RT and ONNX RT
 * ONNX RT Layer -- Gemm
 * Tflite RT Layer -- FullyConnected
 */
int32_t TIDL_checkInnerProductTensorProperties(const sTIDL_LayerPC_t &layerPC)
{
  /** Conditions for allowlisting:
   * Necessary attributes : kernel_shape, strides, pads
   * Number of inputs : 2 or 3
   * Input type : Only 1 variable, others constant
   * Number of dimensions : 2
   */

  /** Check for variable input dimensions
   * Gemm layer definition indicates variable input dimension must be 2, so exported layer itself will be 2 dimensional (M, K) ---> (1, K) for TIDL
   * Weight tensor as per definition should be (K, N)
   * Bias tensor should be broadcastable to (M, N)
   * Output is (M, N)
   */

  sTIDL_allowlistingMetaData md = layerPC.allowlistingMetaData;
  std::string diagMsg;

  //variable input checks

  int32_t varIdx = md.varTensorIndices[0]; //Only one variable input

  if((md.constTensorIndices.size() == 1) || (md.constTensorIndices.size() == 2)) //filter, bias
  {
    int32_t filter = 0;
    int32_t bias = 1;
    if((md.constTensorsDims[filter].size() < 2) )
    {
      diagMsg = "ALLOWLISTING : INNER PRODUCT layer : Filter tensor input should have atleast 2 dimensions, observed " + std::to_string(md.constTensorsDims[0].size())\
      +  " dimensions  --  file info - " + std::string(__FILE__) + " , " + std::string(__func__) + " , " + std::to_string(__LINE__);
      diagsInfo.push_back(diagMsg);
      return TIDL_ALLOWLISTING_LAYER_CHECK_FAILED;
    }
    if(gParams.modelType != TIDL_IMPORT_MODEL_FORMAT_ONNX)
    {
      #if 0
      if(md.constTensorsDims[filter][1] != md.varTensorsDims[0][1]) //var tensor : M X K , filter : N X K
      {
        diagMsg = "ALLOWLISTING : INNER PRODUCT layer : Mismatch in filter dimension and input dimension -- Filter dimension = [" + \
        std::to_string(md.constTensorsDims[filter][0]) + "," + std::to_string(md.constTensorsDims[filter][1]) + "],  input dimensions = [" +  \
        std::to_string(md.varTensorsDims[0][0]) + "," + std::to_string(md.varTensorsDims[0][1]) + "] --- file info - " + \
        std::string(__FILE__) + " , " + std::string(__func__) + " , " + std::to_string(__LINE__);
        diagsInfo.push_back(diagMsg);
        return TIDL_ALLOWLISTING_LAYER_CHECK_FAILED;
      }
      #endif
    }

    if(md.constTensorIndices.size() == 2) //both weight and bias present
    {
      //Bias tensor will have number of dimensions either 1 or 2 as per Gemm layer definition
      if(md.constTensorsDims[bias].size() == 2)
      {
        if((md.constTensorsDims[bias][0] != 1) || (md.constTensorsDims[bias][1] != md.constTensorsDims[filter][0]))
        {
          diagMsg = "ALLOWLISTING : INNER PRODUCT layer : Bias tensor input should be a vector (1, N) and N should match output dimension --  file info - "\
           + std::string(__FILE__) + " , " + std::string(__func__) + " , " + std::to_string(__LINE__);
          diagsInfo.push_back(diagMsg);
          return TIDL_ALLOWLISTING_LAYER_CHECK_FAILED;
        }
      }
      else if(md.constTensorsDims[bias].size() == 1)
      {
        if(md.constTensorsDims[bias][0] != md.constTensorsDims[filter][0])
        {
          diagMsg = "ALLOWLISTING : INNER PRODUCT layer : Bias tensor input should be a vector (1, N) and N should match output dimension --  file info - "\
           + std::string(__FILE__) + " , " + std::string(__func__) + " , " + std::to_string(__LINE__);
          diagsInfo.push_back(diagMsg);
          return TIDL_ALLOWLISTING_LAYER_CHECK_FAILED;
        }
      }
      else
      {
        diagMsg = "ALLOWLISTING : INNER PRODUCT layer : Dimension of bias vector can either be [1, N] or [N] --  file info - "\
                    + std::string(__FILE__) + " , " + std::string(__func__) + " , " + std::to_string(__LINE__);
        diagsInfo.push_back(diagMsg);
        printf("ALLOWLISTING : GEMM layer : Dimension of bias vector can either be [1, N] or [N]-- %s, %s, %d\n", __FILE__, __func__, __LINE__);
        return TIDL_ALLOWLISTING_LAYER_CHECK_FAILED;
      }
    }
  }

  return 0;
}
int32_t TIDL_checkGatherTensorProperties(const sTIDL_LayerPC_t &layerPC)
{
  sTIDL_allowlistingMetaData md = layerPC.allowlistingMetaData;
  int32_t indDim = md.varTensorsDims[1].size();
  int32_t numDim = md.varTensorsDims[0].size();
  int32_t chDim;
  int32_t batchDim;

  std::string diagMsg;
  int32_t status = 0;

  const sTIDL_GatherLayerParams_t& params = layerPC.layerParams.gatherParams;
  int32_t axis = params.axis;
  #if 0
  if (numDim != 2 || axis != 0)
  {
    if (numDim == 2)
    {
      if (axis != 0)
      {
        diagMsg = "ALLOWLISTING : Current gather configuration in the model is not supported,\n Only line gather along axis 0 is supported for now!  --  file info - " + std::string(__FILE__) + " , " \
        + std::string(__func__) + " , " + std::to_string(__LINE__);
        diagsInfo.push_back(diagMsg);
        return TIDL_ALLOWLISTING_LAYER_CHECK_FAILED;
      }
    }
    if (numDim == 3)
    {
      chDim = md.varTensorsDims[0][0];
      if (chDim != 1 || axis != 1)
      {
        diagMsg = "ALLOWLISTING : Current gather configuration in the model is not supported,\n Only line gather is supported for now!  --  file info - " + std::string(__FILE__) + " , " \
        + std::string(__func__) + " , " + std::to_string(__LINE__);
        diagsInfo.push_back(diagMsg);
        return TIDL_ALLOWLISTING_LAYER_CHECK_FAILED;
      }
    }
    else if (numDim == 4)
    {
      batchDim = md.varTensorsDims[0][0];
      chDim = md.varTensorsDims[0][1];
      if (batchDim != 1 || chDim != 1 || axis != 2)
      {
        diagMsg = "ALLOWLISTING : Current gather configuration in the model is not supported,\n Only line gather is supported for now!  --  file info - " + std::string(__FILE__) + " , " \
        + std::string(__func__) + " , " + std::to_string(__LINE__);
        diagsInfo.push_back(diagMsg);
        return TIDL_ALLOWLISTING_LAYER_CHECK_FAILED;
      }
    }
  }
  if (indDim != 1)
  {
    diagMsg = "ALLOWLISTING : Gather requires Indices to be one dimensional, Multi-dimensional indices are not supported!  --  file info - " + std::string(__FILE__) + " , " \
    + std::string(__func__) + " , " + std::to_string(__LINE__);
    diagsInfo.push_back(diagMsg);
    return TIDL_ALLOWLISTING_LAYER_CHECK_FAILED;
  }
  #endif
  return status;
}
/** Common across tflite RT and ONNX RT
 * ONNX RT Layer -- Reshape
 * Tflite RT Layer -- kTfLiteBuiltinReshape
 */
int32_t TIDL_checkReshapeTensorProperties(const sTIDL_LayerPC_t &layerPC)
{
  /** Conditions for allowlisting:
   * Necessary attributes : NA
   * Input type : variable
   * Number of dimensions : TIDL_DIM_MAX
   */
  sTIDL_allowlistingMetaData md = layerPC.allowlistingMetaData;
  std::string diagMsg;
  int32_t status = 0;
  #if 0
  if(gParams.modelType != TIDL_IMPORT_MODEL_FORMAT_ONNX)
  {
    int32_t inBatchDim, outBatchDim;
    inBatchDim = md.varTensorsDims[0][0];
    outBatchDim = md.outputTensorDims[0][0];
  }
  #endif
  return status;
}

/**
 * ONNX RT Layer -- Transpose
 */
int32_t TIDL_checkTransposeTensorProperties(const sTIDL_LayerPC_t &layerPC)
{
  /** Conditions for allowlisting:
   * Necessary attributes : NA
   * Input type : variable
   * Number of dimensions : TIDL_DIM_MAX
   */
  sTIDL_allowlistingMetaData md = layerPC.allowlistingMetaData;
  std::string diagMsg;
  int32_t status = 0;

  if(gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_ONNX)
  {
    int32_t *perm  = (int32_t*)layerPC.weights.ptr;
    int32_t numDim = layerPC.weights.bufSize;
    if(perm[0] != TIDL_DIM_BATCH && numDim == TIDL_DIM_MAX)
    {
      /** Batch dimension is changing in the target permutation*/
      diagMsg = "ALLOWLISTING : Transpose over batch dimension is not supported  --  file info - " + std::string(__FILE__) + " , " \
      + std::string(__func__) + " , " + std::to_string(__LINE__);
      diagsInfo.push_back(diagMsg);
      return TIDL_ALLOWLISTING_LAYER_CHECK_FAILED;
    }
  }

  return status;
}

/** Common across tflite RT and ONNX RT
 * ONNX RT Layer -- MaxPool, AveragePool, GlobalAveragePool
 * Tflite RT Layer -- kTfLiteBuiltinAveragePool2d, kTfLiteBuiltinMaxPool2d
 */
int32_t TIDL_checkPoolingTensorProperties(const sTIDL_LayerPC_t &layerPC)
{
  /** Conditions for allowlisting:
   * Necessary attributes : kernel_shape, strides, pads
   * Number of inputs : 1
   * Input type : variable
   * Number of dimensions : 4
   */
  sTIDL_allowlistingMetaData md = layerPC.allowlistingMetaData;
  std::string diagMsg;

  if(md.numInputs != 1)
  {
    diagMsg = "ALLOWLISTING : POOLING layer : Only one input supported  --  file info - " + std::string(__FILE__) + " , " \
    + std::string(__func__) + " , " + std::to_string(__LINE__);
    diagsInfo.push_back(diagMsg);
    return TIDL_ALLOWLISTING_LAYER_CHECK_FAILED;
  }

  if(md.numVarInputs != 1)
  {
    diagMsg = "ALLOWLISTING : POOLING layer : Input should be variable and not constant/initializer --  file info - " + std::string(__FILE__) + " , " \
    + std::string(__func__) + " , " + std::to_string(__LINE__);
    diagsInfo.push_back(diagMsg);
    return TIDL_ALLOWLISTING_LAYER_CHECK_FAILED;
  }
  #if 0
  if(gParams.modelType != TIDL_IMPORT_MODEL_FORMAT_ONNX)
  {
    if(md.varTensorsDims[0].size() != 4)
    {
      diagMsg = "ALLOWLISTING : POOLING layer : Number of input dimensions must be 4  --  file info - " + std::string(__FILE__) + " , " \
      + std::string(__func__) + " , " + std::to_string(__LINE__);
      diagsInfo.push_back(diagMsg);
      return TIDL_ALLOWLISTING_LAYER_CHECK_FAILED;
    }
  }
  #endif
  return TIDL_ALLOWLISTING_LAYER_CHECK_PASSED;
}


/** Common across tflite RT and ONNX RT
 * ONNX RT Layer -- Slice
 * Tflite RT Layer -- Slice, Strided Slice
 */
int32_t TIDL_checkSliceTensorProperties(const sTIDL_LayerPC_t &layerPC)
{
  int32_t status = TIDL_ALLOWLISTING_LAYER_CHECK_PASSED;
  /** Conditions for allowlisting:
   * Necessary attributes :
   * Number of inputs : 4
   * Input type : variable
   * Number of dimensions :
   */
  sTIDL_allowlistingMetaData md = layerPC.allowlistingMetaData;
  std::string diagMsg;

  if((gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_TFLITE_RT) || (gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_TFLITE))
  {
    if(md.numInputs != 4)
    {
      diagMsg = "ALLOWLISTING : SLICE layer : Must have 4 inputs supported  --  file info - " + std::string(__FILE__) + " , " \
      + std::string(__func__) + " , " + std::to_string(__LINE__);
      diagsInfo.push_back(diagMsg);
      return TIDL_ALLOWLISTING_LAYER_CHECK_FAILED;
    }

    if(md.numVarInputs != 1)
    {
      diagMsg = "ALLOWLISTING : SLICE layer : Only one input should be variable/not constant/initializer --  file info - " + std::string(__FILE__) + " , " \
      + std::string(__func__) + " , " + std::to_string(__LINE__);
      diagsInfo.push_back(diagMsg);
      return TIDL_ALLOWLISTING_LAYER_CHECK_FAILED;
    }

    if(md.numConstInputs != 3)
    {
      diagMsg = "ALLOWLISTING : SLICE layer : Layer should have 3 constant/initializer inputs --  file info - " + std::string(__FILE__) + " , " \
      + std::string(__func__) + " , " + std::to_string(__LINE__);
      diagsInfo.push_back(diagMsg);
      return TIDL_ALLOWLISTING_LAYER_CHECK_FAILED;
    }

    if(md.varTensorsDims[0].size() != 4)
    {
      diagMsg = "ALLOWLISTING : SLICE layer : Number of dimensions for variable inputs must be 4  --  file info - " + std::string(__FILE__) + " , " \
      + std::string(__func__) + " , " + std::to_string(__LINE__);
      diagsInfo.push_back(diagMsg);
      return TIDL_ALLOWLISTING_LAYER_CHECK_FAILED;
    }
    for(int i = 0; i < md.constTensorsDims.size(); i++)
    {
      if(md.constTensorsDims[i].size() != 1)
      {
        diagMsg = "ALLOWLISTING : SLICE layer : Number of dimensions for constant/initializer inputs must be 1  --  file info - " + std::string(__FILE__) + " , " \
        + std::string(__func__) + " , " + std::to_string(__LINE__);
        diagsInfo.push_back(diagMsg);
        return TIDL_ALLOWLISTING_LAYER_CHECK_FAILED;
      }
      if(md.constTensorsDims[i][0] != 4)
      {
        diagMsg = "ALLOWLISTING : SLICE layer : Constant/initializer inputs must have 4 values  --  file info - " + std::string(__FILE__) + " , " \
        + std::string(__func__) + " , " + std::to_string(__LINE__);
        diagsInfo.push_back(diagMsg);
        return TIDL_ALLOWLISTING_LAYER_CHECK_FAILED;
      }
    }
    if(md.outputTensorDims[0].size() != 4)
    {
      diagMsg = "ALLOWLISTING : SLICE layer : Number of dimensions for output tensor must be 4  --  file info - " + std::string(__FILE__) + " , " \
      + std::string(__func__) + " , " + std::to_string(__LINE__);
      diagsInfo.push_back(diagMsg);
      return TIDL_ALLOWLISTING_LAYER_CHECK_FAILED;
    }
    if(md.varTensorsDims[0][0] != 1)
    {
      diagMsg = "ALLOWLISTING : SLICE layer : Only batch size = 1 supported  --  file info - " + std::string(__FILE__) + " , " \
      + std::string(__func__) + " , " + std::to_string(__LINE__);
      diagsInfo.push_back(diagMsg);
      return TIDL_ALLOWLISTING_LAYER_CHECK_FAILED;
    }
  }
  return status;
}



static void checkConvLayers(const sTIDL_LayerPC_t &layerPC, DiagList_t &diags)
{
  int32_t i2, i3, i4;

  bool notOptimal_input64align = false;
  bool notOptimal_output64align = false;
  bool validated = false;
  bool patch_embedding_pattern = false;
  bool warning = false;
  bool notOptimal = false;
  bool fatalError = false;

  const sTIDL_ConvParams_t& params = layerPC.layerParams.convParams;

  /*
   * Validated combinations
   */
  int32_t validatedParams[][8] =
  {
    // -1 means dont care
    // kernelH kernelW strideH strideW dilationH dilationW padH padW
    {        1,      1,      1,      1,       -1,       -1,  -1,  -1},
    {        1,      1,      2,      2,       -1,       -1,  -1,  -1},
    {        2,      2,      1,      1,       -1,       -1,  -1,  -1},
    {        2,      2,      2,      2,        1,        1,  -1,  -1},
    {        3,      3,      1,      1,       -1,       -1,  -1,  -1},
    {        3,      3,      2,      2,        1,        1,  -1,  -1},
    {        3,      3,      3,      3,        1,        1,  -1,  -1},
    {        5,      5,      1,      1,       -1,       -1,  -1,  -1},
    {        5,      5,      2,      2,        1,        1,  -1,  -1},
    {        7,      7,      1,      1,       -1,       -1,  -1,  -1},
    {        7,      7,      2,      2,        1,        1,  -1,  -1},
    {        1,      3,      1,      1,       -1,       -1,  -1,  -1},
    {        3,      1,      1,      1,       -1,       -1,  -1,  -1},
    {        1,      5,      1,      1,       -1,       -1,  -1,  -1},
    {        5,      1,      1,      1,       -1,       -1,  -1,  -1},
    {        1,      7,      1,      1,       -1,       -1,  -1,  -1},
    {        7,      1,      1,      1,       -1,       -1,  -1,  -1},
  };

  for(i2 = 0; i2 < sizeof(validatedParams)/8/sizeof(int32_t); i2++)
  {
    if((validatedParams[i2][0] == -1 || params.kernelH == validatedParams[i2][0]) && (validatedParams[i2][1] == -1 || params.kernelW == validatedParams[i2][1]))
    {
      validated = true;
    }
    else
    {
      validated = false;
      continue;
    }

    if((validatedParams[i2][2] == -1 || params.strideH == validatedParams[i2][2]) && (validatedParams[i2][3] == -1 || params.strideW == validatedParams[i2][3]))
    {
      validated = true;
    }
    else
    {
      validated = false;
      continue;
    }

    if((validatedParams[i2][4] == -1 || params.dilationH == validatedParams[i2][4]) && (validatedParams[i2][5] == -1 || params.dilationW == validatedParams[i2][5]))
    {
      validated = true;
    }
    else
    {
      validated = false;
      continue;
    }

    if((validatedParams[i2][6] == -1 || params.padH == validatedParams[i2][6]) && (validatedParams[i2][7] == -1 || params.padW == validatedParams[i2][7]))
    {
      validated = true;
    }
    else
    {
      validated = false;
      continue;
    }

    if(validated)
      break;
  }

  /* Depthwise separable convolution */
  if (((params.numGroups == params.numInChannels) || (params.numInChannels ==1)) &&
      (params.numGroups == params.numOutChannels))
  {
    // kernelH kernelW strideH strideW dilationH dilationW padH padW
    #define DEPTHWISE_SUPPORTED_PARAMS (4)
    int32_t supportedConfigs[][DEPTHWISE_SUPPORTED_PARAMS] =
    {
      /*kernelW, kernelH, strideW, strideH*/
      {   3,          3,             1,           1},
      {   3,          3,             2,           2},
      {   5,          5,             1,           1},
      {   7,          7,             1,           1},
    };

    for(i2 = 0; i2 < sizeof(validatedParams)/(DEPTHWISE_SUPPORTED_PARAMS *sizeof(int32_t)); i2++)
    {
      if ( ( params.kernelW == supportedConfigs[i2][0] ) &&
	( params.kernelH == supportedConfigs[i2][1] ) &&
	( params.strideW == supportedConfigs[i2][2] ) &&
	( params.strideH == supportedConfigs[i2][3] ) )
      {
	/* Only these combinations are supported */
	validated = true;
	break;
      }
      else
      {
	validated = false;
      }
    }

    if ( validated!= true)
    {
      /* Any other combination is not supported */
      fatalError = true;
      diags.emplace_back(TIDL_ModelDiagnostic::DK_NotSupported, "[TIDL_ConvolutionLayer] "
         "%s Depthwise convolution layer is either not supported or doesn't have "
	 "optimized implementation available.", layerPC.name);
    }

    if(layerPC.layerType == TIDL_ConvolutionLayer )
    {
      /* MMALIB Bug MMALIB-566*/
      int32_t simd_width=-1;
      if((gParams.deviceName & ~TIDL_OTF_FLAG_BIT) == TIDL_AM62A)
      {
        simd_width = 32U;
      }
      else
      {
        simd_width = 64U;
      }
      //check for 16-bit
      if( gParams.numFeatureBits > 8 )
      {
        simd_width = simd_width/2;
      }
      if(params.padW > simd_width)
      {
        fatalError = true;
        diags.emplace_back(TIDL_ModelDiagnostic::DK_NotSupported, "[TIDL_ConvolutionLayer] "
        "%s Dilation greater than the MMA width is not supported",
        layerPC.name);
      }
    }
  }
  else if((layerPC.layerType == TIDL_ConvolutionLayer) && ((gParams.deviceName & ~TIDL_OTF_FLAG_BIT) == TIDL_AM62A))
  {
    bool isSupportedPad = true;
    if((gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_TFLITE_RT) || (gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_ONNX_RT))
    {
      if(params.padW > layerPC.allowlistingMetaData.varTensorsDims[0][layerPC.allowlistingMetaData.varTensorsDims[0].size()-1])
      {
        isSupportedPad = false;
      }
    }
    else if(gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_TVM_RELAY)
    {
      isSupportedPad = true; /* Add check after ensuring dimValues are available in TVM */
    }
    else /* TIDL-RT */
    {
      if(params.padW > layerPC.inData[0].dimValues[TIDL_DIM_WIDTH]) /* inData is not populated before allow-listing call in OSRT */
      {
        isSupportedPad = false;
      }
    }

    if(isSupportedPad == false)
    {
      fatalError = true;
        diags.emplace_back(TIDL_ModelDiagnostic::DK_NotSupported, "[TIDL_ConvolutionLayer] "
        "%s Padding greater than input width is not supported",
        layerPC.name);
    }
  }


  /*
    Convolution which can be reduced to supported Patch embedding format
  */
  if (
        (validated == false) && // check if previous checks fails
        // check if satisfy: N X N stride N
        (params.kernelH == params.kernelW) &&   // N X N
        (params.strideH == params.strideW) &&   // same stride in both dimensions
        (params.kernelH == params.strideH) &&   // stride N
        (params.kernelH >= 3)                   // N > 3
     )
  {
    validated = true;
    patch_embedding_pattern = true;
  }


  /*
   * Not supported
   */
  if(params.kernelH == 11 && params.kernelW == 11 &&
     params.strideH == 4 && params.strideW == 4)
  {
    warning = true;
    diags.emplace_back(TIDL_ModelDiagnostic::DK_NotVerified, "[TIDL_ConvolutionLayer] "
       "%s kernel size 11x11 with stride 4 has gone through limited verification.",
       layerPC.name);
  }

  if(params.kernelH > 7  && params.kernelW > 7 &&
     params.strideH == 2 && params.strideW == 2)
  {
    fatalError = true;
    diags.emplace_back(TIDL_ModelDiagnostic::DK_NotSupported, "[TIDL_ConvolutionLayer] "
       "%s kernel size larger than 7 with stride 2 is not supported !!!",
       layerPC.name);
  }

  if(gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_CAFFE)
  {
    if((params.numGroups > params.numInChannels) || (params.numGroups > params.numOutChannels))
    {
      fatalError = true;
      diags.emplace_back(TIDL_ModelDiagnostic::DK_NotSupported, "[TIDL_ConvolutionLayer] "
        "%s numGroups is greater than In or Out channels !!!",
        layerPC.name);
    }
  }

  if(params.kernelH != 11 && params.kernelW != 11 &&
     params.strideH == 4 && params.strideW == 4   &&
     (!patch_embedding_pattern)     // support by reducing to stride 1 makes this check irrelevant
    )
  {
    fatalError = true;
    diags.emplace_back(TIDL_ModelDiagnostic::DK_NotSupported, "[TIDL_ConvolutionLayer] "
       "%s stride 4 only supports kernel size 11x11 !!!", layerPC.name);
  }

  /*Only for AM62A*/
  if(params.kernelH == 3 && params.kernelW == 3 &&
     params.strideH == 3 && params.strideW == 3 &&
     layerPC.layerType == TIDL_ConvolutionLayer &&
     ((gParams.deviceName & ~TIDL_OTF_FLAG_BIT) == TIDL_AM62A))
  {
    fatalError = true;
    diags.emplace_back(TIDL_ModelDiagnostic::DK_NotSupported, "[TIDL_ConvolutionLayer] "
       "%s Kernel size 3x3 is not supported with stride 3 !!!", layerPC.name);
  }

  if(params.strideH != params.strideW)
  {
    fatalError = true;
    diags.emplace_back(TIDL_ModelDiagnostic::DK_NotSupported, "[TIDL_ConvolutionLayer] "
       "%s strideH != strideW --- only symmetric stride supported !!!", layerPC.name);
  }


  /*
   * Performance Suggestion
   */
  if((params.kernelH * params.kernelW * layerPC.inData[0].dimValues[TIDL_DIM_NUMCH] / params.numGroups + params.enableBias) % 64)
  {
    notOptimal = true;
    notOptimal_input64align = true;
  }
  if(layerPC.outData[0].dimValues[TIDL_DIM_NUMCH] / params.numGroups % 64)
  {
    notOptimal = true;
    notOptimal_output64align = true;
  }

  /*
   * Conclusion
   */
  if(!validated && !fatalError)
  {
    diags.emplace_back(TIDL_ModelDiagnostic::DK_NotVerified, "[TIDL_ConvolutionLayer] "
       "%s Layer parameter combination has undergone limited validation and "
       "may have some issues. Following are the parameters:"
       "\n        Kernel %dx%d Stride %dx%d dilation %dx%d Pad %dx%d Bias %d",
       layerPC.name,
       params.kernelH,   params.kernelW,
       params.strideH,   params.strideW,
       params.dilationH, params.dilationW,
       params.padH,      params.padW,
       params.enableBias);
  }

#if MODEL_CHECK_CONV_SUGGESTIONS
  if(notOptimal_input64align)
    diags.emplace_back(TIDL_ModelDiagnostic::DK_NotPerformant, "[TIDL_ConvolutionLayer] "
       "%s params.kernelH * params.kernelW * layer.inData[0].dimValues[TIDL_DIM_NUMCH] / params.numGroups "
       "is not 64 aligned, the performance is not efficient on MMA!",
       layerPC.name);

  if(notOptimal_output64align)
    diags.emplace_back(TIDL_ModelDiagnostic::DK_NotPerformant, "[TIDL_ConvolutionLayer] "
       "%s layer.outData[0].dimValues[TIDL_DIM_NUMCH] / params.numGroups "
       "is not 64 aligned, the performance is not efficient on MMA!",
       layerPC.name);
#endif
}


static void checkPoolingLayers(const sTIDL_LayerPC_t &layerPC, DiagList_t &diags)
{
  const sTIDL_PoolingParams_t& params = layerPC.layerParams.poolParams;
  bool validated = false;
  bool warning = false;
  bool notOptimal = false;
  bool fatalError = false;

  /*
   * Validated combinations
   */
  if(params.kernelH == 0 && params.kernelW == 0)
  {
    validated = true; // Global pooling
  }

  if(params.kernelH == 2 && params.kernelW == 2 &&
     params.strideH == 2 && params.strideW == 2)
  {
    validated = true;
  }

  if(params.kernelH == 1 && params.kernelW == 1 &&
     ((params.strideH == 2 && params.strideW == 2) || (params.strideH == 1 && params.strideW == 1 )) &&
     params.poolingType == TIDL_MaxPooling)
  {
    validated = true;
  }

  if(params.kernelH == 3 && params.kernelW == 3 &&
     ((params.strideH == 1 && params.strideW == 1) || (params.strideH == 2 && params.strideW == 2)) &&
     params.padH    == 1 && params.padW    == 1)
  {
    validated = true;
  }


  if(params.kernelH == 3 && params.kernelW == 3 &&
     params.strideH == 2 && params.strideW == 2 &&
     params.padH    == 0 && params.padW    == 0)
  {
    validated = true;
  }

  if(params.kernelH == 1 && params.kernelW == 1)
  {
    if((params.strideH == 2 && params.strideW == 1) || (params.strideH == 1 && params.strideW == 2)
    || (params.strideH == 3 && params.strideW == 1) || (params.strideH == 1 && params.strideW == 3))
    {
      validated = true;
    }
  }

  if(params.kernelH == 1 && params.kernelW == 2 && params.strideH == 1 && params.strideW == 2)
  {
    validated = true;
  }
  else if(params.kernelH == 1 && params.kernelW == 3 && params.strideH == 1 && params.strideW == 3)
  {
    validated = true;
  }
  else if(params.kernelH == 2 && params.kernelW == 1 && params.strideH == 2 && params.strideW == 1)
  {
    validated = true;
  }
  else if(params.kernelH == 3 && params.kernelW == 1 && params.strideH == 3 && params.strideW == 1)
  {
    validated = true;
  }

  if(is16bit && params.poolingType == TIDL_MaxPooling)
  {
    validated = true;
  }


  /*
   * Performance Suggestion
   */
  if(params.kernelH == 4 && params.kernelW == 4)
  {
    notOptimal = true;
    diags.emplace_back(TIDL_ModelDiagnostic::DK_NotPerformant, "[TIDL_PoolingLayer] "
       "%s 4x4 pooling is not efficient on MMA!", layerPC.name);
  }

  if (!validated)
    diags.emplace_back(TIDL_ModelDiagnostic::DK_NotSupported, "TIDL_PoolingLayer '%s': "
       "kernel size %dx%d with stride %dx%d not supported",
       layerPC.name, params.kernelH, params.kernelW, params.strideH, params.strideW);
}


static void checkBatchNormLayers(const sTIDL_LayerPC_t &layerPC, DiagList_t &diags)
{
  const sTIDL_BatchNormParams_t& params = layerPC.layerParams.batchNormParams;
  bool notOptimal = false;

  /*
   * Performance Suggestion
   */
  if((is16bit) && (layerPC.actParams.actType != TIDL_Sigmoid) && (layerPC.actParams.actType != TIDL_Tanh)
        && (layerPC.actParams.actType != TIDL_HardSigmoid) && (layerPC.actParams.actType != TIDL_ELU)
        && (layerPC.actParams.actType != TIDL_GELU))
  {
    notOptimal = true;
    diags.emplace_back(TIDL_ModelDiagnostic::DK_NotPerformant, "[TIDL_BatchNormLayer] "
       "%s 16 bits is not optimal in this release.", layerPC.name);
  }
}


static void checkInnerProductLayers(const sTIDL_LayerPC_t &layerPC, DiagList_t &diags)
{
  const sTIDL_InnerProductParams_t& params = layerPC.layerParams.innerProductParams;
  bool validated = false;
  bool warning = false;
  bool notOptimal = false;
  bool fatalError = false;
  int32_t inNodesA = (params.inputATranspose == 0) ? layerPC.inData[0].dimValues[3] : layerPC.inData[0].dimValues[2];
  int32_t inNodesB = (params.inputBTranspose == 0) ? layerPC.inData[1].dimValues[2] : layerPC.inData[1].dimValues[3];
  /*
   * Not supported
   */
  if(inNodesA  !=  inNodesB)
  {
    /*
    fatalError = true;
    diags.emplace_back(TIDL_ModelDiagnostic::DK_NotSupported, "[TIDL_InnerProductLayer] "
        "%s input shape of A and B is not suitable for matrix multiplication", layerPC.name);
    */
  }

  /*
   * Performance Suggestion
   */
  if(layerPC.inData[0].dimValues[TIDL_DIM_WIDTH ] * layerPC.outData[0].dimValues[TIDL_DIM_WIDTH ] > 2048 * 2048)
  {
    notOptimal = true;
    diags.emplace_back(TIDL_ModelDiagnostic::DK_NotPerformant, "[TIDL_InnerProductLayer] "
       "%s Size larger than 2048 * 2048 is not optimal.", layerPC.name);
  }
}


static void checkDeconvLayers(const sTIDL_LayerPC_t &layerPC, DiagList_t &diags)
{
  const sTIDL_ConvParams_t& params = layerPC.layerParams.convParams;
  bool validated = false;
  bool warning = false;
  bool notOptimal = false;
  bool fatalError = false;

  /*
   * Validated combinations
   */
  if(params.kernelH == 4 && params.kernelW == 4 &&
     params.strideH == 2 && params.strideW == 2 &&
     params.padW == 1 && params.padH == 1)
  {
    validated = true;
  }
  else if(params.kernelH == 3 && params.kernelW == 3 &&
     params.strideH == 2 && params.strideW == 2 &&
     params.padW == 1 && params.padH == 1)
  {
    validated = true;
  }
  else if (params.kernelH == 2 && params.kernelW == 2 &&
     params.strideH == 2 && params.strideW == 2 &&
     params.padW == 0 && params.padH == 0)
  {
    validated = true;
  }
  else
  {
    fatalError = true;
    diags.emplace_back(TIDL_ModelDiagnostic::DK_NotSupported, "[TIDL_Deconv2DLayer] "
       "%s is not supported.", layerPC.name);
  }

  /*
   * Performance Suggestion
   */
  diags.emplace_back(TIDL_ModelDiagnostic::DK_NotPerformant, "[TIDL_Deconv2DLayer] "
      "%s Please change to Upsample/Resize if possible. Upsample/Resize will be more efficient.", layerPC.name);
}

static void checkDetectionOutLayers(const sTIDL_LayerPC_t &layerPC, DiagList_t &diags)
{
  const sTIDL_DetectOutputParams_t& params = layerPC.layerParams.detectOutParams;
  bool validated = false;
  bool warning = false;
  bool notOptimal = false;
  bool fatalError = false;

}


static void checkShuffleChannelLayers(const sTIDL_LayerPC_t &layerPC, DiagList_t &diags)
{
  const sTIDL_ShuffleLayerParams_t& params = layerPC.layerParams.shuffleLayerParams;
  bool validated = false;
  bool warning = false;
  bool notOptimal = false;
  bool fatalError = false;

  /*
   * Performance Suggestion
   */
  notOptimal = true;
  diags.emplace_back(TIDL_ModelDiagnostic::DK_NotPerformant, "[TIDL_ShuffleChannelLayer] "
      "%s ShuffleChannel layer is not optimal in this version.", layerPC.name);
}

static void checkDepthToSpaceLayers(const sTIDL_LayerPC_t &layerPC, DiagList_t &diags)
{
  const sTIDL_ShuffleLayerParams_t& params = layerPC.layerParams.shuffleLayerParams;
  bool validated = false;
  bool warning = false;
  bool notOptimal = false;
  bool fatalError = false;

  int32_t blockSize = layerPC.layerParams.depthToSpaceParams.blockSize;
  /*
   * Validated combinations
   */
  if(blockSize == 8 || blockSize == 4 || blockSize == 2)
  {
    validated = true;
  }
  else
  {
    fatalError = true;
    diags.emplace_back(TIDL_ModelDiagnostic::DK_NotSupported, "[TIDL_DepthToSpaceLayer] "
       "%s is not supported.", layerPC.name);
  }

  /*
   * Performance Suggestion
   */
  notOptimal = true;
  diags.emplace_back(TIDL_ModelDiagnostic::DK_NotPerformant, "[TIDL_DepthToSpaceLayer] "
      "%s Standalone DepthToSpace layer is not optimal in this version, it is optimal if it is next to 1x1 convolutions", layerPC.name);
}

static void checkBatchToSpaceLayers(const sTIDL_LayerPC_t &layerPC, DiagList_t &diags)
{
  bool validated = false;
  bool warning = false;
  bool notOptimal = false;
  bool fatalError = false;

  fatalError = true;
  if((gParams.modelType == 0) || (gParams.modelType == 1) || (gParams.modelType == 2) || (gParams.modelType == 3))
  diags.emplace_back(TIDL_ModelDiagnostic::DK_NotSupported, "[TIDL_BatchToSpaceLayer/TIDL_SpaceToBatchLayer] "
      "%s is PC layer, marked supported only for runtimes(to enable dispatch to TIDL), should be already merged for standalone TIDL import check", layerPC.name);
}

static void checkColorConversionLayers(const sTIDL_LayerPC_t &layerPC, DiagList_t &diags)
{
  return;
}

static void checkResizeLayers(const sTIDL_LayerPC_t &layerPC, DiagList_t &diags)
{
  const sTIDL_ResizeLayerParams_t& params = layerPC.layerParams.resizeParams;
  bool warning = false;
  bool notOptimal = false;
  bool fatalError = false;

  /*
   * Validated combinations
   */

  if ( params.resizeRatio[TIDL_DIM_WIDTH] == params.resizeRatio[TIDL_DIM_HEIGHT] )
  {
    if (( params.resizeRatio[TIDL_DIM_WIDTH] == 2.0 ) ||
      (params.resizeRatio[TIDL_DIM_WIDTH] == 4.0 ))
    {
      diags.emplace_back(TIDL_ModelDiagnostic::DK_Info, "[TIDL_ResizeLayer] "
         "%s Any resize ratio which is power of 2 and greater than 4 will be placed by combination of 4x4 resize layer and 2x2 resize layer. "
         "For example a 8x8 resize will be replaced by 4x4 resize followed by 2x2 resize.", layerPC.name);
    }
    else
    {
      warning = 1;
      diags.emplace_back(TIDL_ModelDiagnostic::DK_NotPerformant, "[TIDL_ResizeLayer] "
         "%s Resize kernel with non-power of 2 resize ratio is not optimal.", layerPC.name);
    }
  }
  else
  {
    warning = 1;
    diags.emplace_back(TIDL_ModelDiagnostic::DK_NotPerformant, "[TIDL_ResizeLayer] "
       "%s Resize Layer with non-symmetric resize ratio across width and height is not optimal.", layerPC.name);
  }
  if(params.mode != TIDL_ResizeNearest && params.mode != TIDL_ResizeBilinear)
  {
    diags.emplace_back(TIDL_ModelDiagnostic::DK_NotSupported, "[TIDL_ResizeLayer] "
       "%s Resize layer mode must be either nearest_neighbor or bilinear.", layerPC.name);
  }
}


static void checkPriorBoxLayers(const sTIDL_LayerPC_t &layerPC, DiagList_t &diags)
{
  diags.emplace_back(TIDL_ModelDiagnostic::DK_NotSupported, "[TIDL_PriorBoxLayer] "
     "%s should be removed in import process. Please make sure you have detection out layer in the model. "
     "If not, this model will not work!", layerPC.name);
}


static void checkPermuteLayers(const sTIDL_LayerPC_t &layerPC, DiagList_t &diags)
{
  diags.emplace_back(TIDL_ModelDiagnostic::DK_NotSupported, "[TIDL_PermuteLayer] "
     "%s should be removed in import process. Please make sure you have detection out layer in the model. "
     "If not, this model will not work!", layerPC.name);
}


static void checkEltwiseLayers(const sTIDL_LayerPC_t &layerPC, DiagList_t &diags)
{
  const sTIDL_EltWiseParams_t& params = layerPC.layerParams.eltWiseParams;
  bool validated = false;
  bool warning = false;
  bool notOptimal = false;
  bool fatalError = false;

#ifdef TIDL_EN_MULTI_TENSOR_ELT_WISE
  /* curently using (layer.inData[0].elementType >> 1)+1 instead of TIDL_getDatElementSize */
  if(((layerPC.numInBufs > 2) && (params.eltWiseType == TIDL_EltWiseProduct)) ||
     ((layerPC.numInBufs > ((TIDL_SIMD_WIDTH>>1)/((layerPC.inData[0].elementType >> 1) + 1)))
      && (params.eltWiseType == TIDL_EltWiseSum)))
  {
    fatalError = true;
    diags.emplace_back(TIDL_ModelDiagnostic::DK_NotSupported, "[TIDL_EltWiseLayer] "
       "%s Too many inputs for elementwise operator.", layerPC.name);
  }
#else
  /*
   * Not supported
   */
  if(layerPC.numInBufs > 2)
  {
    fatalError = true;
    diags.emplace_back(TIDL_ModelDiagnostic::DK_NotSupported, "[TIDL_EltWiseLayer] "
       "%s Only supports 2 inputs.", layerPC.name);
  }
#endif
}

static void checkFlattenLayers(const sTIDL_LayerPC_t &layerPC, DiagList_t &diags)
{
  bool validated = false;
  bool warning = false;
  bool notOptimal = false;
  bool fatalError = false;

  if(is16bit)
  {
    notOptimal = true;
    diags.emplace_back(TIDL_ModelDiagnostic::DK_NotPerformant, "[TIDL_FlattenLayer] "
       "%s Flatten with 16 bit is not optimal in this version.", layerPC.name);
  }
}

static void checkNonLinearLUTLayers(const sTIDL_LayerPC_t &layerPC, DiagList_t &diags)
{
  if(TIDL_isDevice("am62a"))
  {
       diags.emplace_back(TIDL_ModelDiagnostic::DK_NotSupported, "[%s] "
     "%s should be removed in import process. This activation type is not supported now !!!",
     TIDL_LayerString[layerPC.layerType], layerPC.name);
  }
  else
  {
    //check for 16-bit
    if(gParams.numParamBits>8 || gParams.numFeatureBits>8)
    {
       diags.emplace_back(TIDL_ModelDiagnostic::DK_NotSupported, "[%s] "
     "%s should be removed in import process. This activation type is not supported for >8bit input/output data type !! \n",
     TIDL_LayerString[layerPC.layerType], layerPC.name);
    }
  }
}

static void checkSliceLayers(const sTIDL_LayerPC_t &layerPC, DiagList_t &diags)
{
  const sTIDL_SliceLayerParams_t& params = layerPC.layerParams.sliceParams;
  bool validated = false;
  bool warning = false;
  bool notOptimal = false;
  bool fatalError = false;

  /*
   * Not supported
   */
  #if 0
  if((params.axis < TIDL_DIM_NUMCH) || (params.axis > TIDL_DIM_WIDTH) ||  (params.stride != 1))
  {
    fatalError = true;
    diags.emplace_back(TIDL_ModelDiagnostic::DK_NotSupported, "[TIDL_SliceLayer] "
       "%s Only supports axis = 1 to 3 and Stride 1.", layerPC.name);
  }
  #endif
}



static void checkArgMaxLayers(const sTIDL_LayerPC_t &layerPC, DiagList_t &diags)
{
  const sTIDL_ArgMaxParams_t& params = layerPC.layerParams.argMaxParams;
  bool validated = false;
  bool warning = false;
  bool notOptimal = false;
  bool fatalError = false;

  /*
   * Not supported
   */
  if(layerPC.outData[0].dimValues[TIDL_DIM_BATCH] != 1 ||
     layerPC.outData[0].dimValues[TIDL_DIM_NUMCH] != 1)
  {
    fatalError = true;
    diags.emplace_back(TIDL_ModelDiagnostic::DK_NotSupported, "[TIDL_ArgMaxLayer] "
       "%s Only supports axis == 1.", layerPC.name);
  }
}


static void checkQuantizeLayers(const sTIDL_LayerPC_t &layerPC, DiagList_t &diags)
{
  bool fatalError = false;
  if(gParams.preQuantizedModel == 0)
  {
    /*Quantize operator is only supported for pre-quantized models*/
    fatalError = true;
    diags.emplace_back(TIDL_ModelDiagnostic::DK_NotSupported, "[TIDL_QuantizeLayer] "
    "is only supported for pre-quantized ONNX models", layerPC.name);
  }
}

static void checkDequantizeLayers(const sTIDL_LayerPC_t &layerPC, DiagList_t &diags)
{
  const sTIDL_DequantizeParams_t& params = layerPC.layerPCParams.dequantParams;
  bool validated = false;
  bool warning = false;
  bool notOptimal = false;
  bool fatalError = false;

  /*
   * Not supported
   */
  if(gParams.preQuantizedModel == 0)
  {
    if((params.scale != 1.0 || params.zeroPoint != 0.0 ))
    {
      fatalError = true;
      diags.emplace_back(TIDL_ModelDiagnostic::DK_NotSupported, "[TIDL_DequantizeLayer] "
        "%s scale = 1.0 and zeroPoint= 0.0 are supported now", layerPC.name);
    }
  }

}

static void checkCastLayers(const sTIDL_LayerPC_t &layerPC, DiagList_t &diags)
{
  bool validated = false;
  bool warning = false;
  bool notOptimal = false;
  bool fatalError = false;
  /*
   * Not supported
   */
  if(gParams.addDataConvertToNet == 0)
  {
    fatalError = true;
    diags.emplace_back(TIDL_ModelDiagnostic::DK_NotSupported, "[TIDL_CastLayer] "
       "%s is NOT supported now", layerPC.name);
  }
}


static void checkSoftmaxLayers(const sTIDL_LayerPC_t &layerPC, DiagList_t &diags)
{
  const sTIDL_SoftMaxParams_t& params = layerPC.layerParams.softMaxParams;
  bool validated = false;
  bool warning = false;
  bool notOptimal = false;
  bool fatalError = false;

}

static void checkConcatenateLayers(const sTIDL_LayerPC_t &layerPC, DiagList_t &diags)
{
  const sTIDL_ConcatParams_t &concatParams = layerPC.layerParams.concatParams;
  int32_t numDim = 0;
  
  numDim = layerPC.allowlistingMetaData.varTensorsDims[0].size();

  if (numDim > 3 && concatParams.axis == 0)
  {
    diags.emplace_back(TIDL_ModelDiagnostic::DK_NotSupported, "[TIDL_ConcatLayer] "
       "Concatenate in batch axis is not supported.");
  }
}

static void checkClipLayers(const sTIDL_LayerPC_t &layerPC, DiagList_t &diags)
{
  const sTIDL_ActParams_t &actParams = layerPC.actParams;

  // Not supported, except min <= 0 and max > 0
  if(!((actParams.clipMin <= 0) && (actParams.clipMax > 0)))
  {
    diags.emplace_back(TIDL_ModelDiagnostic::DK_NotSupported, "[TIDL_ClipLayer] "
       "Clip must have min <= 0 and max > 0.");
  }
}

static void checkReshapeLayers(const sTIDL_LayerPC_t &layerPC, DiagList_t &diags)
{
  int32_t numDims = layerPC.weights.bufSize;

  int32_t inBatchDim  = layerPC.inData[0].dimValues[TIDL_DIM_BATCH];
  int32_t outBatchDim = layerPC.outData[0].dimValues[TIDL_DIM_BATCH];
  int32_t inChDim     = layerPC.inData[0].dimValues[TIDL_DIM_NUMCH];
  int32_t outChDim    = layerPC.outData[0].dimValues[TIDL_DIM_NUMCH];

  if((layerPC.layerPCParams.reshapeParams.isInduced == 0) && (numDims > TIDL_DIM_MAX))
  {
    diags.emplace_back(TIDL_ModelDiagnostic::DK_NotSupported, "[TIDL_ReshapeLayer] "
       "%s Reshape output dimensions > %d not supported ", layerPC.name, TIDL_DIM_MAX);
    return;
  }
  /*
  Temporarily disabling this check
  if((inBatchDim != outBatchDim))
  {
    diags.emplace_back(TIDL_ModelDiagnostic::DK_NotSupported, "[TIDL_ReshapeLayer] "
       "%s Reshape over batch dimension is not supported currently ", layerPC.name);
  }
  */

}

static void checkTransposeLayers(const sTIDL_LayerPC_t &layerPC, DiagList_t &diags)
{
  int32_t *perm = (int32_t*)layerPC.weights.ptr;
  int32_t numDim = layerPC.weights.bufSize;
  #if 0
  /*Update parsing logic to store in transpose params directly and check transpose params here*/
  if(perm[0] != TIDL_DIM_BATCH && numDim == TIDL_DIM_MAX)
  {
    diags.emplace_back(TIDL_ModelDiagnostic::DK_NotSupported, "[TIDL_TransposeLayer] "
       "%s Transpose over batch dimension is not supported ", layerPC.name);
    return;
  }
  #endif

}

static void checkSqueezeLayers(const sTIDL_LayerPC_t &layerPC, DiagList_t &diags)
{
  if(gParams.modelType == 0 || gParams.modelType == 1 || gParams.modelType == 3) //not supposed to be there in network if not open source runtime
  {
    diags.emplace_back(TIDL_ModelDiagnostic::DK_NotSupported, "[TIDL_SqueezeLayer] "
       "%s should be removed in import process. If not, this model will not work!", layerPC.name);
    return;
  }
  else
  {
    //supported
  }

}

static void checkGatherLayers(const sTIDL_LayerPC_t &layerPC, DiagList_t &diags)
{
  int32_t indDim = layerPC.inData[1].numDim;
  int32_t numDim = layerPC.inData[0].numDim;
  int32_t chDim;
  int32_t batchDim;

  std::string diagMsg;
  int32_t status = 0;

  const sTIDL_GatherLayerParams_t& params = layerPC.layerParams.gatherParams;
  int32_t axis = params.axis;

  if (numDim != 2 || axis != 0)
  {
    if (numDim == 2)
    {
      if (axis != 0)
      {
        diagMsg = "ALLOWLISTING : Current gather configuration in the model is not supported,\n Only line gather along axis 0 is supported for now!  --  file info - " + std::string(__FILE__) + " , " \
        + std::string(__func__) + " , " + std::to_string(__LINE__);
        diagsInfo.push_back(diagMsg);
      }
    }
    if (numDim == 3)
    {
      chDim = layerPC.inData[0].dimValues[TIDL_DIM_NUMCH];
      if (chDim != 1 || axis != 1)
      {
        diagMsg = "ALLOWLISTING : Current gather configuration in the model is not supported,\n Only line gather is supported for now!  --  file info - " + std::string(__FILE__) + " , " \
        + std::string(__func__) + " , " + std::to_string(__LINE__);
        diagsInfo.push_back(diagMsg);
      }
    }
    else if (numDim == 4)
    {
      batchDim = layerPC.inData[0].dimValues[TIDL_DIM_BATCH];
      chDim = layerPC.inData[0].dimValues[TIDL_DIM_NUMCH];
      if (batchDim != 1 || chDim != 1 || axis != 2)
      {
        diagMsg = "ALLOWLISTING : Current gather configuration in the model is not supported,\n Only line gather is supported for now!  --  file info - " + std::string(__FILE__) + " , " \
        + std::string(__func__) + " , " + std::to_string(__LINE__);
        diagsInfo.push_back(diagMsg);
      }
    }
  }
  if (indDim != 1)
  {
    diagMsg = "ALLOWLISTING : Gather requires Indices to be one dimensional, Multi-dimensional indices are not supported!  --  file info - " + std::string(__FILE__) + " , " \
    + std::string(__func__) + " , " + std::to_string(__LINE__);
    diagsInfo.push_back(diagMsg);
  }
}
// This function is used to post an error for layers that should be removed by the import process.
static void checkScatterElementsLayers(const sTIDL_LayerPC_t &layerPC, DiagList_t &diags)
{
  const sTIDL_ScatterElementsParams_t& params = layerPC.layerParams.scatterElementsParams;

  int32_t isScatterNd = (params.axis == -1)?1:0;
  
  if(isScatterNd)
  {
    int32_t c, reduction;
    c = layerPC.inData[2].dimValues[TIDL_DIM_NUMCH];
    reduction = layerPC.layerParams.scatterElementsParams.reduction;

    if((c > 1) && (reduction == TIDL_ScatterElementsAdd))
    {
      diags.emplace_back(TIDL_ModelDiagnostic::DK_NotVerified, "Updates tensor shape not supported! "
       "Updates tensor should not have more than 1 channel.");
    }
  }
  else if((params.axis != TIDL_DIM_WIDTH) && (!isScatterNd)) /*only width direction scatter is supported*/
  {
    diags.emplace_back(TIDL_ModelDiagnostic::DK_NotSupported, "[%s] "
      "%s should be removed in import process. If not, this model will not work!",
      TIDL_LayerString[layerPC.layerType], layerPC.name);
  }
}

static void checkReduceLayers(const sTIDL_LayerPC_t &layerPC, DiagList_t &diags)
{
  const sTIDL_ReduceParams_t& params = layerPC.layerParams.reduceParams;

  if((params.axis != TIDL_DIM_HEIGHT) || (params.keepDims != 1 || (params.ops!=TIDL_inReduceOpMax)))
  {
    diags.emplace_back(TIDL_ModelDiagnostic::DK_NotSupported, "[%s] "
      "%s should be removed in import process. If not, this model will not work!",
      TIDL_LayerString[layerPC.layerType], layerPC.name);
  }
}

// This function is used to post an error for layers that should be removed by the import process.
static void checkFoldedLayers(const sTIDL_LayerPC_t &layerPC, DiagList_t &diags)
{
  diags.emplace_back(TIDL_ModelDiagnostic::DK_NotSupported, "[%s] "
     "%s should be removed in import process. If not, this model will not work!",
     TIDL_LayerString[layerPC.layerType], layerPC.name);
}

static void checkQuantStatsAvailable(sTIDL_Network_t * resultTIDLNetStructure, sTIDL_OrgNetwork_t * orgTIDLNetStructure, DiagList_t &diags)
{
  if(resultTIDLNetStructure->isQuantStatsAvailable == 0)
  {
    diags.emplace_back(TIDL_ModelDiagnostic::DK_NotSupported, "TIDL_E_QUANT_STATS_NOT_AVAILABLE] "
       "tidl_quant_stats_tool.out fails to collect dynamic range. Please look into quant stats log. This model will get fault on target.");
  }
}


static void checkDataflowInfoAvailable(sTIDL_Network_t * resultTIDLNetStructure, sTIDL_OrgNetwork_t * orgTIDLNetStructure, DiagList_t &diags)
{
  if(resultTIDLNetStructure->dataFlowInfo == 0)
  {
    if(((gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_TFLITE_RT) || (gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_ONNX_RT) ||
                            (gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_TVM_RELAY))
             && (gParams.numParamBits == 32))
    {
      /* OSRT floating pass -- Corresponding warning is added in OSRT */
    }
    else
    {
      diags.emplace_back(TIDL_ModelDiagnostic::DK_PCOnly, "[TIDL_E_DATAFLOW_INFO_NULL] "
        "Network compiler returned with error or didn't executed, this model can only be used on PC/Host emulation mode, it is not expected to work on target/EVM.");
    }
  }
}

int tidlModelCheck(tidl_import_config * params, sTIDL_OrgNetwork_t * orgTIDLNetStructure)
{
  sTIDL_Network_t * resultNetStructure;
  int32_t ret;

  FILE* fp = fopen((char*)params->outputNetFile, "rb");
  if(fp == NULL)
  {
    printf("ERROR  : TIDL Model Check cannot open the result network file %s.\n", params->outputNetFile);
    return 1;
  }

  resultNetStructure = (sTIDL_Network_t*)malloc(sizeof(sTIDL_Network_t));

  ret = fread(resultNetStructure, 1, sizeof(sTIDL_Network_t), fp);

  is16bit = params->numFeatureBits == 16;

  if(gParams.debugTraceLevel > 0)
  {
    printf("****************************************************\n");
    printf("**               TIDL Model Checker               **\n");
    printf("****************************************************\n");
  }

  // Check each layer for legality and accumulate diagnostics
  DiagList_t diags;
  int32_t errorCount = 0;
  int32_t warningCount = 0;

  for (int i = 0; i < orgTIDLNetStructure->numLayers; i++)
    tidlModelCheckLayer(orgTIDLNetStructure->TIDLPCLayers[i], diags);

  // Check global model properties
  checkQuantStatsAvailable(resultNetStructure, orgTIDLNetStructure, diags);
  checkDataflowInfoAvailable(resultNetStructure, orgTIDLNetStructure, diags);

  // Report any errors or warnings
  for (auto diag : diags)
  {
    diag.emit();
    if (diag.isError())
       ++errorCount;
    else
       ++warningCount;
  }

  if(diags.empty())
  {
    printf("****************************************************\n");
    printf("**                ALL MODEL CHECK PASSED          **\n");
    printf("****************************************************\n\n");
  }
  else
  {
    printf("****************************************************\n");
    printf("**        %3d WARNINGS        %3d ERRORS          **\n", warningCount, errorCount);
    printf("****************************************************\n");
  }
  free(resultNetStructure) ;
  return errorCount;
}


// Check a single layer for legality and enque any errors or warnings. This function is the basis
// of both the model check pass and the allowlist API used by external frameworks.
void tidlModelCheckLayer(const sTIDL_LayerPC_t &layerPC, DiagList_t &diags)
{
  switch (layerPC.layerType)
  {
    case TIDL_DataLayer:
      // fully supported
      break;
    case TIDL_ConvolutionLayer:
      checkConvLayers(layerPC, diags);
      break;
    case TIDL_PoolingLayer:
      checkPoolingLayers(layerPC, diags);
      break;
    case TIDL_ReLULayer:
      // Converted to batchnorm unconditionally if not merged
      break;
    case TIDL_PReLULayer:
      // Converted to batchnorm unconditionally if not merged
      break;
    case TIDL_SigmoidLayer:
      // should be merged or be folded out
      // This layer has no specific check, and is converted to BN layer, so mark as supported for runtimes allowlisting, since it won't be folded during first allowlisting pass
      if ((gParams.modelType != TIDL_IMPORT_MODEL_FORMAT_ONNX_RT) && (gParams.modelType != TIDL_IMPORT_MODEL_FORMAT_TFLITE_RT))
        checkFoldedLayers(layerPC, diags);
      break;
    case TIDL_TanhLayer:
      if((gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_CAFFE) || (gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_TENSORFLOW) || (gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_ONNX) || (gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_TFLITE))
      {
        checkFoldedLayers(layerPC, diags);
      }
      else
      {
        checkNonLinearLUTLayers(layerPC, diags);
      }

      // should be merged or be folded out
      // This layer has no specific check, and is converted to BN layer, so mark as supported for runtimes allowlisting, since it won't be folded during first allowlisting pass
      break;
    case TIDL_HardSigmoidLayer:
      if((gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_CAFFE) || (gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_TENSORFLOW) || (gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_ONNX) || (gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_TFLITE))
      {
        checkFoldedLayers(layerPC, diags);
      }
      else
      {
        checkNonLinearLUTLayers(layerPC, diags);
      }

      // should be merged or be folded out
      // This layer has no specific check, and is converted to BN layer, so mark as supported for runtimes allowlisting, since it won't be folded during first allowlisting pass
      break;
    case TIDL_ELULayer:
      if((gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_CAFFE) || (gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_TENSORFLOW) || (gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_ONNX) || (gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_TFLITE))
      {
        checkFoldedLayers(layerPC, diags);
      }
      else
      {
        checkNonLinearLUTLayers(layerPC, diags);
      }

      // should be merged or be folded out
      // This layer has no specific check, and is converted to BN layer, so mark as supported for runtimes allowlisting, since it won't be folded during first allowlisting pass
      break;
    case TIDL_EltWiseLayer:
      checkEltwiseLayers(layerPC, diags);
      break;
    case TIDL_InnerProductLayer:
      checkInnerProductLayers(layerPC, diags);
      break;
    case TIDL_SoftMaxLayer:
      if (gParams.modelType != TIDL_IMPORT_MODEL_FORMAT_ONNX_RT)
        checkSoftmaxLayers(layerPC, diags);
      break;
    case TIDL_BatchNormLayer:
      checkBatchNormLayers(layerPC, diags);
      break;
    case TIDL_BiasLayer:
      // should be merged or be folded out
      checkFoldedLayers(layerPC, diags);
      break;
    case TIDL_ScaleLayer:
      checkFoldedLayers(layerPC, diags);
      break;
    case TIDL_Deconv2DLayer:
      checkDeconvLayers(layerPC, diags);
      break;
    case TIDL_ConcatLayer:
      checkConcatenateLayers(layerPC, diags);
      break;
    case TIDL_SplitLayer:
      // should be merged or be folded out
      //checkFoldedLayers(layerPC, diags);
      break;
    case TIDL_SliceLayer:
      checkSliceLayers(layerPC, diags);
      break;
    case TIDL_CropLayer:
      // fully supported
      break;
    case TIDL_FlattenLayer:
      checkFlattenLayers(layerPC, diags);
      break;
    case TIDL_DropOutLayer:
      // should be folded out
      checkFoldedLayers(layerPC, diags);
      break;
    case TIDL_ArgMaxLayer:
      checkArgMaxLayers(layerPC, diags);
      break;
    case TIDL_DetectionOutputLayer:
      checkDetectionOutLayers(layerPC, diags);
      break;
    case TIDL_ShuffleChannelLayer:
      checkShuffleChannelLayers(layerPC, diags);
      break;
    case TIDL_ResizeLayer:
      checkResizeLayers(layerPC, diags);
      break;
    case TIDL_RoiPoolingLayer:
      // fully supported
      break;
    case TIDL_OdPostProcessingLayer:
      // fully supported
      break;
    case TIDL_DepthToSpaceLayer:
      checkDepthToSpaceLayers(layerPC, diags);
    case TIDL_ColorConversionLayer:
      checkColorConversionLayers(layerPC, diags);
    case TIDL_PadLayer:
      // fully supported
      break;
    case TIDL_OdOutputReformatLayer:
      //fully supported
      break;
    case TIDL_DataConvertLayer:
      //fully supported
      break;
    case TIDL_BatchToSpaceLayer:
      checkBatchToSpaceLayers(layerPC, diags);
      break;
    case TIDL_SpaceToBatchLayer:
      checkBatchToSpaceLayers(layerPC, diags);
      break;
    case TIDL_BatchReshapeLayer:
      // fully supported
      break;
    case TIDL_CustomLayer:
      // fully supported
      break;
    case TIDL_UnsupportedLayer:
      // already reported
      break;

    // These layerTypes are represented only in the PC imoprt data structure, not in the target model.
    // None of them are supported.
    case TIDL_ConstDataLayer:
      // fully supported
      break;
    case TIDL_PriorBoxLayer:
      checkPriorBoxLayers(layerPC, diags);
      break;
    case TIDL_PermuteLayer:
      checkPermuteLayers(layerPC, diags);
      break;
    case TIDL_ReshapeLayer:
        checkReshapeLayers(layerPC, diags);
      break;
    case TIDL_ShapeLayer:
      checkFoldedLayers(layerPC, diags);
      break;
    case TIDL_SqueezeLayer:
      checkSqueezeLayers(layerPC, diags);
      break;
    case TIDL_TransposeLayer:
        checkTransposeLayers(layerPC, diags);
      break;
    case TIDL_DequantizeLayer:
      //checkDequantizeLayers(layerPC, diags);
      break;
    case TIDL_CastLayer:
      checkCastLayers(layerPC, diags);
      break;
    case TIDL_ClipLayer:
      checkFoldedLayers(layerPC, diags);
      break;
    case TIDL_MinimumLayer:
      checkFoldedLayers(layerPC, diags);
      break;
    case TIDL_LeakyReluLayer:
      //converted to Batchnorm unconditionally
      break;
    case TIDL_ScatterElementsLayer:
      checkScatterElementsLayers(layerPC, diags);
      break;
    case TIDL_ReduceLayer:
      checkReduceLayers(layerPC, diags);
      break;
    case TIDL_QuantizeLayer:
      //checkQuantizeLayers(layerPC, diags);
      break;
    case TIDL_GatherLayer:
      //checkGatherLayers(layerPC, diags);
      break;
    //Add check for all:
    case TIDL_ReduceMeanLayer:
      break;
    case TIDL_PowLayer:
      break;
    case TIDL_SqrtLayer:
      break;
    case TIDL_SubLayer:
      break;
    case TIDL_DivLayer:
      break;
    case TIDL_LayerNormLayer:
      //Add check
      break;

    default:
      diags.emplace_back(TIDL_ModelDiagnostic::DK_NotSupported,
        "unknown layer type: %d", layerPC.layerType);
  }
}

// This function is called by the allowlist interface to decide if a node can/should be supported
// as part of a subgraph offloaded to TIDL.
bool tidlModelCheckOffload(const tidl_import_config &params, const sTIDL_LayerPC_t &layerPC)
{
  DiagList_t diags;
  is16bit = params.numFeatureBits == 16;

  // Check layers which are folded out during import. They need to be checked separately here
  // because tidlModelCheckLayer() assumes folded layers are not present in the optimized graph.
  // These checking can be moved to tidlModelCheckLayer() in future if tidlModelCheckLayer() is
  // called before optimization which does layer folding.
  // Relay op "clip" is mapped to either TIDL_ReLULayer with Relu6 or TIDL_ClipLayer. Relu6 has
  // no constraints but TIDL_ClipLayer has.
  if(layerPC.layerType == TIDL_ReLULayer)
    ; // this is needed because tidlModelCheckLayer() would flag this as error
  else if (layerPC.layerType == TIDL_ArgMaxLayer)
    ; // skip calling tidlModelCheckLayer which checks output dimension that is not available now
  else if (layerPC.layerType == TIDL_BiasLayer)
    ; // will be folded or converted to BatchNorm
  else if (layerPC.layerType == TIDL_ClipLayer)
    checkClipLayers(layerPC, diags);
  // Check layers which are not folded out during import
  else if (layerPC.layerType == TIDL_IdentityLayer)
    ; // PC only layer, skip checking, it will be merged in optimizeNet()
  else
    tidlModelCheckLayer(layerPC, diags);

  if(diags.size() == 0)
  {
    diagsInfo.push_back("");
  }

  for (auto diag : diags)
  {
    std::pair<const char *, std::string> pair = diag.extractDiagMsg();
    std::string infoStr = std::string(pair.first) + " -- " + pair.second;
    diagsInfo.push_back(infoStr);

    if (!diag.isOKToOffload())
    {
      if (params.debugTraceLevel > 0)  diag.emit();
      return false;
    }
  }

  return true;
}

int tidlInputTensorDimCheck(sTIDL_OrgNetwork_t &orgTIDLNetStructure)
{
  for (int i1 = 0; i1 < orgTIDLNetStructure.numLayers; i1++)
  {
    if(orgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_DataLayer)
    {
      for (int i2 = 0; i2 < orgTIDLNetStructure.TIDLPCLayers[i1].numOutBufs; i2++)
      {
        for (int i3 = 0; i3 < TIDL_DIM_MAX; i3++)
        {
          if(orgTIDLNetStructure.TIDLPCLayers[i1].outData[i2].dimValues[i3] <= 0)
          {
              printf("****************************************************\n");
              printf("**   All the Input Tensor Dimensions has to be greater then Zero \n");
              printf("**   DIM Error - For Tensor %d, Dim %d is %d\n", orgTIDLNetStructure.TIDLPCLayers[i1].outData[i2].dataId, i3, orgTIDLNetStructure.TIDLPCLayers[i1].outData[i2].dimValues[i3]);
              printf("****************************************************\n");
              return 0;
          }
        }
      }
    }
  }
  return 1;
}

int tidlModelTensorDimCheck(sTIDL_OrgNetwork_t &orgTIDLNetStructure)
{
  for (int i1 = 0; i1 < orgTIDLNetStructure.numLayers; i1++)
  {
    for (int i2 = 0; i2 < orgTIDLNetStructure.TIDLPCLayers[i1].numOutBufs; i2++)
    {
      for (int i3 = 0; i3 < TIDL_DIM_MAX; i3++)
      {
        if(orgTIDLNetStructure.TIDLPCLayers[i1].outData[i2].dimValues[i3] <= 0)
        {
            printf("****************************************************\n");
            printf("**   All the Tensor Dimensions has to be greater then Zero \n");
            printf("**   DIM Error - For Tensor %d, Dim %d is %d\n", orgTIDLNetStructure.TIDLPCLayers[i1].outData[i2].dataId, i3, orgTIDLNetStructure.TIDLPCLayers[i1].outData[i2].dimValues[i3]);
            printf("****************************************************\n");
            return 0;
        }
      }
    }
  }
  return 1;
}
