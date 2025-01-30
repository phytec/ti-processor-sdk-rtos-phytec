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

#include "VXLIB_gaussian_priv.h"
#include "VXLIB_types.h"

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_gaussian_getHandleSize */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method calculates and returns the size of the handle for the VXLIB_gaussian kernel
int32_t VXLIB_gaussian_getHandleSize(VXLIB_gaussian_InitArgs *pKerInitArgs)
{
   int32_t privBufSize = sizeof(VXLIB_gaussian_PrivArgs);
   return privBufSize;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_gaussian_init_checkParams */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method checks the initialization parameters for the VXLIB_gaussian kernel
VXLIB_STATUS
VXLIB_gaussian_init_checkParams(VXLIB_kernelHandle             handle,
                                const VXLIB_bufParams2D_t     *bufParamsIn,
                                const VXLIB_bufParams2D_t     *bufParamsOut,
                                const VXLIB_gaussian_InitArgs *pKerInitArgs)
{
   VXLIB_STATUS status = VXLIB_SUCCESS;

   // check for null handles
   if (handle == NULL || bufParamsIn == NULL || bufParamsOut == NULL || pKerInitArgs == NULL) {
      status = VXLIB_ERR_NULL_POINTER;
   }

   // check for valid datatype combinations
   else if ((bufParamsIn->data_type != VXLIB_UINT8) || (bufParamsOut->data_type != VXLIB_UINT8)) {
      status = VXLIB_ERR_INVALID_TYPE;
   }

   // disqualify padded implementation
   else if (!((pKerInitArgs->padLeft == 0) && (pKerInitArgs->padRight == 0) && (pKerInitArgs->padTop == 0) &&
              (pKerInitArgs->padBottom == 0))) {
      status = VXLIB_ERR_NOT_IMPLEMENTED;
   }

   // check valid dimensions; input height, width must be greater, equal to filter size for non padded implementation
   else if ((bufParamsIn->dim_x < bufParamsOut->dim_x) ||
            (bufParamsIn->dim_y - pKerInitArgs->filterSize + 1 != bufParamsOut->dim_y) ||
            (bufParamsIn->stride_y / VXLIB_sizeof(bufParamsIn->data_type) < bufParamsIn->dim_x) ||
            (bufParamsOut->stride_y / VXLIB_sizeof(bufParamsOut->data_type) < bufParamsOut->dim_x) ||
            (bufParamsIn->dim_x < pKerInitArgs->filterSize) || (bufParamsIn->dim_y < pKerInitArgs->filterSize)) {
      status = VXLIB_ERR_INVALID_DIMENSION;
   }

   // shift not applicable for 3x3 gaussian filter (any shift given for 3x3 will be ignored)
   else if (pKerInitArgs->filterSize == VXLIB_GAUSSIAN_FILTER_5x5 &&
            pKerInitArgs->shift > VXLIB_GAUSSIAN_FILTER_5x5_MAX_SHIFT) {
      status = VXLIB_ERR_INVALID_SHIFT;
   }

   else {
      status = VXLIB_SUCCESS;
   }

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_gaussian_exec_checkParams */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method checks the execution parameters for the VXLIB_gaussian kernel
VXLIB_STATUS
VXLIB_gaussian_exec_checkParams(VXLIB_kernelHandle handle, const void *restrict pIn, const void *restrict pOut)
{
   VXLIB_STATUS status;

#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_gaussian_exec_checkParams\n");
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
/* VXLIB_gaussian_init */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method is the user-level initialization function for the VXLIB_gaussian kernel
VXLIB_STATUS VXLIB_gaussian_init(VXLIB_kernelHandle             handle,
                                 VXLIB_bufParams2D_t           *bufParamsIn,
                                 VXLIB_bufParams2D_t           *bufParamsOut,
                                 const VXLIB_gaussian_InitArgs *pKerInitArgs)
{
   VXLIB_STATUS             status       = VXLIB_SUCCESS;
   VXLIB_gaussian_PrivArgs *pKerPrivArgs = (VXLIB_gaussian_PrivArgs *) handle;

   // copy pKerinitArgs into pKerPrivargs
   pKerPrivArgs->pKerInitArgs = *pKerInitArgs;

   // set priv args
   pKerPrivArgs->width  = bufParamsIn->dim_x;
   pKerPrivArgs->height = bufParamsIn->dim_y;

   // compute stride in elements as SE/SA params are set to work with given element type
   pKerPrivArgs->strideInElements  = bufParamsIn->stride_y / VXLIB_sizeof(bufParamsIn->data_type);
   pKerPrivArgs->strideOutElements = bufParamsOut->stride_y / VXLIB_sizeof(bufParamsOut->data_type);

#if VXLIB_DEBUGPRINT
   printf("VXLIB_DEBUGPRINT Enter VXLIB_gaussian_init\n");
#endif

   // obtain buffer datatypes
   uint32_t dTypeIn  = bufParamsIn->data_type;
   uint32_t dTypeOut = bufParamsOut->data_type;

   // determine natural C  vs optimized function call
   if (pKerInitArgs->funcStyle == VXLIB_FUNCTION_NATC) {

      // set function pointer for natural C function with appropriate template parameters based on datatypes

      if (VXLIB_GAUSSIAN_I8U_O8U) {
         pKerPrivArgs->execute = VXLIB_gaussian_exec_cn<VXLIB_GAUSSIAN_TYPENAME_I8U_O8U>;
      }
      else {
         status = VXLIB_ERR_INVALID_TYPE;
      }
   }
   else { // Optimized function

      // set function pointer for natural C function with appropriate template parameters based on datatypes

      if (VXLIB_GAUSSIAN_I8U_O8U) {
         status = VXLIB_gaussian_init_ci<VXLIB_GAUSSIAN_DTYPE_I8U_O8U>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
         pKerPrivArgs->execute = VXLIB_gaussian_exec_ci<VXLIB_GAUSSIAN_TYPENAME_I8U_O8U>;
      }
      else {
         status = VXLIB_ERR_INVALID_TYPE;
      }
   }

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_gaussian_exec */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method is the user-level execution function for the VXLIB_gaussian kernel
VXLIB_STATUS
VXLIB_gaussian_exec(VXLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   VXLIB_STATUS status;

#if VXLIB_DEBUGPRINT
   printf("VXLIB_DEBUGPRINT Enter VXLIB_gaussian_exec\n");
#endif

   VXLIB_gaussian_PrivArgs *pKerPrivArgs = (VXLIB_gaussian_PrivArgs *) handle;

   status = pKerPrivArgs->execute(handle, pIn, pOut);

   return status;
}
