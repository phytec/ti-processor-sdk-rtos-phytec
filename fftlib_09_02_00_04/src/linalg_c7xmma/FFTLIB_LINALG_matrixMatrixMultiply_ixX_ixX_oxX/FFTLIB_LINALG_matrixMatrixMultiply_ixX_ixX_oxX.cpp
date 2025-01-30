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
*  @file     FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX.c
*
*  @brief    Matrix-matrix multiplication
*
*  @version  0.1 - Jan 2018 : Initial Version
*            
*******************************************************************************
*/

#include "FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_priv.h"

int32_t FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_getHandleSize(
        FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_InitArgs *pKerInitArgs)
{
  int32_t privBufSize = sizeof(FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_PrivArgs);
  return privBufSize ;
}

FFTLIB_STATUS FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_init_checkParams(FFTLIB_kernelHandle handle,
                                                                              const FFTLIB_bufParams2D_t * src0_addr,
                                                                              const FFTLIB_bufParams2D_t * src1_addr,
                                                                              const FFTLIB_bufParams2D_t * dst_addr,
                                                                              const FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_InitArgs *pKerInitArgs)
{
   FFTLIB_STATUS status  = FFTLIB_SUCCESS ;
   int8_t shift = pKerInitArgs->shift;
   
   if(handle == NULL) {
      status = FFTLIB_ERR_NULL_POINTER;
   }
   // check for valid dimensions
   else if((src0_addr->dim_x != src1_addr->dim_y) ||   // A is m x k, B is k x n, C is m x n
      (src0_addr->dim_y != dst_addr->dim_y)  ||
      (src1_addr->dim_x != dst_addr->dim_x)){
      status = FFTLIB_ERR_INVALID_DIMENSION;
   }
   // check for matching data types
   else if(src0_addr->data_type == FFTLIB_INT8){  // i8s_i8s_o8s
      if((src1_addr->data_type != FFTLIB_INT8) ||
         (dst_addr->data_type != FFTLIB_INT8) ){
         status = FFTLIB_ERR_MATRIX_MATRIX_MULTIPLY_UNSUPPORTED_TYPE_COMBINATION;
      }
      if((status == FFTLIB_SUCCESS) && (shift > (FFTLIB_sizeof(FFTLIB_INT32) * 8))){
         status = FFTLIB_ERR_INVALID_SHIFT;
      }
   } else if(src0_addr->data_type == FFTLIB_INT16){  // i16s_i16s_o16s
      if((src1_addr->data_type != FFTLIB_INT16) ||
         (dst_addr->data_type != FFTLIB_INT16) ){
         status = FFTLIB_ERR_MATRIX_MATRIX_MULTIPLY_UNSUPPORTED_TYPE_COMBINATION;
      }
      if((status == FFTLIB_SUCCESS) && (shift > (FFTLIB_sizeof(FFTLIB_INT64) * 8))){
         status = FFTLIB_ERR_INVALID_SHIFT;
      }
   } else if(src0_addr->data_type == FFTLIB_INT32){  // i32s_i32s_o32s
      if((src1_addr->data_type != FFTLIB_INT32) ||
         (dst_addr->data_type != FFTLIB_INT32) ){
         status = FFTLIB_ERR_MATRIX_MATRIX_MULTIPLY_UNSUPPORTED_TYPE_COMBINATION;
      }
      if((status == FFTLIB_SUCCESS) && (shift > (FFTLIB_sizeof(FFTLIB_INT128) * 8))){
         status = FFTLIB_ERR_INVALID_SHIFT;
      }
   } else {
      status = FFTLIB_ERR_MATRIX_MATRIX_MULTIPLY_UNSUPPORTED_INPUT_TYPE;
   }
   
   return status ;
}

FFTLIB_STATUS FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_exec_checkParams(
  FFTLIB_kernelHandle handle, 
  const void * src0,
  const void * src1,
  const void * dst)
{
  FFTLIB_STATUS status  = FFTLIB_SUCCESS ;

   if((src0 == NULL) || (src1 == NULL) || (dst == NULL)) {
      status = FFTLIB_ERR_NULL_POINTER;
   }

  return status ;
}

