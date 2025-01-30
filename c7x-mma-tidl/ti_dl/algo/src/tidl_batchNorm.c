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
 *  \file tidl_batchNorm.c
 *
 *  \brief This file defines kernel functions for Batch Norm layer
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "tidl_alg_int.h"
#include "tidl_commonUtils.h"
#include "tidl_batchNorm.h"
#include <cmath>
#include "tidl_device_utils.h"
#include "tidl_alg_utils.h"
#include <limits>
#include "tidl_lut_module.h"


//#define TIDL_REF_BATCH_NORM_DEBUG
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

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/* ========================================================================== */
/*                       Static Function Definitions                          */
/* ========================================================================== */
/**
 * @brief Function will do multiplication with slope and rounding
 *
 * @param out  : Input value that requires rounding with slope
 * @param slopeFact : The amount of slope that need to be multiplied
 * @param slopeQBits : roundbits for rounding
 * @return int32_t : output after rounding with slope
 */
int32_t TIDL_refBatchNormCoreRoundSlope(
  int32_t out,
  int32_t slopeFact,
  int32_t slopeQBits)
{
  if(out < 0)
  {
    out = (((int64_t)out * slopeFact) >> slopeQBits);
  }
  return out;
}
/**
 * @brief This is main function perform batchNorm on differnt elementTypes
 *
 * @tparam Tin  : template for input
 * @tparam Tw   : template for weights
 * @tparam Tb   : template for Bias
 * @tparam Tout : template for output
 * @tparam Tsat : template for saturate values
 * @param inPtr : Input pointer on which bacthNorm is applied
 * @param outPtr : Output pointer after batchNorm opreation
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
template <class Tin, class Tw, class Tb, class Tout, class Tacc>
static int32_t TIDL_refBatchNormCore(Tin  *inPtr,
                                     Tout *outPtr,
                                     const Tw *weightsPtr,
                                     const Tw *slopePtr,
                                     const Tb *biasPtr,
                                     Tacc     *refAccPtr,
                                     TIDL_Handle intAlgHandle,
                                     int32_t layerIdx,
                                     sTIDL_BatchNormParams_t *params,
                                     sTIDL_AlgLayer_t *algLayer,
                                     const sTIDL_DataParams_t *inDataParams,
                                     const sTIDL_DataParams_t *outDataParams)
{
  int32_t status = TIDL_SUCCESS;
  sTIDL_Network_t * net = intAlgHandle->createParams->net;
  int32_t i0, i1, i2, i3, i4, i5;
  int32_t imWidth    = (int32_t)inDataParams->dimValues[TIDL_DIM_WIDTH];
  int32_t imHeight   = (int32_t)inDataParams->dimValues[TIDL_DIM_HEIGHT];
  int32_t numDIM1    = (int32_t)inDataParams->dimValues[TIDL_DIM_DIM1];
  int32_t numDIM2    = (int32_t)inDataParams->dimValues[TIDL_DIM_DIM2];
  int32_t inPitch    = (int32_t)inDataParams->pitch[TIDL_LINE_PITCH];
  int32_t inChPitch  = (int32_t)inDataParams->pitch[TIDL_CHANNEL_PITCH];
  int32_t inDIM1Pitch = (int32_t)inDataParams->pitch[TIDL_DIM1_PITCH];
  int32_t inDIM2Pitch = (int32_t)inDataParams->pitch[TIDL_DIM2_PITCH];
  int32_t inBatchPitch  = (int32_t)inDataParams->pitch[TIDL_ROI_PITCH];
  int32_t outPitch   = (int32_t)outDataParams->pitch[TIDL_LINE_PITCH];
  int32_t outChPitch = (int32_t)outDataParams->pitch[TIDL_CHANNEL_PITCH];
  int32_t outDIM1Pitch = (int32_t)outDataParams->pitch[TIDL_DIM1_PITCH];
  int32_t outDIM2Pitch = (int32_t)outDataParams->pitch[TIDL_DIM2_PITCH];
  int32_t outBatchPitch = (int32_t)outDataParams->pitch[TIDL_ROI_PITCH];
  Tin  *inData = (Tin *)inPtr;
  Tout *outData = (Tout *)outPtr;
  Tacc out = 0;
  int32_t numTotBatches = inDataParams->dimValues[TIDL_DIM_BATCH];
  int32_t accMemSize;
  Tw weightVal;
  int32_t slopeQBits = 0;
  int32_t slopeFact  = 1;
  Tw preluScale = 1;
  uint32_t accumulatorSize = (uint32_t)(sizeof(*refAccPtr));

  params->numChannels = inDataParams->dimValues[TIDL_DIM_NUMCH];

  accMemSize = (numTotBatches * outBatchPitch* (int32_t)accumulatorSize);

  if(algLayer->scratchSize >= accMemSize)
  {
     refAccPtr = (Tacc *)algLayer->scratchMem;
  }
  else
  {
    tidl_printf(0,"Memory for  TIDL_refBatchNormCore accumulator is not sufficient exiting...\n    ");
    status = TIDL_ERR_FAILURE;
  }

  int32_t numChannels = params->numChannels;
  Tacc min, max;

  max = std::numeric_limits<Tacc>::lowest();
  min = std::numeric_limits<Tacc>::max();
  if(TIDL_SUCCESS == status)
  {
    //OPENACC(data copyin(inData[: 1 + ((numTotBatches-1)*inBatchPitch) + ((numChannels-1)*inChPitch) + ((imHeight-1)*inPitch) + (imWidth-1)], weightsPtr[:numChannels-1], biasPtr[:numChannels-1], slopePtr[:numChannels-1]) \
            copyout(refAccPtr[: numTotBatches * outBatchPitch]))
    //OPENACC(parallel loop collapse(5))
    for (i5 = 0; i5 < numTotBatches; i5++)
    {
      for (i4 = 0; i4 < numDIM1; i4++)
      {
        for (i3 = 0; i3 < numDIM2; i3++)
        {
          Tin  inDataVal;
          for (i2 = 0; i2 < numChannels; i2++)
          {
            weightVal = weightsPtr[i2];
            if(intAlgHandle->createParams->net->TIDLLayers[layerIdx].actParams.actType == TIDL_PRelU)
            {
              preluScale = slopePtr[i2];
              slopeQBits = 8;
              int32_t tempSlopeFact = preluScale*(((int32_t)1) << slopeQBits);
              float32_tidl floatSlopeFact = (float32_tidl)(tempSlopeFact) /
              intAlgHandle->createParams->net->TIDLLayers[layerIdx].actParams.slopeScale;
              slopeFact = (int32_t)(floatSlopeFact);
              /* for 16-bit weights the deviation is more because of float to int conversion : TIDL-1332 */
              if(sizeof(Tw) == sizeof(int16_t))
              {
                slopeFact = (int32_t)(floatSlopeFact*256);
                slopeQBits += 8;
              }
            }


            for (i0=0; i0<imHeight; i0++)
            {
    #ifdef TIDL_REF_BATCH_NORM_DEBUG
              printf("i = %d \n",i0);
    #endif
              for (i1=0; i1<imWidth; i1++)
              {
                inDataVal = (Tin)(inData[(i5*inBatchPitch) + (i4*inDIM1Pitch) + (i3*inDIM2Pitch) + (i2*inChPitch) + (i0*inPitch) + i1]);
                out = ((inDataVal* weightVal) + (biasPtr[i2]));
                if (out < 0)
                {
                  if (typeid(Tacc) == typeid(float32_tidl))
                  {
                    out = out * preluScale;
                  }
                  else
                  {
                    out = TIDL_refBatchNormCoreRoundSlope((int32_t)out, slopeFact, slopeQBits);
                  }
                }
                min = (out<min) ? out : min;
                max = (out>max) ? out : max;

    #ifdef TIDL_REF_BATCH_NORM_DEBUG
                printf("[%d , %d, %d, %d, %d], ", i1, inDataVal, weightVal, biasPtr[i2], out);
    #endif
                refAccPtr[(i5*outBatchPitch) + (i4*outDIM1Pitch) + (i3*outDIM2Pitch) + (i2*outChPitch) + (i0*outPitch) + i1] = out;
              }
    #ifdef TIDL_REF_BATCH_NORM_DEBUG
                printf("\n");
    #endif
            }
          }
        }
      }
    }

    if((intAlgHandle->createParams->flowCtrl & TIDL_FLOW_CTRL_REF_STAT) == TIDL_FLOW_CTRL_REF_STAT)
    {
      if(TIDL_getDatElementSign(outDataParams->elementType) == 1)
      {
        TIDL_UpdateScaleFactors(intAlgHandle, layerIdx, 1, (int64_t)min, (int64_t)max);
      }
      else
      {
        TIDL_UpdateScaleFactors(intAlgHandle, layerIdx, 1, 0, (int64_t)max);
      }
    }

    int32_t satLow;
    int32_t satHigh;
    int32_t mixedPrecision = 0;
    int32_t outRoundBits;
    Tout temp;
    temp  = std::numeric_limits<Tout>::lowest();
    satLow = (int32_t)temp;
    temp  = std::numeric_limits<Tout>::max();

    satHigh = (int32_t)(temp);
    outRoundBits = outDataParams->roundBits;
    if ( sizeof(*weightsPtr) != sizeof(*outData))
    {
      mixedPrecision = 1;
      /* This is to match with target implementation wherin the saturations
    happens after applying outRoundBits - 8 shift and then the right shift
    of 8 is applied */
      outRoundBits = outRoundBits - 8;
      if ( net->TIDLLayers[layerIdx].outData.elementType == TIDL_SignedChar )
      {
        satLow  = std::numeric_limits<int16_t>::lowest();
        satHigh = std::numeric_limits<int16_t>::max();
      }
      else
      {
        satLow  = std::numeric_limits<uint16_t>::lowest();
        satHigh = std::numeric_limits<uint16_t>::max();
      }
    }

    //OPENACC(data copyout(outData[: 1 + ((numTotBatches-1)*outBatchPitch) + ((numChannels-1)*outChPitch) + ((imHeight-1)*outPitch) + (imWidth-1)]) \
                copyin(refAccPtr[: numTotBatches * outBatchPitch]))
    //OPENACC(parallel loop collapse(6))
    for (i5=0; i5<numTotBatches; i5++)
        {
          for (i4=0; i4<numDIM1; i4++)
          {
            for (i3=0; i3<numDIM2; i3++)
            {
              for (i2=0; i2<params->numChannels; i2++)
              {
                for (i0=0; i0<imHeight; i0++)
                {
                  for (i1=0; i1<imWidth; i1++)
                  {
                    out = refAccPtr[(i5*outBatchPitch) + (i4*outDIM1Pitch) + (i3*outDIM2Pitch) + (i2*outChPitch) + (i0*outPitch) + i1];
                    if (std::is_same<Tacc, float32_tidl>::value)
                    {
                      //OPENACC(routine(TIDL_floatSat))
                      out =  TIDL_floatSat(out, &net->TIDLLayers[layerIdx]);
                    }
                    else
                    {
                      //OPENACC(routine(TIDL_roundSat))
                      out = (int32_t)TIDL_roundSat((int64_t)out, outRoundBits , (int32_t)satLow, (int32_t)satHigh);
                      if ( mixedPrecision == 1 )
                      {
                        out = (int64_t)out >> 8;
                      }
                    }
                    outData[(i5*outBatchPitch) + (i4*outDIM1Pitch) + (i3*outDIM2Pitch) + (i2*outChPitch) + (i0*outPitch) + i1] = (Tout)out;
                  }
                }
              }
            }
          }
        }
      }
      return status;
}

