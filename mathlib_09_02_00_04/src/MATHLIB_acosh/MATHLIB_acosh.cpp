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

#include "MATHLIB_lut.h"

#include "MATHLIB_acosh_scalar.h"
#include "MATHLIB_permute.h"
#include "MATHLIB_types.h"
#include "MATHLIB_utility.h"
/******************************************************************************/
/*                                                                            */
/* MATHLIB_acosh                                                               */
/*                                                                            */
/******************************************************************************/

template <typename T> static inline void MATHLIB_acosh_scalar(size_t length, T *restrict pSrc, T *restrict pDst)
{
   // Call scalar code
   for (uint32_t i = 0; i < length; i++) {
      pDst[i] = MATHLIB_acosh_scalar_ci(pSrc[i]);
   }
}

// this method computes square root expressions in the acosh_sqrt sub-kernel
template <typename vecType> static inline vecType sqrt_acosh_i(vecType a, vecType x)
{
   vecType half, OneP5, zero;
   /* vec zero, maxValue; */

   half  = (vecType) 0.5;
   zero  = (vecType) 0.0;
   OneP5 = (vecType) 1.5;

   vecType p0, p1, r0, d0, y;

   p0 = __recip_sqrt(a);
   r0 = p0;
   d0 = p0 * a;

   p1 = OneP5 - d0 * p0 * half;
   y  = a * r0 * p1;

   vecType x2 = x * x;

   // if input a is x^2, select x as output
   __vpred cmp_xsqr = __cmp_eq_pred(a, x2);
   y                = __select(cmp_xsqr, x, y);

   // if input a is 0, select 0 as output
   __vpred cmp_zero = __cmp_eq_pred(a, zero);
   y                = __select(cmp_zero, zero, y);

   return y;
}

