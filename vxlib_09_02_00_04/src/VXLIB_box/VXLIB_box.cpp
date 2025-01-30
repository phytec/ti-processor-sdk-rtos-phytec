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

#include "VXLIB_box_priv.h"
#include "VXLIB_types.h"
#include <cstdint>

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_box_getHandleSize */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method calculates and returns the size of the handle for the VXLIB_box kernel
int32_t VXLIB_box_getHandleSize(VXLIB_box_InitArgs *pKerInitArgs)
{
   int32_t privBufSize = sizeof(VXLIB_box_PrivArgs);
   return privBufSize;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_box_init_checkParams */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method checks the initialization parameters for the VXLIB_box kernel
VXLIB_STATUS
VXLIB_box_init_checkParams(VXLIB_kernelHandle         handle,
                           const VXLIB_bufParams2D_t *bufParamsIn,
                           const VXLIB_bufParams2D_t *bufParamsOut,
                           const VXLIB_box_InitArgs  *pKerInitArgs)
{
   VXLIB_STATUS status = VXLIB_SUCCESS;

   // obtain input buffer parameters
   uint32_t dTypeIn  = bufParamsIn->data_type;
   uint32_t widthIn  = bufParamsIn->dim_x;
   uint32_t heightIn = bufParamsIn->dim_y;
   uint32_t strideIn = bufParamsIn->stride_y;

   // obtain output buffer parameters
   uint32_t dTypeOut  = bufParamsOut->data_type;
   uint32_t widthOut  = bufParamsOut->dim_x;
   uint32_t heightOut = bufParamsOut->dim_y;
   uint32_t strideOut = bufParamsOut->stride_y;

   // get init arg
   size_t padLeft   = pKerInitArgs->padLeft;
   size_t padRight  = pKerInitArgs->padRight;
   size_t padTop    = pKerInitArgs->padTop;
   size_t padBottom = pKerInitArgs->padBottom;

   size_t filterDim = pKerInitArgs->filterSize;

   bool isNotPadded = (padLeft == 0) && (padRight == 0) && (padTop == 0) && (padBottom == 0);

   uint32_t strideInElements  = bufParamsIn->stride_y / VXLIB_sizeof(bufParamsIn->data_type);
   uint32_t strideOutElements = bufParamsOut->stride_y / VXLIB_sizeof(bufParamsOut->data_type);

   // check for null handles
   if (handle == NULL) {
      status = VXLIB_ERR_NULL_POINTER;
   }

   // check for valid datatype combinations
   else if ((dTypeIn != VXLIB_UINT8) || (dTypeOut != VXLIB_UINT8)) {
      status = VXLIB_ERR_INVALID_TYPE;
   }

   // disqualify padded implementation
   else if (!isNotPadded) {
      status = VXLIB_ERR_NOT_IMPLEMENTED;
   }

   // check valid dimensions; input height, width must be greater, equal to filter size for non padded implementation
   else if ((widthIn < widthOut) || (heightIn - filterDim + 1 != heightOut) || (strideInElements < widthIn) ||
            (strideOutElements < widthOut) || (widthIn < filterDim) || (heightIn < filterDim)) {
      status = VXLIB_ERR_INVALID_DIMENSION;
   }

   else {
      status = VXLIB_SUCCESS;
   }

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_box_exec_checkParams */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method checks the execution parameters for the VXLIB_box kernel
VXLIB_STATUS VXLIB_box_exec_checkParams(VXLIB_kernelHandle handle, const void *restrict pIn, const void *restrict pOut)
{
   VXLIB_STATUS status;

#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_box_exec_checkParams\n");
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
/* VXLIB_box_init */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method is the user-level initialization function for the VXLIB_box kernel
VXLIB_STATUS VXLIB_box_init(VXLIB_kernelHandle        handle,
                            VXLIB_bufParams2D_t      *bufParamsIn,
                            VXLIB_bufParams2D_t      *bufParamsOut,
                            const VXLIB_box_InitArgs *pKerInitArgs)
{
   VXLIB_STATUS        status       = VXLIB_SUCCESS;
   VXLIB_box_PrivArgs *pKerPrivArgs = (VXLIB_box_PrivArgs *) handle;

   // copy pKerinitArgs into pKerPrivargs
   pKerPrivArgs->pKerInitArgs = *pKerInitArgs;

   // set priv args
   pKerPrivArgs->width  = bufParamsIn->dim_x;
   pKerPrivArgs->height = bufParamsIn->dim_y;

   // compute stride in elements as SE/SA params are set to work with given element type
   pKerPrivArgs->strideInElements  = bufParamsIn->stride_y / VXLIB_sizeof(bufParamsIn->data_type);
   pKerPrivArgs->strideOutElements = bufParamsOut->stride_y / VXLIB_sizeof(bufParamsOut->data_type);

#if VXLIB_DEBUGPRINT
   printf("VXLIB_DEBUGPRINT Enter VXLIB_box_init\n");
#endif

   // obtain buffer datatypes
   uint32_t dTypeIn  = bufParamsIn->data_type;
   uint32_t dTypeOut = bufParamsOut->data_type;

   // determine natural C  vs optimized function call
   if (pKerInitArgs->funcStyle == VXLIB_FUNCTION_NATC) {

      // set function pointer for natural C function with appropriate template parameters based on datatypes

      if (VXLIB_BOX_I8U_O8U) {
         pKerPrivArgs->execute = VXLIB_box_exec_cn<VXLIB_BOX_TYPENAME_I8U_O8U>;
      }
      else {
         status = VXLIB_ERR_INVALID_TYPE;
      }
   }
   else { // Optimized function

      // set function pointer for natural C function with appropriate template parameters based on datatypes

      if (VXLIB_BOX_I8U_O8U) {
         status = VXLIB_box_init_ci<VXLIB_BOX_DTYPE_I8U_O8U>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
         pKerPrivArgs->execute = VXLIB_box_exec_ci<VXLIB_BOX_TYPENAME_I8U_O8U>;
      }
      else {
         status = VXLIB_ERR_INVALID_TYPE;
      }
   }

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_box_exec */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method is the user-level execution function for the VXLIB_box kernel
VXLIB_STATUS
VXLIB_box_exec(VXLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   VXLIB_STATUS status;

#if VXLIB_DEBUGPRINT
   printf("VXLIB_DEBUGPRINT Enter VXLIB_box_exec\n");
#endif

   VXLIB_box_PrivArgs *pKerPrivArgs = (VXLIB_box_PrivArgs *) handle;

   status = pKerPrivArgs->execute(handle, pIn, pOut);

   return status;
}
