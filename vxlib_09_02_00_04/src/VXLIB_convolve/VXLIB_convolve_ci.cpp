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

#include "VXLIB_bufParams.h"
#include "VXLIB_convolve_priv.h"
#include "VXLIB_types.h"
#include "VXLIB_utility.h"
#include "c7x_scalable.h"
#include <cstdint>
#include <type_traits>
/**********************************************************************************************************************/
/*                                                                                                                    */
/* DEFINES                                                                                                            */
/*                                                                                                                    */
/**********************************************************************************************************************/

#define SE_PARAM_BASE (0x0000)
#define SE0_PARAM_OFFSET (SE_PARAM_BASE)
#define SE1_PARAM_OFFSET (SE0_PARAM_OFFSET + VXLIB_SE_PARAM_SIZE)
#define SE1B_PARAM_OFFSET (SE1_PARAM_OFFSET + VXLIB_SE_PARAM_SIZE)
#define SA0_PARAM_OFFSET (SE1B_PARAM_OFFSET + VXLIB_SE_PARAM_SIZE)
#define SA1_PARAM_OFFSET (SA0_PARAM_OFFSET + VXLIB_SA_PARAM_SIZE)

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_convolve_init_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method initializes 3x3 convolution kernel
template <uint32_t dTypeIn, uint32_t dTypeFilter, uint32_t dTypeOut>
static VXLIB_STATUS VXLIB_convolve_3x3_init_ci(VXLIB_kernelHandle             handle,
                                               const VXLIB_bufParams2D_t     *bufParamsIn,
                                               const VXLIB_bufParams2D_t     *bufParamsFilter,
                                               const VXLIB_bufParams2D_t     *bufParamsOut,
                                               const VXLIB_convolve_InitArgs *pKerInitArgs)
{
   VXLIB_STATUS status = VXLIB_SUCCESS;

   // structs to hold SE and SA parameters
   __SE_TEMPLATE_v1 se0Params  = __gen_SE_TEMPLATE_v1();
   __SE_TEMPLATE_v1 se1Params  = __gen_SE_TEMPLATE_v1();
   __SE_TEMPLATE_v1 se1bParams = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa0Params  = __gen_SA_TEMPLATE_v1();

   using filter_vec = short4;

   // fetch + SE promote, store all half SIMD width
   __SE_ELETYPE SE_ELETYPE = c7x::se_eletype<c7x::uchar_vec>::value;
   __SE_VECLEN  SE_VECLEN  = c7x::se_veclen<c7x::uchar_hvec>::value;
   __SA_VECLEN  SA_VECLEN  = c7x::sa_veclen<c7x::uchar_hvec>::value;

   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_convolve_PrivArgs *pKerPrivArgs = (VXLIB_convolve_PrivArgs *) handle;

   // number of elements fetched from input each SE get
   size_t elemCount = c7x::element_count_of<c7x::uchar_hvec>::value;

   // get init args
   size_t widthIn  = bufParamsIn->dim_x;
   size_t heightIn = bufParamsIn->dim_y;
   size_t strideIn = bufParamsIn->stride_y;

   size_t widthOut  = bufParamsOut->dim_x;
   size_t heightOut = bufParamsOut->dim_y;
   size_t strideOut = bufParamsOut->stride_y;

   size_t filterHeight = pKerInitArgs->filterHeight;
   size_t filterWidth  = pKerInitArgs->filterWidth;

   size_t padLeft   = pKerInitArgs->padLeft;
   size_t padRight  = pKerInitArgs->padRight;
   size_t padTop    = pKerInitArgs->padTop;
   size_t padBottom = pKerInitArgs->padBottom;

   // determine non-padded vs padded implementation
   bool isNotPadded = (padLeft == 0) && (padRight == 0) && (padTop == 0) && (padBottom == 0);

   uint8_t *pBlock         = pKerPrivArgs->bufPblock;
   pKerPrivArgs->numBlocks = VXLIB_ceilingDiv(widthIn, elemCount);

   // determine output data element size (in bytes)
   size_t dTypeOutSize;

   if (VXLIB_CONVOLVE_I8U_C16S_O8U) {
      dTypeOutSize = sizeof(uint8_t);
   }

   // return error for padded version; not implemented
   else {
      dTypeOutSize = sizeof(int16_t);
   }

   if (isNotPadded) {

      /******************************************/
      /* Prepare SE0 stream to fetch lo samples */
      /******************************************/

      // set SE0 Params
      se0Params.DIMFMT  = __SE_DIMFMT_4D;
      se0Params.ELETYPE = SE_ELETYPE;
      se0Params.VECLEN  = SE_VECLEN;
      se0Params.PROMOTE = __SE_PROMOTE_2X_ZEROEXT;

      // 0: number of elements to be computed per SE fetch
      se0Params.ICNT0 = elemCount;

      // 1: number of accumulation rows in single convolution window
      se0Params.DIM1  = strideIn;
      se0Params.ICNT1 = filterHeight;

      // 2: vertical iterations
      se0Params.DIM2  = strideIn;
      se0Params.ICNT2 = heightIn - filterHeight + 1;

      // 3: horizontal iterations
      se0Params.DIM3  = elemCount;
      se0Params.ICNT3 = VXLIB_ceilingDiv(widthIn, elemCount);

      // input column dimension DECDIM
      se0Params.DECDIM1       = __SE_DECDIM_DIM3;
      se0Params.DECDIM1_WIDTH = widthIn;
      se0Params.DECDIM1SD     = __SE_DECDIMSD_DIM0;

      /******************************************/
      /* Prepare SE1 stream to fetch hi samples */
      /******************************************/

      // set SE1 Params
      se1Params.DIMFMT  = __SE_DIMFMT_4D;
      se1Params.ELETYPE = SE_ELETYPE;
      se1Params.VECLEN  = SE_VECLEN;
      se1Params.PROMOTE = __SE_PROMOTE_2X_ZEROEXT;

      se1Params.ICNT0 = elemCount;

      se1Params.DIM1  = strideIn;
      se1Params.ICNT1 = filterHeight;

      se1Params.DIM2  = strideIn;
      se1Params.ICNT2 = heightIn - filterHeight + 1;

      se1Params.DIM3  = elemCount;
      se1Params.ICNT3 = VXLIB_ceilingDiv(widthIn, elemCount);

      // input column dimension DECDIM
      se1Params.DECDIM1       = __SE_DECDIM_DIM3;
      se1Params.DECDIM1_WIDTH = (widthIn < elemCount) ? 0 : (widthIn - elemCount);
      se1Params.DECDIM1SD     = __SE_DECDIMSD_DIM0;

      /******************************************/
      /* Prepare SE1B stream to fetch coeff     */
      /******************************************/

      se1bParams.DIMFMT  = __SE_DIMFMT_2D;
      se1bParams.ELETYPE = c7x::se_eletype<filter_vec>::value;
      se1bParams.VECLEN  = c7x::se_veclen<filter_vec>::value;
      se1bParams.DIR     = __SE_DIR_DEC;   // reverse stream
      se1bParams.GRPDUP  = __SE_GRPDUP_ON; // group duplicate each 4 elements

      // 0. number of coefficients per fetch
      se1bParams.ICNT0 = filterWidth;

      // 1. fetch lo, hi coeffs of each row - horizontal iteration
      se1bParams.DIM1  = -((int64_t)filterWidth);
      se1bParams.ICNT1 = filterHeight;

      /******************************************/
      /* Prepare SA0 stream to store output     */
      /******************************************/

      // set SA0 Params
      sa0Params.DIMFMT = __SA_DIMFMT_3D;
      sa0Params.VECLEN = SA_VECLEN;

      // 0: number of elements to store per SE fetch
      sa0Params.ICNT0 = elemCount;

      // 1: vertical iterations (1 store per convolution window)
      sa0Params.DIM1  = strideOut / dTypeOutSize; // SA DIM jumps are given in num elements
      sa0Params.ICNT1 = heightOut;

      // 2: horizontal iterations
      sa0Params.DIM2  = elemCount;
      sa0Params.ICNT2 = VXLIB_ceilingDiv(widthOut, elemCount);

      // output column dimension DECDIM
      sa0Params.DECDIM1       = __SA_DECDIM_DIM2;
      sa0Params.DECDIM1_WIDTH = widthOut;
      sa0Params.DECDIM1SD     = __SA_DECDIMSD_DIM0;

      /********************************/
      /* SA1 stream not used in 3x3   */
      /********************************/
   }

   // return error for padded version; not implemented
   else {

      status = VXLIB_ERR_CONVOLVE_PADDED_NOT_IMPLEMENTED;
   }

   // store SE and SA params
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET)  = se0Params;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET)  = se1Params;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1B_PARAM_OFFSET) = se1bParams;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET)  = sa0Params;

   return status;
}

