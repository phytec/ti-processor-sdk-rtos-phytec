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
@file    tidl_alg.c
@brief   This file contains TILD alg inteface implementation
@version 0.1 (Oct 2016) : Initial version [ADK]
@version 0.5 (Jan 2017) : Cleaned up [ADK]
----------------------------------------------------------------------------
*/
#include "tidl_alg_int.h"
#include "tidl_types.h"
#include "tidl_layer.h"
#include "itidl_ti.h"
#include "perfsim.h"
#include "tidl_priv_algo.h"
#include "tidl_deviceInfo.h"
#include "tidl_device_functions.h"
#include "tidl_device_utils.h"
#include "tidl_alg_utils.h"
#include "tidl_commonUtils.h"
#include "tidl_innerProduct_mma.h"

using namespace c7x;
#include "gc.h"
#include "gc_helper.h"

#include "math.h"
#include "float.h"


#define TIDL_L3_SCRATCH_SIZE (128U*1024U)
#define TIDL_L3_MSMC_DDR_STAGING_MEM  ( 1025U * 1024U)

#define USE_16BIT_BIAS_FOR_8BIT_MODE (1)
#define TIDL_ALG_INPUT             (0)
#define TIDL_ALG_OUTPUT            (1)

void TIDL_AllocatePtr(intptr_t basePtr,
                         int32_t *currOffset,
                         int32_t size,
                         int32_t alignMent,
                         void ** assignPtr)
{
  intptr_t outPtr = 0;
  int32_t offset = *currOffset;
  offset = TIDL_ALIGN_CEIL(offset, alignMent);

  if ( basePtr != 0 )
  {
    outPtr = basePtr + offset;
    *assignPtr = (void *)outPtr;
  }
  else
  {
    *assignPtr = NULL;
  }

  *currOffset  = offset + size;

  return;
}



 void TIDL_getMMAv2_ScaleAndShift(float scaleRatio, uint8_t *scale, uint8_t *shift)
{
  int32_t shiftBits = 0;
  float newScaleRatio = scaleRatio;
  //int32_t mmaV2MaxScale = 255;
  //Check if scaleRatio > 255
  if(scaleRatio > 255)
  {
    printf("TIDL_getMMAv2_ScaleAndShift: ScaleRatio exceeds representation capability\n");
  }
  //Since exponent goes only in one direction, repeated multiplication with 2 is performed till it exceeds the max range.
  while(1)
  {
    newScaleRatio *= 2;
    if(shiftBits >= 40 )
    {
      break; //Max capability of a right shift of 40 for 8-bit.
    }
    else if(newScaleRatio > 255.0)
    {
      newScaleRatio /= 2;
      break;
    }
    shiftBits++;
  }

  *shift        = shiftBits;
  /*Add nudge of 0.5 to round correctly:*/
  *scale        = (uint8_t) (newScaleRatio + 0.5);
}

void TIDL_getMMAv2_EltwiseScalesAndShift(float scale1, float scale2, uint8_t *intScale1, uint8_t *intScale2,uint8_t *cScale, uint8_t *shift)
{
  float fMax;
  float fMin;
  int32_t maxScaleID;
  /*TIDL's MMA based eltwise addition kernel has a restriction that the scales in the A vector and B panel need to be 7-bit,
    hence the max ceiling is set to 127U (0b1111111)*/
  uint8_t fScaleVal = 127U;
  if(scale1 > scale2)
  {
    maxScaleID = 0;
    fMax = scale1;
    fMin = scale2;
  }
  else
  {
    maxScaleID = 1;
    fMax = scale2;
    fMin = scale1;
  }
  float fVal = fMax/(float)fScaleVal;
  uint8_t iMin;
  TIDL_getMMAv2_ScaleAndShift(fVal,cScale,shift);
  float commonFactor = (*cScale)/(pow(2,(*shift)));
  iMin = floor(fMin/commonFactor);
  /*Add nudge of 0.5 to round correctly:*/
  if(maxScaleID == 0)
  {
    *intScale1 = fScaleVal + 0.5;
    *intScale2 = iMin + 0.5;
  }
  else
  {
    *intScale1 = iMin + 0.5;
    *intScale2 = fScaleVal + 0.5;
  }

}

/**
 * @brief This function fills padded rows with zero
 *
 * @param ptr : Buffer pointer that needs pad filling
 * @param bufInfo : Properties of the buffer to be filled
 * @param width : Width of the buffer
 * @param height : Height of the buffer
 * @param linePitch : Pitch of the buffer between two rows
 * @param channelPitch :Pitch of the buffer between two channels
 * @param dmautilsContext : Memory to store DMA properties
 * @param zeroVec : Vector containing zeros for row filling
 * @param isPadDmaPending : Flag to indicate the DMA trigger status
 * @param trMem : Memory to store DMA trasfer properties
 * @param elementType : The element type of the pad buffer
 * @param paddingType : Indicates the type of padding to be done
 * @param padLayerParams : Layer level parameters of the Pad layer
 * @param outDataPadH : Pad height of the output buffer
 * @param outDataPadW : Pad Width of the output buffer
 * @param isFlowCtrlNatc : Flag to indicate the flow status
 * @param perChannelPadValPtr : Pointer indicating Pad values per channel
 * @return int32_t : retuns the status of Pad filling
 */
int32_t  TIDL_FillPaddedRows(uint8_t * restrict ptr,
                                     sBufferInfo_t * bufInfo,
                                     int32_t width,
                                     int32_t height,
                                     int32_t numBatches,
                                     int32_t linePitch,
                                     int32_t channelPitch,
                                     uint32_t batchPitch,
                                     void * dmautilsContext,
                                     uint8_t *zeroVec,
                                     uint8_t *isPadDmaPending,
                                     uint8_t * trMem,
                                     int32_t elementType,
                                     int32_t paddingType,
                                     void * padLayerParams,
                                     int32_t outDataPadH,
                                     int32_t outDataPadW,
                                     int32_t padFillValue,
                                     int32_t isFlowCtrlNatc,
                                     void * perChannelPadValPtr)
{

  int32_t i, j, k;
  int32_t retVal = IALG_EOK;
  int32_t elementSize = TIDL_getDatElementSize(elementType);

  int32_t orgBufHeight = bufInfo->bufHeight;

  bufInfo->bufHeight = bufInfo->bufHeight / numBatches;

  /* We have allocated only 1k of zero vector so maximum supported offset filling via dma is 1k */
  if(((dmautilsContext == NULL) || (bufInfo->accessoffset > ((int32_t)TIDL_1K_ZERO_VECTOR))) && (TIDL_PADDING_TYPE_PAD_LAYER != paddingType)
     && (TIDL_PADDING_TYPE_PAD_LAYER_BOTH != paddingType) && (TIDL_PADDING_TYPE_PAD_LAYER_TOP != paddingType) && (TIDL_PADDING_TYPE_PAD_LAYER_BOTTOM != paddingType))
  {
    for(k=0; k<numBatches; k++)
    {
    for(j=0; j<bufInfo->bufHeight; j++)
    {
      if(TIDL_PADDING_TYPE_BOTH == paddingType)
      {
        for(i=0; i<bufInfo->accessoffset; i++)
        {
          ptr[(j*channelPitch) + i] = padFillValue;
          ptr[(j*channelPitch) + i + (bufInfo->activeBufWidth- bufInfo->accessoffset)] = padFillValue;
        }
      }
      else if(TIDL_PADDING_TYPE_TOP == paddingType)
      {
        for(i=0; i<bufInfo->accessoffset; i++)
        {
          ptr[j*channelPitch + i] = padFillValue;
        }
      }
      else if(TIDL_PADDING_TYPE_BOTTOM == paddingType)
      {
        for(i=0; i<bufInfo->accessoffset; i++)
        {
          ptr[j*channelPitch + i + ((outDataPadH + height)*linePitch)] = padFillValue;
        }
      }
      else if((TIDL_PADDING_TYPE_TOP_LEFT    == paddingType) ||
              (TIDL_PADDING_TYPE_ST_TOP_LEFT == paddingType))
      {
        for(i=0; i<bufInfo->padCReq*elementSize; i++)
        {
          ptr[j*channelPitch+bufInfo->accessoffset-(bufInfo->padCReq*elementSize) + i] = padFillValue;
        }
      }
      else
      {
        /* Do nothing */
      }
    }
    ptr = ptr + batchPitch;
    }
  }
  else
  {
    for(k=0; k<numBatches; k++)
    {
      TIDL_DspFillPaddedRows((ptr)+(k*batchPitch), bufInfo, width, height, linePitch,channelPitch,
                                     dmautilsContext, zeroVec, isPadDmaPending, trMem,
                                     elementType, paddingType, padLayerParams, outDataPadH, outDataPadW, isFlowCtrlNatc, perChannelPadValPtr);

      if(k < (numBatches-1))
      {
        DmaUtilsAutoInc3d_wait(dmautilsContext, TIDL_DMA_CHANNEL_MEMCPY);
        (void)DmaUtilsAutoInc3d_deconfigure(dmautilsContext, TIDL_DMA_CHANNEL_MEMCPY, trMem, 1);
      }
    }
  }
  bufInfo->bufHeight = orgBufHeight;
  return retVal;
}

/**
 * @brief This function fills padded columns with zero
 *
 * @param ptr : Buffer pointer that needs pad filling
 * @param bufInfo : Properties of the buffer to be filled
 * @param width : Width of the buffer
 * @param height : Height of the buffer
 * @param linePitch : Pitch of the buffer between two rows
 * @param channelPitch :Pitch of the buffer between two channels
 * @param elementType : The element type of the pad buffer
 * @param flowCtrl : Flag to indicate the flow status
 * @param paddingType : Indicates the type of padding to be done
 * @param padLayerParams : Layer level parameters of the Pad layer
 * @param outDataPadH : Pad height of the output buffer
 * @param outDataPadW : Pad Width of the output buffer
 * @param perChannelPadValPtr : Pointer indicating Pad values per channel
 * @return int32_t : retuns the status of Pad filling
 */
 int32_t  TIDL_FillPaddedCols(uint8_t * ptr, const sBufferInfo_t * bufInfo, int32_t width, int32_t height, int32_t numBatches,
                                      int32_t linePitch, int32_t channelPitch, int32_t batchPitch, int32_t elementType, int32_t flowCtrl, int32_t paddingType,
                                      void * padLayerParams, int32_t outDataPadH, int32_t outDataPadW,int32_t padFillValue, void * perChannelPadValPtr)
{
  int32_t i;
  int32_t ptrOffset;
  int32_t heightToPad;
  int32_t elementSize = TIDL_getDatElementSize(elementType);

  ptrOffset = bufInfo->accessoffset - (bufInfo->padC * elementSize);
  heightToPad = height + bufInfo->padR;
  int32_t numColPad = bufInfo->padC * elementSize;

  int32_t isZeroPaddingNeeded = 0;
  if(paddingType == TIDL_PADDING_TYPE_PAD_LAYER)
  {
    sTIDL_PadLayerParams_t * padParams = (sTIDL_PadLayerParams_t *)padLayerParams;
    int32_t padLayerPadT = padParams->padT;
    int32_t padLayerPadL = padParams->padL;
    int32_t padLayerPadR = padParams->padR;
    ptrOffset = ((linePitch * (padLayerPadT + outDataPadH)) - padLayerPadR) * elementSize; //bufInfo->accessoffset - (bufInfo->padC * elementSize);
    numColPad = (outDataPadW + padLayerPadL + padLayerPadR) * elementSize;
    if(padParams->padType == TIDL_PadZero)
    {
      isZeroPaddingNeeded = 1;
    }
  }

  if(((flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) == TIDL_FLOW_CTRL_REF_ONLY) ||
      ((flowCtrl & TIDL_FLOW_CTRL_DSP_NATC) == TIDL_FLOW_CTRL_DSP_NATC))
  {
    if((paddingType == TIDL_PADDING_TYPE_PAD_LAYER) && isZeroPaddingNeeded == 0)
    {
      /* Don't do 0 padding for pad layer if layer has non zero pad requirement */
      /* Non zero padding for NATC done as part of REF flow itself */
    }
    else
    {
      int32_t  j, k, n, m;
      for (n = 0; n < numBatches; n++)
      {
        for (j = 0; j < bufInfo->bufHeight; j++)
        {
          for (i = 0; i < heightToPad; i++)
          {
            for (k = 0; k < numColPad; k++)  //bufInfo->padC*elementSize
            {
              ptr[ptrOffset + n*batchPitch + j*channelPitch + i*linePitch*elementSize + k] = (int8_t)padFillValue;
            }
            if (paddingType == TIDL_PADDING_TYPE_PAD_LAYER && i == heightToPad -1)
            {
              /** Pad right of the last line is not filled yet*/
              int32_t curOffset = ptrOffset + n*batchPitch + j*channelPitch + (i+1)*linePitch*elementSize;
              int32_t padR = ((sTIDL_PadLayerParams_t *)padLayerParams)->padR;
              for (m = 0;m < padR;m++)
              {
                ptr[curOffset + m] = (int8_t)padFillValue;
              }
            }
          }
        }
      }
    }
  }
  else
  {

    if(TIDL_PADDING_TYPE_PAD_LAYER == paddingType)
    {
      for (int32_t n = 0; n < numBatches; n++)
      {
      if(elementType == TIDL_UnsignedChar)
      {
        uchar_vec padVector = (uchar_vec)0;
        TIDL_DspFillPaddedColsPadLayer((uint8_t *)ptr+ n*batchPitch, padVector, bufInfo, width, height, linePitch, channelPitch, elementSize, flowCtrl,
                                    paddingType, padLayerParams, outDataPadH, outDataPadW, perChannelPadValPtr);
      }
      else if(elementType == TIDL_SignedChar)
      {
        char_vec padVector = (char_vec)0;
        TIDL_DspFillPaddedColsPadLayer((int8_t *)ptr+ n*batchPitch, padVector, bufInfo, width, height, linePitch, channelPitch, elementSize, flowCtrl,
                                    paddingType, padLayerParams, outDataPadH, outDataPadW, perChannelPadValPtr);
      }
      else if(elementType == TIDL_UnsignedShort)
      {
        ushort_vec padVector = (ushort_vec)0;
        TIDL_DspFillPaddedColsPadLayer((uint16_t *)(ptr+ n*batchPitch), padVector, bufInfo, width, height, linePitch, channelPitch, elementSize, flowCtrl,
                                    paddingType, padLayerParams, outDataPadH, outDataPadW, perChannelPadValPtr);
      }
      else if(elementType == TIDL_SignedShort)
      {
        short_vec padVector = (short_vec)0;
        TIDL_DspFillPaddedColsPadLayer((int16_t *)(ptr+ n*batchPitch), padVector, bufInfo, width, height, linePitch, channelPitch, elementSize, flowCtrl,
                                    paddingType, padLayerParams, outDataPadH, outDataPadW, perChannelPadValPtr);
      }
      }
    }
    else
    {
      TIDL_DspFillPaddedCols((uint8_t*)ptr, bufInfo, width, height, linePitch, channelPitch, elementSize, flowCtrl,
                                    paddingType, padLayerParams, outDataPadH, outDataPadW, perChannelPadValPtr, numBatches, batchPitch);
    }
  }
  return IALG_EOK;
}



