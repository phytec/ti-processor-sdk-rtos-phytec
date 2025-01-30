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
@file    tidl_pad.c
@brief   This file defines private functions for pad layer.
@version 0.1 (June 2019) : Initial version [ADK]
----------------------------------------------------------------------------
*/

#include "tidl_alg_int.h"
#include "tidl_pad.h"
#include "tidl_commonUtils.h"
/**
 * @brief This is reference implementation of Pad layer
 *
 * @tparam Tin : template for input data buffers
 * @tparam Tout : template for output data buffers
 * @param pIn : Pointer to input memory
 * @param pOut : Pointer to output memory
 * @param inPtrOffset : offset input buffer pointer
 * @param outPtrOffset : offset output buffer pointer
 * @param outWidth : width of the output buffer
 * @param outHeight : height of the output buffer
 * @param numChs : no of total channels
 * @param numROIs : no of total ROI
 * @param inLinePitch : pitch of the input buffer
 * @param outLinePitch : pitch of the output buffer
 * @param inChPitch : Chnnel pitch of the input buffer
 * @param outChPitch : Chnnel pitch of the output buffer
 * @param inROIPitch : ROI pitch of the input buffer
 * @param outROIPitch : ROI pitch of the output buffer
 * @param padT : Pad amount on the top side
 * @param padB : Pad amount on the bottom side
 * @param padL : Pad amount on the left side
 * @param padR : Pad amount on the right side
 * @param padValue : Pad value to be filled
 * @param padType : type of Padding to be done
 * @param padPerChannelPtr : pointer to per channel buffer
 * @param tensorScaleInput : Scale amount for the input
 */
template <class Tin, class Tout>
void TIDL_refPad(
  const Tin*    pIn,
  Tout*   pOut,
  int32_t inPtrOffset,
  int32_t outPtrOffset,
  int32_t outWidth,
  int32_t outHeight,
  int32_t numChs,
  int32_t numROIs,
  int32_t inLinePitch,
  int32_t outLinePitch,
  int32_t inChPitch,
  int32_t outChPitch,
  int32_t inROIPitch,
  int32_t outROIPitch,
  int32_t padT,
  int32_t padB,
  int32_t padL,
  int32_t padR,
  int32_t padValue,
  int32_t padType,
  float32_tidl * padPerChannelPtr,
  float32_tidl tensorScaleInput
  )
{
  int32_t i0, i1, i2, i3;
  Tin padValueFinal = padValue;

  //OPENACC(data copyin(padPerChannelPtr[0:numChs], pIn[0: 1 + inPtrOffset  + (numROIs*inROIPitch)   + (numChs* inChPitch)   + ((outHeight - padT) * inLinePitch)   + (outWidth - padL)]) \
               copyout(pOut[0: 1 + outPtrOffset + (numROIs*outROIPitch)  + (numChs* outChPitch)  + (outHeight * outLinePitch)  + outWidth]))
  //OPENACC(parallel loop collapse(2))
  for (i0 = 0; i0 < numROIs; i0++)
  {
    for (i1 = 0; i1 < numChs; i1++)
    {
      if(padType == TIDL_PadPerChannel)
      {
        padValueFinal = padPerChannelPtr[i1] * tensorScaleInput;
      }
      //OPENACC(loop collapse(2))
      for (i2 = 0; i2 < outHeight; i2++)
      {
        for (i3 = 0; i3 < outWidth; i3++)
        {
          int32_t outOffset = outPtrOffset + (i0*outROIPitch)  + (i1* outChPitch)  + (i2 * outLinePitch)  + i3;
          if((i3 < padL) || (i3 >= (outWidth - padR)) || (i2 < padT) || (i2 >= (outHeight - padB)))
          {
            pOut[outOffset] = padValueFinal;
          }
          else
          {
            int32_t inOffset  = inPtrOffset  + (i0*inROIPitch)   + (i1* inChPitch)   + ((i2 - padT) * inLinePitch)   + (i3 - padL);
            pOut[outOffset] = pIn[inOffset];
          }
        }
      }
    }
  }
  TIDL_enableL1DandL2CacheWb();
}