// this method initializes 5x5 convolution kernel
template <uint32_t dTypeIn, uint32_t dTypeFilter, uint32_t dTypeOut>
static VXLIB_STATUS VXLIB_convolve_5x5_init_ci(VXLIB_kernelHandle             handle,
                                               const VXLIB_bufParams2D_t     *bufParamsIn,
                                               const VXLIB_bufParams2D_t     *bufParamsFilter,
                                               const VXLIB_bufParams2D_t     *bufParamsOut,
                                               const VXLIB_convolve_InitArgs *pKerInitArgs)
{
   VXLIB_STATUS status = VXLIB_SUCCESS;

   // structs to hold SE and SA parameters
   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1();
   __SE_TEMPLATE_v1 se1Params = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa0Params = __gen_SA_TEMPLATE_v1();

   // fetch + SE promote, store all half SIMD width
   __SE_ELETYPE SE_ELETYPE = c7x::se_eletype<c7x::uchar_vec>::value;
   __SE_VECLEN  SE_VECLEN  = c7x::se_veclen<c7x::uchar_hvec>::value;
   __SA_VECLEN  SA_VECLEN  = c7x::sa_veclen<c7x::uchar_qvec>::value;

   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_convolve_PrivArgs *pKerPrivArgs = (VXLIB_convolve_PrivArgs *) handle;

   // typedef vector types
   using filter_vec = short4;

   // number of elements fetched from input each SE get
   size_t elemCountIn  = c7x::element_count_of<c7x::uchar_hvec>::value;
   size_t elemCountOut = c7x::element_count_of<c7x::uchar_qvec>::value;

   // get init args
   size_t widthIn  = bufParamsIn->dim_x;
   size_t heightIn = bufParamsIn->dim_y;
   size_t strideIn = bufParamsIn->stride_y;

   size_t widthOut  = bufParamsOut->dim_x;
   size_t heightOut = bufParamsOut->dim_y;
   size_t strideOut = bufParamsOut->stride_y;

   size_t filterHeight = pKerInitArgs->filterHeight;
   size_t filterWidth  = pKerInitArgs->filterWidth;

   size_t padLeft   = pKerInitArgs->padLeft;
   size_t padRight  = pKerInitArgs->padRight;
   size_t padTop    = pKerInitArgs->padTop;
   size_t padBottom = pKerInitArgs->padBottom;

   // determine non-padded vs padded implementation
   bool isNotPadded = (padLeft == 0) && (padRight == 0) && (padTop == 0) && (padBottom == 0);

   uint8_t *pBlock         = pKerPrivArgs->bufPblock;
   pKerPrivArgs->numBlocks = VXLIB_ceilingDiv(widthIn, elemCountIn);

   // determine output data element size (in bytes)
   size_t dTypeOutSize;

   if (VXLIB_CONVOLVE_I8U_C16S_O8U) {
      dTypeOutSize = sizeof(uint8_t);
   }

   else {
      dTypeOutSize = sizeof(int16_t);
   }

   if (isNotPadded) {

      /******************************************/
      /* Prepare SE0 stream to fetch samples    */
      /******************************************/

      // set SE0 Params
      se0Params.DIMFMT  = __SE_DIMFMT_4D;
      se0Params.ELETYPE = SE_ELETYPE;
      se0Params.VECLEN  = SE_VECLEN;
      se0Params.PROMOTE = __SE_PROMOTE_2X_ZEROEXT;

      // 0: number of elements to be computed per SE fetch
      se0Params.ICNT0 = elemCountIn;

      // 1: number of accumulation rows in single convolution window
      se0Params.DIM1  = strideIn;
      se0Params.ICNT1 = filterHeight;

      // 2: vertical iterations
      se0Params.DIM2  = strideIn;
      se0Params.ICNT2 = heightIn - filterHeight + 1;

      // 3: horizontal iterations
      se0Params.DIM3  = elemCountIn / 2; // vfir8 is half-vector fetch, quarter-vector horizontal advance
      se0Params.ICNT3 = VXLIB_ceilingDiv(widthIn, (elemCountIn / 2));

      // input column dimension DECDIM
      se0Params.DECDIM1       = __SE_DECDIM_DIM3;
      se0Params.DECDIM1_WIDTH = widthIn;
      se0Params.DECDIM1SD     = __SE_DECDIMSD_DIM0;

      /********************************************/
      /* Prepare SE1 stream to fetch coefficients */
      /********************************************/

      // set SE1 Params
      se1Params.DIMFMT  = __SE_DIMFMT_2D;
      se1Params.ELETYPE = c7x::se_eletype<filter_vec>::value;
      se1Params.VECLEN  = c7x::se_veclen<filter_vec>::value;
      se1Params.DIR     = __SE_DIR_DEC;   // reverse stream
      se1Params.GRPDUP  = __SE_GRPDUP_ON; // group duplicate each 4 elements

      // 0. number of coefficients per fetch
      se1Params.ICNT0 = filterWidth;

      // 1. fetch lo, hi coeffs of each row - horizontal iteration
      se1Params.DIM1  = -((int64_t)filterWidth);
      se1Params.ICNT1 = filterHeight;

      /******************************************/
      /* Prepare SA0 stream to store output     */
      /******************************************/

      // set SA0 Params
      sa0Params.DIMFMT = __SA_DIMFMT_3D;
      sa0Params.VECLEN = SA_VECLEN;

      // 0: number of elements to store per SE fetch
      sa0Params.ICNT0 = elemCountOut;

      // 1: vertical iterations (1 store per convolution window)
      sa0Params.DIM1  = strideOut / dTypeOutSize;
      sa0Params.ICNT1 = heightOut;

      // 2: horizontal iterations
      sa0Params.DIM2  = elemCountOut;
      sa0Params.ICNT2 = VXLIB_ceilingDiv(widthOut, elemCountOut);

      // output column dimension DECDIM
      sa0Params.DECDIM1       = __SA_DECDIM_DIM2;
      sa0Params.DECDIM1_WIDTH = widthOut;
      sa0Params.DECDIM1SD     = __SA_DECDIMSD_DIM0;

      /*********************************************/
      /* SA1 Stream not used in 5x5 convolution    */
      /*********************************************/
   }

   // return error for padded version; not implemented
   else {

      status = VXLIB_ERR_CONVOLVE_PADDED_NOT_IMPLEMENTED;
   }

   // store SE and SA params
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET) = se0Params;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET) = se1Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET) = sa0Params;

   return status;
}

// this method initializes 7x7 convolution kernel
template <uint32_t dTypeIn, uint32_t dTypeFilter, uint32_t dTypeOut>
static VXLIB_STATUS VXLIB_convolve_7x7_init_ci(VXLIB_kernelHandle             handle,
                                               const VXLIB_bufParams2D_t     *bufParamsIn,
                                               const VXLIB_bufParams2D_t     *bufParamsFilter,
                                               const VXLIB_bufParams2D_t     *bufParamsOut,
                                               const VXLIB_convolve_InitArgs *pKerInitArgs)
{
   VXLIB_STATUS status = VXLIB_SUCCESS;

   // structs to hold SE and SA parameters
   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1();
   __SE_TEMPLATE_v1 se1Params = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa0Params = __gen_SA_TEMPLATE_v1();

   // fetch + SE promote, store all half SIMD width
   __SE_ELETYPE SE_ELETYPE = c7x::se_eletype<c7x::uchar_vec>::value;
   __SE_VECLEN  SE_VECLEN  = c7x::se_veclen<c7x::uchar_hvec>::value;
   __SA_VECLEN  SA_VECLEN  = c7x::sa_veclen<c7x::uchar_qvec>::value;

   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_convolve_PrivArgs *pKerPrivArgs = (VXLIB_convolve_PrivArgs *) handle;

   // typedef vector types
   using filter_vec = short4;

   // number of elements fetched from input each SE get
   size_t elemCountIn  = c7x::element_count_of<c7x::uchar_hvec>::value;
   size_t elemCountOut = c7x::element_count_of<c7x::uchar_qvec>::value;

   // get init args
   size_t widthIn  = bufParamsIn->dim_x;
   size_t heightIn = bufParamsIn->dim_y;
   size_t strideIn = bufParamsIn->stride_y;

   size_t widthOut  = bufParamsOut->dim_x;
   size_t heightOut = bufParamsOut->dim_y;
   size_t strideOut = bufParamsOut->stride_y;

   size_t filterHeight = pKerInitArgs->filterHeight;
   size_t filterWidth  = pKerInitArgs->filterWidth;

   size_t padLeft   = pKerInitArgs->padLeft;
   size_t padRight  = pKerInitArgs->padRight;
   size_t padTop    = pKerInitArgs->padTop;
   size_t padBottom = pKerInitArgs->padBottom;

   // determine non-padded vs padded implementation
   bool isNotPadded = (padLeft == 0) && (padRight == 0) && (padTop == 0) && (padBottom == 0);

   uint8_t *pBlock         = pKerPrivArgs->bufPblock;
   pKerPrivArgs->numBlocks = VXLIB_ceilingDiv(widthIn, elemCountIn);

   // determine output data element size (in bytes)
   size_t dTypeOutSize;

   if (VXLIB_CONVOLVE_I8U_C16S_O8U) {
      dTypeOutSize = sizeof(uint8_t);
   }

   else {
      dTypeOutSize = sizeof(int16_t);
   }

   if (isNotPadded) {

      /******************************************/
      /* Prepare SE0 stream to fetch samples    */
      /******************************************/

      // set SE0 Params
      se0Params.DIMFMT  = __SE_DIMFMT_4D;
      se0Params.ELETYPE = SE_ELETYPE;
      se0Params.VECLEN  = SE_VECLEN;
      se0Params.PROMOTE = __SE_PROMOTE_2X_ZEROEXT;

      // 0: number of elements to be computed per SE fetch
      se0Params.ICNT0 = elemCountIn;

      // 1: number of accumulation rows in single convolution window
      se0Params.DIM1  = strideIn;
      se0Params.ICNT1 = filterHeight;

      // 2: vertical iterations
      se0Params.DIM2  = strideIn;
      se0Params.ICNT2 = heightIn - filterHeight + 1;

      // 3: horizontal iterations
      se0Params.DIM3  = elemCountIn / 2; // vfir8 is half-vector fetch, quarter-vec adv
      se0Params.ICNT3 = VXLIB_ceilingDiv(widthIn, (elemCountIn / 2));

      // input column dimension DECDIM
      se0Params.DECDIM1       = __SE_DECDIM_DIM3;
      se0Params.DECDIM1_WIDTH = widthIn;
      se0Params.DECDIM1SD     = __SE_DECDIMSD_DIM0;

      /********************************************/
      /* Prepare SE1 stream to fetch coefficients */
      /********************************************/

      // set SE1 Params
      se1Params.DIMFMT  = __SE_DIMFMT_2D;
      se1Params.ELETYPE = c7x::se_eletype<filter_vec>::value;
      se1Params.VECLEN  = c7x::se_veclen<filter_vec>::value;
      se1Params.DIR     = __SE_DIR_DEC;   // reverse stream
      se1Params.GRPDUP  = __SE_GRPDUP_ON; // group duplicate each 4 elements

      // 0. number of coefficients per fetch
      se1Params.ICNT0 = filterWidth;

      // 1. fetch lo, hi coeffs of each row - horizontal iteration
      se1Params.DIM1  = -((int64_t)filterWidth);
      se1Params.ICNT1 = filterHeight;

      /******************************************/
      /* Prepare SA0 stream to store output     */
      /******************************************/

      // set SA0 Params
      sa0Params.DIMFMT = __SA_DIMFMT_3D;
      sa0Params.VECLEN = SA_VECLEN;

      // 0: number of elements to store per SE fetch
      sa0Params.ICNT0 = elemCountOut;

      // 1: vertical iterations (1 store per convolution window)
      sa0Params.DIM1  = strideOut / dTypeOutSize;
      sa0Params.ICNT1 = heightOut;

      // 2: horizontal iterations
      sa0Params.DIM2  = elemCountOut;
      sa0Params.ICNT2 = VXLIB_ceilingDiv(widthOut, elemCountOut);

      // output column dimension DECDIM
      sa0Params.DECDIM1       = __SA_DECDIM_DIM2;
      sa0Params.DECDIM1_WIDTH = widthOut;
      sa0Params.DECDIM1SD     = __SA_DECDIMSD_DIM0;

      /*********************************************/
      /* SA1 Stream not used in 7x7 convolution    */
      /*********************************************/
   }

   // return error for padded version; not implemented
   else {

      status = VXLIB_ERR_CONVOLVE_PADDED_NOT_IMPLEMENTED;
   }

   // store SE and SA params
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET) = se0Params;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET) = se1Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET) = sa0Params;

   return status;
}

