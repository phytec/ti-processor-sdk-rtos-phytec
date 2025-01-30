/*
*
* Copyright (c) {2015 - 2023} Texas Instruments Incorporated
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
 *  \file TIDL_layerNorm.c
 *
 *  \brief Layer norm process calls & ref implementation
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "tidl_alg_int.h"
#include "tidl_commonUtils.h"
#include "tidl_layerNorm.h"
#include "math.h"
#include <limits>


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
#define SCALE_PRECISION_BITS 8
#define FLOAT_MANTISSA_PRECISION 23
#define LAYERNORM_MAX_VALUE ((float)(std::numeric_limits<uint8_t>::max()) - 1E-2)
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

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/* ========================================================================== */
/*                       Static Function Definitions                          */
/* ========================================================================== */

/*Function to update epsilon constraints such that MMA kernel behaves correctly*/
float TIDL_layerNormUpdateEpsilon(float epsilon, float outScale, float inScale, int32_t elementType)
{

  float layerNormRatio = outScale/(epsilon*inScale*inScale);
  float updatedEpsilon = 0.0;
  float maxScale = LAYERNORM_MAX_VALUE;
  if ( (layerNormRatio > maxScale) && (elementType == TIDL_SignedChar))
  {
    updatedEpsilon = (outScale*outScale)/(inScale*inScale*maxScale*maxScale);
  }
  else
  {
    updatedEpsilon = epsilon;
  }

  return updatedEpsilon;
  
};

/**
 * @brief This is main function perform reduce on differnt elementTypes
 *
 * @tparam Tin  : template for input
 * @tparam Tw   : template for weights
 * @tparam Tb   : template for Bias
 * @tparam Tout : template for output
 * @tparam Tsat : template for saturate values
 * @param inPtr : Input pointer on which bacthNorm is applied
 * @param outPtr : Output pointer after reduce opreation
 * @param weightsPtr : Pointer to weights buffer
 * @param slopePtr : Pointer to the Slope buffer
 * @param biasPtr  : Pointer to the Bias values
 * @param intAlgHandle : tidl algorothm handle
 * @param layerIdx :index of the current layer
 * @param params : copy of bacthNorm parameters
 * @param algLayer : Pointer to the layer specific parameters
 * @param inDataParams : parameters of the input data buffer
 * @param outDataParams : parameters of the output data buffer
 * @param satLow : min value for the saturation
 * @param satHigh : max value for the saturation
 * @return  IALG_EOK   - Successful
 *          IALG_EFAIL - Unspecified error
 */
