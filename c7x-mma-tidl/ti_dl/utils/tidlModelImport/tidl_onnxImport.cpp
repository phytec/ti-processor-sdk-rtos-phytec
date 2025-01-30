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

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/message.h>
#include <google/protobuf/text_format.h>
//#include <io.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <float.h>
#include <cmath>
#include <limits>

#include "ti_dl.h"
#include "tidl_import_api.h"
#include "tidl_import_config.h"
//#include "onnx/onnx-ml.proto3.pb.h"
#include "onnx/onnx-ml.proto3.pb.h"
#include "tidl_custom_import.h"
#include "tidl_import_common.h"
#include "tidl_onnxImport.h"
#include "tidl_import_common_model_check.h"
#include "tidl_import_common_tidlrt_osrt.h"
#include "tidl_deviceInfo.h"

using namespace std;
using namespace onnx;
using ::google::protobuf::Message;
using ::google::protobuf::io::FileInputStream;
using ::google::protobuf::io::FileOutputStream;
using ::google::protobuf::io::ZeroCopyInputStream;
using ::google::protobuf::io::CodedInputStream;
using ::google::protobuf::io::ZeroCopyOutputStream;
using ::google::protobuf::io::CodedOutputStream;


#include "tidl_import_common.h"
#define HIGHEST_OPSET_VERSION_SUPPORTED 18
int64_t onnxOpSetVersion;

extern sTIDL_OrgNetwork_t      orgTIDLNetStructure;
extern sTIDL_OrgNetwork_t      tempTIDLNetStructure;
extern sTIDL_Network_t         tIDLNetStructure;

extern "C"
{
extern std::vector<std::string> diagsInfo;
int32_t TIDL_isInputConstInGraph(GraphProto &onnGraph, const string name);
int32_t TIDL_isInputConstInGraph(GraphProto& onnGraph, const string name)
{
  int i;
  for (i = 0; i < numNetInData; i++)
  {
    if ((strcmp(inDataNames[i], name.c_str()) == 0))
    {
      return(0);
    }
  }
  for (i = 0; i < onnGraph.initializer_size(); i++)
  {
    if ((strcmp(onnGraph.initializer(i).name().c_str(), name.c_str()) == 0))
    {
      return(1);
    }
  }
  for (i = 0; i < onnGraph.node_size(); i++)
  {
    if ((strcmp(onnGraph.node(i).output(0).c_str(), name.c_str()) == 0) && (strcmp(onnGraph.node(i).op_type().c_str(), "Constant") == 0))
    {
      return(1);
    }
  }
  return (0);
}
}

int32_t TIDL_onnxGetNodeIdxFromInput(GraphProto& onnxGraph, const char *bufName)
{
  int32_t i, j, nameLen, nodeIdx = -1;
  char nodeName[TIDL_MAX_DATA_NAME];
  char inDataName[TIDL_MAX_DATA_NAME];
  for (i = 0; i < onnxGraph.node_size(); i++)
  {
    for (j = 0; j < onnxGraph.node(i).input_size(); j++)
    {
      if (strcmp(onnxGraph.node(i).input(j).c_str(), bufName) == 0)
      {
        nodeIdx = i;
        break;
      }
    }
  }
  return nodeIdx;
}

int32_t TIDL_onnxGetNodeIdx(GraphProto&             onnxGraph, const char *bufName)
{
  int32_t i, j, nameLen, nodeIdx = -1;
  char nodeName[TIDL_MAX_DATA_NAME];
  char inDataName[TIDL_MAX_DATA_NAME];
  for (i = 0; i < onnxGraph.node_size(); i++)
  {
    for (j = 0; j < onnxGraph.node(i).output_size(); j++)
    {
      if (strcmp(onnxGraph.node(i).output(j).c_str(), bufName) == 0)
      {
        nodeIdx = i;
        break;
      }
    }
  }
  return nodeIdx;
}

bool TIDL_doesDequantHaveConstTensor(GraphProto& onnGraph, const string name)
{
  /*Check if buffer is output of dequant, and if part of dequant check if it is a constant buffer */
  bool isConstBuf = false;
  /*Find the corresponding DQ layer via it's output name*/
  int32_t DQNodeIdx = TIDL_onnxGetNodeIdx(onnGraph, name.c_str());
  /*Check the buffer to be dequantized*/
  if(DQNodeIdx != -1)
  {
    /*
    Previous layer is Dequant AND
    Input to previous layer is const*/
    if(strcmp(onnGraph.node(DQNodeIdx).op_type().c_str(), "DequantizeLinear") == 0)
    {
      if(TIDL_isInputConstInGraph(onnGraph, onnGraph.node(DQNodeIdx).input(0)))
      {
        isConstBuf = true;
      }
    }
  }
 return isConstBuf;
}


/** Function to compute number of variable inputs of layer
 * Also populates corresponding indices of variable inputs
 */
int32_t TIDL_onnxNumVariableLayerInputs(GraphProto& onnGraph, int32_t i, std::vector<int32_t>& varIdx)
{
  int32_t numVarInputs = 0;
  for(int j = 0; j < onnGraph.node(i).input_size(); j++)
  {
    if(! (TIDL_isInputConstInGraph(onnGraph, onnGraph.node(i).input(j)) || TIDL_doesDequantHaveConstTensor(onnGraph, onnGraph.node(i).input(j))))
    {
      numVarInputs++;
      varIdx.push_back(j);
    }
  }
  return numVarInputs;
}

int32_t TIDL_onnxGetAttrIdx(const NodeProto& node, char * name)
{
  int32_t i;
  for (i = 0; i < node.attribute_size(); i++)
  {
    if ((strcmp(node.attribute(i).name().c_str(), name) == 0))
    {
      return(i);
    }
  }
  return (-1);
}

std::vector<int32_t> getNodeOutputShape(GraphProto& onnxGraph, const std::string name, int32_t debugLevel)
{
  std::vector<int32_t> dims;
  for (int k = 0; k < onnxGraph.output_size(); k++)
  {
    if((strcmp(onnxGraph.output(k).name().c_str(), name.c_str()) == 0))
    {
      if( onnxGraph.output(k).type().tensor_type().has_shape() )
      {
        auto& tensorShape = onnxGraph.output(k).type().tensor_type().shape();
        for (int j = 0; j < tensorShape.dim_size(); j++)
        {
          dims.push_back(tensorShape.dim(j).dim_value());
        }
      }
    }
  }
  for (int k = 0; k < onnxGraph.value_info_size(); k++)
  {
    if((strcmp(onnxGraph.value_info(k).name().c_str(), name.c_str()) == 0))
    {
      if( onnxGraph.value_info(k).type().tensor_type().has_shape() )
      {
        auto& tensorShape = onnxGraph.value_info(k).type().tensor_type().shape();
        for (int j = 0; j < tensorShape.dim_size(); j++)
        {
          dims.push_back(tensorShape.dim(j).dim_value());
        }
      }
    }
  }
  return dims;
}

std::vector<int32_t> getNodeInputShape(GraphProto& onnxGraph, const std::string name, int32_t debugLevel)
{
  std::vector<int32_t> dims;
  for (int k = 0; k < onnxGraph.input_size(); k++)
  {
    if((strcmp(onnxGraph.input(k).name().c_str(), name.c_str()) == 0))
    {
      if( onnxGraph.input(k).type().tensor_type().has_shape() )
      {
        auto& tensorShape = onnxGraph.input(k).type().tensor_type().shape();
        for (int j = 0; j < tensorShape.dim_size(); j++)
        {
          dims.push_back(tensorShape.dim(j).dim_value());
        }
      }
    }
  }

  for (int k = 0; k < onnxGraph.value_info_size(); k++)
  {
    if((strcmp(onnxGraph.value_info(k).name().c_str(), name.c_str()) == 0))
    {
      if( onnxGraph.value_info(k).type().tensor_type().has_shape() )
      {
        auto& tensorShape = onnxGraph.value_info(k).type().tensor_type().shape();
        for (int j = 0; j < tensorShape.dim_size(); j++)
        {
          dims.push_back(tensorShape.dim(j).dim_value());
        }
      }
    }
  }

  for (int k = 0; k < onnxGraph.output_size(); k++)
  {
    if((strcmp(onnxGraph.output(k).name().c_str(), name.c_str()) == 0))
    {
      if( onnxGraph.output(k).type().tensor_type().has_shape() )
      {
        auto& tensorShape = onnxGraph.output(k).type().tensor_type().shape();
        for (int j = 0; j < tensorShape.dim_size(); j++)
        {
          dims.push_back(tensorShape.dim(j).dim_value());
        }
      }
    }
  }
  return dims;
}

int32_t TIDL_onnxGetIntAttr(const NodeProto& node, char * name, int32_t * valuePtr, int32_t idx)
{
  int32_t i = TIDL_onnxGetAttrIdx(node, name);
  if(i != -1)
  {
    if (node.attribute(i).ints_size() > idx)
    {
      *valuePtr = node.attribute(i).ints(idx);
    }
    else if (idx == 0)
    {
      *valuePtr = node.attribute(i).i();
    }
    else
    {
      return (-1);
    }
    return (0);
  }
  return (-1);
}

int32_t TIDL_onnxGetFloatAttr(const NodeProto& node, char * name, float * valuePtr, int32_t idx)
{
  int32_t i = TIDL_onnxGetAttrIdx(node, name);
  if (i != -1)
  {
    if (node.attribute(i).floats_size() > idx)
    {
      *valuePtr = node.attribute(i).floats(idx);
    }
    else if (idx == 0)
    {
      *valuePtr = node.attribute(i).f();
    }
    else
    {
      return (-1);
    }
    return (0);
  }
  return (-1);
}

int32_t TIDL_geTensorDim(TensorProto& tensor, int32_t idx)
{
  if (tensor.dims_size() > idx)
  {
    return(tensor.dims(idx));
  }
  else
  {
    return 0;
  }
}


int32_t TIDL_onnxGetStringAttr(const NodeProto& node, char * name, char * valuePtr, int32_t idx)
{
  strcpy(valuePtr, "");
  int32_t i = TIDL_onnxGetAttrIdx(node, name);
  if (i != -1)
  {
      strcpy(valuePtr, node.attribute(i).s().c_str());
      return (0);
  }
  return (-1);
}


TensorProto TIDL_getInitializerTensor(GraphProto& onnGraph, const string name, int32_t nIdx)
{
  int i;

  for (i = 0; i < onnGraph.initializer_size(); i++)
  {
    if ((strcmp(onnGraph.initializer(i).name().c_str(), name.c_str()) == 0))
    {
      auto & tensor = onnGraph.initializer(i);
      return(tensor);
    }
  }
  for (i = 0; i < onnGraph.node_size(); i++)
  {
    if ((strcmp(onnGraph.node(i).output(0).c_str(), name.c_str()) == 0) && (strcmp(onnGraph.node(i).op_type().c_str(), "Constant") == 0))
    {
      auto & tensor = onnGraph.node(i).attribute(0).t();
      return(tensor);
    }
    else if((strcmp(onnGraph.node(i).output(0).c_str(), name.c_str()) == 0) && (strcmp(onnGraph.node(i).op_type().c_str(), "DequantizeLinear") == 0))
    {
      int32_t DQNodeIdx = TIDL_onnxGetNodeIdx(onnGraph, name.c_str());
      /*Return DQ's input (inIdx)*/
      if(DQNodeIdx != -1)
      {
        return (TIDL_getInitializerTensor(onnGraph, onnGraph.node(DQNodeIdx).input(0) , DQNodeIdx));
      }
      else
      {
        continue;
      }
    }
  }
  printf("Could not find const or initializer of layer %s !!!\n", onnGraph.node(nIdx).name().c_str());
  return (onnx::TensorProto::default_instance());
}

bool TIDL_tfIsTensorDQ(int32_t dType)
{
  bool isDQ = false;
  if(dType == TensorProto_DataType_INT32  ||
     dType == TensorProto_DataType_INT8   ||
     dType == TensorProto_DataType_UINT8)
     {
      isDQ = true;
     }
  return isDQ;
}

int32_t TIDL_tfCopyTensor(GraphProto& onnGraph, TensorProto& tensor, void ** ptr, int32_t * size, const string name)
{
  int32_t i, tensorSize = 1;

  * ptr = NULL;
  if (tensor.data_type() == TensorProto_DataType_FLOAT) //DT_FLOAT
  {
    float *dst;
    for (i = 0; i < tensor.dims_size(); i++)
    {
      tensorSize *= tensor.dims(i);
    }
    *size = tensorSize;
    dst = (float *)my_malloc(*size *sizeof(float));

    if (tensor.float_data_size() > 0)
    {
      if (tensor.float_data_size() != tensorSize)
      {
        printf("Tensor size and Dims size not matching !! \n");
      }

      for (i = 0; i < tensorSize; i++)
      {
        dst[i] = tensor.float_data(i);
      }
    }
    else if (tensor.raw_data().size() > 0)
    {
      if ((tensor.raw_data().size() / sizeof(float)) != tensorSize)
      {
        printf("Tensor size and Dims size not matching !! \n");
      }
      memcpy(dst, (float *)tensor.raw_data().c_str(), tensor.raw_data().size());
    }
    *ptr = dst;
    return 0;
  }
  else if (tensor.data_type() == TensorProto_DataType_INT64) //DT_FLOAT
  {
    long long int *dst;
    for (i = 0; i < tensor.dims_size(); i++)
    {
      tensorSize *= tensor.dims(i);
    }
    *size = tensorSize;
    dst = (long long int *)my_malloc(*size *sizeof(long long int));

    if (tensor.int64_data_size() > 0)
    {
      if (tensor.int64_data_size() != tensorSize)
      {
        printf("Tensor size and Dims size not matching !! \n");
      }

      for (i = 0; i < tensorSize; i++)
      {
        dst[i] = tensor.int64_data(i);
      }
    }
    else if (tensor.raw_data().size() > 0)
    {
      if ((tensor.raw_data().size() / sizeof(long long int)) != tensorSize)
      {
        printf("Tensor size and Dims size not matching !! \n");
      }
      memcpy(dst, (long long int *)tensor.raw_data().c_str(), tensor.raw_data().size());
    }
    *ptr = dst;
    return 0;
  }
  /*All DQ Combinations:*/
  else if(TIDL_tfIsTensorDQ(tensor.data_type()))
  {
      /*Get DQ Node:*/
      bool tensorScaleFromRaw = false;
      bool tensorZPFromRaw = false;
      int32_t DQNodeIdx = TIDL_onnxGetNodeIdxFromInput(onnGraph, tensor.name().c_str());
      if(DQNodeIdx == -1)
      {
        printf("Unsupported data type for onnxImport\n");
        return -1;
      }
      TensorProto tensorScales  = TIDL_getInitializerTensor(onnGraph, onnGraph.node(DQNodeIdx).input(1), DQNodeIdx);
      TensorProto tensorZeroPoints = TIDL_getInitializerTensor(onnGraph, onnGraph.node(DQNodeIdx).input(2), DQNodeIdx);
      tensorScaleFromRaw = tensorScales.float_data_size() == 0;
      tensorZPFromRaw = tensorZeroPoints.int32_data_size() == 0;
      /*Scales and ZeroPoints are 1D*/
      //assert(tensorScales.dims(0) == tensorZeroPoints.dims(0));
      int32_t numOutChannels = 1;
      if(tensorScales.dims_size() > 0)
      {
        numOutChannels = tensorScales.dims(0);
      }

      int32_t i,j;

      for(i = 0 ; i < tensor.dims_size(); i++)
      {
        tensorSize *= tensor.dims(i);
      }
      *size = tensorSize;
      int32_t perChannelSize = tensorSize / numOutChannels;
      /*Allocate float buffer:*/
      float *output = NULL;
      output = (float*) malloc(sizeof(float) * tensorSize);
      *ptr = output;
      if(output == NULL)
      {
        printf("Unable to allocate buffer for dequantized parameters\n");
        return (-1);
      }
      if(tensor.data_type() == TensorProto_DataType_INT32)
      {
        if(TensorProto_DataType_INT32 == tensorZeroPoints.data_type())
        {
          for(i = 0; i < numOutChannels; i++)
          {
            int32_t tensorZeroPoint;
            float  tensorScale;

            if(tensorZPFromRaw)
            {
              tensorZeroPoint = ((int32_t* )(tensorZeroPoints.raw_data().c_str()))[i];
            }
            else
            {
              tensorZeroPoint = (int32_t)(tensorZeroPoints.int32_data(i));
            }

            if(tensorScaleFromRaw)
            {
              tensorScale = ((float* )(tensorScales.raw_data().c_str()))[i];
            }
            else
            {
              tensorScale = (float )(tensorScales.float_data(i));
            }
            for(j = 0; j < perChannelSize; j++)
            {
              int32_t quantizedTensorVal = ((int32_t* )(tensor.raw_data().c_str()))[(i * perChannelSize) + j];
              output[(i * perChannelSize) + j] = ((((float)quantizedTensorVal) - tensorZeroPoint) * tensorScale);
            }
          }
        }
      }
      else if(tensor.data_type() == TensorProto_DataType_INT8)
      {
        if(TensorProto_DataType_INT8 == tensorZeroPoints.data_type())
        {
          for(i = 0; i < numOutChannels; i++)
          {
            int8_t tensorZeroPoint;
            float  tensorScale;

            if(tensorZPFromRaw)
            {
              tensorZeroPoint = ((int8_t* )(tensorZeroPoints.raw_data().c_str()))[i];
            }
            else
            {
              tensorZeroPoint = (int8_t)(tensorZeroPoints.int32_data(i));
            }

            if(tensorScaleFromRaw)
            {
              tensorScale = ((float* )(tensorScales.raw_data().c_str()))[i];
            }
            else
            {
              tensorScale = (float )(tensorScales.float_data(i));
            }
            for(j = 0; j < perChannelSize; j++)
            {
              int8_t quantizedTensorVal = ((int8_t* )(tensor.raw_data().c_str()))[(i * perChannelSize) + j];
              output[(i * perChannelSize) + j] = ((((float)quantizedTensorVal) - tensorZeroPoint) * tensorScale);
            }
          }
        }
        else if(TensorProto_DataType_UINT8 == tensorZeroPoints.data_type())
        {
          for(i = 0; i < numOutChannels; i++)
          {
            uint8_t tensorZeroPoint;
            float  tensorScale;

            if(tensorZPFromRaw)
            {
              tensorZeroPoint = ((uint8_t* )(tensorZeroPoints.raw_data().c_str()))[i];
            }
            else
            {
              tensorZeroPoint = (uint8_t)(tensorZeroPoints.int32_data(i));
            }

            if(tensorScaleFromRaw)
            {
              tensorScale = ((float* )(tensorScales.raw_data().c_str()))[i];
            }
            else
            {
              tensorScale = (float )(tensorScales.float_data(i));
            }
            for(j = 0; j < perChannelSize; j++)
            {
              int8_t quantizedTensorVal = ((int8_t* )(tensor.raw_data().c_str()))[(i * perChannelSize) + j];
              output[(i * perChannelSize) + j] = ((((float)quantizedTensorVal) - tensorZeroPoint) * tensorScale);
            }
          }
        }
      }
      else if(tensor.data_type() == TensorProto_DataType_UINT8)
      {
        if(TensorProto_DataType_INT8 == tensorZeroPoints.data_type())
        {
          for(i = 0; i < numOutChannels; i++)
          {
            int8_t tensorZeroPoint;
            float  tensorScale;

            if(tensorZPFromRaw)
            {
              tensorZeroPoint = ((int8_t* )(tensorZeroPoints.raw_data().c_str()))[i];
            }
            else
            {
              tensorZeroPoint = (int8_t)(tensorZeroPoints.int32_data(i));
            }

            for(j = 0; j < perChannelSize; j++)
            {
              uint8_t quantizedTensorVal = ((uint8_t* )(tensor.raw_data().c_str()))[(i * perChannelSize) + j];
              output[(i * perChannelSize) + j] = ((((float)quantizedTensorVal) - tensorZeroPoint) * tensorScale);
            }
          }
        }
        else if(TensorProto_DataType_UINT8 == tensorZeroPoints.data_type())
        {
          for(i = 0; i < numOutChannels; i++)
          {
            uint8_t tensorZeroPoint;
            float  tensorScale;

            if(tensorZPFromRaw)
            {
              tensorZeroPoint = ((uint8_t* )(tensorZeroPoints.raw_data().c_str()))[i];
            }
            else
            {
              tensorZeroPoint = (uint8_t)(tensorZeroPoints.int32_data(i));
            }

            if(tensorScaleFromRaw)
            {
              tensorScale = ((float* )(tensorScales.raw_data().c_str()))[i];
            }
            else
            {
              tensorScale = (float )(tensorScales.float_data(i));
            }
            for(j = 0; j < perChannelSize; j++)
            {
              uint8_t quantizedTensorVal = ((uint8_t* )(tensor.raw_data().c_str()))[(i * perChannelSize) + j];
              output[(i * perChannelSize) + j] = ((((float)quantizedTensorVal) - tensorZeroPoint) * tensorScale);
            }
          }
        }
      }
  }
  else
  {
    printf("\nUnsupported Onnx import data type : %d \n", tensor.data_type());
    return -1;
  }
  return 0;
}


TensorProto TIDL_getConstInTensorFromDQLinear(GraphProto& onnGraph, const string name, int32_t inputIdx)
{
  /*Find the corresponding DQ layer via it's output name*/
  int32_t DQNodeIdx = TIDL_onnxGetNodeIdx(onnGraph, name.c_str());
  /*Return DQ's input (inIdx)*/
  return (TIDL_getInitializerTensor(onnGraph, onnGraph.node(DQNodeIdx).input(inputIdx) , DQNodeIdx));
}



/*
  nIdx = Node IDX (e.g. Conv)
  inIdx = Which input to the conv layer
*/
int32_t TIDL_tfDequantizeAndCopyFloatTensor(GraphProto& onnGraph, int32_t nIdx, int32_t inIdx, sBuffer_t &buf)
{
  /*Identify the corresponding DQ node and retrieve it's input tensors to perform dequantization*/
  TensorProto quantizedTensor = TIDL_getConstInTensorFromDQLinear(onnGraph, onnGraph.node(nIdx).input(inIdx), 0);
  TensorProto tensorScales  = TIDL_getConstInTensorFromDQLinear(onnGraph, onnGraph.node(nIdx).input(inIdx), 1);
  TensorProto tensorZeroPoints = TIDL_getConstInTensorFromDQLinear(onnGraph, onnGraph.node(nIdx).input(inIdx), 2);
   /*Determine dimensions:*/
  bool perAxis = false;
  /*Scales and ZeroPoints are 1D*/
  assert(tensorScales.dims(0) == tensorZeroPoints.dims(0));
  int32_t numOutChannels = tensorScales.dims(0);
  if(numOutChannels == 1)
  {
    perAxis = false;
  }
  else
  {
    perAxis = true;
  }

  int32_t tensorSize = 1;
  int32_t i,j;
  for(i = 0 ; i < quantizedTensor.dims_size(); i++)
  {
    tensorSize *= quantizedTensor.dims(i);
  }
  int32_t perChannelSize = tensorSize / numOutChannels;
  /*Allocate float buffer:*/
  buf.ptr = (float*) malloc(sizeof(float) * tensorSize);
  if(buf.ptr == NULL)
  {
    printf("Unable to allocate buffer for convolution parameters\n");
    return (-1);
  }
  buf.bufSize = tensorSize;
  float *output = (float*)buf.ptr;
  /*Dequantize and copy over:*/
  if(TensorProto_DataType_INT8 == quantizedTensor.data_type())
  {
    if(TensorProto_DataType_INT8 == tensorZeroPoints.data_type())
    {
      for(i = 0; i < numOutChannels; i++)
      {
        int8_t tensorZeroPoint = ((int8_t* )(tensorZeroPoints.raw_data().c_str()))[i];
        float  tensorScale     = ((float* )(tensorScales.raw_data().c_str()))[i];
        for(j = 0; j < perChannelSize; j++)
        {
          int8_t quantizedTensorVal = ((int8_t* )(quantizedTensor.raw_data().c_str()))[(i * perChannelSize) + j];
          output[(i * perChannelSize) + j] = (((float)quantizedTensorVal)/tensorScale) + tensorZeroPoint;
        }
      }
    }
    else if(TensorProto_DataType_UINT8 == tensorZeroPoints.data_type())
    {
      for(i = 0; i < numOutChannels; i++)
      {
        uint8_t tensorZeroPoint = ((int8_t* )(tensorZeroPoints.raw_data().c_str()))[i];
        float  tensorScale     = ((float* )(tensorScales.raw_data().c_str()))[i];
        for(j = 0; j < perChannelSize; j++)
        {
          int8_t quantizedTensorVal = ((int8_t* )(quantizedTensor.raw_data().c_str()))[(i * perChannelSize) + j];
          output[(i * perChannelSize) + j] = (((float)quantizedTensorVal)/tensorScale) + tensorZeroPoint;
        }
      }
    }
  }
  else if(TensorProto_DataType_UINT8 == quantizedTensor.data_type())
  {
    if(TensorProto_DataType_INT8 == tensorZeroPoints.data_type())
    {
      for(i = 0; i < numOutChannels; i++)
      {
        int8_t tensorZeroPoint = ((int8_t* )(tensorZeroPoints.raw_data().c_str()))[i];
        float  tensorScale     = ((float* )(tensorScales.raw_data().c_str()))[i];
        for(j = 0; j < perChannelSize; j++)
        {
          uint8_t quantizedTensorVal = ((uint8_t* )(quantizedTensor.raw_data().c_str()))[(i * perChannelSize) + j];
          output[(i * perChannelSize) + j] = (((float)quantizedTensorVal)/tensorScale) + tensorZeroPoint;
        }
      }
    }
    else if(TensorProto_DataType_UINT8 == tensorZeroPoints.data_type())
    {
      for(i = 0; i < numOutChannels; i++)
      {
        uint8_t tensorZeroPoint = ((int8_t* )(tensorZeroPoints.raw_data().c_str()))[i];
        float  tensorScale     = ((float* )(tensorScales.raw_data().c_str()))[i];
        for(j = 0; j < perChannelSize; j++)
        {
          uint8_t quantizedTensorVal = ((uint8_t* )(quantizedTensor.raw_data().c_str()))[(i * perChannelSize) + j];
          output[(i * perChannelSize) + j] = (((float)quantizedTensorVal)/tensorScale) + tensorZeroPoint;
        }
      }
    }
  }
  else if(TensorProto_DataType_INT32 == quantizedTensor.data_type())
  {
    if(TensorProto_DataType_INT32 == tensorZeroPoints.data_type())
    {
      for(i = 0; i < numOutChannels; i++)
      {
        int32_t tensorZeroPoint = ((int32_t* )(tensorZeroPoints.raw_data().c_str()))[i];
        float  tensorScale     = ((float* )(tensorScales.raw_data().c_str()))[i];
        for(j = 0; j < perChannelSize; j++)
        {
          int32_t quantizedTensorVal = ((int32_t* )(quantizedTensor.raw_data().c_str()))[(i * perChannelSize) + j];
          output[(i * perChannelSize) + j] = (((float)quantizedTensorVal)/tensorScale) + tensorZeroPoint;
        }
      }
    }
    else
    {
      printf("Error: Unable to dequantize bias values!\n");
    }
  }
  else if(TensorProto_DataType_UINT32 == quantizedTensor.data_type())
  {
    if(TensorProto_DataType_UINT32 == tensorZeroPoints.data_type())
    {
      for(i = 0; i < numOutChannels; i++)
      {
        uint32_t tensorZeroPoint = ((uint32_t* )(tensorZeroPoints.raw_data().c_str()))[i];
        float  tensorScale     = ((float* )(tensorScales.raw_data().c_str()))[i];
        for(j = 0; j < perChannelSize; j++)
        {
          uint32_t quantizedTensorVal = ((uint32_t* )(quantizedTensor.raw_data().c_str()))[(i * perChannelSize) + j];
          output[(i * perChannelSize) + j] = (((float)quantizedTensorVal)/tensorScale) + tensorZeroPoint;
        }
      }
    }
    else
    {
      printf("Error: Unable to dequantize bias values!\n");
    }
  }
  return 0;
}

