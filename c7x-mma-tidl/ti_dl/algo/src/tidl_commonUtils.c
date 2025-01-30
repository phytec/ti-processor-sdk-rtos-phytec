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
@file    tidl_commonUtils.c
@brief   This file contains TILD common utilities
@version 0.1 (May 2018) : Initial version [ADK]
----------------------------------------------------------------------------
*/

#include "tidl_types.h"
#include "tidl_alg_int.h"
#include "tidl_commonUtils.h"
#include "../../custom/tidl_custom.h"
#include "gc_helper.h"
#include <stdint.h>
#include <limits>
#include <math.h>
#include <float.h>

#if not defined(SOC_AM62A) && not defined (SOC_J722S)
#if  !defined(HOST_EMULATION)
#include <ti/csl/arch/c7x/cslr_C7X_CPU.h>
#endif
#endif

#define USE_16BIT_BIAS_FOR_8BIT_MODE (1)

#ifdef SOC_J784S4
#define USE_STACK_MEM_TR   //TODO: Temporary workaround
#endif


#ifdef TIDL_BUILD_WITH_MEMTRACES
extern int8_t* MemTrace_Buf;
int32_t curPos = 0;
#endif

#include "dmautils_ext.h"


static int32_t tidlLogLevel;
static int32_t tidlWriteLevel;
static tidlVprintf_t tidlVprintf;
static tidlWriteBinToFile_t tidlWriteBinToFile;
static tidlReadBinFromFile_t tidlReadBinFromFile;
static char tidlTraceBaseName[TIDL_STRING_SIZE];

void tidl_setWriteLevel(int32_t val){
  tidlWriteLevel = val;
  return ;
}

int32_t tidl_getTraceLogLevel(){
  return tidlLogLevel;
}

static int32_t tidl_getWriteLevel(){
  return tidlWriteLevel;
}

static tidlVprintf_t tidl_getVprintf(){
  return tidlVprintf;
}

static tidlWriteBinToFile_t tidl_getWriteBinToFile(){
  return tidlWriteBinToFile;
}

static tidlReadBinFromFile_t tidl_getReadBinFromFile(){
  return tidlReadBinFromFile;
}

static void* tidl_getTraceBaseName(){
  return tidlTraceBaseName;
}

#if defined (SOC_J784S4) || defined (SOC_AM62A)
/** CSL_c7xCleaninvalidateL1DCache function is part of CSL c7x arch, however the c7x arch build is not maintained by PDK,
 * and is expected to be maintained by user of these files
 * So this function is added within TIDL lib with TIDL_ prefix to avoid any symbols redefined errors in case it gets included in PDK build in future
 */
#ifndef HOST_EMULATION

#ifdef __cplusplus
extern "C" {
#endif
  void TIDL_c7xSetL1DWBINV(uint64_t param);
  uint64_t TIDL_c7xGetL1DWBINV(void);
  void TIDL_c7xSetL2WBINV(uint64_t param);
  uint64_t TIDL_c7xGetL2WBINV(void);
#ifdef __cplusplus
}
#endif
#if defined (SOC_J784S4)
__asm__ __volatile__("TIDL_c7xSetL1DWBINV: \n"
	                 "    MVC	.S1	A4,		ECR259; \n"
	                 "    RET	.B1\n"
          );
__asm__ __volatile__("TIDL_c7xGetL1DWBINV:"
      	" MVC	.S1	ECR259,	A4	;"
	      " RET	.B1;"
       );
#endif
#if defined (SOC_AM62A)
void TIDL_c7xSetL1DWBINV(uint64_t param)
{
  /* Empty function as currently am62a doesnt use cache api's*/
}
uint64_t TIDL_c7xGetL1DWBINV(void)
{
  return 0;
}

#endif

#define TIDL_DMC_L1DWBINV_WBINV_MASK                             (0x00000001U)
#define TIDL_DMC_L1DWBINV_WBINV_SHIFT                            (0U)
#define TIDL_DMC_L1DWBINV_WBINV_RESETVAL                         (0x00000000U)
#define TIDL_DMC_L1DWBINV_WBINV_MAX                              (0x00000001U)
void TIDL_c7xCleaninvalidateL1DCache()
{
    volatile uint64_t wbinv;
    wbinv  = TIDL_c7xGetL1DWBINV();
    wbinv  &= ~TIDL_DMC_L1DWBINV_WBINV_MASK;
    wbinv |= (0x1U << TIDL_DMC_L1DWBINV_WBINV_SHIFT) & TIDL_DMC_L1DWBINV_WBINV_MASK;
    TIDL_c7xSetL1DWBINV(wbinv);
    return;
}

__asm__ __volatile__("TIDL_c7xSetL2WBINV: \n"
	                 "    MVC	.S1	A4,		ECR387  ; \n"
	                 "    RET	.B1\n"
          );
__asm__ __volatile__("TIDL_c7xGetL2WBINV: \n"
      	" MVC	.S1	ECR387,	A4	; \n"
	      " RET	.B1; \n"
       );
#define TIDL_UMC_L2WBINV_WBINV_MASK                              (0x00000001ULL)
#define TIDL_UMC_L2WBINV_WBINV_SHIFT                             (0ULL)
#define TIDL_UMC_L2WBINV_WBINV_RESETVAL                          (0x00000000ULL)
#define TIDL_UMC_L2WBINV_WBINV_MAX                               (0x00000001ULL)
void TIDL_c7xCleaninvalidateL2Cache()
{
    volatile uint64_t       wbinv;
    wbinv = TIDL_c7xGetL2WBINV() & ~TIDL_UMC_L2WBINV_WBINV_MASK;
    wbinv |= (0x1U << TIDL_UMC_L2WBINV_WBINV_SHIFT) & TIDL_UMC_L2WBINV_WBINV_MASK;
    TIDL_c7xSetL2WBINV(wbinv);
    return;
}
#endif
#endif


/**
 * @brief To find min and max in the tensor
 *
 * @param ptr : pointer to tensor data
 * @param dataPrms : parametes of the data buffer
 * @param min : to store the min value
 * @param max : to store the max value
 */
template <class Tsrc, class TminMax>
void TIDL_TensorMinMax(const Tsrc * ptr, const sTIDL_DataParams_t * dataPrms, TminMax *min, TminMax * max)
{
  int32_t padOffset = dataPrms->padW + (dataPrms->padH*dataPrms->pitch[TIDL_LINE_PITCH]);
  int32_t i0, i1, i2, i3, i4, i5;
  TminMax val;

  for (i0 = 0; i0 < dataPrms->dimValues[TIDL_DIM_BATCH]; i0++)
  {
    for (i1 = 0; i1 < dataPrms->dimValues[TIDL_DIM_DIM1]; i1++)
    {
      for (i2 = 0; i2 < dataPrms->dimValues[TIDL_DIM_DIM2]; i2++)
      {
        for (i3 = 0; i3 < dataPrms->dimValues[TIDL_DIM_NUMCH]; i3++)
        {
          for (i4 = 0; i4 < dataPrms->dimValues[TIDL_DIM_HEIGHT]; i4++)
          {
            for (i5 = 0; i5 < dataPrms->dimValues[TIDL_DIM_WIDTH]; i5++)
            {
              val = (TminMax)ptr[padOffset + (i0*dataPrms->pitch[TIDL_ROI_PITCH]) + (i1*dataPrms->pitch[TIDL_DIM_DIM1]) + (i2*dataPrms->pitch[TIDL_DIM_DIM2]) + (i3*dataPrms->pitch[TIDL_DIM_NUMCH]) + (i4*dataPrms->pitch[TIDL_DIM_HEIGHT]) + i5];
              *min = (val < *min) ? val : *min;
              *max = (val > *max) ? val : *max;
            }
          }
        }
      }
    }
  }
}

/**
 * @brief  To find histogram min and max values of the tensor
 *
 * @param ptr  : pointer to the tensor data
 * @param dataPrms : parametes of the tensor data
 * @param numBins : no of bins used for histogram
 * @param percentileActRangeShrink : active range percentage
 * @param histogramPtr : Pointer to the histogram buffer
 * @param min : to store the min value
 * @param max : to store the max value
 */
template <class Tsrc, class TminMax>
int32_t TIDL_TensorMinMaxHist(const Tsrc * ptr,
                                      const sTIDL_DataParams_t * dataPrms,
                                      int32_t numBins,
                                      float32_tidl percentileActRangeShrink,
                                      int32_t * histogramPtr,
                                      TminMax* min,
                                      TminMax* max)
{
  int32_t status = TIDL_SUCCESS;
  int32_t padOffset = dataPrms->padW + (dataPrms->padH*dataPrms->pitch[TIDL_LINE_PITCH]);
  int32_t i0, i1, i2, i3, i4, i;
  TminMax val;
  float32_tidl valFloat, valNorm;
  int32_t numVals;
  int32_t binIdx, minBinIdx = 0, maxBinIdx = (numBins - 1);
  float32_tidl minValue = (*min - dataPrms->tensorZeroPoint) / dataPrms->tensorScale;
  float32_tidl maxValue = (*max - dataPrms->tensorZeroPoint) / dataPrms->tensorScale;
  float32_tidl minFloatOut;
  float32_tidl maxFloatOut;

  if((maxValue - minValue) != 0) /*not all values in tensor constant */
  {
    for (i0 = 0; i0 < dataPrms->dimValues[TIDL_DIM_BATCH]; i0++)
    {
      for (i1 = 0; i1 < dataPrms->dimValues[TIDL_DIM_NUMCH]; i1++)
      {
        for (i2 = 0; i2 < dataPrms->dimValues[TIDL_DIM_HEIGHT]; i2++)
        {
          for (i3 = 0; i3 < dataPrms->dimValues[TIDL_DIM_WIDTH]; i3++)
          {
            val = (TminMax)ptr[padOffset + (i0*dataPrms->pitch[TIDL_ROI_PITCH]) + (i1*dataPrms->pitch[TIDL_CHANNEL_PITCH]) + (i2*dataPrms->pitch[TIDL_LINE_PITCH]) + i3];
            valFloat = (val - dataPrms->tensorZeroPoint) / dataPrms->tensorScale;
            valNorm = (valFloat - minValue)/(maxValue - minValue) * (numBins-1);

            binIdx = (int32_t)(valNorm + 0.5);/* Round to nearest integer */

            if ( binIdx > (numBins-1) )
            {
              binIdx = (numBins-1);
            }
            histogramPtr[binIdx]++;
          }
        }
      }
    }

    numVals = 0;
    for (i = 0; i < numBins; i++)
    {
      numVals += histogramPtr[i];
    }

    int32_t pct_freq = (int32_t)((percentileActRangeShrink / 100.0) * numVals);
    int32_t count = 0;
    if (*min < 0)  /* minimum to be shrinked only for signed */
    {
      for(i4 = 0; i4 < numBins; i4++)
      {
        count += histogramPtr[i4];
        if(count >= pct_freq)
        {
          minBinIdx = i4;
          break;
        }
      }
    }
    else
    {
      minBinIdx = 0;
    }
    count = 0;
    for(i4 = numBins - 1; i4 >= 0; i4--)
    {
      count += histogramPtr[i4];
      if(count >= pct_freq)
      {
        maxBinIdx = i4;
        break;
      }
    }
    minFloatOut = minValue + (float32_tidl)minBinIdx / (numBins-1) * (maxValue - minValue);
    maxFloatOut = minValue + (float32_tidl)maxBinIdx / (numBins-1) * (maxValue - minValue);
    maxFloatOut = (maxFloatOut > maxValue) ? maxValue : maxFloatOut; /*to ensure back calculated bin value not greater than original max */
    minFloatOut = (minFloatOut < minValue) ? minValue : minFloatOut;
    *min = (TminMax)(minFloatOut * dataPrms->tensorScale + dataPrms->tensorZeroPoint);
    *max = (TminMax)(maxFloatOut * dataPrms->tensorScale + dataPrms->tensorZeroPoint);
  }

  return status;
}

/**
 * @brief This function is to find Current Offset For PerChannelMean
 *
 * @param net : tidl network structure
 * @param currLayerNum : layer no of the current layer
 * @param totalMemReq : to store total memory requiremet
 * @return int32_t : returns the current ch offset
 */
static int32_t TIDL_findCurrentOffsetForPerChannelMean(sTIDL_Network_t * net, int32_t currLayerNum, int32_t * totalMemReq)
{
  int layerIdx = 0;
  int32_t totNumOutChannels = 0;
  int32_t currNumOutChannels = 0;
  for (layerIdx = 0; layerIdx < net->numLayers; layerIdx++)
  {

    if ( layerIdx == (currLayerNum) )
    {
      currNumOutChannels = totNumOutChannels;
    }

    if ( (net->TIDLLayers[layerIdx].layerType != TIDL_DataLayer) && ((net->TIDLLayers[layerIdx].layerType != TIDL_ConstDataLayer)))
    {
      //:TODO: This can eventually be done only for the layers where bias is applicable
      totNumOutChannels += net->TIDLLayers[layerIdx].outData.dimValues[TIDL_DIM_NUMCH];
    }
  }

  *totalMemReq = totNumOutChannels * sizeof(float32_tidl);
  return (currNumOutChannels * sizeof(float32_tidl));
}

/**
 * @brief To find tensor perChannel mean
 *
 * @param net : tidl network structure
 * @param currLayerNum : current layer number
 * @param ptr : pointer to the tensor data
 * @param dataPrms : parameters of the data buffer
 * @param scratchBuf : pointer to the scratch memory
 * @param scratchBufSize : size of the scratch buffer
 * @param currIterationCount : count for the current iteration
 * @return int32_t : returns tensor perchannelmean
 */
template <class Tsrc>
int32_t TIDL_TensorPerChannelMean(sTIDL_Network_t * net,
                                                      int32_t currLayerNum,
                                                      const Tsrc * ptr,
                                                      const sTIDL_DataParams_t * dataPrms,
                                                      void * scratchBuf,
                                                      int32_t scratchBufSize,
                                                      int32_t currIterationCount
                                                      )
{
  int32_t status = TIDL_SUCCESS;
  int32_t padOffset = dataPrms->padW + (dataPrms->padH*dataPrms->pitch[TIDL_LINE_PITCH]);
  int32_t i0, i1, i2, i3;
  float32_tidl * meanPtr;
  int32_t totalMemReqInBytes;
  int8_t * traceDumpName = (int8_t *)(scratchBuf);
  int32_t currOffsetInBytes = 0;
  int32_t currOffsetInfloats = 0;
  float32_tidl currChannelMean = 0.0;
  float32_tidl runningChannelMean = 0.0;
  float32_tidl sum;
  float32_tidl updateFactor;
  Tsrc val;

  if(tidl_getTraceBaseName() != NULL)
  {
    sprintf((char *)traceDumpName, "%s_LayerPerChannelMean.bin", (char *)tidl_getTraceBaseName());
  }
  else
  {
    (void)strcpy((char *)traceDumpName, (char *)"");
  }

  /* Find the total memory required and current offset */
  currOffsetInBytes = TIDL_findCurrentOffsetForPerChannelMean(net,
                                                              currLayerNum,
                                                              &totalMemReqInBytes);

  currOffsetInfloats = ( currOffsetInBytes / sizeof(float32_tidl) );

  if (totalMemReqInBytes < (scratchBufSize -TRACE_STRING_SIZE) )
  {
    meanPtr = (float32_tidl*)((int8_t *)scratchBuf + TRACE_STRING_SIZE);
  }
  else
  {
    tidl_printf(0,"TIDL_TensorPerChannelMean : Not enough memory to allocate for per channel mean \n");
    status = TIDL_ERR_FAILURE;
  }

  if ( status == TIDL_SUCCESS )
  {
    if ( (currIterationCount == 0) && (currOffsetInBytes == 0 ) )
    {
      memset(meanPtr, 0, totalMemReqInBytes);
    }
    else
    {
      (void)tidl_getReadBinFromFile()((const char *)traceDumpName, meanPtr, totalMemReqInBytes, 1 );
    }

    updateFactor = 1.0/(currIterationCount + 1U);
      //:TODO: Add minimum value so that when num images are we don't loose information from end images

    for (i0 = 0; i0 < dataPrms->dimValues[TIDL_DIM_BATCH]; i0++)
    {
      for (i1 = 0; i1 < dataPrms->dimValues[TIDL_DIM_NUMCH]; i1++)
      {
        sum = 0;
        for (i2 = 0; i2 < dataPrms->dimValues[TIDL_DIM_HEIGHT]; i2++)
        {
          for (i3 = 0; i3 < dataPrms->dimValues[TIDL_DIM_WIDTH]; i3++)
          {
            val = (Tsrc)ptr[padOffset + (i0*dataPrms->pitch[TIDL_ROI_PITCH]) + (i1*dataPrms->pitch[TIDL_CHANNEL_PITCH]) + (i2*dataPrms->pitch[TIDL_LINE_PITCH]) + i3];
            sum += (float32_tidl)val;
          }
        }
        currChannelMean = sum / (dataPrms->dimValues[TIDL_DIM_WIDTH] *
                                                  dataPrms->dimValues[TIDL_DIM_HEIGHT]);
        currChannelMean = (currChannelMean - dataPrms->tensorZeroPoint) / dataPrms->tensorScale;
        runningChannelMean = meanPtr[currOffsetInfloats + i1];
        meanPtr[currOffsetInfloats + i1] = (runningChannelMean * (1.0 - updateFactor)) +
                                                        (currChannelMean * (updateFactor));
      }
    }

    (void)tidl_getWriteBinToFile()((const char *)traceDumpName, meanPtr, totalMemReqInBytes, 1);
  }
  return status;
}

void TIDL_getSaturationLimits(int32_t elementType, int32_t * satLow, int32_t * satHigh)
{
  if ( elementType == TIDL_UnsignedChar )
  {
    *satLow  = std::numeric_limits<uint8_t>::lowest();
    *satHigh = std::numeric_limits<uint8_t>::max();
  }
  else if ( elementType == TIDL_SignedChar )
  {
    *satLow  = std::numeric_limits<int8_t>::lowest();
    *satHigh = std::numeric_limits<int8_t>::max();
  }
  else if ( elementType == TIDL_UnsignedShort )
  {
    *satLow  = std::numeric_limits<uint16_t>::lowest();
    *satHigh = std::numeric_limits<uint16_t>::max();
  }
  else if ( elementType == TIDL_SignedShort )
  {
    *satLow  = std::numeric_limits<int16_t>::lowest();
    *satHigh = std::numeric_limits<int16_t>::max();
  }
  else
  {
    *satLow  = std::numeric_limits<uint8_t>::lowest();
    *satHigh = std::numeric_limits<uint8_t>::max();
  }

}

#define TIDL_HISTOGRAM_ACTIVATION_RANGEFACTOR (4.0)

/**
 * @brief This function is to update the tesnor range
 *
 * @param intAlgHandle : tidl algorithm handle
 * @param layerIdx : index of the current layer
 * @param outDataIdx : index of the output data buffer
 * @param ptr : pointer to the tensor data
 * @return  IALG_EOK   - Successful
 *          IALG_EFAIL - Unspecified error
 */