template <class Tin, class Tout, class typeExEx2>
static int32_t TIDL_refLayerNormCore(Tin  *inPtr,
                                     Tout *outPtr,
                                     TIDL_Handle intAlgHandle,
                                     int32_t layerIdx,
                                     sTIDL_LayerNormParams_t *params,
                                     sTIDL_AlgLayer_t *algLayer,
                                     const sTIDL_DataParams_t *inDataParams,
                                     const sTIDL_DataParams_t *outDataParams)
{

  int32_t status = TIDL_SUCCESS;
  int32_t i0, i1, i2, i3, i4, i5;
  int32_t icnt[TIDL_DIM_MAX] = {0};
  int32_t dim[TIDL_DIM_MAX] = {0};
  int32_t outDim[TIDL_DIM_MAX] = {0};
  int32_t inVolume = 1;
  int32_t meanOffset = 0;
  int32_t inOffset = 0;
  int32_t outOffset = 0;
  float32_tidl inScale  = inDataParams->tensorScale;
  float32_tidl outScale = outDataParams->tensorScale;
  float32_tidl avg, denom;
  float32_tidl epsilon = params->epsilon;
  epsilon = TIDL_layerNormUpdateEpsilon(epsilon, outScale, inScale, inDataParams->elementType);
  /*Epsilon Correction:*/
  uint8_t scaleAvg, shiftAvg;
  int32_t shiftDenom, scaleDenom, absShift;

  for(i0 = 0; i0 < TIDL_DIM_MAX - 1; i0++)
  {
    icnt[i0] = inDataParams->dimValues[i0];
    inVolume *= icnt[i0];
    dim[i0]  = inDataParams->pitch[i0];
    outDim[i0] = outDataParams->pitch[i0];
  }
  icnt[TIDL_DIM_MAX-1] = inDataParams->dimValues[TIDL_DIM_MAX-1];
  inVolume *= icnt[TIDL_DIM_MAX-1];

  Tin  *inData = (Tin *)inPtr;
  Tout *outData= (Tout *)outPtr;
  int32_t width = icnt[5];

  /*Scratch required for E(x) & E((X-E(X))^2*/
  int32_t scratchSizeRequired = (2U * icnt[4] * sizeof(typeExEx2));
  
  int32_t minValueOutput = std::numeric_limits<Tout>::lowest();
  int32_t maxValueOutput = std::numeric_limits<Tout>::max();
  int32_t minValueAcc = std::numeric_limits<typeExEx2>::lowest();
  int32_t maxValueAcc = std::numeric_limits<typeExEx2>::max();
    
  typeExEx2 *eX;
  typeExEx2 *eX2;
  int64_t acceX, acceX2, inp;
  int64_t tempAcc;
  int32_t tempAccOut;
  if (algLayer->scratchSize >= scratchSizeRequired)
  {
    eX = (typeExEx2 *)algLayer->scratchMem;
    eX2 = (typeExEx2 *)(((uint8_t*)(algLayer->scratchMem)) + (icnt[4]*sizeof(typeExEx2)));
  }
  else
  {
    status = TIDL_E_LAYERNORM_INSUFFICIENT_REF_SCRATCH;
  }

  /*Compute Average:*/
  avg = 1.0 / width;
  TIDL_getMMAv2_ScaleAndShift(avg, &scaleAvg, &shiftAvg);

  if(status == TIDL_SUCCESS)
  {
    if(params->axis == TIDL_DIM_WIDTH) 
    {
      
      for(i0 = 0; i0 < icnt[0]; i0++)
      {
        for(i1 = 0; i1 < icnt[1]; i1++)
        {
          for(i2 = 0; i2 < icnt[2]; i2++)
          {
            for(i3 = 0; i3 < icnt[3]; i3++)
            {
              /*Calculate mean & mean of x^2 */
              for(i4 = 0; i4 < icnt[4]; i4++)
              {          
                meanOffset =  i4;
                eX[meanOffset] = 0;
                eX2[meanOffset] = 0;
                acceX = 0;
                acceX2 = 0;
                for(i5 = 0; i5 < icnt[5]; i5++)
                {
                  inOffset = i0 * dim[0] + i1 * dim[1] + i2 * dim[2] + i3 * dim[3] + i4 * dim[4] + i5;
                  inp = inData[inOffset];
                  acceX  += (inp);
                  acceX2 += (inp * inp);
                }

                if( (intAlgHandle->createParams->net->deviceName == TIDL_TDA4VM) ||
                    (inDataParams->elementType == TIDL_UnsignedChar) ||
                    (inDataParams->elementType == TIDL_UnsignedShort) ||
                    ( (outDataParams->tensorScale/sqrt(epsilon * inScale * inScale)) > (float)(std::numeric_limits<uint8_t>::max())) )
                {
                  /*This works for 16-bit, but is not sufficient for 8-bit. However for 8-bit, it is going to  correct epsilon to
                    not hit this condition (MMAv2)*/
                  eX[meanOffset] = (typeExEx2)(((acceX)/icnt[5]));
                  eX2[meanOffset] = (typeExEx2)(((acceX2)/icnt[5]));
                }
                else
                {
                  tempAcc = ((int64_t)acceX) * (int64_t)scaleAvg;
                  eX[meanOffset] = (typeExEx2)TIDL_roundSat((int64_t)tempAcc, shiftAvg, minValueAcc, maxValueAcc);
                  tempAcc = ((int64_t)acceX2) * (int64_t)scaleAvg;
                  eX2[meanOffset] = (typeExEx2)TIDL_roundSat((int64_t)tempAcc, shiftAvg, minValueAcc, maxValueAcc);
                }
                inp = eX2[meanOffset] - (eX[meanOffset] * eX[meanOffset]);
                if(inp < 0)
                {
                  inp = 0;
                }
                /*Calculate denominator & produce output:*/
                denom = (float32_tidl)( __recip_sqrt(inp + (epsilon * (inScale * inScale)) ));
                denom = outScale * denom;
                TIDL_convertFloatToScaleAndShift(denom, &scaleDenom, &shiftDenom, SCALE_PRECISION_BITS); 

                if(shiftDenom >= 0)
                {
                  for(i5 = 0; i5 < icnt[5]; i5++)
                  {
                    inOffset = i0 * dim[0] + i1 * dim[1] + i2 * dim[2] + i3 * dim[3] + i4 * dim[4] + i5;
                    outOffset = i0 * outDim[0] + i1 * outDim[1] + i2 * outDim[2] + i3 * outDim[3] + i4 * outDim[4] + i5;
                    meanOffset =  i4;
                    tempAccOut = (inData[inOffset] - eX[meanOffset]) * scaleDenom;
                    tempAccOut = (tempAccOut<<shiftDenom);
                    tempAccOut = tempAccOut < minValueOutput ? minValueOutput : tempAccOut;
                    tempAccOut = tempAccOut > maxValueOutput ? maxValueOutput : tempAccOut;               
                    outData[outOffset] = (Tout)tempAccOut;
                  }
                }
                else
                {
                  for(i5 = 0; i5 < icnt[5]; i5++)
                  {
                    absShift = shiftDenom  * -1;
                    inOffset = i0 * dim[0] + i1 * dim[1] + i2 * dim[2] + i3 * dim[3] + i4 * dim[4] + i5;
                    outOffset = i0 * outDim[0] + i1 * outDim[1] + i2 * outDim[2] + i3 * outDim[3] + i4 * outDim[4] + i5;
                    meanOffset =  i4;
                    tempAccOut = (inData[inOffset] - eX[meanOffset]) * scaleDenom;
                    tempAccOut = ((tempAccOut+(1<<(absShift-1)))>>absShift);
                    tempAccOut = tempAccOut < minValueOutput ? minValueOutput : tempAccOut;
                    tempAccOut = tempAccOut > maxValueOutput ? maxValueOutput : tempAccOut;                   
                    outData[outOffset] = (Tout)tempAccOut;
                  }
                }

              }
            }
          }
        }
      }
    }
    else
    {
      status = TIDL_E_LAYERNORM_UNSUPPORTED_AXIS;
    }
  }
  return status;
}


