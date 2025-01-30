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

//! @file tidl_relayImport.cpp

// Standard header files
#include <stdint.h>
#include <stdio.h>
#include <float.h>

// TVM header files
#include <tvm/relay/expr.h>
#include <tvm/relay/attrs/nn.h>
#include <tvm/relay/attrs/image.h>
#include <tvm/relay/attrs/transform.h>
#include <tvm/relay/attrs/reduce.h>
#include <tvm/runtime/ndarray.h>
#include <tvm/runtime/registry.h>
#include <tvm/runtime/ndarray.h>
#include <tvm/tir/op.h>

// TIDL header files
#include <tidl_import_config.h>
#include <tidl_import_common.h>
#include <tidl_import_common_model_check.h>
#include <tidl_import_api.h>
#include <tidl_runtimes_import_common.h>
#include "tidl_meta_arch.pb.h"

using namespace tvm;
using namespace tvm::relay;
extern int32_t gloab_data_format;
extern uint8_t tidl_OdOutputNames[TIDL_MAX_TF_SSD_LAYERS][TIDL_STRING_SIZE];

static int tidl_relay_debuglevel = 0;
#define RelayDebugPrint(...)  if (tidl_relay_debuglevel > 0) \
                                printf("RelayImportDebug: " __VA_ARGS__)
static int tidl_relay_max_batch_size = 1;

#define MAX_NAME_LEN 16

// Global data structures for importing
static struct relayImportState_t {
  int32_t                 dataIndex;
  int32_t                 layerIndex;
  int32_t                 numErrors;
  int32_t                 numUnsupportedLayers;
  int32_t                 numInputDataLayers;
} relay_import_state;

/* Same data structure defined in python/tvm/relay/backend/contrib/tidl.py
*  so that it can be passed from python to C/C++
*/
typedef struct InOutNodes
{
  char *this_node;
  int   num_in_nodes;
  int   num_out_nodes;
  void *in_nodes;
  void *out_nodes;
} InOutNodes_t;

typedef struct TensorDescriptor
{
  double scale;
  int32_t zp;
  int32_t element_type;
  int32_t n;  /* batch size */
  int32_t channel;
  int32_t height;
  int32_t width;
  char   *name;
} TensorDescriptor_t;

typedef struct ODPostProcInfo
{
  char  in_node_names[64][512];
  int   num_in_nodes;
  int   num_out_nodes;
  TensorDescriptor_t out_nodes[64];
} ODPostProcInfo_t;

static tidl_import_config relay_import_init_params;

/* Read parameters from device config file */
static void TIDL_relayReadDeviceConfigParams(char * fileName)
{
  std::ifstream ifile(fileName);
  if(! ifile.good())
  {
    printf("\nERROR : Device config file does not exist : %s \n \n", fileName);
    return;
  }
  std::string line;
  while(std::getline(ifile, line))
  {
    int equalPos = line.find("=");
    if(equalPos != -1) /* if = exists in line */
    {
      std::string tokenName = line.substr(0, equalPos);
      std::string tokenValue = line.substr(equalPos + 1, line.length());
      tokenName.erase(remove(tokenName.begin(), tokenName.end(), ' '), tokenName.end()); /* remove spaces */
      tokenValue.erase(remove(tokenValue.begin(), tokenValue.end(), ' '), tokenValue.end()); /* remove spaces */
      std::istringstream tokenValSs(tokenValue);
      int tokenVal;
      tokenValSs >> tokenVal;

      /* Parse import tool required device config parameters here */
      if(strcmp(tokenName.c_str(), "DEVICE_NAME") == 0)
      {
        gParams.deviceName = tokenVal;
      }
      if(strcmp(tokenName.c_str(), "NUMCORES") == 0)
      {
        gParams.numCores = tokenVal;
      }
    }
  }
}

static int32_t TIDL_relayMapNode(relay::Call& call, sTIDL_LayerPC_t &layer,
                                 int zp_len = 1, int *zp = nullptr,
                                 int scale_len = 1, float *scale = nullptr);

// Functions
static int32_t TIDL_relayImportGetNewLayerIndex()
{
  int32_t index = relay_import_state.layerIndex++;
  if (index >= TIDL_NUM_MAX_PC_LAYERS)
  {
    printf("ERROR : TIDL cannot import more than %d layers\n", index);
    index = -1;
  }
  return index;
}

static int32_t TIDL_relayImportGetNewDataIndex()
{
  return relay_import_state.dataIndex++;
}

// Determine the dimensionality of a Relay Tensor expression. If the expression
// is a Tuple, use the type of the first subexpression
static int32_t TIDL_relayExprDims(Expr expr)
{
  const Type& inDataType = expr->checked_type();
  const TensorTypeNode* inTensorType = inDataType.as<TensorTypeNode>();
  if (!inTensorType && inDataType.as<TupleTypeNode>())
    inTensorType = inDataType.as<TupleTypeNode>()->fields[0].as<TensorTypeNode>();

  if (!inTensorType)
  {
    printf("ERROR : Unknown type in ExprDims\n");
    return 0;
  }

  Array<PrimExpr> inShape = inTensorType->shape;
  int dims = inShape.size();
  return dims;
}

// Normalize an axis parameter to a positive value (negative values count
// from the rightmost dimension), and convert to N12CHW layout.
// Note that TIDL tensor is 6 dims now, N12CHW
static int32_t TIDL_relayNormalizeAxis(int32_t axis, int32_t dims,
                                       int32_t *relay_axis=nullptr)
{
  if (axis < 0)
    axis += dims;

  if (relay_axis != nullptr)  *relay_axis = axis;

  // Don't normalize axis unless tensor is 4-D
  if (dims < 4)
    return axis;

  // NHWC --> N12CHW
  if (gloab_data_format == 0)
  {
     axis = axis == 1 ? TIDL_DIM_HEIGHT :  // H
            axis == 2 ? TIDL_DIM_WIDTH :   // W
            axis == 3 ? TIDL_DIM_NUMCH :   // C
            axis;
  }
  else  // NCHW --> N12CHW
  {
     axis = axis == 1 ? TIDL_DIM_NUMCH :   // C
            axis == 2 ? TIDL_DIM_HEIGHT :  // H
            axis == 3 ? TIDL_DIM_WIDTH :   // W
            axis;
  }

  // TODO: error checking
  return axis;
}

// Ensure tensors have equal shapes, for elementwise operations
static bool TIDL_relayCheckShapesEqual(Expr e1, Expr e2)
{
  Array<PrimExpr> shape1 = e1->checked_type().as<TensorTypeNode>()->shape;
  Array<PrimExpr> shape2 = e2->checked_type().as<TensorTypeNode>()->shape;

  bool ok = shape1.size() == shape2.size();
  if (ok)
    for (int i = 0; i < shape1.size(); ++i)
      if (shape1[i].as<IntImmNode>()->value != shape2[i].as<IntImmNode>()->value)
	ok = false;
  if (!ok)
    RelayDebugPrint("elementwise operator: shapes must agree\n");
  return ok;
}

// Helper to convert a float constant to a constant data pointer, by
// replicating it 'nelem' times
static float* floatConstant(float value, int nelem)
{
  int32_t size = nelem * sizeof(float);
  float* data = (float *)my_malloc(size);
  for (int i = 0; i < nelem; ++i)
    data[i] = value;
  return data;
}

// Helper to convert NDArray to constant data pointer. If the NDArray is a
// scalar, replicate it.
static size_t DLDataTypeBytes(std::string& data_type)
{
  if (data_type == "uint8" || data_type == "int8")  return 1;
  if (data_type == "float32")                       return 4;
  if (data_type == "int32")                         return 4;
  printf("ERROR: Unknown DLDataType size in NDArray: %s\n", data_type.c_str());
  assert(false);
  return 0;
}

static float* NDArrtoFloat(const runtime::NDArray &ndArr, int nelem,
                           int ndarr_zp = 0, float ndarr_scale = 1.0f)
{
  auto shape = ndArr.Shape();
  auto data_type = runtime::DLDataType2String(ndArr->dtype);
  size_t elem_bytes = DLDataTypeBytes(data_type);

  if (shape.empty())
  {
    // replicate scalar value
    float value;
    ndArr.CopyToBytes(&value, elem_bytes);

    if (data_type == "uint8")
      value = (*((uint8_t *) &value) - ndarr_zp) * ndarr_scale;
    else if (data_type == "int8")
      value = (*((int8_t *)  &value) - ndarr_zp) * ndarr_scale;
    else if (data_type == "int32")
      value = (*((int32_t *) &value) - ndarr_zp) * ndarr_scale;

    return floatConstant(value, nelem);
  }

  int32_t size = nelem * sizeof(float);
  int32_t ndarr_size = nelem * elem_bytes;
  float* data = (float *) my_malloc(ndarr_size);
  ndArr.CopyToBytes(data, ndarr_size);

  if (data_type == "uint8" || data_type == "int8" || data_type == "int32")
  {
    void* tmp_data = data;
    data = (float *) my_malloc(size);

    if (data_type == "uint8")
      for (int i = 0; i < nelem; i++)
        data[i] = (((uint8_t *) tmp_data)[i] - ndarr_zp) * ndarr_scale;
    else if (data_type == "int8")
      for (int i = 0; i < nelem; i++)
        data[i] = (((int8_t *)  tmp_data)[i] - ndarr_zp) * ndarr_scale;
    else if (data_type == "int32")
      for (int i = 0; i < nelem; i++)
        data[i] = (((int32_t *) tmp_data)[i] - ndarr_zp) * ndarr_scale;

    my_free(tmp_data);
  }

  return data;
}

/*
   Helper function to verify that a constant matches the semantics for a
   broadcast on the channel axis.  Background: Relay's arithmetic operations
   use numpy broadcast semantics.  For example, a scalar or vector can be
   added to an array by broadcasting it along one or more axes. TIDL supports
   operations between tensors and constants as channel-wise normalization layers
   (biasAdd, batchNorm).  This function is used when we see a relay operator
   between a tensor and a constant, to make sure the constant matches this form.
*/
static bool TIDL_RelayChannelBroadcast(Expr tensor, Expr constant, int32_t &numCh)
{
  Array<PrimExpr> inShape = tensor->checked_type().as<TensorTypeNode>()->shape;
  int32_t dims = inShape.size();
  if (dims != 4)
  {
    // TIDL channel-wise operations (e.g. BatchNorm, Bias) require 4D tensors
    RelayDebugPrint("Illegal broadcast: input must be 4D\n");
    return false;
  }

  // Determine channel axis, and number of channels of the input tensor
  int chIndex = gloab_data_format == 0 ? 3 : 1;
  numCh = inShape[chIndex].as<IntImmNode>()->value;

  // Analyze the dimensions of the constant
  auto constExpr = constant.as<tvm::relay::ConstantNode>()->data;
  Array<PrimExpr> constShape = constant->checked_type().as<TensorTypeNode>()->shape;

  // There are two valid shapes for the constant: scalar, or a vector which
  // broadcasts onto the channel axis.
  if (constShape.size() == 0)
    return true;

  // For a non-scalar constant, make sure it broadcasts onto the channel
  // axis. Relay uses numpy broadcast semantics.  The constant must have
  // dimension '1' along all axes except for the channel axis, which must
  // agree with the input tensor. Dimensions are compared from right to left.
  for (int32_t j = 1; j <= constShape.size(); ++j)
  {
     int32_t kIndex = constShape.size() - j;
     int32_t tIndex = inShape.size() - j;
     int32_t kdim = constShape[kIndex].as<IntImmNode>()->value;

     bool ok = (tIndex == chIndex) ? kdim == numCh : kdim == 1;
     if (!ok)
     {
       RelayDebugPrint("illegal broadcast\n");
       return false;
     }
  }
  return true;
}

/* Helper function to get scalar value from Relay scalar Constant */
template<typename Tvalue>
Tvalue TIDL_relayGetConstantVal(const relay::Expr &expr)
{
  Tvalue value;
  expr.as<ConstantNode>()->data.CopyToBytes(&value, sizeof(Tvalue));
  return value;
}

/* Helper function to get NDArray value from Relay NDArray Constant */
template<typename Tvalue>
Tvalue* TIDL_relayGetConstantArr(const relay::Expr &expr, int32_t &len)
{
  const DLTensor *dltensor = expr.as<ConstantNode>()->data.operator->();
  len = tvm::runtime::GetDataSize(*dltensor) / sizeof(Tvalue);
  return (Tvalue*) dltensor->data;
}