/**
----------------------------------------------------------------------------
@ingroup    TIDL_UTILS
@fn         TIDL_getMemoryChunkFromSysmem
@brief      Function gives you a pointer to system scratch memory based on user given input
            parameters

@param      sysMems : sysMem Handle
@param      size    : Number of bytes to be allocated
@param      alignment  : Alignment required for the pointer
@param      space     : in which memory memory is required. Refer \ref eSysMemScratch for valid values
@param      attribute : IALG_PERSIST/IALG_SCRATCH, note that all persistent memory should be allocated
                       in the beginning
@remarks    None
@return     Pointer to the allocated memory. NULL if there is not enough memory available
----------------------------------------------------------------------------
*/
void * TIDL_getMemoryChunkFromSysmem(sTIDL_sysMemHandle_t sysMems[TIDL_SYSMEM_MAX],
  uint32_t size, uint32_t alignment, uint32_t space, uint32_t attribute)
{
  uint8_t  *mem ;
  uint8_t * ptrCurr = &((uint8_t*)sysMems[space].base)[sysMems[space].offset];
  int32_t alignmentBytes;
  int32_t offsetOrig = sysMems[space].offset;
  if(alignment > 0U)
  {
    alignmentBytes = (int32_t)(((((uintptr_t)ptrCurr) + alignment - 1U) & (~(alignment - 1U))) - ((uintptr_t)ptrCurr));
  }
  else
  {
    alignmentBytes = 0;
  }
  if( (sysMems[space].size - sysMems[space].offset) < (alignmentBytes + (int32_t)size))
  {
    /* ----------------------------------------------------*/
    /* This scenario arrives if remaining space in SL2     */
    /* is not sufficient for the requested chunk of memory */
    /* Control never hits below  instruction at run-time   */
    /* This while(1) is kept here to catch the SL2 memory  */
    /* insufficiency during development phase              */
    /* ----------------------------------------------------*/
    /* Removing while loop. If a memory is not avilable then
     * test app can take action to allocate this memory from
     * DDR instead of just getting stuck in while(1)
    */
    /*while(1){
        ;
    }*/
    mem = NULL;
  }
  else
  {
    mem                 = &ptrCurr[alignmentBytes];
    sysMems[space].offset += (alignmentBytes   + (int32_t)size);
    if ( attribute == (uint32_t)IALG_PERSIST )
    {
      sysMems[space].base = &mem[(int32_t)size];
      sysMems[space].size  = sysMems[space].size - (offsetOrig + alignmentBytes   + (int32_t)size);
      sysMems[space].offset = 0;

    }
  }
  return mem;
}

/**
----------------------------------------------------------------------------
@ingroup    TIDL_UTILS
@fn         TIDL_resetSysmem
@brief      Function gives clears all the memories present in sysMem

@param      sysMems : sysMem Handle
@remarks    None
@return     None
----------------------------------------------------------------------------
*/
void TIDL_resetSysmem(sTIDL_sysMemHandle_t sysMems[TIDL_SYSMEM_MAX])
{
  uint32_t i;

  for ( i = 0; i < TIDL_SYSMEM_MAX; i++)
  {
    sysMems[i].offset = 0;
  }

  return;
}
/**
----------------------------------------------------------------------------
@ingroup    TIDL_IVISION_SUPPORT
@fn         TIDL_isOutDataFlat
@brief      Function to Check whether current out data buffer needs to
            flattened

@param      pTIDLNetStructure : Base pointer for net
@param      dataId            : Current data buffer ID
@remarks    None
@return     IALG_EOK   - Successful
            IALG_EFAIL - Unspecified error
----------------------------------------------------------------------------
*/
int32_t TIDL_isOutDataFlat(const sTIDL_Network_t * pTIDLNetStructure,
int32_t dataId)
{
  int32_t i,j;
  int32_t status = 1;
  for (i = 0 ; i < pTIDLNetStructure->numLayers; i++)
  {
    for (j = 0; j < pTIDLNetStructure->TIDLLayers[i].numInBufs; j++)
    {
      if((pTIDLNetStructure->TIDLLayers[i].layerType != TIDL_InnerProductLayer) &&
          (pTIDLNetStructure->TIDLLayers[i].inData[j] == dataId))
      {
        status = 0;
        break;
      }
    }
    if(status==0)
    {
      break;
    }
  }

  return status;
}
/**
----------------------------------------------------------------------------
@ingroup    TIDL_IVISION_SUPPORT
@fn         TIDL_getDatElementSize
@brief      Function to gives the element size based on elementType

@param      elementType            : Current data element type
@remarks    None
@return     IALG_EOK   - Successful
            IALG_EFAIL - Unspecified error
----------------------------------------------------------------------------
*/
int32_t TIDL_getDatElementSize(int32_t elementType)
{
  int32_t size = 1;
  if ((elementType == TIDL_SignedChar) || (elementType == TIDL_UnsignedChar))
  {
    size = 1;
  }
  else if ((elementType == TIDL_SignedShort) || (elementType == TIDL_UnsignedShort))
  {
    size = 2;
  }
  else if ((elementType == TIDL_SinglePrecFloat) || (elementType == TIDL_UnsignedWord) || (elementType == TIDL_SignedWord))
  {
    size = 4;
  }
  else if ((elementType == TIDL_UnsignedDoubleWord) || (elementType == TIDL_SignedDoubleWord))
  {
    size = 8;
  }
  else
  {
    size = 1;
  }
  return size;
}
/**
----------------------------------------------------------------------------
@ingroup    TIDL_IVISION_SUPPORT
@fn         TIDL_getDatElementSign
@brief      Function to gives the element sign based on elementType

@param      elementType            : Current data element type
@remarks    None
@return     IALG_EOK   - Successful
            IALG_EFAIL - Unspecified error
----------------------------------------------------------------------------
*/
int32_t TIDL_getDatElementSign(int32_t elementType)
{
  int32_t sign = 1;
  if ((elementType == TIDL_SignedChar) || (elementType == TIDL_SignedShort) || (elementType == TIDL_SignedWord) || (elementType == TIDL_SinglePrecFloat))
  {
    sign = 1;
  }
  else
  {
    sign = 0;
  }
  return sign;
}

/**
----------------------------------------------------------------------------
@ingroup    TIDL_IVISION_SUPPORT
@fn         TIDL_isOutDataBuff
@brief      Function to Check whether current data buffer is Output from
            current algorithm instance

@param      pTIDLNetStructure : Base pointer for net
@param      dataId            : Current data buffer ID
@param      layersGroupId     : current algorithm instance Group ID
@remarks    None
@return     IALG_EOK   - Successful
            IALG_EFAIL - Unspecified error
----------------------------------------------------------------------------
*/
int32_t TIDL_isOutDataBuff(const sTIDL_Network_t * pTIDLNetStructure,
int32_t dataId, int32_t layersGroupId)
{
  int32_t i,j;
  int32_t status = 0;
  if(pTIDLNetStructure->numLayers >= TIDL_NUM_MAX_LAYERS)
  {
    status = -1;
  }
  else
  {
    for (i = 0 ; i < pTIDLNetStructure->numLayers; i++)
    {
      for (j = 0; j < pTIDLNetStructure->TIDLLayers[i].numInBufs; j++)
      {
        if((pTIDLNetStructure->TIDLLayers[i].layersGroupId != layersGroupId) &&
            (pTIDLNetStructure->TIDLLayers[i].inData[j] == dataId))
        {
          status = 1;
          break;
        }
      }
      if(status == 1)
      {
        break;
      }
    }
  }

  return status;
}

/**
----------------------------------------------------------------------------
@ingroup    TIDL_IVISION_SUPPORT
@fn         TIDL_isInDataBuff
@brief      Function to Check whether current data buffer is Input to
            current algorithm instance

@param      pTIDLNetStructure : Base pointer for net
@param      dataId            : Current data buffer ID
@param      layersGroupId     : current algorithm instance Group ID
@remarks    None
@return     IALG_EOK   - Successful
            IALG_EFAIL - Unspecified error
----------------------------------------------------------------------------
*/
int32_t TIDL_isInDataBuff(const sTIDL_Network_t * pTIDLNetStructure,
int32_t dataId, int32_t layersGroupId)
{
  int32_t i,j;
  int32_t status = 0;
  for (i = 0 ; i < pTIDLNetStructure->numLayers; i++)
  {
    for (j = 0; j < pTIDLNetStructure->TIDLLayers[i].numOutBufs; j++)
    {
      if((pTIDLNetStructure->TIDLLayers[i].layersGroupId != layersGroupId) &&
          (pTIDLNetStructure->TIDLLayers[i].outData.dataId == dataId))
      {
        status = 1;
        break;
      }
    }
    if(status == 1)
    {
      break;
    }
  }

  return status;
}


/**
----------------------------------------------------------------------------
@ingroup    TIDL_IVISION_SUPPORT
@fn         TIDL_getLayerNum
@brief      Function to get the layer index of data ID

@param      pTIDLNetStructure : Base pointer for net
@param      dataId            : Current data buffer ID
@remarks    None
@return     IALG_EOK   - Successful
IALG_EFAIL - Unspecified error
----------------------------------------------------------------------------
*/
int32_t TIDL_getLayerNum(const sTIDL_Network_t * pTIDLNetStructure, int32_t dataId)
{
  int32_t i, j;
  int32_t layerNum = -1;
  for (i = 0; i < pTIDLNetStructure->numLayers; i++)
  {
    for (j = 0; j < pTIDLNetStructure->TIDLLayers[i].numOutBufs; j++)
    {
      if (pTIDLNetStructure->TIDLLayers[i].outData.dataId == dataId)
      {
        layerNum = i;
      }
    }
  }
  return layerNum;
}

/**
----------------------------------------------------------------------------
@ingroup    TIDL_IVISION_SUPPORT
@fn         TIDL_getDataParams
@brief      Function to Check whether current data buffer is Input to
            current algorithm instance

@param      pTIDLNetStructure : Base pointer for net
@param      dataId            : Current data buffer ID
@remarks    None
@return     Address of the Data Params Structure Base - If Successful
            NULL - If Faile - Requested dataID Not found
----------------------------------------------------------------------------
*/
sTIDL_DataParams_t * TIDL_getDataParams(sTIDL_Network_t * pTIDLNetStructure,
int32_t dataId)
{
  int32_t i,j;
  sTIDL_DataParams_t *dataPtr = NULL;
  if(pTIDLNetStructure->numLayers < TIDL_NUM_MAX_LAYERS)
  {
    for (i = 0 ; i < pTIDLNetStructure->numLayers; i++)
    {
      for (j = 0; j < pTIDLNetStructure->TIDLLayers[i].numOutBufs; j++)
      {
        if(pTIDLNetStructure->TIDLLayers[i].outData.dataId == dataId)
        {
          dataPtr = &(pTIDLNetStructure->TIDLLayers[i].outData);
          break;
        }
      }
      if(dataPtr != NULL)
      {
        break;
      }
    }
  }
  return(dataPtr);
}
/**
----------------------------------------------------------------------------
@ingroup    TIDL_IVISION_SUPPORT
@fn         TIDL_getDataBuff
@brief      Function to get base buffer pointer from Ivision buffer

@param      bufDesc      : ivision buffer descriptor list
@param      numBufs      : Number of buffers in the list
@param      dataId       : Current data ID
@remarks    None
@return     IALG_EOK   - Successful
            IALG_EFAIL - Unspecified error
----------------------------------------------------------------------------
*/
void * TIDL_getDataBuff(IVISION_BufDesc *bufDesc[], uint32_t numBufs,
int32_t dataId, int32_t numBatches, int32_t pitch[], int32_t isPaded)
{
  int32_t i;
  void * ptr;
  void * retVal = NULL;
  for (i = 0 ; i < (int32_t)numBufs; i++)
  {
    if(bufDesc[i]->bufferId  == dataId)
    {
      IVISION_BufPlanes (*ivisionBufPlane)[]  = (IVISION_BufPlanes (*)[])((void*)(&bufDesc[i]->bufPlanes[0]));

      ptr = (void*)(get_uint8_t_pointer((uint8_t*)(*ivisionBufPlane)[0].buf,
      (((((int32_t)(*ivisionBufPlane)[0].frameROI.topLeft.y) * (int32_t)(*ivisionBufPlane)[0].width)) +
      ((int32_t)(*ivisionBufPlane)[0].frameROI.topLeft.x))));
      retVal =  ptr;
      if((((uint32_t)pitch[TIDL_LINE_PITCH] != (*ivisionBufPlane)[0].width) /*|| ((uint32_t)pitch[TIDL_ROI_PITCH] != ((*ivisionBufPlane)[0].width*(*ivisionBufPlane)[0].height))*/) && (isPaded == 1))
      {
        retVal = NULL;
      }
      if (isPaded == 0)
      {
        pitch[TIDL_LINE_PITCH] = (int32_t)((*ivisionBufPlane)[0].width);
        pitch[TIDL_ROI_PITCH] = (*ivisionBufPlane)[0].width*(*ivisionBufPlane)[0].height / numBatches;
      }
      break;
    }
  }

  return retVal;
}

/**
----------------------------------------------------------------------------
@ingroup    TIDL_IVISION_SUPPORT
@fn         TIDL_getLayerTypeForOutDataID
@brief      Function to Check whether current data buffer is Input to
current algorithm instance

@param      pTIDLNetStructure : Base pointer for net
@param      dataId            : Current data buffer ID
@remarks    None
@return     Address of the Data Params Structure Base - If Successful
NULL - If Faile - Requested dataID Not found
----------------------------------------------------------------------------
*/
int32_t TIDL_getLayerTypeForOutDataID(const sTIDL_Network_t * pTIDLNetStructure,
  int32_t dataId)
{
  int32_t i, j;
  int32_t layerType = TIDL_UnsupportedLayer;
  if (pTIDLNetStructure->numLayers < TIDL_NUM_MAX_LAYERS)
  {
    for (i = 0; i < pTIDLNetStructure->numLayers; i++)
    {
      for (j = 0; j < pTIDLNetStructure->TIDLLayers[i].numOutBufs; j++)
      {
        if (pTIDLNetStructure->TIDLLayers[i].outData.dataId == dataId)
        {
          layerType = pTIDLNetStructure->TIDLLayers[i].layerType;
          break;
        }
      }
      if (layerType != TIDL_UnsupportedLayer)
      {
        break;
      }
    }
  }
  return(layerType);
}

