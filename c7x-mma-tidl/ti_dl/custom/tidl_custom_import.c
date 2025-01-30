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
 *        No reverse engineering, decompilation, or disassembly of this software is
 * permitted with respect to any software provided in binary form.
 *
 *        any redistribution and use are licensed by TI for use only with TI Devices.
 *
 *        Nothing shall obligate TI to provide you with source code for the software
 * licensed and provided to you in object code.
 *
 * If software source code is provided to you, modification and redistribution of the
 * source code are permitted provided that the following conditions are met:
 *
 *        any redistribution and use of the source code, including any resulting derivative
 * works, are licensed by TI for use only with TI Devices.
 *
 *        any redistribution and use of any object code compiled from the source code
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

/**
 *  \file tidl_custom_import.c
 *
 *  \brief File containing the APIs needed for importing custom layer.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <stdio.h>
#include <string.h>
#include "tidl_import_api.h"

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/message.h>
#include <google/protobuf/text_format.h>
#include "tensorflow/core/framework/graph.pb.h"
#include "onnx/onnx-ml.proto3.pb.h"
#include "caffe.pb.h"
#include "schema_generated.h"

using namespace std;
using namespace tensorflow;
using ::google::protobuf::Message;
using ::google::protobuf::io::FileInputStream;
using ::google::protobuf::io::FileOutputStream;
using ::google::protobuf::io::ZeroCopyInputStream;
using ::google::protobuf::io::CodedInputStream;
using ::google::protobuf::io::ZeroCopyOutputStream;
using ::google::protobuf::io::CodedOutputStream;
//#include "tidl_import_common.h"
#include "tidl_import_include.h"


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                  Internal/Private Function Declarations                    */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

extern int32_t gloab_data_format;

int32_t TIDL_tfOutReshapeCustomLayer(sTIDL_OrgNetwork_t *pOrgTIDLNetStructure,
                                     int32_t layerIndex)
{
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  sTIDL_CustomParams_t &customParams = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].layerParams.customParams;
  if(TIDL_CUSTOM_TYPE_0 == customParams.customLayerType)
  {
    TIDL_CustomParams0_t *custom0Params = (TIDL_CustomParams0_t *)(TIDLPCLayers.weights.ptr);

    TIDLPCLayers.outData[0].elementType = TIDLPCLayers.inData[0].elementType;
    TIDLPCLayers.outData[0].numDim = TIDLPCLayers.inData[0].numDim;
    TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM1] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_DIM1];
    TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM2] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_DIM2];
    if (custom0Params->kernelH > 0 || custom0Params->kernelW > 0)
    {
      TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_BATCH];
      TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_NUMCH];
      if(custom0Params->useCeil)
      {
        TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] = ceil(((TIDLPCLayers.inData[0].dimValues[TIDL_DIM_HEIGHT] +
          custom0Params->padH*2.0) - (custom0Params->kernelH)) / custom0Params->strideH) + 1;
        TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH] = ceil(((TIDLPCLayers.inData[0].dimValues[TIDL_DIM_WIDTH] +
          custom0Params->padW*2.0) - (custom0Params->kernelW)) / custom0Params->strideW) + 1;
      }
      else
      {
        TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] = floor(((TIDLPCLayers.inData[0].dimValues[TIDL_DIM_HEIGHT] +
          custom0Params->padH*2.0) - (custom0Params->kernelH)) / custom0Params->strideH) + 1;
        TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH] = floor(((TIDLPCLayers.inData[0].dimValues[TIDL_DIM_WIDTH] +
          custom0Params->padW*2.0) - (custom0Params->kernelW)) / custom0Params->strideW) + 1;
      }

      TIDLPCLayers.numMacs =
        (int64_t)((int64_t)TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] *
          TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH] *
          custom0Params->kernelW *custom0Params->kernelH);
    }
    else
    {
      TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_BATCH];
      TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_NUMCH];
      TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] = 1;
      TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH] = 1;
      TIDLPCLayers.numMacs =
        (int64_t)((int64_t)TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] *
          TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH]);
    }
  }
  return 0;
}