// this method initializes 9x9 convolution kernel
template <uint32_t dTypeIn, uint32_t dTypeFilter, uint32_t dTypeOut>
static VXLIB_STATUS VXLIB_convolve_9x9_init_ci(VXLIB_kernelHandle             handle,
                                               const VXLIB_bufParams2D_t     *bufParamsIn,
                                               const VXLIB_bufParams2D_t     *bufParamsFilter,
                                               const VXLIB_bufParams2D_t     *bufParamsOut,
                                               const VXLIB_convolve_InitArgs *pKerInitArgs)
{
   VXLIB_STATUS status = VXLIB_SUCCESS;

   // typedef vector types
   using filter_vec = short4;

   // structs to hold SE and SA parameters
   __SE_TEMPLATE_v1 se0Params  = __gen_SE_TEMPLATE_v1();
   __SE_TEMPLATE_v1 se1Params  = __gen_SE_TEMPLATE_v1();
   __SE_TEMPLATE_v1 se1bParams = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa0Params  = __gen_SA_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa1Params  = __gen_SA_TEMPLATE_v1();

   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_convolve_PrivArgs *pKerPrivArgs = (VXLIB_convolve_PrivArgs *) handle;

   // number of elements fetched from input each SE get
   size_t elemCountIn  = c7x::element_count_of<c7x::uchar_hvec>::value;
   size_t elemCountOut = c7x::element_count_of<c7x::uchar_qvec>::value;

   // get init args
   size_t widthIn  = bufParamsIn->dim_x;
   size_t heightIn = bufParamsIn->dim_y;
   size_t strideIn = bufParamsIn->stride_y;

   size_t widthOut  = bufParamsOut->dim_x;
   size_t heightOut = bufParamsOut->dim_y;
   size_t strideOut = bufParamsOut->stride_y;

   size_t filterHeight = pKerInitArgs->filterHeight;
   size_t filterWidth  = pKerInitArgs->filterWidth;

   size_t padLeft   = pKerInitArgs->padLeft;
   size_t padRight  = pKerInitArgs->padRight;
   size_t padTop    = pKerInitArgs->padTop;
   size_t padBottom = pKerInitArgs->padBottom;

   // determine non-padded vs padded implementation
   bool isNotPadded = (padLeft == 0) && (padRight == 0) && (padTop == 0) && (padBottom == 0);

   uint8_t *pBlock         = pKerPrivArgs->bufPblock;
   pKerPrivArgs->numBlocks = VXLIB_ceilingDiv(widthIn, elemCountIn);

   // determine output data element size (in bytes)
   size_t dTypeOutSize;

   if (VXLIB_CONVOLVE_I8U_C16S_O8U) {
      dTypeOutSize = sizeof(uint8_t);
   }

   else {
      dTypeOutSize = sizeof(int16_t);
   }

   if (isNotPadded) {

      /******************************************/
      /* Prepare SE0 stream to fetch samples    */
      /******************************************/

      // set SE0 Params
      se0Params.DIMFMT  = __SE_DIMFMT_4D;
      se0Params.ELETYPE = c7x::se_eletype<c7x::uchar_vec>::value;
      se0Params.VECLEN  = c7x::se_veclen<c7x::uchar_hvec>::value;
      se0Params.PROMOTE = __SE_PROMOTE_2X_ZEROEXT;

      // 0: number of elements to be computed per SE fetch
      se0Params.ICNT0 = elemCountIn;

      // 1: number of vertical accumulation rows in single convolution window
      se0Params.DIM1  = strideIn;
      se0Params.ICNT1 = filterHeight;

      // 2: vertical iterations
      se0Params.DIM2  = strideIn;
      se0Params.ICNT2 = heightIn - filterHeight + 1;

      // 3: horizontal iterations
      se0Params.DIM3  = elemCountIn / 2; // vfir8 is half-vector fetch, quarter-vec adv horizontally
      se0Params.ICNT3 = VXLIB_ceilingDiv(widthIn, (elemCountIn / 2));

      // input column dimension DECDIM
      se0Params.DECDIM1       = __SE_DECDIM_DIM3;
      se0Params.DECDIM1_WIDTH = widthIn;
      se0Params.DECDIM1SD     = __SE_DECDIMSD_DIM0;

      /********************************************************/
      /* Prepare SE1 stream to fetch last coeff input samples */
      /********************************************************/

      // SE1 will fetch last same values from SE0 with an 8 input element offset to compute MAC operation for last coeff

      // set SE1 Params
      se1Params.DIMFMT  = __SE_DIMFMT_4D;
      se1Params.ELETYPE = c7x::se_eletype<c7x::uchar_vec>::value;
      se1Params.VECLEN  = c7x::se_veclen<c7x::uchar_qvec>::value;
      se1Params.PROMOTE = __SE_PROMOTE_4X_ZEROEXT;

      // 0: number of elements to be computed per SE fetch
      se1Params.ICNT0 = c7x::element_count_of<c7x::uchar_qvec>::value;

      // 1: number of accumulation rows in single convolution window
      se1Params.DIM1  = strideIn;
      se1Params.ICNT1 = filterHeight;

      // 2: vertical iterations
      se1Params.DIM2  = strideIn;
      se1Params.ICNT2 = heightIn - filterHeight + 1;

      // 3: horizontal iterations
      se1Params.DIM3  = elemCountIn / 2; // vfir8 is half-vector fetch, quarter-vec adv
      se1Params.ICNT3 = VXLIB_ceilingDiv(widthIn, (elemCountIn / 2));

      // input column dimension DECDIM
      se1Params.DECDIM1       = __SE_DECDIM_DIM3;
      se1Params.DECDIM1_WIDTH = widthIn - 8;
      se1Params.DECDIM1SD     = __SE_DECDIMSD_DIM0;

      /*********************************************/
      /* Prepare SE1B stream to fetch coefficients */
      /*********************************************/

      // fetch 9x8 coefficients as part of pre-loop processing, use SA1 to load last coeff for the full 9x9

      // set SE1 Params
      se1bParams.DIMFMT  = __SE_DIMFMT_2D;
      se1bParams.ELETYPE = c7x::se_eletype<filter_vec>::value;
      se1bParams.VECLEN  = c7x::se_veclen<filter_vec>::value;
      se1bParams.DIR     = __SE_DIR_DEC;
      se1bParams.GRPDUP  = __SE_GRPDUP_ON; // group duplicate each 4 elements

      // 0. number of coefficients per fetch
      se1bParams.ICNT0 = 8;

      // 1. fetch lo, hi coeffs of each row - horizontal iteration
      se1bParams.DIM1  = -((int64_t)filterWidth);
      se1bParams.ICNT1 = filterHeight;

      /******************************************/
      /* Prepare SA0 stream to store output     */
      /******************************************/

      // set SA0 Params
      sa0Params.DIMFMT = __SA_DIMFMT_3D;
      sa0Params.VECLEN = c7x::sa_veclen<c7x::uchar_qvec>::value;

      // 0: number of elements to store per SE fetch
      sa0Params.ICNT0 = elemCountOut;

      // 1: vertical iterations (1 store per convolution window)
      sa0Params.DIM1  = strideOut / dTypeOutSize; // SA DIM jumps are given in num elements
      sa0Params.ICNT1 = heightOut;

      // 2: horizontal iterations
      sa0Params.DIM2  = elemCountOut;
      sa0Params.ICNT2 = VXLIB_ceilingDiv(widthOut, elemCountOut);

      // output column dimension DECDIM
      sa0Params.DECDIM1       = __SA_DECDIM_DIM2;
      sa0Params.DECDIM1_WIDTH = widthOut;
      sa0Params.DECDIM1SD     = __SA_DECDIMSD_DIM0;

      /********************************************/
      /* Prepare SA1 Stream to fetch last coeffs  */
      /********************************************/

      // each get advance will fetch the last coefficient of each row
      sa1Params.DIMFMT = __SA_DIMFMT_3D;
      sa1Params.VECLEN = __SA_VECLEN_1ELEM;

      sa1Params.ICNT0 = 1;

      sa1Params.DIM1  = -((int64_t)filterWidth);
      sa1Params.ICNT1 = filterHeight;

      sa1Params.DIM2  = 0;
      sa1Params.ICNT2 = heightOut * VXLIB_ceilingDiv(widthIn, (elemCountIn / 2));
   }

   // return error for padded version; not implemented
   else {

      status = VXLIB_ERR_CONVOLVE_PADDED_NOT_IMPLEMENTED;
   }

   // store SE and SA params
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET)  = se0Params;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET)  = se1Params;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1B_PARAM_OFFSET) = se1bParams;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET)  = sa0Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA1_PARAM_OFFSET)  = sa1Params;

   return status;
}

