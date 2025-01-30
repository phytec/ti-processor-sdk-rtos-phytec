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

#include "../VXLIB_addWeight/VXLIB_addWeight_priv.h"
#include "VXLIB_accumulateWeightImage_priv.h"
#include <cstdint>

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_accumulateWeightImage_getHandleSize */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method calculates and returns the size of the handle for the VXLIB_accumulateWeightImage kernel
// in this case, the function returns the size of the handle for the VXLIB_addWeight kernel becuase accumulateSqureImage
// has no internal implementation and it calls addWeight implementation instead.
int32_t VXLIB_accumulateWeightImage_getHandleSize(VXLIB_accumulateWeightImage_InitArgs *pKerInitArgs)
{
   int32_t privBufSize = sizeof(VXLIB_addWeight_PrivArgs);
   return privBufSize;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_accumulateWeightImage_init_checkParams */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method checks the initialization parameters for the VXLIB_accumulateWeightImage kernel
VXLIB_STATUS
VXLIB_accumulateWeightImage_init_checkParams(VXLIB_kernelHandle                          handle,
                                             const VXLIB_bufParams2D_t                  *bufParamsIn,
                                             const VXLIB_bufParams2D_t                  *bufParamsOut,
                                             const VXLIB_accumulateWeightImage_InitArgs *pKerInitArgs)
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

   uint32_t strideInElements  = strideIn / VXLIB_sizeof(bufParamsIn->data_type);
   uint32_t strideOutElements = strideOut / VXLIB_sizeof(bufParamsOut->data_type);

   // check for dimensions and datatype combinations
   if (handle == NULL) {
      status = VXLIB_ERR_NULL_POINTER;
   }
   else if ((widthIn != widthOut) || (heightIn != heightOut)) {
      status = VXLIB_ERR_INVALID_DIMENSION;
   }
   else if (strideInElements < widthIn || strideOutElements < widthOut) {
      status = VXLIB_ERR_NOT_EQUAL_WIDTH_STRIDE;
   }
   else if (!(VXLIB_ACCUMULATEWEIGHTIMAGE_I8U_O8U)) {
      status = VXLIB_ERR_INVALID_TYPE;
   }
   else {
      status = VXLIB_SUCCESS;
   }

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_accumulateWeightImage_exec_checkParams */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method checks the execution parameters for the VXLIB_accumulateWeightImage kernel
VXLIB_STATUS VXLIB_accumulateWeightImage_exec_checkParams(VXLIB_kernelHandle handle,
                                                          const void *restrict pIn,
                                                          const void *restrict pOut,
                                                          float alpha)
{
   VXLIB_STATUS status;

#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_accumulateWeightImage_exec_checkParams\n");
#endif
   if ((handle == NULL) || (pIn == NULL) || (pOut == NULL)) {
      status = VXLIB_ERR_NULL_POINTER;
   }

   // alpha is bounded by [0.0, 1.0]
   else if ((alpha < 0.0) || (alpha > 1.0)) {
      status = VXLIB_ERR_INVALID_VALUE;
   }

   else {
      status = VXLIB_SUCCESS;
   }

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_accumulateWeightImage_init */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method is the user-level initialization function for the VXLIB_accumulateWeightImage kernel
VXLIB_STATUS VXLIB_accumulateWeightImage_init(VXLIB_kernelHandle                          handle,
                                              VXLIB_bufParams2D_t                        *bufParamsIn,
                                              VXLIB_bufParams2D_t                        *bufParamsOut,
                                              const VXLIB_accumulateWeightImage_InitArgs *pKerInitArgs)
{
   VXLIB_STATUS              status       = VXLIB_SUCCESS;
   VXLIB_addWeight_PrivArgs *pKerPrivArgs = (VXLIB_addWeight_PrivArgs *) handle;

   // copy pKerinitArgs into pKerPrivargs
   // cast the VXLIB_accumulateWeightImage_InitArgs to VXLIB_addWeight_InitArgs
   VXLIB_addWeight_InitArgs *kerInitArgsAddWeight = (VXLIB_addWeight_InitArgs *) pKerInitArgs;

   pKerPrivArgs->pKerInitArgs = *kerInitArgsAddWeight;

   // set width and height of image
   pKerPrivArgs->width  = bufParamsIn->dim_x;
   pKerPrivArgs->height = bufParamsIn->dim_y;

   // compute stride in elements as SE/SA params are set to work with given element type
   pKerPrivArgs->strideIn0Elements = bufParamsIn->stride_y / VXLIB_sizeof(bufParamsIn->data_type);
   pKerPrivArgs->strideIn1Elements = bufParamsOut->stride_y / VXLIB_sizeof(bufParamsOut->data_type);
   pKerPrivArgs->strideOutElements = bufParamsOut->stride_y / VXLIB_sizeof(bufParamsOut->data_type);

#if VXLIB_DEBUGPRINT
   printf("VXLIB_DEBUGPRINT Enter VXLIB_accumulateWeightImage_init\n");
#endif

   // obtain buffer datatypes
   uint32_t dTypeIn  = bufParamsIn->data_type;
   uint32_t dTypeOut = bufParamsOut->data_type;

   // determine natural C  vs optimized function call
   if (pKerInitArgs->funcStyle == VXLIB_FUNCTION_NATC) {

      // set function pointer for natural C function with appropriate template parameters based on datatypes

      if (VXLIB_ACCUMULATEWEIGHTIMAGE_I8U_O8U) {
         pKerPrivArgs->execute = VXLIB_addWeight_exec_cn<VXLIB_ADDWEIGHT_TYPENAME_I8U_I8U_O8U>;
      }
      else {
         status = VXLIB_ERR_INVALID_TYPE;
      }
   }
   else { // Optimized function

      // set function pointer for natural C function with appropriate template parameters based on datatypes

      if (VXLIB_ACCUMULATEWEIGHTIMAGE_I8U_O8U) {
         pKerPrivArgs->execute = VXLIB_addWeight_exec_ci<VXLIB_ADDWEIGHT_TYPENAME_I8U_I8U_O8U>;
         status = VXLIB_addWeight_init_ci<VXLIB_ADDWEIGHT_DTYPE_I8U_I8U_O8U>(handle, bufParamsIn, bufParamsOut,
                                                                             bufParamsOut, kerInitArgsAddWeight);
      }
      else {
         status = VXLIB_ERR_INVALID_TYPE;
      }
   }

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_accumulateWeightImage_exec */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method is the user-level execution function for the VXLIB_accumulateWeightImage kernel
VXLIB_STATUS
VXLIB_accumulateWeightImage_exec(VXLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut, float alpha)
{
   VXLIB_STATUS status;

#if VXLIB_DEBUGPRINT
   printf("VXLIB_DEBUGPRINT Enter VXLIB_accumulateWeightImage_exec\n");
#endif

   VXLIB_addWeight_PrivArgs *pKerPrivArgs = (VXLIB_addWeight_PrivArgs *) handle;

   status = pKerPrivArgs->execute(handle, pIn, pOut, pOut, alpha);

   return status;
}

void VXLIB_accumulateWeightImage_perfEst(VXLIB_kernelHandle handle, size_t *archCycles, size_t *estCycles)
{

   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_addWeight_PrivArgs *pKerPrivArgs = (VXLIB_addWeight_PrivArgs *) handle;

   // compute cycle estimation in similar manner to asm report
   size_t numBlocks   = pKerPrivArgs->numBlocks; // iteration count from compute loop
   size_t overheadCnt = 17;                      // profiled code before compute loop entrance

   // get iteration cycles from asm report
   size_t iterConst = 14; // prologue + epilogue cycle counts
   size_t ii        = 6;  // ii of scheduled loop

   *archCycles = iterConst + numBlocks * ii;
   *estCycles  = overheadCnt + *archCycles;
}
