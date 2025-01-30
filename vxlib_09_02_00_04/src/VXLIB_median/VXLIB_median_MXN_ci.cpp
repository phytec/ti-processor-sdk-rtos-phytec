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
#include "VXLIB_median_MXN_unroll.h"
#include "VXLIB_median_priv.h"

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_median_MXN_init_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method initializes the kernel-specific parameters
// mainly, the streaming engine and streaming address generators
template <typename dType>
VXLIB_STATUS VXLIB_median_MXN_init_ci(VXLIB_kernelHandle           handle,
                                      const VXLIB_bufParams2D_t   *bufParamsIn,
                                      const VXLIB_bufParams2D_t   *bufParamsMask,
                                      const VXLIB_bufParams2D_t   *bufParamsOut,
                                      const VXLIB_bufParams2D_t   *bufParamsScratch,
                                      const VXLIB_median_InitArgs *pKerInitArgs)
{
   VXLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");
   VXLIB_STATUS status = VXLIB_SUCCESS; // assign status to success by default

   VXLIB_median_PrivArgs *pKerPrivArgs = (VXLIB_median_PrivArgs *) handle;
   uint8_t               *pBlock       = pKerPrivArgs->bufPblock;

   typedef typename c7x::make_full_vector<dType>::type vec;

   // obtain image parameters and overflow policy
   size_t   width                 = pKerPrivArgs->width;
   size_t   widthOut              = pKerPrivArgs->widthOut;
   size_t   heightOut             = pKerPrivArgs->heightOut;
   size_t   strideInElements      = pKerPrivArgs->strideInElements;
   size_t   strideOutElements     = pKerPrivArgs->strideOutElements;
   size_t   strideMaskElements    = pKerPrivArgs->strideMaskElements;
   size_t   strideScratchElements = pKerPrivArgs->strideScratchElements;
   size_t   M                     = pKerPrivArgs->M;
   size_t   N                     = pKerPrivArgs->N;
   uint32_t eleCount              = c7x::element_count_of<vec>::value;
   uint32_t colBlocks             = (width + eleCount - 1) / eleCount;
   uint32_t unrollFactor          = 0;
   if (width <= eleCount) {
      unrollFactor = 1;
   }
   else if (width <= 3 * eleCount) {
      unrollFactor = 2;
   }
   else if (width <= 6 * eleCount) {
      unrollFactor = 4;
   }
   else {
      unrollFactor = 8;
   }

   __SE_ELETYPE SE_ELETYPE = c7x::se_eletype<vec>::value;
   __SE_VECLEN  SE_VECLEN  = c7x::se_veclen<vec>::value;
   __SA_VECLEN  SA_VECLEN  = c7x::sa_veclen<vec>::value;

   // structs to hold SE and SA parameters
   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1();
   se0Params.ICNT0            = eleCount * colBlocks;
   se0Params.ICNT1            = N;
   se0Params.DIM1             = 1;
   se0Params.ICNT2            = M;
   se0Params.DIM2             = strideInElements;
   se0Params.DIMFMT           = __SE_DIMFMT_3D;
   se0Params.ELETYPE          = SE_ELETYPE;
   se0Params.VECLEN           = SE_VECLEN;
   se0Params.DECDIM1          = __SE_DECDIM_DIM1;
   se0Params.DECDIM1_WIDTH    = width;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET) = se0Params;

   __SE_TEMPLATE_v1 se1Params = __gen_SE_TEMPLATE_v1();
   se1Params.ICNT0            = 1;
   se1Params.ICNT1            = colBlocks;
   se1Params.DIM1             = 0;
   se1Params.ICNT2            = N;
   se1Params.DIM2             = 1;
   se1Params.ICNT3            = M;
   se1Params.DIM3             = strideMaskElements;
   se1Params.DIMFMT           = __SE_DIMFMT_4D;
   se1Params.ELETYPE          = SE_ELETYPE;
   se1Params.VECLEN           = __SE_VECLEN_1ELEM;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET) = se1Params;

   __SA_TEMPLATE_v1 sa0Params = __gen_SA_TEMPLATE_v1();
   //  sa0Params.ICNT0            = eleCount * colBlocks;
   sa0Params.ICNT0  = width;
   sa0Params.ICNT1  = M * N;
   sa0Params.DIM1   = strideScratchElements;
   sa0Params.DIMFMT = __SA_DIMFMT_2D;
   sa0Params.VECLEN = SA_VECLEN;

   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET) = sa0Params;

   int32_t  count        = M * N * unrollFactor;
   int32_t  se0SortCount = (((count) % 2) == 0) ? (count) / 2 : ((count) / 2) + 1;
   int32_t  se1SortCount = count / 2;
   uint32_t unrollBlocks = (colBlocks + unrollFactor - 1) / unrollFactor;

   __SE_TEMPLATE_v1 se0SortParams = __gen_SE_TEMPLATE_v1();
   se0SortParams.ICNT0            = eleCount * unrollFactor;
   se0SortParams.ICNT1            = se0SortCount;
   se0SortParams.DIM1             = 2 * strideScratchElements;
   se0SortParams.ICNT2            = unrollBlocks;
   se0SortParams.DIM2             = eleCount * unrollFactor;
   se0SortParams.DIMFMT           = __SE_DIMFMT_3D;
   se0SortParams.ELETYPE          = SE_ELETYPE;
   se0SortParams.VECLEN           = SE_VECLEN;
   se0SortParams.DECDIM1          = __SE_DECDIM_DIM2;
   se0SortParams.DECDIM1SD        = __SE_DECDIMSD_DIM0;
   se0SortParams.DECDIM1_WIDTH    = eleCount * colBlocks;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_1_PARAM_OFFSET) = se0SortParams;

   __SE_TEMPLATE_v1 se1SortParams = __gen_SE_TEMPLATE_v1();
   se1SortParams.ICNT0            = eleCount * unrollFactor;
   se1SortParams.ICNT1            = se1SortCount;
   se1SortParams.DIM1             = 2 * strideScratchElements;
   se1SortParams.ICNT2            = unrollBlocks;
   se1SortParams.DIM2             = eleCount * unrollFactor;
   se1SortParams.DIMFMT           = __SE_DIMFMT_3D;
   se1SortParams.ELETYPE          = SE_ELETYPE;
   se1SortParams.VECLEN           = SE_VECLEN;
   se1SortParams.DECDIM1          = __SE_DECDIM_DIM2;
   se1SortParams.DECDIM1SD        = __SE_DECDIMSD_DIM0;
   se1SortParams.DECDIM1_WIDTH    = eleCount * colBlocks;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_1_PARAM_OFFSET) = se1SortParams;

   __SA_TEMPLATE_v1 sa1SortParams = __gen_SA_TEMPLATE_v1();
   sa1SortParams.ICNT0            = eleCount * unrollFactor;
   sa1SortParams.ICNT1            = count;
   sa1SortParams.DIM1             = strideScratchElements;
   sa1SortParams.ICNT2            = unrollBlocks;
   sa1SortParams.DIM2             = eleCount * unrollFactor;
   sa1SortParams.DIMFMT           = __SA_DIMFMT_3D;
   sa1SortParams.VECLEN           = SA_VECLEN;
   sa1SortParams.DECDIM1          = __SA_DECDIM_DIM2;
   sa1SortParams.DECDIM1SD        = __SA_DECDIMSD_DIM0;
   sa1SortParams.DECDIM1_WIDTH    = eleCount * colBlocks;

   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA1_1_PARAM_OFFSET) = sa1SortParams;

   __SA_TEMPLATE_v1 sa2StoreParams = __gen_SA_TEMPLATE_v1();
   sa2StoreParams.ICNT0            = widthOut;
   sa2StoreParams.ICNT1            = heightOut;
   sa2StoreParams.DIM1             = strideOutElements;
   sa2StoreParams.DIMFMT           = __SA_DIMFMT_2D;
   sa2StoreParams.VECLEN           = SA_VECLEN;

   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA2_1_PARAM_OFFSET) = sa2StoreParams;

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* Explicit instantiations for VXLIB_median_MXN_init_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

