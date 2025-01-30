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
@file    tidl_resize.c
@brief   This file defines private functions for resize layer.
@version 0.1 (Aug 2019) : Initial version
----------------------------------------------------------------------------
*/

#include <math.h>
#include "tidl_resize.h"
#include "float.h"
#include "tidl_commonUtils.h"
#include "tidl_priv_algo.h"
#include "tidl_device_functions.h"
#include "gc_helper.h"

#define ENABLE_NUMCONSUMERS_FROM_GC (0)

/**
 * @brief This is reference implementation of Resize layer
 *
 * @tparam Tin : template for input data buffers
 * @param pIn : Pointer to input memory
 * @param pOut : Pointer to output memory
 * @param algLayer : Pointer to the layer specific parameters
 * @param tidlLayer : Pointer to the common layer parameters
 * @param params : Copy of Resize layer parameters
 * @param inDataParams : parameters of the input data buffer
 */

void getResizeInfo(sTIDL_ResizeInfo_t* resizeInfo, sTIDL_AlgLayer_t* algLayer, const sTIDL_Layer_t * tidlLayer, const TIDL_CreateParams *createParams, int32_t resizeInHeight)
{
  int32_t copyLineInfo = TIDL_COPY_NO_LINE;
  int32_t inputHeight = resizeInHeight;

  if(algLayer->wlPadParams.tileType != NO_TILE || createParams->net->inferenceMode == TIDL_inferenceModeLowLatency) /*super Tiling or Multi-Core*/
  {
    /*
    *-------------------------Copy Top Line:---------------------------*
    1. layer processed in single core
      i) ST -> first tile ii) Non-ST -> always
    2. Layer Processed across multiple cores
      ii) ST -> First Tile + first core ii) Non-ST -> First Core
    *-------------------------Copy Bottom Line:-------------------------*
    1. layer processed in single core
      i) ST -> Last Tile tile ii) Non-ST -> always
    2. Layer Processed across multiple cores
      i) ST -> Last Tile + Last core ii) Non-ST -> Last Core
    */
    if((/*1*/(tidlLayer->multiCoreMode == TIDL_NOT_MULTI_CORE) && 
        (/* i*/((algLayer->wlPadParams.tileType != NO_TILE) && (algLayer->wlPadParams.isFirstTile == 1) && (algLayer->wlRepeatIter == 0)) || 
         /*ii*/(algLayer->wlPadParams.tileType == NO_TILE)
        )
       )
      || 
       (/*2*/(tidlLayer->multiCoreMode == TIDL_MULTI_CORE) &&
        (/* i*/((algLayer->wlPadParams.tileType != NO_TILE) && (createParams->coreId == createParams->coreStartIdx) && (algLayer->wlPadParams.isFirstTile == 1) && (algLayer->wlRepeatIter == 0)) ||
         /*ii*/((algLayer->wlPadParams.tileType == NO_TILE) && (createParams->coreId == createParams->coreStartIdx))
        )
       )
      )
    {
      copyLineInfo |= TIDL_COPY_TOP_LINE;
    }
    /*Relook Copy_Bottom_Line condition*/
    if((/*1*/(tidlLayer->multiCoreMode == TIDL_NOT_MULTI_CORE) && 
        (/* i */  ((algLayer->wlPadParams.tileType != NO_TILE) && (algLayer->wlPadParams.isLastTile == 1) && (algLayer->wlRepeatIter == 0))  || 
          /*ii*/  (algLayer->wlPadParams.tileType == NO_TILE)  
        )
       )
      ||
       ((/*2*/(tidlLayer->multiCoreMode == TIDL_MULTI_CORE) && 
         (/*i*/  ((algLayer->wlPadParams.tileType != NO_TILE) && (algLayer->wlPadParams.isLastTile == 1) && (algLayer->wlRepeatIter == 0) && 
                    (createParams->coreId == (GET_NUMCORES_FOR_STARTCOREIDX(createParams->net->numCores, createParams->coreStartIdx) - 1))) ||
          /*ii*/ (algLayer->wlPadParams.tileType == NO_TILE) && (createParams->coreId == (GET_NUMCORES_FOR_STARTCOREIDX(createParams->net->numCores, createParams->coreStartIdx) - 1)) 
         ) 
        )
       )
      )
    {
      copyLineInfo |= TIDL_COPY_BOTTOM_LINE;
    }
  }
  else
  {
    copyLineInfo = TIDL_COPY_TOP_LINE | TIDL_COPY_BOTTOM_LINE;
  }

  resizeInfo->copyLineInfo = copyLineInfo;
  resizeInfo->inputHeight = inputHeight;

  return;
}

