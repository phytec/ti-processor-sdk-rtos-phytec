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
*  @file     FFTLIB_FFT_dftLarge_ixX_cxX_oxX.c
*
*  @brief    DFT computation
*
*  @version  0.1 - Jan 2018 : Initial Version
*            
*******************************************************************************
*/

#include "FFTLIB_FFT_dftLarge_ixX_cxX_oxX_priv.h"

int32_t FFTLIB_FFT_dftLarge_ixX_cxX_oxX_getHandleSize(
   FFTLIB_FFT_dftLarge_ixX_cxX_oxX_InitArgs *pKerInitArgs)
{
  int32_t privBufSize = sizeof(FFTLIB_FFT_dftLarge_ixX_cxX_oxX_PrivArgs);
  return privBufSize ;
}

FFTLIB_STATUS FFTLIB_FFT_dftLarge_ixX_cxX_oxX_init_checkParams(
   FFTLIB_kernelHandle handle,
   const FFTLIB_bufParams1D_t * bufParamsX,
   const FFTLIB_bufParams1D_t * bufParamsW,
   const FFTLIB_bufParams1D_t * bufParamsY,
   const FFTLIB_FFT_dftLarge_ixX_cxX_oxX_InitArgs *pKerInitArgs)
{
   FFTLIB_STATUS status  = FFTLIB_SUCCESS ;
   FFTLIB_bufParams1D_t bufParamsXLocal, bufParamsYLocal, bufParamsWLocal; 
   FFTLIB_bufParams2D_t bufParamsMatrixData, bufParamsMatrixW;
   FFTLIB_FFT_dftLarge_ixX_cxX_oxX_PrivArgs *pKerPrivArgs;

   if(handle == NULL) {
      status = FFTLIB_ERR_NULL_POINTER;
   } else {
      pKerPrivArgs = (FFTLIB_FFT_dftLarge_ixX_cxX_oxX_PrivArgs *)handle;

      if (bufParamsX->data_type == FFTLIB_INT16) {
         status = FFTLIB_FFT_dftLarge_ixX_cxX_oxX_getSizes<FFTLIB_INT16>(pKerInitArgs,
                                                                         &bufParamsXLocal,
                                                                         &bufParamsWLocal,
                                                                         &bufParamsYLocal);
      } else if (bufParamsX->data_type == FFTLIB_INT32) {
         status = FFTLIB_FFT_dftLarge_ixX_cxX_oxX_getSizes<FFTLIB_INT32>(pKerInitArgs,
                                                                         &bufParamsXLocal,
                                                                            &bufParamsWLocal,
                                                                         &bufParamsYLocal);
      } else {
         status = FFTLIB_ERR_NOT_IMPLEMENTED;
      }
   }

   if (status == FFTLIB_SUCCESS) {
      if (bufParamsX->dim_x != bufParamsXLocal.dim_x) {
         status = FFTLIB_ERR_INVALID_DIMENSION;
      } else if (bufParamsY->dim_x != bufParamsYLocal.dim_x) {
         status = FFTLIB_ERR_INVALID_DIMENSION;
      } else if (bufParamsW->dim_x != bufParamsWLocal.dim_x) {
         status = FFTLIB_ERR_INVALID_DIMENSION;
      } else if ((bufParamsX->data_type != bufParamsW->data_type) ||
                 (bufParamsX->data_type != bufParamsY->data_type) ) {
         status = FFTLIB_ERR_INVALID_TYPE;
      } else {
         bufParamsMatrixData.dim_x = pKerInitArgs->fftSize*2;
         bufParamsMatrixData.dim_y = pKerInitArgs->batchSize;
         bufParamsMatrixData.stride_y = pKerInitArgs->fftSize*2*FFTLIB_sizeof(bufParamsX->data_type);
         bufParamsMatrixData.data_type = bufParamsX->data_type;
         bufParamsMatrixW.dim_x = pKerInitArgs->fftSize*2;
         bufParamsMatrixW.dim_y = pKerInitArgs->fftSize*2;
         bufParamsMatrixW.stride_y = pKerInitArgs->fftSize*2*FFTLIB_sizeof(bufParamsW->data_type);
         bufParamsMatrixW.data_type = bufParamsW->data_type;
         pKerPrivArgs->matrixMatrixMultiplyInitArgs.shift = pKerInitArgs->shiftVector[0];
         status = FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_init_checkParams(
                     (FFTLIB_kernelHandle)&pKerPrivArgs->matrixMatrixMultiplyPrivArgs, 
                     &bufParamsMatrixData, 
                     &bufParamsMatrixW, 
                     &bufParamsMatrixData, 
                     &pKerPrivArgs->matrixMatrixMultiplyInitArgs);
      }
   }

   return status;
}