template VXLIB_STATUS VXLIB_median_MXN_init_ci<VXLIB_MEDIAN_TYPENAME_8U>(VXLIB_kernelHandle           handle,
                                                                         const VXLIB_bufParams2D_t   *bufParamsIn,
                                                                         const VXLIB_bufParams2D_t   *bufParamsMask,
                                                                         const VXLIB_bufParams2D_t   *bufParamsOut,
                                                                         const VXLIB_bufParams2D_t   *bufParamsScratch,
                                                                         const VXLIB_median_InitArgs *pKerInitArgs);

template VXLIB_STATUS VXLIB_median_MXN_init_ci<VXLIB_MEDIAN_TYPENAME_8S>(VXLIB_kernelHandle           handle,
                                                                         const VXLIB_bufParams2D_t   *bufParamsIn,
                                                                         const VXLIB_bufParams2D_t   *bufParamsMask,
                                                                         const VXLIB_bufParams2D_t   *bufParamsOut,
                                                                         const VXLIB_bufParams2D_t   *bufParamsScratch,
                                                                         const VXLIB_median_InitArgs *pKerInitArgs);

template VXLIB_STATUS VXLIB_median_MXN_init_ci<VXLIB_MEDIAN_TYPENAME_16U>(VXLIB_kernelHandle           handle,
                                                                          const VXLIB_bufParams2D_t   *bufParamsIn,
                                                                          const VXLIB_bufParams2D_t   *bufParamsMask,
                                                                          const VXLIB_bufParams2D_t   *bufParamsOut,
                                                                          const VXLIB_bufParams2D_t   *bufParamsScratch,
                                                                          const VXLIB_median_InitArgs *pKerInitArgs);

