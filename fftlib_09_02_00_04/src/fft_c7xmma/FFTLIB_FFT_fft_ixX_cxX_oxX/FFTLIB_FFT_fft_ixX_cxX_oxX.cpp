/******************************************************************************
*                                                                             *
* module name       :FFTLIB                                                   *
*                                                                             *
* module descripton :Matrix Multiply Accelerator Library module for C7x+MMA   *
*                                                                             *
* Copyright (C) 2017-2018 Texas Instruments Incorporated - http://www.ti.com/ *
* ALL RIGHTS RESERVED                                                         *
*                                                                             *
******************************************************************************/

/**
******************************************************************************
*  @file     FFTLIB_FFT_fft_ixX_cxX_oxX.c
*
*  @brief    DFT computation
*
*  @version  0.1 - Jan 2018 : Initial Version
*            
*******************************************************************************
*/

#include "FFTLIB_FFT_fft_ixX_cxX_oxX_priv.h"

int32_t FFTLIB_FFT_fft_ixX_cxX_oxX_getHandleSize(
   FFTLIB_FFT_fft_ixX_cxX_oxX_InitArgs *pKerInitArgs)
{
  int32_t privBufSize = sizeof(FFTLIB_FFT_fft_ixX_cxX_oxX_PrivArgs);
  return privBufSize ;
}

void FFTLIB_FFT_fft_ixX_cxX_oxX_determineKernelVariant(
   const FFTLIB_FFT_fft_ixX_cxX_oxX_InitArgs *pKerInitArgs,
   int32_t MMA_SIZE,
   int32_t *dftSmall,
   int32_t *dftLarge,
   int32_t *highRadixDecompositions)
{
   int32_t fftSize = pKerInitArgs->fftSize;
   int32_t batchSize = pKerInitArgs->batchSize;
   uint32_t interleave = pKerInitArgs->interleave;

   *dftSmall = 0;
   *dftLarge = 0;
   *highRadixDecompositions = 0;
   if (fftSize <= MMA_SIZE/2) {
      *dftSmall = 1;
   } else if (fftSize == MMA_SIZE) {
      *dftLarge = 1;
   } else if (fftSize == MMA_SIZE*2) {
      if (batchSize < MMA_SIZE/2) {
         *highRadixDecompositions = 1;
      } else if (batchSize == MMA_SIZE/2) {
         if (interleave) {
            *highRadixDecompositions = 1;
         } else {
            *dftLarge = 1;
         }
      } else {
         *dftLarge = 1;
      }
   } else {
      *highRadixDecompositions = 1;
   }
}


FFTLIB_STATUS FFTLIB_FFT_fft_ixX_cxX_oxX_init_checkParams(
   FFTLIB_kernelHandle handle,
   const FFTLIB_bufParams1D_t * bufParamsX,
   const FFTLIB_bufParams1D_t * bufParamsW,
   const FFTLIB_bufParams1D_t * bufParamsY,
   const FFTLIB_FFT_fft_ixX_cxX_oxX_InitArgs *pKerInitArgs)
{
   FFTLIB_STATUS status  = FFTLIB_SUCCESS;
   FFTLIB_FFT_fft_ixX_cxX_oxX_PrivArgs *pKerPrivArgs;
   int32_t MMA_SIZE;
   int32_t dftSmall, dftLarge, highRadixDecompositions;

#if FFTLIB_DEBUGPRINT
   printf("In FFTLIB_FFT_fft_ixX_cxX_oxX_init_checkParams()..\n");
#endif
   if(handle == NULL) {
      status = FFTLIB_ERR_NULL_POINTER;
   } else {
      pKerPrivArgs = (FFTLIB_FFT_fft_ixX_cxX_oxX_PrivArgs *)handle;

      if (bufParamsX->data_type == FFTLIB_INT16) {
         MMA_SIZE = FFTLIB_MMA_SIZE_16_BIT;
      } else if (bufParamsX->data_type == FFTLIB_INT32) {
         MMA_SIZE = FFTLIB_MMA_SIZE_32_BIT;
      } else {
         status = FFTLIB_ERR_NOT_IMPLEMENTED;
      }
   }

   if (status == FFTLIB_SUCCESS) {
      FFTLIB_FFT_fft_ixX_cxX_oxX_determineKernelVariant(
         pKerInitArgs,
         MMA_SIZE,
         &dftSmall,
         &dftLarge,
         &highRadixDecompositions);
   
      if (dftSmall) {
   #if FFTLIB_DEBUGPRINT
         printf("About to call FFTLIB_FFT_dftSmall_ixX_cxX_oxX_init_checkParams()..\n");
   #endif
         status = FFTLIB_FFT_dftSmall_ixX_cxX_oxX_init_checkParams(
                     (FFTLIB_kernelHandle)&pKerPrivArgs->privArgs.dftSmall,
                     bufParamsX,
                     bufParamsW,
                     bufParamsY,
                     (const FFTLIB_FFT_dftSmall_ixX_cxX_oxX_InitArgs *)pKerInitArgs);
      }
   
      if (dftLarge) {
   #if FFTLIB_DEBUGPRINT
         printf("About to call FFTLIB_FFT_dftLarge_ixX_cxX_oxX_init_checkParams()..\n");
   #endif
         status = FFTLIB_FFT_dftLarge_ixX_cxX_oxX_init_checkParams(
                     (FFTLIB_kernelHandle)&pKerPrivArgs->privArgs.dftLarge,
                     bufParamsX,
                     bufParamsW,
                     bufParamsY,
                     (const FFTLIB_FFT_dftLarge_ixX_cxX_oxX_InitArgs *)pKerInitArgs);
      }
   
      if (highRadixDecompositions) {
   #if FFTLIB_DEBUGPRINT
         printf("About to call FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_init_checkParams()..\n");
   #endif
         status = FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_init_checkParams(
                     (FFTLIB_kernelHandle)&pKerPrivArgs->privArgs.highRadixDecompositions,
                     bufParamsX,
                     bufParamsW,
                     bufParamsY,
                     (const FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_InitArgs *)pKerInitArgs);
      }
   }

   return status;
}

