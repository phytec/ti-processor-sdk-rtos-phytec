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

#include "VXLIB_thresholdBinary_priv.h"

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_thresholdBinary_getHandleSize */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method calculates and returns the size of the handle for the VXLIB_thresholdBinary kernel
int32_t VXLIB_thresholdBinary_getHandleSize(VXLIB_thresholdBinary_InitArgs *pKerInitArgs)
{
   int32_t privBufSize = sizeof(VXLIB_thresholdBinary_PrivArgs);
   return privBufSize;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_thresholdBinary_init_checkParams */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method checks the initialization parameters for the VXLIB_thresholdBinary kernel
VXLIB_STATUS
VXLIB_thresholdBinary_init_checkParams(VXLIB_kernelHandle                    handle,
                                       const VXLIB_bufParams2D_t *           bufParamsIn,
                                       const VXLIB_bufParams2D_t *           bufParamsOut,
                                       const VXLIB_thresholdBinary_InitArgs *pKerInitArgs)
{
   VXLIB_STATUS status = VXLIB_SUCCESS;

   if ((handle == NULL) || (bufParamsIn == NULL) || (bufParamsOut == NULL) || (pKerInitArgs == NULL)) {
      status = VXLIB_ERR_NULL_POINTER;
   }

   if (status == VXLIB_SUCCESS) {
      // check for dimensions and datatype combinations
      // obtain input1 buffer parameters
      uint32_t dTypeIn  = bufParamsIn->data_type;
      uint32_t widthIn  = bufParamsIn->dim_x;
      uint32_t heightIn = bufParamsIn->dim_y;
      uint32_t  strideIn = bufParamsIn->stride_y;

      // obtain output buffer parameters
      uint32_t dTypeOut  = bufParamsOut->data_type;
      uint32_t widthOut  = bufParamsOut->dim_x;
      uint32_t heightOut = bufParamsOut->dim_y;
      uint32_t  strideOut = bufParamsOut->stride_y;

      uint32_t strideInElements  = strideIn / VXLIB_sizeof(dTypeIn);
      uint32_t strideOutElements = strideOut / VXLIB_sizeof(dTypeOut);

      if ((widthIn != widthOut) || (heightIn != heightOut)) {
         status = VXLIB_ERR_INVALID_DIMENSION;
      }
      else if ((strideInElements < widthIn) || (strideOutElements < widthOut)) {
         status = VXLIB_ERR_NOT_EQUAL_WIDTH_STRIDE;
      }
      else if (!(VXLIB_THRESHOLDBINARY_I8U_O8U) && !(VXLIB_THRESHOLDBINARY_I8S_O8S) &&
               !(VXLIB_THRESHOLDBINARY_I16U_O16U) && !(VXLIB_THRESHOLDBINARY_I16S_O16S)) {
         status = VXLIB_ERR_INVALID_TYPE;
      }
      else {
         status = VXLIB_SUCCESS;
      }
   }

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_thresholdBinary_exec_checkParams */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method checks the execution parameters for the VXLIB_thresholdBinary kernel
VXLIB_STATUS VXLIB_thresholdBinary_exec_checkParams(VXLIB_kernelHandle   handle,
                                                    const void *restrict pIn,
                                                    const void *restrict pOut,
                                                    const void *restrict pThresholdVal,
                                                    const void *restrict pTrueVal,
                                                    const void *restrict pFalseVal)
{
   VXLIB_STATUS status;

#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_thresholdBinary_exec_checkParams\n");
#endif
   if ((handle == NULL) || (pIn == NULL) || (pOut == NULL) || (pThresholdVal == NULL) || (pTrueVal == NULL) ||
       (pFalseVal == NULL)) {
      status = VXLIB_ERR_NULL_POINTER;
   }
   else {
      status = VXLIB_SUCCESS;
   }

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_thresholdBinary_init */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method is the user-level initialization function for the VXLIB_thresholdBinary kernel
VXLIB_STATUS VXLIB_thresholdBinary_init(VXLIB_kernelHandle                    handle,
                                        VXLIB_bufParams2D_t *                 bufParamsIn,
                                        VXLIB_bufParams2D_t *                 bufParamsOut,
                                        const VXLIB_thresholdBinary_InitArgs *pKerInitArgs)
{
   VXLIB_STATUS                    status       = VXLIB_SUCCESS;
   VXLIB_thresholdBinary_PrivArgs *pKerPrivArgs = (VXLIB_thresholdBinary_PrivArgs *) handle;

   // copy pKerinitArgs into pKerPrivargs
   pKerPrivArgs->pKerInitArgs = *pKerInitArgs;

   // set width and height of image
   pKerPrivArgs->width  = bufParamsIn->dim_x;
   pKerPrivArgs->height = bufParamsIn->dim_y;

   // compute stride in elements as SE/SA params are set to work with given element type
   pKerPrivArgs->strideInElements  = bufParamsIn->stride_y / VXLIB_sizeof(bufParamsIn->data_type);
   pKerPrivArgs->strideOutElements = bufParamsOut->stride_y / VXLIB_sizeof(bufParamsOut->data_type);

#if VXLIB_DEBUGPRINT
   printf("VXLIB_DEBUGPRINT Enter VXLIB_thresholdBinary_init\n");
#endif

   // obtain buffer datatypes
   uint32_t dTypeIn  = bufParamsIn->data_type;
   uint32_t dTypeOut = bufParamsOut->data_type;

   // determine natural C  vs optimized function call
   if (pKerInitArgs->funcStyle == VXLIB_FUNCTION_NATC) {

      // set function pointer for natural C function with appropriate template parameters based on datatypes
      if (VXLIB_THRESHOLDBINARY_I8U_O8U) {
         pKerPrivArgs->execute = VXLIB_thresholdBinary_exec_cn<VXLIB_THRESHOLDBINARY_TYPENAME_I8U_O8U>;
      }
      else if (VXLIB_THRESHOLDBINARY_I8S_O8S) {
         pKerPrivArgs->execute = VXLIB_thresholdBinary_exec_cn<VXLIB_THRESHOLDBINARY_TYPENAME_I8S_O8S>;
      }
      else if (VXLIB_THRESHOLDBINARY_I16U_O16U) {
         pKerPrivArgs->execute = VXLIB_thresholdBinary_exec_cn<VXLIB_THRESHOLDBINARY_TYPENAME_I16U_O16U>;
      }
      else if (VXLIB_THRESHOLDBINARY_I16S_O16S) {
         pKerPrivArgs->execute = VXLIB_thresholdBinary_exec_cn<VXLIB_THRESHOLDBINARY_TYPENAME_I16S_O16S>;
      }
      else {
         status = VXLIB_ERR_INVALID_TYPE;
      }
   }
   else { // Optimized function

      // set function pointer for natural C function with appropriate template parameters based on datatypes
      if (VXLIB_THRESHOLDBINARY_I8U_O8U) {
         pKerPrivArgs->execute = VXLIB_thresholdBinary_exec_ci<VXLIB_THRESHOLDBINARY_TYPENAME_I8U_O8U>;
         status = VXLIB_thresholdBinary_init_ci<VXLIB_THRESHOLDBINARY_DTYPE_I8U_O8U>(handle, bufParamsIn, bufParamsOut,
                                                                                     pKerInitArgs);
      }
      else if (VXLIB_THRESHOLDBINARY_I8S_O8S) {
         pKerPrivArgs->execute = VXLIB_thresholdBinary_exec_ci<VXLIB_THRESHOLDBINARY_TYPENAME_I8S_O8S>;
         status = VXLIB_thresholdBinary_init_ci<VXLIB_THRESHOLDBINARY_DTYPE_I8S_O8S>(handle, bufParamsIn, bufParamsOut,
                                                                                     pKerInitArgs);
      }
      else if (VXLIB_THRESHOLDBINARY_I16U_O16U) {
         pKerPrivArgs->execute = VXLIB_thresholdBinary_exec_ci<VXLIB_THRESHOLDBINARY_TYPENAME_I16U_O16U>;
         status = VXLIB_thresholdBinary_init_ci<VXLIB_THRESHOLDBINARY_DTYPE_I16U_O16U>(handle, bufParamsIn,
                                                                                       bufParamsOut, pKerInitArgs);
      }
      else if (VXLIB_THRESHOLDBINARY_I16S_O16S) {
         pKerPrivArgs->execute = VXLIB_thresholdBinary_exec_ci<VXLIB_THRESHOLDBINARY_TYPENAME_I16S_O16S>;
         status = VXLIB_thresholdBinary_init_ci<VXLIB_THRESHOLDBINARY_DTYPE_I16S_O16S>(handle, bufParamsIn,
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
/* VXLIB_thresholdBinary_exec */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method is the user-level execution function for the VXLIB_thresholdBinary kernel
VXLIB_STATUS VXLIB_thresholdBinary_exec(VXLIB_kernelHandle handle,
                                        void *restrict     pIn,
                                        void *restrict     pOut,
                                        void *restrict     pThresholdVal,
                                        void *restrict     pTrueVal,
                                        void *restrict     pFalseVal)
{
   VXLIB_STATUS status;

#if VXLIB_DEBUGPRINT
   printf("VXLIB_DEBUGPRINT Enter VXLIB_thresholdBinary_exec\n");
#endif

   VXLIB_thresholdBinary_PrivArgs *pKerPrivArgs = (VXLIB_thresholdBinary_PrivArgs *) handle;

   status = pKerPrivArgs->execute(handle, pIn, pOut, pThresholdVal, pTrueVal, pFalseVal);

   return status;
}

/* ======================================================================== */
/*  End of file:  VXLIB_thresholdBinary.cpp                                        */
/* ======================================================================== */
