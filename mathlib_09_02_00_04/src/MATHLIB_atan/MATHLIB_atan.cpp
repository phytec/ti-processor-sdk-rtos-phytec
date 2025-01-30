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
/* the type of data that 'type x data structure' (i.e. vecType) holds */
#define ELEMENT_TYPE(x) typename c7x::element_type_of<x>::type

/******************************************************************************/
/*                                                                            */
/* Includes                                                                   */
/*                                                                            */
/******************************************************************************/

#include "MATHLIB_types.h"
#include "MATHLIB_utility.h"
#include <cstddef>

/******************************************************************************/
/*                                                                            */
/* MATHLIB_atan                                                               */
/*                                                                            */
/******************************************************************************/

template <typename vecType> static inline vecType divspMod_atan_i(vecType a, vecType b);
template <typename vecType> static inline vecType atan2f_sr1i_atan_i(vecType g1, vecType s, vecType an);
template <typename T> MATHLIB_STATUS              MATHLIB_atan(size_t length, T *pSrc, T *pDst);

template <typename vecType> static inline vecType divspMod_atan_i(vecType a, vecType b)
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

template <typename vecType, typename vecBool>
static inline vecType atan2f_sr1i_atan_i(vecType g1, vecBool s, vecBool an)
{

   /*************************************************************************************/
   /* Create and assign values for constants and variables for polynomial approximation */
   /*************************************************************************************/
   vecType coef, negativeCoef, pih, pol, Zero, g2, g4, g6, g8, g10, g12, tmp1, tmp2, c1, c2, c3, c4, c5, c6, c7, c8,
       res, res_minus, res_plus;
   vecBool FALSE;

   FALSE = (vecBool) 0;
   Zero  = (vecType) 0.0;

   pih = (vecType) 1.57079632679;

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
   //     coef = 0.0f;
   //   }
   __vpred cmp_swap = __cmp_eq_pred(s, FALSE);
   coef             = __select(cmp_swap, Zero, coef);

   // check if input to atansp_i is negative
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

// this method performs arc-tangent computation of input vector
template <typename T> MATHLIB_STATUS MATHLIB_atan(size_t length, T *pSrc, T *pDst)
{

   // variables
   MATHLIB_STATUS status       = MATHLIB_SUCCESS; // return function status
   size_t         numBlocks    = 0;               // compute loop's iteration count
   size_t         remNumBlocks = 0;               // when numBlocks is not a multiple of SIMD width

   // derive c7x vector type from template typename
   typedef typename c7x::make_full_vector<T>::type vec;

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
      /* Create and assign values for constants employed on atan computation */
      /***********************************************************************/
      vec           g, res, b, x_abs, One, Zero;
      c7x::char_vec an, s, TRUE, FALSE;

      TRUE  = (c7x::char_vec) 1;
      FALSE = (c7x::char_vec) 0;
      One   = (vec) 1.0;
      Zero  = (vec) 0.0;

      // compute loop to perform vector atan
      for (size_t i = 0; i < numBlocks; i++) {
         vec inVec = c7x::strm_eng<0, vec>::get();

         s     = (c7x::char_vec) 0;
         b     = (vec) 1.0;
         x_abs = __abs(inVec);

         // an = (a < 0.0f) ? TRUE : FALSE;
         __vpred cmp_flag = __cmp_lt_pred(inVec, Zero);
         an               = __select(cmp_flag, TRUE, FALSE);

         // swap a and b before calling division sub routine if a > b
         // if (_fabsf(a) > 1.0f) {
         //    temp = a;
         //    a    = 1.0f;
         //    s    = TRUE;
         // }
         vec a = c7x::strm_eng<0, vec>::get_adv();

         __vpred cmp_div = __cmp_lt_pred(One, x_abs);
         b               = __select(cmp_div, a, b);
         a               = __select(cmp_div, One, a);
         s               = __select(cmp_div, TRUE, s);

         /***********************************************************************/
         /* Division computation for atan                                       */
         /***********************************************************************/
         g = divspMod_atan_i<vec>(a, b);

         /***********************************************************************/
         /* Polynomial computation for atan                                     */
         /***********************************************************************/
         res = atan2f_sr1i_atan_i<vec, c7x::char_vec>(g, s, an);

         /***********************************************************************/
         /* Bounds Checking                                                     */
         /***********************************************************************/

         // if (a == 0.0f) {
         //    res = 0.0f;
         // }
         __vpred cmp_zero = __cmp_eq_pred(a, Zero);
         res              = __select(cmp_zero, Zero, res);

         vec outVec = res;

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
/* Explicit templatization for datatypes supported by MATHLIB_atan              */
/*                                                                            */
/******************************************************************************/

// single precision
template MATHLIB_STATUS MATHLIB_atan<float>(size_t length, float *pSrc, float *pDst);

/******************************************************************************/
/*                                                                            */
/* C-interface wrapper functions                                              */
/*                                                                            */
/******************************************************************************/

extern "C" {

// single-precision wrapper
MATHLIB_STATUS MATHLIB_atan_sp(size_t length, float *pSrc, float *pDst)
{
   MATHLIB_STATUS status = MATHLIB_atan(length, pSrc, pDst);
   return status;
}

} // extern "C"
