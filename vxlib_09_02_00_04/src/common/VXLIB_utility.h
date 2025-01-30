#ifndef COMMON_VXLIB_UTILITY_H_
#define COMMON_VXLIB_UTILITY_H_ 1

/******************************************************************************/
/*!
 * \file
 */
/* Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com/
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

/*******************************************************************************
 *
 * INCLUDES
 *
 ******************************************************************************/

#include <float.h>  // for max float, double values
#include <limits.h> // for min, max integer values
#include <math.h>

#include "VXLIB_bufParams.h"
#include "VXLIB_types.h"

#include "c71/VXLIB_utility.h"
#if VXLIB_DEBUGPRINT >= 1
#include "c71/VXLIB_debug.h"
#endif

#include "c7120/VXLIB_utility.h"

/*******************************************************************************
 *
 * EXTERNAL VARIABLES
 *
 ******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif                     /* __cplusplus */
extern uint64_t beg_count; /* Begin cycle count for profiling */
extern uint64_t end_count; /* End cycle count for profiling */
extern uint64_t overhead;  /* Cycle profiling overhead */
#ifdef __cplusplus
}
#endif /* __cplusplus */

/*******************************************************************************
 *
 * Inline functions
 *
 ******************************************************************************/

/*******************************************************************************
 *
 * Arithmetic with 128-bit signed integers
 *
 ******************************************************************************/
static inline void VXLIB_UTIL_mult(int64_t *ph, int64_t *pl, int64_t a, int64_t b);

static inline void VXLIB_UTIL_mult(int64_t *ph, // result
                                   int64_t *pl,
                                   int64_t  a, // left operand
                                   int64_t  b  // right operand
)
{
   // sum += A[k + m*K] * B[n + k*N];
   *pl = a * b;

   // sign extend the product
   *ph = ((((uint64_t) *pl) & 0x8000000000000000ULL) != 0ULL) ? (int64_t) 0xffffffffffffffffULL : (int64_t) 0ULL;
}

#ifdef __cplusplus

/*******************************************************************************
 *
 * Definition and arithmetic for VXLIB_int128_t class
 *
 ******************************************************************************/

// Define a 128-bit integer class to allow natural-c implementations of VXLIB
// 32-bit input/output functions to be templated.  The class is implemented in
// a header file for easy sharing.  All member functions, including constructors
// are declared inline for two reasons: (1) performance and (2) necessary for
// implementing the class in a multiple-inclusion header file.

class VXLIB_int128_t {
 public:
   int64_t hi;
   int64_t lo;
   VXLIB_int128_t(int64_t h,
                  int64_t l);                              // constructor for both high and low specified
   VXLIB_int128_t(int64_t l);                              // constructor for just low specified (sign extends to high)
   VXLIB_int128_t();                                       // constructor for neither field specified
   VXLIB_int128_t operator+(const VXLIB_int128_t &) const; // operator +
   VXLIB_int128_t operator>>(const int8_t &) const;        // operator >>
};

// define constructor
inline VXLIB_int128_t::VXLIB_int128_t(int64_t h, int64_t l)
{
   hi = h;
   lo = l;
}

// define constructor
inline VXLIB_int128_t::VXLIB_int128_t(int64_t l)
{
   // sign extend l
   hi = (((uint64_t) l & 0x8000000000000000ULL) != 0LL) ? (int64_t) 0xffffffffffffffffULL
                                                        : (int64_t) 0x0000000000000000ULL;
   lo = l;
}

// define constructor
inline VXLIB_int128_t::VXLIB_int128_t()
{
   hi = 0x0000000000000000LL;
   lo = 0x0000000000000000LL;
}

static inline void VXLIB_UTIL_shiftRight128(uint64_t *rh, // result
                                            uint64_t *rl,
                                            uint64_t  ah, // operand
                                            uint64_t  al,
                                            int32_t   sh, // shift amount
                                            int32_t   s)    // signed
{
   uint64_t h;
   uint64_t l;
   int32_t  i;

   h = ah;
   l = al;
   for (i = 0; i < sh; i++) {
      l = (uint64_t)(__shift_right(l, (uint32_t) 1)) | (uint64_t)(__shift_left(h, (uint32_t) 63));
      uint64_t orMaskVal = ((s != 0) ? (h & 0x8000000000000000ULL) : 0ULL); 
      h = (uint64_t)(__shift_right(h, (uint32_t) 1)) | orMaskVal;
   }

   *rh = h;
   *rl = l;
}