int32_t TIDL_tfCopyFloatInitializerTensor(GraphProto& onnGraph, int32_t nIdx, int32_t inIdx, sBuffer_t &buf)
{
  if (onnGraph.node(nIdx).input_size() > inIdx)
  {
    TensorProto tensor = TIDL_getInitializerTensor(onnGraph, onnGraph.node(nIdx).input(inIdx), nIdx);
    return TIDL_tfCopyTensor(onnGraph, tensor, &buf.ptr, &buf.bufSize, onnGraph.node(nIdx).input(inIdx));
  }
  else
  {
    /*Particular initializer/input does not exist (This is valid situation when the input is optional)*/
    return -1;
  }
}

int32_t TIDL_tfCopyFloatConst(GraphProto& onnGraph, int32_t nIdx, int32_t inIdx, sBuffer_t &buf)
{
  if (onnGraph.node(nIdx).input_size() > inIdx)
  {
    TensorProto tensor = TIDL_getInitializerTensor(onnGraph, onnGraph.node(nIdx).input(inIdx), nIdx);
    return TIDL_tfCopyTensor(onnGraph, tensor, &buf.ptr, &buf.bufSize, onnGraph.node(nIdx).input(inIdx));
  }
  else
  {
    printf("Requested Constant tensor %d in %s is not found", inIdx, onnGraph.node(nIdx).name().c_str());
    return -1;
  }
  return 0;
}

/** Broadcast floating point vector buffer to have a modified dimension */
void TIDL_onnxBroadcastFloatTensor(sBuffer_t &buf, int32_t bufSize)
{
  float * ptr = (float *)buf.ptr;
  float val = ptr[0]; //only one const val for computation
  void * newPtr = realloc(ptr, bufSize * sizeof(float)); //ptr is void* so it should be reallocated to a void * ptr
  float *newPtrCast = (float *)newPtr;
  for(int i = 0; i < bufSize; i++)
  {
    newPtrCast[i] = val;  //broadcast the constant along number of channels
  }
  buf.ptr = newPtr;
  buf.bufSize = bufSize;
}

/** Function to save layer level tensor properties metadata to be used for allowlisting
 * Saves data for sTIDL_allowlistingMetaData structure
 */
void TIDL_onnxSaveAllowlistingMetaData(GraphProto&   onnGraph, int32_t i, sTIDL_LayerPC_t &layer)
{
  layer.allowlistingMetaData.numInputs = onnGraph.node(i).input_size();
  layer.allowlistingMetaData.numOutputs = onnGraph.node(i).output_size();
  layer.allowlistingMetaData.numValidInputs = layer.allowlistingMetaData.numInputs;
  layer.allowlistingMetaData.validInputIndices = {};
  layer.allowlistingMetaData.numVarInputs = TIDL_onnxNumVariableLayerInputs(onnGraph, i,  layer.allowlistingMetaData.varTensorIndices);
  layer.allowlistingMetaData.numConstInputs = layer.allowlistingMetaData.numValidInputs - layer.allowlistingMetaData.numVarInputs;

  //Find indices of constant tensors
  for(int j = 0; j < onnGraph.node(i).input_size(); j++)
  {
    if(std::find(layer.allowlistingMetaData.varTensorIndices.begin(), layer.allowlistingMetaData.varTensorIndices.end(), j) == layer.allowlistingMetaData.varTensorIndices.end())
    //idx not found in variable tensors indices => constant tensor idx
    {
      layer.allowlistingMetaData.constTensorIndices.push_back(j);
    }
  }
  //Populate dimensions of variable tensors
  for(auto& varIdx : layer.allowlistingMetaData.varTensorIndices)
  {
    layer.allowlistingMetaData.varTensorsDims.push_back(getNodeInputShape(onnGraph,  onnGraph.node(i).input(varIdx), 0));
  }
  //Populate dimensions of constant tensors
  for(auto& constIdx : layer.allowlistingMetaData.constTensorIndices)
  {
    TensorProto tensor = TIDL_getInitializerTensor(onnGraph, onnGraph.node(i).input(constIdx), i);
    layer.allowlistingMetaData.constTensorNames.push_back(onnGraph.node(i).input(constIdx).c_str());
    std::vector<int32_t> dims;
    for(int j = 0; j < tensor.dims_size(); j++)
    {
      dims.push_back(tensor.dims(j));
    }
    layer.allowlistingMetaData.constTensorsDims.push_back(dims);
  }
  //Populate dimensions of output tensors
  for(int outputIdx=0; outputIdx<layer.allowlistingMetaData.numOutputs; outputIdx++)
  {
    layer.allowlistingMetaData.outputTensorDims.push_back(getNodeOutputShape(onnGraph,  onnGraph.node(i).output(outputIdx), 0));
  }
}

/* Check if Reshape-Transpose-Reshape corresponds to channel-wise shuffle operation */
bool TIDL_checkShuffleOperation(GraphProto& onnxGraph, std::vector<int>supportedNodesIndices)
{
  bool isSupported = false;
  sBuffer_t reshape1OutShape, reshape2OutShape, transposeOutShape;

  int32_t reshape1Idx = supportedNodesIndices[0];
  int32_t transposeIdx = supportedNodesIndices[1];
  int32_t reshape2Idx = supportedNodesIndices[2];

  TIDL_tfCopyFloatInitializerTensor(onnxGraph, reshape1Idx, 1, reshape1OutShape);
  TIDL_tfCopyFloatInitializerTensor(onnxGraph, reshape2Idx, 1, reshape2OutShape);
  NodeProto node = onnxGraph.node(transposeIdx);
  int32_t attrIdx = TIDL_onnxGetAttrIdx(node, "perm");
  if (attrIdx != -1)
  {
    int32_t j;
    int32_t dims = node.attribute(attrIdx).ints_size();
    transposeOutShape.ptr = (void*)malloc(sizeof(int)*dims);
    transposeOutShape.bufSize = dims;
    int32_t *dimVlaue = (int*)transposeOutShape.ptr;
    for (j = 0; j < transposeOutShape.bufSize; j++)
    {
      dimVlaue[j] = node.attribute(attrIdx).ints(j);
    }
  }
  if ((reshape1OutShape.bufSize ==  5) && (transposeOutShape.bufSize ==  5) && (reshape2OutShape.bufSize == 4))
  {
    int64_t * reshape1  = (int64_t *)reshape1OutShape.ptr;
    int64_t * reshape2  = (int64_t *)reshape2OutShape.ptr;
    int32_t * transpose = (int32_t *)transposeOutShape.ptr;
    if ((reshape1[4] == reshape2[3]) && (reshape1[3] == reshape2[2]) && (4 == transpose[4]) && (3 == transpose[3]) && (1 == transpose[2]) && (2 == transpose[1]))
    {
      isSupported = true;
    }
  }
  if ((reshape1OutShape.bufSize ==  6) && (transposeOutShape.bufSize ==  6) && (reshape2OutShape.bufSize == 4))
  {
    int64_t * reshape1  = (int64_t *)reshape1OutShape.ptr;
    int64_t * reshape2  = (int64_t *)reshape2OutShape.ptr;
    int32_t * transpose = (int32_t *)transposeOutShape.ptr;
    if ((reshape1[4]*reshape1[2] == reshape2[2]) && (reshape1[3]*reshape1[5] == reshape2[3]) && (reshape1[1] == reshape2[1]) && (reshape1[2] == reshape1[3]) &&
        (3 == transpose[5]) && (5 == transpose[4]) && (2 == transpose[3]) && (4 == transpose[2]) && (1 == transpose[1]))
    {
      isSupported = true;
    }
    if(TIDL_isDevice("am62a")) /* Pixel shuffle layer is not supported on AM62A */
    {
      isSupported = false;
    }
  }

  return isSupported;
}



/* Check if Div-Erf-Add-Mul corresponds to GeLU operation

	checks:

*/
bool TIDL_checkGELUOperation(GraphProto& onnxGraph, std::vector<int>supportedNodesIndices)
{

	/*
	@TODO: make actual check for GELU
	*/
	bool isSupported = true;
	return isSupported;

}

int32_t TIDL_onnxMapConvBaseParams(GraphProto&   onnGraph, int32_t i, sTIDL_LayerPC_t &layer)
{
  int32_t status;
  int32_t padType;
  int32_t idx1, idx2;
  int32_t pads[4] = {0};
  int32_t kernel_shape[2];
  int32_t padL=-1, padR=-1, padT=-1, padB=-1;

  sTIDL_ConvParams_t &convParams      = layer.layerParams.convParams;
  layer.layerType = TIDL_ConvolutionLayer;

  TIDL_onnxSaveAllowlistingMetaData(onnGraph, i, layer);

  layer.strideOffsetMethod = TIDL_StrideOffsetTopLeft;

  layer.outData[0].elementType = tidl_getElementType(1);
  TensorProto tensor;

  tensor = TIDL_getInitializerTensor(onnGraph, onnGraph.node(i).input(1), i);
  convParams.numOutChannels = TIDL_geTensorDim(tensor, 0);
  convParams.numInChannels  = TIDL_geTensorDim(tensor, 1);
  convParams.kernelH        = TIDL_geTensorDim(tensor, 2);
  convParams.kernelW        = TIDL_geTensorDim(tensor, 3);

  convParams.numGroups = 1;
  convParams.dilationW = 1;
  convParams.dilationH = 1;
  convParams.strideW = 1;
  convParams.strideH = 1;
  convParams.padW = 0;
  convParams.padH = 0;
  convParams.enableBias = 0;
  convParams.enablePooling = 0;

  NodeProto node = onnGraph.node(i);
  TIDL_onnxGetIntAttr(node, "strides",   &convParams.strideH, 0);
  TIDL_onnxGetIntAttr(node, "strides",   &convParams.strideW, 1);
  TIDL_onnxGetIntAttr(node, "dilations", &convParams.dilationH, 0);
  TIDL_onnxGetIntAttr(node, "dilations", &convParams.dilationW, 1);
  TIDL_onnxGetIntAttr(node, "group",     &convParams.numGroups, 0);
  TIDL_onnxGetIntAttr(node, "kernel_shape", &kernel_shape[0], 0);
  TIDL_onnxGetIntAttr(node, "kernel_shape", &kernel_shape[1], 1);

  char autoPad[50];
  status = TIDL_onnxGetStringAttr(node, "auto_pad", autoPad, 0);
  if((status == -1) || (strcmp(autoPad, "NOTSET") == 0))
  {
    TIDL_onnxGetIntAttr(node, "pads",       &pads[0], 0);
    TIDL_onnxGetIntAttr(node, "pads",       &pads[1], 1);
    TIDL_onnxGetIntAttr(node, "pads",       &pads[2], 2);
    TIDL_onnxGetIntAttr(node, "pads",       &pads[3], 3);
    if((pads[0] < pads[2]) && (pads[1] < pads[3])) //padL < padR && padT < padB
    {
      layer.strideOffsetMethod = TIDL_StrideOffsetCenter;
    }
    pads[2] = max(pads[0], pads[2]);
    pads[3] = max(pads[1], pads[3]);
    padL = pads[1];
    padR = pads[3];
    padT = pads[0];
    padB = pads[2];
  }
  else if ((strcmp(autoPad, "SAME_UPPER") == 0) || (strcmp(autoPad, "SAME_LOWER") == 0))
  {
    /* TCs pertaining to this condition are not existent as of now, hence this cannot be validated */
    pads[3] = ((convParams.kernelW - 1)*convParams.dilationW) / 2;
    pads[2] = ((convParams.kernelH - 1)*convParams.dilationH) / 2;
    if(strcmp(autoPad, "SAME_UPPER") == 0)
    {
      layer.strideOffsetMethod = TIDL_StrideOffsetCenter;
    }
    else  //SAME_LOWER
    {
      layer.strideOffsetMethod = TIDL_StrideOffsetTopLeft;
    }
  }
  else if(strcmp(autoPad, "VALID") == 0)
  {
    pads[3] = 0;
    pads[2] = 0;
    padL = 0;
    padR = 0;
    padT = 0;
    padB = 0;
  }
  else
  {
    diagsInfo.push_back("ALLOWLISTING : CONV : Unsupported auto_pad attribute  -- file info - " + std::string(__FILE__) + " , " \
          + std::string(__func__) + " , " + std::to_string(__LINE__));
    return -1;
  }



/*
if ((pads[0] != 0) || (pads[1] != 0))
  {
    printf("Pading Only supported in H and W axis \n");
  }
*/
  convParams.padW = pads[3];
  convParams.padH = pads[2];

  convParams.padT = padT;
  convParams.padL = padL;
  convParams.padB = padB;
  convParams.padR = padR;

  if ((kernel_shape[0] != convParams.kernelH) || (kernel_shape[1] != convParams.kernelW))
  {
    printf("Weight Tensor size is not matching with Proto kernel_shape \n");
  }


  TIDL_tfCopyFloatInitializerTensor(onnGraph, i, 1, layer.weights);
  if (onnGraph.node(i).input_size() > 2)
  {
    convParams.enableBias = 1;
    TIDL_tfCopyFloatInitializerTensor(onnGraph, i, 2, layer.bias);
  }


  return 0;
}


int32_t TIDL_onnxMapConvParams(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure,
  int32_t              i,
  int32_t              layerIndex,
  int32_t              *dataIndex,
  GraphProto&            onnGraph)
{
  int32_t status;
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].dataId = (*dataIndex)++;
  status = TIDL_onnxMapConvBaseParams(onnGraph, i, TIDLPCLayers);
  return status;
}

int32_t TIDL_onnxMapConvTransposeBaseParams(GraphProto&   onnGraph, int32_t i, sTIDL_LayerPC_t &layer)
{
  int32_t status;
  int32_t padType;
  int32_t idx1, idx2;
  sTIDL_ConvParams_t &convParams = layer.layerParams.convParams;
  int32_t pads[4];
  int32_t kernel_shape[2];
  int32_t output_shape[2];

  layer.layerType = TIDL_Deconv2DLayer;

  layer.strideOffsetMethod = TIDL_StrideOffsetTopLeft;

  layer.outData[0].elementType = tidl_getElementType(1);

  TensorProto tensor = TIDL_getInitializerTensor(onnGraph, onnGraph.node(i).input(1), i);
  convParams.numInChannels =  TIDL_geTensorDim(tensor, 0);
  convParams.kernelH =        TIDL_geTensorDim(tensor, 2);
  convParams.kernelW =        TIDL_geTensorDim(tensor, 3);

  convParams.numGroups = 1;
  convParams.dilationW = 1;
  convParams.dilationH = 1;
  convParams.strideW = 1;
  convParams.strideH = 1;
  convParams.padW = 0;
  convParams.padH = 0;
  convParams.enableBias = 0;
  convParams.enablePooling = 0;

  NodeProto node = onnGraph.node(i);
  TIDL_onnxGetIntAttr(node, "strides", &convParams.strideH, 0);
  TIDL_onnxGetIntAttr(node, "strides", &convParams.strideW, 1);
  TIDL_onnxGetIntAttr(node, "dilations", &convParams.dilationH, 0);
  TIDL_onnxGetIntAttr(node, "dilations", &convParams.dilationW, 1);
  TIDL_onnxGetIntAttr(node, "group", &convParams.numGroups, 0);
  TIDL_onnxGetIntAttr(node, "kernel_shape", &kernel_shape[0], 0);
  TIDL_onnxGetIntAttr(node, "kernel_shape", &kernel_shape[1], 1);

  convParams.numOutChannels = TIDL_geTensorDim(tensor, 1) * convParams.numGroups;

  status = TIDL_onnxGetIntAttr(node, "output_shape", &output_shape[0], 0);
  if(status != -1)
  {
    TIDL_onnxGetIntAttr(node, "output_shape", &output_shape[0], 0);
    TIDL_onnxGetIntAttr(node, "output_shape", &output_shape[1], 1);

    layer.outData[0].dimValues[TIDL_DIM_HEIGHT] = -output_shape[0];
    layer.outData[0].dimValues[TIDL_DIM_WIDTH]  = -output_shape[1];

    pads[3] = ((convParams.kernelW - 1)*convParams.dilationW) / 2;
    pads[2] = ((convParams.kernelH - 1)*convParams.dilationH) / 2;
  }
  else
  {
    char autoPad[50];
    status = TIDL_onnxGetStringAttr(node, "auto_pad", autoPad, 0);
    if((status == -1) || (strcmp(autoPad, "NOTSET") == 0))
    {
      TIDL_onnxGetIntAttr(node, "pads",       &pads[0], 0);
      TIDL_onnxGetIntAttr(node, "pads",       &pads[1], 1);
      TIDL_onnxGetIntAttr(node, "pads",       &pads[2], 2);
      TIDL_onnxGetIntAttr(node, "pads",       &pads[3], 3);

      if((pads[0] < pads[2]) && (pads[1] < pads[3])) //padL < padR && padT < padB
      {
        layer.strideOffsetMethod = TIDL_StrideOffsetCenter;
      }
      pads[2] = max(pads[0], pads[2]);
      pads[3] = max(pads[1], pads[3]);
    }
    else if ((strcmp(autoPad, "SAME_UPPER") == 0) || (strcmp(autoPad, "SAME_LOWER") == 0))
    {
      pads[3] = ((convParams.kernelW - 1)*convParams.dilationW) / 2;
      pads[2] = ((convParams.kernelH - 1)*convParams.dilationH) / 2;
      if(strcmp(autoPad, "SAME_UPPER") == 0)
      {
        layer.strideOffsetMethod = TIDL_StrideOffsetCenter;
      }
      else  //SAME_LOWER
      {
        layer.strideOffsetMethod = TIDL_StrideOffsetTopLeft;
      }
    }
    else if(strcmp(autoPad, "VALID") == 0)
    {
      pads[3] = 0;
      pads[2] = 0;
    }
    else
    {
      diagsInfo.push_back("ALLOWLISTING : CONV : Unsupported auto_pad attribute  -- file info - " + std::string(__FILE__) + " , " \
          + std::string(__func__) + " , " + std::to_string(__LINE__));
      return -1;
    }
  }


/*
if ((pads[0] != 0) || (pads[1] != 0))
  {
    printf("Pading Only supported in H and W axis \n");
  }
*/
  convParams.padW = pads[3];
  convParams.padH = pads[2];
  if ((kernel_shape[0] != convParams.kernelH) || (kernel_shape[1] != convParams.kernelW))
  {
    printf("Weight Tensor size is not matching with Proto kernel_shape \n");
  }
  TIDL_tfCopyFloatInitializerTensor(onnGraph, i, 1, layer.weights);
  /*Permute 0123 -> 1023 for onnx deconv weights based on https://github.com/onnx/onnx/blob/main/docs/Operators.md#ConvTranspose*/
  /*W Shape: No X Ni/G X Fr X FC */
  int32_t i1, i2, i3;
  int32_t tensorSize = convParams.numOutChannels * convParams.numInChannels * convParams.kernelH * convParams.kernelW;
  void ** ptr;
  ptr = &layer.weights.ptr;
  float* srcWtPtr = (float*) *ptr;
  float* weightPtr = (float*)malloc(sizeof(float) * tensorSize);
  if(weightPtr == NULL)
  {
    printf("Error allocating transpose buffer for deconvolution\n");
    exit(-1);
  }
  int32_t linePitch = convParams.kernelH * convParams.kernelW;

  for(i1 = 0; i1 < convParams.numOutChannels; i1++)
  {
    for(i2 = 0; i2 < convParams.numInChannels; i2++)
    {
      for(i3 = 0; i3 < linePitch; i3++)
      {
        weightPtr[ i1 * (linePitch * convParams.numInChannels) + i2 * linePitch + i3] = srcWtPtr[i2 * (linePitch * convParams.numOutChannels) + i1 * linePitch  + i3 ];
      }
    }
  }
  free(*ptr);
  *ptr = (void*)weightPtr;
  /******************************************************************************************************************************/
  if (onnGraph.node(i).input_size() > 2)
  {
    convParams.enableBias = 1;
    TIDL_tfCopyFloatInitializerTensor(onnGraph, i, 2, layer.bias);
  }
   return 0;
}

int32_t TIDL_onnxMapConvTransposeParams(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure,
  int32_t              i,
  int32_t              layerIndex,
  int32_t              *dataIndex,
  GraphProto&            onnGraph)
{
  int32_t status;
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].dataId = (*dataIndex)++;
  status = TIDL_onnxMapConvTransposeBaseParams(onnGraph, i, TIDLPCLayers);
  return status;
}