int32_t TIDL_UpdateTensorRange(TIDL_Handle intAlgHandle, int32_t layerIdx, int32_t outDataIdx, void * ptr)
{
  float32_tidl min = FLT_MAX;
  float32_tidl max = -1.0 * FLT_MAX;
  int32_t temp;
  int32_t status = TIDL_SUCCESS;
  sTIDL_Network_t * net = intAlgHandle->createParams->net;
  int32_t numBins = TIDL_NUM_ACTIVATION_HISTOGRAM_BINS;
  int32_t * histogramPtr = NULL;
  sTIDL_DataParams_t dataBuffParamOrig = net->TIDLLayers[layerIdx].outData;

  /* Update the dim and pitch values as expected by NHWC format and layer change it back to original */
  if ( net->TIDLLayers[layerIdx].layerType == TIDL_DataConvertLayer )
  {
    if ( net->TIDLLayers[layerIdx].layerParams.dataConvertParams.layout == TIDL_LT_NHWC )
    {
      if ( net->TIDLLayers[layerIdx].layerParams.dataConvertParams.type == TIDL_DC_TYPE_OUTPUT)
      {
        sTIDL_DataParams_t * dataBuffParam = &net->TIDLLayers[layerIdx].outData;
        dataBuffParam->dimValues[TIDL_DIM_WIDTH]  = dataBuffParamOrig.dimValues[TIDL_DIM_NUMCH];
        dataBuffParam->dimValues[TIDL_DIM_HEIGHT] = dataBuffParamOrig.dimValues[TIDL_DIM_WIDTH];
        dataBuffParam->dimValues[TIDL_DIM_NUMCH]  = dataBuffParamOrig.dimValues[TIDL_DIM_HEIGHT];
        dataBuffParam->dimValues[TIDL_DIM_DIM2]  = dataBuffParamOrig.dimValues[TIDL_DIM_DIM2];
        dataBuffParam->dimValues[TIDL_DIM_DIM1]  = dataBuffParamOrig.dimValues[TIDL_DIM_DIM1];
        dataBuffParam->dimValues[TIDL_DIM_BATCH]  = dataBuffParamOrig.dimValues[TIDL_DIM_BATCH];

        dataBuffParam->pitch[TIDL_LINE_PITCH]    = dataBuffParam->dimValues[TIDL_DIM_WIDTH];
        dataBuffParam->pitch[TIDL_CHANNEL_PITCH] = dataBuffParam->pitch[TIDL_LINE_PITCH] * dataBuffParam->dimValues[TIDL_DIM_HEIGHT];
        dataBuffParam->pitch[TIDL_DIM2_PITCH]     = dataBuffParam->pitch[TIDL_CHANNEL_PITCH] * dataBuffParam->dimValues[TIDL_DIM_NUMCH];
        dataBuffParam->pitch[TIDL_DIM1_PITCH]     = dataBuffParam->pitch[TIDL_DIM2_PITCH] * dataBuffParam->dimValues[TIDL_DIM_DIM2];
        dataBuffParam->pitch[TIDL_ROI_PITCH]     = dataBuffParam->pitch[TIDL_DIM1_PITCH] * dataBuffParam->dimValues[TIDL_DIM_DIM1];
      }
    }
  }


  if (TIDL_getDatElementSize(net->TIDLLayers[layerIdx].outData.elementType) <= 2)
  {
    int32_t minTemp, maxTemp;
    int32_t minActHist;
    int32_t maxActHist;
    minActHist = minTemp  = (int32_t)((((uint32_t)1) << ((sizeof(int32_t) * 8U) - 1U)) - 1U);
    temp = (int32_t)((((uint32_t)1) << ((sizeof(int32_t) * 8U) - 1U)));
    maxActHist = maxTemp = -1 * temp;

    if (intAlgHandle->activationHistPtr != NULL )
    {
      histogramPtr = &intAlgHandle->activationHistPtr[layerIdx * TIDL_NUM_ACTIVATION_HISTOGRAM_BINS];
      /* activationRangePtr being NULL indicates that we want to update histogram for each frame */
      if ( intAlgHandle->activationRangePtr == NULL )
      {
        memset(histogramPtr, 0 , TIDL_NUM_ACTIVATION_HISTOGRAM_BINS * sizeof(int32_t));
      }
      else
      {
        /* THis indicates that user wants to use global histogram across all the
        frames. So read the activation ranges from the previous iteration and multiply
        it by a factor to account for any variation in stats across iterations. These
        min and max value will be use to find the histogram across all the frames */
        minActHist = (int32_t)(intAlgHandle->activationRangePtr[2* layerIdx] *
                    TIDL_HISTOGRAM_ACTIVATION_RANGEFACTOR *
                    net->TIDLLayers[layerIdx].outData.tensorScale);
        maxActHist = (int32_t)(intAlgHandle->activationRangePtr[2* layerIdx + 1] *
                    TIDL_HISTOGRAM_ACTIVATION_RANGEFACTOR *
                    net->TIDLLayers[layerIdx].outData.tensorScale);
      }
    }

    if (net->TIDLLayers[layerIdx].outData.elementType == TIDL_SignedChar)
    {
      TIDL_TensorMinMax((int8_t*)ptr, &net->TIDLLayers[layerIdx].outData, (int32_t *)&minTemp, (int32_t *)&maxTemp);

      if ( histogramPtr != NULL )
      {
        /* For global histogram always pick the range from histogram from previous stats collection
        iteration */
        if ( net->calibrationParams.activationRangeMethod == TIDL_ActivationRangeMethodGlobalHistogram )
        {
          minTemp = minActHist;
          maxTemp = maxActHist;
        }
        status = TIDL_TensorMinMaxHist((int8_t *)ptr,
                                          &net->TIDLLayers[layerIdx].outData,
                                          numBins,
                                          net->calibrationParams.percentileActRangeShrink,
                                          histogramPtr,
                                          (int32_t*)&minTemp, (int32_t*)&maxTemp);

      }
    }
    else if (net->TIDLLayers[layerIdx].outData.elementType == TIDL_UnsignedChar)
    {
      TIDL_TensorMinMax((uint8_t*)ptr, &net->TIDLLayers[layerIdx].outData, (int32_t *)&minTemp, (int32_t *)&maxTemp);

      if ( histogramPtr != NULL )
      {
        /* For global histogram always pick the range from histogram from previous stats collection
        iteration */
        if ( net->calibrationParams.activationRangeMethod == TIDL_ActivationRangeMethodGlobalHistogram )
        {
          minTemp = minActHist;
          maxTemp = maxActHist;
        }
        status = TIDL_TensorMinMaxHist((uint8_t *)ptr,
                                          &net->TIDLLayers[layerIdx].outData,
                                          numBins,
                                          net->calibrationParams.percentileActRangeShrink,
                                          histogramPtr,
                                          (int32_t*)&minTemp, (int32_t*)&maxTemp);

      }
    }
    else if (net->TIDLLayers[layerIdx].outData.elementType == TIDL_SignedShort)
    {
      TIDL_TensorMinMax((int16_t*)ptr, &net->TIDLLayers[layerIdx].outData, (int32_t *)&minTemp, (int32_t *)&maxTemp);

      if ( histogramPtr != NULL )
      {
        /* For global histogram always pick the range from histogram from previous stats collection
        iteration */
        if ( net->calibrationParams.activationRangeMethod == TIDL_ActivationRangeMethodGlobalHistogram )
        {
          minTemp = minActHist;
          maxTemp = maxActHist;
        }
        status = TIDL_TensorMinMaxHist((int16_t *)ptr,
                                          &net->TIDLLayers[layerIdx].outData,
                                          numBins,
                                          net->calibrationParams.percentileActRangeShrink,
                                          histogramPtr,
                                          (int32_t*)&minTemp, (int32_t*)&maxTemp);

      }
    }
    else if (net->TIDLLayers[layerIdx].outData.elementType == TIDL_UnsignedShort)
    {
      TIDL_TensorMinMax((uint16_t*)ptr, &net->TIDLLayers[layerIdx].outData, (int32_t *)&minTemp, (int32_t *)&maxTemp);

      if ( histogramPtr != NULL )
      {
        /* For global histogram always pick the range from histogram from previous stats collection
        iteration */
        if ( net->calibrationParams.activationRangeMethod == TIDL_ActivationRangeMethodGlobalHistogram )
        {
          minTemp = minActHist;
          maxTemp = maxActHist;
        }
        status = TIDL_TensorMinMaxHist((uint16_t *)ptr,
                                          &net->TIDLLayers[layerIdx].outData,
                                          numBins,
                                          net->calibrationParams.percentileActRangeShrink,
                                          histogramPtr,
                                          (int32_t*)&minTemp, (int32_t*)&maxTemp);

      }
    }
    else
    {
      status = TIDL_ERR_FAILURE;
    }
    min = ( float32_tidl) minTemp;
    max = ( float32_tidl) maxTemp;
  }
  else
  {
    TIDL_TensorMinMax((float32_tidl *)ptr, &net->TIDLLayers[layerIdx].outData, (float32_tidl *)&min, (float32_tidl *)&max);
    net->TIDLLayers[layerIdx].outData.tensorScale = 1.0;

#if 0
    if ( ( net->calibrationOption & TIDL_CalibOptionActivationRange ) == TIDL_CalibOptionActivationRange )
    {
      if ( net->calibrationParams.activationRangeMethod == TIDL_ActivationRangeMethodHistogram )
      {
        status = TIDL_TensorMinMaxHist((float32_tidl *)ptr,
                                           &net->TIDLLayers[layerIdx].outData,
                                           numBins,
                                           net->calibrationParams.percentileActRangeShrink,
                                           intAlgHandle->refScratchBuf,
                                           intAlgHandle->refScratchBufSize,
                                           (float32_tidl*)&min, (float32_tidl*)&max);
      }
    }
#endif
  }

  if(status == TIDL_SUCCESS)
  {
    if (net->TIDLLayers[layerIdx].outData.tensorScale != 0.0f)
    {
      float32_tidl curMin = (1.0f * ((float32_tidl)min - net->TIDLLayers[layerIdx].outData.tensorZeroPoint)) / net->TIDLLayers[layerIdx].outData.tensorScale;
      float32_tidl curMax = (1.0f * ((float32_tidl)max - net->TIDLLayers[layerIdx].outData.tensorZeroPoint)) / net->TIDLLayers[layerIdx].outData.tensorScale;
      if(intAlgHandle->createParams->quantRangeExpansionFactor != 1.0f)
      {
        curMin = curMin * intAlgHandle->createParams->quantRangeExpansionFactor;
        curMax = curMax * intAlgHandle->createParams->quantRangeExpansionFactor;
      }

      if ((intAlgHandle->procCallCounter == 0U) && (intAlgHandle->createParams->quantRangeUpdateFactor != 0 ) )
      {
        net->TIDLLayers[layerIdx].outData.minTensorValue = curMin;
        net->TIDLLayers[layerIdx].outData.maxTensorValue = curMax;
      }
      else
      {
        float32_tidl beta = intAlgHandle->createParams->quantRangeUpdateFactor;
        float32_tidl alpha = 1.0f - beta;

        if(intAlgHandle->createParams->quantRangeUpdateFactor == -1.0f)
        {
          beta = 1.0f / (intAlgHandle->procCallCounter + 1.0f);
          alpha = 1.0f - beta;
        }

        net->TIDLLayers[layerIdx].outData.minTensorValue = (net->TIDLLayers[layerIdx].outData.minTensorValue *alpha) + (curMin*beta);
        net->TIDLLayers[layerIdx].outData.maxTensorValue = (net->TIDLLayers[layerIdx].outData.maxTensorValue *alpha) + (curMax*beta);
      }

      /*In high precision mode, expand range by 2.5% in appropriate direction if saturation is detected for any calibration frame*/
      if(TIDL_isKernelHighPrecision(net->TIDLLayers[layerIdx].layerKernelType) && net->TIDLLayers[layerIdx].outData.elementType < TIDL_UnsignedWord)
      {
        int32_t satMin, satMax;
        TIDL_getSaturationLimits(net->TIDLLayers[layerIdx].outData.elementType, &satMin, &satMax);
        if(max == satMax)
        {
          net->TIDLLayers[layerIdx].outData.maxTensorValue *= 1.025;
        }
        else if(min == satMin)
        {
          net->TIDLLayers[layerIdx].outData.minTensorValue *= 1.025;
        }
      }
    }


    if (net->TIDLLayers[layerIdx].actParams.actType == TIDL_Clip)
    {
      net->TIDLLayers[layerIdx].outData.minTensorValue = net->TIDLLayers[layerIdx].actParams.clipMin;
      net->TIDLLayers[layerIdx].outData.maxTensorValue = net->TIDLLayers[layerIdx].actParams.clipMax;
    }

    if (net->TIDLLayers[layerIdx].actParams.actType == TIDL_RelU6)
    {
      if ( net->TIDLLayers[layerIdx].outData.maxTensorValue > 6.0f)
      {
        net->TIDLLayers[layerIdx].outData.maxTensorValue = 6.0f;
      }
    }

    /* For Argmax layer max is decided based on number of input channels*/
    if ( net->TIDLLayers[layerIdx].layerType == TIDL_ArgMaxLayer )
    {
      sTIDL_DataParams_t * indata = TIDL_getDataParams(net, net->TIDLLayers[layerIdx].inData[0]);
      net->TIDLLayers[layerIdx].outData.minTensorValue = 0.0;
      net->TIDLLayers[layerIdx].outData.maxTensorValue = (indata->dimValues[TIDL_DIM_NUMCH] -1);
    }

  }

  /* Restore the original buffer parameters */
  net->TIDLLayers[layerIdx].outData.dimValues[TIDL_DIM_WIDTH]  = dataBuffParamOrig.dimValues[TIDL_DIM_WIDTH];
  net->TIDLLayers[layerIdx].outData.dimValues[TIDL_DIM_HEIGHT] = dataBuffParamOrig.dimValues[TIDL_DIM_HEIGHT];
  net->TIDLLayers[layerIdx].outData.dimValues[TIDL_DIM_NUMCH]  = dataBuffParamOrig.dimValues[TIDL_DIM_NUMCH];
  net->TIDLLayers[layerIdx].outData.dimValues[TIDL_DIM_DIM2]  = dataBuffParamOrig.dimValues[TIDL_DIM_DIM2];
  net->TIDLLayers[layerIdx].outData.dimValues[TIDL_DIM_DIM1]  = dataBuffParamOrig.dimValues[TIDL_DIM_DIM1];
  net->TIDLLayers[layerIdx].outData.dimValues[TIDL_DIM_BATCH]  = dataBuffParamOrig.dimValues[TIDL_DIM_BATCH];

  net->TIDLLayers[layerIdx].outData.pitch[TIDL_LINE_PITCH]    = dataBuffParamOrig.pitch[TIDL_LINE_PITCH];
  net->TIDLLayers[layerIdx].outData.pitch[TIDL_CHANNEL_PITCH] = dataBuffParamOrig.pitch[TIDL_CHANNEL_PITCH];
  net->TIDLLayers[layerIdx].outData.pitch[TIDL_ROI_PITCH]     = dataBuffParamOrig.pitch[TIDL_ROI_PITCH];
  net->TIDLLayers[layerIdx].outData.pitch[TIDL_DIM1_PITCH]     = dataBuffParamOrig.pitch[TIDL_DIM1_PITCH];
  net->TIDLLayers[layerIdx].outData.pitch[TIDL_DIM2_PITCH]     = dataBuffParamOrig.pitch[TIDL_DIM2_PITCH];
  return status;
}

/**
 * @brief This function is to Update Tensor PerChannelMean
 *
 * @param intAlgHandle : tidl algorithm handle
 * @param layerIdx : index of the current layer
 * @param outDataIdx : index of the output data buffer
 * @param ptr : pointer to the tensor data
 * @return  IALG_EOK   - Successful
 *          IALG_EFAIL - Unspecified error
 */
int32_t TIDL_UpdateTensorPerChannelMean(TIDL_Handle intAlgHandle,
                                        int32_t layerIdx,
                                        int32_t outDataIdx,
                                        void * ptr)
{
  int32_t status = TIDL_SUCCESS;
  sTIDL_Network_t * net = intAlgHandle->createParams->net;
  if (TIDL_getDatElementSize(net->TIDLLayers[layerIdx].outData.elementType) <= 2)
  {
    if (net->TIDLLayers[layerIdx].outData.elementType == TIDL_SignedChar)
    {
      TIDL_TensorPerChannelMean(net,
                                             layerIdx,
                                             (int8_t*)ptr,
                                             &net->TIDLLayers[layerIdx].outData,
                                             intAlgHandle->refScratchBuf,
                                             intAlgHandle->refScratchBufSize,
                                             intAlgHandle->procCallCounter);
    }
    else if (net->TIDLLayers[layerIdx].outData.elementType == TIDL_UnsignedChar)
    {
      TIDL_TensorPerChannelMean(net,
                                             layerIdx,
                                             (uint8_t*)ptr,
                                             &net->TIDLLayers[layerIdx].outData,
                                             intAlgHandle->refScratchBuf,
                                             intAlgHandle->refScratchBufSize,
                                             intAlgHandle->procCallCounter);
    }
    else if (net->TIDLLayers[layerIdx].outData.elementType == TIDL_SignedShort)
    {
      TIDL_TensorPerChannelMean(net,
                                             layerIdx,
                                             (int16_t*)ptr,
                                             &net->TIDLLayers[layerIdx].outData,
                                             intAlgHandle->refScratchBuf,
                                             intAlgHandle->refScratchBufSize,
                                             intAlgHandle->procCallCounter);
    }
    else if (net->TIDLLayers[layerIdx].outData.elementType == TIDL_UnsignedShort)
    {
      TIDL_TensorPerChannelMean(net,
                                             layerIdx,
                                             (uint16_t*)ptr,
                                             &net->TIDLLayers[layerIdx].outData,
                                             intAlgHandle->refScratchBuf,
                                             intAlgHandle->refScratchBufSize,
                                             intAlgHandle->procCallCounter);
    }
    else
    {
      status = TIDL_ERR_FAILURE;
    }
  }
  else
  {
    if(net->TIDLLayers[layerIdx].outData.elementType == TIDL_SinglePrecFloat)
    {
      TIDL_TensorPerChannelMean(net,
                                           layerIdx,
                                           (float32_tidl*)ptr,
                                           &net->TIDLLayers[layerIdx].outData,
                                           intAlgHandle->refScratchBuf,
                                           intAlgHandle->refScratchBufSize,
                                           intAlgHandle->procCallCounter);
    }
  }

  return status;
}


/**
 * @brief This function is to update scale factors
 *
 * @param intAlgHandle : tidl algorithm instance handle
 * @param i : Current layer index
 * @param updateStats : flag to update stats
 * @param accMin : Min value in the accumlator buffer
 * @param accMax  : Max value in the accumlator buffer
 */
void TIDL_UpdateScaleFactors(TIDL_Handle intAlgHandle, int32_t i, int32_t updateStats, int64_t accMin, int64_t accMax)
{
  int32_t quantizationStyle = intAlgHandle->createParams->net->quantizationStyle;
  if(TIDL_QuantStyleAsymNP2_TFL != quantizationStyle) //Unnecessary
  {
    sTIDL_Network_t * net = intAlgHandle->createParams->net;
    int32_t elementSizeBytes = TIDL_getDatElementSize(net->TIDLLayers[i].outData.elementType);
    /* For float we don't have to call update anything for stats collection */
    if ( elementSizeBytes == 4 )
    {
      net->TIDLLayers[i].outData.roundBits = 0;
      net->TIDLLayers[i].outData.tensorScale = 1.0f;
    }
  }
  return;
}

//J7AM Code:

//This implementation is temporary : Future versions won't need this sort of reconstruction (Direct 32/64 - bit bias consumption w/o any sort of overhead)
/**
 * @brief The function does the bias split for the conv parameters
 *
 * @param srcPtr : Pointer to the source buffer
 * @param dstPtr : Pointer to the destination buffer
 * @param biasB : Pointer to the bias buffer
 * @param dataSize : no of output channels
 * @param inScaleFactor : scale factor for the input
 * @param satLow : Min value of the saturation
 * @param satHigh : Max value of the saturation
 * @param biasBMax : the max value of the bias values
 * @param inFeatSign : the sign of the input feature map
 */
template <class Tdst, class Tsrc>
void TIDL_AM_conv2dBiasSplit(Tsrc * srcPtr, Tdst * dstPtr, int32_t * biasB, int32_t dataSize, float32_tidl inScaleFactor, int32_t satLow, int32_t satHigh, int32_t biasBMax, int32_t inFeatSign)
{
  
  TIDL_conv2dBiasSplit(srcPtr, dstPtr, biasB, dataSize, inScaleFactor, satLow, satHigh, biasBMax, inFeatSign);
  
  //AM Specific bias restoration:
  //printf("inScaleFactor = %d\n",inScaleFactor);
  for (int32_t idx = 0; idx < dataSize; idx++)
  {
    //tidl_printf(2,"Bias B= %d && dstPtr[%d] = %d && srcPtr[%d] = %d\n",*biasB,idx,dstPtr[idx],idx,srcPtr[idx]);
    dstPtr[idx] *= (*(biasB));
  }

  return;
}



//
/**
 * @brief The function does the bias split for the conv parameters
 *
 * @param srcPtr : Pointer to the source buffer
 * @param dstPtr : Pointer to the destination buffer
 * @param biasB : Pointer to the bias buffer
 * @param dataSize : no of output channels
 * @param inScaleFactor : scale factor for the input
 * @param satLow : Min value of the saturation
 * @param satHigh : Max value of the saturation
 * @param biasBMax : the max value of the bias values
 * @param inFeatSign : the sign of the input feature map
 */
template <class Tdst, class Tsrc>
void TIDL_conv2dBiasSplitWithFixedBiasB(Tsrc * srcPtr,
                                        Tdst * dstPtr,
                                        int32_t * biasB,
                                        int32_t dataSize,
                                        float32_tidl inScaleFactor,
                                        int32_t satLow,
                                        int32_t satHigh,
                                        int32_t biasBMax,
                                        int32_t inFeatSign)
{
  if(inScaleFactor == 0)
  {
    for (int32_t idx = 0; idx < dataSize; idx++)
    {
        dstPtr[idx] = 0;
    }
  }
  else
  {
    int32_t orgBiasVal, biasVal = 0;
    if (inFeatSign == 1)
    {
      biasBMax = biasBMax / 2;
    }

    int32_t min, max, temp;
    if (inFeatSign == 1)
    {
      max = biasBMax;
    }
    else
    {
      max = biasBMax/2;
    }
    temp = max+1;
    min = -1 * temp;

    for (int32_t idx = 0; idx < dataSize; idx++)
    {
      orgBiasVal = srcPtr[idx] * inScaleFactor;
      biasVal = (orgBiasVal < satLow) ? satLow : orgBiasVal;
      biasVal = (biasVal >  satHigh) ? satHigh : biasVal;
      temp = (biasVal / (*biasB));
      temp = (temp < min) ? min : temp;
      temp = (temp > max) ? max : temp;
      dstPtr[idx] = temp;
    }
  }
  return;
}

/**
 * @brief The function does the bias split for the conv parameters
 *
 * @param srcPtr : Pointer to the source buffer
 * @param dstPtr : Pointer to the destination buffer
 * @param biasB : Pointer to the bias buffer
 * @param dataSize : no of output channels
 * @param inScaleFactor : scale factor for the input
 * @param satLow : Min value of the saturation
 * @param satHigh : Max value of the saturation
 * @param biasBMax : the max value of the bias values
 * @param inFeatSign : the sign of the input feature map
 */
template <class Tdst, class Tsrc>
void TIDL_conv2dBiasSplit(Tsrc * srcPtr, Tdst * dstPtr, int32_t * biasB, int32_t dataSize, float32_tidl inScaleFactor, int32_t satLow, int32_t satHigh, int32_t biasBMax, int32_t inFeatSign)
{
  if(inScaleFactor == 0)
  {
    for (int32_t idx = 0; idx < dataSize; idx++)
    {
        dstPtr[idx] = 0;
    }
    *biasB = 0;
  }
  else
  {
#if USE_16BIT_BIAS_FOR_8BIT_MODE
    int64_t orgBiasVal, biasVal = 0;
    int64_t biasBMaxNew = biasBMax;

    if (inFeatSign == 1)
    {
      biasBMaxNew = biasBMax / 2;
    }
    if (inScaleFactor > biasBMaxNew)
    {
      int64_t temp;
      int64_t biasAMin, biasAMax;

      biasAMax = biasBMax/2;

      temp = biasAMax+1;
      biasAMin = -1 * temp;

      if ( (biasBMax == 0xFF) && (satHigh == TIDL_SAT_HI_INT32) )
      {
        /* Not the cleanest way to handle but to avoid function signature
        change doing it this way. This condition indicates signed input with
        16 bit processing and hence biasB is limited to 0xFF. In this case
        biasA can still be 16 bits*/
        biasAMin = std::numeric_limits<int16_t>::lowest();
        biasAMax = std::numeric_limits<int16_t>::max();
      }


      for (int32_t idx = 0; idx < dataSize; idx++)
      {
        orgBiasVal = srcPtr[idx] * inScaleFactor;
        biasVal = (orgBiasVal < satLow) ? satLow : orgBiasVal;
        biasVal = (biasVal >  satHigh) ? satHigh : biasVal;
        temp = (biasVal / biasBMaxNew);
        temp = (temp < biasAMin) ? biasAMin : temp;
        temp = (temp > biasAMax) ? biasAMax : temp;
        dstPtr[idx] = temp;
      }
      *biasB = biasBMaxNew;
   }
    else if (inScaleFactor > 1)
    {
      for (int32_t idx = 0; idx < dataSize; idx++)
      {
        dstPtr[idx] = srcPtr[idx];
      }
      *biasB = inScaleFactor;
    }
    else
    {
      for (int32_t idx = 0; idx < dataSize; idx++)
      {
        dstPtr[idx] = srcPtr[idx]* inScaleFactor;
      }
      *biasB = 1;
    }

#else
    int64_t biasVal = 0;
    int64_t orgBiasVal = 0;
    for (int32_t idx = 0; idx < dataSize; idx++)
    {
      orgBiasVal = srcPtr[idx] * inScaleFactor;
      biasVal = orgBiasVal < satLow ? satLow : orgBiasVal;
      biasVal = biasVal >  satHigh ? satHigh : biasVal;
      dstPtr[idx] = biasVal;
      if (orgBiasVal != biasVal)
      {
        tidl_printf(0,"Conv2D bias is greater than 16 bits\n");
      }
    }
    *biasB = 1;
#endif
  }
  return;
}

