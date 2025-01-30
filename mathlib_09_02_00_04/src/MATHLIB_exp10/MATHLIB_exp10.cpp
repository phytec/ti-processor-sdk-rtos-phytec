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

#include "MATHLIB_exp10_scalar.h"
#include "MATHLIB_lut.h"
#include "MATHLIB_permute.h"
#include "MATHLIB_types.h"
#include "MATHLIB_utility.h"
/******************************************************************************/
/*                                                                            */
/* MATHLIB_exp10                                                                */
/*                                                                            */
/******************************************************************************/

template <typename T> static inline void MATHLIB_exp10_scalar(size_t length, T *restrict pSrc, T *restrict pDst)
{
   // Call scalar code
   for (uint32_t i = 0; i < length; i++) {
      pDst[i] = MATHLIB_exp10_scalar_ci(pSrc[i]);
   }
}
template <typename T>
static inline void
MATHLIB_exp10_cond(__SE_TEMPLATE_v1 *se0Params, __SA_TEMPLATE_v1 *sa0Params, size_t length, T *pSrc0, T *pDst)
{
   // variables
   size_t numBlocks    = 0; // compute loop's iteration count
   size_t remNumBlocks = 0; // when numBlocks is not a multiple of SIMD width

   // derive c7x vector type from template typename
   typedef typename c7x::make_full_vector<T>::type vec;

   // calculate compute loop's iteration counter
   numBlocks    = length / c7x::element_count_of<vec>::value;
   remNumBlocks = length % c7x::element_count_of<vec>::value;
   if (remNumBlocks) {
      numBlocks++;
   }

   // open SE0, SE1, and SA0 for reading and writing operands
   MATHLIB_SE0SE1SA0Open(se0Params, sa0Params, pDst, pSrc0);

   /**********************************************************************/
   /* Create and assign values for constants employed on pow computation */
   /**********************************************************************/

   vec           k10e, LnMin, LnMax, Max, zero;
   c7x::uint_vec inVec_min;

   k10e      = (vec) 2.302585093f;
   LnMin     = (vec) -87.33654475f;
   LnMax     = (vec) 88.72283905f;
   Max       = (vec) 3.402823466E+38f;
   zero      = (vec) 0.0f;
   inVec_min = (c7x::uint_vec) 114u;

   // compute loop to perform vector pow
   for (size_t i = 0; i < numBlocks; i++) {
      vec outVec = c7x::strm_eng<0, vec>::get_adv();
      vec inVec  = c7x::strm_eng<1, vec>::get_adv();

      /**********************************************************************/
      /* Create variables employed on exp computation                       */
      /**********************************************************************/
      vec           Ye;
      c7x::uint_vec inVec_small;

      /**********************************************************************/
      /* Bounds checking                                                    */
      /**********************************************************************/
      Ye = k10e * inVec;
      // Early exit for small input
      //    if (_extu(_ftoi(Ye), 1u, 24u) < 114u) {
      //       res = 1.0f + Ye;
      //    }
      inVec_small       = ((c7x::reinterpret<c7x::uint_vec>(Ye) << 1u) >> 24u);
      __vpred cmp_inVec = __cmp_gt_pred(inVec_min, inVec_small);
      outVec            = __select(cmp_inVec, 1.0f + Ye, outVec);

      // < LnMin returns 0
      // if (x < LnMin) {
      //    res = 0.0f;
      // }
      __vpred cmp_min = __cmp_lt_pred(Ye, LnMin);
      outVec          = __select(cmp_min, zero, outVec);

      // > LnMax returns MAX
      // if (x > LnMax) {
      //    res = Max;
      // }
      __vpred cmp_max = __cmp_lt_pred(LnMax, Ye);
      outVec          = __select(cmp_max, Max, outVec);

      __vpred tmp  = c7x::strm_agen<0, vec>::get_vpred();
      vec    *addr = c7x::strm_agen<0, vec>::get_adv(pDst);
      __vstore_pred(tmp, addr, outVec);
   }
   MATHLIB_SE0SE1SA0Close();
}

