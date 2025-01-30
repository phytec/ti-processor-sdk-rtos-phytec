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

#ifndef C7120_FFTLIB_UTILITY_H_
#define C7120_FFTLIB_UTILITY_H_ 1

/*******************************************************************************
 *
 * INCLUDES
 *
 ******************************************************************************/
#include "../FFTLIB_types.h"



/*******************************************************************************
 *
 * EXTERNAL VARIABLES
 *
 ******************************************************************************/
#ifdef __cplusplus
extern int64_t FFTLIB_CN_dynamicRangeMin;
extern int64_t FFTLIB_CN_dynamicRangeMax;
#endif // #ifdef __cplusplus
/*******************************************************************************
 *
 * Inline functions
 *
 ******************************************************************************/

/**************************************************************************************
 * debug print dynamic range update
 **************************************************************************************/

#ifdef __cplusplus
static inline void FFTLIB_CN_printUpdate(int32_t value, const char *string){
   FFTLIB_DEBUGPRINTFN(2, "(%s) int32_t value is %d.  min is %ld, max is %ld\n", string, value, FFTLIB_CN_dynamicRangeMin, FFTLIB_CN_dynamicRangeMax);
}

static inline void FFTLIB_CN_printUpdate(int64_t value, const char *string){
   FFTLIB_DEBUGPRINTFN(2, "(%s) int64_t value is %ld.  min is %ld, max is %ld\n", string, value, FFTLIB_CN_dynamicRangeMin, FFTLIB_CN_dynamicRangeMax);
}

static inline void FFTLIB_CN_printUpdate(uint32_t value, const char *string){
   FFTLIB_DEBUGPRINTFN(2, "(%s) uint32_t value is %#010x.  min is %#018lx, max is %#018lx\n", string, value, (uint64_t)FFTLIB_CN_dynamicRangeMin, (uint64_t)FFTLIB_CN_dynamicRangeMax);
}

static inline void FFTLIB_CN_printUpdate(uint64_t value, const char *string){
   FFTLIB_DEBUGPRINTFN(2, "(%s) uint64_t value is %#018lx.  min is %#018lx, max is %#018lx\n", string, value, (uint64_t)FFTLIB_CN_dynamicRangeMin, (uint64_t)FFTLIB_CN_dynamicRangeMax);
}
#endif // __cplusplus


/**************************************************************************************
 * update dynamic range
 **************************************************************************************/
#ifdef __cplusplus
template <typename accumulateType>
static inline void FFTLIB_CN_updateDynamicRange(FFTLIB_enableDynamicRange_type enableDynamicRange, accumulateType value)
{
   FFTLIB_CN_printUpdate(value, "before dynamic range update");
   if( enableDynamicRange == FFTLIB_DYNAMIC_RANGE_DISABLE ){
      // do nothing
   } else {
      if (((int64_t) value) > FFTLIB_CN_dynamicRangeMax) {
         FFTLIB_CN_dynamicRangeMax = (int64_t) value;
      }
      if (((int64_t) value) < FFTLIB_CN_dynamicRangeMin) {
         FFTLIB_CN_dynamicRangeMin = (int64_t) value;
      }
   }

   FFTLIB_CN_printUpdate(value, "after dynamic range update ");
   return;
}

template void FFTLIB_CN_updateDynamicRange(FFTLIB_enableDynamicRange_type enableDynamicRange, int32_t value);
template void FFTLIB_CN_updateDynamicRange(FFTLIB_enableDynamicRange_type enableDynamicRange, int64_t value);
template void FFTLIB_CN_updateDynamicRange(FFTLIB_enableDynamicRange_type enableDynamicRange, uint32_t value);

// when accumulator uses an unsigned 64-bit data type
template <>
inline void FFTLIB_CN_updateDynamicRange(FFTLIB_enableDynamicRange_type enableDynamicRange, uint64_t value)
{
   FFTLIB_CN_printUpdate(value, "before dynamic range update");
   if( enableDynamicRange == FFTLIB_DYNAMIC_RANGE_DISABLE ){
      // do nothing
   } else {
      int64_t signValue = value & 0x8000000000000000;
      int64_t magValue  = value & 0x7FFFFFFFFFFFFFFF;

      // logic for the max
      int64_t signMax   = FFTLIB_CN_dynamicRangeMax & 0x8000000000000000;
      int64_t magMax    = FFTLIB_CN_dynamicRangeMax & 0x7FFFFFFFFFFFFFFF;
      if(signValue == signMax){
         // a regular comparison on the magnitudes should work
         if (magValue > magMax) {
            //FFTLIB_CN_dynamicRangeMax = (int64_t) value;
            FFTLIB_CN_dynamicRangeMax = signValue | magValue;
         }
      }  else if (signValue < signMax) {
         // value is positive (leading zero) and max is negative
         //FFTLIB_CN_dynamicRangeMax = (int64_t) value;
         FFTLIB_CN_dynamicRangeMax = signValue | magValue;
      } else {
         // max is positive and value is negative, so don't change the max
      }

      // logic for the min
      int64_t signMin   = FFTLIB_CN_dynamicRangeMin & 0x8000000000000000;
      int64_t magMin    = FFTLIB_CN_dynamicRangeMin & 0x7FFFFFFFFFFFFFFF;
      if(signValue == signMin){
         // a regular comparison on the magnitudes should work
         if (magValue < magMin) {
            FFTLIB_CN_dynamicRangeMin = signValue | magValue;
         }
      }  else if (signValue > signMin) {
         // value is negative (leading one) and min is positive
         FFTLIB_CN_dynamicRangeMin = signValue | magValue;
      } else {
         // value is positive and min is negative, so don't change the max
      }

   }

   FFTLIB_CN_printUpdate(value, "after dynamic range update ");

   return;
}
#endif //#ifdef __cplusplus

/*******************************************************************************
 *
 * Inline saturate with PSAT operation
 *
 ******************************************************************************/
#ifdef __cplusplus

static inline void FFTLIB_UTIL_psat(int32_t x, int8_t minVal, int8_t maxVal, int8_t *y)
{
   if (x < minVal) {
      *y = minVal;
   } else if (x > maxVal) {
      *y = maxVal;
   } else {
      *y = (int8_t)x;
   }

   return;
}