/**
 * @brief This is main function perform reduce on differnt elementTypes
 *
 * @tparam Tin  : template for input
 * @tparam Tw   : template for weights
 * @tparam Tb   : template for Bias
 * @tparam Tout : template for output
 * @tparam Tsat : template for saturate values
 * @param inPtr : Input pointer on which bacthNorm is applied
 * @param outPtr : Output pointer after reduce opreation
 * @param weightsPtr : Pointer to weights buffer
 * @param slopePtr : Pointer to the Slope buffer
 * @param biasPtr  : Pointer to the Bias values
 * @param intAlgHandle : tidl algorothm handle
 * @param layerIdx :index of the current layer
 * @param params : copy of bacthNorm parameters
 * @param algLayer : Pointer to the layer specific parameters
 * @param inDataParams : parameters of the input data buffer
 * @param outDataParams : parameters of the output data buffer
 * @param satLow : min value for the saturation
 * @param satHigh : max value for the saturation
 * @return  IALG_EOK   - Successful
 *          IALG_EFAIL - Unspecified error
 */
template <class Tin, class Tout>
static int32_t TIDL_refLayerNormCoreFloat(Tin  *inPtr,
                                          Tout *outPtr,
                                          TIDL_Handle intAlgHandle,
                                          int32_t layerIdx,
                                          sTIDL_LayerNormParams_t *params,
                                          sTIDL_AlgLayer_t *algLayer,
                                          const sTIDL_DataParams_t *inDataParams,
                                          const sTIDL_DataParams_t *outDataParams)
{
  int32_t status = TIDL_SUCCESS;
  int32_t i0, i1, i2, i3, i4, i5;
  int32_t icnt[TIDL_DIM_MAX] = {0};
  int32_t dim[TIDL_DIM_MAX] = {0};
  int32_t inVolume = 1;
  int32_t meanOffset = 0;
  int32_t inOffset = 0;
  float32_tidl epsilon = params->epsilon;

  for(i0 = 0; i0 < TIDL_DIM_MAX - 1; i0++)
  {
    icnt[i0] = inDataParams->dimValues[i0];
    inVolume *= icnt[i0];
    dim[i0]  = inDataParams->pitch[i0];
  }
  icnt[TIDL_DIM_MAX-1] = inDataParams->dimValues[TIDL_DIM_MAX-1];
  inVolume *= icnt[TIDL_DIM_MAX-1];

  Tin  *inData = (Tin *)inPtr;
  Tout *outData= (Tout *)outPtr;

  /*Scratch required for E(x) & E((X-E(X))^2*/
  int32_t scratchSizeRequired = 2 * inVolume * sizeof(float32_tidl) ;
  scratchSizeRequired /= icnt[5];
  

  float32_tidl *eX;
  float32_tidl *eX2;

  if (algLayer->scratchSize >= scratchSizeRequired)
  {
    eX = (float32_tidl *)algLayer->scratchMem;
    eX2 = (float32_tidl *)(((uint8_t*)(algLayer->scratchMem)) + (inVolume*sizeof(float32_tidl)));
  }
  else
  {
    status = TIDL_E_LAYERNORM_INSUFFICIENT_REF_SCRATCH;
  }

  if(status == TIDL_SUCCESS)
  {
    if(params->axis == TIDL_DIM_WIDTH) 
    {
      
      for(i0 = 0; i0 < icnt[0]; i0++)
      {
        for(i1 = 0; i1 < icnt[1]; i1++)
        {
          for(i2 = 0; i2 < icnt[2]; i2++)
          {
            for(i3 = 0; i3 < icnt[3]; i3++)
            {
              /*Calculate mean:*/
              for(i4 = 0; i4 < icnt[4]; i4++)
              {          
                meanOffset =  i4;    
                eX[meanOffset] = 0.0;
                for(i5 = 0; i5 < icnt[5]; i5++)
                {
                  inOffset = i0 * dim[0] + i1 * dim[1] + i2 * dim[2] + i3 * dim[3] + i4 * dim[4] + i5;
                  eX[meanOffset] += inData[inOffset];
                }
                eX[meanOffset] /= icnt[TIDL_DIM_WIDTH];
              }
              /*Calculate sqrt(mean((X-mean(x))^2) + e)*/
              for(i4 = 0; i4 < icnt[4]; i4++)
              {
                meanOffset =  i4;           
                eX2[meanOffset] = 0.0;
                for(i5 = 0; i5 < icnt[5]; i5++)
                {
                  inOffset = i0 * dim[0] + i1 * dim[1] + i2 * dim[2] + i3 * dim[3] + i4 * dim[4] + i5;
                  eX2[meanOffset] += (float32_tidl)pow((inData[inOffset] - eX[meanOffset]), 2);
                }
                eX2[meanOffset] /= icnt[TIDL_DIM_WIDTH];
                eX2[meanOffset] += epsilon;
                eX2[meanOffset] = (float32_tidl)sqrt(eX2[meanOffset]);
              }
              /*Calculate final output:*/
              for(i4 = 0; i4 < icnt[4]; i4++)
              {
                for(i5 = 0; i5 < icnt[5]; i5++)
                {
                  inOffset = i0 * dim[0] + i1 * dim[1] + i2 * dim[2] + i3 * dim[3] + i4 * dim[4] + i5;
                  meanOffset =  i4;
                  outData[inOffset] = (Tout)(inData[inOffset] - eX[meanOffset]) / eX2[meanOffset];
                }
              }
            }  
          }
        }
      }
    }
    else
    {
      status = TIDL_E_LAYERNORM_UNSUPPORTED_AXIS;
    }
  }

  return status;
}

 /**
 * @brief Reduce layer reference implementation
 *
 * @param intAlgHandle : tidl algorothm handle
 * @param algLayer : Pointer to the layer specific parameters
 * @param tidlLayer : Pointer to the common layer parameters
 * @param params : copy of batch norm layer parameters
 * @param inPtr : Pointer to input buffers to be processed
 * @param outPtr : Pointer to output buffers to be processed
 * @param inDataParams : pointer to input data parameters
 * @param outDataParams : pointer to output data parameters
 * @return  IALG_EOK   - Successful
 *          IALG_EFAIL - Unspecified error
 */
