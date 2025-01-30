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

#include "MATHLIB_permute.h"
#include "MATHLIB_pow_scalar.h"
#include "MATHLIB_types.h"
#include "MATHLIB_utility.h"
#include "MATHLIB_lut.h"

/******************************************************************************/
/*                                                                            */
/* MATHLIB_pow                                                                */
/*                                                                            */
/******************************************************************************/

template <typename vecType> static inline vecType MATHLIB_pow_log(vecType inVec)
{
   /**********************************************************************/
   /* Create and assign values for constants employed on log computation */
   /**********************************************************************/

   vecType         C1, C2, C3, C4, C5, eMax, outVecMax;
   c7x::double_vec ln2;
   c7x::uint_vec   zero;
   zero = (c7x::uint_vec) 0;

   ln2       = (c7x::double_vec) 0.693147180559945;
   C1        = (vecType) -0.2302894f;
   C2        = (vecType) 0.1908169f;
   C3        = (vecType) -0.2505905f;
   C4        = (vecType) 0.3333164f;
   C5        = (vecType) -0.5000002f;
   eMax      = (vecType) 3.402823466e+38f;
   outVecMax = (vecType) 88.72283905313f;

   /**********************************************************************/
   /* Create variables employed on log computation                       */
   /**********************************************************************/

   vecType         pol, r1, r2, r3, r4, outVec;
   c7x::double_vec inVecVals_odd, inVecVals_even, inVecVals_oddReciprocal, inVecVals_evenReciprocal,
       inVecReciprocalApprox_8_15, inVecReciprocalApprox_0_7, inVecVals_8_15, inVecVals_0_7, rVals_0_7, rVals_8_15,
       TVals_8_15, TVals_0_7, NVals_odd, NVals_even, NVals_0_7, NVals_8_15, pol_0_7, pol_8_15, outVec_8_15, outVec_0_7;
   c7x::uint_vec inVecReciprocal_32_63, inVecReciprocalClr_32_63, inVecReciprocalApprox_32_63, indexT, upperBitsIndexT,
       lowerBitsIndexT;
   c7x::int_vec N;

   /**********************************************************************/
   /* Calculate Taylor series approximation for log                      */
   /**********************************************************************/

   // Split vectors to compute r with double precision
   inVecVals_odd            = __high_float_to_double(inVec);
   inVecVals_even           = __low_float_to_double(inVec);
   inVecVals_oddReciprocal  = __recip(inVecVals_odd);
   inVecVals_evenReciprocal = __recip(inVecVals_even);

   // Create floating point reciprocal approximation
   // Upper 32 bits of all inVec reciprocal values
   inVecReciprocal_32_63 = c7x::reinterpret<c7x::uint_vec>(__permute_odd_odd_int(
       MATHLIB_vperm_data_interweave_0_63, c7x::as_uchar_vec(inVecVals_oddReciprocal),
       c7x::as_uchar_vec(inVecVals_evenReciprocal)));

   // Clear bits 0-16 inclusive
   inVecReciprocalClr_32_63 = inVecReciprocal_32_63 & 0xFFFE0000u;

   // Concatenate cleared bit reciprocal with zero bits
   inVecReciprocalApprox_8_15 = c7x::reinterpret<c7x::double_vec>(__permute_high_high(
       MATHLIB_vperm_data_interweave_0_63, c7x::as_uchar_vec(inVecReciprocalClr_32_63),
       c7x::as_uchar_vec(zero)));
   inVecReciprocalApprox_0_7  = c7x::reinterpret<c7x::double_vec>(
       __permute_low_low(MATHLIB_vperm_data_interweave_0_63, c7x::as_uchar_vec(inVecReciprocalClr_32_63),
                          c7x::as_uchar_vec(zero)));

   // Split inVec into two vectors with double precision
   inVecVals_0_7 = c7x::reinterpret<c7x::double_vec>(
       __permute_low_low(MATHLIB_vperm_data_dp_interweave_0_63, c7x::as_uchar_vec(inVecVals_odd),
                         c7x::as_uchar_vec(inVecVals_even)));
   inVecVals_8_15 = c7x::reinterpret<c7x::double_vec>(
       __permute_high_high(MATHLIB_vperm_data_dp_interweave_0_63, c7x::as_uchar_vec(inVecVals_odd),
                           c7x::as_uchar_vec(inVecVals_even)));

   // Calculate r in double precision
   rVals_0_7  = (inVecReciprocalApprox_0_7 * inVecVals_0_7) - 1.0;
   rVals_8_15 = (inVecReciprocalApprox_8_15 * inVecVals_8_15) - 1.0;

   // Convert r to float, compute r to the power of 2, 3, 4
   r1 = c7x::reinterpret<vecType>(
       __permute_even_even_int(MATHLIB_vperm_data_0_63, c7x::as_uchar_vec(__double_to_float(rVals_8_15)),
                               c7x::as_uchar_vec(__double_to_float(rVals_0_7))));
   r2 = r1 * r1;
   r3 = r1 * r2;
   r4 = r2 * r2;

   // Compute Taylor series polynomial
   pol = (C5 * r2) + ((C4 * r3) + ((((C2 * r1) + C3) + (C1 * r2)) * r4));

   /**********************************************************************/
   /* Calculate N                                                        */
   /**********************************************************************/

   // Upper 32 bits of all inVec reciprocal approximation values
   inVecReciprocalApprox_32_63 = c7x::reinterpret<c7x::uint_vec>(
       __permute_odd_odd_int(MATHLIB_vperm_data_0_63, c7x::as_uchar_vec(inVecReciprocalApprox_8_15),
                             c7x::as_uchar_vec(inVecReciprocalApprox_0_7)));

   N = c7x::convert<c7x::int_vec>(((inVecReciprocalApprox_32_63 << 1) >> 21) - 1023);

   // Covert N to double precision for later calculation with LUT values
   NVals_odd  = __high_int_to_double(N);
   NVals_even = __low_int_to_double(N);
   NVals_0_7  = c7x::reinterpret<c7x::double_vec>(__permute_low_low(MATHLIB_vperm_data_dp_interweave_0_63,
                                                                    c7x::as_uchar_vec(NVals_odd),
                                                                    c7x::as_uchar_vec(NVals_even)));
   NVals_8_15 = c7x::reinterpret<c7x::double_vec>(__permute_high_high(MATHLIB_vperm_data_dp_interweave_0_63,
                                                                      c7x::as_uchar_vec(NVals_odd),
                                                                      c7x::as_uchar_vec(NVals_even)));

   /**********************************************************************/
   /* Determine LUT values                                               */
   /**********************************************************************/

   // Calculate LUT index
   indexT = (((inVecReciprocalApprox_32_63 << 12) >> 29) + MATHLIB_LOGTABLE_OFFSET);

   // Read from LUT and reconstruct double values split into two vectors

   upperBitsIndexT = MATHLIB_LUTReadUpperBits(indexT);
   lowerBitsIndexT = MATHLIB_LUTReadLowerBits(indexT);

   TVals_8_15 = c7x::reinterpret<c7x::double_vec>(
       __permute_high_high(MATHLIB_vperm_data_interweave_0_63, c7x::as_uchar_vec(upperBitsIndexT),
                           c7x::as_uchar_vec(lowerBitsIndexT)));
   TVals_0_7 = c7x::reinterpret<c7x::double_vec>(__permute_low_low(MATHLIB_vperm_data_interweave_0_63,
                                                                   c7x::as_uchar_vec(upperBitsIndexT),
                                                                   c7x::as_uchar_vec(lowerBitsIndexT)));

   // Calculate an adjusted T
   TVals_8_15 = TVals_8_15 - (ln2 * NVals_8_15);
   TVals_0_7  = TVals_0_7 - (ln2 * NVals_0_7);

   /**********************************************************************/
   /* Calculate output with adjusted LUT and Taylor series values        */
   /**********************************************************************/

   // Split polynomial result into two vectors with double precision
   pol_0_7  = c7x::reinterpret<c7x::double_vec>(__permute_low_low(
        MATHLIB_vperm_data_dp_interweave_0_63, c7x::as_uchar_vec(__high_float_to_double(pol)),
        c7x::as_uchar_vec(__low_float_to_double(pol))));
   pol_8_15 = c7x::reinterpret<c7x::double_vec>(__permute_high_high(
       MATHLIB_vperm_data_dp_interweave_0_63, c7x::as_uchar_vec(__high_float_to_double(pol)),
       c7x::as_uchar_vec(__low_float_to_double(pol))));

   // Add LUT values and Taylor series values
   outVec_0_7  = rVals_0_7 + TVals_0_7 + pol_0_7;
   outVec_8_15 = rVals_8_15 + TVals_8_15 + pol_8_15;

   // Combine output vector into one floating point result
   outVec = c7x::reinterpret<vecType>(__permute_even_even_int(
       MATHLIB_vperm_data_0_63, c7x::as_uchar_vec(__double_to_float(outVec_8_15)),
       c7x::as_uchar_vec(__double_to_float(outVec_0_7))));

   /**********************************************************************/
   /* Bounds checking                                                    */
   /**********************************************************************/

   //  if (a > MAXe) {
   //     res = 709.7827f;
   //  }
   __vpred cmp_max = __cmp_lt_pred(eMax, inVec);
   outVec          = __select(cmp_max, outVecMax, outVec);

   return outVec;
}

