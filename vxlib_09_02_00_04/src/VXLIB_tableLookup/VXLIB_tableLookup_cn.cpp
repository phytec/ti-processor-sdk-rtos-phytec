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

/**********************************************************************************************************************/
/*                                                                                                                    */
/* DEFINES                                                                                                            */
/*                                                                                                                    */
/**********************************************************************************************************************/

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_tableLookup_set_cn */
/*                                                                                                                    */
/**********************************************************************************************************************/

template <typename dataType> VXLIB_STATUS VXLIB_tableLookup_set_cn(VXLIB_kernelHandle handle, void *restrict pLut)
{

   VXLIB_STATUS status = VXLIB_SUCCESS; // assign status to success by default
   // typecast handle (void) to struct pointer type associated to kernel

   return (status);
}

template VXLIB_STATUS VXLIB_tableLookup_set_cn<uint8_t>(VXLIB_kernelHandle handle, void *restrict pLut);
template VXLIB_STATUS VXLIB_tableLookup_set_cn<int8_t>(VXLIB_kernelHandle handle, void *restrict pLut);

template VXLIB_STATUS VXLIB_tableLookup_set_cn<uint16_t>(VXLIB_kernelHandle handle, void *restrict pLut);
template VXLIB_STATUS VXLIB_tableLookup_set_cn<int16_t>(VXLIB_kernelHandle handle, void *restrict pLut);

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_tableLookup_exec_cn */
/*                                                                                                                    */
/**********************************************************************************************************************/
template <typename dataType>
VXLIB_STATUS
VXLIB_tableLookup_exec_cn(VXLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut, void *restrict pLut)
{

   VXLIB_STATUS status = VXLIB_SUCCESS; // assign status to success by default

#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_tableLookup_exec_cn\n");
#endif

   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_tableLookup_PrivArgs *pKerPrivArgs = (VXLIB_tableLookup_PrivArgs *) handle;

   // obtain image parameters
   size_t width             = pKerPrivArgs->width;
   size_t height            = pKerPrivArgs->height;
   size_t strideInElements  = pKerPrivArgs->strideInElements;
   size_t strideOutElements = pKerPrivArgs->strideOutElements;

   uint32_t count  = pKerPrivArgs->pKerInitArgs.count;
   uint16_t offset = pKerPrivArgs->pKerInitArgs.offset;

   // variables to hold temporary values
   dataType srcIndex;
   int32_t  lutIndex;

   // create local pointers
   dataType *restrict pInLocal  = (dataType *) pIn;
   dataType *restrict pOutLocal = (dataType *) pOut;
   dataType *restrict lutLocal  = (dataType *) pLut;

#if VXLIB_DEBUGPRINT
   printf("In VXLIB_tableLookup_exec_cn, width: %d, height: %d\n", width, height);
#endif

   for (uint32_t i = 0; i < height; i++) {
      for (uint32_t j = 0; j < width; j++) {
         srcIndex = pInLocal[i * strideInElements + j];
         lutIndex = (int32_t) offset + (int32_t) srcIndex;
         if (lutIndex >= 0 && (uint32_t)lutIndex < count) {

            pOutLocal[i * strideOutElements + j] = lutLocal[lutIndex];
         }
         else {
            status = VXLIB_ERR_INVALID_DIMENSION;
         }
      }
   }

   return (status);
}

template VXLIB_STATUS VXLIB_tableLookup_exec_cn<uint8_t>(VXLIB_kernelHandle handle,
                                                         void *restrict     pIn,
                                                         void *restrict     pOut,
                                                         void *restrict     pLut);
template VXLIB_STATUS VXLIB_tableLookup_exec_cn<int8_t>(VXLIB_kernelHandle handle,
                                                        void *restrict     pIn,
                                                        void *restrict     pOut,
                                                        void *restrict     pLut);
template VXLIB_STATUS VXLIB_tableLookup_exec_cn<uint16_t>(VXLIB_kernelHandle handle,
                                                          void *restrict     pIn,
                                                          void *restrict     pOut,
                                                          void *restrict     pLut);
template VXLIB_STATUS VXLIB_tableLookup_exec_cn<int16_t>(VXLIB_kernelHandle handle,
                                                         void *restrict     pIn,
                                                         void *restrict     pOut,
                                                         void *restrict     pLut);
/* ======================================================================== */
/*  End of file:  VXLIB_tableLookup_cn.cpp                                  */
/* ======================================================================== */