// this method initializes the kernel-specific parameters based on filter size
// mainly, the streaming engine and streaming address generators
template <uint32_t dTypeIn, uint32_t dTypeFilter, uint32_t dTypeOut>
VXLIB_STATUS VXLIB_convolve_init_ci(VXLIB_kernelHandle             handle,
                                    const VXLIB_bufParams2D_t     *bufParamsIn,
                                    const VXLIB_bufParams2D_t     *bufParamsFilter,
                                    const VXLIB_bufParams2D_t     *bufParamsOut,
                                    const VXLIB_convolve_InitArgs *pKerInitArgs)
{

   VXLIB_STATUS status = VXLIB_SUCCESS; // assign status to success by default

   // setup se/sa
   size_t filterHeight = pKerInitArgs->filterHeight;
   size_t filterWidth  = pKerInitArgs->filterWidth;

   // call 3x3 init
   if ((filterHeight == filterWidth) && (filterWidth == 3)) {
      status = VXLIB_convolve_3x3_init_ci<dTypeIn, dTypeFilter, dTypeOut>(handle, bufParamsIn, bufParamsFilter,
                                                                          bufParamsOut, pKerInitArgs);
   }

   // call 5x5 init
   else if ((filterHeight == filterWidth) && (filterWidth == 5)) {
      status = VXLIB_convolve_5x5_init_ci<dTypeIn, dTypeFilter, dTypeOut>(handle, bufParamsIn, bufParamsFilter,
                                                                          bufParamsOut, pKerInitArgs);
   }

   // call 7x7 init
   else if ((filterHeight == filterWidth) && (filterWidth == 7)) {
      status = VXLIB_convolve_7x7_init_ci<dTypeIn, dTypeFilter, dTypeOut>(handle, bufParamsIn, bufParamsFilter,
                                                                          bufParamsOut, pKerInitArgs);
   }

   // call 9x9 init
   else if ((filterHeight == filterWidth) && (filterWidth == 9)) {
      status = VXLIB_convolve_9x9_init_ci<dTypeIn, dTypeFilter, dTypeOut>(handle, bufParamsIn, bufParamsFilter,
                                                                          bufParamsOut, pKerInitArgs);
   }

   // call mxn init
   else {

      // structs to hold SE and SA parameters
      __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1();
      __SE_TEMPLATE_v1 se1Params = __gen_SE_TEMPLATE_v1();
      __SA_TEMPLATE_v1 sa0Params = __gen_SA_TEMPLATE_v1();

      // typecast handle (void) to struct pointer type associated to kernel
      VXLIB_convolve_PrivArgs *pKerPrivArgs = (VXLIB_convolve_PrivArgs *) handle;

      // number of elements fetched from input each SE get
      size_t elemCountIn  = c7x::element_count_of<c7x::uchar_hvec>::value;
      size_t elemCountOut = c7x::element_count_of<c7x::uchar_hvec>::value;

      // get init args
      size_t widthIn  = bufParamsIn->dim_x;
      size_t strideIn = bufParamsIn->stride_y;

      size_t widthOut  = bufParamsOut->dim_x;
      size_t heightOut = bufParamsOut->dim_y;
      size_t strideOut = bufParamsOut->stride_y;


      size_t padLeft   = pKerInitArgs->padLeft;
      size_t padRight  = pKerInitArgs->padRight;
      size_t padTop    = pKerInitArgs->padTop;
      size_t padBottom = pKerInitArgs->padBottom;

      // determine non-padded vs padded implementation
      bool isNotPadded = (padLeft == 0) && (padRight == 0) && (padTop == 0) && (padBottom == 0);

      uint8_t *pBlock         = pKerPrivArgs->bufPblock;
      pKerPrivArgs->numBlocks = VXLIB_ceilingDiv(widthIn, elemCountIn);

      // determine output data element size (in bytes)
      size_t dTypeOutSize;

      if (VXLIB_CONVOLVE_I8U_C16S_O8U) {
         dTypeOutSize = sizeof(uint8_t);
      }

      else {
         dTypeOutSize = sizeof(int16_t);
      }

      if (isNotPadded) {

         /******************************************/
         /* Prepare SE0 stream to fetch samples    */
         /******************************************/

         // set SE0 Params
         se0Params.DIMFMT  = __SE_DIMFMT_5D;
         se0Params.ELETYPE = c7x::se_eletype<c7x::uchar_vec>::value;
         se0Params.VECLEN  = c7x::se_veclen<c7x::uchar_hvec>::value;
         se0Params.PROMOTE = __SE_PROMOTE_2X_ZEROEXT;

         // 0: number of elements to be computed per SE fetch
         se0Params.ICNT0 = elemCountIn;

         // 1: number of horizontal slides per convolution window
         se0Params.DIM1  = 1;
         se0Params.ICNT1 = filterWidth;

         // 2: number of vertical slides per convolution window
         se0Params.DIM2  = strideIn;
         se0Params.ICNT2 = filterHeight;

         // 3: vertical iterations of entire input image
         se0Params.DIM3  = strideIn;
         se0Params.ICNT3 = heightOut;

         // 4. horizontal iterations of entire input image
         se0Params.DIM4  = elemCountIn;
         se0Params.ICNT4 = VXLIB_ceilingDiv(widthIn, elemCountIn);

         // input column dimension DECDIM
         se0Params.DECDIM1       = __SE_DECDIM_DIM1;
         se0Params.DECDIM1_WIDTH = widthIn;
         /* se0Params.DECDIM1SD     = __SE_DECDIMSD_DIM0; */

         se0Params.DECDIM2       = __SE_DECDIM_DIM4;
         se0Params.DECDIM2_WIDTH = widthIn;
         se0Params.DECDIM2SD     = __SE_DECDIMSD_DIM1;
         /********************************************/
         /* Prepare SE1 stream to fetch coefficients */
         /********************************************/

         // set SE1 Params
         se1Params.DIMFMT  = __SE_DIMFMT_5D;
         se1Params.ELETYPE = c7x::se_eletype<c7x::short_vec>::value;
         se1Params.VECLEN  = c7x::se_veclen<c7x::short_vec>::value;
         se1Params.DIR     = __SE_DIR_DEC; // reverse stream
         se1Params.ELEDUP  = c7x::se_eledup<int16_t, c7x::short_vec>::value;

         // 0. number of coefficients per fetch
         se1Params.ICNT0 = 1;

         // 1. fetch single convolution window - vertical iteration
         se1Params.DIM1  = -1;
         se1Params.ICNT1 = filterWidth;

         // 2. vertical iteration
         se1Params.DIM2  = -((int64_t)filterWidth);
         se1Params.ICNT2 = filterHeight;

         // 3. horizontal iteration
         se1Params.DIM3  = 0;
         se1Params.ICNT3 = heightOut;

         // 4. horizontal iteration
         se1Params.DIM4  = 0;
         se1Params.ICNT4 = VXLIB_ceilingDiv(widthIn, elemCountIn);

         /******************************************/
         /* Prepare SA0 stream to store output     */
         /******************************************/

         // set SA0 Params
         sa0Params.DIMFMT = __SA_DIMFMT_3D;
         sa0Params.VECLEN = c7x::sa_veclen<c7x::uchar_hvec>::value;

         // 0: number of elements to store per SE fetch
         sa0Params.ICNT0 = elemCountOut;

         // 1: vertical iterations (1 store per convolution window)
         sa0Params.DIM1  = strideOut / dTypeOutSize; // SA DIM jumps are given in num elements
         sa0Params.ICNT1 = heightOut;

         // 2: horizontal iterations
         sa0Params.DIM2  = elemCountOut;
         sa0Params.ICNT2 = VXLIB_ceilingDiv(widthIn, elemCountIn);

         // output column dimension DECDIM
         sa0Params.DECDIM1       = __SA_DECDIM_DIM2;
         sa0Params.DECDIM1_WIDTH = widthOut;
         sa0Params.DECDIM1SD     = __SA_DECDIMSD_DIM0;
      }

      // return error for padded version; not implemented
      else {

         status = VXLIB_ERR_CONVOLVE_PADDED_NOT_IMPLEMENTED;
      }

      // store SE and SA params
      *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET) = se0Params;
      *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET) = se1Params;
      *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET) = sa0Params;
   }

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* Explicit instantiations for VXLIB_convolve_init_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

template VXLIB_STATUS
VXLIB_convolve_init_ci<VXLIB_CONVOLVE_DTYPE_I8U_C16S_O8U>(VXLIB_kernelHandle             handle,
                                                          const VXLIB_bufParams2D_t     *bufParamsIn,
                                                          const VXLIB_bufParams2D_t     *bufParamsFilter,
                                                          const VXLIB_bufParams2D_t     *bufParamsOut,
                                                          const VXLIB_convolve_InitArgs *pKerInitArgs);