/**
 * @brief This is main function perform Sigmoid on differnt elementTypes
 *
 * @tparam Tin  : template for input
 * @tparam Tout : template for output
 * @param inPtr : Input pointer on which bacthNorm is applied
 * @param outPtr : Output pointer after batchNorm opreation
 * @param intAlgHandle : tidl algorothm handle
 * @param layerIdx :index of the current layer
 * @param params : copy of bacthNorm parameters
 * @param inDataParams : parameters of the input data buffer
 * @param outDataParams : parameters of the output data buffer
 * @param slope : Pointer to slope values for sigmoid
 * @param offset : Pointer to offset values for sigmoid
 * @param satLow : min value for the saturation
 * @param satHigh : max value for the saturation
 * @return  IALG_EOK   - Successful
 *          IALG_EFAIL - Unspecified error
 */
template <class Tin, class Tout, class Tacc >
static int32_t TIDL_refSigmoidCore(Tin  *inPtr,
                                   Tout *outPtr,
                                   TIDL_Handle intAlgHandle,
                                   sTIDL_AlgLayer_t *algLayer,
                                   int32_t layerIdx,
                                   sTIDL_BatchNormParams_t *params,
                                   sTIDL_DataParams_t *inDataParams,
                                   sTIDL_DataParams_t *outDataParams,
                                   Tout *slope,
                                   Tout *offset,
                                   Tout offsetScale,
                                   Tacc *scratchMemPtr
                                   )
{
  int32_t status = TIDL_SUCCESS;
  int32_t i0, i1, i2, i3;
  int32_t imWidth    = (int32_t)inDataParams->dimValues[TIDL_DIM_WIDTH];
  int32_t imHeight   = (int32_t)inDataParams->dimValues[TIDL_DIM_HEIGHT];
  int32_t inPitch    = (int32_t)inDataParams->pitch[TIDL_LINE_PITCH];
  int32_t inChPitch  = (int32_t)inDataParams->pitch[TIDL_CHANNEL_PITCH];
  int32_t inBatchPitch  = (int32_t)inDataParams->pitch[TIDL_ROI_PITCH];
  int32_t outPitch   = (int32_t)outDataParams->pitch[TIDL_LINE_PITCH];
  int32_t outChPitch = (int32_t)outDataParams->pitch[TIDL_CHANNEL_PITCH];
  int32_t outBatchPitch = (int32_t)outDataParams->pitch[TIDL_ROI_PITCH];
  int32_t numTotBatches = inDataParams->dimValues[TIDL_DIM_BATCH];
  Tacc outVal;
  params->numChannels = inDataParams->dimValues[TIDL_DIM_NUMCH];
  Tin  *inData = (Tin *)inPtr;
  Tout *outData = (Tout *)outPtr;
  Tout out = 0;
  Tout satHigh, satLow;

  int32_t numChannels = params->numChannels;

  if((intAlgHandle->createParams->reservedCtrl & TIDL_HIGH_ACCURACY_SIGMOID) == 0)
  {
    float inputScale = inDataParams->tensorScale;
    uint16_t thresholdScale = 16; // 4-bits to avoid thresholds overflow
    uint32_t threshold0 = 5*thresholdScale*inputScale;
    uint32_t threshold1 = 2.375*thresholdScale*inputScale;
    uint32_t threshold2 = 1*thresholdScale*inputScale;
    uint16_t inDataScale = thresholdScale;

    Tacc *refAccPtr;
    int32_t accMemSize;
    uint32_t accumulatorSize = (uint32_t)(sizeof(Tacc));
    accMemSize = (numTotBatches * outBatchPitch* (int32_t)accumulatorSize);
    if(algLayer->scratchSize >= accMemSize)
    {
       refAccPtr = (Tacc *)scratchMemPtr;
    }
    else
    {
      tidl_printf(0,"Memory for  TIDL_refSigmoidCore accumulator is not sufficient exiting...\n    ");
      status = TIDL_ERR_FAILURE;
    }

//OPENACC(data copyin(inData[: 1 + ((numTotBatches-1)*inBatchPitch) + ((numChannels-1)*inChPitch) + ((imHeight-1)*inPitch) + (imWidth-1)]) \
             copy(outData[: 1 + ((numTotBatches-1)*outBatchPitch) + ((numChannels-1)*outChPitch) + ((imHeight-1)*outPitch) + (imWidth-1)]) \
             present(refAccPtr[: 1 + ((numTotBatches-1)*outBatchPitch) + ((numChannels-1)*outChPitch) + ((imHeight-1)*outPitch) + (imWidth-1)]))
{ // openacc start
    if(TIDL_SUCCESS == status)
    {
      int32_t min, max;
      min = std::numeric_limits<Tacc>::max();
      max = std::numeric_limits<Tacc>::lowest();
      satHigh = std::numeric_limits<Tout>::max();
      satLow = std::numeric_limits<Tout>::lowest();

      //OPENACC(parallel loop collapse(4))
      for (i3 = 0; i3 < numTotBatches; i3++)
      {
        for (i2 = 0; i2 < numChannels; i2++)
        {
          for (i0=0; i0<imHeight; i0++)
          {
            for (i1=0; i1<imWidth; i1++)
            {
              Tin  inDataVal, inDataValAbs, constZero = 0;
              uint32_t inDataValShl8;
              inDataVal = (inData[(i3*inBatchPitch) + (i2*inChPitch) + (i0*inPitch) + i1]);
              inDataValAbs = inDataVal < constZero?-inDataVal:inDataVal;
              inDataValShl8 = inDataValAbs * inDataScale;
              if(inDataValShl8 >= threshold0)
              {
                outVal = slope[0] * inDataValAbs + offset[0]*offsetScale;
              }
              else if ((inDataValShl8 < threshold0) && (inDataValShl8 >= threshold1))
              {
                outVal = slope[1] * inDataValAbs + offset[1]*offsetScale;
              }
              else if ((inDataValShl8 < threshold1) && (inDataValShl8 >= threshold2))
              {
                outVal = slope[2] * inDataValAbs + offset[2]*offsetScale;
              }
              else if ((inDataValShl8 < threshold2) && (inDataValShl8 >= constZero))
              {
                outVal = slope[3] * inDataValAbs + offset[3]*offsetScale;
              }

              if(inDataVal < constZero)
              {
                outVal = offset[0]*offsetScale - outVal;
              }

              min = (outVal<min) ? outVal : min;
              max = (outVal>max) ? outVal : max;
              refAccPtr[(i3*outBatchPitch) + (i2*outChPitch) + (i0*outPitch) + i1] = outVal;
            }
          }
        }
      }

      if((intAlgHandle->createParams->flowCtrl & TIDL_FLOW_CTRL_REF_STAT) == TIDL_FLOW_CTRL_REF_STAT)
      {
        if(TIDL_getDatElementSign(outDataParams->elementType) == 1)
        {
          TIDL_UpdateScaleFactors(intAlgHandle, layerIdx, 1, min, max);
        }
        else
        {
          TIDL_UpdateScaleFactors(intAlgHandle, layerIdx, 1, 0, max);
        }
      }

      //OPENACC(parallel loop collapse(4))
      for (i3=0; i3<numTotBatches; i3++)
      {
        for (i2=0; i2<params->numChannels; i2++)
        {
          for (i0=0; i0<imHeight; i0++)
          {
            for (i1=0; i1<imWidth; i1++)
            {
              outVal = refAccPtr[(i3*outBatchPitch) + (i2*outChPitch) + (i0*outPitch) + i1];
              //OPENACC(routine(TIDL_roundSat))
              out = (Tout)TIDL_roundSat(outVal, outDataParams->roundBits, satLow, satHigh);
              outData[(i3*outBatchPitch) + (i2*outChPitch) + (i0*outPitch) + i1] = (Tout)out;
            }
          }
        }
      }
    }
  }
  } // //OPENACC end
  else
  {
    float inValF, outValF;
    float inputScale = inDataParams->tensorScale;
    float inDataScaleFact = 1.0/inputScale;

    // OPENACC(parallel loop collapse(4))
    for (i3 = 0; i3 < numTotBatches; i3++)
    {
      for (i2 = 0; i2 < numChannels; i2++)
      {
        for (i0=0; i0<imHeight; i0++)
        {
          for (i1=0; i1<imWidth; i1++)
          {
            Tin  inDataVal;
            inDataVal = (inData[(i3*inBatchPitch) + (i2*inChPitch) + (i0*inPitch) + i1]);
            inValF = inDataVal * inDataScaleFact;
            outValF = div_sp(1.0f, (exp_taylor(-inValF) + 1.0f));
            out = outValF*outDataParams->tensorScale;
            outData[(i3*outBatchPitch) + (i2*outChPitch) + (i0*outPitch) + i1] = (Tout)out;
          }
        }
      }
    }
  }
  return status;
}
/**
 * @brief  This funciton handles differnt input bit-depths for reference batchNorm
 *
 * @tparam Tw   : template for weights
 * @tparam Tb   : template for Bias
 * @param inPtr : Input pointer on which bacthNorm is applied
 * @param outPtr : Output pointer after batchNorm opreation
 * @param weightsPtr : Pointer to weights buffer
 * @param slopePtr : Pointer to the Slope buffer
 * @param biasPtr  : Pointer to the Bias values
 * @param params : copy of bacthNorm parameters
 * @param intAlgHandle : tidl algorothm handle
 * @param layerIdx :index of the current layer
 * @param algLayer : Pointer to the layer specific parameters
 * @param inDataParams : parameters of the input data buffer
 * @param outDataParams : parameters of the output data buffer
 * @return  IALG_EOK   - Successful
 *          IALG_EFAIL - Unspecified error
 */