static inline void FFTLIB_UTIL_psat(int32_t x, uint8_t minVal, uint8_t maxVal, uint8_t *y)
{
   if (x < minVal) {
      *y = minVal;
   } else if (x > maxVal) {
      *y = maxVal;
   } else {
      *y = (uint8_t)x;
   }

   return;
}

static inline void FFTLIB_UTIL_psat(uint32_t x, uint8_t minVal, uint8_t maxVal, uint8_t *y)
{
   if (x < minVal) {
      *y = minVal;
   } else if (x > maxVal) {
      *y = maxVal;
   } else {
      *y = (uint8_t)x;
   }

   return;
}

static inline void FFTLIB_UTIL_psat(int64_t x, int16_t minVal, int16_t maxVal, int16_t *y)
{
   if (x < minVal) {
      *y = minVal;
   } else if (x > maxVal) {
      *y = maxVal;
   } else {
      *y = (int16_t)x;
   }

   return;
}

static inline void FFTLIB_UTIL_psat(int64_t x, uint16_t minVal, uint16_t maxVal, uint16_t *y)
{
   if (x < minVal) {
      *y = minVal;
   } else if (x > maxVal) {
      *y = maxVal;
   } else {
      *y = (uint16_t)x;
   }

   return;
}

static inline void FFTLIB_UTIL_psat(uint64_t x, uint16_t minVal, uint16_t maxVal, uint16_t *y)
{
   if (x < minVal) {
      *y = minVal;
   } else if (x > maxVal) {
      *y = maxVal;
   } else {
      *y = (uint16_t)x;
   }

   return;
}


/*******************************************************************************
 *
 * Inline shift, round and PSAT operation
 *
 ******************************************************************************/

template <typename dataType, typename returnType>
static inline returnType FFTLIB_UTIL_shiftRoundAndPSAT(dataType inVal, uint8_t shift, returnType minVal, returnType maxVal){
   returnType result;

   if(shift == 0){
      // remove the rounding, which doesn't make sense with no shift but causes C code problems
      FFTLIB_UTIL_psat(inVal, minVal, maxVal, &result);
   } else {
      // round and shift
      // Method requires right shift of signed integers be an arithmetic shift, but right
      // shift >> on signed integer types is implementation dependent on whether the shift is
      // arithmetic or logical.  There's no simple way in C to specify the shift type as arithmetic.
      // Instead, we use the __shift_right intrinsic, which is defined to be arithmetic shift.
      dataType temp;
      temp = __shift_right(inVal, (shift - 1)) + 1;
      temp = __shift_right(temp, 1);
      FFTLIB_UTIL_psat(temp, minVal, maxVal, &result);
   }

   return result;
}

template  int8_t  FFTLIB_UTIL_shiftRoundAndPSAT<int32_t,  int8_t>  (int32_t inVal, uint8_t shift, int8_t minVal, int8_t maxVal);
template  int16_t FFTLIB_UTIL_shiftRoundAndPSAT<int64_t,  int16_t> (int64_t inVal, uint8_t shift, int16_t minVal, int16_t maxVal);

template <>
inline uint8_t FFTLIB_UTIL_shiftRoundAndPSAT<int32_t, uint8_t>(int32_t inVal, uint8_t shift, uint8_t minVal, uint8_t maxVal){
   uint8_t result;

   if(shift == 0){
      // remove the rounding, which doesn't make sense with no shift but causes C code problems
      FFTLIB_UTIL_psat(inVal, minVal, maxVal, &result);
   } else {
      // round and shift
      // Method requires right shift of signed integers be an arithmetic shift, but right
      // shift >> on signed integer types is implementation dependent on whether the shift is
      // arithmetic or logical.  There's no simple way in C to specify the shift type as arithmetic.
      // Instead, we use the __shift_right intrinsic, which is defined to be arithmetic shift.
      int32_t temp;
      temp = __shift_right( inVal, (shift - 1) ) + 1;
      temp = __shift_right(temp, 1);
      FFTLIB_UTIL_psat(temp, minVal, maxVal, &result);

   }

   return result;
}

template <>
inline uint8_t FFTLIB_UTIL_shiftRoundAndPSAT<uint32_t, uint8_t>(uint32_t inVal, uint8_t shift, uint8_t minVal, uint8_t maxVal){
   uint8_t result;

   if(shift == 0){
      // remove the rounding, which doesn't make sense with no shift but causes C code problems
      FFTLIB_UTIL_psat(inVal, minVal, maxVal, &result);
   } else {
      uint32_t temp;
      //Subtracting two unsigned values of the same size will result in an unsigned value.
      //If the first operand is less than the second the result will be arithmetically in correct.
      //But if the size of the unsigned types is less than that of an unsigned int, C/C++ will promote the types to
      //signed int before subtracting resulting in an correct result. In either case,
      //there is no indication of an error.
      uint32_t shift32_t = (uint32_t) shift;
      temp = (inVal >> (shift32_t - (uint32_t)1) ) + 1;
      temp = temp >> 1;
      FFTLIB_UTIL_psat(temp, minVal, maxVal, &result);
   }

   return result;
}

template <>
inline uint16_t FFTLIB_UTIL_shiftRoundAndPSAT<int64_t, uint16_t>(int64_t inVal, uint8_t shift, uint16_t minVal, uint16_t maxVal){
   uint16_t result;

   if(shift == 0){
      // remove the rounding, which doesn't make sense with no shift but causes C code problems
      FFTLIB_UTIL_psat(inVal, minVal, maxVal, &result);
   } else {
      // round and shift
      // Method requires right shift of signed integers be an arithmetic shift, but right
      // shift >> on signed integer types is implementation dependent on whether the shift is
      // arithmetic or logical.  There's no simple way in C to specify the shift type as arithmetic.
      // Instead, we use the __shift_right intrinsic, which is defined to be arithmetic shift.
      int64_t temp;
      temp = __shift_right( inVal, (shift - 1) ) + 1;
      temp = __shift_right(temp, 1);
      FFTLIB_UTIL_psat(temp, minVal, maxVal, &result);
   }

   return result;
}

