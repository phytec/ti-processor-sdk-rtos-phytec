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

#include "VXLIB_convertDepth_priv.h"
#include <cstdint>

template <typename dTypeIn0, typename dTypeOut>
inline dTypeOut VXLIB_convertDepth_wrap_cn(dTypeIn0 A, dTypeOut C, uint8_t shift);

inline uint8_t VXLIB_convertDepth_sat_cn(int16_t A, uint8_t shift)
{
   A = (int16_t)((uint32_t)A >> (uint32_t)shift);

   A = A < 0 ? 0 : A;
   A = A > 255 ? 255 : A;

   uint8_t res = A;
   return res;
}

template <> inline uint8_t VXLIB_convertDepth_wrap_cn<int16_t, uint8_t>(int16_t A, uint8_t C, uint8_t shift)
{
   A = (int16_t)((uint32_t)A >> (uint32_t)shift);

   uint8_t res = A;
   return res;
}

template <> inline int16_t VXLIB_convertDepth_wrap_cn<uint8_t, int16_t>(uint8_t A, int16_t C, uint8_t shift)
{
   int16_t res = A;

   res = (int16_t)((uint32_t)res << (uint32_t)shift);
   return res;
}

template <typename dTypeIn0, typename dTypeOut>
VXLIB_STATUS VXLIB_convertDepth_exec_cn(VXLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut)
{
   VXLIB_STATUS status = VXLIB_SUCCESS;

   VXLIB_convertDepth_PrivArgs *pKerPrivArgs = (VXLIB_convertDepth_PrivArgs *) handle;

   int32_t m = 0;
   int32_t n = 0;

   int32_t height    = pKerPrivArgs->height;
   int32_t width     = pKerPrivArgs->width;
   int32_t strideIn0 = pKerPrivArgs->strideIn0Elements;
   int32_t strideOut = pKerPrivArgs->strideOutElements;
   uint8_t shift     = pKerPrivArgs->shift;

#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_convertDepth_exec_cn\n");
#endif

#if VXLIB_DEBUGPRINT
   printf("VXLIB_convertDepth_exec_cn: height = %d, width = %d\n", height, width);
#endif

   const dTypeIn0 *restrict A = (const dTypeIn0 *) pIn0;
   dTypeOut *restrict C       = (dTypeOut *) pOut;

   if (pKerPrivArgs->overFlowPolicy == VXLIB_SATURATE) {
      for (m = 0; m < height; m++) {
         for (n = 0; n < width; n++) {
            C[n + m * strideOut] = VXLIB_convertDepth_sat_cn(A[n + m * strideIn0], shift);
         }
      }
   }
   else {
      for (m = 0; m < height; m++) {
         for (n = 0; n < width; n++) {
            C[n + m * strideOut] = VXLIB_convertDepth_wrap_cn(A[n + m * strideIn0], C[n + m * strideOut], shift);
         }
      }
   }

   return status;
}

template VXLIB_STATUS
VXLIB_convertDepth_exec_cn<int16_t, uint8_t>(VXLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template VXLIB_STATUS
VXLIB_convertDepth_exec_cn<uint8_t, int16_t>(VXLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);
