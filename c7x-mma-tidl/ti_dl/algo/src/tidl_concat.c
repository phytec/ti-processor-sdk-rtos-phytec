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
@file    tidl_conacte.c
@brief   This file defines private functions for conact layer.
@version 0.1 (May 2017) : Initial version [ADK]
----------------------------------------------------------------------------
*/

#include "tidl_alg_int.h"
#include "tidl_commonUtils.h"
#include "tidl_concat.h"
#include "tidl_alg_utils.h"
#include "gc_helper.h"

/**
@struct  tidlConcatBuffParams_t
@brief   This structure contains buffer parameters used for computation.

*/
typedef struct {
  int32_t    inWidth;
  int32_t    inHeight;
  int32_t    outWidth;
  int32_t    outHeight;
  int32_t    inPitch;
  int32_t    outPitch;
  int32_t    inChPitch;
  int32_t    outChPitch;
  int32_t    numInData;
  int32_t    numInChannels;
  int32_t    numOutChannels;
}tidlConcatBuffParams_t;

/**
 * @brief This function will rescale and write out the final output
 *
 * @tparam Tin  : template for the input
 * @tparam Tout  : template for the output
 * @param pIn : Pointer to the Input buffer
 * @param pOut : pointer to the Output buffer
 * @param scale : amount of scale to be applied on the input
 * @param concatBuffParams : copy of concat parameters
 */
template <class Tin, class Tout>
void TIDL_refConcatImbalanced(
  Tin*    pIn,
  Tout*   pOut,
  int32_t scale,
  int32_t    satLow,
  int32_t    satHigh,
  int32_t    roundBits,
  tidlConcatBuffParams_t *concatBuffParams)
{
  int32_t i1, i2, i3;
  int64_t accumulatedValue = 0;
  for (i1 = 0; i1 < concatBuffParams->numInChannels; i1++)
  {
    for (i2 = 0; i2 < (concatBuffParams->inHeight); i2++)
    {
      for (i3 = 0; i3 < (concatBuffParams->inWidth); i3++)
      {
        int32_t inOffset  = (i1* concatBuffParams->inChPitch) + (i2 * concatBuffParams->inPitch) + i3;
        int32_t outOffset = (i1* concatBuffParams->outChPitch) + (i2 * concatBuffParams->outPitch) + i3;
        accumulatedValue = pIn[inOffset] * scale;
        pOut[outOffset]   = (Tout)TIDL_roundSat((int64_t)accumulatedValue, roundBits, satLow, satHigh);
      }
   }
  }
}

/**
 * @brief This function will apply scale on the input data
 *
 * @tparam Tin  : template for the input
 * @tparam Tacc  : template for the accumulator
 * @param pin : Pointer to the Input buffer
 * @param pAcc : pointer to the accumulator
 * @param scale : amount of scale to be applied on the input
 * @param concatBuffParams : copy of concat parameters
 */
template <class Tin, class Tacc>
void TIDL_refConcat(
  Tin*    pIn,
  Tacc*   pAcc,
  int32_t scale,
  tidlConcatBuffParams_t *concatBuffParams)
{
  int32_t i1, i2, i3;

  OPENACC(data copyin(pIn[:1 + ((concatBuffParams->numInChannels-1)* concatBuffParams->inChPitch) + ((concatBuffParams->inHeight-1) * concatBuffParams->inPitch) + (concatBuffParams->inWidth-1)])
               present(pAcc[:1 + ((concatBuffParams->numInChannels-1)* concatBuffParams->outChPitch) + ((concatBuffParams->inHeight-1) * concatBuffParams->outPitch) + (concatBuffParams->inWidth-1)]))

  OPENACC(parallel loop collapse(3))
  for (i1 = 0; i1 < concatBuffParams->numInChannels; i1++)
  {
    for (i2 = 0; i2 < (concatBuffParams->inHeight); i2++)
    {
      for (i3 = 0; i3 < (concatBuffParams->inWidth); i3++)
      {
        int32_t inOffset  = (i1* concatBuffParams->inChPitch) + (i2 * concatBuffParams->inPitch) + i3;
        int32_t outOffset = (i1* concatBuffParams->outChPitch) + (i2 * concatBuffParams->outPitch) + i3;
        pAcc[outOffset]   = pIn[inOffset] * scale;
      }
   }
  }
}
/**
 * @brief This function will do quantization on the accumulator data
 *
 * @tparam Tacc  : template for the accumulator
 * @tparam Tout  : template for the output
 * @param intAlgHandle : tidl algo hanlde
 * @param layerIdx : idx of the current layer
 * @param pAcc : pointer to the accumulator
 * @param pout : Pointer to the putput buffer
 * @param roundBits : amount of bits for rounding
 * @param concatBuffParams : copy of concat parameters
 * @param satLow : min value for saturation
 * @param satHigh : : max value for saturation
 */
