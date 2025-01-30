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

#include "VXLIB_meanStdDev_priv.h"
#include "VXLIB_types.h"

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_meanStdDev_getHandleSize */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method calculates and returns the size of the handle for the VXLIB_meanStdDev kernel
int32_t VXLIB_meanStdDev_getHandleSize(VXLIB_meanStdDev_InitArgs *pKerInitArgs)
{
   int32_t privBufSize = sizeof(VXLIB_meanStdDev_PrivArgs);
   return privBufSize;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_meanStdDev_init_checkParams */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method checks the initialization parameters for the VXLIB_meanStdDev kernel
VXLIB_STATUS
VXLIB_meanStdDev_init_checkParams(VXLIB_kernelHandle               handle,
                                  const VXLIB_bufParams2D_t       *bufParamsIn,
                                  const VXLIB_meanStdDev_InitArgs *pKerInitArgs)
{
   VXLIB_STATUS status = VXLIB_SUCCESS;

   // obtain input0 buffer parameters
   uint32_t dTypeIn  = bufParamsIn->data_type;
   uint32_t widthIn  = bufParamsIn->dim_x;
   uint32_t strideIn = bufParamsIn->stride_y;

   // obtain output buffer parameters
   uint32_t dTypeOut = VXLIB_FLOAT32;

   // check for dimensions and datatype combinations
   if (handle == NULL) {
      status = VXLIB_ERR_NULL_POINTER;
   }
   else if ((!VXLIB_MEANSTDDEV_I8U_O32F) && (!VXLIB_MEANSTDDEV_I16U_O32F)) {
      status = VXLIB_ERR_INVALID_TYPE;
   }
   else if (strideIn < widthIn) {
      status = VXLIB_ERR_INVALID_DIMENSION;
   }
   else {
      status = VXLIB_SUCCESS;
   }

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_meanStdDev_exec_checkParams */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method checks the execution parameters for the VXLIB_meanStdDev kernel
VXLIB_STATUS VXLIB_meanStdDev_exec_checkParams(VXLIB_kernelHandle handle,
                                               const void *restrict pIn,
                                               const void *restrict pOut0,
                                               const void *restrict pOut1,
                                               const void *restrict pPixelsProcessed,
                                               const void *restrict pCurrentSum,
                                               const void *restrict pCurrentSqSum)
{
   VXLIB_STATUS status;

#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_meanStdDev_exec_checkParams\n");
#endif
   if ((handle == NULL) || (pIn == NULL) || (pOut0 == NULL) || (pOut1 == NULL) || (pPixelsProcessed == NULL) ||
       (pCurrentSum == NULL) | (pCurrentSqSum == NULL)) {
      status = VXLIB_ERR_NULL_POINTER;
   }
   else {
      status = VXLIB_SUCCESS;
   }

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_meanStdDev_init */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method is the user-level initialization function for the VXLIB_meanStdDev kernel
VXLIB_STATUS VXLIB_meanStdDev_init(VXLIB_kernelHandle               handle,
                                   VXLIB_bufParams2D_t             *bufParamsIn,
                                   const VXLIB_meanStdDev_InitArgs *pKerInitArgs)
{
   VXLIB_STATUS               status       = VXLIB_SUCCESS;
   VXLIB_meanStdDev_PrivArgs *pKerPrivArgs = (VXLIB_meanStdDev_PrivArgs *) handle;

   // copy pKerinitArgs into pKerPrivargs
   pKerPrivArgs->pKerInitArgs = *pKerInitArgs;

   // set width and height of image
   pKerPrivArgs->width  = bufParamsIn->dim_x;
   pKerPrivArgs->height = bufParamsIn->dim_y;

   // compute stride in elements as SE/SA params are set to work with given element type
   pKerPrivArgs->strideInElements = bufParamsIn->stride_y / VXLIB_sizeof(bufParamsIn->data_type);

#if VXLIB_DEBUGPRINT
   printf("VXLIB_DEBUGPRINT Enter VXLIB_meanStdDev_init\n");
#endif

   // obtain buffer datatypes
   uint32_t dTypeIn  = bufParamsIn->data_type;
   uint32_t dTypeOut = VXLIB_FLOAT32;

   // determine natural C  vs optimized function call
   if (pKerInitArgs->funcStyle == VXLIB_FUNCTION_NATC) {

      // set function pointer for natural C function with appropriate template parameters based on datatypes
      if (VXLIB_MEANSTDDEV_I8U_O32F) {
         pKerPrivArgs->execute = VXLIB_meanStdDev_exec_cn<VXLIB_MEANSTDDEV_TYPENAME_I8U_O32F>;
      }
      else if (VXLIB_MEANSTDDEV_I16U_O32F) {
         pKerPrivArgs->execute = VXLIB_meanStdDev_exec_cn<VXLIB_MEANSTDDEV_TYPENAME_I16U_O32F>;
      }
      else {
         status = VXLIB_ERR_INVALID_TYPE;
      }
   }
   else { // Optimized function

      // set function pointer for natural C function with appropriate template parameters based on datatypes
      if (VXLIB_MEANSTDDEV_I8U_O32F) {
         pKerPrivArgs->execute = VXLIB_meanStdDev_exec_ci<VXLIB_MEANSTDDEV_TYPENAME_I8U_O32F>;
         status = VXLIB_meanStdDev_init_ci<VXLIB_MEANSTDDEV_DTYPE_I8U_O32F>(handle, bufParamsIn, pKerInitArgs);
      }
      else if (VXLIB_MEANSTDDEV_I16U_O32F) {
         pKerPrivArgs->execute = VXLIB_meanStdDev_exec_ci<VXLIB_MEANSTDDEV_TYPENAME_I16U_O32F>;
         status = VXLIB_meanStdDev_init_ci<VXLIB_MEANSTDDEV_DTYPE_I16U_O32F>(handle, bufParamsIn, pKerInitArgs);
      }
      else {
         status = VXLIB_ERR_INVALID_TYPE;
      }
   }

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_meanStdDev_exec */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method is the user-level execution function for the VXLIB_meanStdDev kernel
VXLIB_STATUS
VXLIB_meanStdDev_exec(VXLIB_kernelHandle handle,
                      void *restrict pIn,
                      void *restrict pOut0,
                      void *restrict pOut1,
                      void *restrict pPixelsProcessed,
                      void *restrict pCurrentSum,
                      void *restrict pCurrentSqSum)
{
   VXLIB_STATUS status;

#if VXLIB_DEBUGPRINT
   printf("VXLIB_DEBUGPRINT Enter VXLIB_meanStdDev_exec\n");
#endif

   VXLIB_meanStdDev_PrivArgs *pKerPrivArgs = (VXLIB_meanStdDev_PrivArgs *) handle;

   status = pKerPrivArgs->execute(handle, pIn, pOut0, pOut1, pPixelsProcessed, pCurrentSum, pCurrentSqSum);

   return status;
}
