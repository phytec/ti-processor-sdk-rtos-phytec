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
/* MATHLIB_tan                                                                 */
/*                                                                            */
/******************************************************************************/

template <typename vecType> static inline vecType recipsp_tansp_i(vecType a)
{
   vecType two, zero;

   two  = (vecType) 2.0;
   zero = (vecType) 0.0;

   vecType x1, y;

   x1 = __recip(a);
   x1 = x1 * (two - (a * x1));
   y  = x1 * (two - (a * x1));

   //   if (a == zero) {
   //      y = zero;
   //   }
   __vpred cmp_eqzero = __cmp_eq_pred((vecType) a, zero);
   y                  = __select(cmp_eqzero, zero, y);

   return (y);
} /* End of recipsp_tansp_i */

template <typename vecType> static inline vecType pol_est_tansp_i(vecType x)
{
   vecType c12, c10, c8, c6, c4, c2;

   // coefficients for the polynomial for tan(x)
   c12 = (vecType) 0.00457708084094171;
   c10 = (vecType) 0.00867365970442499;
   c8  = (vecType) 0.0236488997700456;
   c6  = (vecType) 0.0523817525811416;
   c4  = (vecType) 0.133766510899828;
   c2  = (vecType) 0.333299506194791;

   vecType x2, x4, x6, x8, pol;
   vecType tmp1, tmp2;

   // calculate the powers of x
   x2 = x * x;
   x4 = x2 * x2;
   x6 = x2 * x4;
   x8 = x4 * x4;

   /**************************************************************************/
   /* The polynomial calculation is done in two separate parts.              */
   /*   tmp1 =  c2 x^2 + c4 x^4 + c6 x^6 + c8 x^8                            */
   /*   tmp2 =  c10 x^10 + c12 x^12                                          */
   /* In order to reduce the number of multiplications x is factored out of  */
   /* the polynomial and multiplied by later.                                */
   /**************************************************************************/

   tmp1 = ((c8 * x8) + (c6 * x6)) + ((c4 * x4) + (c2 * x2));
   tmp2 = x8 * ((c10 * x2) + (c12 * x4));

   pol = ((tmp1 + tmp2) * x) + x;

   return pol;
}

// this method performs tangent computation of input vector
template <typename T> MATHLIB_STATUS MATHLIB_tan(size_t length, T *pSrc, T *pDst)
{

   // variables
   MATHLIB_STATUS status       = MATHLIB_SUCCESS; // return function status
   size_t         numBlocks    = 0;               // compute loop's iteration count
   size_t         remNumBlocks = 0;               // when numBlocks is not a multiple of SIMD width

   // derive c7x vector type from template typename
   typedef typename c7x::make_full_vector<T>::type vec;

   // Compile-time decision: float_vec => int_vec and double_vec=> long_vec
   typedef
       typename std::conditional<ELEMENT_COUNT(c7x::float_vec) == ELEMENT_COUNT(vec), c7x::int_vec, c7x::long_vec>::type
           vec_type;

   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa0Params = __gen_SA_TEMPLATE_v1();

   // check for null pointers and non-zero length
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

      vec pi4, pi2_h, pi2_l, pi, invPI, C1, C2, C3, one, zero, negativeOne, res, y, temp, temp_gtPI4, res_gtPI4,
          y_gtPI4;

      pi4         = (vec) 0.785398163; // pi/4
      pi2_h       = (vec) 1.570796;
      pi2_l       = (vec) 3.26794896476912e-7; // pi2_h + pi2_l = pi/2
      pi          = (vec) 3.14159265358979;
      C1          = (vec) 3.140625;
      C2          = (vec) 9.676535e-4;
      C3          = (vec) 8.97930618748433e-11; // C1 + C2 + C3 = PI
      invPI       = (vec) 0.318309886183791;    // 1/pi
      one         = (vec) 1.0;
      zero        = (vec) 0.0;
      negativeOne = (vec) -1.0;

      vec_type N;

      // compute loop to perform vector tan
      for (size_t i = 0; i < numBlocks; i++) {
         vec a = c7x::strm_eng<0, vec>::get_adv();

         vec sign = one;
         // extract multiples of pi
         temp = a * invPI;

         // N = multiple of pi in x
         N = __float_to_int(temp);

         y = ((a - (c7x::convert<vec>(N) * C1)) - (c7x::convert<vec>(N) * C2)) - (c7x::convert<vec>(N) * C3);

         //       if(y > pi2_h){
         //          y = y - pi;                               // x is in the range [-pi/2, pi/2]
         //       }
         vec     yminusPi = y - pi;
         __vpred cmp_gt   = __cmp_lt_pred((vec) pi2_h, y);
         y                = __select(cmp_gt, yminusPi, y);

         //       if(y < zero){
         //          sign = -sign;                             // sign variable
         //          y = -y;
         //       }
         vec     negativeY = -y;
         __vpred cmp_zero  = __cmp_lt_pred(y, zero);
         y                 = __select(cmp_zero, negativeY, y);

         vec Sign_vec = __select(cmp_zero, negativeOne, sign);

         __vpred cmp_yPI4 = __cmp_lt_pred(pi4, y);

         y_gtPI4    = (pi2_h - y) + pi2_l; // pi/2 - y
         temp_gtPI4 = pol_est_tansp_i<vec>(y_gtPI4);
         res_gtPI4  = recipsp_tansp_i<vec>(temp_gtPI4); // 1/tan(pi/2 - y)
         res        = pol_est_tansp_i<vec>(y);

         res = __select(cmp_yPI4, res_gtPI4, res);

         res = res * Sign_vec;

         __vpred tmp = c7x::strm_agen<0, vec>::get_vpred();
         vec    *VB1 = c7x::strm_agen<0, vec>::get_adv(pDst);

         __vstore_pred(tmp, VB1, res);
      }

      MATHLIB_SE0SA0Close();
   }

   return status;
}

/******************************************************************************/
/*                                                                            */
/* Explicit templatization for datatypes supported by MATHLIB_tan              */
/*                                                                            */
/******************************************************************************/

// single precision
template MATHLIB_STATUS MATHLIB_tan<float>(size_t length, float *pSrc, float *pDst);

/******************************************************************************/
/*                                                                            */
/* C-interface wrapper functions                                              */
/*                                                                            */
/******************************************************************************/

extern "C" {

// single-precision wrapper
MATHLIB_STATUS MATHLIB_tan_sp(size_t length, float *pSrc, float *pDst)
{
   MATHLIB_STATUS status = MATHLIB_tan(length, pSrc, pDst);
   return status;
}

} // extern "C"
