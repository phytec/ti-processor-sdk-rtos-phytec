/******************************************************************************
 * Copyright (C) 2024 Texas Instruments Incorporated - http://www.ti.com/
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

#include "MATHLIB_lut.h"

#include "MATHLIB_permute.h"
#include "MATHLIB_tanh_scalar.h"
#include "MATHLIB_types.h"
#include "MATHLIB_utility.h"

/******************************************************************************/
/*                                                                            */
/* MATHLIB_tanh                                                               */
/*                                                                            */
/******************************************************************************/

template <typename T> static inline void MATHLIB_tanh_scalar(size_t length, T *restrict pSrc, T *restrict pDst)
{
   // Call scalar code
   for (uint32_t i = 0; i < length; i++) {
      pDst[i] = MATHLIB_tanh_scalar_ci(pSrc[i]);
   }
}

template <typename T>
static inline void
MATHLIB_tanh_exp(__SE_TEMPLATE_v1 *se0Params, __SA_TEMPLATE_v1 *sa0Params, T *pSrc, T *pDst, size_t numBlocks)
{
   // derive c7x vector type from template typename
   typedef typename c7x::make_full_vector<T>::type vec;

   // open SE0, SE1, and SA0 for reading and writing operands
   MATHLIB_SE0SA0Open(se0Params, sa0Params, pSrc);

   /**********************************************************************/
   /* Create and assign values for constants employed on exp computation */
   /**********************************************************************/

   vec             log2_base_x16, half, negativeHalf, C0, C1, C2, one, negTwo;
   c7x::uint_vec   mask;
   c7x::double_vec p;

   log2_base_x16 = (vec) 23.0831206542234f; // 1.442695041 * 16.0
   half          = (vec) 0.5f;
   negativeHalf  = (vec) -0.5f;
   mask          = (c7x::uint_vec) 0x3u;
   p             = (c7x::double_vec) 0.0433216987816623; // 1/log2_base_x16
   one           = (vec) 1.0f;
   negTwo        = (vec) -2.0f;

   // coefficients to approximate the decimal part of the result
   C0 = (vec) 0.166668549286041f;
   C1 = (vec) 0.500016170012920f;
   C2 = (vec) 0.999999998618401f;

   for (size_t i = 0; i < numBlocks; i++) {
      vec inVec = c7x::strm_eng<0, vec>::get_adv();

      /*****************************************************************/
      /* Create variables employed on exp computation                  */
      /*****************************************************************/
      vec             pol, r, r2, r3, outVec, Nf, absNf, rVals_odd, rVals_even;
      c7x::uint_vec   J, K, uN, dTAdjusted_32_63, dT_32_63, dT_0_31, upperBitsK, lowerBitsK, upperBitsJ, lowerBitsJ;
      c7x::int_vec    N, minusN;
      c7x::double_vec KVals_8_15, KVals_0_7, JVals_8_15, JVals_0_7, dTVals_8_15, dTVals_0_7, pol_0_7, pol_8_15,
          outVec_0_7, outVec_8_15, inVecVals_odd, inVecVals_even, NVals_odd, NVals_even;

      inVec = __abs(inVec) * negTwo;

      // Get N such that |N - inVec*16/ln(2)| is minimized
      Nf     = inVec * log2_base_x16;
      absNf  = Nf + half;
      N      = c7x::convert<c7x::int_vec>(absNf);
      minusN = N - 1;

      // if ((x * log2_base_x16) < - half) {
      //    N--;
      // }
      __vpred cmp_N = __cmp_lt_pred(Nf, negativeHalf);
      N             = __select(cmp_N, minusN, N);

      /**********************************************************************/
      /* Calculate Taylor series approximation for exp                      */
      /**********************************************************************/

      // Split vectors to compute r with double precision
      inVecVals_odd  = __high_float_to_double(inVec);
      inVecVals_even = __low_float_to_double(inVec);
      NVals_odd      = __high_int_to_double(N);
      NVals_even     = __low_int_to_double(N);
      rVals_odd      = __double_to_float((inVecVals_odd - (p * NVals_odd)));
      rVals_even     = __double_to_float((inVecVals_even - (p * NVals_even)));

      // Rejoin vectors to create r with floating point precision
      r = c7x::reinterpret<vec>(__permute_even_even_int(MATHLIB_vperm_data_interweave_0_63,
                                                        c7x::reinterpret<c7x::uchar_vec>(rVals_odd),
                                                        c7x::reinterpret<c7x::uchar_vec>(rVals_even)));

      // Taylor series approximation
      r2  = r * r;
      r3  = r2 * r;
      pol = (r * C2) + ((r3 * C0) + (r2 * C1));

      /**********************************************************************/
      /* Get index of LUT and 2^M values                                    */
      /**********************************************************************/

      // Create vectors of LUT indices
      uN = c7x::convert<c7x::uint_vec>(N);
      K  = ((uN << 28u) >> 30) + MATHLIB_KTABLE_OFFSET;
      J  = (uN & mask) + MATHLIB_JTABLE_OFFSET;

      // Read values from LUT and convert and store as doubles in split vectors
      upperBitsK = MATHLIB_LUTReadUpperBits(K);
      lowerBitsK = MATHLIB_LUTReadLowerBits(K);
      upperBitsJ = MATHLIB_LUTReadUpperBits(J);
      lowerBitsJ = MATHLIB_LUTReadLowerBits(J);

      KVals_8_15 = c7x::reinterpret<c7x::double_vec>(__permute_high_high(MATHLIB_vperm_data_interweave_0_63,
                                                                         c7x::reinterpret<c7x::uchar_vec>(upperBitsK),
                                                                         c7x::reinterpret<c7x::uchar_vec>(lowerBitsK)));
      KVals_0_7  = c7x::reinterpret<c7x::double_vec>(__permute_low_low(MATHLIB_vperm_data_interweave_0_63,
                                                                       c7x::reinterpret<c7x::uchar_vec>(upperBitsK),
                                                                       c7x::reinterpret<c7x::uchar_vec>(lowerBitsK)));
      JVals_8_15 = c7x::reinterpret<c7x::double_vec>(__permute_high_high(MATHLIB_vperm_data_interweave_0_63,
                                                                         c7x::reinterpret<c7x::uchar_vec>(upperBitsJ),
                                                                         c7x::reinterpret<c7x::uchar_vec>(lowerBitsJ)));
      JVals_0_7  = c7x::reinterpret<c7x::double_vec>(__permute_low_low(MATHLIB_vperm_data_interweave_0_63,
                                                                       c7x::reinterpret<c7x::uchar_vec>(upperBitsJ),
                                                                       c7x::reinterpret<c7x::uchar_vec>(lowerBitsJ)));

      // Multiply LUT values
      dTVals_8_15 = KVals_8_15 * JVals_8_15;
      dTVals_0_7  = KVals_0_7 * JVals_0_7;

      /**********************************************************************/
      /* Scale exponent to adjust for 2^M                                   */
      /**********************************************************************/

      // Upper 32 bits of all dT values and lower 32 bits of all dT values
      dT_32_63 = c7x::reinterpret<c7x::uint_vec>(__permute_odd_odd_int(MATHLIB_vperm_data_0_63,
                                                                       c7x::reinterpret<c7x::uchar_vec>(dTVals_8_15),
                                                                       c7x::reinterpret<c7x::uchar_vec>(dTVals_0_7)));
      dT_0_31  = c7x::reinterpret<c7x::uint_vec>(__permute_even_even_int(MATHLIB_vperm_data_0_63,
                                                                         c7x::reinterpret<c7x::uchar_vec>(dTVals_8_15),
                                                                         c7x::reinterpret<c7x::uchar_vec>(dTVals_0_7)));

      uN               = (uN >> 4) << 20;
      dTAdjusted_32_63 = dT_32_63 + uN;

      // Concatenate the adjusted upper 32 bits of dT values to the lower 32 bits, convert dT to float
      dTVals_8_15 = c7x::reinterpret<c7x::double_vec>(
          __permute_high_high(MATHLIB_vperm_data_interweave_0_63, c7x::reinterpret<c7x::uchar_vec>(dTAdjusted_32_63),
                              c7x::reinterpret<c7x::uchar_vec>(dT_0_31)));
      dTVals_0_7 = c7x::reinterpret<c7x::double_vec>(
          __permute_low_low(MATHLIB_vperm_data_interweave_0_63, c7x::reinterpret<c7x::uchar_vec>(dTAdjusted_32_63),
                            c7x::reinterpret<c7x::uchar_vec>(dT_0_31)));

      pol_0_7  = c7x::reinterpret<c7x::double_vec>(__permute_low_low(
          MATHLIB_vperm_data_dp_interweave_0_63, c7x::reinterpret<c7x::uchar_vec>(__high_float_to_double(pol)),
          c7x::reinterpret<c7x::uchar_vec>(__low_float_to_double(pol))));
      pol_8_15 = c7x::reinterpret<c7x::double_vec>(__permute_high_high(
          MATHLIB_vperm_data_dp_interweave_0_63, c7x::reinterpret<c7x::uchar_vec>(__high_float_to_double(pol)),
          c7x::reinterpret<c7x::uchar_vec>(__low_float_to_double(pol))));

      outVec_0_7  = dTVals_0_7 * (1.0f + pol_0_7);
      outVec_8_15 = dTVals_8_15 * (1.0f + pol_8_15);

      outVec = c7x::reinterpret<vec>(__permute_even_even_int(
          MATHLIB_vperm_data_0_63, c7x::reinterpret<c7x::uchar_vec>(__double_to_float(outVec_8_15)),
          c7x::reinterpret<c7x::uchar_vec>(__double_to_float(outVec_0_7))));

      outVec = outVec + one;

      __vpred tmp  = c7x::strm_agen<0, vec>::get_vpred();
      vec    *addr = c7x::strm_agen<0, vec>::get_adv(pDst);
      __vstore_pred(tmp, addr, outVec);
   }

   MATHLIB_SE0SA0Close();
}