template <>
inline uint16_t FFTLIB_UTIL_shiftRoundAndPSAT<uint64_t, uint16_t>(uint64_t inVal, uint8_t shift, uint16_t minVal, uint16_t maxVal){
   uint16_t result;

   if(shift == 0){
      // remove the rounding, which doesn't make sense with no shift but causes C code problems
      FFTLIB_UTIL_psat(inVal, minVal, maxVal, &result);
   } else {
      uint64_t temp;
      uint32_t shift32_t = (uint32_t) shift;
      temp = (inVal >> (shift32_t - (uint32_t)1) ) + 1;
      temp = (temp >> 1);
      FFTLIB_UTIL_psat(temp, minVal, maxVal, &result);
   }

   return result;
}

/*******************************************************************************
 *
 * Inline scale, shift, saturate with PSAT operation
 *
 ******************************************************************************/


static inline void FFTLIB_UTIL_scaleShiftPsat(uint8_t scale, uint8_t shift, int32_t x, int8_t minVal, int8_t maxVal, int8_t *y)
{
   int64_t xVal = x;
   int64_t scaledValue = xVal*scale;

   if (shift > 0) {
      xVal = __shift_right(scaledValue, (shift - 1)) + 1;
      xVal = __shift_right(xVal, 1);
   } else {
      xVal = scaledValue;
   }

   if (xVal < minVal) {
      *y = minVal;
   } else if (xVal > maxVal) {
      *y = maxVal;
   } else {
      *y = (int8_t)xVal;
   }

   return;
}

static inline void FFTLIB_UTIL_scaleShiftPsat(uint8_t scale, uint8_t shift, int32_t x, uint8_t minVal, uint8_t maxVal, uint8_t *y)
{
    int64_t xVal = x;
    int64_t scaledValue = xVal*scale;

    if (shift > 0) {
       xVal = __shift_right(scaledValue, (shift - 1)) + 1;
       xVal = __shift_right(xVal, 1);
    } else {
       xVal = scaledValue;
    }

    if (xVal < minVal) {
      *y = minVal;
   } else if (xVal > maxVal) {
      *y = maxVal;
   } else {
      *y = (uint8_t)xVal;
   }

   return;
}

static inline void FFTLIB_UTIL_scaleShiftPsat(uint8_t scale, uint8_t shift, uint32_t x, uint8_t minVal, uint8_t maxVal, uint8_t *y)
{
   uint64_t xVal = x;
   uint64_t scaledValue = xVal*scale;

   uint32_t shift32_t = (uint32_t) shift;

   if (shift32_t > 0) {
      xVal = (scaledValue >> (shift32_t - (uint32_t)1) ) + 1;
      xVal = (xVal >> 1);
   } else {
      xVal = scaledValue;
   }

   if (xVal < minVal) {
      *y = minVal;
   } else if (xVal > maxVal) {
      *y = maxVal;
   } else {
      *y = (uint8_t)xVal;
   }

   return;
}

static inline void FFTLIB_UTIL_scaleShiftPsat(uint8_t scale, uint8_t shift, int64_t x, int16_t minVal, int16_t maxVal, int16_t *y)
{
    int64_t xVal = x;
    int64_t scaledValue = xVal*scale;

    if (shift > 0) {
       xVal = __shift_right(scaledValue, (shift - 1)) + 1;
       xVal = __shift_right(xVal, 1);
    } else {
        xVal = scaledValue;
    }

    if (xVal < minVal) {
      *y = minVal;
   } else if (xVal > maxVal) {
      *y = maxVal;
   } else {
      *y = (int16_t)xVal;
   }

   return;
}

static inline void FFTLIB_UTIL_scaleShiftPsat(uint8_t scale, uint8_t shift, int64_t x, uint16_t minVal, uint16_t maxVal, uint16_t *y)
{
   int64_t xVal = x;
   int64_t scaledValue = xVal*scale;

   if (shift > 0) {
      xVal = __shift_right(scaledValue, (shift - 1)) + 1;
      xVal = __shift_right(xVal, 1);
   } else {
       xVal = scaledValue;
   }

   if (xVal < minVal) {
      *y = minVal;
   } else if (xVal > maxVal) {
      *y = maxVal;
   } else {
      *y = (uint16_t)xVal;
   }

   return;
}

static inline void FFTLIB_UTIL_scaleShiftPsat(uint8_t scale, uint8_t shift, uint64_t x, uint16_t minVal, uint16_t maxVal, uint16_t *y)
{
   uint64_t xVal = x;
   uint64_t scaledValue = xVal*scale;

   uint32_t shift32_t = (uint32_t) shift;

   if (shift32_t > 0) {
      xVal = (scaledValue >> (shift32_t - (uint32_t)1) ) + 1;
      xVal = (xVal >> 1);
   } else {
      xVal = scaledValue;
   }

   if (xVal < minVal) {
      *y = minVal;
   } else if (xVal > maxVal) {
      *y = maxVal;
   } else {
      *y = (uint16_t)xVal;
   }

   return;
}

// scale int8_t
static inline void FFTLIB_UTIL_scaleShiftPsat(int8_t scale, uint8_t shift, int32_t x, int8_t minVal, int8_t maxVal, int8_t *y)
{
   int64_t xVal = x;
   int64_t scaledValue = xVal*scale;

   if (shift > 0) {
      xVal = __shift_right(scaledValue, (shift - 1)) + 1;
      xVal = __shift_right(xVal, 1);
   } else {
      xVal = scaledValue;
   }

   if (xVal < minVal) {
      *y = minVal;
   } else if (xVal > maxVal) {
      *y = maxVal;
   } else {
      *y = (int8_t)xVal;
   }

   return;
}

static inline void FFTLIB_UTIL_scaleShiftPsat(int8_t scale, uint8_t shift, int32_t x, uint8_t minVal, uint8_t maxVal, uint8_t *y)
{
    int64_t xVal = x;
    int64_t scaledValue = xVal*scale;

    if (shift > 0) {
       xVal = __shift_right(scaledValue, (shift - 1)) + 1;
       xVal = __shift_right(xVal, 1);
    } else {
       xVal = scaledValue;
    }

    if (xVal < minVal) {
      *y = minVal;
   } else if (xVal > maxVal) {
      *y = maxVal;
   } else {
      *y = (uint8_t)xVal;
   }

   return;
}

