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

#include "VXLIB_subtract_priv.h"

#define SE_PARAM_BASE (0x0000)
#define SE0_PARAM_OFFSET (SE_PARAM_BASE)
#define SE1_PARAM_OFFSET (SE0_PARAM_OFFSET + VXLIB_SE_PARAM_SIZE)
#define SA0_PARAM_OFFSET (SE1_PARAM_OFFSET + VXLIB_SE_PARAM_SIZE)

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_subtract_init_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method initializes the kernel-specific parameters
// mainly, the streaming engine and streaming address generators
template <uint32_t dTypeIn0, uint32_t dTypeIn1, uint32_t dTypeOut>
VXLIB_STATUS VXLIB_subtract_init_ci(VXLIB_kernelHandle             handle,
                                    const VXLIB_bufParams2D_t *    bufParamsIn0,
                                    const VXLIB_bufParams2D_t *    bufParamsIn1,
                                    const VXLIB_bufParams2D_t *    bufParamsOut,
                                    const VXLIB_subtract_InitArgs *pKerInitArgs)
{
   VXLIB_DEBUGPRINTFN(0, "%s\n", "Enter function");
   VXLIB_STATUS status = VXLIB_SUCCESS; // assign status to success by default

   // structs to hold SE and SA parameters
   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1();
   __SE_TEMPLATE_v1 se1Params = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa0Params = __gen_SA_TEMPLATE_v1();

   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_subtract_PrivArgs *pKerPrivArgs = (VXLIB_subtract_PrivArgs *) handle;
   uint8_t *                pBlock       = pKerPrivArgs->bufPblock; // address to retrieve to store SE/SA params

   // set SE0, SE1, and SA0 params
   pKerPrivArgs->numBlocks = VXLIB_SE0SE1SA0_init<dTypeIn0, dTypeIn1, dTypeOut>(
       &se0Params, &se1Params, &sa0Params, bufParamsIn0, bufParamsIn1, bufParamsOut);

   /**************************/
   /* Store SE and SA params */
   /**************************/

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET) = se0Params;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET) = se1Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET) = sa0Params;

   VXLIB_DEBUGPRINTFN(0, "%s\n", "Exit function");

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* Explicit instantiations for VXLIB_subtract_init_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

template VXLIB_STATUS
VXLIB_subtract_init_ci<VXLIB_SUBTRACT_DTYPE_I8U_I8U_O8U>(VXLIB_kernelHandle             handle,
                                                         const VXLIB_bufParams2D_t *    bufParamsIn0,
                                                         const VXLIB_bufParams2D_t *    bufParamsIn1,
                                                         const VXLIB_bufParams2D_t *    bufParamsOut,
                                                         const VXLIB_subtract_InitArgs *pKerInitArgs);

template VXLIB_STATUS
VXLIB_subtract_init_ci<VXLIB_SUBTRACT_DTYPE_I8U_I8U_O16S>(VXLIB_kernelHandle             handle,
                                                          const VXLIB_bufParams2D_t *    bufParamsIn0,
                                                          const VXLIB_bufParams2D_t *    bufParamsIn1,
                                                          const VXLIB_bufParams2D_t *    bufParamsOut,
                                                          const VXLIB_subtract_InitArgs *pKerInitArgs);

template VXLIB_STATUS
VXLIB_subtract_init_ci<VXLIB_SUBTRACT_DTYPE_I8U_I16S_O16S>(VXLIB_kernelHandle             handle,
                                                           const VXLIB_bufParams2D_t *    bufParamsIn0,
                                                           const VXLIB_bufParams2D_t *    bufParamsIn1,
                                                           const VXLIB_bufParams2D_t *    bufParamsOut,
                                                           const VXLIB_subtract_InitArgs *pKerInitArgs);

