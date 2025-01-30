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

/**********************************************************************************************************************/
/*                                                                                                                    */
/* INCLUDES                                                                                                           */
/*                                                                                                                    */
/**********************************************************************************************************************/

#include "VXLIB_add_priv.h"

/**********************************************************************************************************************/
/*                                                                                                                    */
/* DEFINES                                                                                                            */
/*                                                                                                                    */
/**********************************************************************************************************************/

#define SE_PARAM_BASE (0x0000)
#define SE0_PARAM_OFFSET (SE_PARAM_BASE)
#define SE1_PARAM_OFFSET (SE0_PARAM_OFFSET + VXLIB_SE_PARAM_SIZE)
#define SA0_PARAM_OFFSET (SE1_PARAM_OFFSET + VXLIB_SE_PARAM_SIZE)

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_add_init_ci                                                                                                  */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method initializes the kernel-specific parameters
// mainly, the streaming engine and streaming address generators
template <uint32_t dTypeIn0, uint32_t dTypeIn1, uint32_t dTypeOut>
VXLIB_STATUS VXLIB_add_init_ci(VXLIB_kernelHandle         handle,
                               const VXLIB_bufParams2D_t *bufParamsIn0,
                               const VXLIB_bufParams2D_t *bufParamsIn1,
                               const VXLIB_bufParams2D_t *bufParamsOut,
                               const VXLIB_add_InitArgs  *pKerInitArgs)
{
   VXLIB_STATUS status = VXLIB_SUCCESS; // assign status to success by default

   // structs to hold SE and SA parameters
   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1();
   __SE_TEMPLATE_v1 se1Params = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa0Params = __gen_SA_TEMPLATE_v1();

   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_add_PrivArgs *pKerPrivArgs = (VXLIB_add_PrivArgs *) handle;

   uint8_t *pBlock = pKerPrivArgs->bufPblock; // address to retrieve to store SE/SA params

   // set SE0, SE1, and SA0 params
   pKerPrivArgs->numBlocks = VXLIB_SE0SE1SA0_init<dTypeIn0, dTypeIn1, dTypeOut>(
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
/* Explicit instantiations for VXLIB_add_init_ci                                                                      */
/*                                                                                                                    */
/**********************************************************************************************************************/

template VXLIB_STATUS VXLIB_add_init_ci<VXLIB_ADD_DTYPE_I8U_I8U_O8U>(VXLIB_kernelHandle         handle,
                                                                     const VXLIB_bufParams2D_t *bufParamsIn0,
                                                                     const VXLIB_bufParams2D_t *bufParamsIn1,
                                                                     const VXLIB_bufParams2D_t *bufParamsOut,
                                                                     const VXLIB_add_InitArgs  *pKerInitArgs);

template VXLIB_STATUS VXLIB_add_init_ci<VXLIB_ADD_DTYPE_I8U_I8U_O16S>(VXLIB_kernelHandle         handle,
                                                                      const VXLIB_bufParams2D_t *bufParamsIn0,
                                                                      const VXLIB_bufParams2D_t *bufParamsIn1,
                                                                      const VXLIB_bufParams2D_t *bufParamsOut,
                                                                      const VXLIB_add_InitArgs  *pKerInitArgs);

template VXLIB_STATUS VXLIB_add_init_ci<VXLIB_ADD_DTYPE_I8U_I16S_O16S>(VXLIB_kernelHandle         handle,
                                                                       const VXLIB_bufParams2D_t *bufParamsIn0,
                                                                       const VXLIB_bufParams2D_t *bufParamsIn1,
                                                                       const VXLIB_bufParams2D_t *bufParamsOut,
                                                                       const VXLIB_add_InitArgs  *pKerInitArgs);

template VXLIB_STATUS VXLIB_add_init_ci<VXLIB_ADD_DTYPE_I16S_I16S_O16S>(VXLIB_kernelHandle         handle,
                                                                        const VXLIB_bufParams2D_t *bufParamsIn0,
                                                                        const VXLIB_bufParams2D_t *bufParamsIn1,
                                                                        const VXLIB_bufParams2D_t *bufParamsOut,
                                                                        const VXLIB_add_InitArgs  *pKerInitArgs);

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_add_util_sat                                                                                                 */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method adds two inputs, saturates the result, and writes the result to output buffer
// it is assumed that SEs and SA are open before calling this method
template <typename dType> void VXLIB_add_util_sat(dType *pOut, size_t size)
{
   // derive c7x vector types from template typenames
   typedef typename c7x::make_full_vector<dType>::type vec;

   for (uint32_t counter = 0; counter < size; counter++) {
      vec a = c7x::strm_eng<0, vec>::get_adv(); // fetch input 0 via SE0
      vec b = c7x::strm_eng<1, vec>::get_adv(); // fetch input 1 via SE1

      vec out = __add_sat(a, b);

      // store output via SA0
      __vpred tmp  = c7x::strm_agen<0, vec>::get_vpred();   // generate predicate
      vec    *addr = c7x::strm_agen<0, vec>::get_adv(pOut); // generate address
      __vstore_pred(tmp, addr, out);                        // store value
   }
   return;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_add_util_wrap                                                                                                */
/*                                                                                                                    */
/**********************************************************************************************************************/

template <typename dType> void VXLIB_add_util_wrap(dType *pOut, size_t size)
{
   // derive c7x vector type from template typename
   typedef typename c7x::make_full_vector<dType>::type vec;

   for (uint32_t counter = 0; counter < size; counter++) {
      vec a = c7x::strm_eng<0, vec>::get_adv(); // fetch input 0 via SE0
      vec b = c7x::strm_eng<1, vec>::get_adv(); // fetch input 1 via SE1

      vec out = a + b;

      // store output via SA0
      __vpred tmp  = c7x::strm_agen<0, vec>::get_vpred();   // generate predicate
      vec    *addr = c7x::strm_agen<0, vec>::get_adv(pOut); // generate address
      __vstore_pred(tmp, addr, out);                        // store value
   }
   return;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_add_exec_ci                                                                                                  */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method performs pixel-wise addition of two input images
template <typename dTypeIn0, typename dTypeIn1, typename dTypeOut>
VXLIB_STATUS VXLIB_add_exec_ci(VXLIB_kernelHandle handle, void *restrict pIn0, void *restrict pIn1, void *restrict pOut)
{
   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_add_PrivArgs *pKerPrivArgs = (VXLIB_add_PrivArgs *) handle;

   // structs to retrieve SE and SA paramters
   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;
   __SA_TEMPLATE_v1 sa0Params;

   // create local pointers
   dTypeIn0 *restrict pInLocal0 = (dTypeIn0 *) pIn0;
   dTypeIn1 *restrict pInLocal1 = (dTypeIn1 *) pIn1;
   dTypeOut *restrict pOutLocal = (dTypeOut *) pOut;

#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_add_exec_ci\n");
#endif

   // address of SE and SA parameters
   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   // retrieve SE and SA parameters
   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET);
   se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET);
   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET);

   // open SEs to fetch Input samples
   __SE0_OPEN(pInLocal0, se0Params);
   __SE1_OPEN(pInLocal1, se1Params);

   // open SA0 to write output samples
   __SA0_OPEN(sa0Params);

   VXLIB_OVERFLOW_POLICY overFlowPolicy = pKerPrivArgs->pKerInitArgs.overFlowPolicy;
   size_t                numBlocks      = pKerPrivArgs->numBlocks;

   // call appropriate compute loop based on overflow policy
   if (overFlowPolicy == VXLIB_SATURATE) {
      VXLIB_add_util_sat<dTypeOut>(pOutLocal, numBlocks);
   }
   else {
      VXLIB_add_util_wrap<dTypeOut>(pOutLocal, numBlocks);
   }

   // close SE/SA
   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA0_CLOSE();

   return VXLIB_SUCCESS;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* Explicit instantiations for VXLIB_add_exec_ci                                                                      */
/*                                                                                                                    */
/**********************************************************************************************************************/

template VXLIB_STATUS VXLIB_add_exec_ci<VXLIB_ADD_TYPENAME_I8U_I8U_O8U>(VXLIB_kernelHandle handle,
                                                                        void *restrict pIn0,
                                                                        void *restrict pIn1,
                                                                        void *restrict pOut);

template VXLIB_STATUS VXLIB_add_exec_ci<VXLIB_ADD_TYPENAME_I8U_I8U_O16S>(VXLIB_kernelHandle handle,
                                                                         void *restrict pIn0,
                                                                         void *restrict pIn1,
                                                                         void *restrict pOut);

template VXLIB_STATUS VXLIB_add_exec_ci<VXLIB_ADD_TYPENAME_I8U_I16S_O16S>(VXLIB_kernelHandle handle,
                                                                          void *restrict pIn0,
                                                                          void *restrict pIn1,
                                                                          void *restrict pOut);

template VXLIB_STATUS VXLIB_add_exec_ci<VXLIB_ADD_TYPENAME_I16S_I16S_O16S>(VXLIB_kernelHandle handle,
                                                                           void *restrict pIn0,
                                                                           void *restrict pIn1,
                                                                           void *restrict pOut);
