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

#include "VXLIB_histogram_priv.h"

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_histogram_getHandleSize                                                                                      */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method calculates and returns the size of the handle for the VXLIB_histogram kernel
int32_t VXLIB_histogram_getHandleSize(VXLIB_histogram_InitArgs *pKerInitArgs)
{
   int32_t privBufSize = sizeof(VXLIB_histogram_PrivArgs);
   return privBufSize;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_histogram_init_checkParams */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method checks the initialization parameters for the VXLIB_histogram kernel
VXLIB_STATUS
VXLIB_histogram_init_checkParams(VXLIB_kernelHandle              handle,
                                 const VXLIB_bufParams2D_t      *bufParamsIn,
                                 const VXLIB_bufParams1D_t      *bufParamsOut,
                                 const VXLIB_histogram_InitArgs *pKerInitArgs)
{
   VXLIB_STATUS status = VXLIB_SUCCESS;

   // obtain input0 buffer parameters
   uint32_t dTypeIn  = bufParamsIn->data_type;
   uint32_t widthIn  = bufParamsIn->dim_x;
   uint32_t heightIn = bufParamsIn->dim_y;
   uint32_t strideIn = bufParamsIn->stride_y;

   // obtain output buffer parameters
   uint32_t dTypeOut = bufParamsOut->data_type;
   uint32_t widthOut = bufParamsOut->dim_x;

   uint8_t  offset    = pKerInitArgs->offset;
   uint16_t range     = pKerInitArgs->range;
   uint16_t numBins   = pKerInitArgs->numBins;
   uint8_t  lastBlock = pKerInitArgs->lastBlock;

   // check for dimensions and datatype combinations
   if (handle == NULL) {
      status = VXLIB_ERR_NULL_POINTER;
   }
   else if ((strideIn < widthIn)) {
      status = VXLIB_ERR_INVALID_DIMENSION;
   }
   else if (((offset + range) > VXLIB_HISTOGRAM_MAX_PIXEL_VALUE) || (numBins > VXLIB_HISTOGRAM_MAX_PIXEL_VALUE)) {
      status = VXLIB_ERR_INVALID_VALUE;
   }
   else if ((lastBlock != 0) && (lastBlock != 1)) {
      status = VXLIB_ERR_INVALID_VALUE;
   }
   else {
      status = VXLIB_SUCCESS;
   }

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_histogram_exec_checkParams */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method checks the execution parameters for the VXLIB_histogram kernel
VXLIB_STATUS
VXLIB_histogram_exec_checkParams(VXLIB_kernelHandle handle, const void *restrict pIn, const void *restrict pOut)
{
   VXLIB_STATUS status;

#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_histogram_exec_checkParams\n");
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
/* VXLIB_histogram_init */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method is the user-level initialization function for the VXLIB_histogram kernel
VXLIB_STATUS VXLIB_histogram_init(VXLIB_kernelHandle              handle,
                                  VXLIB_bufParams2D_t            *bufParamsIn,
                                  VXLIB_bufParams1D_t            *bufParamsOut,
                                  const VXLIB_histogram_InitArgs *pKerInitArgs)
{
   VXLIB_STATUS              status       = VXLIB_SUCCESS;
   VXLIB_histogram_PrivArgs *pKerPrivArgs = (VXLIB_histogram_PrivArgs *) handle;

   // copy pKerinitArgs into pKerPrivargs
   pKerPrivArgs->pKerInitArgs = *pKerInitArgs;

   // set width and height of image
   pKerPrivArgs->width  = bufParamsIn->dim_x;
   pKerPrivArgs->height = bufParamsIn->dim_y;

   // compute stride in elements as SE/SA params are set to work with given element type
   pKerPrivArgs->strideInElements = bufParamsIn->stride_y / VXLIB_sizeof(bufParamsIn->data_type);

#if VXLIB_DEBUGPRINT
   printf("VXLIB_DEBUGPRINT Enter VXLIB_histogram_init\n");
#endif

   // obtain buffer datatypes
   uint32_t dTypeIn  = bufParamsIn->data_type;
   uint32_t dTypeOut = bufParamsOut->data_type;

   // determine natural C  vs optimized function call
   if (pKerInitArgs->funcStyle == VXLIB_FUNCTION_NATC) {
      if (VXLIB_HISTOGRAM_I8U_O32U) {
         pKerPrivArgs->execute = VXLIB_histogram_exec_cn<VXLIB_HISTOGRAM_TYPENAME_I8U_O32U>;
      }
   }
   else { // Optimized function
      if (VXLIB_HISTOGRAM_I8U_O32U) {
         status =
             VXLIB_histogram_init_ci<VXLIB_HISTOGRAM_DTYPE_I8U_O32U>(handle, bufParamsIn, bufParamsOut, pKerInitArgs);
         pKerPrivArgs->execute = VXLIB_histogram_exec_ci<VXLIB_HISTOGRAM_TYPENAME_I8U_O32U>;
      }
   }

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_histogram_exec */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method is the user-level execution function for the VXLIB_histogram kernel
VXLIB_STATUS
VXLIB_histogram_exec(VXLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   VXLIB_STATUS status;

#if VXLIB_DEBUGPRINT
   printf("VXLIB_DEBUGPRINT Enter VXLIB_histogram_exec\n");
#endif

   VXLIB_histogram_PrivArgs *pKerPrivArgs = (VXLIB_histogram_PrivArgs *) handle;

   status = pKerPrivArgs->execute(handle, pIn, pOut);

   return status;
}