template <typename T> static inline MATHLIB_STATUS MATHLIB_pow_cond(size_t length, T *pSrc0, T *pSrc1, T *pDst)
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
   __SE_TEMPLATE_v1 se1Params = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa0Params = __gen_SA_TEMPLATE_v1();

   // calculate compute loop's iteration counter
   numBlocks    = length / c7x::element_count_of<vec>::value;
   remNumBlocks = length % c7x::element_count_of<vec>::value;
   if (remNumBlocks) {
      numBlocks++;
   }

   __SE_ELETYPE SE_ELETYPE;
   __SE_VECLEN  SE_VECLEN;
   __SA_VECLEN  SA_VECLEN;

   // derive c7x vector type from template typename
   typedef typename c7x::make_full_vector<T>::type vec;

   // assign SE and SA params based on vector type
   SE_VECLEN  = c7x::se_veclen<vec>::value;
   SA_VECLEN  = c7x::sa_veclen<vec>::value;
   SE_ELETYPE = c7x::se_eletype<vec>::value;

   se0Params.ICNT0   = length;
   se0Params.ELETYPE = SE_ELETYPE;
   se0Params.VECLEN  = SE_VECLEN;
   se0Params.DIMFMT  = __SE_DIMFMT_1D;

   se1Params.DIMFMT        = __SE_DIMFMT_3D;
   se1Params.ICNT0         = c7x::element_count_of<vec>::value;
   se1Params.ELETYPE       = SE_ELETYPE;
   se1Params.VECLEN        = SE_VECLEN;
   se1Params.ICNT1         = 2;
   se1Params.ICNT2         = numBlocks;
   se1Params.DIM2          = c7x::element_count_of<vec>::value;
   se1Params.DECDIM1       = __SE_DECDIM_DIM2;
   se1Params.DECDIM1_WIDTH = length;
   se1Params.DIM1          = ((T *) pSrc1) - ((T *) pSrc0);

   sa0Params.ICNT0  = length;
   sa0Params.DIM1   = length;
   sa0Params.VECLEN = SA_VECLEN;
   sa0Params.DIMFMT = __SA_DIMFMT_1D;


      // open SE0, SE1, and SA0 for reading and writing operands
      __SE0_OPEN(pDst, se0Params);
      __SE1_OPEN(pSrc0, se1Params);
      __SA0_OPEN(sa0Params);

      /**********************************************************************/
      /* Create and assign values for constants employed on pow computation */
      /**********************************************************************/
      vec      zero, one, intMax, inf;
      vec_type zeroInt, signNegative;

      zero         = (vec) 0.0f;
      one          = (vec) 1.0f;
      intMax       = (vec) 0x7fffffffu;
      inf          = (vec) 0x7F800000u;
      zeroInt      = (vec_type) 0;
      signNegative = (vec_type) -1;
      // compute loop to perform vector pow
      for (size_t i = 0; i < numBlocks; i++) {
         vec inVec0 = c7x::strm_eng<1, vec>::get_adv();
         vec outVec = c7x::strm_eng<0, vec>::get_adv();
         vec inVec1 = c7x::strm_eng<1, vec>::get_adv();

         /**********************************************************************/
         /* Create variables employed on pow computation                       */
         /**********************************************************************/
         vec_type inVec1_round, inVec1_roundEven, sign;

         sign = (vec_type) 1;

         /**********************************************************************/
         /* Calculate sign of final result                                     */
         /**********************************************************************/
         // Negative base w/ odd-integer power should be negative
         inVec1_round     = __float_to_int(inVec1);
         inVec1_roundEven = inVec1_round & 1;

         // Check if base is < 0, if the power is an integer, if the power is odd
         __vpred cmp_zero    = __cmp_lt_pred(inVec0, zero);
         __vpred cmp_floateq = __cmp_eq_pred(inVec1, __int_to_float(inVec1_round));
         __vpred cmp_odd     = __negate(__cmp_eq_pred(inVec1_roundEven, zeroInt));
         __vpred cond1       = __and(cmp_zero, cmp_floateq);
         __vpred cond2       = __and(cond1, cmp_odd);

         // If all conditions are true, result is negative
         sign = __select(cond2, signNegative, sign);

         outVec = __int_to_float(sign) * outVec;

         /**********************************************************************/
         /* Bounds checking                                                    */
         /**********************************************************************/
         // Error if base < 0 and power is not an integer
         __vpred cond3 = __and(cmp_zero, __negate(cmp_floateq));
         outVec        = __select(cond3, intMax, outVec);

         __vpred cmp_inVec0_zero    = __cmp_eq_pred(inVec0, zero);
         __vpred cmp_inVec1_zero_lt = __cmp_lt_pred(inVec1, zero);
         __vpred cond4              = __and(cmp_inVec0_zero, __negate(cmp_inVec1_zero_lt));
         __vpred cond5              = __and(cmp_inVec0_zero, cmp_inVec1_zero_lt);

         // 0 if base is 0 and power is greater than or equal to 0
         // inf if base is 0 and power is less than zero
         outVec = __select(cond4, zero, outVec);
         outVec = __select(cond5, inf, outVec);

         // 1 if power is 0
         __vpred cmp_inVec1_zero = __cmp_eq_pred(inVec1, zero);
         outVec                  = __select(cmp_inVec1_zero, one, outVec);

         __vpred tmp  = c7x::strm_agen<0, vec>::get_vpred();
         vec    *addr = c7x::strm_agen<0, vec>::get_adv(pDst);
         __vstore_pred(tmp, addr, outVec);
      }
      MATHLIB_SE0SE1SA0Close();

   return status;
}

