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

#include "VXLIB_bufParams.h"
#include "VXLIB_convolve_priv.h"
#include "VXLIB_types.h"
#include <cstdint>

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_convolve_getHandleSize */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method calculates and returns the size of the handle for the VXLIB_convolve kernel
int32_t VXLIB_convolve_getHandleSize(VXLIB_convolve_InitArgs *pKerInitArgs)
{
   int32_t privBufSize = sizeof(VXLIB_convolve_PrivArgs);
   return privBufSize;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_convolve_init_checkParams */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method checks the initialization parameters for the VXLIB_convolve kernel
VXLIB_STATUS
VXLIB_convolve_init_checkParams(VXLIB_kernelHandle             handle,
                                const VXLIB_bufParams2D_t     *bufParamsIn,
                                const VXLIB_bufParams2D_t     *bufParamsFilter,
                                const VXLIB_bufParams2D_t     *bufParamsOut,
                                const VXLIB_convolve_InitArgs *pKerInitArgs)
{
   VXLIB_STATUS status = VXLIB_SUCCESS;

   // obtain input buffer parameters
   uint32_t dTypeIn  = bufParamsIn->data_type;
   uint32_t widthIn  = bufParamsIn->dim_x;
   uint32_t heightIn = bufParamsIn->dim_y;
   uint32_t strideIn = bufParamsIn->stride_y;

   // get filter parameters
   uint32_t dTypeFilter  = bufParamsFilter->data_type;
   int32_t  filterHeight = pKerInitArgs->filterHeight;
   int32_t  filterWidth  = pKerInitArgs->filterWidth;
   size_t   scale        = pKerInitArgs->scale;

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

   bool isNotPadded = (padLeft == 0) && (padRight == 0) && (padTop == 0) && (padBottom == 0);

   uint32_t strideInElements  = bufParamsIn->stride_y / VXLIB_sizeof(bufParamsIn->data_type);
   uint32_t strideOutElements = bufParamsOut->stride_y / VXLIB_sizeof(bufParamsOut->data_type);

   // check for null handles
   if (handle == NULL) {
      status = VXLIB_ERR_NULL_POINTER;
   }

   // check for valid datatype combinations
   else if ((!VXLIB_CONVOLVE_I8U_C16S_O8U) && (!VXLIB_CONVOLVE_I8U_C16S_O16S)) {
      status = VXLIB_ERR_CONVOLVE_INVALID_TYPE_COMBINATION;
   }

   // check for valid shift parameter
   else if (scale > 31U) {
      status = VXLIB_ERR_CONVOLVE_INVALID_SHIFT;
   }

   // disqualify padded implementation
   else if (!isNotPadded) {
      status = VXLIB_ERR_CONVOLVE_PADDED_NOT_IMPLEMENTED;
   }

   // check valid dimensions; input height, width must be greater, equal to filter size for non padded implementation
   else if ((widthIn < widthOut) || (heightIn - filterHeight + 1 != heightOut) || (strideInElements < widthIn) ||
            (strideOutElements < widthOut) || (widthIn < filterWidth) || (heightIn < filterHeight)) {
      status = VXLIB_ERR_CONVOLVE_INVALID_DIMENSION;
   }

   else {
      status = VXLIB_SUCCESS;
   }

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_convolve_exec_checkParams */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method checks the execution parameters for the VXLIB_convolve kernel
VXLIB_STATUS
VXLIB_convolve_exec_checkParams(VXLIB_kernelHandle handle,
                                const void *restrict pIn,
                                const void *restrict pFilter,
                                const void *restrict pOut)
{
   VXLIB_STATUS status;

#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_convolve_exec_checkParams\n");
#endif
   if ((handle == NULL) || (pIn == NULL) || (pFilter == NULL) || (pOut == NULL)) {
      status = VXLIB_ERR_NULL_POINTER;
   }
   else {
      status = VXLIB_SUCCESS;
   }

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_convolve_init */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method is the user-level initialization function for the VXLIB_convolve kernel
VXLIB_STATUS VXLIB_convolve_init(VXLIB_kernelHandle             handle,
                                 VXLIB_bufParams2D_t           *bufParamsIn,
                                 VXLIB_bufParams2D_t           *bufParamsFilter,
                                 VXLIB_bufParams2D_t           *bufParamsOut,
                                 const VXLIB_convolve_InitArgs *pKerInitArgs)
{
   VXLIB_STATUS             status       = VXLIB_SUCCESS;
   VXLIB_convolve_PrivArgs *pKerPrivArgs = (VXLIB_convolve_PrivArgs *) handle;

   // copy pKerinitArgs into pKerPrivargs
   pKerPrivArgs->pKerInitArgs = *pKerInitArgs;

   // set priv args
   pKerPrivArgs->width  = bufParamsIn->dim_x;
   pKerPrivArgs->height = bufParamsIn->dim_y;

   // compute stride in elements as SE/SA params are set to work with given element type
   pKerPrivArgs->strideInElements  = bufParamsIn->stride_y / VXLIB_sizeof(bufParamsIn->data_type);
   pKerPrivArgs->strideOutElements = bufParamsOut->stride_y / VXLIB_sizeof(bufParamsOut->data_type);

#if VXLIB_DEBUGPRINT
   printf("VXLIB_DEBUGPRINT Enter VXLIB_convolve_init\n");
#endif

   // obtain buffer datatypes
   uint32_t dTypeIn     = bufParamsIn->data_type;
   uint32_t dTypeFilter = bufParamsFilter->data_type;
   uint32_t dTypeOut    = bufParamsOut->data_type;

   // determine natural C  vs optimized function call
   if (pKerInitArgs->funcStyle == VXLIB_FUNCTION_NATC) {

      // set function pointer for natural C function with appropriate template parameters based on datatypes

      if (VXLIB_CONVOLVE_I8U_C16S_O16S) {
         pKerPrivArgs->execute = VXLIB_convolve_exec_cn<VXLIB_CONVOLVE_TYPENAME_I8U_C16S_O16S>;
      }

      else if (VXLIB_CONVOLVE_I8U_C16S_O8U) {
         pKerPrivArgs->execute = VXLIB_convolve_exec_cn<VXLIB_CONVOLVE_TYPENAME_I8U_C16S_O8U>;
      }

      else {
         status = VXLIB_ERR_INVALID_TYPE;
      }
   }
   else { // Optimized function

      // set function pointer for natural C function with appropriate template parameters based on datatypes

      if (VXLIB_CONVOLVE_I8U_C16S_O8U) {
         status = VXLIB_convolve_init_ci<VXLIB_CONVOLVE_DTYPE_I8U_C16S_O8U>(handle, bufParamsIn, bufParamsFilter,
                                                                            bufParamsOut, pKerInitArgs);
         pKerPrivArgs->execute = VXLIB_convolve_exec_ci<VXLIB_CONVOLVE_TYPENAME_I8U_C16S_O8U>;
      }

      else if (VXLIB_CONVOLVE_I8U_C16S_O16S) {
         status = VXLIB_convolve_init_ci<VXLIB_CONVOLVE_DTYPE_I8U_C16S_O16S>(handle, bufParamsIn, bufParamsFilter,
                                                                             bufParamsOut, pKerInitArgs);
         pKerPrivArgs->execute = VXLIB_convolve_exec_ci<VXLIB_CONVOLVE_TYPENAME_I8U_C16S_O16S>;
      }

      else {
         status = VXLIB_ERR_INVALID_TYPE;
      }
   }

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_convolve_exec */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method is the user-level execution function for the VXLIB_convolve kernel
VXLIB_STATUS
VXLIB_convolve_exec(VXLIB_kernelHandle handle, void *restrict pIn, void *restrict pFilter, void *restrict pOut)
{
   VXLIB_STATUS status;

#if VXLIB_DEBUGPRINT
   printf("VXLIB_DEBUGPRINT Enter VXLIB_convolve_exec\n");
#endif

   VXLIB_convolve_PrivArgs *pKerPrivArgs = (VXLIB_convolve_PrivArgs *) handle;

   status = pKerPrivArgs->execute(handle, pIn, pFilter, pOut);

   return status;
}