/*
  Infer the (float) min and max of a quantized tensor, assuming that all
  available bits are used to the full extent.  i.e. (float) min and max of
  the tensor map to the min and max of the quantized range.
  - Only 8-bit quantized model is supported for now.  We should only set
    activation for layers that output 8-bit tensors, not intermediate layers
    that output int32 tensors.
*/
void TIDL_relaySetActivation(int zp_len, int *zp, int scale_len, float *scale,
                             std::string out_type, sTIDL_LayerPC_t &layer)
{
  if (out_type != "int8" && out_type != "uint8")  return;
  if (zp_len != 1 || scale_len != 1)  return;

  float q_min = (out_type == "uint8") ?   0.0f : -128.0f;
  float q_max = (out_type == "uint8") ? 255.0f :  127.0f;

  layer.actParams.actType = TIDL_Clip;
  layer.actParams.clipMin = (q_min - zp[0]) * scale[0];
  layer.actParams.clipMax = (q_max - zp[0]) * scale[0];

  // When importing 8-bit pre-quantized models with asymmetric quantization
  // to 8-bit TIDL formats, we often need advanced quantization because
  // TIDL only supports symmetric weights and feature maps (for now).
  //     e.g. TFLite MobileNetV2
  // Note: Importing 8-bit pre-quantized model to 16-bit TIDL format does NOT
  //     require calibration, we can represent precisely
  // 8-bit case: temporarily forcing calibration with RelU6 actType
  // Keep ClipLayer's actType, so that it can be merged later
  // TODO: Use a flag to force calibration for network w/ all TIDL_Clip actType
  if (gParams.numParamBits <= 8)
    if (layer.actParams.clipMin == 0.0f &&
        layer.actParams.clipMax - 5.999 >= 0 &&
        layer.actParams.clipMax - 5.999 <= 0.01 &&
        layer.layerType != TIDL_ClipLayer)
    {
      layer.actParams.actType = TIDL_RelU6;
    }
}

// Initialize global config for TVM annotation or import
int32_t TIDL_relayInit(int32_t is_nchw, int32_t tensor_bits,
                       int32_t quant_style, int32_t hires,
                       int32_t fold_pre_bn, int32_t constraints_flag)
{
  /* Set global tensor data layout format */
  gloab_data_format = is_nchw;  // 1 for NCHW, 0 for NHWC

  /* Set global import config parameters */
  setDefaultParams(&gParams);

  /* Populate other import parameters */
  gParams.modelType = TIDL_IMPORT_MODEL_FORMAT_TVM_RELAY;
  gParams.numParamBits = tensor_bits;
  gParams.numFeatureBits = tensor_bits;
  /* Set additional exposed import config parameters */
  gParams.quantizationStyle = quant_style;
  gParams.enableHighResOptimization = hires;
  gParams.foldPreBnConv2D = fold_pre_bn;
  gParams.compileConstraintsFlag = constraints_flag;
  char *relay_import_debug = getenv("TIDL_RELAY_IMPORT_DEBUG");
  if (relay_import_debug != nullptr)
  {
    tidl_relay_debuglevel = atoi(relay_import_debug);
    gParams.debugTraceLevel = std::min(tidl_relay_debuglevel, 3);
    gParams.writeTraceLevel = (tidl_relay_debuglevel > 3) ? 3 : 0;
  }
  char *relay_max_batch_size = getenv("TIDL_RELAY_MAX_BATCH_SIZE");
  if (relay_max_batch_size != nullptr)
    tidl_relay_max_batch_size = atoi(relay_max_batch_size);
  gParams.addDataConvertToNet = 0;

  /* Populating device config gParams required by model checker */
  char * tidl_tools_path = getenv("TIDL_TOOLS_PATH");
  if(tidl_tools_path)
  {
    strcpy((char*)gParams.perfSimConfig, tidl_tools_path);
    strcat((char*)gParams.perfSimConfig,"/device_config.cfg");
  }
  else
  {
    printf("Please provide TIDL tools path \n");
    exit(-1);
  }
  TIDL_relayReadDeviceConfigParams((char*)gParams.perfSimConfig);

  relay_import_init_params = gParams;
  return 0;
}

// Mimic setup code in main() in tidl_import_main.cpp
int32_t TIDL_relayImportInit(int32_t subgraph_id,
                             int32_t num_inputs, int32_t num_outputs,
                             void* inout_dscrs,
                             int32_t is_nchw, int32_t tensor_bits,
                             std::string tidl_tools_path,
                             std::string artifacts_temp_dir)
{
  RelayDebugPrint("In TIDL_relayImportInit subgraph_id=%d\n", subgraph_id);
  /* Reset all the memories to to NULL, there could be multiple subgraphs */
  memset(&orgTIDLNetStructure, 0, sizeof(sTIDL_OrgNetwork_t));
  memset(&tIDLNetStructure,    0, sizeof(sTIDL_Network_t));
  relay_import_state.layerIndex = 0;
  relay_import_state.dataIndex  = 0;

  //TIDL_relayInit(is_nchw, tensor_bits);
  gParams = relay_import_init_params;

  TensorDescriptor_t * input_descriptors = (TensorDescriptor_t *)inout_dscrs;
  for(int i = 0; i < num_inputs; i++)
  {
    gParams.numBatches[i] = input_descriptors[i].n;
    gParams.inNumChannels[i] = input_descriptors[i].channel;
    gParams.inHeight[i] = input_descriptors[i].height;
    gParams.inWidth[i] = input_descriptors[i].width;

    gParams.inLayout[i] = (is_nchw == 1) ? TIDL_LT_NCHW : TIDL_LT_NHWC;
    gParams.inElementType[i] = input_descriptors[i].element_type;
    gParams.inQuantFactor[i] = input_descriptors[i].scale;
    gParams.inZeroPoint[i] = input_descriptors[i].zp;
    RelayDebugPrint("in[%d]: scale=%f zp=%d t=%d n=%d c=%d h=%d w=%d nchw=%d\n", i,
                    input_descriptors[i].scale, input_descriptors[i].zp,
                    input_descriptors[i].element_type,
                    input_descriptors[i].n, input_descriptors[i].channel,
                    input_descriptors[i].height, input_descriptors[i].width, is_nchw);
  }
  TensorDescriptor_t * output_descriptors = input_descriptors + num_inputs;
  for (int i = 0; i < num_outputs; i++)
  {
    gParams.outLayout[i] = (is_nchw == 1) ? TIDL_LT_NCHW : TIDL_LT_NHWC;
    gParams.outElementType[i] = output_descriptors[i].element_type;
    gParams.outTensorScale[i] = output_descriptors[i].scale;
    gParams.outZeroPoint[i] = output_descriptors[i].zp;
    RelayDebugPrint("out[%d]: scale=%f zp=%d t=%d nchw=%d\n", i,
                    output_descriptors[i].scale, output_descriptors[i].zp,
                    output_descriptors[i].element_type, is_nchw);
  }
  gParams.addDataConvertToNet = (ADD_DC_LAYER_AT_INPUT | ADD_DC_LAYER_AT_OUTPUT);
  gParams.inFileFormat = 1;  // raw data

  sprintf(inConfigFilename,
          "%s/tidl_import_subgraph%d.txt", artifacts_temp_dir.c_str(), subgraph_id);
  sprintf((char *)gParams.outputNetFile,
          "%s/subgraph%d_net.bin", artifacts_temp_dir.c_str(), subgraph_id);
  sprintf((char *)gParams.outputParamsFile,
          "%s/subgraph%d_params_", artifacts_temp_dir.c_str(), subgraph_id);
  // TODO: windows path
  sprintf((char *)gParams.inData, "%s/calib_raw_data%d.bin", artifacts_temp_dir.c_str(), subgraph_id);

  strcpy((char*)gParams.tidlStatsTool, tidl_tools_path.c_str());
  strcat((char*)gParams.tidlStatsTool, "/PC_dsp_test_dl_algo.out");
  strcpy((char*)gParams.perfSimTool, tidl_tools_path.c_str());
  strcat((char*)gParams.perfSimTool, "/ti_cnnperfsim.out");
  strcpy((char*)gParams.graphVizTool, tidl_tools_path.c_str());
  strcat((char*)gParams.graphVizTool, "/tidl_graphVisualiser.out");
  strcpy((char*)gParams.modelDumpTool, tidl_tools_path.c_str());
  strcat((char*)gParams.modelDumpTool, "/tidl_dump.out");
  strcpy((char*)gParams.perfSimConfig, tidl_tools_path.c_str());
  strcat((char*)gParams.perfSimConfig, "/device_config.cfg");

  /* Create one DataLayer for each input, enforce ordering */
  /* 1. tidl_makeDataIdLayerIdSame() only allows one outDataId per layer */
  /* 2. tidl_sortLayersInProcOrder() topo-sort layers in layer index order */
  /* 3. writeInfo() looks for input tensors in layer index order */
  /* Thus, input DataLayers ordering at creation is preserved in IOBufDesc */
  for(int i = 0; i < num_inputs; i++)
  {
    int32_t layerIndex = TIDL_relayImportGetNewLayerIndex();
    if (layerIndex < 0)  return -1;
    sTIDL_LayerPC_t& layer = orgTIDLNetStructure.TIDLPCLayers[layerIndex];
    layer.layerType         = TIDL_DataLayer;
    layer.numInBufs         = -1;
    layer.numOutBufs        = 1;
    layer.outData[0].dataId = TIDL_relayImportGetNewDataIndex();
    layer.outData[0].elementType  = input_descriptors[i].element_type;
    layer.outData[0].numDim       = TIDL_DIM_MAX; 
    layer.outData[0].tensorScale  = input_descriptors[i].scale;
    layer.outData[0].dimValues[TIDL_DIM_BATCH] = input_descriptors[i].n;
    layer.outData[0].dimValues[TIDL_DIM_NUMCH] = input_descriptors[i].channel;
    layer.outData[0].dimValues[TIDL_DIM_HEIGHT] = input_descriptors[i].height;
    layer.outData[0].dimValues[TIDL_DIM_WIDTH ] = input_descriptors[i].width;
    strcpy((char*)layer.outDataNames[0], input_descriptors[i].name);
    RelayDebugPrint("Layer %d, subgraph input %d, name=%s\n", layerIndex, i,
                    (char*)layer.outDataNames[0]);
    layer.outConsumerCnt[0] = 0;
  }
  relay_import_state.numInputDataLayers = num_inputs;

  /* Initialize rest of the layers */
  for (int i = num_inputs; i < TIDL_NUM_MAX_PC_LAYERS; i++)
  {
    sTIDL_LayerPC_t& layer_i = orgTIDLNetStructure.TIDLPCLayers[i];
    layer_i.actParams.actType  = TIDL_NoAct;
    layer_i.strideOffsetMethod = TIDL_StrideOffsetTopLeft;
  }

  if (tidlValidateImportParams(&gParams) == -1)
  {
    printf("Validation of TIDL relay import config parameters failed!\n");
    return -1;
  }

  return 0;
}

// TIDL does not support asymmetric padding, so in the case of
// asymmetric padding we take the larger of the two values. If asymmetric
// padding is introduced by the "SAME" padding option in TFLite, the extra
// column/row is on the right/bottom. In this case we need to change
// TIDL's strideOffsetMethod from "Left" to "Center".
static int32_t balancePadding(sTIDL_LayerPC_t &layer,
                              int32_t before, int32_t after)
{
  if (after > before)
  {
    if (after == before + 1)
      layer.strideOffsetMethod = TIDL_StrideOffsetCenter;
    return after;
  }
  else return before;
}