/**
 * @brief Function is to prepare transfer prooperties
 *
 * @param trMem  : pointer to trasfer memory
 */
int32_t  TIDL_prePareMemcpyTr
(
  void * trMem
)
{
  int32_t retVal;
  DmaUtilsAutoInc3d_TrPrepareParam trPrepParams;
  DmaUtilsAutoInc3d_TransferProp      transferProp;
  trPrepParams.channelId = TIDL_DMA_CHANNEL_MEMCPY;
  trPrepParams.numTRs    = 1U;
  trPrepParams.trMem          = (uint8_t*)trMem;
  trPrepParams.trMemSize  = 64U;


  transferProp.syncType = (uint32_t)DMAUTILSAUTOINC3D_SYNC_2D;
  transferProp.dmaDfmt  = (uint32_t)DMAUTILSAUTOINC3D_DFMT_NONE;
  transferProp.circProp.circDir = (uint8_t)DMAUTILSAUTOINC3D_CIRCDIR_DST;
  transferProp.circProp.circSize1 = 0;
  transferProp.circProp.circSize2 = 0;
  transferProp.circProp.addrModeIcnt0 = (uint8_t)DMAUTILSAUTOINC3D_ADDR_LINEAR;
  transferProp.circProp.addrModeIcnt1 = (uint8_t)DMAUTILSAUTOINC3D_ADDR_LINEAR;
  transferProp.circProp.addrModeIcnt2 = (uint8_t)DMAUTILSAUTOINC3D_ADDR_LINEAR;
  transferProp.circProp.addrModeIcnt3 = (uint8_t)DMAUTILSAUTOINC3D_ADDR_LINEAR;

  transferProp.transferDim.sicnt0 = 1;
  transferProp.transferDim.sicnt1 =1;
  transferProp.transferDim.sicnt2 =1;
  transferProp.transferDim.sicnt3 =1;
  transferProp.transferDim.sdim1= 1;

  transferProp.transferDim.dicnt0 = 1;
  transferProp.transferDim.dicnt1 = 1;
  transferProp.transferDim.dicnt2 =1;
  transferProp.transferDim.dicnt3 =1;
  transferProp.transferDim.ddim1 = 1;

  transferProp.ioPointers.srcPtr = (uint8_t *)NULL;
  transferProp.ioPointers.dstPtr = (uint8_t *)NULL;

  retVal = DmaUtilsAutoInc3d_prepareTr( &trPrepParams,&transferProp);

  if( retVal == DMAUTILS_SOK) //DMA utils returned ok Status
    retVal = IALG_EOK;
  else
  {
    tidl_printf(0, "DmaUtilsAutoInc3d_prepareTr returned Error code");
    retVal = IALG_EFAIL;
  }
  return retVal;
}

/*
 * Async mode of transfer when Autoincrement TR is not possible
 * and src and dst needs to be accessed in 2D pattern
 *
 * This API uses the same channel as TIDL_memcpy2D which is
 * a blocking call and therefore the following sequence should
 * be avoided at all costs
 *
 * TIDL_memcpy2DAsyncTrigger();
 * TIDL_memcpy2D();
 * TIDL_memcpy2DAsyncWait();
 *
 * TODO: Fix this in a nice way
 */

/**
 * @brief Memory copy for Async trigger
 *
 * @param dstPtr : Pointer to dst memory
 * @param srcPtr : Pointer to src memory
 * @param width : Width of the memory copy
 * @param height : Height of the memory copy
 * @param dstStride : Stride of the dst buffer
 * @param srcStride : Stride of the src buffer
 * @param dmaUtilsContext : memory for dmaUtilsContext
 * @param trMem : pointer to transfer memory
 */
void TIDL_memcpy2DAsyncTrigger(const void   *dstPtr,
    const void   *srcPtr,
    uint16_t     width,
    uint16_t     height,
    int32_t      dstStride,
    int32_t      srcStride,
    void * dmaUtilsContext,
    int32_t   convert16bitTo8Bit,
    uint8_t * trMem)
{
  if(dmaUtilsContext != NULL)
  {
    DmaUtilsAutoInc3d_TrPrepareParam trPrepParam;
    int32_t fmtFlagsBkp;
#ifdef USE_STACK_MEM_TR
    uint8_t    trMemStack[64];
    TIDL_prePareMemcpyTr(trMemStack);
    trMem     = trMemStack;
#endif
    uint32_t convertMask = DMAUTILSAUTOINC3D_ADDRCONVERTMASK_SRCADDR |
                            DMAUTILSAUTOINC3D_ADDRCONVERTMASK_DSTADDR;
    trPrepParam.channelId = TIDL_DMA_CHANNEL_MEMCPY;
    trPrepParam.numTRs    = 1;
    trPrepParam.trMemSize = 64;
    trPrepParam.trMem     = trMem;



    CSL_UdmapTR * tr;
    tr = (CSL_UdmapTR *) trMem;
    tr->addr   = (uintptr_t)srcPtr;
    tr->daddr  = (uintptr_t)dstPtr;
    tr->icnt0  = width;
    tr->icnt1  = height;
    tr->dicnt0 = width;
    tr->dicnt1 = height;
    tr->dim1   = srcStride;
    tr->ddim1  = dstStride;
    fmtFlagsBkp = (tr->fmtflags);
    if ( convert16bitTo8Bit == 1)
    {
      /* Enable conversion from 16 bit to 8 bit */
      tr->fmtflags = (tr->fmtflags) | CSL_FMK(UDMAP_TR_FMTFLAGS_ELYPE, CSL_UDMAP_TR_FMTFLAGS_ELYPE_2_1);
    }

    DmaUtilsAutoInc3d_convertTrVirtToPhyAddr(dmaUtilsContext, &trPrepParam, convertMask);

    /* Re-use the same contexxt as feat for memcpy */
    (void)DmaUtilsAutoInc3d_configure(dmaUtilsContext, TIDL_DMA_CHANNEL_MEMCPY, trMem, 1U);
    tr->fmtflags = fmtFlagsBkp;

    (void)DmaUtilsAutoInc3d_trigger(dmaUtilsContext, TIDL_DMA_CHANNEL_MEMCPY);
  }
  else
  {

    uint32_t rowCnt;

    tidl_printf(0, "Calling TIDL_memcpy2DTrigger with dmaUtilsContext == NULL\n");

    for (rowCnt = 0; rowCnt < height; rowCnt++)
    {
      memcpy((uint8_t *)dstPtr + rowCnt * dstStride, (uint8_t *)srcPtr + rowCnt * srcStride, width);
    }
  }
}

/**
 * @brief Function for memory copy Async wait
 *
 * @param dmaUtilsContext : memory for dmaUtilsContext
 */
void TIDL_memcpy2DAsyncWait(void * dmaUtilsContext)
{
  if(dmaUtilsContext != NULL)
  {
    DmaUtilsAutoInc3d_wait(dmaUtilsContext, TIDL_DMA_CHANNEL_MEMCPY);
    //DmaUtilsAutoInc3d_deconfigure(dmaUtilsContext, TIDL_DMA_CHANNEL_MEMCPY, trMem, 1U);
  }
}

/**
 * @brief Memory copy for trigger
 *
 * @param dstPtr : Pointer to dst memory
 * @param srcPtr : Pointer to src memory
 * @param width : Width of the memory copy
 * @param height : Height of the memory copy
 * @param dstStride : Stride of the dst buffer
 * @param srcStride : Stride of the src buffer
 * @param dmaUtilsContext : memory for dmaUtilsContext
 * @param trMem : pointer to transfer memory
 */
int32_t  TIDL_memcpy2D
(
  const void   *dstPtr,
  const void   *srcPtr,
  uint32_t     width,
  uint16_t     height,
  int32_t      dstStride,
  int32_t      srcStride,
  void * dmaUtilsContext,
  uint8_t * trMem
  )
{
  int32_t retVal = IALG_EOK;
  uint32_t completeTransWidth = 0;
  if(dmaUtilsContext != NULL)
  {
    DmaUtilsAutoInc3d_TrPrepareParam trPrepParam;
#ifdef USE_STACK_MEM_TR
    uint8_t    trMemStack[64];
    TIDL_prePareMemcpyTr(trMemStack);
    trMem     = trMemStack;
#endif
    uint32_t convertMask = DMAUTILSAUTOINC3D_ADDRCONVERTMASK_SRCADDR |
                            DMAUTILSAUTOINC3D_ADDRCONVERTMASK_DSTADDR;
    trPrepParam.channelId = TIDL_DMA_CHANNEL_MEMCPY;
    trPrepParam.numTRs    = 1;
    trPrepParam.trMemSize = 64;
    trPrepParam.trMem     = trMem;

    CSL_UdmapTR * tr;
    tr = (CSL_UdmapTR *) trMem;
    tr->addr   = (uintptr_t)srcPtr;
    tr->daddr  = (uintptr_t)dstPtr;
    // tr->icnt0  = width;
    tr->icnt1  = height;
    // tr->dicnt0 = width;
    tr->dicnt1 = height;
    tr->dim1   = srcStride;
    tr->ddim1  = dstStride;

    while ((width > 0) && (retVal == IALG_EOK))
    {
      uint32_t transWidth = (MIN(((1 << 16) - 1), width));
      tr->icnt0  = transWidth;
      tr->dicnt0 = transWidth;
      tr->addr   = (uintptr_t)srcPtr + completeTransWidth;
      tr->daddr  = (uintptr_t)dstPtr + completeTransWidth;

      width = width - ((uint32_t )transWidth);
      completeTransWidth += transWidth;

      retVal = DmaUtilsAutoInc3d_convertTrVirtToPhyAddr(dmaUtilsContext, &trPrepParam, convertMask);

      if(retVal != DMAUTILS_SOK)
      {
        tidl_printf(0, "DMA utils function DmaUtilsAutoInc3d_convertTrVirtToPhyAddr returned Error Code");
        retVal = IALG_EFAIL;
        return retVal;
      }

      /* Re-use the same contexxt as feat for memcpy */
      retVal = DmaUtilsAutoInc3d_configure(dmaUtilsContext, TIDL_DMA_CHANNEL_MEMCPY, trMem, 1U);

      if(retVal != DMAUTILS_SOK)
      {
        tidl_printf(0, "DMA utils function DmaUtilsAutoInc3d_configure returned Error Code");
        retVal = IALG_EFAIL;
        return retVal;
      }

      DmaUtilsAutoInc3d_trigger(dmaUtilsContext, TIDL_DMA_CHANNEL_MEMCPY);
      DmaUtilsAutoInc3d_wait(dmaUtilsContext, TIDL_DMA_CHANNEL_MEMCPY);

    }
    DmaUtilsAutoInc3d_deconfigure(dmaUtilsContext, TIDL_DMA_CHANNEL_MEMCPY, trMem, 1U);
  }
  else
  {
    uint32_t rowCnt;

    for (rowCnt = 0; rowCnt < height; rowCnt++)
    {
      memcpy((uint8_t *)dstPtr + rowCnt * dstStride, (uint8_t *)srcPtr + rowCnt * srcStride, width);
    }
  }

  return 0;
}

template void TIDL_conv2dBiasSplit<int16_t, int16_t>(int16_t * srcPtr, int16_t * dstPtr, int32_t * biasB, int32_t dataSize, float32_tidl inScaleFactor, int32_t satLow, int32_t satHigh, int32_t biasBMax, int32_t inFeatSign);
template void TIDL_conv2dBiasSplit<int8_t, int16_t>(int16_t * srcPtr, int8_t * dstPtr, int32_t * biasB, int32_t dataSize, float32_tidl inScaleFactor, int32_t satLow, int32_t satHigh, int32_t biasBMax, int32_t inFeatSign);
template void TIDL_conv2dBiasSplit<int32_t, int16_t>(int16_t * srcPtr, int32_t * dstPtr, int32_t * biasB, int32_t dataSize, float32_tidl inScaleFactor, int32_t satLow, int32_t satHigh, int32_t biasBMax, int32_t inFeatSign);

template void TIDL_conv2dBiasSplit<int16_t, int32_t>(int32_t * srcPtr, int16_t * dstPtr, int32_t * biasB, int32_t dataSize, float32_tidl inScaleFactor, int32_t satLow, int32_t satHigh, int32_t biasBMax, int32_t inFeatSign);
template void TIDL_conv2dBiasSplit<int8_t, int32_t>(int32_t * srcPtr, int8_t * dstPtr, int32_t * biasB, int32_t dataSize, float32_tidl inScaleFactor, int32_t satLow, int32_t satHigh, int32_t biasBMax, int32_t inFeatSign);
template void TIDL_conv2dBiasSplit<int32_t, int32_t>(int32_t * srcPtr, int32_t * dstPtr, int32_t * biasB, int32_t dataSize, float32_tidl inScaleFactor, int32_t satLow, int32_t satHigh, int32_t biasBMax, int32_t inFeatSign);

//MMAv2
template void TIDL_AM_conv2dBiasSplit<int32_t, int16_t>(int16_t * srcPtr, int32_t * dstPtr, int32_t * biasB, int32_t dataSize, float32_tidl inScaleFactor, int32_t satLow, int32_t satHigh, int32_t biasBMax, int32_t inFeatSign);
template void TIDL_AM_conv2dBiasSplit<int64_t, int16_t>(int16_t*, int64_t*, int*, int, float, int, int, int, int);
//
template void TIDL_conv2dBiasSplitWithFixedBiasB<int16_t, int16_t>(int16_t * srcPtr, int16_t * dstPtr, int32_t * biasB, int32_t dataSize, float32_tidl inScaleFactor, int32_t satLow, int32_t satHigh, int32_t biasBMax, int32_t inFeatSign);
template void TIDL_conv2dBiasSplitWithFixedBiasB<int8_t, int16_t>(int16_t * srcPtr, int8_t * dstPtr, int32_t * biasB, int32_t dataSize, float32_tidl inScaleFactor, int32_t satLow, int32_t satHigh, int32_t biasBMax, int32_t inFeatSign);
template void TIDL_conv2dBiasSplitWithFixedBiasB<int32_t, int16_t>(int16_t * srcPtr, int32_t * dstPtr, int32_t * biasB, int32_t dataSize, float32_tidl inScaleFactor, int32_t satLow, int32_t satHigh, int32_t biasBMax, int32_t inFeatSign);

template void TIDL_conv2dBiasSplitWithFixedBiasB<int16_t, int32_t>(int32_t * srcPtr, int16_t * dstPtr, int32_t * biasB, int32_t dataSize, float32_tidl inScaleFactor, int32_t satLow, int32_t satHigh, int32_t biasBMax, int32_t inFeatSign);
template void TIDL_conv2dBiasSplitWithFixedBiasB<int8_t, int32_t>(int32_t * srcPtr, int8_t * dstPtr, int32_t * biasB, int32_t dataSize, float32_tidl inScaleFactor, int32_t satLow, int32_t satHigh, int32_t biasBMax, int32_t inFeatSign);
template void TIDL_conv2dBiasSplitWithFixedBiasB<int32_t, int32_t>(int32_t * srcPtr, int32_t * dstPtr, int32_t * biasB, int32_t dataSize, float32_tidl inScaleFactor, int32_t satLow, int32_t satHigh, int32_t biasBMax, int32_t inFeatSign);

void * my_malloc(int32_t size)
{
  void *ptr;

  ptr = malloc(size);
  return ptr;
}

void my_free(void *ptr)
{
  //fprintf(fpAlloc, "Free: Ptr: %0x\n",ptr);
  //fflush(fpAlloc);
  free(ptr);
}

/**
 * @brief Function to initilize the debug parameters
 *
 * @param traceLogLevel : debug  level for trace log
 * @param traceWriteLevel : debug level for trace write
 * @param tifVprintfFuncPtr : Pointer to print buffer
 * @param writeBinFuncPtr : Pointer to write bins
 * @param readBinFuncPtr : Pointer to the read bins
 * @param traceBaseName : Name of the trace buffer
 * @return int32_t : Returns SUCCESS or FAIL
 */
int32_t TIDL_initDebugTraceParams(int32_t traceLogLevel, int32_t traceWriteLevel, tidlVprintf_t tifVprintfFuncPtr, tidlWriteBinToFile_t writeBinFuncPtr,
                                    tidlReadBinFromFile_t readBinFuncPtr, const char * traceBaseName, void * tracePtr)
{
  int32_t status = TIDL_SUCCESS;

  if (((traceLogLevel > 0) && (tifVprintfFuncPtr == NULL)) ||
      ((traceWriteLevel > 0) && (writeBinFuncPtr == NULL)))
  {
    status = TIDL_ERR_FAILURE;
  }

  if(status == TIDL_SUCCESS)
  {
    tidlWriteBinToFile = (tidlWriteBinToFile_t)writeBinFuncPtr;
    tidlReadBinFromFile = (tidlReadBinFromFile_t)readBinFuncPtr;
    tidlVprintf = (tidlVprintf_t)tifVprintfFuncPtr;
    tidlLogLevel = traceLogLevel;
    tidlWriteLevel = traceWriteLevel;
    strncpy(tidlTraceBaseName, (char *)traceBaseName, TIDL_STRING_SIZE);
  }
  return status;
}


void tidl_printf(int32_t traceLevel, const char *format, ...)
{
  va_list args;

  if (traceLevel < tidl_getTraceLogLevel())
  {
    (void)va_start(args, format);
    (void)tidl_getVprintf()(format,args);
    va_end(args);
  }
}

float32_tidl * tidl_convertParamsToFloat(void * buf,
                                               int32_t numElements,
                                               int32_t dataBits,
                                               float32_tidl dataScale,
                                               void * scratchPtr,
                                               int32_t scratchSize)
{
  float32_tidl * weightPtrFloat = NULL;

  if (( numElements  * sizeof(float32_tidl) <= scratchSize ) &&
      (dataScale != 0.0) )
  {
    int32_t i;
    weightPtrFloat = (float32_tidl *)scratchPtr;

    for ( i = 0; i < numElements; i++ )
    {
      if (dataBits <= 8 )
      {
        int8_t * inPtr = (int8_t *)buf;
        weightPtrFloat[i] = (float32_tidl)inPtr[i] / dataScale;

      }
      else if (dataBits <= 16 )
      {
        int16_t * inPtr = (int16_t *)buf;
        weightPtrFloat[i] = ((float32_tidl)inPtr[i]) / dataScale;
      }
      else if (dataBits == 32 )
      {
        float32_tidl * inPtr = (float32_tidl *)buf;
        weightPtrFloat[i] = inPtr[i] / dataScale;
      }
      else
      {
        tidl_printf(0,"Invalid dataBits in tidl_convertParamsToFloat\n");
        weightPtrFloat = NULL;
        break;
      }
    }
  }

  return weightPtrFloat;
}


int32_t tidl_writeTraceParamBuf(sTIDL_Network_t * net,
                                      sTIDL_AlgLayer_t     * algLayer,
                                      sTIDL_Layer_t        * tidlLayer,
                                      int32_t dataBuffId,
                                      int8_t * orgScratchPtr,
                                      uint32_t orgScratchSize)
{
  int32_t status = TIDL_SUCCESS;

  if (tidl_getWriteLevel() == 4)
  {
    if ( ( tidlLayer->layerType == TIDL_ConvolutionLayer ) ||
         ( tidlLayer->layerType == TIDL_Deconv2DLayer) ||
         ( tidlLayer->layerType == TIDL_BatchNormLayer) ||
         ( tidlLayer->layerType == TIDL_BatchReshapeLayer) ||
         ( tidlLayer->layerType == TIDL_InnerProductLayer))
    {
      int8_t * scratchPtr =  (int8_t *)(orgScratchPtr  + TRACE_STRINGS_MEM_SIZE);
      int8_t * traceDumpName = (int8_t *)(orgScratchPtr);
      int8_t * traceStringName = (int8_t *)(orgScratchPtr  + (1*TRACE_STRING_SIZE));
      int8_t * DataIDString  = (int8_t *)(orgScratchPtr  + (2*TRACE_STRING_SIZE));
      int32_t scratchSize =  orgScratchSize - TRACE_STRINGS_MEM_SIZE;
      int32_t dataSize = 0;

      if(tidl_getTraceBaseName() != NULL)
      {
        (void)strcpy((char *)traceDumpName, (char *)tidl_getTraceBaseName());
      }
      else
      {
        (void)strcpy((char *)traceDumpName, (char *)"");
      }

      (void)sprintf((char *)DataIDString, "%04d", dataBuffId);
      (void)strcat((char *)traceDumpName, (char *)DataIDString);
      (void)strcat((char *)traceDumpName, "_");

      if((tidlLayer->layerType == TIDL_ConvolutionLayer) || (tidlLayer->layerType == TIDL_Deconv2DLayer))
      {
        void * weightPtr = ((int8_t *)net + tidlLayer->layerParams.convParams.weights);
        float32_tidl * weightPtrFloat;

        dataSize = ( tidlLayer->layerParams.convParams.numOutChannels *
                   tidlLayer->layerParams.convParams.numInChannels *
                   tidlLayer->layerParams.convParams.kernelW *
                   tidlLayer->layerParams.convParams.kernelH )/
                   tidlLayer->layerParams.convParams.numGroups;

        weightPtrFloat = tidl_convertParamsToFloat(weightPtr,
                                                        dataSize,
                                                        tidlLayer->weightsElementSizeInBits,
                                                        tidlLayer->layerParams.convParams.weightScale,
                                                        scratchPtr,
                                                        scratchSize);

        if ( weightPtrFloat == NULL )
        {
          status = TIDL_ERR_FAILURE;
          tidl_printf(0, " In sufficient Memory to write Weight Traces\n");
          goto Exit;
        }
        sprintf((char *)traceStringName, "%sweights_float.bin", traceDumpName);


        (void)tidl_getWriteBinToFile()((const char *)traceStringName, weightPtrFloat, dataSize*sizeof(float32_tidl), 1);

        if (tidlLayer->layerParams.convParams.enableBias)
        {
          float32_tidl * biasPtrFloat;
          void * biasPtr = ((int8_t *)(net) + tidlLayer->layerParams.convParams.bias);
          dataSize = tidlLayer->layerParams.convParams.numOutChannels;

          //:TODO: Ideally this we should dump after bias split
          biasPtrFloat = tidl_convertParamsToFloat(biasPtr,
                                                  dataSize,
                                                  tidlLayer->weightsElementSizeInBits,
                                                  tidlLayer->layerParams.convParams.biasScale,
                                                  scratchPtr,
                                                  scratchSize);
          if ( biasPtrFloat == NULL )
          {
            status = TIDL_ERR_FAILURE;
            tidl_printf(0, " In sufficient Memory to write Bias Traces\n");

            goto Exit;
          }

          sprintf((char *)traceStringName, "%sbias_float.bin", traceDumpName);
         (void)tidl_getWriteBinToFile()((const char *)traceStringName, biasPtrFloat, dataSize*sizeof(float32_tidl), 1);

        }
      }

    }
    /** Wb/inv L1D/L2 cache afte writing traces*/
    TIDL_enableL1DandL2CacheWb();
  }
Exit:
  return status;
}