template VXLIB_STATUS VXLIB_median_MXN_init_ci<VXLIB_MEDIAN_TYPENAME_16S>(VXLIB_kernelHandle           handle,
                                                                          const VXLIB_bufParams2D_t   *bufParamsIn,
                                                                          const VXLIB_bufParams2D_t   *bufParamsMask,
                                                                          const VXLIB_bufParams2D_t   *bufParamsOut,
                                                                          const VXLIB_bufParams2D_t   *bufParamsScratch,
                                                                          const VXLIB_median_InitArgs *pKerInitArgs);
// template VXLIB_STATUS VXLIB_median_MXN_init_ci<VXLIB_MEDIAN_TYPENAME_64S>(VXLIB_kernelHandle           handle,
//                                                                           const VXLIB_bufParams2D_t   *bufParamsIn,
//                                                                           const VXLIB_bufParams2D_t   *bufParamsMask,
//                                                                           const VXLIB_bufParams2D_t   *bufParamsOut,
//                                                                           const VXLIB_bufParams2D_t   *bufParamsScratch,
//                                                                           const VXLIB_median_InitArgs *pKerInitArgs);

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_median_MXN_exec_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

#define ELE_CNT(x) c7x::element_count_of<x>::value

template <typename dataType> inline dataType maskVal();

template <> inline uint8_t maskVal() { return 0xFF; }

template <> inline uint16_t maskVal() { return 0xFFFF; }