/**
 * @brief This This function is only used to do non zero constant padding
 *        in NATC mode, tensor data is copied to output using DMA
 *
 * @tparam Tin : template for input data buffers
 * @tparam Tout : template for output data buffers
 * @param pIn : Pointer to input memory
 * @param pOut : Pointer to output memory
 * @param inPtrOffset : offset input buffer pointer
 * @param outPtrOffset : offset output buffer pointer
 * @param outWidth : width of the output buffer
 * @param outHeight : height of the output buffer
 * @param numChs : no of total channels
 * @param numROIs : no of total ROI
 * @param inLinePitch : pitch of the input buffer
 * @param outLinePitch : pitch of the output buffer
 * @param inChPitch : Chnnel pitch of the input buffer
 * @param outChPitch : Chnnel pitch of the output buffer
 * @param inROIPitch : ROI pitch of the input buffer
 * @param outROIPitch : ROI pitch of the output buffer
 * @param padT : Pad amount on the top side
 * @param padB : Pad amount on the bottom side
 * @param padL : Pad amount on the left side
 * @param padR : Pad amount on the right side
 * @param padValue : Pad value to be filled
 * @param padType : type of Padding to be done
 * @param padPerChannelPtr : pointer to per channel buffer
 * @param tensorScaleInput : Scale amount for the input
 */
template <class Tin, class Tout>
void TIDL_refPadNATC(
  const Tin*    pIn,
  Tout*   pOut,
  int32_t inPtrOffset,
  int32_t outPtrOffset,
  int32_t outWidth,
  int32_t outHeight,
  int32_t numChs,
  int32_t numROIs,
  int32_t inLinePitch,
  int32_t outLinePitch,
  int32_t inChPitch,
  int32_t outChPitch,
  int32_t inROIPitch,
  int32_t outROIPitch,
  int32_t padT,
  int32_t padB,
  int32_t padL,
  int32_t padR,
  int32_t padValue,
  int32_t padType,
  float32_tidl * padPerChannelPtr,
  float32_tidl tensorScaleInput
  )
{
  int32_t i0, i1, i2, i3;
  Tin padValueFinal = padValue;

  //OPENACC(data copyin(padPerChannelPtr[0:numChs]) \
               copyout(pOut[0:1 + outPtrOffset + (numROIs*outROIPitch)  + (numChs* outChPitch)  + (outHeight * outLinePitch)  + outWidth]))
  //OPENACC(parallel loop collapse(2))
  for (i0 = 0; i0 < numROIs; i0++)
  {
    for (i1 = 0; i1 < numChs; i1++)
    {
      if(padType == TIDL_PadPerChannel)
      {
        padValueFinal = padPerChannelPtr[i1] * tensorScaleInput;
      }
      //OPENACC(loop collapse(2))
      for (i2 = 0; i2 < outHeight; i2++)
      {
        for (i3 = 0; i3 < outWidth; i3++)
        {
          int32_t outOffset = outPtrOffset + (i0*outROIPitch)  + (i1* outChPitch)  + (i2 * outLinePitch)  + i3;
          if((i3 < padL) || (i3 >= (outWidth - padR)) || (i2 < padT) || (i2 >= (outHeight - padB)))
          {
            pOut[outOffset] = padValueFinal;
          }
        }
      }
    }
  }
}

