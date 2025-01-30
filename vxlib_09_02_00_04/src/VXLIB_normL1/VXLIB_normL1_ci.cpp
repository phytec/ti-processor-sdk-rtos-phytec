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

#include "VXLIB_normL1_priv.h"
#include "VXLIB_types.h"

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
/* VXLIB_normL1_init_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method initializes the kernel-specific parameters
// mainly, the streaming engine and streaming address generators
template <uint32_t dTypeIn, uint32_t dTypeOut>
VXLIB_STATUS VXLIB_normL1_init_ci(VXLIB_kernelHandle           handle,
                                  const VXLIB_bufParams2D_t *  bufParamsIn0,
                                  const VXLIB_bufParams2D_t *  bufParamsIn1,
                                  const VXLIB_bufParams2D_t *  bufParamsOut,
                                  const VXLIB_normL1_InitArgs *pKerInitArgs)
{
   VXLIB_STATUS status = VXLIB_SUCCESS; // assign status to success by default

   // structs to hold SE and SA parameters
   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1();
   __SE_TEMPLATE_v1 se1Params = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa0Params = __gen_SA_TEMPLATE_v1();

   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_normL1_PrivArgs *pKerPrivArgs = (VXLIB_normL1_PrivArgs *) handle;
   uint8_t *              pBlock       = pKerPrivArgs->bufPblock; // address to retrieve to store SE/SA params

   // set SE0, SE1, and SA0 params
   pKerPrivArgs->numBlocks = VXLIB_SE0SE1SA0_init<dTypeIn, dTypeIn, dTypeOut>(&se0Params, &se1Params, &sa0Params,
                                                                              bufParamsIn0, bufParamsIn1, bufParamsOut);

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
/* Explicit instantiations for VXLIB_normL1_init_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

template VXLIB_STATUS VXLIB_normL1_init_ci<VXLIB_NORML1_DTYPE_I16S_O16U>(VXLIB_kernelHandle           handle,
                                                                         const VXLIB_bufParams2D_t *  bufParamsIn0,
                                                                         const VXLIB_bufParams2D_t *  bufParamsIn1,
                                                                         const VXLIB_bufParams2D_t *  bufParamsOut,
                                                                         const VXLIB_normL1_InitArgs *pKerInitArgs);

template VXLIB_STATUS VXLIB_normL1_init_ci<VXLIB_NORML1_DTYPE_I8S_O8U>(VXLIB_kernelHandle           handle,
                                                                       const VXLIB_bufParams2D_t *  bufParamsIn0,
                                                                       const VXLIB_bufParams2D_t *  bufParamsIn1,
                                                                       const VXLIB_bufParams2D_t *  bufParamsOut,
                                                                       const VXLIB_normL1_InitArgs *pKerInitArgs);
/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_normL1_exec_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method performs pixel-wise bitwise normL1 of two input images
template <typename dTypeIn, typename dTypeOut>
VXLIB_STATUS
VXLIB_normL1_exec_ci(VXLIB_kernelHandle handle, void *restrict pIn0, void *restrict pIn1, void *restrict pOut)
{
   VXLIB_STATUS status = VXLIB_SUCCESS; // assign status to success by default
   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_normL1_PrivArgs *pKerPrivArgs = (VXLIB_normL1_PrivArgs *) handle;

   // structs to retrieve SE and SA paramters
   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;
   __SA_TEMPLATE_v1 sa0Params;

   // create local pointers
   dTypeIn *restrict  pInLocal0 = (dTypeIn *) pIn0;
   dTypeIn *restrict  pInLocal1 = (dTypeIn *) pIn1;
   dTypeOut *restrict pOutLocal = (dTypeOut *) pOut;

#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_normL1_exec_ci\n");
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

   size_t numBlocks = pKerPrivArgs->numBlocks;

   typedef typename c7x::make_full_vector<dTypeIn>::type  inVec;
   typedef typename c7x::make_full_vector<dTypeOut>::type outVec;

   for (uint32_t counter = 0; counter < numBlocks; counter++) {

      inVec val0 = c7x::strm_eng<0, inVec>::get_adv(); // fetch input 0 via SE0
      inVec val1 = c7x::strm_eng<1, inVec>::get_adv(); // fetch input 1 via SE1

      val0 = __abs(val0);
      val1 = __abs(val1);

      outVec out = c7x::reinterpret<outVec>(val0) + c7x::reinterpret<outVec>(val1);

      // store output via SA0
      __vpred tmp  = c7x::strm_agen<0, outVec>::get_vpred();        // generate predicate
      outVec *addr = c7x::strm_agen<0, outVec>::get_adv(pOutLocal); // generate address
      __vstore_pred(tmp, addr, out);                                // store value
   }

   // close SE/SA
   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA0_CLOSE();

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* Explicit instantiations for VXLIB_normL1_exec_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

template VXLIB_STATUS VXLIB_normL1_exec_ci<VXLIB_NORML1_TYPENAME_I8S_O8U>(VXLIB_kernelHandle handle,
                                                                          void *restrict     pIn0,
                                                                          void *restrict     pIn1,
                                                                          void *restrict     pOut);

template VXLIB_STATUS VXLIB_normL1_exec_ci<VXLIB_NORML1_TYPENAME_I16S_O16U>(VXLIB_kernelHandle handle,
                                                                            void *restrict     pIn0,
                                                                            void *restrict     pIn1,
                                                                            void *restrict     pOut);

/**********************************************************************************************************************/
/*                                                                                                                    */
/* Performance estimation function */
/*                                                                                                                    */
/**********************************************************************************************************************/

void VXLIB_normL1_perfEst(VXLIB_kernelHandle handle, size_t *archCycles, size_t *estCycles)
{

   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_normL1_PrivArgs *pKerPrivArgs = (VXLIB_normL1_PrivArgs *) handle;

   // obtain loop count for compute loop
   size_t numBlocks = pKerPrivArgs->numBlocks;
   size_t overHead  = 56;                // profiled code before entering compute loop
   *archCycles      = 3 + numBlocks * 1; // obtained from asm
   *estCycles       = overHead + *archCycles;
}

/* ======================================================================== */
/*  End of file:  VXLIB_normL1_ci.cpp                                       */
/* ======================================================================== */