template <typename T>
static inline MATHLIB_STATUS
MATHLIB_pow_exp(__SE_TEMPLATE_v1 *se0Params, __SA_TEMPLATE_v1 *sa0Params, size_t length, T *pSrc0, T *pDst)
{
   // variables
   MATHLIB_STATUS status       = MATHLIB_SUCCESS; // return function status
   size_t         numBlocks    = 0;               // compute loop's iteration count
   size_t         remNumBlocks = 0;               // when numBlocks is not a multiple of SIMD width

   // derive c7x vector type from template typename
   typedef typename c7x::make_full_vector<T>::type vec;

   // calculate compute loop's iteration counter
   numBlocks    = length / c7x::element_count_of<vec>::value;
   remNumBlocks = length % c7x::element_count_of<vec>::value;
   if (remNumBlocks) {
      numBlocks++;
   }


      // open SE0, SE1, and SA0 for reading and writing operands
      MATHLIB_SE0SA0Open(se0Params, sa0Params, pDst);

      /**********************************************************************/
      /* Create and assign values for constants employed on pow computation */
      /**********************************************************************/

      vec           log2_base_x16, half, negativeHalf, zero, LnMin, LnMax, Max, C0, C1, C2, P1, P2;
      c7x::uint_vec mask, inVec_min;

      log2_base_x16 = (vec) 23.083120654f;
      half          = (vec) 0.5f;
      negativeHalf  = (vec) -0.5f;
      zero          = (vec) 0.0f;
      LnMin         = (vec) -87.33654475f;
      LnMax         = (vec) 88.72283905f;
      Max           = (vec) 3.402823466E+38f;
      C0            = (vec) 0.166668549286041f;
      C1            = (vec) 0.500016170012920f;
      C2            = (vec) 0.999999998618401f;
      P1            = (vec) 0.04331970214844f;
      P2            = (vec) 1.99663646e-6f;
      mask          = (c7x::uint_vec) 0x3u;
      inVec_min     = (c7x::uint_vec) 114u;

      // compute loop to perform vector pow
      for (size_t i = 0; i < numBlocks; i++) {
         vec inVec = c7x::strm_eng<0, vec>::get_adv();

         /**********************************************************************/
         /* Create variables employed on exp computation                       */
         /**********************************************************************/

         vec           pol, r, r2, r3, outVec, Nf, absNf;
         c7x::uint_vec J, K, uN, dTAdjusted_32_63, dT_32_63, dT_0_31, inVec_small, upperBitsK, lowerBitsK, upperBitsJ,
             lowerBitsJ;
         c7x::int_vec    N, minusN;
         c7x::double_vec KVals_8_15, KVals_0_7, JVals_8_15, JVals_0_7, dTVals_8_15, dTVals_0_7, pol_0_7, pol_8_15,
             outVec_0_7, outVec_8_15;

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
         r = (inVec - (P1 * __int_to_float(N))) - (P2 * __int_to_float(N));

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

         KVals_8_15 = c7x::reinterpret<c7x::double_vec>(
             __permute_high_high(MATHLIB_vperm_data_interweave_0_63, c7x::as_uchar_vec(upperBitsK),
                                 c7x::as_uchar_vec(lowerBitsK)));
         KVals_0_7  = c7x::reinterpret<c7x::double_vec>(__permute_low_low(MATHLIB_vperm_data_interweave_0_63,
                                                                          c7x::as_uchar_vec(upperBitsK),
                                                                          c7x::as_uchar_vec(lowerBitsK)));
         JVals_8_15 = c7x::reinterpret<c7x::double_vec>(
             __permute_high_high(MATHLIB_vperm_data_interweave_0_63, c7x::as_uchar_vec(upperBitsJ),
                                 c7x::as_uchar_vec(lowerBitsJ)));
         JVals_0_7 = c7x::reinterpret<c7x::double_vec>(__permute_low_low(MATHLIB_vperm_data_interweave_0_63,
                                                                         c7x::as_uchar_vec(upperBitsJ),
                                                                         c7x::as_uchar_vec(lowerBitsJ)));

         // Multiply LUT values
         dTVals_8_15 = KVals_8_15 * JVals_8_15;
         dTVals_0_7  = KVals_0_7 * JVals_0_7;

         /**********************************************************************/
         /* Scale exponent to adjust for 2^M                                   */
         /**********************************************************************/

         // Upper 32 bits of all dT values and lower 32 bits of all dT values
         dT_32_63 = c7x::reinterpret<c7x::uint_vec>(
             __permute_odd_odd_int(MATHLIB_vperm_data_0_63, c7x::as_uchar_vec(dTVals_8_15),
                                   c7x::as_uchar_vec(dTVals_0_7)));
         dT_0_31 = c7x::reinterpret<c7x::uint_vec>(
             __permute_even_even_int(MATHLIB_vperm_data_0_63, c7x::as_uchar_vec(dTVals_8_15),
                                     c7x::as_uchar_vec(dTVals_0_7)));

         uN               = (uN >> 4) << 20;
         dTAdjusted_32_63 = dT_32_63 + uN;

         // Concatenate the adjusted upper 32 bits of dT values to the lower 32 bits, convert dT to float
         dTVals_8_15 = c7x::reinterpret<c7x::double_vec>(
             __permute_high_high(MATHLIB_vperm_data_interweave_0_63, c7x::as_uchar_vec(dTAdjusted_32_63),
                                 c7x::as_uchar_vec(dT_0_31)));
         dTVals_0_7 = c7x::reinterpret<c7x::double_vec>(
             __permute_low_low(MATHLIB_vperm_data_interweave_0_63, c7x::as_uchar_vec(dTAdjusted_32_63),
                               c7x::as_uchar_vec(dT_0_31)));

         pol_0_7  = c7x::reinterpret<c7x::double_vec>(__permute_low_low(
              MATHLIB_vperm_data_dp_interweave_0_63, c7x::as_uchar_vec(__high_float_to_double(pol)),
              c7x::as_uchar_vec(__low_float_to_double(pol))));
         pol_8_15 = c7x::reinterpret<c7x::double_vec>(__permute_high_high(
             MATHLIB_vperm_data_dp_interweave_0_63, c7x::as_uchar_vec(__high_float_to_double(pol)),
             c7x::as_uchar_vec(__low_float_to_double(pol))));

         outVec_0_7  = dTVals_0_7 * (1.0f + pol_0_7);
         outVec_8_15 = dTVals_8_15 * (1.0f + pol_8_15);

         outVec = c7x::reinterpret<vec>(__permute_even_even_int(
             MATHLIB_vperm_data_0_63, c7x::as_uchar_vec(__double_to_float(outVec_8_15)),
             c7x::as_uchar_vec(__double_to_float(outVec_0_7))));

         /**********************************************************************/
         /* Bounds checking                                                    */
         /**********************************************************************/

         inVec_small       = ((c7x::as_uint_vec(inVec) << 1u) >> 24u);
         __vpred cmp_inVec = __cmp_gt_pred(inVec_min, inVec_small);
         outVec            = __select(cmp_inVec, 1.0f + inVec, outVec);

         // < LnMin returns 0
         // if (x < LnMin) {
         //    res = 0.0f;
         // }
         __vpred cmp_min = __cmp_lt_pred(inVec, LnMin);
         outVec          = __select(cmp_min, zero, outVec);

         // > LnMax returns MAX
         // if (x > LnMax) {
         //    res = Max;
         // }
         __vpred cmp_max = __cmp_lt_pred(LnMax, inVec);
         outVec          = __select(cmp_max, Max, outVec);

         // Store exp computation result in
         __vpred tmp  = c7x::strm_agen<0, vec>::get_vpred();
         vec    *addr = c7x::strm_agen<0, vec>::get_adv(pDst);
         __vstore_pred(tmp, addr, outVec);
      }
      MATHLIB_SE0SA0Close();
   return status;
}

