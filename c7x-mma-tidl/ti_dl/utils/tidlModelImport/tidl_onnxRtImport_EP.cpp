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

#define ONNX_ML

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
#include <unistd.h>
#include <fstream>

#include "onnx/onnx-ml.proto3.pb.h"
#include "tidl_onnxRtImport_EP.h"
#include "tidl_runtimes_infer_common.h"
#include "tidl_onnxrt_common.h"
#include "ti_dl.h"

using namespace std;
using namespace onnx;

static TIDL_OnnxrtEPData * data_ = new TIDL_OnnxrtEPData;

extern "C"
{
extern std::vector<std::string> diagsInfo;
}

extern "C"
{

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
  return dims;
}

/* Set default values for options for ONNX runtime */
void TIDL_setDefaultOptions(TIDL_osrtOptions * osrt_options)
{
  osrt_options->m_debug_level                                  = 0;
  osrt_options->m_num_param_bits                               = 8;
  osrt_options->m_num_tidl_subgraphs                           = 16;
  osrt_options->m_calibration_frames                           = 20;
  osrt_options->m_calibration_iterations                       = 50;
  osrt_options->m_mixed_precision_factor                       = -1;
  osrt_options->m_model_group_id                               = 0;
  osrt_options->m_tidl_calibration_flags                       = 7;
  osrt_options->m_quantization_scale_type                      = TIDL_QuantStyleNP2Fixed;
  osrt_options->m_prequantized_model                                     = 0;
  osrt_options->m_high_resolution_optimization                 = 0;
  osrt_options->m_compileConstraintsFlag                       = DEFAULT_COMPILE_CONSTRAINT_NC_FLAGS;
  osrt_options->m_pre_batchnorm_fold                           = 1;
  osrt_options->m_activation_clipping                          = 1;
  osrt_options->m_weight_clipping                              = 1;
  osrt_options->m_bias_calibration                             = 1;
  osrt_options->m_channel_wise_quantization                    = 0;
  osrt_options->m_bias_clipping                                = 0;
  osrt_options->m_add_data_convert_ops                         = 0;
  osrt_options->m_meta_arch_type                               = -1;
  osrt_options->m_inference_mode                               = TIDL_inferenceModeDefault;
  osrt_options->m_num_cores                                    = 1;
  osrt_options->m_quant_params_proto_path                      = "";
  osrt_options->m_artifacts_folder                             = "";
  osrt_options->m_temp_folder                                  = "";
  osrt_options->m_tidl_tools_path                              = "";
  osrt_options->m_model_type                                   = "";
  osrt_options->m_output_feature_16bit_names_list              = "";
  osrt_options->m_params_16bit_names_list                      = "";
  osrt_options->m_meta_layers_names_list                       = "";
  osrt_options->m_deny_list                                    = {};
  osrt_options->m_deny_list_layer_type                         = {};
  osrt_options->m_deny_list_layer_name                         = {};
  osrt_options->m_allow_list_layer_name                        = {};
  osrt_options->m_deny_list_int                                = {};
  osrt_options->m_reserved_ctrl_1                              = {};
  osrt_options->m_single_core_layers_names_list                = "";
}

/* Read interface options and populate to TIDL runtimes option class */
bool TIDL_populateOptions(std::vector<std::pair<std::string,std::string>> interface_options)
{
  data_->infer_ops.lib = dlopen("libvx_tidl_rt.so", RTLD_NOW | RTLD_GLOBAL);
  if(! data_->infer_ops.lib)
  {
    printf("Error -   %s \n", dlerror());
  }
  assert(data_->infer_ops.lib);

  TIDL_rtLoadSymbols(&data_->infer_ops);

  TIDL_OnnxrtEPData * options = data_;
  bool status;

  /* This is needed only for ONNX runtime since data_ is a global variable and gets reused when networks are run back to back -
    can result in incorrect option values being used. The default values in the constructor are set only in the global definition and not here
    TODO : This will work but check if there is a better way to handle this */
  TIDL_setDefaultOptions(&options->osrt_options);

  for(auto option : interface_options)
  {
    status = false;
    auto key = option.first;
    auto value = option.second;

    status = TIDL_readInterfaceOptions(&options->osrt_options, key, value);
    if(!status) delete options;
  }

  TIDL_derivedInterfaceOptions(&options->osrt_options);
  status = TIDL_checkInterfaceOptions(&options->osrt_options);

  if(!status) delete options;

  TIDL_printInterfaceOptions(&options->osrt_options);

  return true;
}
} //extern "C"


