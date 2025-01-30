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

/******************************************************************************/
/*                                                                            */
/* MATHLIB_cos                                                                 */
/*                                                                            */
/******************************************************************************/

// this method performs cosine coputation of input vector
template <typename T> MATHLIB_STATUS MATHLIB_cos(size_t length, T *pSrc, T *pDst)
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

      /**********************************************************************/
      /* Create and assign values for constants employed on cos computation */
      /**********************************************************************/

      vec InvPI, HalfPI, One, MAX, Zero, s1, s2, s3, s4, C1, C2;

      InvPI  = (vec) 0.318309886183791;
      HalfPI = (vec) 1.5707963268;
      One    = (vec) 1.0;
      MAX    = (vec) 1048576.0;

      Zero = (vec) 0.0;
      s1   = (vec) -1.666665668e-1;
      s2   = (vec) 8.333025139e-3;
      s3   = (vec) -1.980741872e-4;
      s4   = (vec) 2.601903036e-6;
      C1   = (vec) 3.140625;
      C2   = (vec) 9.67653589793e-4;

      // compute loop to perform vector cos
      for (size_t i = 0; i < numBlocks; i++) {
         vec inVec = c7x::strm_eng<0, vec>::get_adv();

         /**********************************************************************/
         /* Create and assign values for variables employed on cos computation */
         /**********************************************************************/

         vec Sign, X, Y, Z, F, G, R;

         vec_type int_one = (vec_type) 1;
         vec_type N;

         vec negativeOne = (vec) -1;
         vec negativeR;
         Sign = One;

         //      if (Y > MAX) {
         //        Y = HalfPI;
         //      }
         Y              = __abs(inVec) + HalfPI;
         __vpred cmp_gt = __cmp_lt_pred((vec) MAX, Y);
         Y              = __select(cmp_gt, HalfPI, Y);

         // X = Y * (1/PI)
         X = Y * InvPI;

         N = __float_to_int(X);
         Z = c7x::convert<vec>(N);

         /**********************************************************************/
         /* Sign checking for quadrant 3 or 4                                  */
         /**********************************************************************/

         //      if ((N % 2) != 0) {
         //         Sign = -Sign;
         //      }
         vec_type andN    = N & int_one;
         vec convert_andN = c7x::convert<vec>(andN);
         __vpred  cmp_mod = __cmp_le_pred(convert_andN, Zero);
         vec      Sign_T  = __select(cmp_mod, Sign, negativeOne);

         F = (Y - (Z * C1)) - (Z * C2);
         R = F;

         //      if (F < Zero) {
         //        R = -R;
         //      }
         negativeR     = -R;
         __vpred cmp_F = __cmp_lt_pred(F, Zero);
         R             = __select(cmp_F, R, negativeR);

         G = F * F;

         __vpred cmp_RMin = __cmp_lt_pred(R, Zero);

         vec outputRMin = R * Sign_T;
         R              = ((((((s4 * G) + s3) * G) + s2) * G) + s1) * G;
         vec outVec     = ((F + (F * R)) * Sign_T);

         outVec = __select(cmp_RMin, outVec, outputRMin);

         // outVec.print();

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
/* Explicit templatization for datatypes supported by MATHLIB_cos              */
/*                                                                            */
/******************************************************************************/

// single precision
template MATHLIB_STATUS MATHLIB_cos<float>(size_t length, float *pSrc, float *pDst);

/******************************************************************************/
/*                                                                            */
/* C-interface wrapper functions                                              */
/*                                                                            */
/******************************************************************************/

extern "C" {

// single-precision wrapper
MATHLIB_STATUS MATHLIB_cos_sp(size_t length, float *pSrc, float *pDst)
{
   MATHLIB_STATUS status = MATHLIB_cos(length, pSrc, pDst);
   return status;
}

} // extern "C"