template <class Tw, class Tb, class Tacc>
static int32_t TIDL_refBatchNormBitDepth(void *inPtr,
                                         void *outPtr,
                                         const Tw *weightsPtr,
                                         const Tw *slopePtr,
                                         const Tb *biasPtr,
                                         Tacc     *accPtr,
                                         sTIDL_BatchNormParams_t *params,
                                         TIDL_Handle intAlgHandle,
                                         int32_t layerIdx,
                                         sTIDL_AlgLayer_t *algLayer,
                                         const sTIDL_DataParams_t *inDataParams,
                                         const sTIDL_DataParams_t *outDataParams)
{
  int32_t status = TIDL_SUCCESS;
  if(TIDL_SignedChar == ((int32_t)outDataParams->elementType))
  {
    if(TIDL_SignedChar == ((int32_t)inDataParams->elementType))
    {
      status = TIDL_refBatchNormCore((int8_t *)inPtr,
                                     (int8_t *)outPtr,
                                     weightsPtr,
                                     slopePtr,
                                     biasPtr,
                                     accPtr,
                                     intAlgHandle,
                                     layerIdx,
                                     params,
                                     algLayer,
                                     inDataParams,
                                     outDataParams);
    }
    else if(TIDL_UnsignedChar == ((int32_t)inDataParams->elementType))
    {
      status = TIDL_refBatchNormCore((uint8_t *)inPtr,
                                     (int8_t *)outPtr,
                                     weightsPtr,
                                     slopePtr,
                                     biasPtr,
                                     accPtr,
                                     intAlgHandle,
                                     layerIdx,
                                     params,
                                     algLayer,
                                     inDataParams,
                                     outDataParams);
    }
    else if(TIDL_SignedShort == ((int32_t)inDataParams->elementType))
    {
      status = TIDL_refBatchNormCore((int16_t *)inPtr,
                                     (int8_t *)outPtr,
                                     weightsPtr,
                                     slopePtr,
                                     biasPtr,
                                     accPtr,
                                     intAlgHandle,
                                     layerIdx,
                                     params,
                                     algLayer,
                                     inDataParams,
                                     outDataParams);
    }
    else if(TIDL_UnsignedShort == ((int32_t)inDataParams->elementType))
    {
      status = TIDL_refBatchNormCore((uint16_t *)inPtr,
                                     (int8_t *)outPtr,
                                     weightsPtr,
                                     slopePtr,
                                     biasPtr,
                                     accPtr,
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
  }
  else if(TIDL_UnsignedChar == ((int32_t)outDataParams->elementType))
  {
    if(TIDL_SignedChar == ((int32_t)inDataParams->elementType))
    {
      status = TIDL_refBatchNormCore((int8_t *)inPtr,
                                     (uint8_t *)outPtr,
                                     weightsPtr,
                                     slopePtr,
                                     biasPtr,
                                     accPtr,
                                     intAlgHandle,
                                     layerIdx,
                                     params,
                                     algLayer,
                                     inDataParams,
                                     outDataParams);
    }
    else if(TIDL_UnsignedChar == ((int32_t)inDataParams->elementType))
    {
      status = TIDL_refBatchNormCore((uint8_t *)inPtr,
                                     (uint8_t *)outPtr,
                                     weightsPtr,
                                     slopePtr,
                                     biasPtr,
                                     accPtr,
                                     intAlgHandle,
                                     layerIdx,
                                     params,
                                     algLayer,
                                     inDataParams,
                                     outDataParams);
    }
    else if(TIDL_SignedShort == ((int32_t)inDataParams->elementType))
    {
      status = TIDL_refBatchNormCore((int16_t *)inPtr,
                                     (uint8_t *)outPtr,
                                     weightsPtr,
                                     slopePtr,
                                     biasPtr,
                                     accPtr,
                                     intAlgHandle,
                                     layerIdx,
                                     params,
                                     algLayer,
                                     inDataParams,
                                     outDataParams);
    }
    else if(TIDL_UnsignedShort == ((int32_t)inDataParams->elementType))
    {
      status = TIDL_refBatchNormCore((uint16_t *)inPtr,
                                     (uint8_t *)outPtr,
                                     weightsPtr,
                                     slopePtr,
                                     biasPtr,
                                     accPtr,
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
  }
  else if(TIDL_SignedShort == ((int32_t)outDataParams->elementType))
  {
    if(TIDL_SignedChar == ((int32_t)inDataParams->elementType))
    {
      status = TIDL_refBatchNormCore((int8_t *)inPtr,
                                     (int16_t *)outPtr,
                                     weightsPtr,
                                     slopePtr,
                                     biasPtr,
                                     accPtr,
                                     intAlgHandle,
                                     layerIdx,
                                     params,
                                     algLayer,
                                     inDataParams,
                                     outDataParams);
    }
    else if(TIDL_UnsignedChar == ((int32_t)inDataParams->elementType))
    {
      status = TIDL_refBatchNormCore((uint8_t *)inPtr,
                                     (int16_t *)outPtr,
                                     weightsPtr,
                                     slopePtr,
                                     biasPtr,
                                     accPtr,
                                     intAlgHandle,
                                     layerIdx,
                                     params,
                                     algLayer,
                                     inDataParams,
                                     outDataParams);
    }
    else if(TIDL_SignedShort == ((int32_t)inDataParams->elementType))
    {
      status = TIDL_refBatchNormCore((int16_t *)inPtr,
                                     (int16_t *)outPtr,
                                     weightsPtr,
                                     slopePtr,
                                     biasPtr,
                                     accPtr,
                                     intAlgHandle,
                                     layerIdx,
                                     params,
                                     algLayer,
                                     inDataParams,
                                     outDataParams);
    }
    else if(TIDL_UnsignedShort == ((int32_t)inDataParams->elementType))
    {
      status = TIDL_refBatchNormCore((uint16_t *)inPtr,
                                     (int16_t *)outPtr,
                                     weightsPtr,
                                     slopePtr,
                                     biasPtr,
                                     accPtr,
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
  }
  else if(TIDL_UnsignedShort == ((int32_t)outDataParams->elementType))
  {
    if(TIDL_SignedChar == ((int32_t)inDataParams->elementType))
    {
      status = TIDL_refBatchNormCore((int8_t *)inPtr,
                                     (uint16_t *)outPtr,
                                     weightsPtr,
                                     slopePtr,
                                     biasPtr,
                                     accPtr,
                                     intAlgHandle,
                                     layerIdx,
                                     params,
                                     algLayer,
                                     inDataParams,
                                     outDataParams);
    }
    else if(TIDL_UnsignedChar == ((int32_t)inDataParams->elementType))
    {
      status = TIDL_refBatchNormCore((uint8_t *)inPtr,
                                     (uint16_t *)outPtr,
                                     weightsPtr,
                                     slopePtr,
                                     biasPtr,
                                     accPtr,
                                     intAlgHandle,
                                     layerIdx,
                                     params,
                                     algLayer,
                                     inDataParams,
                                     outDataParams);
    }
    else if(TIDL_SignedShort == ((int32_t)inDataParams->elementType))
    {
      status = TIDL_refBatchNormCore((int16_t *)inPtr,
                                     (uint16_t *)outPtr,
                                     weightsPtr,
                                     slopePtr,
                                     biasPtr,
                                     accPtr,
                                     intAlgHandle,
                                     layerIdx,
                                     params,
                                     algLayer,
                                     inDataParams,
                                     outDataParams);
    }
    else if(TIDL_UnsignedShort == ((int32_t)inDataParams->elementType))
    {
      status = TIDL_refBatchNormCore((uint16_t *)inPtr,
                                     (uint16_t *)outPtr,
                                     weightsPtr,
                                     slopePtr,
                                     biasPtr,
                                     accPtr,
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
  }
  else if(TIDL_SinglePrecFloat == ((int32_t)outDataParams->elementType))
  {
    status = TIDL_refBatchNormCore((float32_tidl *)inPtr,
                                    (float32_tidl *)outPtr,
                                    weightsPtr,
                                    slopePtr,
                                    biasPtr,
                                    accPtr,
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
  return status;
}
/**
 * @brief This function is sigmoid reference implementation
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
static int32_t TIDL_refSigmoidProcess(TIDL_Handle intAlgHandle,
                                      sTIDL_AlgLayer_t *algLayer,
                                      const sTIDL_Layer_t    *tidlLayer,
                                      sTIDL_BatchNormParams_t *params,
                                      void *inPtr,
                                      void *outPtr,
                                      sTIDL_DataParams_t *inDataParams,
                                      sTIDL_DataParams_t *outDataParams)
{
  int32_t status = TIDL_SUCCESS;
  int32_t layerIdx = algLayer->layerIdx;

  if(TIDL_UnsignedChar == ((int32_t)outDataParams->elementType))
  {
    /*
    slope = 0, 0.03125, 0.125, 0.25
    offset = 1, 0.84375, 0.625, 0.5
    Convert Slope and offset values to 7-bits (NUM_WHGT_BITS-1) */
    uint8_t TIDL_sigmoidSlope[4] = {0, 4, 16, 32};
    uint8_t TIDL_sigmoidOffset[4] = {128, 108, 80, 64};
    uint8_t OffsetScale = 1;
    /* Divide 16-bit offsets to two 8-bit offsets(Offset0, OffsetScale) and so on */
    if (inDataParams->tensorScale > 1.0)
    {
      OffsetScale = (uint8_t)(inDataParams->tensorScale);
      TIDL_sigmoidOffset[0] = (TIDL_sigmoidOffset[0]*inDataParams->tensorScale)/OffsetScale;
      TIDL_sigmoidOffset[1] = (TIDL_sigmoidOffset[1]*inDataParams->tensorScale)/OffsetScale;
      TIDL_sigmoidOffset[2] = (TIDL_sigmoidOffset[2]*inDataParams->tensorScale)/OffsetScale;
      TIDL_sigmoidOffset[3] = (TIDL_sigmoidOffset[3]*inDataParams->tensorScale)/OffsetScale;
    }
    else
    {
      OffsetScale = 1;
      TIDL_sigmoidOffset[0] *= inDataParams->tensorScale;
      TIDL_sigmoidOffset[1] *= inDataParams->tensorScale;
      TIDL_sigmoidOffset[2] *= inDataParams->tensorScale;
      TIDL_sigmoidOffset[3] *= inDataParams->tensorScale;
    }

    if(TIDL_SignedChar == ((int32_t)inDataParams->elementType))
    {
      status = TIDL_refSigmoidCore((int8_t*)inPtr,
                                  (uint8_t*)outPtr,
                                   intAlgHandle,
                                   algLayer,
                                   layerIdx,
                                   params,
                                   inDataParams,
                                   outDataParams,
                                   TIDL_sigmoidSlope,
                                   TIDL_sigmoidOffset,
                                   (uint8_t)OffsetScale,
                                   (int32_t*)algLayer->scratchMem
                                   );
    }
    else if(TIDL_UnsignedChar == ((int32_t)inDataParams->elementType))
    {
      status = TIDL_refSigmoidCore((uint8_t*)inPtr,
                                  (uint8_t*)outPtr,
                                   intAlgHandle,
                                   algLayer,
                                   layerIdx,
                                   params,
                                   inDataParams,
                                   outDataParams,
                                   TIDL_sigmoidSlope,
                                   TIDL_sigmoidOffset,
                                   (uint8_t)OffsetScale,
                                   (int32_t*)algLayer->scratchMem
                                   );
    }
  }
  else if(TIDL_UnsignedShort == ((int32_t)outDataParams->elementType))
  {
    /*
    slope = 0, 0.03125, 0.125, 0.25
    offset = 1, 0.84375, 0.625, 0.5
    Convert Slope and offset values to 15-bits (NUM_WHGT_BITS-1) */
    uint16_t TIDL_sigmoidSlope[4] = {0, 1024, 4096, 8192};
    uint16_t TIDL_sigmoidOffset[4] = {32768, 27648, 20480, 16384};
    uint16_t OffsetScale = 1;
    if (inDataParams->tensorScale > 1.0)
    {
      OffsetScale = (uint16_t)(inDataParams->tensorScale);
      TIDL_sigmoidOffset[0] = (TIDL_sigmoidOffset[0]*inDataParams->tensorScale)/OffsetScale;
      TIDL_sigmoidOffset[1] = (TIDL_sigmoidOffset[1]*inDataParams->tensorScale)/OffsetScale;
      TIDL_sigmoidOffset[2] = (TIDL_sigmoidOffset[2]*inDataParams->tensorScale)/OffsetScale;
      TIDL_sigmoidOffset[3] = (TIDL_sigmoidOffset[3]*inDataParams->tensorScale)/OffsetScale;
    }
    else
    {
      OffsetScale = 1;
      TIDL_sigmoidOffset[0] *= inDataParams->tensorScale;
      TIDL_sigmoidOffset[1] *= inDataParams->tensorScale;
      TIDL_sigmoidOffset[2] *= inDataParams->tensorScale;
      TIDL_sigmoidOffset[3] *= inDataParams->tensorScale;
    }

    if(TIDL_SignedShort == ((int32_t)inDataParams->elementType))
    {
      status = TIDL_refSigmoidCore((int16_t*)inPtr,
                                   (uint16_t*)outPtr,
                                   intAlgHandle,
                                   algLayer,
                                   layerIdx,
                                   params,
                                   inDataParams,
                                   outDataParams,
                                   TIDL_sigmoidSlope,
                                   TIDL_sigmoidOffset,
                                   (uint16_t)OffsetScale,
                                   (int32_t*)algLayer->scratchMem
                                   );
    }
    else if(TIDL_UnsignedShort == ((int32_t)inDataParams->elementType))
    {
      status = TIDL_refSigmoidCore((uint16_t*)inPtr,
                                   (uint16_t*)outPtr,
                                   intAlgHandle,
                                   algLayer,
                                   layerIdx,
                                   params,
                                   inDataParams,
                                   outDataParams,
                                   TIDL_sigmoidSlope,
                                   TIDL_sigmoidOffset,
                                   (uint16_t)OffsetScale,
                                   (int32_t*)algLayer->scratchMem
                                   );
    }
  }
  else  //if(TIDL_SinglePrecFloat == ((int32_t)buffParams->inElementType))
  {
    int32_t i0, i1, i2, i3;
    int32_t imWidth    = (int32_t)inDataParams->dimValues[TIDL_DIM_WIDTH];
    int32_t imHeight   = (int32_t)inDataParams->dimValues[TIDL_DIM_HEIGHT];
    int32_t inPitch    = (int32_t)inDataParams->pitch[TIDL_LINE_PITCH];
    int32_t inChPitch  = (int32_t)inDataParams->pitch[TIDL_CHANNEL_PITCH];
    int32_t outPitch   = (int32_t)outDataParams->pitch[TIDL_LINE_PITCH];
    int32_t outChPitch = (int32_t)outDataParams->pitch[TIDL_CHANNEL_PITCH];
    int32_t numTotBatches = inDataParams->dimValues[TIDL_DIM_BATCH];
    params->numChannels = inDataParams->dimValues[TIDL_DIM_NUMCH];
    float32_tidl *inData = (float32_tidl *)inPtr ;
    float32_tidl *outData = (float32_tidl *)outPtr;
    float32_tidl out = 0;
    float32_tidl Max = 1.0, Min = 0;

    int32_t c = params->numChannels;

    //OPENACC(data copyin(inData[0:1+((numTotBatches-1)*c*inChPitch) + ((c-1)*inChPitch) + ((imHeight-1)*inPitch) + (imWidth-1)]) \
                 copy(outData[0:1+((numTotBatches-1)*c*outChPitch) + ((c-1)*outChPitch) + ((imHeight-1)*outPitch) + (imWidth-1)]))

    //OPENACC(parallel loop collapse(4))
    for (i3 = 0; i3 < numTotBatches; i3++)
    {
      for (i2 = 0; i2 < c; i2++)
      {
        for (i0=0; i0<imHeight; i0++)
        {
          for (i1=0; i1<imWidth; i1++)
          {
            float32_tidl  inDataVal;
            inDataVal = (inData[(i3*c*inChPitch) + (i2*inChPitch) + (i0*inPitch) + i1]);
            out = 1.0/(1.0+exp(-inDataVal));
            Min = (out<Min) ? out : Min;
            Max = (out>Max) ? out : Max;
            outData[(i3*c*outChPitch) + (i2*outChPitch) + (i0*outPitch) + i1] = (float32_tidl)out;
          }
        }
      }
    }

    if((intAlgHandle->createParams->flowCtrl & TIDL_FLOW_CTRL_REF_STAT) == TIDL_FLOW_CTRL_REF_STAT)
    {
      //OPENACC(routine(TIDL_getDatElementSign))
      if(TIDL_getDatElementSign(outDataParams->elementType) == 1)
      {
        //OPENACC(routine(TIDL_UpdateScaleFactors))
        TIDL_UpdateScaleFactors(intAlgHandle, layerIdx, 1, Min, Max);
      }
      else
      {
        //OPENACC(routine(TIDL_UpdateScaleFactors))
        TIDL_UpdateScaleFactors(intAlgHandle, layerIdx, 1, 0, Max);
      }
    }
  }
  return status;
}

/**
 * @brief This is main function perform Non Linear function tramsformation on differnt elementTypes
 *
 * @tparam Tin  : template for input
 * @tparam Tout : template for output
 * @param inPtr : Input pointer on which bacthNorm is applied
 * @param outPtr : Output pointer after batchNorm opreation
 * @param layerIdx :index of the current layer
 * @param params : copy of bacthNorm parameters
 * @param inDataParams : parameters of the input data buffer
 * @param outDataParams : parameters of the output data buffer
 * @param tidlLayer : Pointer to the common layer parameters
 * @param LUT_data_table : pointer to 256-element table for look up
 * @param readOffsetLUT : initial read offset for indexing LUT directly using input data
 * @return  IALG_EOK   - Successful
 *          IALG_EFAIL - Unspecified error
 */
template <class Tin, class Tout>
static int32_t TIDL_refNonLinearLUTCore(Tin  *inPtr,
                                   Tout *outPtr,
                                   int32_t layerIdx,
                                   sTIDL_BatchNormParams_t *params,
                                   sTIDL_DataParams_t *inDataParams,
                                   sTIDL_DataParams_t *outDataParams,
                                   const sTIDL_Layer_t    *tidlLayer,
                                   Tout *LUT_data_table,
                                   int32_t readOffsetLUT
                                   )
{
  int32_t status = TIDL_SUCCESS;
  int32_t i0, i1, i2, i3;
  int32_t imWidth    = (int32_t)inDataParams->dimValues[TIDL_DIM_WIDTH];
  int32_t imHeight   = (int32_t)inDataParams->dimValues[TIDL_DIM_HEIGHT];
  int32_t inPitch    = (int32_t)inDataParams->pitch[TIDL_LINE_PITCH];
  int32_t inChPitch  = (int32_t)inDataParams->pitch[TIDL_CHANNEL_PITCH];
  int32_t inBatchPitch  = (int32_t)inDataParams->pitch[TIDL_ROI_PITCH];
  int32_t outPitch   = (int32_t)outDataParams->pitch[TIDL_LINE_PITCH];
  int32_t outChPitch = (int32_t)outDataParams->pitch[TIDL_CHANNEL_PITCH];
  int32_t outBatchPitch = (int32_t)outDataParams->pitch[TIDL_ROI_PITCH];
  int32_t numTotBatches = inDataParams->dimValues[TIDL_DIM_BATCH];
  params->numChannels = inDataParams->dimValues[TIDL_DIM_NUMCH];
  /*workload already adds offset so not to add here
  */
  Tin  *inData = (Tin *)inPtr;
  Tout *outData = (Tout *)outPtr;

  int32_t c = params->numChannels;
  //OPENACC(data copyin(inData[0:1+((numTotBatches-1)*inBatchPitch) + ((c-1)*inChPitch) + ((imHeight-1)*inPitch) + (imWidth-1)], \
                      LUT_data_table[0:1+((numTotBatches-1)*inBatchPitch) + ((c-1)*inChPitch) + ((imHeight-1)*inPitch) + (imWidth-1) + 1]) \
               copy(outData[0:1+((numTotBatches-1)*outBatchPitch) + ((c-1)*outChPitch) + ((imHeight-1)*outPitch) + (imWidth-1)]))

  //OPENACC(parallel loop collapse(4))
  for (i3 = 0; i3 < numTotBatches; i3++)
  {
    for (i2 = 0; i2 < c; i2++)
    {
      for (i0=0; i0<imHeight; i0++)
      {
        for (i1=0; i1<imWidth; i1++)
        {
          Tin  inDataVal;
          inDataVal = (inData[(i3*inBatchPitch) + (i2*inChPitch) + (i0*inPitch) + i1]);
          outData[(i3*outBatchPitch) + (i2*outChPitch) + (i0*outPitch) + i1] = LUT_data_table[inDataVal+readOffsetLUT];
        }
      }
    }
  }
  return status;
}

template <class Tin, class Tout>
static int32_t TIDL_refNonLinearNonLUTCore(Tin  *inPtr,
                                                Tout *outPtr,
                                                int32_t layerIdx,
                                                sTIDL_BatchNormParams_t *params,
                                                sTIDL_DataParams_t *inDataParams,
                                                sTIDL_DataParams_t *outDataParams,
                                                const sTIDL_Layer_t    *tidlLayer,
                                                int32_t (*transformation_ptr)(const void *, void *, int32_t, float32_tidl, int32_t, float32_tidl, float32_tidl, float32_tidl))
{
  int32_t status = TIDL_SUCCESS;
  int32_t i0, i1, i2, i3;
  float32_tidl alpha = -1.0, beta = -1.0;
  int32_t imWidth       = (int32_t)inDataParams->dimValues[TIDL_DIM_WIDTH];
  int32_t imHeight      = (int32_t)inDataParams->dimValues[TIDL_DIM_HEIGHT];
  int32_t inPitch       = (int32_t)inDataParams->pitch[TIDL_LINE_PITCH];
  int32_t inChPitch     = (int32_t)inDataParams->pitch[TIDL_CHANNEL_PITCH];
  int32_t inBatchPitch  = (int32_t)inDataParams->pitch[TIDL_ROI_PITCH];
  int32_t outPitch      = (int32_t)outDataParams->pitch[TIDL_LINE_PITCH];
  int32_t outChPitch    = (int32_t)outDataParams->pitch[TIDL_CHANNEL_PITCH];
  int32_t outBatchPitch = (int32_t)outDataParams->pitch[TIDL_ROI_PITCH];
  int32_t numTotBatches = inDataParams->dimValues[TIDL_DIM_BATCH];
  params->numChannels   = inDataParams->dimValues[TIDL_DIM_NUMCH];
  int32_t Zx            = inDataParams->tensorZeroPoint;
  float32_tidl Sx       = inDataParams->tensorScale;
  int32_t Zy            = outDataParams->tensorZeroPoint;
  float32_tidl Sy       = outDataParams->tensorScale;

  /*workload already adds offset so not to add here
  */
  Tin  *inData = (Tin *)inPtr;
  Tout *outData = (Tout *)outPtr;

  if(tidlLayer->actParams.actType == TIDL_ELU)
  {
    alpha = (float)tidlLayer->layerParams.batchNormParams.inDataQ/((float)TIDL_NON_LIN_PARAM_SCALE);
  }
  if(tidlLayer->actParams.actType == TIDL_HardSigmoid)
  {
    alpha = (float)tidlLayer->layerParams.batchNormParams.inDataQ/((float)TIDL_NON_LIN_PARAM_SCALE);
    beta = (float)tidlLayer->layerParams.batchNormParams.weightsQ/((float)TIDL_NON_LIN_PARAM_SCALE);
  }


  for(i3 = 0; i3 < numTotBatches; i3++)
  {
    Tin  inDataVal;
    Tout outDataVal;
    for(i2 = 0; i2 < params->numChannels; i2++)
    {
      for (i0=0; i0<imHeight; i0++)
      {
        for (i1=0; i1<imWidth; i1++)
        {
          inDataVal = (inData[(i3*inBatchPitch) + (i2*inChPitch) + (i0*inPitch) + i1]);
          status = (*transformation_ptr)((const void *)&inDataVal,(void *)&outDataVal, Zx, Sx, Zy, Sy, alpha, beta);
          outData[(i3*outBatchPitch) + (i2*outChPitch) + (i0*outPitch) + i1] = outDataVal;
        }
      }
    }
  }
  return status;
}

/**
 * @brief This function is Non Linear function LUT-based reference implementation
 *
 * @param intAlgHandle : tidl algorothm handle
 * @param algLayer : Pointer to the layer specific parameters
 * @param tidlLayer : Pointer to the common layer parameters
 * @param params : copy of batch norm layer parameters
 * @param inPtr : Pointer to input buffers to be processed
 * @param outPtr : Pointer to output buffers to be processed
 * @param inDataParams : pointer to input data parameters
 * @param outDataParams : pointer to output data parameters
 * @param LUT_data_table : pointer to 256-element table for look up
 * @return  IALG_EOK   - Successful
 *          IALG_EFAIL - Unspecified error
 */
static int32_t TIDL_refNonLinearLUTProcess(TIDL_Handle intAlgHandle,
                                      sTIDL_AlgLayer_t *algLayer,
                                      const sTIDL_Layer_t    *tidlLayer,
                                      sTIDL_BatchNormParams_t *params,
                                      void *inPtr,
                                      void *outPtr,
                                      sTIDL_DataParams_t *inDataParams,
                                      sTIDL_DataParams_t *outDataParams,
                                      void *LUT_data_table)
{
  int32_t status = TIDL_SUCCESS;
  int32_t layerIdx = algLayer->layerIdx;
  int32_t readOffsetLUT, isOutputSigned;

  if(TIDL_UnsignedChar == ((int32_t)outDataParams->elementType))
  {
    if(TIDL_SignedChar == ((int32_t)inDataParams->elementType))
    {
      TIDL_prepareLUTForNonLinearTransform8bit((int8_t*)inPtr, (uint8_t*)outPtr, tidlLayer, (uint8_t*)LUT_data_table, inDataParams, outDataParams, &readOffsetLUT, &isOutputSigned);
      status = TIDL_refNonLinearLUTCore((int8_t*)inPtr,
                                  (uint8_t*)outPtr,
                                   layerIdx,
                                   params,
                                   inDataParams,
                                   outDataParams,
                                   tidlLayer,
                                   (uint8_t*)LUT_data_table,
                                   readOffsetLUT
                                   );
    }
    else if(TIDL_UnsignedChar == ((int32_t)inDataParams->elementType))
    {
      TIDL_prepareLUTForNonLinearTransform8bit((uint8_t*)inPtr, (uint8_t*)outPtr, tidlLayer, (uint8_t*)LUT_data_table, inDataParams, outDataParams, &readOffsetLUT, &isOutputSigned);
      status = TIDL_refNonLinearLUTCore((uint8_t*)inPtr,
                                  (uint8_t*)outPtr,
                                   layerIdx,
                                   params,
                                   inDataParams,
                                   outDataParams,
                                   tidlLayer,
                                   (uint8_t*)LUT_data_table,
                                   readOffsetLUT
                                   );
    }
  }
  else if(TIDL_SignedChar == ((int32_t)outDataParams->elementType))
  {
    if(TIDL_SignedChar == ((int32_t)inDataParams->elementType))
    {
      TIDL_prepareLUTForNonLinearTransform8bit((int8_t*)inPtr, (int8_t*)outPtr, tidlLayer, (int8_t*)LUT_data_table, inDataParams, outDataParams, &readOffsetLUT, &isOutputSigned);
      status = TIDL_refNonLinearLUTCore((int8_t*)inPtr,
                                  (int8_t*)outPtr,
                                   layerIdx,
                                   params,
                                   inDataParams,
                                   outDataParams,
                                   tidlLayer,
                                   (int8_t*)LUT_data_table,
                                   readOffsetLUT
                                   );
    }
    else if(TIDL_UnsignedChar == ((int32_t)inDataParams->elementType))
    {
      TIDL_prepareLUTForNonLinearTransform8bit((uint8_t*)inPtr, (int8_t*)outPtr, tidlLayer, (int8_t*)LUT_data_table, inDataParams, outDataParams, &readOffsetLUT, &isOutputSigned);
      status = TIDL_refNonLinearLUTCore((uint8_t*)inPtr,
                                  (int8_t*)outPtr,
                                   layerIdx,
                                   params,
                                   inDataParams,
                                   outDataParams,
                                   tidlLayer,
                                   (int8_t*)LUT_data_table,
                                   readOffsetLUT
                                   );
    }
  }

  return status;
}

/**
 * @brief This function is Non Linear function Non-LUT based reference implementation
 *
 * @param intAlgHandle : tidl algorothm handle
 * @param algLayer : Pointer to the layer specific parameters
 * @param tidlLayer : Pointer to the common layer parameters
 * @param params : copy of batch norm layer parameters
 * @param inPtr : Pointer to input buffers to be processed
 * @param outPtr : Pointer to output buffers to be processed
 * @param inDataParams : pointer to input data parameters
 * @param outDataParams : pointer to output data parameters
 * @param LUT_data_table : pointer to 256-element table for look up
 * @return  IALG_EOK   - Successful
 *          IALG_EFAIL - Unspecified error
 */
template<class Tin, class Tout>
static int32_t TIDL_refNonLinearNonLUTProcess(TIDL_Handle intAlgHandle,
                                      sTIDL_AlgLayer_t *algLayer,
                                      const sTIDL_Layer_t    *tidlLayer,
                                      sTIDL_BatchNormParams_t *params,
                                      void *inPtr,
                                      void *outPtr,
                                      sTIDL_DataParams_t *inDataParams,
                                      sTIDL_DataParams_t *outDataParams)
{
  int32_t status = TIDL_SUCCESS;
  int32_t layerIdx = algLayer->layerIdx;

  int32_t (*transformation_ptr)(const void *, void *, int32_t, float32_tidl, int32_t, float32_tidl, float32_tidl, float32_tidl);  
  if(tidlLayer->actParams.actType == TIDL_Tanh)
  {
    transformation_ptr = TIDL_tanh_v2<Tin, Tout>; 
    status = TIDL_refNonLinearNonLUTCore<Tin, Tout>((Tin*)inPtr,
                                  (Tout*)outPtr,
                                   layerIdx,
                                   params,
                                   inDataParams,
                                   outDataParams,
                                   tidlLayer,
                                   transformation_ptr);
  }
  else if(tidlLayer->actParams.actType == TIDL_HardSigmoid)
  {
    transformation_ptr = TIDL_hardSigmoid_v2<Tin, Tout>; 
    status = TIDL_refNonLinearNonLUTCore<Tin, Tout>((Tin*)inPtr,
                                  (Tout*)outPtr,
                                   layerIdx,
                                   params,
                                   inDataParams,
                                   outDataParams,
                                   tidlLayer,
                                   transformation_ptr);
  }
  else if(tidlLayer->actParams.actType == TIDL_ELU)
  {
    transformation_ptr = TIDL_elu_v2<Tin, Tout>; 
    status = TIDL_refNonLinearNonLUTCore<Tin, Tout>((Tin*)inPtr,
                                  (Tout*)outPtr,
                                   layerIdx,
                                   params,
                                   inDataParams,
                                   outDataParams,
                                   tidlLayer,
                                   transformation_ptr);
  }
  else if (tidlLayer->actParams.actType == TIDL_GELU)
  {
    transformation_ptr = TIDL_gelu_v2<Tin, Tout>; 
    status = TIDL_refNonLinearNonLUTCore<Tin, Tout>((Tin*)inPtr,
                                  (Tout*)outPtr,
                                   layerIdx,
                                   params,
                                   inDataParams,
                                   outDataParams,
                                   tidlLayer,
                                   transformation_ptr);
  }

  return status;
}

static int32_t TIDL_refNonLinearNonLUTProcessWrapper(TIDL_Handle intAlgHandle,
                                      sTIDL_AlgLayer_t *algLayer,
                                      const sTIDL_Layer_t    *tidlLayer,
                                      sTIDL_BatchNormParams_t *params,
                                      void *inPtr,
                                      void *outPtr,
                                      sTIDL_DataParams_t *inDataParams,
                                      sTIDL_DataParams_t *outDataParams)
{
  int32_t status = TIDL_SUCCESS;

  if(TIDL_UnsignedChar == ((int32_t)outDataParams->elementType))
  {
    if(TIDL_SignedChar == ((int32_t)inDataParams->elementType))
    {
      status = TIDL_refNonLinearNonLUTProcess<int8_t, uint8_t>(intAlgHandle,
                                                                  algLayer,
                                                                  tidlLayer,
                                                                  params,
                                                                  inPtr,
                                                                  outPtr,
                                                                  inDataParams,
                                                                  outDataParams);
    }
    else if(TIDL_UnsignedChar == ((int32_t)inDataParams->elementType))
    {
      status = TIDL_refNonLinearNonLUTProcess<uint8_t, uint8_t>(intAlgHandle,
                                                                  algLayer,
                                                                  tidlLayer,
                                                                  params,
                                                                  inPtr,
                                                                  outPtr,
                                                                  inDataParams,
                                                                  outDataParams);
    }
  }
  else if(TIDL_SignedChar == ((int32_t)outDataParams->elementType))
  {
    if(TIDL_SignedChar == ((int32_t)inDataParams->elementType))
    {
      status = TIDL_refNonLinearNonLUTProcess<int8_t, int8_t>(intAlgHandle,
                                                                  algLayer,
                                                                  tidlLayer,
                                                                  params,
                                                                  inPtr,
                                                                  outPtr,
                                                                  inDataParams,
                                                                  outDataParams);
    }
    else if(TIDL_UnsignedChar == ((int32_t)inDataParams->elementType))
    {
      status = TIDL_refNonLinearNonLUTProcess<uint8_t, int8_t>(intAlgHandle,
                                                                  algLayer,
                                                                  tidlLayer,
                                                                  params,
                                                                  inPtr,
                                                                  outPtr,
                                                                  inDataParams,
                                                                  outDataParams);
    }
  }
  else if(TIDL_UnsignedShort == ((int32_t)outDataParams->elementType))
  {
    if(TIDL_SignedShort == ((int32_t)inDataParams->elementType))
    {
      status = TIDL_refNonLinearNonLUTProcess<int16_t, int16_t>(intAlgHandle,
                                                                  algLayer,
                                                                  tidlLayer,
                                                                  params,
                                                                  inPtr,
                                                                  outPtr,
                                                                  inDataParams,
                                                                  outDataParams);
    }
    else if(TIDL_UnsignedShort == ((int32_t)inDataParams->elementType))
    {
      status = TIDL_refNonLinearNonLUTProcess<uint16_t, int16_t>(intAlgHandle,
                                                                  algLayer,
                                                                  tidlLayer,
                                                                  params,
                                                                  inPtr,
                                                                  outPtr,
                                                                  inDataParams,
                                                                  outDataParams);
    }
  }
  else if(TIDL_SignedShort == ((int32_t)outDataParams->elementType))
  {
    if(TIDL_SignedShort == ((int32_t)inDataParams->elementType))
    {
      status = TIDL_refNonLinearNonLUTProcess<int16_t, int16_t>(intAlgHandle,
                                                                  algLayer,
                                                                  tidlLayer,
                                                                  params,
                                                                  inPtr,
                                                                  outPtr,
                                                                  inDataParams,
                                                                  outDataParams);
    }
    else if(TIDL_UnsignedShort == ((int32_t)inDataParams->elementType))
    {
      status = TIDL_refNonLinearNonLUTProcess<uint16_t, int16_t>(intAlgHandle,
                                                                  algLayer,
                                                                  tidlLayer,
                                                                  params,
                                                                  inPtr,
                                                                  outPtr,
                                                                  inDataParams,
                                                                  outDataParams);
    }
  }

  return status; 
}

/**
 * @brief This is main function perform Tanh operation if inp-output is in float-float manner
 * @tparam Tin  : template for input
 * @tparam Tout : template for output
 * @param inPtr : Input pointer on which bacthNorm is applied
 * @param outPtr : Output pointer after batchNorm opreation
 * @param layerIdx :index of the current layer
 * @param params : copy of bacthNorm parameters
 * @param inDataParams : parameters of the input data buffer
 * @param outDataParams : parameters of the output data buffer
 * @param tidlLayer : Pointer to the common layer parameters
 * @return  IALG_EOK   - Successful
 *          IALG_EFAIL - Unspecified error
 */
template <class Tin, class Tout>
static int32_t TIDL_refFloatNonLinear(Tin  *inPtr,
                                   Tout *outPtr,
                                   int32_t layerIdx,
                                   sTIDL_BatchNormParams_t *params,
                                   sTIDL_DataParams_t *inDataParams,
                                   sTIDL_DataParams_t *outDataParams,
                                   const sTIDL_Layer_t    *tidlLayer
                                   )
{
  int32_t status = TIDL_SUCCESS;
  int32_t i0, i1, i2, i3;
  int32_t imWidth    = (int32_t)inDataParams->dimValues[TIDL_DIM_WIDTH];
  int32_t imHeight   = (int32_t)inDataParams->dimValues[TIDL_DIM_HEIGHT];
  int32_t inPitch    = (int32_t)inDataParams->pitch[TIDL_LINE_PITCH];
  int32_t inChPitch  = (int32_t)inDataParams->pitch[TIDL_CHANNEL_PITCH];
  int32_t inBatchPitch  = (int32_t)inDataParams->pitch[TIDL_ROI_PITCH];
  int32_t outPitch   = (int32_t)outDataParams->pitch[TIDL_LINE_PITCH];
  int32_t outChPitch = (int32_t)outDataParams->pitch[TIDL_CHANNEL_PITCH];
  int32_t outBatchPitch = (int32_t)outDataParams->pitch[TIDL_ROI_PITCH];
  int32_t numTotBatches = inDataParams->dimValues[TIDL_DIM_BATCH];
  params->numChannels = inDataParams->dimValues[TIDL_DIM_NUMCH];
  /*workload already adds offset so not to add here
  */
  Tin  *inData = (Tin *)inPtr;
  Tout *outData = (Tout *)outPtr;
  float32_tidl alpha = (float)tidlLayer->layerParams.batchNormParams.inDataQ/((float)TIDL_NON_LIN_PARAM_SCALE);
  float32_tidl beta = (float)tidlLayer->layerParams.batchNormParams.weightsQ/((float)TIDL_NON_LIN_PARAM_SCALE);

  int32_t c =  params->numChannels;

  //OPENACC(data copyin(inData[((numTotBatches-1)*inBatchPitch) + ((c-1)*inChPitch) + ((imHeight-1)*inPitch) + (imWidth-1)]) \
               copy(outData[((numTotBatches-1)*outBatchPitch) + ((c-1)*outChPitch) + ((imHeight-1)*outPitch) + (imWidth-1)]))

  //OPENACC(parallel loop collapse(4))
  for (i3 = 0; i3 < numTotBatches; i3++)
  {
    for (i2 = 0; i2 < c; i2++)
    {
      for (i0=0; i0<imHeight; i0++)
      {
        for (i1=0; i1<imWidth; i1++)
        {
          Tin  inDataVal;
          if(tidlLayer->actParams.actType == TIDL_Tanh)
          {
            inDataVal = (inData[(i3*inBatchPitch) + (i2*inChPitch) + (i0*inPitch) + i1]);
            outData[(i3*outBatchPitch) + (i2*outChPitch) + (i0*outPitch) + i1] = (Tout)std::tanh(inDataVal);
          }
          else if(tidlLayer->actParams.actType == TIDL_ELU)
          {
            inDataVal = (inData[(i3*inBatchPitch) + (i2*inChPitch) + (i0*inPitch) + i1]);
            outData[(i3*outBatchPitch) + (i2*outChPitch) + (i0*outPitch) + i1] = (Tout)((inDataVal < 0.0) ? (alpha*(std::exp(inDataVal) - 1)) : inDataVal);
          }
          else if(tidlLayer->actParams.actType == TIDL_HardSigmoid)
          {
            inDataVal = (inData[(i3*inBatchPitch) + (i2*inChPitch) + (i0*inPitch) + i1]);
            float32_tidl outValF = alpha*inDataVal + beta;
            float32_tidl omin = 0;
            float32_tidl omax = 1;
            outValF = (outValF > omax) ? omax : outValF;
            outValF = (outValF < omin) ? omin : outValF;
            outData[(i3*outBatchPitch) + (i2*outChPitch) + (i0*outPitch) + i1] = (Tout)outValF;
          }
          else if (tidlLayer->actParams.actType == TIDL_GELU)
          {
            inDataVal = inData[(i3*inBatchPitch) + (i2*inChPitch) + (i0*inPitch) + i1];
            // tanh approx.
            float32_tidl outValF = std::sqrt((2.0/M_PI)) * (inDataVal + (0.044715 * inDataVal * inDataVal * inDataVal));   // z = sqrt(2/PI)*(x+0.044715x^3)
            outValF = inDataVal * (1  + std::tanh(outValF));   // 0.5x(1+tanh(z))


            outData[(i3*inBatchPitch) + (i2*inChPitch) + (i0*inPitch) + i1] = (Tout)outValF;
          }

        }
      }
    }
  }
  return status;
}

/**
 * @brief This function is tanh implementation in fully float manner
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
static int32_t TIDL_fullyFloatReferenceNonLinear(TIDL_Handle intAlgHandle,
                                      sTIDL_AlgLayer_t *algLayer,
                                      const sTIDL_Layer_t    *tidlLayer,
                                      sTIDL_BatchNormParams_t *params,
                                      void *inPtr,
                                      void *outPtr,
                                      sTIDL_DataParams_t *inDataParams,
                                      sTIDL_DataParams_t *outDataParams)
{
  int32_t status = TIDL_SUCCESS;
  int32_t layerIdx = algLayer->layerIdx;


  status = TIDL_refFloatNonLinear((float32_tidl*)inPtr,
                              (float32_tidl*)outPtr,
                                layerIdx,
                                params,
                                inDataParams,
                                outDataParams,
                                tidlLayer
                                );


  return status;
}

 /**
 * @brief BatchNorm layer reference implementation
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
static int32_t TIDL_refBatchNormProcess(TIDL_Handle intAlgHandle,
                                        sTIDL_AlgLayer_t *algLayer,
                                        const sTIDL_Layer_t    *tidlLayer,
                                        sTIDL_BatchNormParams_t *params,
                                        void *inPtr,
                                        void *outPtr,
                                        const sTIDL_DataParams_t *inDataParams,
                                        const sTIDL_DataParams_t *outDataParams)
{
  int32_t status = TIDL_SUCCESS;
  int32_t layerIdx = algLayer->layerIdx;
  void * weightPtr = ((int8_t *)(intAlgHandle->createParams->net) + params->weights);
  sTIDL_ALgBatchNormParams_t * algBatchNormParams = &algLayer->layerParams.batchNormParams;
  int16_t * orgBiasPtr = (int16_t *)get_int8_t_pointer((int8_t *)(intAlgHandle->createParams->net), params->bias);
  void * biasPtr   = ((int8_t *)algBatchNormParams->biasParamMem);
  void * slopePtr  = ((int8_t *)(intAlgHandle->createParams->net) + tidlLayer->actParams.slope);
  int32_t i0;
  int32_t numChannels = intAlgHandle->createParams->net->TIDLLayers[layerIdx].outData.dimValues[TIDL_DIM_NUMCH];
  float inScaleFactor = (params->weightScale * inDataParams->tensorScale) / params->biasScale;
  void * accPtr = NULL;

  if(tidlLayer->weightsElementSizeInBits == 32)
  {
    biasPtr = ((int8_t *)(intAlgHandle->createParams->net) + params->bias);

    status = TIDL_refBatchNormBitDepth(inPtr,
                                       outPtr,
                                       (float32_tidl *)weightPtr,
                                       (float32_tidl *)slopePtr,
                                       (float32_tidl *)biasPtr,
                                       (float32_tidl *)accPtr,
                                       params,
                                       intAlgHandle,
                                       layerIdx,
                                       algLayer,
                                       inDataParams,
                                       outDataParams);
  }
  else if(tidlLayer->weightsElementSizeInBits <= 8)
  {

    if (params->biasScale != -1)
    {
      TIDL_conv2dBiasSplit((int16_t*)orgBiasPtr, (int16_t*)biasPtr, &algBatchNormParams->biasB, numChannels,
      inScaleFactor, TIDL_SAT_LO_INT16, TIDL_SAT_HI_INT16,0xFF, TIDL_getDatElementSign(inDataParams->elementType));
    }

    for (i0 = 0; i0 < numChannels; i0++)
    {
      ((int16_t*)biasPtr)[i0] = ((int16_t*)biasPtr)[i0] * algBatchNormParams->biasB;
    }

    status = TIDL_refBatchNormBitDepth(inPtr,
                                       outPtr,
                                       (int8_t *)weightPtr,
                                       (int8_t *)slopePtr,
                                       (int16_t *)biasPtr,
                                       (int32_t *)accPtr,
                                       params,
                                       intAlgHandle,
                                       layerIdx,
                                       algLayer,
                                       inDataParams,
                                       outDataParams);
  }
  else
  {
    if (params->biasScale != -1)
    {
      int32_t maxBiasB = 0xFFFF;
      /* This condition indicates mixed precision where processing is in 16 bit but input is signed input.
      Here as target implementation scales the input with 256, we need to limit the biasB so that it doesn't
      go beyond 8 bits as the same is later multiplied by 256 to match the scale of accumulator*/
      if ( inDataParams->elementType == TIDL_SignedChar )
      {
        maxBiasB = 0xFF;
      }
      TIDL_conv2dBiasSplit((int16_t*)orgBiasPtr, (int32_t*)biasPtr, &algBatchNormParams->biasB, numChannels,
      inScaleFactor, TIDL_SAT_LO_INT32, TIDL_SAT_HI_INT32, maxBiasB, TIDL_getDatElementSign(inDataParams->elementType));
    }
    for (i0 = 0; i0 < numChannels; i0++)
    {
      ((int32_t*)biasPtr)[i0] = ((((int32_t*)biasPtr)[i0])) * algBatchNormParams->biasB;
    }

    status = TIDL_refBatchNormBitDepth(inPtr,
                                       outPtr,
                                       (int16_t *)weightPtr,
                                       (int16_t *)slopePtr,
                                       (int32_t *)biasPtr,
                                       (int64_t *)accPtr,
                                       params,
                                       intAlgHandle,
                                       layerIdx,
                                       algLayer,
                                       inDataParams,
                                       outDataParams);
  }
  return status;
}

static int32_t TIDL_floatReferenceKernelSupportsActivation( const sTIDL_Layer_t * tidlLayer){

  int32_t doesSupport = 0;
  if(tidlLayer->actParams.actType == TIDL_Tanh || tidlLayer->actParams.actType == TIDL_HardSigmoid || tidlLayer->actParams.actType == TIDL_ELU || tidlLayer->actParams.actType == TIDL_GELU)
  {
    doesSupport = 1;
  }
  return doesSupport;

};

static int32_t TIDL_batchNormRefProcess(TIDL_NetworkCommonParams *commonParams,
                              sTIDL_AlgLayer_t     * algLayer,
                              sTIDL_Layer_t        * tidlLayer,
                              void                 * inPtrs[],
                              void                 * outPtrs[],
                              int32_t                layerIdx)
{
  int32_t status = IALG_EOK;

  sTIDL_BatchNormParams_t   *params = &tidlLayer->layerParams.batchNormParams;
  sTIDL_DataParams_t        *inDataParams;
  inDataParams   = &commonParams->createParams->net->TIDLLayers[algLayer->inLayerIdx[0]].outData;

  TIDL_Obj intAlgObj;
  TIDL_CreateParams createParams;

  memcpy(&createParams, commonParams->createParams, sizeof(TIDL_CreateParams));
  intAlgObj.createParams = (TIDL_CreateParams *) &createParams;

  if(TIDL_LUTSupportsDatatype(inDataParams->elementType, tidlLayer->outData.elementType) &&
    TIDL_DeviceSupportsLUT(commonParams->createParams->net->deviceName) &&
    TIDL_LUTSupportsActivation(tidlLayer))
  {
    uint8_t LUT_data_table[TABLE_SIZE_8BIT];
    status = TIDL_refNonLinearLUTProcess(&intAlgObj,
                                    algLayer,
                                    tidlLayer,
                                    params,
                                    inPtrs[0],
                                    outPtrs[0],
                                    inDataParams,
                                    &tidlLayer->outData,
                                    LUT_data_table);
  }
  else if(TIDL_NonLUTSupportsDatatype(inDataParams->elementType, tidlLayer->outData.elementType) &&
              TIDL_DeviceSupportsNonLUT(commonParams->createParams->net->deviceName) &&
              TIDL_NonLUTSupportsActivation(tidlLayer))
  {
    status = TIDL_refNonLinearNonLUTProcessWrapper(&intAlgObj,
                                    algLayer,
                                    tidlLayer,
                                    params,
                                    inPtrs[0],
                                    outPtrs[0],
                                    inDataParams,
                                    &tidlLayer->outData);
  }
  else if(TIDL_floatReferenceKernelSupportsActivation(tidlLayer) && (inDataParams->elementType == TIDL_SinglePrecFloat))
  {
    status = TIDL_fullyFloatReferenceNonLinear(&intAlgObj,
                                        algLayer,
                                        tidlLayer,
                                        params,
                                        inPtrs[0],
                                        outPtrs[0],
                                        inDataParams,
                                        &tidlLayer->outData);
  }
  else
  {
    if (tidlLayer->actParams.actType == TIDL_Sigmoid)
    {
      status = TIDL_refSigmoidProcess(&intAlgObj,
                                    algLayer,
                                    tidlLayer,
                                    params,
                                    inPtrs[0],
                                    outPtrs[0],
                                    inDataParams,
                                    &tidlLayer->outData);
    }
    else
    {
      status = TIDL_refBatchNormProcess(&intAlgObj,
                                      algLayer,
                                      tidlLayer,
                                      params,
                                      inPtrs[0],
                                      outPtrs[0],
                                      inDataParams,
                                      &tidlLayer->outData);
    }
  }

  return status;
}

/* Return memory to be allocated per buffer in the persistent memory */
static void TIDL_getPersistentMemorySize(sTIDL_Network_t *net,
                                        const void * workloadUnit,
                                        int32_t layerIdx,
                                        int32_t *biasParamSize,
                                        int32_t *slopeFactSize,
                                        int32_t *weightParamSize)
{
  int32_t numChannels, weightSizeInBytes;
  sTIDL_Layer_t *tidlLayer = &net->TIDLLayers[layerIdx];
  TIDL_GetTensorDimensions(net, workloadUnit, layerIdx,
                          NULL, NULL,
                          NULL, NULL,
                          NULL, &numChannels);
  
  numChannels  = numChannels * tidlLayer->outData.dimValues[TIDL_DIM_DIM1] * tidlLayer->outData.dimValues[TIDL_DIM_DIM2];

  *biasParamSize = numChannels *  ((int32_t)(sizeof(int32_t)));
  *slopeFactSize = numChannels *  ((int32_t)(sizeof(int32_t)));
   weightSizeInBytes = ((net->TIDLLayers[layerIdx].weightsElementSizeInBits + 7) / 8);
  *weightParamSize = numChannels * weightSizeInBytes ;
  return ;
}

static int32_t TIDL_batchNormInitLUTParams(sTIDL_Network_t *net,
                                         int32_t layerIdx,
                                         sTIDL_AlgLayer_t *algLayer,
                                         sTIDL_sysMemHandle_t sysMems[TIDL_SYSMEM_MAX])
{
  int32_t status = IALG_EOK;

  sTIDL_Layer_t *tidlLayer  = &net->TIDLLayers[layerIdx];
  sTIDL_DataParams_t * inDataParams = TIDL_getDataParams(net, tidlLayer->inData[0]);
  sTIDL_DataParams_t * outDataParams = &tidlLayer->outData;

  algLayer->layerParams.batchNormParams.isLUTenabled = 0;
  if(TIDL_LUTSupportsDatatype(inDataParams->elementType, outDataParams->elementType) && TIDL_DeviceSupportsLUT(net->deviceName) && TIDL_LUTSupportsActivation(tidlLayer))
  {
    void *memPtr = NULL;
    algLayer->layerParams.batchNormParams.isLUTenabled = 1;
    /* LUT implementation requires that the L1D memory be reserved fully for the kernel which it will use for the L1D during process call.
      By allocating all the L1D scratch memory upfront, we prevent any following part of the init code from using the L1D mem as scratch
      space. */
    memPtr = (void*) TIDL_getMemoryChunkFromSysmem(sysMems,
                                              sysMems[TIDL_SYSMEM_L1_SCRATCH].size,
                                              TIDL_ALIGNMENT_SIZE,
                                              (uint32_t)TIDL_SYSMEM_L1_SCRATCH,
                                              (uint32_t)IALG_SCRATCH);

    if (memPtr == NULL)
    {
      tidl_printf(0, "LUT implementation of batchNorm expects L1 mem to be reserved entirely for the LUT. Please make sure L1 memory is not used for this layer \n");
      status = TIDL_ERR_FAILURE;
    }
  }

  return status;
}

static void TIDL_batchNormInitBiasParamMem(sTIDL_Network_t *net,
                                         int32_t layerIdx,
                                         sTIDL_AlgLayer_t *algLayer,
                                         int32_t biasParamSize,
                                         int32_t *paramMemTabOffset,
                                         uint8_t *memory[TIDL_LAYER_MEMORY_MAX])
{
  sTIDL_Layer_t *tidlLayer  = &net->TIDLLayers[layerIdx];
  sTIDL_DataParams_t * inDataParams = TIDL_getDataParams(net, tidlLayer->inData[0]);
  sTIDL_BatchNormParams_t * batchNormParams = &tidlLayer->layerParams.batchNormParams;
  sTIDL_ALgBatchNormParams_t * algBatchNormParams = &algLayer->layerParams.batchNormParams;
  int32_t flattenedHigherDims = (tidlLayer->outData.dimValues[TIDL_DIM_DIM1] * tidlLayer->outData.dimValues[TIDL_DIM_DIM2]);
  int32_t biasElemSize = 2;
  int32_t biasParamSizePerDim;
  int32_t numChannels;

  TIDL_GetTensorDimensions(net, algLayer->workloadUnit, layerIdx,
                        NULL, NULL,
                        NULL, NULL,
                        NULL, &numChannels);

  algBatchNormParams->biasParamSize = biasParamSize;

  TIDL_AllocatePtr((intptr_t)memory[TIDL_LAYER_MEMORY_PERSISTENT],
                                    paramMemTabOffset,
                                    algBatchNormParams->biasParamSize,
                                    TIDL_ALIGNMENT_SIZE,
                                    &algBatchNormParams->biasParamMem );

  int16_t * orgBiasPtr = (int16_t *)get_int8_t_pointer((int8_t *)(net), batchNormParams->bias);
  int8_t * biasPtr  = (int8_t *)algBatchNormParams->biasParamMem;

  float32_tidl inScaleFactor;
  float32_tidl inTensorScale = TIDL_getTensorScale(net, algLayer->inLayerIdx[0]);

  inScaleFactor = (batchNormParams->weightScale * inTensorScale) / batchNormParams->biasScale;

  if(tidlLayer->weightsElementSizeInBits <= 8)
  {
    TIDL_conv2dBiasSplit((int16_t*)orgBiasPtr, (int16_t*)biasPtr, &algBatchNormParams->biasB, numChannels, inScaleFactor, TIDL_SAT_LO_INT16, TIDL_SAT_HI_INT16,0xFF, TIDL_getDatElementSign(inDataParams->elementType));
  }
  else
  {
    int32_t maxBiasB = 0xFFFF;
    biasElemSize = 4;
    /* This condition indicates mixed precision where processing is in 16 bit but input is signed input.
    Here as target implementation scales the input with 256, we need to limit the biasB so that it doesn't
    go beyond 8 bits as the same is later multiplied by 256 to match the scale of accumulator*/
    // TODO: Need to find a way to do this without using inDataParams. Need to access buffer info of proclink somehow.
    if ( inDataParams->elementType == TIDL_SignedChar )
    {
      maxBiasB = 0xFF;
    }
    TIDL_conv2dBiasSplit((int16_t*)orgBiasPtr, (int32_t*)biasPtr, &algBatchNormParams->biasB, numChannels, inScaleFactor, TIDL_SAT_LO_INT32, TIDL_SAT_HI_INT32, maxBiasB, TIDL_getDatElementSign(inDataParams->elementType));
  }

  /* Values for channels are same across higher dimensions and hence can be broadcasted. Starting from first index to prevent unnecessary copy in 4D cases*/
  biasParamSizePerDim = numChannels * biasElemSize;
  for (int32_t i = 1; i < flattenedHigherDims; i++)
  {
    memcpy(((int8_t *)biasPtr) + i * biasParamSizePerDim, 
            biasPtr, 
            biasParamSizePerDim);
  }
  return;
}

static void TIDL_batchNormInitSlopeFactMem(sTIDL_Network_t *net,
                                         int32_t layerIdx,
                                         sTIDL_AlgLayer_t *algLayer,
                                         int32_t slopeFactSize,
                                         int32_t *paramMemTabOffset,
                                         uint8_t *memory[TIDL_LAYER_MEMORY_MAX]) /*basePtr = params->net*/
{
  sTIDL_Layer_t *tidlLayer  = &net->TIDLLayers[layerIdx];
  sTIDL_ALgBatchNormParams_t * algBatchNormParams = &algLayer->layerParams.batchNormParams;
  int32_t flattenedHigherDims = (tidlLayer->outData.dimValues[TIDL_DIM_DIM1] * tidlLayer->outData.dimValues[TIDL_DIM_DIM2]);
  int32_t slopeFactSizePerDim = slopeFactSize / flattenedHigherDims;
  int32_t numChannels;

  TIDL_GetTensorDimensions(net, algLayer->workloadUnit, layerIdx,
                          NULL, NULL,
                          NULL, NULL,
                          NULL, &numChannels);

  algBatchNormParams->slopeFactSize = slopeFactSize;

  TIDL_AllocatePtr((intptr_t)memory[TIDL_LAYER_MEMORY_PERSISTENT],
                                    paramMemTabOffset,
                                    algBatchNormParams->slopeFactSize,
                                    TIDL_ALIGNMENT_SIZE,
                                    &algBatchNormParams->slopeFactMem);

  algBatchNormParams->slopeFact = (int32_t *)algBatchNormParams->slopeFactMem;
  int32_t preluScale = 1, i;
  void * slopePtr  = get_int8_t_pointer((int8_t *)(net),tidlLayer->actParams.slope);

  for (i = 0; i < numChannels; i++)
  {
    if(tidlLayer->actParams.actType == TIDL_PRelU)
    {
      if(tidlLayer->weightsElementSizeInBits <= 8)
      {
        preluScale = ((int8_t*)slopePtr)[i];
        algBatchNormParams->slopeFact[i] = (int32_t)((preluScale*(1 << 8)) / tidlLayer->actParams.slopeScale);
      }
      else
      {
        preluScale = ((int16_t*)slopePtr)[i];
        algBatchNormParams->slopeFact[i] = (int32_t)(((preluScale*(1 << 8)) / tidlLayer->actParams.slopeScale)*256);
      }
    }
    else
    {
      algBatchNormParams->slopeFact[i] = 1;
    }
  }

  /* Values for channels are same across higher dimensions and hence can be broadcasted. Starting from first index to prevent unnecessary copy in 4D cases*/
  for (int32_t i = 1; i < flattenedHigherDims; i++)
  {
    memcpy(((int8_t *) algBatchNormParams->slopeFact) + i * slopeFactSizePerDim, 
            algBatchNormParams->slopeFact, 
            slopeFactSizePerDim);
  }
  return;
}


static void TIDL_batchNormInitWeightParamMem(sTIDL_Network_t *net,
                                         int32_t layerIdx,
                                         sTIDL_AlgLayer_t *algLayer,
                                         int32_t weightParamSize,
                                         int32_t *paramMemTabOffset,
                                         uint8_t *memory[TIDL_LAYER_MEMORY_MAX]) /*basePtr = params->net*/
{
  sTIDL_Layer_t *tidlLayer  = &net->TIDLLayers[layerIdx];
  sTIDL_ALgBatchNormParams_t * algBatchNormParams = &algLayer->layerParams.batchNormParams;

  algBatchNormParams->weightParamSize = weightParamSize;

  TIDL_AllocatePtr((intptr_t)memory[TIDL_LAYER_MEMORY_PERSISTENT],
                                    paramMemTabOffset,
                                    algBatchNormParams->weightParamSize,
                                    TIDL_ALIGNMENT_SIZE,
                                    &algBatchNormParams->weightParamMem);
  if(weightParamSize)
  {
    int32_t flattenedHigherDims = (tidlLayer->outData.dimValues[TIDL_DIM_DIM1] * tidlLayer->outData.dimValues[TIDL_DIM_DIM2]);
    int32_t weightParamSizePerDim = algBatchNormParams->weightParamSize / flattenedHigherDims;
    void * weightPtr  = get_int8_t_pointer((int8_t *)(net),tidlLayer->layerParams.batchNormParams.weights);

    /* Values for channels are same across higher dimensions and hence can be broadcasted. Starting from first index to prevent unnecessary copy in 4D cases*/    
    for (int32_t i = 0; i < flattenedHigherDims; i++)
    {
      memcpy(((int8_t *) algBatchNormParams->weightParamMem) + i * weightParamSizePerDim, 
              weightPtr, 
              weightParamSizePerDim);
    }
  }
  return;
}
/**
*******************************************************************************
*  \brief      Function to supply memory requirement for batchNorm layer to user
*
*
*  \param [in]  specificParams :  Pointer to specific parameter required for all the layers
*
*  \param [in]  commonParams :  Pointer to common parameter required for all the layers
*
*  \param [in]  layerIdx           : Layer Id of the layer
*
*  \param [out]  memorySize           : Output memory requirement of the layer
*
*  \return     Error:  IALG_EOK ( Successful) else failure
*
*  \remarks
*******************************************************************************
*/
int32_t TIDL_batchNormAllocNew(const TIDL_LayerSpecificParams *layerSpecificParams,
                                   const TIDL_NetworkCommonParams *commonParams,
                                   int32_t layerIdx,
                                   int32_t memorySize[TIDL_LAYER_MEMORY_MAX])

{
  int32_t status = IALG_EOK;
  int32_t biasParamSize;
  int32_t slopeFactSize;
  int32_t weightParamSize;

  TIDL_getPersistentMemorySize(commonParams->net,
                            layerSpecificParams->workloadUnit,
                            layerIdx,
                            &biasParamSize,
                            &slopeFactSize,
                            &weightParamSize);

  if ((commonParams->createParams->flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) == 0)
  {
    status = TIDL_deviceUtilsCommonAlloc(layerSpecificParams, commonParams, layerIdx, memorySize);
  }
  else
  {
    status = TIDL_deviceCommonRefAlloc(layerSpecificParams, commonParams, layerIdx, memorySize);
  }

  memorySize[TIDL_LAYER_MEMORY_PERSISTENT] += biasParamSize + TIDL_ALIGNMENT_SIZE;
  memorySize[TIDL_LAYER_MEMORY_PERSISTENT] += slopeFactSize + TIDL_ALIGNMENT_SIZE;
  memorySize[TIDL_LAYER_MEMORY_PERSISTENT] += weightParamSize + TIDL_ALIGNMENT_SIZE;

  return status;
}



/**
*******************************************************************************
*  \brief      Function to set memory needed by batchNorm layer
*
*
*  \param [in]  specificParams :  Pointer to specific parameter required for all the layers
*
*  \param [in]  commonParams :  Pointer to common parameter required for all the layers
*
*  \param [in]  algLayer :  Pointer to layer specific internal parameters
*
*  \param [in]  layerIdx           : Layer Id of the layer
*
*  \param [in]  memory           : Allocated memory pointers
*
*  \param [out]  memorySize           : Output memory requirement of the layer
*
*  \param [out]  outPtr           : Output memory pointer. Applicable only for reference flow
*
*  \return     Error:  IALG_EOK ( Successful) else failure
*
*  \remarks
*******************************************************************************
*/
int32_t TIDL_batchNormInitNew(const TIDL_LayerSpecificParams *layerSpecificParams,
                                const TIDL_NetworkCommonParams  *commonParams,
                                sTIDL_AlgLayer_t               *algLayer,
                                int32_t layerIdx,
                                uint8_t *memory[TIDL_LAYER_MEMORY_MAX],
                                int32_t memorySize[TIDL_LAYER_MEMORY_MAX],
                                void    **outPtr)

{
  int32_t status = IALG_EOK;
  int32_t currOffset = 0;
  int32_t biasParamSize;
  int32_t slopeFactSize;
  int32_t weightParamSize;

  status = TIDL_batchNormInitLUTParams(commonParams->net,
                                      layerIdx,
                                      algLayer,
                                      commonParams->tidlCommonParams->sysMems);

  if (status != IALG_EOK)
  {
    goto Exit;
  }

  TIDL_getPersistentMemorySize(commonParams->net,
                              layerSpecificParams->workloadUnit,
                              layerIdx,
                              &biasParamSize,
                              &slopeFactSize,
                              &weightParamSize);

  /* Initialize Bias Param Mem */
  TIDL_batchNormInitBiasParamMem(commonParams->net,
                                layerIdx,
                                algLayer,
                                biasParamSize,
                                &currOffset,
                                memory);

  /* Initialize slope Factor Mem */
  TIDL_batchNormInitSlopeFactMem(commonParams->net,
                                layerIdx,
                                algLayer,
                                slopeFactSize,
                                &currOffset,
                                memory);
  /* Initialize weight param Mem */
  TIDL_batchNormInitWeightParamMem(commonParams->net,
                                layerIdx,
                                algLayer,
                                weightParamSize,
                                &currOffset,
                                memory);

  /* APIs calculate offset with regard to base ptr. In this situation, the device utils API would not
   * know that the memory has already been utilized. Thus we add an offset equal to the size of memory
   * space already allocated and remove the offset after the API has run so the caller function sees the
   * same memory.
   **/
  memory[TIDL_LAYER_MEMORY_PERSISTENT] += TIDL_ALIGN_CEIL(currOffset, TIDL_ALIGNMENT_SIZE);

  if ((commonParams->createParams->flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) == 0)
  {
    status = TIDL_deviceUtilsCommonInit(layerSpecificParams,
                                      commonParams,
                                      algLayer,
                                      layerIdx,
                                      memory,
                                      memorySize,
                                      outPtr);
  }
  else
  {
    status = TIDL_deviceCommonRefInit(layerSpecificParams,
                                 commonParams,
                                 algLayer,
                                 layerIdx,
                                 memory,
                                 memorySize,
                                 outPtr);
  }

  memory[TIDL_LAYER_MEMORY_PERSISTENT] -= TIDL_ALIGN_CEIL(currOffset, TIDL_ALIGNMENT_SIZE);
  memorySize[TIDL_LAYER_MEMORY_PERSISTENT] += TIDL_ALIGN_CEIL(currOffset, TIDL_ALIGNMENT_SIZE);

Exit:
  return status;
}



int32_t TIDL_batchNormProcessNew(TIDL_NetworkCommonParams *commonParams,
                              sTIDL_AlgLayer_t     * algLayer,
                              sTIDL_Layer_t        * tidlLayer,
                              void                 * inPtrs[],
                              void                 * outPtrs[],
                              int32_t                layerIdx)

{
  int32_t status = IALG_EOK;

  /* Setup various pointers needed by convolution layers */
  if ((commonParams->createParams->flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) == 0)
  {
    status = TIDL_deviceUtilsCommonProcess(commonParams,
                                   algLayer,
                                   tidlLayer,
                                   inPtrs,
                                   outPtrs,
                                   layerIdx);

  }
  else
  {
    status = TIDL_batchNormRefProcess(commonParams,
                                   algLayer,
                                   tidlLayer,
                                   inPtrs,
                                   outPtrs,
                                   layerIdx);
  }

  return status;
}