/* This function returns the input/output info of subgraph along with vector of node indices included in the subgraph
/* vector( (subgraph1, (inputs_1, outputs_1)), (subgraph2, (inputs_2, outputs_2)), ... ) */
std::vector<std::pair<std::vector<int>, std::pair<std::vector<std::string>, std::vector<std::string>>>> getSubgraphInfo(GraphProto& onnxGraph, std::vector<std::vector<int>> suportedNodeGroups)
{
  std::vector<std::string> nodeOutputs;
  std::vector<std::string> nodeInputs;
  std::pair<std::vector<std::string>, std::vector<std::string>> nodeInputsOutputs;
  std::vector<std::pair<std::vector<int>, std::pair<std::vector<std::string>, std::vector<std::string>>>> info;
  //vector( (subgraph1, (inputs_1, outputs_1)), (subgraph2, (inputs_2, outputs_2)) )

  for(int i = 0; i < suportedNodeGroups.size(); i++)
  {
    std::vector<int> subgraph = suportedNodeGroups[i];
    //Original graph inputs are not read as nodes, so read them separately - check if any of the input node names of subgraph match the original entire graph input names
    // Need this for loop to check for the actual input of the model graph
    for(int m = 0; m < onnxGraph.input_size(); m++)
    {
      //In some OD networks, initializer can be part of graph inputs - 'isInputInitializer' not needed if user is mandated to run onnxruntime/tools/python/remove_initializer_from_input.py
      bool isInputInitializer = false;
      for(int k = 0; k < onnxGraph.initializer_size(); k++)
      {
        if(strcmp(onnxGraph.initializer(k).name().c_str(), onnxGraph.input(m).name().c_str()) == 0)
        {
          isInputInitializer = true;
          break;
        }
      }
      if(! isInputInitializer)
      {
        for(int j = 0; j < subgraph.size(); j++)
        {
          for(int l = 0; l < onnxGraph.node(subgraph[j]).input_size(); l++)
          {
            if(strcmp(onnxGraph.node(subgraph[j]).input(l).c_str(), onnxGraph.input(m).name().c_str()) == 0)
            {
              nodeInputs.push_back(onnxGraph.input(m).name());
            }
          }
        }
      }
    }

    //save all other inputs and outputs of each subgraph
    for(int j = 0; j < subgraph.size(); j++)
    {
      for(int l = 0; l < onnxGraph.node(subgraph[j]).input_size(); l++)
      {
        for (int k = 0; k < onnxGraph.node_size(); k++)
        {
          for(int m = 0; m < onnxGraph.node(k).output_size(); m++)
          {
            if((strcmp(onnxGraph.node(k).output(m).c_str(), onnxGraph.node(subgraph[j]).input(l).c_str()) == 0)) //check for edge inputs(coming from previous layer) and not constant(e.g. weights, biases, etc.)
            {
              nodeInputs.push_back(onnxGraph.node(subgraph[j]).input(l));
            }
          }
        }
      }
    }
    //outputs
    for(int j = 0; j < subgraph.size(); j++)
    {
      for(int l = 0; l < onnxGraph.node(subgraph[j]).output_size(); l++)
      {
        nodeOutputs.push_back(onnxGraph.node(subgraph[j]).output(l));
      }
    }
#if 0
    //delete common elements in inputs and outputs - this removes all intermediate linking inputs/outputs, what is left gives subgraph inputs/outputs
    std::sort(nodeInputs.begin(), nodeInputs.end());
    nodeInputs.erase(std::unique(nodeInputs.begin(), nodeInputs.end()), nodeInputs.end());
    std::sort(nodeOutputs.begin(), nodeOutputs.end());
    nodeOutputs.erase(std::unique(nodeOutputs.begin(), nodeOutputs.end()), nodeOutputs.end());

    bool match;
    for(int i = 0; i < nodeInputs.size(); i++)
    {
      match = false;
      for(int j = 0; j < nodeOutputs.size(); j++)
      {
        if(nodeInputs[i].compare(nodeOutputs[j]) == 0)
        {
          match = true;
          auto itr = std::find(nodeInputs.begin(), nodeInputs.end(), nodeInputs[i]);
          if (itr != nodeInputs.end()) nodeInputs.erase(itr);
          itr = std::find(nodeOutputs.begin(), nodeOutputs.end(), nodeOutputs[j]);
          if (itr != nodeOutputs.end()) nodeOutputs.erase(itr);
          j--;
        }
      }
      if(match)
      {
        i--;
      }
    }
#endif
    nodeInputsOutputs = std::make_pair(nodeInputs, nodeOutputs);
    info.push_back(std::make_pair(subgraph, nodeInputsOutputs));

    nodeInputs.clear();
    nodeOutputs.clear();
  }
#if 0
  printf("info.size() = %d \n", info.size());
  for(int i = 0; i < info.size(); i++)
  {
    printf("**** Subgraph %d *****\n", i);
    std::vector<int> subgraph = info[i].first;
    std::vector<std::string> inputs = info[i].second.first;
    std::vector<std::string> outputs = info[i].second.second;
    for(int j = 0; j < subgraph.size(); j++) printf("%d ", subgraph[j]); printf("\n");
    printf("Inputs --- \n");
    for(int j = 0; j < inputs.size(); j++) printf("%s \n ", inputs[j].c_str());
    printf("Outputs --- \n");
    for(int j = 0; j < outputs.size(); j++) printf("%s \n ", outputs[j].c_str());
  }
#endif
  return info;
}

/** This function takes the preliminary subgraphs created by onnx runtime and checks for further possible optimizations
 * For given subgraph, check if any of the other subgraphs can be merged into it (if all inputs of other subgraph come from outputs of current subgraph)
 * There is no direct way to get actual inputs of a subgraph in onnx runtime, so find inputs of all the nodes, and all of them should either be outputs of the
 * particular subgraph to be merged into, or among node outputs of same subgraph (edges within subgraph)
 * If subgraph can be merged, merge the node indices of this subgraph into the parent subgraph, and delete the merged subgraph
 * Recursively keep doing this till either only 1 subgraph remains or no further subgraphs can be merged
 * */
std::vector<std::vector<int>> optimizeGraphPartition(GraphProto& onnxGraph, std::vector<std::vector<int>> suportedNodeGroups)
{
  std::vector<std::pair<std::vector<int>, std::pair<std::vector<std::string>, std::vector<std::string>>>> info;

  std::vector<int> subgraph_i, subgraph_j;
  std::vector<std::string> inputs_i, inputs_j;
  std::vector<std::string> outputs_i, outputs_j;
  bool canMergeInput, canMergeSubgraph, mergeDone;
  mergeDone = false;
  canMergeSubgraph = false;

  while(mergeDone == false)
  {
    info = getSubgraphInfo(onnxGraph, suportedNodeGroups);
    for(int i = 0; i < info.size(); i++)
    {
      canMergeSubgraph = false;
      subgraph_i = info[i].first;
      inputs_i = info[i].second.first;
      outputs_i = info[i].second.second;
      for(int j = 0; j < info.size(); j++)
      {
        if(j == i) continue; //do not check subgraph with itself
        canMergeSubgraph = true;
        subgraph_j = info[j].first;
        inputs_j = info[j].second.first;
        outputs_j = info[j].second.second;
        for(int k = 0; k < inputs_j.size(); k++)
        {
          canMergeInput = false;
          for(int l = 0; l < outputs_i.size(); l++)
          {
            if(inputs_j[k].compare(outputs_i[l]) == 0)  // "all" inputs should be output of another subgraph, else cannot merge subgraphs
            {
              canMergeInput = true;
              break;
            }
          }
          for(int l = 0; l < outputs_j.size(); l++)
          {
            if(inputs_j[k].compare(outputs_j[l]) == 0)  // if node input is not the output of another subgraph, check if it is part of edge within the subgraph
            {
              canMergeInput = true;
              break;
            }
          }
          if(outputs_j.size() == 0) canMergeInput = false;
          if(canMergeInput == false)
          {
            canMergeSubgraph = false;  //even if any one of the inputs cannot be merged, subgraph can't be merged
            break;
          }
        }
        if(inputs_j.size() == 0) canMergeSubgraph = false;
        if(canMergeSubgraph)
        {
          suportedNodeGroups.clear();
          //put all supported nodes in subgraph_i, then delete subgraph_j
          subgraph_i.insert(subgraph_i.end(), subgraph_j.begin(), subgraph_j.end());
          info[i].first = subgraph_i;
          info.erase(std::find(info.begin(), info.end(), info[j]));
          for(int m = 0; m < info.size(); m++)
          {
            suportedNodeGroups.push_back(info[m].first); //form new set of subgraphs after merging which will be tested recursively
          }
          break;
        }
      }
      if(canMergeSubgraph) break;  //if merge occurs, stop testing current set of subgraphs, and start afresh on new set of merged subgraphs
    }
    if(canMergeSubgraph == false)
    {
      mergeDone = true;   //if none of the subgraphs can be merged into each other, merge is complete
    }
  }
  return suportedNodeGroups;
}