/**
 * @brief Function to write traces from data buffer
 *
 * @param ptr : Pointer to the data buffer
 * @param net : tidl network structure
 * @param dataBuffId : id of the databuffer
 * @param orgScratchPtr : pointer to the scratch buffer
 * @param orgScratchSize : Size of the scratch buffer
 * @param startHeight   : Offset to the current line
 * @param startChannel : Offset to the current channel
 * @param height : No of lines for writting
 * @param numChannels : No of channels for writting
 * @param currChPitch : current channel pitch
 *
 */
int32_t tidl_writeTraceDataBuf(int8_t * ptr,
                              sTIDL_Network_t * net,
                              int32_t dataBuffId,
                              int8_t * orgScratchPtr,
                              uint32_t orgScratchSize,
                              int32_t startHeight,
                              int32_t startChannel,
                              int32_t height,
                              int32_t numChannels,
                              int32_t currChPitch,
                              int32_t coreId,
                              const char * traceBaseNameCreateParams)
{
  int32_t status = TIDL_SUCCESS;
  if (tidl_getWriteLevel() > 0)
  {
    int8_t * scratchPtr =  (int8_t *)(orgScratchPtr  + TRACE_STRINGS_MEM_SIZE);
    int8_t * traceDumpName = (int8_t *)(orgScratchPtr);
    int8_t * traceBaseName = (int8_t *)(orgScratchPtr  + (1*TRACE_STRING_SIZE));
    int8_t * DataIDString  = (int8_t *)(orgScratchPtr  + (2*TRACE_STRING_SIZE));
    int32_t layerIdx;
    int32_t scratchSize =  orgScratchSize - TRACE_STRINGS_MEM_SIZE;
    int32_t tidlWriteLevelOrig = tidl_getWriteLevel();
    sTIDL_DataParams_t * dataBuffParam;
    dataBuffParam = TIDL_getDataParams(net, dataBuffId);
    layerIdx = TIDL_getLayerNum(net, dataBuffId);


    /* for trace Level 4 enable trace level 3 by default */
    if ( tidl_getWriteLevel() == 4 )
    {
      tidl_setWriteLevel(3) ;
    }
    if (dataBuffParam != NULL)
    {
     sTIDL_DataParams_t dataBuffParamOrig = *dataBuffParam;

      if ( net->TIDLLayers[layerIdx].layerType == TIDL_DataConvertLayer )
      {
        int32_t inLayout = net->TIDLLayers[layerIdx].layerParams.dataConvertParams.layout, outLayout = net->TIDLLayers[layerIdx].layerParams.dataConvertParams.outLayout;

        if ( (net->TIDLLayers[layerIdx].layerParams.dataConvertParams.layout == TIDL_LT_NHWC && net->TIDLLayers[layerIdx].layerParams.dataConvertParams.type == TIDL_DC_TYPE_OUTPUT) ||
             (net->TIDLLayers[layerIdx].layerParams.dataConvertParams.type == TIDL_DC_TYPE_INTERMEDIATE) && (inLayout != outLayout))
        {
          dataBuffParam->dimValues[TIDL_DIM_WIDTH]  = dataBuffParamOrig.dimValues[TIDL_DIM_NUMCH];
          dataBuffParam->dimValues[TIDL_DIM_HEIGHT] = dataBuffParamOrig.dimValues[TIDL_DIM_WIDTH];
          dataBuffParam->dimValues[TIDL_DIM_NUMCH]  = dataBuffParamOrig.dimValues[TIDL_DIM_HEIGHT];
          dataBuffParam->dimValues[TIDL_DIM_DIM2]  = dataBuffParamOrig.dimValues[TIDL_DIM_DIM2];
          dataBuffParam->dimValues[TIDL_DIM_DIM1]  = dataBuffParamOrig.dimValues[TIDL_DIM_DIM1];
          dataBuffParam->dimValues[TIDL_DIM_BATCH]  = dataBuffParamOrig.dimValues[TIDL_DIM_BATCH];

          dataBuffParam->pitch[TIDL_LINE_PITCH]    = dataBuffParam->dimValues[TIDL_DIM_WIDTH];
          dataBuffParam->pitch[TIDL_CHANNEL_PITCH] = dataBuffParam->pitch[TIDL_LINE_PITCH] * dataBuffParam->dimValues[TIDL_DIM_HEIGHT];
          dataBuffParam->pitch[TIDL_DIM2_PITCH]     = dataBuffParam->pitch[TIDL_CHANNEL_PITCH] * dataBuffParam->dimValues[TIDL_DIM_NUMCH];
          dataBuffParam->pitch[TIDL_DIM1_PITCH]     = dataBuffParam->pitch[TIDL_DIM2_PITCH]*dataBuffParam->dimValues[TIDL_DIM_DIM2];
          dataBuffParam->pitch[TIDL_ROI_PITCH]      = dataBuffParam->pitch[TIDL_DIM1_PITCH]*dataBuffParam->dimValues[TIDL_DIM_DIM1];
          //:TODO: This needs to be correctly handled for super tiling cases
          height = dataBuffParam->dimValues[TIDL_DIM_HEIGHT];
          numChannels     = dataBuffParam->dimValues[TIDL_DIM_NUMCH];
          currChPitch     = dataBuffParam->pitch[TIDL_CHANNEL_PITCH];
        }
      }
    if ((dataBuffParam->numBatchW>1 || dataBuffParam->numBatchH>1)&& (dataBuffParam->dimValues[TIDL_DIM_BATCH] == 1))
    {
      int32_t elementSizeBytes;
      elementSizeBytes = TIDL_getDatElementSize(dataBuffParam->elementType);
      if(elementSizeBytes == 4)
      {
        tidl_setWriteLevel(tidl_getWriteLevel() == 3 ? 1 : tidl_getWriteLevel());
      }

      int32_t dim1 = (dataBuffParam->dimValues[TIDL_DIM_DIM1]);
      int32_t dim2 = (dataBuffParam->dimValues[TIDL_DIM_DIM2]);
      int32_t outWidth = (dataBuffParam->dimValues[TIDL_DIM_WIDTH] - dataBuffParam->batchPadW * (dataBuffParam->numBatchW -1))/dataBuffParam->numBatchW;
      int32_t outHeight = dataBuffParam->dimValues[TIDL_DIM_HEIGHT];
      int32_t outChannels = dataBuffParam->dimValues[TIDL_DIM_NUMCH];
      int32_t outNumBatches = dataBuffParam->numBatchW;
      int32_t dataSize = outWidth * outHeight * outChannels * outNumBatches;
      int32_t linePitch = dataBuffParam->pitch[TIDL_LINE_PITCH];
      int32_t chPitch = dataBuffParam->pitch[TIDL_CHANNEL_PITCH];
      int32_t dim1Pitch = dataBuffParam->pitch[TIDL_DIM1_PITCH];
      int32_t dim2Pitch = dataBuffParam->pitch[TIDL_DIM2_PITCH];
      int32_t batchPitch  = dataBuffParam->pitch[TIDL_ROI_PITCH];
      int32_t inBlkPitchW        =  outWidth + dataBuffParam->batchPadW;
      int32_t paddeOutSize = linePitch * (dataBuffParam->dimValues[TIDL_DIM_HEIGHT] + (dataBuffParam->padH * 2) + 1);
      int32_t padedDataSize =  dataBuffParam->dimValues[TIDL_DIM_BATCH] * dataBuffParam->dimValues[TIDL_DIM_NUMCH] * paddeOutSize;
      int8_t * outPtr        = (int8_t *)scratchPtr;
      int8_t * outWithPadPtr = (int8_t *)(scratchPtr + (dataSize*elementSizeBytes));
      float32_tidl  * floatPtr = (float32_tidl  *)(scratchPtr + (dataSize*elementSizeBytes));
      int32_t i;
      int32_t j, k1, l, m;
      int32_t batchIdx;
      int32_t batchOffset;
      int32_t offset;
      int32_t totalSize = 0;

      if (tidl_getWriteLevel() == 1)
      {
        totalSize = (dataSize*elementSizeBytes);
      }
      else if (tidl_getWriteLevel() == 2)
      {
        totalSize = ((dataSize*elementSizeBytes) + (padedDataSize*elementSizeBytes));
      }
      else if (tidl_getWriteLevel() == 3)
      {
        totalSize = (dataSize*elementSizeBytes) + ((int32_t)sizeof(float32_tidl)*dataSize);
      }
      else
      {
        tidl_printf(0, "Un supported tidl_getWriteLevel() \n");
        status = TIDL_ERR_FAILURE;
      }

      if(dataBuffParam->elementType == TIDL_SinglePrecFloat)
      {
        float32_tidl curMin = FLT_MAX, curMax = -FLT_MAX;
        TIDL_TensorMinMax((float32_tidl*)ptr, dataBuffParam, &curMin, &curMax);
        tidl_printf(0, " %3d %10.5f %10.5f %10.5f %d\n", dataBuffId, dataBuffParam->tensorScale, curMin, curMax, dataBuffParam->elementType);
      }
      else
      {
        tidl_printf(0, " %3d %10.5f %10.5f %10.5f %d\n", dataBuffId, dataBuffParam->tensorScale, dataBuffParam->minTensorValue, dataBuffParam->maxTensorValue, dataBuffParam->elementType);
      }


      if ((totalSize > scratchSize) && (status == TIDL_SUCCESS))
      {
        tidl_printf(0, " In sufficient Memory to write Traces, as requested size is : %d, allocated size : %d \n", totalSize, scratchSize);
        status = TIDL_ERR_FAILURE;
      }

      if(status == TIDL_SUCCESS)
      {
        if(traceBaseNameCreateParams != NULL)
        {
          (void)strcpy((char *)traceDumpName, (char *)traceBaseNameCreateParams);
        }
        else
        {
          (void)strcpy((char *)traceDumpName, (char *)"");
        }

        (void)sprintf((char *)DataIDString, "%s%04d_%04d_%04d_%05d_%05d_%05dx%05d",
                                            traceDumpName, dataBuffId,
                                            dataBuffParam->dimValues[TIDL_DIM_BATCH],
                                            dataBuffParam->dimValues[TIDL_DIM_DIM1],
                                            dataBuffParam->dimValues[TIDL_DIM_DIM2],
                                            dataBuffParam->dimValues[TIDL_DIM_NUMCH],
                                            dataBuffParam->dimValues[TIDL_DIM_WIDTH],
                                            dataBuffParam->dimValues[TIDL_DIM_HEIGHT]);

        (void)strcpy((char *)traceBaseName, (char *)DataIDString);


        if((startHeight != 0) || (startChannel != 0))
        {
          (void)strcpy((char *)traceDumpName, (char *)traceBaseName);
          (void)strcat((char *)traceDumpName, ".y");

          if(elementSizeBytes == 4)
          {
            (void)strcpy((char *)traceDumpName, (char *)traceBaseName);
            (void)strcat((char *)traceDumpName, "_float.bin");
          }

          (void)tidl_getReadBinFromFile()((const char *)traceDumpName, outPtr, dataSize*elementSizeBytes, 1);
          if (tidl_getWriteLevel() == 2)
          {
            (void)strcpy((char *)traceDumpName, (char *)traceBaseName);
            (void)strcat((char *)traceDumpName, "_paded.y");
            (void)tidl_getReadBinFromFile()((const char *)traceDumpName, outWithPadPtr, padedDataSize*elementSizeBytes, 1);
          }

          if (tidl_getWriteLevel() == 3)
          {
            (void)strcpy((char *)traceDumpName, (char *)traceBaseName);
            (void)strcat((char *)traceDumpName, "_float.bin");
            (void)tidl_getReadBinFromFile()((const char *)traceDumpName, floatPtr, dataSize*((int32_t)sizeof(float32_tidl)), 1);
          }
        }

      for (batchIdx = 0; batchIdx < 1; batchIdx++)
      {
        batchOffset = dim1 *dim2 *outChannels  * outWidth* outHeight;

        for(m = 0; m < dim1; m++)
        {
            for(l = 0; l < dim2; l++)
            {
               for (j = 0; j < numChannels; j++)
               {
                 for (i = 0; i < height; i++)
                 {
                   for (k1 = 0; k1 < outNumBatches; k1++)
                   {
                     offset = m *dim2 *outChannels *outHeight * outWidth + l*outChannels*outHeight * outWidth +((j + startChannel)*outHeight * outWidth) +
                              ((i + startHeight)*outWidth) + k1*batchOffset;
                   if (dataBuffParam->elementType == 0)
                   {
                     uint8_t * src, *dst;
                     int32_t k;
                     src = (uint8_t *)(ptr + (k1 * inBlkPitchW) + (m*dim1Pitch) + (l*dim2Pitch) + (j*currChPitch) + (i*linePitch) + (dataBuffParam->padH*linePitch) + dataBuffParam->padW);
                     dst = (uint8_t *)(outPtr + offset);
                     for (k = 0; k < outWidth; k++)
                     {
                       dst[k] = src[k];
                       if (tidl_getWriteLevel() == 3)
                       {
                         floatPtr[offset + k] = ((float32_tidl)src[k]) / (dataBuffParam->tensorScale);
                       }
                     }
                   }
                   else if (dataBuffParam->elementType == 1)
                   {
                     int8_t * src;
                     int8_t * dst;
                     int32_t k;
                     src = (ptr + (k1 * inBlkPitchW) + (m*dim1Pitch) + (l*dim2Pitch) + (j*currChPitch) + (i*linePitch) + (dataBuffParam->padH*linePitch) + dataBuffParam->padW);
                     dst = (outPtr + offset);
                     for (k = 0; k < outWidth; k++)
                     {
                       //dst[k] = src[k] + 128;
                       dst[k] = src[k];
                       if (tidl_getWriteLevel() == 3)
                       {
                         floatPtr[offset + k] = ((float32_tidl)src[k]) / (dataBuffParam->tensorScale);
                       }
                     }
                   }
                   else if (dataBuffParam->elementType == 2)
                   {
                     uint16_t * src, *dst;
                     int32_t k;
                     src = ((uint16_t *)ptr + (k1 * inBlkPitchW) + (m*dim1Pitch) + (l*dim2Pitch) + (j*currChPitch) + (i*linePitch) + (dataBuffParam->padH*linePitch) + dataBuffParam->padW);
                     dst = ((uint16_t *)outPtr + offset);
                     for (k = 0; k < outWidth; k++)
                     {
                       dst[k] = src[k];
                       if (tidl_getWriteLevel() == 3)
                       {
                         floatPtr[offset + k] = ((float32_tidl)src[k]) / (dataBuffParam->tensorScale);
                       }
                     }
                   }
                   else if (dataBuffParam->elementType == 3)
                   {
                     int16_t * src, *dst;
                     int32_t k;
                     src = ((int16_t *)ptr + (k1 * inBlkPitchW) + (m*dim1Pitch) + (l*dim2Pitch) + (j*currChPitch) + (i*linePitch) + (dataBuffParam->padH*linePitch) + dataBuffParam->padW);
                     dst = ((int16_t *)outPtr + offset);
                     for (k = 0; k < outWidth; k++)
                     {
                       dst[k] = src[k];
                       if (tidl_getWriteLevel() == 3)
                       {
                         floatPtr[offset + k] = ((float32_tidl)src[k]) / (dataBuffParam->tensorScale);
                       }
                     }
                   }
                   else
                   {
                     (void)memcpy((outPtr + (offset*elementSizeBytes)),
                       (ptr + (((k1 * inBlkPitchW) + (m*dim1Pitch) + (l*dim2Pitch) + (j*currChPitch) + (i*linePitch) + (dataBuffParam->padH*linePitch) +
                         dataBuffParam->padW)*elementSizeBytes)),
                       (size_t)(outWidth * elementSizeBytes));
                   }
                 }
                 if (tidl_getWriteLevel() == 2)
                 {
                   if((height != dataBuffParam->dimValues[TIDL_DIM_HEIGHT]) && (startHeight != 0))
                   {
                     tidl_printf(0, "tidl_getWriteLevel() = 2 with ST is not supported now \n");
                     status = IALG_EFAIL;
                   }
                   else
                   {
                     (void)memcpy((outWithPadPtr + batchIdx *dataBuffParam->dimValues[TIDL_DIM_NUMCH] * paddeOutSize + j*paddeOutSize), (ptr + (batchIdx * batchPitch) + (j*chPitch)), paddeOutSize);
                   }
                 }
                }
               }
              }
             }
        (void)strcpy((char *)traceDumpName, (char *)traceBaseName);
        (void)strcat((char *)traceDumpName, ".y");
        if(elementSizeBytes == 4)
        {
          (void)strcpy((char *)traceDumpName, (char *)traceBaseName);
          (void)strcat((char *)traceDumpName, "_float.bin");
        }
        (void)tidl_getWriteBinToFile()((const char *)traceDumpName, outPtr, dataSize*elementSizeBytes, 1);
        if (tidl_getWriteLevel() == 2)
        {
          (void)strcpy((char *)traceDumpName, (char *)traceBaseName);
          (void)strcat((char *)traceDumpName, "_paded.y");
          (void)tidl_getWriteBinToFile()((const char *)traceDumpName, outWithPadPtr, padedDataSize*elementSizeBytes, 1);
        }

        if (tidl_getWriteLevel() == 3)
        {
          (void)strcpy((char *)traceDumpName, (char *)traceBaseName);
          (void)strcat((char *)traceDumpName, "_float.bin");
          (void)tidl_getWriteBinToFile()((const char *)traceDumpName, floatPtr, dataSize*((int32_t)sizeof(float32_tidl)), 1);
        }
      }
    }
    }
    else
    {
      int32_t elementSizeBytes;
      elementSizeBytes = TIDL_getDatElementSize(dataBuffParam->elementType);
      if(elementSizeBytes == 4)
      {
        tidl_setWriteLevel(tidl_getWriteLevel() == 3 ? 1 : tidl_getWriteLevel());
      }
      int32_t dim1 = dataBuffParam->dimValues[TIDL_DIM_DIM1];
      int32_t dim2 = dataBuffParam->dimValues[TIDL_DIM_DIM2];
      int32_t dataSize = dataBuffParam->dimValues[TIDL_DIM_BATCH]* dataBuffParam->dimValues[TIDL_DIM_DIM1] * dataBuffParam->dimValues[TIDL_DIM_DIM2]  * dataBuffParam->dimValues[TIDL_DIM_NUMCH] * dataBuffParam->dimValues[TIDL_DIM_HEIGHT] * dataBuffParam->dimValues[TIDL_DIM_WIDTH];
      int32_t linePitch = dataBuffParam->pitch[TIDL_LINE_PITCH];
      int32_t chPitch = dataBuffParam->pitch[TIDL_CHANNEL_PITCH];
      int32_t dim2Pitch = dataBuffParam->pitch[TIDL_DIM2_PITCH];
      int32_t dim1Pitch = dataBuffParam->pitch[TIDL_DIM1_PITCH];
      int32_t batchPitch  = dataBuffParam->pitch[TIDL_ROI_PITCH];
      int32_t paddeOutSize = linePitch * (dataBuffParam->dimValues[TIDL_DIM_HEIGHT] + (dataBuffParam->padH * 2) + 1);
      int32_t padedDataSize =  dataBuffParam->dimValues[TIDL_DIM_BATCH] * dataBuffParam->dimValues[TIDL_DIM_NUMCH] * paddeOutSize;
      int8_t * outPtr        = (int8_t *)scratchPtr;
      int8_t * outWithPadPtr = (int8_t *)(scratchPtr + (dataSize*elementSizeBytes));
      float32_tidl  * floatPtr = (float32_tidl  *)(scratchPtr + (dataSize*elementSizeBytes));
      int32_t i;
      int32_t j,l,m;
      int32_t batchIdx;
      int32_t batchOffset;
      int32_t offset;
      int32_t totalSize = 0;

      if (tidl_getWriteLevel() == 1)
      {
        totalSize = (dataSize*elementSizeBytes);
      }
      else if (tidl_getWriteLevel() == 2)
      {
        totalSize = ((dataSize*elementSizeBytes) + (padedDataSize*elementSizeBytes));
      }
      else if (tidl_getWriteLevel() == 3)
      {
        totalSize = (dataSize*elementSizeBytes) + ((int32_t)sizeof(float32_tidl)*dataSize);
      }
      else
      {
        tidl_printf(0, "Un supported tidl_getWriteLevel() \n");
        status = TIDL_ERR_FAILURE;
      }

      if(dataBuffParam->elementType == TIDL_SinglePrecFloat)
      {
        float32_tidl curMin = FLT_MAX, curMax = -FLT_MAX;
        TIDL_TensorMinMax((float32_tidl*)ptr, dataBuffParam, &curMin, &curMax);
        tidl_printf(0, " %3d %15.5f %15.5f %15.5f %d\n", dataBuffId, dataBuffParam->tensorScale, curMin, curMax, dataBuffParam->elementType);
      }
      else
      {
        tidl_printf(0, " %3d %15.5f %15.5f %15.5f %d\n", dataBuffId, dataBuffParam->tensorScale, dataBuffParam->minTensorValue, dataBuffParam->maxTensorValue, dataBuffParam->elementType);
      }


      if ((totalSize > scratchSize) && (status == TIDL_SUCCESS))
      {
        tidl_printf(0, " In sufficient Memory to write Traces, requested size is : %d, allocated size : %d \n", totalSize, scratchSize);
        status = TIDL_ERR_FAILURE;
      }

      if(status == TIDL_SUCCESS)
      {
        if(traceBaseNameCreateParams != NULL)
        {
          (void)strcpy((char *)traceDumpName, (char *)traceBaseNameCreateParams);
        }
        else
        {
          (void)strcpy((char *)traceDumpName, (char *)"");
        }

        (void)sprintf((char *)DataIDString, "%s%04d_%04d_%04d_%05d_%05d_%05dx%05d",
                                            traceDumpName, dataBuffId,
                                            dataBuffParam->dimValues[TIDL_DIM_BATCH],
                                            dataBuffParam->dimValues[TIDL_DIM_DIM1],
                                            dataBuffParam->dimValues[TIDL_DIM_DIM2],
                                            dataBuffParam->dimValues[TIDL_DIM_NUMCH],
                                            dataBuffParam->dimValues[TIDL_DIM_WIDTH],
                                            dataBuffParam->dimValues[TIDL_DIM_HEIGHT]);

        (void)strcpy((char *)traceBaseName, (char *)DataIDString);

        if((startHeight != 0) || (startChannel != 0))
        {
          (void)strcpy((char *)traceDumpName, (char *)traceBaseName);
          (void)strcat((char *)traceDumpName, ".y");

          if(elementSizeBytes == 4)
          {
            (void)strcpy((char *)traceDumpName, (char *)traceBaseName);
            (void)strcat((char *)traceDumpName, "_float.bin");
          }
          (void)tidl_getReadBinFromFile()((const char *)traceDumpName, outPtr, dataSize*elementSizeBytes, 1);
          if (tidl_getWriteLevel() == 2)
          {
            (void)strcpy((char *)traceDumpName, (char *)traceBaseName);
            (void)strcat((char *)traceDumpName, "_paded.y");
            (void)tidl_getReadBinFromFile()((const char *)traceDumpName, outWithPadPtr, padedDataSize*elementSizeBytes, 1);
          }

          if (tidl_getWriteLevel() == 3)
          {
            (void)strcpy((char *)traceDumpName, (char *)traceBaseName);
            (void)strcat((char *)traceDumpName, "_float.bin");
            (void)tidl_getReadBinFromFile()((const char *)traceDumpName, floatPtr, dataSize*((int32_t)sizeof(float32_tidl)), 1);
          }
        }

      for (batchIdx = 0; batchIdx < dataBuffParam->dimValues[TIDL_DIM_BATCH]; batchIdx++)
      {
        batchOffset = batchIdx  * dataBuffParam->dimValues[TIDL_DIM_DIM1] * dataBuffParam->dimValues[TIDL_DIM_DIM2] * dataBuffParam->dimValues[TIDL_DIM_NUMCH]  * dataBuffParam->dimValues[TIDL_DIM_HEIGHT] * dataBuffParam->dimValues[TIDL_DIM_WIDTH];
      for(m=0; m < dim1; m++)
      {
        for(l = 0;l < dim2; l++)
        {
        for (j = 0; j < numChannels; j++)
        {
          for (i = 0; i < height; i++)
          {
            offset = m*dataBuffParam->dimValues[TIDL_DIM_DIM2]*dataBuffParam->dimValues[TIDL_DIM_NUMCH]*dataBuffParam->dimValues[TIDL_DIM_HEIGHT] * dataBuffParam->dimValues[TIDL_DIM_WIDTH]
            + l*dataBuffParam->dimValues[TIDL_DIM_NUMCH]*dataBuffParam->dimValues[TIDL_DIM_HEIGHT] * dataBuffParam->dimValues[TIDL_DIM_WIDTH]+
            ((j + startChannel)*dataBuffParam->dimValues[TIDL_DIM_HEIGHT] * dataBuffParam->dimValues[TIDL_DIM_WIDTH]) +
                     ((i + startHeight)*dataBuffParam->dimValues[TIDL_DIM_WIDTH]) + batchOffset;
            if (dataBuffParam->elementType == 0)
            {
              uint8_t * src, *dst;
              int32_t k;
              src = (uint8_t *)(ptr + batchIdx * batchPitch + m*dim1Pitch + l*dim2Pitch + (j*currChPitch) + (i*linePitch) + (dataBuffParam->padH*linePitch) + dataBuffParam->padW);
              dst = (uint8_t *)(outPtr + offset);
              for (k = 0; k < dataBuffParam->dimValues[TIDL_DIM_WIDTH]; k++)
              {
                dst[k] = src[k];
                if (tidl_getWriteLevel() == 3)
                {
                  floatPtr[offset + k] = ((float32_tidl)src[k] - dataBuffParam->tensorZeroPoint) / (dataBuffParam->tensorScale);
                }
              }
            }
            else if (dataBuffParam->elementType == 1)
            {
              int8_t * src;
              int8_t * dst;
              int32_t k;
              src = (ptr + batchIdx * batchPitch + m*dim1Pitch + l*dim2Pitch + (j*currChPitch) + (i*linePitch) + (dataBuffParam->padH*linePitch) + dataBuffParam->padW);
              dst = (outPtr + offset);
              for (k = 0; k < dataBuffParam->dimValues[TIDL_DIM_WIDTH]; k++)
              {
                //dst[k] = src[k] + 128;
                dst[k] = src[k];
                if (tidl_getWriteLevel() == 3)
                {
                  floatPtr[offset + k] = ((float32_tidl)src[k] - dataBuffParam->tensorZeroPoint) / (dataBuffParam->tensorScale);
                }
              }
            }
            else if (dataBuffParam->elementType == 2)
            {
              uint16_t * src;
              uint16_t * dst;
              int32_t k;
              src = (((uint16_t *)ptr) + batchIdx * batchPitch + m*dim1Pitch + l*dim2Pitch + (j*currChPitch) + (i*linePitch) + (dataBuffParam->padH*linePitch) + dataBuffParam->padW);
              dst = (((uint16_t *)outPtr) + offset);
              for (k = 0; k < dataBuffParam->dimValues[TIDL_DIM_WIDTH]; k++)
              {
                dst[k] = src[k];
                if (tidl_getWriteLevel() == 3)
                {
                  floatPtr[offset + k] = ((float32_tidl)src[k] - dataBuffParam->tensorZeroPoint) / (dataBuffParam->tensorScale);
                }
              }
            }
            else if (dataBuffParam->elementType == 3)
            {
              int16_t * src;
              int16_t * dst;
              int32_t k;
              src = (((int16_t *)ptr) + batchIdx * batchPitch + m*dim1Pitch + l*dim2Pitch + (j*currChPitch) + (i*linePitch) + (dataBuffParam->padH*linePitch) + dataBuffParam->padW);
              dst = (((int16_t *)outPtr) + offset);
              for (k = 0; k < dataBuffParam->dimValues[TIDL_DIM_WIDTH]; k++)
              {
                //dst[k] = src[k] + 128;
                dst[k] = src[k];
                if (tidl_getWriteLevel() == 3)
                {
                  floatPtr[offset + k] = ((float32_tidl)src[k] - dataBuffParam->tensorZeroPoint) / (dataBuffParam->tensorScale);
                }
              }
            }
            else
            {
              (void)memcpy((outPtr + ((batchIdx * dataBuffParam->dimValues[TIDL_DIM_DIM1]* dataBuffParam->dimValues[TIDL_DIM_DIM2]* dataBuffParam->dimValues[TIDL_DIM_NUMCH]  * dataBuffParam->dimValues[TIDL_DIM_HEIGHT] * dataBuffParam->dimValues[TIDL_DIM_WIDTH])
              + (m*dataBuffParam->dimValues[TIDL_DIM_DIM2]*dataBuffParam->dimValues[TIDL_DIM_NUMCH]*dataBuffParam->dimValues[TIDL_DIM_HEIGHT] * dataBuffParam->dimValues[TIDL_DIM_WIDTH])
              + (l*dataBuffParam->dimValues[TIDL_DIM_NUMCH]*dataBuffParam->dimValues[TIDL_DIM_HEIGHT] * dataBuffParam->dimValues[TIDL_DIM_WIDTH])
              + (j*dataBuffParam->dimValues[TIDL_DIM_HEIGHT] * dataBuffParam->dimValues[TIDL_DIM_WIDTH]) +
                (i*dataBuffParam->dimValues[TIDL_DIM_WIDTH]))*elementSizeBytes),
                (ptr + (((batchIdx * batchPitch) + m*dim1Pitch + l*dim2Pitch + (j*currChPitch) + (i*linePitch) + (dataBuffParam->padH*linePitch) +
                  dataBuffParam->padW)*elementSizeBytes)),
                (size_t)(dataBuffParam->dimValues[TIDL_DIM_WIDTH] * elementSizeBytes));
            }

          }
          if (tidl_getWriteLevel() == 2)
          {
            if((height != dataBuffParam->dimValues[TIDL_DIM_HEIGHT]) && (startHeight != 0))
            {
              tidl_printf(0, "tidl_getWriteLevel() = 2 with ST is not supported now \n");
              status = IALG_EFAIL;
            }
            else
            {
              (void)memcpy((outWithPadPtr + batchIdx *dataBuffParam->dimValues[TIDL_DIM_NUMCH] * paddeOutSize + j*paddeOutSize), (ptr + (batchIdx * batchPitch) + (j*chPitch)), paddeOutSize);
            }
          }
        }
        }
      }
      }
        (void)strcpy((char *)traceDumpName, (char *)traceBaseName);
        (void)strcat((char *)traceDumpName, ".y");
        if(elementSizeBytes == 4)
        {
          (void)strcpy((char *)traceDumpName, (char *)traceBaseName);
          (void)strcat((char *)traceDumpName, "_float.bin");
        }
        (void)tidl_getWriteBinToFile()((const char *)traceDumpName, outPtr, dataSize*elementSizeBytes, 1);
        if (tidl_getWriteLevel() == 2)
        {
          (void)strcpy((char *)traceDumpName, (char *)traceBaseName);
          (void)strcat((char *)traceDumpName, "_paded.y");
          (void)tidl_getWriteBinToFile()((const char *)traceDumpName, outWithPadPtr, padedDataSize*elementSizeBytes, 1);
        }

        if (tidl_getWriteLevel() == 3)
        {
          (void)strcpy((char *)traceDumpName, (char *)traceBaseName);
          (void)strcat((char *)traceDumpName, "_float.bin");
          (void)tidl_getWriteBinToFile()((const char *)traceDumpName, floatPtr, dataSize*((int32_t)sizeof(float32_tidl)), 1);
        }
      }
    }
    *dataBuffParam = dataBuffParamOrig;

    }
    tidl_setWriteLevel(tidlWriteLevelOrig) ;

    /** Wb/inv L1D/L2 cache after writing traces*/
    TIDL_enableL1DandL2CacheWb();

  }
  return status;
}


