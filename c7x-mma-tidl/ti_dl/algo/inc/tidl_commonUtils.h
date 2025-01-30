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
@file    tidl_commonUtils.h
@brief   This file contains TILD common utilities
@version 0.1 (May 2018) : Initial version [ADK]
----------------------------------------------------------------------------
*/

#ifndef ITIDL_COMMON_UTILS_H
#define ITIDL_COMMON_UTILS_H

#if defined (__C7100__) || defined (__C7120__) || defined (__C7504__) || defined (__C7524__)
#if !defined(_HOST_BUILD)
 #include <c7x.h>
 #define RESTRICT restrict
 #else
 #include <c7x.h>
 #define RESTRICT
 #endif
#endif

#include "tidl_alg_int.h"
#include "tidl_bufParams.h"
#include "tidl_alg_int.h"
#include <typeinfo>
#include <limits>


#define TIDL_NUM_ACTIVATION_HISTOGRAM_BINS (1024U)

#define TIDL_INTERNAL_POOLING_WEIGHT_Q_U8   ((uint32_t)8)
#define TIDL_INTERNAL_POOLING_WEIGHT_Q_U16   ((uint32_t)12)
#define TIDL_INTERNAL_INDATA_Q              ((uint32_t)7)


#if defined(HOST_EMULATION)
#define ENABLE_PROFILE  (0)
#else
#define ENABLE_PROFILE  (1)
#endif
#define TRACE_STRING_SIZE (1024U)
#define TRACE_STRINGS_MEM_SIZE (3U*1024U)


#define TIDL_MAX_CORENUM (TIDL_MAX_NUM_CORES) /* IMP: Should be in synch and same as TIDL_TB_MAX_CORENUM*/
/**
----------------------------------------------------------------------------
@ingroup    TIDL_COMMON_UTILS
@fn         TIDL_getCoreNum
@brief      Function returns the core number on which the code is executing
@param      void : None

@return     Core number on which the code is executing
----------------------------------------------------------------------------
*/
uint32_t TIDL_getCoreNum(void);

#if defined (SOC_J784S4)  || defined (SOC_AM62A)
#ifndef HOST_EMULATION
void TIDL_c7xCleaninvalidateL1DCache();
void TIDL_c7xCleaninvalidateL2Cache();
#endif
#endif

#define TIDL_GLOBAL_SYNC_OFFSET (1) /* Space reserved for global frame level sync */
/* Modes to define execution state of given workload - may require bit wise operations */
typedef enum{
  TIDL_SYNC_WL_STATE_DEFAULT = -1,
  TIDL_SYNC_WL_STATE_RESET = 1,
  TIDL_SYNC_WL_STATE_DMA_READY = 2,  
  TIDL_SYNC_WL_STATE_COMPLETE = 4,
} eTIDL_syncWlState;

/* Modes to indicate location where sync is needed - this determines which of eTIDL_syncWlState to update/poll on */
typedef enum{
  TIDL_SYNC_POINT_START = 0,
  TIDL_SYNC_POINT_DMA_READY = 1,
  TIDL_SYNC_POINT_COMPLETE = 2,
  TIDL_SYNC_POINT_GLOBAL_SYNC = 3
} eTIDL_syncPoint;

void TIDL_syncWl(TIDL_sharedBufferProperties * ctrlSetArgs, int32_t wlToPoll[], int32_t syncPoint, int32_t wlId, 
                  uint32_t coreId, int32_t numCores);

void static inline TIDL_enableL1DCacheWb()
{
#if defined (SOC_J784S4)
#ifndef HOST_EMULATION
  TIDL_c7xCleaninvalidateL1DCache();
#endif
#endif
}

void static inline TIDL_enableL2CacheWb()
{
#if defined (SOC_J784S4)
#ifndef HOST_EMULATION
  TIDL_c7xCleaninvalidateL2Cache();
#endif
#endif
}

void static inline TIDL_enableL1DandL2CacheWb()
{
#if defined (SOC_J784S4)
#ifndef HOST_EMULATION
  TIDL_c7xCleaninvalidateL1DCache();
  TIDL_c7xCleaninvalidateL2Cache();
#endif
#endif
}

void TIDL_getSaturationLimits(int32_t elementType, int32_t * satLow, int32_t * satHigh);

void TIDL_getSaturationFloat(sTIDL_Layer_t        * tidlLayer, float * min, float * max);
float32_tidl TIDL_floatSat(float32_tidl outAcc, sTIDL_Layer_t  * tidlLayer);

