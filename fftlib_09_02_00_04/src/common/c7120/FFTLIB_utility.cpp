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
#include "FFTLIB_utility.h"


/**************************************************************************************
 *
 * Storage for emulating the dynamic range min/max feature of the MMA
 *
 **************************************************************************************/

int64_t FFTLIB_CN_dynamicRangeMin;
int64_t FFTLIB_CN_dynamicRangeMax;


/**************************************************************************************
 *
 * Functions for emulating the dynamic range min/max feature of the MMA
 *
 **************************************************************************************/

template <typename accumulateType>
void FFTLIB_CN_initDynamicRange(FFTLIB_initDynamicRange_type initDynamicRange, accumulateType temp)
{
   if( initDynamicRange == FFTLIB_DYNAMIC_RANGE_NOINIT ){
      // do nothing
   } else {
      FFTLIB_CN_dynamicRangeMin = 0x7FFFFFFFFFFFFFFF;
      FFTLIB_CN_dynamicRangeMax = 0x8000000000000000;
   }

   return;
}

template void FFTLIB_CN_initDynamicRange(FFTLIB_initDynamicRange_type initDynamicRange, int32_t temp);
template void FFTLIB_CN_initDynamicRange(FFTLIB_initDynamicRange_type initDynamicRange, int64_t temp);



template <>
void FFTLIB_CN_initDynamicRange(FFTLIB_initDynamicRange_type initDynamicRange, uint32_t temp)
{
   if( initDynamicRange == FFTLIB_DYNAMIC_RANGE_NOINIT ){
      // do nothing
   } else {
      FFTLIB_CN_dynamicRangeMin = 0x00000000FFFFFFFF;
      FFTLIB_CN_dynamicRangeMax = 0x0000000000000000;
   }

   return;
}

template <>
void FFTLIB_CN_initDynamicRange(FFTLIB_initDynamicRange_type initDynamicRange, uint64_t temp)
{
   if( initDynamicRange == FFTLIB_DYNAMIC_RANGE_NOINIT ){
      // do nothing
   } else {
      FFTLIB_CN_dynamicRangeMin = 0xFFFFFFFFFFFFFFFF;
      FFTLIB_CN_dynamicRangeMax = 0x0000000000000000;
   }

   return;
}