static inline void VXLIB_UTIL_Add128(uint64_t *rh, // result
                                     uint64_t *rl,
                                     uint64_t  ah, // left operand
                                     uint64_t  al,
                                     uint64_t  bh, // right operand
                                     uint64_t  bl)
{
   // break up the operands into 4 32b chunks packed into 64b uints
   uint64_t all;
   uint64_t alh;
   uint64_t ahl;
   uint64_t ahh;
   uint64_t bll;
   uint64_t blh;
   uint64_t bhl;
   uint64_t bhh;
   uint64_t s0;
   uint64_t s1;
   uint64_t s2;
   uint64_t s3;
   uint64_t sh;
   uint64_t sl;
   uint64_t maskingValue = 0x0ffffffffULL;
   all = (uint64_t)(__shift_right(al, (uint32_t) 0)) & maskingValue;
   alh = (uint64_t)(__shift_right(al, (uint32_t) 32)) & maskingValue;
   ahl = (uint64_t)(__shift_right(ah, (uint32_t) 0)) & maskingValue;
   ahh = (uint64_t)(__shift_right(ah, (uint32_t) 32)) & maskingValue;

   bll = (uint64_t)(__shift_right(bl, (uint32_t) 0)) & maskingValue;
   blh = (uint64_t)(__shift_right(bl, (uint32_t) 32)) & maskingValue;
   bhl = (uint64_t)(__shift_right(bh, (uint32_t) 0)) & maskingValue;
   bhh = (uint64_t)(__shift_right(bh, (uint32_t) 32)) & maskingValue;

   // the adds
   s0 = all + bll;
   s1 = alh + blh + __shift_right(s0, (uint32_t) 32);
   s2 = ahl + bhl + __shift_right(s1, (uint32_t) 32);
   s3 = ahh + bhh + __shift_right(s2, (uint32_t) 32);

   // pack the results
   sl = (uint64_t)(__shift_left(s1, (uint32_t) 32)) | (uint64_t)(s0 & maskingValue);
   sh = (uint64_t)(__shift_left(s3, (uint32_t) 32)) | (uint64_t)(s2 & maskingValue);

   *rl = sl;
   *rh = sh;
}

// define overloaded + (plus) operator
inline VXLIB_int128_t VXLIB_int128_t::operator+(const VXLIB_int128_t &b) const
{
   VXLIB_int128_t result;

   VXLIB_UTIL_Add128((uint64_t *) &(result.hi), (uint64_t *) &(result.lo), this->hi, this->lo, b.hi, b.lo);

   return result;
}

// define overloaded >> (bit shift right) operator
inline VXLIB_int128_t VXLIB_int128_t::operator>>(const int8_t &shift) const
{
   VXLIB_int128_t result;

   VXLIB_UTIL_shiftRight128((uint64_t *) &result.hi, (uint64_t *) &result.lo, this->hi, this->lo, (int32_t) shift, 1);
   return result;
}

/*******************************************************************************
 *
 * We need special utility to do negation because range of values is from
 * -2^(bit-width-1) to 2^(bit-width-1)-1. For example, with int16_t, the
 * range is from -32768 to 32767 - that is, -0x8000 to 0x7FFF. Now, if we want
 * to evaluate negation of -32768 and we try simply -(-32768) and store the
 * result in int16_t, we would get -32768 itself. Instead, we want to get 32767.
 *
 ******************************************************************************/

static inline int16_t VXLIB_UTIL_negate(int16_t a)
{
   int16_t result;

   result = (a == -32768) ? 32767 : -a;
   return result;
}

static inline int32_t VXLIB_UTIL_negate(int32_t a)
{
   int32_t result;

   result = (a == -2147483648) ? 2147483647 : -a;
   return result;
}

/*******************************************************************************
 *
 * Inline multiply with higher bit-width output type
 *
 ******************************************************************************/

static inline int32_t VXLIB_UTIL_mult(int8_t a, int8_t b) { return (int16_t) a * (int16_t) b; }