// Helper for conv2d and conv2dTranspose
int32_t convImportHelper(sTIDL_LayerPC_t &layer, std::string kernel_layout,
                         std::string data_layout, std::string out_layout,
                         const int &groups,
                         const Array<IndexExpr> &strides,
                         const Array<IndexExpr> &padding,
                         const Array<IndexExpr> &dilation,
                         const Array<IndexExpr> &kernel_size,
                         const relay::Call &call,
                         int32_t w_zp_len = 1, int32_t *w_zp_ptr = nullptr,
                         int32_t w_scale_len = 1, float *w_scale_ptr = nullptr)
{
  sTIDL_ConvParams_t &convParams = layer.layerParams.convParams;
  convParams.numGroups =  groups;
  convParams.strideH = strides[0].as<IntImmNode>()->value;
  convParams.strideW = strides[1].as<IntImmNode>()->value;
  convParams.dilationH = dilation[0].as<IntImmNode>()->value;
  convParams.dilationW = dilation[1].as<IntImmNode>()->value;
  convParams.kernelH = kernel_size[0].as<IntImmNode>()->value;
  convParams.kernelW = kernel_size[1].as<IntImmNode>()->value;

  //layer.strideOffsetMethod = TIDL_StrideOffsetTopLeft;
  //RelayDebugPrint("Weight dtype is %d\n", num);
  //layer.strideOffsetMethod = TIDL_StrideOffsetTopLeft;

  // Padding attr array [top, left, bottom, right]
  int32_t pad_t, pad_l, pad_b, pad_r = 0;
  if(padding.size() == 1)
  {
    pad_t = pad_l = pad_b = pad_r = padding[0].as<IntImmNode>()->value;
  }
  else if(padding.size() == 2)
  {
    pad_t = pad_b = padding[0].as<IntImmNode>()->value;
    pad_l = pad_r = padding[1].as<IntImmNode>()->value;
  }
  else
  {
    pad_t = padding[0].as<IntImmNode>()->value;
    pad_l = padding[1].as<IntImmNode>()->value;
    pad_b = padding[2].as<IntImmNode>()->value;
    pad_r = padding[3].as<IntImmNode>()->value;
  }

  convParams.padW = balancePadding(layer, pad_l, pad_r);
  convParams.padH = balancePadding(layer, pad_t, pad_b);
  convParams.padL = -1;
  convParams.padR = -1;
  convParams.padT = -1;
  convParams.padB = -1;
  if (convParams.padW == 0 && convParams.padH == 0)
  {
   convParams.padL = 0;
   convParams.padR = 0;
   convParams.padT = 0;
   convParams.padB = 0;
  }

  // Set input/output channels
  auto in_shape  = call->args[0]->checked_type().as<TensorTypeNode>()->shape;
  auto out_shape = call->checked_type().as<TensorTypeNode>()->shape;
  if (data_layout == "NCHW")
  {
    convParams.numInChannels  = in_shape[1].as<IntImmNode>()->value;
    convParams.numOutChannels = out_shape[1].as<IntImmNode>()->value;
  }
  else if (data_layout == "NHWC")
  {
    convParams.numInChannels  = in_shape[3].as<IntImmNode>()->value;
    convParams.numOutChannels = out_shape[3].as<IntImmNode>()->value;
  }
  else
  {
    RelayDebugPrint("Data layout %s not supported\n", data_layout.c_str());
    return -1;
  }

  // Set weights
  auto weight = call->args[1];
  auto weight_checked_type = weight->checked_type().as<TensorTypeNode>();
  Array<PrimExpr> weight_shape = weight_checked_type->shape;
  auto weight_type = DLDataType2String(weight_checked_type->dtype);
  auto weight_data = weight.as<tvm::relay::ConstantNode>()->data;

  int shape0 = weight_shape[0].as<IntImmNode>()->value;
  int shape1 = weight_shape[1].as<IntImmNode>()->value;
  int shape2 = weight_shape[2].as<IntImmNode>()->value;
  int shape3 = weight_shape[3].as<IntImmNode>()->value;

  layer.weights.bufSize = shape0 * shape1 * shape2 * shape3;
  // Copy NDArray to Array pointer, scale later on output channel
  layer.weights.ptr     = NDArrtoFloat(weight_data, layer.weights.bufSize);

  // TIDL natively uses "OIHW" kernel/weights layout
  int num_o_chs = shape0;
  if (kernel_layout == "HWIO")
  {
    TIDL_kernelReshape((float*)layer.weights.ptr,
                       shape1, shape0, shape2, shape3);
    num_o_chs = shape3;
  }
  else if (kernel_layout == "HWOI")
  {
    TIDL_kernelReshapeHWOI((float*)layer.weights.ptr,
                           shape1, shape0, shape3, shape2);
    num_o_chs = shape2;
  }
  else if (kernel_layout == "IOHW")
  {
    TIDL_kernelReshapeIOHW((float*)layer.weights.ptr,
                           shape3, shape2, shape0, shape1);
    num_o_chs = shape1;
  }
  else if (kernel_layout != "OIHW")
  {
    RelayDebugPrint("Kernel layout %s not supported\n", kernel_layout.c_str());
    return -1;
  }

  // scale on TIDL kernel weights "OIHW" layout
  float *weights = (float *) layer.weights.ptr;
  if (w_scale_ptr != nullptr)
  {
    for (int o_ch = 0; o_ch < num_o_chs; o_ch++)
    {
      int32_t zp = (w_zp_len == 1) ? w_zp_ptr[0] : w_zp_ptr[o_ch];
      float scale = (w_scale_len == 1) ? w_scale_ptr[0] : w_scale_ptr[o_ch];
      for (int i = 0; i < layer.weights.bufSize / num_o_chs; i++)
      {
        int id = o_ch * (layer.weights.bufSize / num_o_chs) + i;
        weights[id] = (weights[id] - zp) * scale;
      }
    }
  }

  convParams.enableBias      = 0;
  convParams.enableEWRelU    = 0;
  convParams.enablePooling   = 0;

  //strcpy((char*)layer.inDataNames[0], "???");   // can we get name?
  //strcpy((char*)layer.outDataNames[0], "???");  // can we get name?
  layer.inData[0].dimValues[TIDL_DIM_NUMCH] = convParams.numInChannels;
  layer.outData[0].dimValues[TIDL_DIM_NUMCH] = convParams.numOutChannels;

  return 0;
}

// Import tvm.relay.nn.conv2d operator to TIDL
// nn.conv2d(input, weight) with <Conv2DAttrs>
// Import tvm.relay.qnn.conv2d operator to TIDL
// qnn.conv2d(input, weight, input_zero_point, weight_zero_point,
//            input_scale, weight_scale) with <Conv2DAttrs>
int32_t TIDL_relayMapConvParams(relay::Call& call, sTIDL_LayerPC_t &layer,
                                std::string op_name)
{
  layer.layerType = TIDL_ConvolutionLayer;
  layer.outData[0].elementType = tidl_getElementType(1);

  auto weight = call->args[1];
  // TODO: make sure weight is a ConstantNode
  auto weight_type = DLDataType2String(
                           weight->checked_type().as<TensorTypeNode>()->dtype);
  if ((op_name == "nn.conv2d" && weight_type != "float32") ||
      (op_name == "qnn.conv2d" && weight_type != "uint8" &&
                                  weight_type != "int8"))
  {
    RelayDebugPrint("Weight type %s not supported\n", weight_type.c_str());
    return -1;
  }

  int32_t  w_zp_len = 1;
  int32_t  w_scale_len = 1;
  int32_t *w_zp_ptr = nullptr;     // default value is 0
  float   *w_scale_ptr = nullptr;  // default value is 1.0f
  if (op_name == "qnn.conv2d")
  {
    w_zp_ptr = TIDL_relayGetConstantArr<int32_t>(call->args[3], w_zp_len);
    w_scale_ptr = TIDL_relayGetConstantArr<float>(call->args[5], w_scale_len);
  }

  auto param = call->attrs.as<Conv2DAttrs>();

  return convImportHelper(layer, param->kernel_layout, param->data_layout,
                          param->out_layout, param->groups,
                          param->strides, param->padding, param->dilation,
                          param->kernel_size, call,
                          w_zp_len, w_zp_ptr, w_scale_len, w_scale_ptr);
}

// Import tvm.relay.nn.conv2d_tranpose to TIDL
int32_t TIDL_relayMapConvTransposeParams(relay::Call& call, sTIDL_LayerPC_t &layer)
{
  layer.layerType = TIDL_Deconv2DLayer;
  layer.outData[0].elementType = tidl_getElementType(1);

  auto weight = call->args[1];
  auto weight_type   = DLDataType2String(weight->checked_type().as<TensorTypeNode>()->dtype);

  sTIDL_ConvParams_t &convParams = layer.layerParams.convParams;
  convParams.numGroups = 1;
  convParams.dilationW = 1;
  convParams.dilationH = 1;
  convParams.strideW = 1;
  convParams.strideH = 1;
  convParams.padW = 0;
  convParams.padH = 0;

  if (weight_type != "float32")
  {
    RelayDebugPrint("Weight type %s not supported\n", weight_type.c_str());
    return -1;
  }

  auto param = call->attrs.as<Conv2DTransposeAttrs>();

  return convImportHelper(layer, param->kernel_layout, param->data_layout,
                          param->out_layout, param->groups,
                          param->strides, param->padding, param->dilation,
                          param->kernel_size, call);
}

// Import tvm.relay.nn.batch_norm operator to TIDL
int32_t TIDL_relayMapBatchNormParams(relay::Call& call, sTIDL_LayerPC_t &layer)
{
  layer.layerType = TIDL_BatchNormLayer;

  auto weight = call->args[1];
  auto weight_type = DLDataType2String(weight->checked_type().
                     as<TensorTypeNode>()->dtype);
  Array<PrimExpr> weight_shape = weight->checked_type().
                     as<TensorTypeNode>()->shape;

  int params_size = 0;
  int num_params = weight_shape[0].as<IntImmNode>()->value;
  if (weight_type != "float32")
  {
    RelayDebugPrint("Weight type %s not supported\n", weight_type.c_str());
    return -1;
  }
  else
  {
    params_size = sizeof(float)*num_params;
  }

  layer.weights.ptr     = my_malloc(params_size);
  layer.weights.bufSize = num_params;
  layer.bias.ptr        = my_malloc(params_size);
  layer.bias.bufSize    = num_params;

  auto params = call->attrs.as<BatchNormAttrs>();

  auto epsilon = params->epsilon;
  float* gamma = NDArrtoFloat(call->args[1].as<tvm::relay::ConstantNode>()->data,
                                num_params);
  float* beta  = NDArrtoFloat(call->args[2].as<tvm::relay::ConstantNode>()->data,
                                num_params);
  float* mean  = NDArrtoFloat(call->args[3].as<tvm::relay::ConstantNode>()->data,
                                num_params);
  float* var   = NDArrtoFloat(call->args[4].as<tvm::relay::ConstantNode>()->data,
                                num_params);

  // Calculate scale and bias for weights/bias
  TIDL_tfBNToScaleBias((float *)layer.weights.ptr, (float *)layer.bias.ptr,
                       num_params, mean, var,
                       gamma, beta, epsilon);

  return 0;
}

/*
     Import tvm.relay.add operator to TIDL
*/
int32_t TIDL_relayMapAddParams(relay::Call& call, sTIDL_LayerPC_t &layer,
                               std::string op_name)
{
  if(call->args[1].as<ConstantNode>())
  {
    int32_t numCh;
    // Adding a tensor with a constant: map to BiasAdd layer
    layer.layerType = TIDL_BiasLayer;
    if (!TIDL_RelayChannelBroadcast(call->args[0], call->args[1], numCh))
      return -1;
    // if qnn.add, get zp, scale (haven't seen this case yet, just in case)
    int weight_zp = 0;
    float weight_scale = 1.0f;
    if (op_name == "qnn.add")
    {
      weight_zp = TIDL_relayGetConstantVal<int32_t>(call->args[5]);
      weight_scale = TIDL_relayGetConstantVal<float>(call->args[4]);
    }
    // If constant is a scalar, replicate it; otherwise copy it
    auto constExpr = call->args[1].as<tvm::relay::ConstantNode>()->data;
    layer.bias.ptr = NDArrtoFloat(constExpr, numCh, weight_zp, weight_scale);
    layer.bias.bufSize = numCh;
  }
  else
  {
    // Adding two tensors: map to elementwise add layer
    if (!TIDL_relayCheckShapesEqual(call->args[0], call->args[1]))
      return -1;
    layer.layerType = TIDL_EltWiseLayer;
    layer.layerParams.eltWiseParams.eltWiseType = TIDL_EltWiseSum;
    layer.layerParams.eltWiseParams.numInData = 2;
    layer.numInBufs = 2;
  }
  return 0;
}

/*
     Import tvm.relay.nn.bias_add operator to TIDL
     A "bias_add" operator adds a Tensor and a 1D vector broadcast across
     one of the axes (must be C for TIDL).
*/
int32_t TIDL_relayMapBiasAddParams(relay::Call& call, sTIDL_LayerPC_t &layer,
                                   int zp_len, int *zp,
                                   int scale_len, float *scale)
{
  layer.layerType = TIDL_BiasLayer;

  auto bias = call->args[1];
  auto bias_type = DLDataType2String(bias->checked_type().as<TensorTypeNode>()->dtype);
  auto attrs = call->attrs.as<BiasAddAttrs>();
  int32_t dims = TIDL_relayExprDims(call->args[0]);
  int32_t axis = TIDL_relayNormalizeAxis(attrs->axis, dims);
  Array<PrimExpr> bias_shape = bias->checked_type().as<TensorTypeNode>()->shape;

  if(!call->args[1].as<ConstantNode>())
  {
    RelayDebugPrint("bias_add op must have args[1] as expr.Constant\n");
    return -1;
  }
  if (bias_type != "float32" &&  bias_type != "int32")
  {
    RelayDebugPrint("Unsupported data type of bias_add\n");
    return -1;
  }
  if (dims == 4 && axis != TIDL_DIM_NUMCH)  //changed
  {
    RelayDebugPrint("bias_add: unsupported axis: %d (must be channel axis)\n", axis);
    return -1;
  }
  auto bias_data = bias.as<tvm::relay::ConstantNode>()->data;
  auto numParams =  bias_shape[0].as<IntImmNode>()->value;

  // Scale bias per-tensor or per per-channel
  if ((zp_len != 1 && zp_len != numParams) ||
      (scale_len != 1 && scale_len != numParams))
  {
    RelayDebugPrint("bias_add: unsupported quant zp_len=%d, scale_len=%d\n",
                    zp_len, scale_len);
    return -1;
  }
  // Copy NDArray to Array pointer
  layer.bias.ptr = NDArrtoFloat(bias_data, numParams, 0, 1.0f);
  layer.bias.bufSize = numParams;
  float *bias_ptr = (float *)layer.bias.ptr;
  for (int i = 0; i < numParams; i++)
  {
    int zp_val = (zp_len == 1) ? zp[0] : zp[i];
    float scale_val = (scale_len == 1) ? scale[0] : scale[i];
    bias_ptr[i] = (bias_ptr[i] - zp_val) * scale_val;
  }

  return 0;
}

