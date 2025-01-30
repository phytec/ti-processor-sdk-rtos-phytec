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

#include "VXLIB_median_priv.h"

/********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_median_exec_cn */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method computes median in via natural C code
template <typename dType>
VXLIB_STATUS VXLIB_median_exec_cn(VXLIB_kernelHandle handle,
                                  void *restrict     pIn,
                                  void *restrict     pMask,
                                  void *restrict     pOut,
                                  void *restrict     pScratch)
{

   VXLIB_STATUS status = VXLIB_SUCCESS; // assign status to success by default

#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_median_exec_cn\n");
#endif

   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_median_PrivArgs *pKerPrivArgs = (VXLIB_median_PrivArgs *) handle;

   // obtain image parameters and overflow policy
   size_t widthOut   = pKerPrivArgs->widthOut;
   size_t heightOut  = pKerPrivArgs->heightOut;
   bool   kernelType = pKerPrivArgs->pKerInitArgs.kernelType;
   size_t M          = pKerPrivArgs->M;
   size_t N          = pKerPrivArgs->N;

   size_t strideInElements   = pKerPrivArgs->strideInElements;
   size_t strideMaskElements = pKerPrivArgs->strideMaskElements;
   size_t strideOutElements  = pKerPrivArgs->strideOutElements;

   // create local pointers
   dType *restrict pInLocal   = (dType *) pIn;
   dType *restrict pMaskLocal = (dType *) pMask;
   dType *restrict pOutLocal  = (dType *) pOut;

#if VXLIB_DEBUGPRINT
   printf("In VXLIB_median_exec_cn, widthOut: %ld, heightOut: %ld\n", widthOut, heightOut);
#endif

   if (!kernelType) {
      size_t x, y, j, k;
      dType  inWindow[9];
      for (y = 0; y < heightOut; y++) {
         for (x = 0; x < widthOut; x++) {

            /* Extract 3x3 window into stack array */
            for (j = 0; j < 3; j++) {
               for (k = 0; k < 3; k++) {
                  inWindow[j * 3 + k] = pInLocal[(y + j) * strideInElements + (x + k)];
               }
            }

            /* Sort the values in window from largest to smallest */
            for (j = 0; j < 8; j++) {
               for (k = 0; k < 8; k++) {
                  if (inWindow[k] < inWindow[k + 1]) {
                     dType tmp       = inWindow[k];
                     inWindow[k]     = inWindow[k + 1];
                     inWindow[k + 1] = tmp;
                  }
               }
            }

            /* Median is the middle value */
            pOutLocal[y * strideOutElements + x] = inWindow[4];
         }
      }
   }
   else {
      size_t x, y, j, k, cnt;
      dType  inWindow[9 * 9];
      for (y = 0; y < heightOut; y++) {
         for (x = 0; x < widthOut; x++) {

            cnt = 0;

            /* Extract MxN window into stack array */
            for (j = 0; j < M; j++) {
               for (k = 0; k < N; k++) {
                  if (pMaskLocal[j * strideMaskElements + k]) {
                     inWindow[cnt] = pInLocal[(y + j) * strideInElements + (x + k)];
                     cnt++;
                  }
               }
            }

            if (cnt > 0) {
               /* Sort the values in window from largest to smallest */
               for (j = 0; j < (cnt - 1); j++) {
                  for (k = 0; k < (cnt - 1); k++) {
                     if (inWindow[k] < inWindow[k + 1]) {
                        dType tmp       = inWindow[k];
                        inWindow[k]     = inWindow[k + 1];
                        inWindow[k + 1] = tmp;
                     }
                  }
               }

               /* Median is the middle value */
               pOutLocal[y * strideOutElements + x] = inWindow[cnt / 2];
            }
            else {
               status = VXLIB_ERR_FAILURE;
               VXLIB_DEBUGPRINTFN(0, "ERROR CN: True Count = %lu. Cannot execute kernel\n", cnt);
            }
         }
      }
   }

   return (status);
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* Explicit instantiation for the different data type versions                                                        */
/*                                                                                                                    */
/**********************************************************************************************************************/

template VXLIB_STATUS VXLIB_median_exec_cn<VXLIB_MEDIAN_TYPENAME_8U>(VXLIB_kernelHandle handle,
                                                                     void *restrict     pIn,
                                                                     void *restrict     pMask,
                                                                     void *restrict     pOut,
                                                                     void *restrict     pScratch);

template VXLIB_STATUS VXLIB_median_exec_cn<VXLIB_MEDIAN_TYPENAME_8S>(VXLIB_kernelHandle handle,
                                                                     void *restrict     pIn,
                                                                     void *restrict     pMask,
                                                                     void *restrict     pOut,
                                                                     void *restrict     pScratch);

template VXLIB_STATUS VXLIB_median_exec_cn<VXLIB_MEDIAN_TYPENAME_16U>(VXLIB_kernelHandle handle,
                                                                      void *restrict     pIn,
                                                                      void *restrict     pMask,
                                                                      void *restrict     pOut,
                                                                      void *restrict     pScratch);

template VXLIB_STATUS VXLIB_median_exec_cn<VXLIB_MEDIAN_TYPENAME_16S>(VXLIB_kernelHandle handle,
                                                                      void *restrict     pIn,
                                                                      void *restrict     pMask,
                                                                      void *restrict     pOut,
                                                                      void *restrict     pScratch);

// template VXLIB_STATUS VXLIB_median_exec_cn<VXLIB_MEDIAN_TYPENAME_64S>(VXLIB_kernelHandle handle,
//                                                                       void *restrict pIn,
//                                                                       void *restrict pMask,
//                                                                       void *restrict pOut,
//                                                                       void *restrict pScratch);

/* ======================================================================== */
/*  End of file:  VXLIB_median_cn.cpp                                     */
/* ======================================================================== */