float32_tidl TIDL_getCustomLayerOutputTensorScale(sTIDL_OrgNetwork_t *pOrgTIDLNetStructure,
                                                         void * customUserParams,
                                                         int32_t layerIndex,
                                                         float32_tidl minTensorValue,
                                                         float32_tidl maxTensorValue,
                                                         float32_tidl maxOutputTensorScale)
{
  sTIDL_LayerPC_t * TIDLPCLayers = &pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  sTIDL_CustomParams_t &customParams = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].layerParams.customParams;
  float32_tidl outTensorScale = 1.0;

  if(TIDL_CUSTOM_TYPE_0 == customParams.customLayerType)
  {
    /* This function  is supposed to take min and max along with inputTensorScale
           and expected to return the output tensor scale.
           For max pooling layer as there is no change in the data output tensor scale
           is same as input tensor scale.
           But this may not be always true, lets take a simple example to understand this.
           Lets say we have a layer in which we are multiplying each elment by some
           floating point weight.

           So the operation which is happening here is :
           outputFloat[k] = inputFloat[k] * weightFloat[k];

           The quantizied representation for the same is as below :
           inputFixed[k]  = inputFloat[k] * inTensorScale
           outputFixed[k] = outputFloat[k] * outTensorScale
           weightFixed[k] = weightFloat[k] * weightScale

           Our goal here is to find outTensorScale based on input tensor scale and
           make sure that final output is computed in such a way that its 8/16 bit
           based on 8 bit quantization of 16 bit quantization.

           Lets call numQuantBits = 8 (for 8 bit quantization) or 16 ( for 16bit quantization)

           Here output tensor scale (lets call in accumulator scale ) can be written
           interms of inTensorScale and weightScale as follows :

           accumScale = inTensorScale * weightScale;

           fixedMin = min * accumScale;
           fixedMax = max * accumScale;

           Find absolute max of this ( As we support only symmetric quantization)
           absMax = abs(fixedMax) > abs(fixedMin) ? abs(fixedMax) : abs(fixedMin)

           Find how many bits this is going to take in accumulator :
           numBits = (int32_t)ceil(log((float64_tidl)absMax) / log((float64_tidl)2));

           Now our goal  is to make sure the output doesn;t go beyond 8 or 16 bits
           based on 8/16 bit quantization.

           if output data is signed then
           {
            numQuantBits = numQuantBits - 1;
           }

           if (numBits > numQuantBits)
           {
            Then we apply a right shift on the output by (numBits - numQuantBits);
            This shift is stored in the network as below :
            net->TIDLLayers[layerIdx].outData[0].roundBits = (numBits - numQuantBits);

            The same needs to be compensated in the outputScale
            outTensorScale = accumScale / ( 1 << net->TIDLLayers[layerIdx].outData[0].roundBits );

            During layer processing we are expected to apply this shift as part of
            thec compute.
           }
           else
           {
            outTensorScale = accumScale;
           }


            Note : We also have to make sure that outTensorScale in not greater than
            the max tensor scale provided as input to custom Layer process function.

       */

    /* If user wants to access user defined parameters for a given custom layer */
    TIDL_CustomParams0_t *custom0Params = (TIDL_CustomParams0_t *)customUserParams;

    sTIDL_DataParams_t *inDataParams = &TIDLPCLayers->inData[0];
    float32_tidl inputTensorScale = inDataParams->tensorScale;
    /* For max pooling layer as there is no change in the data output tensor scale
 is same as input tensor scale. */
    outTensorScale = inputTensorScale;
    if ( outTensorScale > maxOutputTensorScale )
    {
      /* If output tensor scale for a given layer is greater than maxTensorScale then this will
              result into bias saturation for consumer layers ( if it has bias). This can result into accuracy
              degradation, hence user should impose this constraint while computing the output tensor
              scale based on input tensor scale. User can reduce bits either in the activations or parameters
              to make sure final output tensor scale is less than the maxOutputTensorScale. THis is not required
              for max pool layer as it doesnt change the tensor scale but if a given layer changes the tensor scale
              then this will be required. Please refer comments in tidl_custom_import.h file for this function to
              understand how to compute this */
    }
  }

  return outTensorScale;
}


void TIDL_setDefaultCustomParams(sTIDL_CustomParams_t * customParams)
{
  customParams->doesLayerChangePadding   = 1;
  customParams->doesLayerFillOutXPadding = 0;
  customParams->memOverlapType           = 0;
  customParams->forceInPlace             = 0;
  customParams->padW                     = 0;
  customParams->padH                     = 0;
}