template <class Tin>
void TIDL_refResize(
    Tin *pIn,
    Tin *pOut,
    sTIDL_AlgLayer_t *algLayer,
    sTIDL_Layer_t *tidlLayer,
    sTIDL_ResizeLayerParams_t *params,
    sTIDL_DataParams_t *inDataParams,
    const TIDL_CreateParams *createParams)
{
  uint32_t numInChannels        = (uint16_t)inDataParams->dimValues[TIDL_DIM_NUMCH];
  uint32_t inWidth              = (uint16_t)inDataParams->dimValues[TIDL_DIM_WIDTH];
  uint32_t inHeight             = (uint16_t)inDataParams->dimValues[TIDL_DIM_HEIGHT];
  uint32_t inPitch              = (uint16_t)inDataParams->pitch[TIDL_LINE_PITCH];
  uint32_t inChPitch            = (uint32_t)inDataParams->pitch[TIDL_CHANNEL_PITCH];
  uint32_t outPitch             = (uint16_t)tidlLayer->outData.pitch[TIDL_LINE_PITCH];
  uint32_t outChPitch           = (uint32_t)tidlLayer->outData.pitch[TIDL_CHANNEL_PITCH];
  uint32_t outWidth             = (uint16_t)tidlLayer->outData.dimValues[TIDL_DIM_WIDTH];
  uint32_t outHeight            = (uint16_t)tidlLayer->outData.dimValues[TIDL_DIM_HEIGHT];
  int32_t  numBatches              = (int32_t)inDataParams->dimValues[TIDL_DIM_BATCH];
  uint32_t inBatchPitch           = (uint32_t)inDataParams->pitch[TIDL_ROI_PITCH];
  uint32_t outBatchPitch          = (uint32_t)tidlLayer->outData.pitch[TIDL_ROI_PITCH];

  float32_tidl wRatio ;
  float32_tidl hRatio ;

  int32_t widthResizeRatio  = params->resizeRatio[TIDL_DIM_WIDTH];
  int32_t heightResizeRatio = params->resizeRatio[TIDL_DIM_HEIGHT];

  if (params->resizeRatio[TIDL_DIM_WIDTH] > 0)
  {
    wRatio = 1 / params->resizeRatio[TIDL_DIM_WIDTH];
  }
  else
  {
    wRatio = 0;
  }
  if (params->resizeRatio[TIDL_DIM_HEIGHT] > 0)
  {
    hRatio = 1 / params->resizeRatio[TIDL_DIM_HEIGHT];
  }
  else
  {
    hRatio = 0;
  }

  OPENACC(data copyin(pIn[0: 1+((numBatches-1) * inBatchPitch + (numInChannels-1) * inChPitch + (inPitch * (inHeight-1) + (inWidth-1)))])
               copy(pOut[0: 1+((numBatches-1) * outBatchPitch + (numInChannels-1) * outChPitch + (outPitch * (outHeight-1) + (outWidth-1)))]))
{
  if (params->mode == TIDL_ResizeNearest)
  {
    int i = 0;
    int j = 0;
    int k = 0;
    int l = 0;
    float32_tidl hLoc, wLoc;
    int hIdx, wIdx;

    OPENACC(parallel loop collapse(4))
    for(l=0; l<numBatches; l++)
    {
    for (k = 0; k < numInChannels; k++)
    {
      for (i = 0; i < outHeight; i++)
      {
        for (j = 0; j < outWidth; j++)
      {
          hLoc = hRatio * (i + 0.5) - 0.5;
          hLoc = (hLoc < 0) ? 0 : hLoc;
          hIdx = hLoc + 0.5;
          hIdx = (hIdx < inHeight) ? hIdx : (inHeight - 1);

          wLoc = wRatio * (j + 0.5) - 0.5;
          wLoc = (wLoc < 0) ? 0 : wLoc;
          wIdx = wLoc + 0.5;
          wIdx = (wIdx < inWidth) ? wIdx : inWidth - 1;
          int32_t inputOffset = inPitch * hIdx + wIdx;
          int32_t outputOffset = outPitch * i + j;

          pOut[l * outBatchPitch + k * outChPitch + outputOffset] = pIn[l * inBatchPitch + k * inChPitch + inputOffset];
          //*(pOut + l * outBatchPitch + k * outChPitch + outputOffset) = *(pIn + l * inBatchPitch + k * inChPitch + inputOffset);
        }
      }
    }
    }
  }
  else
  if (params->mode == TIDL_ResizeBilinear)
  {
    int i = 0;
    int j = 0;
    int k = 0;
    int l = 0;
    float32_tidl hLoc, wLoc;
    int hIdx, wIdx, hNext, wNext;
    float32_tidl w00, w01, w10, w11;
    int32_t enableHClip = 0;

    /* Horizontal clipping should not be done while processing different proc sizes */
    if ((algLayer->wlPadParams.isFirstTile == 1) &&
        (algLayer->wlRepeatIter == 0)            &&
        ((tidlLayer->multiCoreMode == TIDL_NOT_MULTI_CORE) || (createParams->coreId == createParams->coreStartIdx))
       )
    {
      enableHClip = 1;
    }

    OPENACC(parallel loop collapse(4))
    for(l=0; l<numBatches; l++)
    {
    for (k = 0; k < numInChannels; k++)
    {
      for (i = 0; i < outHeight; i++)
      {
        for (j = 0; j < outWidth; j++)
        {

          hLoc = hRatio * (i + 0.5) - 0.5;
          if(1 == enableHClip)
          {
            hLoc = (hLoc < 0) ? 0 : hLoc;
          }
          if(hLoc < 0)
          {
            hIdx = floor(hLoc);
          }
          else
          {
            hIdx = hLoc;
          }
          hNext = 1; /* By default, we are at current line and hNext = 1 corresponds to next line */
          w11 = hLoc - hIdx;
          w10 = 1 - w11;

          wLoc = wRatio * (j + 0.5) - 0.5;
          wLoc = (wLoc < 0) ? 0 : wLoc;
          wIdx = wLoc;
          w01 = wLoc - wIdx;
          w00 = 1 - w01;
          int32_t inputOffset = inPitch * hIdx + wIdx;
          int32_t inputOffset2 = inputOffset;
          int32_t outputOffset = outPitch * (i) + (j);
          
          wNext = (wIdx < (inWidth - 1)) ? 1 : 0;
          /* Calculate the offset for second line differently when horizontal clipping is disabled */
          if(inputOffset < 0)
          {
            inputOffset2 = wIdx;
            /* pIn corresponds to valid data (i.e. excludes top padding). inputOffset < 0 indicates we are at first valid line and need to start from top padding region
            to generate first output line for resize
            In this context, i00 and i01 calculated below correspond to top padding region. Next line for resize is the first valid line which corresponds to current ptr pIn
            so hNext must be 0 for i10 and i11 calculation */
            hNext = 0; 
          }

          Tin i00 = *(pIn + l * inBatchPitch + k * inChPitch + inputOffset);
          Tin i01 = *(pIn + l * inBatchPitch + k * inChPitch + inputOffset + wNext);
          Tin i10 = *(pIn + l * inBatchPitch + k * inChPitch + inputOffset2 + hNext * inPitch);
          Tin i11 = *(pIn + l * inBatchPitch + k * inChPitch + inputOffset2 + hNext * inPitch + wNext);

          int32_t inter1, inter2;
          uint8_t w00Int, w01Int;
          uint8_t w10Int, w11Int;

          if ((params->resizeRatio[TIDL_DIM_HEIGHT] == params->resizeRatio[TIDL_DIM_WIDTH]) && ((params->resizeRatio[TIDL_DIM_HEIGHT] == 2.0)|| (params->resizeRatio[TIDL_DIM_HEIGHT] == 4.0)))
          {
            if(params->resizeRatio[TIDL_DIM_HEIGHT] == 4)
            {
              heightResizeRatio = 3;
              widthResizeRatio  = 3;
            }
            else
            {
              heightResizeRatio = 2;
              widthResizeRatio  = 2;
            }
            w00Int = w00 * (1 << widthResizeRatio);
            w01Int = (1 << widthResizeRatio) - w00Int;
            w10Int = w10 * (1 << heightResizeRatio);
            w11Int = (1 << heightResizeRatio) - w10Int;
            inter1 = (i00 * w00Int + i01 * w01Int) ;
            inter2 = (i10 * w00Int + i11 * w01Int) ;
            // *(pOut + l * outBatchPitch + k * outChPitch + outputOffset) = (w10Int * inter1 + w11Int * inter2) >> (heightResizeRatio + widthResizeRatio);
            pOut[l * outBatchPitch + k * outChPitch + outputOffset] = (w10Int * inter1 + w11Int * inter2) >> (heightResizeRatio + widthResizeRatio);

          }
          else
          {
            pOut[l * outBatchPitch + k * outChPitch + outputOffset] = (w10 * (i00 * w00 + i01 * w01) + w11 * (i10 * w00 + i11 * w01));
            //*(pOut + l * outBatchPitch + k * outChPitch + outputOffset) = (w10 * (i00 * w00 + i01 * w01) + w11 * (i10 * w00 + i11 * w01));
          }
        }
      }
    }
    }
  }
  else
  {
    tidl_printf(0, "params->mode is  Not supported !!!\n ");
    assert(0);
  }
  }
}

