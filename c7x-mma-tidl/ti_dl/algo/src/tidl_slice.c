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
@file    tidl_slice.c
@brief   This file defines private functions for Slice layer.
@version 0.1 (June 2019) : Initial version [ADK]
----------------------------------------------------------------------------
*/
#include "tidl_alg_int.h"
#include "tidl_slice.h"
#include "tidl_commonUtils.h"
#include "tidl_device_utils.h"

/**
 * @brief This is reference implementation of Slice layer
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
 */
template <class Tin, class Tout>
void TIDL_refSlice(
  const Tin*    pIn,
  Tout*   pOut,
  int32_t inPtrOffset,
  int32_t outPtrOffset,
  int32_t outWidth,
  int32_t outHeight,
  int32_t numChs,
  int32_t numDim1,
  int32_t numDim2,
  int32_t numROIs,
  int32_t inLinePitch,
  int32_t outLinePitch,
  int32_t inChPitch,
  int32_t outChPitch,
  int32_t inDim1Pitch,
  int32_t outDim1Pitch,
  int32_t inDim2Pitch,
  int32_t outDim2Pitch,
  int32_t inROIPitch,
  int32_t outROIPitch
  )
{
  int32_t i0, i1, i2, i3, i4, i5;

  OPENACC(data copyin(pIn[:1+inPtrOffset  + ((numROIs-1)*inROIPitch)   + ((numChs-1)* inChPitch)   + ((outHeight-1) * inLinePitch)   + (outWidth-1)]) \
               copy(pOut[:1+outPtrOffset  + ((numROIs-1)*outROIPitch)   + ((numChs-1)* outChPitch)   + ((outHeight-1) * outLinePitch)   + (outWidth-1)]))
  OPENACC(parallel loop collapse(6))
  for (i0 = 0; i0 < numROIs; i0++)
  {
    for (i1 = 0; i1 < numDim1; i1++)
    {
      for (i2 = 0; i2 < numDim2; i2++)
      {
        for (i3 = 0; i3 < numChs; i3++)
        {
          for (i4 = 0; i4 < outHeight; i4++)
          {
            for (i5 = 0; i5 < outWidth; i5++)
            {
              int32_t inOffset  = inPtrOffset + (i0*inROIPitch)   + (i1* inDim1Pitch) + (i2*inDim2Pitch)   +(i3* inChPitch)   + (i4 * inLinePitch)   + i5;
              int32_t outOffset = outPtrOffset + (i0*outROIPitch) + (i1*outDim1Pitch) + (i2*outDim2Pitch) + (i3* outChPitch)  + (i4 * outLinePitch)  + i5;
              pOut[outOffset] = pIn[inOffset];
            }
          }
        }
      }
    }
  }
  TIDL_enableL1DandL2CacheWb();
}

