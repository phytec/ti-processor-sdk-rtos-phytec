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


/**
----------------------------------------------------------------------------
@file    ti_dl.h
@brief     This file defines the ivision interface for TI deep learning libary
@version 0.1 (Oct 2016) : Initial Code [ADK]
@version 0.5 (Jan 2017) : Cleaned up [ADK]
----------------------------------------------------------------------------
*/

#ifndef TIDL_H_
#define TIDL_H_ 1

#include "itidl_ti.h"

#include <vector>
#include <string>

typedef enum
{  
  TIDL_PriorBoxLayer        = TIDL_UnsupportedLayer+1,
  TIDL_PermuteLayer          ,
  TIDL_ShapeLayer            ,
  TIDL_ClipLayer             ,
  TIDL_MinimumLayer          ,
  TIDL_LeakyReluLayer        ,
  TIDL_IdentityLayer         ,
  TIDL_BatchToSpaceLayer     ,
  TIDL_SpaceToBatchLayer     ,
  TIDL_PackLayer             ,
  TIDL_DequantizeLayer       ,
  TIDL_QuantizeLayer         ,
  TIDL_SqrtLayer             ,
  TIDL_ReduceMeanLayer       ,
  TIDL_PowLayer              ,
  TIDL_DivLayer              ,
  TIDL_SubLayer              , 
  TIDL_PatchMergeLayer       ,
  TIDL_CastLayer             
}eTIDL_PCLayerType;


typedef enum
{
  TIDL_QUANT_STATS_NONE = 0,
  TIDL_QUANT_STATS_FLOAT,
  TIDL_QUANT_STATS_FIXED,
}eTIDL_QuantStatsType;

typedef struct{
  int32_t size;
  int32_t elementType;
  void **prmPtr;
  float *scalePtr;
  float min;
  float max;
}sTIDL_QuantParams_t;

typedef enum
{
  TIDL_BIAS_QUANT_PARAMS = 0,
  TIDL_WEIGHT_QUANT_PARAMS,
  TIDL_SLOPE_QUANT_PARAMS,
  TIDL_MAX_QUANT_PARAMS,
}eTIDL_QuantParamsType;

extern const char * TIDL_LayerString[];
#define TIDL_NUM_MAX_PC_LAYERS (2048)


typedef struct {
  /** Buffer containing Dim values for output tensor */
  int32_t   outDims[TIDL_DIM_MAX];
    /** Indicates if Reshape is introduced by TIDL for internal use*/
  int32_t isInduced;
  /** */
}sTIDL_ReshapeParams_t;

typedef struct {
  /** Buffer containing Axis values, to be squeezed if 1*/
  int32_t   axis[TIDL_DIM_MAX];
}sTIDL_SqueezeParams_t;

typedef struct {
  float   min;
}sTIDL_MinimumParams_t;

typedef struct {
  float   alpha;
}sTIDL_LeakyReluParams_t;

typedef struct{
  int32_t blockHeight;
  int32_t blockWidth;
  int32_t cropT;
  int32_t cropB;
  int32_t cropL;
  int32_t cropR;
}sTIDL_BatchToSpaceParams_t;

typedef struct{
  int32_t blockHeight;
  int32_t blockWidth;
  int32_t padT;
  int32_t padB;
  int32_t padL;
  int32_t padR;
}sTIDL_SpaceToBatchParams_t;

typedef struct{
  int32_t axis;
  int32_t valuesCount;
}sTIDL_PackParams_t;

typedef struct{
  float scale;
  float zeroPoint;
}sTIDL_DequantizeParams_t;

typedef struct{
  float scale;
  float zeroPoint;
  int32_t elementType;
}sTIDL_QuantizeParams_t;

typedef struct {
  float   alpha;
  float   beta;
}sTIDL_HardSigmoidParams_t;

typedef struct {
  float   alpha;
}sTIDL_EluParams_t;

typedef struct{
  int32_t axis;
  int32_t reduceDims;
}sTIDL_ReduceMeanParams_t;

typedef struct{
  float pow;
}sTIDL_PowParams_t;

typedef struct{
  int32_t axis;
}sTIDL_SqrtParams_t;