/**
 * @brief This is reference implementation of Resize layer
 *
 * @param tidlLayer : Pointer to the common layer parameters
 * @param inPtrs : Pointer to input memory
 * @param outPtr : Pointer to output memory
 * @param inDataParams : parameters of the input data buffer
 * @param outDataParams : parameters of the output data buffer
 * @param basePrmPtr : Copy of Resize layer parameters
 */
int32_t TIDL_resizeProcessSP(
  sTIDL_Layer_t        * tidlLayer,
  void                 * inPtrs[],
  void                 * outPtr,
  sTIDL_DataParams_t   * inDataParams[],
  sTIDL_DataParams_t   * outDataParams,
  uint8_t              * basePrmPtr
  )
{
  sTIDL_ResizeLayerParams_t *params = &tidlLayer->layerParams.resizeParams;
  float32_tidl *in   = (float32_tidl*)inPtrs[0];
  float32_tidl *out  = (float32_tidl*)outPtr;

  uint32_t numInChannels        = (uint16_t)inDataParams[0]->dimValues[TIDL_DIM_NUMCH];
  uint32_t inWidth              = (uint16_t)inDataParams[0]->dimValues[TIDL_DIM_WIDTH];
  uint32_t inHeight             = (uint16_t)inDataParams[0]->dimValues[TIDL_DIM_HEIGHT];
  uint32_t inPitch              = (uint16_t)inDataParams[0]->pitch[TIDL_LINE_PITCH];
  uint32_t inChPitch            = (uint32_t)inDataParams[0]->pitch[TIDL_CHANNEL_PITCH];
  uint32_t outPitch             = (uint16_t)tidlLayer->outData.pitch[TIDL_LINE_PITCH];
  uint32_t outChPitch           = (uint32_t)tidlLayer->outData.pitch[TIDL_CHANNEL_PITCH];
  uint32_t outWidth             = (uint16_t)tidlLayer->outData.dimValues[TIDL_DIM_WIDTH];
  uint32_t outHeight            = (uint16_t)tidlLayer->outData.dimValues[TIDL_DIM_HEIGHT];
  int32_t  numBatches              = (int32_t)inDataParams[0]->dimValues[TIDL_DIM_BATCH];
  uint32_t inBatchPitch           = (uint32_t)inDataParams[0]->pitch[TIDL_ROI_PITCH];
  uint32_t outBatchPitch          = (uint32_t)tidlLayer->outData.pitch[TIDL_ROI_PITCH];

  int32_t inOffset = (1) * inPitch + 1;
  int32_t outOffset = 0;

  float32_tidl wRatio ;
  float32_tidl hRatio ;

  if (params->resizeRatio[TIDL_DIM_WIDTH] > 0)
  {
    wRatio = 1 / params->resizeRatio[TIDL_DIM_WIDTH];
  }
  else
  {
    wRatio = 0;
  }
  if (params->resizeRatio[TIDL_DIM_HEIGHT] > 0)
  {
    hRatio = 1 / params->resizeRatio[TIDL_DIM_HEIGHT];
  }
  else
  {
    hRatio = 0;
  }

  OPENACC(data copyin(in[0: 1+((numBatches-1) * inBatchPitch + (numInChannels-1) * inChPitch + (inPitch * (inHeight-1) + (inWidth-1)))])
               copyout(out[0: 1+((numBatches-1) * outBatchPitch + (numInChannels-1) * outChPitch + (outPitch * (outHeight-1) + (outWidth-1)))]))
{
  if (params->mode == TIDL_ResizeNearest)
  {
    int i = 0;
    int j = 0;
    int k = 0;
    int l = 0;
    float32_tidl hLoc, wLoc;
    int hIdx, wIdx;

   OPENACC(parallel loop collapse(4))
    for (l = 0; l < numBatches; l++)
    {
    for (i = 0; i < outHeight; i++)
    {
      for (j = 0; j < outWidth; j++)
      {
        for (k = 0; k < numInChannels; k++)
      {
        hLoc = hRatio * (i + 0.5) - 0.5;
        hLoc = (hLoc < 0) ? 0 : hLoc;
        hIdx = hLoc+0.5;
        hIdx = (hIdx < inHeight) ? hIdx : (inHeight-1);

        wLoc = wRatio * (j + 0.5) - 0.5;
        wLoc = (wLoc < 0) ? 0 : wLoc;
        wIdx = wLoc+0.5;
        wIdx = (wIdx < inWidth) ? wIdx : inWidth-1;
        int32_t inputOffset = inOffset + inPitch  * hIdx + wIdx;
        int32_t outputOffset = outOffset + outPitch * i + j;

        out[l*outBatchPitch + k*outChPitch + outputOffset] = in[l*inBatchPitch + k*inChPitch + inputOffset];
        //*(out + l*outBatchPitch + k*outChPitch + outputOffset) = *(in + l*inBatchPitch + k*inChPitch + inputOffset);
        }
      }
    }
    }
  }
  else
  if (params->mode == TIDL_ResizeBilinear)
  {
    int i = 0;
    int j = 0;
    int k = 0;
    int l = 0;
    float32_tidl hLoc, wLoc;
    int hIdx, wIdx, hNext, wNext;
    float32_tidl w00, w01, w10, w11;

    OPENACC(parallel loop collapse(4))
    for (l = 0; l < numBatches; l++)
    {
    for (i = 0; i < outHeight; i++)
    {
      for (j = 0; j < outWidth; j++)
      {
        for (k = 0; k < numInChannels; k++)
      {
        hLoc = hRatio * (i + 0.5) - 0.5;
        hLoc = (hLoc < 0) ? 0 : hLoc;
        hIdx = hLoc;
        hNext = (hIdx < (inHeight-1)) ? 1 : 0;
        w11 = hLoc - hIdx;
        w10 = 1 - w11;

        wLoc = wRatio * (j + 0.5) - 0.5;
        wLoc = (wLoc < 0) ? 0 : wLoc;
        wIdx = wLoc;
        w01 = wLoc - wIdx;
        w00 = 1 - w01;
        int32_t inputOffset = inOffset+ inPitch  * hIdx + wIdx;
        int32_t outputOffset = outOffset + outPitch * (i) + (j);
        wNext = (wIdx < (inWidth-1)) ? 1 : 0;

        float32_tidl i00 = *(in + l*inBatchPitch + k*inChPitch + inputOffset);
        float32_tidl i01 = *(in + l*inBatchPitch + k*inChPitch + inputOffset + wNext);
        float32_tidl i10 = *(in + l*inBatchPitch + k*inChPitch + inputOffset + hNext*inPitch);
        float32_tidl i11 = *(in + l*inBatchPitch + k*inChPitch + inputOffset + hNext*inPitch + wNext);

        out[l*outBatchPitch + k*outChPitch + outputOffset] = (w10*(i00* w00 +  i01* w01) + w11*(i10* w00 +  i11* w01));
        //*(out + l*outBatchPitch + k*outChPitch + outputOffset) = (w10*(i00* w00 +  i01* w01) + w11*(i10* w00 +  i11* w01));
        }
      }
    }
    }
  }
  else
  {
    tidl_printf(0,"params->mode is  Not supported !!!\n ");
    assert(0);
  }
}
  return IALG_EOK;
}

