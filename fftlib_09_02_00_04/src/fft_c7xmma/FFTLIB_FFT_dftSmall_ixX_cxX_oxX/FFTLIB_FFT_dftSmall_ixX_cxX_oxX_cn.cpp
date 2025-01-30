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


#include "FFTLIB_FFT_dftSmall_ixX_cxX_oxX_priv.h"

FFTLIB_STATUS FFTLIB_FFT_dftSmall_ixX_cxX_oxX_init_cn(
   FFTLIB_kernelHandle handle,
   FFTLIB_bufParams1D_t *bufParamsX,
   FFTLIB_bufParams1D_t *bufParamsW,
   FFTLIB_bufParams1D_t *bufParamsY,
   FFTLIB_FFT_dftSmall_ixX_cxX_oxX_InitArgs *pKerInitArgs)
{
   FFTLIB_STATUS status = FFTLIB_SUCCESS;
   
   return status;
}

template <typename dataType, typename accumulateType, uint32_t MMA_SIZE>
FFTLIB_STATUS FFTLIB_FFT_dftSmall_ixX_cxX_oxX_exec_cn(
   FFTLIB_kernelHandle handle,
   const void *restrict pX,
   const void *restrict pW,
   void *restrict pY)
{
    FFTLIB_STATUS    status = FFTLIB_SUCCESS;
    uint32_t j, k, l;
    accumulateType yProd, yTemp;
    const dataType *pXLocal = (const dataType *)pX;
    const dataType *pWLocal = (const dataType *)pW;
    dataType *pYLocal = (dataType *)pY;
    FFTLIB_FFT_dftSmall_ixX_cxX_oxX_PrivArgs *pKerPrivArgs = 
        (FFTLIB_FFT_dftSmall_ixX_cxX_oxX_PrivArgs *)handle;
    uint32_t fftSize = pKerPrivArgs->initArgs.fftSize;
    uint32_t batchSize = pKerPrivArgs->initArgs.batchSize;
    uint32_t shift = pKerPrivArgs->initArgs.shiftVector[0];

    for (l = 0; l < batchSize; l++) {
        for (j = 0; j < fftSize*2; j++) {
            yTemp = 0;
            for (k = 0; k < fftSize*2; k++) {
                yProd = FFTLIB_UTIL_mult(pXLocal[l*fftSize*2+k], pWLocal[MMA_SIZE*k+j]);
                yTemp = yTemp + yProd;
            }
            pYLocal[l*fftSize*2+j] = FFTLIB_UTIL_shiftAndRound<accumulateType, dataType>(yTemp, 
                                                                                         shift);
        }
    }
    return (status);
}

// explicit instantiation for the different data type versions
/* Check the template parameter values here */
template FFTLIB_STATUS FFTLIB_FFT_dftSmall_ixX_cxX_oxX_exec_cn<int16_t, int64_t, FFTLIB_MMA_SIZE_16_BIT>(
   FFTLIB_kernelHandle handle,
   const void *restrict pX,
   const void *restrict pW,
   void *restrict pY);

/* Check the template parameter values here */
template FFTLIB_STATUS FFTLIB_FFT_dftSmall_ixX_cxX_oxX_exec_cn<int32_t, FFTLIB_int128_t, FFTLIB_MMA_SIZE_32_BIT> (
   FFTLIB_kernelHandle handle,
   const void *restrict pX,
   const void *restrict pW,
   void *restrict pY);