bool TIDL_checkShuffleOperation(GraphProto& onnxGraph, std::vector<int>supportedNodesIndices);
bool TIDL_checkGELUOperation(GraphProto& onnxGraph, std::vector<int>supportedNodesIndices);


/* Supported fused combinations in TIDL - {Operation name : {Layers as part of operation} } */
std::vector<std::pair<std::string, std::vector<std::string>>> fusedCombinations =
{
  {"Shuffle",                           {"Reshape", "Transpose", "Reshape"}},
  {"Pointpillar",                       {"ScatterElements", "Reshape"}},
  /*
    @todo:GELU: removing gelu for individual mapping
  */
  // {"GELU",                              {"Div", "Erf", "Add", "Mul"}}
};

/** After identifying a sequence of layers which can possibly be converted to a supported fused operator
 * add checks for individual layers in this combination to check if they satisfy constraints imposed by the fused operator
  */
std::vector<int> TIDL_checkFusedCombinationSupported(std::vector<std::pair<int, std::vector<int>>> supportedFusedCombinations, GraphProto& onnxGraph)
{
  std::vector<int> supportedNodes = {};
  bool isSupported;
  for(auto & supportedFusedCombination : supportedFusedCombinations)
  {
    isSupported = false;
    if(fusedCombinations[supportedFusedCombination.first].first.compare("Shuffle") == 0) /* Checks for shuffle*/
    {
      isSupported = TIDL_checkShuffleOperation(onnxGraph, supportedFusedCombination.second); /*onnxGraph, supportedNodesIndices*/
    }
    /*
      @todo:GELU: removing gelu for individual mapping
    */
    // else if (fusedCombinations[supportedFusedCombination.first].first.compare("GELU") == 0)  /* checks for GELU*/
    // {
    //   isSupported = TIDL_checkGELUOperation(onnxGraph, supportedFusedCombination.second);
    // }
    else /* Replace this else condition with pointpillars specific check */
    {
      isSupported = true;
    }

    /* If current combination is supported, add corresponding nodes to final supported nodes list */
    if(isSupported)
    {
      for(auto& supportedNode : supportedFusedCombination.second)
      {
        supportedNodes.push_back(supportedNode);
      }
    }
  }
  return supportedNodes;
}

/** Logic for allowlisting possible fusable combinations of layers:
 * Maintain vector fusedCombinations of possible combinations
 * Maintain vector activeChecks of form {{fusedCombinations Index, current check location in fusedCombinations Index}, {supported nodes in this combination till now}}
 * If a layer matches the beginning of a combination, say i, then push {{i, 1}, {current node index}} in activeChecks; if i not already in activeChecks
 * If activeChecks not empty, then check if next location in index i matches next layer - if yes, increment location by 1 and push node in supported list; if
 * doesn't match, remove the vector from activeChecks
 * On reaching end of a combination, push nodes from activeChecks to vector supportedNodes to be returned; and remove the vector from activeChecks
 * */
std::vector<int> TIDL_onnxAllowlistFusedLayers(GraphProto& onnxGraph)
{
    std::vector<int> supportedNodes = {};
    std::vector<std::pair<int, std::vector<int>>> supportedFusedCombinations = {}; /* < Combination idx, {Node indices in combination} > */
    std::vector<std::pair<std::vector<int>, std::vector<int>>> activeChecks = {};

    bool checking = false;

    for (int k = 0; k < onnxGraph.node_size(); k++)
    {
        if(activeChecks.size() > 0)
        {
            for(int i = 0; i < activeChecks.size(); i++)
            {
                if(onnxGraph.node(k).op_type().compare(fusedCombinations[activeChecks[i].first[0]].second[activeChecks[i].first[1]]) == 0)
                {
                    activeChecks[i].first[1] += 1;
                    activeChecks[i].second.push_back(k);
                    if(activeChecks[i].first[1] == fusedCombinations[activeChecks[i].first[0]].second.size()) //we have checked all layers in combination
                    {
                        supportedNodes.insert(supportedNodes.end(), activeChecks[i].second.begin(), activeChecks[i].second.end());
                        supportedFusedCombinations.push_back(std::make_pair(activeChecks[i].first[0], supportedNodes));
                        activeChecks.erase(activeChecks.begin() + i);
                        i--;
                    }
                }
                else
                {
                    activeChecks.erase(activeChecks.begin() + i);
                    i--;
                }
            }
        }
        for(int i = 0; i < fusedCombinations.size(); i++)
        {
            if(onnxGraph.node(k).op_type().compare(fusedCombinations[i].second[0]) == 0)
            {
                checking = false;
                for(int j = 0; j < activeChecks.size(); j++) //check if i already in activeChecks
                {
                    if(i == activeChecks[j].first[0])
                    {
                        checking = true;
                        break;
                    }
                }
                if(!checking)
                {
                    std::vector<int> map = {i,1};
                    std::vector<int> nodes = {k};
                    activeChecks.push_back(std::make_pair(map, nodes));
                }
            }
        }
    }
    return TIDL_checkFusedCombinationSupported(supportedFusedCombinations, onnxGraph);
}

/** Store all information needed for graph visualization like input/output adjacent nodes, node names, etc and
  * write to graphvizInfo.txt, which will be used by graphviz executable for visualization */