/**
----------------------------------------------------------------------------
@fn         TIDL_getNumConsumersForDataId
@brief      Function to Check number of consumers for given data Id

@param      pTIDLNetStructure : Base pointer for net
@param      dataId            : Indata Id of current layer
@remarks    None
@return     Number of consumers with given data Id
----------------------------------------------------------------------------
*/
int32_t TIDL_getNumConsumersForDataId(const sTIDL_Network_t * pTIDLNetStructure, int32_t dataId)
{
  int32_t numConsumers = 0;
  for(int i = 0; i < pTIDLNetStructure->numLayers; i++)
  {
    for(int j = 0; j < pTIDLNetStructure->TIDLLayers[i].numInBufs; j++)
    {
      if(pTIDLNetStructure->TIDLLayers[i].inData[j] == dataId)
      {
        numConsumers++;
      }
    }
  }
  return numConsumers;
}


/**
 * @brief This function returns the ROI pitch in bytes
 *
 * @param dataParams : This has the parameters of data or kernel buffer
 * @return int32_t :  returns the ROI pitch in bytes
 */
int32_t TIDL_updateDataParamsPitch(const sTIDL_DataParams_t  * dataParams)
{
  int32_t dataSize;
  dataSize = dataParams->dimValues[TIDL_DIM_BATCH]*dataParams->pitch[TIDL_ROI_PITCH] * TIDL_getDatElementSize(dataParams->elementType);

  return (dataSize);
}

/**
 * @brief This function the no of Input and output tensors that requires the
 *        new generic dataflow
 *
 * @param params : Create time parameters for tidl
 * @param layerIdx : Index of the layer to be processed
 * @param numInTensors : no of input tensors that requires the generic dataflow
 * @param numOutTensors : no of output tensors that requires the generic dataflow
 * @return int32_t : returns function status
 */
int32_t TIDL_getNumInOutTensors(const TIDL_CreateParams *params,
                                int32_t layerIdx,
                                int32_t *numInTensors,
                                int32_t *numOutTensors
                                )
{
  int32_t status = IALG_EOK;
  *numInTensors = params->net->TIDLLayers[layerIdx].numInBufs;
  *numOutTensors = params->net->TIDLLayers[layerIdx].numOutBufs;
  sTIDL_Layer_t *tidlLayer = &params->net->TIDLLayers[layerIdx];
  /* For concat layer we process one tensor at a time, so set numInTensors as 1 */
  if(tidlLayer->layerType == TIDL_ConcatLayer)
  {
    *numInTensors = 1;
  }

  if(tidlLayer->layerType == TIDL_DetectionOutputLayer)
  {
    *numInTensors = 1;
  }
  if (*numInTensors > TIDL_MAX_IN_TENSORS)
  {
    tidl_printf(0, "For Eltwise layer num of Inputs are morethan TIDL_MAX_IN_TENSORS supported for New GenericFlow. \n");
    status = IALG_EFAIL; /* TODO : Replace with proper error code */
  }

  return status;
}

/**
 * @brief This function returns the output processing height for the given inHeight
 *
 * @param inHeight : input processing height
 * @param currentLayer : layer properties of the current layer
 * @return int32_t : returns the output processing height
 */
int32_t TIDL_getOutProcHeight(int32_t inHeight, sTIDL_Layer_t *currentLayer, sTIDL_DataParams_t *inDataParams)
{
  int32_t retVal;
  if ((currentLayer->layerType == TIDL_ArgMaxLayer) ||
     (currentLayer->layerType == TIDL_EltWiseLayer) ||
     (currentLayer->layerType == TIDL_BatchNormLayer) ||
     (currentLayer->layerType == TIDL_BatchReshapeLayer) ||
     (currentLayer->layerType == TIDL_ConcatLayer) ||
     (currentLayer->layerType == TIDL_DataConvertLayer) ||
     (currentLayer->layerType == TIDL_ReduceLayer) ||
     (currentLayer->layerType == TIDL_ScatterElementsLayer) ||
     (currentLayer->layerType == TIDL_GatherLayer) ||
     (currentLayer->layerType == TIDL_LayerNormLayer)
    )
  {
    retVal = inHeight;
  }
  else if(currentLayer->layerType == TIDL_PoolingLayer)
  {
    int32_t strideH    = currentLayer->layerParams.poolParams.strideH;
    /* TODO : Need to re-visit this code */
    retVal = TIDL_CEIL_DIV(inHeight, strideH);
  }
  else if(currentLayer->layerType == TIDL_ResizeLayer)
  {
    int32_t resizeFact = currentLayer->layerParams.resizeParams.resizeRatio[3];
    retVal = inHeight * resizeFact;
  }
  else if(currentLayer->layerType == TIDL_ColorConversionLayer)
  {
    retVal = inHeight * 2;
  }
  else if (currentLayer->layerType == TIDL_CustomLayer)
  {
    /* For custom layer if more than one inputs are present it is expected
    that the input to output ratio is 1. If not then it is assumed that input
    to output ratio is same for all input heights */
    float32_tidl outInHeightRatio = 1.0;
    if ( currentLayer->numInBufs == 1 )
    {
      outInHeightRatio = (float32_tidl)currentLayer->outData.dimValues[TIDL_DIM_HEIGHT] /
              (float32_tidl)inDataParams->dimValues[TIDL_DIM_HEIGHT];
    }
    retVal = inHeight * outInHeightRatio;
  }
  else
  {
    tidl_printf(0, "This layer is not currently supported for New GenericFlow. \n");
    retVal = -1;
  }
  return retVal;
}


void TIDL_poolingGetUpdatedPadValues(int32_t Fr, int32_t strideVal, int32_t padH, int32_t strideOffsetMethod, int32_t heightVal, int32_t *topPad, int32_t *bottomPad)
{
  if(strideOffsetMethod == TIDL_StrideOffsetTopLeft)
  {
    if((strideVal == 2) && ((heightVal & 1) == 0))
    {
      if(padH > 0)
      {
        *bottomPad -= 1U;
      }
    }
  }
  else
  {
    if((strideVal == 2) && ((heightVal & 1) == 0))
    {
      if(padH > 0)
      {
        *topPad -= 1U;
      }
    }
  }
}


int32_t TIDL_refGetOutDataSize(const TIDL_NetworkCommonParams *commonParams,
                                const TIDL_LayerSpecificParams *layerSpecificParams,
                                int32_t layerIdx)
{
  int32_t outDataSize;
  outDataSize = TIDL_updateDataParamsPitch(&commonParams->net->TIDLLayers[layerIdx].outData);


  if (commonParams->createParams->optimiseExtMem != TIDL_OptimiseExtMemL0)
  {
    outDataSize = TIDL_findFreeOutBuff(commonParams->createParams, layerIdx,
                                      outDataSize, commonParams->TIDLLayersBufPtr);
    commonParams->TIDLLayersBufPtr->outDataSize[layerIdx] = outDataSize;
  }

  /* If NC information is available then no output data required to be allocated */
  if (layerSpecificParams->workloadUnit != NULL )
  {
    outDataSize = 0;
  }

  return outDataSize;
}

int32_t TIDL_refGetScratchDataSize(const TIDL_NetworkCommonParams *commonParams,
                                          int32_t layerIdx)
{
  int32_t outBatchPitch;
  int32_t scratchDataSize;
  sTIDL_DataParams_t *dataParams = TIDL_getDataParams(commonParams->net,
                                                    commonParams->net->TIDLLayers[layerIdx].inData[0]);

  outBatchPitch    = commonParams->net->TIDLLayers[layerIdx].outData.pitch[TIDL_ROI_PITCH];
  scratchDataSize  = dataParams->dimValues[TIDL_DIM_BATCH] * outBatchPitch *
                     TIDL_getDatElementSize(dataParams->elementType);

  scratchDataSize = (int32_t)((float32_tidl)scratchDataSize * 1.5f); //:TODO: This will not be needed if we use perfsim output during alloc also
  /*Need to implement callbacks for scratchsizes!*/
  if (TIDL_getDatElementSize(commonParams->net->TIDLLayers[layerIdx].outData.elementType) <= 2)
  {
    scratchDataSize *= ((commonParams->net->TIDLLayers[layerIdx].weightsElementSizeInBits + 7) / 8) *  ((int32_t)(sizeof(int32_t)));
  }

  return scratchDataSize;
}



/**
----------------------------------------------------------------------------
@ingroup    TIDL_IVISION_SUPPORT
@fn         TIDL_outBuffAlloc
@brief      Function to Request Memory for layer output Memory

@param      params       : Create Parameters
@param      layerIdx     : Current Layer index in Net structure
@param      memRec       : memory table populated by the Algorithm
@remarks    None
@return     IALG_EOK   - Successful
            IALG_EFAIL - Unspecified error
----------------------------------------------------------------------------
*/
void TIDL_outBuffAlloc(const TIDL_CreateParams *params,
int32_t layerIdx, IALG_MemRec memRec[], int32_t outDataSize)
{

  if((TIDL_isOutDataBuff(params->net,
        params->net->TIDLLayers[layerIdx].outData.dataId,
        params->currLayersGroupId) == 0)
     && (params->net->dataFlowInfo == NULL)
    )
  {
    memRec[ALG_SCRATCH_DATA_BUFF_MEMREC].size += ((uint32_t)outDataSize + 128U);
    //tidl_printf(0," ALG_SCRATCH_DATA_BUFF_MEMREC Size for Layer , %4d,  %8d \n", layerIdx, memRec[ALG_SCRATCH_DATA_BUFF_MEMREC].size);
  }
  return;
}
/**
 * @brief This function requests scratch memory size
 *
 * @param layerIdx : current layer index
 * @param memRec : memory table populated by the Algorithm
 * @param outDataSize : Ouptut buffer size
 */
void TIDL_scratchBufAlloc(int32_t layerIdx, IALG_MemRec memRec[], int32_t outDataSize)
{
  /* Allocate the maximum scratch buf required */
  if ( outDataSize >  (int32_t)memRec[ALG_REF_SCRATCH_BUFF_MEMREC].size )
  {
     memRec[ALG_REF_SCRATCH_BUFF_MEMREC].size = (uint32_t)outDataSize + 128U;
  }

  /*tidl_printf(0," ALG_REF_SCRATCH_BUFF_MEMREC Size for Layer , %4d,  %8d \n", layerIdx, memRec[ALG_REF_SCRATCH_BUFF_MEMREC].size);*/
  return;
}
/**
 * @brief This function initializes scratch memory pointer
 *
 * @param layerIdx : current layer index
 * @param memRec : memory table populated by the Algorithm
 * @param outPtr : ouptut scratch pointer
 * @param outDataSize : Ouptut buffer size
 */
void TIDL_scratchBufInit(int32_t layerIdx,
  const IALG_MemRec memRec[], void ** outPtr, int32_t outDataSize)
{
  *outPtr = memRec[ALG_REF_SCRATCH_BUFF_MEMREC].base;

  return;
}

/**
----------------------------------------------------------------------------
@ingroup    TIDL_IVISION_SUPPORT
@fn         TIDL_findFreeOutBuff
@brief      Function to find the free output buffer used for tjis layer

@param      params       : Create Parameters
@param      layerIdx     : Current Layer index in Net structure
@param      outDataSize  : outDataSize required for this layer
@param      TIDLLayersBuf: Temp structure to store outDataSize and Data Id's
@remarks    None
@return     outDataSize  : outDataSize required for this layer
----------------------------------------------------------------------------
*/
int32_t TIDL_findFreeOutBuff(const TIDL_CreateParams *params,
int32_t layerIdx, int32_t outDataSize, sTIDL_LayerBuf_t *TIDLLayersBufPtr);