int32_t TIDL_onnxMapSplitBaseParams(GraphProto&   onnGraph, int32_t i, sTIDL_LayerPC_t &layer)
{
  int32_t status, j;
  int32_t axis = 0;
  int32_t splitSize, splitIdx = -1, numDim, attrIdx = -1, split_status = -1, num_out_status = -1, num_outputs = 0;
  sTIDL_SliceLayerParams_t &sliceParams = layer.layerParams.sliceParams;

  layer.layerType = TIDL_SliceLayer;
  layer.numOutBufs = onnGraph.node(i).output_size();
  sliceParams.stride = 1;

  for (j = 0; j < layer.numOutBufs; j++)
  {
    layer.outData[j].elementType = tidl_getElementType(1);
  }

  NodeProto node = onnGraph.node(i);
  TIDL_onnxGetIntAttr(node, "axis", &axis, 0);

  TIDL_onnxSaveAllowlistingMetaData(onnGraph, i, layer);
  /** If shape inference is not done on model, we assume the tensor is of 4 dimensions by default*/
  if (layer.allowlistingMetaData.varTensorsDims.size() != 0)
  {
    numDim = layer.allowlistingMetaData.varTensorsDims[0].size();
    numDim = (numDim == 0)? 4:numDim;
  }
  else
  {
    numDim = 4;
  }
  layer.inData[0].numDim = numDim;

  layer.layerParams.sliceParams.axis = axis + (TIDL_DIM_MAX - numDim);

  sliceParams.slicePoints[0] = 0;
  splitIdx = TIDL_onnxGetAttrIdx(node, "split");

  if (splitIdx != -1)
  {
    split_status = splitIdx;
    splitSize = node.attribute(splitIdx).ints_size();
    if(splitSize != layer.numOutBufs)
    {
      diagsInfo.push_back("ALLOWLISTING : Split : Unsupported slice - axis parameters, in Slice  -- file info - " + std::string(__FILE__) + " , " \
            + std::string(__func__) + " , " + std::to_string(__LINE__));
      return -1;
    }
    for (j = 0; j < layer.numOutBufs; j++)
    {
      /* split attribute of split layer indicates the length of each slice */
      sliceParams.slicePoints[j+1] = sliceParams.slicePoints[j] + node.attribute(splitIdx).ints(j);
    }
  }
  else
  {
    /**Split can be a input in opset 18*/
    sBuffer_t buf;
    split_status = TIDL_tfCopyFloatInitializerTensor(onnGraph, i, 1, buf);
    if (split_status != -1)
    {
      if (buf.bufSize != layer.numOutBufs)
      {
        diagsInfo.push_back("ALLOWLISTING : Split : Unsupported slice - axis parameters, in Slice  -- file info - " + std::string(__FILE__) + " , " \
              + std::string(__func__) + " , " + std::to_string(__LINE__));
        return -1;
      }
      int64_t *split_ptr = (int64_t*)buf.ptr;
      for (j = 0; j < layer.numOutBufs; j++)
      {
        /* split attribute of split layer indicates the length of each slice */
        sliceParams.slicePoints[j+1] = sliceParams.slicePoints[j] + split_ptr[j];
      }
      free (buf.ptr);
      buf.ptr = NULL;
      buf.bufSize = 0;
    }
    else
    {
      for (j = 0; j < layer.numOutBufs; j++)
      {
        sliceParams.slicePoints[j] = -1;
      }
    }
  }

  num_out_status = TIDL_onnxGetIntAttr(node, "num_outputs", &num_outputs, 0);

  /* Either num_outputs or split should be defined, but not both*/
  if (num_out_status != -1 && split_status == -1)
  {
    if (layer.numOutBufs != num_outputs)
    {
      diagsInfo.push_back("ALLOWLISTING : Split : Unsupported slice - axis parameters, in Slice  -- file info - " + std::string(__FILE__) + " , " \
            + std::string(__func__) + " , " + std::to_string(__LINE__));
      return -1;
    }
  }

  return 0;
}
int32_t TIDL_onnxSplitParams(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure,
  int32_t              i,
  int32_t              layerIndex,
  int32_t              *dataIndex,
  GraphProto&            onnGraph)
{
  int32_t status, j;
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.numOutBufs = onnGraph.node(i).output_size();
  for (j = 0; j < TIDLPCLayers.numOutBufs; j++)
  {
    TIDLPCLayers.outData[j].dataId = (*dataIndex)++;
  }
  status = TIDL_onnxMapSplitBaseParams(onnGraph, i, TIDLPCLayers);
  return status;
}

int32_t TIDL_onnxMapSliceBaseParams(GraphProto&   onnGraph, int32_t i, sTIDL_LayerPC_t &layer)
{
  int32_t status, j;
  int32_t axis = 0, numDim;
  int32_t starts;
  sTIDL_SliceLayerParams_t &sliceParams = layer.layerParams.sliceParams;

  layer.layerType  = TIDL_SliceLayer;

  NodeProto node = onnGraph.node(i);
  sliceParams.stride = 1;

  if (onnxOpSetVersion == 9)
  {
    TIDL_onnxGetIntAttr(node, "axes", &axis, 0);

    TIDL_onnxGetIntAttr(node, "starts", &sliceParams.slicePoints[0], 0);
    TIDL_onnxGetIntAttr(node, "ends", &sliceParams.slicePoints[1], 0);
  }
  else if (onnxOpSetVersion == 11 || onnxOpSetVersion == 13)
  {
    if(node.input_size() < 4)
    {
      axis = 0;
    }
    else
    {
      if (node.input_size() == 5)
      {
        sBuffer_t buf;
        TIDL_tfCopyFloatConst(onnGraph, i, 4, buf);
        long long int *ptr = (long long int *)buf.ptr;
        if (ptr[0] != 1)
        {
          sliceParams.stride = ptr[0];
        }
        free(ptr);
      }

      sBuffer_t starts;
      TIDL_tfCopyFloatConst(onnGraph, i, 1, starts);
      long long int *startsPtr = (long long int *)starts.ptr;

      sBuffer_t ends;
      TIDL_tfCopyFloatConst(onnGraph, i, 2, ends);
      long long int *endsPtr = (long long int *)ends.ptr;

      sBuffer_t axes;
      TIDL_tfCopyFloatConst(onnGraph, i, 3, axes);
      long long int *axesPtr = (long long int *)axes.ptr;

      if(starts.bufSize > 1)
      {
        axis = 0;
      }
      else
      {
        sliceParams.slicePoints[0] = startsPtr[0];
        if(endsPtr[0] == 0x7fffffffffffffff)
        {
          sliceParams.slicePoints[1] = 0x7fffffff;
        }
        else if(endsPtr[0] == -1)
        {
          sliceParams.slicePoints[1] = -1;
        }
        else
        {
          sliceParams.slicePoints[1] = endsPtr[0];
        }
        axis = axesPtr[0];
      }
      free(startsPtr);
      free(endsPtr);
      free(axesPtr);
    }
  }
  else
  {
    printf("\n Slice layer : Unsupported onnxOpSetVersion %d -- %s\n", onnxOpSetVersion, onnGraph.node(i).name().c_str());
    /**
     * Disabling this check temporarily for swin development
     * return -1;
     */
  }

  TIDL_onnxSaveAllowlistingMetaData(onnGraph, i, layer);
  /** If shape inference is not done on model, we assume the tensor is of 4 dimensions by default*/
  if (layer.allowlistingMetaData.varTensorsDims.size() != 0)
  {
    numDim = layer.allowlistingMetaData.varTensorsDims[0].size();
    numDim = (numDim == 0)? 4:numDim;
  }
  else
  {
    numDim = 4;
  }

  /** Adjust axis to TIDL dimensions*/
  axis += (TIDL_DIM_MAX - numDim);
  layer.layerParams.sliceParams.axis = axis;

  if ((axis < TIDL_DIM_NUMCH) || (axis > TIDL_DIM_WIDTH))
  {
    printf("\n Unsupported slice - axis parameters %d , in Slice -- %s \n", axis, onnGraph.node(i).name().c_str());
  }
  return 0;
}
int32_t TIDL_onnxSliceParams(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure,
  int32_t              i,
  int32_t              layerIndex,
  int32_t              *dataIndex,
  GraphProto&            onnGraph)
{
  int32_t status;
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].dataId = (*dataIndex)++;
  status = TIDL_onnxMapSliceBaseParams(onnGraph, i, TIDLPCLayers);
  return status;
}

int32_t TIDL_onnxGemmBaseParams(GraphProto&   onnGraph, int32_t i, sTIDL_LayerPC_t &layer)
{
  int32_t status = 0;
  int32_t transA = 0;
  int32_t transB = 1;
  float alpha = 1.0;
  float beta  = 1.0;
  sTIDL_InnerProductParams_t &innerProductParams = layer.layerParams.innerProductParams;

  layer.layerType = TIDL_InnerProductLayer;
  layer.outData[0].elementType = tidl_getElementType(1);

  //Do ONNX attribute checks
  NodeProto node = onnGraph.node(i);

  if(TIDL_onnxGetIntAttr(node, "transA", &transA, 0) == -1)
  {
    transA = 0;
  }

  if(TIDL_onnxGetIntAttr(node, "transB", &transB, 0) == -1)
  {
    transB = 0;
  }

  if(TIDL_onnxGetFloatAttr(node, "alpha", &alpha, 0) == -1)
  {
    alpha = 1.0;
  }

  if(TIDL_onnxGetFloatAttr(node, "beta", &beta, 0) == -1)
  {
    beta = 1.0;
  }

  innerProductParams.inputATranspose = transA;
  innerProductParams.inputBTranspose = transB;

  if ((transA != 0) || (alpha != 1.0) || (beta != 1.0))
  {
    diagsInfo.push_back("ALLOWLISTING : GEMM : Only supported Gemm Params are transA = 0, alpha = 1.0 and beta = 1.0. The same will processed as Inner product or fully connected layer in TIDL  -- file info - "\
    + std::string(__FILE__) + " , " + std::string(__func__) + " , " + std::to_string(__LINE__));
    return -1;
  }

  TIDL_onnxSaveAllowlistingMetaData(onnGraph, i, layer);

  status = TIDL_checkInnerProductTensorProperties(layer);
  if(status == -1) return -1;

  sTIDL_allowlistingMetaData md = layer.allowlistingMetaData;

  /** 
   * Set the numInBufs to 2 in case of MatMul operator
   * In case of Gemm operator keep the numInBufs to 1 to avoid the addition of const data layer (which results in layer going through MatMul flow)
  */
  if (strcmp ("Gemm", (char*)node.op_type().c_str()) == 0)
  {
    layer.numInBufs = 1;
  }
  else
  {
    layer.numInBufs = 2;
  }

  if((md.constTensorIndices.size() == 1) || (md.constTensorIndices.size() == 2)) //filter, bias. If there is no const means it pure matrix multiplication with both inputs as variable
  {
    int32_t filterIdx = md.constTensorIndices[0]; //first constant tensor should be filter
    TensorProto filterTensor = TIDL_getInitializerTensor(onnGraph, onnGraph.node(i).input(filterIdx), i);
    int32_t filter_rows, filter_cols;

    {
      // 1xK(inp) X KxN(wt)   = 1xN --> M = 1
      // filterTensor.dims(0) = K
      // filterTensor.dims(1) = N

      int dimDiff = TIDL_DIM_MAX -  md.constTensorsDims[0].size();

      filter_rows = md.constTensorsDims[0][TIDL_DIM_HEIGHT - dimDiff];
      filter_cols = md.constTensorsDims[0][TIDL_DIM_WIDTH - dimDiff];

      if (md.varTensorIndices[0] < md.constTensorIndices[0])
      {
        /* If const is the second input*/
        if(transB)
        {
          innerProductParams.numOutCols = filter_rows;
          innerProductParams.numInCols  = filter_cols;
        }
        else
        {
          innerProductParams.numOutCols = filter_cols;
          innerProductParams.numInCols  = filter_rows;
        }
      }
      else 
      {
        /* If const is the second input*/
        if(transA)
        {
          innerProductParams.numInCols = filter_rows;
          innerProductParams.numInRows  = filter_cols;
        }
        else
        {
          innerProductParams.numInCols = filter_cols;
          innerProductParams.numInRows  = filter_rows;
        }
      }

      layer.inData[1].dimValues[TIDL_DIM_BATCH]  = (filterTensor.dims_size() > 3) ? filterTensor.dims(filterTensor.dims_size() - 4) : 1;
      layer.inData[1].dimValues[TIDL_DIM_NUMCH]  = (filterTensor.dims_size() > 2) ? filterTensor.dims(filterTensor.dims_size() - 3) : 1;
      layer.inData[1].dimValues[TIDL_DIM_HEIGHT] = filterTensor.dims(filterTensor.dims_size() - 2);
      layer.inData[1].dimValues[TIDL_DIM_WIDTH]  = filterTensor.dims(filterTensor.dims_size() - 1);

      /* Internally channels*batches is treated as num of batches for matrix multiplication*/
      innerProductParams.numBatches = layer.inData[1].dimValues[TIDL_DIM_BATCH]*layer.inData[1].dimValues[TIDL_DIM_NUMCH];

      assert(filter_rows == layer.inData[1].dimValues[TIDL_DIM_HEIGHT]);
      assert(filter_cols == layer.inData[1].dimValues[TIDL_DIM_WIDTH]);

    }

    status = TIDL_tfCopyFloatInitializerTensor(onnGraph, i, filterIdx, layer.weights);
    if(status == -1)
    {
      diagsInfo.push_back("Cannot read initializer tensor : Only float and INT64 tensor is supported  -- file info - " + std::string(__FILE__) + " , " \
      + std::string(__func__) + " , " + std::to_string(__LINE__));
      return -1;
    }

    if(md.constTensorIndices.size() == 2) //both weight and bias present
    {
      int32_t biasIdx = md.constTensorIndices[1]; //2nd constant tensor should be bias
      innerProductParams.isBias = 1;
      if(layer.numInBufs == 2)
      {
        innerProductParams.constIdx = 1;
      }
      else
      {
        innerProductParams.constIdx = -1;
      }
      TensorProto biasTensor = TIDL_getInitializerTensor(onnGraph, onnGraph.node(i).input(biasIdx), i);

      status = TIDL_tfCopyFloatInitializerTensor(onnGraph, i, biasIdx, layer.bias);
      if(status == -1)
      {
        diagsInfo.push_back("Cannot read initializer tensor : Only float and INT64 tensor is supported  -- file info - " + std::string(__FILE__) + " , " \
        + std::string(__func__) + " , " + std::to_string(__LINE__));
        return -1;
      }
    }
    else  //bias tensor not specified, values should default to 0
    {
      innerProductParams.isBias = 0;
      innerProductParams.constIdx = 1;
      layer.bias.ptr = calloc(innerProductParams.numOutCols, sizeof(float));
      layer.bias.bufSize = innerProductParams.numOutCols;
    }
  }
  else
  {
      /*Number of constant tensro is zero*/
      innerProductParams.numInRows  = -1;
      innerProductParams.numInCols  = -1;
      innerProductParams.numOutCols = -1;
      innerProductParams.numBatches = -1;
      innerProductParams.isBias     = 0;
      innerProductParams.constIdx = -1;
      layer.weights.ptr = NULL;
      layer.weights.bufSize = 0;
      layer.bias.ptr= NULL;
      layer.bias.bufSize = NULL;
  }

  return 0;
}

int32_t TIDL_onnxGemmParams(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure,
  int32_t              i,
  int32_t              layerIndex,
  int32_t              *dataIndex,
  GraphProto&            onnGraph)
{
  int32_t status;
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].dataId = (*dataIndex)++;
  status = TIDL_onnxGemmBaseParams(onnGraph, i, TIDLPCLayers);
  return status;
}

int32_t TIDL_onnxMapSoftmaxBaseParams(GraphProto&   onnGraph, int32_t i, sTIDL_LayerPC_t &layer)
{
  layer.layerType = TIDL_SoftMaxLayer;
  int32_t axis = -1;
  TIDL_onnxSaveAllowlistingMetaData(onnGraph, i, layer);
  sTIDL_allowlistingMetaData md = layer.allowlistingMetaData;
  int32_t numDim = md.varTensorsDims[0].size();
  if(numDim > TIDL_DIM_MAX)
  {
    printf("Unsupported number of dims\n");
    return -1;
  }
  NodeProto node = onnGraph.node(i);
  TIDL_onnxGetIntAttr(node, "axis", &axis, 0);
  /*Translate to TIDL Axis:*/
  if(axis >= 0)
  {
    /*+ve Axis*/
    axis += (TIDL_DIM_MAX - numDim);
  }
  else
  {
    /*-ve Axis*/
    axis = (TIDL_DIM_MAX + axis);
  }
  layer.layerParams.softMaxParams.axis = axis;
  /* Only support axis are last 2 dimension */
  if ((axis != TIDL_DIM_HEIGHT) && (axis != TIDL_DIM_WIDTH))
  {
    std::string diagMsg = "ALLOWLISTING : SoftMax layer : TIDL_RT only supports axis width or height  --  file info - " + std::string(__FILE__) + " , " \
        + std::string(__func__) + " , " + std::to_string(__LINE__);
    diagsInfo.push_back(diagMsg);
  }

  /*Prevent transpose of output*/
  layer.layerParams.softMaxParams.outTranspose = 0;
  return 0;
}

int32_t TIDL_onnxSoftmaxParams(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure,
  int32_t              i,
  int32_t              layerIndex,
  int32_t              *dataIndex,
  GraphProto&            onnGraph)
{
  int32_t status;
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].dataId = (*dataIndex)++;
  status = TIDL_onnxMapSoftmaxBaseParams(onnGraph, i, TIDLPCLayers);
  return status;
}

void TIDL_tfBNToScaleBias(
  float    * scale,
  float    * bias,
  uint32_t  numCh,
  float * mean,
  float * var,
  float * gamma,
  float * beta,
  float eps
  );

int32_t TIDL_onnxMapBNBaseParams(GraphProto&   onnGraph, int32_t i, sTIDL_LayerPC_t &layer)
{
  sBuffer_t gamma;
  sBuffer_t beta;
  sBuffer_t mean;
  sBuffer_t variance;
  float epsilon = 1e-05;
  int32_t dataSize;

  layer.layerType = TIDL_BatchNormLayer;
  NodeProto node = onnGraph.node(i);

  TIDL_onnxSaveAllowlistingMetaData(onnGraph, i, layer);

  TIDL_tfCopyFloatInitializerTensor(onnGraph, i, 1, gamma);
  TIDL_tfCopyFloatInitializerTensor(onnGraph, i, 2, beta);
  TIDL_tfCopyFloatInitializerTensor(onnGraph, i, 3, mean);
  TIDL_tfCopyFloatInitializerTensor(onnGraph, i, 4, variance);
  TIDL_onnxGetFloatAttr(node, "epsilon", &epsilon, 0);

  /* ONNX Opset Version 18 introduces a new attribute called training mode in the Batch Normalization Operator.*/
  int32_t training_mode = 0;
  TIDL_onnxGetIntAttr(node, "training_mode", &training_mode, 0);
  /* TIDL_RT doesn't support training mode in Batch Normalization. */
  if(training_mode)
  {
    std::string diagMsg = "ALLOWLISTING : BN layer : Training Mode not supported by TIDL_RT  --  file info - " + std::string(__FILE__) + " , " \
        + std::string(__func__) + " , " + std::to_string(__LINE__);
    diagsInfo.push_back(diagMsg);
    return -1;
  }

  dataSize = gamma.bufSize;
  layer.weights.ptr = my_malloc(dataSize*sizeof(float));
  layer.weights.bufSize = dataSize;
  layer.bias.ptr = my_malloc(dataSize*sizeof(float));
  layer.bias.bufSize = dataSize;


  TIDL_tfBNToScaleBias((float *)layer.weights.ptr,
    (float *)layer.bias.ptr, dataSize,
    (float *)mean.ptr, (float *)variance.ptr, (float *)gamma.ptr, (float *)beta.ptr, epsilon
    );
  my_free(mean.ptr);
  my_free(variance.ptr);
  my_free(gamma.ptr);
  my_free(beta.ptr);

  return 0;
}

int32_t TIDL_onnxMapBNParams(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure,
  int32_t              i,
  int32_t              layerIndex,
  int32_t              *dataIndex,
  GraphProto&            onnGraph)
{
  int32_t status;
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].dataId = (*dataIndex)++;
  status = TIDL_onnxMapBNBaseParams(onnGraph, i, TIDLPCLayers);
  return status;
}

int32_t TIDL_onnxMapMaxPoolBaseParams(GraphProto&   onnGraph, int32_t i, sTIDL_LayerPC_t &layer)
{
  int32_t status = 0;
  TIDL_onnxSaveAllowlistingMetaData(onnGraph, i, layer);
  status = TIDL_checkPoolingTensorProperties(layer);
  if(status == -1) return -1;

  int32_t padType;
  int32_t idx1, idx2;
  int32_t pads[4] = {0};
  sTIDL_PoolingParams_t &poolParams = layer.layerParams.poolParams;
  layer.layerType = TIDL_PoolingLayer;
  poolParams.poolingType = TIDL_MaxPooling;
  poolParams.strideW = 1;
  poolParams.strideH = 1;
  poolParams.padW = 0;
  poolParams.padH = 0;
  poolParams.padT = 0;
  poolParams.padB = 0;
  poolParams.padL = 0;
  poolParams.padR = 0;

  poolParams.useCeil = 0;
  NodeProto node = onnGraph.node(i);

  std::vector<int32_t> attr;
  attr.push_back(TIDL_onnxGetIntAttr(node, "kernel_shape", &poolParams.kernelW, 1));
  attr.push_back(TIDL_onnxGetIntAttr(node, "kernel_shape", &poolParams.kernelH, 0));

  if(TIDL_onnxGetAttrIdx(node, "strides") != -1)
  {
    attr.push_back(TIDL_onnxGetIntAttr(node, "strides", &poolParams.strideW, 1));
    attr.push_back(TIDL_onnxGetIntAttr(node, "strides", &poolParams.strideH, 0));
  }

  char autoPad[50];
  status = TIDL_onnxGetStringAttr(node, "auto_pad", autoPad, 0);
  if((status == -1) || (strcmp(autoPad, "NOTSET") == 0))
  {
    if(TIDL_onnxGetAttrIdx(node, "pads") != -1)
    {
      attr.push_back(TIDL_onnxGetIntAttr(node, "pads",       &pads[0], 0));
      attr.push_back(TIDL_onnxGetIntAttr(node, "pads",       &pads[1], 1));
      attr.push_back(TIDL_onnxGetIntAttr(node, "pads",       &pads[2], 2));
      attr.push_back(TIDL_onnxGetIntAttr(node, "pads",       &pads[3], 3));
      if((pads[0] < pads[2]) && (pads[1] < pads[3]) && (poolParams.strideW != 1) && (poolParams.strideH != 1)) //padL < padR && padT < padB
      {
        /* This condition will occur when stride = 2 and size of input tensor is even
          strideOffsetMethod is not handled in pooling layer, so take care of this in TIDL_tfOutReshapePoolingLayer */
        layer.strideOffsetMethod = TIDL_StrideOffsetCenter;
      }
      poolParams.padT = pads[0];
      poolParams.padB = pads[1];
      poolParams.padL = pads[2];
      poolParams.padR = pads[3];
      pads[2] = max(pads[0], pads[2]);
      pads[3] = max(pads[1], pads[3]);

    }
  }
  else if ((strcmp(autoPad, "SAME_UPPER") == 0) || (strcmp(autoPad, "SAME_LOWER") == 0))
  {
    if(poolParams.strideW > 1)
    {
      pads[3] = (poolParams.kernelW - 1) / 2;
    }
    else
    {
      pads[3] = poolParams.kernelW / 2;
    }
    if(poolParams.strideH > 1)
    {
      pads[2] = (poolParams.kernelH - 1) / 2;
    }
    else
    {
      pads[2] = poolParams.kernelH / 2;
    }

    if(strcmp(autoPad, "SAME_UPPER") == 0)
    {
      layer.strideOffsetMethod = TIDL_StrideOffsetCenter;
    }
    else  //SAME_LOWER
    {
      layer.strideOffsetMethod = TIDL_StrideOffsetTopLeft;
    }
  }
  else if(strcmp(autoPad, "VALID") == 0)
  {
    pads[3] = 0;
    pads[2] = 0;
  }
  else
  {
    diagsInfo.push_back("ALLOWLISTING : MAX POOL : Unsupported auto_pad attribute  -- file info - " + std::string(__FILE__) + " , " \
        + std::string(__func__) + " , " + std::to_string(__LINE__));
    return -1;
  }

  if(std::find(attr.begin(), attr.end(), -1) != attr.end())
  {
    diagsInfo.push_back("ALLOWLISTING : MAX POOL : Missing attribute  -- file info - " + std::string(__FILE__) + " , " \
        + std::string(__func__) + " , " + std::to_string(__LINE__));
    return -1;
  }

  poolParams.padW = pads[3];
  poolParams.padH = pads[2];

  return 0;
}

int32_t TIDL_onnxMapMaxPoolParams(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure,
  int32_t              i,
  int32_t              layerIndex,
  int32_t              *dataIndex,
  GraphProto&            onnGraph)
{
  int32_t status;
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].dataId = (*dataIndex)++;
  status = TIDL_onnxMapMaxPoolBaseParams(onnGraph, i, TIDLPCLayers);
  return status;
}

int32_t TIDL_onnxMapArgmaxBaseParams(GraphProto&   onnGraph, int32_t i, sTIDL_LayerPC_t &layer)
{
  int32_t status;
  int32_t axis = 0, keepdim = 1;
  sTIDL_ArgMaxParams_t &argMaxParams = layer.layerParams.argMaxParams;
  layer.layerType = TIDL_ArgMaxLayer;

  NodeProto node = onnGraph.node(i);
  TIDL_onnxGetIntAttr(node, "axis", &axis, 0);
  TIDL_onnxGetIntAttr(node, "keepdims", &keepdim, 0);
  if (keepdim != 1)
  {
    std::string diagMsg = "ALLOWLISTING : Argmax Layer: Warning : only keepdims = 1 supported for Argmax Layer - forcing it to be 1  --  file info - " + std::string(__FILE__) + " , " + std::string(__func__) + " , " + std::to_string(__LINE__);
    diagsInfo.push_back(diagMsg);
  }
  if (axis != 1)
  {
    std::string diagMsg = "ALLOWLISTING : Argmax Layer: Only axis = 1 supported for Argmax Layer  --  file info - " + std::string(__FILE__) + " , " + std::string(__func__) + " , " + std::to_string(__LINE__);
    diagsInfo.push_back(diagMsg);
    return -1;
  }
  /*New attribute called select last index introduced. */
  int32_t select_last_index = 0;
  TIDL_onnxGetIntAttr(node, "select_last_index", &select_last_index, 0);
  /* If select_last_index is set true then exit. */
  if(select_last_index)
  {
    std::string diagMsg = "ALLOWLISTING : Argmax Layer: Selecting Last Index not supported by TIDL_RT  --  file info - " + std::string(__FILE__) + " , " + std::string(__func__) + " , " + std::to_string(__LINE__);
    diagsInfo.push_back(diagMsg);
    return(-1);
  }
  return 0;
}
int32_t TIDL_onnxMapArgmaxParams(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure,
  int32_t              i,
  int32_t              layerIndex,
  int32_t              *dataIndex,
  GraphProto&            onnGraph)
{
  int32_t status;
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].dataId = (*dataIndex)++;
  status = TIDL_onnxMapArgmaxBaseParams(onnGraph, i, TIDLPCLayers);
  return status;
}