int32_t TIDL_sliceProcessNew(TIDL_NetworkCommonParams *commonParams,
                              sTIDL_AlgLayer_t     * algLayer,
                              sTIDL_Layer_t        * tidlLayer,
                              void                 * inPtrs[],
                              void                 * outPtrs[],
                              int32_t                layerIdx)
{
  sTIDL_DataParams_t *inDataParams   = &commonParams->net->TIDLLayers[algLayer->inLayerIdx[0]].outData;

  int32_t outWidth           = tidlLayer->outData.dimValues[TIDL_DIM_WIDTH];
  int32_t outHeight          = tidlLayer->outData.dimValues[TIDL_DIM_HEIGHT];
  int32_t outChs             = tidlLayer->outData.dimValues[TIDL_DIM_NUMCH];
  int32_t outDim1            = tidlLayer->outData.dimValues[TIDL_DIM_DIM1];
  int32_t outDim2            = tidlLayer->outData.dimValues[TIDL_DIM_DIM2];
  int32_t numROIs            =  inDataParams->dimValues[TIDL_DIM_BATCH]; /*ROI  */
  int32_t inPitch            = (int32_t)inDataParams->pitch[TIDL_LINE_PITCH];
  int32_t outPitch           = (int32_t)tidlLayer->outData.pitch[TIDL_LINE_PITCH];
  int32_t inChPitch          = (int32_t)inDataParams->pitch[TIDL_CHANNEL_PITCH];
  int32_t outChPitch         = (int32_t)tidlLayer->outData.pitch[TIDL_CHANNEL_PITCH];
  int32_t inDim1Pitch          = (int32_t)inDataParams->pitch[TIDL_DIM1_PITCH];
  int32_t inDim2Pitch          = (int32_t)inDataParams->pitch[TIDL_DIM2_PITCH];
  int32_t status             = IALG_EOK;
  int32_t inROIPitch         = (int32_t)inDataParams->pitch[TIDL_ROI_PITCH];
  int32_t outROIPitch        = (int32_t)tidlLayer->outData.pitch[TIDL_ROI_PITCH];
  int32_t outDim1Pitch        = (int32_t)tidlLayer->outData.pitch[TIDL_DIM_DIM1];
  int32_t outDim2Pitch        = (int32_t)tidlLayer->outData.pitch[TIDL_DIM_DIM2];
  const sTIDL_SliceLayerParams_t *params = &tidlLayer->layerParams.sliceParams;
  int32_t slicePoint        = params->slicePoints[0];
  int32_t inPtrOffset        = 0;
  int32_t outPtrOffset       = 0;

  void * refInPtr;
  void * refOutPtr;
  void * targetInPtr;

  void    *inPtr = inPtrs[0];
  uint8_t(*outPtr)[] = (uint8_t(*)[])outPtrs[0];

  if (commonParams->gcHelperHandle == NULL)
  {
    if(params->axis == TIDL_DIM_DIM1)
    {
      inPtrOffset       += (slicePoint*inDim1Pitch);
    }
    else if(params->axis == TIDL_DIM_DIM2)
    {
      inPtrOffset       += (slicePoint*inDim2Pitch);
    }
    else if(params->axis == TIDL_DIM_NUMCH)
    {
      inPtrOffset       += (slicePoint*inChPitch);
    }
    else if(params->axis == TIDL_DIM_HEIGHT)
    {
      inPtrOffset       += (slicePoint*inPitch);
    }
    else if(params->axis == TIDL_DIM_WIDTH)
    {
      inPtrOffset       += (slicePoint);
    }
    else
    {
      status = IALG_EFAIL;
    }
  }

  if(status == IALG_EOK)
  {
    targetInPtr = ((int8_t *)inPtr);
    refOutPtr   =(*outPtr);
    refInPtr    = targetInPtr;
    if ((commonParams->createParams->flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) != TIDL_FLOW_CTRL_REF_ONLY)
    {
      if(inPtr != outPtr)
      {
        status = TIDL_deviceUtilsCommonProcess(commonParams,
                                    algLayer,
                                    tidlLayer,
                                    inPtrs,
                                    outPtrs,
                                    layerIdx);
      }
    }
    else
    {
      if (inDataParams->elementType == TIDL_SinglePrecFloat)
      {
        TIDL_refSlice((float32_tidl *)refInPtr, (float32_tidl *)refOutPtr, inPtrOffset, outPtrOffset, outWidth, outHeight, outChs, outDim1, outDim2, numROIs, inPitch, outPitch, inChPitch, outChPitch, inDim1Pitch, outDim1Pitch, inDim2Pitch, outDim2Pitch, inROIPitch, outROIPitch);
      }
      else if ((inDataParams->elementType == TIDL_SignedChar) || (inDataParams->elementType == TIDL_UnsignedChar))
      {
        TIDL_refSlice((uint8_t *)refInPtr, (uint8_t *)refOutPtr, inPtrOffset, outPtrOffset, outWidth, outHeight, outChs, outDim1, outDim2, numROIs, inPitch, outPitch, inChPitch, outChPitch, inDim1Pitch, outDim1Pitch, inDim2Pitch, outDim2Pitch, inROIPitch, outROIPitch);
      }
      else
      {
        TIDL_refSlice((uint16_t *)refInPtr, (uint16_t *)refOutPtr, inPtrOffset, outPtrOffset, outWidth, outHeight, outChs, outDim1, outDim2, numROIs, inPitch, outPitch, inChPitch, outChPitch, inDim1Pitch, outDim1Pitch, inDim2Pitch, outDim2Pitch, inROIPitch, outROIPitch);
      }
      if((commonParams->createParams->flowCtrl & TIDL_FLOW_CTRL_REF_STAT) == TIDL_FLOW_CTRL_REF_STAT)
      {
        TIDL_Obj intAlgObj;
        TIDL_CreateParams createParams;

        memcpy(&createParams, commonParams->createParams, sizeof(TIDL_CreateParams));
        intAlgObj.createParams = (TIDL_CreateParams *) &createParams;

        TIDL_UpdateScaleFactors(&intAlgObj, algLayer->layerIdx, 0, 0, 1);
      }
    }
  }

  return status;
}