FFTLIB_STATUS FFTLIB_FFT_fft_ixX_cxX_oxX_exec_checkParams(
   FFTLIB_kernelHandle handle, 
   const void *restrict pX,
   const void *restrict pW,
   const void *restrict pY)
{
   FFTLIB_STATUS status;

   if ((pX == NULL) || (pW == NULL) || (pY == NULL) ) {
      status = FFTLIB_ERR_NULL_POINTER;
   } else {
      status  = FFTLIB_SUCCESS ;
   }

   return status;
}

FFTLIB_STATUS FFTLIB_FFT_fft_ixX_cxX_oxX_getSizes(
   const FFTLIB_FFT_fft_ixX_cxX_oxX_InitArgs * pKerInitArgs,
   FFTLIB_bufParams1D_t * bufParamsX,
   FFTLIB_bufParams1D_t * bufParamsW,
   FFTLIB_bufParams1D_t * bufParamsY)
{
   FFTLIB_STATUS status ;
   
   if (bufParamsX->data_type == FFTLIB_INT16) {
      status = FFTLIB_FFT_fft_ixX_cxX_oxX_getSizes< FFTLIB_INT16>(pKerInitArgs, 
                                                                  bufParamsX, 
                                                                  bufParamsW, 
                                                                  bufParamsY);
   } else if (bufParamsX->data_type == FFTLIB_INT32) {
      status = FFTLIB_FFT_fft_ixX_cxX_oxX_getSizes<FFTLIB_INT32>(pKerInitArgs,
                                                                 bufParamsX, 
                                                                 bufParamsW, 
                                                                 bufParamsY);
   } else {
      status = FFTLIB_ERR_NOT_IMPLEMENTED;
   }
   
   return status ;
}

FFTLIB_STATUS FFTLIB_FFT_fft_ixX_cxX_oxX_twGen(
   const FFTLIB_FFT_fft_ixX_cxX_oxX_InitArgs * pKerInitArgs,
   void *pW,
   const FFTLIB_bufParams1D_t * bufParamsW)
{
   FFTLIB_STATUS status ;
   
   if (bufParamsW->data_type == FFTLIB_INT16) {
      status = FFTLIB_FFT_fft_ixX_cxX_oxX_twGen<int16_t>(pKerInitArgs, 
                                                         pW, 
                                                         bufParamsW); 
   } else if (bufParamsW->data_type == FFTLIB_INT32) {
      status = FFTLIB_FFT_fft_ixX_cxX_oxX_twGen<int32_t>(pKerInitArgs, 
                                                         pW, 
                                                         bufParamsW);
   } else {
      status = FFTLIB_ERR_NOT_IMPLEMENTED;
   }
   
   return status ;
}