template <class Tacc, class Tout>
void TIDL_refConcatQuantize(
  TIDL_Handle intAlgHandle,
  int32_t layerIdx,
  Tacc*    pAcc,
  Tout*    pout,
  int32_t roundBits,
  tidlConcatBuffParams_t *concatBuffParams,
  int32_t    satLow,
  int32_t    satHigh
  )
{
  int32_t i1, i2, i3;
  Tacc outAcc;
  sTIDL_Network_t * net = intAlgHandle->createParams->net;
  sTIDL_Layer_t   *tidlLayer = &net->TIDLLayers[layerIdx];

  Tacc      min, max;
  int64_t   temp;
  min = (Tacc)((((uint64_t)1)<< ((sizeof(Tacc)*8)-1)) - 1);
    temp =(int64_t) (((uint64_t)1) << ((sizeof(Tacc) * 8) - 1));

  max = (Tacc)temp*-1;

  OPENACC(data present(pAcc[:1+((concatBuffParams->numOutChannels-1) * concatBuffParams->outChPitch) + ((concatBuffParams->outHeight-1) * concatBuffParams->outPitch) + (concatBuffParams->outWidth-1)]))

  OPENACC(parallel loop collapse(3) reduction(min:min) reduction(max:max))
  for (i1 = 0; i1 < concatBuffParams->numOutChannels; i1++)
    {
      for (i2 = 0; i2 < (concatBuffParams->outHeight); i2++)
      {
        for (i3 = 0; i3 < (concatBuffParams->outWidth); i3++)
        {
          int32_t outOffset = (i1* concatBuffParams->outChPitch) + (i2 * concatBuffParams->outPitch) + i3;
          outAcc = pAcc[outOffset];
          min = outAcc < min ? outAcc : min;
          max = outAcc > max ? outAcc : max;
        }
      }
    }

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

  float elementtype = tidlLayer -> outData.elementType;

  OPENACC(data present(pAcc[:1+((concatBuffParams->numOutChannels-1) * concatBuffParams->outChPitch) + ((concatBuffParams->outHeight-1) * concatBuffParams->outPitch) + (concatBuffParams->outWidth-1)])
               copy(pout[:1+((concatBuffParams->numOutChannels-1) * concatBuffParams->outChPitch) + ((concatBuffParams->outHeight-1) * concatBuffParams->outPitch) + (concatBuffParams->outWidth-1)]))

  OPENACC(parallel loop collapse(3))
  for (i1 = 0; i1 < concatBuffParams->numOutChannels; i1++)
  {
    for (i2 = 0; i2 < (concatBuffParams->outHeight); i2++)
    {
      for (i3 = 0; i3 < (concatBuffParams->outWidth); i3++)
      {
        int32_t outOffset = (i1* concatBuffParams->outChPitch) + (i2 * concatBuffParams->outPitch) + i3;
        outAcc = pAcc[outOffset];
        if (elementtype == TIDL_SinglePrecFloat)
        {
          OPENACC(routine(TIDL_floatSat))
          outAcc = TIDL_floatSat(outAcc, &net->TIDLLayers[layerIdx]);
        }
        else
        {
          OPENACC(routine(TIDL_roundSat))
          outAcc = (Tacc)TIDL_roundSat((int64_t)outAcc, roundBits, satLow, satHigh);
        }
        pout[outOffset] = outAcc;
      }
    }
  }
}