#if 0
/**
 * @brief Function to write traces from data buffer
 *
 * @param ptr : Pointer to the data buffer
 * @param net : tidl network structure
 * @param dataBuffId : id of the databuffer
 * @param orgScratchPtr : pointer to the scratch buffer
 * @param orgScratchSize : Size of the scratch buffer
 * @param currentLineWriteOffset : Offset to the current line
 * @param currentNumLines : No of lines for writting
 * @param currChPitch : current channel pitch
 */
int32_t tidl_writeTraceDataBuf(int8_t * ptr, sTIDL_Network_t * net, int32_t dataBuffId, int8_t * orgScratchPtr, uint32_t orgScratchSize, int32_t currentLineWriteOffset, int32_t currentNumLines, int32_t currChPitch)
{
  int32_t status = TIDL_SUCCESS;
  if (tidl_getWriteLevel() > 0)
  {
    int8_t * scratchPtr =  (int8_t *)(orgScratchPtr  + TRACE_STRINGS_MEM_SIZE);
    int8_t * traceDumpName = (int8_t *)(orgScratchPtr);
    int8_t * traceBaseName = (int8_t *)(orgScratchPtr  + (1*TRACE_STRING_SIZE));
    int8_t * DataIDString  = (int8_t *)(orgScratchPtr  + (2*TRACE_STRING_SIZE));
    int32_t layerIdx;
    int32_t scratchSize =  orgScratchSize - TRACE_STRINGS_MEM_SIZE;
    int32_t tidlWriteLevelOrig = tidl_getWriteLevel();
    sTIDL_DataParams_t * dataBuffParam;
    dataBuffParam = TIDL_getDataParams(net, dataBuffId);
    layerIdx = TIDL_getLayerNum(net, dataBuffId);


    /* for trace Level 4 enable trace level 3 by default */
    if ( tidl_getWriteLevel() == 4 )
    {
      tidl_setWriteLevel(3) ;
    }
    if (dataBuffParam != NULL)
    {
     sTIDL_DataParams_t dataBuffParamOrig = *dataBuffParam;

      if ( net->TIDLLayers[layerIdx].layerType == TIDL_DataConvertLayer )
      {
        int32_t inLayout = net->TIDLLayers[layerIdx].layerParams.dataConvertParams.layout, outLayout = net->TIDLLayers[layerIdx].layerParams.dataConvertParams.outLayout;
        if ( (net->TIDLLayers[layerIdx].layerParams.dataConvertParams.layout == TIDL_LT_NHWC && net->TIDLLayers[layerIdx].layerParams.dataConvertParams.type == TIDL_DC_TYPE_OUTPUT) ||
             (net->TIDLLayers[layerIdx].layerParams.dataConvertParams.type == TIDL_DC_TYPE_INTERMEDIATE) && (inLayout != outLayout))
        {
          dataBuffParam->dimValues[TIDL_DIM_WIDTH]  = dataBuffParamOrig.dimValues[TIDL_DIM_NUMCH];
          dataBuffParam->dimValues[TIDL_DIM_HEIGHT] = dataBuffParamOrig.dimValues[TIDL_DIM_WIDTH];
          dataBuffParam->dimValues[TIDL_DIM_NUMCH]  = dataBuffParamOrig.dimValues[TIDL_DIM_HEIGHT];
          dataBuffParam->dimValues[TIDL_DIM_BATCH]  = dataBuffParamOrig.dimValues[TIDL_DIM_BATCH];

          dataBuffParam->pitch[TIDL_LINE_PITCH]    = dataBuffParam->dimValues[TIDL_DIM_WIDTH];
          dataBuffParam->pitch[TIDL_CHANNEL_PITCH] = dataBuffParam->pitch[TIDL_LINE_PITCH] * dataBuffParam->dimValues[TIDL_DIM_HEIGHT];
          dataBuffParam->pitch[TIDL_ROI_PITCH]     = dataBuffParam->pitch[TIDL_CHANNEL_PITCH] * dataBuffParam->dimValues[TIDL_DIM_NUMCH];
          //:TODO: This needs to be correctly handled for super tiling cases
          currentNumLines = dataBuffParam->dimValues[TIDL_DIM_HEIGHT];
          currChPitch     = dataBuffParam->pitch[TIDL_CHANNEL_PITCH];
        }
      }
    if ((dataBuffParam->numBatchW>1 || dataBuffParam->numBatchH>1)&& (dataBuffParam->dimValues[TIDL_DIM_BATCH] == 1))
    {
      int32_t elementSizeBytes;
      elementSizeBytes = TIDL_getDatElementSize(dataBuffParam->elementType);
      if(elementSizeBytes == 4)
      {
        tidl_setWriteLevel(tidl_getWriteLevel() == 3 ? 1 : tidl_getWriteLevel());
      }

      int32_t outWidth = (dataBuffParam->dimValues[TIDL_DIM_WIDTH] - dataBuffParam->batchPadW * (dataBuffParam->numBatchW -1))/dataBuffParam->numBatchW;
      int32_t outHeight = dataBuffParam->dimValues[TIDL_DIM_HEIGHT];
      int32_t outChannels = dataBuffParam->dimValues[TIDL_DIM_NUMCH];
      int32_t outNumBatches = dataBuffParam->numBatchW;
      int32_t dataSize = outWidth * outHeight * outChannels * outNumBatches;
      int32_t linePitch = dataBuffParam->pitch[TIDL_LINE_PITCH];
      int32_t chPitch = dataBuffParam->pitch[TIDL_CHANNEL_PITCH];
      int32_t batchPitch  = dataBuffParam->pitch[TIDL_ROI_PITCH];
      int32_t inBlkPitchW        =  outWidth + dataBuffParam->batchPadW;
      int32_t paddeOutSize = linePitch * (dataBuffParam->dimValues[TIDL_DIM_HEIGHT] + (dataBuffParam->padH * 2) + 1);
      int32_t padedDataSize =  dataBuffParam->dimValues[TIDL_DIM_BATCH] * dataBuffParam->dimValues[TIDL_DIM_NUMCH] * paddeOutSize;
      int8_t * outPtr        = (int8_t *)scratchPtr;
      int8_t * outWithPadPtr = (int8_t *)(scratchPtr + (dataSize*elementSizeBytes));
      float32_tidl  * floatPtr = (float32_tidl  *)(scratchPtr + (dataSize*elementSizeBytes));
      int32_t i;
      int32_t j, k1;
      int32_t batchIdx;
      int32_t batchOffset;
      int32_t offset;
      int32_t totalSize = 0;

      if (tidl_getWriteLevel() == 1)
      {
        totalSize = (dataSize*elementSizeBytes);
      }
      else if (tidl_getWriteLevel() == 2)
      {
        totalSize = ((dataSize*elementSizeBytes) + (padedDataSize*elementSizeBytes));
      }
      else if (tidl_getWriteLevel() == 3)
      {
        totalSize = (dataSize*elementSizeBytes) + ((int32_t)sizeof(float32_tidl)*dataSize);
      }
      else
      {
        tidl_printf(0, "Un supported tidl_getWriteLevel() \n");
        status = TIDL_ERR_FAILURE;
      }

      if(dataBuffParam->elementType == TIDL_SinglePrecFloat)
      {
        float32_tidl curMin = FLT_MAX, curMax = -FLT_MAX;
        TIDL_TensorMinMax((float32_tidl*)ptr, dataBuffParam, &curMin, &curMax);
        tidl_printf(0, " %3d %10.5f %10.5f %10.5f %d\n", dataBuffId, dataBuffParam->tensorScale, curMin, curMax, dataBuffParam->elementType);
      }
      else
      {
        tidl_printf(0, " %3d %10.5f %10.5f %10.5f %d\n", dataBuffId, dataBuffParam->tensorScale, dataBuffParam->minTensorValue, dataBuffParam->maxTensorValue, dataBuffParam->elementType);
      }


      if ((totalSize > scratchSize) && (status == TIDL_SUCCESS))
      {
        tidl_printf(0, " In sufficient Memory to write Traces, as requested size is : %d, allocated size : %d \n", totalSize, scratchSize);
        status = TIDL_ERR_FAILURE;
      }

      if(status == TIDL_SUCCESS)
      {
        if(tidl_getTraceBaseName() != NULL)
        {
          (void)strcpy((char *)traceDumpName, (char *)tidl_getTraceBaseName());
        }
        else
        {
          (void)strcpy((char *)traceDumpName, (char *)"");
        }

        (void)sprintf((char *)DataIDString, "%04d", dataBuffId);
        (void)strcat((char *)traceDumpName, (char *)DataIDString);
        (void)strcat((char *)traceDumpName, "_");
        (void)sprintf((char *)DataIDString, "%05d", outNumBatches);
        (void)strcat((char *)traceDumpName, (char *)DataIDString);
        (void)strcat((char *)traceDumpName, "_");
        (void)sprintf((char *)DataIDString, "%05d", outChannels);
        (void)strcat((char *)traceDumpName, (char *)DataIDString);
        (void)strcat((char *)traceDumpName, "_");
        (void)sprintf((char *)DataIDString, "%05d", outWidth);
        (void)strcat((char *)traceDumpName, (char *)DataIDString);
        (void)strcat((char *)traceDumpName, "x");
        (void)sprintf((char *)DataIDString, "%05d", outHeight);
        (void)strcat((char *)traceDumpName, (char *)DataIDString);
        (void)strcpy((char *)traceBaseName, (char *)traceDumpName);

        if(currentLineWriteOffset != 0)
        {
          (void)strcpy((char *)traceDumpName, (char *)traceBaseName);
          (void)strcat((char *)traceDumpName, ".y");

          if(elementSizeBytes == 4)
          {
            (void)strcpy((char *)traceDumpName, (char *)traceBaseName);
            (void)strcat((char *)traceDumpName, "_float.bin");
          }

          (void)tidl_getReadBinFromFile()((const char *)traceDumpName, outPtr, dataSize*elementSizeBytes, 1);
          if (tidl_getWriteLevel() == 2)
          {
            (void)strcpy((char *)traceDumpName, (char *)traceBaseName);
            (void)strcat((char *)traceDumpName, "_paded.y");
            (void)tidl_getReadBinFromFile()((const char *)traceDumpName, outWithPadPtr, padedDataSize*elementSizeBytes, 1);
          }

          if (tidl_getWriteLevel() == 3)
          {
            (void)strcpy((char *)traceDumpName, (char *)traceBaseName);
            (void)strcat((char *)traceDumpName, "_float.bin");
            (void)tidl_getReadBinFromFile()((const char *)traceDumpName, floatPtr, dataSize*((int32_t)sizeof(float32_tidl)), 1);
          }
        }

      for (batchIdx = 0; batchIdx < 1; batchIdx++)
      {
        batchOffset =  outChannels  * outWidth* outHeight;

        for (j = 0; j < outChannels; j++)
        {
          for (i = 0; i < currentNumLines; i++)
          {
            for (k1 = 0; k1 < outNumBatches; k1++)
            {
              offset = (j*outHeight * outWidth) +
                       ((i + currentLineWriteOffset)*outWidth) + k1*batchOffset;
            if (dataBuffParam->elementType == 0)
            {
              uint8_t * src, *dst;
              int32_t k;
              src = (uint8_t *)(ptr + (k1 * inBlkPitchW) + (j*currChPitch) + (i*linePitch) + (dataBuffParam->padH*linePitch) + dataBuffParam->padW);
              dst = (uint8_t *)(outPtr + offset);
              for (k = 0; k < outWidth; k++)
              {
                dst[k] = src[k];
                if (tidl_getWriteLevel() == 3)
                {
                  floatPtr[offset + k] = ((float32_tidl)src[k]) / (dataBuffParam->tensorScale);
                }
              }
            }
            else if (dataBuffParam->elementType == 1)
            {
              int8_t * src;
              int8_t * dst;
              int32_t k;
              src = (ptr + (k1 * inBlkPitchW) + (j*currChPitch) + (i*linePitch) + (dataBuffParam->padH*linePitch) + dataBuffParam->padW);
              dst = (outPtr + offset);
              for (k = 0; k < outWidth; k++)
              {
                //dst[k] = src[k] + 128;
                dst[k] = src[k];
                if (tidl_getWriteLevel() == 3)
                {
                  floatPtr[offset + k] = ((float32_tidl)src[k]) / (dataBuffParam->tensorScale);
                }
              }
            }
            else if (dataBuffParam->elementType == 2)
            {
              uint16_t * src, *dst;
              int32_t k;
              src = ((uint16_t *)ptr + (k1 * inBlkPitchW) + (j*currChPitch) + (i*linePitch) + (dataBuffParam->padH*linePitch) + dataBuffParam->padW);
              dst = ((uint16_t *)outPtr + offset);
              for (k = 0; k < outWidth; k++)
              {
                dst[k] = src[k];
                if (tidl_getWriteLevel() == 3)
                {
                  floatPtr[offset + k] = ((float32_tidl)src[k]) / (dataBuffParam->tensorScale);
                }
              }
            }
            else if (dataBuffParam->elementType == 3)
            {
              int16_t * src, *dst;
              int32_t k;
              src = ((int16_t *)ptr + (k1 * inBlkPitchW) + (j*currChPitch) + (i*linePitch) + (dataBuffParam->padH*linePitch) + dataBuffParam->padW);
              dst = ((int16_t *)outPtr + offset);
              for (k = 0; k < outWidth; k++)
              {
                dst[k] = src[k];
                if (tidl_getWriteLevel() == 3)
                {
                  floatPtr[offset + k] = ((float32_tidl)src[k]) / (dataBuffParam->tensorScale);
                }
              }
            }
            else
            {
              (void)memcpy((outPtr + (offset*elementSizeBytes)),
                (ptr + (((k1 * inBlkPitchW) + (j*currChPitch) + (i*linePitch) + (dataBuffParam->padH*linePitch) +
                  dataBuffParam->padW)*elementSizeBytes)),
                (size_t)(outWidth * elementSizeBytes));
            }
          }
          if (tidl_getWriteLevel() == 2)
          {
            if((currentNumLines != dataBuffParam->dimValues[TIDL_DIM_HEIGHT]) && (currentLineWriteOffset != 0))
            {
              tidl_printf(0, "tidl_getWriteLevel() = 2 with ST is not supported now \n");
              status = IALG_EFAIL;
            }
            else
            {
              (void)memcpy((outWithPadPtr + batchIdx *dataBuffParam->dimValues[TIDL_DIM_NUMCH] * paddeOutSize + j*paddeOutSize), (ptr + (batchIdx * batchPitch) + (j*chPitch)), paddeOutSize);
            }
          }
        }
      }
        (void)strcpy((char *)traceDumpName, (char *)traceBaseName);
        (void)strcat((char *)traceDumpName, ".y");
        if(elementSizeBytes == 4)
        {
          (void)strcpy((char *)traceDumpName, (char *)traceBaseName);
          (void)strcat((char *)traceDumpName, "_float.bin");
        }
        (void)tidl_getWriteBinToFile()((const char *)traceDumpName, outPtr, dataSize*elementSizeBytes, tidl_getTracePtr());
        if (tidl_getWriteLevel() == 2)
        {
          (void)strcpy((char *)traceDumpName, (char *)traceBaseName);
          (void)strcat((char *)traceDumpName, "_paded.y");
          (void)tidl_getWriteBinToFile()((const char *)traceDumpName, outWithPadPtr, padedDataSize*elementSizeBytes, tidl_getTracePtr());
        }

        if (tidl_getWriteLevel() == 3)
        {
          (void)strcpy((char *)traceDumpName, (char *)traceBaseName);
          (void)strcat((char *)traceDumpName, "_float.bin");
          (void)tidl_getWriteBinToFile()((const char *)traceDumpName, floatPtr, dataSize*((int32_t)sizeof(float32_tidl)), tidl_getTracePtr());
        }
      }
    }
    }
    else
    {
      int32_t elementSizeBytes;
      elementSizeBytes = TIDL_getDatElementSize(dataBuffParam->elementType);
      if(elementSizeBytes == 4)
      {
        tidl_setWriteLevel(tidl_getWriteLevel() == 3 ? 1 : tidl_getWriteLevel());
      }

      int32_t dataSize = dataBuffParam->dimValues[TIDL_DIM_BATCH] * dataBuffParam->dimValues[TIDL_DIM_NUMCH] * dataBuffParam->dimValues[TIDL_DIM_HEIGHT] * dataBuffParam->dimValues[TIDL_DIM_WIDTH];
      int32_t linePitch = dataBuffParam->pitch[TIDL_LINE_PITCH];
      int32_t chPitch = dataBuffParam->pitch[TIDL_CHANNEL_PITCH];
      int32_t batchPitch  = dataBuffParam->pitch[TIDL_ROI_PITCH];
      int32_t paddeOutSize = linePitch * (dataBuffParam->dimValues[TIDL_DIM_HEIGHT] + (dataBuffParam->padH * 2) + 1);
      int32_t padedDataSize =  dataBuffParam->dimValues[TIDL_DIM_BATCH] * dataBuffParam->dimValues[TIDL_DIM_NUMCH] * paddeOutSize;
      int8_t * outPtr        = (int8_t *)scratchPtr;
      int8_t * outWithPadPtr = (int8_t *)(scratchPtr + (dataSize*elementSizeBytes));
      float32_tidl  * floatPtr = (float32_tidl  *)(scratchPtr + (dataSize*elementSizeBytes));
      int32_t i;
      int32_t j;
      int32_t batchIdx;
      int32_t batchOffset;
      int32_t offset;
      int32_t totalSize = 0;

      if (tidl_getWriteLevel() == 1)
      {
        totalSize = (dataSize*elementSizeBytes);
      }
      else if (tidl_getWriteLevel() == 2)
      {
        totalSize = ((dataSize*elementSizeBytes) + (padedDataSize*elementSizeBytes));
      }
      else if (tidl_getWriteLevel() == 3)
      {
        totalSize = (dataSize*elementSizeBytes) + ((int32_t)sizeof(float32_tidl)*dataSize);
      }
      else
      {
        tidl_printf(0, "Un supported tidl_getWriteLevel() \n");
        status = TIDL_ERR_FAILURE;
      }

      if(dataBuffParam->elementType == TIDL_SinglePrecFloat)
      {
        float32_tidl curMin = FLT_MAX, curMax = -FLT_MAX;
        TIDL_TensorMinMax((float32_tidl*)ptr, dataBuffParam, &curMin, &curMax);
        tidl_printf(0, " %3d %15.5f %15.5f %15.5f %d\n", dataBuffId, dataBuffParam->tensorScale, curMin, curMax, dataBuffParam->elementType);
      }
      else
      {
        tidl_printf(0, " %3d %15.5f %15.5f %15.5f %d\n", dataBuffId, dataBuffParam->tensorScale, dataBuffParam->minTensorValue, dataBuffParam->maxTensorValue, dataBuffParam->elementType);
      }


      if ((totalSize > scratchSize) && (status == TIDL_SUCCESS))
      {
        tidl_printf(0, " In sufficient Memory to write Traces, requested size is : %d, allocated size : %d \n", totalSize, scratchSize);
        status = TIDL_ERR_FAILURE;
      }

      if(status == TIDL_SUCCESS)
      {
        if(tidl_getTraceBaseName() != NULL)
        {
          (void)strcpy((char *)traceDumpName, (char *)tidl_getTraceBaseName());
        }
        else
        {
          (void)strcpy((char *)traceDumpName, (char *)"");
        }

        (void)sprintf((char *)DataIDString, "%04d", dataBuffId);
        (void)strcat((char *)traceDumpName, (char *)DataIDString);
        (void)strcat((char *)traceDumpName, "_");
        (void)sprintf((char *)DataIDString, "%05d", dataBuffParam->dimValues[TIDL_DIM_BATCH]);
        (void)strcat((char *)traceDumpName, (char *)DataIDString);
        (void)strcat((char *)traceDumpName, "_");
        (void)sprintf((char *)DataIDString, "%05d", dataBuffParam->dimValues[TIDL_DIM_NUMCH]);
        (void)strcat((char *)traceDumpName, (char *)DataIDString);
        (void)strcat((char *)traceDumpName, "_");
        (void)sprintf((char *)DataIDString, "%05d", dataBuffParam->dimValues[TIDL_DIM_WIDTH]);
        (void)strcat((char *)traceDumpName, (char *)DataIDString);
        (void)strcat((char *)traceDumpName, "x");
        (void)sprintf((char *)DataIDString, "%05d", dataBuffParam->dimValues[TIDL_DIM_HEIGHT]);
        (void)strcat((char *)traceDumpName, (char *)DataIDString);
        (void)strcpy((char *)traceBaseName, (char *)traceDumpName);

        if(currentLineWriteOffset != 0)
        {
          (void)strcpy((char *)traceDumpName, (char *)traceBaseName);
          (void)strcat((char *)traceDumpName, ".y");

          if(elementSizeBytes == 4)
          {
            (void)strcpy((char *)traceDumpName, (char *)traceBaseName);
            (void)strcat((char *)traceDumpName, "_float.bin");
          }

          (void)tidl_getReadBinFromFile()((const char *)traceDumpName, outPtr, dataSize*elementSizeBytes, 1);
          if (tidl_getWriteLevel() == 2)
          {
            (void)strcpy((char *)traceDumpName, (char *)traceBaseName);
            (void)strcat((char *)traceDumpName, "_paded.y");
            (void)tidl_getReadBinFromFile()((const char *)traceDumpName, outWithPadPtr, padedDataSize*elementSizeBytes, 1);
          }

          if (tidl_getWriteLevel() == 3)
          {
            (void)strcpy((char *)traceDumpName, (char *)traceBaseName);
            (void)strcat((char *)traceDumpName, "_float.bin");
            (void)tidl_getReadBinFromFile()((const char *)traceDumpName, floatPtr, dataSize*((int32_t)sizeof(float32_tidl)),1 );
          }
        }

      for (batchIdx = 0; batchIdx < dataBuffParam->dimValues[TIDL_DIM_BATCH]; batchIdx++)
      {
        batchOffset = batchIdx * dataBuffParam->dimValues[TIDL_DIM_NUMCH]  * dataBuffParam->dimValues[TIDL_DIM_HEIGHT] * dataBuffParam->dimValues[TIDL_DIM_WIDTH];

        for (j = 0; j < dataBuffParam->dimValues[TIDL_DIM_NUMCH]; j++)
        {
          for (i = 0; i < currentNumLines; i++)
          {
            offset = (j*dataBuffParam->dimValues[TIDL_DIM_HEIGHT] * dataBuffParam->dimValues[TIDL_DIM_WIDTH]) +
                     ((i + currentLineWriteOffset)*dataBuffParam->dimValues[TIDL_DIM_WIDTH]) + batchOffset;

            if (dataBuffParam->elementType == 0)
            {
              uint8_t * src, *dst;
              int32_t k;
              src = (uint8_t *)(ptr + batchIdx * batchPitch + (j*currChPitch) + (i*linePitch) + (dataBuffParam->padH*linePitch) + dataBuffParam->padW);
              dst = (uint8_t *)(outPtr + offset);
              for (k = 0; k < dataBuffParam->dimValues[TIDL_DIM_WIDTH]; k++)
              {
                dst[k] = src[k];
                if (tidl_getWriteLevel() == 3)
                {
                  floatPtr[offset + k] = ((float32_tidl)src[k]) / (dataBuffParam->tensorScale);
                }
              }
            }
            else if (dataBuffParam->elementType == 1)
            {
              int8_t * src;
              int8_t * dst;
              int32_t k;
              src = (ptr + batchIdx * batchPitch + (j*currChPitch) + (i*linePitch) + (dataBuffParam->padH*linePitch) + dataBuffParam->padW);
              dst = (outPtr + offset);
              for (k = 0; k < dataBuffParam->dimValues[TIDL_DIM_WIDTH]; k++)
              {
                //dst[k] = src[k] + 128;
                dst[k] = src[k];
                if (tidl_getWriteLevel() == 3)
                {
                  floatPtr[offset + k] = ((float32_tidl)src[k]) / (dataBuffParam->tensorScale);
                }
              }
            }
            else if (dataBuffParam->elementType == 2)
            {
              uint16_t * src;
              uint16_t * dst;
              int32_t k;
              src = (((uint16_t *)ptr) + batchIdx * batchPitch + (j*currChPitch) + (i*linePitch) + (dataBuffParam->padH*linePitch) + dataBuffParam->padW);
              dst = (((uint16_t *)outPtr) + offset);
              for (k = 0; k < dataBuffParam->dimValues[TIDL_DIM_WIDTH]; k++)
              {
                dst[k] = src[k];
                if (tidl_getWriteLevel() == 3)
                {
                  floatPtr[offset + k] = ((float32_tidl)src[k]) / (dataBuffParam->tensorScale);
                }
              }
            }
            else if (dataBuffParam->elementType == 3)
            {
              int16_t * src;
              int16_t * dst;
              int32_t k;
              src = (((int16_t *)ptr) + batchIdx * batchPitch + (j*currChPitch) + (i*linePitch) + (dataBuffParam->padH*linePitch) + dataBuffParam->padW);
              dst = (((int16_t *)outPtr) + offset);
              for (k = 0; k < dataBuffParam->dimValues[TIDL_DIM_WIDTH]; k++)
              {
                //dst[k] = src[k] + 128;
                dst[k] = src[k];
                if (tidl_getWriteLevel() == 3)
                {
                  floatPtr[offset + k] = ((float32_tidl)src[k]) / (dataBuffParam->tensorScale);
                }
              }
            }
            else
            {
              (void)memcpy((outPtr + ((batchIdx * dataBuffParam->dimValues[TIDL_DIM_NUMCH]  * dataBuffParam->dimValues[TIDL_DIM_HEIGHT] * dataBuffParam->dimValues[TIDL_DIM_WIDTH])
              + (j*dataBuffParam->dimValues[TIDL_DIM_HEIGHT] * dataBuffParam->dimValues[TIDL_DIM_WIDTH]) +
                (i*dataBuffParam->dimValues[TIDL_DIM_WIDTH]))*elementSizeBytes),
                (ptr + (((batchIdx * batchPitch) + (j*currChPitch) + (i*linePitch) + (dataBuffParam->padH*linePitch) +
                  dataBuffParam->padW)*elementSizeBytes)),
                (size_t)(dataBuffParam->dimValues[TIDL_DIM_WIDTH] * elementSizeBytes));
            }

          }
          if (tidl_getWriteLevel() == 2)
          {
            if((currentNumLines != dataBuffParam->dimValues[TIDL_DIM_HEIGHT]) && (currentLineWriteOffset != 0))
            {
              tidl_printf(0, "tidl_getWriteLevel() = 2 with ST is not supported now \n");
              status = IALG_EFAIL;
            }
            else
            {
              (void)memcpy((outWithPadPtr + batchIdx *dataBuffParam->dimValues[TIDL_DIM_NUMCH] * paddeOutSize + j*paddeOutSize), (ptr + (batchIdx * batchPitch) + (j*chPitch)), paddeOutSize);
            }
          }
        }
      }
        (void)strcpy((char *)traceDumpName, (char *)traceBaseName);
        (void)strcat((char *)traceDumpName, ".y");
        if(elementSizeBytes == 4)
        {
          (void)strcpy((char *)traceDumpName, (char *)traceBaseName);
          (void)strcat((char *)traceDumpName, "_float.bin");
        }
        (void)tidl_getWriteBinToFile()((const char *)traceDumpName, outPtr, dataSize*elementSizeBytes, tidl_getTracePtr());
        if (tidl_getWriteLevel() == 2)
        {
          (void)strcpy((char *)traceDumpName, (char *)traceBaseName);
          (void)strcat((char *)traceDumpName, "_paded.y");
          (void)tidl_getWriteBinToFile()((const char *)traceDumpName, outWithPadPtr, padedDataSize*elementSizeBytes, tidl_getTracePtr());
        }

        if (tidl_getWriteLevel() == 3)
        {
          (void)strcpy((char *)traceDumpName, (char *)traceBaseName);
          (void)strcat((char *)traceDumpName, "_float.bin");
          (void)tidl_getWriteBinToFile()((const char *)traceDumpName, floatPtr, dataSize*((int32_t)sizeof(float32_tidl)), tidl_getTracePtr());
        }
      }
    }
    *dataBuffParam = dataBuffParamOrig;

    }
    tidl_setWriteLevel(tidlWriteLevelOrig) ;

    /** Wb/inv L1D/L2 cache after writing traces*/
    TIDL_enableL1DandL2CacheWb();
  }
  return status;
}
#endif