static inline int32_t VXLIB_UTIL_mult(uint8_t a, int8_t b) { return (int16_t) a * (int16_t) b; }

static inline int64_t VXLIB_UTIL_mult(int16_t a, int16_t b) { return (int32_t) a * (int32_t) b; }

static inline int64_t VXLIB_UTIL_mult(uint16_t a, int16_t b) { return (int32_t) a * (int32_t) b; }

static inline VXLIB_int128_t VXLIB_UTIL_mult(int32_t a, int32_t b)
{
   VXLIB_int128_t result(0, 0);

   result.lo = (int64_t) a * (int64_t) b;
   // sign extend the product
   result.hi =
       (((uint64_t) result.lo & 0x8000000000000000ULL) != 0LL) ? (int64_t) 0xffffffffffffffffULL : (int64_t) 0ULL;

   return result;
}

/*******************************************************************************
 *
 * Inline saturate with ReLU operation
 *
 ******************************************************************************/

static inline void VXLIB_UTIL_saturate_relu(int32_t x, int8_t *y)
{
   if (x > 0x7F) {
      *y = 0x7F;
   }
   else if (x < 0) {
      *y = 0;
   }
   else {
      *y = (int8_t) x;
   }

   return;
}

static inline void VXLIB_UTIL_saturate_relu(int32_t x, uint8_t *y)
{
   if (x > 0xFF) {
      *y = 0xFF;
   }
   else if (x < 0) {
      *y = 0;
   }
   else {
      *y = (uint8_t) x;
   }

   return;
}

static inline void VXLIB_UTIL_saturate_relu(uint32_t x, uint8_t *y)
{
   if (x > 0xFF) {
      *y = 0xFF;
   }
   else {
      *y = (uint8_t) x;
   }

   return;
}

static inline void VXLIB_UTIL_saturate_relu(int64_t x, int16_t *y)
{
   if (x > 0x7FFF) {
      *y = 0x7FFF;
   }
   else if (x < 0x0000) {
      *y = 0x0000;
   }
   else {
      *y = (int16_t) x;
   }

   return;
}

static inline void VXLIB_UTIL_saturate_relu(int64_t x, uint16_t *y)
{
   if (x > 0xFFFF) {
      *y = 0xFFFF;
   }
   else if (x < 0x0000) {
      *y = 0x0000;
   }
   else {
      *y = (uint16_t) x;
   }

   return;
}

static inline void VXLIB_UTIL_saturate_relu(uint64_t x, uint16_t *y)
{
   if (x > 0xFFFF) {
      *y = 0xFFFF;
   }
   else {
      *y = (uint16_t) x;
   }

   return;
}

/*******************************************************************************
 *
 * Inline shift, round and ReLU operation
 *
 ******************************************************************************/

template <typename dataType, typename returnType>
static inline returnType VXLIB_UTIL_shiftRoundAndReLU(dataType inVal, uint8_t shift)
{
   returnType result;

   if (shift == 0) {
      // remove the rounding, which doesn't make sense with no shift but causes C
      // code problems
      VXLIB_UTIL_saturate_relu(inVal, &result);
   }
   else {
      // round and shift
      // Method requires right shift of signed integers be an arithmetic shift,
      // but right shift >> on signed integer types is implementation dependent on
      // whether the shift is arithmetic or logical.  There's no simple way in C
      // to specify the shift type as arithmetic. Instead, we use the
      // __shift_right intrinsic, which is defined to be arithmetic shift.
      dataType temp;
      temp = __shift_right(inVal, (shift - 1)) + 1;
      temp = __shift_right(temp, 1);
      VXLIB_UTIL_saturate_relu(temp, &result);
   }

   return result;
}

template int8_t  VXLIB_UTIL_shiftRoundAndReLU<int32_t, int8_t>(int32_t inVal, uint8_t shift);
template int16_t VXLIB_UTIL_shiftRoundAndReLU<int64_t, int16_t>(int64_t inVal, uint8_t shift);

// template  uint8_t  VXLIB_UTIL_shiftRoundAndReLU<uint32_t,  uint8_t>
// (uint32_t inVal, uint8_t shift); template  uint16_t
// VXLIB_UTIL_shiftRoundAndReLU<uint64_t,  uint16_t> (uint64_t inVal, uint8_t
// shift);