template <typename dType>
VXLIB_STATUS VXLIB_median_MXN_exec_ci(VXLIB_kernelHandle handle,
                                      void *restrict pIn,
                                      void *restrict pMask,
                                      void *restrict pOut,
                                      void *restrict pScratch)
{
   VXLIB_STATUS status = VXLIB_SUCCESS; // assign status to success by default

#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_median_MXN_exec_ci\n");
#endif

   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_median_PrivArgs *pKerPrivArgs = (VXLIB_median_PrivArgs *) handle;

   //  bool kernelType = pKerPrivArgs->pKerInitArgs.kernelType;

   typedef typename c7x::make_full_vector<dType>::type vec;

   typedef typename std::conditional<ELE_CNT(c7x::uchar_vec) == ELE_CNT(vec), uint8_t, uint16_t>::type unsignedType;

   uint32_t eleCount           = c7x::element_count_of<vec>::value;
   uint32_t widthIn            = pKerPrivArgs->width;
   int32_t  M                  = pKerPrivArgs->M;
   int32_t  N                  = pKerPrivArgs->N;
   int32_t  strideMaskElements = pKerPrivArgs->strideMaskElements;
   dType *restrict pMaskLocal  = (dType *) pMask;
   int32_t trueCount           = 0;

   for (int32_t i = 0; i < M; i++) {
      for (int32_t j = 0; j < N; j++) {
         if ((unsignedType) pMaskLocal[j + i * strideMaskElements] == maskVal<unsignedType>()) {
            trueCount++;
         }
      }
   }

   if (trueCount > 0) {
      if (widthIn <= eleCount) {
         VXLIB_median_MXN<dType>(handle, pIn, pMask, pOut, pScratch, trueCount);
      }
      else if (widthIn <= 3 * eleCount) {
         VXLIB_median_MXN_UNROLL2<dType>(handle, pIn, pMask, pOut, pScratch, trueCount);
      }
      else if (widthIn <= 6 * eleCount) {
         VXLIB_median_MXN_UNROLL4<dType>(handle, pIn, pMask, pOut, pScratch, trueCount);
      }
      else {
         VXLIB_median_MXN_UNROLL8<dType>(handle, pIn, pMask, pOut, pScratch, trueCount);
      }
   }
   else {
      status = VXLIB_ERR_FAILURE;
      VXLIB_DEBUGPRINTFN(0, "ERROR CI: True Count = %d. Cannot execute kernel\n", trueCount);
   }

   return (status);
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* Explicit instantiations for VXLIB_median_MXN_exec_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

template VXLIB_STATUS VXLIB_median_MXN_exec_ci<VXLIB_MEDIAN_TYPENAME_8U>(VXLIB_kernelHandle handle,
                                                                         void *restrict pIn,
                                                                         void *restrict pMask,
                                                                         void *restrict pOut,
                                                                         void *restrict pScratch);

template VXLIB_STATUS VXLIB_median_MXN_exec_ci<VXLIB_MEDIAN_TYPENAME_8S>(VXLIB_kernelHandle handle,
                                                                         void *restrict pIn,
                                                                         void *restrict pMask,
                                                                         void *restrict pOut,
                                                                         void *restrict pScratch);

template VXLIB_STATUS VXLIB_median_MXN_exec_ci<VXLIB_MEDIAN_TYPENAME_16U>(VXLIB_kernelHandle handle,
                                                                          void *restrict pIn,
                                                                          void *restrict pMask,
                                                                          void *restrict pOut,
                                                                          void *restrict pScratch);

template VXLIB_STATUS VXLIB_median_MXN_exec_ci<VXLIB_MEDIAN_TYPENAME_16S>(VXLIB_kernelHandle handle,
                                                                          void *restrict pIn,
                                                                          void *restrict pMask,
                                                                          void *restrict pOut,
                                                                          void *restrict pScratch);
// template VXLIB_STATUS VXLIB_median_MXN_exec_ci<VXLIB_MEDIAN_TYPENAME_64S>(VXLIB_kernelHandle handle,
//                                                                           void *restrict pIn,
//                                                                           void *restrict pMask,
//                                                                           void *restrict pOut,
//                                                                           void *restrict pScratch);

/* ======================================================================== */
/*  End of file:  VXLIB_median_MXN_ci.cpp                                     */
/* ======================================================================== */
