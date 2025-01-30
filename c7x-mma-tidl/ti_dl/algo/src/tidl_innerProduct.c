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
 @file    tidl_innerProduct.c
 @brief   This file contains TILD InnerProduct Layer implementation
 @version 0.1 (Jan 2017) : initial version [EPR]
 ----------------------------------------------------------------------------
*/

#include "tidl_innerProduct.h"
#include "tidl_innerProduct_int.h"
#include <math.h>
#include <float.h>
#include "tidl_alg_utils.h"
#include "tidl_device_functions.h"
#include "tidl_commonUtils.h"
#include "tidl_innerProduct_mma.h"
#include "tidl_matMul_mma.h"

#define USE_16BIT_BIAS_FOR_8BIT_MODE (1)

/**
 * @brief  This funciton is the core kernel of reference innerProduct
 *
 * @param intAlgHandle : tidl algorothm handle
 * @param layerIdx :index of the current layer
 * @param accPtr : pointer to the accumulator buffer
 * @param inPtr : Input pointer on which innerProduct is applied
 * @param outPtr : Output pointer after innerProduct opreation
 * @param biasPtr  : Pointer to the Bias values
 * @param weightsPtr : Pointer to weights buffer
 * @param buffParams : parameters of the innerProduct buffer
 * @param satLow : Min value for the saturation
 * @param satHigh : Max value for the saturation
 */
template <class Tin,class TBin, class Tw, class Tb, class Tout, class Tacc>
void TIDL_refInnerProduct(
  TIDL_Handle intAlgHandle,
  int32_t layerIdx,
  Tacc*   accPtr,
  Tin*    inPtr,
  TBin*   inBPtr,
  Tout*   outPtr,
  Tb*     biasPtr,
  Tw*     weightsPtr,
  tidlInnerProductBuffParams_t  * buffParams,
  int32_t  satLow,
  int32_t  satHigh
  )
{
  sTIDL_Network_t * net = intAlgHandle->createParams->net;
  int32_t quantizationStyle = intAlgHandle->createParams->net->quantizationStyle;
  int32_t mmaScale = 1U;
  sTIDL_Layer_t  * tidlLayer = &net->TIDLLayers[layerIdx];

  sTIDL_DataParams_t *inADataParams   = TIDL_getDataParams(net, tidlLayer->inData[0]);
  sTIDL_DataParams_t *inBDataParams   = TIDL_getDataParams(net, tidlLayer->inData[1]);
  sTIDL_DataParams_t *inOutDataParams = TIDL_getDataParams(net, tidlLayer->outData.dataId);

  Tin      *inPtrBatch;
  Tin      *inPtrDIM1;
  Tin      *inPtrDIM2;
  Tin      *pInAData;
  Tb       *pBiasData;
  int32_t  roundBits;
  sTIDL_InnerProductParams_t IPparams = tidlLayer->layerParams.innerProductParams;

  int32_t inputATranspose = IPparams.inputATranspose;
  int32_t inputBTranspose = IPparams.inputBTranspose;
  Tacc  sum;

  uint32_t  numBatches      =  IPparams.numBatches;
  uint32_t  numInCols       =  IPparams.numInCols;
  uint32_t  numOutCols      =  IPparams.numOutCols;
  uint32_t  numInRows       =  IPparams.numInRows;
  uint32_t  numOutRows      =  numInRows;

  uint32_t outChPitch      = (uint32_t)inOutDataParams->pitch[TIDL_CHANNEL_PITCH];
  uint32_t outLinePitch    = (uint32_t)inOutDataParams->pitch[TIDL_LINE_PITCH];
  uint32_t outDIM1Pitch    = (uint32_t)inOutDataParams->pitch[TIDL_DIM1_PITCH];
  uint32_t outDIM2Pitch    = (uint32_t)inOutDataParams->pitch[TIDL_DIM2_PITCH];
  uint32_t outBatchPitch    = (uint32_t)inOutDataParams->pitch[TIDL_ROI_PITCH];

  uint32_t  inAChPitch     = (uint32_t)inADataParams->pitch[TIDL_CHANNEL_PITCH];
  uint32_t  inALinePitch   = (uint32_t)inADataParams->pitch[TIDL_LINE_PITCH];
  uint32_t  inADIM1Pitch   = (uint32_t)inADataParams->pitch[TIDL_DIM1_PITCH];
  uint32_t  inADIM2Pitch   = (uint32_t)inADataParams->pitch[TIDL_DIM2_PITCH];
  uint32_t  inABatchPitch   = (uint32_t)inADataParams->pitch[TIDL_ROI_PITCH];

  uint32_t  inBChPitch;
  uint32_t  inBLinePitch;
  uint32_t  inBDIM1Pitch;
  uint32_t  inBDIM2Pitch;

  if(inBPtr == NULL)
  {
    if(inputBTranspose)
    {
      inBLinePitch  = numInCols;
    }
    else
    {
      inBLinePitch  = numOutCols;
    }
    inBChPitch    = numInCols*numOutCols;
  }
  else
  {
    inBLinePitch  = (uint32_t)inBDataParams->pitch[TIDL_LINE_PITCH];
    inBChPitch    = (uint32_t)inBDataParams->pitch[TIDL_CHANNEL_PITCH];
    inBDIM1Pitch  = (uint32_t)inBDataParams->pitch[TIDL_DIM1_PITCH];
    inBDIM2Pitch  = (uint32_t)inBDataParams->pitch[TIDL_DIM2_PITCH];
  }

  // handle B input's pitch in absence of broadcast
  // only channel-wise possible (as need at least 2 dim for inner product)
  if (
      (inBPtr != NULL) &&
      (inBDataParams->dimValues[TIDL_DIM_NUMCH] != inADataParams->dimValues[TIDL_DIM_NUMCH]) && // unequal channel 
      (inBDataParams->dimValues[TIDL_DIM_NUMCH] == 1)                                           // channel-wise broadcast possible
    )
  {
    /*
     * Set channel pitch as 0 so that after going through a channel in A 
     * it comes back to same ptr in B for next channel in A 
     * Hence ptr B is repeated every time a new channel in A is taken
    */
    inBChPitch = 0;
  }

  Tacc      min, max;
  int32_t   accMemSize = numBatches * numOutRows * outLinePitch * sizeof(Tacc);
  int64_t   temp;

  pBiasData = (Tb *)biasPtr;

  if (buffParams->scratchSize >= accMemSize)
  {
    accPtr = (Tacc *)buffParams->scratchMem;
  }
  else
  {
    tidl_printf(0,"Memory for  TIDL_refInnerProduct accumulator is not sufficient exiting... %d %d \n", buffParams->scratchSize, accMemSize);
    exit(-1);
  }
  min = (Tacc)((((uint64_t)1)<< ((sizeof(Tacc)*8)-1)) - 1);
  temp =(int64_t) (((uint64_t)1) << ((sizeof(Tacc) * 8) - 1));

  max = (Tacc)temp*-1;

  /*OPENACC(data create(accPtr[: 1 + numBatches * numOutRows * outLinePitch]) \
               copyin(inPtr[:(1+ inABatchPitch*(inADataParams->dimValues[TIDL_DIM_BATCH]-1) + ((inADataParams->dimValues[TIDL_DIM_NUMCH]-1) * inAChPitch) + (numOutRows-1) * inALinePitch) + (numInCols-1)], \
                      inBPtr[:((inADataParams->dimValues[TIDL_DIM_NUMCH]-1) * inBChPitch) + (((numOutCols-1) * inBLinePitch) + (numInCols-1))], \
                      weightsPtr[:1 + ((inADataParams->dimValues[TIDL_DIM_NUMCH]-1) * inBChPitch) + (((numOutCols-1) * inBLinePitch) + (numInCols-1))], \
                      pBiasData[: 1+ ((numOutRows-1)*numOutCols + (numOutCols-1))]) \
               copyout(outPtr[:1 + ((inADataParams->dimValues[TIDL_DIM_BATCH]-1)*outBatchPitch) + ((inADataParams->dimValues[TIDL_DIM_NUMCH]-1) * outChPitch) + (numInRows * numOutCols -1)]))
  */
  {
  //OPENACC(parallel loop collapse(7))
  for (int batchIdx = 0; batchIdx < inADataParams->dimValues[TIDL_DIM_BATCH]; batchIdx++)
  {
    inPtrBatch = (Tin *)inPtr + inABatchPitch*batchIdx;
    for (int dim1Idx = 0; dim1Idx < inADataParams->dimValues[TIDL_DIM_DIM1]; dim1Idx++)
    {
      inPtrDIM1 = (Tin *)inPtrBatch + inADIM1Pitch*dim1Idx;
      for (int dim2Idx = 0; dim2Idx < inADataParams->dimValues[TIDL_DIM_DIM2]; dim2Idx++)
      {
        inPtrDIM2 = (Tin *)inPtrDIM1 + inADIM2Pitch*dim2Idx;
        for (int chIdx = 0; chIdx < inADataParams->dimValues[TIDL_DIM_NUMCH]; chIdx++)
        {
          pInAData = ((Tin *)inPtrDIM2 + (chIdx * inAChPitch));
          for (int outRwIdx = 0; outRwIdx < numOutRows; outRwIdx++)
          {
            for (int outColIdx = 0; outColIdx < numOutCols; outColIdx++)
            {
              if(IPparams.isBias && IPparams.constIdx == -1)
              {
                sum = pBiasData[outRwIdx*numOutCols + outColIdx];
              }
              else if(IPparams.isBias && IPparams.constIdx == 0)
              {
                sum = pBiasData[outRwIdx];
              }
              else if(IPparams.isBias && IPparams.constIdx == 1)
              {
                sum = pBiasData[outColIdx];
              }
              else
              {
                sum = 0;
              }
              for (int inColIdx = 0; inColIdx < numInCols; inColIdx++)
              {
                /* Multiply inputA nodes with input B nodes and accumulate */
                if(inBPtr != NULL)
                {
                  if(inputATranspose)/* transpose needed for inputA */
                    sum += pInAData[(inColIdx * inALinePitch) + outColIdx ] * inBPtr[(dim1Idx * inBDIM1Pitch) + (dim2Idx * inBDIM2Pitch) + (chIdx * inBChPitch) + ((inColIdx * inBLinePitch) + outColIdx)];/* Not tested*/
                  else if (inputBTranspose) /* transpose needed for inputB */
                    sum += pInAData[(inALinePitch * outRwIdx) + inColIdx] * inBPtr[(dim1Idx * inBDIM1Pitch) + (dim2Idx * inBDIM2Pitch) + (chIdx * inBChPitch) + (outColIdx * inBLinePitch) + inColIdx];
                  else/* no transpose needed */
                    sum += pInAData[ (inALinePitch * outRwIdx) + inColIdx] * inBPtr[(dim1Idx * inBDIM1Pitch) + (dim2Idx * inBDIM2Pitch) + (chIdx * inBChPitch) + ((inColIdx * inBLinePitch) + outColIdx)];
                }
                else
                {
                  if(inputATranspose)/* transpose needed for inputA */
                    sum += pInAData[(inColIdx * inALinePitch) + outColIdx ] * weightsPtr[(dim1Idx * inBDIM1Pitch) + (dim2Idx * inBDIM2Pitch) + (chIdx * inBChPitch) + ((inColIdx * inBLinePitch) + outColIdx)];/* Not tested*/
                  else if (inputBTranspose) /* transpose needed for inputB */
                    sum += pInAData[(inALinePitch * outRwIdx) + inColIdx] * weightsPtr[(dim1Idx * inBDIM1Pitch) + (dim2Idx * inBDIM2Pitch) + (chIdx * inBChPitch) + (outColIdx * inBLinePitch) + inColIdx];
                  else/* no transpose needed */
                    sum += pInAData[ (inALinePitch * outRwIdx) + inColIdx] * weightsPtr[(dim1Idx * inBDIM1Pitch) + (dim2Idx * inBDIM2Pitch) + (chIdx * inBChPitch) + ((inColIdx * inBLinePitch) + outColIdx)];
                }
              }
              min = sum < min ? sum : min;
              max = sum > max ? sum : max;
              accPtr[(batchIdx*outBatchPitch) + (dim1Idx*outDIM1Pitch) + (dim2Idx*outDIM2Pitch) + (chIdx * outChPitch) + (outRwIdx * outLinePitch) + outColIdx] = sum;
            }
          }
        }
      }
    }
  }

  if(!TIDL_isAsymQuantEnabledTFL(quantizationStyle) && !TIDL_isKernelHighPrecision(net->TIDLLayers[layerIdx].layerKernelType))
  {
    if((intAlgHandle->createParams->flowCtrl & TIDL_FLOW_CTRL_REF_STAT) == TIDL_FLOW_CTRL_REF_STAT)
    {
      if (TIDL_getDatElementSign(net->TIDLLayers[layerIdx].outData.elementType) == 1)
      {
        TIDL_UpdateScaleFactors(intAlgHandle, layerIdx, 1, min, max);
      }
      else
      {
        TIDL_UpdateScaleFactors(intAlgHandle, layerIdx, 1, 0, max);
      }
    }
    roundBits = net->TIDLLayers[layerIdx].outData.roundBits;
    mmaScale  = 1;
  }
  else if(TIDL_isAsymQuantEnabledTFL(quantizationStyle))
  {
    roundBits = buffParams->mmaShift;
    mmaScale  = buffParams->mmaScale;
  }


  int32_t isHighPrecision = TIDL_isKernelHighPrecision(net->TIDLLayers[layerIdx].layerKernelType) == 1;
  uint8_t* mmav2_Scales = (uint8_t *)get_int8_t_pointer((int8_t *)(net), net->TIDLLayers[layerIdx].layerParams.innerProductParams.derivedScales);
  uint8_t* mmav2_Shifts = (uint8_t *)get_int8_t_pointer((int8_t *)(net), net->TIDLLayers[layerIdx].layerParams.innerProductParams.derivedShifts);
  int64_t tempAcc;
  int32_t chIdx,outIdx;

  //OPENACC(data present(accPtr[0:((inADataParams->dimValues[TIDL_DIM_BATCH]-1)*outBatchPitch) + ((inADataParams->dimValues[TIDL_DIM_NUMCH]-1) * outChPitch) + (numInRows * numOutCols -1)]) \
               present(outPtr[0:((inADataParams->dimValues[TIDL_DIM_BATCH]-1)*outBatchPitch) + ((inADataParams->dimValues[TIDL_DIM_NUMCH]-1) * outChPitch) + (numInRows * numOutCols -1)]))

  //OPENACC(parallel loop collapse(3))
  for (int batchIdx = 0; batchIdx < inADataParams->dimValues[TIDL_DIM_BATCH]; batchIdx++)
    {
      for (int dim1Idx = 0; dim1Idx < inADataParams->dimValues[TIDL_DIM_DIM1]; dim1Idx++)
      {
        for (int dim2Idx = 0; dim2Idx < inADataParams->dimValues[TIDL_DIM_DIM2]; dim2Idx++)
        {
          for (chIdx = 0; chIdx < inADataParams->dimValues[TIDL_DIM_NUMCH]; chIdx++)
          {
            for (outIdx = 0; outIdx < (numInRows * numOutCols); outIdx++)
            {
              sum = accPtr[(batchIdx*outBatchPitch) + (dim1Idx*outDIM1Pitch) + (dim2Idx*outDIM2Pitch) + (chIdx * outChPitch) + outIdx];
              if (tidlLayer -> outData.elementType == TIDL_SinglePrecFloat)
              {
                //OPENACC(routine(TIDL_floatSat))
                sum = TIDL_floatSat(sum, &net->TIDLLayers[layerIdx]);
              }
              else if (isHighPrecision)
              {
                int32_t i6 = outIdx % numOutCols;
                tempAcc = (int64_t)sum * mmav2_Scales[i6];
                sum  = (Tacc)TIDL_roundSatMMA(tempAcc, mmav2_Shifts[i6], satLow, satHigh);
              }
              else
              {
                tempAcc = sum * mmaScale;
                sum = (Tacc)TIDL_roundSat((int64_t)tempAcc, roundBits, satLow, satHigh);
              }
              outPtr[(batchIdx*outBatchPitch) + (dim1Idx*outDIM1Pitch) + (dim2Idx*outDIM2Pitch) + (chIdx * outChPitch) + outIdx] = sum;
            }
          }
        }
      }
    }
  }
}
/**
 * @brief  This funciton handles differnt input bit-depths for reference innerProduct
 *
 * @param intAlgHandle : tidl algorothm handle
 * @param layerIdx :index of the current layer
 * @param inPtr : Input pointer on which innerProduct is applied
 * @param refPtr : Output pointer after innerProduct opreation
 * @param biasPtr  : Pointer to the Bias values
 * @param weightsPtr : Pointer to weights buffer
 * @param accPtr : pointer to the accumulator buffer
 * @param outElementType : elementType of the output
 * @param buffParams : parameters of the innerProduct buffer
 */