void TIDL_getGraphVisualizationInfo(GraphProto& onnxGraph, std::vector<std::vector<int>> outputAdjacencyList, std::vector<std::vector<int>> inputAdjacencyList)
{
  sTIDL_runtimesVisualisationInfo_t * visInfo = new sTIDL_runtimesVisualisationInfo_t[onnxGraph.node_size()];
  for(int i = 0; i < onnxGraph.node_size(); i++)
  {
    visInfo[i].nodeModelIdx = i;
    if(! onnxGraph.node(i).name().empty())
    {
      visInfo[i].nodeName = onnxGraph.node(i).name();
    }
    else
    {
      visInfo[i].nodeName = onnxGraph.node(i).output(0);
    }
    visInfo[i].outputAdjNodes = outputAdjacencyList[i];
    visInfo[i].inputAdjNodes = inputAdjacencyList[i];
    visInfo[i].opType = onnxGraph.node(i).op_type();
    visInfo[i].diagnosticInfo = diagsInfo[i];
  }

  std::ofstream outfile;
  std::string graphvizFileName = data_->osrt_options.m_artifacts_folder + "/tempDir/graphvizInfo.txt";
  outfile.open(graphvizFileName);
  if(outfile.is_open())
  {
    outfile << std::to_string(onnxGraph.node_size()) <<endl;
    for(int i = 0; i < onnxGraph.node_size(); i++)
    {
      outfile << std::to_string(visInfo[i].nodeModelIdx) << " " << visInfo[i].nodeName << " " << visInfo[i].opType << " ";
      int outputsAdjNodesSize = 0;
      int add_output_cnt = 0;
      std::vector<std::string> add_output_name;
      for(auto& adjNode : visInfo[i].outputAdjNodes)
      {
        //Mark -1 added to identify the case in wich an nodes' output is also an input to other nodes and missed as an overall output
        if(adjNode == -1)
        {
          //Total outputAdjNodes size could include mark "-1" and could confuse graphviz. Reduced by one to keep current graphviz code flow
          outputsAdjNodesSize = visInfo[i].outputAdjNodes.size() -1;
        }
        else
        {
          outputsAdjNodesSize = visInfo[i].outputAdjNodes.size();
        }
      }
      if(visInfo[i].outputAdjNodes.size() == 0)
      {
        outfile << "outputAdjNodes " << visInfo[i].outputAdjNodes.size() << " ";
        std::string output_name = onnxGraph.node(i).output(0);
        outfile << output_name << " " ;
      }
      else
      {
        outfile << "outputAdjNodes " << outputsAdjNodesSize << " ";
      }
      //Run twice
      for(auto& adjNode : visInfo[i].outputAdjNodes)
      {
        if(adjNode != -1)
        {
          outfile << adjNode << " ";
        }
        else
        {
          //If a node's output is the input to other nodes and also an overall output, it should be only "1".
          add_output_name.push_back(onnxGraph.node(i).output(0));
          add_output_cnt++;
        }
      }
      outfile << "inputAdjNodes " << visInfo[i].inputAdjNodes.size() << " ";
      if(visInfo[i].inputAdjNodes.size() == 0)
      {
        std::string input_name = onnxGraph.node(i).input(0);
        outfile << input_name << " " ;
      }
      for(auto& adjNode : visInfo[i].inputAdjNodes)
      {
        outfile << adjNode << " ";
      }
      size_t pos=0;
      std::string x="\n";
      std::string y=" ";
      while ((pos=visInfo[i].diagnosticInfo.find(x))!=std::string::npos)
      {
        visInfo[i].diagnosticInfo.replace(pos,x.length(),y);
      }
      outfile << "additionalOutput " << add_output_cnt << " ";
      if(add_output_name.size() != 0)
      {
        for(int i; i<add_output_name.size(); i++)
        {
          outfile << add_output_name[i] << " ";
        }
      }
      outfile << "diagInfo " << visInfo[i].diagnosticInfo;
      outfile << endl;
    }
    outfile.close();
  }
  else
  {
    printf("Warning :: Cannot open %s -- graph visualisation will not work \n", graphvizFileName.c_str());
  }
  delete [] visInfo;
}

/** Check if input is constant/initializer */
int32_t TIDL_isInputConstInGraph(GraphProto& onnGraph, const string name)
{
  int i;
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


int32_t TIDL_isInputConst(std::string * string_buf, const string name)
{
  ModelProto model_proto;
  model_proto.ParseFromString(*string_buf);
  auto onnxGraph = model_proto.graph();
  return (TIDL_isInputConstInGraph(onnxGraph, name));
}

/** Find the names of ONNX graph outputs and concatenate them as part of outList */
int32_t TIDL_findOnnxRtOutputNames(GraphProto&   onnxGraph, char * outList)
{
  int i, j, k, l;
  char tensorName[TIDL_STRING_SIZE];
  char inTensorName[TIDL_STRING_SIZE];
  int outPutSize = 0;
  int node_idx = 0;

  for (i = 0; i < onnxGraph.node_size(); i++)
  {
    outPutSize = onnxGraph.node(i).output_size();
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
        node_idx = i;
        strcat(outList, tensorName);
      }
    }
  }
  return (node_idx);
}

/** Print properties of ONNX graph nodes */
int32_t TIDL_printOnnxGraphProperties(GraphProto&   onnxGraph)
{
  int32_t i;
  for (i = 0; i < onnxGraph.node_size(); i++)
  {
    printf("%3d, %15s, %d, %d, %s, %s\n", i,
    onnxGraph.node(i).op_type().c_str(),
    onnxGraph.node(i).input_size(), onnxGraph.node(i).output_size(),
    onnxGraph.node(i).input(0).c_str(), onnxGraph.node(i).output(0).c_str());
  }
  return 0;
}

char* TIDL_replaceChar(char* string, char c1, char c2, int length)
{
  for (int32_t i = 0; i < length; i++)
  {
    if (string[i] == c1)
        string[i] = c2;
  }
  return string;
}