int32_t TIDL_onnxMapReluBaseParams(GraphProto&   onnGraph, int32_t i, sTIDL_LayerPC_t &layer)
{
  layer.layerType = TIDL_ReLULayer;
  layer.actParams.actType = TIDL_RelU;
  return 0;
}

int32_t TIDL_onnxMapReluParams(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure,
  int32_t              i,
  int32_t              layerIndex,
  int32_t              *dataIndex,
  GraphProto&            onnGraph)
{
  int32_t status;
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].dataId = (*dataIndex)++;
  status = TIDL_onnxMapReluBaseParams(onnGraph, i, TIDLPCLayers);
  return status;
}

int32_t TIDL_onnxMapClipBaseParams(GraphProto&   onnGraph, int32_t i, sTIDL_LayerPC_t &layer)
{
  layer.layerType = TIDL_ClipLayer;
  layer.actParams.actType = TIDL_Clip;
  NodeProto node = onnGraph.node(i);
  int32_t minStatus = -1, maxStatus = -1;
  int32_t numInputs = node.input_size();
  sBuffer_t buf;

  /* Try to fetch min and max from attributes */
  minStatus = TIDL_onnxGetFloatAttr(node, "min", &layer.actParams.clipMin, 0);
  maxStatus = TIDL_onnxGetFloatAttr(node, "max", &layer.actParams.clipMax, 0);

  /* If min not found */
  if (minStatus == -1)
  {
    /* Try to fetch min from 2nd input*/
    if (numInputs > 1)
    {
      TIDL_tfCopyFloatConst(onnGraph, i, 1, buf);
      float *ptr = (float *)buf.ptr;
      layer.actParams.clipMin = *ptr;
      my_free(ptr);
    }
    /* Set default value of min */
    else
    {
      layer.actParams.clipMin = (float32_tidl)std::numeric_limits<float>::lowest();
    }
  }
  /* If max not found */
  if (maxStatus == -1)
  {
    /* Try to fetch max from 3rd input*/
    if ( numInputs > 2 )
    {
      TIDL_tfCopyFloatConst(onnGraph, i, 2, buf);
      float *ptr = (float *)buf.ptr;
      layer.actParams.clipMax = *ptr;
      my_free(ptr);
    }
    /* Set default value of max */
    else
    {
      layer.actParams.clipMax = (float32_tidl)std::numeric_limits<float>::max();
    }
  }

  return 0;
}

int32_t TIDL_onnxMapClipParams(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure,
  int32_t              i,
  int32_t              layerIndex,
  int32_t              *dataIndex,
  GraphProto&            onnGraph)
{
  int32_t status;
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].dataId = (*dataIndex)++;
  status = TIDL_onnxMapClipBaseParams(onnGraph, i, TIDLPCLayers);
  return status;
}

int32_t TIDL_onnxMapPReluBaseParams(GraphProto&   onnGraph, int32_t i, sTIDL_LayerPC_t &layer)
{
  int32_t status;
  status = TIDL_onnxMapReluBaseParams(onnGraph, i, layer);
  layer.actParams.actType = TIDL_PRelU;
  return 0;
}

int32_t TIDL_onnxMapPReluParams(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure,
  int32_t              i,
  int32_t              layerIndex,
  int32_t              *dataIndex,
  GraphProto&            onnGraph)
{
  int32_t status;
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].dataId = (*dataIndex)++;
  status = TIDL_onnxMapPReluBaseParams(onnGraph, i, TIDLPCLayers);
  return status;
}

int32_t TIDL_onnxMapSigmoidBaseParams(GraphProto&   onnGraph, int32_t i, sTIDL_LayerPC_t &layer)
{
  layer.layerType = TIDL_SigmoidLayer;
  layer.actParams.actType = TIDL_Sigmoid;
  layer.outData[0].elementType = tidl_getElementType(0);
  return 0;
}

int32_t TIDL_onnxMapSigmoidParams(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure,
  int32_t              i,
  int32_t              layerIndex,
  int32_t              *dataIndex,
  GraphProto&            onnGraph)
{
  int32_t status;
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].dataId = (*dataIndex)++;
  status = TIDL_onnxMapSigmoidBaseParams(onnGraph, i, TIDLPCLayers);
  return status;
}

int32_t TIDL_onnxMapTanhBaseParams(GraphProto&   onnGraph, int32_t i, sTIDL_LayerPC_t &layer)
{
  layer.layerType = TIDL_TanhLayer;
  layer.actParams.actType = TIDL_Tanh;
  layer.outData[0].elementType = tidl_getElementType(1);
  return 0;
}

int32_t TIDL_onnxMapTanhParams(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure,
  int32_t              i,
  int32_t              layerIndex,
  int32_t              *dataIndex,
  GraphProto&            onnGraph)
{
  int32_t status = 0;
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].dataId = (*dataIndex)++;
  status = TIDL_onnxMapTanhBaseParams(onnGraph, i, TIDLPCLayers);
  return status;
}

int32_t TIDL_onnxMapHardSigmoidBaseParams(GraphProto&   onnGraph, int32_t i, sTIDL_LayerPC_t &layer)
{
  layer.layerType = TIDL_HardSigmoidLayer;
  layer.actParams.actType = TIDL_HardSigmoid;
  layer.outData[0].elementType = tidl_getElementType(0);
  sTIDL_HardSigmoidParams_t &hardSigmoidParams      = layer.layerPCParams.hardSigmoidParams;
  NodeProto node = onnGraph.node(i);
  TIDL_onnxGetFloatAttr(node, "alpha",   &hardSigmoidParams.alpha, 0);
  hardSigmoidParams.beta = 0.5;
  return 0;
}

int32_t TIDL_onnxMapHardSigmoidParams(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure,
  int32_t              i,
  int32_t              layerIndex,
  int32_t              *dataIndex,
  GraphProto&            onnGraph)
{
  int32_t status = 0;
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].dataId = (*dataIndex)++;
  status = TIDL_onnxMapHardSigmoidBaseParams(onnGraph, i, TIDLPCLayers);
  return status;
}

int32_t TIDL_onnxMapELUBaseParams(GraphProto&   onnGraph, int32_t i, sTIDL_LayerPC_t &layer)
{
  layer.layerType = TIDL_ELULayer;
  layer.actParams.actType = TIDL_ELU;
  layer.outData[0].elementType = tidl_getElementType(1);
  sTIDL_EluParams_t &eluParams      = layer.layerPCParams.eluParams;
  NodeProto node = onnGraph.node(i);
  TIDL_onnxGetFloatAttr(node, "alpha",   &eluParams.alpha, 0);
  return 0;
}

int32_t TIDL_onnxMapELUParams(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure,
  int32_t              i,
  int32_t              layerIndex,
  int32_t              *dataIndex,
  GraphProto&            onnGraph)
{
  int32_t status = 0;
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].dataId = (*dataIndex)++;
  status = TIDL_onnxMapELUBaseParams(onnGraph, i, TIDLPCLayers);
  return status;
}

int32_t TIDL_onnxMapLeakyReluBaseParams(GraphProto&   onnGraph, int32_t i, sTIDL_LayerPC_t &layer)
{
  layer.layerType = TIDL_LeakyReluLayer;
  layer.outData[0].elementType = tidl_getElementType(1);
  NodeProto node = onnGraph.node(i);
  TIDL_onnxGetFloatAttr(node, "alpha", &layer.layerPCParams.leakyReluParams.alpha, 0);
  return 0;
}
int32_t TIDL_onnxMapLeakyReluParams(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure,
  int32_t              i,
  int32_t              layerIndex,
  int32_t              *dataIndex,
  GraphProto&            onnGraph)
{
  int32_t status;
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].dataId = (*dataIndex)++;
  status = TIDL_onnxMapLeakyReluBaseParams(onnGraph, i, TIDLPCLayers);
  return status;
}

int32_t TIDL_onnxMapFlattenBaseParams(GraphProto&   onnGraph, int32_t i, sTIDL_LayerPC_t &layer)
{
  layer.layerType = TIDL_FlattenLayer;
  return 0;
}
int32_t TIDL_onnxMapFlattenParams(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure,
  int32_t              i,
  int32_t              layerIndex,
  int32_t              *dataIndex,
  GraphProto&            onnGraph)
{
  int32_t status;
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].dataId = (*dataIndex)++;
  status = TIDL_onnxMapFlattenBaseParams(onnGraph, i, TIDLPCLayers);
  return status;
}

int32_t TIDL_onnxMapPadBaseParams(GraphProto&   onnGraph, int32_t i, sTIDL_LayerPC_t &layer)
{
  int32_t idx, j;
  layer.layerType = TIDL_PadLayer;

  NodeProto node = onnGraph.node(i);
  int32_t padT, padB, padL, padR;
  int32_t tot_axis, atrIdx, axes_status, numDims;
  char padMode[50];
  float32_tidl constant_value;
  sBuffer_t buf, axes;
  int32_t* padValStore;

  numDims = node.input_size();

  TIDL_onnxGetStringAttr(node, "mode", padMode, 0);

  if ((strcmp(padMode, "constant") != 0))
  {
    std::string diagMsg = "ALLOWLISTING : Pad layer : only constant pad mode is supported  --  file info - " + std::string(__FILE__) + " , " \
    + std::string(__func__) + " , " + std::to_string(__LINE__);
    diagsInfo.push_back(diagMsg);
    return -1;
  }

  /* Pad v1 & v2 can have value in Attributes */
  atrIdx = TIDL_onnxGetFloatAttr(node, "value", &constant_value, 0);
  if(atrIdx != -1)
  {
    layer.layerParams.padLayerParams.padConstValue = (int32_t)constant_value;
  }
  else
  {
    /* In other cases pad value can be in 3rd input */
    atrIdx = TIDL_tfCopyFloatConst(onnGraph, i, 2, buf);
    if (atrIdx != -1)
    {
      float32_tidl *padPtr = (float32_tidl *)buf.ptr;
      layer.layerParams.padLayerParams.padConstValue = (int32_t)padPtr[0];
      free(buf.ptr);
      buf.bufSize = 0;
      buf.ptr = NULL;
    }
    /* If still not found, set default value as 0 */
    else
    {
      layer.layerParams.padLayerParams.padConstValue = 0;
    }
  }


  /* Axes can be input in v18 */
  axes_status = TIDL_tfCopyFloatInitializerTensor(onnGraph, i, 3, axes);
  int64_t *axesPtr = (int64_t *)axes.ptr; /* The axesPtr points to the pointer in the axes buffer.*/

  /* If the optional input axes is populated.*/
  if(axes_status != -1)
  {
    /** Iterating through the axes buffer.
     * Ignore repetition as described as undefined behavior in onnx
    */
    for(int j = 0; j < axes.bufSize; j++)
    {
      /* Adjust to TIDL dimensions */
      if(axesPtr[j] >= 0)
      {
        /*+ve Axis*/
        axesPtr[j] += (TIDL_DIM_MAX - numDims);
      }
      else
      {
        /*-ve Axis*/
        axesPtr[j] += TIDL_DIM_MAX;
      }

      /* If axes are not height or width, do not support */
      if ((axesPtr[j] != TIDL_DIM_HEIGHT) && (axesPtr[j] != TIDL_DIM_WIDTH))
      {
        std::string diagMsg = "ALLOWLISTING : Pad layer : Padding only supported for Width/Height Axes   --  file info - " + std::string(__FILE__) + " , " + std::string(__func__) + " , " + std::to_string(__LINE__);
        diagsInfo.push_back(diagMsg);
        return -1;
      }
    }
  }

  /**
   * Pad can be in attribute
   * in v1 named as padding
   * in v2 named as pads
  */
  atrIdx = TIDL_onnxGetAttrIdx(node, "pads");
  if (atrIdx == -1)
  {
    atrIdx = TIDL_onnxGetAttrIdx(node, "paddings");
  }
  /* If found in attribute */
  if (atrIdx != -1)
  {
    tot_axis = node.attribute(atrIdx).ints_size() >> 1;
    padValStore = (int32_t *)my_malloc(tot_axis * 2 * sizeof(int32_t));

    for (j = 0; j < tot_axis*2; j++)
    {
      TIDL_onnxGetIntAttr(node, "pads", &padValStore[j], j);
    }

    if(tot_axis > 2) /* more than width and height */
    {
      for (j = 0; j < (tot_axis-2); j++)
      {
        /* Cannot be non-zero in these dimensions */
        if(( padValStore[j] != 0 ) || ( padValStore[tot_axis + j] != 0 ))
        {
          std::string diagMsg = "ALLOWLISTING : Pad layer : Padding is only supported in Width/height axis  --  file info - " + std::string(__FILE__) + " , " \
          + std::string(__func__) + " , " + std::to_string(__LINE__);
          diagsInfo.push_back(diagMsg);
          return -1;
        }
      }
    }

    padT = padValStore[tot_axis   - 2];
    padB = padValStore[tot_axis*2 - 2];
    padL = padValStore[tot_axis   - 1];
    padR = padValStore[tot_axis*2 - 1];
    my_free(padValStore);
  }
  /* Pad value can be in 2nd input */
  else
  {
    TIDL_tfCopyFloatConst(onnGraph, i, 1, buf);
    int64_t *padPtr = (int64_t *)buf.ptr;
    tot_axis = buf.bufSize >> 1;

    /* Axes is present */
    if(axes_status != -1)
    {
      if (tot_axis != axes.bufSize) /*The size of the axes tensor must be half the pad input size if axes exists*/
      {
        std::string diagMsg = "ALLOWLISTING : Pad layer : Invalid Input for axes or pads - dimensions not matching   --  file info - " + std::string(__FILE__) + " , " + std::string(__func__) + " , " + std::to_string(__LINE__);
        diagsInfo.push_back(diagMsg);
        return -1;
      }
      else
      {
        for (int j = 0; j < tot_axis; j++)
        {
          if (axesPtr[j] == TIDL_DIM_WIDTH)
          {
            padL = padPtr[j];
            padR = padPtr[tot_axis+j];
          }
          else if (axesPtr[j] == TIDL_DIM_HEIGHT)
          {
            padT = padPtr[j];
            padB = padPtr[tot_axis+j];
          }
          else
          {
            std::string diagMsg = "ALLOWLISTING : Pad layer : Padding only supported for Width/Height Axes   --  file info - " + std::string(__FILE__) + " , " + std::string(__func__) + " , " + std::to_string(__LINE__);
            diagsInfo.push_back(diagMsg);
            return -1;
          }
        }
      }
    }
    /* Axes is not given */
    else
    {
      if(tot_axis > 2) /*more than width and height*/
      {
        for (j = 0; j < (tot_axis-2); j++)
        {
          /* Cannot be non-zero in these dimensions */
          if(( padPtr[j] != 0 ) || ( padPtr[tot_axis + j] != 0 ))
          {
            std::string diagMsg = "ALLOWLISTING : Pad layer : Padding is only supported in Width/height axis  --  file info - " + std::string(__FILE__) + " , " \
            + std::string(__func__) + " , " + std::to_string(__LINE__);
            diagsInfo.push_back(diagMsg);
            return -1;
          }
        }
      }

      padT = padPtr[tot_axis   - 2];
      padB = padPtr[tot_axis*2 - 2];
      padL = padPtr[tot_axis   - 1];
      padR = padPtr[tot_axis*2 - 1];
    }
    my_free(buf.ptr);
  }


  /* if axis was being used, free axis ptr */
  if (axes_status != -1)
  {
    my_free(axes.ptr);
    axes.bufSize = 0;
    axes.ptr = NULL;
  }

  layer.layerParams.padLayerParams.padT = padT;
  layer.layerParams.padLayerParams.padB = padB;
  layer.layerParams.padLayerParams.padL = padL;
  layer.layerParams.padLayerParams.padR = padR;
  return 0;
}
int32_t TIDL_onnxMapPadParams(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure,
  int32_t              i,
  int32_t              layerIndex,
  int32_t              *dataIndex,
  GraphProto&            onnGraph)
{
  int32_t status;
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].dataId = (*dataIndex)++;
  status = TIDL_onnxMapPadBaseParams(onnGraph, i, TIDLPCLayers);
  return status;
}

int32_t TIDL_onnxMapAddBaseParams(GraphProto&   onnGraph, int32_t i, sTIDL_LayerPC_t &layer)
{
  int32_t status = 0;

  TIDL_onnxSaveAllowlistingMetaData(onnGraph, i, layer);
  status = TIDL_checkAddMulSubDivTensorProperties(layer,BASE_OP_ADD);
  if(status == -1) return -1;

  sTIDL_allowlistingMetaData md = layer.allowlistingMetaData;
  int32_t is1DConstTensor = checkDimConstTensor (layer);
  if(md.numConstInputs > 0) //addition with constant tensor
  {
    int constTensorIdx = md.constTensorIndices[0];
    if (is1DConstTensor == 1)
    {
      TensorProto constTensor = TIDL_getInitializerTensor(onnGraph, onnGraph.node(i).input(constTensorIdx), i);

      int32_t varTensorIdx = md.varTensorIndices[0];

      if(constTensor.dims_size() == 0) //tensor dim size = 0 --- Add constant to layer
      {
          layer.layerType = TIDL_BatchNormLayer;
          status = TIDL_tfCopyFloatInitializerTensor(onnGraph, i, constTensorIdx, layer.bias);
          if(status == -1)
          {
            diagsInfo.push_back("Cannot read initializer tensor : Only float and INT64 tensor is supported  -- file info - " + std::string(__FILE__) + " , " \
            + std::string(__func__) + " , " + std::to_string(__LINE__));
            return -1;
          }
          int32_t biasSize = md.varTensorsDims[0][md.varTensorsDims[0].size()-3]; //broadcast the constant along number of channels

          int32_t nodeIdx = TIDL_onnxGetNodeIdx(onnGraph, onnGraph.node(i).input(constTensorIdx).c_str());
          int32_t isInputDQ = 0;
          if (nodeIdx != -1)
          {
            isInputDQ = strcmp(onnGraph.node(nodeIdx).op_type().c_str(), "DequantizeLinear") == 0;
          }
          if(constTensor.data_type() == TensorProto_DataType_FLOAT ||
              isInputDQ)
          {
            TIDL_onnxBroadcastFloatTensor(layer.bias, biasSize);
          }
          else
          {
            std::string diagMsg = "ALLOWLISTING : ADD layer : Supported only if data type of constant is float  --  file info - " + std::string(__FILE__) + " , " \
            + std::string(__func__) + " , " + std::to_string(__LINE__);
            diagsInfo.push_back(diagMsg);
            return -1;
          }
      }
      else //addition with constant vector
      {
        layer.layerType = TIDL_BatchNormLayer;
        status = TIDL_tfCopyFloatInitializerTensor(onnGraph, i, constTensorIdx, layer.bias);
        if(status == -1)
        {
          diagsInfo.push_back("Cannot read initializer tensor : Only float and INT64 tensor is supported  -- file info - " + std::string(__FILE__) + " , " \
            + std::string(__func__) + " , " + std::to_string(__LINE__));
          return -1;
        }
      }

      layer.weights.ptr             = my_malloc(layer.bias.bufSize*sizeof(float));
      layer.weights.bufSize         = layer.bias.bufSize;
      float *ptr = (float *)layer.weights.ptr;
      for (int lc = 0; lc < layer.weights.bufSize; lc++)
        ptr[lc] = 1.0;
      return 0;
    }
    else
    {
      layer.layerType = TIDL_EltWiseLayer;
      layer.layerParams.eltWiseParams.eltWiseType = TIDL_EltWiseSum;
      layer.numInBufs = md.numInputs;
      status = TIDL_tfCopyFloatInitializerTensor(onnGraph, i, constTensorIdx, layer.weights);
      if(status == -1)
      {
        diagsInfo.push_back("Cannot read initializer tensor : Only float and INT64 tensor is supported  -- file info - " + std::string(__FILE__) + " , " \
          + std::string(__func__) + " , " + std::to_string(__LINE__));
        return -1;
      }
    }
  }
  else //both inputs variable
  {
    layer.layerType = TIDL_EltWiseLayer;
    layer.layerParams.eltWiseParams.eltWiseType = TIDL_EltWiseSum;
    layer.numInBufs = md.numInputs;
  }
  return 0;
}

int32_t TIDL_onnxMapAddParams(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure,
  int32_t              i,
  int32_t              layerIndex,
  int32_t              *dataIndex,
  GraphProto&            onnGraph)
{
  int32_t status;
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].dataId = (*dataIndex)++;
  status = TIDL_onnxMapAddBaseParams(onnGraph, i, TIDLPCLayers);

  return status;
}

int32_t TIDL_onnxMapSubBaseParams(GraphProto&   onnGraph, int32_t i, sTIDL_LayerPC_t &layer)
{
  int32_t status = 0;
  if(gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_ONNX && gParams.debugTraceLevel > 2)
  {
    printf("*** WARNING : Subtract with constant tensor requires input dimensions of sub layer to be present as part of the network.\
      If present, this warning can be ignored. If not, please use open source runtimes offering to run this model or run shape inference on this model before executing import  *** \n");
  }
  TIDL_onnxSaveAllowlistingMetaData(onnGraph, i, layer);
  status = TIDL_checkAddMulSubDivTensorProperties(layer, BASE_OP_SUB);
  if(status == -1) return -1;
  sTIDL_allowlistingMetaData md = layer.allowlistingMetaData;

  if(md.numConstInputs > 0) //subtraction with constant tensor
  {
    int constTensorIdx = md.constTensorIndices[0];
    TensorProto constTensor = TIDL_getInitializerTensor(onnGraph, onnGraph.node(i).input(constTensorIdx), i);
    int32_t varTensorIdx = md.varTensorIndices[0];

    if(constTensor.dims_size() == 0) //tensor dim size = 0 --- Sub constant to layer
    {
        layer.layerType = TIDL_BatchNormLayer;
        status = TIDL_tfCopyFloatInitializerTensor(onnGraph, i, constTensorIdx, layer.bias);
        if(status == -1)
        {
          diagsInfo.push_back("Cannot read initializer tensor : Only float and INT64 tensor is supported  -- file info - " + std::string(__FILE__) + " , " \
          + std::string(__func__) + " , " + std::to_string(__LINE__));
          return -1;
        }
        int32_t biasSize = md.varTensorsDims[0][md.varTensorsDims[0].size()-3];   //broadcast the constant along number of channels

        int32_t nodeIdx = TIDL_onnxGetNodeIdx(onnGraph, onnGraph.node(i).input(constTensorIdx).c_str());
        int32_t isInputDQ = 0;
        if (nodeIdx != -1)
        {
          isInputDQ = strcmp(onnGraph.node(nodeIdx).op_type().c_str(), "DequantizeLinear") == 0;
        }

        if(constTensor.data_type() == TensorProto_DataType_FLOAT ||
            isInputDQ)
        {
          TIDL_onnxBroadcastFloatTensor(layer.bias, biasSize);
        }
        else
        {
          std::string diagMsg = "ALLOWLISTING : SUB layer : Supported only if data type of constant is float  --  file info - " + std::string(__FILE__) + " , " \
          + std::string(__func__) + " , " + std::to_string(__LINE__);
          diagsInfo.push_back(diagMsg);
          return -1;
        }
    }
    else //Subtraction with constant vector
    {
      layer.layerType = TIDL_BatchNormLayer;
      status = TIDL_tfCopyFloatInitializerTensor(onnGraph, i, constTensorIdx, layer.bias);
      if(status == -1)
      {
        diagsInfo.push_back("Cannot read initializer tensor : Only float and INT64 tensor is supported  -- file info - " + std::string(__FILE__) + " , " \
          + std::string(__func__) + " , " + std::to_string(__LINE__));
        return -1;
      }
    }

    layer.weights.ptr             = my_malloc(layer.bias.bufSize*sizeof(float));
    layer.weights.bufSize         = layer.bias.bufSize;
    float *ptr = (float *)layer.weights.ptr;
    /* case: const - X */
    if(md.constTensorIndices[0]  < md.varTensorIndices[0] )
    {
      for (int lc = 0; lc < layer.weights.bufSize; lc++)
        ptr[lc] = -1.0;
    }
    /* case: X - const */
    else
    {
      /* scaling bias with: -1 */
      float *biasPtr = (float *)layer.bias.ptr;
      for (int lc = 0; lc < layer.bias.bufSize; lc++)
        biasPtr[lc] = -1.0*biasPtr[lc];

      for (int lc = 0; lc < layer.weights.bufSize; lc++)
        ptr[lc] = 1.0;
    }
    return 0;
  }
  else
  {
    layer.layerType = TIDL_SubLayer;
    layer.numInBufs = md.numInputs;
  }
  return 0;
}

int32_t TIDL_onnxMapSubParams(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure,
  int32_t              i,
  int32_t              layerIndex,
  int32_t              *dataIndex,
  GraphProto&            onnGraph)
{
  int32_t status;
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].dataId = (*dataIndex)++;
  status = TIDL_onnxMapSubBaseParams(onnGraph, i, TIDLPCLayers);
  return status;
}

int32_t TIDL_onnxMapDivParams(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure,
  int32_t              i,
  int32_t              layerIndex,
  int32_t              *dataIndex,
  GraphProto&            onnGraph)
{
  int32_t status;
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].dataId = (*dataIndex)++;
  status = TIDL_onnxMapDivBaseParams(onnGraph, i, TIDLPCLayers);
  return status;
}