int32_t TIDL_findFreeOutBuff(const TIDL_CreateParams *params,
int32_t layerIdx, int32_t outDataSize, sTIDL_LayerBuf_t *TIDLLayersBufPtr)
{
  int32_t outDataId = params->net->TIDLLayers[layerIdx].outData.dataId;
  int32_t ii, jj, kk, skipLoop, curOutDataId = outDataId;
  const sTIDL_Layer_t *TIDLLayer = &params->net->TIDLLayers[layerIdx];
  int32_t currDataSize = outDataSize;
  if((TIDL_isOutDataBuff(params->net,
        outDataId,
        params->currLayersGroupId) == 0))
  {
    if(params->optimiseExtMem == TIDL_OptimiseExtMemL2)
    {
      /* For loop to find an output buffer of same size in previous layers */
      for(ii = (layerIdx - 2); ii > 0; ii--)
      {
        /* To check whether output memory is allocated to this layer */
        if(TIDLLayersBufPtr->outDataSize[ii] != 0)
        {
          /* Compare current layer outDataSize with previous layers */
          if(currDataSize <= TIDLLayersBufPtr->outDataSize[ii])
          {
            /* Found an output buffer of required size in previous layers */
            curOutDataId = params->net->TIDLLayers[ii].outData.dataId;

            skipLoop = 0;
            /* Check whether that layer is used as input to any future layers */
            for(kk = layerIdx; kk < params->net->numLayers; kk++)
            {
              const sTIDL_Layer_t *TIDLLayerCur = &params->net->TIDLLayers[kk];
              for(jj = 0; jj < TIDLLayerCur->numInBufs; jj++)
              {
                int32_t inDataId  =  (int32_t)TIDLLayersBufPtr->newInDataId[kk][jj];
                if((inDataId == curOutDataId) && (skipLoop == 0))
                {
                  /* The output buffer found is still in use for future layers,
                     So, break this loop and continue to find
                     next free output buffer */
                  curOutDataId = outDataId;
                  /* kk = params->net->numLayers; */
                  /* break; */
                  skipLoop = 1;
                }
              }
            }
            /* Free output buffer found, so break from for loop */
            if(curOutDataId != outDataId)
            {
              /* ii = 0; */
              break;
            }
          }
        }
      }
    }
    else /* if(params->optimiseExtMem == TIDL_OptimiseExtMemL1) */
    {
      int32_t outWidth       = TIDLLayer->outData.dimValues[TIDL_DIM_WIDTH];
      int32_t outHeight      = TIDLLayer->outData.dimValues[TIDL_DIM_HEIGHT];
      int32_t numChannels    = TIDLLayer->outData.dimValues[TIDL_DIM_NUMCH];
      int32_t numDim1        = TIDLLayer->outData.dimValues[TIDL_DIM_DIM1];
      int32_t numDim2        = TIDLLayer->outData.dimValues[TIDL_DIM_DIM2];
      int32_t numBatch       = TIDLLayer->outData.dimValues[TIDL_DIM_BATCH]; 
      int32_t outDimSizeMin = numBatch * numDim2 * numDim1 * numChannels;
      int32_t padW = TIDLLayer->outData.padW;
      int32_t padH = TIDLLayer->outData.padH;
      const sTIDL_Layer_t   *TIDLLayerCur;

      /* For loop to find an output buffer of same size in previous layers */
      for(ii = (layerIdx - 2); ii > 0; ii--)
      {
        /* To check whether output memory is allocated to this layer */
        if(TIDLLayersBufPtr->outDataSize[ii] != 0)
        {
          TIDLLayerCur = &params->net->TIDLLayers[ii];
          /* Compare the output elementsize with previous layer */
          if ( TIDL_getDatElementSize(TIDLLayerCur->outData.elementType) == TIDL_getDatElementSize(TIDLLayer->outData.elementType))
          {
            /* Compare width and height with previous layers */
            if((outWidth == TIDLLayerCur->outData.dimValues[TIDL_DIM_WIDTH]) &&
                (outHeight == TIDLLayerCur->outData.dimValues[TIDL_DIM_HEIGHT]))
            {
              /* Compare numChannels with previous layers */
              int64_t outerDimSize = TIDLLayerCur->outData.dimValues[TIDL_DIM_NUMCH] * TIDLLayerCur->outData.dimValues[TIDL_DIM_DIM1]
                                     * TIDLLayerCur->outData.dimValues[TIDL_DIM_DIM2] *TIDLLayerCur->outData.dimValues[TIDL_DIM_BATCH];
              if(outDimSizeMin <= outerDimSize)
              {
                /* Bypass the following layerType check */
                /* Compare layerType with previous layers, re-use output from
                   same layerType, otherwise padding may be differnt. This is
                   because different layers will be processed with different
                   blockwidth and heights, so padding will change.
                */
                if((padW == TIDLLayerCur->outData.padW) && (padH == TIDLLayerCur->outData.padH))
                {
                  /* Found an output buffer of required size in previous layers */
                  curOutDataId = params->net->TIDLLayers[ii].outData.dataId;

                  skipLoop = 0;
                  /* Check whether that layer is used as input to any future layers */
                  for(kk = layerIdx; kk < params->net->numLayers; kk++)
                  {
                    const sTIDL_Layer_t *TIDLLayerCurkk = &params->net->TIDLLayers[kk];
                    for(jj = 0; jj < TIDLLayerCurkk->numInBufs; jj++)
                    {
                      int32_t inDataId  =  (int32_t)TIDLLayersBufPtr->newInDataId[kk][jj];
                      if((inDataId == curOutDataId) && (skipLoop == 0))
                      {
                        /* The output buffer found is still in use for future layers,
                           So, break this loop and continue to find
                           next free output buffer */
                        curOutDataId = outDataId;
                        /* kk = params->net->numLayers; */
                        /* break; */
                        skipLoop = 1;
                      }
                    }
                  }
                  /* Free output buffer found, so break from for loop */
                  if(curOutDataId != outDataId)
                  {
                    /* ii = 0; */
                    break;
                  }
                }
              }
            }
          }
        }
      }
    }

    /* Check the curOutDataId is used as input to SliceLayer */
    skipLoop = 0;
    if(curOutDataId != outDataId)
    {
      for(kk = 0; kk < layerIdx; kk++)
      {
        const sTIDL_Layer_t *TIDLLayerCur = &params->net->TIDLLayers[kk];
        for(jj = 0; jj < TIDLLayerCur->numInBufs; jj++)
        {
          int32_t inDataId  =  (int32_t)TIDLLayersBufPtr->newInDataId[kk][jj];
          if((inDataId == curOutDataId) && (skipLoop == 0))
          {
            if(TIDLLayerCur->layerType == TIDL_SliceLayer)
            {
                /* The output buffer found is used for Slice layers,
                    So can not be re-used */
                curOutDataId = outDataId;
                skipLoop = 1;
            }
          }
        }
      }
    }

    /*
       As the free output buffer found, update the input dataID to point to
       the new output buffer, Also make outDataSize = 0, to not allocate the
       new memory for output(as it can re-use the memory from previous layers)
   */
    if(curOutDataId != outDataId)
    {
      for(kk = layerIdx; kk < params->net->numLayers; kk++)
      {
        const sTIDL_Layer_t *TIDLLayerCur = &params->net->TIDLLayers[kk];
        for(jj = 0; jj < TIDLLayerCur->numInBufs; jj++)
        {
          int32_t inDataId  =  (int32_t)TIDLLayersBufPtr->newInDataId[kk][jj];
          if(inDataId == outDataId)
          {
            TIDLLayersBufPtr->newInDataId[kk][jj] = curOutDataId;
          }
        }
      }
      TIDLLayersBufPtr->newOutDataId[layerIdx][0] = curOutDataId;
      currDataSize = 0;
    }
  }
  else
  {
    currDataSize = 0;
  }
  return currDataSize;
}
/**
----------------------------------------------------------------------------
@ingroup    TIDL_IVISION_SUPPORT
@fn         TIDL_outBuffInit
@brief      Function to Initialize the Layer output Memory

@param      params       : Create Parameters
@param      layerIdx     : Current Layer index in Net structure
@param      dataMemTabOffset : Current Offset in ALG_SCRATCH_DATA_BUFF_MEMREC
@param      memRec       : memory table populated by the Algorithm
@param      outPtr       : Pointer to Layer output Pointers list
@param      outDataSize  : Size of the ouput buffer to be allocated
@remarks    None
@return     IALG_EOK   - Successful
            IALG_EFAIL - Unspecified error
----------------------------------------------------------------------------
*/
void TIDL_outBuffInit(const TIDL_CreateParams *params,
int32_t layerIdx, int32_t *dataMemTabOffset,
const IALG_MemRec memRec[], void ** outPtr, int32_t outDataSize)
{

  if((TIDL_isOutDataBuff(params->net,
        params->net->TIDLLayers[layerIdx].outData.dataId,
        params->currLayersGroupId) == 1) || (params->net->dataFlowInfo != NULL) )
  {
    *outPtr = NULL;
  }
  else
  {
    /* outDataSize is 0 means, not allocate the new memory for output buffer */
    if(outDataSize == 0)
    {
      *outPtr = NULL;
    }
    else
    {
      *outPtr  =  (void*)(get_uint8_t_pointer((uint8_t*)(void*)memRec[ALG_SCRATCH_DATA_BUFF_MEMREC].base, \
      *dataMemTabOffset));
      *dataMemTabOffset += ALIGN_SIZE((int32_t)outDataSize, 128);
    }
    //tidl_printf(0," ALG_SCRATCH_DATA_BUFF_MEMREC Offset for Layer, %4d, %8d \n", layerIdx, *dataMemTabOffset);
  }
  return;
}

/**
----------------------------------------------------------------------------
@ingroup    TIDL_IVISION_SUPPORT
@fn         TIDL_shuffleChannelAlloc
@brief      Function to Request Memory required by the ShuffleChannel layer

@param      params       : Create Parameters
@param      layerIdx     : Current Layer index in Net structure
@param      memRec       : memory table populated by the Algorithm
@remarks    None
@return     IALG_EOK   - Successful
            IALG_EFAIL - Unspecified error
----------------------------------------------------------------------------
*/
int32_t TIDL_shuffleChannelAlloc(const TIDL_CreateParams *params,
int32_t layerIdx, IALG_MemRec *memRec, sTIDL_LayerBuf_t *TIDLLayersBufPtr)
{
  int32_t outDataSize = TIDL_updateDataParamsPitch(&params->net->TIDLLayers[layerIdx].outData);
  int32_t scratchDataSize = outDataSize * ( ((int32_t)(sizeof(int32_t))));

  if(params->optimiseExtMem != TIDL_OptimiseExtMemL0)
  {
    outDataSize = TIDL_findFreeOutBuff(params, layerIdx,
                                       outDataSize, TIDLLayersBufPtr);
    TIDLLayersBufPtr->outDataSize[layerIdx] = outDataSize;
  }
  TIDL_outBuffAlloc(params, layerIdx, memRec, outDataSize);
  TIDL_scratchBufAlloc(layerIdx, memRec, scratchDataSize);

  return 0;
}

/**
----------------------------------------------------------------------------
@ingroup    TIDL_IVISION_SUPPORT
@fn         TIDL_shuffleChannelInit
@brief      Function to Initialize the ShuffleChannel layer specific params

@param      params       : Create Parameters
@param      layerIdx     : Current Layer index in Net structure
@param      algLayer     : Pointer to current layer algorithm specific params
@param      paramMemTabOffset : Current Offset in ALG_LAYERS_PARAMS_BUFF_MEMREC
@param      dataMemTabOffset : Current Offset in ALG_SCRATCH_DATA_BUFF_MEMREC
@param      memRec       : memory table populated by the Algorithm
@param      outPtr       : Pointer to Layer output Pointers list
@param      TIDLLayersBuf: Temp structure to store outDataSize and Data Id's
@remarks    None
@return     IALG_EOK   - Successful
            IALG_EFAIL - Unspecified error
----------------------------------------------------------------------------
*/
int32_t TIDL_shuffleChannelInit(
const TIDL_CreateParams *params, int32_t layerIdx,
sTIDL_AlgLayer_t * algLayer, int32_t *paramMemTabOffset,
int32_t *dataMemTabOffset, const IALG_MemRec *memRec,
void ** outPtr, sTIDL_LayerBuf_t *TIDLLayersBufPtr)
{
  int32_t status = IALG_EOK;

  int32_t outDataSize = TIDL_updateDataParamsPitch(&params->net->TIDLLayers[layerIdx].outData);
  int32_t scratchDataSize = outDataSize * ( ((int32_t)(sizeof(int32_t))));

  if(params->optimiseExtMem != TIDL_OptimiseExtMemL0)
  {
    outDataSize = TIDL_findFreeOutBuff(params, layerIdx,
                                       outDataSize, TIDLLayersBufPtr);
    TIDLLayersBufPtr->outDataSize[layerIdx] = outDataSize;
  }
  if(status == IALG_EOK)
  {
    TIDL_outBuffInit(params, layerIdx, dataMemTabOffset, memRec, outPtr, outDataSize);
    TIDL_scratchBufInit(layerIdx, memRec, &algLayer->scratchMem, scratchDataSize);
    algLayer->scratchSize = (int32_t)memRec[ALG_REF_SCRATCH_BUFF_MEMREC].size;

    algLayer->metaData.totalOps  =
    (params->net->TIDLLayers[layerIdx].outData.dimValues[TIDL_DIM_NUMCH]); // Verify this:
    algLayer->metaData.actualOps = algLayer->metaData.totalOps;
  }
  return status;
}

/**
----------------------------------------------------------------------------
@ingroup    TIDL_IVISION_SUPPORT
@fn         TIDL_flattenAlloc
@brief      Function to Request Memory required by the Flatten layer

@param      params       : Create Parameters
@param      layerIdx     : Current Layer index in Net structure
@param      memRec       : memory table populated by the Algorithm
@param      TIDLLayersBuf: Temp structure to store outDataSize and Data Id's
@remarks    None
@return     IALG_EOK   - Successful
IALG_EFAIL - Unspecified error
----------------------------------------------------------------------------
*/
int32_t TIDL_flattenAlloc(const TIDL_CreateParams *params,
  int32_t layerIdx, IALG_MemRec *memRec, sTIDL_LayerBuf_t *TIDLLayersBufPtr)
{
  int32_t outDataSize = TIDL_updateDataParamsPitch(&params->net->TIDLLayers[layerIdx].outData);

  if (params->optimiseExtMem != TIDL_OptimiseExtMemL0)
  {
    outDataSize = TIDL_findFreeOutBuff(params, layerIdx,
      outDataSize, TIDLLayersBufPtr);
    TIDLLayersBufPtr->outDataSize[layerIdx] = outDataSize;
  }
  TIDL_outBuffAlloc(params, layerIdx, memRec, outDataSize);

  return 0;
}

/**
----------------------------------------------------------------------------
@ingroup    TIDL_IVISION_SUPPORT
@fn         TIDL_flattenInit
@brief      Function to Initialize the Flatten layer specific params

@param      params       : Create Parameters
@param      layerIdx     : Current Layer index in Net structure
@param      algLayer     : Pointer to current layer algorithm specific params
@param      paramMemTabOffset : Current Offset in ALG_LAYERS_PARAMS_BUFF_MEMREC
@param      dataMemTabOffset : Current Offset in ALG_SCRATCH_DATA_BUFF_MEMREC
@param      memRec       : memory table populated by the Algorithm
@param      outPtr       : Pointer to Layer output Pointers list
@param      TIDLLayersBuf: Temp structure to store outDataSize and Data Id's
@remarks    None
@return     IALG_EOK   - Successful
IALG_EFAIL - Unspecified error
----------------------------------------------------------------------------
*/
int32_t TIDL_flattenInit(
  const TIDL_CreateParams *params, int32_t layerIdx,
  sTIDL_AlgLayer_t * algLayer, int32_t *paramMemTabOffset,
  int32_t *dataMemTabOffset, const IALG_MemRec *memRec,
  void ** outPtr, sTIDL_LayerBuf_t *TIDLLayersBufPtr)
{
  int32_t status = IALG_EOK;

  int32_t outDataSize = TIDL_updateDataParamsPitch(&params->net->TIDLLayers[layerIdx].outData);

  if (params->optimiseExtMem != TIDL_OptimiseExtMemL0)
  {
    outDataSize = TIDL_findFreeOutBuff(params, layerIdx,
      outDataSize, TIDLLayersBufPtr);
    TIDLLayersBufPtr->outDataSize[layerIdx] = outDataSize;
  }
  if (status == IALG_EOK)
  {
    TIDL_outBuffInit(params, layerIdx, dataMemTabOffset, memRec, outPtr, outDataSize);
    algLayer->scratchSize = 0;
    algLayer->metaData.totalOps =
      (params->net->TIDLLayers[layerIdx].outData.dimValues[TIDL_DIM_NUMCH] *
        params->net->TIDLLayers[layerIdx].layerParams.eltWiseParams.numInData);
    algLayer->metaData.actualOps = algLayer->metaData.totalOps;
  }
  return status;
}


int32_t TIDL_padAlloc(const TIDL_CreateParams *params,
  int32_t layerIdx, IALG_MemRec *memRec, sTIDL_LayerBuf_t *TIDLLayersBufPtr)
{
  int32_t status = TIDL_flattenAlloc(params, layerIdx,memRec,TIDLLayersBufPtr);
  sTIDL_PadLayerParams_t *layerParams = &params->net->TIDLLayers[layerIdx].layerParams.padLayerParams;
  int32_t padParamSize = layerParams->perChannelPadConstTensorSize ;
  memRec[ALG_LAYERS_PARAMS_BUFF_MEMREC].size += ((uint32_t)padParamSize + 128U);

  return status;
}

