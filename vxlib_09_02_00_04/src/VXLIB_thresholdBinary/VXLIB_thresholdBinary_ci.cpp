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

#include "VXLIB_thresholdBinary_priv.h"

/**********************************************************************************************************************/
/*                                                                                                                    */
/* DEFINES                                                                                                            */
/*                                                                                                                    */
/**********************************************************************************************************************/

#define SE_PARAM_BASE (0x0000)
#define SE0_PARAM_OFFSET (SE_PARAM_BASE)
#define SA0_PARAM_OFFSET (SE0_PARAM_OFFSET + VXLIB_SE_PARAM_SIZE)

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_thresholdBinary_init_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method initializes the kernel-specific parameters
// mainly, the streaming engine and streaming address generators
template <uint32_t dTypeIn, uint32_t dTypeOut>
VXLIB_STATUS VXLIB_thresholdBinary_init_ci(VXLIB_kernelHandle                    handle,
                                           const VXLIB_bufParams2D_t *           bufParamsIn,
                                           const VXLIB_bufParams2D_t *           bufParamsOut,
                                           const VXLIB_thresholdBinary_InitArgs *pKerInitArgs)
{
   VXLIB_STATUS status = VXLIB_SUCCESS; // assign status to success by default

   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_thresholdBinary_PrivArgs *pKerPrivArgs = (VXLIB_thresholdBinary_PrivArgs *) handle;

   // structs to hold SE and SA parameters
   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa0Params = __gen_SA_TEMPLATE_v1();

   uint8_t *pBlock = pKerPrivArgs->bufPblock; // address to retrieve to store SE/SA params

   //  set SE0, and SA0 params
   pKerPrivArgs->numBlocks = VXLIB_SE0SA0_init<dTypeIn, dTypeIn>(&se0Params, &sa0Params, bufParamsIn, bufParamsOut);
   /**************************/
   /* Store SE and SA params */
   /**************************/
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET) = se0Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET) = sa0Params;

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* Explicit instantiations for VXLIB_thresholdBinary_init_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

template VXLIB_STATUS
VXLIB_thresholdBinary_init_ci<VXLIB_THRESHOLDBINARY_DTYPE_I8U_O8U>(VXLIB_kernelHandle                    handle,
                                                                   const VXLIB_bufParams2D_t *           bufParamsIn,
                                                                   const VXLIB_bufParams2D_t *           bufParamsOut,
                                                                   const VXLIB_thresholdBinary_InitArgs *pKerInitArgs);

template VXLIB_STATUS
VXLIB_thresholdBinary_init_ci<VXLIB_THRESHOLDBINARY_DTYPE_I8S_O8S>(VXLIB_kernelHandle                    handle,
                                                                   const VXLIB_bufParams2D_t *           bufParamsIn,
                                                                   const VXLIB_bufParams2D_t *           bufParamsOut,
                                                                   const VXLIB_thresholdBinary_InitArgs *pKerInitArgs);

template VXLIB_STATUS VXLIB_thresholdBinary_init_ci<VXLIB_THRESHOLDBINARY_DTYPE_I16U_O16U>(
    VXLIB_kernelHandle                    handle,
    const VXLIB_bufParams2D_t *           bufParamsIn,
    const VXLIB_bufParams2D_t *           bufParamsOut,
    const VXLIB_thresholdBinary_InitArgs *pKerInitArgs);