int32_t TIDL_onnxMapMulBaseParams(GraphProto&   onnGraph, int32_t i, sTIDL_LayerPC_t &layer)
{
  int32_t status = 0;
  if(gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_ONNX  && gParams.debugTraceLevel > 2)
  {
    printf("*** WARNING : Mul with constant tensor requires input dimensions of mul layer to be present as part of the network.\
      If present, this warning can be ignored. If not, please use open source runtimes offering to run this model or run shape inference on this model before executing import  *** \n");
  }
  TIDL_onnxSaveAllowlistingMetaData(onnGraph, i, layer);
  status = TIDL_checkAddMulSubDivTensorProperties(layer, BASE_OP_MUL);
  if(status == -1) return -1;

  sTIDL_allowlistingMetaData md = layer.allowlistingMetaData;
  int32_t is1DConstTensor = checkDimConstTensor (layer);
  if(md.numConstInputs > 0) //multiply with constant tensor
  {
    int constTensorIdx = md.constTensorIndices[0];
    status = TIDL_tfCopyFloatInitializerTensor(onnGraph, i, constTensorIdx, layer.weights);
    if(status == -1)
    {
      diagsInfo.push_back("Cannot read initializer tensor : Only float and INT64 tensor is supported  -- file info - " + std::string(__FILE__) + " , " \
        + std::string(__func__) + " , " + std::to_string(__LINE__));
      return -1;
    }

    if (is1DConstTensor == 1)
    {
      layer.layerType = TIDL_BatchNormLayer;

      TensorProto constTensor = TIDL_getInitializerTensor(onnGraph, onnGraph.node(i).input(constTensorIdx), i);
      if(constTensor.dims_size() == 0) //tensor dim size = 0 --- Mul constant to layer
      {
        int32_t nodeIdx = TIDL_onnxGetNodeIdx(onnGraph, onnGraph.node(i).input(constTensorIdx).c_str());
        int32_t isInputDQ = 0;
        if (nodeIdx != -1)
        {
          isInputDQ = strcmp(onnGraph.node(nodeIdx).op_type().c_str(), "DequantizeLinear") == 0;
        }

        int32_t wtSize = 1;
        if (md.varTensorsDims[0].size() >= 3)
        {
          wtSize = md.varTensorsDims[0][md.varTensorsDims[0].size()-3]; //broadcast the constant along number of channels
        }
        else
        {
          /* We have only H and W dims, so constant addition to the available single channel, no need to broadcast 
          We preserve default wtSize = 1 set above */
        }
        
        if(constTensor.data_type() == TensorProto_DataType_FLOAT ||
            isInputDQ)
        {
          TIDL_onnxBroadcastFloatTensor(layer.weights, wtSize);
        }
        else
        {
          std::string diagMsg = "ALLOWLISTING : Mul layer : Supported only if data type of constant is float  --  file info - " + std::string(__FILE__) + " , " \
          + std::string(__func__) + " , " + std::to_string(__LINE__);
          diagsInfo.push_back(diagMsg);
          return -1;
        }
      }
      else //Multiply with constant vector
      {
        /* no broadcast required for vector */
      }

      /* Update bias for batchnorm */
      layer.bias.ptr = calloc(layer.weights.bufSize, sizeof(float));
      layer.bias.bufSize = layer.weights.bufSize;
    }
    else /* > 1D const tensor */
    {
      layer.layerType = TIDL_EltWiseLayer;
      layer.layerParams.eltWiseParams.eltWiseType = TIDL_EltWiseProduct;
      layer.numInBufs = md.numInputs;
    }
  }
  else /* both inputs variable */
  {
    layer.layerType = TIDL_EltWiseLayer;
    layer.layerParams.eltWiseParams.eltWiseType = TIDL_EltWiseProduct;
    layer.numInBufs = md.numInputs;
  }
  return 0;
}

int32_t TIDL_onnxMapMulParams(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure,
  int32_t              i,
  int32_t              layerIndex,
  int32_t              *dataIndex,
  GraphProto&            onnGraph)
{
  int32_t status;
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].dataId = (*dataIndex)++;
  status = TIDL_onnxMapMulBaseParams(onnGraph, i, TIDLPCLayers);
  return status;
}



int32_t TIDL_onnxMapDivBaseParams (GraphProto&  onnGraph, int32_t i, sTIDL_LayerPC_t &layer)
{
  int32_t status = 0;
  if(gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_ONNX && gParams.debugTraceLevel > 2)
  {
    printf("*** WARNING : Div by constant tensor requires input dimensions of div layer to be present as part of the network.\
      If present, this warning can be ignored. If not, please use open source runtimes offering to run this model or run shape inference on this model before executing import  *** \n");
  }

  TIDL_onnxSaveAllowlistingMetaData(onnGraph, i, layer);
  status = TIDL_checkAddMulSubDivTensorProperties(layer, BASE_OP_DIV);
  if (status == -1)
  {
    return -1;
  }

  sTIDL_allowlistingMetaData md = layer.allowlistingMetaData;
  int32_t is1DConstTensor = checkDimConstTensor (layer);
	if (md.numConstInputs > 0)	// div by constant tensor
	{
    int constTensorIdx = md.constTensorIndices[0];

    status = TIDL_tfCopyFloatInitializerTensor(onnGraph, i, constTensorIdx, layer.weights);
    if(status == -1)
    {
      std::string diagMsg = "Cannot read initializer tensor : Only float and INT64 tensor is supported  -- file info - "\
                        + std::string(__FILE__) + " , " \
                        + std::string(__func__) + " , " + std::to_string(__LINE__);
      diagsInfo.push_back(diagMsg);
      return -1;
    }
    
    if (is1DConstTensor == 1) /* 1D constant - either scalar value or 1D vector */
    {
      layer.layerType = TIDL_BatchNormLayer;

      /**
       * TIDL doesnot support div support as is, 
       * Change the div to mul by adjusting the constant
       * VarTensor/Constant -> VarTensor * (1/Constant)
      */
      float* weights_ptr = (float*)layer.weights.ptr;
      for(int i = 0; i < layer.weights.bufSize; i++)
      {
        float val = weights_ptr[i];
        val = 1/val;  /* ONNX checks for division by 0 */
        weights_ptr[i] = val;
      }
      
      TensorProto constTensor = TIDL_getInitializerTensor(onnGraph, onnGraph.node(i).input(constTensorIdx), i);
      if (constTensor.dims_size() == 0) /* scalar constant */
      {
        int32_t nodeIdx = TIDL_onnxGetNodeIdx(onnGraph, onnGraph.node(i).input(constTensorIdx).c_str());
        int32_t isInputDQ = 0;
        if (nodeIdx != -1)
        {
          isInputDQ = strcmp(onnGraph.node(nodeIdx).op_type().c_str(), "DequantizeLinear") == 0;
        }

        int32_t wtSize = 1;
        if (md.varTensorsDims[0].size() >= 3)
        {
          wtSize = md.varTensorsDims[0][md.varTensorsDims[0].size()-3]; //broadcast the constant along number of channels
        }
        else 
        {
          /* We have only H and W dims, so constant addition to the available single channel, no need to broadcast 
          We preserve default wtSize = 1 set above*/
        }

        if(constTensor.data_type() == TensorProto_DataType_FLOAT ||
            isInputDQ)
        {
          TIDL_onnxBroadcastFloatTensor(layer.weights, wtSize);
        }
        else
        {
          std::string diagMsg = "ALLOWLISTING : Div layer : Supported only if data type of constant is float  --  file info - " + std::string(__FILE__) + " , " \
          + std::string(__func__) + " , " + std::to_string(__LINE__);
          diagsInfo.push_back(diagMsg);
          return -1;
        }
      }
      else //Divide with constant vector
      {
        /* no broadcast required for vector */
      }
      
      /* Update bias for batchnorm */
      layer.bias.ptr = calloc(layer.weights.bufSize, sizeof(float));
      layer.bias.bufSize = layer.weights.bufSize;
    }
    else /* > 1D const tensor */
    {
      layer.layerType = TIDL_EltWiseLayer;
      layer.layerParams.eltWiseParams.eltWiseType = TIDL_EltWiseProduct;
      layer.numInBufs = md.numInputs;
    }
	}
  else	/* both input variable - not supported */
	{
    /* TODO : This is done specifically for layernorm. Ideally should exit as not supported
    if div is not in layernorm fusable combination */
    layer.layerType = TIDL_DivLayer;
    layer.numInBufs = md.numInputs;
  }

	return 0;
}


int32_t TIDL_onnxMapConcatBaseParams(GraphProto&   onnGraph, int32_t i, sTIDL_LayerPC_t &layer)
{
  int32_t axis, status=0, numDim;
  layer.layerType = TIDL_ConcatLayer;
  layer.numInBufs = onnGraph.node(i).input_size();
  NodeProto node = onnGraph.node(i);
  TIDL_onnxGetIntAttr(node, "axis", &axis, 0);
  layer.layerParams.concatParams.axis = axis;

  TIDL_onnxSaveAllowlistingMetaData(onnGraph, i, layer);
  sTIDL_allowlistingMetaData md = layer.allowlistingMetaData;

  if (md.numConstInputs == 1)
  {
    status = TIDL_tfCopyFloatInitializerTensor(onnGraph, i, md.constTensorIndices[0], layer.weights);
  }
  if(status == -1)
  {
    diagsInfo.push_back("Cannot read initializer tensor : Only float and INT64 tensor is supported  -- file info - " + std::string(__FILE__) + " , " \
    + std::string(__func__) + " , " + std::to_string(__LINE__));
    return -1;
  }
  return 0;
}

int32_t TIDL_onnxMapConcatParams(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure,
  int32_t              i,
  int32_t              layerIndex,
  int32_t              *dataIndex,
  GraphProto&            onnGraph)
{
  int32_t status;
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].dataId = (*dataIndex)++;
  status = TIDL_onnxMapConcatBaseParams(onnGraph, i, TIDLPCLayers);
  return status;
}

int32_t TIDL_onnxMapUpsampleBaseParams(GraphProto&   onnGraph, int32_t i, sTIDL_LayerPC_t &layer)
{
  int32_t idx, j;
  char upsampleMode[50];
  layer.layerType = TIDL_ResizeLayer;
  int32_t axis;
  layer.numInBufs = 1;
  NodeProto node = onnGraph.node(i);
  TIDL_onnxGetStringAttr(node, "mode", upsampleMode, 0);
  if ((strcmp(upsampleMode, "nearest") == 0))
  {
    layer.layerParams.resizeParams.mode = TIDL_ResizeNearest;
  }
  else if((strcmp(upsampleMode, "linear") == 0))
  {
    layer.layerParams.resizeParams.mode = TIDL_ResizeBilinear;
  }
  else
  {
    printf("Un suported Upsample Mode \n");
    return -1;
  }
  if ((layer.layerParams.resizeParams.mode == TIDL_ResizeBilinear) ||
      (layer.layerParams.resizeParams.mode == TIDL_ResizeNearest))
  {
    sBuffer_t buf;
    TIDL_tfCopyFloatConst(onnGraph, i, 1, buf);
    float *ptr = (float *)buf.ptr;

    int indices[6]={TIDL_DIM_WIDTH,TIDL_DIM_HEIGHT,TIDL_DIM_NUMCH,TIDL_DIM_BATCH,TIDL_DIM_DIM2,TIDL_DIM_DIM1};
    for(int i=buf.bufSize-1,j=0;i>=0;i--,j++)
    {
      layer.layerParams.resizeParams.resizeRatio[indices[j]]=ptr[i];
    }
    my_free(ptr);
  }
  return 0;
}

int32_t TIDL_onnxMapUpsampleParams(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure,
  int32_t              i,
  int32_t              layerIndex,
  int32_t              *dataIndex,
  GraphProto&            onnGraph)
{
  int32_t status;
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].dataId = (*dataIndex)++;
  status = TIDL_onnxMapUpsampleBaseParams(onnGraph, i, TIDLPCLayers);
  return status;
}

int32_t TIDL_onnxMapResizeBaseParams(GraphProto&   onnGraph, int32_t i, sTIDL_LayerPC_t &layer)
{
  int32_t idx, j, axes_idx = -1;
  int32_t status = 0;
  char upsampleMode[50];
  layer.layerType = TIDL_ResizeLayer;
  int32_t axis;
  layer.numInBufs = 1;
  sBuffer_t buf;
  NodeProto node = onnGraph.node(i);
  int32_t attrRetVal = TIDL_onnxGetStringAttr(node, "mode", upsampleMode, 0);
  layer.layerParams.resizeParams.mode = TIDL_ResizeNearest; /* default value of attribute */
  if ((strcmp(upsampleMode, "nearest") == 0))
  {
    layer.layerParams.resizeParams.mode = TIDL_ResizeNearest;
  }
  else if((strcmp(upsampleMode, "linear") == 0))
  {
    layer.layerParams.resizeParams.mode = TIDL_ResizeBilinear;
  }
  else if(attrRetVal != -1) /* Deny only if attribute(mode) actually present in the model but none of above */
  {
    std::string diagMsg = "ALLOWLISTING : Resize layer : Unsupported Resize mode --  file info - " + std::string(__FILE__) + " , " + std::string(__func__) + " , " + std::to_string(__LINE__);
    diagsInfo.push_back(diagMsg);
    return -1;
  }

  if ((layer.layerParams.resizeParams.mode == TIDL_ResizeBilinear) ||
      (layer.layerParams.resizeParams.mode == TIDL_ResizeNearest))
  {
    /* Copy scales from input */
    /* For versions lower than 10, it is 2nd input */
    if (onnxOpSetVersion <= 10)
    {
      TIDL_tfCopyFloatConst(onnGraph, i, 1, buf);
    }
    /* For any version upper, it is 3rd input */
    else
    {
      if ( node.input_size() <= 4 )
      {
        TIDL_tfCopyFloatConst(onnGraph, i, 2, buf);
      }
      else
      {
        std::string diagMsg = "ALLOWLISTING : Resize layer : more than 4 inputs are not supported for Resize layer --  file info - " + std::string(__FILE__) + " , " + std::string(__func__) + " , " + std::to_string(__LINE__);
        diagsInfo.push_back(diagMsg);
        return -1;
      }
    }

    /* New Operator called antialias introduced. */
    int32_t antialias = 0;
    TIDL_onnxGetIntAttr(node, "antialias",   &antialias, 0);

    /* Antialiasing operation is not supported by TIDL_RT hence exit condition initiated. */
    if(antialias != 0)
    {
      std::string diagMsg = "ALLOWLISTING : Resize layer : Antialiasing feature not supported by TIDL_RT --  file info - " + std::string(__FILE__) + " , " + std::string(__func__) + " , " + std::to_string(__LINE__);
      diagsInfo.push_back(diagMsg);
      return -1;
    }

    /* New attribute keep_aspect_ratio_policy introduced. */
    char keep_aspect_ratio[50];
    int32_t status = TIDL_onnxGetStringAttr(node, "keep_aspect_ratio_policy", keep_aspect_ratio, 0);

    /* Keep_Aspect_Ratio only is required when interpreting the sizes input. So either reject the attribute if not provided or compare with the default value and proceed */
    if((status != -1) && (strcmp(keep_aspect_ratio,"stretch") != 0))
    {
      std::string diagMsg = "ALLOWLISTING : Resize layer : The attribute keep_aspect_ratio_policy only required when sizes given as input. Size input not supported by TIDL_RT --  file info - " + std::string(__FILE__) + " , " + std::string(__func__) + " , " + std::to_string(__LINE__);
      diagsInfo.push_back(diagMsg);
      return -1;
    }

    /* Interpret axes attibute */
    int32_t numDims = node.input_size();
    int32_t *axes = NULL;
    axes_idx = TIDL_onnxGetAttrIdx(node, "axes");
    if (axes_idx != -1)
    {
      /* Read the axes values only if length of scales and axes are same*/
      if (node.attribute(axes_idx).ints_size() == buf.bufSize)
      {
        /* Read axes values */
        axes = (int32_t*)malloc (node.attribute(axes_idx).ints_size() * sizeof (int32_t));
        for (int32_t i = 0; i < node.attribute(axes_idx).ints_size(); i++)
        {
          TIDL_onnxGetIntAttr(node, "axes", &axes[i], i);
          /* Adjust axes[i] to TIDL dimension */
          if(axes[i] >= 0)
          {
            /*+ve Axis*/
            axes[i] += (TIDL_DIM_MAX - numDims);
          }
          else
          {
            /*-ve Axis*/
            axes[i] += TIDL_DIM_MAX;
          }
        }

        /* Read resize Ratios from scales according to axes provided */
        float32_tidl* resizeRatios = (float32_tidl*)buf.ptr;
        for (int32_t i = 0; i < node.attribute(axes_idx).ints_size(); i++)
        {
          layer.layerParams.resizeParams.resizeRatio[axes[i]] = resizeRatios[i];
        }
      }
      else
      {
        std::string diagMsg = "ALLOWLISTING : Resize layer : Axes size & scales size mismatch, unsupported resize node --  file info - " + std::string(__FILE__) + " , " + std::string(__func__) + " , " + std::to_string(__LINE__);
        diagsInfo.push_back(diagMsg);
        return -1;
      }
    }
    /* No axes, copy all scale params to layer params */
    else
    {
      float *ptr = (float *)buf.ptr;
      for (int i = buf.bufSize-1, j = 1; i >= 0, j <= TIDL_DIM_MAX; i--, j++)
      {
        layer.layerParams.resizeParams.resizeRatio[TIDL_DIM_MAX - j] = ptr[i];
      }
      my_free(ptr);
    }
  }
  return 0;
}


int32_t TIDL_onnxMapResizeParams(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure,
  int32_t              i,
  int32_t              layerIndex,
  int32_t              *dataIndex,
  GraphProto&            onnGraph)
{
  int32_t status;
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].dataId = (*dataIndex)++;
  status = TIDL_onnxMapResizeBaseParams(onnGraph, i, TIDLPCLayers);
  return status;
}

int32_t TIDL_onnxMapAvgPoolBaseParams(GraphProto&   onnGraph, int32_t i, sTIDL_LayerPC_t &layer)
{
  int32_t status = 0;
  TIDL_onnxSaveAllowlistingMetaData(onnGraph, i, layer);

  status = TIDL_checkPoolingTensorProperties(layer);
  if(status == -1) return -1;

  sTIDL_allowlistingMetaData md = layer.allowlistingMetaData;

  sTIDL_PoolingParams_t &poolParams = layer.layerParams.poolParams;

  std::vector<int32_t> attr;
  attr.push_back(TIDL_onnxGetIntAttr(onnGraph.node(i), "kernel_shape", &poolParams.kernelW, 0));
  attr.push_back(TIDL_onnxGetIntAttr(onnGraph.node(i), "kernel_shape", &poolParams.kernelH, 1));

  if(std::find(attr.begin(), attr.end(), -1) != attr.end())
  {
    diagsInfo.push_back("ALLOWLISTING : AVG POOL : Missing attribute  -- file info - " + std::string(__FILE__) + " , " \
        + std::string(__func__) + " , " + std::to_string(__LINE__));
    return -1;
  }

  int32_t isGlobalAvgPool = 0;
  if(gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_ONNX_RT)
  {
    if((md.varTensorsDims[0][md.varTensorsDims[0].size()-2] == poolParams.kernelH) && (md.varTensorsDims[0][md.varTensorsDims[0].size()-1] == poolParams.kernelW)) //global avg pool - e.g. squeezenet1.1
    {
      isGlobalAvgPool = 1;
    }
  }
  if(isGlobalAvgPool)
  {
    status = TIDL_onnxMapGlobalAvgPoolBaseParams(onnGraph, i, layer);
  }
  else
  {
    status = TIDL_onnxMapMaxPoolBaseParams(onnGraph, i, layer);
    poolParams.poolingType = TIDL_AveragePooling;

    // Convert 1x1 average pooling into max pooling (operations are identical)
    // mxnet_resnet50_v1d
    if (poolParams.kernelH == 1 && poolParams.kernelW == 1 &&
        ((poolParams.strideH == 2 && poolParams.strideW == 2) ||
        (poolParams.strideH == 1 && poolParams.strideW == 1)))
    {
      poolParams.poolingType = TIDL_MaxPooling;
    }
  }

  return 0;
}

int32_t TIDL_onnxMapAvgPoolParams(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure,
  int32_t              i,
  int32_t              layerIndex,
  int32_t              *dataIndex,
  GraphProto&            onnGraph)
{
  TIDL_onnxMapMaxPoolParams(pOrgTIDLNetStructure, i, layerIndex, dataIndex, onnGraph);
  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].layerParams.poolParams.poolingType = TIDL_AveragePooling;
  return 0;
}

int32_t TIDL_onnxMapGlobalAvgPoolBaseParams(GraphProto&   onnGraph, int32_t i, sTIDL_LayerPC_t &layer)
{
  int32_t status = 0;
  TIDL_onnxSaveAllowlistingMetaData(onnGraph, i, layer);

  status = TIDL_checkPoolingTensorProperties(layer);
  if(status == -1) return -1;

  int32_t padType;
  int32_t idx1, idx2;
  int32_t pads[4];
  sTIDL_PoolingParams_t &poolParams = layer.layerParams.poolParams;


  if (layer.allowlistingMetaData.varTensorsDims[0].size() != 0 && layer.allowlistingMetaData.varTensorsDims[0].size() != 4 &&
      layer.allowlistingMetaData.varTensorsDims[0].size() > 2)
  {
    /** Global average pool is different for non-image done
     * Ex: NxCxHxW ( image ) -> Global average pool is done over dims H, W
     * Ex: NxCxD1 (non image) -> Global average pool is done over D1
     * Ex: NxCxD1XD2xD3 (non image) -> Global average pool is done over D1, D2, D3
    */

    poolParams.avgDims = layer.allowlistingMetaData.varTensorsDims[0].size() - 2;
  }
  else
  {
    poolParams.avgDims = 2;
  }

  layer.layerType = TIDL_PoolingLayer;
  poolParams.poolingType = TIDL_AveragePooling;
  poolParams.strideW = 1;
  poolParams.strideH = 1;
  poolParams.padW = 0;
  poolParams.padH = 0;
  poolParams.padT = 0;
  poolParams.padB = 0;
  poolParams.padL = 0;
  poolParams.padR = 0;

  poolParams.kernelW = 0;
  poolParams.kernelH = 0;
  return 0;
}

int32_t TIDL_onnxMapGlobalAvgPoolParams(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure,
  int32_t              i,
  int32_t              layerIndex,
  int32_t              *dataIndex,
  GraphProto&            onnGraph)
{
  int32_t status;
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].dataId = (*dataIndex)++;
  status = TIDL_onnxMapGlobalAvgPoolBaseParams(onnGraph, i, TIDLPCLayers);

  return status;

}

int32_t TIDL_onnxMapReshapeBaseParams(GraphProto&   onnGraph, int32_t i, sTIDL_LayerPC_t &layer)
{
  int32_t status = 0, attrIdx = -1, allowzero = 0;
  layer.layerType = TIDL_ReshapeLayer;
  TIDL_onnxSaveAllowlistingMetaData(onnGraph, i, layer);

  status = TIDL_checkReshapeTensorProperties(layer);
  if(status == -1) return -1;
  TIDL_tfCopyFloatInitializerTensor(onnGraph, i, 1, layer.weights);
  NodeProto node = onnGraph.node(i);

  attrIdx = TIDL_onnxGetIntAttr(node, "allowzero", &allowzero, 0);

  if (attrIdx != -1 && allowzero == 1)
  {
    /** TIDL doesn't support allowzero attribute*/
    diagsInfo.push_back("ALLOWLISTING : RESHAPE : Unsupported allow_zero attribute  -- file info - " + std::string(__FILE__) + " , " \
          + std::string(__func__) + " , " + std::to_string(__LINE__));
    return -1;
  }

  return 0;
}

int32_t TIDL_onnxMapReshapeParams(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure,
  int32_t              i,
  int32_t              layerIndex,
  int32_t              *dataIndex,
  GraphProto&            onnGraph)
{
  int32_t status;
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].dataId = (*dataIndex)++;
  status = TIDL_onnxMapReshapeBaseParams(onnGraph, i, TIDLPCLayers);
  return status;
}

int32_t TIDL_onnxMapTransposeBaseParams(GraphProto&   onnGraph, int32_t i, sTIDL_LayerPC_t &layer)
{
  int32_t attrIdx, status = 0;
  layer.layerType = TIDL_TransposeLayer;
  NodeProto node = onnGraph.node(i);
  TIDL_onnxSaveAllowlistingMetaData(onnGraph, i, layer);
  attrIdx = TIDL_onnxGetAttrIdx(node, "perm");
  if (attrIdx != -1)
  {
    int32_t j;
    int32_t dims = node.attribute(attrIdx).ints_size();
    layer.weights.ptr = (void*)malloc(sizeof(int)*dims);
    layer.weights.bufSize = dims;
    int32_t *dimVlaue = (int*)layer.weights.ptr;
    for (j = 0; j < layer.weights.bufSize; j++)
    {
      dimVlaue[j] = node.attribute(attrIdx).ints(j);
    }
  }
  status = TIDL_checkTransposeTensorProperties(layer);
  return status;
}

int32_t TIDL_onnxMapTransposeParams(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure,
  int32_t              i,
  int32_t              layerIndex,
  int32_t              *dataIndex,
  GraphProto&            onnGraph)
{
  int32_t status;
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].dataId = (*dataIndex)++;
  status = TIDL_onnxMapTransposeBaseParams(onnGraph, i, TIDLPCLayers);
  return status;
}

