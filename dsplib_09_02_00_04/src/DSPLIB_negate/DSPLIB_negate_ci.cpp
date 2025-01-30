/******************************************************************************/
/*!
 * \file DSPLIB_negate.c
 */
/* Copyright (C) 2017 Texas Instruments Incorporated - http://www.ti.com/
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

#include "../common/c71/DSPLIB_inlines.h"
#include "DSPLIB_negate_priv.h"
#include <cstdint>
#include <float.h>

template <typename dataType>
DSPLIB_STATUS DSPLIB_negate_init_ci(DSPLIB_kernelHandle           handle,
                                    const DSPLIB_bufParams1D_t   *bufParamsIn,
                                    const DSPLIB_bufParams1D_t   *bufParamsOut,
                                    const DSPLIB_negate_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS           status       = DSPLIB_SUCCESS;
   DSPLIB_negate_PrivArgs *pKerPrivArgs = (DSPLIB_negate_PrivArgs *) handle;
   uint32_t                blockSize    = pKerPrivArgs->blockSize;

   __SE_TEMPLATE_v1 se0Params;
   __SA_TEMPLATE_v1 sa0Params;

   __SE_ELETYPE SE_ELETYPE;
   __SE_VECLEN  SE_VECLEN;
   __SA_VECLEN  SA_VECLEN;

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   typedef typename c7x::make_full_vector<dataType>::type vec;
   SE_VECLEN                                                       = c7x::se_veclen<vec>::value;
   SA_VECLEN                                                       = c7x::sa_veclen<vec>::value;
   SE_ELETYPE                                                      = c7x::se_eletype<vec>::value;

#if DSPLIB_DEBUGPRINT
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;
   printf("Enter eleCount %d\n", eleCount);
#endif

   /**********************************************************************/
   /* Prepare streaming engine 0 to fetch the input                      */
   /**********************************************************************/

   se0Params = __gen_SE_TEMPLATE_v1();

   se0Params.ICNT0   = blockSize;
   se0Params.VECLEN  = SE_VECLEN;
   se0Params.ELETYPE = SE_ELETYPE;
   se0Params.DIMFMT  = __SE_DIMFMT_1D;

   /**********************************************************************/
   /* Prepare SA template to store output                                */
   /**********************************************************************/
   sa0Params = __gen_SA_TEMPLATE_v1();

   sa0Params.ICNT0  = blockSize;
   sa0Params.DIM1   = blockSize;
   sa0Params.VECLEN = SA_VECLEN;
   sa0Params.DIMFMT = __SA_DIMFMT_1D;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET) = sa0Params;

   return status;
}

template DSPLIB_STATUS DSPLIB_negate_init_ci<int32_t>(DSPLIB_kernelHandle           handle,
                                                      const DSPLIB_bufParams1D_t   *bufParamsIn,
                                                      const DSPLIB_bufParams1D_t   *bufParamsOut,
                                                      const DSPLIB_negate_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_negate_init_ci<int16_t>(DSPLIB_kernelHandle           handle,
                                                      const DSPLIB_bufParams1D_t   *bufParamsIn,
                                                      const DSPLIB_bufParams1D_t   *bufParamsOut,
                                                      const DSPLIB_negate_InitArgs *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_negate_init_ci<int8_t>(DSPLIB_kernelHandle           handle,
                                                     const DSPLIB_bufParams1D_t   *bufParamsIn,
                                                     const DSPLIB_bufParams1D_t   *bufParamsOut,
                                                     const DSPLIB_negate_InitArgs *pKerInitArgs);

template <typename dataType, int32_t dataIn>
DSPLIB_STATUS DSPLIB_negate_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_negate_PrivArgs *pKerPrivArgs = (DSPLIB_negate_PrivArgs *) handle;
   uint32_t                blockSize    = pKerPrivArgs->blockSize;

   __SE_TEMPLATE_v1 se0Params;
   __SA_TEMPLATE_v1 sa0Params;

   dataType *restrict pInLocal  = (dataType *) pIn;
   dataType *restrict pOutLocal = (dataType *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_negate_exec_ci\n");
#endif

   typedef typename c7x::make_full_vector<dataType>::type vec;
   int32_t                                                eleCount = c7x::element_count_of<vec>::value;

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);
   // Input samples
   __SE0_OPEN(pInLocal, se0Params);

   // Output samples
   __SA0_OPEN(sa0Params);

   vec zero = (vec) 0;

   for (int32_t counter = 0; counter < blockSize; counter += eleCount) {

      vec a = c7x::strm_eng<0, vec>::get_adv();
      /* printf("\ninput: \n"); */
      /* a.print(); */
      vec out = __sub_sat(zero, a);
      /* printf("\noutput: \n"); */
      /* out.print(); */
      /* vec     out = -a; */
      __vpred tmp = c7x::strm_agen<0, vec>::get_vpred();
      vec    *VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);

      __vstore_pred(tmp, VB1, out);
   }

   __SE0_CLOSE();
   __SA0_CLOSE();

   return DSPLIB_SUCCESS;
}

template DSPLIB_STATUS
DSPLIB_negate_exec_ci<int32_t, DSPLIB_INT32>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_negate_exec_ci<int16_t, DSPLIB_INT16>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_negate_exec_ci<int8_t, DSPLIB_INT8>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
