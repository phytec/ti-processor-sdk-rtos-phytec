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

/*******************************************************************************
 *
 * INCLUDES
 *
 ******************************************************************************/

#include "../common/c71/VXLIB_inlines.h"
#include "VXLIB_absDiff_priv.h"

/*******************************************************************************
 *
 * DEFINES
 *
 ******************************************************************************/

#define SE_PARAM_BASE (0x0000)
#define SE0_PARAM_OFFSET (SE_PARAM_BASE)
#define SE1_PARAM_OFFSET (SE0_PARAM_OFFSET + VXLIB_SE_PARAM_SIZE)
#define SA0_PARAM_OFFSET (SE1_PARAM_OFFSET + VXLIB_SE_PARAM_SIZE)

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_absDiff_init_ci                                                                                              */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method initializes the kernel-specific parameters
// mainly, the streaming engine and streaming address generators
template <uint32_t dTypeIn0>
VXLIB_STATUS VXLIB_absDiff_init_ci(VXLIB_kernelHandle            handle,
                                   const VXLIB_bufParams2D_t    *bufParamsIn0,
                                   const VXLIB_bufParams2D_t    *bufParamsIn1,
                                   const VXLIB_bufParams2D_t    *bufParamsOut,
                                   const VXLIB_absDiff_InitArgs *pKerInitArgs)
{
   VXLIB_STATUS status = VXLIB_SUCCESS; // assign status to success by default

   // structs to hold SE and SA parameters
   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1();
   __SE_TEMPLATE_v1 se1Params = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa0Params = __gen_SA_TEMPLATE_v1();

   VXLIB_absDiff_PrivArgs *pKerPrivArgs = (VXLIB_absDiff_PrivArgs *) handle;

   uint8_t *pBlock = pKerPrivArgs->bufPblock; // address to retrieve to store SE/SA params

   // set SE0, SE1, and SA0 params
   pKerPrivArgs->numBlocks = VXLIB_SE0SE1SA0_init<dTypeIn0, dTypeIn0, dTypeIn0>(
       &se0Params, &se1Params, &sa0Params, bufParamsIn0, bufParamsIn1, bufParamsOut);

   /**************************/
   /* Store SE and SA params */
   /**************************/

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET) = se0Params;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET) = se1Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET) = sa0Params;

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* Explicit instantiations for VXLIB_absDiff_init_ci                                                                  */
/*                                                                                                                    */
/**********************************************************************************************************************/

template VXLIB_STATUS VXLIB_absDiff_init_ci<VXLIB_INT16>(VXLIB_kernelHandle            handle,
                                                         const VXLIB_bufParams2D_t    *bufParamsIn0,
                                                         const VXLIB_bufParams2D_t    *bufParamsIn1,
                                                         const VXLIB_bufParams2D_t    *bufParamsOut,
                                                         const VXLIB_absDiff_InitArgs *pKerInitArgs);

template VXLIB_STATUS VXLIB_absDiff_init_ci<VXLIB_UINT8>(VXLIB_kernelHandle            handle,
                                                         const VXLIB_bufParams2D_t    *bufParamsIn0,
                                                         const VXLIB_bufParams2D_t    *bufParamsIn1,
                                                         const VXLIB_bufParams2D_t    *bufParamsOut,
                                                         const VXLIB_absDiff_InitArgs *pKerInitArgs);

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_absDiff_calc                                                                                                 */
/*                                                                                                                    */
/**********************************************************************************************************************/

// calculates the absolute difference based on input type
template <typename dataType> inline dataType VXLIB_absDiff_calc(dataType vecA, dataType vecB);

/**********************************************************************************************************************/
/*                                                                                                                    */
/* Explicit instantiations for VXLIB_absDiff_init_calc                                                                */
/*                                                                                                                    */
/**********************************************************************************************************************/

// calculation for a uint8_t
template <> inline c7x::uchar_vec VXLIB_absDiff_calc<c7x::uchar_vec>(c7x::uchar_vec vecA, c7x::uchar_vec vecB)
{
   c7x::uchar_vec absDiffOut = (__abs_diff(vecA, vecB));
   return absDiffOut;
}

// calculation for a int16_t with saturation
template <> inline c7x::short_vec VXLIB_absDiff_calc<c7x::short_vec>(c7x::short_vec vecA, c7x::short_vec vecB)
{
   c7x::short_vec absDiffOut = __abs(__sub_sat(vecA, vecB));
   return absDiffOut;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_absDiff_exec_ci                                                                                              */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method performs pixel-wise absolute difference of two input images
template <typename dataType>
VXLIB_STATUS
VXLIB_absDiff_exec_ci(VXLIB_kernelHandle handle, void *restrict pIn0, void *restrict pIn1, void *restrict pOut)
{
   VXLIB_absDiff_PrivArgs *pKerPrivArgs = (VXLIB_absDiff_PrivArgs *) handle;

   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;
   __SA_TEMPLATE_v1 sa0Params;

   dataType *restrict pInLocal0 = (dataType *) pIn0;
   dataType *restrict pInLocal1 = (dataType *) pIn1;
   dataType *restrict pOutLocal = (dataType *) pOut;

#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_add_exec_ci\n");
#endif

   typedef typename c7x::make_full_vector<dataType>::type vec;

#if VXLIB_DEBUGPRINT
   printf("Enter eleCount %d\n", eleCount);
#endif
   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET);
   se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET);
   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET);

   // Input samples
   __SE0_OPEN(pInLocal0, se0Params);
   __SE1_OPEN(pInLocal1, se1Params);

   // Output samples
   __SA0_OPEN(sa0Params);

   size_t numBlocks = pKerPrivArgs->numBlocks;

   vec out;
   for (uint32_t counter = 0; counter < numBlocks; counter++) {
      vec a = c7x::strm_eng<0, vec>::get_adv();
      vec b = c7x::strm_eng<1, vec>::get_adv();

      out = VXLIB_absDiff_calc(a, b);

      __vpred tmp  = c7x::strm_agen<0, vec>::get_vpred();
      vec    *addr = c7x::strm_agen<0, vec>::get_adv(pOutLocal);

      __vstore_pred(tmp, addr, out);
   }
   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA0_CLOSE();

   return VXLIB_SUCCESS;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* Explicit instantiations for VXLIB_absDiff_exec_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

template VXLIB_STATUS VXLIB_absDiff_exec_ci<uint8_t>(VXLIB_kernelHandle handle,
                                                     void *restrict pIn0,
                                                     void *restrict pIn1,
                                                     void *restrict pOut);

template VXLIB_STATUS VXLIB_absDiff_exec_ci<int16_t>(VXLIB_kernelHandle handle,
                                                     void *restrict pIn0,
                                                     void *restrict pIn1,
                                                     void *restrict pOut);