static int32_t TIDL_refResizeProcess(const TIDL_CreateParams *createParams,
                              sTIDL_AlgLayer_t     * algLayer,
                              sTIDL_Layer_t        * tidlLayer,
                              void                 * inPtrs[],
                              void                 * outPtrs[],
                              int32_t                layerIdx)
{
  int32_t status = TIDL_SUCCESS;
  sTIDL_DataParams_t *inDataParams;
  inDataParams = &createParams->net->TIDLLayers[algLayer->inLayerIdx[0]].outData;
  sTIDL_ResizeLayerParams_t *params = &tidlLayer->layerParams.resizeParams;

  uint32_t inElmtSize     = TIDL_getDatElementSize(inDataParams->elementType);
  uint32_t outElmtSize    = TIDL_getDatElementSize(tidlLayer->outData.elementType);

  uint16_t inPitch        = (uint16_t)inDataParams->pitch[TIDL_LINE_PITCH];
  uint16_t inPitchBytes   = inPitch * inElmtSize;
  int32_t leftPadResize = TIDL_isPadOTF(createParams->net->deviceName) ? 0 : 1;

  /* Output parameters */
  uint32_t resizeOutChPitch        = tidlLayer->outData.pitch[TIDL_CHANNEL_PITCH];
  uint32_t resizeOutChPitchBytes   = resizeOutChPitch * TIDL_getDatElementSize(tidlLayer->outData.elementType);
  uint32_t resizeNumChannels       = tidlLayer->outData.dimValues[TIDL_DIM_NUMCH];
  uint32_t resizeOutLinePitch      = tidlLayer->outData.pitch[TIDL_LINE_PITCH];
  uint32_t resizeOutLinePitchBytes = resizeOutLinePitch * TIDL_getDatElementSize(tidlLayer->outData.elementType);

  /* Input parameters */
  uint32_t resizeInChPitch    = inDataParams->pitch[TIDL_CHANNEL_PITCH];
  uint32_t resizeInHeight     = inDataParams->dimValues[TIDL_DIM_HEIGHT];
  uint32_t resizeInWidth      = inDataParams->dimValues[TIDL_DIM_WIDTH];
  uint32_t resizeInWidthBytes = resizeInWidth * inElmtSize;
  uint32_t resizeInChPitchBytes= resizeInChPitch * inElmtSize;

  uint8_t *outPtr         = ((uint8_t *)outPtrs[0]);
  uint8_t *inPtrOrig = ((uint8_t *)inPtrs[0]);
  uint8_t *inPtr     = ((uint8_t *)inPtrs[0]) + ((1)*inPitch + leftPadResize) * inElmtSize;
  int32_t copyTopLine = 0, copyBottomLine = 0;
  int32_t inputHeight = 0;

  if(createParams->net->weightsElementSize == 4)
  {
    sTIDL_DataParams_t * inDataPrms[TIDL_NUM_IN_BUFS] = {0};
    for(int j=0; j < tidlLayer->numInBufs; j++)
    {
      inDataPrms[j]  = &createParams->net->TIDLLayers[algLayer->inLayerIdx[j]].outData;
    }
    status = TIDL_resizeProcessSP(tidlLayer, inPtrs, outPtrs[0], inDataPrms, &tidlLayer->outData, (uint8_t*)createParams->net);
    return (status);
  }

  if (params->mode == TIDL_ResizeBilinear)
  {
    // inPtr     = ((uint8_t *)inPtrs[0]) +  ((1)*inPitch + 1) * inElmtSize;
    // if(TIDL_privGetProcType(localDataFlowPtr) == 0)
    // {
    sTIDL_ResizeInfo_t resizeInfo;
    getResizeInfo(&resizeInfo, algLayer, tidlLayer, createParams, resizeInHeight);

    inputHeight = resizeInfo.inputHeight;

    copyTopLine = ( (resizeInfo.copyLineInfo & TIDL_COPY_TOP_LINE) == TIDL_COPY_TOP_LINE);
    copyBottomLine = ( (resizeInfo.copyLineInfo & TIDL_COPY_BOTTOM_LINE) == TIDL_COPY_BOTTOM_LINE);


    int32_t numBatches     = (int32_t)inDataParams->dimValues[TIDL_DIM_BATCH];
    uint32_t inBatchPitch  = (uint32_t)inDataParams->pitch[TIDL_ROI_PITCH];

    OPENACC(data copy(inPtrOrig[0:1+(numBatches-1) * inBatchPitch + (resizeNumChannels-1) * resizeInChPitchBytes + (inElmtSize*leftPadResize) + (inputHeight + 1) * inPitchBytes + (resizeInWidthBytes-1)]))
    // OPENACC(data copyin(inPtr[0:1+(numBatches-1) * inBatchPitch + (resizeNumChannels-1) * resizeInChPitchBytes + (inputHeight + 1) * inPitchBytes + (resizeInWidthBytes-1)]))
    OPENACC(parallel loop collapse(3))
    for (int k = 0; k < numBatches; k++)
    {
      for (int c = 0; c < resizeNumChannels; c++)
      {
        for (int j = 0; j < resizeInWidthBytes; j++)
        {
          if(1 == copyTopLine)
          {
            // *(inPtrOrig + k * inBatchPitch + c * resizeInChPitchBytes + (inElmtSize*leftPadResize) + j) = *(inPtr + k * inBatchPitch + c * resizeInChPitchBytes + j);
            inPtrOrig[k * inBatchPitch + c * resizeInChPitchBytes + (inElmtSize*leftPadResize) + j] = inPtrOrig[k * inBatchPitch + c * resizeInChPitchBytes + j + ((1)*inPitch + leftPadResize) * inElmtSize];
          }
          if(1 == copyBottomLine)
          {
            // *(inPtrOrig + k * inBatchPitch + c * resizeInChPitchBytes + (inElmtSize*leftPadResize) + (inputHeight + 1) * inPitchBytes + j) =
                //     *(inPtr + k * inBatchPitch + c * resizeInChPitchBytes + (inputHeight - 1) * inPitchBytes + j);
            inPtrOrig[k * inBatchPitch + c * resizeInChPitchBytes + (inElmtSize*leftPadResize) + (inputHeight + 1) * inPitchBytes + j] =
                inPtrOrig[k * inBatchPitch + c * resizeInChPitchBytes + (inputHeight - 1) * inPitchBytes + j + ((1)*inPitch + leftPadResize) * inElmtSize];
          }
        }
      }
    }
  }
  if (inDataParams->elementType == TIDL_SignedChar)
  {
    TIDL_refResize((int8_t *)inPtr, (int8_t *)outPtr, algLayer, tidlLayer, params, inDataParams, createParams);
  }
  else if (inDataParams->elementType == TIDL_UnsignedChar)
  {
    TIDL_refResize((uint8_t *)inPtr, (uint8_t *)outPtr, algLayer, tidlLayer, params, inDataParams, createParams);
  }
  else if (inDataParams->elementType == TIDL_SignedShort)
  {
    TIDL_refResize((int16_t *)inPtr, (int16_t *)outPtr, algLayer, tidlLayer, params, inDataParams, createParams);
  }
  else if (inDataParams->elementType == TIDL_UnsignedShort)
  {
    TIDL_refResize((uint16_t *)inPtr, (uint16_t *)outPtr, algLayer, tidlLayer, params, inDataParams, createParams);
  }
  else
  {
    tidl_printf(0, "TIDL_ResizeProcess in elementType is  Not supported !!!\n ");
  }
  if ((createParams->flowCtrl & TIDL_FLOW_CTRL_REF_STAT) == TIDL_FLOW_CTRL_REF_STAT)
  {
    TIDL_Obj intAlgObj;
    TIDL_CreateParams createParamsCpy;

    memcpy(&createParamsCpy, createParams, sizeof(TIDL_CreateParams));
    intAlgObj.createParams = (TIDL_CreateParams *) &createParamsCpy;

    TIDL_UpdateScaleFactors(&intAlgObj, layerIdx, 1, 0, 0);
  }

  return 0;
}

