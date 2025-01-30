/******************************************************************************/
/*!
 * \file FFTLIB_UTIL_zeroBload.h
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

#ifndef COMMON_FFTLIB_UTIL_ZEROBLOAD_H_
#define COMMON_FFTLIB_UTIL_ZEROBLOAD_H_ 1

/*******************************************************************************
 *
 * INCLUDES
 *
 ******************************************************************************/




/******************************************************************************/

/******************************************************************************/
/*!
 * \ingroup fftlib_util
 * \brief   load numRows of B into the Bload buffer
 * \details use streaming engine 1 to load numRows of B into the Bload buffer
  * \param  numRows
  * \return  void
 */
/******************************************************************************/
__INLINE_FUNC(FFTLIB_UTIL_zeroBload)
static inline void FFTLIB_UTIL_zeroBload(int32_t numRows)
{
   int32_t r;
   __mma_vec zeroVect = (__mma_vec)(0x00);

   for(r = 0; r < numRows; r++){
      __HWALDB(zeroVect);
      __HWAADV();
   }

   return;
}

__INLINE_FUNC(FFTLIB_UTIL_zeroBload_unroll32)
static inline void FFTLIB_UTIL_zeroBload_unroll32(int32_t numRows)
{
   int32_t r;
   __mma_vec zeroVect = (__mma_vec)(0);

   FFTLIB_UNROLL(32)
   for(r = 0; r < numRows; r++){
      __HWALDB(zeroVect);
      __HWAADV();
   }

   return;
}

__INLINE_FUNC(FFTLIB_UTIL_zeroBload_unroll)
template <int32_t UNROLL_TIMES>
static inline void FFTLIB_UTIL_zeroBload_unroll(int32_t numRows)
{
   int32_t r;
   __mma_vec zeroVect = (__mma_vec)(0);


   FFTLIB_UNROLL(UNROLL_TIMES)
   for(r = 0; r < numRows; r++){
      __HWALDB(zeroVect);
      __HWAADV();
   }

   return;
}

#endif /* COMMON_FFTLIB_UTIL_ZEROBLOAD_H_ */
