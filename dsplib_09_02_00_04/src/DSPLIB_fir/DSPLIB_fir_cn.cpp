/*******************************************************************************
**+--------------------------------------------------------------------------+**
**|                            ****                                          |**
**|                            ****                                          |**
**|                            ******o***                                    |**
**|                      ********_///_****                                   |**
**|                      ***** /_//_/ ****                                   |**
**|                       ** ** (__/ ****                                    |**
**|                           *********                                      |**
**|                            ****                                          |**
**|                            ***                                           |**
**|                                                                          |**
**|         Copyright (c) 2016 Texas Instruments Incorporated                |**
**|                        ALL RIGHTS RESERVED                               |**
**|                                                                          |**
**| Permission to use, copy, modify, or distribute this software,            |**
**| whether in part or in whole, for any purpose is forbidden without        |**
**| a signed licensing agreement and NDA from Texas Instruments              |**
**| Incorporated (TI).                                                       |**
**|                                                                          |**
**| TI makes no representation or warranties with respect to the             |**
**| performance of this computer program, and specifically disclaims         |**
**| any responsibility for any damages, special or consequential,            |**
**| connected with the use of this program.                                  |**
**|                                                                          |**
**+--------------------------------------------------------------------------+**
*******************************************************************************/

#include "DSPLIB_fir_priv.h"

DSPLIB_STATUS DSPLIB_fir_init_cn(DSPLIB_kernelHandle   handle,
                                 DSPLIB_bufParams2D_t *bufParamsIn,
                                 DSPLIB_bufParams1D_t *bufParamsFilter,
                                 DSPLIB_bufParams2D_t *bufParamsOut,
                                 DSPLIB_fir_InitArgs  *pKerInitArgs)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;
   return status;
}

template <typename dataType, typename accumulateType>
DSPLIB_STATUS
DSPLIB_fir_exec_cn(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pFilter, void *restrict pOut)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_fir_exec_cn\n");
#endif

   int32_t              j, k, l;
   dataType             outProd, outTemp;
   dataType            *pInLocal     = (dataType *) pIn;
   dataType            *pFilterLocal = (dataType *) pFilter;
   dataType            *pOutLocal    = (dataType *) pOut;
   DSPLIB_fir_PrivArgs *pKerPrivArgs = (DSPLIB_fir_PrivArgs *) handle;
   //uint32_t             dataSize     = pKerPrivArgs->initArgs.dataSize;
   uint32_t             batchSize    = pKerPrivArgs->initArgs.batchSize;
   uint32_t             filterSize   = pKerPrivArgs->initArgs.filterSize;
   int32_t              outputSize   = pKerPrivArgs->outputSize;
   //uint32_t             shift        = pKerPrivArgs->initArgs.shift;

   uint32_t dataBufferInPitch  = pKerPrivArgs->dataBufferInPitch;
   uint32_t dataBufferOutPitch = pKerPrivArgs->dataBufferOutPitch;

#if DSPLIB_DEBUGPRINT
   printf("Enter pFilter %p\n", pFilterLocal);
#endif
   int32_t dataOffset    = 0;
   int32_t dataOffsetOut = 0;

   for (l = 0; l < batchSize; l++) {
      for (j = 0; j < outputSize; j++) {
         outTemp           = 0;
         dataOffset        = l * dataBufferInPitch + j;
         dataOffsetOut     = l * dataBufferOutPitch + j;
         int32_t filterCtr = 0;
         for (k = filterSize - 1; k > -1; k--) {
            outProd = pInLocal[dataOffset + filterCtr] * pFilterLocal[k];
            outTemp = outTemp + outProd;
#if DSPLIB_DEBUGPRINT
//            printf("DSPLIB_DEBUGPRINT l %d  dataOffset %d filterCtr %d
//            dataBufferInPitch %d dataBufferOutPitch %d\n", l, dataOffset,
//            filterCtr, dataBufferInPitch, dataBufferOutPitch);
//            printf("DSPLIB_DEBUGPRINT pInLocal: %f, pFilterLocal[k]: %f,
//            dataSize: %d outTemp: %f outProd %f j %d k %d\n",
//            pInLocal[dataOffset + filterCtr], pFilterLocal[k], dataSize,
//            outTemp, outProd, j, k);
#endif
            filterCtr++;
         }
         pOutLocal[dataOffsetOut] = outTemp;
#if DSPLIB_DEBUGPRINT
//       printf("DSPLIB_DEBUGPRINT outTemp: %f, filterSize: %d, dataSize: %d
//       batchSize: %d j %d k %d\n", outTemp, filterSize, dataSize, batchSize,
//       j, k);
#endif
      }
   }

   return (status);
}

// explicit instantiation for the different data type versions
template DSPLIB_STATUS DSPLIB_fir_exec_cn<int16_t, int64_t>(DSPLIB_kernelHandle handle,
                                                            void *restrict pIn,
                                                            void *restrict pFilter,
                                                            void *restrict pOut);

template DSPLIB_STATUS DSPLIB_fir_exec_cn<int32_t, DSPLIB_int128_t>(DSPLIB_kernelHandle handle,
                                                                    void *restrict pIn,
                                                                    void *restrict pFilter,
                                                                    void *restrict pOut);

template DSPLIB_STATUS DSPLIB_fir_exec_cn<int8_t, int32_t>(DSPLIB_kernelHandle handle,
                                                           void *restrict pIn,
                                                           void *restrict pFilter,
                                                           void *restrict pOut);
template DSPLIB_STATUS DSPLIB_fir_exec_cn<float, int32_t>(DSPLIB_kernelHandle handle,
                                                          void *restrict pIn,
                                                          void *restrict pFilter,
                                                          void *restrict pOut);