#if ENABLE_NUMCONSUMERS_FROM_GC
static int32_t TIDL_getNumConsumersForDataIdGC(const sWorkloadUnit_t *workloadUnit,
                                    const sGCHelperHandle * gcHelperHandle)
{

  int32_t numConsumers = 0;
  int32_t dataBufId;

  for(int32_t linkId = 0; linkId < workloadUnit->numLinks; linkId++)
  {
    const sLink_t *link = getLinkPtr(workloadUnit, NOT_VALID, linkId);

    for(int32_t srcId = 0; srcId < link->numSrc; srcId++)
    {
      const sJoint_t *joint = &link->src[srcId];
      const sBufParams_t *srcBuf = getBufParamsFromBufIndex(gcHelperHandle, joint->bufDBindex);

      if(srcBuf->type == BUF_FM_FULL)
      {
        databufId = joint->bufDBindex;
        /* After finding the required buffer, break the loop */
        linkId = workloadUnit->numLinks;
        /* First consumer of the buffer is the reference workload itself.
        It won't be counted again in the next loop for counting consumers. */
        numConsumers++;
        break;
      }
    }
  }

  for(int32_t workloadId = 0; workloadId < /* getWLCount() */ ; workloadId++) // /* Not expected to be used here */
  {
    const sWorkloadUnit_t *currWorkloadUnit = getWLUnitPtr(workloadId);
    for(int32_t linkId = 0; linkId < workloadUnit->numLinks; linkId++)
    {
      const sLink_t *link = getLinkPtr(workloadUnit, NOT_VALID, linkId);

      for(int32_t srcId = 0; srcId < link->numSrc; srcId++)
      {
        const sJoint_t *joint = &link->src[srcId];

        if(joint->bufDBindex == databufId)
        {
          numConsumers++;
        }
      }
    }
  }

  return numConsumers;
}

#endif