/*
  Import tvm.relay.clip, tvm.relay.nn.relu, tvm.relay.nn.leaky_relu,
  tvm.relay.nn.prelu operators to TIDL
*/
int32_t TIDL_relayMapReluParams(relay::Call& call, sTIDL_LayerPC_t &layer,
                                std::string reluType="")
{
  layer.layerType = TIDL_ReLULayer;
  if(reluType == "Relu6")
  {
    // This is mapped to TIDL_ClipLayer if min/max are 0/6
    //  -otherwise map to TIDL_RelU6Layer
    auto attrs = call->attrs.as<ClipAttrs>();
    auto a_min = attrs->a_min;
    auto a_max = attrs->a_max;

    if(a_min == 0 && a_max == 6)
      layer.actParams.actType = TIDL_RelU6;
    else
    {
      layer.layerType = TIDL_ClipLayer;
      layer.actParams.actType = TIDL_Clip;
      layer.actParams.clipMin = a_min;
      layer.actParams.clipMax = a_max;
    }
  }
  else if(reluType == "PRelu")
  {
    // This is a TIDL_Relu layer in TFLite but TIDL_PRelu in ONNX
    layer.actParams.actType = TIDL_PRelU;
  }
  else if(reluType == "LeakyRelu")
  {
    layer.layerType = TIDL_LeakyReluLayer;
    layer.outData[0].elementType = tidl_getElementType(1);
    auto attrs = call->attrs.as<LeakyReluAttrs>();
    layer.layerPCParams.leakyReluParams.alpha = attrs->alpha;
  }
  else
  {
    layer.actParams.actType = TIDL_RelU;
  }

  return 0;
}

/*
  Import tvm.relay.sigmoid, map to TIDL_SigmoidLayer
*/
int32_t TIDL_relayMapSigmoidParams(relay::Call& call, sTIDL_LayerPC_t &layer)
{
  layer.layerType = TIDL_SigmoidLayer;
  layer.actParams.actType = TIDL_Sigmoid;
  return 0;
}

int32_t TIDL_relayMapTanhParams(relay::Call& call, sTIDL_LayerPC_t &layer)
{
  layer.layerType = TIDL_TanhLayer;
  layer.actParams.actType = TIDL_Tanh;
  return 0;
}

int32_t TIDL_relayMapHardSigmoidParams(relay::Call& call, sTIDL_LayerPC_t &layer)
{
  layer.layerType = TIDL_HardSigmoidLayer;
  layer.actParams.actType = TIDL_HardSigmoid;
  return 0;
}

int32_t TIDL_relayMapELUParams(relay::Call& call, sTIDL_LayerPC_t &layer)
{
  layer.layerType = TIDL_ELULayer;
  layer.actParams.actType = TIDL_ELU;
  return 0;
}

/*
  Import tvm.relay.qnn.requantize
  map to TIDL_Clip, use tensor bits and scale to derive min/max value,
  assuming that all tensors bits are used for the full range possible
*/
int32_t TIDL_relayMapRequantizeParams(relay::Call& call, sTIDL_LayerPC_t &layer)
{
  layer.layerType = TIDL_ClipLayer;
  return 0;
}

/*
  Import tvm.relay.cast
  map to TIDL_IdentityLayer (PC representation only, merged in optimizeNet)
*/
int32_t TIDL_relayMapCastParams(relay::Call& call, sTIDL_LayerPC_t &layer)
{
  layer.layerType = TIDL_IdentityLayer;
  return 0;
}

// Helper for pool import based on inputted Max or Average Pooling params
int32_t poolParamHelper(sTIDL_LayerPC_t &layer,
                        Array<PrimExpr> pool_size, Array<PrimExpr> strides,
                        Array<PrimExpr> padding, bool ceil_mode)
{
  sTIDL_PoolingParams_t &poolParams = layer.layerParams.poolParams;
  poolParams.kernelH =  pool_size[0].as<IntImmNode>()->value;
  poolParams.kernelW =  pool_size[1].as<IntImmNode>()->value;
  poolParams.strideH =  strides[0].as<IntImmNode>()->value;
  poolParams.strideW =  strides[1].as<IntImmNode>()->value;
  poolParams.useCeil =  ceil_mode;

  // padding is specified either as [H,W] or [T,L,B,R]
  int32_t padT = padding[0].as<IntImmNode>()->value;
  int32_t padL = padding[1].as<IntImmNode>()->value;
  int32_t padB = (padding.size() == 4) ? padding[2].as<IntImmNode>()->value : padT;
  int32_t padR = (padding.size() == 4) ? padding[3].as<IntImmNode>()->value : padL;

  poolParams.padW = balancePadding(layer, padL, padR);
  poolParams.padH = balancePadding(layer, padT, padB);
  poolParams.padT = padT;
  poolParams.padB = padB;
  poolParams.padL = padL;
  poolParams.padR = padR;

  return 0;
}

/*
  Import tvm.relay.nn.global_avg_pool2d, tvm.relay.nn.avg_pool2d,
  tvm.relay.nn.max_pool2d operator to TIDL
*/
int32_t TIDL_relayMapPoolingParams(relay::Call& call, sTIDL_LayerPC_t &layer,
                                   std::string op_name)
{
  layer.layerType = TIDL_PoolingLayer;
  sTIDL_PoolingParams_t &poolParams = layer.layerParams.poolParams;
  poolParams.useCeil = 0;
  int32_t status = 0;

  // Convert avgpool2d to global pooling if the kernel size matches the HxW dimensions.
  // This happens in TIDL_tfOutReshapePoolingLayer but we need it prior to calling the model
  // checker for allow/deny.
  if (op_name == "nn.avg_pool2d")
  {
      auto param = call->attrs.as<AvgPool2DAttrs>();
      uint32_t kernelH =  param->pool_size[0].as<IntImmNode>()->value;
      uint32_t kernelW =  param->pool_size[1].as<IntImmNode>()->value;
      auto inShape = call->args[0]->checked_type().as<TensorTypeNode>()->shape;
      int  dims = inShape.size();
      if (dims == 4)   
      {
        int hAxis = param->layout == "NHWC" ? 1 : 2; 
        int wAxis = param->layout == "NHWC" ? 2 : 3;
        uint32_t inH = inShape[hAxis].as<IntImmNode>()->value;
	uint32_t inW = inShape[wAxis].as<IntImmNode>()->value;

	if (kernelH == inH && kernelW == inW)
	  op_name = "nn.global_avg_pool2d";
      }
  }

  if (op_name == "nn.global_avg_pool2d")
  {
    poolParams.poolingType = TIDL_AveragePooling;
    // Pool size is set to input layer size
    poolParams.kernelH = 0;
    poolParams.kernelW = 0;
    poolParams.padH    = 0;
    poolParams.padW    = 0;
    poolParams.padT    = 0;
    poolParams.padB    = 0;
    poolParams.padL    = 0;
    poolParams.padR    = 0;

    poolParams.strideH = 1;
    poolParams.strideW = 1;
  }
  else if(op_name == "nn.avg_pool2d")
  {
    poolParams.poolingType = TIDL_AveragePooling;
    auto param = call->attrs.as<AvgPool2DAttrs>();
    status = poolParamHelper(layer, param->pool_size, param->strides,
                             param->padding, param->ceil_mode);
    // Convert 1x1 average pooling into max pooling (operations are identical)
    // mxnet_resnet50_v1d
    if (poolParams.kernelH == 1 && poolParams.kernelW == 1 &&
       ((poolParams.strideH == 2 && poolParams.strideW == 2) ||
        (poolParams.strideH == 1 && poolParams.strideW == 1)))
      poolParams.poolingType = TIDL_MaxPooling;
  }
  else if (op_name == "nn.max_pool2d")
  {
    poolParams.poolingType = TIDL_MaxPooling;
    auto param = call->attrs.as<MaxPool2DAttrs>();
    status = poolParamHelper(layer, param->pool_size, param->strides,
                             param->padding, param->ceil_mode);
  }
  else
  {
    RelayDebugPrint("unknown pooling op: %s\n", op_name.c_str());
    return -1;
  }

  return status;
}

// Import tvm.relay.multiply operator to TIDL
int32_t TIDL_relayMapMulParams(relay::Call& call, sTIDL_LayerPC_t &layer,
                               std::string op_name)
{
  if(call->args[1].as<ConstantNode>())
  {
    // Multiplying a tensor with a constant: map to BatchNorm layer
    layer.layerType = TIDL_BatchNormLayer;
    int32_t numCh;
    if (!TIDL_RelayChannelBroadcast(call->args[0], call->args[1], numCh))
      return -1;
    int32_t weight_zp = 0;
    float weight_scale = 1.0;
    if (op_name == "qnn.mul")
    {
      weight_zp = TIDL_relayGetConstantVal<int32_t>(call->args[5]);
      weight_scale = TIDL_relayGetConstantVal<float>(call->args[4]);
    }
    // If scale constant is a scalar, replicate it; otherwise copy it. Set
    // bias to 0.
    auto constExpr = call->args[1].as<tvm::relay::ConstantNode>()->data;
    layer.weights.ptr = NDArrtoFloat(constExpr, numCh, weight_zp, weight_scale);
    layer.bias.ptr = floatConstant(0.0, numCh);
    layer.weights.bufSize = layer.bias.bufSize = numCh;
  }
  else
  {
    // Multiplying two tensors: map to elementwise product layer
    if (!TIDL_relayCheckShapesEqual(call->args[0], call->args[1]))
      return -1;
    layer.layerType = TIDL_EltWiseLayer;
    layer.layerParams.eltWiseParams.eltWiseType = TIDL_EltWiseProduct;
    layer.layerParams.eltWiseParams.numInData = 2;
    layer.numInBufs = 2;
  }

  return 0;
}

// Import tvm.relay.divde operator to TIDL
int32_t TIDL_relayMapDivParams(relay::Call& call, sTIDL_LayerPC_t &layer)
{
  // Map divide as multiply, then invert divisor constants
  if(!call->args[1].as<ConstantNode>())
  {
    RelayDebugPrint("Divisor must be constant\n");
    return -1;
  }
  if (TIDL_relayMapMulParams(call, layer, "multiply") != 0)
    return -1;

  for (int i = 0; i < layer.weights.bufSize; ++i)
  {
    float value = ((float *)layer.weights.ptr)[i];
    if (value == 0)
    {
      RelayDebugPrint("Divisor is 0\n");
      return -1;
    }
    ((float *)layer.weights.ptr)[i] = 1.0/value;
  }
  return 0;
}

// Import tvm.relay.reshape operator to TIDL
int32_t TIDL_relayMapReshapeParams(relay::Call& call, sTIDL_LayerPC_t &layer)
{
  auto attrs = call->attrs.as<ReshapeAttrs>();
  // Use int64_t for reshape parameters because tidl_FindOnnxShuffleLayer()
  // expects int64_t numbers.
  layer.weights.ptr = my_malloc(attrs->newshape.size()*sizeof(int64_t));
  layer.weights.bufSize = attrs->newshape.size();
  int64_t * shape = (int64_t *)layer.weights.ptr;
  for(int i=0; i<attrs->newshape.size(); i++)
    shape[i] = (int64_t)attrs->newshape[i]->value;

  layer.layerType = TIDL_ReshapeLayer;
  return 0;
}

// Import tvm.relay.nn.softmax operator to TIDL
int32_t TIDL_relayMapSoftmaxParams(relay::Call& call, sTIDL_LayerPC_t &layer)
{
  layer.layerType = TIDL_SoftMaxLayer;

  // Softmax input has shape [Batch, WIDTH] or [Batch, HEIGHT, WIDTH]
  auto inShape = call->args[0]->checked_type().as<TensorTypeNode>()->shape;
  int  dims = inShape.size();
  auto attrs = call->attrs.as<SoftmaxAttrs>();
  int32_t relay_axis;
  int32_t axis = TIDL_relayNormalizeAxis(attrs->axis, dims, &relay_axis);
  if (relay_axis == dims - 1)
  {
    axis = TIDL_DIM_WIDTH;
  }
  else if (relay_axis == dims - 2)
  {
    axis = TIDL_DIM_HEIGHT;
  }
  else
  {
    RelayDebugPrint("softmax axis %d of %d-D tensor not supported yet\n", relay_axis, dims);
    return -1;
  }

  layer.inData[0].dimValues[TIDL_DIM_BATCH]  = inShape[0].as<IntImmNode>()->value;
  layer.inData[0].dimValues[TIDL_DIM_NUMCH]  =
                    (dims > 3) ? inShape[dims - 3].as<IntImmNode>()->value : 1;
  layer.inData[0].dimValues[TIDL_DIM_HEIGHT] =
                    (dims > 2) ? inShape[dims - 2].as<IntImmNode>()->value : 1;
  layer.inData[0].dimValues[TIDL_DIM_WIDTH]  =
                                 inShape[dims - 1].as<IntImmNode>()->value;
  layer.layerParams.softMaxParams.axis = axis;
  layer.layerParams.softMaxParams.outTranspose = 0;

  return 0;
}

