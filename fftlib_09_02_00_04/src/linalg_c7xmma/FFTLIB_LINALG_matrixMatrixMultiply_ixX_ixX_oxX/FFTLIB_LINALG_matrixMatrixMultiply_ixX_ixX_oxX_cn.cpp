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


#include "FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_priv.h"

FFTLIB_STATUS FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_init_cn(
                                                                     FFTLIB_kernelHandle handle,
                                                                     const FFTLIB_bufParams2D_t *src0_addr,
                                                                     const FFTLIB_bufParams2D_t *src1_addr,
                                                                     const FFTLIB_bufParams2D_t *dst_addr,
                                                                     const FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_InitArgs *pKerInitArgs)
{
   FFTLIB_STATUS status = FFTLIB_SUCCESS;
   
   // store parameters
   FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_PrivArgs *pKerPrivArgs =
   (FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_PrivArgs*) handle;
   pKerPrivArgs->M = src0_addr->dim_y;
   pKerPrivArgs->K = src0_addr->dim_x;
   pKerPrivArgs->N = src1_addr->dim_x;
   
   return status;
}

template <typename dataType, typename accumulateType>
FFTLIB_STATUS FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_exec_cn(FFTLIB_kernelHandle handle,
                                                                     const void * src0,
                                                                     const void * src1,
                                                                     void *dst)
{
   FFTLIB_STATUS    status = FFTLIB_SUCCESS;
   int32_t m, k, n;
   accumulateType product, sum;
   
   FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_PrivArgs *pKerPrivArgs = (FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_PrivArgs *)handle;
   
   int32_t M = pKerPrivArgs->M;
   int32_t K = pKerPrivArgs->K;
   int32_t N = pKerPrivArgs->N;
   int32_t strideA = pKerPrivArgs->strideAElements;
   int32_t strideB = pKerPrivArgs->strideBElements;
   int32_t strideC = pKerPrivArgs->strideCElements;
   
   int8_t shift = pKerPrivArgs->initArgs.shift;
   
   
   const dataType *A = (const dataType *)src0;
   const dataType *B = (const dataType *)src1;
   dataType *C = (dataType *)dst;
   
   for (m = 0; m < M; m++) {
      for (n = 0; n < N; n++) {
         sum = 0;
         for (k = 0; k < K; k++){
            product = FFTLIB_UTIL_mult(A[k + m*strideA], B[n + k*strideB]);
            sum = sum + product;
         }
         C[n + m*strideC] = FFTLIB_UTIL_shiftAndRound<accumulateType, dataType>(sum, shift);
         
#if defined(FFTLIB_INDUCE_NATURALC_ERROR)
         C[n + m*StrideC] = 77;
#endif
      }
   }
   return (status);
}

// explicit instantiation for the different data type versions
template FFTLIB_STATUS FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_exec_cn<int8_t, int32_t> (FFTLIB_kernelHandle handle,
                                                                                  const void * src0,
                                                                                  const void * src1,
                                                                                  void *dst);

template FFTLIB_STATUS FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_exec_cn<int16_t, int64_t> (FFTLIB_kernelHandle handle,
                                                                                   const void * src0,
                                                                                   const void * src1,
                                                                                   void *dst);

template FFTLIB_STATUS FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_exec_cn<int32_t, FFTLIB_int128_t> (FFTLIB_kernelHandle handle,
                                                                                           const void * src0,
                                                                                           const void * src1,
                                                                                           void *dst);
