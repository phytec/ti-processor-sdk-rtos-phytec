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

#include "VXLIB_meanStdDev_priv.h"

/**********************************************************************************************************************/
/*                                                                                                                    */
/* DEFINES                                                                                                            */
/*                                                                                                                    */
/**********************************************************************************************************************/

#define VXLIB_MEANSTDDEV_NUMERIC_MIN(x) std::numeric_limits<x>::min()
#define VXLIB_MEANSTDDEV_NUMERIC_MAX(x) std::numeric_limits<x>::max()

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_meanStdDev_exec_cn */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method computes add in via natural C code
template <typename dTypeIn, typename dTypeOut>
VXLIB_STATUS VXLIB_meanStdDev_exec_cn(VXLIB_kernelHandle handle,
                                      void *restrict pIn,
                                      void *restrict pOut0,
                                      void *restrict pOut1,
                                      void *restrict pPixelsProcessed,
                                      void *restrict pCurrentSum,
                                      void *restrict pCurrentSqSum)
{

   VXLIB_STATUS status = VXLIB_SUCCESS; // assign status to success by default

   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_meanStdDev_PrivArgs *pKerPrivArgs = (VXLIB_meanStdDev_PrivArgs *) handle;

   // obtain image parameters and overflow policy
   size_t width            = pKerPrivArgs->width;
   size_t height           = pKerPrivArgs->height;
   size_t strideInElements = pKerPrivArgs->strideInElements;

#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_meanStdDev_exec_cn\n");
#endif

   typedef typename std::conditional<std::is_same<dTypeIn, uint8_t>::value, uint32_t, uint64_t>::type dTypeAcc;

   // create local pointers
   dTypeIn *restrict pInLocal               = (dTypeIn *) pIn;
   dTypeOut *restrict pOut0Local            = (dTypeOut *) pOut0;
   dTypeOut *restrict pOut1Local            = (dTypeOut *) pOut1;
   uint32_t *restrict pPixelsProcessedLocal = (uint32_t *) pPixelsProcessed;
   dTypeAcc *restrict pCurrentSumLocal      = (dTypeAcc *) pCurrentSum;
   dTypeAcc *restrict pCurrentSqSumLocal    = (dTypeAcc *) pCurrentSqSum;

   // variables to hold temporary values
   size_t N = width * height;

#if VXLIB_DEBUGPRINT
   printf("In VXLIB_meanStdDev_exec_cn, width: %d, height: %d\n", width, height);
#endif

   for (uint32_t i = 0; i < height; i++) {
      for (uint32_t j = 0; j < width; j++) {
         *pCurrentSumLocal += (dTypeAcc) pInLocal[i * strideInElements + j];
         *pCurrentSqSumLocal +=
             ((dTypeAcc) pInLocal[i * strideInElements + j] * (dTypeAcc) pInLocal[i * strideInElements + j]);
      }
   }

   // calculate mean
   *pPixelsProcessedLocal += N;

   *pOut0Local = (dTypeOut) (*pCurrentSumLocal) / (*pPixelsProcessedLocal);

   // calculate sqrt
   dTypeOut variance = ((dTypeOut) (*pCurrentSqSumLocal) -
                        ((dTypeOut) (*pCurrentSumLocal) * (*pCurrentSumLocal) / (*pPixelsProcessedLocal))) /
                       (*pPixelsProcessedLocal);

   *pOut1Local = (dTypeOut) sqrt(variance);

   return (status);
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* Explicit instantiation for the different data type versions                                                        */
/*                                                                                                                    */
/**********************************************************************************************************************/

template VXLIB_STATUS VXLIB_meanStdDev_exec_cn<VXLIB_MEANSTDDEV_TYPENAME_I8U_O32F>(VXLIB_kernelHandle handle,
                                                                                   void *restrict pIn,
                                                                                   void *restrict pOut0,
                                                                                   void *restrict pOut1,
                                                                                   void *restrict pPixelsProcessed,
                                                                                   void *restrict pCurrentSum,
                                                                                   void *restrict pCurrentSqSum);

template VXLIB_STATUS VXLIB_meanStdDev_exec_cn<VXLIB_MEANSTDDEV_TYPENAME_I16U_O32F>(VXLIB_kernelHandle handle,
                                                                                    void *restrict pIn,
                                                                                    void *restrict pOut0,
                                                                                    void *restrict pOut1,
                                                                                    void *restrict pPixelsProcessed,
                                                                                    void *restrict pCurrentSum,
                                                                                    void *restrict pCurrentSqSum);
