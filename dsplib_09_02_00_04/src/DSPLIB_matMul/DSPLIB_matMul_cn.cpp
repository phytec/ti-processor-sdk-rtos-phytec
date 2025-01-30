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

#include "DSPLIB_matMul_priv.h"
#include <cstdint>

template <typename dataType>
DSPLIB_STATUS
DSPLIB_matMul_exec_cn(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pIn1, void *restrict pOut)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_matMul_PrivArgs *pKerPrivArgs = (DSPLIB_matMul_PrivArgs *) handle;

   int32_t m = 0;
   int32_t k = 0;
   int32_t n = 0;

   dataType product, sum;

   int32_t M         = pKerPrivArgs->M;
   int32_t K         = pKerPrivArgs->K;
   int32_t N         = pKerPrivArgs->N;
   int32_t strideIn0 = pKerPrivArgs->strideIn0Elements;
   int32_t strideIn1 = pKerPrivArgs->strideIn1Elements;
   int32_t strideOut = pKerPrivArgs->strideOutElements;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_matMul_exec_cn\n");
#endif

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_matMul_exec_cn: M = %d, N = %d, K = %d, strideIn0 = %d, strideIn1 = %d, strideOut = %d\n", M, N, K,
          strideIn0, strideIn1, strideOut);
#endif

   const dataType *restrict A = (const dataType *) pIn0;
   const dataType *restrict B = (const dataType *) pIn1;
   dataType *restrict C       = (dataType *) pOut;

   for (m = 0; m < M; m++) {
      for (n = 0; n < N; n++) {
         sum = 0;
         for (k = 0; k < K; k++) {
            /* #if DSPLIB_DEBUGPRINT */
            /*             printf("%.2f * %.2f\n", A[k + m * strideIn0], B[n + k * strideIn1]); */
            /* #endif */
            product = A[k + m * strideIn0] * B[n + k * strideIn1];
            sum     = sum + product;
         }
         C[n + m * strideOut] = sum;
      }
   }

   return (status);
}

// explicit insmatMultiation for the different data type versions
template DSPLIB_STATUS
DSPLIB_matMul_exec_cn<float>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pIn1, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_matMul_exec_cn<double>(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pIn1, void *restrict pOut);