/**
 * @brief Function to compare to reference and target implementations
 *
 * @param targetPtr : output from target implementation
 * @param refPtr : output from reference implementation
 * @param outWidth : Width of the ouptut buffer
 * @param outHeight : Height of the ouptut buffer
 * @param numOutChannels : no of output channels
 * @param numBatches : no of Batches
 * @param outPitch : Pitch of the output buffer
 * @param outChPitch : Channel pitch of the output buffer
 * @param outBatchPitch : Batch pitch of the output buffer
 * @param layerIdx : current layer Index
 * @return  IALG_EOK   - for no Mismatch
 *          IALG_EFAIL - for mismatch
 */
template <class Tfeat>
int32_t TIDL_RefComparison(
  Tfeat * targetPtr,
  Tfeat * refPtr,
  int32_t outWidth,
  int32_t outHeight,
  int32_t numOutChannels,
  int32_t numBatches,
  int32_t outPitch,
  int32_t outChPitch,
  int32_t outBatchPitch,
  int32_t layerIdx
)
{
  int32_t i;
  int32_t j;
  int32_t k;
  int32_t batchIdx;
  int32_t status = TIDL_SUCCESS;
  int32_t offset;

  for(batchIdx = 0;batchIdx < numBatches; batchIdx++)
  {
    for(i = 0; i < numOutChannels; i++)
    {
      for(j = 0; j < (outHeight); j++)
      {
        for(k = 0; k < (outWidth); k++)
        {
          offset = (batchIdx * outBatchPitch) +  (i*outChPitch) +  (j * outPitch) + k;
          if(targetPtr[offset] != refPtr[offset])
          {
            tidl_printf(0,"Layer %d ,Ref Comparison :  FAILED!!!!!! at ROI %4d Channel %4d, height %5d, width %5d, target %5d, ref %5d \n", layerIdx, batchIdx, i, j, k,
              targetPtr[(i*outChPitch) +  (j * outPitch) + k],
              refPtr[(i*outChPitch) + (j*outPitch) + k]);
            status = TIDL_ERR_FAILURE;
            break;
          }
        }
        if(TIDL_SUCCESS != status)
        {
          break;
        }
      }
      if(TIDL_SUCCESS != status)
      {
        break;
      }
    }
  }
  return status;
}

/**
 * @brief Function to generate Fill Seam Predicate Registers
 *
 * @param numTiles : no of tiles in the cuurent tensor
 * @param bufPredicateStore : buffer to store predicate stores
 * @param srcAddr : pointer to source data buffer
 * @param dstAddr : pointer to dst data buffer
 * @return int32_t : total bytes to store
 */
int32_t TIDL_generateFillSeamPredicateRegisters(
        int32_t numTiles,
        void *bufPredicateStore,
        const TIDL_bufParams3D_t * srcAddr,
        const TIDL_bufParams3D_t * dstAddr)
{
    int32_t numBlocks;
    int32_t mmaWidth;
    int32_t totalBytes;
    int32_t i,j, k;
    int32_t numBytes = TIDL_sizeof(srcAddr->data_type);
    int32_t inputWidth            = srcAddr->dim_x;
    int32_t processHeight         = dstAddr->dim_y;
    int32_t processWidth          = srcAddr->stride_y / numBytes;
    int32_t matrixColumn          = processWidth*processHeight;
    int32_t blockCounter          = 0;
    int64_t *predRegister         = (int64_t *)bufPredicateStore;
    uint64_t predictedValue = 0x0;

    numBlocks = numTiles;
    totalBytes = numBlocks*8;

    j = 0;
    k = 1;

    if (numBytes== 1) {
        mmaWidth= MMALIB_MMA_SIZE_8_BIT;
    }
    else if (numBytes== 2) {
        mmaWidth= MMALIB_MMA_SIZE_8_BIT/2;
    }
    else {
        mmaWidth= MMALIB_MMA_SIZE_8_BIT/4;
    }

    while (blockCounter < numBlocks) {
        for (i = 0; i < mmaWidth; i++) {
            if (j < inputWidth) {
                if (mmaWidth == (MMALIB_MMA_SIZE_8_BIT/2)) {
                    predictedValue |= (uint64_t)((uint64_t)1 << i*2);
                    predictedValue |= (uint64_t)((uint64_t)1 << (i*2 + 1));
                }
                else if (mmaWidth == MMALIB_MMA_SIZE_8_BIT)
                    predictedValue |= (uint64_t)((uint64_t)1 << i);
                j++;
                k++;
            }
            else if(j >= inputWidth && k <= processWidth) {
                j++;
                k++;
            }
            else if ((blockCounter == numBlocks - 1) && ((blockCounter*mmaWidth + i) >= matrixColumn)) {
                k = 1;
                j = processWidth;
            }
            else {
                k = 1;
                j = 0;
                if (mmaWidth == (MMALIB_MMA_SIZE_8_BIT/2)) {
                    predictedValue |= (uint64_t)((uint64_t)1 << i*2);
                    predictedValue |= (uint64_t)((uint64_t)1 << (i*2 + 1));
                }
                else if (mmaWidth == MMALIB_MMA_SIZE_8_BIT)
                    predictedValue |= (uint64_t)((uint64_t)1 << i);
                j++;
                k++;
            }
        }
        *predRegister = predictedValue;

        predRegister++;
        blockCounter++;
        predictedValue = 0x0;
    }

    return totalBytes;
}