extern "C"
{

/* Called directly from ONNX runtime : run allowlisting API and return a vector of TIDL supported node groups */
std::vector<std::vector<int>> TIDL_getSupportedNodesImport(std::string& data, int32_t opSetVersion)
{
  ModelProto model_proto;
  model_proto.ParseFromString(data);
  auto onnxGraph = model_proto.graph();

  diagsInfo = {};
  std::vector<std::vector<int>> outputAdjacencyList = {{}};
  std::vector<std::vector<int>> inputAdjacencyList = {{}};
  outputAdjacencyList = TIDL_createOutputAdjacencyList(onnxGraph);
  inputAdjacencyList = TIDL_createInputAdjacencyList(onnxGraph);

  bool isObjectDetectionNetwork = false;
  for (int i = 0; i < onnxGraph.node_size(); i++)
  {
    /** This condition assumes OD network has either "NonMaxSuppression" layer or "TopK" layer
     * If network doesn't have these layers, network will still be identified as OD network and will function fine if meta arch file is provided
     * else it will result in error(this is the case when post processing has to be run on ARM) - this particular case is covered in below Warning message  */
    if((strcmp(onnxGraph.node(i).op_type().c_str(), "NonMaxSuppression") == 0) || (strcmp(onnxGraph.node(i).op_type().c_str(), "TopK") == 0)
      || (data_->osrt_options.m_meta_arch_type != -1)||(strcmp(data_->osrt_options.m_model_type.c_str(),"OD")==0))
    {
      isObjectDetectionNetwork = true;
      break;
    }
  }
  if(! isObjectDetectionNetwork)
  {
    TIDL_osrtDebugPrint(data_->osrt_options.osrtDebugPrintLevel, "\n ****** WARNING : Network not identified as Object Detection network : (1) Ignore if network is not Object Detection network (2) If network is Object Detection network, please specify \"model_type\":\"OD\" as part of OSRT compilation options******\n\n");
  }

  std::vector<int> odBackboneNodeIds = {};
  data_->odBackboneNodeNames = {};
  if(isObjectDetectionNetwork)
  {
    bool odCheckStatus = TIDL_OdOptionChecks(&data_->osrt_options, {TIDL_metaArchTIDLSSD, TIDL_metaArchTIDLYolo, TIDL_metaArchTIDLRetinaNet, TIDL_metaArchTIDLYoloV5, TIDL_metaArchTIDLYoloV8, TIDL_metaArchTIDL3DOD});  //These checks make sense only if network is OD
    if(! odCheckStatus)
    {
      delete data_;
      exit(-1);
    }
    data_->odPostProcHeadNames = TIDL_readMetaArchInfo(data_->osrt_options.m_meta_layers_names_list, onnxGraph.output_size(),data_->osrt_options.m_meta_arch_type);

    std::vector<int> postProcInputIds = TIDL_getPostProcInputIds(onnxGraph, data_->odPostProcHeadNames);

    std::vector<bool> visited;
    visited.assign(onnxGraph.node_size(), false);

    // Run DFS on the graph with OD "heads" as root and traversal towards the network input to get nodes of the backbone network
    odBackboneNodeIds =  TIDL_callNodeTraversal(inputAdjacencyList, postProcInputIds, onnxGraph.node_size());

    printf("Size of odBackboneNodeIds = %d \n", odBackboneNodeIds.size());
    for(int i = 0; i < odBackboneNodeIds.size(); i++)
    {
      data_->odBackboneNodeNames.push_back(onnxGraph.node(odBackboneNodeIds[i]).name()); //convert node indices to corresponding names since indices change after graph partition
    }
  }
  else
  {
    data_->odBackboneNodeNames = {};
    data_->odPostProcHeadNames = {};
    data_->osrt_options.m_meta_arch_type = -1;
    data_->osrt_options.m_meta_layers_names_list = "";
  }


  std::vector<std::vector<int>> suportedNodeGroups;
  std::vector<int> nodeGroup;

  FILE *fp;
  char fileName[500];

  sprintf((char *)fileName, "%s/allowedNode.txt", data_->osrt_options.m_artifacts_folder.c_str());

  fp = fopen(fileName, "w+");
  if(fp == NULL)
  {
    printf("Could not open %s for writing...exiting !\n", fileName);
    exit(-1);
  }

  std::vector<int> supportedFusedNodes = TIDL_onnxAllowlistFusedLayers(onnxGraph);

  bool isSupported = false;
  int32_t i, num_subGraphs = 0;

  for (i = 0; i < onnxGraph.node_size(); i++)
  {
    isSupported = false;
    for(int j = 0; j < supportedFusedNodes.size(); j++)
    {
        if(supportedFusedNodes[j] == i)
        {
            isSupported = true;
            break;
        }
    }
    if(!isSupported)
    {
      isSupported = TIDL_onnxAllowlistNode(onnxGraph, i, &data_->osrt_options, opSetVersion, isObjectDetectionNetwork, odBackboneNodeIds);
    }
    else
    {
      diagsInfo.push_back("");
    }

    if (isSupported)
    {
      nodeGroup.push_back(i);
    }
    else
    {
      if(!nodeGroup.empty())
      {
        suportedNodeGroups.push_back(nodeGroup);
        nodeGroup.clear();
        num_subGraphs++;
      }
    }
  }
  if(!nodeGroup.empty())
  {
    suportedNodeGroups.push_back(nodeGroup);
    nodeGroup.clear();
    num_subGraphs++;
  }

  printf("\nPreliminary subgraphs created = %d \n", suportedNodeGroups.size());

  std::vector<std::vector<int>> suportedNodeGroupsOptimized = optimizeGraphPartition(onnxGraph, suportedNodeGroups);

  if(suportedNodeGroupsOptimized.size() > data_->osrt_options.m_num_tidl_subgraphs)
  {
    printf("\n*** WARNING : Number of subgraphs generated > max_num_subgraphs provided in options - additional subgraphs are delegated to ARM *** \n");
  }

  /* Remove the additional subgraphs > max_num_sibgraphs from the end */
  while(suportedNodeGroupsOptimized.size() > data_->osrt_options.m_num_tidl_subgraphs)
  {
    suportedNodeGroupsOptimized.pop_back();
  }

  int32_t numSuportedNodes = 0;
  fprintf(fp, "%d\n", suportedNodeGroupsOptimized.size());
  for(int i = 0; i < suportedNodeGroupsOptimized.size(); i++)
  {
    std::vector<int> subgraph = suportedNodeGroupsOptimized[i];
    fprintf(fp, "%d\n", subgraph.size());
    for(int j = 0; j < subgraph.size(); j++)
    {
      fprintf(fp, "%d\n", subgraph[j]);
      numSuportedNodes++;
    }
  }

  fclose(fp);
  printf("Final number of subgraphs created are : %d, - Offloaded Nodes - %d, Total Nodes - %d \n", suportedNodeGroupsOptimized.size(), numSuportedNodes, onnxGraph.node_size());

  data_->m_num_subgraphs_created = suportedNodeGroupsOptimized.size();

  if(data_->osrt_options.osrtDebugPrintLevel)
  {
    for(auto &msg : diagsInfo)
    {
      if(! msg.empty())
        printf("%s  \n", msg.c_str());
    }
  }

  std::fstream onnxrtMetaData(data_->osrt_options.m_artifacts_folder + "/onnxrtMetaData.txt", std::ios::app);
  onnxrtMetaData << "numGraphNodes=" << std::to_string(onnxGraph.node_size()) << endl;

  TIDL_getGraphVisualizationInfo(onnxGraph, outputAdjacencyList, inputAdjacencyList);

  TIDL_runGraphvizToolRuntimes(data_->osrt_options.m_tidl_tools_path, data_->osrt_options.m_artifacts_folder, data_->osrt_options.m_debug_level);

  if(suportedNodeGroupsOptimized.empty())
  {
    return {{}};
  }
  else
  {
    return suportedNodeGroupsOptimized;
  }
}

} //extern C

