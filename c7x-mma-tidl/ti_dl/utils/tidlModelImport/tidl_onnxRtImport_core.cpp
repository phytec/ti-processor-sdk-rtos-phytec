/*
*
* Copyright (c) {2015 - 2017} Texas Instruments Incorporated
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
using namespace std;
using ::google::protobuf::Message;
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <cmath>
#include <stdarg.h>

#include "ti_dl.h"
#include "tidl_import_api.h"
#include "tidl_import_config.h"
#include "onnx/onnx-ml.proto3.pb.h"
#include "tidl_custom_import.h"
#include "tidl_meta_arch.pb.h"

using namespace std;
using namespace onnx;

#include "tidl_onnxImport.h"
#include "tidl_onnxRtImport_core.h"
#include "tidl_import_common.h"
#include "tidl_import_common_model_check.h"
#include "itidl_ti.h"
#include "tidl_onnxRtImport_EP.h"
#include "tidl_onnxrt_common.h"


extern uint8_t tidl_OdOutputNames[TIDL_MAX_TF_SSD_LAYERS][TIDL_STRING_SIZE];

extern "C"
{
extern std::vector<std::string> diagsInfo;
}

extern sTIDL_runtimesImportState_t runtimes_import_state;

// Functions

/** This function checks if current subgraph has OD post processing happening as part of it or not */
bool TIDL_checkIsSubgraphOd(GraphProto& onnxGraph)
{
  for (int i = 0; i < onnxGraph.output_size(); i++)
  {
    if(strcmp((char *)tidl_OdOutputNames[0], onnxGraph.output(i).name().c_str()) == 0)
    {
      return true;
    }
  }
  return false;
}

/* This function finds graph indices corresponding to names of OD heads */ 
std::vector<int> TIDL_getPostProcInputIds(GraphProto& onnxGraph, std::vector<std::string> odPostProcIndataNames)
{
  std::vector<int> odPostProcIndataIds = {};
  for(int i = 0; i < odPostProcIndataNames.size(); i++)
  {
    for(int j = 0; j < onnxGraph.node_size(); j++)
    {
      if((strcmp(odPostProcIndataNames[i].c_str(), onnxGraph.node(j).output(0).c_str())) == 0)
      {
        odPostProcIndataIds.push_back(j);
      }
    }
  }
  return odPostProcIndataIds;
}