int32_t TIDL_onnxMapDepthToSpaceBaseParams(GraphProto&   onnGraph, int32_t i, sTIDL_LayerPC_t &layer)
{
  layer.layerType = TIDL_DepthToSpaceLayer;
  NodeProto node = onnGraph.node(i);
  TIDL_onnxGetIntAttr(node, "blocksize",   &layer.layerParams.depthToSpaceParams.blockSize, 0);
  return 0;
}

int32_t TIDL_onnxMapDepthToSpaceParams(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure,
  int32_t              i,
  int32_t              layerIndex,
  int32_t              *dataIndex,
  GraphProto&            onnGraph)
{
  int32_t status;
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].dataId = (*dataIndex)++;
  status = TIDL_onnxMapDepthToSpaceBaseParams(onnGraph, i, TIDLPCLayers);
  return 0;
}

int32_t TIDL_onnxMapDropOutBaseParams(GraphProto&   onnGraph, int32_t i, sTIDL_LayerPC_t &layer)
{
  layer.layerType = TIDL_DropOutLayer;
  return 0;
}

int32_t TIDL_onnxMapDropoutParams(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure,
  int32_t              i,
  int32_t              layerIndex,
  int32_t              *dataIndex,
  GraphProto&            onnGraph)
{
  int32_t status;
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].dataId = (*dataIndex)++;
  status = TIDL_onnxMapDropOutBaseParams(onnGraph, i, TIDLPCLayers);
  return 0;
}

int32_t TIDL_onnxMapReduceMaxBaseParams(GraphProto&   onnGraph, int32_t i, sTIDL_LayerPC_t &layer)
{
  int32_t numDim;
  int32_t axis, axisStatus = 0;
  layer.layerType = TIDL_ReduceLayer;
  layer.numInBufs = onnGraph.node(i).input_size();
  NodeProto node  = onnGraph.node(i);
  TIDL_onnxGetIntAttr(node, "keepdims",   &layer.layerParams.reduceParams.keepDims, 0);
  layer.layerParams.reduceParams.ops = 0;

  TIDL_onnxSaveAllowlistingMetaData(onnGraph, i, layer);
  /** If shape inference is not done on model, we assume the tensor is of 4 dimensions by default*/
  if (layer.allowlistingMetaData.varTensorsDims.size() != 0)
  {
    numDim = layer.allowlistingMetaData.varTensorsDims[0].size();
    numDim = (numDim == 0)? 4:numDim;
  }
  else
  {
    numDim = 4;
  }

  layer.layerParams.reduceParams.ops = 0;  /* This value is reset if ReduceMax and set if ReduceMin. */

  /* axes attribute is there in lower than 18 opset */
  axisStatus = TIDL_onnxGetIntAttr(node, "axes",   &axis, 0);

  /* axis not found in attibute */
  if (axisStatus == -1)
  {
    /* Attribute added in opset 18 */
    int32_t noop_with_empty_axes = 0;
    TIDL_onnxGetIntAttr(node, "noop_with_empty_axes", &noop_with_empty_axes, 0);

    TensorProto axes, inputTensor;
    /* Obtaining the axes input from the node. */
    axes = TIDL_getInitializerTensor(onnGraph, onnGraph.node(i).input(1), i);
    /* Obtaining the input tensor from the node. */
    inputTensor = TIDL_getInitializerTensor(onnGraph, onnGraph.node(i).input(0), i);

    if(axes.dims_size() > 0) /* Check whether axes input is given. */
    {
      /* Only 1 element in axes we take*/
      if (axes.dims(0) >= 1)
      {
        axis = axes.int32_data(0);
        if (axes.dims(0) > 1)
        {
          std::string diagMsg = "ALLOWLISTING : ReduceMax layer : TIDL_RT only supports 1 axis - using the first value in axes  --  file info - " + std::string(__FILE__) + " , " + std::string(__func__) + " , " + std::to_string(__LINE__);
          diagsInfo.push_back(diagMsg);
        }
      }
    }
    else
    {
      /* If axes value is not given, and noop_with_empty_axes is true then perform no operation on the input tensor. Return it as such.*/
      if(noop_with_empty_axes)
      {
        std::string diagMsg = "ALLOWLISTING : ReduceMax layer : No operation to be performed on the Input Tensor --  file info - " + std::string(__FILE__) + " , " + std::string(__func__) + " , " + std::to_string(__LINE__);
        diagsInfo.push_back(diagMsg);
        return 0;
      }
      /* If noop_with_empty_axes is not true(not set) then reduce all the axes in the input tensor. */
      else
      {
        std::string diagMsg = "ALLOWLISTING : ReduceMax layer : Reducing in all dimensions is not supported in TIDL-RT --  file info - " + std::string(__FILE__) + " , " + std::string(__func__) + " , " + std::to_string(__LINE__);
        diagsInfo.push_back(diagMsg);
        return -1;
      }
    }
  }

  layer.layerParams.reduceParams.axis = axis;
  /** Adjust axis to TIDL dimensions*/
  layer.layerParams.reduceParams.axis += (TIDL_DIM_MAX - numDim);

  return 0;
}

int32_t TIDL_onnxMapReduceMinBaseParams(GraphProto&   onnGraph, int32_t i, sTIDL_LayerPC_t &layer)
{
  int32_t numDim;
  int32_t axis, axisStatus = 0;
  layer.layerType = TIDL_ReduceLayer;
  layer.numInBufs = onnGraph.node(i).input_size();
  NodeProto node  = onnGraph.node(i);
  TIDL_onnxGetIntAttr(node, "keepdims",   &layer.layerParams.reduceParams.keepDims, 0);
  layer.layerParams.reduceParams.ops = 1;

  TIDL_onnxSaveAllowlistingMetaData(onnGraph, i, layer);
  /** If shape inference is not done on model, we assume the tensor is of 4 dimensions by default*/
  if (layer.allowlistingMetaData.varTensorsDims.size() != 0)
  {
    numDim = layer.allowlistingMetaData.varTensorsDims[0].size();
    numDim = (numDim == 0)? 4:numDim;
  }
  else
  {
    numDim= 4;
  }

  layer.layerParams.reduceParams.ops = 1; /* This value is reset(=0) if ReduceMax and set(=1) if ReduceMin. */

  /* axes attribute is there in lower than 18 opset */
  axisStatus = TIDL_onnxGetIntAttr(node, "axes",   &axis, 0);

  /* axis not found in attibute */
  if(axisStatus == -1)
  {

    /* Attribute added in opset 18 */
    int32_t noop_with_empty_axes = 0;
    TIDL_onnxGetIntAttr(node, "noop_with_empty_axes", &noop_with_empty_axes, 0);

    TensorProto axes, inputTensor;
    /* Obtaining the axes input from the node. */
    axes = TIDL_getInitializerTensor(onnGraph, onnGraph.node(i).input(1), i);
    /* Obtaining the input tensor from the node. */
    inputTensor = TIDL_getInitializerTensor(onnGraph, onnGraph.node(i).input(0), i);

    if(axes.dims_size() > 0) /* Check whether axes input is given. */
    {
      for(int j = 0; j < axes.dims(0); j++) /* Axes is an optional 1D Tensor Input. */
      /* Only 1 element in axes we take*/
      if (axes.dims(0) >= 1)
      {
        axis = axes.int32_data(0);
        if (axes.dims(0) > 1)
        {
          std::string diagMsg = "ALLOWLISTING : ReduceMin layer : TIDL_RT only supports 1 axis - using the first value in axes  --  file info - " + std::string(__FILE__) + " , " + std::string(__func__) + " , " + std::to_string(__LINE__);
          diagsInfo.push_back(diagMsg);
        }
      }
    }
    else
    {
      /* If axes value is not given, and noop_with_empty_axes is true then perform no operation on the input tensor. Return it as such.*/
      if(noop_with_empty_axes)
      {
        std::string diagMsg = "ALLOWLISTING : ReduceMin layer : No operation to be performed on the Input Tensor --  file info - " + std::string(__FILE__) + " , " + std::string(__func__) + " , " + std::to_string(__LINE__);
        diagsInfo.push_back(diagMsg);
        return 0;
      }
      /* If noop_with_empty_axes is not true(not set) then reduce all the axes in the input tensor. */
      else
      {
        std::string diagMsg = "ALLOWLISTING : ReduceMin layer : Reducing in all dimensions is not supported in TIDL-RT --  file info - " + std::string(__FILE__) + " , " + std::string(__func__) + " , " + std::to_string(__LINE__);
        diagsInfo.push_back(diagMsg);
        return -1;
      }
    }
  }

  layer.layerParams.reduceParams.axis = axis;
  /** Adjust axis to TIDL dimensions*/
  layer.layerParams.reduceParams.axis += (TIDL_DIM_MAX - numDim);


  return 0;
}

int32_t TIDL_onnxMapReduceMaxParams(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure,
  int32_t              i,
  int32_t              layerIndex,
  int32_t              *dataIndex,
  GraphProto&            onnGraph)
{
  int32_t status;
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].dataId = (*dataIndex)++;
  status = TIDL_onnxMapReduceMaxBaseParams(onnGraph, i, TIDLPCLayers);

  return 0;
}

int32_t TIDL_onnxMapReduceMinParams(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure,
  int32_t              i,
  int32_t              layerIndex,
  int32_t              *dataIndex,
  GraphProto&            onnGraph)
{
  int32_t status;
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].dataId = (*dataIndex)++;
  status = TIDL_onnxMapReduceMinBaseParams(onnGraph, i, TIDLPCLayers);

  return 0;
}

int32_t TIDL_onnxMapScatterElementsBaseParams(GraphProto&   onnGraph, int32_t i, sTIDL_LayerPC_t &layer)
{
  int32_t numDim;
  layer.layerType = TIDL_ScatterElementsLayer;
  layer.numInBufs = onnGraph.node(i).input_size();
  TIDL_onnxGetIntAttr(onnGraph.node(i), "axis",   &layer.layerParams.scatterElementsParams.axis, 0);

  TIDL_onnxSaveAllowlistingMetaData(onnGraph, i, layer);
  /** If shape inference is not done on model, we assume the tensor is of 4 dimensions by default*/
  if (layer.allowlistingMetaData.varTensorsDims.size() != 0)
  {
    numDim = layer.allowlistingMetaData.varTensorsDims[0].size();
    numDim = (numDim == 0)? 4:numDim;
  }
  else
  {
    numDim= 4;
  }

  /** Adjust axis to TIDL dimensions*/
  layer.layerParams.scatterElementsParams.axis += (TIDL_DIM_MAX - numDim);

  strcpy((char*)layer.layerParams.scatterElementsParams.dataTensorName, (char*)onnGraph.node(i).input(0).c_str());
  strcpy((char*)layer.layerParams.scatterElementsParams.indicesTensorName, (char*)onnGraph.node(i).input(1).c_str());
  strcpy((char*)layer.layerParams.scatterElementsParams.updateTensorName, (char*)onnGraph.node(i).input(2).c_str());

  /* New attribute called reduction introduced. */
  char reduction[50];
  int32_t reduction_status = -1;

  /** The reduction attribute can have the following values:
   * "none". "add", "mul", "max", "min".
   * Only none value(also the default value) supported by TIDL_RT.
  */
  reduction_status = TIDL_onnxGetStringAttr(onnGraph.node(i), "reduction", reduction, 0);
  if( (reduction_status != -1) &&
      (strcmp(reduction, "none") != 0)
    )
  {
    std::string diagMsg = "ALLOWLISTING : ScatterElements layer : Reduction attribute value not 'none', not supported by TIDL_RT --  file info - " + std::string(__FILE__) + " , " + std::string(__func__) + " , " + std::to_string(__LINE__);
    diagsInfo.push_back(diagMsg);
    return -1;
  }

  layer.layerParams.scatterElementsParams.reduction = TIDL_ScatterElementsNone;
  return 0;
}

int32_t TIDL_onnxMapScatterElementsParams(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure,
  int32_t              i,
  int32_t              layerIndex,
  int32_t              *dataIndex,
  GraphProto&            onnxGraph)
{
  int32_t status;
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].dataId = (*dataIndex)++;
  status = TIDL_onnxMapScatterElementsBaseParams(onnxGraph, i, TIDLPCLayers);
  return 0;
}


int32_t TIDL_onnxMapScatterNDBaseParams(GraphProto&   onnGraph, int32_t i, sTIDL_LayerPC_t &layer)
{
  int32_t status = 0, status2 = 0;
  layer.layerType = TIDL_ScatterElementsLayer;
  layer.numInBufs = onnGraph.node(i).input_size();
  TIDL_onnxSaveAllowlistingMetaData(onnGraph, i, layer);
  sTIDL_allowlistingMetaData md = layer.allowlistingMetaData;
  if (md.numConstInputs == 1)
  {
    status = TIDL_tfCopyFloatInitializerTensor(onnGraph, i, md.constTensorIndices[0], layer.weights);
  }
  else if (md.numConstInputs == 2)
  {
    status = TIDL_tfCopyFloatInitializerTensor(onnGraph, i, md.constTensorIndices[0], layer.weights);
    status2 = TIDL_tfCopyFloatInitializerTensor(onnGraph, i, md.constTensorIndices[1], layer.bias);
  }
  if(status == -1 || status2 == -1)
  {
    diagsInfo.push_back("Cannot read initializer tensor : Only float and INT64 tensor is supported  -- file info - " + std::string(__FILE__) + " , " \
    + std::string(__func__) + " , " + std::to_string(__LINE__));
    return -1;
  }
  layer.layerParams.scatterElementsParams.axis = -1;

  NodeProto node  = onnGraph.node(i);
  char reduction[32];
  int32_t reduction_status = TIDL_onnxGetStringAttr(node, "reduction", reduction, 0);
  /* No reduction attribute */
  if(reduction_status == -1)
  {
    layer.layerParams.scatterElementsParams.reduction = TIDL_ScatterElementsNone;
  }
  /* reduction must be one of none, add, min, max */
  else
  {
    if (strcmp("add", reduction) == 0)
    {
      layer.layerParams.scatterElementsParams.reduction = TIDL_ScatterElementsAdd;
    }
    else if(strcmp("max", reduction) == 0)
    {
      layer.layerParams.scatterElementsParams.reduction = TIDL_ScatterElementsMax;
    }
    else if(strcmp("min", reduction) == 0)
    {
      layer.layerParams.scatterElementsParams.reduction = TIDL_ScatterElementsMin;
    }
    else if(strcmp("none", reduction) == 0)
    {
      layer.layerParams.scatterElementsParams.reduction = TIDL_ScatterElementsNone;
    }
  }

  return status;
}

int32_t TIDL_onnxMapGatherBaseParams(GraphProto&   onnGraph, int32_t i, sTIDL_LayerPC_t &layer)
{
  int32_t status =0, axis, numDim;
  layer.layerType = TIDL_GatherLayer;
  layer.numInBufs = onnGraph.node(i).input_size();

  TIDL_onnxGetIntAttr(onnGraph.node(i), "axis", &axis, 0);
  layer.layerParams.gatherParams.axis = axis;
  TIDL_onnxSaveAllowlistingMetaData(onnGraph, i, layer);

  sTIDL_allowlistingMetaData md = layer.allowlistingMetaData;

  if(md.numConstInputs > 0)
  {
    int constTensorIdx = md.constTensorIndices[0];
    status = TIDL_tfCopyFloatInitializerTensor(onnGraph, i, constTensorIdx, layer.weights);
    if(status == -1)
    {
      diagsInfo.push_back("Cannot read initializer tensor : Only float and INT64 tensor is supported  -- file info - " + std::string(__FILE__) + " , " \
        + std::string(__func__) + " , " + std::to_string(__LINE__));
      return -1;
    }
  }
  layer.inData[0].numDim = md.varTensorsDims[0].size();
  layer.inData[1].numDim = md.varTensorsDims[1].size();

  status = TIDL_checkGatherTensorProperties(layer);
  if (status == -1) return -1;
  return 0;
}

int32_t TIDL_onnxMapReduceMeanParams(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure,
  int32_t              i,
  int32_t              layerIndex,
  int32_t              *dataIndex,
  GraphProto&            onnxGraph)
{
  sTIDL_LayerPC_t &TIDLPCLayers       = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].dataId = (*dataIndex)++;
  int32_t status = TIDL_onnxMapReduceMeanBaseParams(onnxGraph, i, TIDLPCLayers);
  return status;
}

int32_t TIDL_onnxMapReduceMeanBaseParams(GraphProto&   onnGraph, int32_t i, sTIDL_LayerPC_t &layer)
{
  int32_t numDim;
  layer.layerType = TIDL_ReduceMeanLayer;
  layer.numInBufs = onnGraph.node(i).input_size();
  NodeProto node  = onnGraph.node(i);
  int32_t axis = -1;
  int32_t status = TIDL_onnxGetIntAttr(node, "axes",   &layer.layerPCParams.reduceMeanParams.axis, 0);
  TIDL_onnxGetIntAttr(node, "keepdims", &layer.layerPCParams.reduceMeanParams.reduceDims, 0);

  TIDL_onnxSaveAllowlistingMetaData(onnGraph, i, layer);
  /** If shape inference is not done on model, we assume the tensor is of 4 dimensions by default*/
  if (layer.allowlistingMetaData.varTensorsDims.size() != 0)
  {
    numDim = layer.allowlistingMetaData.varTensorsDims[0].size();
    numDim = (numDim == 0)? 4:numDim;
  }
  else
  {
    numDim = 4;
  }

  if(status == -1)
  {
    axis = -1;
  }
  /*Translate to TIDL Axis:*/
  if(axis >= 0)
  {
    /*+ve Axis*/
    axis += (TIDL_DIM_MAX - numDim);
  }
  else
  {
    /*-ve Axis*/
    axis = (TIDL_DIM_MAX + axis);
  }
  /** Adjust axis to TIDL dimensions*/
  layer.layerParams.reduceParams.axis = axis;
  return 0;
}

int32_t TIDL_onnxMapPowParams(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure,
  int32_t              i,
  int32_t              layerIndex,
  int32_t              *dataIndex,
  GraphProto&            onnxGraph)
{
  sTIDL_LayerPC_t &TIDLPCLayers       = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].dataId = (*dataIndex)++;
  int32_t status = TIDL_onnxMapPowBaseParams(onnxGraph, i, TIDLPCLayers);
  return status;
}

int32_t TIDL_onnxMapPowBaseParams(GraphProto&   onnGraph, int32_t i, sTIDL_LayerPC_t &layer)
{

  int32_t status =0;
  layer.layerType = TIDL_PowLayer;
  TIDL_onnxSaveAllowlistingMetaData(onnGraph, i, layer);
  sTIDL_allowlistingMetaData md = layer.allowlistingMetaData;
  /*Currently only supporting a single global pow() fxn*/
  if(md.numConstInputs > 0)
  {
    int constTensorIdx = md.constTensorIndices[0];
    TensorProto constTensor = TIDL_getInitializerTensor(onnGraph, onnGraph.node(i).input(constTensorIdx), i);
    int32_t varTensorIdx = md.varTensorIndices[0];
    if(constTensor.dims_size() == 0)
    {
        status = TIDL_tfCopyFloatInitializerTensor(onnGraph, i, constTensorIdx, layer.bias);
        if(status == -1)
        {
          diagsInfo.push_back("Cannot read initializer tensor : Only float and INT64 tensor is supported  -- file info - " + std::string(__FILE__) + " , " \
          + std::string(__func__) + " , " + std::to_string(__LINE__));
          return -1;
        }
        layer.layerPCParams.powParams.pow = *((float*)layer.bias.ptr);
    }
  }
  else
  {
    printf("Only global Pow() is supported currently\n");
    return -1;
  }

  return 0;
}

int32_t TIDL_onnxMapSqrtParams(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure,
  int32_t              i,
  int32_t              layerIndex,
  int32_t              *dataIndex,
  GraphProto&            onnxGraph)
{
  sTIDL_LayerPC_t &TIDLPCLayers       = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].dataId = (*dataIndex)++;
  int32_t status = TIDL_onnxMapSqrtBaseParams(onnxGraph, i, TIDLPCLayers);
  return status;
}

int32_t TIDL_onnxMapSqrtBaseParams(GraphProto&   onnGraph, int32_t i, sTIDL_LayerPC_t &layer)
{
  layer.layerType = TIDL_SqrtLayer;
  return 0;
}

int32_t TIDL_onnxMapDequantizeLinearParams(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure,
  int32_t              i,
  int32_t              layerIndex,
  int32_t              *dataIndex,
  GraphProto&            onnxGraph)
{
  sTIDL_LayerPC_t &TIDLPCLayers       = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].dataId = (*dataIndex)++;
  int32_t status = TIDL_onnxMapDequantizeLinearBaseParams(onnxGraph, i, TIDLPCLayers);
  return status;
}

int32_t TIDL_onnxMapQuantizeLinearParams(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure,
  int32_t              i,
  int32_t              layerIndex,
  int32_t              *dataIndex,
  GraphProto&            onnxGraph)
{
  sTIDL_LayerPC_t &TIDLPCLayers       = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].dataId = (*dataIndex)++;
  int32_t status = TIDL_onnxMapQuantizeLinearBaseParams(onnxGraph, i, TIDLPCLayers);
  return status;
}

int32_t TIDL_onnxMapDequantizeLinearBaseParams(GraphProto&   onnGraph, int32_t i, sTIDL_LayerPC_t &layer)
{

  /*Dequantize nodes are not directly supported - they need to either be optimized out (with stats collected)
    or routed to DataConvert layers which implement it's functionality*/
  int32_t status = 0;
  layer.layerType = TIDL_DequantizeLayer;
  /*Map Dequantize nodes corresponding to activations: */
  TensorProto tensorScale       = TIDL_getInitializerTensor(onnGraph, onnGraph.node(i).input(1) , i);
  TensorProto tensorZeroPoint   = TIDL_getInitializerTensor(onnGraph, onnGraph.node(i).input(2) , i);
  /*Update scale and zero point:*/
  float tZeroPoint = 0.0;
  if(TensorProto_DataType_INT8 == tensorZeroPoint.data_type())
  {
    bool tensorZPFromRaw = tensorZeroPoint.int32_data_size() == 0;
    int8_t tZP;
    if(tensorZPFromRaw)
    {
      tZP = ((int8_t* )(tensorZeroPoint.raw_data().c_str()))[0];
    }
    else
    {
      tZP = (int8_t )(tensorZeroPoint.int32_data(0));
    }
    tZeroPoint = (float)tZP;
    layer.layerPCParams.quantizeParams.elementType = TIDL_SignedChar;
  }
  else
  {
    bool tensorZPFromRaw = tensorZeroPoint.int32_data_size() == 0;
    uint8_t tZP;
    if(tensorZPFromRaw)
    {
      tZP = ((uint8_t* )(tensorZeroPoint.raw_data().c_str()))[0];
    }
    else
    {
      tZP = (uint8_t )(tensorZeroPoint.int32_data(0));
    }
    tZeroPoint = (float)tZP;
    layer.layerPCParams.quantizeParams.elementType = TIDL_UnsignedChar;
  }
  float  tScale;
  bool tensorScaleFromRaw = tensorScale.float_data_size() == 0;
  if(tensorScaleFromRaw)
  {
    tScale = ((float* )(tensorScale.raw_data().c_str()))[0];
  }
  else
  {
    tScale = (float )(tensorScale.float_data(0));
  }
  layer.layerPCParams.dequantParams.scale = tScale;
  layer.layerPCParams.dequantParams.zeroPoint = tZeroPoint;
  layer.outData[0].tensorScale = 1/tScale;
  layer.outData[0].tensorZeroPoint = tZeroPoint;
  /*OSRT Metadata:*/
  TIDL_onnxSaveAllowlistingMetaData(onnGraph, i, layer);
  sTIDL_allowlistingMetaData md = layer.allowlistingMetaData;
  layer.numInBufs = md.numVarInputs;

  return 0;
}


int32_t TIDL_onnxMapQuantizeLinearBaseParams(GraphProto&   onnGraph, int32_t i, sTIDL_LayerPC_t &layer)
{
  /*Dequantize nodes are not directly supported - they need to either be optimized out (with stats collected)
    or routed to DataConvert layers which implement it's functionality*/
  int32_t status = 0;
  layer.layerType = TIDL_QuantizeLayer;
  /*Map Dequantize nodes corresponding to activations: */
  TensorProto tensorScale       = TIDL_getInitializerTensor(onnGraph, onnGraph.node(i).input(1) , i);
  TensorProto tensorZeroPoint   = TIDL_getInitializerTensor(onnGraph, onnGraph.node(i).input(2) , i);
  /*Update scale and zero point:*/
  float tZeroPoint = 0.0;
  if(TensorProto_DataType_INT8 == tensorZeroPoint.data_type())
  {
    bool tensorZPFromRaw = tensorZeroPoint.int32_data_size() == 0;
    int8_t tZP;
    if(tensorZPFromRaw)
    {
      tZP = ((int8_t* )(tensorZeroPoint.raw_data().c_str()))[0];
    }
    else
    {
      tZP = (int8_t )(tensorZeroPoint.int32_data(0));
    }
    tZeroPoint = (float)tZP;
    layer.layerPCParams.quantizeParams.elementType = TIDL_SignedChar;
  }
  else
  {
    bool tensorZPFromRaw = tensorZeroPoint.int32_data_size() == 0;
    uint8_t tZP;
    if(tensorZPFromRaw)
    {
      tZP = ((uint8_t* )(tensorZeroPoint.raw_data().c_str()))[0];
    }
    else
    {
      tZP = (uint8_t )(tensorZeroPoint.int32_data(0));
    }
    tZeroPoint = (float)tZP;
    layer.layerPCParams.quantizeParams.elementType = TIDL_UnsignedChar;
  }

  float  tScale;
  bool tensorScaleFromRaw = tensorScale.float_data_size() == 0;
  if(tensorScaleFromRaw)
  {
    tScale = ((float* )(tensorScale.raw_data().c_str()))[0];
  }
  else
  {
    tScale = (float )(tensorScale.float_data(0));
  }


  layer.layerPCParams.quantizeParams.scale = tScale;
  layer.layerPCParams.quantizeParams.zeroPoint = tZeroPoint;

  /*OSRT Metadata:*/
  TIDL_onnxSaveAllowlistingMetaData(onnGraph, i, layer);
  sTIDL_allowlistingMetaData md = layer.allowlistingMetaData;
  layer.numInBufs = md.numVarInputs;

  return 0;
}