// Import tvm.relay.squeeze operator to TIDL
int32_t TIDL_relayMapSqueezeParams(relay::Call& call, sTIDL_LayerPC_t &layer)
{
  layer.layerType = TIDL_SqueezeLayer;
  auto attrs = call->attrs.as<SqueezeAttrs>();
  uint32_t dims = TIDL_relayExprDims(call->args[0]);
  auto axisList = attrs->axis;

  // TODO: are squeezeParams used by TIDL?
  for (int i = 0; i < axisList.size(); i++)
  {
    uint32_t axis = axisList[i].as<IntImmNode>()->value;
    axis = TIDL_relayNormalizeAxis(axis, dims);
    layer.layerPCParams.squeezeParams.axis[i] = axis;
  }

  return 0;
}

// Import tvm.relay.nn.batch_flatten operator to TIDL
int32_t TIDL_relayMapBatchFlattenParams(relay::Call& call, sTIDL_LayerPC_t &layer)
{
  auto inShape = call->args[0]->checked_type().as<TensorTypeNode>()->shape;
  if (inShape[0].as<IntImmNode>()->value != 1)
  {
    RelayDebugPrint("batch_flatten n!=1 not supported yet\n");
    return -1;
  }

  layer.layerType = TIDL_FlattenLayer;
  return 0;
}

// Import tvm.relay.dropout operator to TIDL
int32_t TIDL_relayMapDropoutParams(relay::Call& call, sTIDL_LayerPC_t &layer)
{
  layer.layerType = TIDL_DropOutLayer;
  return 0;
}

// Import tvm.relay.nn.dense operator to TIDL
int32_t TIDL_relayMapDenseParams(relay::Call& call, sTIDL_LayerPC_t &layer,
                                 std::string op_name)
{
  layer.layerType = TIDL_InnerProductLayer;
  sTIDL_InnerProductParams_t &ipParams = layer.layerParams.innerProductParams;

  auto weight = call->args[1];
  auto weight_data  = weight.as<tvm::relay::ConstantNode>()->data;
  auto data_shape  = weight_data->shape;

  auto numOutNodes = data_shape[0];
  auto numInNodes = data_shape[1];

  ipParams.numInRows = 1;
  ipParams.isBias    = 1;
  ipParams.numInCols = numInNodes;
  ipParams.numOutCols= numOutNodes;
  ipParams.inputATranspose = 0;
  ipParams.inputBTranspose = 1;
  ipParams.constIdx = -1;

  int32_t weight_zp = 0;
  float weight_scale = 1.0;
  if (op_name == "qnn.dense")
  {
    weight_zp = TIDL_relayGetConstantVal<int32_t>(call->args[3]);
    weight_scale = TIDL_relayGetConstantVal<float>(call->args[5]);
  }

  // Allocate memory to store weights
  layer.weights.bufSize = numInNodes * numOutNodes;

  // Copy NDArray to Array Pointer
  layer.weights.ptr = NDArrtoFloat(weight_data, layer.weights.bufSize,
                                   weight_zp, weight_scale);

  // Set default bias as zero, if next layer has bias it will get merged and this buffer will be used
  layer.bias.bufSize = numOutNodes;
  layer.bias.ptr = floatConstant(0.0, numOutNodes);

  auto inShape = call->args[0]->checked_type().as<TensorTypeNode>()->shape;

  layer.inData[0].dimValues[TIDL_DIM_BATCH]  = inShape[0].as<IntImmNode>()->value;
  layer.inData[0].dimValues[TIDL_DIM_NUMCH]  = 1;
  layer.inData[0].dimValues[TIDL_DIM_HEIGHT] = 1;
  layer.inData[0].dimValues[TIDL_DIM_WIDTH]  = ipParams.numInCols;
  layer.outData[0].dimValues[TIDL_DIM_WIDTH] = ipParams.numOutCols;

  return 0;
}

// Import tvm.relay.nn.pad operator to TIDL
int32_t TIDL_relayMapPadParams(relay::Call& call, sTIDL_LayerPC_t &layer)
{
  layer.layerType = TIDL_PadLayer;

  auto attrs = call->attrs.as<PadAttrs>();
  auto pad_mode  = attrs->pad_mode;
  auto pad_width = attrs->pad_width;

  // check pad_mode
  if (pad_mode != "constant")
    return -1;

  // 1 for NCHW, 0 for NHWC
  int32_t relay_n_dim  = 0;
  int32_t relay_ch_dim = gloab_data_format == 0 ? 3 : 1;
  int32_t relay_h_dim  = gloab_data_format == 0 ? 1 : 2;
  int32_t relay_w_dim  = gloab_data_format == 0 ? 2 : 3;

  int32_t padT = pad_width[relay_h_dim][0].as<IntImmNode>()->value;
  int32_t padB = pad_width[relay_h_dim][1].as<IntImmNode>()->value;
  int32_t padL = pad_width[relay_w_dim][0].as<IntImmNode>()->value;
  int32_t padR = pad_width[relay_w_dim][1].as<IntImmNode>()->value;

  // check pad_width, asymmetric padding not supported yet
  if (pad_width.size() != 4 ||
      pad_width[relay_n_dim ][0].as<IntImmNode>()->value != 0 ||
      pad_width[relay_n_dim ][1].as<IntImmNode>()->value != 0 ||
      pad_width[relay_ch_dim][0].as<IntImmNode>()->value != 0 ||
      pad_width[relay_ch_dim][1].as<IntImmNode>()->value != 0 ||
      padT != padB || padL != padR)
    return -1;

  layer.layerParams.padLayerParams.padT = padT;
  layer.layerParams.padLayerParams.padB = padB;
  layer.layerParams.padLayerParams.padL = padL;
  layer.layerParams.padLayerParams.padR = padR;

  return 0;
}

/*
    Helper for concat import
    Find the input nodes of a given relay.expr.Call node.

    Only find input nodes that are relay.expr.Call.
    If an input node is a relay.expr.TupleGetItem, then check this input
    node's input node.
*/
static int inNodesHelper(Array<Expr> args, Array<Expr> fields={})
{
  Array<Expr> in_nodes = args.size()==0 ? fields:args;
  int len=0;
  for (auto& node : in_nodes)
  {
    if (node.as<CallNode>()) len++;
    else if (node.as<TupleGetItemNode>()) len++;
    else if (node.as<TupleNode>()) len += inNodesHelper({},
                                          node.as<TupleNode>()->fields);
    else if (node.as<VarNode>()){
      std::string name_hint = node.as<VarNode>()->name_hint();
      if (name_hint.find("tidl_") != std::string::npos &&
          name_hint.find("_i") != std::string::npos){
          // this is the input to the subgraph
          len++;
      }
    }
    //else: ignore all other types of nodes: var, const, etc.
  }
  return len;
}

static int findNumInNodes(relay::Call& call)
{
  return inNodesHelper(call->args);
}

// Import tvm.relay.concatenate operator to TIDL
int32_t TIDL_relayMapConcatParams(relay::Call& call, sTIDL_LayerPC_t &layer)
{
  layer.layerType = TIDL_ConcatLayer;
  layer.numInBufs = findNumInNodes(call);

  // TIDL currently supports concat only on the C axis of 4D tensors.
  // The axis is checked in the modelCheck code.
  int32_t dims = TIDL_relayExprDims(call->args[0]);
  if (dims != 4)
  {
    RelayDebugPrint("concat requires 4D tensors\n");
    return -1;
  }

  auto attrs = call->attrs.as<ConcatenateAttrs>();
  int32_t axis = TIDL_relayNormalizeAxis(attrs->axis, dims);
  if (axis != TIDL_DIM_NUMCH)
  {
    RelayDebugPrint("concat requires axis being channel dim\n");
    return -1;
  }
  // "-2" to set "axis" in NCHW.  OutReshapeConcat() will convert to N12CHW.
  layer.layerParams.concatParams.axis = axis - 2;
  return 0;
}

// Import tvm.relay.mean operator to TIDL
int32_t TIDL_relayMapMeanParams(relay::Call& call, sTIDL_LayerPC_t &layer)
{
  int32_t status = -1;

  // Check attributes exclude and axis here because tidlModelCheckLayer() doesn't have
  // access to them, but tidlModelCheckLayer() will still check kernel, pad, etc.
  auto attrs = call->attrs.as<ReduceAttrs>();
  int32_t dims = TIDL_relayExprDims(call->args[0]);
  if(attrs->exclude == false && attrs->axis.defined() && attrs->axis.size() == 2)
  {
    // Only supports mean over height & width axes - global averaging by TIDL.
    // Keepdims can be either true or false - TIDL import to decide output dimension
    int axis_h = TIDL_relayNormalizeAxis(attrs->axis[0].as<IntImmNode>()->value, dims);
    int axis_w = TIDL_relayNormalizeAxis(attrs->axis[1].as<IntImmNode>()->value, dims);
    if (axis_h == TIDL_DIM_HEIGHT && axis_w == TIDL_DIM_WIDTH)
    {
      TIDL_relayMapPoolingParams(call, layer, "nn.global_avg_pool2d");
      status = 0;  // No error - this op is supported
    }
  }

  // Return -1 if there are unsupported attributes and consequently
  // TIDL_relayAllowNode will return false (0) to TVM.
  return status;
}

// Import tvm.relay.image.resize or tvm.relay.nn.upsampling operator to TIDL
int32_t TIDL_relayMapResize2DParams(relay::Call& call, sTIDL_LayerPC_t &layer,
                                    std::string type="none")
{
  sTIDL_ResizeLayerParams_t& params = layer.layerParams.resizeParams;
  layer.layerType = TIDL_ResizeLayer;
  std::string mode;

  if(type == "upsample")
  {
    auto attrs = call->attrs.as<UpSamplingAttrs>();
    // resizeRatio[0] and resizeRatio[1] are not needed
    params.resizeRatio[TIDL_DIM_HEIGHT] = attrs->scale_h;
    params.resizeRatio[TIDL_DIM_WIDTH] = attrs->scale_w;
    mode = attrs->method;
  }
  else
  {
    auto attrs = call->attrs.as<Resize2DAttrs>();
    int new_height = attrs->size[0].as<IntImmNode>()->value;
    int new_width  = attrs->size[1].as<IntImmNode>()->value;

    auto buf = call->args[0];
    Array<PrimExpr> buf_shape = buf->checked_type().as<TensorTypeNode>()->shape;
    int height_dim = gloab_data_format == 0 ? 1 : 2;
    int width_dim  = gloab_data_format == 0 ? 2 : 3;
    int orig_height = buf_shape[height_dim].as<IntImmNode>()->value;
    int orig_width  = buf_shape[width_dim].as<IntImmNode>()->value;

    params.resizeRatio[TIDL_DIM_HEIGHT] = (new_height * 1.0) / orig_height;
    params.resizeRatio[TIDL_DIM_WIDTH]  = (new_width  * 1.0) / orig_width;

    mode = attrs->method;
  }

  // mode is the same for "image.resize" and "nn.upsampling"
  if(mode == "nearest_neighbor")
    layer.layerParams.resizeParams.mode = TIDL_ResizeNearest;
  else if(mode == "bilinear" || mode == "linear")
    layer.layerParams.resizeParams.mode = TIDL_ResizeBilinear;
  else
    layer.layerParams.resizeParams.mode = TIDL_ResizeMax;  // unsupported mode

  return 0;
}

// Import tvm.relay.shape_of operator to TIDL
int32_t TIDL_relayMapShapeParams(relay::Call& call, sTIDL_LayerPC_t &layer)
{
  layer.layerType = TIDL_ShapeLayer;
  return 0;
}