template <typename T>
static inline void
MATHLIB_tanh_pol(__SE_TEMPLATE_v1 *se0Params, __SA_TEMPLATE_v1 *sa0Params, T *pSrc, T *pDst, size_t numBlocks)
{
   // derive c7x vector type from template typename
   typedef typename c7x::make_full_vector<T>::type vec;

   // open SE0, SE1, and SA0 for reading and writing operands
   MATHLIB_SE0SE1SA0Open(se0Params, sa0Params, pSrc, pDst);

   /*************************************************************************************************/
   /* Create and assign values for constants employed on tanh polynomial and reciprocal computation */
   /*************************************************************************************************/
   vec pol_bound, C16, C14, C12, C10, C8, C6, C4, C2, two, zero, fltMax, limit;
   pol_bound = (vec) 1.0f;
   limit     = (vec) 9.0f;
   C16       = (vec) 0.000244528812992865f;
   C14       = (vec) -0.00119005741172407f;
   C12       = (vec) 0.00349212803657248f;
   C10       = (vec) -0.00886323552990220f;
   C8        = (vec) 0.0218794885361552f;
   C6        = (vec) -0.0539682539682540f;
   C4        = (vec) 0.133333333333333f;
   C2        = (vec) -0.333333333333333f;
   two       = (vec) 2.0f;
   zero      = (vec) 0.0f;
   fltMax    = (vec) 3.40282347e+38f;

   for (size_t i = 0; i < numBlocks; i++) {
      vec expOut = c7x::strm_eng<1, vec>::get_adv(); // e^(-2x)

      /**********************************************************************************************/
      /* Create variables employed on tanh polynomial and reciprocal computation                    */
      /**********************************************************************************************/
      vec inVec_abs, x2, x4, x6, x8, x10, x12, pol1, pol2, pol, x1, div, expRecip, outVec, sign;

      sign = (vec) 1.0;

      /**************************************************************/
      /* Division computation done for inputs  1 < |input| <= 9     */
      /**************************************************************/
      x1  = __recip(expOut);
      x1  = x1 * (two - (expOut * x1));
      div = (x1 * (two - (expOut * x1))) * two;

      /************************************************/
      /* Boundary checking for reciprocal computation */
      /************************************************/

      //      if (__abs(expOut) > max) {
      //       div = 0;
      //     }
      __vpred cmp_gt_flt = __cmp_lt_pred(fltMax, __abs(expOut));
      div                = __select(cmp_gt_flt, zero, div);

      expRecip = div - 1.0f; // -1 + 2 / (1 + e^(-2x))

      /**************************************************************/
      /* Polynomial computation done for inputs  |input| < 1        */
      /**************************************************************/
      vec inVec = c7x::strm_eng<0, vec>::get_adv();
      inVec_abs = __abs(inVec);
      // calculate the power of input vector
      x2  = inVec_abs * inVec_abs;
      x4  = x2 * x2;
      x6  = x2 * x4;
      x8  = x4 * x4;
      x10 = x6 * x4;
      x12 = x8 * x4;

      pol1 = ((C8 * x8) + (C6 * x6)) + ((C4 * x4) + (C2 * x2));
      pol2 = (((C16 * x4) + (C14 * x2) + C12) * x12) + (C10 * x10);

      pol = pol1 + pol2;
      pol = (pol * inVec_abs) + inVec_abs;

      /**************************************************************/
      /* Determining tanh computation for each input                */
      /**************************************************************/

      /**************************************************************************/
      /* The type of calculation for tanh(x) depends on the value of x.         */
      /*   x_abs < 1.0, res = pol_est_tanhsp_i(input x)                         */
      /*                                                                        */
      /*   x_abs >= 1.0, res = -1 + 2 / (1 + e^(-2x))                           */
      /*                                                                        */
      /*   x_abs > 9.0, res = 1.0 (maximum value for tanh)                      */
      /* where x_abs is the absolute value of the input and res is the          */
      /* calculated value for tanh(x).                                          */
      /**************************************************************************/

      // if(x < pol_bound){
      //    compute polynomial
      // }
      __vpred cmp_lt_pol = __cmp_le_pred(inVec_abs, pol_bound); // |x| <= 1

      // if(x_abs <= limit){
      //    exp computation with |x|
      // }
      __vpred cmp_le_exp = __cmp_lt_pred(inVec_abs, limit); // 1.0 < |x|<= 9

      __vpred cmp_else_exp = __negate(__or(cmp_le_exp, cmp_lt_pol)); // |x| > 9

      outVec = __select(cmp_else_exp, pol_bound, expRecip);
      outVec = __select(cmp_lt_pol, pol, outVec);

      __vpred cmp_sign = __cmp_lt_pred(inVec, zero);
      sign             = __select(cmp_sign, -sign, sign);

      outVec = outVec * sign;

      __vpred tmp  = c7x::strm_agen<0, vec>::get_vpred();
      vec    *addr = c7x::strm_agen<0, vec>::get_adv(pDst);
      __vstore_pred(tmp, addr, outVec);
   }
}

