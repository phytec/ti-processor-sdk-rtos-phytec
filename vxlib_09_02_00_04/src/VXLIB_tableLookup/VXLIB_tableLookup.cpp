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

#include "VXLIB_tableLookup_priv.h"
#include "VXLIB_types.h"
#include <cstdint>

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_tableLookup_getHandleSize */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method calculates and returns the size of the handle for the VXLIB_tableLookup kernel
int32_t VXLIB_tableLookup_getHandleSize(VXLIB_tableLookup_InitArgs *pKerInitArgs)
{
   int32_t privBufSize = sizeof(VXLIB_tableLookup_PrivArgs);
   return privBufSize;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_tableLookup_init_checkParams */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method checks the initialization parameters for the VXLIB_tableLookup kernel
VXLIB_STATUS
VXLIB_tableLookup_init_checkParams(VXLIB_kernelHandle                handle,
                                   const VXLIB_bufParams2D_t *       bufParamsIn,
                                   const VXLIB_bufParams2D_t *       bufParamsOut,
                                   const VXLIB_bufParams1D_t *       bufParamsLut,
                                   const VXLIB_tableLookup_InitArgs *pKerInitArgs)
{
   VXLIB_STATUS status = VXLIB_SUCCESS;

   // check for null handles
   if (handle == NULL || bufParamsIn == NULL || bufParamsOut == NULL || bufParamsLut == NULL || pKerInitArgs == NULL) {
      status = VXLIB_ERR_NULL_POINTER;
   }
   // check for valid datatype combinations
   else if (((bufParamsIn->data_type != VXLIB_UINT8) || (bufParamsOut->data_type != VXLIB_UINT8) || (bufParamsLut->data_type != VXLIB_UINT8) || (pKerInitArgs->count > 256U)) &&
            ((bufParamsIn->data_type != VXLIB_INT8) || (bufParamsOut->data_type != VXLIB_INT8) || (bufParamsLut->data_type != VXLIB_INT8) || (pKerInitArgs->count > 256U)) &&
            ((bufParamsIn->data_type != VXLIB_UINT16) || (bufParamsOut->data_type != VXLIB_UINT16) || (bufParamsLut->data_type != VXLIB_UINT16) || (pKerInitArgs->count > 65536U)) &&
            ((bufParamsIn->data_type != VXLIB_INT16) || (bufParamsOut->data_type != VXLIB_INT16) || (bufParamsLut->data_type != VXLIB_INT16) || (pKerInitArgs->count > 65536U))) {
      status = VXLIB_ERR_INVALID_TYPE;
   }
   else if ((bufParamsIn->dim_x != bufParamsOut->dim_x) || (bufParamsIn->dim_y != bufParamsOut->dim_y)) {
      status = VXLIB_ERR_INVALID_DIMENSION;
   }
   // check valid dimensions; input height, width must be greater, equal to filter size for non padded implementation
   else if ((uint32_t)(bufParamsIn->stride_y / VXLIB_sizeof(bufParamsIn->data_type)) < bufParamsIn->dim_x  || (uint32_t)(bufParamsOut->stride_y / VXLIB_sizeof(bufParamsOut->data_type)) < bufParamsOut->dim_x) {
      status = VXLIB_ERR_NOT_EQUAL_WIDTH_STRIDE;
   }
   else {
      status = VXLIB_SUCCESS;
   }
   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_tableLookup_exec_checkParams */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method checks the execution parameters for the VXLIB_tableLookup kernel

// this method checks the execution parameters for the VXLIB_tableLookup kernel
VXLIB_STATUS VXLIB_tableLookup_exec_checkParams(VXLIB_kernelHandle   handle,
                                                const void *restrict pIn,
                                                const void *restrict pOut,
                                                const void *restrict pLut)
{
   VXLIB_STATUS status;

#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_tableLookup_exec_checkParams\n");
#endif
   if ((handle == NULL) || (pIn == NULL) || (pOut == NULL) || (pLut == NULL)) {
      status = VXLIB_ERR_NULL_POINTER;
   }
   else {
      status = VXLIB_SUCCESS;
   }

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_tableLookup_init */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method is the user-level initialization function for the VXLIB_tableLookup kernel
VXLIB_STATUS VXLIB_tableLookup_init(VXLIB_kernelHandle                handle,
                                    VXLIB_bufParams2D_t *             bufParamsIn,
                                    VXLIB_bufParams2D_t *             bufParamsOut,
                                    VXLIB_bufParams1D_t *             bufParamsLut,
                                    const VXLIB_tableLookup_InitArgs *pKerInitArgs)
{
   VXLIB_STATUS                status       = VXLIB_SUCCESS;
   VXLIB_tableLookup_PrivArgs *pKerPrivArgs = (VXLIB_tableLookup_PrivArgs *) handle;

   // copy pKerinitArgs into pKerPrivargs
   pKerPrivArgs->pKerInitArgs = *pKerInitArgs;

   // set priv args
   pKerPrivArgs->width  = bufParamsIn->dim_x;
   pKerPrivArgs->height = bufParamsIn->dim_y;

   // compute stride in elements as SE/SA params are set to work with given element type
   pKerPrivArgs->strideInElements  = bufParamsIn->stride_y / VXLIB_sizeof(bufParamsIn->data_type);
   pKerPrivArgs->strideOutElements = bufParamsOut->stride_y / VXLIB_sizeof(bufParamsOut->data_type);

#if VXLIB_DEBUGPRINT
   printf("VXLIB_DEBUGPRINT Enter VXLIB_tableLookup_init\n");
#endif

   // obtain buffer datatypes
   uint32_t dTypeIn  = bufParamsIn->data_type;
   uint32_t dTypeOut = bufParamsOut->data_type;
   uint32_t dTypeLut = bufParamsLut->data_type;

   // determine natural C  vs optimized function call
   if (pKerInitArgs->funcStyle == VXLIB_FUNCTION_NATC) {
      // set function pointer for natural C function with appropriate template parameters based on datatypes
      if (VXLIB_TABLELOOKUP_I8U_I8U_O8U) {
         pKerPrivArgs->lutSet  = VXLIB_tableLookup_set_cn<uint8_t>;
         pKerPrivArgs->execute = VXLIB_tableLookup_exec_cn<uint8_t>;
      }
      else if (VXLIB_TABLELOOKUP_I8S_I8S_O8S) {
         pKerPrivArgs->lutSet  = VXLIB_tableLookup_set_cn<int8_t>;
         pKerPrivArgs->execute = VXLIB_tableLookup_exec_cn<int8_t>;
      }
      else if (VXLIB_TABLELOOKUP_I16U_I16U_O16U) {
         pKerPrivArgs->lutSet  = VXLIB_tableLookup_set_cn<uint16_t>;
         pKerPrivArgs->execute = VXLIB_tableLookup_exec_cn<uint16_t>;
      }
      else if (VXLIB_TABLELOOKUP_I16S_I16S_O16S) {
         pKerPrivArgs->lutSet  = VXLIB_tableLookup_set_cn<int16_t>;
         pKerPrivArgs->execute = VXLIB_tableLookup_exec_cn<int16_t>;
      }
      else {
         status = VXLIB_ERR_INVALID_TYPE;
      }
   }
   else { // Optimized function

      if (VXLIB_TABLELOOKUP_I8U_I8U_O8U) {
         status = VXLIB_tableLookup_init_ci<VXLIB_TABLELOOKUP_DTYPE_I8U_O8U>(handle, bufParamsIn, bufParamsOut, bufParamsLut, 
                                                                             pKerInitArgs);
         pKerPrivArgs->lutSet  = VXLIB_tableLookup_set_ci<uint8_t>;
         pKerPrivArgs->execute = VXLIB_tableLookup_exec_ci<uint8_t>;
      }
      else if (VXLIB_TABLELOOKUP_I8S_I8S_O8S) {
         status = VXLIB_tableLookup_init_ci<VXLIB_TABLELOOKUP_DTYPE_I8S_O8S>(handle, bufParamsIn, bufParamsOut, bufParamsLut,
                                                                             pKerInitArgs);
         pKerPrivArgs->lutSet  = VXLIB_tableLookup_set_ci<int8_t>;
         pKerPrivArgs->execute = VXLIB_tableLookup_exec_ci<int8_t>;
      }
      else if (VXLIB_TABLELOOKUP_I16U_I16U_O16U) {
         status = VXLIB_tableLookup_init_ci<VXLIB_TABLELOOKUP_DTYPE_I16U_O16U>(handle, bufParamsIn, bufParamsOut, bufParamsLut,
                                                                               pKerInitArgs);
         pKerPrivArgs->lutSet  = VXLIB_tableLookup_set_ci<uint16_t>;
         pKerPrivArgs->execute = VXLIB_tableLookup_exec_ci<uint16_t>;
      }
      else if (VXLIB_TABLELOOKUP_I16S_I16S_O16S) {
         status = VXLIB_tableLookup_init_ci<VXLIB_TABLELOOKUP_DTYPE_I16S_O16S>(handle, bufParamsIn, bufParamsOut, bufParamsLut,
                                                                               pKerInitArgs);
         pKerPrivArgs->lutSet  = VXLIB_tableLookup_set_ci<int16_t>;
         pKerPrivArgs->execute = VXLIB_tableLookup_exec_ci<int16_t>;
      }
      else {
         status = VXLIB_ERR_INVALID_TYPE;
      }
   }

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_tableLookup_exec */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method is the user-level execution function for the VXLIB_tableLookup kernel
VXLIB_STATUS
VXLIB_tableLookup_set(VXLIB_kernelHandle handle, void *restrict pLut)
{
   VXLIB_STATUS status;

   VXLIB_tableLookup_PrivArgs *pKerPrivArgs = (VXLIB_tableLookup_PrivArgs *) handle;

   status = pKerPrivArgs->lutSet(handle, pLut);

   return status;
}

VXLIB_STATUS
VXLIB_tableLookup_exec(VXLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut, void *restrict pLut)
{
   VXLIB_STATUS status;

#if VXLIB_DEBUGPRINT
   printf("VXLIB_DEBUGPRINT Enter VXLIB_tableLookup_exec\n");
#endif

   VXLIB_tableLookup_PrivArgs *pKerPrivArgs = (VXLIB_tableLookup_PrivArgs *) handle;

   status = pKerPrivArgs->execute(handle, pIn, pOut, pLut);

   return status;
}
/* ======================================================================== */
/*  End of file:  VXLIB_tableLookup.cpp                                     */
/* ======================================================================== */