FFTLIB_STATUS FFTLIB_FFT_dftLarge_ixX_cxX_oxX_exec_checkParams(
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

FFTLIB_STATUS FFTLIB_FFT_dftLarge_ixX_cxX_oxX_getSizes(
   const FFTLIB_FFT_dftLarge_ixX_cxX_oxX_InitArgs * pKerInitArgs,
   FFTLIB_bufParams1D_t * bufParamsX,
   FFTLIB_bufParams1D_t * bufParamsW,
   FFTLIB_bufParams1D_t * bufParamsY)
{
   FFTLIB_STATUS status ;

   if (bufParamsX->data_type == FFTLIB_INT16) {
      status = FFTLIB_FFT_dftLarge_ixX_cxX_oxX_getSizes<FFTLIB_INT16>(pKerInitArgs, 
                                                                      bufParamsX, 
                                                                      bufParamsW, 
                                                                      bufParamsY);
   } else if (bufParamsX->data_type == FFTLIB_INT32) {
      status = FFTLIB_FFT_dftLarge_ixX_cxX_oxX_getSizes<FFTLIB_INT32>(pKerInitArgs, 
                                                                      bufParamsX, 
                                                                      bufParamsW, 
                                                                      bufParamsY);
   } else {
      status = FFTLIB_ERR_NOT_IMPLEMENTED;
   }

   return status ;
}

FFTLIB_STATUS FFTLIB_FFT_dftLarge_ixX_cxX_oxX_twGen(
   const FFTLIB_FFT_dftLarge_ixX_cxX_oxX_InitArgs * pKerInitArgs,
   void *pW,
   const FFTLIB_bufParams1D_t * bufParamsW)
{
   FFTLIB_STATUS status ;

   if (bufParamsW->data_type == FFTLIB_INT16) {
      status = FFTLIB_FFT_dftLarge_ixX_cxX_oxX_twGen<int16_t>(pKerInitArgs, 
                                                              pW, 
                                                              bufParamsW); 
   } else if (bufParamsW->data_type == FFTLIB_INT32) {
      status = FFTLIB_FFT_dftLarge_ixX_cxX_oxX_twGen<int32_t>(pKerInitArgs, 
                                                              pW, 
                                                              bufParamsW);
   } else {
      status = FFTLIB_ERR_NOT_IMPLEMENTED;
   }

   return status ;
}

FFTLIB_STATUS FFTLIB_FFT_dftLarge_ixX_cxX_oxX_init(
   FFTLIB_kernelHandle handle, 
   const FFTLIB_bufParams1D_t * bufParamsX,
   const FFTLIB_bufParams1D_t * bufParamsW,
   const FFTLIB_bufParams1D_t * bufParamsY,
   const FFTLIB_FFT_dftLarge_ixX_cxX_oxX_InitArgs *pKerInitArgs)
{
   FFTLIB_STATUS status ;
   FFTLIB_FFT_dftLarge_ixX_cxX_oxX_PrivArgs *pKerPrivArgs =
      (FFTLIB_FFT_dftLarge_ixX_cxX_oxX_PrivArgs*) handle ;
   FFTLIB_bufParams2D_t bufParamsMatrixData, bufParamsMatrixW;

   pKerPrivArgs->matrixMatrixMultiplyInitArgs.shift =
      pKerInitArgs->shiftVector[0];
   pKerPrivArgs->matrixMatrixMultiplyInitArgs.funcStyle =
      pKerInitArgs->funcStyle;
   pKerPrivArgs->initArgs = *pKerInitArgs;
   bufParamsMatrixData.dim_x = pKerInitArgs->fftSize*2;
   bufParamsMatrixData.dim_y = pKerInitArgs->batchSize;
   bufParamsMatrixData.stride_y = pKerInitArgs->fftSize*2*FFTLIB_sizeof(bufParamsX->data_type);
   bufParamsMatrixData.data_type = bufParamsX->data_type;
   bufParamsMatrixW.dim_x = pKerInitArgs->fftSize*2;
   bufParamsMatrixW.dim_y = pKerInitArgs->fftSize*2;
   bufParamsMatrixW.stride_y = pKerInitArgs->fftSize*2*FFTLIB_sizeof(bufParamsW->data_type);
   bufParamsMatrixW.data_type = bufParamsW->data_type;
   status = FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_init(
               (FFTLIB_kernelHandle)&pKerPrivArgs->matrixMatrixMultiplyPrivArgs, 
               &bufParamsMatrixData, 
               &bufParamsMatrixW, 
               &bufParamsMatrixData, 
               &pKerPrivArgs->matrixMatrixMultiplyInitArgs);

   return status;
}

FFTLIB_STATUS FFTLIB_FFT_dftLarge_ixX_cxX_oxX_exec(
   FFTLIB_kernelHandle handle, 
   const void *restrict pX,
   const void *restrict pW,
   void *restrict pY)
{
   FFTLIB_STATUS status ;

   FFTLIB_FFT_dftLarge_ixX_cxX_oxX_PrivArgs *pKerPrivArgs =
      (FFTLIB_FFT_dftLarge_ixX_cxX_oxX_PrivArgs*) handle;
  
   status = FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_exec(
               (FFTLIB_kernelHandle)&pKerPrivArgs->matrixMatrixMultiplyPrivArgs,
               pX,
               pW,
               pY);
   return status ;
}