template <typename T> static inline void MATHLIB_exp10_vector(size_t length, T *pSrc, T *pDst)
{
   size_t numBlocks    = 0; // compute loop's iteration count
   size_t remNumBlocks = 0; // when numBlocks is not a multiple of SIMD width

   // derive c7x vector type from template typename
   typedef typename c7x::make_full_vector<T>::type vec;

   // Compile-time decision: float_vec => int_vec and double_vec=> long_vec
   typedef
       typename std::conditional<ELEMENT_COUNT(c7x::float_vec) == ELEMENT_COUNT(vec), c7x::int_vec, c7x::long_vec>::type
           vec_type;

   typedef typename std::conditional<ELEMENT_COUNT(c7x::float_vec) == ELEMENT_COUNT(vec), c7x::uint_vec,
                                     c7x::ulong_vec>::type uvec_type;

   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa0Params = __gen_SA_TEMPLATE_v1();

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
   /* Create and assign values for constants employed on exp10 computation */
   /**********************************************************************/

   vec       log2_base_x16, half, negativeHalf, C0, C1, C2, P1, P2, k10e;
   uvec_type mask;

   log2_base_x16 = (vec) (16.0f * 3.321928095f);
   half          = (vec) 0.5f;
   negativeHalf  = (vec) -0.5f;
   C0            = (vec) 0.1667361910f;
   C1            = (vec) 0.4999999651f;
   C2            = (vec) 0.9999998881f;
   P1            = (vec) 0.04331970214844f;
   P2            = (vec) 1.99663646e-6f;
   k10e          = (vec) 2.302585093f;
   mask          = (uvec_type) 0x3u;

   // compute loop to perform vector exp10
   for (size_t i = 0; i < numBlocks; i++) {
      vec inVec = c7x::strm_eng<0, vec>::get_adv();

      /**********************************************************************/
      /* Create variables employed on exp10 computation                       */
      /**********************************************************************/

      vec             pol, r, r2, r3, outVec, Nf, absNf, Ye;
      uvec_type       J, K, uN, dTAdjusted_32_63, dT_32_63, dT_0_31;
      vec_type        N, minusN;
      c7x::double_vec KVals_8_15, KVals_0_7, JVals_8_15, JVals_0_7, dTVals_8_15, dTVals_0_7, pol_0_7, pol_8_15,
          outVec_0_7, outVec_8_15;
      c7x::uint_vec upperBitsK, lowerBitsK, upperBitsJ, lowerBitsJ;

      Ye = k10e * inVec;

      // Get N such that |N - inVec*16/ln(2)| is minimized
      Nf     = inVec * log2_base_x16;
      absNf  = Nf + half;
      N      = c7x::convert<vec_type>(absNf);
      minusN = N - 1;

      // if ((x * log2_base_x16) < - half) {
      //    N--;
      // }
      __vpred cmp_N = __cmp_lt_pred(Nf, negativeHalf);
      N             = __select(cmp_N, minusN, N);

      /**********************************************************************/
      /* Calculate Taylor series approximation for exp10                      */
      /**********************************************************************/
      r = (Ye - (P1 * __int_to_float(N))) - (P2 * __int_to_float(N));
      // Taylor series approximation
      r2  = r * r;
      r3  = r2 * r;
      pol = (r * C2) + ((r3 * C0) + (r2 * C1));

      /**********************************************************************/
      /* Get index of LUT and 2^M values                                    */
      /**********************************************************************/

      // Create vectors of LUT indices
      uN = c7x::convert<uvec_type>(N);
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

      // TODO: adjust calculation so that DT and POL are doubles then out is float

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

      __vpred tmp  = c7x::strm_agen<0, vec>::get_vpred();
      vec    *addr = c7x::strm_agen<0, vec>::get_adv(pDst);
      __vstore_pred(tmp, addr, outVec);
   }

   MATHLIB_SE0SA0Close();
   MATHLIB_exp10_cond(&se0Params, &sa0Params, length, pSrc, pDst);
}
// this method performs exponential 10 computation of input vector
template <typename T> MATHLIB_STATUS MATHLIB_exp10(size_t length, T *pSrc, T *pDst)
{
   MATHLIB_STATUS status = MATHLIB_SUCCESS; // return function status

   // check for null pointers and non-zero length
   status = MATHLIB_checkParams(length, pSrc, pDst);

   if (status == MATHLIB_SUCCESS) {
      MATHLIB_exp10_vector(length, pSrc, pDst);
   }
   return status;
}

/******************************************************************************/
/*                                                                            */
/* Explicit templatization for datatypes supported by MATHLIB_exp10             */
/*                                                                            */
/******************************************************************************/

// single precision
template MATHLIB_STATUS MATHLIB_exp10<float>(size_t length, float *pSrc, float *pDst);

// double precision
// template MATHLIB_STATUS MATHLIB_exp10<double>(size_t length, double *pSrc0, double *pDst);

/******************************************************************************/
/*                                                                            */
/* C-interface wrapper functions                                              */
/*                                                                            */
/******************************************************************************/

extern "C" {

// single-precision wrapper
MATHLIB_STATUS MATHLIB_exp10_sp(size_t length, float *pSrc, float *pDst)
{
   MATHLIB_STATUS status = MATHLIB_exp10(length, pSrc, pDst);
   return status;
}

// double-precision wrapper
// MATHLIB_STATUS MATHLIB_exp10_dp(size_t length, double *pSrc, double *pDst)
// {
//    MATHLIB_STATUS status = MATHLIB_exp10(length, pSrc, pDst);
//    return status;
// }
} // extern "C"