template VXLIB_STATUS
VXLIB_convolve_init_ci<VXLIB_CONVOLVE_DTYPE_I8U_C16S_O16S>(VXLIB_kernelHandle             handle,
                                                           const VXLIB_bufParams2D_t     *bufParamsIn,
                                                           const VXLIB_bufParams2D_t     *bufParamsFilter,
                                                           const VXLIB_bufParams2D_t     *bufParamsOut,
                                                           const VXLIB_convolve_InitArgs *pKerInitArgs);

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_convolve_exec_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method performs 3x3 convolve filter computation
template <typename dTypeIn, typename dTypeFilter, typename dTypeOut>
static VXLIB_STATUS
VXLIB_convolve_3x3_exec_ci(VXLIB_kernelHandle handle, void *restrict pIn, void *restrict pFilter, void *restrict pOut)
{
   // set status
   VXLIB_STATUS status = VXLIB_SUCCESS;

   // get kernel handle
   VXLIB_convolve_PrivArgs *pKerPrivArgs = (VXLIB_convolve_PrivArgs *) handle;

   // SE/SA params template
   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;
   __SE_TEMPLATE_v1 se1bParams;
   __SA_TEMPLATE_v1 sa0Params;

   // create local pointers
   dTypeIn *restrict pInLocal         = (dTypeIn *) pIn;
   dTypeFilter *restrict pFilterLocal = (dTypeFilter *) pFilter;
   dTypeOut *restrict pOutLocal       = (dTypeOut *) pOut;



   uint8_t *pBlock    = pKerPrivArgs->bufPblock;
   size_t   elemCount = c7x::element_count_of<c7x::uchar_hvec>::value;

   // get SE/SA params
   se0Params  = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET);
   se1Params  = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET);
   se1bParams = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1B_PARAM_OFFSET);
   sa0Params  = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET);

   // get init parameters
   size_t filterWidth  = pKerPrivArgs->pKerInitArgs.filterWidth;
   size_t filterHeight = pKerPrivArgs->pKerInitArgs.filterHeight;
   size_t scale        = pKerPrivArgs->pKerInitArgs.scale;

   size_t width  = pKerPrivArgs->width;
   size_t height = pKerPrivArgs->height;

   // compute loop iteration counts
   size_t wBlocks = VXLIB_ceilingDiv(width, elemCount);
   size_t hBlocks = height - filterHeight + 1;

   c7x::int_vec vShift = (c7x::int_vec) scale;

   c7x::int_vec vResA_lo, vResA_hi, vResB_lo, vResB_hi, vResC_lo, vResC_hi;
   c7x::int_vec vResAB_lo, vResAB_hi;
   c7x::int_vec vResABC_lo, vResABC_hi;

   c7x::short_vec vCoeff0, vCoeff1, vCoeff2;

   // open streams
   __SE0_OPEN(pInLocal, se0Params);
   __SE1_OPEN(pFilterLocal + (filterWidth * filterHeight), se1bParams);
   __SA0_OPEN(sa0Params);

   // fetch filter coefficients in reverse order
   vCoeff0 = c7x::strm_eng<1, c7x::short_vec>::get_adv();
   vCoeff1 = c7x::strm_eng<1, c7x::short_vec>::get_adv();
   vCoeff2 = c7x::strm_eng<1, c7x::short_vec>::get_adv();

   __SE1_CLOSE();

   __SE1_OPEN(pInLocal + elemCount, se1Params);

   for (int32_t wCounter = 0; wCounter < wBlocks; wCounter++) {

      for (int32_t hCounter = 0; hCounter < hBlocks; hCounter++) {

         // vfir4 row A (.N2/4)
         __vfir4hw_vww(vCoeff0, __SE_REG_PAIR_0_ADV, vResA_lo, vResA_hi);

         // vfir4 row B (.N2/4)
         __vfir4hw_vww(vCoeff1, __SE_REG_PAIR_0_ADV, vResB_lo, vResB_hi);

         // accumulate AB lo (.L2, .S2, .C2, .M2/1)
         vResAB_lo = vResA_lo + vResB_lo;

         // accumulate AB hi (.L2, .S2, .C2, .M2/1)
         vResAB_hi = vResA_hi + vResB_hi;

         // vfir4 row C (.N2/4)
         __vfir4hw_vww(vCoeff2, __SE_REG_PAIR_0_ADV, vResC_lo, vResC_hi);

         // accumulate ABC lo (.L2, .S2, .C2, .M2/1)
         vResABC_lo = vResAB_lo + vResC_lo;

         // accumulate ABC hi (.L2, .S2, .C2, .M2/1)
         vResABC_hi = vResAB_hi + vResC_hi;

         // scale (shift) ABC lo (.L2, S2/1)
         vResABC_lo = __shift_right(vResABC_lo, vShift);

         // scale (shift) ABC hi (.L2, S2/1)
         vResABC_hi = __shift_right(vResABC_hi, vShift);

         // predicated store result (.D2/1)
         if (VXLIB_CONVOLVE_I8U_C16S_O8U_TEMPLATE(dTypeOut)) {

            // saturate from 32b signed to unsigned 8b limits
            c7x::uint_vec vSatU_lo = __saturate_s2u(vResABC_lo, (c7x::uchar_qvec) 8);
            c7x::uint_vec vSatU_hi = __saturate_s2u(vResABC_hi, (c7x::uchar_qvec) 8);

            __vpred     tmp  = c7x::strm_agen<0, c7x::uchar_qvec>::get_vpred();
            c7x::uchar_hvec *addr = c7x::strm_agen<0, c7x::uchar_hvec>::get_adv(pOutLocal);

            // predicated pack low byte store, 2 src
            __vstore_pred_pack_byte_2src(tmp, addr, vSatU_lo, vSatU_hi);
         }

         else {

            // saturate from 32b signed to 16b signed limits
            c7x::int_vec vSat_lo = __saturate(vResABC_lo, (c7x::uchar_qvec) 16);
            c7x::int_vec vSat_hi = __saturate(vResABC_hi, (c7x::uchar_qvec) 16);

            __vpred    tmp  = c7x::strm_agen<0, c7x::short_vec>::get_vpred();
            c7x::short_vec *addr = c7x::strm_agen<0, c7x::short_vec>::get_adv(pOutLocal);
            // predicated pack low store, 2 src
            __vstore_pred_packl_2src(tmp, addr, vSat_lo, vSat_hi);
         }

      }
   }

   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA0_CLOSE();
   __SA1_CLOSE();

   return status;
}

template <VXLIB_CONVOLVE_TYPENAME_I8U_C16S_O8U>
static VXLIB_STATUS
VXLIB_convolve_3x3_exec_ci(VXLIB_kernelHandle handle, void *restrict pIn, void *restrict pFilter, void *restrict pOut);

template <VXLIB_CONVOLVE_TYPENAME_I8U_C16S_O16S>
static VXLIB_STATUS
VXLIB_convolve_3x3_exec_ci(VXLIB_kernelHandle handle, void *restrict pIn, void *restrict pFilter, void *restrict pOut);

// this method performs 5x5 convolve filter computation
template <typename dTypeIn, typename dTypeFilter, typename dTypeOut>
static VXLIB_STATUS
VXLIB_convolve_5x5_exec_ci(VXLIB_kernelHandle handle, void *restrict pIn, void *restrict pFilter, void *restrict pOut)
{
   // set status
   VXLIB_STATUS status = VXLIB_SUCCESS;

   // get kernel handle
   VXLIB_convolve_PrivArgs *pKerPrivArgs = (VXLIB_convolve_PrivArgs *) handle;

   // SE/SA params template
   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;
   __SA_TEMPLATE_v1 sa0Params;

   // create local pointers
   dTypeIn *restrict pInLocal         = (dTypeIn *) pIn;
   dTypeFilter *restrict pFilterLocal = (dTypeFilter *) pFilter;
   dTypeOut *restrict pOutLocal       = (dTypeOut *) pOut;



   uint8_t *pBlock    = pKerPrivArgs->bufPblock;
   size_t   elemCount = c7x::element_count_of<c7x::uchar_hvec>::value;

   // get SE/SA params
   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET);
   se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET);
   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET);

   // get init parameters
   size_t filterWidth  = pKerPrivArgs->pKerInitArgs.filterWidth;
   size_t filterHeight = pKerPrivArgs->pKerInitArgs.filterHeight;
   size_t scale        = pKerPrivArgs->pKerInitArgs.scale;

   size_t width  = pKerPrivArgs->width;
   size_t height = pKerPrivArgs->height;

   // compute loop iteration counts
   size_t wBlocks = VXLIB_ceilingDiv(width, (elemCount / 2));
   size_t hBlocks = height - filterHeight + 1;

   c7x::int_vec vShift = (c7x::int_vec) scale;

   c7x::short_vec vCoeff0_lo, vCoeff1_lo, vCoeff2_lo, vCoeff3_lo, vCoeff4_lo;
   c7x::short_vec vCoeff0_hi, vCoeff1_hi, vCoeff2_hi, vCoeff3_hi, vCoeff4_hi;

   c7x::int_vec vRes0, vRes1, vRes2, vRes3, vRes4;
   c7x::int_vec vAcc1, vAcc2, vAcc3, vAcc4, vRes;

   // open streams
   __SE0_OPEN(pInLocal, se0Params);
   __SE1_OPEN(pFilterLocal + (filterWidth * filterHeight), se1Params); // access filter in reverse
   __SA0_OPEN(sa0Params);

   // fetch filter coeffs from SE1
   vCoeff0_lo = c7x::strm_eng<1, c7x::short_vec>::get_adv();
   vCoeff0_hi = c7x::strm_eng<1, c7x::short_vec>::get_adv();

   vCoeff1_lo = c7x::strm_eng<1, c7x::short_vec>::get_adv();
   vCoeff1_hi = c7x::strm_eng<1, c7x::short_vec>::get_adv();

   vCoeff2_lo = c7x::strm_eng<1, c7x::short_vec>::get_adv();
   vCoeff2_hi = c7x::strm_eng<1, c7x::short_vec>::get_adv();

   vCoeff3_lo = c7x::strm_eng<1, c7x::short_vec>::get_adv();
   vCoeff3_hi = c7x::strm_eng<1, c7x::short_vec>::get_adv();

   vCoeff4_lo = c7x::strm_eng<1, c7x::short_vec>::get_adv();
   vCoeff4_hi = c7x::strm_eng<1, c7x::short_vec>::get_adv();

   for (int32_t wCounter = 0; wCounter < wBlocks; wCounter++) {
      for (int32_t hCounter = 0; hCounter < hBlocks; hCounter++) {

         // vfir8 row 0, se0adv
         vRes0 = __vfir8hw_wvv(vCoeff0_lo, vCoeff0_hi, __SE_REG_0_ADV);

         // vfir8 row 1, se0adv
         vRes1 = __vfir8hw_wvv(vCoeff1_lo, vCoeff1_hi, __SE_REG_0_ADV);

         // sum 0-1
         vAcc1 = vRes0 + vRes1;

         // vfir8 row 2, se0adv
         vRes2 = __vfir8hw_wvv(vCoeff2_lo, vCoeff2_hi, __SE_REG_0_ADV);

         // sum 0-2
         vAcc2 = vAcc1 + vRes2;

         // vfir8 row 3, se0adv
         vRes3 = __vfir8hw_wvv(vCoeff3_lo, vCoeff3_hi, __SE_REG_0_ADV);

         // sum 0-3
         vAcc3 = vAcc2 + vRes3;

         // vfir8 row 4, se0adv
         vRes4 = __vfir8hw_wvv(vCoeff4_lo, vCoeff4_hi, __SE_REG_0_ADV);

         // sum 0-4
         vAcc4 = vAcc3 + vRes4;

         // shift right by scale factor
         vRes = __shift_right(vAcc4, vShift);

         // predicated store result (.D2/1)

         if (VXLIB_CONVOLVE_I8U_C16S_O8U_TEMPLATE(dTypeOut)) {

            // saturate from 32b signed to 8b unsigned
            c7x::uint_vec vSatU = __saturate_s2u(vRes, (c7x::uchar_qvec) 8);
            // predicated store
            __vpred tmp = c7x::strm_agen<0, c7x::uint_vec>::get_vpred();

            c7x::uchar_qvec *addr = c7x::strm_agen<0, c7x::uchar_qvec>::get_adv(pOutLocal);
            __vstore_pred_pack_byte(tmp, addr, vSatU);
         }

         else  {

            // saturate from 32b signed to 16b signed
            c7x::int_vec vSat = __saturate(vRes, (c7x::uchar_qvec) 16);

            // predicated store
            __vpred tmp = c7x::strm_agen<0, c7x::int_vec>::get_vpred();

            c7x::short_hvec *addr = c7x::strm_agen<0, c7x::short_hvec>::get_adv(pOutLocal);

            __vstore_pred_packl(tmp, addr, vSat);
         }
      }
   }

   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA0_CLOSE();

   return status;
}