//TIDL_refConcatImbalancedProcess
int32_t TIDL_refConcatImbalancedProcess(
                                          TIDL_Handle          intAlgHandle,
                                          sTIDL_AlgLayer_t     * algLayer,
                                          sTIDL_Layer_t        * tidlLayer,
                                          void *inPtrs[],
                                          uint8_t *outPtr,
                                          uint16_t numTotRoi,
                                          tidlConcatBuffParams_t *concatBuffParams)
{
  int32_t  i4;
  int32_t  j;
  int32_t inDataScale[TIDL_NUM_IN_BUFS] = {1};
  float32_tidl accScale;
  //sTIDL_Network_t * net = intAlgHandle->createParams->net;
  //int32_t layerIdx = algLayer->layerIdx;
  int32_t elementJump = 1U;
  sTIDL_DataParams_t *outDataParams = &tidlLayer->outData;
  uint8_t* outPtrDst = outPtr;

  if(outDataParams->elementType == TIDL_SignedChar || outDataParams->elementType == TIDL_UnsignedChar)
  {
    elementJump = 1U;
  }
  else
  {
    elementJump = 2U;
  }

  accScale = tidlLayer->outData.tensorScale *  ((float32_tidl)(((uint32_t)1)  << tidlLayer->outData.roundBits));

  for (j = 0; j < tidlLayer->numInBufs; j++)
  {
    sTIDL_DataParams_t * indata = TIDL_getDataParams(intAlgHandle->createParams->net, tidlLayer->inData[j]);
    inDataScale[j] = (int32_t)(accScale / indata->tensorScale);
  }

  for (j = 0; j < numTotRoi; j++)
  {
    outPtrDst = outPtr + j * elementJump * concatBuffParams->numOutChannels * concatBuffParams->outChPitch;
    for (i4 = 0; i4 < (concatBuffParams->numInData); i4++)
    {
      sTIDL_DataParams_t *inDataParams;
      inDataParams = &intAlgHandle->createParams->net->TIDLLayers[algLayer->inLayerIdx[i4]].outData;
      concatBuffParams->inPitch       = inDataParams->pitch[TIDL_LINE_PITCH];
      concatBuffParams->inChPitch     = inDataParams->pitch[TIDL_CHANNEL_PITCH];
      concatBuffParams->numInChannels = inDataParams->dimValues[TIDL_DIM_NUMCH];
      concatBuffParams->inWidth       = inDataParams->dimValues[TIDL_DIM_WIDTH];
      concatBuffParams->inHeight      = inDataParams->dimValues[TIDL_DIM_HEIGHT];

      void * inPtr = ((uint8_t *)inPtrs[i4] +
        ((j*concatBuffParams->numInChannels*concatBuffParams->inChPitch)) *
        TIDL_getDatElementSize(inDataParams->elementType));

      if (inDataParams->elementType == TIDL_SignedChar)
      {
        TIDL_refConcatImbalanced((int8_t*)inPtr, (int8_t*)outPtrDst, inDataScale[i4], TIDL_SAT_LO_INT8, TIDL_SAT_HI_INT8, tidlLayer->outData.roundBits, concatBuffParams);
      }
      else if (inDataParams->elementType == TIDL_UnsignedChar)
      {
        TIDL_refConcatImbalanced((uint8_t*)inPtr, (uint8_t*)outPtrDst, inDataScale[i4], TIDL_SAT_LO_UINT8, TIDL_SAT_HI_UINT8, tidlLayer->outData.roundBits, concatBuffParams);
      }
      else if (inDataParams->elementType == TIDL_SignedShort)
      {
        TIDL_refConcatImbalanced((int16_t*)inPtr, (int16_t*)outPtrDst, inDataScale[i4], TIDL_SAT_LO_INT16, TIDL_SAT_HI_INT16, tidlLayer->outData.roundBits ,concatBuffParams);
      }
      else if (inDataParams->elementType == TIDL_UnsignedShort)
      {
        TIDL_refConcatImbalanced((uint16_t*)inPtr, (uint16_t*)outPtrDst, inDataScale[i4],TIDL_SAT_LO_UINT16, TIDL_SAT_HI_UINT16, tidlLayer->outData.roundBits, concatBuffParams);
      }
      else
      {
        /*No support for float kerenel during process*/
        tidl_printf(0,"TIDL_Concatlayer in elementType is  Not suported !!!\n ");
        return(TIDL_ERR_FAILURE);
      }

      if (tidlLayer->layerParams.concatParams.axis == TIDL_DIM_NUMCH)
      {
        /* Channel wise feature concate */
        outPtrDst += elementJump * concatBuffParams->numInChannels*concatBuffParams->outChPitch;
      }
      else if (tidlLayer->layerParams.concatParams.axis == TIDL_DIM_WIDTH)
      {
        /* Width wise feature concate */
        outPtrDst += elementJump * (uint32_t)inDataParams->dimValues[TIDL_DIM_WIDTH];
      }
      else if (tidlLayer->layerParams.concatParams.axis == TIDL_DIM_HEIGHT)
      {
        /* Width wise feature concate */
        outPtrDst += elementJump * (uint32_t)inDataParams->dimValues[TIDL_DIM_HEIGHT]*concatBuffParams->outPitch;
      }
      else
      {
        tidl_printf(0,"TIDL_Concatlayer Un suported layerParams.concatParams.axis  !!!\n ");
      }
    }
  }
  TIDL_enableL1DandL2CacheWb();
  return IALG_EOK;
}