int32_t TIDL_onnxMapGatherParams(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure,
  int32_t              i,
  int32_t              layerIndex,
  int32_t              *dataIndex,
  GraphProto&            onnxGraph)
{
  int32_t status;
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].dataId = (*dataIndex)++;
  status = TIDL_onnxMapGatherBaseParams(onnxGraph, i, TIDLPCLayers);
  /*Need to put some error checks on the input and weight dimension*/
  //@TODO
  return 0;
}

int32_t TIDL_onnxMapScatterNDParams(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure,
  int32_t              i,
  int32_t              layerIndex,
  int32_t              *dataIndex,
  GraphProto&            onnxGraph)
{
  int32_t status;
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].dataId = (*dataIndex)++;
  TIDLPCLayers.outData[0].numDim = 4;
  status = TIDL_onnxMapScatterNDBaseParams(onnxGraph, i, TIDLPCLayers);
  /*Need to put some error checks on the input and weight dimension*/
  //@TODO
  return 0;
}


int32_t TIDL_onnxMapSqueezeBaseParams(GraphProto&   onnGraph, int32_t i, sTIDL_LayerPC_t &layer)
{
  /** Num inputs for squeeze is 2 from onnx opset 13 (includes axes as an input), but it's an initializer actually, hence hard coding numInputs to 1*/
  layer.layerType = TIDL_SqueezeLayer;
  layer.numInBufs = 1;
  NodeProto node  = onnGraph.node(i);

  int32_t axes_status = -1, axesIdx = -1;
  int32_t j, ii, numDim = 0;
  int32_t num_axes_4_squeeze = -1;
  int32_t axes[TIDL_DIM_MAX];

  axesIdx = TIDL_onnxGetAttrIdx(node, "axes");
  if (axesIdx != -1)
  {
    axes_status = axesIdx;
    num_axes_4_squeeze = node.attribute(axesIdx).ints_size();
    for(ii = 0; ii< num_axes_4_squeeze; ii++)
    {
      TIDL_onnxGetIntAttr(node, "axes",   &axes[ii], ii);
    }
  }
  else
  {
    /* axes can be an input in opset 18*/
    sBuffer_t buf;
    axes_status = TIDL_tfCopyFloatInitializerTensor(onnGraph, i, 1, buf);
    if (axes_status != -1)
    {
      num_axes_4_squeeze = buf.bufSize;
      memcpy (axes, buf.ptr, sizeof(int32_t) * num_axes_4_squeeze);
    }
    free (buf.ptr);
  }

  if (axes_status == -1)
  {
    diagsInfo.push_back("ALLOWLISTING : Squeeze : No axis given for  Squeeze  -- file info - " + std::string(__FILE__) + " , " \
          + std::string(__func__) + " , " + std::to_string(__LINE__));
    return -1;
  }
  TIDL_onnxSaveAllowlistingMetaData(onnGraph, i, layer);
  /** If shape inference is not done on model, we assume the tensor is of 4 dimensions by default*/
  if (layer.allowlistingMetaData.varTensorsDims.size() != 0)
  {
    numDim = layer.allowlistingMetaData.varTensorsDims[0].size();
    numDim = (numDim == 0)? 4:numDim;
  }
  else
  {
    numDim = 4;
  }

  /* Updating the axes to TIDL max dimensions*/
  for (ii = 0; ii < num_axes_4_squeeze; ii++)
  {
    axes[ii] = axes[ii] + (TIDL_DIM_MAX - numDim);
  }

  for(ii = 0; ii< TIDL_DIM_MAX; ii++)
  {
    layer.layerPCParams.squeezeParams.axis[ii] = 0;
  }

  for(ii = 0; ii< TIDL_DIM_MAX; ii++)
  {
    for(j=0;j<num_axes_4_squeeze;j++)
    {
      if(ii == axes[j])
      {
        layer.layerPCParams.squeezeParams.axis[ii] = 1; // squeeze this particular axis
      }
    }
  }

  return 0;
}
int32_t TIDL_onnxMapSqueezeParams(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure,
  int32_t              i,
  int32_t              layerIndex,
  int32_t              *dataIndex,
  GraphProto&            onnGraph )
{
  int32_t status;
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].dataId = (*dataIndex)++;
  status = TIDL_onnxMapSqueezeBaseParams(onnGraph, i, TIDLPCLayers);

  return 0;
}
int32_t TIDL_onnxMapConstParams(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure,
  int32_t              i,
  int32_t              layerIndex,
  int32_t              *dataIndex,
  GraphProto&            onnGraph)
{
  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].layerType = TIDL_ConstDataLayer;
  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outData[0].dataId = (*dataIndex)++;
  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].numInBufs = 0;
  return 0;
}


int32_t TIDL_onnxMapIdentityBaseParams(GraphProto&   onnGraph, int32_t i, sTIDL_LayerPC_t &layer)
{
  layer.layerType = TIDL_IdentityLayer;
  return 0;
}

int32_t TIDL_onnxMapIdentityParams(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure,
  int32_t              i,
  int32_t              layerIndex,
  int32_t              *dataIndex,
  GraphProto&            onnGraph)
{
  int32_t status;
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].dataId = (*dataIndex)++;
  status = TIDL_onnxMapIdentityBaseParams(onnGraph, i, TIDLPCLayers);
  return status;
}

int32_t TIDL_tfMapUnSuportedlayerParams(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure,
  int32_t              i,
  int32_t              layerIndex,
  int32_t              *dataIndex,
  GraphProto&            onnGraph)
{
  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].layerType = TIDL_UnsupportedLayer;
  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outData[0].dataId = (*dataIndex)++;
  return 0;
}

typedef struct {
  int8_t name[TIDL_STRING_SIZE];
  int32_t(*tidl_onnxMapFunc)(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure,
    int32_t              i,
    int32_t              layerIndex,
    int32_t              *dataIndex,
    GraphProto&             onnxGraph);
}sTIDL_onnxOpParamMap_t;

sTIDL_onnxOpParamMap_t tidl_onnxOpParamMapTable[] =
{
  { "Conv",                          TIDL_onnxMapConvParams },
  { "MaxPool",                       TIDL_onnxMapMaxPoolParams },
  { "Relu",                          TIDL_onnxMapReluParams },
  { "Clip",                          TIDL_onnxMapClipParams },
  { "PRelu",                         TIDL_onnxMapPReluParams },
  { "Sigmoid",                       TIDL_onnxMapSigmoidParams },
  { "Tanh",                          TIDL_onnxMapTanhParams },
  { "HardSigmoid",                   TIDL_onnxMapHardSigmoidParams },
  { "Elu",                           TIDL_onnxMapELUParams },
  { "Logistic",                      TIDL_onnxMapSigmoidParams },
  { "LeakyRelu",                     TIDL_onnxMapLeakyReluParams },
  { "Concat",                        TIDL_onnxMapConcatParams },
  { "AveragePool",                   TIDL_onnxMapAvgPoolParams },
  { "GlobalAveragePool",             TIDL_onnxMapGlobalAvgPoolParams },
  { "Upsample",                      TIDL_onnxMapUpsampleParams },
  { "Resize",                        TIDL_onnxMapResizeParams },
  { "ConvTranspose",                 TIDL_onnxMapConvTransposeParams },
  { "Reshape",                       TIDL_onnxMapReshapeParams },
  { "Transpose",                     TIDL_onnxMapTransposeParams },
  { "Add",                           TIDL_onnxMapAddParams },
  { "Sum",                           TIDL_onnxMapAddParams },
  { "Sub",                           TIDL_onnxMapSubParams },
  { "ArgMax",                        TIDL_onnxMapArgmaxParams },
  { "BatchNormalization",            TIDL_onnxMapBNParams },
  { "Gemm",                          TIDL_onnxGemmParams },
  { "MatMul",                        TIDL_onnxGemmParams },
  { "Softmax",                       TIDL_onnxSoftmaxParams },
  { "Split",                         TIDL_onnxSplitParams },
  { "Slice",                         TIDL_onnxSliceParams },
  { "Flatten",                       TIDL_onnxMapFlattenParams },
  { "Pad",                           TIDL_onnxMapPadParams },
  { "Mul",                           TIDL_onnxMapMulParams },
  { "DepthToSpace",                  TIDL_onnxMapDepthToSpaceParams },
  { "ReduceMax",                     TIDL_onnxMapReduceMaxParams },
  { "ReduceMin",                     TIDL_onnxMapReduceMinParams },
  { "ScatterElements",               TIDL_onnxMapScatterElementsParams },
  { "Squeeze",                       TIDL_onnxMapSqueezeParams },
  { "Dropout",                       TIDL_onnxMapDropoutParams },
  { "ScatterND",                     TIDL_onnxMapScatterNDParams},
  { "Cast",                          TIDL_onnxMapIdentityParams },
  { "Gather",                        TIDL_onnxMapGatherParams },
  { "DequantizeLinear",              TIDL_onnxMapDequantizeLinearParams },
  { "QuantizeLinear",                TIDL_onnxMapQuantizeLinearParams },
  { "Erf",                           TIDL_onnxMapIdentityParams},
  { "ReduceMean",                    TIDL_onnxMapReduceMeanParams},
  { "Pow",                           TIDL_onnxMapPowParams},
  { "Sqrt",                          TIDL_onnxMapSqrtParams},
  { "Div",                           TIDL_onnxMapDivParams}
};
int32_t TIDL_getOnnxOpParamMapId(const char  * name)
{
  int32_t i = -1;
  for (i = 0; i < sizeof(tidl_onnxOpParamMapTable) / sizeof(sTIDL_onnxOpParamMap_t); i++)
  {
    if ((strcmp(name, (const char *)tidl_onnxOpParamMapTable[i].name) == 0))
    {
      return (i);
    }
  }
  return (-1);
}

TIDL_TFLayerMapping_t TIDL_OnnxLayerMap[] =
{
  { (char*)"TIDL_MatMulBias",         (char*)"TIDL_InnerProductLayerTIDL_EltWiseLayer", 2},
  { (char*)"TIDL_OnnxShuffle",        (char*)"TIDL_ReshapeLayerTIDL_TransposeLayerTIDL_ReshapeLayer", 3},
  { (char*)"TIDL_TFSlimShuffle",        (char*)"NANA"              , 3 },
  { (char*)"TIDL_LayernormGamma",     (char*)"TIDL_LayerNormLayerTIDL_EltWiseLayerTIDL_EltWiseLayer", 3},
  { (char*)"TIDL_PatchEmbedding",      (char*)"TIDL_ConvolutionLayer",  1}
};

bool tidl_is1DBroadCastTensor(sTIDL_LayerPC_t &layer)
{
  bool status = true;
  int32_t i;
  /*Const idx:*/
  if (layer.allowlistingMetaData.numConstInputs == 1)
  {
    int32_t bufIdx = layer.allowlistingMetaData.constTensorIndices[0];
    for(i = TIDL_DIM_HEIGHT; i >= 0; i--)
    {
      if(layer.inData[bufIdx].dimValues[i] != 1)
      {
        status = false;
        break;
      }
    }
  }
  else
  {
    status = false;
  }
  
  return status;
}

int32_t tidl_fuseLayerNormBetaGamma(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex, int32_t* dataIndex)
{
  /*This function attempts to fuse the beta & gamma terms into a projection in front of the layernorm,
    if a projection is not found, it will introduce an innerproduct layer instead (Identity matrix mutated)*/
  int32_t i1;
  int32_t status = 0;
  int32_t outLayerIdx = -1;
  for (i1 = 0; i1 < layerIndex; i1++)
  {
     if (tf_isLayerType("TIDL_LayernormGamma", i1, pOrgTIDLNetStructure, TIDL_OnnxLayerMap, (sizeof(TIDL_OnnxLayerMap) / sizeof(TIDL_TFLayerMapping_t))))
     {
        int32_t mapIdx = tf_getLayreTypeMapIdx("TIDL_LayernormGamma", TIDL_OnnxLayerMap, (sizeof(TIDL_OnnxLayerMap) / sizeof(TIDL_TFLayerMapping_t)));
        sTIDL_LayerPC_t &TIDLLayerNormLayer = pOrgTIDLNetStructure.TIDLPCLayers[i1];
        int32_t eltwiseMulLayerIdx = tidl_getOutLayer(pOrgTIDLNetStructure, pOrgTIDLNetStructure.numLayers, pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0].dataId);
        sTIDL_LayerPC_t &eltwiseMulLayer = pOrgTIDLNetStructure.TIDLPCLayers[eltwiseMulLayerIdx];
        int32_t eltwiseAddLayerIdx = tidl_getOutLayer(pOrgTIDLNetStructure, pOrgTIDLNetStructure.numLayers, pOrgTIDLNetStructure.TIDLPCLayers[eltwiseMulLayerIdx].outData[0].dataId);
        sTIDL_LayerPC_t &eltwiseAddLayer = pOrgTIDLNetStructure.TIDLPCLayers[eltwiseAddLayerIdx];
        if(!(eltwiseMulLayer.layerParams.eltWiseParams.eltWiseType == TIDL_EltWiseProduct && eltwiseAddLayer.layerParams.eltWiseParams.eltWiseType == TIDL_EltWiseSum && tidl_is1DBroadCastTensor(eltwiseMulLayer) && tidl_is1DBroadCastTensor(eltwiseAddLayer)))
        {
          /*Non-Fusable pattern*/
          continue;
        }
        else if(TIDLLayerNormLayer.layerParams.layerNormParams.axis != TIDL_DIM_WIDTH)
        {
          /*Only widthwise layernorm is currently fused*/
          continue;
        }
        else
        {
          /*Layernorm Beta and Gamma Fusion*/
          int32_t layerNormConsumerIdx = tidl_getOutLayer(pOrgTIDLNetStructure, pOrgTIDLNetStructure.numLayers, pOrgTIDLNetStructure.TIDLPCLayers[eltwiseAddLayerIdx].outData[0].dataId);
          sTIDL_LayerPC_t &layerNormConsumer = pOrgTIDLNetStructure.TIDLPCLayers[layerNormConsumerIdx];
          /*Case one: MatMul exists ahead (Projection)*/
          /*Weights (Part of const layer) are first scaled (Per column)*/
          /*Get the const weight buffer (Gamma) from EltwiseMul [It has been already checked that this exists]*/
          int32_t gammaIdx = tidl_getInLayer (pOrgTIDLNetStructure, eltwiseMulLayerIdx, eltwiseMulLayer.inData[eltwiseMulLayer.allowlistingMetaData.constTensorIndices[0]].dataId);
          float* pGamma = (float*) pOrgTIDLNetStructure.TIDLPCLayers[gammaIdx].weights.ptr;
          /*Get Beta*/
          int32_t betaIdx = tidl_getInLayer (pOrgTIDLNetStructure, eltwiseAddLayerIdx, eltwiseAddLayer.inData[eltwiseAddLayer.allowlistingMetaData.constTensorIndices[0]].dataId);
          float* pBeta = (float*) pOrgTIDLNetStructure.TIDLPCLayers[betaIdx].weights.ptr;
          if( (eltwiseAddLayer.outConsumerCnt[0] == 1U) && layerNormConsumer.layerParams.innerProductParams.constIdx == 1U && (layerNormConsumer.layerParams.innerProductParams.numInCols == layerNormConsumer.layerParams.innerProductParams.numOutCols)&& layerNormConsumer.layerType == TIDL_InnerProductLayer)
          {
            /*Get the innerproduct weights pointer:*/
            int32_t matmulWeightsIdx = tidl_getInLayer (pOrgTIDLNetStructure, layerNormConsumerIdx, layerNormConsumer.inData[1].dataId);
            assert(pOrgTIDLNetStructure.TIDLPCLayers[matmulWeightsIdx].layerType == TIDL_ConstDataLayer);
            /*FC Weights & biases*/
            float* pWeights = (float*) pOrgTIDLNetStructure.TIDLPCLayers[matmulWeightsIdx].weights.ptr;
            float* pBias = (float*) layerNormConsumer.bias.ptr;
            //assert(layerNormConsumer.bias.bufSize == layerNormConsumer.layerParams.innerProductParams.numOutCols);           
            int32_t i2, i3;
            int32_t numInCols = layerNormConsumer.layerParams.innerProductParams.numInCols;
            int32_t numOutCols = layerNormConsumer.layerParams.innerProductParams.numOutCols;
            /*Update bias by adding broadCastAdd x W*/
            for(i3 = 0; i3 < numOutCols; i3++)
            {
              for(i2 = 0; i2 < numInCols; i2++)
              {
                pBias[i3] += pWeights[i2*numOutCols + i3] * pBeta[i2];
              }
            }
            free(pBeta);
            /*Weights are numInCols X numOutCols & Gamme is numOutCols wide*/
            for(i2 = 0; i2 < numInCols; i2++)
            {
              for(i3 = 0; i3 < numOutCols; i3++)
              {
                pWeights[i2*numOutCols + i3] *= pGamma[i2];
              }
            }
            free(pGamma);
            /*Update link from Layernorm -> Layernormconsumer*/
            layerNormConsumer.inData[0] = TIDLLayerNormLayer.outData[0];
            strcpy((char*)layerNormConsumer.inDataNames[0],(const char*)TIDLLayerNormLayer.outDataNames[0]);
          }
          else
          {
            /*Replace Gamma (eltwise mul) & Beta (eltwise add) with an innerproduct layer:*/
            /*Add an innerproduct layer:*/
            TIDL_addLayer(pOrgTIDLNetStructure, i1, TIDL_InnerProductLayer, dataIndex, pOrgTIDLNetStructure.numLayers, ADD_LAYER_AFTER);
            sTIDL_LayerPC_t& ip       = pOrgTIDLNetStructure.TIDLPCLayers[pOrgTIDLNetStructure.numLayers-1];

            /**
             * Setup Inner Product Layer
            */
            int32_t numInCols = TIDLLayerNormLayer.outData[0].dimValues[TIDL_DIM_WIDTH];
            ip.numInBufs = 2;
            ip.layerParams.innerProductParams.numInRows  = TIDLLayerNormLayer.outData[0].dimValues[TIDL_DIM_HEIGHT];
            ip.layerParams.innerProductParams.numOutCols = numInCols;
            ip.layerParams.innerProductParams.numInCols  = numInCols;
            ip.layerParams.innerProductParams.isBias     = 1;
            ip.layerParams.innerProductParams.inputBTranspose = 0;
            ip.layerParams.innerProductParams.constIdx = 1; /*"B" is the constant buffer*/
            int32_t bufSize = numInCols * numInCols;
            int32_t biasSize = numInCols; /*numOutCols == numInCols*/
            float *weightsPtr = (float*)malloc(sizeof(float)*bufSize);
            int32_t i2, i3;
            for(i2 = 0; i2 < numInCols; i2++)
            {
              for(i3 = 0; i3 < numInCols; i3++)
              {
                if(i2 == i3)
                {
                  /*Diagonal elements are populated:*/
                  weightsPtr[i2*numInCols + i3] = pGamma[i2];
                }
                else
                {
                  weightsPtr[i2*numInCols + i3] = 0.0;
                }
              }
            }
            free(pGamma);
            /*Create a const buffer for weights:*/
            int32_t constIdx = tidl_createConstDataLayer (pOrgTIDLNetStructure, dataIndex, weightsPtr, bufSize, pOrgTIDLNetStructure.numLayers);
            sTIDL_LayerPC_t& constLayer = pOrgTIDLNetStructure.TIDLPCLayers[constIdx];
            constLayer.outData[0].dimValues[TIDL_DIM_BATCH]  = 1;
            constLayer.outData[0].dimValues[TIDL_DIM_DIM1]   = 1;
            constLayer.outData[0].dimValues[TIDL_DIM_DIM2]   = 1;
            constLayer.outData[0].dimValues[TIDL_DIM_NUMCH]  = 1;
            constLayer.outData[0].dimValues[TIDL_DIM_HEIGHT] = numInCols;
            constLayer.outData[0].dimValues[TIDL_DIM_WIDTH]  = numInCols;
            /*Populate the bias buffer:*/
            ip.bias.bufSize = biasSize;
            float * pBias = (float*) malloc(sizeof(float)*biasSize);
            ip.bias.ptr = (void*) pBias;
            /*Populate bias:*/
            if(ip.bias.ptr != NULL)
            {
              for(i2 = 0; i2 < numInCols; i2++)
              {
                pBias[i2] = pBeta[i2];
              }
              free(pBeta);
            }
            else
            {
              printf("Error populating layernorm innerproduct bias\n");
              exit(-1);
            }
            /*Create network links*/
            /*Populate const input for innerproduct:*/
            ip.inData[1] = constLayer.outData[0];
            strcpy((char*)ip.inDataNames[1], (char*)constLayer.outDataNames[0]);
            /*Populate the regular input to innerproduct:*/
            ip.inData[0] = TIDLLayerNormLayer.outData[0];
            strcpy((char*)ip.inDataNames[0], (char*)TIDLLayerNormLayer.outDataNames[0]);
            /*Link output*/
            ip.outData[0] = eltwiseAddLayer.outData[0];
            strcpy((char*)ip.outDataNames[0], (char*)eltwiseAddLayer.outDataNames[0]);
            ip.outConsumerCnt[0] = eltwiseAddLayer.outConsumerCnt[0];
          }
          /*Remove eltwise add and eltwise mul layers (& corresponding const buffers)*/
          eltwiseAddLayer.numInBufs  = -1;
          eltwiseAddLayer.numOutBufs = -1;
          eltwiseMulLayer.numInBufs  = -1;
          eltwiseMulLayer.numOutBufs = -1;
          /*Eltwise Add const:*/
          pOrgTIDLNetStructure.TIDLPCLayers[betaIdx].numInBufs  = -1;
          pOrgTIDLNetStructure.TIDLPCLayers[betaIdx].numOutBufs = -1;
          /*Eltwise Mul Const:*/
          pOrgTIDLNetStructure.TIDLPCLayers[gammaIdx].numInBufs  = -1;
          pOrgTIDLNetStructure.TIDLPCLayers[gammaIdx].numOutBufs = -1;
        }
     }
  }
  return 0;
}