template <VXLIB_CONVOLVE_TYPENAME_I8U_C16S_O8U>
static VXLIB_STATUS
VXLIB_convolve_5x5_exec_ci(VXLIB_kernelHandle handle, void *restrict pIn, void *restrict pFilter, void *restrict pOut);

template <VXLIB_CONVOLVE_TYPENAME_I8U_C16S_O16S>
static VXLIB_STATUS
VXLIB_convolve_5x5_exec_ci(VXLIB_kernelHandle handle, void *restrict pIn, void *restrict pFilter, void *restrict pOut);

// this method performs 5x5 convolve filter computation
template <typename dTypeIn, typename dTypeFilter, typename dTypeOut>
static VXLIB_STATUS
VXLIB_convolve_7x7_exec_ci(VXLIB_kernelHandle handle, void *restrict pIn, void *restrict pFilter, void *restrict pOut)
{
   // set status
   VXLIB_STATUS status = VXLIB_SUCCESS;

   // get kernel handle
   VXLIB_convolve_PrivArgs *pKerPrivArgs = (VXLIB_convolve_PrivArgs *) handle;

   // SE/SA params template
   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;
   __SA_TEMPLATE_v1 sa0Params;

   // create local pointers
   dTypeIn *restrict pInLocal         = (dTypeIn *) pIn;
   dTypeFilter *restrict pFilterLocal = (dTypeFilter *) pFilter;
   dTypeOut *restrict pOutLocal       = (dTypeOut *) pOut;



   uint8_t *pBlock    = pKerPrivArgs->bufPblock;
   size_t   elemCount = c7x::element_count_of<c7x::uchar_hvec>::value;

   // get SE/SA params
   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET);
   se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET);
   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET);

   // get init parameters
   size_t filterWidth  = pKerPrivArgs->pKerInitArgs.filterWidth;
   size_t filterHeight = pKerPrivArgs->pKerInitArgs.filterHeight;
   size_t scale        = pKerPrivArgs->pKerInitArgs.scale;

   size_t width  = pKerPrivArgs->width;
   size_t height = pKerPrivArgs->height;

   // compute loop iteration counts
   size_t wBlocks = VXLIB_ceilingDiv(width, (elemCount / 2));
   size_t hBlocks = height - filterHeight + 1;

   c7x::int_vec vShift = (c7x::int_vec) scale;

   c7x::short_vec vCoeff0_lo, vCoeff1_lo, vCoeff2_lo, vCoeff3_lo, vCoeff4_lo, vCoeff5_lo, vCoeff6_lo;
   c7x::short_vec vCoeff0_hi, vCoeff1_hi, vCoeff2_hi, vCoeff3_hi, vCoeff4_hi, vCoeff5_hi, vCoeff6_hi;

   c7x::int_vec vRes0, vRes1, vRes2, vRes3, vRes4, vRes5, vRes6;
   c7x::int_vec vAcc1, vAcc2, vAcc3, vAcc4, vAcc5, vAcc6, vRes;

   // open streams
   __SE0_OPEN(pInLocal, se0Params);
   __SE1_OPEN(pFilterLocal + (filterHeight * filterWidth), se1Params);
   __SA0_OPEN(sa0Params);

   // fetch filter coeffs from SE1
   vCoeff0_lo = c7x::strm_eng<1, c7x::short_vec>::get_adv();
   vCoeff0_hi = c7x::strm_eng<1, c7x::short_vec>::get_adv();

   vCoeff1_lo = c7x::strm_eng<1, c7x::short_vec>::get_adv();
   vCoeff1_hi = c7x::strm_eng<1, c7x::short_vec>::get_adv();

   vCoeff2_lo = c7x::strm_eng<1, c7x::short_vec>::get_adv();
   vCoeff2_hi = c7x::strm_eng<1, c7x::short_vec>::get_adv();

   vCoeff3_lo = c7x::strm_eng<1, c7x::short_vec>::get_adv();
   vCoeff3_hi = c7x::strm_eng<1, c7x::short_vec>::get_adv();

   vCoeff4_lo = c7x::strm_eng<1, c7x::short_vec>::get_adv();
   vCoeff4_hi = c7x::strm_eng<1, c7x::short_vec>::get_adv();

   vCoeff5_lo = c7x::strm_eng<1, c7x::short_vec>::get_adv();
   vCoeff5_hi = c7x::strm_eng<1, c7x::short_vec>::get_adv();

   vCoeff6_lo = c7x::strm_eng<1, c7x::short_vec>::get_adv();
   vCoeff6_hi = c7x::strm_eng<1, c7x::short_vec>::get_adv();

   for (int32_t wCounter = 0; wCounter < wBlocks; wCounter++) {
      for (int32_t hCounter = 0; hCounter < hBlocks; hCounter++) {

         // vfir8 row 0, se0adv
         vRes0 = __vfir8hw_wvv(vCoeff0_lo, vCoeff0_hi, __SE_REG_0_ADV);

         // vfir8 row 1, se0adv
         vRes1 = __vfir8hw_wvv(vCoeff1_lo, vCoeff1_hi, __SE_REG_0_ADV);

         // sum 0-1
         vAcc1 = vRes0 + vRes1;

         // vfir8 row 2, se0adv
         vRes2 = __vfir8hw_wvv(vCoeff2_lo, vCoeff2_hi, __SE_REG_0_ADV);

         // sum 0-2
         vAcc2 = vAcc1 + vRes2;

         // vfir8 row 3, se0adv
         vRes3 = __vfir8hw_wvv(vCoeff3_lo, vCoeff3_hi, __SE_REG_0_ADV);

         // sum 0-3
         vAcc3 = vAcc2 + vRes3;

         // vfir8 row 4, se0adv
         vRes4 = __vfir8hw_wvv(vCoeff4_lo, vCoeff4_hi, __SE_REG_0_ADV);

         // sum 0-4
         vAcc4 = vAcc3 + vRes4;

         // vfir8 row 5, se0adv
         vRes5 = __vfir8hw_wvv(vCoeff5_lo, vCoeff5_hi, __SE_REG_0_ADV);

         // sum 0-5
         vAcc5 = vAcc4 + vRes5;

         // vfir8 row 6, se0adv
         vRes6 = __vfir8hw_wvv(vCoeff6_lo, vCoeff6_hi, __SE_REG_0_ADV);

         // sum 0-6
         vAcc6 = vAcc5 + vRes6;

         // shift right by scale factor
         vRes = __shift_right(vAcc6, vShift);

         // predicated store result (.D2/1)

         if (VXLIB_CONVOLVE_I8U_C16S_O8U_TEMPLATE(dTypeOut)) {

            // saturate from 32b signed to 8b unsigned
            c7x::uint_vec vSatU = __saturate_s2u(vRes, (c7x::uchar_qvec) 8);
            // predicated store
            __vpred tmp = c7x::strm_agen<0, c7x::uint_vec>::get_vpred();

            c7x::uchar_qvec *addr = c7x::strm_agen<0, c7x::uchar_qvec>::get_adv(pOutLocal);
            __vstore_pred_pack_byte(tmp, addr, vSatU);
         }

         else {

            // saturate from 32b signed to 16b signed
            c7x::int_vec vSat = __saturate(vRes, (c7x::uchar_qvec) 16);

            // predicated store
            __vpred tmp = c7x::strm_agen<0, c7x::int_vec>::get_vpred();

            c7x::short_hvec *addr = c7x::strm_agen<0, c7x::short_hvec>::get_adv(pOutLocal);

            __vstore_pred_packl(tmp, addr, vSat);
         }

      }
   }

   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA0_CLOSE();

   return status;
}

template <VXLIB_CONVOLVE_TYPENAME_I8U_C16S_O8U>
static VXLIB_STATUS
VXLIB_convolve_7x7_exec_ci(VXLIB_kernelHandle handle, void *restrict pIn, void *restrict pFilter, void *restrict pOut);

template <VXLIB_CONVOLVE_TYPENAME_I8U_C16S_O16S>
static VXLIB_STATUS
VXLIB_convolve_7x7_exec_ci(VXLIB_kernelHandle handle, void *restrict pIn, void *restrict pFilter, void *restrict pOut);

