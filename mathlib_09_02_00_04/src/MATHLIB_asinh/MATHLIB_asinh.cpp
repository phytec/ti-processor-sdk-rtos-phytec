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

#include "c7x_scalable.h"
#define ELEMENT_COUNT(x) c7x::element_count_of<x>::value
/* the type of data that 'type x data structure' (i.e. vecType) holds */
#define ELEMENT_TYPE(x) typename c7x::element_type_of<x>::type

/******************************************************************************/
/*                                                                            */
/* Includes                                                                   */
/*                                                                            */
/******************************************************************************/

#include "MATHLIB_asinh_scalar.h"
#include "MATHLIB_permute.h"
#include "MATHLIB_types.h"
#include "MATHLIB_utility.h"
#include <cstddef>
#include <float.h>
#include "MATHLIB_lut.h"

/******************************************************************************/
/*                                                                            */
/* MATHLIB_asinh                                                              */
/*                                                                            */
/******************************************************************************/

// this method performs iterative scalar asinh computation

// this method computes the logarithm expressions in asinh_log sub-kernel
template <typename vecType> static inline vecType log_asinh_i(vecType inVec)
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

// this method computes square root expressions in the asinh_sqrt sub-kernel
template <typename vecType> static inline vecType sqrt_asinh_i(vecType a, vecType x)
{

   /* // define type of elements vecType vector holds as elemType */
   /* typedef ELEMENT_TYPE(vecType) elemType; */

   vecType half, OneP5;
   /* vec zero, maxValue; */

   half  = (vecType) 0.5;
   OneP5 = (vecType) 1.5;
   /* zero  = (vecType) 0.0; */
   /* maxValue = (vecType) std::numeric_limits<elemType>::max(); */

   vecType p0, p1, r0, d0, y;

   p0 = __recip_sqrt(a);
   r0 = p0;
   d0 = p0 * a;

   p1 = OneP5 - d0 * p0 * half;
   y  = a * r0 * p1;

   vecType x2 = x * x;

   // if input a is x^2, select x as output
   __vpred cmp_xsqr = __cmp_eq_pred(a, x2);

   y = __select(cmp_xsqr, x, y);

   return y;
}

// this method is 1 of 3 sub-kernels within asinh, computes sqrt expressions
template <typename T>
static inline void
MATHLIB_asinh_sqrt(size_t length, T *pSrc, T *pDst, __SE_TEMPLATE_v1 *se0Params, __SA_TEMPLATE_v1 *sa0Params)
{
   // variables
   size_t numBlocks    = 0; // compute loop's iteration count
   size_t remNumBlocks = 0; // when numBlocks is not a multiple of SIMD width

   // derive c7x vector type from template typename
   typedef typename c7x::make_full_vector<T>::type vec;

   numBlocks    = length / c7x::element_count_of<vec>::value;
   remNumBlocks = length % c7x::element_count_of<vec>::value;

   if (remNumBlocks) {
      numBlocks++;
   }

   // open SE0, SE1, and SA0 for reading and writing operands
   MATHLIB_SE0SA0Open(se0Params, sa0Params, pSrc);

   vec one  = (vec) 1.0;
   vec half = (vec) 0.5;

   vec x_abs, x2, sqrt_;

   for (size_t i = 0; i < numBlocks; i++) {

      vec inVec_sqrt = c7x::strm_eng<0, vec>::get_adv();

      x2    = inVec_sqrt * inVec_sqrt;
      x_abs = __abs(inVec_sqrt);

      sqrt_ = sqrt_asinh_i(x2 + one, x_abs); // sqrt(x^2 + 1)

      /* prevent overflow for large x, log(2x) where x > max/2 */
      vec outVec_sqrt = (sqrt_ * half) + (x_abs * half); /* (x+sqrt(x^2 + 1))/2 */

      __vpred tmp  = c7x::strm_agen<0, vec>::get_vpred();
      vec    *addr = c7x::strm_agen<0, vec>::get_adv(pDst);
      __vstore_pred(tmp, addr, outVec_sqrt);
   }

   MATHLIB_SE0SA0Close();
}

// this method is 1 of 3 sub-kernels in asinh, computes log
template <typename T>
static inline void
MATHLIB_asinh_log(size_t length, T *pSrc, T *pDst, __SE_TEMPLATE_v1 *se0Params, __SA_TEMPLATE_v1 *sa0Params)
{
   // variables
   size_t numBlocks    = 0; // compute loop's iteration count
   size_t remNumBlocks = 0; // when numBlocks is not a multiple of SIMD width

   // derive c7x vector type from template typename
   typedef typename c7x::make_full_vector<T>::type vec;

   numBlocks    = length / c7x::element_count_of<vec>::value;
   remNumBlocks = length % c7x::element_count_of<vec>::value;

   if (remNumBlocks) {
      numBlocks++;
   }

   MATHLIB_SE0SA0Open(se0Params, sa0Params, pDst);

   vec ln2 = (vec) 0.69314718056;

   for (size_t i = 0; i < numBlocks; i++) {

      vec inVec_log = c7x::strm_eng<0, vec>::get_adv();

      vec outVec_log = log_asinh_i(inVec_log) + ln2;

      __vpred tmp  = c7x::strm_agen<0, vec>::get_vpred();
      vec    *addr = c7x::strm_agen<0, vec>::get_adv(pDst);
      __vstore_pred(tmp, addr, outVec_log);
   }

   MATHLIB_SE0SA0Close();
}