template <class Tw, class Tb, class Tacc>
void TIDL_refInnerProductParamBitDepth(
  TIDL_Handle intAlgHandle,
  int32_t layerIdx,
  void*    inPtr,
  void*    inBPtr,
  void*    refPtr,
  Tb*     biasPtr,
  Tw*     weightsPtr,
  Tacc*   accPtr,
  int32_t outElementType,
  tidlInnerProductBuffParams_t  * buffParams
  )
{
  int32_t satLow, sathigh;
  int32_t quantizationStyle = intAlgHandle->createParams->net->quantizationStyle;
  if (outElementType == TIDL_SinglePrecFloat)
  {
    sTIDL_Layer_t  * tidlLayer = &intAlgHandle->createParams->net->TIDLLayers[layerIdx];
    float32_tidl minFloat, maxFloat;
    TIDL_getSaturationFloat(tidlLayer, &minFloat, &maxFloat);
    TIDL_refInnerProduct(intAlgHandle, layerIdx, accPtr, (float32_tidl *)inPtr, (float32_tidl *)inBPtr, (float32_tidl *)refPtr, biasPtr, weightsPtr, buffParams, minFloat, maxFloat);
  }
  if(inBPtr == NULL)
  {
    if(sizeof(Tw) == 1)
    {
      buffParams->inBElementType = TIDL_SignedChar;
    }
    else if(sizeof(Tw) == 2)
    {
      buffParams->inBElementType = TIDL_SignedShort;
    }
  }

  if(TIDL_isAsymQuantEnabledTFL(quantizationStyle))
  {
    satLow = TIDL_SAT_LO_INT8;
    sathigh = TIDL_SAT_HI_INT8;
    TIDL_refInnerProduct(intAlgHandle, layerIdx, accPtr, (int8_t *)inPtr, (int8_t *)inBPtr, (int8_t *)refPtr,
      biasPtr, weightsPtr, buffParams, satLow, sathigh);
  }


  if(!TIDL_isAsymQuantEnabledTFL(quantizationStyle))
  {
    if ((outElementType == TIDL_SignedChar) ||
        (outElementType == TIDL_SignedShort))
    {
      if (TIDL_getDatElementSize(outElementType) == 1)
      {
        satLow = TIDL_SAT_LO_INT8;
        sathigh = TIDL_SAT_HI_INT8;
      }
      else
      {
        satLow = TIDL_SAT_LO_INT16;
        sathigh = TIDL_SAT_HI_INT16;
      }


      if ((buffParams->inElementType == TIDL_SignedChar) && (TIDL_getDatElementSize(outElementType) == 1))
      {
        if(buffParams->inBElementType == TIDL_SignedChar)
        {
          TIDL_refInnerProduct(intAlgHandle, layerIdx, accPtr, (int8_t *)inPtr, (int8_t *)inBPtr, (int8_t *)refPtr,
            biasPtr, weightsPtr, buffParams, satLow, sathigh);
        }
        else if (buffParams->inBElementType == TIDL_UnsignedChar)
        {
          TIDL_refInnerProduct(intAlgHandle, layerIdx, accPtr, (int8_t *)inPtr, (uint8_t *)inBPtr, (int8_t *)refPtr,
            biasPtr, weightsPtr, buffParams, satLow, sathigh);
        }
        else
        {
          tidl_printf(0,"Unsupported data type for inBData for IP layer exiting...\n    ");
          exit(-1);
        }
      }
      else if ((buffParams->inElementType == TIDL_UnsignedChar) && (TIDL_getDatElementSize(outElementType) == 1))
      {
        if(buffParams->inBElementType == TIDL_SignedChar)
        {
          TIDL_refInnerProduct(intAlgHandle, layerIdx, accPtr, (uint8_t *)inPtr, (int8_t *)inBPtr, (int8_t *)refPtr,
            biasPtr, weightsPtr, buffParams, satLow, sathigh);
        }
        else if (buffParams->inBElementType == TIDL_UnsignedChar)
        {
          TIDL_refInnerProduct(intAlgHandle, layerIdx, accPtr, (uint8_t *)inPtr, (uint8_t *)inBPtr, (int8_t *)refPtr,
            biasPtr, weightsPtr, buffParams, satLow, sathigh);
        }
        else
        {
          tidl_printf(0,"Unsupported data type for inBData for IP layer exiting...\n    ");
          exit(-1);
        }
      }
      else if ((buffParams->inElementType == TIDL_SignedChar) && (TIDL_getDatElementSize(outElementType) == 2))
      {
        if(buffParams->inBElementType == TIDL_SignedChar)
        {
          TIDL_refInnerProduct(intAlgHandle, layerIdx, accPtr, (int8_t *)inPtr, (int8_t *)inBPtr, (int16_t *)refPtr,
            biasPtr, weightsPtr, buffParams, satLow, sathigh);
        }
        else if (buffParams->inBElementType == TIDL_UnsignedChar)
        {
          TIDL_refInnerProduct(intAlgHandle, layerIdx, accPtr, (int8_t *)inPtr, (uint8_t *)inBPtr, (int16_t *)refPtr,
          biasPtr, weightsPtr, buffParams, satLow, sathigh);
        }
        else
        {
          tidl_printf(0,"Unsupported data type for inBData for IP layer exiting...\n    ");
          exit(-1);
        }
      }
      else if ((buffParams->inElementType == TIDL_UnsignedChar) && (TIDL_getDatElementSize(outElementType) == 2))
      {
        if(buffParams->inBElementType == TIDL_SignedChar)
        {
          TIDL_refInnerProduct(intAlgHandle, layerIdx, accPtr, (uint8_t *)inPtr, (int8_t *)inBPtr, (int16_t *)refPtr,
            biasPtr, weightsPtr, buffParams, satLow, sathigh);
        }
        else if (buffParams->inBElementType == TIDL_UnsignedChar)
        {
          TIDL_refInnerProduct(intAlgHandle, layerIdx, accPtr, (uint8_t *)inPtr, (uint8_t *)inBPtr, (int16_t *)refPtr,
            biasPtr, weightsPtr, buffParams, satLow, sathigh);
        }
        else
        {
          tidl_printf(0,"Unsupported data type for inBData for IP layer exiting...\n    ");
          exit(-1);
        }
      }
      else  if ((buffParams->inElementType == TIDL_SignedShort) && (TIDL_getDatElementSize(outElementType) == 1))
      {
        if(buffParams->inBElementType == TIDL_SignedShort)
        {
          TIDL_refInnerProduct(intAlgHandle, layerIdx, accPtr, (int16_t *)inPtr, (int16_t *)inBPtr, (int8_t *)refPtr,
            biasPtr, weightsPtr, buffParams, satLow, sathigh);
        }
        else if (buffParams->inBElementType == TIDL_UnsignedShort)
        {
          TIDL_refInnerProduct(intAlgHandle, layerIdx, accPtr, (int16_t *)inPtr, (uint16_t *)inBPtr, (int8_t *)refPtr,
            biasPtr, weightsPtr, buffParams, satLow, sathigh);
        }
        else
        {
          tidl_printf(0,"Unsupported data type for inBData for IP layer exiting...\n    ");
          exit(-1);
        }
      }
      else if ((buffParams->inElementType == TIDL_UnsignedShort) && (TIDL_getDatElementSize(outElementType) == 1))
      {
        if(buffParams->inBElementType == TIDL_SignedShort)
        {
          TIDL_refInnerProduct(intAlgHandle, layerIdx, accPtr, (uint16_t *)inPtr, (int16_t *)inBPtr, (int8_t *)refPtr,
            biasPtr, weightsPtr, buffParams, satLow, sathigh);
        }
        else if (buffParams->inBElementType == TIDL_UnsignedShort)
        {
          TIDL_refInnerProduct(intAlgHandle, layerIdx, accPtr, (uint16_t *)inPtr, (uint16_t *)inBPtr, (int8_t *)refPtr,
            biasPtr, weightsPtr, buffParams, satLow, sathigh);
        }
        else
        {
          tidl_printf(0,"Unsupported data type for inBData for IP layer exiting...\n    ");
          exit(-1);
        }
      }
      else if ((buffParams->inElementType == TIDL_SignedShort) && (TIDL_getDatElementSize(outElementType) == 2))
      {
        if(buffParams->inBElementType == TIDL_SignedShort)
        {
          TIDL_refInnerProduct(intAlgHandle, layerIdx, accPtr, (int16_t *)inPtr, (int16_t *)inBPtr, (int16_t *)refPtr,
            biasPtr, weightsPtr, buffParams, satLow, sathigh);
        }
        else if (buffParams->inBElementType == TIDL_UnsignedShort)
        {
          TIDL_refInnerProduct(intAlgHandle, layerIdx, accPtr, (int16_t *)inPtr, (uint16_t *)inBPtr, (int16_t *)refPtr,
            biasPtr, weightsPtr, buffParams, satLow, sathigh);
        }
        else
        {
          tidl_printf(0,"Unsupported data type for inBData for IP layer exiting...\n    ");
          exit(-1);
        }

      }
      else if ((buffParams->inElementType == TIDL_UnsignedShort) && (TIDL_getDatElementSize(outElementType) == 2))
      {
        if(buffParams->inBElementType == TIDL_SignedShort)
        {
          TIDL_refInnerProduct(intAlgHandle, layerIdx, accPtr, (uint16_t *)inPtr, (int16_t *)inBPtr, (int16_t *)refPtr,
            biasPtr, weightsPtr, buffParams, satLow, sathigh);
        }
        else if (buffParams->inBElementType == TIDL_UnsignedShort)
        {
          TIDL_refInnerProduct(intAlgHandle, layerIdx, accPtr, (uint16_t *)inPtr, (uint16_t *)inBPtr, (int16_t *)refPtr,
            biasPtr, weightsPtr, buffParams, satLow, sathigh);
        }
        else
        {
          tidl_printf(0,"Unsupported data type for inBData for IP layer exiting...\n    ");
          exit(-1);
        }

      }
    }
    else /* Unsigned output */
    {

      if (TIDL_getDatElementSize(outElementType) == 1)
      {
        satLow = 0;
        sathigh = TIDL_SAT_HI_UINT8;
      }
      else
      {
        satLow = 0;
        sathigh = TIDL_SAT_HI_UINT16;
      }

      if ((buffParams->inElementType == TIDL_SignedChar) && (TIDL_getDatElementSize(outElementType) == 1))
      {
        if(buffParams->inBElementType == TIDL_SignedChar)
        {
          TIDL_refInnerProduct(intAlgHandle, layerIdx, accPtr, (int8_t *)inPtr,(int8_t *)inBPtr, (uint8_t *)refPtr,
            biasPtr, weightsPtr, buffParams, satLow, sathigh);
        }
        else if (buffParams->inBElementType == TIDL_UnsignedChar)
        {
          TIDL_refInnerProduct(intAlgHandle, layerIdx, accPtr, (int8_t *)inPtr,(uint8_t *)inBPtr, (uint8_t *)refPtr,
            biasPtr, weightsPtr, buffParams, satLow, sathigh);
        }
        else
        {
          tidl_printf(0,"Unsupported data type for inBData for IP layer exiting...\n    ");
          exit(-1);
        }
      }
      else if ((buffParams->inElementType == TIDL_UnsignedChar) && (TIDL_getDatElementSize(outElementType) == 1))
      {
        if(buffParams->inBElementType == TIDL_SignedChar)
        {
          TIDL_refInnerProduct(intAlgHandle, layerIdx, accPtr, (uint8_t *)inPtr,(int8_t *)inBPtr, (uint8_t *)refPtr,
            biasPtr, weightsPtr, buffParams, satLow, sathigh);
        }
        else if (buffParams->inBElementType == TIDL_UnsignedChar)
        {
          TIDL_refInnerProduct(intAlgHandle, layerIdx, accPtr, (uint8_t *)inPtr,(uint8_t *)inBPtr, (uint8_t *)refPtr,
            biasPtr, weightsPtr, buffParams, satLow, sathigh);
        }
        else
        {
          tidl_printf(0,"Unsupported data type for inBData for IP layer exiting...\n    ");
          exit(-1);
        }
      }
      else if ((buffParams->inElementType == TIDL_SignedChar) && (TIDL_getDatElementSize(outElementType) == 2))
      {
        if(buffParams->inBElementType == TIDL_SignedChar)
        {
          TIDL_refInnerProduct(intAlgHandle, layerIdx, accPtr, (int8_t *)inPtr, (int8_t *)inBPtr, (uint16_t *)refPtr,
            biasPtr, weightsPtr, buffParams, satLow, sathigh);
        }
        else if (buffParams->inBElementType == TIDL_UnsignedChar)
        {
          TIDL_refInnerProduct(intAlgHandle, layerIdx, accPtr, (int8_t *)inPtr, (uint8_t *)inBPtr, (uint16_t *)refPtr,
            biasPtr, weightsPtr, buffParams, satLow, sathigh);
        }
        else
        {
          tidl_printf(0,"Unsupported data type for inBData for IP layer exiting...\n    ");
          exit(-1);
        }
      }
      else if ((buffParams->inElementType == TIDL_UnsignedChar) && (TIDL_getDatElementSize(outElementType) == 2))
      {
        if(buffParams->inBElementType == TIDL_SignedChar)
        {
          TIDL_refInnerProduct(intAlgHandle, layerIdx, accPtr, (uint8_t *)inPtr, (int8_t *)inBPtr, (uint16_t *)refPtr,
            biasPtr, weightsPtr, buffParams, satLow, sathigh);
        }
        else if (buffParams->inBElementType == TIDL_UnsignedChar)
        {
          TIDL_refInnerProduct(intAlgHandle, layerIdx, accPtr,(uint8_t *)inPtr, (uint8_t *)inPtr, (uint16_t *)refPtr,
            biasPtr, weightsPtr, buffParams, satLow, sathigh);
        }
        else
        {
          tidl_printf(0,"Unsupported data type for inBData for IP layer exiting...\n    ");
          exit(-1);
        }

      }
      else if ((buffParams->inElementType == TIDL_SignedShort) && (TIDL_getDatElementSize(outElementType) == 1))
      {
        if(buffParams->inBElementType == TIDL_SignedShort)
        {
          TIDL_refInnerProduct(intAlgHandle, layerIdx, accPtr, (int16_t *)inPtr, (int16_t *)inBPtr, (uint8_t *)refPtr,
            biasPtr, weightsPtr, buffParams, satLow, sathigh);
        }
        else if (buffParams->inBElementType == TIDL_UnsignedShort)
        {
          TIDL_refInnerProduct(intAlgHandle, layerIdx, accPtr, (int16_t *)inPtr, (uint16_t *)inBPtr, (uint8_t *)refPtr,
            biasPtr, weightsPtr, buffParams, satLow, sathigh);
        }
        else
        {
          tidl_printf(0,"Unsupported data type for inBData for IP layer exiting...\n    ");
          exit(-1);
        }

      }
      else if ((buffParams->inElementType == TIDL_UnsignedShort) && (TIDL_getDatElementSize(outElementType) == 1))
      {
        if(buffParams->inBElementType == TIDL_SignedShort)
        {
          TIDL_refInnerProduct(intAlgHandle, layerIdx, accPtr, (uint16_t *)inPtr, (int16_t *)inBPtr, (uint8_t *)refPtr,
            biasPtr, weightsPtr, buffParams, satLow, sathigh);
        }
        else if (buffParams->inBElementType == TIDL_UnsignedShort)
        {
          TIDL_refInnerProduct(intAlgHandle, layerIdx, accPtr, (uint16_t *)inPtr, (uint16_t *)inBPtr, (uint8_t *)refPtr,
            biasPtr, weightsPtr, buffParams, satLow, sathigh);
        }
        else
        {
          tidl_printf(0,"Unsupported data type for inBData for IP layer exiting...\n    ");
          exit(-1);
        }

      }
      else if ((buffParams->inElementType == TIDL_SignedShort) && (TIDL_getDatElementSize(outElementType) == 2))
      {
        if(buffParams->inBElementType == TIDL_SignedShort)
        {
          TIDL_refInnerProduct(intAlgHandle, layerIdx, accPtr, (int16_t *)inPtr, (int16_t *)inBPtr, (uint16_t *)refPtr,
            biasPtr, weightsPtr, buffParams, satLow, sathigh);
        }
        else if (buffParams->inBElementType == TIDL_UnsignedShort)
        {
          TIDL_refInnerProduct(intAlgHandle, layerIdx, accPtr, (int16_t *)inPtr, (int16_t *)inBPtr, (uint16_t *)refPtr,
            biasPtr, weightsPtr, buffParams, satLow, sathigh);
        }
        else
        {
          tidl_printf(0,"Unsupported data type for inBData for IP layer exiting...\n    ");
          exit(-1);
        }

      }
      else if ((buffParams->inElementType == TIDL_UnsignedShort) && (TIDL_getDatElementSize(outElementType) == 2))
      {
        if(buffParams->inBElementType == TIDL_SignedShort)
        {
          TIDL_refInnerProduct(intAlgHandle, layerIdx, accPtr, (uint16_t *)inPtr, (int16_t *)inBPtr, (uint16_t *)refPtr,
            biasPtr, weightsPtr, buffParams, satLow, sathigh);
        }
        else if (buffParams->inBElementType == TIDL_UnsignedShort)
        {
          TIDL_refInnerProduct(intAlgHandle, layerIdx, accPtr, (uint16_t *)inPtr, (uint16_t *)inBPtr, (uint16_t *)refPtr,
            biasPtr, weightsPtr, buffParams, satLow, sathigh);
        }
        else
        {
          tidl_printf(0,"Unsupported data type for inBData for IP layer exiting...\n    ");
          exit(-1);
        }
      }

    }
  }

}
 /**
 * @brief InnerProduct layer reference implementation
 *
 * @param intAlgHandle : tidl algorothm handle
 * @param algLayer : Pointer to the layer specific parameters
 * @param tidlLayer : Pointer to the common layer parameters
 * @param params : copy of innerProduct layer parameters
 * @param buffParams : parameters of the innerProduct buffer
 * @param inPtr : Pointer to input buffers to be processed
 * @param outPtr : Pointer to output buffers to be processed
 * @return  IALG_EOK   - Successful
 *          IALG_EFAIL - Unspecified error
 */
