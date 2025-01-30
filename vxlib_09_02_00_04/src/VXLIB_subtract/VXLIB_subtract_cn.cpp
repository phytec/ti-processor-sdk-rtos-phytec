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

#include "VXLIB_subtract_priv.h"

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_subtract_exec_cn */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method computes add in via natural C code
template <typename dTypeIn0, typename dTypeIn1, typename dTypeOut>
VXLIB_STATUS
VXLIB_subtract_exec_cn(VXLIB_kernelHandle handle, void *restrict pIn0, void *restrict pIn1, void *restrict pOut)
{
   VXLIB_DEBUGPRINTFN(0, "%s\n", "Enter function");

   VXLIB_STATUS status = VXLIB_SUCCESS; // assign status to success by default

   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_subtract_PrivArgs *pKerPrivArgs = (VXLIB_subtract_PrivArgs *) handle;

   // obtain image parameters and overflow policy
   size_t                width             = pKerPrivArgs->width;
   size_t                height            = pKerPrivArgs->height;
   size_t                strideIn0Elements = pKerPrivArgs->strideIn0Elements;
   size_t                strideIn1Elements = pKerPrivArgs->strideIn1Elements;
   size_t                strideOutElements = pKerPrivArgs->strideOutElements;
   VXLIB_OVERFLOW_POLICY overFlowPolicy    = pKerPrivArgs->pKerInitArgs.overFlowPolicy;
   uint8_t               subtractPolicy    = pKerPrivArgs->pKerInitArgs.subtractPolicy;

   // create local pointers
   dTypeIn0 *restrict pInLocal0 = (dTypeIn0 *) pIn0;
   dTypeIn1 *restrict pInLocal1 = (dTypeIn1 *) pIn1;
   dTypeOut *restrict pOutLocal = (dTypeOut *) pOut;

   // variables to hold temporary values
   dTypeIn0 a;
   dTypeIn1 b;
   dTypeOut out;

#if VXLIB_DEBUGPRINT
   printf("In VXLIB_subtract_exec_cn, width: %lu, height: %lu\n", width, height);
#endif

   // perform add
   for (uint32_t i = 0; i < height; i++) {
      for (uint32_t j = 0; j < width; j++) {
         a = pInLocal0[i * strideIn0Elements + j];
         b = pInLocal1[i * strideIn1Elements + j];

         if (overFlowPolicy == VXLIB_SATURATE) {
            int32_t temp;
            if (subtractPolicy == 0) {
               temp = a - b;
            }
            else {
               temp = b - a;
            }
#if VXLIB_DEBUGPRINT
            if (i == 0 && j < 7)
               printf("row: %d col: %d a %d b %d temp %d \n", i, j, a, b, temp);
#endif
            dTypeOut min = VXLIB_SUBTRACT_NUMERIC_MIN(dTypeOut);
            dTypeOut max = VXLIB_SUBTRACT_NUMERIC_MAX(dTypeOut);

            temp = (temp < min) ? min : temp;
            temp = (temp > max) ? max : temp;

            out = temp;
         }
         else {
            if (subtractPolicy == 0) {
               out = a - b;
            }
            else {
               out = b - a;
            }
         }

         pOutLocal[i * strideOutElements + j] = out;
      }
   }

   VXLIB_DEBUGPRINTFN(0, "%s\n", "Exit function");

   return (status);
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* Explicit instantiation for the different data type versions                                                        */
/*                                                                                                                    */
/**********************************************************************************************************************/

template VXLIB_STATUS VXLIB_subtract_exec_cn<VXLIB_SUBTRACT_TYPENAME_I8U_I8U_O8U>(VXLIB_kernelHandle handle,
                                                                                  void *restrict     pIn0,
                                                                                  void *restrict     pIn1,
                                                                                  void *restrict     pOut);

template VXLIB_STATUS VXLIB_subtract_exec_cn<VXLIB_SUBTRACT_TYPENAME_I8U_I8U_O16S>(VXLIB_kernelHandle handle,
                                                                                   void *restrict     pIn0,
                                                                                   void *restrict     pIn1,
                                                                                   void *restrict     pOut);

template VXLIB_STATUS VXLIB_subtract_exec_cn<VXLIB_SUBTRACT_TYPENAME_I8U_I16S_O16S>(VXLIB_kernelHandle handle,
                                                                                    void *restrict     pIn0,
                                                                                    void *restrict     pIn1,
                                                                                    void *restrict     pOut);

template VXLIB_STATUS VXLIB_subtract_exec_cn<VXLIB_SUBTRACT_TYPENAME_I16S_I16S_O16S>(VXLIB_kernelHandle handle,
                                                                                     void *restrict     pIn0,
                                                                                     void *restrict     pIn1,
                                                                                     void *restrict     pOut);

/* ======================================================================== */
/*  End of file:  VXLIB_subtract_cn.cpp                                     */
/* ======================================================================== */