// this method is 1 of 3 sub-kernels in asinh, computes polynomial estimation
template <typename T>
static inline void
MATHLIB_asinh_polyEst(size_t length, T *pSrc, T *pDst, __SE_TEMPLATE_v1 *se0Params, __SA_TEMPLATE_v1 *sa0Params)
{
   // variables
   size_t numBlocks    = 0; // compute loop's iteration count
   size_t remNumBlocks = 0; // when numBlocks is not a multiple of SIMD width

   // derive c7x vector type from template typename
   typedef typename c7x::make_full_vector<T>::type vec;

   numBlocks    = length / c7x::element_count_of<vec>::value;
   remNumBlocks = length % c7x::element_count_of<vec>::value;

   if (remNumBlocks) {
      numBlocks++;
   }

   // open se0, se1, sa0
   MATHLIB_SE0SE1SA0Open(se0Params, sa0Params, pDst, pSrc);

   vec c2, c4, c6;
   vec pol_bound;

   vec zero = (vec) 0.0;

   c2 = (vec) -0.166605362341955;
   c4 = (vec) 0.0734464812833510;
   c6 = (vec) -0.0330279320352987;

   pol_bound = (vec) 0.5;

   for (size_t i = 0; i < numBlocks; i++) {
      vec inVec_log = c7x::strm_eng<0, vec>::get_adv();
      vec inVec     = c7x::strm_eng<1, vec>::get_adv();

      vec sign = (vec) 1.0;

      __vpred cmp_sign = __cmp_lt_pred(inVec, zero);
      sign             = __select(cmp_sign, -sign, sign);

      // polynomial estimation
      vec x2 = inVec * inVec;
      vec x4 = x2 * x2;
      vec x6 = x4 * x2;

      vec pol = (x2 * c2) + (x4 * c4) + (x6 * c6);
      pol     = (pol * inVec) + inVec;

      // give correct sign to log computation
      vec res = inVec_log * sign;

      vec x_abs = __abs(inVec);

      __vpred cmp_bound = __cmp_le_pred(x_abs, pol_bound);
      res               = __select(cmp_bound, pol, res);

      vec outVec = res;

      __vpred tmp  = c7x::strm_agen<0, vec>::get_vpred();
      vec    *addr = c7x::strm_agen<0, vec>::get_adv(pDst);
      __vstore_pred(tmp, addr, outVec);
   }

   MATHLIB_SE0SE1SA0Close();
}

// this method incorporates 3 sub-kernels into vector asinh computation
template <typename T> static inline void MATHLIB_asinh_vector(size_t length, T *pSrc, T *pDst)
{

   // variables
   size_t numBlocks    = 0; // compute loop's iteration count
   size_t remNumBlocks = 0; // when numBlocks is not a multiple of SIMD width

   // derive c7x vector type from template typename
   typedef typename c7x::make_full_vector<T>::type vec;

   numBlocks    = length / c7x::element_count_of<vec>::value;
   remNumBlocks = length % c7x::element_count_of<vec>::value;

   if (remNumBlocks) {
      numBlocks++;
   }

   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa0Params = __gen_SA_TEMPLATE_v1();

   MATHLIB_SE0SA01DSequentialInit(&se0Params, &sa0Params, length, pSrc, pDst);

   // sqrt setup
   MATHLIB_asinh_sqrt(length, pSrc, pDst, &se0Params, &sa0Params);

   // log setup
   MATHLIB_asinh_log(length, pSrc, pDst, &se0Params, &sa0Params);

   // polynomial estimation and bound check setup
   MATHLIB_asinh_polyEst(length, pSrc, pDst, &se0Params, &sa0Params);
}

// this method is top level module of asinh, deploys scalar or vector version
template <typename T> MATHLIB_STATUS MATHLIB_asinh(size_t length, T *pSrc, T *pDst)
{
   MATHLIB_STATUS status = MATHLIB_SUCCESS; // return function status

   status = MATHLIB_checkParams(length, pSrc, pDst);

   if (status == MATHLIB_SUCCESS) {
      MATHLIB_asinh_vector(length, pSrc, pDst);
   }

   return status;
}
/******************************************************************************/
/*                                                                            */
/* Explicit templatization for datatypes supported by MATHLIB_asinh           */
/*                                                                            */
/******************************************************************************/

// single precision
template MATHLIB_STATUS MATHLIB_asinh<float>(size_t length, float *pSrc, float *pDst);

/******************************************************************************/
/*                                                                            */
/* C-interface wrapper functions                                              */
/*                                                                            */
/******************************************************************************/

extern "C" {

// single-precision wrapper
MATHLIB_STATUS MATHLIB_asinh_sp(size_t length, float *pSrc, float *pDst)
{
   MATHLIB_STATUS status = MATHLIB_asinh(length, pSrc, pDst);
   return status;
}

} // extern "C"