int32_t TIDL_innerProductRefProcess(
                                  TIDL_Handle          intAlgHandle,
                                  sTIDL_AlgLayer_t     * algLayer,
                                  sTIDL_Layer_t        * tidlLayer,
                                  sTIDL_InnerProductParams_t * params,
                                  tidlInnerProductBuffParams_t  * buffParams,
                                  void   *inPtr,
                                  void   *inBPtr,
                                  void   *outPtr
                                  )
{
  int32_t     i0, status = IALG_EOK;

  sTIDL_Network_t * net = intAlgHandle->createParams->net;
  int32_t quantizationStyle = intAlgHandle->createParams->net->quantizationStyle;
  int32_t layerIdx = algLayer->layerIdx;
  void * accPtr = NULL;
  void * weightPtr    = ((int8_t *)(intAlgHandle->createParams->net) + params->weights);
  void * orgBiasPtr   = ((int8_t *)(intAlgHandle->createParams->net) + params->bias);
  void * biasPtr;
  int32_t numOutputs;

  if(TIDL_forceInnerProductRef(intAlgHandle->createParams,tidlLayer)){
    //Forced Refrence case. in that case the space to hold weights is from algLayer
    weightPtr   = algLayer->layerParams.innerProductParams.wtTranformMem;
    orgBiasPtr  = (void*)get_int8_t_pointer((int8_t *)(weightPtr), params->bias - params->weights);
    if (tidlLayer->numInBufs == 1) 
    {
      inBPtr      = NULL;
    }
  }

  float32_tidl * orgBiasPtrFloat = (float32_tidl *)orgBiasPtr;
  numOutputs = params->numBatches * params->numInRows * params->numOutCols;

  if(params->isBias)
  {
    if (algLayer->layerParams.innerProductParams.biasParamSize >= (numOutputs*sizeof(int16_t)))
    {
      biasPtr = (void *)algLayer->layerParams.innerProductParams.biasParamMem;
    }
    else
    {
      tidl_printf(0,"Memory for  biasptr  is not sufficient exiting...\n    ");
      exit(-1);
    }
  }

  if(TIDL_isAsymQuantEnabledTFL(quantizationStyle))
  {
    int32_t* fBiasPtr = (int32_t*)algLayer->layerParams.innerProductParams.biasParamMem; //Txed bias
    TIDL_refInnerProductParamBitDepth(intAlgHandle, layerIdx, inPtr, inBPtr, outPtr, (int32_t *)fBiasPtr, (int8_t *)weightPtr, (int32_t*)accPtr, tidlLayer->outData.elementType, buffParams);
  }
  else
  {
    if (tidlLayer->weightsElementSizeInBits <= 8)
    {
      if(TIDL_isKernelHighPrecision(tidlLayer->layerKernelType))
      {
        /*Set bias ptr:*/
        int32_t* fBiasPtr  = (int32_t *)get_int8_t_pointer((int8_t *)(net), net->TIDLLayers[layerIdx].layerParams.innerProductParams.derivedBias);
        TIDL_refInnerProductParamBitDepth(intAlgHandle, layerIdx, inPtr, inBPtr, outPtr, (int32_t *)fBiasPtr, (int8_t *)weightPtr, (int32_t*)accPtr, tidlLayer->outData.elementType, buffParams);        
      }
      else
      {
        if(params->isBias)
        {
          if (params->biasScale != -1)
          {
            TIDL_conv2dBiasSplit((int16_t*)orgBiasPtr, (int16_t*)biasPtr, &buffParams->biasB, numOutputs, buffParams->inScaleFactor, TIDL_SAT_LO_INT16, TIDL_SAT_HI_INT16,0xFF, TIDL_getDatElementSign(buffParams->inElementType));
          }
          for (i0 = 0; i0 < numOutputs; i0++)
          {
            ((int16_t*)biasPtr)[i0] = ((int16_t*)biasPtr)[i0] * buffParams->biasB;
          }
        }
        TIDL_refInnerProductParamBitDepth(intAlgHandle, layerIdx, inPtr, inBPtr, outPtr, (int16_t *)biasPtr, (int8_t *)weightPtr, (int32_t*)accPtr, tidlLayer->outData.elementType, buffParams);
      }
    }
    else if (tidlLayer->weightsElementSizeInBits <= 16)
    {
      if(params->isBias)
      {
        if (params->biasScale != -1)
        {
          TIDL_conv2dBiasSplit((int16_t*)orgBiasPtr, (int32_t*)biasPtr, &buffParams->biasB, numOutputs, buffParams->inScaleFactor, TIDL_SAT_LO_INT32, TIDL_SAT_HI_INT32, 0xFFFF, TIDL_getDatElementSign(buffParams->inElementType));
        }
        for (i0 = 0; i0 < numOutputs; i0++)
        {
          ((int32_t*)biasPtr)[i0] = ((int32_t*)biasPtr)[i0] * buffParams->biasB;
        }
      }
      TIDL_refInnerProductParamBitDepth(intAlgHandle, layerIdx, inPtr, inBPtr, outPtr, (int32_t *)biasPtr, (int16_t *)weightPtr, (int64_t*)accPtr, tidlLayer->outData.elementType, buffParams);
    }
    else if (tidlLayer->weightsElementSizeInBits == 32)
    {
      TIDL_refInnerProductParamBitDepth(intAlgHandle, layerIdx, inPtr, inBPtr, outPtr, orgBiasPtrFloat, (float32_tidl *)weightPtr, (float32_tidl*)accPtr, tidlLayer->outData.elementType, buffParams);
    }
  }
  TIDL_enableL1DandL2CacheWb();
  return status;
}