/**
----------------------------------------------------------------------------
@ingroup    TIDL_CONCAT
@fn         TIDL_refConcatProcess
@brief      Concat layer operation currently supports ad
            This function does the C-Natural implementation of block level
@param      intAlgHandle : tidl algorothm handle
@param      algLayer : Ptr Common Layer parameters and other layer parameters.
@param      inPtrs: Ptr array to input buffers  in external memory.
@param      outPtr: Ptr to output buffer in external memory.
@param      numTotRoi: Indicates the no of ROI
@param      concatBuffParams: Ptr to buffer params used for in/out transfers
@remarks    None
@return     IALG_EOK   - Successful
            IALG_EFAIL - Unspecified error
----------------------------------------------------------------------------
*/

int32_t TIDL_refConcatProcess(
  TIDL_Handle          intAlgHandle,
  sTIDL_AlgLayer_t     * algLayer,
  sTIDL_Layer_t        * tidlLayer,
  void *inPtrs[],
  uint8_t *outPtr,
  uint16_t numTotRoi,
  tidlConcatBuffParams_t *concatBuffParams)
{
  int32_t  i4;
  int32_t  j;
  int32_t inDataScale[TIDL_NUM_IN_BUFS] = {1};
  float32_tidl accScale;
  int32_t * refAccPtr;
  int32_t * refAccPtrTemp;
  //sTIDL_Network_t * net = intAlgHandle->createParams->net;
  int32_t layerIdx = algLayer->layerIdx;
  int32_t accMemSize;
  accMemSize = sizeof(int32_t)*numTotRoi*concatBuffParams->numOutChannels*concatBuffParams->outChPitch;
  if (algLayer->scratchSize >= accMemSize)
  {
    refAccPtr = (int32_t *)algLayer->scratchMem;
  }
  else
  {
    tidl_printf(0,"Memory for  TIDL_refConcatProcess accumulator is not sufficient exiting...\n    ");
    return(TIDL_ERR_FAILURE);
  }

  accScale = tidlLayer->outData.tensorScale *  ((float32_tidl)(((uint32_t)1)  << tidlLayer->outData.roundBits));

  for (j = 0; j < tidlLayer->numInBufs; j++)
  {
    sTIDL_DataParams_t * indata = TIDL_getDataParams(intAlgHandle->createParams->net, tidlLayer->inData[j]);
    inDataScale[j] = (int32_t)(accScale / indata->tensorScale);
  }
  for (j = 0; j < numTotRoi; j++)
  {
    refAccPtrTemp = refAccPtr + j*concatBuffParams->numOutChannels*concatBuffParams->outChPitch;

    for (i4 = 0; i4 < (concatBuffParams->numInData); i4++)
    {
      sTIDL_DataParams_t *inDataParams;
      inDataParams = &intAlgHandle->createParams->net->TIDLLayers[algLayer->inLayerIdx[i4]].outData;
      concatBuffParams->inPitch       = inDataParams->pitch[TIDL_LINE_PITCH];
      concatBuffParams->inChPitch     = inDataParams->pitch[TIDL_CHANNEL_PITCH];
      concatBuffParams->numInChannels = inDataParams->dimValues[TIDL_DIM_NUMCH];
      concatBuffParams->inWidth       = inDataParams->dimValues[TIDL_DIM_WIDTH];
      concatBuffParams->inHeight      = inDataParams->dimValues[TIDL_DIM_HEIGHT];

      void * inPtr = ((uint8_t *)inPtrs[i4] +
        ((j*concatBuffParams->numInChannels*concatBuffParams->inChPitch)) *
        TIDL_getDatElementSize(inDataParams->elementType));

      if (inDataParams->elementType == TIDL_SignedChar)
      {
        TIDL_refConcat((int8_t*)inPtr, refAccPtrTemp, inDataScale[i4], concatBuffParams);
      }
      else if (inDataParams->elementType == TIDL_UnsignedChar)
      {
        TIDL_refConcat((uint8_t*)inPtr, refAccPtrTemp, inDataScale[i4], concatBuffParams);
      }
      else if (inDataParams->elementType == TIDL_SignedShort)
      {
        TIDL_refConcat((int16_t*)inPtr, refAccPtrTemp, inDataScale[i4], concatBuffParams);
      }
      else if (inDataParams->elementType == TIDL_UnsignedShort)
      {
        TIDL_refConcat((uint16_t*)inPtr, refAccPtrTemp, inDataScale[i4], concatBuffParams);
      }
      else if (inDataParams->elementType == TIDL_SinglePrecFloat)
      {
        TIDL_refConcat((float32_tidl*)inPtr, (float32_tidl*)refAccPtrTemp, 1.0, concatBuffParams);
      }
      else
      {
        tidl_printf(0,"TIDL_Concatlayer in elementType is  Not suported !!!\n ");
        return(TIDL_ERR_FAILURE);
      }

      if (tidlLayer->layerParams.concatParams.axis == TIDL_DIM_NUMCH)
      {
        /* Channel wise feature concate */
        refAccPtrTemp += concatBuffParams->numInChannels*concatBuffParams->outChPitch;
      }
      else if (tidlLayer->layerParams.concatParams.axis == TIDL_DIM_WIDTH)
      {
        /* Width wise feature concate */
        refAccPtrTemp += (uint32_t)inDataParams->dimValues[TIDL_DIM_WIDTH];
      }
      else if (tidlLayer->layerParams.concatParams.axis == TIDL_DIM_HEIGHT)
      {
        /* Width wise feature concate */
        refAccPtrTemp += (uint32_t)inDataParams->dimValues[TIDL_DIM_HEIGHT]*concatBuffParams->outPitch;
      }
      else
      {
        tidl_printf(0,"TIDL_Concatlayer Un suported layerParams.concatParams.axis  !!!\n ");
      }
    }
  }

  if (tidlLayer->outData.elementType == TIDL_SignedChar)
  {
    TIDL_refConcatQuantize(intAlgHandle, layerIdx, refAccPtr, (int8_t*)outPtr,
    tidlLayer->outData.roundBits, concatBuffParams, TIDL_SAT_LO_INT8, TIDL_SAT_HI_INT8);
  }
  else if (tidlLayer->outData.elementType == TIDL_UnsignedChar)
  {
    TIDL_refConcatQuantize(intAlgHandle, layerIdx, refAccPtr, (uint8_t*)outPtr,
    tidlLayer->outData.roundBits, concatBuffParams, TIDL_SAT_LO_UINT8, TIDL_SAT_HI_UINT8);
  }
  else if (tidlLayer->outData.elementType == TIDL_SignedShort)
  {
    TIDL_refConcatQuantize(intAlgHandle, layerIdx, refAccPtr, (int16_t*)outPtr,
    tidlLayer->outData.roundBits, concatBuffParams, TIDL_SAT_LO_INT16, TIDL_SAT_HI_INT16);
  }
  else if (tidlLayer->outData.elementType == TIDL_UnsignedShort)
  {
    TIDL_refConcatQuantize(intAlgHandle, layerIdx, refAccPtr, (uint16_t*)outPtr,
    tidlLayer->outData.roundBits, concatBuffParams, TIDL_SAT_LO_UINT16, TIDL_SAT_HI_UINT16);
  }
  else if (tidlLayer->outData.elementType == TIDL_SinglePrecFloat)
  {
    TIDL_refConcatQuantize(intAlgHandle, layerIdx, (float32_tidl *)refAccPtr, (float32_tidl *)outPtr,
    tidlLayer->outData.roundBits, concatBuffParams, TIDL_SAT_LO_UINT16, TIDL_SAT_HI_UINT16);
  }
  else
  {
    tidl_printf(0,"TIDL_Concatlayer out elementType is  Not suported !!!\n ");
    return(TIDL_ERR_FAILURE);
  }
  TIDL_enableL1DandL2CacheWb();
  return IALG_EOK;
}

