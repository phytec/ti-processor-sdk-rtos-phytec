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

#include <cstdint>
#define ELEMENT_COUNT(x) c7x::element_count_of<x>::value

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
/* MATHLIB_atan2                                                               */
/*                                                                            */
/******************************************************************************/

// this method performs a/b division for atan2 computation
template <typename vecType> static inline vecType divspMod_atan2_i(vecType a, vecType b)
{
   vecType res, r0, d0, d1, p0, p1, Two;

   Two = (vecType) 2.0;

   p0 = __recip(b);
   d0 = p0 * b;
   r0 = p0;

   p1 = Two - d0;
   d1 = r0 * p1;

   res = a * d1;

   return res;
}

// this method constructs the polynomial estimation for atan2 computation
template <typename vecType, typename vecBool>
static inline vecType atan22f_sr1i_atan2_i(vecType g1, vecType pih, vecBool s, vecBool bn, vecBool an)
{

   /*************************************************************************************/
   /* Create and assign values for constants and variables for polynomial approximation */
   /*************************************************************************************/
   vecType coef, negativeCoef, pi, pol, Zero, g2, g4, g6, g8, g10, g12, tmp1, tmp2, c1, c2, c3, c4, c5, c6, c7, c8, res,
       res_minus, res_plus;
   vecBool FALSE;

   FALSE = (vecBool) 0;
   Zero  = (vecType) 0.0;
   pi    = (vecType) 3.1415927;
   // pih = (vecType) 1.57079632679;

   c1 = (vecType) 0.00230158202;
   c2 = (vecType) -0.01394551000;
   c3 = (vecType) 0.03937087815;
   c4 = (vecType) -0.07235669163;
   c5 = (vecType) 0.10521499322;
   c6 = (vecType) -0.14175076797;
   c7 = (vecType) 0.19989300877;
   c8 = (vecType) -0.33332930041;

   coef = pih;

   // get coef based on the flags
   // check 'swap' flag
   //    if (s == FALSE) {
   //     coef = pi;
   //   }
   __vpred cmp_swap = __cmp_eq_pred(s, FALSE);
   coef             = __select(cmp_swap, pi, coef);
   // if ((s == FALSE) && (bn == FALSE)) {
   //  coef = 0.0f;

   __vpred cmp_negB      = __cmp_eq_pred(bn, FALSE);
   __vpred and_negB_swap = __and(cmp_negB, cmp_swap);

   coef = __select(and_negB_swap, Zero, coef);

   // check if input to atan2sp_i is negative
   // if (an != FALSE) {
   //    coef = -coef;
   // }
   negativeCoef     = -coef;
   __vpred cmp_sign = __cmp_eq_pred(an, FALSE);
   coef             = __select(cmp_sign, coef, negativeCoef);

   // calculate polynomial
   g2  = g1 * g1;
   g4  = g2 * g2;
   g6  = g2 * g4;
   g8  = g4 * g4;
   g10 = g6 * g4;
   g12 = g8 * g4;

   tmp1 = ((c5 * g8) + (c6 * g6)) + ((c7 * g4) + (c8 * g2));
   tmp2 = ((((c1 * g4) + (c2 * g2)) + c3) * g12) + (c4 * g10);

   pol = tmp1 + tmp2;
   pol = (pol * g1) + g1;

   // return ((s != FALSE) ? (coef - pol) : (coef + pol));
   res_minus       = coef - pol;
   res_plus        = coef + pol;
   __vpred cmp_res = __cmp_eq_pred(s, FALSE);
   res             = __select(cmp_res, res_plus, res_minus);

   return res;
}