/** Write inputs for each subgraph corresponding to all input frames in float mode in bin file
  * and also store corresponding quantFactors for quantization information */
void TIDL_writeQuantizedInput(onnxRtParams_t * onnxRtParams, char * inputName,
                              int32_t isCurrFrameIdx1, int32_t numParamBits, int32_t inferenceMode, float ** inQuantFactorInput)
{
  if(isCurrFrameIdx1) //remove file at the beginning if it exists, in order to avoid appending contents from previous run
  {
    remove(inputName);
  }
  FILE* fp = fopen(inputName, "ab+");

  int32_t w[16];
  int32_t h[16];
  int32_t c[16];
  int32_t n[16];
  float * inQuantFactor = *inQuantFactorInput;
  float * scratch_mem = NULL;

  if (fp == NULL)
  {
    printf("Could not open file to save the input tensors - %s \n", inputName);
    exit(-1);
  }

  for(int i = 0; i < onnxRtParams->numNetInData; i++)
  {
    //TODO: Need to put if based on tensor element type for quantized models
    if( (onnxRtParams->inputTensorElementType[i] != ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT)  &&
        (onnxRtParams->inputTensorElementType[i] != ONNX_TENSOR_ELEMENT_DATA_TYPE_INT32)  &&
        (onnxRtParams->inputTensorElementType[i] != ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT8)  &&
        (onnxRtParams->inputTensorElementType[i] != ONNX_TENSOR_ELEMENT_DATA_TYPE_INT64) )

    {
      printf("WARNING : input ONNX tensor element type - %d. Only FLOAT, UINT8, INT32 and INT64 inputs supported for ONNX runtime \n", onnxRtParams->inputTensorElementType[i]);
    }
    float* input = (float *)onnxRtParams->inputTensorData[i];

    auto& tensor_shape = onnxRtParams->tensorShape[i];

    if(inferenceMode == TIDL_inferenceModeHighThroughput)
    {
      tensor_shape[0] = 1;
    }

    w[i] = tensor_shape[3];
    h[i] = tensor_shape[2];
    c[i] = tensor_shape[1];
    n[i] = tensor_shape[0];

    size_t tensorSize = w[i] * h[i] * c[i] *n[i];
    if(onnxRtParams->inputTensorElementType[i] == ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT8)
    {
      float* scratch_mem = (float*)malloc(sizeof(float)*tensorSize);
      if(scratch_mem)
      {
        unsigned char * src_ptr = (unsigned char *)onnxRtParams->inputTensorData[i];
        for( int j = 0; j < tensorSize; j++)
        {
          scratch_mem[j] = src_ptr[j];
        }
        input = scratch_mem;
      }
      else
      {
        printf("ERROR :Could NOT allocate Memory in %s, %d\n", __FILE__, __LINE__);
      }
    }
    if(onnxRtParams->inputTensorElementType[i] == ONNX_TENSOR_ELEMENT_DATA_TYPE_INT64)
    {
      float* scratch_mem = (float*)malloc(sizeof(float)*tensorSize);
      if(scratch_mem)
      {
        int64_t * src_ptr = (int64_t *)onnxRtParams->inputTensorData[i];
        for( int j = 0; j < tensorSize; j++)
        {
          scratch_mem[j] = src_ptr[j];
        }
        input = scratch_mem;
      }
      else
      {
        printf("ERROR :Could NOT allocate Memory in %s, %d\n", __FILE__, __LINE__);
      }
    }

    float min, max;
    TIDL_runtimesFindRange((float *)(input), tensor_shape[0], tensor_shape[1], tensor_shape[2], tensor_shape[3], 1.0, 0, min, max);

    fwrite(input, 1, tensorSize * (32 / 8), fp);

    inQuantFactor[i] = TIDL_findMaxQuantizationScale(min, max, numParamBits);

    /* For pointpillars, index input is passed as it is */
    if((onnxRtParams->inputTensorElementType[i] == ONNX_TENSOR_ELEMENT_DATA_TYPE_INT32) ||
       (onnxRtParams->inputTensorElementType[i] == ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT8) ||
       (onnxRtParams->inputTensorElementType[i] == ONNX_TENSOR_ELEMENT_DATA_TYPE_INT64))
    {
      inQuantFactor[i] = 1.0;
    }
    free(scratch_mem);
  }
  fclose(fp);
}

/** First save the inputs to the subgraph corresponding to "m_calibration_frames" number of frames.
  * For the last frame, given all the inputs are now available, run postProcessNet, which runs calibration on the subgraph
  * After the final calibrated and quantized subgraph is saved in net.bin and io.bin files, copy these files from
  * tempDir to the artifacts folder for inference
  */