int32_t TIDL_MapCustomParams(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure,
                               int32_t              nodeIndex,
                               int32_t              layerIndex,
                               int32_t              *dataIndex,
                               const void           *parserObj1,
                               const void           *parserObj2,
                               int32_t               modelType)
{
  int32_t status;
  int32_t padType;
  int32_t idx1, idx2;
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  sTIDL_CustomParams_t &customParams = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].layerParams.customParams;



  /* Model Type is 1 for tensor flow */
  if ( modelType == TIDL_IMPORT_MODEL_FORMAT_TENSORFLOW)
  {
    GraphDef * tfGraphDef = (GraphDef *)parserObj1;
    const char  * currLayerName = tfGraphDef->node(nodeIndex).op().c_str();

    if (strcmp(currLayerName, (const char *)"MaxPool") == 0)
    {
      int32_t kernelW, kernelH;
      int32_t strideW, strideH;

      TIDL_getAttr_data_format(tfGraphDef->node(nodeIndex), "data_format");
      if (gloab_data_format == 1)
      {
        idx1 = 3;
        idx2 = 2;
      }
      else
      {
        idx1 = 2;
        idx2 = 1;
      }

      TIDL_getAttr_value(tfGraphDef->node(nodeIndex), "strides", &strideW, idx1);
      TIDL_getAttr_value(tfGraphDef->node(nodeIndex), "strides", &strideH, idx2);
      TIDL_getAttr_value(tfGraphDef->node(nodeIndex), "ksize",   &kernelW, idx1);
      TIDL_getAttr_value(tfGraphDef->node(nodeIndex), "ksize",   &kernelH, idx2);

      /* This example is only having custom layer for 3x3 stride 2 max pooling, for
         rest of the configuration map to original max pooling layer */
      if (( kernelW == 3 ) && ( kernelH == 3 ) && ( strideW == 2 ) && ( strideH == 2 ))
      {
        /* Set the default parameters for custom layer */
        TIDL_setDefaultCustomParams(&customParams);
        customParams.customLayerType = TIDL_CUSTOM_TYPE_0;

        /* Max pooling layer has no parameter hence parameter size is zero */
        int32_t paramSize = 0;

        /* If user needs memory for storing parameters for a given custom layer then the same can be
        requested by adding it to the total size of the memory requested for the layer. Here paramSize
        is the memory needed for the given layer in bytes. */
        int32_t totalSize = sizeof(TIDL_CustomParams0_t) + paramSize;
        TIDL_CustomParams0_t *custom0Params = (TIDL_CustomParams0_t *)malloc(totalSize);

        TIDL_getAttr_padding(tfGraphDef->node(nodeIndex), "padding", &padType);

        TIDLPCLayers.layerType = TIDL_CustomLayer;
        TIDLPCLayers.outData[0].dataId = (*dataIndex)++;
        custom0Params->poolingType = TIDL_MaxPooling;

        custom0Params->useCeil = 0;
        custom0Params->kernelW = kernelW;
        custom0Params->kernelH = kernelH;
        custom0Params->strideW = strideW;
        custom0Params->strideH = strideH;
        custom0Params->padW = 0;
        custom0Params->padH = 0;
        if (padType == 0)
        {
          custom0Params->padW = ((custom0Params->kernelW - 1)) / 2;
          custom0Params->padH = ((custom0Params->kernelH - 1)) / 2;
        }

        customParams.padH = custom0Params->padW;
        customParams.padW = custom0Params->padH;
        customParams.doesLayerChangePadding = 1;
        /* Kernel supports Seam filling (column padding) only for non-strided max pooling */
        customParams.doesLayerFillOutXPadding = 0;

        TIDLPCLayers.weights.ptr = (void*)custom0Params;
        TIDLPCLayers.weights.bufSize = totalSize;
      }
    }
  }
  else if ( modelType == TIDL_IMPORT_MODEL_FORMAT_ONNX)
  {
    onnx::GraphProto * onnGraph = ( onnx::GraphProto *)parserObj1;
    const char  * currLayerName = onnGraph->node(nodeIndex).op_type().c_str();

    if (strcmp(currLayerName, (const char *)"MaxPool") == 0)
    {
      onnx::NodeProto node = onnGraph->node(nodeIndex);
      /* Parameters can be parsed based from node object based on the
      layer */
    }
  }
  else if ( modelType == TIDL_IMPORT_MODEL_FORMAT_TFLITE)
  {
    const tflite::Model *tfliteModel = (tflite::Model *)parserObj1;
    auto operators = (*tfliteModel->subgraphs())[0]->operators();

    const auto *op = operators->Get(nodeIndex);
    auto operator_codes = (*tfliteModel->operator_codes())[op->opcode_index()]->builtin_code();
    const char* currLayerName = EnumNameBuiltinOperator(operator_codes);
    if (strcmp(currLayerName, (const char *)"MAX_POOL_2D") == 0)
    {
      /* Parameters can be parsed based from node object based on the
      layer */
    }
  }
  else if ( modelType == TIDL_IMPORT_MODEL_FORMAT_CAFFE)
  {
    caffe::NetParameter * netStructure = (caffe::NetParameter *)parserObj1;
    caffe::NetParameter * netParams    = (caffe::NetParameter *)parserObj2;
    const char* currLayerName = netStructure->layer(nodeIndex).type().c_str();

    if (strcmp(currLayerName, (const char *)"Pooling") == 0)
    {
      if(netStructure->layer(nodeIndex).pooling_param().pool() == TIDL_MaxPooling)
      {
        /* Parameters can be parsed based from node object based on the
        layer */
      }
    }
  }
  else
  {
    printf("Unsupported model format \n");
    return -1;
  }

  return 0;
}



/* ========================================================================== */
/*                       Static Function Definitions                          */
/* ========================================================================== */

/* None */
