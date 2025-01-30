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
@file    tidl_argmax.c
@brief   This file defines private functions for Arg max layer.
@version 0.1 (Oct 2016) : Initial version [ADK]
@version 0.2 (Nov 2016) : Added C66x DSP support [EPR]
@version 0.3 (Jan 2017) : Cleaned up [EPR]
----------------------------------------------------------------------------
*/

#include "tidl_alg_int.h"
#include "tidl_commonUtils.h"
#include "tidl_argmax.h"
#include "tidl_device_utils.h"
#include "tidl_alg_utils.h"

#define FIND_FIRST_MAX (0)
/**
 * @brief This is reference implementation of ArgMax layer
 *
 * @tparam Tin : template for input data buffers
 * @tparam Tout : template for output data buffers
 * @param algLayer : Pointer to the layer specific parameters
 * @param params : Copy of argMax layer parameters
 * @param tidlLayer : Pointer to the common layer parameters
 * @param inPtr : Pointer to input memory
 * @param outPtr : Pointer to output memory
 * @param imWidth : width of the input buffer
 * @param imHeight : height of the input buffer
 * @param numTotRoi : no of total ROI
 * @param inPitch : pitch of the input buffer
 * @param outPitch : pitch of the output buffer
 * @param inChPitch : Chnnel pitch of the input buffer
 * @param outChPitch : Chnnel pitch of the output buffer
 * @return  IALG_EOK   - Successful
 *          IALG_EFAIL - Unspecified error
 */
template <class Tin, class Tout>
int32_t TIDL_argmaxRefProcess(
  sTIDL_AlgLayer_t     * algLayer,
  sTIDL_ArgMaxParams_t * params,
  sTIDL_DataParams_t   * inDataParams,
  sTIDL_DataParams_t   * outDataParams,
  Tin                  * inPtr,
  Tout                 * outPtr,
  uint32_t               imWidth,
  uint32_t               imHeight,
  uint32_t               numTotRoi,
  uint32_t               inPitch,
  uint32_t               outPitch,
  uint32_t               inChPitch,
  uint32_t               outChPitch
  )
{
  int32_t   j, i0, i1, i2;
  int32_t   status = IALG_EOK;
  Tin   maxVal,currVal;
  uint32_t  maxIdx;
  Tin   *inData      = ((Tin *)inPtr);
  Tout   *outData    = ((Tout *)outPtr);

  OPENACC(data copyin(inData[:1+(numTotRoi-1)*params->numChannels*inChPitch + ((params->numChannels-1)*inChPitch) + ((imHeight-1)*inPitch) + (imWidth-1)]) \
               copyout(outData[:1+(numTotRoi-1)*outChPitch + (imHeight-1)*outPitch + (imWidth-1)]))
  OPENACC(parallel loop collapse(3))
  for(j = 0; j < numTotRoi; j++)
  {
    for(i0 = 0; i0 < imHeight; i0++)
    {
      for(i1 = 0; i1 < imWidth; i1++)
      {
        maxIdx = 0;
        OPENACC(loop)
        for(i2 = 0; i2 < params->numChannels; i2++)
        {
          currVal = inData[j*params->numChannels*inChPitch + i2*inChPitch + i0*inPitch + i1];
          if(i2 == 0)
          {
            maxVal = currVal;
          }
#if (FIND_FIRST_MAX)
          if(currVal > maxVal)
#else
          if(currVal >= maxVal)
#endif
          {
            maxVal = currVal;
            maxIdx = i2;
          }
        }
        outData[j*outChPitch + i0*outPitch + i1] = maxIdx;
      }
    }
  }
  TIDL_enableL1DandL2CacheWb();
  return status;
}