int32_t TIDL_padInit(
  const TIDL_CreateParams *params, int32_t layerIdx,
  sTIDL_AlgLayer_t * algLayer, int32_t *paramMemTabOffset,
  int32_t *dataMemTabOffset, const IALG_MemRec *memRec,
  void ** outPtr, sTIDL_LayerBuf_t *TIDLLayersBufPtr)
{
  int32_t status = IALG_EOK;
  sTIDL_PadLayerParams_t *layerParams = &params->net->TIDLLayers[layerIdx].layerParams.padLayerParams;
  int32_t paramSize   = layerParams->perChannelPadConstTensorSize ;
  int32_t paramOffset = layerParams->perChannelPadConstTensorOffset ;
  void *dst ;

  sTIDL_AlgPadParams_t* algPadParams = &algLayer->layerParams.padParams;

  status = TIDL_flattenInit(
    params, layerIdx, algLayer, paramMemTabOffset,
    dataMemTabOffset,memRec, outPtr, TIDLLayersBufPtr);

  dst = get_uint8_t_pointer((uint8_t*)(void*)memRec[ALG_LAYERS_PARAMS_BUFF_MEMREC].base, *paramMemTabOffset);
  *paramMemTabOffset += ALIGN_SIZE(paramSize, 128);

  algPadParams->perChannelPadConstTensorPtr = dst ;
  algPadParams->perChannelPadConstTensorSize = paramSize ;

  if(paramSize && paramOffset){
    void * src   = ((int8_t *)(params->net) + paramOffset);
    memcpy(dst, src, paramSize);
  }

  return status;
}

/**
----------------------------------------------------------------------------
@fn         TIDL_detectOutAlloc
@brief      Function to Request Memory required by the SSD detcetion output layer

@param      params       : Create Parameters
@param      layerIdx     : Current Layer index in Net structure
@param      memRec       : memory table populated by the Algorithm
@param      TIDLLayersBuf: Temp structure to store outDataSize and Data Id's
@remarks    None
@return     IALG_EOK   - Successful
IALG_EFAIL - Unspecified error
----------------------------------------------------------------------------
*/
int32_t TIDL_detectOutAlloc(const TIDL_CreateParams *params,
  int32_t layerIdx, IALG_MemRec *memRec, sTIDL_LayerBuf_t *TIDLLayersBufPtr)
{
  int32_t outDataSize = TIDL_updateDataParamsPitch(&params->net->TIDLLayers[layerIdx].outData);
  int32_t numConfPts = 0;
  int32_t curData = 0;
  int32_t scratchDataSize;
  int32_t curCnfPlaneSize = 0;
  int32_t maxConfPlaneSize = 0;
  int32_t elementSize = 0;
  int32_t priorBoxParamSize ;

  sTIDL_DataParams_t * inDataParams = TIDL_getDataParams(params->net,
      params->net->TIDLLayers[layerIdx].inData[0]);
  elementSize = TIDL_getDatElementSize(inDataParams->elementType);

  sTIDL_DetectOutputParams_t  * detectOutParams = &params->net->TIDLLayers[layerIdx].layerParams.detectOutParams;
  priorBoxParamSize = detectOutParams->numHeads * detectOutParams->numSubHeads * sizeof(sTIDL_AnchorBoxParams_t) ;
  memRec[ALG_LAYERS_PARAMS_BUFF_MEMREC].size += ((uint32_t)priorBoxParamSize + 128U);


  if(detectOutParams->metaArchType != TIDL_metaArchTIDLYolo)
  {
    /*total number input buffers will be twice the number of heads, and first half are location heads,
      and second half is confidence head*/
    for(int32_t i = (params->net->TIDLLayers[layerIdx].numInBufs >> 1) ; i < (params->net->TIDLLayers[layerIdx].numInBufs); i++)
    {
      const int32_t(*indimValues)[TIDL_DIM_MAX] =
        (const int32_t(*)[TIDL_DIM_MAX])\
        inDataParams->dimValues;

      const int32_t(*inpitchValues)[TIDL_DIM_MAX] =
        (const int32_t(*)[TIDL_DIM_MAX])\
        inDataParams->pitch;

      curData = ((int32_t)(*indimValues)[TIDL_DIM_BATCH] *
        (int32_t)(*indimValues)[TIDL_DIM_NUMCH] *
        ((int32_t)(*indimValues)[TIDL_DIM_HEIGHT])*
        ((int32_t)(*inpitchValues)[TIDL_LINE_PITCH]));

      curCnfPlaneSize = ((int32_t)(*indimValues)[TIDL_DIM_HEIGHT]) * ((int32_t)(*inpitchValues)[TIDL_LINE_PITCH]);

      numConfPts += curData;

      if(maxConfPlaneSize < curCnfPlaneSize)
      {
        maxConfPlaneSize = curCnfPlaneSize;
      }
    }
  }
  else
  {
    for(int32_t i = 0 ; i < (params->net->TIDLLayers[layerIdx].numInBufs); i++)
    {
      const int32_t(*indimValues)[TIDL_DIM_MAX] =
        (const int32_t(*)[TIDL_DIM_MAX])\
        inDataParams->dimValues;

      const int32_t(*inpitchValues)[TIDL_DIM_MAX] =
        (const int32_t(*)[TIDL_DIM_MAX])\
        inDataParams->pitch;

      curData = ((int32_t)(*indimValues)[TIDL_DIM_BATCH] *
        (((int32_t)(*indimValues)[TIDL_DIM_NUMCH])  - 5)* // yolo has conf and location as common head, and it has 5 channels for box parameter(4) and objectness score(1)
        ((int32_t)(*indimValues)[TIDL_DIM_HEIGHT])*
        ((int32_t)(*inpitchValues)[TIDL_LINE_PITCH]));

      numConfPts += curData;

      curCnfPlaneSize = ((int32_t)(*indimValues)[TIDL_DIM_HEIGHT]) * ((int32_t)(*inpitchValues)[TIDL_LINE_PITCH]);

      if(maxConfPlaneSize < curCnfPlaneSize)
      {
        maxConfPlaneSize = curCnfPlaneSize;
      }
    }
  }

  if (params->optimiseExtMem != TIDL_OptimiseExtMemL0)
  {
    outDataSize = TIDL_findFreeOutBuff(params, layerIdx,
      outDataSize, TIDLLayersBufPtr);
    TIDLLayersBufPtr->outDataSize[layerIdx] = outDataSize;
  }
  TIDL_outBuffAlloc(params, layerIdx, memRec, outDataSize);

  /*For worst scenario all the sratch memory is reserved in DDR*/

  // L2 related pointers
  /* topMScore, topMScoreSorted, topMIndices, topMIndicesSorted*/
  scratchDataSize = numConfPts * ((sizeof(int16_t) * 2) + (sizeof(int32_t) * 2)) + 128*4;

  // L1 related pointers
  scratchDataSize += (detectOutParams->topK * sizeof(BBox) + 128); //topKBbox
  scratchDataSize += (detectOutParams->topK * sizeof(uint16_t) + 128); //topKScore
  scratchDataSize += (detectOutParams->topK * sizeof(int32_t)  + 128); //topKIndices
  scratchDataSize += (detectOutParams->topK * sizeof(int32_t)  + 128); //nmsKeptIndices
  scratchDataSize += (sizeof(int32_t) * detectOutParams->numClasses + 128); //countMList
  scratchDataSize += (sizeof(int32_t) * detectOutParams->numClasses + 128); //countMListAcc
  scratchDataSize += (detectOutParams->numClasses * sizeof(float) + 128); // if (softmax) tempScore

  // L3 related pointers
  maxConfPlaneSize = (maxConfPlaneSize >> 6) << 6; // make it multiple of 64 for SIMD width
  scratchDataSize += (((maxConfPlaneSize + 63) >> 1) + 128); //pred
  scratchDataSize += ((maxConfPlaneSize * 2 * elementSize) + 128); //featMaxMinVal

  if(scratchDataSize > memRec[ALG_SCRATCH_DDR_MEM_MEMREC].size)
  {
    memRec[ALG_SCRATCH_DDR_MEM_MEMREC].size = (uint32_t)(scratchDataSize);
  }

  return 0;
}

/**
----------------------------------------------------------------------------
@ingroup    TIDL_IVISION_SUPPORT
@fn         TIDL_detectOutInit
@brief      Function to Initialize the SSD detcetion output layer specific params

@param      params       : Create Parameters
@param      layerIdx     : Current Layer index in Net structure
@param      algLayer     : Pointer to current layer algorithm specific params
@param      paramMemTabOffset : Current Offset in ALG_LAYERS_PARAMS_BUFF_MEMREC
@param      dataMemTabOffset : Current Offset in ALG_SCRATCH_DATA_BUFF_MEMREC
@param      memRec       : memory table populated by the Algorithm
@param      outPtr       : Pointer to Layer output Pointers list
@param      TIDLLayersBuf: Temp structure to store outDataSize and Data Id's
@remarks    None
@return     IALG_EOK   - Successful
IALG_EFAIL - Unspecified error
----------------------------------------------------------------------------
*/
int32_t TIDL_detectOutInit(
  const TIDL_CreateParams *params, int32_t layerIdx,
  sTIDL_AlgLayer_t * algLayer, int32_t *paramMemTabOffset,
  int32_t *dataMemTabOffset, const IALG_MemRec *memRec,
  void ** outPtr, sTIDL_LayerBuf_t *TIDLLayersBufPtr)
{
  int32_t status = IALG_EOK;
  int32_t outDataSize = TIDL_updateDataParamsPitch(&params->net->TIDLLayers[layerIdx].outData);
  int32_t numConfPts = 0;
  int32_t priorBoxParamSize;
  int32_t curData = 0;
  int32_t scratchDataSize;
  int32_t locScratchOffset = 0;
  sTIDL_DataParams_t * inDataParams = TIDL_getDataParams(params->net,
      params->net->TIDLLayers[layerIdx].inData[0]);
  sTIDL_ALgDetectOutputParams_t * algDetectOutParams = &algLayer->layerParams.detectionOutputParams;
  float32_tidl* priorData ;

  sTIDL_DetectOutputParams_t  * detectOutParams = &params->net->TIDLLayers[layerIdx].layerParams.detectOutParams;
  priorData    = (float32_tidl*)(((int8_t *)params->net) + detectOutParams->priorBox);

  if(detectOutParams->metaArchType != TIDL_metaArchTIDLYolo)
  {
    /*total number input buffers will be twice the number of heads, and first half are location heads,
      and second half is confidence head*/
    for(int32_t i = (params->net->TIDLLayers[layerIdx].numInBufs >> 1) ; i < (params->net->TIDLLayers[layerIdx].numInBufs); i++)
    {
      const int32_t(*indimValues)[TIDL_DIM_MAX] =
        (const int32_t(*)[TIDL_DIM_MAX])\
        inDataParams->dimValues;

      const int32_t(*inpitchValues)[TIDL_DIM_MAX] =
        (const int32_t(*)[TIDL_DIM_MAX])\
        inDataParams->pitch;

      curData = ((int32_t)(*indimValues)[TIDL_DIM_BATCH] *
        (int32_t)(*indimValues)[TIDL_DIM_NUMCH] *
        ((int32_t)(*indimValues)[TIDL_DIM_HEIGHT])*
        ((int32_t)(*inpitchValues)[TIDL_LINE_PITCH]));

      numConfPts += curData;
    }
  }
  else
  {
    for(int32_t i = 0 ; i < (params->net->TIDLLayers[layerIdx].numInBufs); i++)
    {
      const int32_t(*indimValues)[TIDL_DIM_MAX] =
        (const int32_t(*)[TIDL_DIM_MAX])\
        inDataParams->dimValues;

      const int32_t(*inpitchValues)[TIDL_DIM_MAX] =
        (const int32_t(*)[TIDL_DIM_MAX])\
        inDataParams->pitch;

      curData = ((int32_t)(*indimValues)[TIDL_DIM_BATCH] *
        (((int32_t)(*indimValues)[TIDL_DIM_NUMCH])  - 5)* // yolo has conf and location as common head, and it has 5 channels for box parameter(4) and objectness score(1)
        ((int32_t)(*indimValues)[TIDL_DIM_HEIGHT])*
        ((int32_t)(*inpitchValues)[TIDL_LINE_PITCH]));

      numConfPts += curData;
    }
  }

  if (params->optimiseExtMem != TIDL_OptimiseExtMemL0)
  {
    outDataSize = TIDL_findFreeOutBuff(params, layerIdx,
      outDataSize, TIDLLayersBufPtr);
    TIDLLayersBufPtr->outDataSize[layerIdx] = outDataSize;
  }

  priorBoxParamSize = detectOutParams->numHeads * detectOutParams->numSubHeads * sizeof(sTIDL_AnchorBoxParams_t) ;
  algDetectOutParams->priorBoxPtr =
    get_uint8_t_pointer((uint8_t*)(void*)memRec[ALG_LAYERS_PARAMS_BUFF_MEMREC].base, *paramMemTabOffset);
  *paramMemTabOffset += ALIGN_SIZE(priorBoxParamSize, 128);
  memcpy(algDetectOutParams->priorBoxPtr, priorData, priorBoxParamSize);


  if (status == IALG_EOK)
  {
    TIDL_outBuffInit(params, layerIdx, dataMemTabOffset, memRec, outPtr, outDataSize);
    algLayer->scratchSize = 0;
    scratchDataSize = numConfPts * sizeof(int16_t);
    algLayer->layerParams.detectionOutputParams.topMScoreDdr =
      (uint16_t*)get_uint8_t_pointer((uint8_t*)(void*)memRec[ALG_SCRATCH_DDR_MEM_MEMREC].base, locScratchOffset);
    locScratchOffset = locScratchOffset + ALIGN_SIZE((int32_t)scratchDataSize, 128);

    scratchDataSize = numConfPts * sizeof(int16_t);
    algLayer->layerParams.detectionOutputParams.topMScoreSortedDdr =
      (uint16_t*)get_uint8_t_pointer((uint8_t*)(void*)memRec[ALG_SCRATCH_DDR_MEM_MEMREC].base, locScratchOffset);
    locScratchOffset = locScratchOffset + ALIGN_SIZE((int32_t)scratchDataSize, 128);

    scratchDataSize = numConfPts * sizeof(int32_t);
    algLayer->layerParams.detectionOutputParams.topMIndicesDdr =
      (int32_t*)get_uint8_t_pointer((uint8_t*)(void*)memRec[ALG_SCRATCH_DDR_MEM_MEMREC].base, locScratchOffset);
    locScratchOffset = locScratchOffset + ALIGN_SIZE((int32_t)scratchDataSize, 128);

    scratchDataSize = numConfPts * sizeof(int32_t);
    algLayer->layerParams.detectionOutputParams.topMIndicesSortedDdr =
      (int32_t*)get_uint8_t_pointer((uint8_t*)(void*)memRec[ALG_SCRATCH_DDR_MEM_MEMREC].base, locScratchOffset);
    locScratchOffset = locScratchOffset + ALIGN_SIZE((int32_t)scratchDataSize, 128);

    algLayer->layerParams.detectionOutputParams.topMDdr = numConfPts;
    algLayer->layerParams.detectionOutputParams.scratchDDRConsumed = locScratchOffset;

    algLayer->metaData.totalOps =
      (params->net->TIDLLayers[layerIdx].outData.dimValues[TIDL_DIM_NUMCH] *
        params->net->TIDLLayers[layerIdx].layerParams.eltWiseParams.numInData);
    algLayer->metaData.actualOps = algLayer->metaData.totalOps;

    TIDL_collectLocConfHeadInfo(params->net->TIDLLayers, algLayer, NULL, algDetectOutParams->priorBoxPtr);

    if ((params->flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) == 0)
    {
      TIDL_odFindValidLocAndScoreKernelInit(params,algLayer,&params->net->TIDLLayers[layerIdx],0,0,0);
    }


  }

  return status;
}

