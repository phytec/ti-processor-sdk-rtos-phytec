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
#include <float.h>

/******************************************************************************/
/*                                                                            */
/* MATHLIB_asin                                                               */
/*                                                                            */
/******************************************************************************/

template <typename vecType> static inline vecType sqrt_asin_i(vecType a);
template <typename vecType> static inline vecType pol_est_asin_i(vecType x);
template <typename T> MATHLIB_STATUS              MATHLIB_asin(size_t length, T *pSrc, T *pDst);

template <typename vecType> static inline vecType sqrt_asin_i(vecType a)
{

   // define type of elements vecType vector holds as elemType
   typedef ELEMENT_TYPE(vecType) elemType;

   vecType half, OneP5, zero, maxValue;

   half     = (vecType) 0.5;
   OneP5    = (vecType) 1.5;
   zero     = (vecType) 0.0;
   maxValue = (vecType) std::numeric_limits<elemType>::max();

   vecType p0, p1, r0, d0, y;

   p0 = __recip_sqrt(a);
   r0 = p0;
   d0 = p0 * a;

   p1 = OneP5 - d0 * p0 * half;
   y  = a * r0 * p1;

   //   if (a <= zero) {
   //     y = zero;
   //   }
   __vpred cmp_lezero = __cmp_le_pred((vecType) a, zero);
   y                  = __select(cmp_lezero, zero, y);

   //   if (a > FLT_MAX) {
   //     y = FLT_MAX;
   //   }
   __vpred cmp_gtmax = __cmp_lt_pred(maxValue, (vecType) a);
   y                 = __select(cmp_gtmax, maxValue, y);

   return y;
}

template <typename vecType> static inline vecType pol_est_asin_i(vecType x)
{
   /**********************************************************************/
   /* Coefficient declaration for the polynomial for asin(x)             */
   /**********************************************************************/

   vecType c16, c14, c12, c10, c8, c6, c4, c2, x2, x4, x6, x8, x10, x12, pol, tmp1, tmp2;

   c16 = (vecType) 0.053002771381990;
   c14 = (vecType) -0.010980624698693;
   c12 = (vecType) 0.020659425186833;
   c10 = (vecType) 0.022862784546374;
   c8  = (vecType) 0.030636056280974;
   c6  = (vecType) 0.044450959710588;
   c4  = (vecType) 0.075034659380970;
   c2  = (vecType) 0.166664771293503;

   // calculate the powers of x in polynomial
   x2  = x * x;
   x4  = x2 * x2;
   x6  = x2 * x4;
   x8  = x4 * x4;
   x10 = x6 * x4;
   x12 = x8 * x4;

   /****************************************************************************/
   /* Polynomial calculation to estimate the arc_sine funtion.                 */
   /* The polynomial used is as follows:                                       */
   /*   pol = x + c2 x^3 + c4 x^5 + c6 x^7 + c8 x^9 + c10 x^11 + c12 x^13 +    */
   /*          c14 x^15 + c16 x^17,                                            */
   /* where x is the input, c2 through c16 are the corresponding coefficients  */
   /* to the polynomial, and pol is the result of the polynomial. This         */
   /* polynomial only covers inputs in the range [0, 1/sqrt(2)].               */
   /****************************************************************************/

   /**************************************************************************/
   /* The polynomial calculation is done in two separate parts.              */
   /*   tmp1 =  c2 x^2 + c4 x^4 + c6 x^6 + c8 x^8                            */
   /*   tmp2 =  c10 x^10 + c12 x^12 + c14 x^14 + c16 x^16                    */
   /* In order to reduce the number of multiplications x is factored out of  */
   /* the polynomial and multiplied by later.                                */
   /**************************************************************************/

   tmp1 = ((c8 * x8) + (c6 * x6)) + ((c4 * x4) + (c2 * x2));
   tmp2 = ((((c16 * x4) + (c14 * x2)) + c12) * x12) + (c10 * x10);

   pol = tmp1 + tmp2;
   pol = (pol * x) + x;

   return pol;
}