template <typename T>
static inline MATHLIB_STATUS
MATHLIB_acosh_log(__SE_TEMPLATE_v1 *se0Params, __SA_TEMPLATE_v1 *sa0Params, size_t length, T *pSrc0, T *pDst)
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

   if (status == MATHLIB_SUCCESS) {

      // open SE0, SE1, and SA0 for reading and writing operands
      // MATHLIB_SE0SA0Open(se0Params, sa0Params, pDst);
      MATHLIB_SE0SE1SA0Open(se0Params, sa0Params, pDst, pSrc0);

      /**********************************************************************/
      /* Create and assign values for constants employed on pow computation */
      /**********************************************************************/

      vec             C1, C2, C3, C4, C5, eMax, outVecMax;
      c7x::double_vec ln2;
      c7x::uint_vec   zero;
      zero = (c7x::uint_vec) 0;

      ln2       = (c7x::double_vec) 0.693147180559945;
      C1        = (vec) -0.2302894f;
      C2        = (vec) 0.1908169f;
      C3        = (vec) -0.2505905f;
      C4        = (vec) 0.3333164f;
      C5        = (vec) -0.5000002f;
      eMax      = (vec) 3.402823466e+38f;
      outVecMax = (vec) 88.72283905313f;

      vec one, nan, ln2sp;

      one   = (vec) 1.0f;
      nan   = (vec) 0x7FFFFFFFu;
      ln2sp = (vec) 0.69314718056f; // ln(2)

      // compute loop to perform vector pow
      for (size_t i = 0; i < numBlocks; i++) {
         vec inVec = c7x::strm_eng<0, vec>::get_adv();

         /**********************************************************************/
         /* Create variables employed on log computation                       */
         /**********************************************************************/

         vec             pol, r1, r2, r3, r4, outVec;
         c7x::double_vec inVecVals_odd, inVecVals_even, inVecVals_oddReciprocal, inVecVals_evenReciprocal,
             inVecReciprocalApprox_8_15, inVecReciprocalApprox_0_7, inVecVals_8_15, inVecVals_0_7, rVals_0_7,
             rVals_8_15, TVals_8_15, TVals_0_7, NVals_odd, NVals_even, NVals_0_7, NVals_8_15, pol_0_7, pol_8_15,
             outVec_8_15, outVec_0_7;
         c7x::uint_vec inVecReciprocal_32_63, inVecReciprocalClr_32_63, inVecReciprocalApprox_32_63, indexT,
             upperBitsIndexT, lowerBitsIndexT;
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
             MATHLIB_vperm_data_interweave_0_63, c7x::reinterpret<c7x::uchar_vec>(inVecVals_oddReciprocal),
             c7x::reinterpret<c7x::uchar_vec>(inVecVals_evenReciprocal)));

         // Clear bits 0-16 inclusive
         inVecReciprocalClr_32_63 = inVecReciprocal_32_63 & 0xFFFE0000;

         // Concatenate cleared bit reciprocal with zero bits
         inVecReciprocalApprox_8_15 = c7x::reinterpret<c7x::double_vec>(__permute_high_high(
             MATHLIB_vperm_data_interweave_0_63, c7x::reinterpret<c7x::uchar_vec>(inVecReciprocalClr_32_63),
             c7x::reinterpret<c7x::uchar_vec>(zero)));
         inVecReciprocalApprox_0_7  = c7x::reinterpret<c7x::double_vec>(__permute_low_low(
             MATHLIB_vperm_data_interweave_0_63, c7x::reinterpret<c7x::uchar_vec>(inVecReciprocalClr_32_63),
             c7x::reinterpret<c7x::uchar_vec>(zero)));

         // Split inVec into two vectors with double precision
         inVecVals_0_7 = c7x::reinterpret<c7x::double_vec>(
             __permute_low_low(MATHLIB_vperm_data_dp_interweave_0_63, c7x::reinterpret<c7x::uchar_vec>(inVecVals_odd),
                               c7x::reinterpret<c7x::uchar_vec>(inVecVals_even)));
         inVecVals_8_15 = c7x::reinterpret<c7x::double_vec>(
             __permute_high_high(MATHLIB_vperm_data_dp_interweave_0_63, c7x::reinterpret<c7x::uchar_vec>(inVecVals_odd),
                                 c7x::reinterpret<c7x::uchar_vec>(inVecVals_even)));

         // Calculate r in double precision
         rVals_0_7  = (inVecReciprocalApprox_0_7 * inVecVals_0_7) - 1.0;
         rVals_8_15 = (inVecReciprocalApprox_8_15 * inVecVals_8_15) - 1.0;

         // Convert r to float, compute r to the power of 2, 3, 4
         r1 = c7x::reinterpret<vec>(__permute_even_even_int(
             MATHLIB_vperm_data_0_63, c7x::reinterpret<c7x::uchar_vec>(__double_to_float(rVals_8_15)),
             c7x::reinterpret<c7x::uchar_vec>(__double_to_float(rVals_0_7))));
         r2 = r1 * r1;
         r3 = r1 * r2;
         r4 = r2 * r2;

         // Compute Taylor series polynomial
         pol = (C5 * r2) + ((C4 * r3) + ((((C2 * r1) + C3) + (C1 * r2)) * r4));

         /**********************************************************************/
         /* Calculate N                                                        */
         /**********************************************************************/

         // Upper 32 bits of all inVec reciprocal approximation values
         inVecReciprocalApprox_32_63 = c7x::reinterpret<c7x::uint_vec>(__permute_odd_odd_int(
             MATHLIB_vperm_data_0_63, c7x::reinterpret<c7x::uchar_vec>(inVecReciprocalApprox_8_15),
             c7x::reinterpret<c7x::uchar_vec>(inVecReciprocalApprox_0_7)));

         N = c7x::convert<c7x::int_vec>(((inVecReciprocalApprox_32_63 << 1) >> 21) - 1023);

         // Covert N to double precision for later calculation with LUT values
         NVals_odd  = __high_int_to_double(N);
         NVals_even = __low_int_to_double(N);
         NVals_0_7  = c7x::reinterpret<c7x::double_vec>(__permute_low_low(MATHLIB_vperm_data_dp_interweave_0_63,
                                                                          c7x::reinterpret<c7x::uchar_vec>(NVals_odd),
                                                                          c7x::reinterpret<c7x::uchar_vec>(NVals_even)));
         NVals_8_15 = c7x::reinterpret<c7x::double_vec>(
             __permute_high_high(MATHLIB_vperm_data_dp_interweave_0_63, c7x::reinterpret<c7x::uchar_vec>(NVals_odd),
                                 c7x::reinterpret<c7x::uchar_vec>(NVals_even)));

         /**********************************************************************/
         /* Determine LUT values                                               */
         /**********************************************************************/

         // Calculate LUT index
         indexT = (((inVecReciprocalApprox_32_63 << 12) >> 29) + MATHLIB_LOGTABLE_OFFSET);

         // Read from LUT and reconstruct double values split into two vectors
         upperBitsIndexT = MATHLIB_LUTReadUpperBits(indexT);
         lowerBitsIndexT = MATHLIB_LUTReadLowerBits(indexT);

         TVals_8_15 = c7x::reinterpret<c7x::double_vec>(
             __permute_high_high(MATHLIB_vperm_data_interweave_0_63, c7x::reinterpret<c7x::uchar_vec>(upperBitsIndexT),
                                 c7x::reinterpret<c7x::uchar_vec>(lowerBitsIndexT)));
         TVals_0_7 = c7x::reinterpret<c7x::double_vec>(
             __permute_low_low(MATHLIB_vperm_data_interweave_0_63, c7x::reinterpret<c7x::uchar_vec>(upperBitsIndexT),
                               c7x::reinterpret<c7x::uchar_vec>(lowerBitsIndexT)));

         // Calculate an adjusted T
         TVals_8_15 = TVals_8_15 - (ln2 * NVals_8_15);
         TVals_0_7  = TVals_0_7 - (ln2 * NVals_0_7);

         /**********************************************************************/
         /* Calculate output with adjusted LUT and Taylor series values        */
         /**********************************************************************/

         // Split polynomial result into two vectors with double precision
         pol_0_7  = c7x::reinterpret<c7x::double_vec>(__permute_low_low(
             MATHLIB_vperm_data_dp_interweave_0_63, c7x::reinterpret<c7x::uchar_vec>(__high_float_to_double(pol)),
             c7x::reinterpret<c7x::uchar_vec>(__low_float_to_double(pol))));
         pol_8_15 = c7x::reinterpret<c7x::double_vec>(__permute_high_high(
             MATHLIB_vperm_data_dp_interweave_0_63, c7x::reinterpret<c7x::uchar_vec>(__high_float_to_double(pol)),
             c7x::reinterpret<c7x::uchar_vec>(__low_float_to_double(pol))));

         // Add LUT values and Taylor series values
         outVec_0_7  = rVals_0_7 + TVals_0_7 + pol_0_7;
         outVec_8_15 = rVals_8_15 + TVals_8_15 + pol_8_15;

         // Combine output vector into one floating point result
         outVec = c7x::reinterpret<vec>(__permute_even_even_int(
             MATHLIB_vperm_data_0_63, c7x::reinterpret<c7x::uchar_vec>(__double_to_float(outVec_8_15)),
             c7x::reinterpret<c7x::uchar_vec>(__double_to_float(outVec_0_7))));

         /**********************************************************************/
         /* Bounds checking                                                    */
         /**********************************************************************/

         //  if (inVec > eMax) {
         //     outVec = 88.72f;
         //  }
         __vpred cmp_max = __cmp_lt_pred(eMax, inVec);
         outVec          = __select(cmp_max, outVecMax, outVec);

         outVec = outVec + ln2sp;

         vec inVecOriginal = c7x::strm_eng<1, vec>::get_adv();

         __vpred cmp_bound = __cmp_lt_pred(inVecOriginal, one);
         outVec            = __select(cmp_bound, nan, outVec);

         // Store exp computation result in
         __vpred tmp  = c7x::strm_agen<0, vec>::get_vpred();
         vec    *addr = c7x::strm_agen<0, vec>::get_adv(pDst);
         __vstore_pred(tmp, addr, outVec);
      }
      MATHLIB_SE0SE1SA0Close();
   }
   return status;
}

