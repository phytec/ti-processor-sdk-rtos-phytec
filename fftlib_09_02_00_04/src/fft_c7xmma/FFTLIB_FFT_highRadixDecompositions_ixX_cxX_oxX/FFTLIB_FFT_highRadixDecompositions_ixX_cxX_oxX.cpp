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
*  @file     FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX.c
*
*  @brief    DFT computation
*
*  @version  0.1 - Jan 2018 : Initial Version
*            
*******************************************************************************
*/

#include "FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_priv.h"

int32_t FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_getHandleSize(
   FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_InitArgs *pKerInitArgs)
{
  int32_t privBufSize = sizeof(FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_PrivArgs);
  return privBufSize ;
}

FFTLIB_STATUS FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_init_checkParams(
   FFTLIB_kernelHandle handle,
   const FFTLIB_bufParams1D_t * bufParamsX,
   const FFTLIB_bufParams1D_t * bufParamsW,
   const FFTLIB_bufParams1D_t * bufParamsY,
   const FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_InitArgs *pKerInitArgs)
{
   FFTLIB_STATUS status  = FFTLIB_SUCCESS ;
   FFTLIB_bufParams1D_t bufParamsXLocal, bufParamsYLocal, bufParamsWLocal; 
   uint32_t fftSize = pKerInitArgs->fftSize;
   uint32_t MMA_SIZE;

   if(handle == NULL) {
      status = FFTLIB_ERR_NULL_POINTER;
   } else {
      if (bufParamsX->data_type == FFTLIB_INT16) {
         MMA_SIZE = FFTLIB_MMA_SIZE_16_BIT;
         status = FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_getSizes<FFTLIB_INT16>(pKerInitArgs,
                                                                                        &bufParamsXLocal,
                                                                                        &bufParamsWLocal,
                                                                                        &bufParamsYLocal);
      } else if (bufParamsX->data_type == FFTLIB_INT32) {
         MMA_SIZE = FFTLIB_MMA_SIZE_32_BIT;
         status = FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_getSizes<FFTLIB_INT32>(pKerInitArgs,
                                                                                        &bufParamsXLocal,
                                                                                        &bufParamsWLocal,
                                                                                        &bufParamsYLocal);
      } else {
         status = FFTLIB_ERR_NOT_IMPLEMENTED;
      }
   }

   if (status == FFTLIB_SUCCESS) {
      if (fftSize < MMA_SIZE*2) {
         status = FFTLIB_ERR_INVALID_VALUE;
      } else if (bufParamsX->dim_x != bufParamsXLocal.dim_x) {
         status = FFTLIB_ERR_INVALID_DIMENSION;
      } else if (bufParamsY->dim_x != bufParamsYLocal.dim_x) {
         status = FFTLIB_ERR_INVALID_DIMENSION;
      } else if (bufParamsW->dim_x != bufParamsWLocal.dim_x) {
         status = FFTLIB_ERR_INVALID_DIMENSION;
      } else if ((bufParamsX->data_type != bufParamsW->data_type) ||
                 (bufParamsX->data_type != bufParamsY->data_type) ) {
         status = FFTLIB_ERR_INVALID_TYPE;
      } else {
         /* Nothing to do here */
      }
   }

   return status;
}

FFTLIB_STATUS FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_exec_checkParams(
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

   return status ;
}

FFTLIB_STATUS FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_getSizes(
   const FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_InitArgs * pKerInitArgs,
   FFTLIB_bufParams1D_t * bufParamsX,
   FFTLIB_bufParams1D_t * bufParamsW,
   FFTLIB_bufParams1D_t * bufParamsY)
{
   FFTLIB_STATUS status ;
   
   if (bufParamsX->data_type == FFTLIB_INT16) {
      status = FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_getSizes<FFTLIB_INT16>(pKerInitArgs,
                                                                                     bufParamsX, 
                                                                                     bufParamsW, 
                                                                                     bufParamsY);
   } else if (bufParamsX->data_type == FFTLIB_INT32) {
      status = FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_getSizes<FFTLIB_INT32>(pKerInitArgs,
                                                                                     bufParamsX,
                                                                                     bufParamsW,
                                                                                     bufParamsY);
   } else {
      status = FFTLIB_ERR_NOT_IMPLEMENTED;
   }
   
   return status ;
}