// Convert a onnx operator to a TIDL layer
static int32_t TIDL_onnxRtMapNode(GraphProto&   onnxGraph, int32_t nodeIdx, sTIDL_LayerPC_t &layer)
{
  int32_t status = 0;
  
  /*** TODO : Add layer mapping using table as in tfliteImport ***/
  if( strcmp(onnxGraph.node(nodeIdx).op_type().c_str(), "Conv") ==0)
  {
    status = TIDL_onnxMapConvBaseParams(onnxGraph, nodeIdx, layer);
  }
  else if( strcmp(onnxGraph.node(nodeIdx).op_type().c_str(), "Relu") ==0)
  {
    status = TIDL_onnxMapReluBaseParams(onnxGraph, nodeIdx, layer);
  }
  else if( strcmp(onnxGraph.node(nodeIdx).op_type().c_str(), "PRelu") ==0)
  {
    status = TIDL_onnxMapPReluBaseParams(onnxGraph, nodeIdx, layer);
  }  
  else if( strcmp(onnxGraph.node(nodeIdx).op_type().c_str(), "LeakyRelu") ==0)
  {
    status = TIDL_onnxMapLeakyReluBaseParams(onnxGraph, nodeIdx, layer);
  }
  else if( (strcmp(onnxGraph.node(nodeIdx).op_type().c_str(), "Identity") ==0) ||
            (strcmp(onnxGraph.node(nodeIdx).op_type().c_str(), "Cast") ==0) ||
            (strcmp(onnxGraph.node(nodeIdx).op_type().c_str(), "Erf") ==0)  )
  {
    status = TIDL_onnxMapIdentityBaseParams(onnxGraph, nodeIdx, layer);
  }
  else if( strcmp(onnxGraph.node(nodeIdx).op_type().c_str(), "Div") ==0)
  {
    status = TIDL_onnxMapDivBaseParams(onnxGraph, nodeIdx, layer);
  }    
  else if( strcmp(onnxGraph.node(nodeIdx).op_type().c_str(), "Clip") ==0)
  {
    status = TIDL_onnxMapClipBaseParams(onnxGraph, nodeIdx, layer);
  }
  else if((strcmp(onnxGraph.node(nodeIdx).op_type().c_str(), "Sigmoid") ==0) ||
           strcmp(onnxGraph.node(nodeIdx).op_type().c_str(), "Logistic") ==0)
  {
    status = TIDL_onnxMapSigmoidBaseParams(onnxGraph, nodeIdx, layer);
  }
  else if(strcmp(onnxGraph.node(nodeIdx).op_type().c_str(), "Tanh") ==0)
  {
    status = TIDL_onnxMapTanhBaseParams(onnxGraph, nodeIdx, layer);
  }
  else if(strcmp(onnxGraph.node(nodeIdx).op_type().c_str(), "HardSigmoid") ==0)
  {
    status = TIDL_onnxMapHardSigmoidBaseParams(onnxGraph, nodeIdx, layer);
  }
  else if(strcmp(onnxGraph.node(nodeIdx).op_type().c_str(), "Elu") ==0)
  {
    status = TIDL_onnxMapELUBaseParams(onnxGraph, nodeIdx, layer);
  }
  else if( strcmp(onnxGraph.node(nodeIdx).op_type().c_str(), "Concat") ==0)
  {
    status = TIDL_onnxMapConcatBaseParams(onnxGraph, nodeIdx, layer);
  }
  else if( (strcmp(onnxGraph.node(nodeIdx).op_type().c_str(), "Add") ==0) ||
           (strcmp(onnxGraph.node(nodeIdx).op_type().c_str(), "Sum") ==0))
  {
    status = TIDL_onnxMapAddBaseParams(onnxGraph, nodeIdx, layer);
  }
  else if( strcmp(onnxGraph.node(nodeIdx).op_type().c_str(), "Sub") ==0)
  {
    status = TIDL_onnxMapSubBaseParams(onnxGraph, nodeIdx, layer);
  }  
  else if( strcmp(onnxGraph.node(nodeIdx).op_type().c_str(), "Div") ==0)
  {
    status = TIDL_onnxMapDivBaseParams(onnxGraph, nodeIdx, layer);
  }
  else if( strcmp(onnxGraph.node(nodeIdx).op_type().c_str(), "MaxPool") ==0)
  {
    status = TIDL_onnxMapMaxPoolBaseParams(onnxGraph, nodeIdx, layer);
  }
  else if( strcmp(onnxGraph.node(nodeIdx).op_type().c_str(), "GlobalAveragePool") ==0)
  {
    status = TIDL_onnxMapGlobalAvgPoolBaseParams(onnxGraph, nodeIdx, layer);
  }
  else if( strcmp(onnxGraph.node(nodeIdx).op_type().c_str(), "AveragePool") ==0)
  {
    status = TIDL_onnxMapAvgPoolBaseParams(onnxGraph, nodeIdx, layer);
  }
  else if( strcmp(onnxGraph.node(nodeIdx).op_type().c_str(), "Upsample") ==0)
  {
    status = TIDL_onnxMapUpsampleBaseParams(onnxGraph, nodeIdx, layer);
  }
  else if( strcmp(onnxGraph.node(nodeIdx).op_type().c_str(), "Resize") ==0)
  {
    status = TIDL_onnxMapResizeBaseParams(onnxGraph, nodeIdx, layer);
  }
  else if( strcmp(onnxGraph.node(nodeIdx).op_type().c_str(), "ConvTranspose") ==0)
  {
    status = TIDL_onnxMapConvTransposeBaseParams(onnxGraph, nodeIdx, layer);
  }
  else if( strcmp(onnxGraph.node(nodeIdx).op_type().c_str(), "Reshape") ==0)
  {
    status = TIDL_onnxMapReshapeBaseParams(onnxGraph, nodeIdx, layer); 
  }
  else if( strcmp(onnxGraph.node(nodeIdx).op_type().c_str(), "Transpose") ==0)
  {
    status = TIDL_onnxMapTransposeBaseParams(onnxGraph, nodeIdx, layer);
  }
  else if( strcmp(onnxGraph.node(nodeIdx).op_type().c_str(), "ArgMax") ==0)
  {
    status = TIDL_onnxMapArgmaxBaseParams(onnxGraph, nodeIdx, layer);
  }
  else if( strcmp(onnxGraph.node(nodeIdx).op_type().c_str(), "BatchNormalization") ==0)
  {
    status = TIDL_onnxMapBNBaseParams(onnxGraph, nodeIdx, layer);
  }
  else if( strcmp(onnxGraph.node(nodeIdx).op_type().c_str(), "Gemm") ==0)
  {
    status = TIDL_onnxGemmBaseParams(onnxGraph, nodeIdx, layer); 
  }
  else if( strcmp(onnxGraph.node(nodeIdx).op_type().c_str(), "Softmax") ==0)
  {
    status = TIDL_onnxMapSoftmaxBaseParams(onnxGraph, nodeIdx, layer); 
  }
  else if( strcmp(onnxGraph.node(nodeIdx).op_type().c_str(), "Split") ==0)
  {
    status = TIDL_onnxMapSplitBaseParams(onnxGraph, nodeIdx, layer); 
  }
  else if( strcmp(onnxGraph.node(nodeIdx).op_type().c_str(), "Slice") ==0)
  {
    status = TIDL_onnxMapSliceBaseParams(onnxGraph, nodeIdx, layer); 
  }
  else if( strcmp(onnxGraph.node(nodeIdx).op_type().c_str(), "Flatten") ==0)
  {
    status = TIDL_onnxMapFlattenBaseParams(onnxGraph, nodeIdx, layer); 
  }
  else if( strcmp(onnxGraph.node(nodeIdx).op_type().c_str(), "Pad") ==0)
  {
    status = TIDL_onnxMapPadBaseParams(onnxGraph, nodeIdx, layer); 
  }
  else if( strcmp(onnxGraph.node(nodeIdx).op_type().c_str(), "Mul") ==0)
  {
    status = TIDL_onnxMapMulBaseParams(onnxGraph, nodeIdx, layer); 
  }
  else if( strcmp(onnxGraph.node(nodeIdx).op_type().c_str(), "DepthToSpace") ==0)
  {
    status = TIDL_onnxMapDepthToSpaceBaseParams(onnxGraph, nodeIdx, layer); 
  }
  else if( strcmp(onnxGraph.node(nodeIdx).op_type().c_str(), "DropOut") ==0)
  {
    status = TIDL_onnxMapDropOutBaseParams(onnxGraph, nodeIdx, layer);
  }
  else if( strcmp(onnxGraph.node(nodeIdx).op_type().c_str(), "Squeeze") ==0)
  {
    status = TIDL_onnxMapSqueezeBaseParams(onnxGraph, nodeIdx, layer);
  }
  else if( strcmp(onnxGraph.node(nodeIdx).op_type().c_str(), "ReduceMax") ==0)
  {
    status = TIDL_onnxMapReduceMaxBaseParams(onnxGraph, nodeIdx, layer);
  }
  else if( strcmp(onnxGraph.node(nodeIdx).op_type().c_str(), "ReduceMin") ==0)
  {
    status = TIDL_onnxMapReduceMinBaseParams(onnxGraph, nodeIdx, layer);
  }
  else if( strcmp(onnxGraph.node(nodeIdx).op_type().c_str(), "ScatterElements") ==0)
  {
    status = TIDL_onnxMapScatterElementsBaseParams(onnxGraph, nodeIdx, layer);
  }
  else if( strcmp(onnxGraph.node(nodeIdx).op_type().c_str(), "ScatterND") ==0)
  {
    status = TIDL_onnxMapScatterNDBaseParams(onnxGraph, nodeIdx, layer);
  }
  else if( strcmp(onnxGraph.node(nodeIdx).op_type().c_str(), "Gather") ==0)
  {
    status = TIDL_onnxMapGatherBaseParams(onnxGraph, nodeIdx, layer);    
  }
  else if( strcmp(onnxGraph.node(nodeIdx).op_type().c_str(), "MatMul") ==0)
  {
    status = TIDL_onnxGemmBaseParams(onnxGraph, nodeIdx, layer);  
  }  
  else if(strcmp(onnxGraph.node(nodeIdx).op_type().c_str(), "DequantizeLinear") ==0)
  {
    status = TIDL_onnxMapDequantizeLinearBaseParams(onnxGraph, nodeIdx, layer);
  }
  else if(strcmp(onnxGraph.node(nodeIdx).op_type().c_str(), "QuantizeLinear") ==0)
  {
    status = TIDL_onnxMapQuantizeLinearBaseParams(onnxGraph, nodeIdx, layer);
  }
  else if (strcmp(onnxGraph.node(nodeIdx).op_type().c_str(), "ReduceMean") ==0)
  {
    status = TIDL_onnxMapReduceMeanBaseParams(onnxGraph, nodeIdx, layer);
  }
  else if (strcmp(onnxGraph.node(nodeIdx).op_type().c_str(), "Pow") ==0)
  {
    status = TIDL_onnxMapPowBaseParams(onnxGraph, nodeIdx, layer);
  }
  else if (strcmp(onnxGraph.node(nodeIdx).op_type().c_str(), "Sqrt") ==0)
  {
    status = TIDL_onnxMapSqrtBaseParams(onnxGraph, nodeIdx, layer);
  }
  else
  {
    return TIDL_ALLOWLISTING_LAYER_TYPE_UNSUPPORTED;
  }
  return status;
}
extern int64_t onnxOpSetVersion;