static int32_t TIDL_refLayerNormProcess(TIDL_Handle intAlgHandle,
                                        sTIDL_AlgLayer_t *algLayer,
                                        const sTIDL_Layer_t    *tidlLayer,
                                        sTIDL_LayerNormParams_t *params,
                                        void *inPtr,
                                        void *outPtr,
                                        const sTIDL_DataParams_t *inDataParams,
                                        const sTIDL_DataParams_t *outDataParams)
{
  int32_t status = TIDL_SUCCESS;
  int32_t layerIdx = algLayer->layerIdx;


  if(TIDL_SignedChar == ((int32_t)inDataParams->elementType))
  {
    status = TIDL_refLayerNormCore<int8_t, int8_t, int16_t>((int8_t *)inPtr,
                                    (int8_t *)outPtr,
                                    intAlgHandle,
                                    layerIdx,
                                    params,
                                    algLayer,
                                    inDataParams,
                                    outDataParams);
  }
  else if(TIDL_UnsignedChar == ((int32_t)inDataParams->elementType))
  {
    status = TIDL_refLayerNormCore<uint8_t, int8_t, int16_t>((uint8_t *)inPtr,
                                      (int8_t *)outPtr,
                                      intAlgHandle,
                                      layerIdx,
                                      params,
                                      algLayer,
                                      inDataParams,
                                      outDataParams);
  }
  else if(TIDL_SignedShort == ((int32_t)inDataParams->elementType))
  {
    status = TIDL_refLayerNormCore<int16_t, int16_t, int32_t>((int16_t *)inPtr,
                                    (int16_t *)outPtr,
                                    intAlgHandle,
                                    layerIdx,
                                    params,
                                    algLayer,
                                    inDataParams,
                                    outDataParams);
  }
  else if(TIDL_UnsignedShort == ((int32_t)inDataParams->elementType))
  {
    status = TIDL_refLayerNormCore<uint16_t, int16_t, int32_t>((uint16_t *)inPtr,
                                    (int16_t *)outPtr,
                                    intAlgHandle,
                                    layerIdx,
                                    params,
                                    algLayer,
                                    inDataParams,
                                    outDataParams);
  }
  else if(TIDL_SinglePrecFloat == ((int32_t)inDataParams->elementType))
  {
    status = TIDL_refLayerNormCoreFloat((float32_tidl *)inPtr,
                                    (float32_tidl *)outPtr,
                                    intAlgHandle,
                                    layerIdx,
                                    params,
                                    algLayer,
                                    inDataParams,
                                    outDataParams);
  }
  else
  {
    status = TIDL_ERR_FAILURE;
  }

  TIDL_enableL1DandL2CacheWb();
  return status;
}