static void TIDL_resizeUpdateWorkload(sWorkloadUnit_t *workloadUnit,
                                    const sGCHelperHandle * gcHelperHandle,
                                    sTIDL_Layer_t *tidlLayer)
{
  const sTIDL_ResizeLayerParams_t *params = &tidlLayer->layerParams.resizeParams;
  int32_t numSplit = getNumSplits((const void *) workloadUnit);
  int32_t outPitch = tidlLayer->outData.pitch[TIDL_LINE_PITCH];
  int32_t outPad = 1;

  if(params->resizeRatio[TIDL_DIM_WIDTH] == 4.0 )
  {
    outPad = 2;
  }
  /* Resize kernel overwrites the pad in the output buffer and expects the joint to point to the padded region*/
  for (int32_t linkIdx = 0; linkIdx < workloadUnit->numLinks; linkIdx++)
  {
    sLink_t *link = getLinkPtr(workloadUnit, NOT_VALID, linkIdx);
    const sBufParams_t *buf = getBufParamsFromBufIndex(gcHelperHandle, link->sink.bufDBindex);

    /* In resize LFMP case, there exists overlap in the output as well. This is because resize expects 'padded'
      region for every split and we provide last few rows of the previous split to satisfy the kernel requirement.
      This overlapping region in the input creates the overlapping region in the output between split feature maps.
      To optimize things further, resize layer understands that it is processing the overlap region and skips
      processing the overlapping input in every split. However, it leaves space in the output assuming that it was
      overwriting the overlap output that the previous split generated.
      When the kernel writes directly to output buffer, gc can handle by manipulating flowDims. However, in
      case of output DMA we:
      1) Write only the valid region that the kernel generates for the split (basically (kernel expected output) - (overlap output))
        This is handled by gc
      2) Offset the read from staging buffer by amount equal to the output overlap size.
        This is handled by TIDL in the following code.
      */
    if ((link->subType == LINK_X_FM_OUT_PART_TO_FULL) && (numSplit > 1))
    {
      link->src[0].offset = link->src[0].offset + (outPad * outPitch + outPad);
    }
    if((buf->type == BUF_FM_FULL))
    {
      /* LFMP case we overwrite padded region only in case there is no output DMA*/
      if ((link->type == LINK_PROC) && (numSplit > 1))
      {
        link->sink.offset = link->sink.offset - (outPad * outPitch + outPad);
      }
      /* SFMP case we overwrite padded region of the final buffer by links responsible for generating output info */
      else if ((numSplit == 1) && ((link->type == LINK_PROC) || (link->subType == LINK_X_FM_OUT_PART_TO_FULL)))
      {
        link->sink.offset = link->sink.offset - (outPad * outPitch + outPad);
      }
    }
  }
}

static int32_t TIDL_resizeGetPermuteBufSize(sTIDL_Network_t *net, int32_t layerIdx)
{
  sTIDL_Layer_t *tidlLayer = &net->TIDLLayers[layerIdx];
  sTIDL_DataParams_t * inDataParams = TIDL_getDataParams(net, tidlLayer->inData[0]);
  int32_t resizeInWidth = inDataParams->dimValues[TIDL_DIM_WIDTH];
  int32_t resizeOutWidth = tidlLayer->outData.dimValues[TIDL_DIM_WIDTH];
  int32_t simdWidth = (TIDL_SIMD_WIDTH/2) - 2;

  /*scartch size for permute_control and control variables in the kernel. 128 bytes for 2 control patterns*/
  int32_t scratchSize = ((resizeInWidth + 2 + simdWidth - 1)/simdWidth)*(resizeOutWidth/resizeInWidth)*TIDL_SIMD_WIDTH + TIDL_SIMD_WIDTH*2;

  return scratchSize;
}

static inline int32_t TIDL_resizeAllocPermuteBufMem(const TIDL_NetworkCommonParams  *commonParams,
                                         int32_t layerIdx,
                                         sTIDL_AlgLayer_t *algLayer,
                                         int32_t permuteBufSize,
                                         int32_t *paramMemTabOffset,
                                         uint8_t *memory[TIDL_LAYER_MEMORY_MAX])
{
  sTIDL_ALgResizeLayer_t    *algResizeLayerParams = &algLayer->layerParams.resizeParams;

  TIDL_AllocatePtr((intptr_t)memory[TIDL_LAYER_MEMORY_PERSISTENT],
                                    paramMemTabOffset,
                                    permuteBufSize,
                                    TIDL_ALIGNMENT_SIZE,
                                    &algResizeLayerParams->permutePatternBuf);

  algResizeLayerParams->permutePatternBufInt = (int8_t *)TIDL_getMemoryChunkFromSysmem(commonParams->tidlCommonParams->sysMems,
                                                                                    permuteBufSize,
                                                                                    128U,
                                                                                    TIDL_SYSMEM_L3_SCRATCH,
                                                                                    IALG_SCRATCH);

  if (algResizeLayerParams->permutePatternBufInt == NULL)
  {
    tidl_printf(1, "Can't fit resize permute buffer in MSMC, Trying DDR\n");

    algResizeLayerParams->permutePatternBufInt = (int8_t *)TIDL_getMemoryChunkFromSysmem(commonParams->tidlCommonParams->sysMems,
                                                                                    permuteBufSize,
                                                                                    128U,
                                                                                    TIDL_SYSMEM_DDR_SCRATCH,
                                                                                    IALG_SCRATCH);

    if (algResizeLayerParams->permutePatternBufInt == NULL)
    {
      tidl_printf(0, "DDR alloc failed for resize: %d\n", permuteBufSize);
      return -1;
    }
  }

  return TIDL_SUCCESS;
}