// Import tvm.relay.nn.strided_slice operator to TIDL
int32_t TIDL_relayMapStridedSliceParams(relay::Call& call, sTIDL_LayerPC_t &layer)
{
  layer.layerType = TIDL_SliceLayer;
  auto attrs = call->attrs.as<StridedSliceAttrs>();
  auto tensorType = call->args[0]->checked_type().as<TensorTypeNode>();
  Array<PrimExpr> data_shape = tensorType->shape;
  int data_dims = data_shape.size();
  // Relay might have 1) smaller begin[]/end[] dims than tensor data_dims,
  // 2) vague end[] spec (ONNX frontend used INT_MAX instead of -1), e.g.
  // %286 = strided_slice(%285, begin=[0, 0], end=[2147483647, 1], strides=[1]) /* ty=Tensor[(1, 1, 96, 192), float32] */;
  int ndims = attrs->begin.value().size();  // <= data_dims

  // Parse the strided slice parameters into the following arrays. In relay,
  // the end value means:
  //   mode == 'end': the ending indeces
  //   mode == 'size': the size of the slice
  //   -1 means 'all remaining elements'
  int32_t dim[TIDL_DIM_MAX];
  int32_t begin[TIDL_DIM_MAX];
  int32_t end[TIDL_DIM_MAX];
  int32_t stride[TIDL_DIM_MAX];
  int32_t axes[TIDL_DIM_MAX];

  if (ndims > TIDL_DIM_MAX)
  {
    RelayDebugPrint("Unsupported strided_slice - too many dimensions (max 4)\n");
    return -1;
  }

  for (int i = 0; i < data_dims; ++i)
  {
    dim[i] = data_shape[i].as<IntImmNode>()->value;
  }

  for (int i = 0; i < ndims; ++i)
  {
    begin[i] = attrs->begin.value()[i].IntValue();
    stride[i] = !attrs->strides ? 1 :
		attrs->strides.value().size() == 1 ? (int32_t)attrs->strides.value()[0].IntValue() :
		(int32_t)attrs->strides.value()[i].IntValue();
    axes[i] = !attrs->axes ? i : attrs->axes.value()[i].IntValue();
    if (axes[i] < 0)  axes[i] += data_dims;
    int32_t endval = attrs->end.value()[i].IntValue();
    // endval is vague in TVM spec, ONNX frontend used INT_MAX in "end" mode
    if (attrs->slice_mode == "end")
      end[i] = (endval == -1 || endval > dim[axes[i]]) ? dim[axes[i]] : endval;
    else if (attrs->slice_mode == "size")
    {
      end[i] = begin[i] + (endval == -1 ? dim[axes[i]] : endval);
      stride[i] = 1;
    }
    else
    {
      RelayDebugPrint("Unknown strided_slice mode: %s\n", attrs->slice_mode.c_str());
      return -1;
    }
  }

  // TIDL supports slicing along a single dimension.
  // All the other axes must remain intact.
  int32_t slice_axis = -1;

  for (int i = 0; i < ndims; ++i)
  {
    // Look for the slicing axis, and make sure there is only one
    bool intact = begin[i] == 0 && end[i] == dim[axes[i]] && stride[i] == 1;
    if (!intact)
    {
      if (slice_axis == -1)
        slice_axis = i;
      else
      {
        RelayDebugPrint("strided_slice only supported on one axis\n");
        return -1;
      }
    }
  }
  if (slice_axis == -1)
  {
    RelayDebugPrint("can't find axis for strided slice\n");
    return -1;
  }

  // TIDL model checker verifies axis and stride.
  int tidl_axis = TIDL_relayNormalizeAxis(axes[slice_axis], data_dims);
  layer.layerParams.sliceParams.axis = tidl_axis;
  layer.layerParams.sliceParams.stride = stride[slice_axis];
  layer.layerParams.sliceParams.slicePoints[0] = begin[slice_axis];
  layer.layerParams.sliceParams.slicePoints[1] = end[slice_axis];

  return 0;
}

// Import tvm.relay.argmax operator to TIDL and check if attributes are supported
int32_t TIDL_relayMapArgMaxParams(relay::Call& call, sTIDL_LayerPC_t &layer)
{
  int32_t status = -1;
  auto attrs = call->attrs.as<ArgReduceAttrs>();
  int32_t dims = TIDL_relayExprDims(call->args[0]);

  layer.layerType = TIDL_ArgMaxLayer;
  // TIDL supports argmax only on axis 1.
  // Check attributes here because tidlModelCheckLayer() doesn't have access to them
  if(attrs->axis.defined() && attrs->axis.size() == 1)
  {
    int axis = attrs->axis[0].as<IntImmNode>()->value;
    axis = TIDL_relayNormalizeAxis(axis, dims);
    if (axis == TIDL_DIM_NUMCH && attrs->keepdims == true && attrs->exclude == false)
      status = 0;  // No error - this op is supported
  }

  // Return -1 if there are unsupported attributes and consequently
  // TIDL_relayAllowNode will return false (0) to TVM.
  return status;
}

// Import tvm.relay.maximum operator to TIDL
int32_t TIDL_relayMapMaximumParams(relay::Call& call, sTIDL_LayerPC_t &layer)
{
  if (!TIDL_relayCheckShapesEqual(call->args[0], call->args[1]))
    return -1;
  layer.layerType = TIDL_EltWiseLayer;
  layer.layerParams.eltWiseParams.eltWiseType = TIDL_EltWiseMax;
  layer.layerParams.eltWiseParams.numInData = 2;
  layer.numInBufs = 2;
  return 0;
}
// Import tvm.relay.minimum operator to TIDL
int32_t TIDL_relayMapMinimumParams(relay::Call& call, sTIDL_LayerPC_t &layer)
{
  // TIDL supports minimum with a single scalar constant
  if(call->args[1].as<ConstantNode>())
  {
    const runtime::NDArray &constExpr = call->args[1].as<tvm::relay::ConstantNode>()->data;
    const Array<PrimExpr> &constShape = call->args[1]->checked_type().as<TensorTypeNode>()->shape;
    if (constShape.size() != 0)
    {
      RelayDebugPrint("minimum requires scalar constant argument\n");
      return -1;
    }
    layer.layerType = TIDL_MinimumLayer;
    float value;
    constExpr.CopyToBytes(&value, sizeof(float));
    layer.layerPCParams.minimumParams.min = value;
  }
  return 0;
}

// Import tvm.relay.split operator to TIDL
int32_t TIDL_relayMapSplitParams(relay::Call& call, sTIDL_LayerPC_t &layer)
{
  layer.layerType = TIDL_SliceLayer;
  sTIDL_SliceLayerParams_t &sliceParams = layer.layerParams.sliceParams;

  auto tensorType = call->args[0]->checked_type().as<TensorTypeNode>();
  auto attrs = call->attrs.as<SplitAttrs>();
  uint32_t dims = TIDL_relayExprDims(call->args[0]);
  int32_t relay_axis;
  int32_t axis = TIDL_relayNormalizeAxis(attrs->axis, dims, &relay_axis);
  Array<PrimExpr> data_shape = tensorType->shape;
  int dim = data_shape[relay_axis].as<IntImmNode>()->value;

  // The loop at bottom of TIDL_tfOutReshapeSliceLayer expects slice points
  // at both ends, so nsections+1 slice points. For example, to slice
  // an axis in half with a dim value of 10 we need slicepoints (0,5,10).
  int nsections = 0;
  if (const IntImmNode* sections = attrs->indices_or_sections.as<IntImmNode>())
  {
    nsections = sections->value;
    // indices_or_sections is an integer that must evenly divide the axis
    if (nsections == 0 || (dim % nsections) != 0)
    {
      RelayDebugPrint("Unsupported slice - uneven division\n");
      return -1;
    }

    int increment = dim / nsections;
    for (int i = 0; i <= nsections; ++i)  // note <=; iterates nsections+1 times
      sliceParams.slicePoints[i] = i * increment;
  }
  else
  {
    // indices_or_sections is an array that specifies slice points.
    // Add points at 0 and 'dim' if not present.
    auto indices = Downcast<Array<ObjectRef>>(attrs->indices_or_sections);
    nsections = 0;
    if (Downcast<IndexExpr>(indices[0]).as<IntImmNode>()->value != 0)
      sliceParams.slicePoints[nsections++] = 0;
    for (int i = 0; i < indices.size(); ++i)
      sliceParams.slicePoints[nsections++] =
                     Downcast<IndexExpr>(indices[i]).as<IntImmNode>()->value;
    if (sliceParams.slicePoints[nsections-1] != dim)
      sliceParams.slicePoints[nsections++] = dim;
    --nsections;   // adjust for extra slicepoint
  }

  if (nsections > TIDL_NUM_OUT_BUFS)
  {
    RelayDebugPrint("Unsupported slice - too many sections\n");
    return -1;
  }
  sliceParams.axis = axis;
  sliceParams.stride = 1;

  // allocate additional outbufs for this layer
  layer.numOutBufs = nsections;
  for (int i = 1; i < nsections; ++i)
     layer.outData[i].dataId = TIDL_relayImportGetNewDataIndex();

  return 0;
}

// Import tvm.relay.transpose operator to TIDL
int32_t TIDL_relayMapTransposeParams(relay::Call& call, sTIDL_LayerPC_t &layer)
{
  layer.layerType = TIDL_TransposeLayer;

  auto attrs = call->attrs.as<TransposeAttrs>();
  auto axes = attrs->axes;
  int32_t dims = axes.size();

  layer.weights.ptr = (void*)my_malloc(sizeof(int)*dims);
  layer.weights.bufSize = dims;

  int32_t *dimVlaue = (int*)layer.weights.ptr;
  for (int j = 0; j < layer.weights.bufSize; j++)
  {
    dimVlaue[j] = axes[j].IntValue();
  }

  return 0;
}

int32_t TIDL_relayImportNode(relay::Call call, int zp_len, void *zp_ptr,
                             int scale_len, void *scale_ptr)
{
  RelayDebugPrint("In TIDL_relayImportNode: \n");
  const auto* op_node = call->op.as<tvm::OpNode>();
  const auto  op_name = GetRef<tvm::Op>(op_node)->name;
  int         num_args = call->args.size();
  int32_t     *zp = (int32_t *)zp_ptr;
  float       *scale = (float *)scale_ptr;
  RelayDebugPrint("op name: %s, num_args: %d\n", op_name.c_str(), num_args);
  for (int i = 0; i < num_args; i++)
  {
    if (auto tensor = call->args[i]->checked_type().as<TensorTypeNode>())
    {
      std::stringstream dims_stream;
      dims_stream << tensor->shape;
      RelayDebugPrint("  args[%d] dims: %s\n", i, dims_stream.str().c_str());
    }
  }
  RelayDebugPrint("  zp_len=%d, scale_len=%d\n", zp_len, scale_len);
  RelayDebugPrint("  zp[0]=%d, scale[0]=%f\n", zp[0], scale[0]);

  // Get new layerIndex, dataIndex
  int32_t layerIndex = TIDL_relayImportGetNewLayerIndex();
  if (layerIndex < 0)  return -1;
  int32_t dataIndex  = TIDL_relayImportGetNewDataIndex();
  sTIDL_LayerPC_t& layer = orgTIDLNetStructure.TIDLPCLayers[layerIndex];
  // set layer defaults
  layer.numInBufs         = 1;
  layer.numOutBufs        = 1;
  layer.outData[0].dataId = dataIndex;

  int32_t status = TIDL_relayMapNode(call, layer, zp_len, zp, scale_len, scale);
  return status;
}