/**
----------------------------------------------------------------------------
@ingroup    TIDL_IVISION_SUPPORT
@fn         TIDL_roiPoolingAlloc
@brief      Function to Request Memory required by the ArgMax layer

@param      params       : Create Parameters
@param      layerIdx     : Current Layer index in Net structure
@param      memRec       : memory table populated by the Algorithm
@param      TIDLLayersBuf: Temp structure to store outDataSize and Data Id's
@remarks    None
@return     IALG_EOK   - Successful
            IALG_EFAIL - Unspecified error
----------------------------------------------------------------------------
*/
int32_t TIDL_roiPoolingAlloc(const TIDL_CreateParams *params,
int32_t layerIdx, IALG_MemRec *memRec, sTIDL_LayerBuf_t *TIDLLayersBufPtr)
{
  int32_t outDataSize = TIDL_updateDataParamsPitch(&params->net->TIDLLayers[layerIdx].outData);

  if(params->optimiseExtMem != TIDL_OptimiseExtMemL0)
  {
    outDataSize = TIDL_findFreeOutBuff(params, layerIdx,
                                       outDataSize, TIDLLayersBufPtr);
    TIDLLayersBufPtr->outDataSize[layerIdx] = outDataSize;
  }
  TIDL_outBuffAlloc(params, layerIdx, memRec, outDataSize);

  return 0;
}

/**
----------------------------------------------------------------------------
@ingroup    TIDL_IVISION_SUPPORT
@fn         TIDL_roiPoolingInit
@brief      Function to Initialize the ArgMax layer specific params

@param      params       : Create Parameters
@param      layerIdx     : Current Layer index in Net structure
@param      algLayer     : Pointer to current layer algorithm specicfic params
@param      paramMemTabOffset : Current Offset in ALG_LAYERS_PARAMS_BUFF_MEMREC
@param      dataMemTabOffset : Current Offset in ALG_SCRATCH_DATA_BUFF_MEMREC
@param      memRec       : memory table populated by the Algorithm
@param      outPtr       : Pointer to Layer ouput Pointers list
@param      TIDLLayersBuf: Temp structure to store outDataSize and Data Id's
@remarks    None
@return     IALG_EOK   - Successful
            IALG_EFAIL - Unspecified error
----------------------------------------------------------------------------
*/
int32_t TIDL_roiPoolingInit(
const TIDL_CreateParams *params, int32_t layerIdx,
sTIDL_AlgLayer_t * algLayer, int32_t *paramMemTabOffset,
int32_t *dataMemTabOffset, const IALG_MemRec *memRec,
void ** outPtr, sTIDL_LayerBuf_t *TIDLLayersBufPtr)
{
  int32_t status = IALG_EOK;

  int32_t outDataSize = TIDL_updateDataParamsPitch(&params->net->TIDLLayers[layerIdx].outData);

  if(params->optimiseExtMem != TIDL_OptimiseExtMemL0)
  {
    outDataSize = TIDL_findFreeOutBuff(params, layerIdx,
                                       outDataSize, TIDLLayersBufPtr);
    TIDLLayersBufPtr->outDataSize[layerIdx] = outDataSize;
  }

  if(status == IALG_EOK)
  {
    TIDL_outBuffInit(params, layerIdx, dataMemTabOffset, memRec, outPtr, outDataSize);
  }
  return status;
}

int32_t TIDL_elementPromotionEnabledInAvgPool(const sTIDL_Network_t * net, sTIDL_Layer_t *tidlLayer)
{
  sTIDL_PoolingParams_t *poolParams  = &tidlLayer->layerParams.poolParams;

  int32_t promoteModeOn = 0;
  if(net->quantizationStyle==TIDL_QuantStyleAsymNP2_TFL)
  {
    if(poolParams->poolingType== TIDL_AveragePooling)
    {
      if(((poolParams->kernelW== 3) && (poolParams->kernelH== 3)) && ((poolParams->strideW== 2) && (poolParams->strideH== 2)))
      {
        promoteModeOn = 1;
      }
    }
  }
  return promoteModeOn;
}

int32_t TIDL_getScaleMemSize(sTIDL_PoolingParams_t *poolParams,
                             int32_t outWidth,
                             int32_t outHeight,
                             int32_t outLinePitch,
                             int32_t scaleFact,
                             int32_t elementSize,
                             int32_t scaleElementSize,
                             int32_t promoteModeOn)
{
  int32_t scaleMemSize;
  if(promoteModeOn)
  {
    elementSize *= 2;
  }
  if((poolParams->kernelW == 0) && (poolParams->kernelH == 0))
  {
    scaleMemSize = 0;
  }
  else if(poolParams->poolingType== TIDL_AveragePooling)
  {
    if((poolParams->strideW== 2) && (poolParams->strideH== 2) &&
       (((poolParams->kernelW== 2) && (poolParams->kernelH== 2)) ||
        ((poolParams->kernelW== 3) && (poolParams->kernelH== 3))))
    {
      scaleMemSize = 2*outWidth*elementSize;
    }
    else
    {
      scaleMemSize = outHeight*outLinePitch*scaleElementSize*scaleFact;
    }
  }
  else
  {
    scaleMemSize = 0;
  }

  return scaleMemSize;
}

/**
 * @brief This function returns the input buffer pointers for the layer
 *
 * @param inBufs : Inpupt buffers
 * @param outBufs : Ouput buffers
 * @param intAlgHandle : Algorithm Instance handle
 * @param algLayer : Pointer to current layer algorithm specific params
 * @param TIDLLayer : Pointer to common layer parameters
 * @param inPtrs : input buffer pointers
 * @param i : layer Index
 * @return int32_t : returns the status
 */
int32_t TIDL_getLayerInPtrs(
  IVISION_InBufs      *inBufs,
  IVISION_InBufs      *outBufs,
  TIDL_Handle          intAlgHandle,
  sTIDL_AlgLayer_t     * algLayer,
  sTIDL_Layer_t        * TIDLLayer,
  void                 * inPtrs[],
  int32_t i)
{
  int32_t j, twStatus, status = IALG_EOK;
  int32_t relativeCoreId = GET_RELATIVE_COREIDX(intAlgHandle->createParams->coreId,intAlgHandle->createParams->coreStartIdx);

  for(j = 0; j < TIDLLayer->numInBufs; j++)
  {
    int32_t curInDataId, inDataId  =  TIDLLayer->inData[j];
    if(intAlgHandle->createParams->optimiseExtMem != TIDL_OptimiseExtMemL0)
    {
      curInDataId = (int32_t)intAlgHandle->TIDLLayersBuf->newInDataId[i][j];
    }
    else
    {
      curInDataId = inDataId;
    }
    if(algLayer->isInData[j] == 1)
    {
      sTIDL_DataParams_t *inDataParams;
      inDataParams = &intAlgHandle->createParams->net->TIDLLayers[algLayer->inLayerIdx[j]].outData;
      if (inDataParams != NULL)
      {
        inPtrs[j] =
        TIDL_getDataBuff(inBufs->bufDesc,inBufs->numBufs,curInDataId, inDataParams->dimValues[TIDL_DIM_BATCH], inDataParams->pitch, intAlgHandle->createParams->isInbufsPaded);
        if(inPtrs[j] != NULL)
        {
          TIDLLayer->inData[j] = intAlgHandle->createParams->net->TIDLLayers[algLayer->inLayerIdx[j]].outData.dataId;
          twStatus = tidl_writeTraceDataBuf((int8_t*)inPtrs[j],
                                      intAlgHandle->createParams->net,
                                      curInDataId,
                                      (int8_t *)intAlgHandle->memRec[ALG_REF_OUTPUT_BUFF_MEMREC].base,
                                      intAlgHandle->memRec[ALG_REF_OUTPUT_BUFF_MEMREC].size,
                                      0,
                                      0,
                                      inDataParams->dimValues[TIDL_DIM_HEIGHT],
                                      inDataParams->dimValues[TIDL_DIM_NUMCH],
                                      inDataParams->pitch[TIDL_CHANNEL_PITCH],
                                      relativeCoreId,
                                      intAlgHandle->createParams->traceBaseName);

          if(twStatus != IALG_EOK)
          {
            tidl_printf(0,"Trace write failed\n");
          }
        }
      }
    }
    else if(algLayer->isInOutData[j] == 1)
    {
      sTIDL_DataParams_t * inDataParams = TIDL_getDataParams(intAlgHandle->createParams->net,
                                                            TIDLLayer->inData[j]);

      inPtrs[j] =
      TIDL_getDataBuff(outBufs->bufDesc,outBufs->numBufs,curInDataId, inDataParams->dimValues[TIDL_DIM_BATCH], inDataParams->pitch, 1);
    }
    else
    {
        if (intAlgHandle->gcHelperHandle != NULL )
        {
          //const sTIDL_AlgLayer_t *inAlgLayer = get_AlgLayer_t_pointer(intAlgHandle->alglayerParams, algLayer->inWorkloadIdx[j]);

          inPtrs[j] = TIDL_getBasePointerFromGCUsingBufIdx(intAlgHandle->gcHelperHandle,
                                                          &intAlgHandle->sysScratchPtr,
                                                          algLayer->inWorkloadIdx[j]);

          /* Code to print the traces for the const data layer */
          if(intAlgHandle->createParams->net->TIDLLayers[algLayer->inLayerIdx[j]].layerType == TIDL_ConstDataLayer)
          {
            sTIDL_Layer_t& constLayer = intAlgHandle->createParams->net->TIDLLayers[algLayer->inLayerIdx[j]];
            twStatus = tidl_writeTraceDataBuf((int8_t*)inPtrs[j],
                                      intAlgHandle->createParams->net,
                                      constLayer.outData.dataId,
                                      (int8_t *)intAlgHandle->memRec[ALG_REF_OUTPUT_BUFF_MEMREC].base,
                                      intAlgHandle->memRec[ALG_REF_OUTPUT_BUFF_MEMREC].size,
                                      0,
                                      0,
                                      constLayer.outData.dimValues[TIDL_DIM_HEIGHT],
                                      constLayer.outData.dimValues[TIDL_DIM_NUMCH],
                                      constLayer.outData.pitch[TIDL_CHANNEL_PITCH],
                                      relativeCoreId,
                                      intAlgHandle->createParams->traceBaseName);

            if(twStatus != IALG_EOK)
            {
              tidl_printf(0,"Trace write failed\n");
            }
          }
        }
        else
        {
          if ( intAlgHandle->createParams->net->TIDLLayers[algLayer->inLayerIdx[j]].layerType != TIDL_ConstDataLayer )
          {
            inPtrs[j] = (uint8_t *)intAlgHandle->dataBuf[curInDataId];
          }
          else
          {
            int32_t algIdx = getAlgLyrIdx (intAlgHandle->alglayerParams, algLayer->inLayerIdx[j]);
            sTIDL_AlgLayer_t *constalgLayer = get_AlgLayer_t_pointer(intAlgHandle->alglayerParams, algIdx);
            inPtrs[j] = constalgLayer->layerParams.constDataParams.paramPtr;

            sTIDL_Layer_t& constLayer = intAlgHandle->createParams->net->TIDLLayers[algLayer->inLayerIdx[j]];
            twStatus = tidl_writeTraceDataBuf((int8_t*)inPtrs[j],
                                      intAlgHandle->createParams->net,
                                      constLayer.outData.dataId,
                                      (int8_t *)intAlgHandle->memRec[ALG_REF_OUTPUT_BUFF_MEMREC].base,
                                      intAlgHandle->memRec[ALG_REF_OUTPUT_BUFF_MEMREC].size,
                                      0,
                                      0,
                                      constLayer.outData.dimValues[TIDL_DIM_HEIGHT],
                                      constLayer.outData.dimValues[TIDL_DIM_NUMCH],
                                      constLayer.outData.pitch[TIDL_CHANNEL_PITCH],
                                      relativeCoreId,
                                      intAlgHandle->createParams->traceBaseName);

            if(twStatus != IALG_EOK)
            {
              tidl_printf(0,"Trace write failed\n");
            }
          }
        }
    }
    if(inPtrs[j] == NULL)
    {
      status = TIDL_E_INVALID_IO_LINE_PITCH;
      break;
    }
  }

  if((intAlgHandle->createParams->flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) == TIDL_FLOW_CTRL_REF_ONLY)
  {
    if (intAlgHandle->createParams->net->dataFlowInfo != 0)
    {
      int32_t totalBufSize = 0;
      for (j = 0; j < TIDLLayer->numInBufs; j++)
      {
      sTIDL_DataParams_t * inDataParams = TIDL_getDataParams(intAlgHandle->createParams->net,
                                                            TIDLLayer->inData[j]);

        int32_t dataElementSize = TIDL_getDatElementSize(inDataParams->elementType);
        int32_t numBatches = inDataParams->dimValues[TIDL_DIM_BATCH];
        /* For first only 16-bit BN layer dataElementSize is different from that of rest of the layers in the network */
        if( (TIDL_getDatElementSize(inDataParams->elementType) != TIDL_getDatElementSize(TIDLLayer->outData.elementType)) &&
            (TIDLLayer->layerType == TIDL_BatchNormLayer))
        {
          dataElementSize = TIDL_getDatElementSize(inDataParams->elementType);
        }
        int32_t numBytesToCopy = inDataParams->pitch[TIDL_ROI_PITCH]*dataElementSize*numBatches;
        if ( (intAlgHandle->gcHelperHandle != NULL ) &&
             (intAlgHandle->createParams->net->TIDLLayers[algLayer->inLayerIdx[j]].layerType != TIDL_DataLayer) )
        {
          const sBufParams_t *bufParams = getBufParamsFromBufIndex(intAlgHandle->gcHelperHandle, algLayer->inWorkloadIdx[j]);
          numBytesToCopy = bufParams->bufSize * dataElementSize;
        }
        memcpy(intAlgHandle->refScratchBuf+totalBufSize, inPtrs[j], numBytesToCopy);
        inPtrs[j] = intAlgHandle->refScratchBuf+totalBufSize;
        totalBufSize += numBytesToCopy;
      }
    }
  }
  return status;
}
/**
 * @brief This function returns the input buffer pointers for the layer
 *
 * @param outBufs : Output buffers
 * @param intAlgHandle : Algorithm Instance handle
 * @param algLayer : Pointer to current layer algorithm specific params
 * @param TIDLLayer : Pointer to common layer parameters
 * @param outPtrs : output buffer pointers
 * @param i : layer Index
 * @return int32_t : returns the status
 */