// this method performs tanh computation of input vector
template <typename T> static inline void MATHLIB_tanh_vector(size_t length, T *pSrc, T *pDst)
{
   // variables
   size_t numBlocks    = 0; // compute loop's iteration count
   size_t remNumBlocks = 0; // when numBlocks is not a multiple of SIMD width

   // derive c7x vector type from template typename
   typedef typename c7x::make_full_vector<T>::type vec;

   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa0Params = __gen_SA_TEMPLATE_v1();

   MATHLIB_SE0SA01DSequentialInit(&se0Params, &sa0Params, length, pSrc, pDst);

   // calculate compute loop's iteration counter
   numBlocks    = length / c7x::element_count_of<vec>::value;
   remNumBlocks = length % c7x::element_count_of<vec>::value;
   if (remNumBlocks) {
      numBlocks++;
   }

   /***********************************************************************/
   /* Exp computation for tanh                                            */
   /***********************************************************************/

   MATHLIB_tanh_exp(&se0Params, &sa0Params, pSrc, pDst, numBlocks);

   /***********************************************************************/
   /* Polynomial and reciprocal computation for tanh                      */
   /***********************************************************************/
   MATHLIB_tanh_pol(&se0Params, &sa0Params, pSrc, pDst, numBlocks);
}

// this method performs exponential computation of input vector
template <typename T> MATHLIB_STATUS MATHLIB_tanh(size_t length, T *pSrc, T *pDst)
{
   MATHLIB_STATUS status = MATHLIB_SUCCESS; // return function status

   // check for null pointers and non-zero length
   status = MATHLIB_checkParams(length, pSrc, pDst);

   if (status == MATHLIB_SUCCESS) {
      MATHLIB_tanh_vector(length, pSrc, pDst);
   }
   return status;
}

/******************************************************************************/
/*                                                                            */
/* Explicit templatization for datatypes supported by MATHLIB_tanh            */
/*                                                                            */
/******************************************************************************/

// single precision
template MATHLIB_STATUS MATHLIB_tanh<float>(size_t length, float *pSrc, float *pDst);

/******************************************************************************/
/*                                                                            */
/* C-interface wrapper functions                                              */
/*                                                                            */
/******************************************************************************/

extern "C" {

// single-precision wrapper
MATHLIB_STATUS MATHLIB_tanh_sp(size_t length, float *pSrc, float *pDst)
{
   MATHLIB_STATUS status = MATHLIB_tanh(length, pSrc, pDst);
   return status;
}

} // extern "C"