static void TIDL_updateConcatMemorySizes(const TIDL_LayerSpecificParams *layerSpecificParams,
                              const TIDL_NetworkCommonParams *commonParams,
                              int32_t layerIdx,
                              int32_t *scratchDataSizeOut,
                              int32_t *outDataSizeOut,
                              int32_t memorySize[TIDL_LAYER_MEMORY_MAX])
{
  int32_t paramMemSize = 0;
  int32_t outDataSize = TIDL_refGetOutDataSize(commonParams, layerSpecificParams, layerIdx);
  int32_t scratchDataSize = TIDL_updateDataParamsPitch(&commonParams->net->TIDLLayers[layerIdx].outData)
                  *((int32_t)sizeof(int32_t));

  if(scratchDataSizeOut != NULL)
  {
    *scratchDataSizeOut = scratchDataSize;
  }

  if (outDataSizeOut != NULL)
  {
    *outDataSizeOut = outDataSize;
  }

  memorySize[TIDL_LAYER_MEMORY_SCRATCH]    = scratchDataSize + TIDL_ALIGNMENT_SIZE;
  memorySize[TIDL_LAYER_MEMORY_PERSISTENT] = paramMemSize + TIDL_ALIGNMENT_SIZE;
  memorySize[TIDL_LAYER_MEMORY_OUTPUT]     = outDataSize + TIDL_ALIGNMENT_SIZE;
}

