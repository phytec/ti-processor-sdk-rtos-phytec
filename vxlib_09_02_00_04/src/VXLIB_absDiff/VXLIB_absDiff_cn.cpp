/******************************************************************************
 * Copyright (C) 2023 Texas Instruments Incorporated - http://www.ti.com/
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************/

#include "VXLIB_absDiff_priv.h"
#include <cstdint>

template <typename dataType> inline dataType VXLIB_absDiff_calc_cn(dataType A, dataType B);

template <> inline uint8_t VXLIB_absDiff_calc_cn<uint8_t>(uint8_t A, uint8_t B)
{
   int16_t diff = (int16_t) A - (int16_t) B;
   uint8_t out  = (diff < 0) ? -diff : diff;
   return out;
}

template <> inline int16_t VXLIB_absDiff_calc_cn<int16_t>(int16_t A, int16_t B)
{
   int32_t diff = A - B;

   diff        = (diff < 0) ? -diff : diff;
   int16_t out = (diff > INT16_MAX) ? INT16_MAX : diff;

   return out;
}

template <typename dataType>
VXLIB_STATUS
VXLIB_absDiff_exec_cn(VXLIB_kernelHandle handle, void *restrict pIn0, void *restrict pIn1, void *restrict pOut)
{
   VXLIB_STATUS status = VXLIB_SUCCESS;

   VXLIB_absDiff_PrivArgs *pKerPrivArgs = (VXLIB_absDiff_PrivArgs *) handle;

   int32_t m = 0;
   int32_t n = 0;

   int32_t height    = pKerPrivArgs->height;
   int32_t width     = pKerPrivArgs->width;
   int32_t strideIn0 = pKerPrivArgs->strideIn0Elements;
   int32_t strideIn1 = pKerPrivArgs->strideIn1Elements;
   int32_t strideOut = pKerPrivArgs->strideOutElements;

#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_absDiff_exec_cn\n");
#endif

#if VXLIB_DEBUGPRINT
   printf("VXLIB_absDiff_exec_cn: height = %d, width = %d\n", height, width);
#endif

   const dataType *restrict A = (const dataType *) pIn0;
   const dataType *restrict B = (const dataType *) pIn1;
   dataType *restrict C       = (dataType *) pOut;

   for (m = 0; m < height; m++) {
      for (n = 0; n < width; n++) {
         C[n + m * strideOut] = VXLIB_absDiff_calc_cn(A[n + m * strideIn0], B[n + m * strideIn1]);
      }
   }

   return (status);
}

template VXLIB_STATUS VXLIB_absDiff_exec_cn<uint8_t>(VXLIB_kernelHandle handle,
                                                     void *restrict pIn0,
                                                     void *restrict pIn1,
                                                     void *restrict pOut);

template VXLIB_STATUS VXLIB_absDiff_exec_cn<int16_t>(VXLIB_kernelHandle handle,
                                                     void *restrict pIn0,
                                                     void *restrict pIn1,
                                                     void *restrict pOut);