static inline void FFTLIB_UTIL_scaleShiftPsat(int8_t scale, uint8_t shift, uint32_t x, uint8_t minVal, uint8_t maxVal, uint8_t *y)
{
   int64_t xVal = x;
   int64_t scaledValue = xVal*scale;

//   uint32_t shift32_t = (uint32_t) shift;
//
//   if (shift32_t > 0) {
//      xVal = (scaledValue >> (shift32_t - (uint32_t)1) ) + 1;
//      xVal = (xVal >> 1);
//   } else {
//      xVal = scaledValue;
//   }
   int32_t tmp;
   if (shift > 0) {
      tmp = __shift_right(scaledValue, (shift - 1)) + 1;
      tmp = __shift_right(tmp, 1);
   } else {
      tmp = scaledValue;
   }

   if (tmp < minVal) {
      *y = minVal;
   } else if (tmp > maxVal) {
      *y = maxVal;
   } else {
      *y = (uint8_t)tmp;
   }

   return;
}

static inline void FFTLIB_UTIL_scaleShiftPsat(int8_t scale, uint8_t shift, int64_t x, int16_t minVal, int16_t maxVal, int16_t *y)
{
    int64_t xVal = x;
    int64_t scaledValue = xVal*scale;

    if (shift > 0) {
       xVal = __shift_right(scaledValue, (shift - 1)) + 1;
       xVal = __shift_right(xVal, 1);
    } else {
        xVal = scaledValue;
    }

    if (xVal < minVal) {
      *y = minVal;
   } else if (xVal > maxVal) {
      *y = maxVal;
   } else {
      *y = (int16_t)xVal;
   }

   return;
}

static inline void FFTLIB_UTIL_scaleShiftPsat(int8_t scale, uint8_t shift, int64_t x, uint16_t minVal, uint16_t maxVal, uint16_t *y)
{
   int64_t xVal = x;
   int64_t scaledValue = xVal*scale;

   if (shift > 0) {
      xVal = __shift_right(scaledValue, (shift - 1)) + 1;
      xVal = __shift_right(xVal, 1);
   } else {
       xVal = scaledValue;
   }

   if (xVal < minVal) {
      *y = minVal;
   } else if (xVal > maxVal) {
      *y = maxVal;
   } else {
      *y = (uint16_t)xVal;
   }

   return;
}

static inline void FFTLIB_UTIL_scaleShiftPsat(int8_t scale, uint8_t shift, uint64_t x, uint16_t minVal, uint16_t maxVal, uint16_t *y)
{
   int64_t xVal = x;
   int64_t scaledValue = xVal*scale;

//   uint32_t shift32_t = (uint32_t) shift;
//
//   if (shift32_t > 0) {
//      xVal = (scaledValue >> (shift32_t - (uint32_t)1) ) + 1;
//      xVal = (xVal >> 1);
//   } else {
//      xVal = scaledValue;
//   }

   int32_t tmp;
   if (shift > 0) {
       tmp = __shift_right(scaledValue, (shift - 1)) + 1;
       tmp = __shift_right(tmp, 1);
   } else {
       tmp = scaledValue;
   }

   if (tmp < minVal) {
      *y = minVal;
   } else if (tmp > maxVal) {
      *y = maxVal;
   } else {
      *y = (uint16_t)tmp;
   }

   return;
}



/*******************************************************************************
 *
 * Inline scale, shift, round and PSAT operation
 *
 ******************************************************************************/

template <typename dataType, typename dataTypeS, typename returnType>
static inline returnType FFTLIB_UTIL_scaleShiftRoundAndPSAT(dataType inVal, dataTypeS scale, uint8_t shift, returnType minVal, returnType maxVal){
   returnType result;

   // round and shift
   // Method requires right shift of signed integers be an arithmetic shift, but right
   // shift >> on signed integer types is implementation dependent on whether the shift is
   // arithmetic or logical.  There's no simple way in C to specify the shift type as arithmetic.
   // Instead, we use the __shift_right intrinsic, which is defined to be arithmetic shift.

   FFTLIB_UTIL_scaleShiftPsat(scale, shift, inVal, minVal, maxVal, &result);

   return result;
}

template  int8_t  FFTLIB_UTIL_scaleShiftRoundAndPSAT<int32_t,  uint8_t, int8_t>  (int32_t inVal, uint8_t scale, uint8_t shift, int8_t minVal, int8_t maxVal);
template  int16_t FFTLIB_UTIL_scaleShiftRoundAndPSAT<int64_t,  uint8_t, int16_t> (int64_t inVal, uint8_t scale, uint8_t shift, int16_t minVal, int16_t maxVal);
template  int8_t  FFTLIB_UTIL_scaleShiftRoundAndPSAT<int32_t,  int8_t, int8_t>  (int32_t inVal, int8_t scale, uint8_t shift, int8_t minVal, int8_t maxVal);
template  int16_t FFTLIB_UTIL_scaleShiftRoundAndPSAT<int64_t,  int8_t, int16_t> (int64_t inVal, int8_t scale, uint8_t shift, int16_t minVal, int16_t maxVal);

/*******************************************************************************
 *
 * Inline scale, shift saturate with ReLU operation
 *
 ******************************************************************************/

static inline void FFTLIB_UTIL_scale_shift_saturate_relu(int32_t x, uint8_t scale, uint8_t shift, int8_t *y)
{
  int64_t xVal = x;
  int64_t scaledValue = xVal*scale;

//#if FFTLIB_DEBUGPRINT
//             printf("FFTLIB_DEBUGPRINT FFTLIB_UTIL_scale_shift_saturate_relu: scaledValue %ll xVal %d scale %d shift %d\n", scaledValue, xVal,scale, shift);
//#endif
  if(shift > 0) {
     xVal = __shift_right(scaledValue, (shift - 1)) + 1;
     xVal = __shift_right(xVal, 1);
  } else {
     xVal = scaledValue;
  }

  if (xVal > 0x7F) {
    *y = 0x7F;
  } else if (xVal < 0) {
    *y = 0;
  } else {
    *y = (int8_t)xVal;
  }

  return;
}

