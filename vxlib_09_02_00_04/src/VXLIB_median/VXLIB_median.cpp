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

#include "VXLIB_median_priv.h"

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_median_getHandleSize */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method calculates and returns the size of the handle for the VXLIB_median kernel
int32_t VXLIB_median_getHandleSize(VXLIB_median_InitArgs *pKerInitArgs)
{
   int32_t privBufSize = sizeof(VXLIB_median_PrivArgs);
   return privBufSize;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_median_init_checkParams */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method checks the initialization parameters for the VXLIB_median kernel
VXLIB_STATUS
VXLIB_median_init_checkParams(VXLIB_kernelHandle           handle,
                              const VXLIB_bufParams2D_t   *bufParamsIn,
                              const VXLIB_bufParams2D_t   *bufParamsMask,
                              const VXLIB_bufParams2D_t   *bufParamsOut,
                              const VXLIB_bufParams2D_t   *bufParamsScratch,
                              const VXLIB_median_InitArgs *pKerInitArgs)
{
   VXLIB_DEBUGPRINTFN(0, "%s\n", "Enter function");
   VXLIB_STATUS status = VXLIB_SUCCESS;

   // check for dimensions and datatype combinations
   if ((handle == NULL) || (bufParamsIn == NULL) || (bufParamsMask == NULL) || (bufParamsOut == NULL) ||
       (bufParamsScratch == NULL) || (pKerInitArgs == NULL)) {
      status = VXLIB_ERR_NULL_POINTER;
   }

   if (status == VXLIB_SUCCESS) {
      // obtain input buffer parameters
      uint32_t dType    = bufParamsIn->data_type;
      uint32_t widthIn  = bufParamsIn->dim_x;
      uint32_t heightIn = bufParamsIn->dim_y;
      uint32_t strideIn = bufParamsIn->stride_y;

      // obtain mask buffer parameters
      uint32_t widthMask  = bufParamsMask->dim_x;
      uint32_t heightMask = bufParamsMask->dim_y;
      uint32_t strideMask = bufParamsMask->stride_y;

      // obtain output buffer parameters
      uint32_t widthOut  = bufParamsOut->dim_x;
      uint32_t heightOut = bufParamsOut->dim_y;
      uint32_t strideOut = bufParamsOut->stride_y;

      uint32_t strideInElements = strideIn / VXLIB_sizeof(dType);
      uint32_t strideMaskElements = strideMask / VXLIB_sizeof(dType);
      uint32_t strideOutElements = strideOut / VXLIB_sizeof(dType);

      if (strideInElements < widthIn || strideMaskElements < widthMask || strideOutElements < widthOut) {
         status = VXLIB_ERR_INVALID_DIMENSION;
      }
      else if (!(VXLIB_MEDIAN_8U) && !(VXLIB_MEDIAN_8S) && !(VXLIB_MEDIAN_16U) && !(VXLIB_MEDIAN_16S)) {
         status = VXLIB_ERR_INVALID_TYPE;
      }
      else if ((widthOut != (widthIn - (widthMask - 1u))) || (heightOut != (heightIn - (heightMask - 1u)))) {
         status = VXLIB_ERR_INVALID_DIMENSION;
      }
      else if (heightMask < 1u || widthMask < 1u) {
         status = VXLIB_ERR_INVALID_DIMENSION;
      }
      else if (heightMask > 9u || widthMask > 9u) {
         status = VXLIB_ERR_INVALID_DIMENSION;
      }
      else {
         status = VXLIB_SUCCESS;
      }
   }

   VXLIB_DEBUGPRINTFN(0, "Exit with status %d\n", status);
   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_median_exec_checkParams */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method checks the execution parameters for the VXLIB_median kernel
VXLIB_STATUS VXLIB_median_exec_checkParams(VXLIB_kernelHandle handle,
                                           const void *restrict pIn,
                                           const void *restrict pMask,
                                           const void *restrict pOut,
                                           const void *restrict pScratch)
{
   VXLIB_STATUS status;

#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_median_exec_checkParams\n");
#endif
   if ((handle == NULL) || (pIn == NULL) || (pMask == NULL) || (pOut == NULL) || (pScratch == NULL)) {
      status = VXLIB_ERR_NULL_POINTER;
   }
   else {
      status = VXLIB_SUCCESS;
   }

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_median_init */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method is the user-level initialization function for the VXLIB_median kernel
VXLIB_STATUS VXLIB_median_init(VXLIB_kernelHandle           handle,
                               VXLIB_bufParams2D_t         *bufParamsIn,
                               VXLIB_bufParams2D_t         *bufParamsMask,
                               VXLIB_bufParams2D_t         *bufParamsOut,
                               VXLIB_bufParams2D_t         *bufParamsScratch,
                               const VXLIB_median_InitArgs *pKerInitArgs)
{
   VXLIB_STATUS           status       = VXLIB_SUCCESS;
   VXLIB_median_PrivArgs *pKerPrivArgs = (VXLIB_median_PrivArgs *) handle;

   // copy pKerinitArgs into pKerPrivargs
   pKerPrivArgs->pKerInitArgs = *pKerInitArgs;

   // set width and height of image
   pKerPrivArgs->width         = bufParamsIn->dim_x;
   pKerPrivArgs->height        = bufParamsIn->dim_y;
   pKerPrivArgs->M             = bufParamsMask->dim_y;
   pKerPrivArgs->N             = bufParamsMask->dim_x;
   pKerPrivArgs->widthOut      = bufParamsOut->dim_x;
   pKerPrivArgs->heightOut     = bufParamsOut->dim_y;
   pKerPrivArgs->widthScratch  = bufParamsScratch->dim_x;
   pKerPrivArgs->heightScratch = bufParamsScratch->dim_y;

   // compute stride in elements as SE/SA params are set to work with given element type
   pKerPrivArgs->strideInElements      = bufParamsIn->stride_y / VXLIB_sizeof(bufParamsIn->data_type);
   pKerPrivArgs->strideMaskElements    = bufParamsMask->stride_y / VXLIB_sizeof(bufParamsMask->data_type);
   pKerPrivArgs->strideOutElements     = bufParamsOut->stride_y / VXLIB_sizeof(bufParamsOut->data_type);
   pKerPrivArgs->strideScratchElements = bufParamsScratch->stride_y / VXLIB_sizeof(bufParamsScratch->data_type);

#if VXLIB_DEBUGPRINT
   printf("VXLIB_DEBUGPRINT Enter VXLIB_median_init\n");
#endif

   // obtain buffer datatypes
   uint32_t dType = bufParamsIn->data_type;
   //  uint32_t dTypeMask = bufParamsMask->data_type;
   //  uint32_t dTypeOut = bufParamsOut->data_type;

   // determine natural C  vs optimized function call
   if (pKerInitArgs->funcStyle == VXLIB_FUNCTION_NATC) {

      // set function pointer for natural C function with appropriate template parameters based on datatypes
      if (VXLIB_MEDIAN_8U) {
         pKerPrivArgs->execute = VXLIB_median_exec_cn<VXLIB_MEDIAN_TYPENAME_8U>;
      }
      else if (VXLIB_MEDIAN_8S) {
         pKerPrivArgs->execute = VXLIB_median_exec_cn<VXLIB_MEDIAN_TYPENAME_8S>;
      }
      else if (VXLIB_MEDIAN_16U) {
         pKerPrivArgs->execute = VXLIB_median_exec_cn<VXLIB_MEDIAN_TYPENAME_16U>;
      }
      else if (VXLIB_MEDIAN_16S) {
         pKerPrivArgs->execute = VXLIB_median_exec_cn<VXLIB_MEDIAN_TYPENAME_16S>;
      }
      else {
         status = VXLIB_ERR_INVALID_TYPE;
      }
   }
   else { // Optimized function

      // set function pointer for natural C function with appropriate template parameters based on datatypes
      if (!(pKerPrivArgs->pKerInitArgs.kernelType)) {
         if (VXLIB_MEDIAN_8U) {
            pKerPrivArgs->execute = VXLIB_median_3X3_exec_ci<VXLIB_MEDIAN_TYPENAME_8U>;
            status = VXLIB_median_3X3_init_ci<VXLIB_MEDIAN_TYPENAME_8U>(handle, bufParamsIn, bufParamsMask,
                                                                        bufParamsOut, bufParamsScratch, pKerInitArgs);
         }
         else if (VXLIB_MEDIAN_8S) {
            pKerPrivArgs->execute = VXLIB_median_3X3_exec_ci<VXLIB_MEDIAN_TYPENAME_8S>;
            status = VXLIB_median_3X3_init_ci<VXLIB_MEDIAN_TYPENAME_8S>(handle, bufParamsIn, bufParamsMask,
                                                                        bufParamsOut, bufParamsScratch, pKerInitArgs);
         }
         else if (VXLIB_MEDIAN_16U) {
            pKerPrivArgs->execute = VXLIB_median_3X3_exec_ci<VXLIB_MEDIAN_TYPENAME_16U>;
            status = VXLIB_median_3X3_init_ci<VXLIB_MEDIAN_TYPENAME_16U>(handle, bufParamsIn, bufParamsMask,
                                                                         bufParamsOut, bufParamsScratch, pKerInitArgs);
         }
         else if (VXLIB_MEDIAN_16S) {
            pKerPrivArgs->execute = VXLIB_median_3X3_exec_ci<VXLIB_MEDIAN_TYPENAME_16S>;
            status = VXLIB_median_3X3_init_ci<VXLIB_MEDIAN_TYPENAME_16S>(handle, bufParamsIn, bufParamsMask,
                                                                         bufParamsOut, bufParamsScratch, pKerInitArgs);
         }
         else {
            status = VXLIB_ERR_INVALID_TYPE;
         }
      }
      else {
         if (VXLIB_MEDIAN_8U) {
            pKerPrivArgs->execute = VXLIB_median_MXN_exec_ci<VXLIB_MEDIAN_TYPENAME_8U>;
            status = VXLIB_median_MXN_init_ci<VXLIB_MEDIAN_TYPENAME_8U>(handle, bufParamsIn, bufParamsMask,
                                                                        bufParamsOut, bufParamsScratch, pKerInitArgs);
         }
         else if (VXLIB_MEDIAN_8S) {
            pKerPrivArgs->execute = VXLIB_median_MXN_exec_ci<VXLIB_MEDIAN_TYPENAME_8S>;
            status = VXLIB_median_MXN_init_ci<VXLIB_MEDIAN_TYPENAME_8S>(handle, bufParamsIn, bufParamsMask,
                                                                        bufParamsOut, bufParamsScratch, pKerInitArgs);
         }
         else if (VXLIB_MEDIAN_16U) {
            pKerPrivArgs->execute = VXLIB_median_MXN_exec_ci<VXLIB_MEDIAN_TYPENAME_16U>;
            status = VXLIB_median_MXN_init_ci<VXLIB_MEDIAN_TYPENAME_16U>(handle, bufParamsIn, bufParamsMask,
                                                                         bufParamsOut, bufParamsScratch, pKerInitArgs);
         }
         else if (VXLIB_MEDIAN_16S) {
            pKerPrivArgs->execute = VXLIB_median_MXN_exec_ci<VXLIB_MEDIAN_TYPENAME_16S>;
            status = VXLIB_median_MXN_init_ci<VXLIB_MEDIAN_TYPENAME_16S>(handle, bufParamsIn, bufParamsMask,
                                                                         bufParamsOut, bufParamsScratch, pKerInitArgs);
         }
         else {
            status = VXLIB_ERR_INVALID_TYPE;
         }
      }
   }

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_median_exec */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method is the user-level execution function for the VXLIB_median kernel
VXLIB_STATUS VXLIB_median_exec(VXLIB_kernelHandle handle,
                               void *restrict pIn,
                               void *restrict pMask,
                               void *restrict pOut,
                               void *restrict pScratch)
{
   VXLIB_STATUS status;

#if VXLIB_DEBUGPRINT
   printf("VXLIB_DEBUGPRINT Enter VXLIB_median_exec\n");
#endif

   VXLIB_median_PrivArgs *pKerPrivArgs = (VXLIB_median_PrivArgs *) handle;

   status = pKerPrivArgs->execute(handle, pIn, pMask, pOut, pScratch);

   return status;
}

/* ======================================================================== */
/*  End of file:  VXLIB_median.cpp                                        */
/* ======================================================================== */