FFTLIB_STATUS FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_init(
  FFTLIB_kernelHandle handle, 
  const FFTLIB_bufParams2D_t * src0_addr,
  const FFTLIB_bufParams2D_t * src1_addr,
  const FFTLIB_bufParams2D_t * dst_addr,
  const FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_InitArgs *pKerInitArgs)
{
   FFTLIB_STATUS status ;
   //Error Checks and update status with appropriate error code
   FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_PrivArgs *pKerPrivArgs =
   (FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_PrivArgs*) handle ;
   
   // error check all parameters and handle for NULLNESS
   // return if error
#if defined(FFTLIB_CHECK_PARAMS) || defined(FFTLIB_LINALG_MATRIXMATRIXMULTIPLY_IXX_IXX_OXX_CHECK_PARAMS)
   status = FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_init_checkParams(
      handle, src0_addr,  src1_addr,  dst_addr, pKerInitArgs);
   if(status != FFTLIB_SUCCESS)
   {
      return status ;
   }
#endif
   
   pKerPrivArgs->initArgs = *pKerInitArgs;
   
   pKerPrivArgs->strideAElements = src0_addr->stride_y / FFTLIB_sizeof(src0_addr->data_type);
   pKerPrivArgs->strideBElements = src1_addr->stride_y / FFTLIB_sizeof(src1_addr->data_type);
   pKerPrivArgs->strideCElements = dst_addr->stride_y / FFTLIB_sizeof(dst_addr->data_type);
   
   if(pKerInitArgs->funcStyle == FFTLIB_FUNCTION_NATC)
   {
      status = FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_init_cn(handle, src0_addr,  src1_addr,  dst_addr, pKerInitArgs) ;
      if (src0_addr->data_type == FFTLIB_INT8 && src1_addr->data_type == FFTLIB_INT8 && dst_addr->data_type == FFTLIB_INT8) {
         pKerPrivArgs->execute  = FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_exec_cn<int8_t, int32_t>;
      
      } else if (src0_addr->data_type == FFTLIB_INT16 && src1_addr->data_type == FFTLIB_INT16 && dst_addr->data_type == FFTLIB_INT16) {
         pKerPrivArgs->execute  = FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_exec_cn<int16_t, int64_t>;
      
      } else if (src0_addr->data_type == FFTLIB_INT32 && src1_addr->data_type == FFTLIB_INT32 && dst_addr->data_type == FFTLIB_INT32)  {
         pKerPrivArgs->execute  = FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_exec_cn<int32_t, FFTLIB_int128_t>;

      }  else {
         status = FFTLIB_ERR_MATRIX_MATRIX_MULTIPLY_UNSUPPORTED_INPUT_TYPE;
      }
   } else {
      if (dst_addr->data_type == FFTLIB_INT8) {
         pKerPrivArgs->execute  = FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_exec_ci<FFTLIB_MMA_SIZE_8_BIT>;
         status = FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_init_ci<FFTLIB_INT8>(handle, src0_addr,  src1_addr,  dst_addr, pKerInitArgs) ;
         
      } else if (dst_addr->data_type == FFTLIB_INT16) {
         pKerPrivArgs->execute  = FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_exec_ci<FFTLIB_MMA_SIZE_16_BIT>;
         status = FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_init_ci<FFTLIB_INT16>(handle, src0_addr,  src1_addr,  dst_addr, pKerInitArgs) ;

      } else if (dst_addr->data_type == FFTLIB_INT32)  {
         pKerPrivArgs->execute  = FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_exec_ci<FFTLIB_MMA_SIZE_32_BIT>;
         status = FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_init_ci<FFTLIB_INT32>(handle, src0_addr,  src1_addr,  dst_addr, pKerInitArgs) ;
   
      }  else {
         status = FFTLIB_ERR_MATRIX_MATRIX_MULTIPLY_UNSUPPORTED_OUTPUT_TYPE;
      }
   }
   
   return status ;
}

FFTLIB_STATUS FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_exec(
  FFTLIB_kernelHandle handle, 
  const void * src0,
  const void * src1,
  void *dst)
{
  FFTLIB_STATUS status ;

  FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_PrivArgs *pKerPrivArgs =
    (FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX_PrivArgs*) handle;
  
  status = pKerPrivArgs->execute(handle, src0, src1, dst) ;
  
  return status ;
}