int32_t TIDL_padAllocNew(const TIDL_LayerSpecificParams *layerSpecificParams,
                                   const TIDL_NetworkCommonParams *commonParams,
                                   int32_t layerIdx,
                                   int32_t memorySize[TIDL_LAYER_MEMORY_MAX])
{
  int32_t status = IALG_EOK;

  int32_t outDataSize = TIDL_updateDataParamsPitch(&commonParams->net->TIDLLayers[layerIdx].outData);
  sTIDL_PadLayerParams_t *layerParams = &commonParams->net->TIDLLayers[layerIdx].layerParams.padLayerParams;
  int32_t paramMemSize = layerParams->perChannelPadConstTensorSize;

  memorySize[TIDL_LAYER_MEMORY_SCRATCH]    += 0;
  memorySize[TIDL_LAYER_MEMORY_PERSISTENT] += ((uint32_t)paramMemSize + 128U);
  memorySize[TIDL_LAYER_MEMORY_OUTPUT]     += ((uint32_t)outDataSize + 128U);

  return status;
}

int32_t TIDL_padInitNew(const TIDL_LayerSpecificParams *layerSpecificParams,
                                const TIDL_NetworkCommonParams  *commonParams,
                                sTIDL_AlgLayer_t               *algLayer,
                                int32_t layerIdx,
                                uint8_t *memory[TIDL_LAYER_MEMORY_MAX],
                                int32_t memorySize[TIDL_LAYER_MEMORY_MAX],
                                void    **outPtr)
{
  int32_t status = IALG_EOK;

  int32_t outDataSize = TIDL_updateDataParamsPitch(&commonParams->net->TIDLLayers[layerIdx].outData);
  int32_t currOffset = 0;

  TIDL_AllocatePtr((intptr_t)memory[TIDL_LAYER_MEMORY_OUTPUT],
                    &currOffset,
                    outDataSize,
                    TIDL_ALIGNMENT_SIZE,
                    outPtr);

  sTIDL_PadLayerParams_t *layerParams = &commonParams->net->TIDLLayers[layerIdx].layerParams.padLayerParams;
  int32_t paramMemSize = layerParams->perChannelPadConstTensorSize;
  int32_t paramOffset = layerParams->perChannelPadConstTensorOffset;
  void *dst;
  currOffset = 0;

  TIDL_AllocatePtr((intptr_t)memory[TIDL_LAYER_MEMORY_PERSISTENT],
                    &currOffset,
                    paramMemSize,
                    TIDL_ALIGNMENT_SIZE,
                    &dst);

  sTIDL_AlgPadParams_t* algPadParams = &algLayer->layerParams.padParams;
  algPadParams->perChannelPadConstTensorPtr = dst;
  algPadParams->perChannelPadConstTensorSize = paramMemSize;


  memorySize[TIDL_LAYER_MEMORY_SCRATCH]    += 0;
  memorySize[TIDL_LAYER_MEMORY_PERSISTENT] += ((uint32_t)paramMemSize + 128U);
  memorySize[TIDL_LAYER_MEMORY_OUTPUT]     += ((uint32_t)outDataSize + 128U);

  if(paramMemSize && paramOffset){
    void * src   = ((int8_t *)(commonParams->createParams->net) + paramOffset);
    memcpy(dst, src, paramMemSize);
  }

  return status;
}