/**
@struct sTIDL_allowlistingMetaData
@brief  This structure contains layer level tensor related meta data to be used for allowlisting
*/
typedef struct
{
  /** Number of inputs for given layer - this includes inputs from previous layers and other initializer/constant inputs */
  int32_t numInputs;
  /** Number of valid inputs of layer - Field used for Tflite runtimes, some inputs with tensor Location = -1 are not valid and should be ignored */
  int32_t numValidInputs;
  /** Number of variable inputs of layer - Inputs coming from previous layers */
  int32_t numVarInputs;
  /** Number of constant inputs of layer */
  int32_t numConstInputs;
  /** Number of outputs of layer */
  int32_t numOutputs;
  /** Node input indices for variable tensors of layer */
  std::vector<int32_t> varTensorIndices;
  /** Node input indices for constant tensors of layer */
  std::vector<int32_t> constTensorIndices;
  /** Node input indices for valid tensors of layer -- for tflite runtime */
  std::vector<int32_t> validInputIndices;
  /** Dimenstions of output tensors */
  std::vector<std::vector<int32_t>> outputTensorDims;
  /** Dimensions of variable input tensors -- these are not available for ONNX without shape inference */
  std::vector<std::vector<int32_t>> varTensorsDims;
  /** Dimenstions of constant input tensors */
  std::vector<std::vector<int32_t>> constTensorsDims;
  /** Datanames of Const Inputs*/
  std::vector<std::string> constTensorNames;
} sTIDL_allowlistingMetaData;

typedef union {
  sTIDL_ReshapeParams_t      reshapeParams;
  sTIDL_SqueezeParams_t      squeezeParams;
  sTIDL_MinimumParams_t      minimumParams;
  sTIDL_LeakyReluParams_t    leakyReluParams;
  sTIDL_BatchToSpaceParams_t  batchToSpaceParams;
  sTIDL_SpaceToBatchParams_t  spaceToBatchParams;
  sTIDL_PackParams_t  packParams;
  sTIDL_DequantizeParams_t dequantParams;
  sTIDL_ReduceMeanParams_t reduceMeanParams;
  sTIDL_SqrtParams_t sqrtParams;
  sTIDL_PowParams_t powParams;
  sTIDL_QuantizeParams_t   quantizeParams;
  sTIDL_HardSigmoidParams_t hardSigmoidParams;
  sTIDL_EluParams_t eluParams;
} sTIDL_LayerPCParams_t;

typedef struct {
    sTIDL_LayerParams_t layerParams;
    sTIDL_LayerPCParams_t layerPCParams;
    sTIDL_ActParams_t    actParams;
    int32_t layerType;
    int32_t layerKernelType;
    int32_t numInBufs;
    int32_t numOutBufs;
    int64_t numMacs;
    int8_t  name[TIDL_STRING_SIZE];
    int8_t  inDataNames[TIDL_NUM_IN_BUFS][TIDL_STRING_SIZE];
    int8_t  outDataNames[TIDL_NUM_OUT_BUFS][TIDL_STRING_SIZE];
    int32_t outConsumerCnt[TIDL_NUM_OUT_BUFS];
    int32_t outConsumerLinked[TIDL_NUM_OUT_BUFS];
    sTIDL_DataParams_t inData[TIDL_NUM_IN_BUFS];
    sTIDL_DataParams_t outData[TIDL_NUM_OUT_BUFS];
    sBuffer_t weights;
    sBuffer_t bias;
    sBuffer_t weightScales;
    sBuffer_t biasScales;
    sBuffer_t weightZeroPoints;
    sBuffer_t biasZeroPoints;
    /*Derived quantities ready for hardware consumption:*/
    sBuffer_t derivedBias;
    sBuffer_t derivedScales;
    sBuffer_t derivedShifts;
    int32_t   minPSAT;
    int32_t   maxPSAT;
    /* Buffer to store per channel weight scales */
    sBuffer_t perChannelWeightScale;
    sBuffer_t slope;
    sBuffer_t priorBox;
    sBuffer_t perChannelPadConstTensor;
    int32_t weightsElementSizeInBits;  //kernel weights in bits
    /** Offset selection method for stride. \ref eTIDL_StrideOffsetMethod */
    int32_t strideOffsetMethod;
    /* Indicates whether layer is split across multiple cores */
    int32_t multiCoreMode;
    int32_t scratchMemRequired;
    int32_t weightsReordered;
    sTIDL_QuantParams_t quantParams[TIDL_MAX_QUANT_PARAMS];
    sTIDL_allowlistingMetaData allowlistingMetaData;
}sTIDL_LayerPC_t;

typedef struct {
  int32_t numLayers;
  /** Quantization statistics availability - Updated with tidl_quant_stats_tool */
  /* Refere eTIDL_QuantStatsType for valid values */
  int32_t quantStats;
  /* Size of model written after import pass before running NC */
  int64_t modelSizeWithMaxNC;
  sTIDL_LayerPC_t TIDLPCLayers[TIDL_NUM_MAX_PC_LAYERS];
}sTIDL_OrgNetwork_t;

typedef struct {
  int32_t numBatches;
  int32_t numCores;
} sTIDL_multiCoreBatchProcessing;

#endif  /* TI_DL_H_ */

/* =========================================================================*/
/*  End of file:  ti_od_cnn.h                                               */
/* =========================================================================*/
