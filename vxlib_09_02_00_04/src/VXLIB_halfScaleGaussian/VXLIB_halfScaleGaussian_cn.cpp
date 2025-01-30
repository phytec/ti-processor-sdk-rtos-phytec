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

#include "VXLIB_halfScaleGaussian_priv.h"

/**********************************************************************************************************************/
/*                                                                                                                    */
/* DEFINES                                                                                                            */
/*                                                                                                                    */
/**********************************************************************************************************************/

#define VXLIB_HALF_SCALE_GAUSSIAN_NUMERIC_MIN(x) std::numeric_limits<x>::min()
#define VXLIB_HALF_SCALE_GAUSSIAN_NUMERIC_MAX(x) std::numeric_limits<x>::max()

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_halfScaleGaussian_exec_cn */
/*                                                                                                                    */
/**********************************************************************************************************************/
// this method computes 5x5 half scale gaussian filter in via natural C code
template <typename dTypeIn, typename dTypeOut>
VXLIB_STATUS VXLIB_halfScaleGaussian_exec_cn(VXLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{

   VXLIB_STATUS status = VXLIB_SUCCESS; // assign status to success by default

#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_halfScaleGaussian_exec_cn\n");
#endif

   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_halfScaleGaussian_PrivArgs *pKerPrivArgs = (VXLIB_halfScaleGaussian_PrivArgs *) handle;

   // obtain image parameters
   size_t widthOut          = pKerPrivArgs->widthOut;
   size_t heightOut         = pKerPrivArgs->heightOut;
   size_t strideInElements  = pKerPrivArgs->strideInElements;
   size_t strideOutElements = pKerPrivArgs->strideOutElements;

   size_t padLeft     = pKerPrivArgs->pKerInitArgs.padLeft;
   size_t padRight    = pKerPrivArgs->pKerInitArgs.padRight;
   size_t padTop      = pKerPrivArgs->pKerInitArgs.padTop;
   size_t padBottom   = pKerPrivArgs->pKerInitArgs.padBottom;
   bool   isNotPadded = (padLeft == 0) && (padRight == 0) && (padTop == 0) && (padBottom == 0);

   size_t filterDim = pKerPrivArgs->pKerInitArgs.filterSize;

   // create local pointers
   dTypeIn *restrict pInLocal   = (dTypeIn *) pIn;
   dTypeOut *restrict pOutLocal = (dTypeOut *) pOut;

#if VXLIB_DEBUGPRINT
   printf("In VXLIB_halfScaleGaussian_exec_cn, width: %d, height: %d\n", width, height);
#endif

   // 5x5 implementation
   if (filterDim == VXLIB_HALF_SCALE_GAUSSIAN_FILTER_5x5) {
      // non-padded natc implementation
      if (isNotPadded) {

         int32_t x, y, i, j;
         int32_t sum;

         int16_t gaussianFilter5x5[5][5] = {
             {1, 4, 6, 4, 1}, {4, 16, 24, 16, 4}, {6, 24, 36, 24, 6}, {4, 16, 24, 16, 4}, {1, 4, 6, 4, 1}};

         for (y = 0; y < heightOut; y++) {
            for (x = 0; x < widthOut; x++) {

               sum = 0;

               for (j = 0; j < filterDim; j++) {
                  for (i = 0; i < filterDim; i++) {
                     sum += (pInLocal[((y * 2) + j) * strideInElements + ((x * 2) + i)]) * gaussianFilter5x5[j][i];
                  }
               }
               pOutLocal[y * strideOutElements + x] = (dTypeOut)((uint64_t)sum >> 8LLU);
            }
         }
      }
      // padded natc implementation
      else {
         status = VXLIB_ERR_NOT_IMPLEMENTED;
      }
   }
   // other filter dimensions
   else {
      status = VXLIB_ERR_NOT_IMPLEMENTED;
   }

   return (status);
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* Explicit instantiation for the different data type versions */
/*                                                                                                                    */
/**********************************************************************************************************************/

template VXLIB_STATUS
VXLIB_halfScaleGaussian_exec_cn<VXLIB_HALF_SCALE_GAUSSIAN_TYPENAME_I8U_O8U>(VXLIB_kernelHandle handle,
                                                                            void *restrict pIn,
                                                                            void *restrict pOut);