template VXLIB_STATUS
VXLIB_subtract_init_ci<VXLIB_SUBTRACT_DTYPE_I16S_I16S_O16S>(VXLIB_kernelHandle             handle,
                                                            const VXLIB_bufParams2D_t *    bufParamsIn0,
                                                            const VXLIB_bufParams2D_t *    bufParamsIn1,
                                                            const VXLIB_bufParams2D_t *    bufParamsOut,
                                                            const VXLIB_subtract_InitArgs *pKerInitArgs);

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_subtract_util_sat */
/*                                                                                                                    */
/**********************************************************************************************************************/

template <typename V> static inline V subtractSaturate(V vecIn1, V vecIn2, c7x::uchar_vec zeroVec);

template <>
inline c7x::short_vec
subtractSaturate<c7x::short_vec>(c7x::short_vec vecIn1, c7x::short_vec vecIn2, c7x::uchar_vec zeroVec)
{
   return __sub_sat(vecIn1, vecIn2);
}

template <>
inline c7x::uchar_vec
subtractSaturate<c7x::uchar_vec>(c7x::uchar_vec vecIn1, c7x::uchar_vec vecIn2, c7x::uchar_vec zeroVec)
{
   c7x::uchar_vec vecSub   = vecIn1 - vecIn2;
   __vpred        predComp = __cmp_gt_pred(vecIn1, vecIn2);
   return __select(predComp, vecSub, zeroVec);
}

// this method subtracts two inputs, saturates the result, and writes the result to output buffer
// it is assumed that SEs and SA are open before calling this method

template <typename dType> static inline void VXLIB_subtract_util_sat(dType *pOut, size_t size)
{
   VXLIB_DEBUGPRINTFN(0, "%s\n", "Enter function");

   typedef typename c7x::make_full_vector<dType>::type vec;

   dType *        pOutLocal = (dType *) pOut;
   c7x::uchar_vec zeroVec   = (c7x::uchar_vec) 0;

   for (uint32_t counter = 0; counter < size; counter++) {
      vec a = c7x::strm_eng<0, vec>::get_adv(); // fetch input 0 via SE0
      vec b = c7x::strm_eng<1, vec>::get_adv(); // fetch input 1 via SE1

      vec out = subtractSaturate<vec>(a, b, zeroVec);

      // store output via SA0
      __vpred tmp  = c7x::strm_agen<0, vec>::get_vpred();        // generate predicate
      vec *   addr = c7x::strm_agen<0, vec>::get_adv(pOutLocal); // generate address
      __vstore_pred(tmp, addr, out);                             // store value
   }
   VXLIB_DEBUGPRINTFN(0, "%s\n", "Exit function");

   return;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_subtract_util_wrap */
/*                                                                                                                    */
/**********************************************************************************************************************/

template <typename dType> static inline void VXLIB_subtract_util_wrap(dType *pOut, size_t size)
{
   VXLIB_DEBUGPRINTFN(0, "%s\n", "Enter function");
   // derive c7x vector type from template typename
   typedef typename c7x::make_full_vector<dType>::type vec;

   for (uint32_t counter = 0; counter < size; counter++) {
      vec a = c7x::strm_eng<0, vec>::get_adv(); // fetch input 0 via SE0
      vec b = c7x::strm_eng<1, vec>::get_adv(); // fetch input 1 via SE1

      vec out = a - b;

      // store output via SA0
      __vpred tmp  = c7x::strm_agen<0, vec>::get_vpred();   // generate predicate
      vec *   addr = c7x::strm_agen<0, vec>::get_adv(pOut); // generate address
      __vstore_pred(tmp, addr, out);                        // store value
   }
   VXLIB_DEBUGPRINTFN(0, "%s\n", "Exit function");

   return;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_subtract_exec_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method performs pixel-wise addition of two input images
template <typename dTypeIn0, typename dTypeIn1, typename dTypeOut>
VXLIB_STATUS
VXLIB_subtract_exec_ci(VXLIB_kernelHandle handle, void *restrict pIn0, void *restrict pIn1, void *restrict pOut)
{
   VXLIB_DEBUGPRINTFN(0, "%s\n", "Enter function");
   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_subtract_PrivArgs *pKerPrivArgs = (VXLIB_subtract_PrivArgs *) handle;

   // structs to retrieve SE and SA paramters
   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;
   __SA_TEMPLATE_v1 sa0Params;

   // create local pointers
   dTypeIn0 *restrict pInLocal0 = (dTypeIn0 *) pIn0;
   dTypeIn1 *restrict pInLocal1 = (dTypeIn1 *) pIn1;
   dTypeOut *restrict pOutLocal = (dTypeOut *) pOut;

   // address of SE and SA parameters
   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   // retrieve SE and SA parameters
   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET);
   se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET);
   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET);

   // open SEs to fetch Input samples
   uint8_t subtractPolicy = pKerPrivArgs->pKerInitArgs.subtractPolicy;

   if (subtractPolicy == 0) {
      __SE0_OPEN(pInLocal0, se0Params);
      __SE1_OPEN(pInLocal1, se1Params);
   }
   else {
      __SE0_OPEN(pInLocal1, se1Params);
      __SE1_OPEN(pInLocal0, se0Params);
   }

   // open SA0 to write output samples
   __SA0_OPEN(sa0Params);

   VXLIB_OVERFLOW_POLICY overFlowPolicy = pKerPrivArgs->pKerInitArgs.overFlowPolicy;
   size_t                numBlocks      = pKerPrivArgs->numBlocks;

   //  call appropriate compute loop based on overflow policy
   if (overFlowPolicy == VXLIB_SATURATE) {
      VXLIB_subtract_util_sat<dTypeOut>(pOutLocal, numBlocks);
   }
   else {
      VXLIB_subtract_util_wrap<dTypeOut>(pOutLocal, numBlocks);
   }

   // close SE/SA
   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA0_CLOSE();
   VXLIB_DEBUGPRINTFN(0, "%s\n", "Exit function");

   return VXLIB_SUCCESS;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* Explicit instantiations for VXLIB_subtract_exec_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