static int32_t TIDL_commonArgmaxRefProcess(sTIDL_Network_t      * net,
                              sTIDL_AlgLayer_t     * algLayer,
                              sTIDL_Layer_t        * tidlLayer,
                              void                 * inPtrs[],
                              void                 * outPtrs[],
                              int32_t                flowCtrl,
                              int32_t                layerIdx)
{
  int32_t   status = IALG_EOK;
  uint8_t  (*inPtr)[]     = (uint8_t (*)[])(inPtrs[0]);
  uint8_t  (*outPtr)[]    = (uint8_t (*)[])(outPtrs[0]);

  sTIDL_ArgMaxParams_t  * params    = &tidlLayer->layerParams.argMaxParams;
  sTIDL_DataParams_t *inDataParams  = &net->TIDLLayers[algLayer->inLayerIdx[0]].outData;
  sTIDL_DataParams_t *outDataParams = &tidlLayer->outData;

  uint32_t inElmtSize     = TIDL_getDatElementSize(inDataParams->elementType);
  uint32_t outElmtSize    = TIDL_getDatElementSize(tidlLayer->outData.elementType);
  uint32_t imWidth        = inDataParams->dimValues[TIDL_DIM_WIDTH];
  uint32_t imHeight       = inDataParams->dimValues[TIDL_DIM_HEIGHT];
  uint32_t numTotRoi      = inDataParams->dimValues[TIDL_DIM_BATCH];
  uint32_t inPitch        = inDataParams->pitch[TIDL_LINE_PITCH];
  uint32_t inChPitch      = inDataParams->pitch[TIDL_CHANNEL_PITCH];
  uint32_t outPitch       = tidlLayer->outData.pitch[TIDL_LINE_PITCH];
  uint32_t outChPitch     = tidlLayer->outData.pitch[TIDL_CHANNEL_PITCH];

  if (inDataParams->elementType == TIDL_SignedChar)
  {
    status = TIDL_argmaxRefProcess(algLayer,
      params,
      inDataParams,
      outDataParams,
      (int8_t *)inPtr,
      (uint8_t *)outPtr,
      imWidth,
      imHeight,
      numTotRoi,
      inPitch, outPitch, inChPitch, outChPitch);
  }
  else if (inDataParams->elementType == TIDL_UnsignedChar)
  {
    status = TIDL_argmaxRefProcess(algLayer,
      params,
      inDataParams,
      outDataParams,
      (uint8_t *)inPtr,
      (uint8_t *)outPtr,
      imWidth,
      imHeight,
      numTotRoi,
      inPitch, outPitch, inChPitch, outChPitch);
  }
  else if (inDataParams->elementType == TIDL_SignedShort)
  {
    status = TIDL_argmaxRefProcess(algLayer,
      params,
      inDataParams,
      outDataParams,
      (int16_t *)inPtr,
      (uint16_t *)outPtr,
      imWidth,
      imHeight,
      numTotRoi,
      inPitch, outPitch, inChPitch, outChPitch);
  }
  else if (inDataParams->elementType == TIDL_UnsignedShort)
  {
    status = TIDL_argmaxRefProcess(algLayer,
      params,
      inDataParams,
      outDataParams,
      (uint16_t *)inPtr,
      (uint16_t *)outPtr,
      imWidth,
      imHeight,
      numTotRoi,
      inPitch, outPitch, inChPitch, outChPitch);
  }
  else if (inDataParams->elementType == TIDL_SinglePrecFloat)
  {
    status = TIDL_argmaxRefProcess(algLayer,
      params,
      inDataParams,
      outDataParams,
      (float32_tidl *)inPtr,
      (float32_tidl *)outPtr,
      imWidth,
      imHeight,
      numTotRoi,
      inPitch, outPitch, inChPitch, outChPitch);
    tidlLayer->outData.minTensorValue = 0;
    tidlLayer->outData.maxTensorValue = inDataParams->dimValues[TIDL_DIM_NUMCH]-1;
  }
  else
  {
    tidl_printf(0,"TIDL_ArgMaxLayer in elementType is  Not suported !!!\n ");
  }
  if((flowCtrl & TIDL_FLOW_CTRL_REF_STAT) == TIDL_FLOW_CTRL_REF_STAT)
  {
    // TODO: Need to remove dependance in intAlgHandle from all functions
    TIDL_Obj intAlgObj;
    TIDL_CreateParams createParams;

    createParams.net = net;
    intAlgObj.createParams = (TIDL_CreateParams *) &createParams;
    TIDL_UpdateScaleFactors(&intAlgObj, layerIdx, 1, 0, (inDataParams->dimValues[TIDL_DIM_NUMCH]-1));
  }

  return status;
}

int32_t TIDL_argmaxProcessNew(TIDL_NetworkCommonParams *commonParams,
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
    status = TIDL_commonArgmaxRefProcess(commonParams->net,
                              algLayer,
                              tidlLayer,
                              inPtrs,
                              outPtrs,
                              commonParams->createParams->flowCtrl,
                              layerIdx);
  }

  return status;
}