int32_t TIDL_getLayerOutPtrs(
  IVISION_InBufs      *outBufs,
  TIDL_Handle          intAlgHandle,
  sTIDL_AlgLayer_t     * algLayer,
  sTIDL_Layer_t        * TIDLLayer,
  void                 * outPtrs[],
  int32_t i
  )
{
  int32_t j, status = IALG_EOK;
  for(j = 0; j < TIDLLayer->numOutBufs; j++)
  {
    int32_t outDataId = TIDLLayer->outData.dataId;
    if(algLayer->isOutData[j] == 1)
    {
      outPtrs[j] = (uint8_t *)
      TIDL_getDataBuff(outBufs->bufDesc,outBufs->numBufs,outDataId, TIDLLayer->outData.dimValues[TIDL_DIM_BATCH], TIDLLayer->outData.pitch, 1);
      if(outPtrs[j] == NULL)
      {
        status = TIDL_E_INVALID_IO_LINE_PITCH;
        break;
      }
    }
    else
    {
      if (intAlgHandle->gcHelperHandle != NULL)
      {
        outPtrs[j] = TIDL_getBasePointerFromGCUsingBufIdx(intAlgHandle->gcHelperHandle,
                                                         &intAlgHandle->sysScratchPtr,
                                                         algLayer->outBufIdxWl);
      }
      else
      {
        outPtrs[j] = (uint8_t *)intAlgHandle->dataBuf[outDataId];
        if(outPtrs[j] == NULL)
        {
          int32_t curOutDataId = (int32_t)intAlgHandle->TIDLLayersBuf->newOutDataId[i][j];
          outPtrs[j] = (uint8_t *)intAlgHandle->dataBuf[curOutDataId];
          intAlgHandle->dataBuf[outDataId]  = (uint8_t *)(outPtrs[j]);
        }
      }
    }
    if((intAlgHandle->createParams->flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) == TIDL_FLOW_CTRL_REF_ONLY)
    {
      /* This is required so that padded regions are filled with zero's as Padding is done only
      when NC information is available */
      if(intAlgHandle->createParams->net->dataFlowInfo == 0)
      {
        int32_t dataElementSize = TIDL_getDatElementSize(TIDLLayer->outData.elementType);
        /* Assign to zero point */
        if(dataElementSize > 1)
        {
          int32_t i0 = 0;
          int16_t *bufPtr = (int16_t*)outPtrs[j];
          for( i0 = 0; i0 < (TIDLLayer->outData.dimValues[TIDL_DIM_NUMCH] * TIDLLayer->outData.pitch[TIDL_CHANNEL_PITCH]); i0++)
          {
            bufPtr[i0] = TIDLLayer->outData.tensorZeroPoint;
          }
        }
        else
        {
        (void)memset(outPtrs[j],  TIDLLayer->outData.tensorZeroPoint,
                     TIDLLayer->outData.dimValues[TIDL_DIM_NUMCH] *
                     TIDLLayer->outData.pitch[TIDL_CHANNEL_PITCH] *
                     dataElementSize);
        }
      }
    }
  }
  return status;
}
/**
 * @brief This function Updates TensorStats for all the layers
 *
 * @param intAlgHandle : Algorithm Instance handle
 * @param TIDLLayer : Pointer to common layer parameters
 * @param outPtrs : output buffer pointers
 * @param i : layer Index
 * @return int32_t : returns the status
 */
int32_t TIDL_updateAllLayersTensorStats(
  TIDL_Handle          intAlgHandle,
  sTIDL_Layer_t        * TIDLLayer,
  void                 * outPtrs[],
  int32_t i)
{
  int32_t j, status = TIDL_SUCCESS;

  if( ((intAlgHandle->createParams->flowCtrl & TIDL_FLOW_CTRL_REF_STAT) == TIDL_FLOW_CTRL_REF_STAT) ||
      (intAlgHandle->createParams->quantRangeUpdateFactor != 0.0f))
  {
    for (j = 0; j < TIDLLayer->numOutBufs; j++)
    {
      status = TIDL_UpdateTensorRange(intAlgHandle, i, j, outPtrs[j]);
      if( status != TIDL_SUCCESS)
      {
        break;
      }
      if  (( intAlgHandle->createParams->net->calibrationOption & TIDL_CalibOptionBiasCalibration) == TIDL_CalibOptionBiasCalibration )
      {
        status = TIDL_UpdateTensorPerChannelMean(intAlgHandle,
                                                                           i,
                                                                           j,
                                                                           outPtrs[j]);
        if( status != TIDL_SUCCESS)
        {
          break;
        }
      }

    }
  }
  return status;
}
/**
 * @brief This function copies persistent weights to internal memory
 *
 * @param intAlgHandle : Algorithm Instance handle
 */
void TIDL_copyPersistentWeightsToIntMem(TIDL_Handle intAlgHandle)
{
  if (( intAlgHandle->gcHelperHandle != NULL ) &&
      ((intAlgHandle->createParams->flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) == 0))
  {
    /* Need to get group number somehow */
    int32_t groupId = intAlgHandle->groupId;

    if( groupId <= (intAlgHandle->wlSuperGrp->numGroups - 1) )
    {
      int32_t wlActivateGrpIdx;
      sWorkloadGroup_t * wlGroup = &intAlgHandle->wlSuperGrp->workloadGrpList[groupId];
      sGCHelperHandle  *gcHelperHandle = intAlgHandle->gcHelperHandle;

      wlActivateGrpIdx = wlGroup->activateGroup.startIndexWLUnitDB;

      while (wlActivateGrpIdx <= wlGroup->activateGroup.endIndexWLUnitDB)
      {
        sWorkloadUnit_t *workloadUnit = getWLUnitPtr(gcHelperHandle, wlActivateGrpIdx);

        for (int32_t linkIdx = 0; linkIdx < workloadUnit->numLinks; linkIdx++)
        {
          sLink_t *link = getLinkPtr(workloadUnit, NOT_VALID, linkIdx);
          if (link->type == LINK_PROC)
          {
            tidl_printf(0, "Process link with id = %d not supported during activate\n", link->id);
          }
          else
          {
            int8_t * pSrcMem = (int8_t *)TIDL_getPointerFromGC(&(link->src[0]), gcHelperHandle, (TIDL_sysScratchPtr *) &intAlgHandle->sysScratchPtr);
            int8_t * pDstMem = (int8_t *)TIDL_getPointerFromGC(&(link->sink)  , gcHelperHandle, (TIDL_sysScratchPtr *) &intAlgHandle->sysScratchPtr);

            int32_t srcElemSize = getByteCnt(getBufParamsFromBufIndex(gcHelperHandle, link->src[0].bufDBindex)->dataType);
            int32_t dstElemSize = getByteCnt(getBufParamsFromBufIndex(gcHelperHandle, link->sink.bufDBindex)->dataType);

            if (link->src[0].flowCnt[FLOW_PIPELINE] != 0)
            {
              TIDL_memcpy2D(pDstMem,
                            pSrcMem,
                            link->src[0].flowCnt[FLOW_PIPELINE][0] * srcElemSize,
                            link->src[0].flowCnt[FLOW_PIPELINE][1],
                            link->sink.flowDim[FLOW_PIPELINE][0] * dstElemSize,
                            link->src[0].flowDim[FLOW_PIPELINE][0] * srcElemSize,
                            intAlgHandle->dmaUtilsContext,
                            intAlgHandle->memcpyTr);
            }
          }
        }

        wlActivateGrpIdx++;
      }
    }
  }
}

void TIDL_setMultiCoreCtxtLinkIndex(TIDL_Handle algHandle, sTIDL_AlgLayer_t* algLayer, int32_t currAlgLayer)
{
  int32_t numMultiCoreCtxtLink = 0;
  sLink_t *link;
  sWorkloadUnit_t *pWLUnit =  getWLUnitPtr(algHandle->gcHelperHandle , currAlgLayer);

  for (int32_t linkIdx = 0; linkIdx < pWLUnit->numLinks; linkIdx++)
    {
      link = getLinkPtr(pWLUnit, NOT_VALID, linkIdx);

      if (link->subType == LINK_X_FM_CTXT_ACROSS_CORE)
      {
        algLayer->multiCoreCtxtLinkIndex[numMultiCoreCtxtLink] = linkIdx;
        numMultiCoreCtxtLink++;
      }
    }
}

void TIDL_getPadParams(WorkloadUnitExec_padParams *padParams,
                  sTIDL_Layer_t * tidlLayer,
                  int32_t flowCtrl,
                  sWorkloadUnit_t *workloadUnit,
                  const sGCHelperHandle   *gcHelperHandle)
{
  memset(padParams, 0, sizeof(*padParams));
  sTIDL_DataParams_t *dataParams = &tidlLayer->outData;

  padParams->flowCtrl  = flowCtrl;

  padParams->outPadOffset = (dataParams->padH)*dataParams->pitch[TIDL_LINE_PITCH] + (dataParams->padW);
  padParams->outOffset      =  padParams->outPadOffset;
  padParams->elemSize       =  TIDL_getDatElementSize(tidlLayer->outData.elementType);
  padParams->outChannels    =  tidlLayer->outData.dimValues[TIDL_DIM_NUMCH];
  padParams->isFirstTile    =  1; //NO_TILE case exhibit property of FIRST_TILE and LAST_TILE so setting to 1 by default
  padParams->isLastTile     =  1; //NO_TILE case exhibit property of FIRST_TILE and LAST_TILE so setting to 1 by default

  /* This is a temporary implementation for getting pad info from workload until pad links are made
      available */
  if (workloadUnit != NULL && gcHelperHandle != NULL)
  {
    sWLMetaDataID_t wlMetaData;
    getWLMetaDataID(workloadUnit->metadata, &wlMetaData);

    padParams->tileType   = wlMetaData.tileType;
    padParams->outHeight  = wlMetaData.outTileHeight;

    padParams->isFirstTile = wlMetaData.isFirstTile;
    padParams->isLastTile  = wlMetaData.isLastTile;

    for (int32_t linkIdx = 0; linkIdx < workloadUnit->numLinks; linkIdx++)
    {
      sLink_t *link = getLinkPtr(workloadUnit, NOT_VALID, linkIdx);;

      if((getBufParamsFromBufIndex(gcHelperHandle, link->sink.bufDBindex)->type == BUF_FM_FULL) &&
          !((link->subType == LINK_X_FM_CTXT_RST) || (link->subType == LINK_X_FM_CTXT_ACROSS_CORE)) )
      {
        if (padParams->outBufParams == NULL)
        {
          padParams->outOffset    = link->sink.offset;
          padParams->superOffset  = link->sink.superOffset;
          padParams->outBufParams = getBufParamsFromBufIndex(gcHelperHandle, link->sink.bufDBindex);
          padParams->outChannels  = padParams->outBufParams->bufHeight;
        }
        else if (padParams->outBufParams == getBufParamsFromBufIndex(gcHelperHandle, link->sink.bufDBindex))
        {
          /* Concat layer with no output DMA may show this situation where proc links point to the same buffer */
          if (link->sink.offset < padParams->outOffset)
          {
            /* the lowest offset in the out buffer indicates where the first valud pixel starts and hence where
              row pad should end */
            padParams->outOffset = link->sink.offset;
            padParams->superOffset  = link->sink.superOffset;
          }
        }
      }
    }


  }
}

/* [TIDL-1840] Temp fix for adding space between ping and pong buffers for SFM until fix from GC is ready*/
static void TIDL_correctColFlowCnt(int32_t layerIdx,
                                        const sTIDL_Network_t *net,
                                        sLink_t *link,
                                        sWorkloadUnit_t* workloadUnit)
{
  if (net->TIDLLayers[layerIdx].layerType == TIDL_ConvolutionLayer)
  {
    /* Is column flow? */
    if((net->TIDLLayers[layerIdx].layerParams.convParams.numGroups == net->TIDLLayers[layerIdx].layerParams.convParams.numInChannels) &&
       (net->TIDLLayers[layerIdx].layerParams.convParams.numInChannels == net->TIDLLayers[layerIdx].layerParams.convParams.numOutChannels))
    {
      #if TIDL_2386_NOT_FIXED
      if (link->subType == LINK_X_COEFF_IN_ANY_TO_FINAL)
      {
        int32_t flow = FLOW_PIPELINE;
        if(link->src[0].flowCnt[flow][0] == 0)
          flow = FLOW_PIPEUP;
        /*Only flatten if < 64K of volume*/
        int32_t flattenedVolume = link->src[0].flowCnt[flow][0] * link->src[0].flowCnt[flow][1];
        if(flattenedVolume < ((uint32_t)0xFFFF))
        {
          link->src[0].flowCnt[flow][0] = link->src[0].flowCnt[flow][0] * link->src[0].flowCnt[flow][1];
          link->src[0].flowCnt[flow][1] = 1;
          link->sink.flowCnt[flow][0] = link->sink.flowCnt[flow][0] * link->sink.flowCnt[flow][1];
          link->sink.flowCnt[flow][1] = 1;
        }
      }
      #endif
    }
  }
  return;
}


static inline int32_t TIDL_isColumnFlowlayer(const sTIDL_Layer_t *tidlLayer,
                                          const sWorkloadUnit_t* workloadUnit,
                                          const sGCHelperHandle * gcHelperHandle)
{
  int32_t isColFlow = 0;

  if(tidlLayer->layerType == TIDL_ArgMaxLayer)
  {
    isColFlow = 0;
    return isColFlow;
  }

  for(int32_t linkIdx = 0; linkIdx < workloadUnit->numLinks; linkIdx++)
  {
    const sLink_t *link = getLinkPtr(workloadUnit, NOT_VALID, linkIdx);
    if(link->type == LINK_PROC)
    {
      const sBufParams_t *srcBuf = getBufParamsFromBufIndex(gcHelperHandle, link->src[0].bufDBindex);
      if(srcBuf->shapeType == BUF_PING_PONG)
      {
        isColFlow = 1;
      }
    }
  }

  return isColFlow;
}

int32_t TIDL_getDataFlowType(const sWorkloadUnit_t* workloadUnit,
                                            const sGCHelperHandle * gcHelperHandle)
{
  int32_t dataFlowType = -1;

  for(int32_t linkIdx = 0; linkIdx < workloadUnit->numLinks; linkIdx++)
  {
    const sLink_t *link = getLinkPtr(workloadUnit, NOT_VALID, linkIdx);
    if(link->type == LINK_PROC)
    {
      const sBufParams_t *srcBuf = getBufParamsFromBufIndex(gcHelperHandle, link->src[0].bufDBindex);
      if(srcBuf->shapeType == BUF_PING_PONG)
      {
        dataFlowType = (int32_t)COL;
      }
      else if(srcBuf->shapeType == BUF_LINEAR)
      {
        dataFlowType = (int32_t)ROW_LINEAR;
      }
      else if(srcBuf->shapeType == BUF_CIRCULAR)
      {
        dataFlowType = (int32_t)ROW_CIRCULAR;
      }
    }
  }

  return dataFlowType;
}