static inline void FFTLIB_UTIL_scale_shift_saturate_relu(int32_t x, uint8_t scale, uint8_t shift, uint8_t *y)
{
  int64_t xVal = x;
  int64_t scaledValue = xVal*scale;
//#if FFTLIB_DEBUGPRINT
//            printf("scaledValue  %" PRId64 "\n", scaledValue);
//            printf("FFTLIB_DEBUGPRINT FFTLIB_UTIL_scale_shift_saturate_relu: xVal %d scale %d shift %d\n", xVal,scale, shift);
//#endif
  if(shift > 0) {
     xVal = __shift_right(scaledValue, (shift - 1)) + 1;
     xVal = __shift_right(xVal, 1);
  } else {
     xVal = scaledValue;
  }

  if (xVal > 0xFF) {
    *y = 0xFF;
  } else if (xVal < 0) {
    *y = 0;
  } else {
    *y = (uint8_t)xVal;
  }

  return;
}

static inline void FFTLIB_UTIL_scale_shift_saturate_relu(uint32_t x, uint8_t scale, uint8_t shift, uint8_t *y)
{
    uint64_t xVal = x;
    uint64_t scaledValue = xVal*scale;

    uint32_t shift32_t = (uint32_t) shift;

    if (shift32_t > 0) {
       xVal = (scaledValue >> (shift32_t - (uint32_t)1) ) + 1;
       xVal = (xVal >> 1);
    } else {
       xVal = scaledValue;
    }

    if (xVal > 0xFF) {
      *y = 0xFF;
   } else {
      *y = (uint8_t)xVal;
   }

   return;
}

static inline void FFTLIB_UTIL_scale_shift_saturate_relu(int64_t x, uint8_t scale, uint8_t shift, int16_t *y)
{
   int64_t xVal = x;
   int64_t scaledValue = xVal*scale;

   if(shift > 0) {
      xVal = __shift_right(scaledValue, (shift - 1)) + 1;
      xVal = __shift_right(xVal, 1);
   } else {
      xVal = scaledValue;
   }


  if (xVal > 0x7FFF) {
    *y = 0x7FFF;
  } else if (xVal < 0x0000) {
    *y = 0x0000;
  } else {
    *y = (int16_t)xVal;
  }

  return;
}

static inline void FFTLIB_UTIL_scale_shift_saturate_relu(int64_t x, uint8_t scale, uint8_t shift, uint16_t *y)
{
   int64_t xVal = x;
   int64_t scaledValue = xVal*scale;

   if(shift > 0) {
      xVal = __shift_right(scaledValue, (shift - 1)) + 1;
      xVal = __shift_right(xVal, 1);
   } else {
      xVal = scaledValue;
   }


  if (xVal > 0xFFFF) {
    *y = 0xFFFF;
  } else if (xVal < 0x0000) {
    *y = 0x0000;
  } else {
    *y = (uint16_t)xVal;
  }

  return;
}

static inline void FFTLIB_UTIL_scale_shift_saturate_relu(uint64_t x, uint8_t scale, uint8_t shift, uint16_t *y)
{
   uint64_t xVal = x;
   uint64_t scaledValue = xVal*scale;

   uint32_t shift32_t = (uint32_t) shift;

   if (shift32_t > 0) {
      xVal = (scaledValue >> (shift32_t - (uint32_t)1) ) + 1;
      xVal = (xVal >> 1);
   } else {
      xVal = scaledValue;
   }

   if (xVal > 0xFFFF) {
      *y = 0xFFFF;
   } else {
      *y = (uint16_t)xVal;
   }

   return;
}

//int8_t scale
static inline void FFTLIB_UTIL_scale_shift_saturate_relu(int32_t x, int8_t scale, uint8_t shift, int8_t *y)
{
  int64_t xVal = x;
  int64_t scaledValue = xVal*scale;

//#if FFTLIB_DEBUGPRINT
//             printf("FFTLIB_DEBUGPRINT FFTLIB_UTIL_scale_shift_saturate_relu: scaledValue %ll xVal %d scale %d shift %d\n", scaledValue, xVal,scale, shift);
//#endif
  if(shift > 0) {
     xVal = __shift_right(scaledValue, (shift - 1)) + 1;
     xVal = __shift_right(xVal, 1);
  } else {
     xVal = scaledValue;
  }

  if (xVal > 0x7F) {
    *y = 0x7F;
  } else if (xVal < 0) {
    *y = 0;
  } else {
    *y = (int8_t)xVal;
  }

  return;
}

static inline void FFTLIB_UTIL_scale_shift_saturate_relu(int32_t x, int8_t scale, uint8_t shift, uint8_t *y)
{
  int64_t xVal = x;
  int64_t scaledValue = xVal*scale;
//#if FFTLIB_DEBUGPRINT
//            printf("scaledValue  %" PRId64 "\n", scaledValue);
//            printf("FFTLIB_DEBUGPRINT FFTLIB_UTIL_scale_shift_saturate_relu: xVal %d scale %d shift %d\n", xVal,scale, shift);
//#endif
  if(shift > 0) {
     xVal = __shift_right(scaledValue, (shift - 1)) + 1;
     xVal = __shift_right(xVal, 1);
  } else {
     xVal = scaledValue;
  }

  if (xVal > 0xFF) {
    *y = 0xFF;
  } else if (xVal < 0) {
    *y = 0;
  } else {
    *y = (uint8_t)xVal;
  }

  return;
}

static inline void FFTLIB_UTIL_scale_shift_saturate_relu(uint32_t x, int8_t scale, uint8_t shift, uint8_t *y)
{
    int64_t xVal = x;
    int64_t scaledValue = xVal*scale;

//    uint32_t shift32_t = (uint32_t) shift;
//
//    if (shift32_t > 0) {
//       xVal = (scaledValue >> (shift32_t - (uint32_t)1) ) + 1;
//       xVal = (xVal >> 1);
//    } else {
//       xVal = scaledValue;
//    }
    int32_t tmp;
    if(shift > 0) {
       tmp = __shift_right(scaledValue, (shift - 1)) + 1;
       tmp = __shift_right(tmp, 1);
    } else {
       tmp = scaledValue;
    }

    if (tmp > 0xFF) {
      *y = 0xFF;
   } else {
      *y = (uint8_t)tmp;
   }

   return;
}

static inline void FFTLIB_UTIL_scale_shift_saturate_relu(int64_t x, int8_t scale, uint8_t shift, int16_t *y)
{
   int64_t xVal = x;
   int64_t scaledValue = xVal*scale;

   if(shift > 0) {
      xVal = __shift_right(scaledValue, (shift - 1)) + 1;
      xVal = __shift_right(xVal, 1);
   } else {
      xVal = scaledValue;
   }


  if (xVal > 0x7FFF) {
    *y = 0x7FFF;
  } else if (xVal < 0x0000) {
    *y = 0x0000;
  } else {
    *y = (int16_t)xVal;
  }

  return;
}