void TIDL_UpdateScaleFactors(TIDL_Handle intAlgHandle, int32_t i, int32_t updateStats, int64_t accMin, int64_t accMax);

int32_t tidl_updateAcrossLayerStates(TIDL_Handle algHandle,
                                                                           int32_t currLayerNum,
                                                                           uint8_t ** coeffStagePtr,
                                                                           int32_t isChannelwiseQuantEnable,
                                                                           int32_t targetDevice);
template <class Tdst, class Tsrc>
void TIDL_conv2dBiasSplit(Tsrc * srcPtr, Tdst * dstPtr, int32_t * biasB, int32_t dataSize, float inScaleFactor, int32_t satLow, int32_t satHigh, int32_t biasBMax , int32_t inFeatSign);
template <class Tdst, class Tsrc>
void TIDL_AM_conv2dBiasSplit(Tsrc * srcPtr, Tdst * dstPtr, int32_t * biasB, int32_t dataSize, float inScaleFactor, int32_t satLow, int32_t satHigh, int32_t biasBMax , int32_t inFeatSign);

template <class Tdst, class Tsrc>
void TIDL_conv2dBiasSplitWithFixedBiasB(Tsrc * srcPtr,
                                        Tdst * dstPtr,
                                        int32_t * biasB,
                                        int32_t dataSize,
                                        float32_tidl inScaleFactor,
                                        int32_t satLow,
                                        int32_t satHigh,
                                        int32_t biasBMax,
                                        int32_t inFeatSign);

int32_t  TIDL_prePareMemcpyTr(void * trMem);
int32_t  TIDL_memcpy2D
(
  const void          *dstPtr,
  const void          *srcPtr,
  uint32_t     width,
  uint16_t     height,
  int32_t      dstStride,
  int32_t      srcStride,
  void * dmaUtilsContext,
  uint8_t * trMem
  );
void TIDL_memcpy2DAsyncTrigger(const void   *dstPtr,
    const void   *srcPtr,
    uint16_t     width,
    uint16_t     height,
    int32_t      dstStride,
    int32_t      srcStride,
    void * dmaUtilsContext,
    int32_t   convert16bitTo8Bit,
    uint8_t * trMem);
void TIDL_memcpy2DAsyncWait(void * dmaUtilsContext);

/**
 * @brief Memory copy for trigger (TIDL_memcpy2D) with cache writeback feature at destination pointer memory region
 *
 * @param dstPtr : Pointer to dst memory
 * @param srcPtr : Pointer to src memory
 * @param width : Width of the memory copy
 * @param height : Height of the memory copy
 * @param dstStride : Stride of the dst buffer
 * @param srcStride : Stride of the src buffer
 * @param dmaUtilsContext : memory for dmaUtilsContext
 * @param trMem : pointer to transfer memory
 * @param sysMems : System memory information
 * @param enableL1DWbForL2OrMsmc : Flag depicting whether L1D wb must be enabled for L2/MSMC regions
 */
int32_t static inline  TIDL_memcpy2DCacheWbInv
(
  const void   *dstPtr,
  const void   *srcPtr,
  uint32_t     width,
  uint16_t     height,
  int32_t      dstStride,
  int32_t      srcStride,
  void * dmaUtilsContext,
  uint8_t * trMem,
  sTIDL_sysMemHandle_t sysMems[TIDL_SYSMEM_MAX],
  bool enableL1DWbForL2OrMsmc = false
  )
{
  int32_t retVal = IALG_EOK;
#if defined (SOC_J784S4) || defined (SOC_AM62A)
  if(((uint8_t *)dstPtr >= (uint8_t *)sysMems[TIDL_SYSMEM_DDR_SCRATCH].base) && ((uint8_t *)dstPtr < &((uint8_t *)sysMems[TIDL_SYSMEM_DDR_SCRATCH].base)[sysMems[TIDL_SYSMEM_DDR_SCRATCH].size]))
  {
    /* DDR requires both L1D and L2 cache writeback */
    TIDL_enableL1DandL2CacheWb();
  }
#if defined(SOC_J784S4)
  else if(((uint8_t *)dstPtr >= (uint8_t *)sysMems[TIDL_SYSMEM_L1_SCRATCH].base) && ((uint8_t *)dstPtr < &((uint8_t *)sysMems[TIDL_SYSMEM_L1_SCRATCH].base)[sysMems[TIDL_SYSMEM_L1_SCRATCH].size]))
  {
    /* No writeback needed if destination is L1D */
  }
  else if(enableL1DWbForL2OrMsmc)
  {
    /* L1D cache writeback for L2 and MSMC regions to be done conditionally if specified in function parameters */
    TIDL_enableL1DCacheWb();
  }
#endif
#endif
  retVal = TIDL_memcpy2D(dstPtr, srcPtr, width, height, dstStride, srcStride, dmaUtilsContext, trMem);
  return retVal;
}

