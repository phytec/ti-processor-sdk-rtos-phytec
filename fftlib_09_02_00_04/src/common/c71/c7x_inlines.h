
/******************************************************************************/
/*!
 * \file c7x_inlines.h
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

#ifndef COMMON_FFTLIB_C7X_INLINES_H_
#define COMMON_FFTLIB_C7X_INLINES_H_ 1

/*******************************************************************************
 *
 * INCLUDES
 *
 ******************************************************************************/




/******************************************************************************/

/******************************************************************************/
/*!
 * \ingroup c7x_inlines 
 * \brief   c7x specific inlines
 *
 * \details 
 * \param 
 * \return  void
 */
/******************************************************************************/

#ifdef __cplusplus
__INLINE_FUNC(c7x_permute_even_even_xxx)
template <typename dataType>
static inline uchar64 c7x_permute_even_even_xxx(
                         uchar64 permCtrl,
                         uchar64 in1,
                         uchar64 in0)
{
   uchar64 retVal;

   if (sizeof(dataType) == sizeof(int32_t)) {
       retVal = __permute_even_even_int(permCtrl, in1, in0);
   } else if (sizeof(dataType) == sizeof(int16_t)) {
       retVal = __permute_even_even_short(permCtrl, in1, in0);
   } else {
       retVal = (uchar64)(0);
   }
   return retVal;
}

__INLINE_FUNC(c7x_permute_odd_odd_xxx)
template <typename dataType>
static inline uchar64 c7x_permute_odd_odd_xxx(
                         uchar64 permCtrl,
                         uchar64 in1,
                         uchar64 in0)
{
   uchar64 retVal;

   if (sizeof(dataType) == sizeof(int32_t)) {
       retVal = __permute_odd_odd_int(permCtrl, in1, in0);
   } else if (sizeof(dataType) == sizeof(int16_t)) {
       retVal = __permute_odd_odd_short(permCtrl, in1, in0);
   } else {
       retVal = (uchar64)(0);
   }
   return retVal;
}
#endif
#endif /* COMMON_FFTLIB_C7X_INLINES_H_ */
