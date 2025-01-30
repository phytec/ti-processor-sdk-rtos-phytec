/******************************************************************************/
/*!
 * \file FFTLIB_UTIL_SE0AloadSE1BloadComputeC.c
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

#ifndef COMMON_FFTLIB_UTIL_SE0ALOADSE1BLOADCOMPUTEC_H_
#define COMMON_FFTLIB_UTIL_SE0ALOADSE1BLOADCOMPUTEC_H_ 1

/*******************************************************************************
 *
 * INCLUDES
 *
 ******************************************************************************/




/******************************************************************************/

/******************************************************************************/
/*!
 * \ingroup fftlib_util
 * \brief   
 * \details 
  * \param[in] numRows
  * \return  void
 */
/******************************************************************************/
__INLINE_FUNC(FFTLIB_UTIL_SE0AloadSE1BloadComputeC)
static inline void FFTLIB_UTIL_SE0AloadSE1BloadComputeC(int32_t numRows)
{
    int32_t r;
   _nassert(numRows > 0);
   for(r = 0; r < numRows; r++){
       __mma_vec valA  = c7x::strm_eng<0, __mma_vec>::get_adv();
       __mma_vec valB  = c7x::strm_eng<1, __mma_vec>::get_adv();
      __HWALDAB(valA, valB);
      __HWAOP(__MMA_A_LDA);
      __HWAADV();
   }
   return;
}

__INLINE_FUNC(FFTLIB_UTIL_SE0AloadSE1BloadComputeC_unroll16)
static inline void FFTLIB_UTIL_SE0AloadSE1BloadComputeC_unroll16(int32_t numRows)
{
   int32_t r;
   
   FFTLIB_UNROLL(16)
   for(r = 0; r < numRows; r++){
       __mma_vec valA  = c7x::strm_eng<0, __mma_vec>::get_adv();
       __mma_vec valB  = c7x::strm_eng<1, __mma_vec>::get_adv();
      __HWALDAB(valA, valB);
      __HWAOP(__MMA_A_LDA);
      __HWAADV();
   }
   return;
}

__INLINE_FUNC(FFTLIB_UTIL_SE0AloadSE1BloadComputeC_unroll32)
static inline void FFTLIB_UTIL_SE0AloadSE1BloadComputeC_unroll32(int32_t numRows)
{
   int32_t r;

   FFTLIB_UNROLL(32)
   for(r = 0; r < numRows; r++){
       __mma_vec valA  = c7x::strm_eng<0, __mma_vec>::get_adv();
       __mma_vec valB  = c7x::strm_eng<1, __mma_vec>::get_adv();
      __HWALDAB(valA, valB);
      __HWAOP(__MMA_A_LDA);
      __HWAADV();
   }
   return;
}

__INLINE_FUNC(FFTLIB_UTIL_SE0AloadSE1BloadComputeC_unroll64)
static inline void FFTLIB_UTIL_SE0AloadSE1BloadComputeC_unroll64(int32_t numRows)
{
   int32_t r;

   FFTLIB_UNROLL(64)
   for(r = 0; r < numRows; r++){
       __mma_vec valA  = c7x::strm_eng<0, __mma_vec>::get_adv();
       __mma_vec valB  = c7x::strm_eng<1, __mma_vec>::get_adv();
      __HWALDAB(valA, valB);
      __HWAOP(__MMA_A_LDA);
      __HWAADV();
   }
   return;
}

#ifdef __cplusplus
__INLINE_FUNC(FFTLIB_UTIL_SE0AloadSE1BloadComputeC_unroll)
template <int32_t UNROLL_TIMES>
static inline void FFTLIB_UTIL_SE0AloadSE1BloadComputeC_unroll(int32_t numRows)
{
   int32_t r;

   FFTLIB_UNROLL(UNROLL_TIMES)
   for(r = 0; r < numRows; r++){
       __mma_vec valA  = c7x::strm_eng<0, __mma_vec>::get_adv();
       __mma_vec valB  = c7x::strm_eng<1, __mma_vec>::get_adv();
      __HWALDAB(valA, valB);
      __HWAOP(__MMA_A_LDA);
      __HWAADV();
   }
   return;
}
#endif

#ifdef __cplusplus
__INLINE_FUNC(FFTLIB_UTIL_SE0AloadSE1BloadComputeC_peel)
template <int32_t UNROLL_TIMES>
static inline void FFTLIB_UTIL_SE0AloadSE1BloadComputeC_peel(int32_t numRows, int32_t peelSize)
{
   int32_t r;

   FFTLIB_UNROLL(UNROLL_TIMES)
   for(r = 0; r < peelSize; r++){
       __mma_vec valA  = c7x::strm_eng<0, __mma_vec>::get_adv();
       __mma_vec valB  = c7x::strm_eng<1, __mma_vec>::get_adv();
      __HWALDAB(valA, valB);
      __HWAOP(__MMA_A_LDA);
      __HWAADV();
   }

   _nassert(numRows > 0);
   for(r = peelSize; r < numRows; r++){
       __mma_vec valA  = c7x::strm_eng<0, __mma_vec>::get_adv();
       __mma_vec valB  = c7x::strm_eng<1, __mma_vec>::get_adv();
      __HWALDAB(valA, valB);
      __HWAOP(__MMA_A_LDA);
      __HWAADV();
   }
   return;
}
#endif
#endif /* COMMON_FFTLIB_UTIL_SE0ALOADSE1BLOADCOMPUTEC_H_ */