// this method performs arc-sine computation of input vector
template <typename T> MATHLIB_STATUS MATHLIB_asin(size_t length, T *pSrc, T *pDst)
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

      /**********************************************************************/
      /* Create and assign values for constants employed on asin computation */
      /**********************************************************************/

      vec HalfPI, Zero, One, rsqr2, s, negativeOne;
      vec res, x_abs, a, temp1, temp2, negativeS;
      vec poly, scale, offset;

      HalfPI      = (vec) 1.570796327;  // pi/2
      rsqr2       = (vec) 0.7071067811; // 1/sqrt(2)
      One         = (vec) 1.0;
      Zero        = (vec) 0.0;
      negativeOne = -One;

      vec nan = (vec) 0x7FFFFFFFu;

      // compute loop to perform vector asin
      for (size_t i = 0; i < numBlocks; i++) {
         vec inVec = c7x::strm_eng<0, vec>::get();

         // Sign Vector
         s = (vec) 1.0;

         /******************************************************************/
         /* Modified Asin algorithm to decrease number of cycles (ii)      */
         /* Instead of pre-calculating both outcomes of |x| > 1/sqrt(2)    */
         /* only determine which arg to pass to pol_est and call once      */
         /*                                                                */
         /*                   Final Form of Result                         */
         /*              res = scale * poly(arg) + offset                  */
         /* for |x| <= 1/sqrt(2): scale = 1, offset = 0, arg = |x|         */
         /* for |x| > 1/np.sqrt(2): scale = -1, offset = pi/2, arg = 1-x^2 */
         /******************************************************************/

         /***********************************************************************/
         /* Calculate sqrt for asin computation                                 */
         /***********************************************************************/
         x_abs = __abs(inVec);
         a     = One - (x_abs * x_abs);
         // sqrt(1 - x^2)
         temp1 = sqrt_asin_i<vec>(a);

         // |x| > 1/sqrt(2)
         __vpred cmp_x_abs = __cmp_lt_pred(rsqr2, x_abs);
         temp2             = __select(cmp_x_abs, temp1, x_abs);
         offset            = __select(cmp_x_abs, HalfPI, Zero);
         scale             = __select(cmp_x_abs, negativeOne, One);

         /***********************************************************************/
         /* Calculate polynomial for asin computation                           */
         /***********************************************************************/
         // For |x| <= 1/sqrt(2)
         poly = pol_est_asin_i<vec>(temp2);

         res = scale * poly + offset;

         /***********************************************************************/
         /* Sign adjustment for quadrant 3 & 4                                  */
         /***********************************************************************/

         // if(x < 0.0f){
         //    s = -s;
         // }
         negativeS = -s;

         __vpred cmp_lt_zero = __cmp_lt_pred(inVec, Zero);
         s                   = __select(cmp_lt_zero, negativeS, s);
         res                 = res * s;

         /***********************************************************************/
         /* Bounds checking                                                     */
         /***********************************************************************/

         // if(x_abs > 1.0f){
         //    res = _itof(0x7FFFFFFFu); NaN
         // }
         vec     inVec1     = c7x::strm_eng<0, vec>::get_adv();
         vec     x_abs1     = __abs(inVec1);
         __vpred cmp_gt_one = __cmp_lt_pred(One, x_abs1);
         res                = __select(cmp_gt_one, nan, res);

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
/* Explicit templatization for datatypes supported by MATHLIB_asin            */
/*                                                                            */
/******************************************************************************/

// single precision
template MATHLIB_STATUS MATHLIB_asin<float>(size_t length, float *pSrc, float *pDst);

/******************************************************************************/
/*                                                                            */
/* C-interface wrapper functions                                              */
/*                                                                            */
/******************************************************************************/

extern "C" {

// single-precision wrapper
MATHLIB_STATUS MATHLIB_asin_sp(size_t length, float *pSrc, float *pDst)
{
   MATHLIB_STATUS status = MATHLIB_asin(length, pSrc, pDst);
   return status;
}

} // extern "C"