/* For a layer, TIDL does not support if input has more than TIDL_DIM_MAX dimensions */
int32_t TIDL_checkLayerInputDimConstraints(GraphProto& onnGraph, int32_t layerIdx)
{
  std::vector<int32_t> nodeInputDims = getNodeInputShape(onnGraph,  onnGraph.node(layerIdx).input(0), 0);
  if(nodeInputDims.size() > TIDL_DIM_MAX) //layer supported only if number of dimensions of input <= TIDL_DIM_MAX
  {
    return -1;
  }
  return 0;
}

/* Identify nodes that are supported by TIDL */
int32_t TIDL_onnxAllowlistNode(GraphProto&   onnxGraph, int32_t i, TIDL_osrtOptions * osrtOptions, int32_t opSetVersion, bool isObjectDetectionNetwork, std::vector<int> odProcNodesComplement)
{
  int32_t debugLevel = osrtOptions->m_debug_level;
  int32_t osrtDebugPrintLevel = osrtOptions->osrtDebugPrintLevel;
  std::vector<std::string> denyList = osrtOptions->m_deny_list;
  std::vector<std::string> denyList_layer_type = osrtOptions->m_deny_list_layer_type;
  std::vector<std::string> denyList_layer_name = osrtOptions->m_deny_list_layer_name;
  std::vector<std::string> allowList_layer_name = osrtOptions->m_allow_list_layer_name;

  int32_t status = -1;
  sTIDL_LayerPC_t layer;
  memset(&layer, 0, sizeof(sTIDL_LayerPC_t));

  /* If allowlist is specified, all layers which are not in allow-list must be delegated to ARM */
  if(allowList_layer_name.size() > 0)
  {
    int32_t isNodeInAllowListName = 0;
    for(int k = 0; k < allowList_layer_name.size(); k++)
    {
      std::string onnxNodeName = "";
      if(onnxGraph.node(i).name() != "")
      {
        onnxNodeName = onnxGraph.node(i).name();
      } 
      else
      {
        onnxNodeName = onnxGraph.node(i).output(0);
      }
      if(onnxNodeName == allowList_layer_name[k])
      {
        isNodeInAllowListName = 1;
        break;
      }
    }
    if(isNodeInAllowListName == 0)
    {
      TIDL_osrtDebugPrint(osrtDebugPrintLevel, "Op type '%s'  added to unsupported nodes since it is not part of allow list specified in runtime options \n", onnxGraph.node(i).op_type().c_str());
      diagsInfo.push_back("Node not in allow list...delegated to ARM --- layer type - " + onnxGraph.node(i).op_type() + ", Node name - " + onnxGraph.node(i).name());
      return 0;
    }
  }
  
  int32_t isNodeInDenyList = 0;                      //option for Deny List
  for(int j = 0; j < denyList.size(); j++)
  {
    if(onnxGraph.node(i).op_type().c_str() == denyList[j])
    {
      isNodeInDenyList = 1;
      break;
    }
  }


  int32_t isNodeInDenyListName = 0;                   //option for Deny List Name
  for(int k = 0; k < denyList_layer_name.size(); k++)
  {
    std::string onnxNodeName = "";
    if(onnxGraph.node(i).name() != "")
    {
      onnxNodeName = onnxGraph.node(i).name();
    } 
    else
    {
      onnxNodeName = onnxGraph.node(i).output(0);
    }
    if(onnxNodeName == denyList_layer_name[k])
    {
      isNodeInDenyListName = 1;
      break;
    }
  }
  int32_t isNodeInDenyListLayer = 0;                  //option for Deny List Type
  for(int j = 0; j < denyList_layer_type.size(); j++)
  {
    if(onnxGraph.node(i).op_type().c_str() == denyList_layer_type[j])
    { 
      isNodeInDenyListLayer = 1;
      break;
    }
  }

  if(isNodeInDenyList || isNodeInDenyListLayer || isNodeInDenyListName)
  {
    TIDL_osrtDebugPrint(osrtDebugPrintLevel, "Op type '%s'  added to unsupported nodes as specified in deny list \n", onnxGraph.node(i).op_type().c_str());
    diagsInfo.push_back("Node in deny list...delegated to ARM --- layer type - " + onnxGraph.node(i).op_type() + ", Node name - " + onnxGraph.node(i).name());
    return 0;
  }
  if(isObjectDetectionNetwork)
  {
    if(odProcNodesComplement.size() == 0) //means ARM mode, allowlist based on numDims != 4
    {
      std::vector<int32_t> nodeInputDims = getNodeInputShape(onnxGraph,  onnxGraph.node(i).input(0), debugLevel);

      int32_t numDims = nodeInputDims.size();

      TIDL_osrtDebugPrint(osrtDebugPrintLevel, "Layer %d -- layer name -- %s \n Input dims size = %d     dims --- ", i, onnxGraph.node(i).name().c_str(), numDims);
      for(int i = 0; i < numDims; i++)
      {
        TIDL_osrtDebugPrint(osrtDebugPrintLevel, "%d   ", nodeInputDims[i]);
      }
      TIDL_osrtDebugPrint(osrtDebugPrintLevel, "\n");

      if(TIDL_checkLayerInputDimConstraints(onnxGraph, i) == -1)
      {
        TIDL_osrtDebugPrint(osrtDebugPrintLevel, "Layer %d --- op type -  %s,   Number of input dims %d  !=  %d .. not supported by TIDL \n", i, onnxGraph.node(i).op_type().c_str(), numDims, TIDL_DIM_MAX);
        diagsInfo.push_back("Number of input dims > 6 not supported for OD networks");
        return 0;
      }
    }
    else if(std::find(odProcNodesComplement.begin(), odProcNodesComplement.end(), i) == odProcNodesComplement.end()) //if node is not part of backbone network, mark it supported
    {
      diagsInfo.push_back("");
      return 1;
    }
  }

  if(TIDL_checkLayerInputDimConstraints(onnxGraph, i) == -1) //if layer input has more than accepted dimensions, not supported
  {
    TIDL_osrtDebugPrint(osrtDebugPrintLevel, 
    "Unsupported layer %d --- op type -  %s,   Number of input dims > %d .. not supported by TIDL \n", i, onnxGraph.node(i).op_type().c_str(), TIDL_DIM_MAX);
    
    diagsInfo.push_back("Number of input dims > 6 .. not supported by TIDL");
    return 0;
  }

  onnxOpSetVersion = opSetVersion;

  /* Populating gParams required by model checker */
  gParams.modelType = TIDL_IMPORT_MODEL_FORMAT_ONNX_RT; 
  gParams.numParamBits = osrtOptions->m_num_param_bits;
  char * tidl_tools_path = const_cast<char *>(osrtOptions->m_tidl_tools_path.c_str());
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

  /* Device specific allow-listing checks require deviceName to be populated */
  TIDL_readDeviceConfigParams((char*)gParams.perfSimConfig);


  status = TIDL_onnxRtMapNode(onnxGraph, i, layer);

  if (status == TIDL_ALLOWLISTING_LAYER_TYPE_UNSUPPORTED)
  {
    TIDL_osrtDebugPrint(osrtDebugPrintLevel, "Unsupported (imports) TIDL layer type for ONNX op type --- %s \n", onnxGraph.node(i).op_type().c_str());
    diagsInfo.push_back("Layers type not supported by TIDL --- layer type - " + onnxGraph.node(i).op_type() + ",  Node name -" + onnxGraph.node(i).name());
    return 0;
  }
  if(status == TIDL_ALLOWLISTING_LAYER_CHECK_FAILED)
  {
    TIDL_osrtDebugPrint(osrtDebugPrintLevel, "Unsupported (TIDL check) TIDL layer type --- %15s \n", onnxGraph.node(i).op_type().c_str());
    return 0;
  }

  // Run the model check on the layer to see if it's supported  
  if (!tidlModelCheckOffload(gParams, layer))
  {
    TIDL_osrtDebugPrint(osrtDebugPrintLevel, "Unsupported (TIDL check) TIDL layer type --- %15s \n", onnxGraph.node(i).op_type().c_str());
    return 0;
  }
  TIDL_osrtDebugPrint(osrtDebugPrintLevel, "Supported TIDL layer type --- %15s -- %s \n", onnxGraph.node(i).op_type().c_str(), onnxGraph.node(i).name().c_str());
  return 1;
}