static void TIDL_applyConstraintsOnWorkloadUnit(sWorkloadUnit_t* workloadUnit,
                                                            sGCHelperHandle * gcHelperHandle,
                                                            const sTIDL_Network_t *net,
                                                            int32_t currLayersGroupId)
{
  sLink_t  *link;
  sJoint_t *joint;
  const sTIDL_Layer_t *tidlLayer;
  int32_t linkIdx, srcIdx;

  tidlLayer = &net->TIDLLayers[workloadUnit->layerId];

  for (linkIdx = 0; linkIdx < workloadUnit->numLinks; linkIdx++)
  {
    link = getLinkPtr(workloadUnit, NOT_VALID, linkIdx);

    TIDL_correctColFlowCnt(workloadUnit->layerId,
                          net,
                          link,
                          workloadUnit);

    for ( srcIdx = 0; srcIdx <link->numSrc + 1; srcIdx++)
    {
      sBufParams_t *bufParam;
      int32_t dataId;

      if ( srcIdx < link->numSrc)
      {
        joint = &link->src[srcIdx];
      }
      else
      {
        joint = &link->sink;
      }

      bufParam =  getBufParamsFromBufIndex(gcHelperHandle, joint->bufDBindex);
      sMetaDataID_t pMetaDataID;
      getMetaDataID(bufParam->dataId, &pMetaDataID);
      dataId = pMetaDataID.layerId;

      /* Below condition identifies if a given buffer is input or output of the network */
      if (((TIDL_isInDataBuff(net, dataId, currLayersGroupId) == 1) ||
         (TIDL_isOutDataBuff(net, dataId, currLayersGroupId) == 1)) &&
         (pMetaDataID.type == BUF_FM_FULL ))
      {
        int32_t bufIdx;

        for (bufIdx = 0; bufIdx < tidlLayer->numInBufs + 1;bufIdx++)
        {
          int32_t tidlLayerDataId;
          if (bufIdx < tidlLayer->numInBufs)
          {
            tidlLayerDataId = tidlLayer->inData[bufIdx];
          }
          else
          {
            tidlLayerDataId = tidlLayer->outData.dataId;
          }

          if (tidlLayerDataId == dataId)
          {
#if TIDL_1913_NOT_FIXED
            /* If a given buffer is input or output of the network then set baseMem as NOT_VALID */
            bufParam->baseMem = NOT_VALID;
#endif
            break;
          }
        }
      }

#if TIDL_3350_NOT_FIXED
      int32_t isColflowLayer = TIDL_isColumnFlowlayer(tidlLayer,
                                                (const sWorkloadUnit_t *) workloadUnit,
                                                (const sGCHelperHandle*) gcHelperHandle);

      if(isColflowLayer == 1 && (tidlLayer->layerType == TIDL_PoolingLayer || tidlLayer->layerType == TIDL_ConvolutionLayer ))
      {
        int32_t numSplit = getNumSplits((const void *) workloadUnit);

        if(numSplit == 2) /* LFM Case */
        {
          if( srcIdx < link->numSrc && pMetaDataID.type == BUF_FM_OUT_PART)
          {
            /* Presently not handled at gc and therefore source tr should be programmed in sync with the sink tr */
            joint->flowCnt[FLOW_PIPELINE][TWO_D] = link->sink.flowCnt[FLOW_PIPELINE][TWO_D];
          }
          else if(srcIdx == link->numSrc && pMetaDataID.type == BUF_FM_IN_PART)
          {
            joint->flowCnt[FLOW_PIPELINE][TWO_D] = link->src[0].flowCnt[FLOW_PIPELINE][TWO_D];
          }
        }
      }
#endif
    }
  }
}

static void TIDL_applyConstraintsOnGCBufferType(sGCHelperHandle * gcHelperHandle,
                                                const sTIDL_Network_t *net,
                                                sWorkloadSuperGroup_t * wlSuperGroup,
                                                int32_t currLayersGroupId)
{
  int32_t workloadUnitIdx;
  int32_t groupIdx, subGroupIdx;

  //:TODO: Remove this once information comes from NC
  wlSuperGroup->numGroups = 1;

  for (groupIdx = 0; groupIdx < wlSuperGroup->numGroups; groupIdx++ )
  {
    sWorkloadGroup_t * wlGroup = &wlSuperGroup->workloadGrpList[groupIdx];
    for (subGroupIdx = 0; subGroupIdx < wlGroup->numSubGroups; subGroupIdx++)
    {
      sWorkloadSubGroup_t *wlSubGroup = &wlGroup->subGroup[subGroupIdx];
      for(workloadUnitIdx = wlSubGroup->startIndexWLUnitDB; workloadUnitIdx <= wlSubGroup->endIndexWLUnitDB; workloadUnitIdx++)
      {
        sWorkloadUnit_t* workloadUnit = getWLUnitPtr(gcHelperHandle, workloadUnitIdx);

        TIDL_applyConstraintsOnWorkloadUnit(workloadUnit, gcHelperHandle, net, currLayersGroupId);
      }
    }
  }
  return;
}


int32_t TIDL_applyConstraintsOnGC(const TIDL_CreateParams * createParams,
                        sWorkloadSuperGroup_t * wlSuperGroup,
                        sGCHelperHandle * gcHelperHandle)
{
  int32_t status = IALG_EOK;

  TIDL_applyConstraintsOnGCBufferType(gcHelperHandle, createParams->net, wlSuperGroup, createParams->currLayersGroupId);

  return status;
}

void TIDL_switchHandles(void* algHandle)
{
  TIDL_Handle handle      = (TIDL_Handle)algHandle;
  int32_t relativeCoreId = GET_RELATIVE_COREIDX(handle->createParams->coreId, handle->createParams->coreStartIdx);
  void * lastActiveHandle = TIDL_getLastActiveHandle(relativeCoreId);

  if(lastActiveHandle != handle && lastActiveHandle != NULL)
  {
    TIDL_deactivate((IALG_Handle)lastActiveHandle);
  }
  if((lastActiveHandle != handle) || (handle->algState == (uint8_t)ALG_NOT_ACTIVE))
  {
    TIDL_activate((IALG_Handle)(void*)handle);
    TIDL_setLastActiveHandle(relativeCoreId, handle);
  }
}

int32_t TIDL_referencFlow(const TIDL_CreateParams *pCreateParams){
  int32_t refFlow = 0 ;
  if(((pCreateParams->flowCtrl & TIDL_FLOW_CTRL_REF_STAT) != 0) ||
    ((pCreateParams->flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) != 0))
  {
    refFlow = 1 ;
  }
#if ENABLE_BACKWARDS_COMPATIBILITY
  if((TIDL_isPadOTF(pCreateParams->net->deviceName) == TRUE))
  {
    refFlow = 1 ;
  }
#endif
  return refFlow ;
}

int32_t TIDL_getSizeNet(const TIDL_CreateParams *pCreateParams){
  int32_t size = 128  ;
  /** we need the basic structure and compiled part of network always
   * and additionally the parameter part in case of reference flow
   */
#if !(TIDL_KEEP_NET_FROM_APP_ALIVE)
  size = pCreateParams->net->netBufInfo[TIDL_NET_STRUCTURE_BUF].size +
    pCreateParams->net->netBufInfo[TIDL_NET_COMPILER_BUF].size +
    pCreateParams->net->netBufInfo[TIDL_NET_GC_BUF].size +
    pCreateParams->net->netBufInfo[TIDL_NET_PARAMS_BUF].size;
#if !ENABLE_BACKWARDS_COMPATIBILITY
  if(TIDL_referencFlow(pCreateParams))
#endif
  {
    /* TIDL_NET_PARAMS_BUF is allocated memory at the last position in ALG_PERSIST_DDR_NET_MEMREC */
    size =  pCreateParams->net->netBufInfo[TIDL_NET_GC_BUF].offset + pCreateParams->net->netBufInfo[TIDL_NET_GC_BUF].size ;
  }
#endif
  return size ;
}

void TIDL_MoveNetToPrivateMemory(TIDL_Handle algHandle,
  const IALG_MemRec memRec[],int32_t relCoreId)
{
  sTIDL_Network_t *pNet = algHandle->createParams->net ;
#if !(TIDL_KEEP_NET_FROM_APP_ALIVE)
  int32_t size, bufIdx;
  int8_t *src, *dst, *pBaseSrc ;
  int32_t isReference = TIDL_referencFlow(algHandle->createParams);

  pBaseSrc = (int8_t*) pNet ;
  for(bufIdx = TIDL_NET_STRUCTURE_BUF; bufIdx <  TIDL_NET_TOTAL_BUF; bufIdx++)
  {
    if( (isReference) ||
        (bufIdx == TIDL_NET_STRUCTURE_BUF) ||
        (bufIdx == TIDL_NET_COMPILER_BUF) )
    {
      src = get_int8_t_pointer(pBaseSrc,pNet->netBufInfo[bufIdx].offset);
      dst = get_int8_t_pointer((int8_t*) memRec[ALG_PERSIST_DDR_NET_MEMREC].base,
        pNet->netBufInfo[bufIdx].offset);
      size = pNet->netBufInfo[bufIdx].size ;
      memcpy(dst,src,size);
    }
  }
  algHandle->createParams->net = (sTIDL_Network_t*) memRec[ALG_PERSIST_DDR_NET_MEMREC].base ;
  if(algHandle->createParams->net->dataFlowInfo != 0)
  {
    sGraphCompilerOutArgs_t *gcOutArgs = NULL;
    sGCDataBase_t           *gcDatabase = NULL;
    sGCCommonDataBase_t     *gcCommonDataBase = NULL;
    sWorkloadSuperGroup_t *wlSuperGrp = NULL;
    sGraphCompilerOutArgsCore_t *gcOut = NULL;
    //TODO: [8.5 rebase]. Make this similar to perfsimInfo
    gcOutArgs    = (sGraphCompilerOutArgs_t *) TIDL_getNetObjBufPtr( algHandle->createParams->net, memRec, TIDL_NET_GC_BUF);
    gcOut = (sGraphCompilerOutArgsCore_t *)getGCDataBase(gcOutArgs , DATABASE_GCOUT_1+relCoreId);
    gcDatabase   = &gcOut->dataBase;
    gcCommonDataBase = (sGCCommonDataBase_t *)getGCDataBase(gcOutArgs,DATABASE_COMMON);
    wlSuperGrp  = &gcOut->superWorkload;
    algHandle->wlSuperGrp = wlSuperGrp;

    sBufParams_t* bufNodesDB = (sBufParams_t*) &gcCommonDataBase->dbPayLoad[gcCommonDataBase->dbParams[DB_COMMON_BUF].startOffset];
    sWorkloadUnit_t* workLoadUnitDB = (sWorkloadUnit_t*) &gcDatabase->dbPayLoad[gcDatabase->dbParams[DB_WL].startOffset];
    sWorkloadUnitAuxilary_t* auxilaryWorkloadDB = (sWorkloadUnitAuxilary_t*) &gcDatabase->dbPayLoad[gcDatabase->dbParams[DB_AUXWL].startOffset];

    gcOutArgs->commonDatabaseInfo.bufDB.pBufList = bufNodesDB;
    gcOut->dataBaseInfo.wlDB.pWLList = workLoadUnitDB;
    gcOut->dataBaseInfo.wlAuxDB.pWLAuxList = auxilaryWorkloadDB;
  }
#else
  sTIDL_AlgLayer_t * alglayerParams = algHandle->alglayerParams ;
  sTIDL_DetectOutputParams_t *pDetParams;
  sTIDL_ALgDetectOutputParams_t * algDetLyrParams = NULL;

  for(int32_t i = 0 ; i < pNet->numLayers ; i++){
    switch(pNet->TIDLLayers[i].layerType){
      case TIDL_DetectionOutputLayer:
      case TIDL_OdPostProcessingLayer:
        //Find the alg Layer Params for this layer
        for(int32_t j = 0 ; j < pNet->numLayers  && alglayerParams !=NULL ; j++){
          if(alglayerParams[j].layerIdx == i){
            algDetLyrParams = &alglayerParams[j].layerParams.detectionOutputParams;
            break;
          }
        }
        pDetParams = &pNet->TIDLLayers[i].layerParams.detectOutParams;
        if(algDetLyrParams){
          algDetLyrParams->priorBoxPtr = (void*) get_int8_t_pointer((int8_t*)pNet,
          pDetParams->priorBox);
        }
        break;
    }
  }
#endif
  return ;
}

void * TIDL_getNetObjBufPtr( sTIDL_Network_t *pNet,
                      const IALG_MemRec memRec[],
                      int32_t netMember)
{
  void* netMemberSrcAddr = NULL;
  void* netMemberDstAddr = NULL;
  int32_t size = 0;
  int32_t offset = 0;

  if (netMember >= TIDL_NET_TOTAL_BUF)
  {
    tidl_printf (0, "Member not available as part of net structure \n");
    assert(0);
  }

  size   = pNet->netBufInfo[netMember].size;
  offset = pNet->netBufInfo[netMember].offset;

  netMemberDstAddr = netMemberSrcAddr = (uint8_t *)get_int8_t_pointer((int8_t *)(pNet), offset);
#if TIDL_KEEP_NET_FROM_APP_ALIVE
   memRec = NULL ;
#endif
  if(memRec != NULL)
  {
    netMemberDstAddr = (uint8_t *)get_int8_t_pointer((int8_t *)memRec[ALG_PERSIST_DDR_NET_MEMREC].base, offset);
    memcpy(netMemberDstAddr, netMemberSrcAddr, size);
  }
  return netMemberDstAddr ;
}

void TIDL_forceTargetDeviceNonOTF(int32_t *targetDevice, int32_t layerType, int32_t convType)
{
  if(layerType==TIDL_Deconv2DLayer || convType==TIDLCONV_TYPE_DECONV)
  {
    *targetDevice &= (~TIDL_OTF_FLAG_BIT);
  }
}

template <class paramType>
void TIDL_setTensorVirtualPad(sTIDL_virtualPad* virtualPad, paramType* params, int32_t targetDevice)
{
  if(TIDL_isPadOTF(targetDevice))
  {
    /*Set the default tensor padding:*/
    virtualPad->padT = params->padT;
    virtualPad->padL = params->padL;
    virtualPad->padR = params->padR;
    virtualPad->padB = params->padB;
  }
  else
  {
    virtualPad->padT = 0;
    virtualPad->padL = 0;
    virtualPad->padR = 0;
    virtualPad->padB = 0;
  }
}

