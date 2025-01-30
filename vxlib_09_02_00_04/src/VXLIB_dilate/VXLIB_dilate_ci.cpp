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

#include "VXLIB_dilate_priv.h"
#include "VXLIB_types.h"
#include "VXLIB_utility.h"
#include "c7x_scalable.h"
#include <type_traits>
/**********************************************************************************************************************/
/*                                                                                                                    */
/* DEFINES                                                                                                            */
/*                                                                                                                    */
/**********************************************************************************************************************/

#define SE_PARAM_BASE (0x0000)
#define SE0_PARAM_OFFSET (SE_PARAM_BASE)
#define SE1_PARAM_OFFSET (SE0_PARAM_OFFSET + VXLIB_SE_PARAM_SIZE)
#define SA0_PARAM_OFFSET (SE1_PARAM_OFFSET + VXLIB_SE_PARAM_SIZE)
#define SA1_PARAM_OFFSET (SA0_PARAM_OFFSET + VXLIB_SA_PARAM_SIZE)

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_dilate_init_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method initializes the kernel-specific parameters
// mainly, the streaming engine and streaming address generators
template <uint32_t dTypeIn, uint32_t dTypeOut>
VXLIB_STATUS VXLIB_dilate_init_ci(VXLIB_kernelHandle           handle,
                                  const VXLIB_bufParams2D_t   *bufParamsIn,
                                  const VXLIB_bufParams2D_t   *bufParamsOut,
                                  const VXLIB_dilate_InitArgs *pKerInitArgs)
{
   VXLIB_STATUS status = VXLIB_SUCCESS; // assign status to success by default

   // structs to hold SE and SA parameters
   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1();
   __SE_TEMPLATE_v1 se1Params = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa0Params = __gen_SA_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa1Params = __gen_SA_TEMPLATE_v1();

   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_dilate_PrivArgs *pKerPrivArgs = (VXLIB_dilate_PrivArgs *) handle;

   typedef typename std::conditional<dTypeIn == VXLIB_UINT8, uint8_t, uint16_t>::type elemType;

   typedef typename std::conditional<dTypeIn == VXLIB_UINT8, c7x::uchar_vec, c7x::ushort_vec>::type vecType;

   // number of elements fetched from input each SE get
   size_t elemCountIn = c7x::element_count_of<vecType>::value;

   // get init args
   size_t widthIn  = bufParamsIn->dim_x;
   size_t heightIn = bufParamsIn->dim_y;
   size_t strideIn = pKerPrivArgs->strideInElements;

   size_t widthOut  = bufParamsOut->dim_x;
   size_t heightOut = bufParamsOut->dim_y;
   size_t strideOut = pKerPrivArgs->strideOutElements;

   size_t filterWidth  = pKerInitArgs->filterWidth;
   size_t filterHeight = pKerInitArgs->filterHeight;

   uint8_t *pBlock         = pKerPrivArgs->bufPblock;
   pKerPrivArgs->numBlocks = widthIn * VXLIB_ceilingDiv(widthIn, elemCountIn);

   se0Params.DIMFMT  = __SE_DIMFMT_5D;
   se0Params.ELETYPE = c7x::se_eletype<vecType>::value;
   se0Params.VECLEN  = c7x::se_veclen<vecType>::value;

   se0Params.ICNT0 = elemCountIn;

   se0Params.DIM1  = 1;
   se0Params.ICNT1 = filterWidth;

   se0Params.DIM2  = strideIn;
   se0Params.ICNT2 = filterHeight;

   se0Params.DIM3  = strideIn * 2;
   se0Params.ICNT3 = VXLIB_ceilingDiv(heightOut, 2);

   se0Params.DIM4  = elemCountIn;
   se0Params.ICNT4 = VXLIB_ceilingDiv(widthIn, elemCountIn);

   se0Params.DECDIM1       = __SE_DECDIM_DIM4;
   se0Params.DECDIM1_WIDTH = widthIn;
   se0Params.DECDIM1SD     = __SE_DECDIMSD_DIM1;

   se0Params.DECDIM2       = __SE_DECDIM_DIM3;
   se0Params.DECDIM2_WIDTH = strideIn * heightIn;

   se1Params.DIMFMT  = __SE_DIMFMT_5D;
   se1Params.ELETYPE = c7x::se_eletype<vecType>::value;
   se1Params.VECLEN  = c7x::se_veclen<vecType>::value;

   se1Params.ICNT0 = elemCountIn;

   se1Params.DIM1  = 1;
   se1Params.ICNT1 = filterWidth;

   se1Params.DIM2  = strideIn;
   se1Params.ICNT2 = filterHeight;

   se1Params.DIM3  = strideIn * 2;
   se1Params.ICNT3 = (heightOut % 2 == 0) ? (VXLIB_ceilingDiv(heightOut, 2)) : (VXLIB_ceilingDiv(heightOut, 2) - 1);

   se1Params.DIM4  = elemCountIn;
   se1Params.ICNT4 = VXLIB_ceilingDiv(widthIn, elemCountIn);

   se1Params.DECDIM1       = __SE_DECDIM_DIM4;
   se1Params.DECDIM1_WIDTH = widthIn;
   se1Params.DECDIM1SD     = __SE_DECDIMSD_DIM1;

   se1Params.DECDIM2       = __SE_DECDIM_DIM3;
   se1Params.DECDIM2_WIDTH = heightIn * strideIn;

   sa0Params.DIMFMT = __SA_DIMFMT_3D;
   sa0Params.VECLEN = c7x::sa_veclen<vecType>::value;

   sa0Params.ICNT0 = elemCountIn;

   sa0Params.DIM1  = strideOut * 2;
   sa0Params.ICNT1 = VXLIB_ceilingDiv(heightOut, 2);

   sa0Params.DIM2  = elemCountIn;
   sa0Params.ICNT2 = VXLIB_ceilingDiv(widthIn, elemCountIn);

   sa0Params.DECDIM1       = __SA_DECDIM_DIM2;
   sa0Params.DECDIM1_WIDTH = widthOut;
   sa0Params.DECDIM1SD     = __SA_DECDIMSD_DIM0;

   sa0Params.DECDIM2       = __SA_DECDIM_DIM1;
   sa0Params.DECDIM2_WIDTH = strideOut * heightOut;

   sa1Params.DIMFMT = __SA_DIMFMT_3D;
   sa1Params.VECLEN = c7x::sa_veclen<vecType>::value;

   sa1Params.ICNT0 = elemCountIn;

   sa1Params.DIM1  = strideOut * 2;
   sa1Params.ICNT1 = (heightOut % 2 == 0) ? (VXLIB_ceilingDiv(heightOut, 2)) : (VXLIB_ceilingDiv(heightOut, 2) - 1);

   sa1Params.DIM2  = elemCountIn;
   sa1Params.ICNT2 = VXLIB_ceilingDiv(widthIn, elemCountIn);

   sa1Params.DECDIM1       = __SA_DECDIM_DIM2;
   sa1Params.DECDIM1_WIDTH = widthOut;
   sa1Params.DECDIM1SD     = __SA_DECDIMSD_DIM0;

   sa1Params.DECDIM2       = __SA_DECDIM_DIM1;
   sa1Params.DECDIM2_WIDTH = strideOut * heightOut;

   // store SE and SA params
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET) = se0Params;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET) = se1Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET) = sa0Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA1_PARAM_OFFSET) = sa1Params;

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* Explicit instantiations for VXLIB_dilate_init_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