static inline void FFTLIB_UTIL_scale_shift_saturate_relu(int64_t x, int8_t scale, uint8_t shift, uint16_t *y)
{
   int64_t xVal = x;
   int64_t scaledValue = xVal*scale;

   if(shift > 0) {
      xVal = __shift_right(scaledValue, (shift - 1)) + 1;
      xVal = __shift_right(xVal, 1);
   } else {
      xVal = scaledValue;
   }


  if (xVal > 0xFFFF) {
    *y = 0xFFFF;
  } else if (xVal < 0x0000) {
    *y = 0x0000;
  } else {
    *y = (uint16_t)xVal;
  }

  return;
}

static inline void FFTLIB_UTIL_scale_shift_saturate_relu(uint64_t x, int8_t scale, uint8_t shift, uint16_t *y)
{
   int64_t xVal = x;
   int64_t scaledValue = xVal*scale;

//   uint32_t shift32_t = (uint32_t) shift;
//
//   if (shift32_t > 0) {
//      xVal = (scaledValue >> (shift32_t - (uint32_t)1) ) + 1;
//      xVal = (xVal >> 1);
//   } else {
//      xVal = scaledValue;
//   }
   int32_t tmp;
   if(shift > 0) {
      tmp = __shift_right(scaledValue, (shift - 1)) + 1;
      tmp = __shift_right(tmp, 1);
   } else {
      tmp = scaledValue;
   }

   if (tmp > 0xFFFF) {
      *y = 0xFFFF;
   } else {
      *y = (uint16_t)tmp;
   }

   return;
}


/*******************************************************************************
 *
 * Inline scale, shift, round and ReLU operation
 *
 ******************************************************************************/

template <typename dataType, typename dataTypeS, typename returnType>
static inline returnType FFTLIB_UTIL_scaleShiftRoundAndReLU(dataType inVal, dataTypeS scale, uint8_t shift){
  returnType result;
//#if FFTLIB_DEBUGPRINT
//             printf("FFTLIB_DEBUGPRINT C: inVal %d scale %d shift %d\n", inVal,scale, shift);
//#endif
  FFTLIB_UTIL_scale_shift_saturate_relu(inVal, scale, shift, &result);

//  if(shift == 0){
//    // remove the rounding, which doesn't make sense with no shift but causes C code problems
//    FFTLIB_UTIL_saturate_relu(inVal, &result);
//  } else {
//     // round and shift
//     // Method requires right shift of signed integers be an arithmetic shift, but right
//     // shift >> on signed integer types is implementation dependent on whether the shift is
//     // arithmetic or logical.  There's no simple way in C to specify the shift type as arithmetic.
//     // Instead, we use the __shift_right intrinsic, which is defined to be arithmetic shift.
//     dataType temp;
//     temp = __shift_right(inVal, (shift - 1)) + 1;
//     temp = __shift_right(temp, 1);
//     FFTLIB_UTIL_saturate_relu(temp, &result);
//  }

  return result;
}

template  int8_t  FFTLIB_UTIL_scaleShiftRoundAndReLU<int32_t,  uint8_t, int8_t>  (int32_t inVal, uint8_t scale, uint8_t shift);
template  int16_t FFTLIB_UTIL_scaleShiftRoundAndReLU<int64_t,  uint8_t, int16_t> (int64_t inVal, uint8_t scale, uint8_t shift);
template  int8_t  FFTLIB_UTIL_scaleShiftRoundAndReLU<int32_t,  int8_t, int8_t>  (int32_t inVal, int8_t scale, uint8_t shift);
template  int16_t FFTLIB_UTIL_scaleShiftRoundAndReLU<int64_t,  int8_t, int16_t> (int64_t inVal, int8_t scale, uint8_t shift);

/*******************************************************************************
 *
 * Inline scale shift saturate operation
 *
 ******************************************************************************/

static inline int8_t FFTLIB_UTIL_scaleShiftSaturate(int32_t x, uint8_t scale, uint8_t shift)
{
  int8_t retVal;
  int64_t xVal = x;
  int64_t scaledValue = xVal*scale;

  if(shift > 0) {
     xVal = __shift_right(scaledValue, (shift - 1)) + 1;
     xVal = __shift_right(xVal, 1);
  } else {
     xVal = scaledValue;
  }

  if (xVal > 0x7F) {
     retVal = 0x7F;
  } else if (xVal < -0x80) {
     retVal = -0x80;
  } else {
     retVal = (int8_t)xVal;
  }
  return retVal;
}

static inline uint8_t FFTLIB_UTIL_scaleShiftSaturate(uint32_t x, uint8_t scale, uint8_t shift)
{
   uint8_t retVal;
   uint64_t xVal = x;
   uint64_t scaledValue = xVal*scale;

   uint32_t shift32_t = (uint32_t) shift;

   if (shift32_t > 0) {
      xVal = (scaledValue >> (shift32_t - (uint32_t)1) ) + 1;
      xVal = (xVal >> 1);
   } else {
      xVal = scaledValue;
   }

   if (xVal > 0xFF) {
      retVal = 0xFF;
   } else {
      retVal = (uint8_t)xVal;
   }
   return retVal;
}

static inline int16_t FFTLIB_UTIL_scaleShiftSaturate(int64_t x, uint8_t scale, uint8_t shift)
{
  int16_t retVal;
  int64_t xVal = x;
  int64_t scaledValue = xVal*scale;

  if(shift > 0) {
     xVal = __shift_right(scaledValue, (shift - 1)) + 1;
     xVal = __shift_right(xVal, 1);
  } else {
     xVal = scaledValue;
  }

  if (xVal > 0x7FFF) {
     retVal = 0x7FFF;
  } else if (xVal < -0x8000) {
     retVal = -0x8000;
  } else {
     retVal = (int16_t)xVal;
  }
  return retVal;
}