static int32_t TIDL_concatRefAlloc(const TIDL_LayerSpecificParams *layerSpecificParams,
                                   const TIDL_NetworkCommonParams *commonParams,
                                   int32_t layerIdx,
                                   int32_t memorySize[TIDL_LAYER_MEMORY_MAX])
{
  int32_t status = IALG_EOK;

  // int32_t scratchDataSize;
  // int32_t outDataSize;
  // int32_t paramMemSize = 0;

  // outDataSize = TIDL_refGetOutDataSizeTemp(commonParams, layerSpecificParams, layerIdx);
  // scratchDataSize = outDataSize * ( ((int32_t)(sizeof(int32_t))));

  // memorySize[TIDL_LAYER_MEMORY_SCRATCH]    = scratchDataSize + 128;
  // memorySize[TIDL_LAYER_MEMORY_PERSISTENT] = paramMemSize + 128;
  // memorySize[TIDL_LAYER_MEMORY_OUTPUT]     = outDataSize + 128;

  TIDL_updateConcatMemorySizes(layerSpecificParams, commonParams, layerIdx,
                              NULL, NULL, memorySize);

  return status;
}

int32_t TIDL_concatRefInit(const TIDL_LayerSpecificParams *layerSpecificParams,
                                const TIDL_NetworkCommonParams  *commonParams,
                                sTIDL_AlgLayer_t               *algLayer,
                                int32_t layerIdx,
                                uint8_t *memory[TIDL_LAYER_MEMORY_MAX],
                                int32_t memorySize[TIDL_LAYER_MEMORY_MAX],
                                void    **outPtr)
{
  int32_t status = IALG_EOK;
  int32_t currOffset = 0;
  int32_t scratchDataSize;
  int32_t outDataSize;
  // int32_t paramMemSize = 0;

  // outDataSize = TIDL_refGetOutDataSizeTemp(commonParams, layerSpecificParams, layerIdx);
  // scratchDataSize = outDataSize * ( ((int32_t)(sizeof(int32_t))));
  TIDL_updateConcatMemorySizes(layerSpecificParams, commonParams, layerIdx,
                              &scratchDataSize, &outDataSize, memorySize);

  if (outPtr != NULL )
  {
    /* Point to NULL if output of network */
    if (TIDL_isOutDataBuff(commonParams->net,
            commonParams->net->TIDLLayers[layerIdx].outData.dataId,
            commonParams->createParams->currLayersGroupId) == 1)
    {
      *outPtr = NULL;
    }
    else if (outDataSize != 0) /* Not output of network and outDataSize > 0 */
    {
      TIDL_AllocatePtr((intptr_t)memory[TIDL_LAYER_MEMORY_OUTPUT],
                       &currOffset,
                       outDataSize,
                       128,
                       outPtr);
    }
    else /* OutDataSize > 0 */
    {
      *outPtr = NULL;
    }
  }

  currOffset = 0;
  TIDL_AllocatePtr((intptr_t)memory[TIDL_LAYER_MEMORY_SCRATCH],
                                     &currOffset,
                                     scratchDataSize,
                                     128,
                                     &algLayer->scratchMem );
  algLayer->scratchSize = scratchDataSize;

  // memorySize[TIDL_LAYER_MEMORY_SCRATCH]    = scratchDataSize + 128;
  // memorySize[TIDL_LAYER_MEMORY_PERSISTENT] = paramMemSize + 128;
  // memorySize[TIDL_LAYER_MEMORY_OUTPUT]     = outDataSize + 128;

  return status;
}