template VXLIB_STATUS VXLIB_dilate_init_ci<VXLIB_DILATE_DTYPE_I8U_O8U>(VXLIB_kernelHandle           handle,
                                                                       const VXLIB_bufParams2D_t   *bufParamsIn,
                                                                       const VXLIB_bufParams2D_t   *bufParamsOut,
                                                                       const VXLIB_dilate_InitArgs *pKerInitArgs);

template VXLIB_STATUS VXLIB_dilate_init_ci<VXLIB_DILATE_DTYPE_I16U_O16U>(VXLIB_kernelHandle           handle,
                                                                         const VXLIB_bufParams2D_t   *bufParamsIn,
                                                                         const VXLIB_bufParams2D_t   *bufParamsOut,
                                                                         const VXLIB_dilate_InitArgs *pKerInitArgs);

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_dilate_exec_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method performs the M x N dilation filter on an input image
template <typename dTypeIn, typename dTypeOut>
VXLIB_STATUS VXLIB_dilate_exec_ci(VXLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{

   VXLIB_STATUS status = VXLIB_SUCCESS;

   // get kernel handle
   VXLIB_dilate_PrivArgs *pKerPrivArgs = (VXLIB_dilate_PrivArgs *) handle;

   // SE/SA params template
   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;
   __SA_TEMPLATE_v1 sa0Params;
   __SA_TEMPLATE_v1 sa1Params;

   // create local pointers
   dTypeIn *restrict pInLocal   = (dTypeIn *) pIn;
   dTypeOut *restrict pOutLocal = (dTypeOut *) pOut;

   size_t widthIn  = pKerPrivArgs->width;
   size_t heightIn = pKerPrivArgs->height;
   size_t strideIn = pKerPrivArgs->strideInElements;

   size_t filterWidth  = pKerPrivArgs->pKerInitArgs.filterWidth;
   size_t filterHeight = pKerPrivArgs->pKerInitArgs.filterHeight;

   size_t widthOut  = widthIn - filterWidth + 1;
   size_t heightOut = heightIn - filterHeight + 1;
   size_t strideOut = pKerPrivArgs->strideOutElements;

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET);
   se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET);
   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET);
   sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA1_PARAM_OFFSET);

   typedef
       typename std::conditional<std::is_same<dTypeIn, uint8_t>::value, c7x::uchar_vec, c7x::ushort_vec>::type vecType;

   size_t elemCountIn = c7x::element_count_of<vecType>::value;

   // open streams
   __SE0_OPEN(pInLocal, se0Params);
   __SE1_OPEN(pInLocal + strideIn, se1Params);
   __SA0_OPEN(sa0Params);
   __SA1_OPEN(sa1Params);

   size_t wBlocks = VXLIB_ceilingDiv(widthIn, elemCountIn);
   size_t hBlocks = VXLIB_ceilingDiv(heightOut, 2);

   for (int32_t wCounter = 0; wCounter < wBlocks; wCounter++) {

      for (int32_t hCounter = 0; hCounter < hBlocks; hCounter++) {

         vecType vMaxA = (vecType) 0;
         vecType vMaxB = (vecType) 0;

         for (int32_t fCounter = 0; fCounter < (filterWidth * filterHeight); fCounter++) {

            vecType a = c7x::strm_eng<0, vecType>::get_adv();
            vecType b = c7x::strm_eng<1, vecType>::get_adv();

            vMaxA = __max(vMaxA, a);
            vMaxB = __max(vMaxB, b);
         }

         __vpred tmp0 = c7x::strm_agen<0, vecType>::get_vpred();
         __vpred tmp1 = c7x::strm_agen<1, vecType>::get_vpred();

         vecType *addr0 = c7x::strm_agen<0, vecType>::get_adv(pOutLocal);
         vecType *addr1 = c7x::strm_agen<1, vecType>::get_adv(pOutLocal + strideOut);

         __vstore_pred(tmp0, addr0, vMaxA);
         __vstore_pred(tmp1, addr1, vMaxB);
      }
   }
   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA0_CLOSE();
   __SA1_CLOSE();

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* Explicit instantiations for VXLIB_dilate_exec_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

template VXLIB_STATUS
VXLIB_dilate_exec_ci<VXLIB_DILATE_TYPENAME_I8U_O8U>(VXLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template VXLIB_STATUS VXLIB_dilate_exec_ci<VXLIB_DILATE_TYPENAME_I16U_O16U>(VXLIB_kernelHandle handle,
                                                                            void *restrict pIn,
                                                                            void *restrict pOut);

void VXLIB_dilate_perfEst(VXLIB_kernelHandle handle, size_t *archCycles, size_t *estCycles)
{

   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_dilate_PrivArgs *pKerPrivArgs = (VXLIB_dilate_PrivArgs *) handle;

   // obtain loop count for compute loop
   size_t numBlocks   = pKerPrivArgs->numBlocks;
   size_t overheadCnt = 17; // profiled code before entering compute loop
   size_t iterConst   = 1;
   size_t ii          = 1;

   *archCycles = iterConst + numBlocks * ii; // obtained from asm
   *estCycles  = overheadCnt + *archCycles;
}