template <typename T> static inline void MATHLIB_pow_vector(size_t length, T *pSrc0, T *pSrc1, T *pDst)
{
   size_t numBlocks    = 0; // compute loop's iteration count
   size_t remNumBlocks = 0; // when numBlocks is not a multiple of SIMD width

   // derive c7x vector type from template typename
   typedef typename c7x::make_full_vector<T>::type vec;

   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa0Params = __gen_SA_TEMPLATE_v1();

   // calculate compute loop's iteration counter
   numBlocks    = length / c7x::element_count_of<vec>::value;
   remNumBlocks = length % c7x::element_count_of<vec>::value;
   if (remNumBlocks) {
      numBlocks++;
   }
   MATHLIB_SE0SA01DSequentialInit(&se0Params, &sa0Params, length, pSrc0, pDst);

   // open SE0, SE1, and SA0 for reading and writing operands
   MATHLIB_SE0SE1SA0Open(&se0Params, &sa0Params, pSrc0, pSrc1);

   /**********************************************************************/
   /* Create and assign values for constants employed on pow computation */
   /**********************************************************************/

   vec zero, one;

   zero = (vec) 0.0f;
   one  = (vec) 1.0f;

   // compute loop to perform vector pow
   for (size_t i = 0; i < numBlocks; i++) {
      vec inVec0 = c7x::strm_eng<0, vec>::get_adv();
      vec inVec1 = c7x::strm_eng<1, vec>::get_adv();

      /**********************************************************************/
      /* Create variables employed on pow computation                       */
      /**********************************************************************/
      vec inVec_log, inVec_exp;

      /**********************************************************************/
      /* Log and exp computation                                            */
      /**********************************************************************/
      inVec_log       = __abs(inVec0);
      inVec_exp       = inVec1 * MATHLIB_pow_log(inVec_log);
      __vpred cmp_one = __cmp_eq_pred(inVec_log, one);
      inVec_exp       = __select(cmp_one, zero, inVec_exp);

      // Store log computation result in
      __vpred tmp  = c7x::strm_agen<0, vec>::get_vpred();
      vec    *addr = c7x::strm_agen<0, vec>::get_adv(pDst);
      __vstore_pred(tmp, addr, inVec_exp);
   }
   MATHLIB_SE0SE1SA0Close();

   MATHLIB_pow_exp(&se0Params, &sa0Params, length, pSrc0, pDst);
   MATHLIB_pow_cond(length, pSrc0, pSrc1, pDst);
}

