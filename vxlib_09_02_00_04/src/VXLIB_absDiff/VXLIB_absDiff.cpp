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

#include "VXLIB_absDiff_priv.h"

int32_t VXLIB_absDiff_getHandleSize(VXLIB_absDiff_InitArgs *pKerInitArgs)
{
   int32_t privBufSize = sizeof(VXLIB_absDiff_PrivArgs);
   return privBufSize;
}

VXLIB_STATUS
VXLIB_absDiff_init_checkParams(VXLIB_kernelHandle            handle,
                               const VXLIB_bufParams2D_t    *bufParamsIn0,
                               const VXLIB_bufParams2D_t    *bufParamsIn1,
                               const VXLIB_bufParams2D_t    *bufParamsOut,
                               const VXLIB_absDiff_InitArgs *pKerInitArgs)
{
   VXLIB_STATUS status = VXLIB_SUCCESS;

#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_absDiff_init_checkParams\n");
#endif
   if (handle == NULL || bufParamsIn0 == NULL || bufParamsIn1 == NULL || bufParamsOut == NULL || pKerInitArgs == NULL) {
      status = VXLIB_ERR_NULL_POINTER;
   }

   if (status == VXLIB_SUCCESS) {
      if (((bufParamsIn0->data_type != VXLIB_INT16) && (bufParamsIn1->data_type != VXLIB_INT16)) &&
          ((bufParamsIn0->data_type != VXLIB_UINT8) && (bufParamsIn1->data_type != VXLIB_UINT8))) {
         status = VXLIB_ERR_INVALID_TYPE;
      }
      else if (bufParamsIn0->data_type != bufParamsOut->data_type ||
               bufParamsIn1->data_type != bufParamsOut->data_type) {
         status = VXLIB_ERR_INVALID_TYPE;
      }
      else {
         /* Nothing to do here */
      }
   }

   if (status == VXLIB_SUCCESS) {

      uint32_t strideIn0Elements = bufParamsIn0->stride_y / VXLIB_sizeof(bufParamsIn0->data_type);
      uint32_t strideIn1Elements = bufParamsIn1->stride_y / VXLIB_sizeof(bufParamsIn1->data_type);
      uint32_t strideOutElements = bufParamsOut->stride_y / VXLIB_sizeof(bufParamsOut->data_type);

      if ((bufParamsIn0->dim_x != bufParamsIn1->dim_x) || (bufParamsIn0->dim_y != bufParamsIn1->dim_y) ||
          (bufParamsIn0->dim_x != bufParamsOut->dim_x) || (bufParamsIn0->dim_y != bufParamsOut->dim_y) ||
          (strideIn0Elements < bufParamsIn0->dim_x) || (strideIn1Elements < bufParamsIn1->dim_x) ||
          (strideOutElements < bufParamsOut->dim_x)) {
         status = VXLIB_ERR_INVALID_DIMENSION;
      }
   }

   return status;
}

VXLIB_STATUS VXLIB_absDiff_exec_checkParams(VXLIB_kernelHandle handle,
                                            const void *restrict pIn0,
                                            const void *restrict pIn1,
                                            const void *restrict pOut)
{
   VXLIB_STATUS status;

#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_absDiff_exec_checkParams\n");
#endif
   if ((handle == NULL) || (pIn0 == NULL) || (pIn1 == NULL) || (pOut == NULL)) {
      status = VXLIB_ERR_NULL_POINTER;
   }
   else {
      status = VXLIB_SUCCESS;
   }

   return status;
}

VXLIB_STATUS VXLIB_absDiff_init(VXLIB_kernelHandle            handle,
                                VXLIB_bufParams2D_t          *bufParamsIn0,
                                VXLIB_bufParams2D_t          *bufParamsIn1,
                                VXLIB_bufParams2D_t          *bufParamsOut,
                                const VXLIB_absDiff_InitArgs *pKerInitArgs)
{
   VXLIB_STATUS            status       = VXLIB_SUCCESS;
   VXLIB_absDiff_PrivArgs *pKerPrivArgs = (VXLIB_absDiff_PrivArgs *) handle;

   uint32_t height = bufParamsOut->dim_y;
   uint32_t width  = bufParamsOut->dim_x;

   uint32_t datatype          = bufParamsIn0->data_type;
   uint32_t strideIn0Elements = bufParamsIn0->stride_y / VXLIB_sizeof(datatype);
   uint32_t strideIn1Elements = bufParamsIn1->stride_y / VXLIB_sizeof(datatype);
   uint32_t strideOutElements = bufParamsOut->stride_y / VXLIB_sizeof(datatype);

   pKerPrivArgs->height            = height;
   pKerPrivArgs->width             = width;
   pKerPrivArgs->strideIn0Elements = strideIn0Elements;
   pKerPrivArgs->strideIn1Elements = strideIn1Elements;
   pKerPrivArgs->strideOutElements = strideOutElements;

#if VXLIB_DEBUGPRINT
   printf("VXLIB_DEBUGPRINT Enter VXLIB_absDiff_init\n");
#endif

   // determine natural C  vs optimized function call
   if (pKerInitArgs->funcStyle == VXLIB_FUNCTION_NATC) {

      // set function pointer for natural C function with appropriate template parameters based on datatypes
      if (bufParamsIn0->data_type == VXLIB_UINT8) {
         pKerPrivArgs->execute = VXLIB_absDiff_exec_cn<uint8_t>;
      }
      else if (bufParamsIn0->data_type == VXLIB_INT16) {
         pKerPrivArgs->execute = VXLIB_absDiff_exec_cn<int16_t>;
      }
      else {
         status = VXLIB_ERR_INVALID_TYPE;
      }
   }
   else {

      // optimized function
      // set function pointer for optimized function with appropriate template parameters based on datatypes
      if (bufParamsIn0->data_type == VXLIB_UINT8) {
         pKerPrivArgs->execute = VXLIB_absDiff_exec_ci<uint8_t>;
         status = VXLIB_absDiff_init_ci<VXLIB_UINT8>(handle, bufParamsIn0, bufParamsIn1, bufParamsOut, pKerInitArgs);
      }
      else if (bufParamsIn0->data_type == VXLIB_INT16) {
         pKerPrivArgs->execute = VXLIB_absDiff_exec_ci<int16_t>;
         status = VXLIB_absDiff_init_ci<VXLIB_INT16>(handle, bufParamsIn0, bufParamsIn1, bufParamsOut, pKerInitArgs);
      }
      else {
         status = VXLIB_ERR_INVALID_TYPE;
      }
   }

   return status;
}

VXLIB_STATUS
VXLIB_absDiff_exec(VXLIB_kernelHandle handle, void *restrict pIn0, void *restrict pIn1, void *restrict pOut)
{
   VXLIB_STATUS status;

#if VXLIB_DEBUGPRINT
   printf("VXLIB_DEBUGPRINT Enter VXLIB_absDiff_exec\n");
#endif

   VXLIB_absDiff_PrivArgs *pKerPrivArgs = (VXLIB_absDiff_PrivArgs *) handle;

   status = pKerPrivArgs->execute(handle, pIn0, pIn1, pOut);

   return status;
}