/** Create adjacency list of inputs of all nodes in graph
 * Returns list of form < (input node x1, input node y1...), (input node x2, input node y2...) > for nodes < node 1, node 2....>
 * */
std::vector<std::vector<int>> TIDL_createInputAdjacencyList(GraphProto&   onnxGraph)
{
  std::vector<int> inputAdjacentNodes = {};
  std::vector<std::vector<int>> adjacencyList;
  for(int i = 0; i < onnxGraph.node_size(); i++)
  {
    inputAdjacentNodes.clear();
    for(int j = 0; j < onnxGraph.node(i).input_size(); j++)
    {
      for(int k = 0; k < onnxGraph.node_size(); k++)
      {
        for(int l = 0; l < onnxGraph.node(k).output_size(); l++)
        {
          if(strcmp(onnxGraph.node(i).input(j).c_str(), onnxGraph.node(k).output(l).c_str()) == 0)
          {
            inputAdjacentNodes.push_back(k);
          }
        }
      }
    }
    adjacencyList.push_back(inputAdjacentNodes);
  }
  return adjacencyList;
}

/** Create adjacency list of outputs of all nodes in graph
 * Returns list of form < (input node x1, input node y1...), (input node x2, input node y2...) > for nodes < node 1, node 2....>
 * */
std::vector<std::vector<int>> TIDL_createOutputAdjacencyList(GraphProto& onnxGraph)
{
  std::vector<int> outputAdjacentNodes = {};
  std::vector<std::vector<int>> adjacencyList;
  for(int i = 0; i < onnxGraph.node_size(); i++)
  {
    outputAdjacentNodes.clear();
    int foundMatch =0;
    for(int j = 0; j < onnxGraph.node(i).output_size(); j++)
    {      
      for(int k = 0; k < onnxGraph.node_size(); k++)
      {
        for(int l = 0; l < onnxGraph.node(k).input_size(); l++)
        {          
          if(strcmp(onnxGraph.node(i).output(j).c_str(), onnxGraph.node(k).input(l).c_str()) == 0)
          {
            outputAdjacentNodes.push_back(k);
            foundMatch++;
            break;
          }
        }
      }
      //PC-- catching missed outputs - Case: overall output which is also input to another node(s)
      for(int m = 0; m < onnxGraph.output().size(); m++)
      {
        if((strcmp(onnxGraph.output(m).name().c_str(), onnxGraph.node(i).output(j).c_str()) == 0) && (foundMatch != 0))
        {
          //TIDL_osrtDebugPrint(1, "PC-- GRAPH OUTPUT onnxGraph.node(%d).output(%d).c_str() = %s.\n",i,j,onnxGraph.node(i).output(j).c_str());
          outputAdjacentNodes.push_back(-1);
        }
      }
    }
    adjacencyList.push_back(outputAdjacentNodes);
  }
  return adjacencyList;
}