FFTLIB_STATUS FFTLIB_FFT_fft_ixX_cxX_oxX_init(
   FFTLIB_kernelHandle handle, 
   FFTLIB_bufParams1D_t * bufParamsX,
   FFTLIB_bufParams1D_t * bufParamsW,
   FFTLIB_bufParams1D_t * bufParamsY,
   const FFTLIB_FFT_fft_ixX_cxX_oxX_InitArgs *pKerInitArgs)
{

   FFTLIB_STATUS status  = FFTLIB_SUCCESS;
   FFTLIB_FFT_fft_ixX_cxX_oxX_PrivArgs *pKerPrivArgs;
   int32_t MMA_SIZE;
   int32_t dftSmall, dftLarge, highRadixDecompositions;

#if FFTLIB_DEBUGPRINT
   printf("In FFTLIB_FFT_fft_ixX_cxX_oxX_init()..\n");
#endif
   if(handle == NULL) {
      status = FFTLIB_ERR_NULL_POINTER;
   } else {
      pKerPrivArgs = (FFTLIB_FFT_fft_ixX_cxX_oxX_PrivArgs *)handle;

      if (bufParamsX->data_type == FFTLIB_INT16) {
         MMA_SIZE = FFTLIB_MMA_SIZE_16_BIT;
      } else if (bufParamsX->data_type == FFTLIB_INT32) {
         MMA_SIZE = FFTLIB_MMA_SIZE_32_BIT;
      } else {
         status = FFTLIB_ERR_NOT_IMPLEMENTED;
      }
   }

   if (status == FFTLIB_SUCCESS) {
      pKerPrivArgs->mmaSize = MMA_SIZE;
      pKerPrivArgs->initArgs = *pKerInitArgs;
   
      FFTLIB_FFT_fft_ixX_cxX_oxX_determineKernelVariant(
         pKerInitArgs,
         MMA_SIZE,
         &dftSmall,
         &dftLarge,
         &highRadixDecompositions);
   
      if (dftSmall) {
   #if FFTLIB_DEBUGPRINT
         printf("About to call FFTLIB_FFT_dftSmall_ixX_cxX_oxX_init()..\n");
   #endif
         status = 
            FFTLIB_FFT_dftSmall_ixX_cxX_oxX_init(
               (FFTLIB_kernelHandle)&pKerPrivArgs->privArgs.dftSmall,
               bufParamsX,
               bufParamsW,
               bufParamsY,
               (FFTLIB_FFT_dftSmall_ixX_cxX_oxX_InitArgs *)&pKerPrivArgs->initArgs);
      }
   
      if (dftLarge) {
   #if FFTLIB_DEBUGPRINT
         printf("About to call FFTLIB_FFT_dftLarge_ixX_cxX_oxX_init()..\n");
   #endif
         status = 
            FFTLIB_FFT_dftLarge_ixX_cxX_oxX_init(
               (FFTLIB_kernelHandle)&pKerPrivArgs->privArgs.dftLarge,
               bufParamsX,
               bufParamsW,
               bufParamsY,
               (FFTLIB_FFT_dftLarge_ixX_cxX_oxX_InitArgs *)&pKerPrivArgs->initArgs);
      }
      
      if (highRadixDecompositions) {
   #if FFTLIB_DEBUGPRINT
         printf("About to call FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_init()..\n");
   #endif
         status = 
            FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_init(
               (FFTLIB_kernelHandle)&pKerPrivArgs->privArgs.highRadixDecompositions,
               bufParamsX,
               bufParamsW,
               bufParamsY,
               (FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_InitArgs *)&pKerPrivArgs->initArgs);
      }
   }
   return status;
}

FFTLIB_STATUS FFTLIB_FFT_fft_ixX_cxX_oxX_exec(
   FFTLIB_kernelHandle handle, 
   void *restrict pX,
   void *restrict pW,
   void *restrict pY)
{
   FFTLIB_STATUS status = FFTLIB_ERR_FAILURE;
   FFTLIB_FFT_fft_ixX_cxX_oxX_PrivArgs *pKerPrivArgs =
      (FFTLIB_FFT_fft_ixX_cxX_oxX_PrivArgs*) handle;
   int32_t MMA_SIZE = pKerPrivArgs->mmaSize; 
   int32_t dftSmall, dftLarge, highRadixDecompositions;

#if FFTLIB_DEBUGPRINT
   printf("In FFTLIB_FFT_fft_ixX_cxX_oxX_exec()..\n");
#endif

   FFTLIB_FFT_fft_ixX_cxX_oxX_determineKernelVariant(
      &(pKerPrivArgs->initArgs),
      MMA_SIZE,
      &dftSmall,
      &dftLarge,
      &highRadixDecompositions);

   if (dftSmall) {
#if FFTLIB_DEBUGPRINT
      printf("About to call FFTLIB_FFT_dftSmall_ixX_cxX_oxX_exec()..\n");
#endif
      status = 
         FFTLIB_FFT_dftSmall_ixX_cxX_oxX_exec(
            (FFTLIB_kernelHandle)&pKerPrivArgs->privArgs.dftSmall,
            pX,
            pW,
            pY);
   }

   if (dftLarge) {
#if FFTLIB_DEBUGPRINT
      printf("About to call FFTLIB_FFT_dftLarge_ixX_cxX_oxX_exec()..\n");
#endif
      status = 
         FFTLIB_FFT_dftLarge_ixX_cxX_oxX_exec(
            (FFTLIB_kernelHandle)&pKerPrivArgs->privArgs.dftLarge,
            pX,
            pW,
            pY);
   }

   if (highRadixDecompositions) {
#if FFTLIB_DEBUGPRINT
      printf("About to call FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_exec()..\n");
#endif
      status = 
         FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_exec(
            (FFTLIB_kernelHandle)&pKerPrivArgs->privArgs.highRadixDecompositions,
            pX,
            pW,
            pY);
   }

   return status;
}