template <> inline uint8_t VXLIB_UTIL_shiftRoundAndReLU<int32_t, uint8_t>(int32_t inVal, uint8_t shift)
{
   uint8_t result;

   if (shift == 0) {
      // remove the rounding, which doesn't make sense with no shift but causes C
      // code problems
      VXLIB_UTIL_saturate_relu(inVal, &result);
   }
   else {
      // round and shift
      // Method requires right shift of signed integers be an arithmetic shift,
      // but right shift >> on signed integer types is implementation dependent on
      // whether the shift is arithmetic or logical.  There's no simple way in C
      // to specify the shift type as arithmetic. Instead, we use the
      // __shift_right intrinsic, which is defined to be arithmetic shift.
      int32_t temp;
      temp = __shift_right(inVal, (shift - 1)) + 1;
      temp = __shift_right(temp, 1);
      VXLIB_UTIL_saturate_relu(temp, &result);
   }

   return result;
}

template <> inline uint8_t VXLIB_UTIL_shiftRoundAndReLU<uint32_t, uint8_t>(uint32_t inVal, uint8_t shift)
{
   uint8_t result;

   if (shift == 0) {
      // remove the rounding, which doesn't make sense with no shift but causes C
      // code problems
      VXLIB_UTIL_saturate_relu(inVal, &result);
   }
   else {
      uint32_t temp;
      // Subtracting two unsigned values of the same size will result in an
      // unsigned value. If the first operand is less than the second the result
      // will be arithmetically in correct. But if the size of the unsigned types
      // is less than that of an unsigned int, C/C++ will promote the types to
      // signed int before subtracting resulting in an correct result. In either
      // case, there is no indication of an error.
      uint32_t shift32_t = (uint32_t) shift;
      temp               = (inVal >> (shift32_t - (uint32_t) 1)) + 1;
      temp               = temp >> 1;
      VXLIB_UTIL_saturate_relu(temp, &result);
   }

   return result;
}

template <> inline uint16_t VXLIB_UTIL_shiftRoundAndReLU<int64_t, uint16_t>(int64_t inVal, uint8_t shift)
{
   uint16_t result;

   if (shift == 0) {
      // remove the rounding, which doesn't make sense with no shift but causes C
      // code problems
      VXLIB_UTIL_saturate_relu(inVal, &result);
   }
   else {
      // round and shift
      // Method requires right shift of signed integers be an arithmetic shift,
      // but right shift >> on signed integer types is implementation dependent on
      // whether the shift is arithmetic or logical.  There's no simple way in C
      // to specify the shift type as arithmetic. Instead, we use the
      // __shift_right intrinsic, which is defined to be arithmetic shift.
      int64_t temp;
      temp = __shift_right(inVal, (shift - 1)) + 1;
      temp = __shift_right(temp, 1);
      VXLIB_UTIL_saturate_relu(temp, &result);
   }

   return result;
}

template <> inline uint16_t VXLIB_UTIL_shiftRoundAndReLU<uint64_t, uint16_t>(uint64_t inVal, uint8_t shift)
{
   uint16_t result;

   if (shift == 0) {
      // remove the rounding, which doesn't make sense with no shift but causes C
      // code problems
      VXLIB_UTIL_saturate_relu(inVal, &result);
   }
   else {
      uint64_t temp;
      uint32_t shift32_t = (uint32_t) shift;
      temp               = (inVal >> (shift32_t - (uint32_t) 1)) + 1;
      temp               = (temp >> 1);
      VXLIB_UTIL_saturate_relu(temp, &result);
   }

   return result;
}

/*******************************************************************************
 *
 * Inline saturate operation
 *
 ******************************************************************************/

static inline int8_t VXLIB_UTIL_saturate(int32_t x)
{
   int8_t retVal;
   if (x > 0x7F) {
      retVal = 0x7F;
   }
   else if (x < -0x80) {
      retVal = -0x80;
   }
   else {
      retVal = (int8_t) x;
   }
   return retVal;
}

static inline uint8_t VXLIB_UTIL_saturate(uint32_t x)
{
   uint8_t retVal;
   if (x > 0xFF) {
      retVal = 0xFF;
   }
   else {
      retVal = (uint8_t) x;
   }
   return retVal;
}

