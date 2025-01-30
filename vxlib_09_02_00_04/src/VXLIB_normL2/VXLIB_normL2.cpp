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

#include "VXLIB_normL2_priv.h"
#include <cstdint>

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_normL2_getHandleSize */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method calculates and returns the size of the handle for the VXLIB_normL2 kernel
int32_t VXLIB_normL2_getHandleSize(VXLIB_normL2_InitArgs *pKerInitArgs)
{
   int32_t privBufSize = sizeof(VXLIB_normL2_PrivArgs);
   return privBufSize;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_normL2_init_checkParams */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method checks the initialization parameters for the VXLIB_normL2 kernel
VXLIB_STATUS
VXLIB_normL2_init_checkParams(VXLIB_kernelHandle           handle,
                              const VXLIB_bufParams2D_t *  bufParamsIn0,
                              const VXLIB_bufParams2D_t *  bufParamsIn1,
                              const VXLIB_bufParams2D_t *  bufParamsOut,
                              const VXLIB_normL2_InitArgs *pKerInitArgs)
{
   VXLIB_STATUS status = VXLIB_SUCCESS;

   if (handle == NULL || bufParamsIn0 == NULL || bufParamsIn1 == NULL || bufParamsOut == NULL || pKerInitArgs == NULL) {
      status = VXLIB_ERR_NULL_POINTER;
   }

   if (status == VXLIB_SUCCESS) {
      if (((bufParamsIn0->data_type != VXLIB_INT8) || (bufParamsIn1->data_type != VXLIB_INT8) ||
           (bufParamsOut->data_type != VXLIB_UINT8)) &&
          ((bufParamsIn0->data_type != VXLIB_INT16) || (bufParamsIn1->data_type != VXLIB_INT16) ||
           (bufParamsOut->data_type != VXLIB_UINT16))) {
         status = VXLIB_ERR_INVALID_TYPE;
      }
   }

   if (status == VXLIB_SUCCESS) {
      if ((bufParamsIn0->dim_x != bufParamsIn1->dim_x) || (bufParamsIn0->dim_y != bufParamsIn1->dim_y) ||
          (bufParamsIn0->dim_x != bufParamsOut->dim_x) || (bufParamsIn0->dim_y != bufParamsOut->dim_y)) {
         status = VXLIB_ERR_INVALID_DIMENSION;
      }
   }

   if (status == VXLIB_SUCCESS) {
      uint32_t strideIn0Elements = bufParamsIn0->stride_y / VXLIB_sizeof(bufParamsIn0->data_type); 
      uint32_t strideIn1Elements = bufParamsIn1->stride_y / VXLIB_sizeof(bufParamsIn1->data_type); 
      uint32_t strideOutElements = bufParamsOut->stride_y / VXLIB_sizeof(bufParamsOut->data_type); 
      if ((strideIn0Elements < bufParamsIn0->dim_x) || (strideIn1Elements < bufParamsIn1->dim_x) ||
          (strideOutElements < bufParamsOut->dim_x)) {
         status = VXLIB_ERR_NOT_EQUAL_WIDTH_STRIDE;
      }
   }

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_normL2_exec_checkParams */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method checks the execution parameters for the VXLIB_normL2 kernel
VXLIB_STATUS VXLIB_normL2_exec_checkParams(VXLIB_kernelHandle   handle,
                                           const void *restrict pIn0,
                                           const void *restrict pIn1,
                                           const void *restrict pOut)
{
   VXLIB_STATUS status;

#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_normL2_exec_checkParams\n");
#endif
   if ((handle == NULL) || (pIn0 == NULL) || (pIn1 == NULL) || (pOut == NULL)) {
      status = VXLIB_ERR_NULL_POINTER;
   }
   else {
      status = VXLIB_SUCCESS;
   }

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_normL2_init */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method is the user-level initialization function for the VXLIB_normL2 kernel
VXLIB_STATUS VXLIB_normL2_init(VXLIB_kernelHandle           handle,
                               VXLIB_bufParams2D_t *        bufParamsIn0,
                               VXLIB_bufParams2D_t *        bufParamsIn1,
                               VXLIB_bufParams2D_t *        bufParamsOut,
                               const VXLIB_normL2_InitArgs *pKerInitArgs)
{
   VXLIB_STATUS           status       = VXLIB_SUCCESS;
   VXLIB_normL2_PrivArgs *pKerPrivArgs = (VXLIB_normL2_PrivArgs *) handle;

   // set width and height of image
   pKerPrivArgs->width  = bufParamsIn0->dim_x;
   pKerPrivArgs->height = bufParamsIn0->dim_y;

   // compute stride in elements as SE/SA params are set to work with given element type
   pKerPrivArgs->strideIn0Elements = bufParamsIn0->stride_y / VXLIB_sizeof(bufParamsIn0->data_type);
   pKerPrivArgs->strideIn1Elements = bufParamsIn1->stride_y / VXLIB_sizeof(bufParamsIn1->data_type);
   pKerPrivArgs->strideOutElements = bufParamsOut->stride_y / VXLIB_sizeof(bufParamsOut->data_type);

#if VXLIB_DEBUGPRINT
   printf("VXLIB_DEBUGPRINT Enter VXLIB_normL2_init\n");
#endif

   // obtain buffer datatypes
   uint32_t dTypeIn  = bufParamsIn0->data_type;
   uint32_t dTypeOut = bufParamsOut->data_type;

   // determine natural C  vs optimized function call
   if (pKerInitArgs->funcStyle == VXLIB_FUNCTION_NATC) {

      // set function pointer for natural C function with appropriate template parameters based on datatypes
      if (VXLIB_NOMRL2_I8S_O8U) {
         pKerPrivArgs->execute = VXLIB_normL2_exec_cn<VXLIB_NORML2_TYPENAME_I8S_O8U>;
      }
      else if (VXLIB_NORML2_I16S_O16U) {
         pKerPrivArgs->execute = VXLIB_normL2_exec_cn<VXLIB_NORML2_TYPENAME_I16S_O16U>;
      }
      else {
         status = VXLIB_ERR_INVALID_TYPE;
      }
   }
   else { // Optimized function

      // set function pointer for optimized function with appropriate template parameters based on datatypes

      if (VXLIB_NOMRL2_I8S_O8U) {
         pKerPrivArgs->execute = VXLIB_normL2_exec_ci<VXLIB_NORML2_TYPENAME_I8S_O8U>;
         status = VXLIB_normL2_init_ci<VXLIB_NORML2_DTYPE_I8S_O8U>(handle, bufParamsIn0, bufParamsIn1, bufParamsOut,
                                                                   pKerInitArgs);
      }
      else if (VXLIB_NORML2_I16S_O16U) {
         pKerPrivArgs->execute = VXLIB_normL2_exec_ci<VXLIB_NORML2_TYPENAME_I16S_O16U>;
         status = VXLIB_normL2_init_ci<VXLIB_NORML2_DTYPE_I16S_O16U>(handle, bufParamsIn0, bufParamsIn1, bufParamsOut,
                                                                     pKerInitArgs);
      }
      else {
         status = VXLIB_ERR_INVALID_TYPE;
      }
   }

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_normL2_exec */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method is the user-level execution function for the VXLIB_normL2 kernel
VXLIB_STATUS VXLIB_normL2_exec(VXLIB_kernelHandle handle, void *restrict pIn0, void *restrict pIn1, void *restrict pOut)
{
   VXLIB_STATUS status;

#if VXLIB_DEBUGPRINT
   printf("VXLIB_DEBUGPRINT Enter VXLIB_normL2_exec\n");
#endif

   VXLIB_normL2_PrivArgs *pKerPrivArgs = (VXLIB_normL2_PrivArgs *) handle;

   status = pKerPrivArgs->execute(handle, pIn0, pIn1, pOut);

   return status;
}
/* ======================================================================== */
/*  End of file:  VXLIB_normL2.cpp                                          */
/* ======================================================================== */
