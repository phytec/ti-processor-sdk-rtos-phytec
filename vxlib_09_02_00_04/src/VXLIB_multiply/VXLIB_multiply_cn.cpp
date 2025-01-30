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

#include "VXLIB_multiply_priv.h"
/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_multiply_exec_cn */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method computes multiply in via natural C code

template <typename dTypeIn0, typename dTypeIn1, typename dTypeOut>
VXLIB_STATUS
VXLIB_multiply_exec_cn(VXLIB_kernelHandle handle, void *restrict pIn0, void *restrict pIn1, void *restrict pOut)
{
   VXLIB_DEBUGPRINTFN(0, "%s\n", "Enter Function");
   VXLIB_STATUS status = VXLIB_SUCCESS; // assign status to success by default

   typedef typename c7x::make_full_vector<dTypeIn0>::type vec;
   typedef
       typename std::conditional<ELEM_COUNT(c7x::uchar_vec) == ELEM_COUNT(vec), int32_t, int64_t>::type typeResultInt;

   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_multiply_PrivArgs *pKerPrivArgs = (VXLIB_multiply_PrivArgs *) handle;

   // obtain image parameters and overflow policy
   size_t                width             = pKerPrivArgs->width;
   size_t                height            = pKerPrivArgs->height;
   size_t                strideIn0Elements = pKerPrivArgs->strideIn0Elements;
   size_t                strideIn1Elements = pKerPrivArgs->strideIn1Elements;
   size_t                strideOutElements = pKerPrivArgs->strideOutElements;
   VXLIB_OVERFLOW_POLICY overFlowPolicy    = pKerPrivArgs->pKerInitArgs.overFlowPolicy;
   VXLIB_F32             scale             = pKerPrivArgs->pKerInitArgs.scale;

   // create local pointers
   dTypeIn0 *restrict pInLocal0 = (dTypeIn0 *) pIn0;
   dTypeIn1 *restrict pInLocal1 = (dTypeIn1 *) pIn1;
   dTypeOut *restrict pOutLocal = (dTypeOut *) pOut;

   // variables to hold temporary values
   uint32_t      x, y, src0Index, src1Index, dstIndex;
   int32_t       unscaledIntResult, finalResult;
   typeResultInt scaledIntResult;
   VXLIB_D64     unscaledDoubleResult, scaledDoubleResut;

   {
      for (y = 0; y < height; y++) {

         for (x = 0; x < width; x++) {
            // printf("\n\nrow: %d col: %d\n", y, x);
            src0Index = y * strideIn0Elements + x;
            src1Index = y * strideIn1Elements + x;
            dstIndex  = y * strideOutElements + x;

            unscaledIntResult    = pInLocal0[src0Index] * pInLocal1[src1Index];
            unscaledDoubleResult = (double) unscaledIntResult;
            scaledDoubleResut    = scale * unscaledDoubleResult;

            scaledIntResult = (typeResultInt) scaledDoubleResut;

            /* Finally, overflow-check as per the target type and policy. */
            if (overFlowPolicy == VXLIB_SATURATE) {
               if (scaledIntResult > VXLIB_MULTIPLY_NUMERIC_MAX(dTypeOut)) {
                  finalResult = VXLIB_MULTIPLY_NUMERIC_MAX(dTypeOut);
               }
               else if (scaledIntResult < VXLIB_MULTIPLY_NUMERIC_MIN(dTypeOut)) {
                  finalResult = VXLIB_MULTIPLY_NUMERIC_MIN(dTypeOut);
               }
               else {
                  finalResult = (dTypeOut) scaledIntResult;
               }
            }
            else {
               /* Just for show: the final assignment will wrap too. */
               finalResult = (dTypeOut) scaledIntResult;
            }
            pOutLocal[dstIndex] = (dTypeOut) finalResult;
         }
      }
   }

   VXLIB_DEBUGPRINTFN(0, "%s\n", "Exit Function");
   return (status);
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* Explicit instantiation for the different data type versions                                                        */
/*                                                                                                                    */
/**********************************************************************************************************************/

template VXLIB_STATUS VXLIB_multiply_exec_cn<VXLIB_MULTIPLY_TYPENAME_I8U_I8U_O8U>(VXLIB_kernelHandle handle,
                                                                                  void *restrict     pIn0,
                                                                                  void *restrict     pIn1,
                                                                                  void *restrict     pOut);

template VXLIB_STATUS VXLIB_multiply_exec_cn<VXLIB_MULTIPLY_TYPENAME_I8U_I8U_O16S>(VXLIB_kernelHandle handle,
                                                                                   void *restrict     pIn0,
                                                                                   void *restrict     pIn1,
                                                                                   void *restrict     pOut);

template VXLIB_STATUS VXLIB_multiply_exec_cn<VXLIB_MULTIPLY_TYPENAME_I8U_I16S_O16S>(VXLIB_kernelHandle handle,
                                                                                    void *restrict     pIn0,
                                                                                    void *restrict     pIn1,
                                                                                    void *restrict     pOut);

template VXLIB_STATUS VXLIB_multiply_exec_cn<VXLIB_MULTIPLY_TYPENAME_I16S_I16S_O16S>(VXLIB_kernelHandle handle,
                                                                                     void *restrict     pIn0,
                                                                                     void *restrict     pIn1,
                                                                                     void *restrict     pOut);

/* ======================================================================== */
/*  End of file:  VXLIB_multiply_cn.cpp                                     */
/* ======================================================================== */
