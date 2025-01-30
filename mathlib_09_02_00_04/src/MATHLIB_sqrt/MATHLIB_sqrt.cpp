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

#define ELEMENT_COUNT(x) c7x::element_count_of<x>::value
#define ELEMENT_TYPE(x) typename c7x::element_type_of<x>::type

/******************************************************************************/
/*                                                                            */
/* Includes                                                                   */
/*                                                                            */
/******************************************************************************/

#include "MATHLIB_types.h"
#include "MATHLIB_utility.h"
#include <cstddef>
#include <limits>

/******************************************************************************/
/*                                                                            */
/* MATHLIB_sqrt                                                                 */
/*                                                                            */
/******************************************************************************/

// this method performs sqrt computation of input vector
template <typename T> MATHLIB_STATUS MATHLIB_sqrt(size_t length, T *pSrc, T *pDst)
{

   // variables
   MATHLIB_STATUS status       = MATHLIB_SUCCESS; // return function status
   size_t         numBlocks    = 0;               // compute loop's iteration count
   size_t         remNumBlocks = 0;               // when numBlocks is not a multiple of SIMD width

   // derive c7x vector type from template typename
   typedef typename c7x::make_full_vector<T>::type vec;

   /* define type of elements vec vector holds as elemType */
   typedef ELEMENT_TYPE(vec) elemType;

   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa0Params = __gen_SA_TEMPLATE_v1();

   status = MATHLIB_checkParams(length, pSrc, pDst);

   if (status == MATHLIB_SUCCESS) {
      MATHLIB_SE0SA01DSequentialInit(&se0Params, &sa0Params, length, pSrc, pDst);

      // calculate compute loop's iteration counter
      numBlocks    = length / c7x::element_count_of<vec>::value;
      remNumBlocks = length % c7x::element_count_of<vec>::value;
      if (remNumBlocks) {
         numBlocks++;
      }

      // open SE0, SE1, and SA0 for reading and writing operands
      MATHLIB_SE0SA0Open(&se0Params, &sa0Params, pSrc);

      /***********************************************************************/
      /* Create and assign values for constants employed on sqrt computation */
      /***********************************************************************/

      vec half, OneP5, zero, maxValue;

      half     = (vec) 0.5;
      OneP5    = (vec) 1.5;
      zero     = (vec) 0.0;
      maxValue = (vec) std::numeric_limits<elemType>::max();

      // compute loop to perform vector sqrt
      for (size_t i = 0; i < numBlocks; i++) {
         vec inVec = c7x::strm_eng<0, vec>::get_adv();

         /**********************************************************************/
         /* Create variables employed on sqrt computation                      */
         /**********************************************************************/

         vec y, p0, p1, d0;

         /**********************************************************************/
         /* Sqrt computation                                                   */
         /**********************************************************************/

         // Reciprocal square root calculation
         p0 = __recip_sqrt(inVec);
         d0 = p0 * inVec;
         p1 = OneP5 - d0 * p0 * half;
         y  = inVec * p0 * p1;

         /**********************************************************************/
         /* Bounds checking                                                    */
         /**********************************************************************/

         // If input is <= 0, output defaults to 0
         __vpred cmp_lezero = __cmp_le_pred(inVec, zero);
         y                  = __select(cmp_lezero, zero, y);

         // If input is greater than maximum allowed by data type, then output is max of datatype
         __vpred cmp_gtmax = __cmp_le_pred(maxValue, inVec);
         vec     outVec    = __select(cmp_gtmax, maxValue, y);

         __vpred tmp  = c7x::strm_agen<0, vec>::get_vpred();
         vec    *addr = c7x::strm_agen<0, vec>::get_adv(pDst);
         __vstore_pred(tmp, addr, outVec);
      }

      MATHLIB_SE0SA0Close();
   }

   return status;
}

/******************************************************************************/
/*                                                                            */
/* Explicit templatization for datatypes supported by MATHLIB_sqrt              */
/*                                                                            */
/******************************************************************************/

// single precision
template MATHLIB_STATUS MATHLIB_sqrt<float>(size_t length, float *pSrc, float *pDst);

/******************************************************************************/
/*                                                                            */
/* C-interface wrapper functions                                              */
/*                                                                            */
/******************************************************************************/

extern "C" {

// single-precision wrapper
MATHLIB_STATUS MATHLIB_sqrt_sp(size_t length, float *pSrc, float *pDst)
{
   MATHLIB_STATUS status = MATHLIB_sqrt(length, pSrc, pDst);
   return status;
}

} // extern "C"