template VXLIB_STATUS VXLIB_subtract_exec_ci<VXLIB_SUBTRACT_TYPENAME_I8U_I8U_O8U>(VXLIB_kernelHandle handle,
                                                                                  void *restrict     pIn0,
                                                                                  void *restrict     pIn1,
                                                                                  void *restrict     pOut);

template VXLIB_STATUS VXLIB_subtract_exec_ci<VXLIB_SUBTRACT_TYPENAME_I8U_I8U_O16S>(VXLIB_kernelHandle handle,
                                                                                   void *restrict     pIn0,
                                                                                   void *restrict     pIn1,
                                                                                   void *restrict     pOut);

template VXLIB_STATUS VXLIB_subtract_exec_ci<VXLIB_SUBTRACT_TYPENAME_I8U_I16S_O16S>(VXLIB_kernelHandle handle,
                                                                                    void *restrict     pIn0,
                                                                                    void *restrict     pIn1,
                                                                                    void *restrict     pOut);

template VXLIB_STATUS VXLIB_subtract_exec_ci<VXLIB_SUBTRACT_TYPENAME_I16S_I16S_O16S>(VXLIB_kernelHandle handle,
                                                                                     void *restrict     pIn0,
                                                                                     void *restrict     pIn1,
                                                                                     void *restrict     pOut);



/**********************************************************************************************************************/
/*                                                                                                                    */
/* Performance estimation function */
/*                                                                                                                    */
/**********************************************************************************************************************/

void VXLIB_subtract_perfEst(VXLIB_kernelHandle handle, size_t *archCycles, size_t *estCycles)
{

   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_subtract_PrivArgs *pKerPrivArgs = (VXLIB_subtract_PrivArgs *) handle;

   // obtain loop count for compute loop
   size_t numBlocks   = pKerPrivArgs->numBlocks;
   size_t overheadCnt = 56;                // profiled code before entering compute loop
   *archCycles        = 1 + numBlocks * 1; // obtained from asm
   *estCycles         = overheadCnt + *archCycles;
}

/* ======================================================================== */
/*  End of file:  VXLIB_subtract_ci.cpp                                     */
/* ======================================================================== */