/** This function adds input and output data layers to subgraphs and sets their properties
Also validates the import paramters and does meta architecture import for OD models with meta arch file provided*/
int32_t TIDL_onnxRtImportInit(GraphProto& onnxGraph, onnxRtParams_t *onnxRtParams, char* subgraph_name, TIDL_osrtOptions * osrt_options, int32_t opSetVersion, 
                                std::vector<std::string>outDataOd, bool isSubgraphOD, std::string * string_buf)
{
  int32_t i;
  int32_t status, elementSize;
  TIDL_osrtDebugPrint(osrt_options->osrtDebugPrintLevel, "In TIDL_onnxRtImportInit subgraph_name=%s\n", subgraph_name);
  // Reset all the memories to to NULL, there could be multiple subgraphs
  memset(&orgTIDLNetStructure, 0, sizeof(sTIDL_OrgNetwork_t));
  memset(&tIDLNetStructure,    0, sizeof(sTIDL_Network_t));
  runtimes_import_state.layerIndex = 0;
  runtimes_import_state.dataIndex  = 0;
  gParams.onnxProto = string_buf;
  
  // Initialize layer independent parameters of gParams
  onnxOpSetVersion = opSetVersion;
  TIDL_runtimesGparamsInit(osrt_options, TIDL_IMPORT_MODEL_FORMAT_ONNX_RT);
  
  sprintf((char *)gParams.metaLayersNamesList, osrt_options->m_meta_layers_names_list.c_str());
  
  if((outDataOd.size() == 0) || (! isSubgraphOD)) //Not an OD network / Subgaph does not contain OD post processing part
  {
    gParams.metaArchType = -1;
    numNetOutData = onnxRtParams->numNetOutData ;
    for (i = 0; i < numNetOutData; i++) 
    {
      int32_t layerIndex = TIDL_addOutputDataLayer(i, (char *)onnxRtParams->outDataNames[i]);
      TIDL_osrtDebugPrint(osrt_options->osrtDebugPrintLevel, "Layer %d, subgraph id %s, name=%s\n", layerIndex, subgraph_name, (char*)onnxRtParams->outDataNames[i]);
      
      if(gParams.addDataConvertToNet & ADD_DC_LAYER_AT_OUTPUT)
      {
        gParams.outTensorScale[i] = 1.0;
        gParams.outZeroPoint[i] = 0.0;
        status = TIDL_ortGetType(onnxRtParams->outputTensorElementType[i], &gParams.outElementType[i], &elementSize);
        if(status != 0)
        {
          return status;
        }
      }
    }
  }
  else
  {
    gParams.metaArchType = osrt_options->m_meta_arch_type;
    numNetOutData = outDataOd.size() ; //The OD heads are treated as outputs, since meta arch import assumes output data layers at heads
    for (i = 0; i < numNetOutData; i++) 
    {
      int32_t layerIndex = TIDL_addOutputDataLayer(i, (char*)outDataOd[i].c_str());

      TIDL_osrtDebugPrint(osrt_options->osrtDebugPrintLevel, "Layer %d, subgraph id %s, name=%s\n", layerIndex, subgraph_name, (char*)outDataOd[i].c_str());
      if(gParams.addDataConvertToNet & ADD_DC_LAYER_AT_OUTPUT)
      {
        gParams.outTensorScale[i] = 1.0;
        gParams.outZeroPoint[i] = 0.0;
        gParams.outElementType[i] = TIDL_SinglePrecFloat; /* conv heads will have float output */
      }
    }
  }

  for (i = 0; i < onnxRtParams->numNetInData; i++) 
  {
    status = TIDL_ortGetType(onnxRtParams->inputTensorElementType[i], &gParams.inElementType[i], &elementSize);
    int32_t layerIndex = TIDL_addInputDataLayer(i, onnxRtParams->tensorShape[i][0], onnxRtParams->tensorShape[i][1], onnxRtParams->tensorShape[i][2], onnxRtParams->tensorShape[i][3],
                                                     (char*)onnxRtParams->inDataNames[i]);
    TIDL_ortUpdateType(&gParams.inElementType[i]);
    if(status != 0)
    {
      return status;
    }
    if(gParams.addDataConvertToNet & ADD_DC_LAYER_AT_INPUT)
    {
      gParams.inQuantFactor[i] = 1.0;
      gParams.inZeroPoint[i] = 0.0;
    }
    else
    {
      if(gParams.inElementType[i] != TIDL_SignedWord && gParams.inElementType[i] != TIDL_SignedDoubleWord) //For pointpillars, keep the inElementType same (int32) for indices
      {
        if(gParams.numFeatureBits == 32)
        {
          gParams.inElementType[i] = TIDL_SinglePrecFloat;
        }
      }
      if(gParams.inElementType[i] == TIDL_SignedDoubleWord)
      {
        // indices can be 64-bit for scatter/gather, tidl doesn't support 64-bit indices, hence convert them to 32-bit
        gParams.inElementType[i] = TIDL_SignedWord;
      }
    }

    tidl_onnxLayerUpdateConsumerCount(&orgTIDLNetStructure, layerIndex, layerIndex, onnxGraph);
    tidl_linkOutputTensors(&orgTIDLNetStructure, layerIndex);
    TIDL_osrtDebugPrint(osrt_options->osrtDebugPrintLevel, "Layer %d, subgraph id %s, name=%s\n", layerIndex, subgraph_name, (char*)onnxRtParams->inDataNames[i]);
  }

  // Initialize rest of the layers
  for (int i = onnxRtParams->numNetInData; i < TIDL_NUM_MAX_PC_LAYERS; i++)
  {
    sTIDL_LayerPC_t& layer_i = orgTIDLNetStructure.TIDLPCLayers[i];
    layer_i.actParams.actType  = TIDL_NoAct;
    layer_i.strideOffsetMethod = TIDL_StrideOffsetTopLeft;
  }
  
  /* Subgraph inputs are saved in raw file in float data format with corresponding quantization scales */
  for (int i = 0; i < TIDL_MAX_ALG_IN_BUFS; i++)
  {
    gParams.rawDataInElementType[i]   = TIDL_SinglePrecFloat;
  }

  if (tidlValidateImportParams(&gParams) == -1)
  {
    printf("Validation of TIDL ONNX runtime import config parameters failed!\n");
    return -1;
  }

  if(gParams.metaArchType != -1)
  {
    tidl_metaArch_import(&gParams);
  }

  return 0;
}