void TIDL_subgraphImport(onnxRtParams_t * onnxRtParams, TIDL_OnnxrtEPData* options,
                          void * subGraphPtr, char* subGraphName, int32_t currFrameIdx)
{
  if(currFrameIdx <= options->osrt_options.m_calibration_frames) //need to copy input of subgraphs only before calibration is done
  {
    std::string inputName = options->osrt_options.m_temp_folder + "/" + subGraphName + "_calib_raw_data.bin";

    int32_t isCurrFrameIdx1 = (currFrameIdx == 1) ? 1 : 0;
    int32_t numParamBits = options->osrt_options.m_num_param_bits;
    int32_t inferenceMode = options->osrt_options.m_inference_mode;

    float * inQuantFactorCurrTensor = (float *)malloc(16 * sizeof(float));
    memset(inQuantFactorCurrTensor, 0, 16 * sizeof(float));
    TIDL_writeQuantizedInput(onnxRtParams, const_cast<char *>(inputName.c_str()), isCurrFrameIdx1, numParamBits, inferenceMode, &inQuantFactorCurrTensor);

    if((currFrameIdx == options->osrt_options.m_calibration_frames))
    {
      if(numParamBits == 32)
      {
        printf("\n ************ Frame index %d : Running float inference **************** \n", currFrameIdx);
      }
      else
      {
        printf("\n**********  Frame Index %d : Running fixed point mode for calibration **********\n", currFrameIdx);
        TIDL_runtimesPostProcessNet(&options->osrt_options, options->osrt_options.m_calibration_frames, options->osrt_options.m_num_param_bits, subGraphPtr, NULL,
                                      inQuantFactorCurrTensor, NULL, subGraphName);
      }
    }
    else
    {
      printf("\n**********  Frame Index %d : Running float inference **********\n", currFrameIdx);
    }

    if(currFrameIdx == options->osrt_options.m_calibration_frames)
    {
      std::string subGraphId;
      std::stringstream(subGraphName) >> subGraphId;
      TIDL_copyFile(subGraphId + "_tidl_net.bin", options->osrt_options.m_artifacts_folder, options->osrt_options.m_temp_folder);
      TIDL_copyFile(subGraphId + "_tidl_io_1.bin", options->osrt_options.m_artifacts_folder, options->osrt_options.m_temp_folder);
    }
  }
  else
  {
    printf("\n**********  Frame Index %d Running inference - currFrameIdx > numFramesCalibration **********\n", currFrameIdx);
    //No need to run postProcessNet, run inference directly on the saved graph
  }
}

/** Write metadata to be read during inference in order to prevent protobuf read */
void TIDL_writeOnnxRtMetaDataForInference(GraphProto& onnxGraph, OnnxTIDLSubGraphParams * state_subGraph, std::string artifactsFolder, char * outDataNamesList)
{
  std::string serialNumber = std::to_string(state_subGraph->serialNumber);

  std::fstream onnxrtMetaData(artifactsFolder + "/onnxrtMetaData.txt", std::ios::app);
  onnxrtMetaData << serialNumber + ":subGraphName=" <<  state_subGraph->subGraphName_ << endl;
  onnxrtMetaData << serialNumber + ":numInputs=" << state_subGraph->numInputs << endl;
  onnxrtMetaData << serialNumber + ":numOutputs=" << state_subGraph->numOutputs << endl;
  onnxrtMetaData << serialNumber + ":inputIdx=";
  for(int i = 0; i < state_subGraph->numInputs; i++)
  {
    if(i > 0) onnxrtMetaData << ",";
    onnxrtMetaData << state_subGraph->inputIdx[i];
  }
  onnxrtMetaData << endl;

  onnxrtMetaData << serialNumber + ":inDataNames=";
  for(int i = 0; i < state_subGraph->numInputs; i++)
  {
    if(i > 0) onnxrtMetaData << ",";
    onnxrtMetaData << onnxGraph.input(state_subGraph->inputIdx[i]).name().c_str();
  }
  onnxrtMetaData << endl;

  onnxrtMetaData << serialNumber + ":outDataNames=";
  for(int i = 0; i < state_subGraph->numOutputs; i++)
  {
    if(i > 0) onnxrtMetaData << ",";
    onnxrtMetaData << onnxGraph.output(i).name().c_str();
  }
  onnxrtMetaData << endl;
}