int32_t TIDL_forceInnerProductRef(
  const TIDL_CreateParams  *createParamsPtr,
  sTIDL_Layer_t* tidlLayer
)
{
  int32_t forceRef ;
  sTIDL_Network_t *pNet = createParamsPtr->net ;
  sTIDL_DataParams_t *inDataParams;
  uint16_t numInNodes, numOutNodes ;
  int32_t isMatmul = tidlLayer->numInBufs > 1 ? TRUE : FALSE;

  inDataParams   = &pNet->TIDLLayers[tidlLayer->inData[0]].outData;
  numOutNodes    = (uint16_t)tidlLayer->outData.dimValues[TIDL_DIM_WIDTH];
  numInNodes     = (uint16_t)inDataParams->dimValues[TIDL_DIM_WIDTH];

  forceRef = !TIDL_referencFlow(createParamsPtr);

  if (forceRef == 1)
  {
    if (((createParamsPtr->flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) == 0) &&
        ((createParamsPtr->reservedCtrl & TIDL_REF_CODE_FOR_IP) == 0))
    {
      if(isMatmul == TRUE)
      {
        forceRef = 0 ;
      }
      else if (((numInNodes <= 2048) && (numOutNodes <= 1024)) && (numOutNodes >= 16)) /*InnerProduct*/
      {
        forceRef = 0 ;
      }
    }
  }
  return forceRef ;
}