// this method performs arc-tangent2 computation of input vector
template <typename T> MATHLIB_STATUS MATHLIB_atan2(size_t length, T *pSrc0, T *pSrc1, T *pDst)
{

   // variables
   MATHLIB_STATUS status       = MATHLIB_SUCCESS; // return function status
   size_t         numBlocks    = 0;               // compute loop's iteration count
   size_t         remNumBlocks = 0;               // when numBlocks is not a multiple of SIMD width

   // derive c7x vector type from template typename
   typedef typename c7x::make_full_vector<T>::type vec;
   __SE_TEMPLATE_v1                                se0Params = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1                                sa0Params = __gen_SA_TEMPLATE_v1();

   status = MATHLIB_checkParams(length, pSrc0, pSrc1, pDst);

   if (status == MATHLIB_SUCCESS) {
      MATHLIB_SE0SA01DSequentialInit(&se0Params, &sa0Params, length, pSrc0, pDst);

      // calculate compute loop's iteration counter
      numBlocks    = length / c7x::element_count_of<vec>::value;
      remNumBlocks = length % c7x::element_count_of<vec>::value;
      if (remNumBlocks) {
         numBlocks++;
      }

      // open SE0, SE1, and SA0 for reading and writing operands
      MATHLIB_SE0SE1SA0Open(&se0Params, &sa0Params, pSrc0, pSrc1);

      /***********************************************************************/
      /* Create and assign values for constants employed on atan2 computation */
      /***********************************************************************/
      vec           g, res, Zero, temp, abs_a, abs_b;
      c7x::char_vec an, bn, s, TRUE, FALSE;
      vec           pih = (vec) 1.570796327;
      vec           pi  = (vec) 3.141592741;
      vec           MAX = std::numeric_limits<T>::max();

      TRUE  = (c7x::char_vec) 1;
      FALSE = (c7x::char_vec) 0;
      Zero  = (vec) 0.0;

      // compute loop to perform vector atan2
      for (size_t i = 0; i < numBlocks; i++) {
         vec a = c7x::strm_eng<0, vec>::get_adv();
         vec b = c7x::strm_eng<1, vec>::get_adv();

         vec x = a;
         vec y = b;

         /* if (i == 0) { */
         /*    x.print(); */
         /*    y.print(); */
         /* } */

         s                = FALSE;
         __vpred cmp_negA = __cmp_lt_pred(a, Zero);
         an               = __select(cmp_negA, TRUE, FALSE);
         __vpred cmp_negB = __cmp_lt_pred(b, Zero);
         bn               = __select(cmp_negB, TRUE, FALSE);

         // swap a and b before calling division sub routine if a > b
         // if (_fabsf(a) > _fabsf(b) {
         //    temp = b;
         //    b = a;
         //    a    = temp;
         //    s    = TRUE;
         // }

         abs_a = __abs(a);
         abs_b = __abs(b);

         __vpred cmp_AgtB = __cmp_lt_pred(abs_b, abs_a);
         temp             = __select(cmp_AgtB, b, Zero);
         b                = __select(cmp_AgtB, a, b);
         a                = __select(cmp_AgtB, temp, a);
         s                = __select(cmp_AgtB, TRUE, s);
         /***********************************************************************/
         /* Division computation for atan2                                       */
         /***********************************************************************/
         g = divspMod_atan2_i<vec>(a, b);

         /***********************************************************************/
         /* Polynomial computation for atan2                                     */
         /***********************************************************************/
         res = atan22f_sr1i_atan2_i<vec, c7x::char_vec>(g, pih, s, bn, an);

         /***********************************************************************/
         /* Bounds Checking                                                     */
         /***********************************************************************/

         // if (x == 0.0f) {
         //    res = y >= 0.0f ? 0.0f : pi;
         // }

         __vpred cmp_zeroX = __cmp_eq_pred(x, Zero);

         __vpred cmp_zeroY = __cmp_le_pred(Zero, y);
         vec     resY      = __select(cmp_zeroY, Zero, pi);

         res = __select(cmp_zeroX, resY, res);

         // if (g > MAX) {

         // res = pih;
         // }

         __vpred gMax = __cmp_lt_pred(MAX, g);
         res          = __select(gMax, pih, res);

         // if (g < -MAX) {
         // res = -pih;
         // }

         vec MIN         = -MAX;
         vec negativepih = -pih;

         __vpred gMin = __cmp_lt_pred(g, MIN);
         res          = __select(gMin, negativepih, res);

         vec outVec = res;

         __vpred tmp  = c7x::strm_agen<0, vec>::get_vpred();
         vec    *addr = c7x::strm_agen<0, vec>::get_adv(pDst);
         __vstore_pred(tmp, addr, outVec);
      }

      MATHLIB_SE0SE1SA0Close();
   }

   return status;
}

/******************************************************************************/
/*                                                                            */
/* Explicit templatization for datatypes supported by MATHLIB_atan2              */
/*                                                                            */
/******************************************************************************/

// single precision
template MATHLIB_STATUS MATHLIB_atan2<float>(size_t length, float *pSrc0, float *pSrc1, float *pDst);

/******************************************************************************/
/*                                                                            */
/* C-interface wrapper functions                                              */
/*                                                                            */
/******************************************************************************/

extern "C" {

// single-precision wrapper
MATHLIB_STATUS MATHLIB_atan2_sp(size_t length, float *pSrc0, float *pSrc1, float *pDst)
{
   MATHLIB_STATUS status = MATHLIB_atan2(length, pSrc0, pSrc1, pDst);
   return status;
}

} // extern "C"