static int32_t TIDL_concatImbalancedProcess(TIDL_NetworkCommonParams *commonParams,
                                            sTIDL_AlgLayer_t     * algLayer,
                                            sTIDL_Layer_t        * tidlLayer,
                                            void                 * inPtrs[],
                                            void                 * outPtrs[],
                                            int32_t                layerIdx)
{
  int32_t  status       = IALG_EOK;
  uint8_t *outPtr            = (uint8_t *)outPtrs[0];

  sTIDL_DataParams_t *inDataParams  = &commonParams->net->TIDLLayers[algLayer->inLayerIdx[0]].outData;
  sTIDL_DataParams_t *outDataParams = &tidlLayer->outData;
  int32_t outElmtSize= TIDL_getDatElementSize(outDataParams -> elementType);
  tidlConcatBuffParams_t concatBuffParams;

  concatBuffParams.outWidth       = outDataParams->dimValues[TIDL_DIM_WIDTH];
  concatBuffParams.outHeight      = outDataParams->dimValues[TIDL_DIM_HEIGHT];
  concatBuffParams.outPitch       = outDataParams->pitch[TIDL_LINE_PITCH];
  concatBuffParams.outChPitch     = outDataParams->pitch[TIDL_CHANNEL_PITCH];
  concatBuffParams.numOutChannels = outDataParams->dimValues[TIDL_DIM_NUMCH];
  concatBuffParams.numInData      = tidlLayer->numInBufs;

  TIDL_Obj intAlgObj;
  TIDL_CreateParams createParams;

  memcpy(&createParams, commonParams->createParams, sizeof(TIDL_CreateParams));
  intAlgObj.createParams = (TIDL_CreateParams *) &createParams;

  status = TIDL_refConcatImbalancedProcess(&intAlgObj,
                                algLayer,
                                tidlLayer,
                                inPtrs,
                                outPtr,
                                inDataParams->dimValues[TIDL_DIM_BATCH],
                                &concatBuffParams);

  return status;
}