void TIDL_innerProductGetBiasScaleSize(sTIDL_Layer_t        * tidlLayer,
                                  int32_t                   * biasSize,
                                  int32_t                   * scaleSize,
                                  int32_t                   * shiftSize)
{
  *scaleSize = (tidlLayer->outData.dimValues[TIDL_DIM_WIDTH] * 
            tidlLayer->outData.dimValues[TIDL_DIM_NUMCH]  *
            tidlLayer->outData.dimValues[TIDL_DIM_DIM1]   * 
            tidlLayer->outData.dimValues[TIDL_DIM_DIM2]) * sizeof(uint8_t);

  *shiftSize = *scaleSize;
  *biasSize = (*scaleSize) * sizeof(int32_t);
  return;
}


/**
----------------------------------------------------------------------------
@ingroup    TIDL_IVISION_SUPPORT
@fn         TIDL_innerProductAlloc
@brief      Function to Request Memory required by the Innerproduct layer

@param      specificParams : Pointer to specific parameter required for all the layers
@param      commonParams   : Pointer to common parameter required for all the layers
@param      layerIdx       : Layer Id of the layer
@param      memorySize     : Output memory requirement of the layer
@remarks    None
@return     IALG_EOK   - Successful
            IALG_EFAIL - Unspecified error
----------------------------------------------------------------------------
*/

int32_t TIDL_innerProductAllocNew(const TIDL_LayerSpecificParams *layerSpecificParams,
                                    const TIDL_NetworkCommonParams *commonParams,
                                    int32_t layerIdx,
                                    int32_t memorySize[TIDL_LAYER_MEMORY_MAX])
{
  int32_t status = IALG_EOK;
  int32_t forceRef = TIDL_forceInnerProductRef(commonParams->createParams, &commonParams->net->TIDLLayers[layerIdx]);

  // TODO take from dataparams
  int32_t numOutputs = commonParams->net->TIDLLayers[layerIdx].layerParams.innerProductParams.numBatches * \
                        commonParams->net->TIDLLayers[layerIdx].layerParams.innerProductParams.numInRows * \
                        commonParams->net->TIDLLayers[layerIdx].layerParams.innerProductParams.numOutCols;

  int32_t biasParamSize = 0;
  int32_t tranformSize  = TIDL_getDatElementSize(commonParams->net->TIDLLayers[layerIdx].outData.elementType) *
      TIDL_innerProductCoeffMemRequired(&commonParams->net->TIDLLayers[layerIdx].layerParams.innerProductParams);

  if(commonParams->net->TIDLLayers[layerIdx].layerParams.innerProductParams.isBias)
  {
    biasParamSize = numOutputs *  (((commonParams->net->TIDLLayers[layerIdx].weightsElementSizeInBits + 7) / 8)) *  ((int32_t)(sizeof(int32_t)));
  }

  if (((commonParams->createParams->flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) == 0) && (forceRef == 0))
  {
    if (commonParams->net->TIDLLayers[layerIdx].layerParams.innerProductParams.weights == 0) /* Matmul */
    {
      status = TIDL_deviceUtilsCommonAlloc(layerSpecificParams, commonParams, layerIdx, memorySize);
      if (TIDL_isKernelHighPrecision(commonParams->net->TIDLLayers[layerIdx].layerKernelType) == TRUE)
      {
        int32_t biasSize = 0;
        int32_t scaleSize = 0;
        int32_t shiftSize = 0;
        TIDL_innerProductGetBiasScaleSize(&commonParams->net->TIDLLayers[layerIdx], &biasSize, &scaleSize, &shiftSize);
        memorySize[TIDL_LAYER_MEMORY_PERSISTENT] += ((uint32_t)biasSize + scaleSize + shiftSize + TIDL_ALIGNMENT_SIZE);
      }
    }
    else /* Inner Product */
    {
      memorySize[TIDL_LAYER_MEMORY_PERSISTENT] += ((uint32_t)tranformSize + TIDL_ALIGNMENT_SIZE);
    }
  }
  else
  {
    status = TIDL_deviceCommonRefAlloc(layerSpecificParams, commonParams, layerIdx, memorySize);
    /* Have addtional space for bias during forced reference for inner product */
    if(forceRef == 1)
    {
      memorySize[TIDL_LAYER_MEMORY_PERSISTENT] += ((uint32_t)tranformSize + biasParamSize + TIDL_ALIGNMENT_SIZE);
    }
  }

  if((commonParams->createParams->flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) == 0)
  {
    memorySize[TIDL_LAYER_MEMORY_PERSISTENT] += ((uint32_t)tranformSize + TIDL_ALIGNMENT_SIZE);
  }

  memorySize[TIDL_LAYER_MEMORY_PERSISTENT] += biasParamSize   + TIDL_ALIGNMENT_SIZE;

  return status;
}

