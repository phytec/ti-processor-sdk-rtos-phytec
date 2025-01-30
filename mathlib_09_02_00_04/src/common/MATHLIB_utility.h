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

#ifndef MATHLIB_UTILITY_H_
#define MATHLIB_UTILITY_H_ 1

/******************************************************************************/
/*                                                                            */
/* Includes                                                                   */
/*                                                                            */
/******************************************************************************/

#include "MATHLIB_debugPrint.h"
#include "MATHLIB_types.h"
#include "c7x_scalable.h"
#include <limits>

/**
 * \ingroup MATHLIB_COMMON
 */

/**
 * \defgroup MATHLIB_SE_SA_COMMON SE0, SE1, and SA0 utilities
 * \brief This module consists of utility functions tailored towards SE/SA that are commonly applicable to various
 * MATHLIB kernels.
 */
/**@{*/

/******************************************************************************/
/*                                                                            */
/* MATHLIB_SESA1DSequential_init                                              */
/*                                                                            */
/******************************************************************************/

template <typename T>
static inline void MATHLIB_SE0SA01DSequentialInit(__SE_TEMPLATE_v1 *se0Params,
                                                  __SA_TEMPLATE_v1 *sa0Params,
                                                  size_t            length,
                                                  T                *pSrc,
                                                  T                *pDst)
{

   // SE and SA params that vary by precision type
   __SE_ELETYPE SE_ELETYPE;
   __SE_VECLEN  SE_VECLEN;
   __SA_VECLEN  SA_VECLEN;

   // derive c7x vector type from template typename
   typedef typename c7x::make_full_vector<T>::type vec;

   // assign SE and SA params based on vector type
   SE_VECLEN  = c7x::se_veclen<vec>::value;
   SA_VECLEN  = c7x::sa_veclen<vec>::value;
   SE_ELETYPE = c7x::se_eletype<vec>::value;

   // configure SE0 to fetch the input
   se0Params->ELETYPE = SE_ELETYPE;
   se0Params->VECLEN  = SE_VECLEN;
   se0Params->DIMFMT  = __SE_DIMFMT_1D;
   se0Params->ICNT0   = length;

   // configure SA0 to store output
   sa0Params->VECLEN = SA_VECLEN;
   sa0Params->DIMFMT = __SA_DIMFMT_1D;
   sa0Params->ICNT0  = length;

   return;
}

/******************************************************************************/
/*                                                                            */
/* MATHLIB_SE0SA0Open                                                         */
/*                                                                            */
/******************************************************************************/

/**
 * @brief This method performs SE0 and SA0 open.
 *
 * @tparam      T       : datatype of implementation
 * @param [in]  *se0Params   : pointer to buffer with SE0 params
 * @param [in]  *sa0Params   : pointer to buffer with SA0 params
 * @param [in]  *pSrc   : pointer to buffer with input vector
 * @remarks None
 */

template <typename T>
static inline void MATHLIB_SE0SA0Open(__SE_TEMPLATE_v1 *se0Params, __SA_TEMPLATE_v1 *sa0Params, T *pSrc)
{

   __SE0_OPEN(pSrc, *se0Params);
   __SA0_OPEN(*sa0Params);

   return;
}

/******************************************************************************/
/*                                                                            */
/* MATHLIB_SE0SE1SA0Open                                                      */
/*                                                                            */
/******************************************************************************/

/**
 * @brief This method performs SE0, SE1, and SA0 open.
 *
 * @tparam      T       : datatype of implementation
 * @param [in]  *se0Params   : pointer to buffer with SE0 params
 * @param [in]  *sa0Params   : pointer to buffer with SA0 params
 * @param [in]  *pSrc0   : pointer to buffer with input vector 0
 * @param [in]  *pSrc1   : pointer to buffer with input vector 1
 * @remarks None
 */

template <typename T>
static inline void MATHLIB_SE0SE1SA0Open(__SE_TEMPLATE_v1 *se0Params, __SA_TEMPLATE_v1 *sa0Params, T *pSrc0, T *pSrc1)
{

   __SE0_OPEN(pSrc0, *se0Params);
   __SE1_OPEN(pSrc1, *se0Params);
   __SA0_OPEN(*sa0Params);

   return;
}

/******************************************************************************/
/*                                                                            */
/* MATHLIB_SE0SA0Close                                                          */
/*                                                                            */
/******************************************************************************/

/**
 * @brief This method performs SE0 and SA0 close.
 *
 * @remarks None
 */

static inline void MATHLIB_SE0SA0Close()
{

   __SE0_CLOSE();
   __SA0_CLOSE();

   return;
}

/******************************************************************************/
/*                                                                            */
/* MATHLIB_SE0SE1SA0Close                                                     */
/*                                                                            */
/******************************************************************************/

/**
 * @brief This method performs SE0, SE1, and SA0 close.
 *
 * @remarks None
 */

static inline void MATHLIB_SE0SE1SA0Close()
{

   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA0_CLOSE();

   return;
}

/******************************************************************************/
/*                                                                            */
/* MATHLIB_checkParams                                                        */
/*                                                                            */
/******************************************************************************/

/**
 * @brief This method performs parameter checks for MATHLIB function
 *
 * @tparam      T       : datatype of implementation
 * @param [in]  length   : length of input vector
 * @param [in]  *pSrc   : pointer to buffer with input vector
 * @param [out] *pDst   : pointer to buffer with input vector
 *
 * @remarks None
 */

template <typename T> static inline MATHLIB_STATUS MATHLIB_checkParams(size_t length, T *pSrc, T *pDst)
{
   MATHLIB_STATUS status = MATHLIB_ERR_FAILURE;

   if ((pSrc == NULL) || (pDst == NULL)) {
      status = MATHLIB_ERR_NULL_POINTER;
   }
   else if (length == 0) {
      status = MATHLIB_ERR_INVALID_DIMENSION;
   }
   else {
      status = MATHLIB_SUCCESS;
   }

   return status;
}
template <typename T> static inline MATHLIB_STATUS MATHLIB_checkParams(size_t length, T *pSrc0, T *pSrc1, T *pDst)
{
   MATHLIB_STATUS status = MATHLIB_ERR_FAILURE;

   if ((pSrc0 == NULL) || (pSrc1 == NULL) || (pDst == NULL)) {
      status = MATHLIB_ERR_NULL_POINTER;
   }
   else if (length == 0) {
      status = MATHLIB_ERR_INVALID_DIMENSION;
   }
   else {
      status = MATHLIB_SUCCESS;
   }

   return status;
}

/** @}*/

#endif // MATHLIB_UTILITY_H_