FFTLIB_STATUS FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_twGen(
   const FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_InitArgs * pKerInitArgs,
   void *pW,
   const FFTLIB_bufParams1D_t * bufParamsW)
{
   FFTLIB_STATUS status ;
   
   if (bufParamsW->data_type == FFTLIB_INT16) {
      status = FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_twGen<int16_t>(pKerInitArgs, 
                                                                             pW, 
                                                                             bufParamsW); 
   } else if (bufParamsW->data_type == FFTLIB_INT32) {
      status = FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_twGen<int32_t>(pKerInitArgs, 
                                                                             pW, 
                                                                             bufParamsW);
   } else {
      status = FFTLIB_ERR_NOT_IMPLEMENTED;
   }
   
   return status ;
}

FFTLIB_STATUS FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_init(
   FFTLIB_kernelHandle handle, 
   FFTLIB_bufParams1D_t * bufParamsX,
   FFTLIB_bufParams1D_t * bufParamsW,
   FFTLIB_bufParams1D_t * bufParamsY,
   FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_InitArgs *pKerInitArgs)
{
   FFTLIB_STATUS status ;
   FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_PrivArgs *pKerPrivArgs =
   (FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_PrivArgs*) handle ;
   
   pKerPrivArgs->initArgs = *pKerInitArgs ;
   pKerPrivArgs->pW = &FFTLIB_FFT_twData[0];
   
   if(pKerInitArgs->funcStyle == FFTLIB_FUNCTION_NATC)
   {
      status = FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_init_cn(handle, 
                                                                      bufParamsX,
                                                                      bufParamsW,
                                                                      bufParamsY,
                                                                      pKerInitArgs) ;
      if (bufParamsX->data_type == FFTLIB_INT16) {
         pKerPrivArgs->execute = 
            FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_exec_cn<int16_t, 
                                                                   int64_t,
                                                                   FFTLIB_MMA_SIZE_16_BIT>;
      } else if (bufParamsX->data_type == FFTLIB_INT32) {
         pKerPrivArgs->execute = 
            FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_exec_cn<int32_t, 
                                                                   FFTLIB_int128_t,
                                                                   FFTLIB_MMA_SIZE_32_BIT>;
      }  else {
         status = FFTLIB_ERR_NOT_IMPLEMENTED;
      }
   }
   else
   {
      if (bufParamsX->data_type == FFTLIB_INT16) {
         pKerPrivArgs->execute = 
            FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_exec_ci<int16_t,
                                                                   FFTLIB_MMA_SIZE_16_BIT>;
         status = FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_init_ci<FFTLIB_INT16>(handle, 
                                                                                       bufParamsX, 
                                                                                       bufParamsW, 
                                                                                       bufParamsY, 
                                                                                       pKerInitArgs);
         
      } else if (bufParamsX->data_type == FFTLIB_INT32) {
         pKerPrivArgs->execute = 
            FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_exec_ci<int32_t,
                                                                   FFTLIB_MMA_SIZE_32_BIT>;
         status = FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_init_ci<FFTLIB_INT32>(handle, 
                                                                                       bufParamsX,
                                                                                       bufParamsW,
                                                                                       bufParamsX,
                                                                                       pKerInitArgs);

      }  else {
         status = FFTLIB_ERR_NOT_IMPLEMENTED;
      }
   }
   
   return status ;
}

FFTLIB_STATUS FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_exec(
   FFTLIB_kernelHandle handle, 
   void *restrict pX,
   void *restrict pW,
   void *restrict pY)
{
   FFTLIB_STATUS status ;

   FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_PrivArgs *pKerPrivArgs =
      (FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_PrivArgs*) handle;
  
   status = pKerPrivArgs->execute(handle, 
                                  pX, 
                                  pW, 
                                  pY) ;
  
   return status ;
}