template <typename dTypeIn, typename dTypeFilter, typename dTypeOut>
static VXLIB_STATUS
VXLIB_convolve_9x9_exec_ci(VXLIB_kernelHandle handle, void *restrict pIn, void *restrict pFilter, void *restrict pOut)
{
   // set status
   VXLIB_STATUS status = VXLIB_SUCCESS;

   // get kernel handle
   VXLIB_convolve_PrivArgs *pKerPrivArgs = (VXLIB_convolve_PrivArgs *) handle;

   // SE/SA params template
   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;
   __SE_TEMPLATE_v1 se1bParams;
   __SA_TEMPLATE_v1 sa0Params;
   __SA_TEMPLATE_v1 sa1Params;

   // get init parameters
   size_t filterWidth  = pKerPrivArgs->pKerInitArgs.filterWidth;
   size_t filterHeight = pKerPrivArgs->pKerInitArgs.filterHeight;
   size_t scale        = pKerPrivArgs->pKerInitArgs.scale;

   size_t width  = pKerPrivArgs->width;
   size_t height = pKerPrivArgs->height;

   // create local pointers
   dTypeIn *restrict pInLocal             = (dTypeIn *) pIn;
   dTypeFilter *restrict pFilterLocal     = (dTypeFilter *) pFilter;
   dTypeOut *restrict pOutLocal           = (dTypeOut *) pOut;
   dTypeFilter *restrict pFilterLastLocal = (dTypeFilter *) (pFilterLocal + (filterHeight - 1) * filterWidth);



   uint8_t *pBlock    = pKerPrivArgs->bufPblock;
   size_t   elemCount = c7x::element_count_of<c7x::uchar_hvec>::value;

   // get SE/SA params
   se0Params  = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET);
   se1Params  = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET);
   se1bParams = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1B_PARAM_OFFSET);
   sa0Params  = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET);
   sa1Params  = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA1_PARAM_OFFSET);

   // compute loop iteration counts
   size_t wBlocks = VXLIB_ceilingDiv(width, (elemCount / 2));
   size_t hBlocks = height - filterHeight + 1;

   c7x::int_vec vShift = (c7x::int_vec) scale;

   c7x::short_vec vCoeff0_lo, vCoeff1_lo, vCoeff2_lo, vCoeff3_lo, vCoeff4_lo, vCoeff5_lo, vCoeff6_lo, vCoeff7_lo, vCoeff8_lo;
   c7x::short_vec vCoeff0_hi, vCoeff1_hi, vCoeff2_hi, vCoeff3_hi, vCoeff4_hi, vCoeff5_hi, vCoeff6_hi, vCoeff7_hi, vCoeff8_hi;

   c7x::int_vec vCoeff0_last, vCoeff1_last, vCoeff2_last, vCoeff3_last, vCoeff4_last, vCoeff5_last, vCoeff6_last,
       vCoeff7_last, vCoeff8_last;

   c7x::int_vec vRes0, vRes1, vRes2, vRes3, vRes4, vRes5, vRes6, vRes7, vRes8;
   c7x::int_vec vAcc1, vAcc2, vAcc3, vAcc4, vAcc5, vAcc6, vAcc7, vAcc8, vRes;

   // open streams
   __SE0_OPEN(pInLocal, se0Params);
   __SE1_OPEN(pFilterLocal + (filterWidth * filterHeight), se1bParams);
   __SA0_OPEN(sa0Params);
   __SA1_OPEN(sa1Params);

   // fetch filter coeffs from SE1
   vCoeff0_lo = c7x::strm_eng<1, c7x::short_vec>::get_adv();
   vCoeff0_hi = c7x::strm_eng<1, c7x::short_vec>::get_adv();

   vCoeff1_lo = c7x::strm_eng<1, c7x::short_vec>::get_adv();
   vCoeff1_hi = c7x::strm_eng<1, c7x::short_vec>::get_adv();

   vCoeff2_lo = c7x::strm_eng<1, c7x::short_vec>::get_adv();
   vCoeff2_hi = c7x::strm_eng<1, c7x::short_vec>::get_adv();

   vCoeff3_lo = c7x::strm_eng<1, c7x::short_vec>::get_adv();
   vCoeff3_hi = c7x::strm_eng<1, c7x::short_vec>::get_adv();

   vCoeff4_lo = c7x::strm_eng<1, c7x::short_vec>::get_adv();
   vCoeff4_hi = c7x::strm_eng<1, c7x::short_vec>::get_adv();

   vCoeff5_lo = c7x::strm_eng<1, c7x::short_vec>::get_adv();
   vCoeff5_hi = c7x::strm_eng<1, c7x::short_vec>::get_adv();

   vCoeff6_lo = c7x::strm_eng<1, c7x::short_vec>::get_adv();
   vCoeff6_hi = c7x::strm_eng<1, c7x::short_vec>::get_adv();

   vCoeff7_lo = c7x::strm_eng<1, c7x::short_vec>::get_adv();
   vCoeff7_hi = c7x::strm_eng<1, c7x::short_vec>::get_adv();

   vCoeff8_lo = c7x::strm_eng<1, c7x::short_vec>::get_adv();
   vCoeff8_hi = c7x::strm_eng<1, c7x::short_vec>::get_adv();

   __SE1_CLOSE();
   __SE1_OPEN(pInLocal + 8, se1Params);

   for (int32_t wCounter = 0; wCounter < wBlocks; wCounter++) {
      for (int32_t hCounter = 0; hCounter < hBlocks; hCounter++) {

         // fetch last coefficients from each row
         dTypeFilter *addr0 = c7x::strm_agen<1, dTypeFilter>::get_adv(pFilterLastLocal);
         vCoeff0_last       = __duplicate((int32_t) *addr0);
         dTypeFilter *addr1 = c7x::strm_agen<1, dTypeFilter>::get_adv(pFilterLastLocal);
         vCoeff1_last       = __duplicate((int32_t) (*addr1));

         dTypeFilter *addr2 = c7x::strm_agen<1, dTypeFilter>::get_adv(pFilterLastLocal);
         vCoeff2_last       = __duplicate((int32_t) (*addr2));

         dTypeFilter *addr3 = c7x::strm_agen<1, dTypeFilter>::get_adv(pFilterLastLocal);
         vCoeff3_last       = __duplicate((int32_t) (*addr3));

         dTypeFilter *addr4 = c7x::strm_agen<1, dTypeFilter>::get_adv(pFilterLastLocal);
         vCoeff4_last       = __duplicate((int32_t) (*addr4));

         dTypeFilter *addr5 = c7x::strm_agen<1, dTypeFilter>::get_adv(pFilterLastLocal);
         vCoeff5_last       = __duplicate((int32_t) (*addr5));

         dTypeFilter *addr6 = c7x::strm_agen<1, dTypeFilter>::get_adv(pFilterLastLocal);
         vCoeff6_last       = __duplicate((int32_t) (*addr6));

         dTypeFilter *addr7 = c7x::strm_agen<1, dTypeFilter>::get_adv(pFilterLastLocal);
         vCoeff7_last       = __duplicate((int32_t) (*addr7));

         dTypeFilter *addr8 = c7x::strm_agen<1, dTypeFilter>::get_adv(pFilterLastLocal);
         vCoeff8_last       = __duplicate((int32_t) (*addr8));

         // vfir8 row 0, se0adv
         c7x::int_vec v0_0 = __vfir8hw_wvv(vCoeff0_lo, vCoeff0_hi, __SE_REG_0_ADV);
         c7x::int_vec v0_1 = c7x::strm_eng<1, c7x::int_vec>::get_adv();

         vRes0 = v0_0 + v0_1 * vCoeff0_last;

         // vfir8 row 1, se0adv
         c7x::int_vec v1_0 = __vfir8hw_wvv(vCoeff1_lo, vCoeff1_hi, __SE_REG_0_ADV);
         c7x::int_vec v1_1 = c7x::strm_eng<1, c7x::int_vec>::get_adv();

         vRes1 = v1_0 + v1_1 * vCoeff1_last;

         // sum 0-1
         vAcc1 = vRes0 + vRes1;

         // vfir8 row 2, se0adv
         c7x::int_vec v2_0 = __vfir8hw_wvv(vCoeff2_lo, vCoeff2_hi, __SE_REG_0_ADV);
         c7x::int_vec v2_1 = c7x::strm_eng<1, c7x::int_vec>::get_adv();

         vRes2 = v2_0 + v2_1 * vCoeff2_last;

         // sum 0-2
         vAcc2 = vAcc1 + vRes2;

         // vfir8 row 3, se0adv
         c7x::int_vec v3_0 = __vfir8hw_wvv(vCoeff3_lo, vCoeff3_hi, __SE_REG_0_ADV);
         c7x::int_vec v3_1 = c7x::strm_eng<1, c7x::int_vec>::get_adv();

         vRes3 = v3_0 + v3_1 * vCoeff3_last;

         // sum 0-3
         vAcc3 = vAcc2 + vRes3;

         // vfir8 row 4, se0adv
         c7x::int_vec v4_0 = __vfir8hw_wvv(vCoeff4_lo, vCoeff4_hi, __SE_REG_0_ADV);
         c7x::int_vec v4_1 = c7x::strm_eng<1, c7x::int_vec>::get_adv();
         vRes4        = v4_0 + v4_1 * vCoeff4_last;

         // sum 0-4
         vAcc4 = vAcc3 + vRes4;

         // vfir8 row 5, se0adv
         c7x::int_vec v5_0 = __vfir8hw_wvv(vCoeff5_lo, vCoeff5_hi, __SE_REG_0_ADV);
         c7x::int_vec v5_1 = c7x::strm_eng<1, c7x::int_vec>::get_adv();
         vRes5        = v5_0 + v5_1 * vCoeff5_last;

         // sum 0-5
         vAcc5 = vAcc4 + vRes5;

         // vfir8 row 6, se0adv
         c7x::int_vec v6_0 = __vfir8hw_wvv(vCoeff6_lo, vCoeff6_hi, __SE_REG_0_ADV);
         c7x::int_vec v6_1 = c7x::strm_eng<1, c7x::int_vec>::get_adv();
         vRes6        = v6_0 + v6_1 * vCoeff6_last;
         // sum 0-6
         vAcc6 = vAcc5 + vRes6;

         // vfir8 row 7, se0adv
         c7x::int_vec v7_0 = __vfir8hw_wvv(vCoeff7_lo, vCoeff7_hi, __SE_REG_0_ADV);
         c7x::int_vec v7_1 = c7x::strm_eng<1, c7x::int_vec>::get_adv();
         vRes7        = v7_0 + v7_1 * vCoeff7_last;

         // sum 0-7
         vAcc7 = vAcc6 + vRes7;

         // vfir8 row 8, se0adv
         c7x::int_vec v8_0 = __vfir8hw_wvv(vCoeff8_lo, vCoeff8_hi, __SE_REG_0_ADV);
         c7x::int_vec v8_1 = c7x::strm_eng<1, c7x::int_vec>::get_adv();
         vRes8        = v8_0 + v8_1 * vCoeff8_last;

         // sum 0-8
         vAcc8 = vAcc7 + vRes8;

         // shift right by scale factor
         vRes = __shift_right(vAcc8, vShift);

         // predicated store result (.D2/1)

         if (VXLIB_CONVOLVE_I8U_C16S_O8U_TEMPLATE(dTypeOut)) {

            // saturate from 32b signed to 8b unsigned
            c7x::uint_vec vSatU = __saturate_s2u(vRes, (c7x::uchar_qvec) 8);
            // predicated store
            __vpred tmp = c7x::strm_agen<0, c7x::uint_vec>::get_vpred();

            c7x::uchar_qvec *addr = c7x::strm_agen<0, c7x::uchar_qvec>::get_adv(pOutLocal);
            __vstore_pred_pack_byte(tmp, addr, vSatU);
         }

         else {

            // saturate from 32b signed to 16b signed
            c7x::int_vec vSat = __saturate(vRes, (c7x::uchar_qvec) 16);

            // predicated store
            __vpred tmp = c7x::strm_agen<0, c7x::int_vec>::get_vpred();

            c7x::short_hvec *addr = c7x::strm_agen<0, c7x::short_hvec>::get_adv(pOutLocal);

            __vstore_pred_packl(tmp, addr, vSat);
         }
      }
   }

   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA0_CLOSE();
   __SA1_CLOSE();

   return status;
}