int32_t TIDL_layerNormProcess(TIDL_NetworkCommonParams *commonParams,
                              sTIDL_AlgLayer_t     * algLayer,
                              sTIDL_Layer_t        * tidlLayer,
                              void                 * inPtrs[],
                              void                 * outPtrs[],
                              int32_t                layerIdx)
{
  int32_t  status      = TIDL_SUCCESS;
  uint8_t  (*inPtr)[]     = (uint8_t (*)[])(inPtrs[0]);
  int8_t   (*outPtr)[]    = (int8_t (*)[])(outPtrs[0]);

  TIDL_Obj intAlgObj;
  TIDL_CreateParams createParams;
  memcpy(&createParams, commonParams->createParams, sizeof(TIDL_CreateParams));
  intAlgObj.createParams = (TIDL_CreateParams *) &createParams;

  if ((commonParams->createParams->flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) != TIDL_FLOW_CTRL_REF_ONLY)
  {
    status = TIDL_deviceUtilsCommonProcess(commonParams,
                                algLayer,
                                tidlLayer,
                                inPtrs,
                                outPtrs,
                                layerIdx);

  }
  else /* if ((commonParams->createParams->flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) == TIDL_FLOW_CTRL_REF_ONLY) */
  {
    sTIDL_LayerNormParams_t   *params = &tidlLayer->layerParams.layerNormParams;
    sTIDL_DataParams_t        *inDataParams;
    inDataParams   = &commonParams->createParams->net->TIDLLayers[algLayer->inLayerIdx[0]].outData;

    status = TIDL_refLayerNormProcess(&intAlgObj,
                                      algLayer,
                                      tidlLayer,
                                      params,
                                      inPtr,
                                      outPtr,
                                      inDataParams,
                                      &tidlLayer->outData);
  }
  return status;
}