/**
*******************************************************************************
*  \brief      Function to supply memory requirement for Resize layer to user
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
int32_t TIDL_resizeAllocNew(const TIDL_LayerSpecificParams *layerSpecificParams,
                                   const TIDL_NetworkCommonParams *commonParams,
                                   int32_t layerIdx,
                                   int32_t memorySize[TIDL_LAYER_MEMORY_MAX])
{
  int32_t status = IALG_EOK;
  sTIDL_Layer_t *tidlLayer = &commonParams->net->TIDLLayers[layerIdx];
  sTIDL_ResizeLayerParams_t *resizeLayerParams = &tidlLayer->layerParams.resizeParams;

  int32_t isOptFlowAvailable = isOptimizedResizeAvailable(resizeLayerParams);
  int32_t flowCtrl = (isOptFlowAvailable == 1) ? commonParams->createParams->flowCtrl : TIDL_FLOW_CTRL_REF_ONLY;

  if ((flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) == 0)
  {
    status = TIDL_deviceUtilsCommonAlloc(layerSpecificParams, commonParams, layerIdx, memorySize);
    memorySize[TIDL_LAYER_MEMORY_PERSISTENT] += TIDL_resizeGetPermuteBufSize(commonParams->net, layerIdx) + 128U;
  }
  else
  {
    status = TIDL_deviceCommonRefAlloc(layerSpecificParams, commonParams, layerIdx, memorySize);
  }
  return status;
}


/**
*******************************************************************************
*  \brief      Function to set memory needed by Resize layer
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
int32_t TIDL_resizeInitNew(const TIDL_LayerSpecificParams *layerSpecificParams,
                                const TIDL_NetworkCommonParams  *commonParams,
                                sTIDL_AlgLayer_t               *algLayer,
                                int32_t layerIdx,
                                uint8_t *memory[TIDL_LAYER_MEMORY_MAX],
                                int32_t memorySize[TIDL_LAYER_MEMORY_MAX],
                                void    **outPtr)
{
  int32_t status = IALG_EOK;
  sTIDL_Layer_t *tidlLayer = &commonParams->net->TIDLLayers[layerIdx];
  sTIDL_ResizeLayerParams_t *resizeLayerParams = &tidlLayer->layerParams.resizeParams;
  sTIDL_ALgResizeLayer_t    *algResizeLayerParams = &algLayer->layerParams.resizeParams;

  int32_t isOptFlowAvailable = isOptimizedResizeAvailable(resizeLayerParams);
  int32_t flowCtrl = commonParams->createParams->flowCtrl;

  algResizeLayerParams->isOptResizeAvail = isOptFlowAvailable;

  if (isOptFlowAvailable == 0 && (flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) == 0)
  {
    tidl_printf(0," WARNING : Running unoptimized code (Natural C code) for the resize layer number  %d !!!\n ", algLayer->layerIdx);
    flowCtrl = TIDL_FLOW_CTRL_REF_ONLY;
  }

  /** Resize layer duplicates the top and bottom rows into the input buffer's padded region
   * If resize is the only consumer of the input layer, then there is no issue
   * However, if the input layer has any other consumer layer, then padded region should be restored with zeroes
   * Below code checks for this and populates "inputPadRefillZeros" to be used in resize code
   */
  algResizeLayerParams->inputPadRefillZeros = 0;
  int32_t resizeInDataId = commonParams->net->TIDLLayers[layerIdx].inData[0];
  int32_t numConsumersResizeId = TIDL_getNumConsumersForDataId(commonParams->net, resizeInDataId);
  if((numConsumersResizeId > 1) && (resizeLayerParams->mode == TIDL_ResizeBilinear))
  {
    /* Top and bottom row duplication is required only for bilinear resize, so refilling it with zero padding also will be required only for bilinear resize */
    algResizeLayerParams->inputPadRefillZeros = 1;
  }

  if ((flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) == 0)
  {
    int32_t currOffset = 0;
    algResizeLayerParams->scratchSize = TIDL_resizeGetPermuteBufSize(commonParams->net, layerIdx);

    TIDL_resizeUpdateWorkload((sWorkloadUnit_t* )algLayer->workloadUnit, algLayer->gcHelperHandle, tidlLayer);

    /* Initialization of this buffer will be taken care of by the device init routine */
    status = TIDL_resizeAllocPermuteBufMem(commonParams,
                                          layerIdx,
                                          algLayer,
                                          algResizeLayerParams->scratchSize,
                                          &currOffset,
                                          memory);

    /* APIs calculate offset with regard to base ptr. In this situation, the device utils API would not
    * know that the memory has already been utilized. Thus we add an offset equal to the size of memory
    * space already allocated and remove the offset after the API has run so the caller function sees the
    * same memory.
    **/
    memory[TIDL_LAYER_MEMORY_PERSISTENT] += TIDL_ALIGN_CEIL(currOffset, TIDL_ALIGNMENT_SIZE);

    if (status == TIDL_SUCCESS)
    {
      status = TIDL_deviceUtilsCommonInit(layerSpecificParams,
                                        commonParams,
                                        algLayer,
                                        layerIdx,
                                        memory,
                                        memorySize,
                                        outPtr);
    }

    memory[TIDL_LAYER_MEMORY_PERSISTENT] -= TIDL_ALIGN_CEIL(currOffset, TIDL_ALIGNMENT_SIZE);
    memorySize[TIDL_LAYER_MEMORY_PERSISTENT] += TIDL_ALIGN_CEIL(currOffset, TIDL_ALIGNMENT_SIZE);
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
  return status;
}

static inline void TIDL_resizeDmaPadOverwrite(void *dst,
                                  void *src,
                                  int32_t icnt0,
                                  int32_t icnt1,
                                  int32_t dim1,
                                  int32_t dim2,
                                  void* dmaUtilsContext,
                                  uint8_t *trMem)
{
  TIDL_prePareMemcpyTr(trMem); //:TODO: Remove this
  TIDL_memcpy2D(dst, src, icnt0, icnt1, dim1, dim2, dmaUtilsContext, trMem);
}