static inline uint16_t FFTLIB_UTIL_scaleShiftSaturate(uint64_t x, uint8_t scale, uint8_t shift)
{
   uint16_t retVal;
   uint64_t xVal = x;
   uint64_t scaledValue = xVal*scale;

   uint32_t shift32_t = (uint32_t) shift;

   if (shift32_t > 0) {
      xVal = (scaledValue >> (shift32_t - (uint32_t)1) ) + 1;
      xVal = (xVal >> 1);
   } else {
      xVal = scaledValue;
   }

   if (xVal > 0xFFFF) {
      retVal = 0xFFFF;
   } else {
      retVal = (uint16_t)xVal;
   }
   return retVal;
}

/*static inline int32_t FFTLIB_UTIL_scaleShiftSaturate(int64_t xh, int64_t xl)
{
  int32_t retVal;
  //printf("%s: xh = %" PRIx64 ", xl = %" PRIx64 "\n", __FUNCTION__, xh, xl);
  // if negative
  if(((uint64_t)xh & 0x8000000000000000ULL) != 0LL){
    if( ((~(uint64_t)xh & 0xFFFFFFFFFFFFFFFFULL) != 0LL) || ((~(uint64_t)xl & 0xFFFFFFFF80000000ULL) != 0LL)){
      retVal = ((int32_t)0x80000000U);
    } else {
      retVal = (int32_t)xl;
    }
  } else if (((uint64_t)xl & 0xFFFFFFFF80000000ULL) != 0LL){
    //(xl > 0x000000007FFFFFFFLL){ // positive and saturated
    retVal = ((int32_t)0x7FFFFFFFU);
  } else {
    retVal = (int32_t)xl;
  }
  return retVal;
}

static inline int32_t FFTLIB_UTIL_scaleShiftSaturate(FFTLIB_int128_t x)
{
  return FFTLIB_UTIL_saturate(x.hi(), x.lo());
}
*/

//int8_t scale
static inline int8_t FFTLIB_UTIL_scaleShiftSaturate(int32_t x, int8_t scale, uint8_t shift)
{
  int8_t retVal;
  int64_t xVal = x;
  int64_t scaledValue = xVal*scale;

  if(shift > 0) {
     xVal = __shift_right(scaledValue, (shift - 1)) + 1;
     xVal = __shift_right(xVal, 1);
  } else {
     xVal = scaledValue;
  }

  if (xVal > 0x7F) {
     retVal = 0x7F;
  } else if (xVal < -0x80) {
     retVal = -0x80;
  } else {
     retVal = (int8_t)xVal;
  }
  return retVal;
}

static inline uint8_t FFTLIB_UTIL_scaleShiftSaturate(uint32_t x, int8_t scale, uint8_t shift)
{
   uint8_t retVal;
   int64_t xVal = x;
   int64_t scaledValue = xVal*scale;

//   uint32_t shift32_t = (uint32_t) shift;
//
//   if (shift32_t > 0) {
//      xVal = (scaledValue >> (shift32_t - (uint32_t)1) ) + 1;
//      xVal = (xVal >> 1);
//   } else {
//      xVal = scaledValue;
//   }
   int32_t tmp;
   if(shift > 0) {
      tmp = __shift_right(scaledValue, (shift - 1)) + 1;
      tmp = __shift_right(tmp, 1);
   } else {
      tmp = scaledValue;
   }

   if (tmp > 0xFF) {
      retVal = 0xFF;
   } else {
      retVal = (uint8_t)tmp;
   }
   return retVal;
}

static inline int16_t FFTLIB_UTIL_scaleShiftSaturate(int64_t x, int8_t scale, uint8_t shift)
{
  int16_t retVal;
  int64_t xVal = x;
  int64_t scaledValue = xVal*scale;

  if(shift > 0) {
     xVal = __shift_right(scaledValue, (shift - 1)) + 1;
     xVal = __shift_right(xVal, 1);
  } else {
     xVal = scaledValue;
  }

  if (xVal > 0x7FFF) {
     retVal = 0x7FFF;
  } else if (xVal < -0x8000) {
     retVal = -0x8000;
  } else {
     retVal = (int16_t)xVal;
  }
  return retVal;
}

static inline uint16_t FFTLIB_UTIL_scaleShiftSaturate(uint64_t x, int8_t scale, uint8_t shift)
{
   uint16_t retVal;
   int64_t xVal = x;
   int64_t scaledValue = xVal*scale;

//   uint32_t shift32_t = (uint32_t) shift;
//
//   if (shift32_t > 0) {
//      xVal = (scaledValue >> (shift32_t - (uint32_t)1) ) + 1;
//      xVal = (xVal >> 1);
//   } else {
//      xVal = scaledValue;
//   }
   int32_t tmp;
   if(shift > 0) {
      tmp = __shift_right(scaledValue, (shift - 1)) + 1;
      tmp = __shift_right(tmp, 1);
   } else {
      tmp = scaledValue;
   }

   if (tmp > 0xFFFF) {
      retVal = 0xFFFF;
   } else {
      retVal = (uint16_t)tmp;
   }
   return retVal;
}

/*******************************************************************************
 *
 * Inline shift and round operation
 *
 ******************************************************************************/

template <typename dataType, typename dataTypeS, typename returnType>
inline returnType FFTLIB_UTIL_scaleShiftAndRound(dataType inVal, dataTypeS scale, uint8_t shift){
   returnType result;

   result = FFTLIB_UTIL_scaleShiftSaturate(inVal, scale, shift);

   return result;
}

template int8_t  FFTLIB_UTIL_scaleShiftAndRound<int32_t, uint8_t, int8_t>  (int32_t inVal, uint8_t scale, uint8_t shift);
template int16_t FFTLIB_UTIL_scaleShiftAndRound<int64_t, uint8_t, int16_t> (int64_t inVal, uint8_t scale, uint8_t shift);
template int8_t  FFTLIB_UTIL_scaleShiftAndRound<int32_t, int8_t, int8_t>  (int32_t inVal, int8_t scale, uint8_t shift);
template int16_t FFTLIB_UTIL_scaleShiftAndRound<int64_t, int8_t, int16_t> (int64_t inVal, int8_t scale, uint8_t shift);

#endif //#ifdef __cplusplus
/*******************************************************************************
 *
 * Assembly functions
 *
 ******************************************************************************/

uint64_t FFTLIB_UTIL_mma_bist(void);


/*******************************************************************************
 *
 * C++ function prototypes
 *
 ******************************************************************************/
#ifdef __cplusplus
// functions only implemented in C++.  Could potentially develop a wrapper for C
// linkage if necessary