/**
----------------------------------------------------------------------------
@ingroup    TIDL_IVISION_SUPPORT
@fn         TIDL_innerProductInit
@brief      Function to Initialize the InnerProduct layer specific params

@param      specificParams :  Pointer to specific parameter required for all the layers
@param      commonParams   :  Pointer to common parameter required for all the layers
@param      algLayer       :  Pointer to layer specific internal parameters
@param      layerIdx       :  Layer Id of the layer
@param      memory         :  Allocated memory pointers
@param      memorySize     :  Output memory requirement of the layer
@param      outPtr         :  Output memory pointer. Applicable only for reference flow
@remarks    None
@return     IALG_EOK   - Successful
            IALG_EFAIL - Unspecified error
----------------------------------------------------------------------------
*/

int32_t TIDL_innerProductInitNew(const TIDL_LayerSpecificParams *layerSpecificParams,
                                const TIDL_NetworkCommonParams  *commonParams,
                                sTIDL_AlgLayer_t               *algLayer,
                                int32_t layerIdx,
                                uint8_t *memory[TIDL_LAYER_MEMORY_MAX],
                                int32_t memorySize[TIDL_LAYER_MEMORY_MAX],
                                void    **outPtr)

{
  int32_t status = IALG_EOK;
  int32_t forceRef = TIDL_forceInnerProductRef(commonParams->createParams, &commonParams->net->TIDLLayers[layerIdx]);
  int32_t tranformSize;
  int32_t paramMemTabOffset = 0;
  int32_t quantizationStyle = commonParams->net->quantizationStyle;

  tranformSize  = TIDL_getDatElementSize(commonParams->net->TIDLLayers[layerIdx].outData.elementType) *
                  TIDL_innerProductCoeffMemRequired(&commonParams->net->TIDLLayers[layerIdx].layerParams.innerProductParams);

  sTIDL_InnerProductParams_t * innerProductParams = &commonParams->net->TIDLLayers[layerIdx].layerParams.innerProductParams;

  int32_t numOutputs = commonParams->net->TIDLLayers[layerIdx].layerParams.innerProductParams.numBatches * \
                      commonParams->net->TIDLLayers[layerIdx].layerParams.innerProductParams.numInRows * \
                      commonParams->net->TIDLLayers[layerIdx].layerParams.innerProductParams.numOutCols;

  algLayer->layerParams.innerProductParams.accMem     = NULL;
  algLayer->layerParams.innerProductParams.accMemSize = 0;

  algLayer->layerParams.innerProductParams.wtTranformMem  = NULL;
  algLayer->layerParams.innerProductParams.wtTranformSize = 0 ;

  algLayer->layerParams.innerProductParams.biasParamMem  = NULL;
  algLayer->layerParams.innerProductParams.biasParamSize = 0;

  algLayer->layerParams.innerProductParams.matmulScaleMem  = NULL;
  algLayer->layerParams.innerProductParams.matmulScaleMemSize = 0;

  algLayer->layerParams.innerProductParams.matmulShiftMem  = NULL;
  algLayer->layerParams.innerProductParams.matmulShiftMemSize = 0;
  
  algLayer->layerParams.innerProductParams.maxWorkloadLoopIter = 1;

  if(commonParams->net->TIDLLayers[layerIdx].layerParams.innerProductParams.isBias)
  {
    if(TIDL_isBiasHighPrecision(commonParams->net->deviceName) == TRUE)
    {
      if(commonParams->net->TIDLLayers[layerIdx].weightsElementSizeInBits <= 8)
      {
        algLayer->layerParams.innerProductParams.biasParamSize =
          ALIGN(numOutputs, 64) *  ((int32_t)(sizeof(int32_t)));
      }
      else
      {
        algLayer->layerParams.innerProductParams.biasParamSize =
        ALIGN(numOutputs, 64) *  ((int32_t)(sizeof(int64_t)));
      }
    }
    else
    {
      algLayer->layerParams.innerProductParams.biasParamSize = numOutputs *  ((int32_t)(sizeof(int32_t)));
    }

    TIDL_AllocatePtr((intptr_t)memory[TIDL_LAYER_MEMORY_PERSISTENT],
                        &paramMemTabOffset,
                        algLayer->layerParams.innerProductParams.biasParamSize,
                        TIDL_ALIGNMENT_SIZE,
                        &algLayer->layerParams.innerProductParams.biasParamMem);
  }
  else
  {
    algLayer->layerParams.innerProductParams.biasParamMem = NULL;
  }


  TIDL_AllocatePtr((intptr_t)memory[TIDL_LAYER_MEMORY_PERSISTENT],
                    &paramMemTabOffset,
                    algLayer->layerParams.innerProductParams.accMemSize,
                    TIDL_ALIGNMENT_SIZE,
                    &algLayer->layerParams.innerProductParams.accMem);

  // algLayer->layerParams.innerProductParams.accMem =
  //   get_uint8_t_pointer((uint8_t*)(void*)memRec[ALG_LAYERS_PARAMS_BUFF_MEMREC].base, *paramMemTabOffset);
  // *paramMemTabOffset += ALIGN_SIZE((int32_t)scratchDataSize, 128);

  algLayer->layerParams.innerProductParams.wtTranformMem = NULL;
  algLayer->layerParams.innerProductParams.wtTranformSize = 0 ;

  //Have addtional space for bias during forced reference for inner product
  if(forceRef == 1)
  {
    tranformSize += algLayer->layerParams.innerProductParams.biasParamSize;
  }
  if(((commonParams->createParams->flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) == 0) &&
      (commonParams->net->TIDLLayers[layerIdx].layerParams.innerProductParams.weights != 0))
  {
    algLayer->layerParams.innerProductParams.wtTranformSize = tranformSize;

    TIDL_AllocatePtr((intptr_t)memory[TIDL_LAYER_MEMORY_PERSISTENT],
                      &paramMemTabOffset,
                      algLayer->layerParams.innerProductParams.wtTranformSize,
                      TIDL_ALIGNMENT_SIZE,
                      &algLayer->layerParams.innerProductParams.wtTranformMem);

    // algLayer->layerParams.innerProductParams.wtTranformMem =
    //   get_uint8_t_pointer((uint8_t*)(void*)memRec[ALG_LAYERS_PARAMS_BUFF_MEMREC].base, *paramMemTabOffset);

    memset(algLayer->layerParams.innerProductParams.wtTranformMem, 0, (size_t)algLayer->layerParams.innerProductParams.wtTranformSize);
  }

  int8_t * weightPtr = get_int8_t_pointer((int8_t *)(commonParams->net), innerProductParams->weights);
  void * orgBiasPtr = NULL;
  void * biasPtr = NULL;
  float32_tidl inScaleFactor;
  sTIDL_DataParams_t * inDataParams = &commonParams->net->TIDLLayers[algLayer->inLayerIdx[0]].outData;
  int32_t layerType = commonParams->net->TIDLLayers[algLayer->inLayerIdx[0]].layerType;
  if (layerType == TIDL_ConstDataLayer)
  {
    inDataParams = &commonParams->net->TIDLLayers[algLayer->inLayerIdx[1]].outData;
  }

  if(TIDL_QuantStyleAsymNP2_TFL != quantizationStyle)
  {
    orgBiasPtr = (int16_t *)get_int8_t_pointer((int8_t*)(commonParams->net), innerProductParams->bias);
    biasPtr = (int16_t*)(algLayer->layerParams.innerProductParams.biasParamMem);
    if (innerProductParams->weightScale != 0)
    {
      inScaleFactor = (innerProductParams->weightScale * inDataParams->tensorScale) / innerProductParams->biasScale;
    }
    else
    {
      sTIDL_DataParams_t *inDataParams2;
      inDataParams = &commonParams->net->TIDLLayers[algLayer->inLayerIdx[0]].outData;
      inDataParams2 = &commonParams->net->TIDLLayers[algLayer->inLayerIdx[1]].outData;
      inScaleFactor = (inDataParams->tensorScale * inDataParams2->tensorScale) / innerProductParams->biasScale;
    }
    if (innerProductParams->biasScale == -1.0f)
    {
      inScaleFactor = 0.0f;
    }
  }

  int32_t * orgbiasptr;
  if(TIDL_QuantStyleAsymNP2_TFL == quantizationStyle)
  {
    //Populate scale and shift for MMA for convolution:
    //Scale ratio
    float32_tidl scaleRatio = 1;
    float32_tidl weightScalePtr =  innerProductParams->weightScale;
    float32_tidl scale_x = inDataParams->tensorScale;
    float32_tidl scale_y = commonParams->net->TIDLLayers[layerIdx].outData.tensorScale;
    //Assuming per axis: (should be single iteration for non per ch) !Replace with axis length
    //Temp logic:
    scaleRatio = (scale_x * weightScalePtr)/scale_y;
    TIDL_getMMAv2_ScaleAndShift(scaleRatio, &algLayer->layerParams.innerProductParams.mmaScale,&algLayer->layerParams.innerProductParams.mmaShift);
    int32_t axisCtr;
    orgbiasptr = (int32_t *)get_int8_t_pointer((int8_t *)(commonParams->net), innerProductParams->bias);
    biasPtr  = (int32_t *)algLayer->layerParams.innerProductParams.biasParamMem;
    //Simple copy over of bias from model to local memory.
    for(axisCtr = 0; axisCtr < numOutputs; axisCtr++)
    {
      ((int32_t*)biasPtr)[axisCtr] = orgbiasptr[axisCtr];
    }
  }

#if USE_16BIT_BIAS_FOR_8BIT_MODE
  if((innerProductParams->constIdx == -1))
  {
    if (commonParams->net->TIDLLayers[layerIdx].weightsElementSizeInBits <= 8)
    {
      if(TIDL_QuantStyleAsymNP2_TFL != quantizationStyle)//No Splitting
      {
        if(innerProductParams->isBias)
        {
          if(TIDL_isBiasHighPrecision(commonParams->net->deviceName) == TRUE)
          {
            TIDL_AM_conv2dBiasSplit((int16_t*)orgBiasPtr, (int32_t*)biasPtr, &innerProductParams->biasB, numOutputs, inScaleFactor, TIDL_SAT_LO_INT16, TIDL_SAT_HI_INT16,0xFF, TIDL_getDatElementSign(inDataParams->elementType));
          }
          else
          {
            TIDL_conv2dBiasSplit((int16_t*)orgBiasPtr, (int16_t*)biasPtr, &innerProductParams->biasB,
            numOutputs, inScaleFactor, TIDL_SAT_LO_INT16, TIDL_SAT_HI_INT16,0xFF,TIDL_getDatElementSign(inDataParams->elementType));
          }
        }

        if(((commonParams->createParams->flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) == 0) &&
            (commonParams->net->TIDLLayers[layerIdx].layerParams.innerProductParams.weights != 0))
        {

          int8_t *tranformPtr = (int8_t*)algLayer->layerParams.innerProductParams.wtTranformMem;
          algLayer->layerParams.innerProductParams.wtTransformBatchPitch=
          TIDL_innerProductTransform(weightPtr, tranformPtr, (int16_t*)biasPtr,
            innerProductParams->numInCols, innerProductParams->numOutCols, innerProductParams->numBatches,
            innerProductParams->isBias, innerProductParams->inputBTranspose, commonParams->net->deviceName);
          algLayer->layerParams.innerProductParams.biasBatchPitch = innerProductParams->numOutCols;
        }
      }
    }
    else
  #endif
    {
      if(TIDL_QuantStyleAsymNP2_TFL != quantizationStyle)//No Splitting
      {
        if(innerProductParams->isBias)
        {
          if((TIDL_isBiasHighPrecision(commonParams->net->deviceName) == TRUE))
          {
            TIDL_AM_conv2dBiasSplit((int16_t*)orgBiasPtr, (int64_t*)biasPtr, &innerProductParams->biasB, numOutputs, inScaleFactor, TIDL_SAT_LO_INT32, TIDL_SAT_HI_INT32, 0xFFFF, TIDL_getDatElementSign(inDataParams->elementType));
          }
          else
          {
            TIDL_conv2dBiasSplit((int16_t*)orgBiasPtr, (int16_t*)biasPtr,  &innerProductParams->biasB,
            numOutputs, inScaleFactor, TIDL_SAT_LO_INT32, TIDL_SAT_HI_INT32,0xFFFF,TIDL_getDatElementSign(inDataParams->elementType));
          }
        }

        if(((commonParams->createParams->flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) == 0) &&
            (commonParams->net->TIDLLayers[layerIdx].layerParams.innerProductParams.weights != 0))
        {
          int16_t *tranformPtr = (int16_t*)algLayer->layerParams.innerProductParams.wtTranformMem;
          algLayer->layerParams.innerProductParams.wtTransformBatchPitch=
          TIDL_innerProductTransform((int16_t*)weightPtr, tranformPtr, (int16_t*)biasPtr,
            innerProductParams->numInCols, innerProductParams->numOutCols, innerProductParams->numBatches,
            innerProductParams->isBias, innerProductParams->inputBTranspose, commonParams->net->deviceName);
          algLayer->layerParams.innerProductParams.biasBatchPitch = innerProductParams->numOutCols;
        }
      }
    }

    if(TIDL_QuantStyleAsymNP2_TFL == quantizationStyle)
    {
      if((commonParams->createParams->flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) == 0)
      {
        int8_t *tranformPtr = (int8_t*)algLayer->layerParams.innerProductParams.wtTranformMem;
        algLayer->layerParams.innerProductParams.wtTransformBatchPitch=
        TIDL_innerProductTransform(weightPtr, tranformPtr, (int32_t*)biasPtr,
          innerProductParams->numInCols, innerProductParams->numOutCols, innerProductParams->numBatches,
          innerProductParams->isBias, innerProductParams->inputBTranspose, commonParams->net->deviceName);
        algLayer->layerParams.innerProductParams.biasBatchPitch = innerProductParams->numOutCols;
      }
    }
  }

  if (((commonParams->createParams->flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) == 0) && 
      (TIDL_isKernelHighPrecision(commonParams->net->TIDLLayers[layerIdx].layerKernelType) == TRUE) &&
      (commonParams->net->TIDLLayers[layerIdx].layerParams.innerProductParams.weights == 0))
  {
    void *biasScaleShiftPtr = NULL;
    uint8_t* mmav2_bias   = (uint8_t *)get_int8_t_pointer((int8_t *)(commonParams->net), commonParams->net->TIDLLayers[layerIdx].layerParams.innerProductParams.derivedBias);
    uint8_t* mmav2_Scales = (uint8_t *)get_int8_t_pointer((int8_t *)(commonParams->net), commonParams->net->TIDLLayers[layerIdx].layerParams.innerProductParams.derivedScales);
    uint8_t* mmav2_Shifts = (uint8_t *)get_int8_t_pointer((int8_t *)(commonParams->net), commonParams->net->TIDLLayers[layerIdx].layerParams.innerProductParams.derivedShifts);

    TIDL_innerProductGetBiasScaleSize(&commonParams->net->TIDLLayers[layerIdx], 
                                  &algLayer->layerParams.innerProductParams.biasParamSize, 
                                  &algLayer->layerParams.innerProductParams.matmulScaleMemSize,
                                  &algLayer->layerParams.innerProductParams.matmulShiftMemSize);
    
    TIDL_AllocatePtr((intptr_t)memory[TIDL_LAYER_MEMORY_PERSISTENT],
                  &paramMemTabOffset,
                  (algLayer->layerParams.innerProductParams.biasParamSize + 
                  algLayer->layerParams.innerProductParams.matmulScaleMemSize +
                  algLayer->layerParams.innerProductParams.matmulShiftMemSize),
                  TIDL_ALIGNMENT_SIZE,
                  &biasScaleShiftPtr);

    algLayer->layerParams.innerProductParams.biasParamMem = biasScaleShiftPtr;
    algLayer->layerParams.innerProductParams.matmulScaleMem = (void *)((uint8_t *)algLayer->layerParams.innerProductParams.biasParamMem + algLayer->layerParams.innerProductParams.biasParamSize);
    algLayer->layerParams.innerProductParams.matmulShiftMem = (void *)((uint8_t *)algLayer->layerParams.innerProductParams.matmulScaleMem + algLayer->layerParams.innerProductParams.matmulScaleMemSize);

    memcpy(algLayer->layerParams.innerProductParams.biasParamMem, mmav2_bias, algLayer->layerParams.innerProductParams.biasParamSize);
    memcpy(algLayer->layerParams.innerProductParams.matmulScaleMem, mmav2_Scales, algLayer->layerParams.innerProductParams.matmulScaleMemSize);
    memcpy(algLayer->layerParams.innerProductParams.matmulShiftMem, mmav2_Shifts, algLayer->layerParams.innerProductParams.matmulShiftMemSize);
  }

  {
    if(forceRef)
    {
      //copy the original weights and bias in alg layer buffer space
      void * src = get_int8_t_pointer((int8_t *)(commonParams->net), innerProductParams->weights);
      void *dst = algLayer->layerParams.innerProductParams.wtTranformMem;
      int32_t size = algLayer->layerParams.innerProductParams.wtTranformSize;
      if(size && (dst != NULL) && (src != NULL)) memcpy(dst,src,size );
    }
  }

  /* APIs calculate offset with regard to base ptr. In this situation, the device utils API would not
   * know that the memory has already been utilized. Thus we add an offset equal to the size of memory
   * space already allocated and remove the offset after the API has run so the caller function sees the
   * same memory.
   **/
  memory[TIDL_LAYER_MEMORY_PERSISTENT] += TIDL_ALIGN_CEIL(paramMemTabOffset, TIDL_ALIGNMENT_SIZE);

  if (((commonParams->createParams->flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) == 0) && (forceRef == 0))
  {
    if (commonParams->net->TIDLLayers[layerIdx].layerParams.innerProductParams.weights == 0)
    {
      status = TIDL_deviceUtilsCommonInit(layerSpecificParams,
                                        commonParams,
                                        algLayer,
                                        layerIdx,
                                        memory,
                                        memorySize,
                                        outPtr);

      algLayer->layerParams.innerProductParams.maxWorkloadLoopIter = TIDL_matmulDeviceGetWlLoopMaxIterWrap(layerSpecificParams, 
                                                                                        &commonParams->net->TIDLLayers[layerIdx], 
                                                                                        commonParams);
    }
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
    
    //TODO: Deprecate accMem in innerProduct since we have scratchData that performs same function
    algLayer->layerParams.innerProductParams.accMem     = algLayer->scratchMem;
    algLayer->layerParams.innerProductParams.accMemSize = algLayer->scratchSize;
  }

  memory[TIDL_LAYER_MEMORY_PERSISTENT] -= TIDL_ALIGN_CEIL(paramMemTabOffset, TIDL_ALIGNMENT_SIZE);
  memorySize[TIDL_LAYER_MEMORY_PERSISTENT] += TIDL_ALIGN_CEIL(paramMemTabOffset, TIDL_ALIGNMENT_SIZE);

  return status;
}

