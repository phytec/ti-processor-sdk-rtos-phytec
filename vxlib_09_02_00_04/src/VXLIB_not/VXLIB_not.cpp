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

#include "VXLIB_not_priv.h"
#include <cstdint>

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_not_getHandleSize */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method calculates and returns the size of the handle for the VXLIB_not kernel
int32_t VXLIB_not_getHandleSize(VXLIB_not_InitArgs *pKerInitArgs)
{
   int32_t privBufSize = sizeof(VXLIB_not_PrivArgs);
   return privBufSize;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_not_init_checkParams */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method checks the initialization parameters for the VXLIB_not kernel
VXLIB_STATUS
VXLIB_not_init_checkParams(VXLIB_kernelHandle         handle,
                           const VXLIB_bufParams2D_t *bufParamsIn,
                           const VXLIB_bufParams2D_t *bufParamsOut,
                           const VXLIB_not_InitArgs * pKerInitArgs)
{
   VXLIB_STATUS status = VXLIB_SUCCESS;

   if (handle == NULL || bufParamsIn == NULL || bufParamsOut == NULL || pKerInitArgs == NULL) {
      status = VXLIB_ERR_NULL_POINTER;
   }

   if (status == VXLIB_SUCCESS) {
      if (((bufParamsIn->data_type != VXLIB_UINT8) || (bufParamsOut->data_type != VXLIB_UINT8)) &&
          ((bufParamsIn->data_type != VXLIB_UINT16) || (bufParamsOut->data_type != VXLIB_UINT16)) &&
          ((bufParamsIn->data_type != VXLIB_INT8) || (bufParamsOut->data_type != VXLIB_INT8)) &&
          ((bufParamsIn->data_type != VXLIB_INT16) || (bufParamsOut->data_type != VXLIB_INT16))) {
         status = VXLIB_ERR_INVALID_TYPE;
      }
   }

   if (status == VXLIB_SUCCESS) {
      if ((bufParamsIn->dim_x != bufParamsOut->dim_x) || (bufParamsIn->dim_y != bufParamsOut->dim_y)) {
         status = VXLIB_ERR_INVALID_DIMENSION;
      }
   }

   if (status == VXLIB_SUCCESS) {
      uint32_t strideInElements = bufParamsIn->stride_y / VXLIB_sizeof(bufParamsIn->data_type); 
      uint32_t strideOutElements = bufParamsOut->stride_y / VXLIB_sizeof(bufParamsOut->data_type); 
      if ((strideInElements < bufParamsIn->dim_x) || (strideOutElements < bufParamsOut->dim_x)) {
         status = VXLIB_ERR_NOT_EQUAL_WIDTH_STRIDE;
      }
   }

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_not_exec_checkParams */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method checks the execution parameters for the VXLIB_not kernel
VXLIB_STATUS VXLIB_not_exec_checkParams(VXLIB_kernelHandle handle, const void *restrict pIn, const void *restrict pOut)
{
   VXLIB_STATUS status;

#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_not_exec_checkParams\n");
#endif
   if ((handle == NULL) || (pIn == NULL) || (pOut == NULL)) {
      status = VXLIB_ERR_NULL_POINTER;
   }
   else {
      status = VXLIB_SUCCESS;
   }

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_not_init */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method is the user-level initialization function for the VXLIB_not kernel
VXLIB_STATUS VXLIB_not_init(VXLIB_kernelHandle        handle,
                            VXLIB_bufParams2D_t *     bufParamsIn,
                            VXLIB_bufParams2D_t *     bufParamsOut,
                            const VXLIB_not_InitArgs *pKerInitArgs)
{
   VXLIB_STATUS        status       = VXLIB_SUCCESS;
   VXLIB_not_PrivArgs *pKerPrivArgs = (VXLIB_not_PrivArgs *) handle;

   // set width and height of image
   pKerPrivArgs->width  = bufParamsIn->dim_x;
   pKerPrivArgs->height = bufParamsIn->dim_y;

   // compute stride in elements as SE/SA params are set to work with given element type
   pKerPrivArgs->strideInElements  = bufParamsIn->stride_y / VXLIB_sizeof(bufParamsIn->data_type);
   pKerPrivArgs->strideOutElements = bufParamsOut->stride_y / VXLIB_sizeof(bufParamsOut->data_type);

#if VXLIB_DEBUGPRINT
   printf("VXLIB_DEBUGPRINT Enter VXLIB_not_init\n");
#endif

   // obtain buffer datatypes
   uint32_t dTypeIn = bufParamsIn->data_type;

   // determine natural C  vs optimized function call
   if (pKerInitArgs->funcStyle == VXLIB_FUNCTION_NATC) {

      // set function pointer for natural C function with appropriate template parameters based on datatypes

      if ((dTypeIn == VXLIB_UINT8)) {
         pKerPrivArgs->execute = VXLIB_not_exec_cn<uint8_t>;
      }
      else if (dTypeIn == VXLIB_UINT16) {
         pKerPrivArgs->execute = VXLIB_not_exec_cn<uint16_t>;
      }
      else if ((dTypeIn == VXLIB_INT8)) {
         pKerPrivArgs->execute = VXLIB_not_exec_cn<int8_t>;
      }
      else if (dTypeIn == VXLIB_INT16) {
         pKerPrivArgs->execute = VXLIB_not_exec_cn<int16_t>;
      }
      else {
         status = VXLIB_ERR_INVALID_TYPE;
      }
   }
   else { // Optimized function

      // set function pointer for optimized function with appropriate template parameters based on datatypes

      if (dTypeIn == VXLIB_UINT8) {
         pKerPrivArgs->execute = VXLIB_not_exec_ci<uint8_t>;
         status                = VXLIB_not_init_ci<VXLIB_UINT8>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
      }
      else if (dTypeIn == VXLIB_UINT16) {
         pKerPrivArgs->execute = VXLIB_not_exec_ci<uint16_t>;
         status                = VXLIB_not_init_ci<VXLIB_UINT16>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
      }
      else if (dTypeIn == VXLIB_INT8) {
         pKerPrivArgs->execute = VXLIB_not_exec_ci<int8_t>;
         status                = VXLIB_not_init_ci<VXLIB_INT8>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
      }
      else if (dTypeIn == VXLIB_INT16) {
         pKerPrivArgs->execute = VXLIB_not_exec_ci<int16_t>;
         status                = VXLIB_not_init_ci<VXLIB_INT16>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
      }
      else {
         status = VXLIB_ERR_INVALID_TYPE;
      }
   }

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_not_exec */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method is the user-level execution function for the VXLIB_not kernel
VXLIB_STATUS VXLIB_not_exec(VXLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   VXLIB_STATUS status;

#if VXLIB_DEBUGPRINT
   printf("VXLIB_DEBUGPRINT Enter VXLIB_not_exec\n");
#endif

   VXLIB_not_PrivArgs *pKerPrivArgs = (VXLIB_not_PrivArgs *) handle;

   status = pKerPrivArgs->execute(handle, pIn, pOut);

   return status;
}
/* ======================================================================== */
/*  End of file:  VXLIB_not.cpp                                             */
/* ======================================================================== */
