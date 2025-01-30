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
@file    tidl_flatten.c
@brief   This file defines private functions for flatten layer.
@version 0.1 (Nov 2017) : Initial version [EPR]
----------------------------------------------------------------------------
*/

#include "tidl_alg_int.h"
#include "tidl_flatten.h"
#include "tidl_commonUtils.h"
#include "tidl_device_utils.h"

/**
 * @brief This is reference implementation of Flatten layer
 * 
 * @tparam Tin : template for input data buffers 
 * @tparam Tout : template for output data buffers 
 * @param pIn : Pointer to input memory
 * @param pOut : Pointer to output memory 
 * @param inPtrOffset : offset input buffer pointer
 * @param outPtrOffset : offset output buffer pointer
 * @param width : width of the input buffer
 * @param height : height of the input buffer 
 * @param numChs : no of total channels
 * @param numROIs : no of total ROI
 * @param inLinePitch : pitch of the input buffer
 * @param inChPitch : Chnnel pitch of the input buffer
 * @param inROIPitch : ROI pitch of the input buffer
 * @param outROIPitch : ROI pitch of the output buffer 
 */
template <class Tin, class Tout>
void TIDL_refFaltten(const Tin*    pIn,
                     Tout*   pOut,
                     int32_t inPtrOffset,
                     int32_t outPtrOffset,
                     int32_t width,
                     int32_t height,
                     int32_t numChs,
                     int32_t numROIs,
                     int32_t inLinePitch,
                     int32_t inChPitch,
                     int32_t inROIPitch,
                     int32_t outROIPitch)
{
  int32_t i0, i1, i2, i3;
  int32_t outCount;
  for (i0 = 0; i0 < numROIs; i0++)
  {
    outCount = 0;
    for (i1 = 0; i1 < numChs; i1++)
    {
      for (i2 = 0; i2 < height; i2++)
      {
        for (i3 = 0; i3 < width; i3++)
        {
          int32_t inOffset  = inPtrOffset + (i0*inROIPitch)  + (i1* inChPitch)  + (i2 * inLinePitch)  + i3;
          int32_t outOffset = outPtrOffset + (i0*outROIPitch) + outCount;
          pOut[outOffset] = pIn[inOffset];
          outCount++;
        }
      }
    }
  }
  TIDL_enableL1DandL2CacheWb();
}

int32_t TIDL_flattenProcessNew(TIDL_NetworkCommonParams *commonParams,
                              sTIDL_AlgLayer_t     * algLayer,
                              sTIDL_Layer_t        * tidlLayer,
                              void                 * inPtrs[],
                              void                 * outPtrs[],
                              int32_t                layerIdx)
{
  sTIDL_DataParams_t *inDataParams   = &commonParams->net->TIDLLayers[algLayer->inLayerIdx[0]].outData;
  int32_t inWidth            = inDataParams->dimValues[TIDL_DIM_WIDTH];
  int32_t inHeight           = inDataParams->dimValues[TIDL_DIM_HEIGHT];
  int32_t inChs              = inDataParams->dimValues[TIDL_DIM_NUMCH];
  int32_t numROIs            = inDataParams->dimValues[TIDL_DIM_BATCH]; /*ROI  */
  int32_t inPitch            = (int32_t)inDataParams->pitch[TIDL_LINE_PITCH];
  int32_t inChPitch          = (int32_t)inDataParams->pitch[TIDL_CHANNEL_PITCH];
  int32_t status             = IALG_EOK;
  int32_t inPtrOffset        = 0;
  int32_t outPtrOffset       = 0;
  int32_t inROIPitch         = (int32_t)inDataParams->pitch[TIDL_ROI_PITCH];
  int32_t outROIPitch        = (int32_t)tidlLayer->outData.pitch[TIDL_ROI_PITCH];


  if (((commonParams->createParams->flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) != TIDL_FLOW_CTRL_REF_ONLY) &&
      ( TIDL_getDatElementSize(inDataParams->elementType)  == 1 ) &&
      ( TIDL_getDatElementSize(tidlLayer->outData.elementType) == 1 ))

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
    if ((inDataParams->elementType == TIDL_SignedChar) || (inDataParams->elementType == TIDL_UnsignedChar))
    {
      TIDL_refFaltten((uint8_t *)inPtrs[0], (uint8_t *)outPtrs[0], inPtrOffset, outPtrOffset, inWidth, inHeight, inChs, numROIs, inPitch, inChPitch, inROIPitch, outROIPitch);
    }
    else if((inDataParams->elementType == TIDL_SignedShort) || (inDataParams->elementType == TIDL_UnsignedShort))
    {
      TIDL_refFaltten((uint16_t *)inPtrs[0], (uint16_t *)outPtrs[0], inPtrOffset, outPtrOffset, inWidth, inHeight, inChs, numROIs, inPitch, inChPitch, inROIPitch, outROIPitch);
    }
    else if(inDataParams->elementType == TIDL_SinglePrecFloat)
    {
      TIDL_refFaltten((float32_tidl *)inPtrs[0], (float32_tidl *)outPtrs[0], inPtrOffset, outPtrOffset, inWidth, inHeight, inChs, numROIs, inPitch, inChPitch, inROIPitch, outROIPitch);
    }
    else
    {
      tidl_printf(0,"Un supported  elementType in %s File, %d Line \n  ", __FILE__, __LINE__);
      status = IALG_EFAIL;
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

  return status;
}