static inline int16_t VXLIB_UTIL_saturate(int64_t x)
{
   int16_t retVal;
   if (x > 0x7FFF) {
      retVal = 0x7FFF;
   }
   else if (x < -0x8000) {
      retVal = -0x8000;
   }
   else {
      retVal = (int16_t) x;
   }
   return retVal;
}

static inline uint16_t VXLIB_UTIL_saturate(uint64_t x)
{
   uint16_t retVal;
   if (x > 0xFFFF) {
      retVal = 0xFFFF;
   }
   else {
      retVal = (uint16_t) x;
   }
   return retVal;
}

static inline int32_t VXLIB_UTIL_saturate(int64_t xh, int64_t xl)
{
   int32_t retVal;
   // printf("%s: xh = %" PRIx64 ", xl = %" PRIx64 "\n", __FUNCTION__, xh, xl);
   //  if negative
   if (((uint64_t) xh & 0x8000000000000000ULL) != 0LL) {
      if (((~(uint64_t) xh & 0xFFFFFFFFFFFFFFFFULL) != 0LL) || ((~(uint64_t) xl & 0xFFFFFFFF80000000ULL) != 0LL)) {
         retVal = ((int32_t) 0x80000000U);
      }
      else {
         retVal = (int32_t) xl;
      }
   }
   else if (((uint64_t) xl & 0xFFFFFFFF80000000ULL) != 0LL) {
      //(xl > 0x000000007FFFFFFFLL){ // positive and saturated
      retVal = ((int32_t) 0x7FFFFFFFU);
   }
   else {
      retVal = (int32_t) xl;
   }
   return retVal;
}

static inline int32_t VXLIB_UTIL_saturate(VXLIB_int128_t x) { return VXLIB_UTIL_saturate(x.hi, x.lo); }

/*******************************************************************************
 *
 * Inline shift and round operation
 *
 ******************************************************************************/

template <typename dataType, typename returnType>
inline returnType VXLIB_UTIL_shiftAndRound(dataType inVal, uint8_t shift)
{
   returnType result;

   if (shift == 0) {
      // remove the rounding, which doesn't make sense with no shift but causes C
      // code problems
      result = VXLIB_UTIL_saturate(inVal);
   }
   else {
      // round and shift
      dataType temp;
      temp   = (__shift_right(inVal, (shift - 1)) + 1);
      temp   = __shift_right(temp, 1);
      result = VXLIB_UTIL_saturate(temp);
   }

   return result;
}

template int8_t  VXLIB_UTIL_shiftAndRound<int32_t, int8_t>(int32_t inVal, uint8_t shift);
template int16_t VXLIB_UTIL_shiftAndRound<int64_t, int16_t>(int64_t inVal, uint8_t shift);

template <> inline uint8_t VXLIB_UTIL_shiftAndRound(uint32_t inVal, uint8_t shift)
{
   uint8_t result;

   if (shift == 0) {
      // remove the rounding, which doesn't make sense with no shift but causes C
      // code problems
      result = VXLIB_UTIL_saturate(inVal);
   }
   else {
      // round and shift
      uint32_t temp;
      uint32_t shift32_t = (uint32_t) shift;
      temp               = (inVal >> (shift32_t - (uint32_t) 1)) + 1;
      temp               = (temp >> 1);
      result             = VXLIB_UTIL_saturate(temp);
   }

   return result;
}

template <> inline uint16_t VXLIB_UTIL_shiftAndRound(uint64_t inVal, uint8_t shift)
{
   uint16_t result;

   if (shift == 0) {
      // remove the rounding, which doesn't make sense with no shift but causes C
      // code problems
      result = VXLIB_UTIL_saturate(inVal);
   }
   else {
      // round and shift
      uint64_t temp;
      uint32_t shift32_t = (uint32_t) shift;
      temp               = (inVal >> (shift32_t - (uint32_t) 1)) + 1;
      temp               = (temp >> 1);
      result             = VXLIB_UTIL_saturate(temp);
   }

   return result;
}

