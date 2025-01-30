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
#include "FFTLIB_test.h"

/*******************************************************************************
*
* Internal helper functions (utilizing C++)
*
******************************************************************************/

// this function is called "i64s" because the value is signed 64bits stored in a signed 64-bit container
int16_t outputProcessDynamicRange_i64s_o16s(int64_t value, uint8_t shift, uint8_t activationType){
   int16_t result;
   
   if (activationType == FFTLIB_RELU) {
      result = FFTLIB_UTIL_shiftRoundAndReLU<int64_t, int16_t>(value, shift);
   } else {
      result = FFTLIB_UTIL_shiftAndRound<int64_t, int16_t>(value, shift);
   }
   
   return result;
}

// this function is called "i32s" because the value is signed 32bits stored in a signed 64-bit container
int8_t outputProcessDynamicRange_i32s_o8s(int64_t value, uint8_t shift, uint8_t activationType){
   int2 temp = __as_int2(value);
   int8_t result;
   
   if (activationType == FFTLIB_RELU) {
      result = FFTLIB_UTIL_shiftRoundAndReLU<int32_t, int8_t>(temp.s[0], shift);
   } else {
      result = FFTLIB_UTIL_shiftAndRound<int32_t, int8_t>(temp.s[0], shift);
   }
   
   return result;
}

// this function is called "i64u" because the value is unsigned 64bits stored in a signed 64-bit container
uint16_t outputProcessDynamicRange_i64u_o16u(int64_t value, uint8_t shift, uint8_t activationType){
   uint64_t temp = __as_ulong(value);
   uint16_t result;
   
   if (activationType == FFTLIB_RELU) {
      result = FFTLIB_UTIL_shiftRoundAndReLU<uint64_t, uint16_t>(temp, shift);
   } else {
      result = FFTLIB_UTIL_shiftAndRound<uint64_t, uint16_t>(temp, shift);
   }
   
   return result;
}

// this function is called "i32u" because the value is unsigned 32bits stored in a signed 64-bit container
uint8_t outputProcessDynamicRange_i32u_o8u(int64_t value, uint8_t shift, uint8_t activationType){
   uint2 temp = __as_uint2(value);
   uint8_t result;
   
   if (activationType == FFTLIB_RELU) {
      result = FFTLIB_UTIL_shiftRoundAndReLU<uint32_t, uint8_t>(temp.s[0], shift);
   } else {
      result = FFTLIB_UTIL_shiftAndRound<uint32_t, uint8_t>(temp.s[0], shift);
   }
   
   return result;
}


/*******************************************************************************
*
* Public functions with C linkage
*
******************************************************************************/

// want this function to have C-linkage in library...
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

// Why does this function exist?  Well, our reference test case generator does not provide access to
// internal accumulator bit-width values, so we determine min/max from the output feature map at the
// output bit width.  However, the MMA returns min/max from the internal accumulator at its bit width
// (always packed into a int64_t container).  This function takes the dynamicRangeComputed values and
// output processes it the same way the output are so that it can be compared to the reference values

// Note: A "passing" comparison is returned as "0" and a "failing" as "1"
uint8_t FFTLIB_TEST_compareDynamicRange(void *dynamicRangeRef, int64_t dynamicRangeComputed, uint32_t outputType, uint8_t shift, uint8_t activationType ){
   uint8_t result = 1;
   
   if(outputType == FFTLIB_INT16){
      int16_t ref = *((int16_t *) dynamicRangeRef);
      int16_t comp = outputProcessDynamicRange_i64s_o16s(dynamicRangeComputed, shift, activationType);
      if (ref == comp) {
         result = 0;
      } else {
         result = 1;
      }
   } else if(outputType == FFTLIB_INT8){
      int8_t ref = *((int8_t *) dynamicRangeRef);
      int8_t comp = outputProcessDynamicRange_i32s_o8s(dynamicRangeComputed, shift, activationType);
      if (ref == comp) {
         result = 0;
      } else {
         result = 1;
      }
   } else if(outputType == FFTLIB_UINT16){
      uint16_t ref = *((uint16_t *) dynamicRangeRef);
      uint16_t comp = outputProcessDynamicRange_i64u_o16u(dynamicRangeComputed, shift, activationType);
      if (ref == comp) {
         result = 0;
      } else {
         result = 1;
      }
   } else if(outputType == FFTLIB_UINT8){
      uint8_t ref = *((uint8_t *) dynamicRangeRef);
      uint8_t comp = outputProcessDynamicRange_i32u_o8u(dynamicRangeComputed, shift, activationType);
      if (ref == comp) {
         result = 0;
      } else {
         result = 1;
      }
   } else {
      result = 1;
   }
   
   return result;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

