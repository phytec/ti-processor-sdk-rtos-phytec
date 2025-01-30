/******************************************************************************/
/*!
 * \file FFTLIB_FFT_fft_ixX_cxX_oxX.c
 */
/* Copyright (C) 2017 Texas Instruments Incorporated - http://www.ti.com/
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


/*******************************************************************************
 *
 * INCLUDES
 *
 ******************************************************************************/

#include "../FFTLIB_FFT_fft_ixX_cxX_oxX_priv.h"
#include "../../../common/c71/FFTLIB_inlines.h"


/*******************************************************************************
 *
 * DEFINES
 *
 ******************************************************************************/
template <uint32_t dataType>
FFTLIB_STATUS FFTLIB_FFT_fft_ixX_cxX_oxX_getSizes(
   const FFTLIB_FFT_fft_ixX_cxX_oxX_InitArgs * pKerInitArgs,
   FFTLIB_bufParams1D_t * bufParamsX, 
   FFTLIB_bufParams1D_t * bufParamsW, 
   FFTLIB_bufParams1D_t * bufParamsY) 
{
   FFTLIB_STATUS    status = FFTLIB_SUCCESS;
   int32_t MMA_SIZE;
   int32_t dftSmall, dftLarge, highRadixDecompositions;

#if FFTLIB_DEBUGPRINT
   printf("In FFTLIB_FFT_fft_ixX_cxX_oxX_getSizes()..\n");
#endif
   if (dataType == FFTLIB_INT32) {
      MMA_SIZE = FFTLIB_MMA_SIZE_32_BIT;
   } else if (dataType == FFTLIB_INT16) {
      MMA_SIZE = FFTLIB_MMA_SIZE_16_BIT;
   } else {
      status = FFTLIB_ERR_NOT_IMPLEMENTED;
   }
   
   if (status == FFTLIB_SUCCESS) {
      FFTLIB_FFT_fft_ixX_cxX_oxX_determineKernelVariant(pKerInitArgs,
                                                        MMA_SIZE,
                                                        &dftSmall,
                                                        &dftLarge,
                                                        &highRadixDecompositions);
   
      if (dftSmall) {
   #if FFTLIB_DEBUGPRINT
         printf("About to call FFTLIB_FFT_dftSmall_ixX_cxX_oxX_getSizes()..\n");
   #endif
         status = FFTLIB_FFT_dftSmall_ixX_cxX_oxX_getSizes<dataType>(
                     (const FFTLIB_FFT_dftSmall_ixX_cxX_oxX_InitArgs *)pKerInitArgs,
                     bufParamsX,
                     bufParamsW,
                     bufParamsY);
      }
   
      if (dftLarge) {
   #if FFTLIB_DEBUGPRINT
         printf("About to call FFTLIB_FFT_dftLarge_ixX_cxX_oxX_getSizes()..\n");
   #endif
         status = FFTLIB_FFT_dftLarge_ixX_cxX_oxX_getSizes<dataType>(
                     (const FFTLIB_FFT_dftLarge_ixX_cxX_oxX_InitArgs *)pKerInitArgs,
                     bufParamsX,
                     bufParamsW,
                     bufParamsY);
      }
   
      if (highRadixDecompositions) {
   #if FFTLIB_DEBUGPRINT
         printf("About to call FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_getSizes()..\n");
   #endif
         status = FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_getSizes<dataType>(
                     (const FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_InitArgs *)pKerInitArgs,
                     bufParamsX,
                     bufParamsW,
                     bufParamsY);
      }
   }

   return status;
}

template FFTLIB_STATUS FFTLIB_FFT_fft_ixX_cxX_oxX_getSizes<
   FFTLIB_INT16>(
   const FFTLIB_FFT_fft_ixX_cxX_oxX_InitArgs * pKerInitArgs,
   FFTLIB_bufParams1D_t * bufParamsX, 
   FFTLIB_bufParams1D_t * bufParamsW, 
   FFTLIB_bufParams1D_t * bufParamsY);
template FFTLIB_STATUS FFTLIB_FFT_fft_ixX_cxX_oxX_getSizes<
   FFTLIB_INT32>(
   const FFTLIB_FFT_fft_ixX_cxX_oxX_InitArgs * pKerInitArgs,
   FFTLIB_bufParams1D_t * bufParamsX, 
   FFTLIB_bufParams1D_t * bufParamsW, 
   FFTLIB_bufParams1D_t * bufParamsY);

template <typename dataType>
FFTLIB_STATUS FFTLIB_FFT_fft_ixX_cxX_oxX_twGen(
   const FFTLIB_FFT_fft_ixX_cxX_oxX_InitArgs * pKerInitArgs,
   void *restrict pW,
   const FFTLIB_bufParams1D_t * bufParamsW) 
{
   FFTLIB_STATUS    status = FFTLIB_SUCCESS;
   int32_t MMA_SIZE;
   int32_t dftSmall, dftLarge, highRadixDecompositions;

#if FFTLIB_DEBUGPRINT
   printf("In FFTLIB_FFT_fft_ixX_cxX_oxX_twGen()..\n");
#endif
   if (sizeof(dataType) == 4) {
      MMA_SIZE = FFTLIB_MMA_SIZE_32_BIT;
   } else if (sizeof(dataType) == 2) {
      MMA_SIZE = FFTLIB_MMA_SIZE_16_BIT;
   } else {
      status = FFTLIB_ERR_NOT_IMPLEMENTED;
   }

   if (status == FFTLIB_SUCCESS) {
      FFTLIB_FFT_fft_ixX_cxX_oxX_determineKernelVariant(pKerInitArgs,
                                                        MMA_SIZE,
                                                        &dftSmall,
                                                        &dftLarge,
                                                        &highRadixDecompositions);
   
      if (dftSmall) {
   #if FFTLIB_DEBUGPRINT
         printf("About to call FFTLIB_FFT_dftSmall_ixX_cxX_oxX_twGen()..\n");
   #endif
         status = FFTLIB_FFT_dftSmall_ixX_cxX_oxX_twGen<dataType>(
                     (const FFTLIB_FFT_dftSmall_ixX_cxX_oxX_InitArgs *)pKerInitArgs,
                     pW,
                     bufParamsW);
      }
   
      if (dftLarge) {
   #if FFTLIB_DEBUGPRINT
         printf("About to call FFTLIB_FFT_dftLarge_ixX_cxX_oxX_twGen()..\n");
   #endif
         status = FFTLIB_FFT_dftLarge_ixX_cxX_oxX_twGen<dataType>(
                     (const FFTLIB_FFT_dftLarge_ixX_cxX_oxX_InitArgs *)pKerInitArgs,
                     pW,
                     bufParamsW);
      }
   
      if (highRadixDecompositions) {
   #if FFTLIB_DEBUGPRINT
         printf("About to call FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_twGen()..\n");
   #endif
         status = FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_twGen<dataType>(
                     (const FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_InitArgs *)pKerInitArgs,
                     pW,
                     bufParamsW);
      }
   }
   return status;
}

template FFTLIB_STATUS FFTLIB_FFT_fft_ixX_cxX_oxX_twGen<int16_t>(
   const FFTLIB_FFT_fft_ixX_cxX_oxX_InitArgs * pKerInitArgs,
   void *restrict pW,
   const FFTLIB_bufParams1D_t * bufParamsW);

template FFTLIB_STATUS FFTLIB_FFT_fft_ixX_cxX_oxX_twGen<int32_t>(
   const FFTLIB_FFT_fft_ixX_cxX_oxX_InitArgs * pKerInitArgs,
   void *restrict pW,
   const FFTLIB_bufParams1D_t * bufParamsW);