extern "C"
{
int32_t TIDLEP_getDdrStats(uint64_t * read, uint64_t * write)
{
  return(data_->infer_ops.TIDLRT_getDdrStats(read, write));
}

/** This function populates onnxRtParams_t and OnnxTIDLSubGraphParams structures
  * Also finds index of input/output in original ONNX graph, save it and save corresponding input/output layer names
  * as part of onnxRtParams_t
  * This function is Init kind of function, and is called only once for each subgraph
  */
void TIDL_createStateImportFunc(OnnxTIDLSubGraphParams * state_subGraph, std::string * string_buf, const std::string node_name)
{
  TIDL_osrtDebugPrint(data_->osrt_options.osrtDebugPrintLevel, "*** In TIDL_createStateImportFunc *** \n");
  onnxRtParams_t * onnxRtParams = &state_subGraph->onnxRtParams;
  state_subGraph->currFrameIdx_ = 0;
  state_subGraph->subGraphPtr_ = NULL;
  state_subGraph->string_buf = string_buf;

  ModelProto model_proto;
  model_proto.ParseFromString(*string_buf);
  auto onnxGraph = model_proto.graph();

  for(int i = 0; i < onnxGraph.output_size(); i++)
  {
    onnxRtParams->outputTensorElementType[i] = onnxGraph.output(i).type().tensor_type().elem_type();
  }

  if(data_->osrt_options.osrtDebugPrintLevel)
  {
    printf("Compute on node : %s\n",  node_name.c_str());
    TIDL_printOnnxGraphProperties(onnxGraph);
  }

  char outDataNamesList[TIDL_MAX_ALG_OUT_BUFS * TIDL_STRING_SIZE] = "";
  TIDL_findOnnxRtOutputNames(onnxGraph, (char*)outDataNamesList);
  strcpy((char*)state_subGraph->subGraphName_, (char*)outDataNamesList);
  strcpy((char*)state_subGraph->subGraphName_, TIDL_replaceChar((char*)state_subGraph->subGraphName_, '/', '_', strlen((const char*)state_subGraph->subGraphName_)));

  int32_t currIdx = 0;
  for (int i = 0; i < onnxGraph.input_size(); i++)
  {
    if (TIDL_isInputConst(string_buf, onnxGraph.input(i).name()))
    {
      continue;
    }
    state_subGraph->inputIdx[currIdx++] = i;
  }
  state_subGraph->numInputs = currIdx;
  state_subGraph->numOutputs = onnxGraph.output_size();

  for (int i = 0; i < state_subGraph->numInputs; i++)
  {
    TIDL_osrtDebugPrint(data_->osrt_options.osrtDebugPrintLevel, "\nInput tensor name -  %s \n", onnxGraph.input(state_subGraph->inputIdx[i]).name().c_str());
    strcpy((char *)onnxRtParams->inDataNames[i],  (char*)onnxGraph.input(state_subGraph->inputIdx[i]).name().c_str());
  }
  for (int i = 0; i < state_subGraph->numOutputs; i++)
  {
    TIDL_osrtDebugPrint(data_->osrt_options.osrtDebugPrintLevel, "Output tensor name - %s \n", onnxGraph.output(i).name().c_str());
    strcpy((char *)onnxRtParams->outDataNames[i],  onnxGraph.output(i).name().c_str());
  }

  TIDL_writeOnnxRtMetaDataForInference(onnxGraph, state_subGraph, data_->osrt_options.m_artifacts_folder, outDataNamesList);

}

/** This function is responsbile for complete import of a subgraph including following steps:
  * Import init - Adding input/output data layers for subgraph and setting their properties, setting default import parameters
  * Mapping framework nodes to TIDL layers and linking all the layers
  * Performing layer level optimization of the network
  * Calibration, quantization and saving final subgraph to net.bin and io.bin
  * ** IMPORTANT POINT TO NOTE **: This function is called for each frame, but subgraph import is done only for the first frame with floating point mode,
  * followed by TIDL RT handle creation for float mode, the reason being we want to save intermediate outputs of all subgraphs in
  * float mode only, so the TIDL RT float mode handle created here (for first frame) is invoked for each frame in "TIDL_computeInvokeFunc"
 */
void TIDL_computeImportFunc(OnnxTIDLSubGraphParams * state_subGraph, std::string * string_buf,int32_t opSetVersion)
{
  ModelProto model_proto;
  model_proto.ParseFromString(*string_buf);

  auto onnxGraph = model_proto.graph();

  // Check if subgraph contains OD post processing part
  bool isSubgraphOd = false;

  /** We need to check whether subgraph is OD only when post processing happens on TIDL(which is when meta arch file is provided)
   * else the post processing part is delegated to ARM and not seen as TIDL subgraph here */
  if(data_->osrt_options.m_meta_arch_type != -1) //meta arch provided
  {
    isSubgraphOd = TIDL_checkIsSubgraphOd(onnxGraph);
  }

  onnxRtParams_t * onnxRtParams = &state_subGraph->onnxRtParams;

  int32_t status;
  state_subGraph->currFrameIdx_++;

  if ((state_subGraph->currFrameIdx_ == 1))
  {
    char outDataNamesList[TIDL_MAX_ALG_OUT_BUFS * TIDL_STRING_SIZE] = "";
    TIDL_findOnnxRtOutputNames(onnxGraph, (char*)outDataNamesList);
    strcpy((char*)state_subGraph->subGraphName_, (char*)outDataNamesList);
    strcpy((char*)state_subGraph->subGraphName_, TIDL_replaceChar((char*)state_subGraph->subGraphName_, '/', '_', strlen((const char*)state_subGraph->subGraphName_)));

    TIDL_onnxRtImportInit(onnxGraph, onnxRtParams, (char*)state_subGraph->subGraphName_, &data_->osrt_options, opSetVersion,
                              data_->odPostProcHeadNames, isSubgraphOd, (std::string* )state_subGraph->string_buf);

    for (int i = 0; i < onnxGraph.node_size(); i++)
    {
      if((data_->odBackboneNodeNames.size() == 0)  //non OD network
          || (std::find(data_->odBackboneNodeNames.begin(), data_->odBackboneNodeNames.end(), onnxGraph.node(i).name()) != data_->odBackboneNodeNames.end()))
      {
        //Map all nodes for non OD network. For OD network, map nodes only if they are part of backbone, do not map the post proc nodes
        TIDL_onnxRtImportAndLinkNode(onnxGraph, i, data_->osrt_options.m_debug_level);
      }
    }
    TIDL_runtimesOptimizeNet(data_->osrt_options.osrtDebugPrintLevel);
    TIDL_saveTidlSubGraph(&state_subGraph->subGraphPtr_);

    std::vector<float32_tidl> qfloat(TIDL_MAX_ALG_IN_BUFS);
    printf("\n ************** Frame index 1 : Running float import ************* \n");

    TIDL_runtimesPostProcessNet(&data_->osrt_options, 1, 32, state_subGraph->subGraphPtr_, NULL, qfloat.data(), NULL, state_subGraph->subGraphName_);

    sTIDL_tidlRtParams_t * subgraphParams = reinterpret_cast<sTIDL_tidlRtParams_t *>(&state_subGraph->tidlRtParams);
    status = TIDL_subgraphRtCreate(data_->osrt_options.m_temp_folder, data_->osrt_options.m_debug_level, data_->osrt_options.osrtDebugPrintLevel,
                                    0, 0, 1, 1, &data_->infer_ops, state_subGraph->subGraphName_, subgraphParams);

  }
}


/** This function calls TIDL RT invoke to perform graph processing - this function is called for each subgraph each frame
  * "TIDL_subgraphRtInvoke" processes the subgraph and computes outputs
  * "TIDL_subgraphImport" utilises these outputs to be saved as inputs for the consumer subgraphs and only for the last frame,
  * runs calibration, performs quantization and saves the final quantized subgraph
  */
void TIDL_computeInvokeFunc(OnnxTIDLSubGraphParams * state_subGraph)
{
  int32_t status;
  if((data_->osrt_options.m_num_param_bits != 32) && (data_->osrt_options.m_prequantized_model == 0))
  {
    /* Floating point pass for subgraph input collection not required in case of execution with numParamBits = 32 */
    status = TIDL_subgraphRtInvoke(data_->osrt_options.osrtDebugPrintLevel, state_subGraph, &data_->infer_ops);
  }
  TIDL_subgraphImport(&state_subGraph->onnxRtParams, data_, state_subGraph->subGraphPtr_, state_subGraph->subGraphName_, state_subGraph->currFrameIdx_);

  if((state_subGraph->serialNumber == (data_->m_num_subgraphs_created-1)) && (state_subGraph->currFrameIdx_ == (data_->osrt_options.m_calibration_frames)))
  {
    TIDL_concatOutNetPrototxtIntoUserProtoOnnx();
  }
}

/** This function is used to free any allocated memory and delete any allocated TIDL RT handles */
void TIDL_releaseRtFunc(OnnxTIDLSubGraphParams * state_subGraph)
{
  delete state_subGraph->subGraphPtr_;
  sTIDL_tidlRtParams_t * subgraphParams = reinterpret_cast<sTIDL_tidlRtParams_t *>(&state_subGraph->tidlRtParams);
  int32_t status = TIDL_subgraphRtDelete(data_->osrt_options.osrtDebugPrintLevel, &data_->infer_ops, subgraphParams);
}

} //extern C