// MISRA-C prohibits using >> on signed integers because it is implementation
// dependent on whether that shift is arithmetic or logical.  However, for
// VXLIB_int128_t, this code implements the shift in software and ensures that
// it is arithmetic.  To avoid the MISRA-C violation, we use the function
// version of the shift rather than the >> operator.
template <> inline int32_t VXLIB_UTIL_shiftAndRound<VXLIB_int128_t, int32_t>(VXLIB_int128_t inVal, uint8_t shift)
{
   int32_t result;

   if (shift == 0) {
      // remove the rounding, which doesn't make sense with no shift but causes C
      // code problems
      result = VXLIB_UTIL_saturate(inVal);
   }
   else {
      // round and shift
      // result = VXLIB_UTIL_saturate(((inVal >> ((uint8_t)(shift - 1))) + 1) >>
      // 1);
      VXLIB_int128_t temp;
      // temp = inVal >> (shift - 1)
      VXLIB_UTIL_shiftRight128((uint64_t *) &temp.hi, (uint64_t *) &temp.lo, inVal.hi, inVal.lo, (int32_t) (shift - 1),
                               1);
      temp = temp + 1;
      // temp = temp >> 1
      VXLIB_UTIL_shiftRight128((uint64_t *) &temp.hi, (uint64_t *) &temp.lo, temp.hi, temp.lo, 1, 1);
      result = VXLIB_UTIL_saturate(temp);
   }

   return result;
}

/*******************************************************************************
 *
 * Convert a double-precision floating point number to 16-bit integer
 *
 ******************************************************************************/

template <typename returnType> static inline returnType VXLIB_UTIL_typeConv_i64f_oxX(VXLIB_D64 x)
{
   int64_t    xLocal, maxValue;
   returnType returnValue;

   /* Set maxValue to the maximumum possible value for the returnType        */

   // original code
   //  maxValue  = (1 << (sizeof(returnType)*8-2)) - 1;
   //  maxValue += (1 << (sizeof(returnType)*8-2));
   maxValue = ((int64_t) ((uint32_t) 1 << ((uint32_t) (sizeof(returnType) * 8 - 2)))) - 1;
   maxValue += (int64_t) ((uint32_t) 1 << ((uint32_t) (sizeof(returnType) * 8 - 2)));

   xLocal = (int64_t) floor(0.5 + x); /* Explicit rounding to integer */
   if (xLocal >= maxValue) {
      returnValue = (returnType) maxValue;
   }
   else if (xLocal <= -maxValue - 1) {
      returnValue = (returnType) (-maxValue - 1);
   }
   else {
      returnValue = (returnType) xLocal;
   }
   return returnValue;
}

template int16_t VXLIB_UTIL_typeConv_i64f_oxX<int16_t>(VXLIB_D64 x);
template int32_t VXLIB_UTIL_typeConv_i64f_oxX<int32_t>(VXLIB_D64 x);

/*******************************************************************************
 *
 * Evaluate cos function, and apply appropriate scale factor
 *
 ******************************************************************************/
template <typename returnType> static returnType VXLIB_UTIL_cos_i64f_oxX(VXLIB_D64 x, VXLIB_D64 scaleFactor)
{
   return VXLIB_UTIL_typeConv_i64f_oxX<returnType>(scaleFactor * cos(x));
}

template int16_t VXLIB_UTIL_cos_i64f_oxX<int16_t>(VXLIB_D64 x, VXLIB_D64 scaleFactor);
template int32_t VXLIB_UTIL_cos_i64f_oxX<int32_t>(VXLIB_D64 x, VXLIB_D64 scaleFactor);
/*******************************************************************************
 *
 * Evaluate sin function, and apply appropriate scale factor
 *
 ******************************************************************************/
template <typename returnType> static inline returnType VXLIB_UTIL_sin_i64f_oxX(VXLIB_D64 x, VXLIB_D64 scaleFactor)
{
   return VXLIB_UTIL_typeConv_i64f_oxX<returnType>(scaleFactor * sin(x));
}

template int16_t VXLIB_UTIL_sin_i64f_oxX<int16_t>(VXLIB_D64 x, VXLIB_D64 scaleFactor);
template int32_t VXLIB_UTIL_sin_i64f_oxX<int32_t>(VXLIB_D64 x, VXLIB_D64 scaleFactor);

#endif

#endif
