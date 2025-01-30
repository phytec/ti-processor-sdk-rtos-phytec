/******************************************************************************/
/*!
 * \file FFTLIB_UTIL_SA0Cback.c
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

#ifndef COMMON_FFTLIB_UTIL_SA0CSTORE_H_
#define COMMON_FFTLIB_UTIL_SA0CSTORE_H_ 1

/*******************************************************************************
 *
 * INCLUDES
 *
 ******************************************************************************/




/******************************************************************************/

/* ---------------------------------------------------------------- */
/*  MISRAC Rule 8.13(NEEDS.CONST) Deviation: CbackDestination cannot*/
/*  be declared with const qualifier because data is copied into    */
/*  buffer pointed by the pointer.                                  */
/* ---------------------------------------------------------------- */
/******************************************************************************/
/*!
 * \ingroup fftlib_util
 * \brief   
 * \details 
 * \param[in] numRows
 * \param[in] CbackDestination
 * \param[in] paramsC
 * \return  void
 */
/******************************************************************************/
#ifdef __cplusplus
#pragma FUNC_ALWAYS_INLINE
#else
#pragma FUNC_ALWAYS_INLINE(FFTLIB_UTIL_SA0Cstore)
#endif
static inline void FFTLIB_UTIL_SA0Cstore(int32_t numRows, uint8_t *CbackDestination)
{
   int32_t r;
   __mma_vec VB0;

   for(r = 0; r < numRows; r++){
      // move row of CBack to transfer buffer
      __HWAXFER(__MMA_XFER_SRC_C);
      // move row from transfer buffer to VB (data enters C7x core)
      VB0 = __HWARCV(0);
      __HWAADV();
        // move row from VB to memory
      __vpred tmp = c7x::strm_agen<0, __mma_vec>::get_vpred();
      __mma_vec * VB1 = c7x::strm_agen<0, __mma_vec>::get_adv(CbackDestination);
      //FFTLIB_DEBUGPRINTFN(2, "store address: %p\n", __SA0(uchar64, CbackDestination));
      __vstore_pred(tmp, VB1, VB0);
   }
   return;
}

/* ---------------------------------------------------------------- */
/*  MISRAC Rule 8.13(NEEDS.CONST) Deviation: CbackDestination cannot*/
/*  be declared with const qualifier because data is copied into    */
/*  buffer pointed by the pointer.                                  */
/* ---------------------------------------------------------------- */
#ifdef __cplusplus
#pragma FUNC_ALWAYS_INLINE
#else
#pragma FUNC_ALWAYS_INLINE(FFTLIB_UTIL_SA0Cstore_unroll)
#endif
#ifdef __cplusplus
template <int32_t UNROLL_TIMES>
static inline void FFTLIB_UTIL_SA0Cstore_unroll(int32_t numRows, uint8_t *CbackDestination)
{
   int32_t r;
   __mma_vec VB0;
   __vpred tmp;
    //_nassert(numRows == 32);
   FFTLIB_UNROLL(UNROLL_TIMES)
   for(r = 0; r < numRows; r++){
      // move row of CBack to transfer buffer
      __HWAXFER(__MMA_XFER_SRC_C);
      // move row from transfer buffer to VB (data enters C7x core)
      VB0 = __HWARCV(0);
      __HWAADV();
        // move row from VB to memory
      tmp = c7x::strm_agen<0, __mma_vec>::get_vpred();
      __mma_vec * VB1 = c7x::strm_agen<0, __mma_vec>::get_adv(CbackDestination);
      //FFTLIB_DEBUGPRINTFN(2, "store address: %p\n", __SA0(uchar64, CbackDestination));
      __vstore_pred(tmp, VB1, VB0);
   }
   return;
}
#endif
#endif /* COMMON_FFTLIB_UTIL_SA0CSTORE_H_ */