template VXLIB_STATUS VXLIB_thresholdBinary_init_ci<VXLIB_THRESHOLDBINARY_DTYPE_I16S_O16S>(
    VXLIB_kernelHandle                    handle,
    const VXLIB_bufParams2D_t *           bufParamsIn,
    const VXLIB_bufParams2D_t *           bufParamsOut,
    const VXLIB_thresholdBinary_InitArgs *pKerInitArgs);

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_thresholdBinary_exec_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method performs pixel-wise addition of two input images
template <typename dTypeIn, typename dTypeOut>
VXLIB_STATUS VXLIB_thresholdBinary_exec_ci(VXLIB_kernelHandle handle,
                                           void *restrict     pIn,
                                           void *restrict     pOut,
                                           void *restrict     pThresholdVal,
                                           void *restrict     pTrueVal,
                                           void *restrict     pFalseVal)
{

   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_thresholdBinary_PrivArgs *pKerPrivArgs = (VXLIB_thresholdBinary_PrivArgs *) handle;
   size_t                          size         = pKerPrivArgs->numBlocks;

   // create local pointers
   dTypeIn *restrict  pInLocal  = (dTypeIn *) pIn;
   dTypeOut *restrict pOutLocal = (dTypeOut *) pOut;
   // create local pointers
   dTypeIn *restrict pThresholdValLocal = (dTypeIn *) pThresholdVal;
   dTypeIn *restrict pTrueValLocal      = (dTypeIn *) pTrueVal;
   dTypeIn *restrict pFalseValLocal     = (dTypeIn *) pFalseVal;

#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_Thresholdbinary_exec_ci\n");
#endif

   // address of SE and SA parameters
   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   // retrieve SE and SA parameters
   __SE_TEMPLATE_v1 se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET);
   __SA_TEMPLATE_v1 sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET);

   __SE0_OPEN(pInLocal, se0Params);
   // open SA0 to write output samples
   __SA0_OPEN(sa0Params);

   // derive c7x vector type from template typename
   typedef typename c7x::make_full_vector<dTypeIn>::type vec;

   vec vThreshold = (vec) *pThresholdValLocal;
   vec vTrue      = (vec) *pTrueValLocal;
   vec vFalse     = (vec) *pFalseValLocal;

   for (uint32_t counter = 0; counter < size; counter++) {

      vec a = c7x::strm_eng<0, vec>::get_adv(); // fetch input 0 via SE0

      __vpred vPred = __cmp_gt_pred(a, vThreshold);
      vec     vout  = __select(vPred, vTrue, vFalse);

      // store output via SA0
      __vpred tmp  = c7x::strm_agen<0, vec>::get_vpred();        // generate predicate
      vec *   addr = c7x::strm_agen<0, vec>::get_adv(pOutLocal); // generate address
      __vstore_pred(tmp, addr, vout);                            // store value
   }
   // close SE/SA
   __SE0_CLOSE();
   __SA0_CLOSE();

   return VXLIB_SUCCESS;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* Explicit instantiations for VXLIB_thresholdBinary_exec_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

template VXLIB_STATUS
VXLIB_thresholdBinary_exec_ci<VXLIB_THRESHOLDBINARY_TYPENAME_I8U_O8U>(VXLIB_kernelHandle handle,
                                                                      void *restrict     pIn,
                                                                      void *restrict     pOut,
                                                                      void *restrict     pThresholdVal,
                                                                      void *restrict     pTrueVal,
                                                                      void *restrict     pFalseVal);

template VXLIB_STATUS
VXLIB_thresholdBinary_exec_ci<VXLIB_THRESHOLDBINARY_TYPENAME_I8S_O8S>(VXLIB_kernelHandle handle,
                                                                      void *restrict     pIn,
                                                                      void *restrict     pOut,
                                                                      void *restrict     pThresholdVal,
                                                                      void *restrict     pTrueVal,
                                                                      void *restrict     pFalseVal);

template VXLIB_STATUS
VXLIB_thresholdBinary_exec_ci<VXLIB_THRESHOLDBINARY_TYPENAME_I16U_O16U>(VXLIB_kernelHandle handle,
                                                                        void *restrict     pIn,
                                                                        void *restrict     pOut,
                                                                        void *restrict     pThresholdVal,
                                                                        void *restrict     pTrueVal,
                                                                        void *restrict     pFalseVal);

template VXLIB_STATUS
VXLIB_thresholdBinary_exec_ci<VXLIB_THRESHOLDBINARY_TYPENAME_I16S_O16S>(VXLIB_kernelHandle handle,
                                                                        void *restrict     pIn,
                                                                        void *restrict     pOut,
                                                                        void *restrict     pThresholdVal,
                                                                        void *restrict     pTrueVal,
                                                                        void *restrict     pFalseVal);



/**********************************************************************************************************************/
/*                                                                                                                    */
/* Performance estimation function */
/*                                                                                                                    */
/**********************************************************************************************************************/


void VXLIB_thresholdBinary_perfEst(VXLIB_kernelHandle handle, size_t *archCycles, size_t *estCycles)
{

   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_thresholdBinary_PrivArgs *pKerPrivArgs = (VXLIB_thresholdBinary_PrivArgs *) handle;

   // obtain loop count for compute loop
   size_t numBlocks   = pKerPrivArgs->numBlocks;
   size_t overheadCnt = 44;                // profiled code before entering compute loop
   *archCycles        = 2 + numBlocks * 1; // obtained from asm
   *estCycles         = overheadCnt + *archCycles;
}

/* ======================================================================== */
/*  End of file:  VXLIB_thresholdBinary_ci.cpp                                     */
/* ======================================================================== */