// this method performs acosh computation of input vector
template <typename T> static inline void MATHLIB_acosh_vector(size_t length, T *pSrc, T *pDst)
{
   // variables
   size_t numBlocks    = 0; // compute loop's iteration count
   size_t remNumBlocks = 0; // when numBlocks is not a multiple of SIMD width

   // derive c7x vector type from template typename
   typedef typename c7x::make_full_vector<float>::type vec;

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

   /***********************************************************************/
   /* Create and assign values for constants employed on acosh computation */
   /***********************************************************************/

   vec half, one;

   half = (vec) 0.5f;
   one  = (vec) 1.0f;

   // compute loop to perform vector acosh
   for (size_t i = 0; i < numBlocks; i++) {
      vec inVec = c7x::strm_eng<0, vec>::get_adv();

      /**********************************************************************/
      /* Create variables employed on acosh computation                     */
      /**********************************************************************/

      vec sqrt, temp, inVecSquare;

      inVecSquare = inVec * inVec;

      sqrt = sqrt_acosh_i((inVecSquare - one), inVec);
      temp = (sqrt * half) + (inVec * half);

      __vpred tmp  = c7x::strm_agen<0, vec>::get_vpred();
      vec    *addr = c7x::strm_agen<0, vec>::get_adv(pDst);
      __vstore_pred(tmp, addr, temp);
   }
   MATHLIB_SE0SA0Close();

   MATHLIB_acosh_log(&se0Params, &sa0Params, length, pSrc, pDst);
}

// this method performs exponential computation of input vector
template <typename T> MATHLIB_STATUS MATHLIB_acosh(size_t length, T *pSrc, T *pDst)
{
   MATHLIB_STATUS status = MATHLIB_SUCCESS; // return function status

   // check for null pointers and non-zero length
   status = MATHLIB_checkParams(length, pSrc, pDst);

   if (status == MATHLIB_SUCCESS) {
      MATHLIB_acosh_vector(length, pSrc, pDst);
   }
   return status;
}

/******************************************************************************/
/*                                                                            */
/* Explicit templatization for datatypes supported by MATHLIB_acosh            */
/*                                                                            */
/******************************************************************************/

// single precision
template MATHLIB_STATUS MATHLIB_acosh<float>(size_t length, float *pSrc, float *pDst);

/******************************************************************************/
/*                                                                            */
/* C-interface wrapper functions                                              */
/*                                                                            */
/******************************************************************************/

extern "C" {

// single-precision wrapper
MATHLIB_STATUS MATHLIB_acosh_sp(size_t length, float *pSrc, float *pDst)
{
   MATHLIB_STATUS status = MATHLIB_acosh(length, pSrc, pDst);
   return status;
}

} // extern "C"