template <VXLIB_CONVOLVE_TYPENAME_I8U_C16S_O8U>
static VXLIB_STATUS
VXLIB_convolve_9x9_exec_ci(VXLIB_kernelHandle handle, void *restrict pIn, void *restrict pFilter, void *restrict pOut);

template <VXLIB_CONVOLVE_TYPENAME_I8U_C16S_O16S>
static VXLIB_STATUS
VXLIB_convolve_9x9_exec_ci(VXLIB_kernelHandle handle, void *restrict pIn, void *restrict pFilter, void *restrict pOut);

// this method is the top level module for k x k convolve filter operation on an input image
template <typename dTypeIn, typename dTypeFilter, typename dTypeOut>
VXLIB_STATUS
VXLIB_convolve_exec_ci(VXLIB_kernelHandle handle, void *restrict pIn, void *restrict pFilter, void *restrict pOut)
{

   VXLIB_STATUS status = VXLIB_SUCCESS;

   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_convolve_PrivArgs *pKerPrivArgs = (VXLIB_convolve_PrivArgs *) handle;

   size_t filterHeight = pKerPrivArgs->pKerInitArgs.filterHeight;
   size_t filterWidth  = pKerPrivArgs->pKerInitArgs.filterWidth;

   // call 3x3 convolve filter exec
   if ((filterHeight == 3) && (filterWidth == filterHeight)) {
      status = VXLIB_convolve_3x3_exec_ci<dTypeIn, dTypeFilter, dTypeOut>(handle, pIn, pFilter, pOut);
   }

   // call 5x5 exec
   else if ((filterHeight == 5) && (filterWidth == filterHeight)) {
      status = VXLIB_convolve_5x5_exec_ci<dTypeIn, dTypeFilter, dTypeOut>(handle, pIn, pFilter, pOut);
   }

   // call 7x7 exec
   else if ((filterHeight == 7) && (filterWidth == filterHeight)) {
      status = VXLIB_convolve_7x7_exec_ci<dTypeIn, dTypeFilter, dTypeOut>(handle, pIn, pFilter, pOut);
   }

   // call 9x9 exec
   else if ((filterHeight == 9) && (filterWidth == filterHeight)) {
      status = VXLIB_convolve_9x9_exec_ci<dTypeIn, dTypeFilter, dTypeOut>(handle, pIn, pFilter, pOut);
   }

   // call generic exec
   else {

      // SE/SA params template
      __SE_TEMPLATE_v1 se0Params;
      __SE_TEMPLATE_v1 se1Params;
      __SA_TEMPLATE_v1 sa0Params;

      // create local pointers
      dTypeIn *restrict pInLocal         = (dTypeIn *) pIn;
      dTypeFilter *restrict pFilterLocal = (dTypeFilter *) pFilter;
      dTypeOut *restrict pOutLocal       = (dTypeOut *) pOut;



      uint8_t *pBlock    = pKerPrivArgs->bufPblock;
      size_t   elemCount = c7x::element_count_of<c7x::uchar_hvec>::value;

      // get SE/SA params
      se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET);
      se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET);
      sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET);

      // get init parameters

      size_t scale        = pKerPrivArgs->pKerInitArgs.scale;

      size_t width  = pKerPrivArgs->width;
      size_t height = pKerPrivArgs->height;

      // compute loop iteration counts
      size_t wBlocks = VXLIB_ceilingDiv(width, elemCount);
      size_t hBlocks = height - filterHeight + 1;

      c7x::int_vec vShift = (c7x::int_vec) scale;

      c7x::short_vec vCoeff, vIn;
      c7x::int_vec   vRes_lo, vRes_hi;
      c7x::int_vec   vSum_lo, vSum_hi;

      // open streams
      __SE0_OPEN(pInLocal, se0Params);
      // fetch filter coeffs from SE1
      __SE1_OPEN(pFilterLocal + (filterWidth * filterHeight), se1Params); // access filter in reverse
      __SA0_OPEN(sa0Params);

      for (int32_t wCounter = 0; wCounter < wBlocks; wCounter++) {

         for (int32_t hCounter = 0; hCounter < hBlocks; hCounter++) {

            vSum_lo = 0;
            vSum_hi = 0;

            for (int32_t fCounter = 0; fCounter < (filterWidth * filterHeight); fCounter++) {

               vIn    = c7x::strm_eng<0, c7x::short_vec>::get_adv();
               vCoeff = c7x::strm_eng<1, c7x::short_vec>::get_adv();

               __vmpyhw_vvw(vIn, vCoeff, vRes_lo, vRes_hi);

               vSum_lo = vSum_lo + vRes_lo;
               vSum_hi = vSum_hi + vRes_hi;
            }

            vSum_lo = __shift_right(vSum_lo, vShift);
            vSum_hi = __shift_right(vSum_hi, vShift);

            if (VXLIB_CONVOLVE_I8U_C16S_O8U_TEMPLATE(dTypeOut)) {

               c7x::short_vec  vSat  = __pack_sat(vSum_hi, vSum_lo);
               c7x::ushort_vec vSatU = __saturate_s2u(vSat, (c7x::uchar_hvec) 8);

               __vpred     tmp  = c7x::strm_agen<0, c7x::ushort_vec>::get_vpred();
               c7x::uchar_hvec *addr = c7x::strm_agen<0, c7x::uchar_hvec>::get_adv(pOutLocal);

               // predicated pack low byte store, 2 src
               __vstore_pred_packl(tmp, addr, vSatU);
            }

            else {

               // saturate from 32b signed to 16b signed limits

               c7x::short_vec  vSat = __pack_sat(vSum_hi, vSum_lo);
               __vpred    tmp  = c7x::strm_agen<0, c7x::short_vec>::get_vpred();
               c7x::short_vec *addr = c7x::strm_agen<0, c7x::short_vec>::get_adv(pOutLocal);
               // predicated store
               __vstore_pred(tmp, addr, vSat);
            }
         }
      }

      __SE0_CLOSE();
      __SE1_CLOSE();
      __SA0_CLOSE();
   }

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* Explicit instantiations for VXLIB_convolve_exec_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

template VXLIB_STATUS VXLIB_convolve_exec_ci<VXLIB_CONVOLVE_TYPENAME_I8U_C16S_O8U>(VXLIB_kernelHandle handle,
                                                                                   void *restrict pIn,
                                                                                   void *restrict pFilter,
                                                                                   void *restrict pOut);

template VXLIB_STATUS VXLIB_convolve_exec_ci<VXLIB_CONVOLVE_TYPENAME_I8U_C16S_O16S>(VXLIB_kernelHandle handle,
                                                                                    void *restrict pIn,
                                                                                    void *restrict pFilter,
                                                                                    void *restrict pOut);

void VXLIB_convolve_perfEst(VXLIB_kernelHandle handle, size_t *archCycles, size_t *estCycles)
{

   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_convolve_PrivArgs *pKerPrivArgs = (VXLIB_convolve_PrivArgs *) handle;

   // obtain loop count for compute loop
   size_t numBlocks = pKerPrivArgs->numBlocks;
   size_t Fr        = pKerPrivArgs->pKerInitArgs.filterHeight;
   size_t Fc        = pKerPrivArgs->pKerInitArgs.filterWidth;

   size_t overheadCycles  = 17; // profiled code before entering compute loop
   size_t iterConst = 0;
   size_t ii        = 0;

   if ((Fr == Fc) && (Fr == 3)) {
      ii = 3;
      /* iterConst; */
   }

   else if ((Fr == Fc) && (Fr == 5)) {
      ii = 5;
      /* iterConst; */
   }

   else if ((Fr == Fc) && (Fr == 7)) {
      ii = 7;
      /* iterConst; */
   }

   else if ((Fr == Fc) && (Fr == 9)) {
      ii = 9;
      /* iterConst; */
   }

   else {
      ii = 1;
      /* iterConst; */
   }

   *archCycles = iterConst + numBlocks * ii; // obtained from asm
   *estCycles  = overheadCycles + *archCycles;
}
