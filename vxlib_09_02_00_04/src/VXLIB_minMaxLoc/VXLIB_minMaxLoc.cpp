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

#include "VXLIB_minMaxLoc_priv.h"

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_minMaxLoc_getHandleSize */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method calculates and returns the size of the handle for the VXLIB_minMaxLoc kernel
int32_t VXLIB_minMaxLoc_getHandleSize(VXLIB_minMaxLoc_InitArgs *pKerInitArgs)
{
   int32_t privBufSize = sizeof(VXLIB_minMaxLoc_PrivArgs);
   return privBufSize;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_minMaxLoc_init_checkParams */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method checks the initialization parameters for the VXLIB_minMaxLoc kernel
VXLIB_STATUS
VXLIB_minMaxLoc_init_checkParams(VXLIB_kernelHandle              handle,
                                 const VXLIB_bufParams2D_t      *bufParamsIn,
                                 const VXLIB_bufParams1D_t      *bufParamsMinLoc,
                                 const VXLIB_bufParams1D_t      *bufParamsMaxLoc,
                                 const VXLIB_minMaxLoc_InitArgs *pKerInitArgs)
{
   VXLIB_STATUS status = VXLIB_SUCCESS;


   if ((handle == NULL) || (bufParamsIn == NULL) || (bufParamsMinLoc == NULL) || (bufParamsMaxLoc == NULL) ||
       (pKerInitArgs == NULL)) {
      status = VXLIB_ERR_NULL_POINTER;
   }

   if (status == VXLIB_SUCCESS) {

      // obtain input0 buffer parameters
      uint32_t dType    = bufParamsIn->data_type;
      uint32_t widthIn  = bufParamsIn->dim_x;
      uint32_t strideIn = bufParamsIn->stride_y;
      uint32_t strideInElements = strideIn / VXLIB_sizeof(dType);

   // check for dimensions and datatype combinations
      if (strideInElements < widthIn) {
         status = VXLIB_ERR_INVALID_DIMENSION;
      }
      else if (!(VXLIB_MINMAXLOC_8U) && !(VXLIB_MINMAXLOC_8S) && !(VXLIB_MINMAXLOC_16U) && !(VXLIB_MINMAXLOC_16S)) {
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
/* VXLIB_minMaxLoc_exec_checkParams */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method checks the execution parameters for the VXLIB_minMaxLoc kernel
VXLIB_STATUS VXLIB_minMaxLoc_exec_checkParams(VXLIB_kernelHandle handle,
                                              const void *restrict pIn,
                                              const void *restrict pMinVal,
                                              const void *restrict pMaxVal,
                                              const void *restrict pMinCount,
                                              const void *restrict pMaxCount,
                                              const void *restrict pMinLocCapacity,
                                              const void *restrict pMaxLocCapacity,
                                              const void *restrict pMinLoc,
                                              const void *restrict pMaxLoc,
                                              const void *restrict pStartX,
                                              const void *restrict pStartY)
{
   VXLIB_STATUS status;

#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_minMaxLoc_exec_checkParams\n");
#endif
   if ((handle == NULL) || (pIn == NULL) || (pMinVal == NULL) || (pMaxVal == NULL)) {
      status = VXLIB_ERR_NULL_POINTER;
   }
   else {
      status = VXLIB_SUCCESS;
   }

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_minMaxLoc_init */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method is the user-level initialization function for the VXLIB_minMaxLoc kernel
VXLIB_STATUS VXLIB_minMaxLoc_init(VXLIB_kernelHandle              handle,
                                  VXLIB_bufParams2D_t            *bufParamsIn,
                                  VXLIB_bufParams1D_t            *bufParamsMinLoc,
                                  VXLIB_bufParams1D_t            *bufParamsMaxLoc,
                                  const VXLIB_minMaxLoc_InitArgs *pKerInitArgs)
{
   VXLIB_STATUS              status       = VXLIB_SUCCESS;
   VXLIB_minMaxLoc_PrivArgs *pKerPrivArgs = (VXLIB_minMaxLoc_PrivArgs *) handle;

   // copy pKerinitArgs into pKerPrivargs
   pKerPrivArgs->pKerInitArgs = *pKerInitArgs;

   // set width and height of image
   pKerPrivArgs->width  = bufParamsIn->dim_x;
   pKerPrivArgs->height = bufParamsIn->dim_y;

   // compute stride in elements as SE/SA params are set to work with given element type
   pKerPrivArgs->strideInElements = bufParamsIn->stride_y / VXLIB_sizeof(bufParamsIn->data_type);

#if VXLIB_DEBUGPRINT
   printf("VXLIB_DEBUGPRINT Enter VXLIB_minMaxLoc_init\n");
#endif

   // obtain buffer datatypes
   uint32_t dType = bufParamsIn->data_type;

   // determine natural C  vs optimized function call
   if (pKerInitArgs->funcStyle == VXLIB_FUNCTION_NATC) {

      // set function pointer for natural C function with appropriate template parameters based on datatypes
      if (VXLIB_MINMAXLOC_8U) {
         pKerPrivArgs->execute = VXLIB_minMaxLoc_exec_cn<VXLIB_MINMAXLOC_TYPENAME_8U>;
      }
      else if (VXLIB_MINMAXLOC_8S) {
         pKerPrivArgs->execute = VXLIB_minMaxLoc_exec_cn<VXLIB_MINMAXLOC_TYPENAME_8S>;
      }
      else if (VXLIB_MINMAXLOC_16U) {
         pKerPrivArgs->execute = VXLIB_minMaxLoc_exec_cn<VXLIB_MINMAXLOC_TYPENAME_16U>;
      }
      else if (VXLIB_MINMAXLOC_16S) {
         pKerPrivArgs->execute = VXLIB_minMaxLoc_exec_cn<VXLIB_MINMAXLOC_TYPENAME_16S>;
      }
      else {
         status = VXLIB_ERR_INVALID_TYPE;
      }
   }
   else { // Optimized function

      // set function pointer for natural C function with appropriate template parameters based on datatypes
      if (VXLIB_MINMAXLOC_8U) {
         pKerPrivArgs->execute = VXLIB_minMaxLoc_exec_ci<VXLIB_MINMAXLOC_TYPENAME_8U>;
         status                = VXLIB_minMaxLoc_init_ci<VXLIB_MINMAXLOC_DTYPE_8U>(handle, bufParamsIn, bufParamsMinLoc,
                                                                    bufParamsMaxLoc, pKerInitArgs);
      }
      else if (VXLIB_MINMAXLOC_8S) {
         pKerPrivArgs->execute = VXLIB_minMaxLoc_exec_ci<VXLIB_MINMAXLOC_TYPENAME_8S>;
         status                = VXLIB_minMaxLoc_init_ci<VXLIB_MINMAXLOC_DTYPE_8S>(handle, bufParamsIn, bufParamsMinLoc,
                                                                    bufParamsMaxLoc, pKerInitArgs);
      }
      else if (VXLIB_MINMAXLOC_16U) {
         pKerPrivArgs->execute = VXLIB_minMaxLoc_exec_ci<VXLIB_MINMAXLOC_TYPENAME_16U>;
         status = VXLIB_minMaxLoc_init_ci<VXLIB_MINMAXLOC_DTYPE_16U>(handle, bufParamsIn, bufParamsMinLoc,
                                                                     bufParamsMaxLoc, pKerInitArgs);
      }
      else if (VXLIB_MINMAXLOC_16S) {
         pKerPrivArgs->execute = VXLIB_minMaxLoc_exec_ci<VXLIB_MINMAXLOC_TYPENAME_16S>;
         status = VXLIB_minMaxLoc_init_ci<VXLIB_MINMAXLOC_DTYPE_16S>(handle, bufParamsIn, bufParamsMinLoc,
                                                                     bufParamsMaxLoc, pKerInitArgs);
      }
      else {
         status = VXLIB_ERR_INVALID_TYPE;
      }
   }

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_minMaxLoc_exec */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method is the user-level execution function for the VXLIB_minMaxLoc kernel
VXLIB_STATUS
VXLIB_minMaxLoc_exec(VXLIB_kernelHandle handle,
                     void *restrict pIn,
                     void *restrict pMinVal,
                     void *restrict pMaxVal,
                     void *restrict pMinCount,
                     void *restrict pMaxCount,
                     void *restrict pMinLocCapacity,
                     void *restrict pMaxLocCapacity,
                     void *restrict pMinLoc,
                     void *restrict pMaxLoc,
                     void *restrict pStartX,
                     void *restrict pStartY)
{
   VXLIB_STATUS status;

#if VXLIB_DEBUGPRINT
   printf("VXLIB_DEBUGPRINT Enter VXLIB_minMaxLoc_exec\n");
#endif

   VXLIB_minMaxLoc_PrivArgs *pKerPrivArgs = (VXLIB_minMaxLoc_PrivArgs *) handle;

   status = pKerPrivArgs->execute(handle, pIn, pMinVal, pMaxVal, pMinCount, pMaxCount, pMinLocCapacity, pMaxLocCapacity,
                                  pMinLoc, pMaxLoc, pStartX, pStartY);

   return status;
}
/* ======================================================================== */
/*  End of file:  VXLIB_minMaxLoc.cpp                                       */
/* ======================================================================== */
