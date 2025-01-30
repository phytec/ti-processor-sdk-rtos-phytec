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

#include "VXLIB_addSquare_priv.h"
#include <cstdint>

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_addSquare_getHandleSize */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method calculates and returns the size of the handle for the VXLIB_addSquare kernel
int32_t VXLIB_addSquare_getHandleSize(VXLIB_addSquare_InitArgs *pKerInitArgs)
{
   int32_t privBufSize = sizeof(VXLIB_addSquare_PrivArgs);
   return privBufSize;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_addSquare_init_checkParams */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method checks the initialization parameters for the VXLIB_addSquare kernel
VXLIB_STATUS
VXLIB_addSquare_init_checkParams(VXLIB_kernelHandle              handle,
                                 const VXLIB_bufParams2D_t      *bufParamsIn0,
                                 const VXLIB_bufParams2D_t      *bufParamsIn1,
                                 const VXLIB_bufParams2D_t      *bufParamsOut,
                                 const VXLIB_addSquare_InitArgs *pKerInitArgs)
{
   VXLIB_STATUS status = VXLIB_SUCCESS;

   // obtain input0 buffer parameters
   uint32_t dTypeIn0  = bufParamsIn0->data_type;
   uint32_t widthIn0  = bufParamsIn0->dim_x;
   uint32_t heightIn0 = bufParamsIn0->dim_y;
   uint32_t strideIn0 = bufParamsIn0->stride_y;

   // obtain input1 buffer parameters
   uint32_t dTypeIn1  = bufParamsIn1->data_type;
   uint32_t widthIn1  = bufParamsIn1->dim_x;
   uint32_t heightIn1 = bufParamsIn1->dim_y;
   uint32_t strideIn1 = bufParamsIn1->stride_y;

   // obtain output buffer parameters
   uint32_t dTypeOut  = bufParamsOut->data_type;
   uint32_t widthOut  = bufParamsOut->dim_x;
   uint32_t heightOut = bufParamsOut->dim_y;
   uint32_t strideOut = bufParamsOut->stride_y;

   uint32_t strideIn0Elements = strideIn0 / VXLIB_sizeof(bufParamsIn0->data_type);
   uint32_t strideIn1Elements = strideIn1 / VXLIB_sizeof(bufParamsIn1->data_type);
   uint32_t strideOutElements = strideOut / VXLIB_sizeof(bufParamsOut->data_type);

   // check for dimensions and datatype combinations
   if (handle == NULL) {
      status = VXLIB_ERR_NULL_POINTER;
   }
   else if ((widthIn0 != widthIn1) || (widthIn0 != widthOut) || (heightIn0 != heightIn1) || (heightIn0 != heightOut)) {
      status = VXLIB_ERR_INVALID_DIMENSION;
   }
   else if (strideIn0Elements < widthIn0 || strideIn1Elements < widthIn1 || strideOutElements < widthOut) {
      status = VXLIB_ERR_NOT_EQUAL_WIDTH_STRIDE;
   }
   else if (!(VXLIB_ADDSQUARE_I8U_I16S_O16S)) {
      status = VXLIB_ERR_INVALID_TYPE;
   }
   else {
      status = VXLIB_SUCCESS;
   }

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_addSquare_exec_checkParams */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method checks the execution parameters for the VXLIB_addSquare kernel
VXLIB_STATUS VXLIB_addSquare_exec_checkParams(VXLIB_kernelHandle handle,
                                              const void *restrict pIn0,
                                              const void *restrict pIn1,
                                              const void *restrict pOut,
                                              const uint32_t shift)
{
   VXLIB_STATUS status;

#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_addSquare_exec_checkParams\n");
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
/* VXLIB_addSquare_init */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method is the user-level initialization function for the VXLIB_addSquare kernel
VXLIB_STATUS VXLIB_addSquare_init(VXLIB_kernelHandle              handle,
                                  VXLIB_bufParams2D_t            *bufParamsIn0,
                                  VXLIB_bufParams2D_t            *bufParamsIn1,
                                  VXLIB_bufParams2D_t            *bufParamsOut,
                                  const VXLIB_addSquare_InitArgs *pKerInitArgs)
{
   VXLIB_STATUS              status       = VXLIB_SUCCESS;
   VXLIB_addSquare_PrivArgs *pKerPrivArgs = (VXLIB_addSquare_PrivArgs *) handle;

   // copy pKerinitArgs into pKerPrivargs
   pKerPrivArgs->pKerInitArgs = *pKerInitArgs;

   // set width and height of image
   pKerPrivArgs->width  = bufParamsIn0->dim_x;
   pKerPrivArgs->height = bufParamsIn0->dim_y;

   // compute stride in elements as SE/SA params are set to work with given element type
   pKerPrivArgs->strideIn0Elements = bufParamsIn0->stride_y / VXLIB_sizeof(bufParamsIn0->data_type);
   pKerPrivArgs->strideIn1Elements = bufParamsIn1->stride_y / VXLIB_sizeof(bufParamsIn1->data_type);
   pKerPrivArgs->strideOutElements = bufParamsOut->stride_y / VXLIB_sizeof(bufParamsOut->data_type);

#if VXLIB_DEBUGPRINT
   printf("VXLIB_DEBUGPRINT Enter VXLIB_addSquare_init\n");
#endif

   // obtain buffer datatypes
   uint32_t dTypeIn0 = bufParamsIn0->data_type;
   uint32_t dTypeIn1 = bufParamsIn1->data_type;
   uint32_t dTypeOut = bufParamsOut->data_type;

   // determine natural C  vs optimized function call
   if (pKerInitArgs->funcStyle == VXLIB_FUNCTION_NATC) {

      // set function pointer for natural C function with appropriate template parameters based on datatypes

      if (VXLIB_ADDSQUARE_I8U_I16S_O16S) {
         pKerPrivArgs->execute = VXLIB_addSquare_exec_cn<VXLIB_ADDSQUARE_TYPENAME_I8U_I16S_O16S>;
      }
      else {
         status = VXLIB_ERR_INVALID_TYPE;
      }
   }
   else { // Optimized function

      // set function pointer for natural C function with appropriate template parameters based on datatypes

      if (VXLIB_ADDSQUARE_I8U_I16S_O16S) {
         pKerPrivArgs->execute = VXLIB_addSquare_exec_ci<VXLIB_ADDSQUARE_TYPENAME_I8U_I16S_O16S>;
         status = VXLIB_addSquare_init_ci<VXLIB_ADDSQUARE_DTYPE_I8U_I16S_O16S>(handle, bufParamsIn0, bufParamsIn1,
                                                                               bufParamsOut, pKerInitArgs);
      }
      else {
         status = VXLIB_ERR_INVALID_TYPE;
      }
   }

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_addSquare_exec */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method is the user-level execution function for the VXLIB_addSquare kernel
VXLIB_STATUS VXLIB_addSquare_exec(VXLIB_kernelHandle handle,
                                  void *restrict pIn0,
                                  void *restrict pIn1,
                                  void *restrict pOut,
                                  uint32_t shift)
{
   VXLIB_STATUS status;

#if VXLIB_DEBUGPRINT
   printf("VXLIB_DEBUGPRINT Enter VXLIB_addSquare_exec\n");
#endif

   VXLIB_addSquare_PrivArgs *pKerPrivArgs = (VXLIB_addSquare_PrivArgs *) handle;

   status = pKerPrivArgs->execute(handle, pIn0, pIn1, pOut, shift);

   return status;
}