void * my_malloc(int size);
void my_free(void *ptr);
int32_t TIDL_UpdateTensorRange(TIDL_Handle intAlgHandle, int32_t i, int32_t j, void * ptr);

int32_t TIDL_UpdateTensorPerChannelMean(TIDL_Handle intAlgHandle,
                                                                                int32_t layerIdx,
                                                                                int32_t outDataIdx,
                                                                                void * ptr);

static inline void TIDL_getTscTime(uint64_t * time)
{
#if ENABLE_PROFILE
  *time = __TSC;
#endif
}

static inline void TIDL_readEmifCounter(uint32_t * count, uint32_t counterIdx)
{
#if !defined(_HOST_BUILD)
#if 0
  uint32_t * currPtr = (uint32_t *) (0x02980104 + ((counterIdx) * 0x4));
  *count = *currPtr;
#else
  *count = 0;
#endif
#endif
}

static inline void TIDL_updateprofileData(uint64_t * ptr, uint32_t idx,  uint64_t tStart, uint64_t tEnd)
{
#if  ENABLE_PROFILE
  ptr[idx] += (tEnd  - tStart);
#endif
}

void tidl_printf(int32_t traceLevel, const char *format, ...);
int32_t tidl_getTraceLogLevel();

typedef int32_t(*tidlVprintf_t)(const char * format, va_list arg);
typedef int32_t(*tidlWriteBinToFile_t)(const char * fileName, void * addr, int32_t size, int32_t count);
typedef int32_t(*tidlReadBinFromFile_t)(const char * fileName, void * addr, int32_t size, int32_t count);
int32_t TIDL_initDebugTraceParams(int32_t traceLogLevel, int32_t traceWriteLevel, tidlVprintf_t tifVprintfFuncPtr, tidlWriteBinToFile_t writeBinFuncPtr, 
                                    tidlReadBinFromFile_t readBinFuncPtr, const char * traceBaseName, void * tracePtr);
int32_t tidl_writeTraceParamBuf(sTIDL_Network_t * net,
                                      sTIDL_AlgLayer_t     * algLayer,
                                      sTIDL_Layer_t        * tidlLayer,
                                      int32_t dataBuffId,
                                      int8_t * orgScratchPtr,
                                      uint32_t orgScratchSize);

int32_t tidl_writeLayerMinMax(sTIDL_Network_t * net, int32_t currLayersGroupId, int8_t * scratchPtr, uint32_t scratchSize);

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
                              const char * traceBaseName);



//Static inline function definitions:
static inline int32_t TIDL_calcLCM(int32_t a, int32_t b)
{
  int lcm, i;
  for(i = 1; i <= b; i++)
  {
      lcm = a * i;
      if(lcm % b == 0)
      {
          break;
      }
  }
  return lcm;
}

static inline int32_t TIDL_ceilDiv(int32_t n, int32_t d)
{
  int32_t result;

  if(n%d==0)
    result =  n/d;
  else
  {
    result = ((n/d) + 1);
  }

  return result;

}

static inline int32_t TIDL_clipNeg(int32_t x)
{
  int32_t result;

  if(x < 0)
  {
    result = 0;
  }
  else
  {
    result = x;
  }
  return result;
}

template <typename Tdst>
static inline Tdst tidl_sat(float val)
{
    Tdst out;
    float max = std::numeric_limits<Tdst>::max();
    float min = std::numeric_limits<Tdst>::lowest();
    val = (val<min) ? min : val;
    out = (val>max) ? max : val;
    return out;
}

int32_t TIDL_generateFillSeamPredicateRegisters(
        int32_t numTiles,
        void *bufPredicateStore,
        const TIDL_bufParams3D_t * srcAddr,
        const TIDL_bufParams3D_t * dstAddr);

static inline int32_t TIDL_isAsymQuantEnabledTFL(int32_t quantizationStyle)
{
  int32_t quantStyleAsym = FALSE;
  if((quantizationStyle == TIDL_QuantStyleAsymNP2_TFL))
  {
    quantStyleAsym = TRUE;
  }
  return quantStyleAsym;
}