// this method performs power computation of input vectors
template <typename T> MATHLIB_STATUS MATHLIB_pow(size_t length, T *pSrc0, T *pSrc1, T *pDst)
{
   // variables
   MATHLIB_STATUS status = MATHLIB_SUCCESS; // return function status

   status = MATHLIB_checkParams(length, pSrc0, pSrc1, pDst);

   if (status == MATHLIB_SUCCESS) {
      MATHLIB_pow_vector(length, pSrc0, pSrc1, pDst);
   }

   return status;
}

/******************************************************************************/
/*                                                                            */
/* Explicit templatization for datatypes supported by MATHLIB_pow             */
/*                                                                            */
/******************************************************************************/

// single precision
template MATHLIB_STATUS MATHLIB_pow<float>(size_t length, float *pSrc0, float *pSrc1, float *pDst);

/******************************************************************************/
/*                                                                            */
/* C-interface wrapper functions                                              */
/*                                                                            */
/******************************************************************************/

extern "C" {

// single-precision wrapper
MATHLIB_STATUS MATHLIB_pow_sp(size_t length, float *pSrc0, float *pSrc1, float *pDst)
{
   MATHLIB_STATUS status = MATHLIB_pow(length, pSrc0, pSrc1, pDst);
   return status;
}

} // extern "C"