static int32_t TIDL_resizeDspProcessNew(TIDL_NetworkCommonParams *commonParams,
                              sTIDL_AlgLayer_t     * algLayer,
                              sTIDL_Layer_t        * tidlLayer,
                              void                 * inPtrs[],
                              void                 * outPtrs[],
                              int32_t                layerIdx)
{
  int32_t status = TIDL_SUCCESS;
  uint8_t trMem[64]={0};
  uint8_t * inPtrOrig;
  uint8_t * inPtr;

  sTIDL_DataParams_t *inDataParams = &commonParams->net->TIDLLayers[algLayer->inLayerIdx[0]].outData;
  sTIDL_ResizeLayerParams_t *params = &tidlLayer->layerParams.resizeParams;
  sTIDL_ALgResizeLayer_t *resizeParams = &algLayer->layerParams.resizeParams;

  int32_t inElmtSize         = TIDL_getDatElementSize(inDataParams->elementType);
  int32_t inPitch            = inDataParams->pitch[TIDL_LINE_PITCH];
  int32_t inPitchBytes       = inPitch * inElmtSize;
  int32_t resizeNumChannels, resizeInChPitch, resizeInHeight, resizeInWidth, resizeInWidthBytes, resizeInChPitchBytes;
  int32_t bPtrOffset = 0;


  int32_t leftPadResize = 1;

  /* Might affect performance since variables are in DDR */
  for (int32_t linkIdx =0 ; linkIdx < algLayer->workloadUnit->numLinks; linkIdx++)
  {
    const sLink_t *link = getLinkPtr(algLayer->workloadUnit,NULL,linkIdx);
    for (int32_t jointIdx = 0; jointIdx < link->numSrc; jointIdx++)
    {
      sBufParams_t * bufParams = getBufParamsFromBufIndex(commonParams->gcHelperHandle, link->src[jointIdx].bufDBindex);
      if (bufParams->type == BUF_FM_FULL)
      {
        resizeNumChannels  = bufParams->metaParams.featMetaParams.channel; /* TODO: Handle correctly for channel split */
        resizeInChPitch    = bufParams->bufWidth;
        resizeInWidth      = bufParams->metaParams.featMetaParams.width;
        resizeInChPitchBytes = resizeInChPitch * inElmtSize;
        resizeInWidthBytes = resizeInWidth * inElmtSize;

        bPtrOffset = link->src[jointIdx].offset * inElmtSize;
        goto Exit;
      }
    }
  }
Exit:

  TIDL_prePareMemcpyTr(trMem);
  /* permutePatternBufInt is not part of handle, so generic handle wb does not cover this, hence separate writeback required */
  TIDL_memcpy2DCacheWbInv((uint8_t *)resizeParams->permutePatternBufInt,
          (uint8_t *)resizeParams->permutePatternBuf,
          resizeParams->scratchSize,
          1,
          0,
          0,
          commonParams->tidlCommonParams->dmaUtilsContext,
          trMem,
          commonParams->tidlCommonParams->sysMems);
  /*for non otf cases we required 1 pad in the left but not for otf*/
  if(TIDL_isPadOTF(commonParams->net->deviceName)) leftPadResize = 0;


  /* Here inPtrs[0] is expected to be A pointer and hence, we must derive B offset */
  inPtrOrig = ((uint8_t *)inPtrs[0]) +  bPtrOffset;
  inPtr     = ((uint8_t *)inPtrOrig) +  ((1) * inPitch + leftPadResize) * inElmtSize;

  if (params->mode == TIDL_ResizeBilinear)
  {
    WorkloadUnitExec_CommonParams commonParamsLocalCpy = *commonParams->tidlCommonParams;
    sWLMetaDataID_t wlMetaData;
    getWLMetaDataID(algLayer->workloadUnit->metadata, &wlMetaData);
    resizeInHeight = wlMetaData.inTileHeight;

    sTIDL_ResizeInfo_t resizeInfo;
    getResizeInfo(&resizeInfo, algLayer, tidlLayer, commonParams->createParams, resizeInHeight);

    resizeInHeight = resizeInfo.inputHeight;


    if((resizeInfo.copyLineInfo & TIDL_COPY_TOP_LINE) == TIDL_COPY_TOP_LINE)
    {
      /* Duplicate top line into the padding region */
      TIDL_resizeDmaPadOverwrite((inPtrOrig) + (inElmtSize * leftPadResize),
                    inPtr,
                    resizeInWidthBytes,
                    resizeNumChannels,
                    resizeInChPitchBytes,
                    resizeInChPitchBytes,
                    commonParamsLocalCpy.dmaUtilsContext,
                    trMem);
    }

    if((resizeInfo.copyLineInfo & TIDL_COPY_BOTTOM_LINE) == TIDL_COPY_BOTTOM_LINE)
    {
      /* Duplicate bottom line into the padding region */
      TIDL_resizeDmaPadOverwrite((inPtrOrig + (inElmtSize * leftPadResize) + (resizeInHeight + 1) * inPitchBytes),
                    (inPtr + (resizeInHeight - 1) * inPitchBytes),
                    resizeInWidthBytes,
                    resizeNumChannels,
                    resizeInChPitchBytes,
                    resizeInChPitchBytes,
                    commonParamsLocalCpy.dmaUtilsContext,
                    trMem);
    }
  }

  status = TIDL_deviceUtilsCommonProcess(commonParams,
                                  algLayer,
                                  tidlLayer,
                                  inPtrs,
                                  outPtrs,
                                  layerIdx);

  if(resizeParams->inputPadRefillZeros == 1)
  {
    WorkloadUnitExec_CommonParams commonParamsLocalCpy = *commonParams->tidlCommonParams;
    int32_t remainingTransferSize = resizeInWidthBytes;

    /* Replace the padding region duplicated with top line back to zeroes */
    while(remainingTransferSize > 0)
    {
      int32_t transferSize = remainingTransferSize;

      if(transferSize > TIDL_1K_ZERO_VECTOR)
      {
        transferSize = TIDL_1K_ZERO_VECTOR;
      }

      TIDL_resizeDmaPadOverwrite((inPtrOrig + (inElmtSize * leftPadResize)),
                        commonParams->tidlCommonParams->zeroVector1k,
                        transferSize,
                        resizeNumChannels,
                        resizeInChPitchBytes,
                        0,
                        commonParamsLocalCpy.dmaUtilsContext,
                        trMem);

      TIDL_resizeDmaPadOverwrite((inPtrOrig + (inElmtSize * leftPadResize) + (resizeInHeight + 1) * inPitchBytes),
                        commonParams->tidlCommonParams->zeroVector1k,
                        transferSize,
                        resizeNumChannels,
                        resizeInChPitchBytes,
                        0,
                        commonParamsLocalCpy.dmaUtilsContext,
                        trMem);

      remainingTransferSize -= transferSize;
    }
  }

  return status;
}

int32_t TIDL_resizeProcessNew(TIDL_NetworkCommonParams *commonParams,
                              sTIDL_AlgLayer_t     * algLayer,
                              sTIDL_Layer_t        * tidlLayer,
                              void                 * inPtrs[],
                              void                 * outPtrs[],
                              int32_t                layerIdx)
{
  int32_t status = IALG_EOK;
  int32_t flowCtrl = commonParams->createParams->flowCtrl;

  if(algLayer->layerParams.resizeParams.isOptResizeAvail == 0)
  {
    flowCtrl |= TIDL_FLOW_CTRL_REF_ONLY;
  }

  /* Setup various pointers needed by convolution layers */
  if ((flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) == 0)
  {
    status = TIDL_resizeDspProcessNew(commonParams,
                                   algLayer,
                                   tidlLayer,
                                   inPtrs,
                                   outPtrs,
                                   layerIdx);
  }
  else
  {
    status = TIDL_refResizeProcess(commonParams->createParams,
                                   algLayer,
                                   tidlLayer,
                                   inPtrs,
                                   outPtrs,
                                   layerIdx);
  }

  return status;
}