/**
----------------------------------------------------------------------------
@ingroup    TIDL_IVISION_SUPPORT
@fn         TIDL_innerProductProcessNew
@brief InnerProduct layer Process function

@param      commonParams   :  Pointer to common parameter required for all the layers
@param      algLayer       :  Pointer to layer specific internal parameters
@param      layerIdx       :  Layer Id of the layer
@param      memory         :  Allocated memory pointers
@param      memorySize     :  Output memory requirement of the layer
@param      outPtr         :  Output memory pointer. Applicable only for reference flow
@remarks    None
@return     IALG_EOK   - Successful
            IALG_EFAIL - Unspecified error
----------------------------------------------------------------------------
*/
int32_t TIDL_innerProductProcessNew(TIDL_NetworkCommonParams *commonParams,
                              sTIDL_AlgLayer_t     * algLayer,
                              sTIDL_Layer_t        * tidlLayer,
                              void                 * inPtrs[],
                              void                 * outPtrs[],
                              int32_t                layerIdx)

{
  sTIDL_InnerProductParams_t   * params =
  &tidlLayer->layerParams.innerProductParams;
  tidlInnerProductBuffParams_t buffParams;
  sTIDL_DataParams_t *inDataParams; /*Points to parameters of first non-const tensor*/
  sTIDL_DataParams_t *inBDataParams = NULL; /*Points to parameters of second (B) tensor*/
  inDataParams    = &commonParams->net->TIDLLayers[algLayer->inLayerIdx[0]].outData;

  int32_t layerType = commonParams->net->TIDLLayers[algLayer->inLayerIdx[0]].layerType;
  if (layerType == TIDL_ConstDataLayer)
  {
    inDataParams = &commonParams->net->TIDLLayers[algLayer->inLayerIdx[1]].outData;
  }

  if (tidlLayer->numInBufs > 1)
  {
    inBDataParams   = &commonParams->net->TIDLLayers[algLayer->inLayerIdx[1]].outData;
  }

  int32_t quantizationStyle = commonParams->net->quantizationStyle;
  uint16_t  outPitch          = (uint16_t)tidlLayer->outData.pitch[TIDL_LINE_PITCH];
  uint8_t   *inPtr            = (uint8_t*)inPtrs[0];
  uint8_t   *inBPtr           = (uint8_t*)inPtrs[1];
  uint8_t   *outPtr           = (uint8_t*)outPtrs[0];

  int32_t   status = 1;
  int32_t   inSize  = TIDL_getDatElementSize(inDataParams->elementType);
  int32_t   outSize = TIDL_getDatElementSize(tidlLayer->outData.elementType);
  int32_t   forceRef = 0;

  if(TIDL_isAsymQuantEnabledTFL(quantizationStyle))
  {
    buffParams.mmaShift = algLayer->layerParams.innerProductParams.mmaShift;
    buffParams.mmaScale = algLayer->layerParams.innerProductParams.mmaScale;
  }

  buffParams.inElementType  =  inDataParams->elementType;
  
  if (inBDataParams != NULL)
  {
    buffParams.inBElementType =  inBDataParams->elementType;
  }
  else
  {
    if(tidlLayer->weightsElementSizeInBits <= 8)
    {
      buffParams.inBElementType = TIDL_SignedChar;
    }
    else if(tidlLayer->weightsElementSizeInBits <= 16)
    {
      buffParams.inBElementType = TIDL_SignedShort;
    }
    else if (tidlLayer->weightsElementSizeInBits == 32)
    {
      buffParams.inBElementType = TIDL_SinglePrecFloat;
    }
  }
  
  
  if (inBDataParams != NULL)
  {
    buffParams.inBElementType =  inBDataParams->elementType;
  }
  else
  {
    if(tidlLayer->weightsElementSizeInBits <= 8)
    {
      buffParams.inBElementType = TIDL_SignedChar;
    }
    else if(tidlLayer->weightsElementSizeInBits <= 16)
    {
      buffParams.inBElementType = TIDL_SignedShort;
    }
    else if (tidlLayer->weightsElementSizeInBits == 32)
    {
      buffParams.inBElementType = TIDL_SinglePrecFloat;
    }
  }
  
  buffParams.scratchSize    =  algLayer->layerParams.innerProductParams.accMemSize;
  buffParams.scratchMem     =  algLayer->layerParams.innerProductParams.accMem;

  if (((commonParams->net->quantizationStyle == TIDL_QuantStyleNP2Fixed) ||
      (commonParams->net->quantizationStyle == TIDL_QuantStyleP2Dynamic) || !TIDL_isKernelHighPrecision(tidlLayer->layerKernelType)) &&
      commonParams->net->quantizationStyle != TIDL_QuantStyleAsymNP2_TFL)
  {
    if (params->weightScale != 0)
    {
      buffParams.inScaleFactor = (params->weightScale * inDataParams->tensorScale) / params->biasScale;
    }
    else
    {
      inDataParams = &commonParams->net->TIDLLayers[algLayer->inLayerIdx[0]].outData;
      inBDataParams = &commonParams->net->TIDLLayers[algLayer->inLayerIdx[1]].outData;
      buffParams.inScaleFactor = (inDataParams->tensorScale * inBDataParams->tensorScale) / params->biasScale;
    }
    buffParams.outRoundBits  = tidlLayer->outData.roundBits;
    if(params->biasScale == -1)
    {
      buffParams.inScaleFactor = 0;
    }
  }

  if(algLayer->layerParams.innerProductParams.wtTranformSize != 0)
  {
    /* NULL or not NULL to just indicate 1 or 2 tensor scenario */
    inBPtr  =  NULL;
  }

  outPtr  =  outPtr + ((tidlLayer->outData.padH*outPitch)  + tidlLayer->outData.padW) * outSize;

  forceRef = TIDL_forceInnerProductRef(commonParams->createParams, tidlLayer);
  if(forceRef || TIDL_referencFlow(commonParams->createParams) )
  {

    TIDL_Obj intAlgObj;
    TIDL_CreateParams createParams;

    memcpy(&createParams, commonParams->createParams, sizeof(TIDL_CreateParams));
    
    intAlgObj.createParams = (TIDL_CreateParams *) &createParams;
    tidl_printf(1,"Executing reference flow for inner product layer \n");
    /*Refrence code is common if there is two input or one input.
      In real projection kind of scenario, there are two inputs in all models except in caffe
      it is only one tensor input in projection kind of scenarion for caffe
    */
    status = TIDL_innerProductRefProcess(&intAlgObj,
                                          algLayer,
                                          tidlLayer,
                                          params,
                                          &buffParams,
                                          (void*)inPtr,
                                          (void*)inBPtr,
                                          (void*)outPtr);

  }
  else
  {
    if(algLayer->layerParams.innerProductParams.wtTranformMem != NULL)
    {
      status = TIDL_innerProductMMAProcess(commonParams,
                              algLayer,
                              tidlLayer,
                              (void*)(inPtr),
                              (void*)(algLayer->layerParams.innerProductParams.wtTranformMem),
                              (void*)(algLayer->layerParams.innerProductParams.biasParamMem),
                              (void*)(outPtr),
                              &buffParams);
      /* This call does not use standard WL flow, so need to handle layer level cache writeback explicitly in this layer */ 
      TIDL_enableL1DCacheWb();
    }
    else
    {
      status = TIDL_innerProductDeviceProcess(commonParams,
                                            algLayer,
                                            tidlLayer,
                                            inPtrs,
                                            outPtrs,
                                            layerIdx);
    }
  }
  return status;
}