/** This function maps ONNX nodes to TIDL nodes and links them to form TIDL imported graph */
int32_t TIDL_onnxRtImportAndLinkNode(GraphProto&   onnxGraph, int32_t nodeIdx, int32_t debugLevel)
{
  int32_t status = 0;

  // Get new layerIndex, dataIndex
  int32_t layerIndex = runtimes_import_state.layerIndex++;
  sTIDL_LayerPC_t& layer = orgTIDLNetStructure.TIDLPCLayers[layerIndex];
  // set layer defaults
  layer.numInBufs  = 1;
  layer.numOutBufs = onnxGraph.node(nodeIdx).output_size(); 
  for (int j = 0; j < layer.numOutBufs; j++)
  {
    int32_t dataIndex  = runtimes_import_state.dataIndex++;
    layer.outData[j].dataId = dataIndex;
  }
  
  status = TIDL_onnxRtMapNode(onnxGraph, nodeIdx, layer);
  tidl_onnxLayerFillTensorNames(&orgTIDLNetStructure, nodeIdx, layerIndex, onnxGraph);
  tidl_onnxLayerUpdateConsumerCount(&orgTIDLNetStructure, nodeIdx, layerIndex, onnxGraph);
  tidl_linkInputTensors(&orgTIDLNetStructure, layerIndex);
  tidl_linkOutputTensors(&orgTIDLNetStructure, layerIndex);

  return status;
}

void TIDL_concatOutNetPrototxtIntoUserProtoOnnx()
{
  if(gParams.isQuantParamsToBeExported)
  {
    TIDL_exportQuantParamsIntoUserPath(&gParams);
  }
}