int32_t TIDL_padProcessNew(TIDL_NetworkCommonParams *commonParams,
                                  sTIDL_AlgLayer_t     * algLayer,
                                  sTIDL_Layer_t        * tidlLayer,
                                  void                 * inPtrs[],
                                  void                 * outPtrs[],
                                  int32_t                layerIdx)
{
  sTIDL_DataParams_t *inDataParams   = TIDL_getDataParams(commonParams->createParams->net,
  tidlLayer->inData[0]);
  int32_t outWidth           = tidlLayer->outData.dimValues[TIDL_DIM_WIDTH];
  int32_t outHeight          = tidlLayer->outData.dimValues[TIDL_DIM_HEIGHT];
  int32_t inChs              = inDataParams->dimValues[TIDL_DIM_NUMCH];
  int32_t numROIs            = inDataParams->dimValues[TIDL_DIM_BATCH]; /*ROI  */
  int32_t inPitch            = (int32_t)inDataParams->pitch[TIDL_LINE_PITCH];
  int32_t outPitch           = (int32_t)tidlLayer->outData.pitch[TIDL_LINE_PITCH];
  int32_t inChPitch          = (int32_t)inDataParams->pitch[TIDL_CHANNEL_PITCH];
  int32_t outChPitch          = (int32_t)tidlLayer->outData.pitch[TIDL_CHANNEL_PITCH];
  int32_t status             = IALG_EOK;
  int32_t inROIPitch         = (int32_t)inDataParams->pitch[TIDL_ROI_PITCH];
  int32_t outROIPitch        = (int32_t)tidlLayer->outData.pitch[TIDL_ROI_PITCH];
  float32_tidl tensorScaleInput = inDataParams->tensorScale;
  sTIDL_AlgPadParams_t* algPadParams = &algLayer->layerParams.padParams;
  sTIDL_PadLayerParams_t *params = &tidlLayer->layerParams.padLayerParams;
  int32_t padT  =  params->padT;
  int32_t padB  =  params->padB;
  int32_t padL  =  params->padL;
  int32_t padR  =  params->padR;
  int32_t padType = params->padType;
  int32_t padConstValue = 0;

  if(params->padType == TIDL_PadZero)
  {
    padConstValue = 0;
  }
  else if (params->padType == TIDL_PadNonZeroConstant)
  {
    padConstValue   = params->padConstValue;
  }

  int32_t inPtrOffset        = 0;
  int32_t outPtrOffset       = 0;

  float32_tidl * perChannelPadValPtr = (float32_tidl *)((int8_t *)algPadParams->perChannelPadConstTensorPtr);

  if((commonParams->createParams->flowCtrl & TIDL_FLOW_CTRL_REF_STAT) == TIDL_FLOW_CTRL_REF_STAT)
  {
    TIDL_Obj intAlgObj;
    TIDL_CreateParams createParams;
    memcpy(&createParams, commonParams->createParams, sizeof(TIDL_CreateParams));
    intAlgObj.createParams = (TIDL_CreateParams *) &createParams;

    TIDL_UpdateScaleFactors(&intAlgObj, layerIdx, 0, 0, 1);
  }

  /* Non zero padding for NATC flow done using reference code, zero padding done using DMA */
  if(((commonParams->createParams->flowCtrl & TIDL_FLOW_CTRL_DSP_NATC) == TIDL_FLOW_CTRL_DSP_NATC) && (params->padType != TIDL_PadZero))
  {
    if (inDataParams->elementType == TIDL_SinglePrecFloat)
    {
      TIDL_refPadNATC((float32_tidl *)inPtrs[0], (float32_tidl *)outPtrs[0], inPtrOffset, outPtrOffset, outWidth, outHeight, inChs, numROIs, inPitch,
      outPitch, inChPitch, outChPitch, inROIPitch, outROIPitch, padT, padB, padL, padR, padConstValue, padType, perChannelPadValPtr, tensorScaleInput);
    }
    else if ((inDataParams->elementType == TIDL_SignedChar))
    {
      TIDL_refPadNATC((int8_t *)inPtrs[0], (int8_t *)outPtrs[0], inPtrOffset, outPtrOffset, outWidth, outHeight, inChs, numROIs, inPitch,
      outPitch, inChPitch, outChPitch, inROIPitch, outROIPitch, padT, padB, padL, padR, padConstValue, padType, perChannelPadValPtr, tensorScaleInput);
    }
    else if ((inDataParams->elementType == TIDL_UnsignedChar))
    {
      TIDL_refPadNATC((uint8_t *)inPtrs[0], (uint8_t *)outPtrs[0], inPtrOffset, outPtrOffset, outWidth, outHeight, inChs, numROIs, inPitch,
      outPitch, inChPitch, outChPitch, inROIPitch, outROIPitch, padT, padB, padL, padR, padConstValue, padType, perChannelPadValPtr, tensorScaleInput);
    }
    else if ((inDataParams->elementType == TIDL_SignedShort))
    {
      TIDL_refPadNATC((int16_t *)inPtrs[0], (int16_t *)outPtrs[0], inPtrOffset, outPtrOffset, outWidth, outHeight, inChs, numROIs, inPitch,
      outPitch, inChPitch, outChPitch, inROIPitch, outROIPitch, padT, padB, padL, padR, padConstValue, padType, perChannelPadValPtr, tensorScaleInput);
    }
    else if ((inDataParams->elementType == TIDL_UnsignedShort))
    {
      TIDL_refPadNATC((uint16_t *)inPtrs[0], (uint16_t *)outPtrs[0], inPtrOffset, outPtrOffset, outWidth, outHeight, inChs, numROIs, inPitch,
      outPitch, inChPitch, outChPitch, inROIPitch, outROIPitch, padT, padB, padL, padR, padConstValue, padType, perChannelPadValPtr, tensorScaleInput);
    }
  }

  if ((commonParams->createParams->flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) != TIDL_FLOW_CTRL_REF_ONLY)
  {
    status = TIDL_padDspProcessNew(commonParams, algLayer, tidlLayer, inPtrs, outPtrs, commonParams->tidlCommonParams->sysMems);
    /* This layer does not use standard WL flow, so need to handle layer level cache writeback explicitly in this layer */ 
    TIDL_enableL1DCacheWb();
  }
  else
  {
    if (inDataParams->elementType == TIDL_SinglePrecFloat)
    {
      TIDL_refPad((float32_tidl *)inPtrs[0], (float32_tidl *)outPtrs[0], inPtrOffset, outPtrOffset, outWidth, outHeight, inChs, numROIs, inPitch,
      outPitch, inChPitch, outChPitch, inROIPitch, outROIPitch, padT, padB, padL, padR, padConstValue, padType, perChannelPadValPtr, tensorScaleInput);
    }
    else if ((inDataParams->elementType == TIDL_SignedChar))
    {
      TIDL_refPad((int8_t *)inPtrs[0], (int8_t *)outPtrs[0], inPtrOffset, outPtrOffset, outWidth, outHeight, inChs, numROIs, inPitch,
      outPitch, inChPitch, outChPitch, inROIPitch, outROIPitch, padT, padB, padL, padR, padConstValue, padType, perChannelPadValPtr, tensorScaleInput);
    }
    else if ((inDataParams->elementType == TIDL_UnsignedChar))
    {
      TIDL_refPad((uint8_t *)inPtrs[0], (uint8_t *)outPtrs[0], inPtrOffset, outPtrOffset, outWidth, outHeight, inChs, numROIs, inPitch,
      outPitch, inChPitch, outChPitch, inROIPitch, outROIPitch, padT, padB, padL, padR, padConstValue, padType, perChannelPadValPtr, tensorScaleInput);
    }
    else if ((inDataParams->elementType == TIDL_SignedShort))
    {
      TIDL_refPad((int16_t *)inPtrs[0], (int16_t *)outPtrs[0], inPtrOffset, outPtrOffset, outWidth, outHeight, inChs, numROIs, inPitch,
      outPitch, inChPitch, outChPitch, inROIPitch, outROIPitch, padT, padB, padL, padR, padConstValue, padType, perChannelPadValPtr, tensorScaleInput);
    }
    else if ((inDataParams->elementType == TIDL_UnsignedShort))
    {
      TIDL_refPad((uint16_t *)inPtrs[0], (uint16_t *)outPtrs[0], inPtrOffset, outPtrOffset, outWidth, outHeight, inChs, numROIs, inPitch,
      outPitch, inChPitch, outChPitch, inROIPitch, outROIPitch, padT, padB, padL, padR, padConstValue, padType, perChannelPadValPtr, tensorScaleInput);
    }
  }

  return status;
}