template int32_t TIDL_RefComparison<uint16_t>(
  uint16_t * targetPtr,
  uint16_t * refPtr,
  int32_t outWidth,
  int32_t outHeight,
  int32_t numOutChannels,
  int32_t numBatches,
  int32_t outPitch,
  int32_t outChPitch,
  int32_t outBatchPitch,
  int32_t layerIdx
);

template int32_t TIDL_RefComparison<uint8_t>(
  uint8_t * targetPtr,
  uint8_t * refPtr,
  int32_t outWidth,
  int32_t outHeight,
  int32_t numOutChannels,
  int32_t numBatches,
  int32_t outPitch,
  int32_t outChPitch,
  int32_t outBatchPitch,
  int32_t layerIdx
);

template int32_t TIDL_RefComparison<int16_t>(
  int16_t * targetPtr,
  int16_t * refPtr,
  int32_t outWidth,
  int32_t outHeight,
  int32_t numOutChannels,
  int32_t numBatches,
  int32_t outPitch,
  int32_t outChPitch,
  int32_t outBatchPitch,
  int32_t layerIdx
);

template int32_t TIDL_RefComparison<int8_t>(
  int8_t * targetPtr,
  int8_t * refPtr,
  int32_t outWidth,
  int32_t outHeight,
  int32_t numOutChannels,
  int32_t numBatches,
  int32_t outPitch,
  int32_t outChPitch,
  int32_t outBatchPitch,
  int32_t layerIdx
);




/**
 * @brief Function to store saturation float values
 *
 * @param tidlLayer : Pointer to the common layer parameters
 * @param min : pointer to store min value
 * @param max : pointer to store max value
 * @return None
 */
void TIDL_getSaturationFloat(
  sTIDL_Layer_t        * tidlLayer,
  float * min,
  float * max
  )
{
  if(tidlLayer->actParams.actType == TIDL_NoAct ||
    tidlLayer->actParams.actType == TIDL_PRelU ||
    tidlLayer->actParams.actType == TIDL_GELU )
  {
    *max =  FLT_MAX;
    *min = -FLT_MAX;
  }
  else if(tidlLayer->actParams.actType == TIDL_RelU)
  {
    *max =  FLT_MAX;
    *min = 0;
  }
  else if(tidlLayer->actParams.actType == TIDL_RelU6)
  {
    *max = 6.0;
    *min = 0;
  }
  else if(tidlLayer->actParams.actType == TIDL_Clip)
  {
    *max = tidlLayer->actParams.clipMax;
    *min = tidlLayer->actParams.clipMin;
  }
  else
  {
#ifndef BUILD_WITH_OPENACC
    tidl_printf(0, "actType Not supported in Line %d of %s \n", __LINE__, __FILE__);
    tidl_printf(0, "layerType = %d, tidlLayer->actParams.actType = %d \n", tidlLayer->layerType, tidlLayer->actParams.actType);
#endif
    /*return IALG_EFAIL;*/
  }
  /*return IALG_EOK;*/
}

/**
 * @brief Function to calculate saturation float values
 *
 * @param tidlLayer : Pointer to the common layer parameters
 * @param outAcc : current accumlator value
 */
OPENACC(routine(TIDL_floatSat))
float32_tidl TIDL_floatSat(
  float32_tidl outAcc,
  sTIDL_Layer_t  * tidlLayer
  )
{
  float32_tidl min, max;
  TIDL_getSaturationFloat(tidlLayer, &min, &max);
  outAcc = (outAcc > max) ? max : outAcc;
  outAcc = (outAcc < min) ? min : outAcc;
  return outAcc;
}

int32_t TIDL_getProcessingElementSizeInBytes(const sTIDL_Layer_t  * tidlLayer)
{
  int32_t procElemSizeInBytes;
  if ( tidlLayer->weightsElementSizeInBits <= 8 )
  {
    procElemSizeInBytes = 1;
  }
  else if ( tidlLayer->weightsElementSizeInBits <= 16 )
  {
    procElemSizeInBytes = 2;
  }
  else
  {
    procElemSizeInBytes = 4;
  }

  return procElemSizeInBytes;
}

int32_t TIDL_conv2dGetKernelDataType(int32_t tidlElemType, int32_t procElemSize)
{
  int32_t mmaDataType;

  if (tidlElemType == (int32_t)TIDL_SignedChar)
  {
    mmaDataType = (uint32_t)TIDL_INT8;
    if (procElemSize == 2 )
    {
      mmaDataType = (uint32_t)TIDL_INT16;
    }
  }
  else if (tidlElemType  == (int32_t)TIDL_UnsignedChar)
  {
   mmaDataType = (uint32_t)TIDL_UINT8;
   if (procElemSize == 2 )
   {
     mmaDataType = (uint32_t)TIDL_UINT16;
   }
  }
  else if (tidlElemType == (int32_t)TIDL_UnsignedShort)
  {
   mmaDataType = (uint32_t)TIDL_UINT16;
  }
  else if (tidlElemType == (int32_t)TIDL_SignedShort)
  {
   mmaDataType = (uint32_t)TIDL_INT16;
  }
  else
  {
    mmaDataType = (uint32_t)TIDL_UINT8;
    if (procElemSize == 2 )
    {
     mmaDataType = (uint32_t)TIDL_UINT16;
    }
  }

  return mmaDataType;

}

int32_t TIDL_GetKernelDataTypeFromNcDataType(int32_t ncDataType)
{
  int32_t kernelDataType;

  if (ncDataType == (int32_t)DATA_8S_BIT)
  {
    kernelDataType = TIDL_INT8;
  }
  else if (ncDataType  == (int32_t)DATA_8_BIT)
  {
   kernelDataType = TIDL_UINT8;

  }
  else if (ncDataType == (int32_t)DATA_16_BIT)
  {
   kernelDataType = (uint32_t)TIDL_UINT16;
  }
  else if (ncDataType == (int32_t)DATA_16S_BIT)
  {
   kernelDataType = (uint32_t)TIDL_INT16;
  }
  else if (ncDataType == (int32_t)DATA_32S_BIT)
  {
    kernelDataType = (uint32_t)TIDL_INT32;
  }
  else if (ncDataType == (int32_t)DATA_32_BIT)
  {
    kernelDataType = (uint32_t)TIDL_UINT32;
  }
  else if (ncDataType == (int32_t) DATA_64S_BIT)
  {
    kernelDataType = (uint32_t)TIDL_INT64;
  }
  else if (ncDataType == (int32_t) DATA_64_BIT)
  {
    kernelDataType = (uint32_t)TIDL_UINT64;
  }
  else if (ncDataType == (int32_t) FLOAT_32S_BIT)
  {
    kernelDataType = (uint32_t)TIDL_FLOAT32;
  }

  return kernelDataType;

}



int32_t TIDL_getMmalibDatatypeFromGcDatatype(int32_t gcDataType, int32_t procElemSize)
{
  int32_t mmaDataType;

  if (gcDataType == (int32_t)DATA_8S_BIT)
  {
    mmaDataType = (uint32_t)MMALIB_INT8;
    if (procElemSize == 2 )
    {
      mmaDataType = (uint32_t)MMALIB_INT16;
    }
  }
  else if (gcDataType  == (int32_t)DATA_8_BIT)
  {
   mmaDataType = (uint32_t)MMALIB_UINT8;
   if (procElemSize == 2 )
   {
     mmaDataType = (uint32_t)MMALIB_UINT16;
   }
  }
  else if (gcDataType == (int32_t)DATA_16_BIT)
  {
   mmaDataType = (uint32_t)MMALIB_UINT16;
  }
  else if (gcDataType == (int32_t)DATA_16S_BIT)
  {
   mmaDataType = (uint32_t)MMALIB_INT16;
  }
  else if (gcDataType == (int32_t)DATA_32S_BIT)
  {
    mmaDataType = (uint32_t)MMALIB_INT32;
  }
  else if (gcDataType == (int32_t)DATA_32_BIT)
  {
    mmaDataType = (uint32_t)MMALIB_UINT32;
  }
  else if (gcDataType == (int32_t) DATA_64S_BIT)
  {
    mmaDataType = (uint32_t)MMALIB_INT64;
  }
  else if (gcDataType == (int32_t) DATA_64_BIT)
  {
    mmaDataType = (uint32_t)MMALIB_UINT64;
  }
  else if (gcDataType == (int32_t) FLOAT_32S_BIT)
  {
    mmaDataType = (uint32_t)MMALIB_FLOAT32;
  }

  return mmaDataType;

}

int32_t TIDL_getMmalibDatatypeFromTidlDatatype(int32_t tidlElemType, int32_t procElemSize)
{
  int32_t mmaDataType;

  if (tidlElemType == (int32_t)TIDL_SignedChar)
  {
    mmaDataType = (uint32_t)MMALIB_INT8;
    if (procElemSize == 2 )
    {
      mmaDataType = (uint32_t)MMALIB_INT16;
    }
  }
  else if (tidlElemType  == (int32_t)TIDL_UnsignedChar)
  {
   mmaDataType = (uint32_t)MMALIB_UINT8;
   if (procElemSize == 2 )
   {
     mmaDataType = (uint32_t)MMALIB_UINT16;
   }
  }
  else if (tidlElemType == (int32_t)TIDL_UnsignedShort)
  {
   mmaDataType = (uint32_t)MMALIB_UINT16;
  }
  else if (tidlElemType == (int32_t)TIDL_SignedShort)
  {
   mmaDataType = (uint32_t)MMALIB_INT16;
  }
  else if (tidlElemType == (int32_t)TIDL_SignedWord)
  {
    mmaDataType = (uint32_t)MMALIB_INT32;
  }
  else if (tidlElemType == (int32_t)TIDL_UnsignedWord)
  {
    mmaDataType = (uint32_t)MMALIB_UINT32;
  }
  else if (tidlElemType == (int32_t)TIDL_SignedDoubleWord)
  {
    mmaDataType = (uint32_t)MMALIB_INT64;
  }
  else if (tidlElemType == (int32_t)TIDL_UnsignedDoubleWord)
  {
    mmaDataType = (uint32_t)MMALIB_UINT64;
  }
  else if (tidlElemType == (int32_t)TIDL_SinglePrecFloat)
  {
    mmaDataType = (uint32_t)MMALIB_FLOAT32;
  }

  return mmaDataType;

}

void TIDL_printDMATr(void * dmaTr)
{
  CSL_UdmapTR * tr = (CSL_UdmapTR  *) dmaTr;
  tidl_printf(0,"sicnt_0 %16d \n", tr->icnt0);
  tidl_printf(0,"sicnt_1 %16d sdim1 %16d\n", tr->icnt1, tr->dim1);
  tidl_printf(0,"sicnt_2 %16d sdim1 %16d\n", tr->icnt2, tr->dim2);
  tidl_printf(0,"sicnt_3 %16d sdim1 %16d\n", tr->icnt3, tr->dim3);

  tidl_printf(0,"dicnt_0 %16d \n", tr->dicnt0);
  tidl_printf(0,"dicnt_1 %16d ddim1 %16d\n", tr->dicnt1, tr->ddim1);
  tidl_printf(0,"dicnt_2 %16d ddim1 %16d\n", tr->dicnt2, tr->ddim2);
  tidl_printf(0,"dicnt_3 %16d ddim1 %16d\n", tr->dicnt3, tr->ddim3);
}

/**
* @brief Check for border pixel
*
* @param spatialOffsetY : Offset value in Height dimension
* @param spatialOffsetX : Offset value in Width dimension
* @param validPosXMin : Minimum width value to be a valid pixel
* @param validPosXMax : Maximum width value to be a valid pixel
* @param validPosYMin : Minimum height value to be a valid pixel
* @param validPosYMax : Maximum height value to be a valid pixel
* @return isBorderPixel : Returning true for border pixels
*/
OPENACC(routine(TIDL_checkPixelInPadRegion))
int32_t TIDL_checkPixelInPadRegion(int32_t spatialOffsetY,
                       int32_t spatialOffsetX,
                       int32_t validPosXMin,
                       int32_t validPosXMax,
                       int32_t validPosYMin,
                       int32_t validPosYMax)
{
  int32_t isBorderPixel = 0;

  if ((spatialOffsetY < validPosYMin) || (spatialOffsetY >= validPosYMax))
    isBorderPixel = 1;
  if ((spatialOffsetX < validPosXMin) || (spatialOffsetX >= validPosXMax))
    isBorderPixel = 1;

  return isBorderPixel;
}

/**
 * @brief This function provides the core number on which
 * the code is executing
 *
 * @param void : None
 * @retuen : Core number
 */
uint32_t TIDL_getCoreNum(void){
  uint32_t corePacNum = 0;
#ifdef SOC_J784S4 
#if  !defined(HOST_EMULATION) && (TIDL_MAX_CORENUM > 1)
  #include <c7x.h>
  uint64_t dnum;
  /* Get the bits from bit 7 to bit 15, which represents the core pac number */
  dnum = __DNUM;
  /* Enums are common across devices, subtract enum for C7x_1 since TIDL array indexing starts from C7x_1, CSL has support for other cores so C7x_1 may not be enum 0 */
  corePacNum = CSL_REG64_FEXT(&dnum, C7X_CPU_DNUM_COREPACNUM) - CSL_C7X_CPU_COREPACK_NUM_C7X1;
#endif
#endif
  return corePacNum ;
}

/*Function to determine the saturation values given a scale and clip value*/
int32_t TIDL_getParametricSatLimit(float32_tidl clipVal, float32_tidl tensorScale, int32_t zeroPoint)
{
  int32_t satVal;
  satVal = round(clipVal/tensorScale) + zeroPoint;
  return satVal;
}
typedef union {
  float32_tidl f;
  struct {
    unsigned int mantisa : 23;
    unsigned int exponent : 8;
    unsigned int sign : 1;
  } parts;
} float_cast;

#define FLOAT_MANTISSA_PRECISION 23
//:TODO: Eventually this will be removed ( not applicable for multi input tensors)
void TIDL_GetTensorDimensions(const sTIDL_Network_t *net,
                              const void *workloadUnit,
                              int32_t  layerIdx,
                              int32_t *inWidth,
                              int32_t *inHeight,
                              int32_t *inChannels,
                              int32_t *outWidth,
                              int32_t *outHeight,
                              int32_t *outChannels)
{
  sTIDL_Network_t * tidlNet = (sTIDL_Network_t *)net;
  sTIDL_DataParams_t *inDataParams = TIDL_getDataParams(tidlNet, tidlNet->TIDLLayers[layerIdx].inData[0]);
  sWLMetaDataID_t wlMetaData;
  const sWorkloadUnit_t * wlUnit = (const sWorkloadUnit_t *)workloadUnit;

  if (wlUnit != NULL )
  {
    getWLMetaDataID(wlUnit->metadata, &wlMetaData);
  }


  if (inWidth != NULL)
  {
    *inWidth  = inDataParams->dimValues[TIDL_DIM_WIDTH];
  }
  if (inHeight != NULL)
  {
    *inHeight = inDataParams->dimValues[TIDL_DIM_HEIGHT];
    if ( wlUnit != NULL )
    {
      *inHeight = wlMetaData.inTileHeight;
    }
  }
  if (inChannels != NULL)
  {
    *inChannels = inDataParams->dimValues[TIDL_DIM_NUMCH];
  }

  if (outWidth != NULL)
  {
    *outWidth = tidlNet->TIDLLayers[layerIdx].outData.dimValues[TIDL_DIM_WIDTH];
  }
  if (outHeight != NULL)
  {
    *outHeight = tidlNet->TIDLLayers[layerIdx].outData.dimValues[TIDL_DIM_HEIGHT];
    if ( wlUnit != NULL )
    {
      *outHeight = wlMetaData.outTileHeight;
    }
  }
  if (outChannels != NULL)
  {
    *outChannels = tidlNet->TIDLLayers[layerIdx].outData.dimValues[TIDL_DIM_NUMCH];
  }

  return;
}


/**
* @brief Helper function to convert float real value to scale and shift. Inputs should be positive float values only.
val can be represented as val = scale*(2^(shift)).
*
* @param val : Input float value
* @param scale : Pointer to the pass the calculated scale value
* @param shift : Pointer to the pass the calculated shift value
* @param precisionBits : Precision to which scale has to calculated

* @return void
*/
OPENACC(routine(TIDL_convertFloatToScaleAndShift))
void TIDL_convertFloatToScaleAndShift(float32_tidl val, int32_t *scale, int32_t *shift, int32_t precisionBits)
{
  float_cast FloatStruct;
  uint32_t mant;

  FloatStruct.f = val;
  mant = FloatStruct.parts.mantisa;

  mant += (1<<FLOAT_MANTISSA_PRECISION);
  mant = (mant)>>(FLOAT_MANTISSA_PRECISION+1-precisionBits);
  *shift = FloatStruct.parts.exponent-127-precisionBits+1;
  *scale = mant;

}

/**** Compression Utilities for Weight compression ******/
/**
----------------------------------------------------------------------------
@ingroup    TIDL_CommonUtils
@fn         TIDL_getUncompressedBitstreamLengthTensor
@brief      This function to calculate tensor input bitstream length for compression.

@param      histogram : Pointer to the histogram. Currently is Int32_t type
            should be changed to Int64_t if necessary.
@param      bitdepth: Tensor bitdepth
@return     bitstream length in bytes
----------------------------------------------------------------------------
*/
int64_t TIDL_getUncompressedBitstreamLengthTensor(const int64_t *histogram)
{
  int32_t histogramLength;
  int32_t i0;
  int64_t samples = 0;
  int64_t bitstreamLengthBits, bitstreamLengthBytes;
  int32_t bitdepth = 8;
  histogramLength = 1U << bitdepth;

  for (i0 = 0; i0 < histogramLength; i0++)
  {
    samples += *(histogram + i0);
  }

  bitstreamLengthBits = (int64_t)samples * (int64_t)bitdepth;
  bitstreamLengthBytes = bitstreamLengthBits / 8;
  bitstreamLengthBytes = (bitstreamLengthBits % 8 == 0) ? bitstreamLengthBytes : bitstreamLengthBytes + 1;

  return bitstreamLengthBytes;
}


//:TODO: Eventually this will be removed
float32_tidl TIDL_getTensorScale(const sTIDL_Network_t *net, int32_t layerIdx)
{
  const sTIDL_DataParams_t *dataParams = &net->TIDLLayers[layerIdx].outData;
  return dataParams->tensorScale;
}

//:TODO: Eventually this will be removed
int32_t TIDL_getTensorZeroPoint(const sTIDL_Network_t *net, int32_t layerIdx)
{
  const sTIDL_DataParams_t *dataParams = &net->TIDLLayers[layerIdx].outData;
  return dataParams->tensorZeroPoint;
}

//:TODO: Eventually this will be removed
int32_t TIDL_getRoundbits(const sTIDL_Network_t *net, int32_t layerIdx)
{
  const sTIDL_DataParams_t *dataParams = &net->TIDLLayers[layerIdx].outData;
  return dataParams->roundBits;
}

const sBufParams_t * TIDL_getBufferParams(const sJoint_t *joint,const sGCHelperHandle * gcHelperHandle)
{
  const sBufParams_t *buf = getBufParamsFromBufIndex(gcHelperHandle, joint->bufDBindex);
  int32_t bufType = buf->type;

  /* To access full tensor info, we need to look at the BUF_FM_FULL buffer to which the FM_PART buffer is associated.
    For rest of the buffers, we can simply return the same buffer itself */
  for (int32_t i = 0; i < 2; i++)
  {
    if (bufType == BUF_FM_IN_PART)
    {
      buf = getBufParamsFromBufIndex(gcHelperHandle, joint->parentBufDBIndex[i]);
    }
    else if (bufType == BUF_FM_OUT_PART)
    {
      buf = getBufParamsFromBufIndex(gcHelperHandle, joint->childBufDBIndex[i]);
    }

    if(buf->type == BUF_FM_FULL)
    {
      break;
    }
  }

  return buf;
}

void TIDL_getminMaxValuesHistogram(const int64_t *histogram, int32_t dataType, int32_t *min, int32_t *max)
{
  int32_t bitdepth = 8;
  int32_t histogramLength;
  int32_t i0;
  histogramLength = 1U << bitdepth;

  int32_t smallestValue, largestValue;
  int64_t binCount;
  int64_t minBinCount = INT64_MAX, maxBinCount = 0;

  if (dataType == 0) // unsigned
  {
    for (i0 = 0; i0 < histogramLength; i0++)
    {
      binCount = *(histogram + i0);
      if (binCount < minBinCount)
      {
        smallestValue = i0;
        minBinCount = binCount;
      }
      if (binCount > maxBinCount)
      {
        largestValue = i0;
        maxBinCount = binCount;
      }
    }
  }
  else
  {
    smallestValue = (-histogramLength / 2) - 1;
    for (i0 = -histogramLength / 2; i0 < histogramLength; i0++)
    {
      int32_t idx = (i0 <= 0) ? -2 * i0 : (2 * i0) - 1;
      binCount = *(histogram + idx);
      binCount = *(histogram + i0);
      if (smallestValue == ((-histogramLength / 2) - 1) && binCount > 0)
      {
        smallestValue = i0;
      }
      if (binCount > 0)
      {
        largestValue = i0;
      }
    }
  }
  *min = smallestValue;
  *max = largestValue;
}