static int32_t TIDL_concatRefProcess(TIDL_NetworkCommonParams *commonParams,
                              sTIDL_AlgLayer_t     * algLayer,
                              sTIDL_Layer_t        * tidlLayer,
                              void                 * inPtrs[],
                              void                 * outPtrs[],
                              int32_t                layerIdx)
{
  int32_t  status       = IALG_EOK;
  uint8_t *outPtr            = (uint8_t *)outPtrs[0];

  sTIDL_DataParams_t *inDataParams  = &commonParams->net->TIDLLayers[algLayer->inLayerIdx[0]].outData;
  sTIDL_DataParams_t *outDataParams = &tidlLayer->outData;
  int32_t outElmtSize= TIDL_getDatElementSize(outDataParams -> elementType);
  tidlConcatBuffParams_t concatBuffParams;

  concatBuffParams.outWidth       = outDataParams->dimValues[TIDL_DIM_WIDTH];
  concatBuffParams.outHeight      = outDataParams->dimValues[TIDL_DIM_HEIGHT];
  concatBuffParams.outPitch       = outDataParams->pitch[TIDL_LINE_PITCH];
  concatBuffParams.outChPitch     = outDataParams->pitch[TIDL_CHANNEL_PITCH];
  concatBuffParams.numOutChannels = outDataParams->dimValues[TIDL_DIM_NUMCH];
  concatBuffParams.numInData      = tidlLayer->numInBufs;

  TIDL_Obj intAlgObj;
  TIDL_CreateParams createParams;

  memcpy(&createParams, commonParams->createParams, sizeof(TIDL_CreateParams));
  intAlgObj.createParams = (TIDL_CreateParams *) &createParams;

  status = TIDL_refConcatProcess(&intAlgObj,
                                algLayer,
                                tidlLayer,
                                inPtrs,
                                outPtr,
                                inDataParams->dimValues[TIDL_DIM_BATCH],
                                &concatBuffParams);

  return status;
}

/**
*******************************************************************************
*  \brief      Function to supply memory requirement for concat layer to user
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
int32_t TIDL_concatAllocNew(const TIDL_LayerSpecificParams *layerSpecificParams,
                                   const TIDL_NetworkCommonParams *commonParams,
                                   int32_t layerIdx,
                                   int32_t memorySize[TIDL_LAYER_MEMORY_MAX])

{
  int32_t status = IALG_EOK;


  if ((commonParams->createParams->flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) == 0)
  {
    status = TIDL_deviceUtilsCommonAlloc(layerSpecificParams, commonParams, layerIdx, memorySize);
  }
  else
  {
    status = TIDL_concatRefAlloc(layerSpecificParams, commonParams, layerIdx, memorySize);
  }

  return status;
}

/**
*******************************************************************************
*  \brief      Function to set memory needed by concat layer
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
int32_t TIDL_concatInitNew(const TIDL_LayerSpecificParams *layerSpecificParams,
                                const TIDL_NetworkCommonParams  *commonParams,
                                sTIDL_AlgLayer_t               *algLayer,
                                int32_t layerIdx,
                                uint8_t *memory[TIDL_LAYER_MEMORY_MAX],
                                int32_t memorySize[TIDL_LAYER_MEMORY_MAX],
                                void    **outPtr)

{
  int32_t status = IALG_EOK;

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
    status = TIDL_concatRefInit(layerSpecificParams,
                                 commonParams,
                                 algLayer,
                                 layerIdx,
                                 memory,
                                 memorySize,
                                 outPtr);
  }

  return status;
}


int32_t TIDL_concatProcessNew(TIDL_NetworkCommonParams *commonParams,
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
    if(1)
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
      /*Run reference implementation for the imbalanced case*/
      status = TIDL_concatImbalancedProcess(commonParams,
                                    algLayer,
                                    tidlLayer,
                                    inPtrs,
                                    outPtrs,
                                    layerIdx);
    }
  }
  else
  {
    status = TIDL_concatRefProcess(commonParams,
                                   algLayer,
                                   tidlLayer,
                                   inPtrs,
                                   outPtrs,
                                   layerIdx);
  }

  return status;
}