/*
  Convert a relay call operator to a TIDL layer
  - (zp, scale) are quantization for the layer's output tensor.  They are used
    to import the weights and set activation range.  For floating point models,
    they should always be (0, 1.0f).  AllowNode() also uses (0, 1.0f) and it
    does not affect the correctness of AllowNode() logic.
*/
static int32_t TIDL_relayMapNode(relay::Call& call, sTIDL_LayerPC_t &layer,
        int zp_len /*= 1*/, int *zp /*= nullptr*/,
        int scale_len /*= 1*/, float *scale /*= nullptr*/)
{
  // Limited support of batch_size > 1, uniform, up to TIDL_RELAY_MAX_BATCH_SIZE
  if (auto tensor = call->args[0]->checked_type().as<TensorTypeNode>())
  {
    int batch_size = tensor->shape[0].as<IntImmNode>()->value;
    if (batch_size != 1 && batch_size > tidl_relay_max_batch_size)
    {
      RelayDebugPrint("  args[0]: batch_size=%d not supported!\n", batch_size);
      return -1;
    }
  }

  static int default_zp = 0;
  static float default_scale = 1.0f;
  if (zp == nullptr) zp = &default_zp;
  if (scale == nullptr) scale = &default_scale;

  const auto* op_node = call->op.as<tvm::OpNode>();
  const auto  op_name = GetRef<tvm::Op>(op_node)->name;

  // TODO: based on name, call corresponding Map<name>Params()
  int32_t status = 0;
  if (op_name == "nn.conv2d" || op_name == "qnn.conv2d")
    status = TIDL_relayMapConvParams(call, layer, op_name);
  else if (op_name == "nn.batch_norm")
    status = TIDL_relayMapBatchNormParams(call, layer);
  else if (op_name == "add" || op_name == "qnn.add")
    status = TIDL_relayMapAddParams(call, layer, op_name);
  else if (op_name == "nn.bias_add")
      status = TIDL_relayMapBiasAddParams(call, layer, zp_len, zp,
                                          scale_len, scale);
  else if (op_name == "clip")
    status = TIDL_relayMapReluParams(call, layer, "Relu6");
  else if (op_name == "nn.relu")
    status = TIDL_relayMapReluParams(call, layer, "Relu");
  else if (op_name == "nn.leaky_relu")
    status = TIDL_relayMapReluParams(call, layer, "LeakyRelu");
  else if (op_name == "nn.prelu")
    status = TIDL_relayMapReluParams(call, layer, "PRelu");
  else if (op_name == "sigmoid")
    status = TIDL_relayMapSigmoidParams(call, layer);
  else if (op_name == "qnn.requantize")
    status = TIDL_relayMapRequantizeParams(call, layer);
  else if (op_name == "nn.avg_pool2d")
    status = TIDL_relayMapPoolingParams(call, layer, op_name);
  else if (op_name == "nn.max_pool2d")
    status = TIDL_relayMapPoolingParams(call, layer, op_name);
  else if (op_name == "nn.global_avg_pool2d")
    status = TIDL_relayMapPoolingParams(call, layer, op_name);
  else if (op_name == "multiply" || op_name == "qnn.mul")
    status = TIDL_relayMapMulParams(call, layer, op_name);
  else if (op_name == "divide")
    status = TIDL_relayMapDivParams(call, layer);
  else if (op_name == "nn.softmax")
    status = TIDL_relayMapSoftmaxParams(call, layer);
  else if (op_name == "squeeze")
    status = TIDL_relayMapSqueezeParams(call, layer);
  else if (op_name == "reshape")
    status = TIDL_relayMapReshapeParams(call, layer);
  else if (op_name == "nn.dense" || op_name == "qnn.dense")
    status = TIDL_relayMapDenseParams(call, layer, op_name);
  else if (op_name == "nn.batch_flatten")
    status = TIDL_relayMapBatchFlattenParams(call, layer);
  else if (op_name == "nn.dropout")
    status = TIDL_relayMapDropoutParams(call, layer);
  else if (op_name == "nn.pad")
    status = TIDL_relayMapPadParams(call, layer);
  else if (op_name == "concatenate" || op_name == "qnn.concatenate")
    status = TIDL_relayMapConcatParams(call, layer);
  else if (op_name == "transpose")
    status = TIDL_relayMapTransposeParams(call, layer);
  else if (op_name == "tanh")
    status = TIDL_relayMapTanhParams(call, layer);
  // From tfImport
  else if (op_name == "mean")
    status = TIDL_relayMapMeanParams(call, layer);
  else if (op_name == "image.resize2d")
    status = TIDL_relayMapResize2DParams(call, layer);
  else if (op_name == "shape_of")
    status = TIDL_relayMapShapeParams(call, layer);
  // From tfLiteImport
  else if (op_name == "maximum")
    status = TIDL_relayMapMaximumParams(call, layer);
  else if (op_name == "minimum")
    status = TIDL_relayMapMinimumParams(call, layer);
  else if (op_name == "argmax")
    status = TIDL_relayMapArgMaxParams(call, layer);
  else if (op_name == "strided_slice")
    status = TIDL_relayMapStridedSliceParams(call, layer);
  //From ONNX
  else if (op_name == "split")
    status = TIDL_relayMapSplitParams(call, layer);
  else if (op_name == "nn.upsampling")
    status = TIDL_relayMapResize2DParams(call, layer, "upsample");
  else if (op_name == "nn.conv2d_transpose")
    status = TIDL_relayMapConvTransposeParams(call, layer);
  // Additional ops from quantized models
  else if (op_name == "cast")
    status = TIDL_relayMapCastParams(call, layer);
  else
  {
    RelayDebugPrint("Node is not supported.\n");
    return -1;
  }
  // more layer types are mapped here

  // for quantized model, set activation (output tensor) dynamic range
  // TODO: handle special case of BatchNorm, which outputs a tuple in Relay
  if (auto out_tensor_type = call->checked_type().as<TensorTypeNode>())
  {
    auto out_type = DLDataType2String(out_tensor_type->dtype);
    if (out_type != "float32")
        TIDL_relaySetActivation(zp_len, zp, scale_len, scale, out_type, layer);
  }

  return status;
}

// Import output data layer when a subgraph has multiple outputs.
// This import must be followed by TIDL_relayImportLinkNode()
int32_t TIDL_relayImportOutDataLayer(int num_inputs)
{
  RelayDebugPrint("In TIDL_relayImportOutDataLayer: %d inputs\n", num_inputs);
  // Get new layerIndex, dataIndex
  int32_t layerIndex = TIDL_relayImportGetNewLayerIndex();
  if (layerIndex < 0)  return -1;
  //int32_t dataIndex  = TIDL_relayImportGetNewDataIndex();
  sTIDL_LayerPC_t& layer = orgTIDLNetStructure.TIDLPCLayers[layerIndex];

  layer.layerType         = TIDL_DataLayer;
  layer.numInBufs         = num_inputs;
  layer.numOutBufs        = -1;
  //layer.outData[0].dataId = dataIndex;

  return 0;
}

// use node name in the subgraph as the unique data/tensor name for each node
static void TIDL_relayFillTensorName(int8_t *name, char * node_name,
                                     int suffix_index = 0)
{
  char str[MAX_NAME_LEN];
  if (suffix_index <= 0)
    snprintf(str, MAX_NAME_LEN, "%s", node_name);
  else
    snprintf(str, MAX_NAME_LEN, "%s:%d", node_name, suffix_index);
  strcpy((char *)name, str);
}

/* This function is equivalent to the following tfImport sequence:
*  - tidl_tfLayerFillTensorNames()
*  - tidl_tfLayerUpdateConsumerCount()
*  - tidl_linkInputTensors()
*  - tidl_linkOutputTensors()
*/
int32_t TIDL_relayImportLinkNode(void *in_out_indices)
{
  RelayDebugPrint("In TIDL_relayImportLinkNode: \n");
  int32_t layerIndex = relay_import_state.layerIndex - 1;  // current layer
  sTIDL_LayerPC_t& layer  = orgTIDLNetStructure.TIDLPCLayers[layerIndex];
  InOutNodes_t *in_out = (InOutNodes_t *) in_out_indices;
  char **in_nodes  = (char **) in_out->in_nodes;
  char **out_nodes = (char **) in_out->out_nodes;
  char    str[MAX_NAME_LEN];

  RelayDebugPrint("Layer %d, name: %s, num_ins: %d, num_outs: %d\n", layerIndex,
           in_out->this_node, in_out->num_in_nodes, in_out->num_out_nodes);
  for (int i = 0; i < in_out->num_in_nodes; i++)
    RelayDebugPrint(" in: %s\n", in_nodes[i]);
  for (int i = 0; i < in_out->num_out_nodes; i++)
    RelayDebugPrint(" out: %s\n", out_nodes[i]);

  // Fill in layer data/tensor name. If this node is the last node with single
  // output tensor, "this_node" has the output tensor name provided by Python
  // wrapper.
  TIDL_relayFillTensorName(layer.name, in_out->this_node);

  // fill in input data/tensor names
  if (in_out->num_in_nodes > 0)
  {
    for (int i = 0; i < in_out->num_in_nodes; i++)
    {
      // input nodes can be a Relay call node or Relay var (input tensor)
      TIDL_relayFillTensorName(layer.inDataNames[i], in_nodes[i]);
      RelayDebugPrint("Layer %d, input %d name %s\n",
                      layerIndex, i, (char*)layer.inDataNames[i]);
      layer.inData[i].dataId = -1;

      // update inputDataLayer's outConsumerCnt if this node is an input tensor
      for(int j = 0; j < relay_import_state.numInputDataLayers; j++)
      {
        sTIDL_LayerPC_t& inputLayer = orgTIDLNetStructure.TIDLPCLayers[j];
        if(strcmp((const char*)layer.inDataNames[i],
                  (const char*)inputLayer.outDataNames[0]) == 0)
          inputLayer.outConsumerCnt[0]++;
      }
    }
  }
  else
  {
    printf("\nError: Layer %d doesn't have any inputs!\n", layerIndex);
    return -1;
  }

  // Fill in output data/tensor names - "this_node" has the name of
  // output data/tensor.
  TIDL_relayFillTensorName(layer.outDataNames[0], in_out->this_node);
  if (in_out->num_out_nodes > 0)
  {
    if(layer.numOutBufs != -1)
    {
      // if multiple outputs, subsequent ones named with suffix ":%d"
      for (int i = 1; i < layer.numOutBufs; i++)
        TIDL_relayFillTensorName(layer.outDataNames[i], in_out->this_node, i);
    }
    else
    {
      // This is a tuple node and is the last node of the subgraph, and
      // layer.numOutBufs is set to -1 by TIDL_relayImportOutDataLayer().
      RelayDebugPrint("Layer %d, tuple output node of subgraph\n", layerIndex);
      if(in_out->num_out_nodes != layer.numInBufs)
      {
        // num_out_nodes is the number of output tensors and must be equal to
        // layer.numInBufs, which is the number of inputs to this tuple node and
        // is set by TIDL_relayImportOutDataLayer().
        printf("Error! Numbers of inputs and outputs of the last and tuple "
               "node are different.\n");
        exit(0);
      }

      // Fill in outDataNames with output tensor names in right order:
      // - this_node is not used for outDataNames as other layers,
      // - out_nodes[] holds the output tensor names provided by Python wrapper,
      // - outDataNames[] are to be used by tidl_relayFillOutputTensorNames()
      for(int i=0; i<in_out->num_out_nodes; i++)
        TIDL_relayFillTensorName(layer.outDataNames[i], out_nodes[i]);
    }

    memset(layer.outConsumerLinked, 0, TIDL_NUM_OUT_BUFS * sizeof(int32_t));
  }
  else
  {
    // This is the last node of the subgraph and it is not a tuple node
    RelayDebugPrint("Layer %d, single output node of subgraph\n", layerIndex);
  }

  layer.weightsElementSizeInBits = NUM_WHGT_BITS;
  // If output is tuple, assume each consumer uses all the outputs
  for (int i = 0; i < layer.numOutBufs; i++)
     layer.outConsumerCnt[i] = in_out->num_out_nodes;

  tidl_linkInputTensors( &orgTIDLNetStructure, layerIndex);
  tidl_linkOutputTensors(&orgTIDLNetStructure, layerIndex);

  return 0;
}

static void tidl_relayAddSingleOutDataLayer()
{
  int32_t layerIndex = relay_import_state.layerIndex;
  int32_t addOneLayer = 0;
  sTIDL_LayerPC_t& layer = orgTIDLNetStructure.TIDLPCLayers[layerIndex];

  for (int32_t i = 0; i < layerIndex; i++)
  {
    sTIDL_LayerPC_t& layer_i = orgTIDLNetStructure.TIDLPCLayers[i];
    if (layer_i.layerType != TIDL_DataLayer)
    {
      if (layer_i.outConsumerCnt[0] == 0)
      {
        RelayDebugPrint("AddSingleOutDataLayer for layer %d\n", i);
        layer.layerType  = TIDL_DataLayer;
        layer.numInBufs = 1;
        layer.numOutBufs = -1;
        layer.inData[0].dataId = layer_i.outData[0].dataId;
        memcpy(layer.inDataNames[0], layer_i.outDataNames[0], TIDL_STRING_SIZE);
        memcpy(layer.outDataNames[0], layer.inDataNames[0], TIDL_STRING_SIZE);
        layer_i.outConsumerCnt[0] = 1;
        layer_i.outConsumerLinked[0] = 1;
        addOneLayer = 1;
      }
    }
  }

  relay_import_state.layerIndex += addOneLayer;
}

// This function populates output tensor names to the network structure.
// For each dataId of the output DataLayer's output, replace all inDataNames
// and outDataNames associated with the dataId with the official output
// tensor name stored in output DataLayer's outDataNames
static void tidl_relayFillOutputTensorNames()
{
  int32_t numLayers = relay_import_state.layerIndex;

  for (int i = 0; i < numLayers; i++)
  {
    // Look for any output data layer added by TIDL_relayImportOutDataLayer
    sTIDL_LayerPC_t& layer = orgTIDLNetStructure.TIDLPCLayers[i];
    if (layer.layerType == TIDL_DataLayer && layer.numOutBufs == -1)
    {
      RelayDebugPrint("Found tuple output data layer at layer %d.\n", i);

      // For this output data layer, inDataNames[] contains the old names of
      // input nodes and outDataNames[] contains the new (official) names of
      // output tensors.  Find other layers that has inDataName or outDataName
      // with the same dataId, replace those old names with the new names:
      // OutDataLayer.outDataName (official) --> OutDataLayer.inDataName
      //   --> layer.{inDataName, outDataName} if same dataId
      for(int ind = 0; ind < layer.numInBufs; ind++)
      {
        uint32_t dataId = layer.inData[ind].dataId;
        char *new_name  = (char *) layer.outDataNames[ind];
        strcpy((char *)layer.inDataNames[ind], new_name);

        for(int j = 0; j < numLayers; j++)
        {
          sTIDL_LayerPC_t& layer_j = orgTIDLNetStructure.TIDLPCLayers[j];
          if (layer_j.layerType != TIDL_DataLayer || layer_j.numOutBufs != -1)
          {
            for(int ind_j = 0; ind_j < layer_j.numInBufs; ind_j++)
            {
              if(layer_j.inData[ind_j].dataId == dataId)
                strcpy((char *)layer_j.inDataNames[ind_j], new_name);
            }
            for(int ind_j = 0; ind_j < layer_j.numOutBufs; ind_j++)
            {
              if(layer_j.outData[ind_j].dataId == dataId)
                strcpy((char *)layer_j.outDataNames[ind_j], new_name);
            }
          }
        }
      }
    }
  }
}