int32_t tidl_FindMatMulBias (sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex)
{
  int32_t i1, i2, i3, i4;
  int32_t status = 0;
  int32_t outLayerIdx = -1;

  /**
   * Do not fuse bias for TDA4VM
  */
  if (gParams.deviceName != TIDL_TDA4VM)
  {
    for (i1 = 0; i1 < layerIndex; i1++)
    {
      if (tf_isLayerType("TIDL_MatMulBias", i1, pOrgTIDLNetStructure, TIDL_OnnxLayerMap, (sizeof(TIDL_OnnxLayerMap) / sizeof(TIDL_TFLayerMapping_t))))
      {
        int32_t mapIdx = tf_getLayreTypeMapIdx("TIDL_MatMulBias", TIDL_OnnxLayerMap, (sizeof(TIDL_OnnxLayerMap) / sizeof(TIDL_TFLayerMapping_t)));

        sTIDL_LayerPC_t &TIDLPCLayers1 = pOrgTIDLNetStructure.TIDLPCLayers[i1];
        int32_t nextlayerIdx = tidl_getOutLayer(pOrgTIDLNetStructure, pOrgTIDLNetStructure.numLayers, pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0].dataId);
        sTIDL_LayerPC_t &TIDLPCLayers2 = pOrgTIDLNetStructure.TIDLPCLayers[nextlayerIdx];
        /** Check if the eltwise is an add layer and identify if it is a bias to the innerproduct layer*/

        if (TIDLPCLayers2.layerParams.eltWiseParams.eltWiseType == TIDL_EltWiseSum &&
            TIDLPCLayers2.allowlistingMetaData.numConstInputs == 1 &&
            TIDLPCLayers2.allowlistingMetaData.constTensorsDims[0].size() == 1 &&
            TIDLPCLayers1.outData[0].dimValues[TIDL_DIM_NUMCH] == 1 &&
            TIDLPCLayers1.outData[0].dimValues[TIDL_DIM_DIM2] == 1  &&
            TIDLPCLayers1.outData[0].dimValues[TIDL_DIM_DIM1] == 1 )
        {
          if (TIDLPCLayers2.allowlistingMetaData.constTensorsDims[0][0] == TIDLPCLayers1.layerParams.innerProductParams.numInRows ||
              TIDLPCLayers2.allowlistingMetaData.constTensorsDims[0][0] == TIDLPCLayers1.layerParams.innerProductParams.numOutCols &&
              (TIDL_doesLayerSupportAsymTensors(&TIDLPCLayers1)))
          {
            int32_t constIdx = tidl_getInLayer (pOrgTIDLNetStructure, layerIndex, TIDLPCLayers2.inData[0].dataId);
            if (pOrgTIDLNetStructure.TIDLPCLayers[constIdx].layerType != TIDL_ConstDataLayer)
            {
              constIdx = tidl_getInLayer (pOrgTIDLNetStructure, layerIndex, TIDLPCLayers2.inData[1].dataId);
            }
            float* bias = (float*)malloc (sizeof (float) * TIDLPCLayers1.layerParams.innerProductParams.numInRows * TIDLPCLayers1.layerParams.innerProductParams.numOutCols);
            float* src_bias = (float*) pOrgTIDLNetStructure.TIDLPCLayers[constIdx].weights.ptr;
            if (pOrgTIDLNetStructure.TIDLPCLayers[constIdx].weights.bufSize != TIDLPCLayers1.layerParams.innerProductParams.numOutCols * TIDLPCLayers1.layerParams.innerProductParams.numInRows)
            {
              int32_t offset = 0;
              if (TIDLPCLayers2.allowlistingMetaData.constTensorsDims[0][0] == TIDLPCLayers1.layerParams.innerProductParams.numOutCols)
              {
                /** Broadcast the bias column wise*/
                for (int32_t i = 0; i < TIDLPCLayers1.layerParams.innerProductParams.numInRows; i++)
                {
                  for (int32_t j = 0; j < TIDLPCLayers1.layerParams.innerProductParams.numOutCols; j++)
                  {
                    bias[offset++] = src_bias[j];
                  }
                }
                free (src_bias);
              }
              else if (TIDLPCLayers2.allowlistingMetaData.constTensorsDims[0][0] == TIDLPCLayers1.layerParams.innerProductParams.numInRows)
              {
                /** Broadcast the bias row wise*/
                for (int32_t i = 0; i < TIDLPCLayers1.layerParams.innerProductParams.numInRows; i++)
                {
                  for (int32_t j = 0; j < TIDLPCLayers1.layerParams.innerProductParams.numOutCols; j++)
                  {
                    bias[offset++] = src_bias[i];
                  }
                }
                free (src_bias);
              }
              TIDLPCLayers1.layerParams.innerProductParams.isBias = 1;
              TIDLPCLayers1.bias.ptr = bias;
              TIDLPCLayers1.bias.bufSize = TIDLPCLayers1.layerParams.innerProductParams.numInRows * TIDLPCLayers1.layerParams.innerProductParams.numOutCols;
              TIDLPCLayers1.actParams = TIDLPCLayers2.actParams;
              if((TIDLPCLayers1.actParams.actType == TIDL_RelU6) || (TIDLPCLayers1.actParams.actType == TIDL_RelU)||
                ((TIDLPCLayers1.actParams.actType == TIDL_Clip) && (TIDLPCLayers1.actParams.clipMin >= 0)))
              {
                TIDLPCLayers1.outData[0].elementType = tidl_getElementType(0);
              }
              outLayerIdx = tidl_getOutLayer (pOrgTIDLNetStructure, layerIndex, TIDLPCLayers2.outData[0].dataId);
              /** Remove the Eltwise & ConstData layers*/
              do
              {
                for (int32_t i = 0; i < pOrgTIDLNetStructure.TIDLPCLayers[outLayerIdx].numInBufs; i++)
                {
                  if (pOrgTIDLNetStructure.TIDLPCLayers[outLayerIdx].inData[i].dataId == TIDLPCLayers2.outData[0].dataId)
                  {
                    pOrgTIDLNetStructure.TIDLPCLayers[outLayerIdx].inData[i].dataId = TIDLPCLayers1.outData[0].dataId;
                    strcpy ((char*)TIDLPCLayers1.outDataNames[0], (char*)TIDLPCLayers2.outDataNames[0]);
                    strcpy ((char*)pOrgTIDLNetStructure.TIDLPCLayers[outLayerIdx].inDataNames[i], (char*)TIDLPCLayers1.outDataNames[0]);
                  }
                }
                outLayerIdx = tidl_getOutLayer (pOrgTIDLNetStructure, layerIndex, TIDLPCLayers2.outData[0].dataId);
              } while (outLayerIdx != -1);

              TIDLPCLayers2.numInBufs  = -1;
              TIDLPCLayers2.numOutBufs = -1;
              pOrgTIDLNetStructure.TIDLPCLayers[constIdx].numInBufs  = -1;
              pOrgTIDLNetStructure.TIDLPCLayers[constIdx].numOutBufs = -1;
            }
          }
        }
      }
    }
  }

  return status;
}


int32_t tidl_FindOnnxShuffleLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex)
{
  int32_t i1, i2, i3, i4;
  int32_t status = 0;
  for (i1 = 0; i1 < layerIndex; i1++)
  {
    if (tf_isLayerType("TIDL_OnnxShuffle", i1, pOrgTIDLNetStructure, TIDL_OnnxLayerMap, (sizeof(TIDL_OnnxLayerMap) / sizeof(TIDL_TFLayerMapping_t))))
    {
      int32_t mapIdx = tf_getLayreTypeMapIdx("TIDL_OnnxShuffle", TIDL_OnnxLayerMap, (sizeof(TIDL_OnnxLayerMap) / sizeof(TIDL_TFLayerMapping_t)));

      sTIDL_LayerPC_t &TIDLPCLayers1 = pOrgTIDLNetStructure.TIDLPCLayers[i1];
      int32_t nextlayerIdx = tidl_getOutLayer(pOrgTIDLNetStructure, pOrgTIDLNetStructure.numLayers, pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0].dataId);
      sTIDL_LayerPC_t &TIDLPCLayers2 = pOrgTIDLNetStructure.TIDLPCLayers[nextlayerIdx];
      nextlayerIdx = tidl_getOutLayer(pOrgTIDLNetStructure, pOrgTIDLNetStructure.numLayers, pOrgTIDLNetStructure.TIDLPCLayers[nextlayerIdx].outData[0].dataId);
      sTIDL_LayerPC_t &TIDLPCLayers3 = pOrgTIDLNetStructure.TIDLPCLayers[nextlayerIdx];

      if ((TIDLPCLayers1.weights.bufSize ==  5) && (TIDLPCLayers2.weights.bufSize ==  5) && (TIDLPCLayers3.weights.bufSize == 4))
      {
        int64_t * reshape1  = (int64_t *)TIDLPCLayers1.weights.ptr;
        int64_t * reshape2  = (int64_t *)TIDLPCLayers3.weights.ptr;
        int32_t * transpose = (int32_t *)TIDLPCLayers2.weights.ptr;
        if ((reshape1[4] == reshape2[3]) && (reshape1[3] == reshape2[2]) && (4 == transpose[4]) && (3 == transpose[3]) && (1 == transpose[2]) && (2 == transpose[1]))
        {
          pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0] = TIDLPCLayers3.outData[0];
          strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[i1].outDataNames[0], (char *)TIDLPCLayers3.outDataNames[0]);
          pOrgTIDLNetStructure.TIDLPCLayers[i1].outConsumerCnt[0] = TIDLPCLayers3.outConsumerCnt[0];

          TIDLPCLayers2.numInBufs = -1;
          TIDLPCLayers2.numOutBufs = -1;
          TIDLPCLayers3.numInBufs = -1;
          TIDLPCLayers3.numOutBufs = -1;

          TIDLPCLayers1.layerType = TIDL_ShuffleChannelLayer;
          TIDLPCLayers1.layerParams.shuffleLayerParams.numGroups = reshape1[1];
        }
      }

      if ((TIDLPCLayers1.weights.bufSize ==  6) && (TIDLPCLayers2.weights.bufSize ==  6) && (TIDLPCLayers3.weights.bufSize == 4))
      {
        int64_t * reshape1  = (int64_t *)TIDLPCLayers1.weights.ptr;
        int64_t * reshape2  = (int64_t *)TIDLPCLayers3.weights.ptr;
        int32_t * transpose = (int32_t *)TIDLPCLayers2.weights.ptr;
        if ((reshape1[4]*reshape1[2] == reshape2[2]) && (reshape1[3]*reshape1[5] == reshape2[3]) && (reshape1[1] == reshape2[1]) && (reshape1[2] == reshape1[3]) &&
            (3 == transpose[5]) && (5 == transpose[4]) && (2 == transpose[3]) && (4 == transpose[2]) && (1 == transpose[1]))
        {
          pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0] = TIDLPCLayers3.outData[0];
          strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[i1].outDataNames[0], (char *)TIDLPCLayers3.outDataNames[0]);
          pOrgTIDLNetStructure.TIDLPCLayers[i1].outConsumerCnt[0] = TIDLPCLayers3.outConsumerCnt[0];

          TIDLPCLayers2.numInBufs = -1;
          TIDLPCLayers2.numOutBufs = -1;
          TIDLPCLayers3.numInBufs = -1;
          TIDLPCLayers3.numOutBufs = -1;

          TIDLPCLayers1.layerType = TIDL_DepthToSpaceLayer;

          if((gParams.deviceName & ~TIDL_OTF_FLAG_BIT) == TIDL_AM62A)
          {
            printf("ERROR: Device AM62A currently does not support Pixel Shuffle Layer\n");
            exit(-1);
          }
          TIDLPCLayers1.layerParams.depthToSpaceParams.blockSize = reshape1[2];
        }
      }
    }
  }
  return 0;
}

int32_t tidl_identifyPatchEmbeddingBlock(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex)
{
  int32_t i1, i2, i3, i4;
  int32_t status = 0;

  /**
   * Identify patch embedding block consisting of convolution layer
   * Convolution parameters - kernel size (x,y) and stride size (a,b) should match
  */
  if (tf_isLayerType("TIDL_PatchEmbedding", layerIndex, pOrgTIDLNetStructure, TIDL_OnnxLayerMap, (sizeof(TIDL_OnnxLayerMap) / sizeof(TIDL_TFLayerMapping_t))))
  {
    sTIDL_LayerPC_t &TIDLPCLayers1 = pOrgTIDLNetStructure.TIDLPCLayers[layerIndex];
    sTIDL_ConvParams_t* params = &TIDLPCLayers1.layerParams.convParams;
    if (params->kernelH == params->strideH &&
        params->kernelW == params->strideW &&
        params->kernelH == params->kernelW &&
        params->kernelH > 3)
        {
          /**
           * Identified a patch embedding block
          */
          status = 1;
        }
  }

  return status;
}

int32_t tidl_isConstantUsedAsInData(int32_t layerType)
{
  /*
  Below are the layer for which constant/initializer can be input data to TIDL node
  For these layers constant/initializer names also can be  used for inData names
  */
  if((layerType == TIDL_InnerProductLayer) ||
    (layerType == TIDL_EltWiseLayer) ||
    (layerType == TIDL_ScatterElementsLayer) ||
    (layerType == TIDL_ConcatLayer) ||
    (layerType == TIDL_GatherLayer)
  )
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

int32_t tidl_doesTIDLchangenumInputs (sTIDL_LayerPC_t &layer)
{
  /** This function identifies layers where TIDL could potentially change numInputs from what is
   * expected in the model
  */
  int32_t status = 0;

  if (layer.layerType == TIDL_BatchNormLayer)
  {
    /** Eltwise Add/Mul with const Input (1D) gets converted to BatchNorm Layer
     * Model has numInputs 2 for this layer, but batchnorm has only one input in TIDL
    */
    status = 1;
  }

  return status;
}

int32_t tidl_updateInDataNameForInputChangingLayers (sTIDL_LayerPC_t &layer,
  int32_t              i,
  GraphProto&             onnxGraph)
{
  /** Eltwise Add/Mul can be converted to BatchNorm Layer that has only one input
   * If the Add/Mul first input is const input, inDataName for BatchNorm Layer is not filled
   * Check if the first input is const, and fill the second input name in BatchNorm inDataName
  */

  if (tidl_doesTIDLchangenumInputs (layer) == 1)
  {
    if (layer.allowlistingMetaData.numInputs == 2 &&
        layer.allowlistingMetaData.numVarInputs == 1 &&
        layer.allowlistingMetaData.numConstInputs == 1 &&
        layer.allowlistingMetaData.constTensorIndices[0] == 0 &&
        layer.allowlistingMetaData.varTensorIndices[0] == 1)
    {
      strcpy((char*)layer.inDataNames[0], onnxGraph.node(i).input(1).c_str());
    }
  }

  return 0;
}

int32_t tidl_onnxLayerFillTensorNames(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure,
  int32_t              i,
  int32_t              layerIndex,
  GraphProto&             onnxGraph)
{
  int32_t j;
  if(strlen(onnxGraph.node(i).name().c_str()) < TIDL_STRING_SIZE)
  {
    strcpy((char*)pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].name, onnxGraph.node(i).name().c_str());
  }
  else
  {
    printf("\n*** Tensor name exceeds %d characters, exiting ! *** \nTensor name --- \n %s \n", TIDL_STRING_SIZE, onnxGraph.node(i).name().c_str());
    exit(-1);
  }

  if (pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].numInBufs > 0)
  {
    int k=0;
    for (j = 0; j < pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].numInBufs; j++)
    {
      if(strlen(onnxGraph.node(i).input(j).c_str()) < TIDL_STRING_SIZE)
      {
          /* If constant/initializer is used as input to TIDL node/layer then use constant name itself for inData name
             If constant/initializer is not used as input to TIDL node/layer then dont use constant name for inData name.
          */
        while( ((1 == TIDL_isInputConstInGraph(onnxGraph, onnxGraph.node(i).input(j+k).c_str())) && (tidl_isConstantUsedAsInData(pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].layerType)==0)) ||
                TIDL_doesDequantHaveConstTensor(onnxGraph, onnxGraph.node(i).input(j+k)) )
        {
          k++;
          if(j+k >= pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].numInBufs)
          {
            break;
          }
        }
        if((j+k) < pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].numInBufs)
        {
          strcpy((char*)pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].inDataNames[j], onnxGraph.node(i).input(j+k).c_str());
        }

        tidl_updateInDataNameForInputChangingLayers (pOrgTIDLNetStructure->TIDLPCLayers[layerIndex], i, onnxGraph);
      }
      else
      {
        printf("\n*** Tensor name exceeds %d characters, exiting ! *** \nTensor name --- \n %s \n", TIDL_STRING_SIZE, onnxGraph.node(i).input(j).c_str());
        exit(-1);
      }
      pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].inData[j].dataId = -1;
    }
  }
  if (pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].numOutBufs > 0)
  {
    for (j = 0; j < pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].numOutBufs; j++)
    {
      if(strlen(onnxGraph.node(i).output(j).c_str()) < TIDL_STRING_SIZE)
      {
        strcpy((char*)pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outDataNames[j], onnxGraph.node(i).output(j).c_str());
      }
      else
      {
        printf("\n*** Tensor name exceeds %d characters, exiting ! *** \nTensor name --- \n %s \n", TIDL_STRING_SIZE, onnxGraph.node(i).output(j).c_str());
        exit(-1);
      }
      pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outConsumerLinked[j] = 0;
    }
  }
  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].weightsElementSizeInBits = NUM_WHGT_BITS;

  return 0;
}

int32_t tidl_onnxLayerUpdateConsumerCount(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure,
  int32_t              i,
  int32_t              layerIndex,
  GraphProto&             onnxGraph)
{
  int32_t i0, i1, i2;
  int32_t numCons = 0;
  for (i0 = 0; i0 < pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].numOutBufs; i0++)
  {

    for (i1 = 0; i1 < onnxGraph.node_size(); i1++)
    {
      for (i2 = 0; i2 < onnxGraph.node(i1).input_size(); i2++)
      {
        if (strcmp((const char *)pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outDataNames[i0], onnxGraph.node(i1).input(i2).c_str()) == 0)
        {
          numCons++;
        }
      }
    }
    for (i2 = 0; i2 < numNetOutData; i2++)
    {
      if (strcmp((const char *)pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outDataNames[i0], outDataNames[i2]) == 0)
      {
        numCons++;
      }
    }
    pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outConsumerCnt[i0] = numCons;
  }
  return 0;
}








int32_t tidl_onnxGetNewNodeToAdd(sTIDL_OrgNetwork_t   &orgTIDLNetStructure,
  int32_t              layerIndex,
  GraphProto&             onnxGraph)
{
  int32_t i, j, nodeIdx = -1;

  for (i = 0; i < layerIndex; i++)
  {
    for (j = 0; j < orgTIDLNetStructure.TIDLPCLayers[i].numInBufs; j++)
    {
      if (TIDL_getLayerIdx(&orgTIDLNetStructure, layerIndex, (const char *)orgTIDLNetStructure.TIDLPCLayers[i].inDataNames[j]) == -1)
      {
        nodeIdx = TIDL_onnxGetNodeIdx(onnxGraph, (const char *)orgTIDLNetStructure.TIDLPCLayers[i].inDataNames[j]);
        if (nodeIdx != -1)
        {
          if (strcmp(onnxGraph.node(nodeIdx).op_type().c_str(), "Constant") == 0)
          {
            nodeIdx = -1;
          }
        }
        if (nodeIdx != -1)
        {
          break;
        }
      }
    }
    if (nodeIdx != -1)
    {
      break;
    }
  }
  return nodeIdx;
}

int32_t tidl_findOnnxOutputNames(GraphProto&             onnxGraph, char * outList)
{
  int i, j, k, l;
  char tensorName[TIDL_STRING_SIZE];
  char inTensorName[TIDL_STRING_SIZE];
  int outPutSize = 0;

  if(onnxGraph.output_size() > 0)
  {
    for (i = 0; i < onnxGraph.output_size(); i++)
    {
      strcat(outList, onnxGraph.output(i).name().c_str());
      strcat(outList, ",");
    }
  }
  else
  {
    for (i = 0; i < onnxGraph.node_size(); i++)
    {
      outPutSize = onnxGraph.node(i).output_size();
      if (strcmp(onnxGraph.node(i).op_type().c_str(), "Dropout") == 0)
      {
        outPutSize = 1;
      }
      for (j = 0; j < outPutSize; j++)
      {
        int outDataUsed = 0;
        strncpy((char *)tensorName, onnxGraph.node(i).output(j).c_str(), TIDL_STRING_SIZE);
        for (k = 0; k < onnxGraph.node_size(); k++)
        {
          for (l = 0; l < onnxGraph.node(k).input_size(); l++)
          {
            strncpy((char *)inTensorName, onnxGraph.node(k).input(l).c_str(), TIDL_STRING_SIZE);
            if (strcmp(tensorName, inTensorName) == 0)
            {
              outDataUsed = 1;
              break;
            }
          }
          if (outDataUsed)
            break;
        }
        if (outDataUsed == 0)
        {
          strcat(outList, tensorName);
          strcat(outList, ",");
        }
      }
    }
  }
  return (0);
}

void onnx_import(tidl_import_config * params, int32_t *totalData, int32_t* totalLayers)
{
  int32_t                    i,j;
  int32_t                    layerNum;
  int32_t                    inputSize;
  int32_t                    pad,stride;
  int32_t                    layerIndex;
  int32_t                    tiLayerIndex;
  int32_t                    dataIndex;
  const uint8_t             *name;
  const uint8_t             *inputName[10];
  const uint8_t             *outputName;
  ModelProto         onnxModel;
  int32_t status = 0;
  int32_t                    dataSize;
  int32_t                    id;
  int paramSet  = 0;
  int conv2DRandParams = 0;
  string attrKey;
  int32_t inLayerId = 0;
  int32_t weightsElementSizeInBits;
  int32_t mapTblIdx = -1;


  string key = "value";

  printf("ONNX Model (Proto) File  : %s  \n",(const char *)params->inputNetFile);
  printf("TIDL Network File      : %s  \n", (const char *)params->outputNetFile);
  printf("TIDL IO Info File      : %s  \n", (const char *)params->outputParamsFile);

  TIDL_readProtoFromBinaryFile((const char *)params->inputNetFile, (google::protobuf::Message*)&onnxModel);
  GraphProto onnxGraph = onnxModel.graph();
  auto & opset_import = onnxModel.opset_import(0);
  onnxOpSetVersion =  opset_import.version();
  printf("Current ONNX OpSet Version   : %d  \n", onnxOpSetVersion);
  if (onnxOpSetVersion > HIGHEST_OPSET_VERSION_SUPPORTED)
  {
    printf("WARNING: TIDL-RT only supports operators upto OPSET %d\nUsing operators from a higher OPSET may result in undefined behavior and/or errors\n", HIGHEST_OPSET_VERSION_SUPPORTED);
  }

  layerIndex = 0;
  dataIndex  = 0;


  if (strcmp((char*)params->outDataNamesList, "") == 0)
  {
    char outDataNamesList[TIDL_MAX_ALG_OUT_BUFS * TIDL_STRING_SIZE] = "";
    tidl_findOnnxOutputNames(onnxGraph, (char*)outDataNamesList);
    numNetOutData = tidl_getStringsFromList((char *)outDataNamesList, (char*)outDataNames, TIDL_MAX_DATA_NAME);
  }
  else
  {
    numNetOutData = tidl_getStringsFromList((char *)params->outDataNamesList, (char*)outDataNames, TIDL_MAX_DATA_NAME);
  }
  for (i = 0; i < numNetOutData; i++)
  {
    orgTIDLNetStructure.TIDLPCLayers[layerIndex].numInBufs = 1;
    orgTIDLNetStructure.TIDLPCLayers[layerIndex].numOutBufs = -1;
    strcpy((char*)orgTIDLNetStructure.TIDLPCLayers[layerIndex].name, outDataNames[i]);
    strcpy((char *)orgTIDLNetStructure.TIDLPCLayers[layerIndex].inDataNames[0], outDataNames[i]);
    strcpy((char *)orgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], outDataNames[i]);
    layerIndex++;
  }

  if (strcmp((char *)params->inDataNamesList, "") != 0)
  {
    numNetInData = tidl_getStringsFromList((char *)params->inDataNamesList, (char *)inDataNames, TIDL_MAX_DATA_NAME);
    for (i = 0; i < numNetInData; i++)
    {
      orgTIDLNetStructure.TIDLPCLayers[layerIndex].numInBufs = -1;
      orgTIDLNetStructure.TIDLPCLayers[layerIndex].numOutBufs = 1;
      strcpy((char *)orgTIDLNetStructure.TIDLPCLayers[layerIndex].name, inDataNames[i]);
      strcpy((char *)orgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], inDataNames[i]);
      orgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerCnt[0] = 1;
      orgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerLinked[0] = 0;
      orgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dataId = dataIndex++;
      orgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].numDim = TIDL_DIM_MAX;
      tidl_onnxLayerUpdateConsumerCount(&orgTIDLNetStructure, layerIndex, layerIndex, onnxGraph);
      tidl_linkOutputTensors(&orgTIDLNetStructure, layerIndex);
      layerIndex++;
    }
  }

  int newNode = tidl_onnxGetNewNodeToAdd(orgTIDLNetStructure, layerIndex, onnxGraph);
  while (newNode != -1)
  {
    orgTIDLNetStructure.TIDLPCLayers[layerIndex].numInBufs = 1;
    orgTIDLNetStructure.TIDLPCLayers[layerIndex].numOutBufs = 1;
    orgTIDLNetStructure.TIDLPCLayers[layerIndex].inData[0].dataId = -1;
    orgTIDLNetStructure.TIDLPCLayers[layerIndex].actParams.actType = TIDL_NoAct;

    mapTblIdx = TIDL_getOnnxOpParamMapId(onnxGraph.node(newNode).op_type().c_str());
    if (mapTblIdx == -1)
    {
      printf(" ONNX operator %s is not suported now..  By passing\n", onnxGraph.node(newNode).op_type().c_str());
      TIDL_tfMapUnSuportedlayerParams(&orgTIDLNetStructure, newNode, layerIndex, &dataIndex, onnxGraph);
    }
    else
    {
      status = tidl_onnxOpParamMapTable[mapTblIdx].tidl_onnxMapFunc(&orgTIDLNetStructure, newNode, layerIndex, &dataIndex, onnxGraph);
    }

    if(status == -1)
    {
      for(auto &msg : diagsInfo)
      {
        if(! msg.empty())
          printf("%s  \n", msg.c_str());
      }
      exit(-1);
    }

    if (params->enableCustomLayers)
    {
      TIDL_MapCustomParams(&orgTIDLNetStructure,
                            newNode,
                            layerIndex,
                            &dataIndex,
                            &onnxGraph,
                            NULL,
                            TIDL_IMPORT_MODEL_FORMAT_ONNX);
    }

    tidl_onnxLayerFillTensorNames(&orgTIDLNetStructure, newNode, layerIndex, onnxGraph);
    tidl_onnxLayerUpdateConsumerCount(&orgTIDLNetStructure, newNode, layerIndex, onnxGraph);
    tidl_linkInputTensors(&orgTIDLNetStructure, layerIndex);
    tidl_linkOutputTensors(&orgTIDLNetStructure, layerIndex);
    layerIndex++;

    newNode = tidl_onnxGetNewNodeToAdd(orgTIDLNetStructure, layerIndex, onnxGraph);
  }
  *totalData = dataIndex;
  *totalLayers = layerIndex;
}