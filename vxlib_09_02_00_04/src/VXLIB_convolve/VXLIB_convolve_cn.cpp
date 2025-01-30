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

/**********************************************************************************************************************/
/*                                                                                                                    */
/* INCLUDES                                                                                                           */
/*                                                                                                                    */
/**********************************************************************************************************************/

#include "VXLIB_convolve_priv.h"
#include "VXLIB_types.h"

/**********************************************************************************************************************/
/*                                                                                                                    */
/* DEFINES                                                                                                            */
/*                                                                                                                    */
/**********************************************************************************************************************/

#define VXLIB_CONVOLVE_NUMERIC_MIN(x) std::numeric_limits<x>::min()
#define VXLIB_CONVOLVE_NUMERIC_MAX(x) std::numeric_limits<x>::max()

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_convolve_exec_cn */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method computes convolution of an input image with a filterHeight x filterWidth filter via natural C code
template <typename dTypeIn, typename dTypeFilter, typename dTypeOut>
VXLIB_STATUS
VXLIB_convolve_exec_cn(VXLIB_kernelHandle handle, void *restrict pIn, void *restrict pFilter, void *restrict pOut)
{

   VXLIB_STATUS status = VXLIB_SUCCESS; // assign status to success by default

#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_convolve_exec_cn\n");
#endif

   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_convolve_PrivArgs *pKerPrivArgs = (VXLIB_convolve_PrivArgs *) handle;

   // obtain image parameters and overflow policy
   size_t width             = pKerPrivArgs->width;
   size_t height            = pKerPrivArgs->height;
   size_t strideInElements  = pKerPrivArgs->strideInElements;
   size_t strideOutElements = pKerPrivArgs->strideOutElements;

   size_t filterHeight = pKerPrivArgs->pKerInitArgs.filterHeight;
   size_t filterWidth  = pKerPrivArgs->pKerInitArgs.filterWidth;
   size_t scale        = pKerPrivArgs->pKerInitArgs.scale;

   size_t padLeft   = pKerPrivArgs->pKerInitArgs.padLeft;
   size_t padRight  = pKerPrivArgs->pKerInitArgs.padRight;
   size_t padTop    = pKerPrivArgs->pKerInitArgs.padTop;
   size_t padBottom = pKerPrivArgs->pKerInitArgs.padBottom;

   bool isNotPadded = (padLeft == 0) && (padRight == 0) && (padTop == 0) && (padBottom == 0);

   // create local pointers
   dTypeIn *restrict pInLocal         = (dTypeIn *) pIn;
   dTypeFilter *restrict pFilterLocal = (dTypeFilter *) pFilter;
   dTypeOut *restrict pOutLocal       = (dTypeOut *) pOut;

#if VXLIB_DEBUGPRINT
   printf("In VXLIB_convolve_exec_cn, width: %d, height: %d\n", width, height);
#endif

   // non-padded natc implementation
   if (isNotPadded) {

      int32_t outHeight = (height < filterHeight) ? height : (height - filterHeight + 1);
      int32_t outWidth  = (width < filterWidth) ? width : (width - filterWidth + 1);
      int32_t i, j, m, n;
      int32_t sum, result;

      int32_t MAX = VXLIB_CONVOLVE_NUMERIC_MAX(dTypeOut);
      int32_t MIN = VXLIB_CONVOLVE_NUMERIC_MIN(dTypeOut);

      for (i = 0; i < outHeight; i++) {
         for (j = 0; j < outWidth; j++) {

            sum = 0;

            for (m = 0; m < filterHeight; m++) {
               for (n = 0; n < filterWidth; n++) {

                  if ((i + m < height) && (j + n < width)) {
                     sum += (pInLocal[(i + m) * strideInElements + (j + n)]) *
                            pFilterLocal[(filterHeight - 1 - m) * filterWidth + (filterWidth - 1 - n)];
                  }
               }
            }

            result = (int32_t)((uint64_t)sum >> (uint64_t)scale);

            // saturate the scaled result
            result = (result > MAX) ? MAX : result;
            result = (result < MIN) ? MIN : result;

            pOutLocal[i * strideOutElements + j] = result;
         }
      }
   }

   // padded natc implementation
   else {

      status = VXLIB_ERR_CONVOLVE_PADDED_NOT_IMPLEMENTED;
   }

   return (status);
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* Explicit instantiation for the different data type versions */
/*                                                                                                                    */
/**********************************************************************************************************************/

template VXLIB_STATUS VXLIB_convolve_exec_cn<VXLIB_CONVOLVE_TYPENAME_I8U_C16S_O8U>(VXLIB_kernelHandle handle,
                                                                                   void *restrict pIn,
                                                                                   void *restrict pFilter,
                                                                                   void *restrict pOut);

template VXLIB_STATUS VXLIB_convolve_exec_cn<VXLIB_CONVOLVE_TYPENAME_I8U_C16S_O16S>(VXLIB_kernelHandle handle,
                                                                                    void *restrict pIn,
                                                                                    void *restrict pFilter,
                                                                                    void *restrict pOut);