/**
----------------------------------------------------------------------------
@ingroup    TIDL_CommonUtils
@fn         TIDL_getBitstreamLengthDRUCodingZeroBasedScheme
@brief      This function calculates tensor bitstream length when encoded as
            zero based compression scheme supported by the DRU.

@param      histogram : Pointer to the histogram. Currently is Int32_t type
            should be changed to Int64_t if necessary.
@param      bitdepth: Tensor bitdepth
@param      bias:  pointer to the varibale in which the selected bias
            value is to be returned. Null will implicitly assume bias to 0
@return     bitstream length in bytes
----------------------------------------------------------------------------
*/
int64_t TIDL_getBitstreamLengthDRUCodingZeroBasedScheme(const int64_t *histogram, int32_t elementType, int32_t *bias = NULL)
{
  int32_t bitdepth = 8;
  int32_t histogramLength;
  int32_t i0;
  int64_t numNonZeros = 0;
  int32_t nonZeroSymbolSize;
  int64_t bitstreamLengthBits, bitstreamLengthBytes;
  int32_t selectedBias = 0;
  nonZeroSymbolSize = (bitdepth == 8) ? 9 : 17;
  histogramLength = 1U << bitdepth;
  if (bias != NULL)
  {
    int64_t maxHistValue = 0;
    for (i0 = 0; i0 < histogramLength; i0++)
    {
      if (*(histogram + i0) > maxHistValue)
      {
        maxHistValue = *(histogram + i0);
        selectedBias = i0;
      }
    }
    *bias = (elementType != TIDL_UnsignedChar) ? selectedBias - 128 : selectedBias;
  }

  for (i0 = 0; i0 < histogramLength; i0++)
  {
    if (i0 != selectedBias)
    {
      numNonZeros += *(histogram + i0);
    }
  }
  bitstreamLengthBits = (int64_t)(*(histogram + selectedBias)) + (numNonZeros * (int64_t)nonZeroSymbolSize);
  bitstreamLengthBytes = bitstreamLengthBits / 8;
  bitstreamLengthBytes = (bitstreamLengthBits % 8 == 0) ? bitstreamLengthBytes : bitstreamLengthBytes + 1;

  return bitstreamLengthBytes;
}

/**
----------------------------------------------------------------------------
@ingroup    TIDL_CommonUtils
@fn         TIDL_getExpGolombCodeLength
@brief      This function returns the length of the VLC for input symbol.

@param      symbol : Symbol for which the exponential golomb code length is to be
            calculated.
@return     length of the exponential golomb code for the given symbol
----------------------------------------------------------------------------
*/
int32_t TIDL_getExpGolombCodeLength(const int32_t symbol)
{
  int32_t codeLength;
  codeLength = 2 * int32_t(floor(log2f(float32_tidl(symbol + 1)))) + 1;
  return codeLength;
}

/**
----------------------------------------------------------------------------
@ingroup    TIDL_CommonUtils
@fn         TIDL_DRUbiasSubtraction
@brief      This function replicates the DMAUtils bias subtraction logic.

@param      symbol : Pointer to the histogram. Currently is Int32_t type
            should be changed to Int64_t if necessary.
@param      bias: Tensor bitdepth
----------------------------------------------------------------------------
*/
inline int32_t TIDL_DRUCompBiasSubtraction(int32_t symbol, int32_t bias)
{
  int32_t length = 8;
  int32_t temp = symbol - bias;
  /*prevent underflowing*/
  temp &= (1 << (length)) - 1;
  return temp;

}

/**
----------------------------------------------------------------------------
@ingroup    TIDL_CommonUtils
@fn         TIDL_ExpGolombSignedToUnsignedRemapping
@brief      This function remaps signed integer values to unsigned integer
            valus required for exponential Golomb coding.

@param      value : Value to be remapped.
@return     Remapped value
----------------------------------------------------------------------------
*/
inline int32_t TIDL_ExpGolombSignedToUnsignedRemapping(int32_t value)
{
  int32_t remappedVal = (value > 0) ? (2 * value - 1) : (-2 * value);
  return (remappedVal);
}

/**
----------------------------------------------------------------------------
@ingroup    TIDL_CommonUtils
@fn         TIDL_getBitstreamLengthDRUExpGolombCodingScheme
@brief      This function calculates tensor bitstream length when encoded as
            exponential golomb coding scheme supported by the DRU.

@param      histogram : Pointer to the histogram. Currently is Int32_t type
            should be changed to Int64_t if necessary.
@param      bitdepth: Tensor bitdepth
@param      bias:  To be added
@return     bitstream length in bytes
----------------------------------------------------------------------------
*/

int64_t TIDL_getBitstreamLengthDRUExpGolombCodingScheme(const int64_t *histogram, int32_t elementType, int32_t bias)
{
  int32_t bitdepth = 8;
  int32_t histogramLength;
  int32_t i0;
  int64_t symbolBitwidth, bitstreamLengthBits = 0, bitstreamLengthBytes;
  histogramLength = 1U << bitdepth;

  for (i0 = 0; i0 < histogramLength; i0++)
  {
    int32_t symbol = (elementType != TIDL_UnsignedChar) ? TIDL_ExpGolombSignedToUnsignedRemapping(TIDL_DRUCompBiasSubtraction((i0 - 128), bias)) : TIDL_DRUCompBiasSubtraction(i0, bias);
    symbolBitwidth = TIDL_getExpGolombCodeLength(symbol);
    bitstreamLengthBits += (int64_t)(*(histogram + i0)) * (int64_t)(symbolBitwidth);
  }
  bitstreamLengthBytes = bitstreamLengthBits / 8;
  bitstreamLengthBytes = (bitstreamLengthBits % 8 == 0) ? bitstreamLengthBytes : bitstreamLengthBytes + 1;
  return bitstreamLengthBytes;
}

void TIDL_computeHistogram1D(int64_t *histogram, uint8_t *bufferPtr, uint64_t bufLength, int32_t elementType)
{
  /*Assumption that histogram buffer has been allocated by the function calling computeHistogram (e.g. 8*256 bytes for 8-bit datatype)*/
  int32_t offset = 0;
  int32_t unsignedVal;
  if (elementType == TIDL_SignedChar)
  {
    offset = 128;
  }
  if (elementType == TIDL_SignedShort || elementType == TIDL_UnsignedShort)
  {
    offset = 128;
  }
  int32_t i;
  for (i = 0; i < bufLength; i++)
  {
    unsignedVal = (int32_t)(bufferPtr[i]) + offset;
    *(histogram + unsignedVal) += 1;
  }
}

void TIDL_computeCompressionAlgo(uint8_t *bufferPtr, uint64_t bufLength, int32_t elementType, int32_t *retBias, int32_t *retAlg)
{
  // 8-bit:
  int32_t bitdepth = 8;
  int32_t i;
  int64_t *histogram;
  int32_t min, max;
  int32_t bias = NULL;
  int64_t histogramBuffer[256];
  histogram = &histogramBuffer[0];
  memset(histogram, 0, 8 * (1 << bitdepth));
  TIDL_computeHistogram1D(histogram, bufferPtr, bufLength, elementType);
  TIDL_getminMaxValuesHistogram(histogram, elementType, &min, &max);
  // Compression ratio:
  int64_t uncompressedLength = TIDL_getUncompressedBitstreamLengthTensor(histogram);
  int64_t zbLen = TIDL_getBitstreamLengthDRUCodingZeroBasedScheme(histogram, elementType, &bias);
  // Iterate over possible egBiases:
  int32_t egBias = 0;
  int64_t egLen, minLen = INT64_MAX;
  for (i = 0; i < (1 << bitdepth); i++)
  {
    egLen = TIDL_getBitstreamLengthDRUExpGolombCodingScheme(histogram, elementType, i);
    if (egLen < minLen)
    {
      egBias = i;
      minLen = egLen;
    }
  }
  egLen = TIDL_getBitstreamLengthDRUExpGolombCodingScheme(histogram, elementType, egBias);
  tidl_printf(2, "Compression Factors: ZB = %f, Bias = %d\n", (((float)zbLen) / uncompressedLength), bias);
  tidl_printf(2, "Compression Factors: EG = %f, Bias = %d\n", (((float)egLen) / uncompressedLength), egBias);
  if (egLen < zbLen && egLen < uncompressedLength)
  {
    *retAlg = 1;
    *retBias = egBias;
  }
  else if (zbLen < egLen && zbLen < uncompressedLength)
  {
    *retAlg = 0;
    *retBias = bias;
  }
  else
  {
    *retAlg = -1;
  }

}

#define TIDL_MAX_SBSIZE (512U * 1024U)
int32_t TIDL_set1DCompressionProperties(int32_t bufSize, sTIDL_compressionDimensions* compDim)
{
  int32_t status = TIDL_SUCCESS;
  int32_t remainingBufSize = 0;
  int32_t pow2Factor = 1;
  int32_t nonPow2Factor = 1U;

  /*Single Superblock Case:*/
  if(bufSize % 16U != 0 || bufSize == 0)
  {
    /*Innermost count has to be a multiple of 2*/
    status = TIDL_ERR_INVALID_VALUE;
  }

  /*Divide such that sbICNT0 is a power of 2 value < 64K*/
  if(TIDL_SUCCESS == status)
  {
    remainingBufSize = bufSize / 16U;
    while((remainingBufSize % pow2Factor == 0) && (pow2Factor < remainingBufSize))
    {
      pow2Factor *= 2U;
    }
    pow2Factor /= 2U; /*Correct the factor*/
    nonPow2Factor = remainingBufSize / pow2Factor;
    /*BufSize = 16 x pow2Factor x nonPow2Factor*/
    if(bufSize > TIDL_MAX_SBSIZE)
    {
      if(pow2Factor == 1U)
      {
        status = TIDL_ERR_INVALID_VALUE;
      }
      else
      {
        /*Need to maximize superblock size*/
        int32_t pow2subFactor = 1U;
        while(pow2Factor > 2048U)
        {
          pow2subFactor *= 2U;
          pow2Factor /= 2U;
        }
        /*16 x pow2Factor | pow2subFactor | nonPow2Factor*/
        compDim->sbIcnt0 = 16U * pow2Factor;
        compDim->sbIcnt1 = pow2subFactor;
        compDim->sIcnt0  = nonPow2Factor;
        compDim->sbDim1  = compDim->sbIcnt0;
        compDim->sDim0   = compDim->sbDim1 * compDim->sbIcnt1;
      }
    }
    else
    {
      int32_t pow2subFactor = 1U;
      while(pow2Factor > 2048U)
      {
        pow2subFactor *= 2U;
        pow2Factor /= 2U;
      }
      /*16 x pow2Factor | pow2subFactor | nonPow2Factor*/
      compDim->sbIcnt0 = 16U * pow2Factor;
      compDim->sbIcnt1 = pow2subFactor * nonPow2Factor;
      compDim->sIcnt0  = 1U;
      compDim->sbDim1  = compDim->sbIcnt0;
      compDim->sDim0   = 0U;
    }
  }

  return status;
}

int32_t TIDL_compressFlatSW( uint8_t* const pInput,
                            uint8_t* const pOutput,
                            uint8_t* const sectr,
                            uint8_t* const cdbTable,
                            uint8_t* trMem,
                            sTIDL_compressionDimensions* pCompDims
                            )
{

  int32_t retVal = CSL_PASS ;

  uint32_t TrSize;

  DmaUtilsAutoInc3d_TrPrepareParam   trPrepParam;
  DmaUtilsAutoInc3d_TransferProp     transferProp;

  TrSize = DmaUtilsAutoInc3d_getTrMemReq(1U);

  trPrepParam.channelId = 0U; //Dummy Value
  trPrepParam.numTRs    = 1U;
  trPrepParam.trMem     = trMem;
  trPrepParam.trMemSize = TrSize;
  transferProp.syncType               = DMAUTILSAUTOINC3D_SYNC_2D;
  transferProp.dmaDfmt                = DMAUTILSAUTOINC3D_DFMT_COMP;
  transferProp.transferDim.sicnt0     = pCompDims->sIcnt0;
  transferProp.transferDim.sicnt1     = 1U;
  transferProp.transferDim.sicnt2     = 1U;
  transferProp.transferDim.sicnt3     = 1U;

  transferProp.transferDim.dicnt0     = pCompDims->sIcnt0;
  transferProp.transferDim.dicnt1     = 1U;
  transferProp.transferDim.dicnt2     = 1U;
  transferProp.transferDim.dicnt3     = 1U;

  transferProp.circProp.circSize1     = 0;
  transferProp.circProp.circSize2     = 0;
  transferProp.circProp.addrModeIcnt0 = 0;
  transferProp.circProp.addrModeIcnt1 = 0;
  transferProp.circProp.addrModeIcnt2 = 0;
  transferProp.circProp.addrModeIcnt3 = 0;
  transferProp.circProp.circDir       = 0;


  transferProp.cmpProp.cmpAlg         = pCompDims->algorithm;
  transferProp.cmpProp.cmpBias        = pCompDims->bias;
  transferProp.cmpProp.varKStartK     = 0; //generic not supported
  transferProp.cmpProp.varKUpdateK    = 0; //generic not supported
  transferProp.cmpProp.varKElemSize   = 0; //generic not supported
  transferProp.cmpProp.varKSubType    = 0; //generic not supported
  transferProp.cmpProp.varKSubSel0    = 0; //generic not supported
  transferProp.cmpProp.varKSubSel1    = 0; //generic not supported
  transferProp.cmpProp.varKSubSel2    = 0; //generic not supported
  transferProp.cmpProp.varKSubSel3    = 0; //generic not supported
  transferProp.cmpProp.sbIcnt0        = pCompDims->sbIcnt0 / 16U;
  transferProp.cmpProp.sbIcnt1        = pCompDims->sbIcnt1; //2 rows per superblock
  transferProp.cmpProp.sbDim1         = pCompDims->sbDim1;
  transferProp.cmpProp.sbAM0          = 0;
  transferProp.cmpProp.sbAM1          = 0;

  transferProp.transferDim.sdim1      = 0;
  transferProp.transferDim.sdim2      = 0;
  transferProp.transferDim.sdim3      = 0;

  transferProp.transferDim.ddim1      = 0U; // table dims
  transferProp.transferDim.ddim2      = 0;
  transferProp.transferDim.ddim3      = 0;

  transferProp.cmpProp.sDim0          = pCompDims->sDim0;
  transferProp.cmpProp.dDim0          = 8U; /*CDB Entry Size*/


  transferProp.ioPointers.srcPtr      = pInput;
  transferProp.ioPointers.dstPtr      = pOutput;
  transferProp.ioPointers.strPtr      = sectr;
  transferProp.ioPointers.cdbPtr      = cdbTable;


  retVal = DmaUtilsAutoInc3d_prepareTr(&trPrepParam, &transferProp);
  if ( retVal != CSL_PASS )
  {
    goto Exit;
  }

  DmaUitlsAutoInc3d_CompressSW(trMem);

  Exit:
  {
      return retVal ;
  }

}

/* #define SYNCHRONIZATION_DEBUG */

/** Sync function for multi-core execution */
void TIDL_syncWl(TIDL_sharedBufferProperties * ctrlSetArgs, int32_t wlToPoll[],int32_t syncPoint, int32_t wlExecId, uint32_t coreId, int32_t numCores)
{
#ifdef SYNCHRONIZATION_DEBUG
  printf("Core Id %d Wl Exec Id -- %d ...starting sync \n", coreId, wlExecId);
#endif
  volatile int8_t * syncBuf;
  volatile int8_t updatedStateToWrite = NOT_VALID; /* don't update state if this value is NOT_VALID */
  volatile int8_t stateToPoll = NOT_VALID;  /* no need to poll/sync if this value is NOT_VALID */
  volatile int32_t wlToPollInt[TIDL_MAX_NUM_CORES];
  int8_t syncPtrOffset = (int8_t)TIDL_GLOBAL_SYNC_OFFSET;
  memset((void *)wlToPollInt, 0, TIDL_MAX_NUM_CORES * sizeof(int32_t));
  
  if(syncPoint == TIDL_SYNC_POINT_START)
  {
    updatedStateToWrite = NOT_VALID;
    for(int i = 0; i < numCores; i++)
    {
      wlToPollInt[i] = wlToPoll[i]; /* wlToPoll will come as dependId in the function arguments */
    }
    stateToPoll = TIDL_SYNC_WL_STATE_COMPLETE;
  }
  else if(syncPoint == TIDL_SYNC_POINT_DMA_READY)
  {
    updatedStateToWrite = TIDL_SYNC_WL_STATE_DMA_READY; /* current core state to be updated to indicate it is ready to start cross core DMA */
    syncBuf = (int8_t *)ctrlSetArgs[coreId].syncBufferPtr[TIDL_Virtual] + syncPtrOffset; /* Current core's sync buffer */
    syncBuf[wlExecId] = updatedStateToWrite; /* Update the state for current core as soon as code reaches this point */

    stateToPoll = TIDL_SYNC_WL_STATE_DMA_READY | TIDL_SYNC_WL_STATE_COMPLETE; /* Just polling on TIDL_SYNC_WL_STATE_DMA_READY can result in race condition
    if one of the cores' passes sync polling condition and completes execution(TIDL_SYNC_WL_STATE_COMPLETE) before other cores have consumed TIDL_SYNC_WL_STATE_DMA_READY state*/
    for(int i = 0; i < numCores; i++)
    {
      wlToPollInt[i] = wlExecId; /* Should poll on current layer id across cores to check if they are ready for DMA / have completed execution */
    }
    updatedStateToWrite = NOT_VALID; /* State already updated above, no need to update again */
  }
  else if(syncPoint == TIDL_SYNC_POINT_COMPLETE)
  {
    updatedStateToWrite = TIDL_SYNC_WL_STATE_COMPLETE;
    stateToPoll = NOT_VALID;     /* No need to poll/sync here - just update the state */
  }
  else if(syncPoint == TIDL_SYNC_POINT_GLOBAL_SYNC)
  {
    updatedStateToWrite = TIDL_SYNC_WL_STATE_RESET;
    syncBuf = (int8_t *)ctrlSetArgs[coreId].syncBufferPtr[TIDL_Virtual]; /* Current core's sync buffer */
    syncBuf[0] = updatedStateToWrite; /* Update the state for current core as soon as code reaches this point */
    for(int i = 0; i < numCores; i++)
    {
      wlToPollInt[i] = 0; /* Poll at offset 0 in sync buf which is reserved for global sync */
    }
    stateToPoll = TIDL_SYNC_WL_STATE_RESET | TIDL_SYNC_WL_STATE_DMA_READY | TIDL_SYNC_WL_STATE_COMPLETE; /* Poll on any of the states >= current state */
    updatedStateToWrite = NOT_VALID; /* State already updated above, no need to update again */
    syncPtrOffset = 0;
  }
  else
  {
    tidl_printf(0, "Incorrect sync point - %d , continuing without sync\n", syncPoint);
    stateToPoll = NOT_VALID;
  }

#ifdef SYNCHRONIZATION_DEBUG
  tidl_printf(0, "coreId %d syncPoint - %d  stateToPoll - %d updatedStateToWrite - %d wlToPollInt - %d %d \n", coreId, 
  syncPoint, stateToPoll, updatedStateToWrite, wlToPollInt[0], wlToPollInt[1], wlToPollInt[2], wlToPollInt[3]);
#endif

  int32_t waitCondition = 0;
  while(!waitCondition)
  {
    for(int i = 0; i < numCores; i++)
    {
      waitCondition = 1;
      if(stateToPoll != NOT_VALID)
      {
        if(wlToPollInt[i] != (int32_t)NOT_VALID) /* can be NOT_VALID for don't care condition */
        {
          syncBuf = (int8_t *)ctrlSetArgs[i].syncBufferPtr[TIDL_Virtual] + syncPtrOffset;
          if((syncBuf[wlToPollInt[i]] & stateToPoll) != syncBuf[wlToPollInt[i]])
          {
#ifdef SYNCHRONIZATION_DEBUG
            tidl_printf(0, "Core id %d Wl Exec id - %d , Polling on - %d , wlToPoll %d buffer val[%d] %d \n", coreId, wlExecId, stateToPoll, wlToPollInt[i], i, syncBuf[wlToPollInt[i]]);
#endif
            waitCondition = 0;
            break;
          }
        }
      }
      else
      {
        break;
      }
    }
    if(waitCondition)
    {
      syncBuf = (int8_t *)ctrlSetArgs[coreId].syncBufferPtr[TIDL_Virtual] + syncPtrOffset;
      if(updatedStateToWrite != NOT_VALID)
      {
#ifdef SYNCHRONIZATION_DEBUG
        tidl_printf(0, "Core Id %d Layer Id %d Writing state %d \n", coreId, layerId, updatedStateToWrite);
#endif
        syncBuf[wlExecId] = updatedStateToWrite;
      }
    }
  }
}

int32_t TIDL_isConcatBalanced( TIDL_NetworkCommonParams *commonParams, sTIDL_Layer_t *layer, sTIDL_AlgLayer_t *algLayer, int32_t layerIdx)
{
  /*Concat with different tensor sizes being concatenated along the width and height dimensions aren't
    supported*/
  int32_t isSupported = 1;
  const sTIDL_ConcatParams_t &concatParams = layer->layerParams.concatParams;
  if (layer->layerType == TIDL_ConcatLayer)
  {
    if ( (concatParams.axis == TIDL_DIM_HEIGHT) || (concatParams.axis == TIDL_DIM_WIDTH)  )
    {
      int32_t inBufIdx;
      sTIDL_DataParams_t *inDataParams  = &commonParams->net->TIDLLayers[algLayer->inLayerIdx[0]].outData;
      int32_t width  = inDataParams->dimValues[TIDL_DIM_WIDTH];
      int32_t height = inDataParams->dimValues[TIDL_DIM_HEIGHT];
      /*Loop from the second buffer onwards and check if width or height change:*/
      for ( inBufIdx = 1; inBufIdx < layer->numInBufs; inBufIdx++)
      {
        sTIDL_DataParams_t *inDataParams  = &commonParams->net->TIDLLayers[algLayer->inLayerIdx[inBufIdx]].outData;
        if((width != inDataParams->dimValues[TIDL_DIM_WIDTH]) ||
            (height != inDataParams->dimValues[TIDL_DIM_HEIGHT]))
        {
          isSupported = 0;
          break;
        }
      }
    }
  }
  return isSupported;
}

/*
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
}*/