template <typename accumulateType>
void FFTLIB_CN_initDynamicRange(FFTLIB_initDynamicRange_type initDynamicRange, accumulateType temp);

#endif //__cplusplus


/*******************************************************************************
 *
 * C function prototypes
 *
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

// put C function prototypes here

#ifdef __cplusplus
}
#endif /* __cplusplus */

/*******************************************************************************
 *
 * Test code, removed by #if, left for future use.
 *
 ******************************************************************************/
#if 0
// int32_t quick tests
FFTLIB_CN_initDynamicRange(FFTLIB_DYNAMIC_RANGE_INIT, (int32_t)0x0);
FFTLIB_CN_updateDynamicRange(FFTLIB_DYNAMIC_RANGE_DISABLE, (int32_t)0x1);        // no change, new min & max
FFTLIB_CN_updateDynamicRange(FFTLIB_DYNAMIC_RANGE_ENABLE, (int32_t)0x1);         // new min & max
FFTLIB_CN_updateDynamicRange(FFTLIB_DYNAMIC_RANGE_ENABLE, (int32_t)0x2);         // new max
FFTLIB_CN_updateDynamicRange(FFTLIB_DYNAMIC_RANGE_ENABLE, (int32_t) -1);         // new min
FFTLIB_CN_updateDynamicRange(FFTLIB_DYNAMIC_RANGE_ENABLE, (int32_t)0x7FFFFFFF);  // new max, largest positive
FFTLIB_CN_updateDynamicRange(FFTLIB_DYNAMIC_RANGE_ENABLE, (int32_t)0x80000000);  // new min, most negative
FFTLIB_CN_updateDynamicRange(FFTLIB_DYNAMIC_RANGE_ENABLE, (int32_t)0x2);         // no change
FFTLIB_CN_updateDynamicRange(FFTLIB_DYNAMIC_RANGE_ENABLE, (int32_t) -1);         // no change

// uint32_t quick tests
FFTLIB_CN_initDynamicRange(FFTLIB_DYNAMIC_RANGE_INIT, (uint32_t)0x0);
FFTLIB_DEBUGPRINTFN(2, "uint32_t test%s", "\n");
FFTLIB_CN_updateDynamicRange(FFTLIB_DYNAMIC_RANGE_DISABLE, (uint32_t)0x1);        // no change, new min & max
FFTLIB_CN_updateDynamicRange(FFTLIB_DYNAMIC_RANGE_ENABLE, (uint32_t)0x1);        // new min & max
FFTLIB_CN_updateDynamicRange(FFTLIB_DYNAMIC_RANGE_ENABLE, (uint32_t)0x2);        // new max
FFTLIB_CN_updateDynamicRange(FFTLIB_DYNAMIC_RANGE_ENABLE, (uint32_t)0x7FFFFFFF); // new max, half-way point
FFTLIB_CN_updateDynamicRange(FFTLIB_DYNAMIC_RANGE_ENABLE, (uint32_t)0xFFFFFFFF); // new max, largest value
FFTLIB_CN_updateDynamicRange(FFTLIB_DYNAMIC_RANGE_ENABLE, (uint32_t)0x0);        // new min, minimum value
FFTLIB_CN_updateDynamicRange(FFTLIB_DYNAMIC_RANGE_ENABLE, (uint32_t)0xbeef);     // no change

// int64_t quick tests
FFTLIB_CN_initDynamicRange(FFTLIB_DYNAMIC_RANGE_INIT, (int64_t)0x0);
FFTLIB_CN_updateDynamicRange(FFTLIB_DYNAMIC_RANGE_DISABLE, (int64_t)0x1);        // no change, new min & max
FFTLIB_CN_updateDynamicRange(FFTLIB_DYNAMIC_RANGE_ENABLE, (int64_t)0x1);         // new min & max
FFTLIB_CN_updateDynamicRange(FFTLIB_DYNAMIC_RANGE_ENABLE, (int64_t)0x2);         // new max
FFTLIB_CN_updateDynamicRange(FFTLIB_DYNAMIC_RANGE_ENABLE, (int64_t) -1);         // new min
FFTLIB_CN_updateDynamicRange(FFTLIB_DYNAMIC_RANGE_ENABLE, (int64_t)0x7FFFFFFFFFFFFFFF);  // new max, largest positive
FFTLIB_CN_updateDynamicRange(FFTLIB_DYNAMIC_RANGE_ENABLE, (int64_t)0x8000000000000000);  // new min, most negative
FFTLIB_CN_updateDynamicRange(FFTLIB_DYNAMIC_RANGE_ENABLE, (int64_t)0x2);         // no change
FFTLIB_CN_updateDynamicRange(FFTLIB_DYNAMIC_RANGE_ENABLE, (int64_t) -1);         // no change

// uint64_t quick tests
FFTLIB_CN_initDynamicRange(FFTLIB_DYNAMIC_RANGE_INIT, (uint64_t)0x0);
FFTLIB_DEBUGPRINTFN(2, "uint64_t test%s", "\n");
FFTLIB_CN_updateDynamicRange(FFTLIB_DYNAMIC_RANGE_DISABLE, (uint64_t)0x1);        // no change, new min & max
FFTLIB_CN_updateDynamicRange(FFTLIB_DYNAMIC_RANGE_ENABLE, (uint64_t)0x1);        // new min & max
FFTLIB_CN_updateDynamicRange(FFTLIB_DYNAMIC_RANGE_ENABLE, (uint64_t)0x2);        // new max
FFTLIB_CN_updateDynamicRange(FFTLIB_DYNAMIC_RANGE_ENABLE, (uint64_t)0x7FFFFFFFFFFFFFFF);  // new max, largest positive
FFTLIB_CN_updateDynamicRange(FFTLIB_DYNAMIC_RANGE_ENABLE, (uint64_t)0xFFFFFFFFFFFFFFFF); // new max, largest value
FFTLIB_CN_updateDynamicRange(FFTLIB_DYNAMIC_RANGE_ENABLE, (uint64_t)0x0);        // new min, minimum value
FFTLIB_CN_updateDynamicRange(FFTLIB_DYNAMIC_RANGE_ENABLE, (uint64_t)0xdeadbeefbeefdead);     // no change
#endif // #if 0

#endif /* C7120_FFTLIB_UTILITY_H_ */