int TIDL_readMetaArchInfo2(std::string filePath,int32_t graphOutputSize,int metaType,ODPostProcInfo_t *out)
{
  int32_t  i, j, k, l, m;

  if(metaType != -1)
  {
    if(metaType == TIDL_metaArchTFSSD)
    {
      //TODO: TIDL_metaArchTFSSD
      return -1;
    }
    else
    {
      tidl_meta_arch::TIDLMetaArch  tidlMetaArch;
      TIDL_readProtoFromTextFile((const char *)filePath.c_str(), &tidlMetaArch);
      printf("%s is meta arch name \n",tidlMetaArch.name().c_str());
      for (j = 0; j < tidlMetaArch.caffe_ssd_size(); j++)
      {
        printf("%s\n",tidlMetaArch.caffe_ssd(j).name().c_str());
        m = 0;
        for(k =0; k < tidlMetaArch.caffe_ssd(j).box_input_size(); k++)
        {
          strcpy(out->in_node_names[m], tidlMetaArch.caffe_ssd(j).box_input(k).c_str());
          m++;
        }
        for(k =0; k < tidlMetaArch.caffe_ssd(j).class_input_size(); k++)
        {
          strcpy(out->in_node_names[m], tidlMetaArch.caffe_ssd(j).class_input(k).c_str());
          m++;
        }
        out->num_in_nodes = m;
        if(tidlMetaArch.caffe_ssd(j).output_size() != graphOutputSize)
        {
          printf("\nERROR : Number of output names provided in prototxt - %d - do not match actual number of outputs of OD network - %d \n\n", tidlMetaArch.caffe_ssd(j).output_size(), graphOutputSize);
          exit(-1);
        }
        for(k =0; k < tidlMetaArch.caffe_ssd(j).output_size(); k++)
        {
          strcpy((char *)tidl_OdOutputNames[k],tidlMetaArch.caffe_ssd(j).output(k).c_str());
        }
        std::string framework = tidlMetaArch.caffe_ssd(j).framework();
        int keep_top_k = tidlMetaArch.caffe_ssd(j).detection_output_param().keep_top_k();

        out->num_out_nodes = 2;
        out->out_nodes[0].n = 1;
        out->out_nodes[0].channel = 1;
        out->out_nodes[0].height = keep_top_k;
        out->out_nodes[0].width = 5;
        out->out_nodes[0].element_type = TIDL_SinglePrecFloat;

        out->out_nodes[1].n = 1;
        out->out_nodes[1].channel = 1;
        out->out_nodes[1].height = 1;
        out->out_nodes[1].width = keep_top_k;
        out->out_nodes[1].element_type = TIDL_SinglePrecFloat;
      }

      for (j = 0; j < tidlMetaArch.tidl_retinanet_size(); j++)
      {
        return -1;
        //TODO: support TIDL_metaArchTIDLRetinaNet
      }

      for (j = 0; j < tidlMetaArch.tidl_yolo_size(); j++)
      {
        int num_keypoint, keypoint_confidence;
        if (tidlMetaArch.tidl_yolo(j).detection_output_param().has_num_keypoint()) {
          num_keypoint = tidlMetaArch.tidl_yolo(j).detection_output_param().num_keypoint();
        } else {
          num_keypoint = 0;
        }

        if (tidlMetaArch.tidl_yolo(j).detection_output_param().has_keypoint_confidence()) {
          keypoint_confidence = tidlMetaArch.tidl_yolo(j).detection_output_param().keypoint_confidence();
        } else {
          keypoint_confidence = 0;
        }

        int keep_top_k = tidlMetaArch.tidl_yolo(j).detection_output_param().keep_top_k();
        printf("%s\n",tidlMetaArch.tidl_yolo(j).name().c_str());
        for(k =0; k < tidlMetaArch.tidl_yolo(j).yolo_param_size(); k++)
        {
          strcpy(out->in_node_names[k],tidlMetaArch.tidl_yolo(j).yolo_param(k).input().c_str());
        }
        out->num_in_nodes = k;
        if(tidlMetaArch.tidl_yolo(j).output_size() != graphOutputSize)
        {
          printf("\nERROR : Number of output names provided in prototxt - %d - do not match actual number of outputs of OD network - %d \n\n", tidlMetaArch.tidl_yolo(j).output_size(), graphOutputSize);
          exit(-1);
        }
        for(int k = 0; k < tidlMetaArch.tidl_yolo(j).output_size(); k++)
        {
          strcpy((char *)tidl_OdOutputNames[k],tidlMetaArch.tidl_yolo(j).output(k).c_str());
        }
        out->num_out_nodes = 1;
        out->out_nodes[0].n = 1;
        out->out_nodes[0].channel = 1;
        out->out_nodes[0].height = keep_top_k;
        out->out_nodes[0].width = 6 + num_keypoint*(2+keypoint_confidence);
        out->out_nodes[0].element_type = TIDL_SinglePrecFloat;
      }

      for (j = 0; j < tidlMetaArch.tidl_3dod_size(); j++)
      {
        printf("%s\n",tidlMetaArch.tidl_3dod(j).name().c_str());
        m = 0;
        for(k =0; k < tidlMetaArch.tidl_3dod(j).box_input_size(); k++)
        {
          strcpy(out->in_node_names[m], tidlMetaArch.tidl_3dod(j).box_input(k).c_str());
          m++;
        }
        for(k =0; k < tidlMetaArch.tidl_3dod(j).class_input_size(); k++)
        {
          strcpy(out->in_node_names[m], tidlMetaArch.tidl_3dod(j).class_input(k).c_str());
          m++;
        }
        for(k =0; k < tidlMetaArch.tidl_3dod(j).dir_input_size(); k++)
        {
          strcpy(out->in_node_names[m], tidlMetaArch.tidl_3dod(j).dir_input(k).c_str());
          m++;
        }
        out->num_in_nodes = m;
        if(tidlMetaArch.tidl_3dod(j).output_size() != graphOutputSize)
        {
          printf("\nERROR : Number of output names provided in prototxt - %d - do not match actual number of outputs of OD network - %d \n\n", tidlMetaArch.tidl_yolo(j).output_size(), graphOutputSize);
          exit(-1);
        }
        for(k =0; k < tidlMetaArch.tidl_3dod(j).output_size(); k++)
        {
          strcpy((char *)tidl_OdOutputNames[k],tidlMetaArch.tidl_3dod(j).output(k).c_str());
        }
        int keep_top_k = tidlMetaArch.tidl_3dod(j).detection_output_param().keep_top_k();
        out->num_out_nodes = 1;
        out->out_nodes[0].n = 1;
        out->out_nodes[0].channel = 1;
        out->out_nodes[0].height = keep_top_k;
        out->out_nodes[0].width = 9;
        out->out_nodes[0].element_type = TIDL_SinglePrecFloat;
      }
    }
  }
  return 0;
}


// TODO: need to return names of heads, number of graph outputs and correponding shapes and types
int TIDL_relayGetODMetaArchInfo(int m_meta_arch_type, int m_num_graph_outputs,
                                    std::string m_meta_layers_names_list, void *postproc_info)
{
  RelayDebugPrint("In TIDL_relayGetODMetaArchInfo: \n");
  ODPostProcInfo_t *out = (ODPostProcInfo_t *) postproc_info;

  return TIDL_readMetaArchInfo2(m_meta_layers_names_list, m_num_graph_outputs, m_meta_arch_type, out);
}


// Setup TIDL OD PostProc, real detection layers are added during OptimizeNet()
int32_t TIDL_relaySetupODPostProc(int m_meta_arch_type, int m_num_graph_outputs,
                                  std::string m_meta_layers_names_list)
{
  RelayDebugPrint("In TIDL_relaySetupODPostProc: \n");
  gParams.metaArchType = m_meta_arch_type;
  strcpy((char *) gParams.metaLayersNamesList, m_meta_layers_names_list.c_str());
  tidl_metaArch_import(&gParams);
  for (int i = 0; i < m_num_graph_outputs; i++)
    sprintf((char *)tidl_OdOutputNames[i], "tidl_0_o%d", i);
  return 0;
}


int32_t TIDL_relayOptimizeNet()
{
  RelayDebugPrint("In TIDL_relayOptimizeNet: \n");

  // Populate output tensor names for the case of multiple output tensors
  tidl_relayFillOutputTensorNames();

  // tidl_optimizeNet() assumes that a DataLayer always exist.  Add an
  // outDataLayer for the single output tensor case here.  OutDataLayer for
  // multiple output tensors case are already added by importTupleNode()
  tidl_relayAddSingleOutDataLayer();

  tidl_optimizeNet(orgTIDLNetStructure, relay_import_state.layerIndex,
                   relay_import_state.dataIndex);

  // Prevent OD PostProc layers from being added to non-postprocessing subgraphs
  gParams.metaArchType = -1;

  return 0;
}

int32_t TIDL_relayPostProcessNet(int numFrames, int calibrationOption,
                                 int biasCalibrationIterations,
                                 std::string outputFeature16bitNamesList,
                                 std::string params16bitNamesList,
                                 double mixed_precision_factor,
                                 int model_group_id)
{
  RelayDebugPrint("In TIDL_relayPostProcessNet: \n");
  gParams.numFrames = numFrames;
  gParams.numFramesBiasCalibration = numFrames;
  gParams.calibrationOption = calibrationOption;
  gParams.biasCalibrationIterations = biasCalibrationIterations;
  strcpy((char*) gParams.outputFeature16bitNamesList,
         outputFeature16bitNamesList.c_str());
  strcpy((char*) gParams.params16bitNamesList, params16bitNamesList.c_str());
  gParams.mixedPrecisionFactor = mixed_precision_factor;
  gParams.modelGroupId = model_group_id;
  //TODO_PKS: Understand from where TIDL_relayPostProcessNet is called

  return TIDL_import_backend(relay_import_state.layerIndex);
}

// Called externally from TVM annotation pass to identify nodes that are supported by TIDL.
int32_t TIDL_relayAllowNode(relay::Call call)
{
  RelayDebugPrint("In TIDL_relayAllowNode: \n");
  const auto* op_node = call->op.as<tvm::OpNode>();
  const auto  op_name = GetRef<tvm::Op>(op_node)->name;
  RelayDebugPrint("  name: %s\n", op_name.c_str());
  sTIDL_LayerPC_t layer;
  memset(&layer, 0, sizeof(sTIDL_LayerPC_t));
  strncpy((char *)layer.name, op_name.c_str(), MAX_NAME_LEN);

  // Try to import the relay node as TIDL layer.
  int32_t status = TIDL_relayMapNode(call, layer);
  if (status != 0)
  {
    RelayDebugPrint("  unsupported (import)\n");
    return 0;
  }

  // Run the model check on the layer to see if it's supported
  if (!tidlModelCheckOffload(gParams, layer))
  {
    RelayDebugPrint("  unsupported (model check)\n");
    return 0;
  }

  return 1;
}

// Globally registered functions so that TVM can find and call
TVM_REGISTER_GLOBAL("TIDL_relayInit")
.set_body_typed(TIDL_relayInit);

TVM_REGISTER_GLOBAL("TIDL_relayImportInit")
.set_body_typed(TIDL_relayImportInit);

TVM_REGISTER_GLOBAL("TIDL_relayImportNode")
.set_body_typed(TIDL_relayImportNode);

TVM_REGISTER_GLOBAL("TIDL_relayImportOutDataLayer")
.set_body_typed(TIDL_relayImportOutDataLayer);

TVM_REGISTER_GLOBAL("TIDL_relayImportLinkNode")
.set_body_typed(TIDL_relayImportLinkNode);

TVM_REGISTER_GLOBAL("TIDL_relayGetODMetaArchInfo")
.set_body_typed(TIDL_relayGetODMetaArchInfo);

TVM_REGISTER_GLOBAL("TIDL_relaySetupODPostProc")
.set_body_typed(TIDL_relaySetupODPostProc);

TVM_REGISTER_GLOBAL("TIDL_relayOptimizeNet")
.set_body_typed(TIDL_relayOptimizeNet);

TVM_REGISTER_GLOBAL("TIDL_relayPostProcessNet")
.set_body_typed(TIDL_relayPostProcessNet);

TVM_REGISTER_GLOBAL("TIDL_relayAllowNode")
.set_body_typed(TIDL_relayAllowNode);