static inline int32_t TIDL_isAsymQuantEnabled(int32_t quantizationStyle, int32_t procElemSize)
{
  int32_t quantStyleAsym = FALSE;
  if((quantizationStyle == TIDL_QuantStyleAsymNP2) && (procElemSize != 32U))
  {
    quantStyleAsym = TRUE;
  }
  return quantStyleAsym;
}

static inline int32_t TIDL_isKernelHighPrecision(int32_t layerKernelType)
{
  int32_t isHighPrecision = FALSE;
  if(layerKernelType == TIDL_HighPrecisionKernel)
  {
    isHighPrecision = TRUE;
  }
  return isHighPrecision;
}

void TIDL_printOutDataProperties(sTIDL_DataParams_t* outData, int32_t layerIdx);

#define TIDL_FLOW_CTRL_REF_ONLY (0x00000001)
#define TIDL_FLOW_CTRL_REF_STAT (0x00000002)
#define TIDL_FLOW_CTRL_MMA_NATC (0x00000004)
#define TIDL_FLOW_CTRL_DSP_NATC (0x00000008)
#define TIDL_FLOW_CTRL_REF_COMP (0x00000010)
/* AVX Optimised kernels for refernce flow */
#define TIDL_FLOW_CTRL_AVX_REF (0x00000020)
#define TIDL_FLOW_CTRL_AVX_FLOAT (0x00000040)

int32_t TIDL_getProcessingElementSizeInBytes(const sTIDL_Layer_t  * tidlLayer);
int32_t TIDL_conv2dGetKernelDataType(int32_t tidlElemType, int32_t procElemSize);
int32_t TIDL_GetKernelDataTypeFromNcDataType(int32_t ncDataType);




int32_t TIDL_getMmalibDatatypeFromGcDatatype(int32_t gcDataType, int32_t procElemSize);
int32_t TIDL_getMmalibDatatypeFromTidlDatatype(int32_t tidlElemType, int32_t procElemSize);



void TIDL_printDMATr(void * dmaTr);
int32_t TIDL_checkPixelInPadRegion(
                       int32_t spatialOffsetY,
                       int32_t spatialOffsetX,
                       int32_t validPosXMin,
                       int32_t validPosXMax,
                       int32_t validPosYMin,
                       int32_t validPosYMax);
int32_t TIDL_getParametricSatLimit(float32_tidl clipVal, float32_tidl tensorScale, int32_t zeroPoint);

void TIDL_convertFloatToScaleAndShift(float32_tidl val, int32_t *scale, int32_t *shift, int32_t precisionBits);

void TIDL_GetTensorDimensions(const sTIDL_Network_t *net,
                              const void *workloadUnit,
                              int32_t  layerIdx,
                              int32_t *inWidth,
                              int32_t *inHeight,
                              int32_t *inChannels,
                              int32_t *outWidth,
                              int32_t *outHeight,
                              int32_t *outChannels);


/* Function to derive tensorScale value from net structure for a given layer*/
// TODO: Need to remove this later when value comes from gc
float32_tidl TIDL_getTensorScale(const sTIDL_Network_t *net, int32_t layerIdx);

// TODO: Need to remove this later when value comes from gc
int32_t TIDL_getTensorZeroPoint(const sTIDL_Network_t *net, int32_t layerIdx);


/* Function to derive roundbits value from net structure for a given layer*/
// TODO: Need to remove this later when value comes from gc
int32_t TIDL_getRoundbits(const sTIDL_Network_t *net, int32_t layerIdx);

/* Returns pointer to buffer containing info about the tensor the joint is trying to access */
const sBufParams_t * TIDL_getBufferParams(const sJoint_t *joint, const sGCHelperHandle * gcHelperHandle);

void TIDL_computeCompressionAlgo(uint8_t *bufferPtr, uint64_t bufLength, int32_t elementType, int32_t *retBias,int32_t *retAlg);
int32_t TIDL_set1DCompressionProperties(int32_t bufSize, sTIDL_compressionDimensions* compDim);
int32_t TIDL_compressFlatSW( uint8_t* const pInput,
                            uint8_t* const pOutput,
                            uint8_t* const sectr,
                            uint8_t* const cdbTable,
                            uint8_t* trMem,
                            sTIDL_compressionDimensions* pCompDims
                            );
int32_t TIDL_isConcatBalanced( TIDL_NetworkCommonParams *commonParams, sTIDL_Layer_t *layer, sTIDL_AlgLayer_t *algLayer, int32_t layerIdx);
#endif /*ITIDL_COMMON_UTILS_H */

