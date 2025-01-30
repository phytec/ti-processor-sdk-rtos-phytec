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

#include "VXLIB_gaussian_priv.h"
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

#define VXLIB_GAUSSIANFILTER3x3_COEFF_SIZE (sizeof(uint64_t) * 8)

#define Q_BITS 15
#define Q_FACTOR (1 << Q_BITS)

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_gaussian_init_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method initializes the kernel-specific parameters
// mainly, the streaming engine and streaming address generators
template <uint32_t dTypeIn, uint32_t dTypeOut>
VXLIB_STATUS VXLIB_gaussian_init_ci(VXLIB_kernelHandle             handle,
                                    const VXLIB_bufParams2D_t     *bufParamsIn,
                                    const VXLIB_bufParams2D_t     *bufParamsOut,
                                    const VXLIB_gaussian_InitArgs *pKerInitArgs)
{
   VXLIB_STATUS status = VXLIB_SUCCESS; // assign status to success by default

   // structs to hold SE and SA parameters
   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1();
   __SE_TEMPLATE_v1 se1Params = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa0Params = __gen_SA_TEMPLATE_v1();

   // fetch + SE promote, store all half SIMD width
   __SE_ELETYPE SE_ELETYPE = c7x::se_eletype<c7x::uchar_vec>::value;
   __SE_VECLEN  SE_VECLEN  = c7x::se_veclen<c7x::uchar_hvec>::value;
   __SA_VECLEN  SA_VECLEN  = c7x::sa_veclen<c7x::uchar_hvec>::value;

   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_gaussian_PrivArgs *pKerPrivArgs = (VXLIB_gaussian_PrivArgs *) handle;

   // number of elements fetched from input each SE get
   size_t elemCount = c7x::element_count_of<c7x::char_hvec>::value;

   // get init args
   size_t width  = bufParamsIn->dim_x;
   size_t height = bufParamsIn->dim_y;
   size_t stride = bufParamsIn->stride_y;

   size_t filterSize = pKerInitArgs->filterSize;
   size_t padLeft    = pKerInitArgs->padLeft;
   size_t padRight   = pKerInitArgs->padRight;
   size_t padTop     = pKerInitArgs->padTop;
   size_t padBottom  = pKerInitArgs->padBottom;

   // determine non-padded vs padded implementation
   bool isNotPadded = (padLeft == 0) && (padRight == 0) && (padTop == 0) && (padBottom == 0);

   uint8_t *pBlock         = pKerPrivArgs->bufPblock;
   pKerPrivArgs->numBlocks = width * VXLIB_ceilingDiv(width, elemCount);

   // setup se/sa for k x k implementation
   if (filterSize == VXLIB_GAUSSIAN_FILTER_3x3) {

      // Creating gaussian filter 3x3 coefficient matrix
      // gaussianFilter3x3 = [[1, 2, 1], [2, 4, 2], [1, 2, 1]] * 1/16
      // gaussianFilter3x3 = gaussianFilter3x3 * Q_FACTOR
      int16_t gaussianFilter3x3[6] = {2048, 4096, 2048, 4096, 8192, 4096};

      // call 3x3 gaussian init non-padded implementation
      if (isNotPadded) {

         // set SE0 Params
         se0Params.DIMFMT  = __SE_DIMFMT_3D;
         se0Params.ELETYPE = SE_ELETYPE;
         se0Params.VECLEN  = SE_VECLEN;
         se0Params.PROMOTE = __SE_PROMOTE_2X_ZEROEXT;

         se0Params.ICNT0 = elemCount;

         se0Params.DIM1  = stride;
         se0Params.ICNT1 = (filterSize - 1) + VXLIB_ceilingDiv(bufParamsOut->dim_y, filterSize) * filterSize;

         se0Params.DIM2  = elemCount;
         se0Params.ICNT2 = VXLIB_ceilingDiv(width, elemCount);

         // input column dimension DECDIM
         se0Params.DECDIM1       = __SE_DECDIM_DIM2;
         se0Params.DECDIM1_WIDTH = width;
         se0Params.DECDIM1SD     = __SE_DECDIMSD_DIM0;

         // input row dimension DECDIM
         se0Params.DECDIM2       = __SE_DECDIM_DIM1;
         se0Params.DECDIM2_WIDTH = height * stride;

         // set SE1 Params
         se1Params.DIMFMT  = __SE_DIMFMT_3D;
         se1Params.ELETYPE = SE_ELETYPE;
         se1Params.VECLEN  = SE_VECLEN;
         se1Params.PROMOTE = __SE_PROMOTE_2X_ZEROEXT;

         se1Params.ICNT0 = (width <= elemCount) ? 0 : elemCount;

         se1Params.DIM1  = stride;
         se1Params.ICNT1 = (filterSize - 1) + VXLIB_ceilingDiv(bufParamsOut->dim_y, filterSize) * filterSize;

         se1Params.DIM2  = elemCount;
         se1Params.ICNT2 = VXLIB_ceilingDiv(width, elemCount);

         // input column dimension DECDIM
         se1Params.DECDIM1       = __SE_DECDIM_DIM2;
         se1Params.DECDIM1_WIDTH = width - elemCount;
         se1Params.DECDIM1SD     = __SE_DECDIMSD_DIM0;

         // input row dimension DECDIM
         se1Params.DECDIM2       = __SE_DECDIM_DIM1;
         se1Params.DECDIM2_WIDTH = height * stride;

         // set SA0 Params
         sa0Params.DIMFMT = __SA_DIMFMT_3D;
         sa0Params.VECLEN = SA_VECLEN;

         sa0Params.ICNT0 = (bufParamsOut->dim_x < elemCount) ? bufParamsOut->dim_x : elemCount;
         sa0Params.DIM1  = pKerPrivArgs->strideOutElements;
         sa0Params.ICNT1 = VXLIB_ceilingDiv(bufParamsOut->dim_y, filterSize) * filterSize;

         sa0Params.DIM2  = elemCount;
         sa0Params.ICNT2 = VXLIB_ceilingDiv(bufParamsOut->dim_x, elemCount);

         // output column dimension DECDIM
         sa0Params.DECDIM1       = __SA_DECDIM_DIM2;
         sa0Params.DECDIM1_WIDTH = bufParamsOut->dim_x;
         sa0Params.DECDIM1SD     = __SA_DECDIMSD_DIM0;

         // output row dimension DECDIM
         sa0Params.DECDIM2       = __SA_DECDIM_DIM1;
         sa0Params.DECDIM2_WIDTH = bufParamsOut->dim_y * bufParamsOut->stride_y;

         // store gaussian filter 3x3 coefficients
         // 1st and 3rd rows of matrix are same, only store once

         /* 0                     64b */
         /* +-----------------------+ */
         /* | 2048 | 4096 | 2048 |  0  | */
         /* +-----------------------+ */

         /* 0                     64b */
         /* +-----------------------+ */
         /* | 4096 | 8192 | 4096 |  0  | */
         /* +-----------------------+ */

         // Store first row of coefficient matrix
         uint64_t filter = gaussianFilter3x3[2];
         filter          = (filter << 16LLU) | (uint64_t)gaussianFilter3x3[1];
         filter          = (filter << 16LLU) | (uint64_t)gaussianFilter3x3[0];

         size_t filter_offset = SA0_PARAM_OFFSET + VXLIB_GAUSSIANFILTER3x3_COEFF_SIZE;

         *(uint64_t *) ((uint8_t *) pBlock + filter_offset) = filter;

         // Store second row of the coefficient matrix
         filter = gaussianFilter3x3[5];
         filter = (filter << 16LLU) | (uint64_t)gaussianFilter3x3[4];
         filter = (filter << 16LLU) | (uint64_t)gaussianFilter3x3[3];

         filter_offset = filter_offset + VXLIB_GAUSSIANFILTER3x3_COEFF_SIZE;

         *(uint64_t *) ((uint8_t *) pBlock + filter_offset) = filter;
      }

      // return error for padded version; not implemented
      else {

         status = VXLIB_ERR_NOT_IMPLEMENTED;
      }
   }

   else if (filterSize == VXLIB_GAUSSIAN_FILTER_5x5) {
      int16_t gaussianFilter5x5[15] = {1, 4, 6, 4, 1, 4, 16, 24, 16, 4, 6, 24, 36, 24, 6};

      // call 3x3 gaussian init non-padded implementation
      if (isNotPadded) {
         size_t elemCountOut = c7x::element_count_of<c7x::uchar_qvec>::value;

         // set SE0 Params
         se0Params.DIMFMT  = __SE_DIMFMT_4D;
         se0Params.ELETYPE = SE_ELETYPE;
         se0Params.VECLEN  = SE_VECLEN;
         se0Params.PROMOTE = __SE_PROMOTE_2X_ZEROEXT;

         // 0: number of elements to be computed per SE fetch
         se0Params.ICNT0 = elemCount;

         // 1: number of accumulation rows in single convolution window
         se0Params.DIM1  = stride;
         se0Params.ICNT1 = filterSize;

         // 2: vertical iterations
         se0Params.DIM2  = stride;
         se0Params.ICNT2 = height - filterSize + 1;

         // 3: horizontal iterations
         se0Params.DIM3  = elemCount / 2; // vfir8 is half-vector fetch, quarter-vec adv
         se0Params.ICNT3 = VXLIB_ceilingDiv(width, (elemCount / 2));

         // input column dimension DECDIM
         se0Params.DECDIM1       = __SE_DECDIM_DIM3;
         se0Params.DECDIM1_WIDTH = width;
         se0Params.DECDIM1SD     = __SE_DECDIMSD_DIM0;

         // set SA0 Params
         sa0Params.DIMFMT = __SA_DIMFMT_3D;
         sa0Params.VECLEN = SA_VECLEN;

         // 0: number of elements to store per SE fetch
         sa0Params.ICNT0 = elemCountOut;

         // 1: vertical iterations (1 store per convolution window)
         sa0Params.DIM1  = pKerPrivArgs->strideOutElements; // SA DIM jumps are given in num elements
         sa0Params.ICNT1 = bufParamsOut->dim_y;

         // 2: horizontal iterations
         sa0Params.DIM2  = elemCountOut;
         sa0Params.ICNT2 = VXLIB_ceilingDiv(bufParamsOut->dim_x, elemCountOut);

         // output column dimension DECDIM
         sa0Params.DECDIM1       = __SA_DECDIM_DIM2;
         sa0Params.DECDIM1_WIDTH = bufParamsOut->dim_x;
         sa0Params.DECDIM1SD     = __SA_DECDIMSD_DIM0;

         // store gaussian filter 5x5 coefficients
         // 1st row of coefficients split into low and high for VFIR8

         /* 0                     64b */
         /* +-----------------------+ */
         /* |  1  |  4  |  6  |  4  | */
         /* +-----------------------+ */

         /* 0                     64b */
         /* +-----------------------+ */
         /* |  1  |  0  |  0  |  0  | */
         /* +-----------------------+ */

         // Store first row (low) of coefficient matrix
         uint64_t filter = gaussianFilter5x5[3];
         filter          = (filter << 16LLU) | (uint64_t)gaussianFilter5x5[2];
         filter          = (filter << 16LLU) | (uint64_t)gaussianFilter5x5[1];
         filter          = (filter << 16LLU) | (uint64_t)gaussianFilter5x5[0];

         size_t filter_offset = SA0_PARAM_OFFSET + VXLIB_GAUSSIANFILTER3x3_COEFF_SIZE;

         *(uint64_t *) ((uint8_t *) pBlock + filter_offset) = filter;

         // high
         filter        = gaussianFilter5x5[4];
         filter_offset = filter_offset + VXLIB_GAUSSIANFILTER3x3_COEFF_SIZE;

         *(uint64_t *) ((uint8_t *) pBlock + filter_offset) = filter;

         // 2nd row of coefficients split into low and high for VFIR8

         /* 0                     64b */
         /* +--------------------------+ */
         /* |  4  |  16  |  24  |  16  | */
         /* +--------------------------+ */

         /* 0                     64b */
         /* +-----------------------+ */
         /* |  4  |  0  |  0  |  0  | */
         /* +-----------------------+ */

         // Store second row (low) of the coefficient matrix
         filter = gaussianFilter5x5[8];
         filter = (filter << 16LLU) | (uint64_t)gaussianFilter5x5[7];
         filter = (filter << 16LLU) | (uint64_t)gaussianFilter5x5[6];
         filter = (filter << 16LLU) | (uint64_t)gaussianFilter5x5[5];

         filter_offset = filter_offset + VXLIB_GAUSSIANFILTER3x3_COEFF_SIZE;

         *(uint64_t *) ((uint8_t *) pBlock + filter_offset) = filter;

         // high
         filter        = gaussianFilter5x5[9];
         filter_offset = filter_offset + VXLIB_GAUSSIANFILTER3x3_COEFF_SIZE;

         *(uint64_t *) ((uint8_t *) pBlock + filter_offset) = filter;

         // 3rd row of coefficients split into low and high for VFIR8

         /* 0                     64b */
         /* +--------------------------+ */
         /* |  6  |  24  |  36  |  24  | */
         /* +--------------------------+ */

         /* 0                     64b */
         /* +-----------------------+ */
         /* |  6  |  0  |  0  |  0  | */
         /* +-----------------------+ */

         // Store third row (low) of the coefficient matrix
         filter = gaussianFilter5x5[13];
         filter = (filter << 16LLU) | (uint64_t)gaussianFilter5x5[12];
         filter = (filter << 16LLU) | (uint64_t)gaussianFilter5x5[11];
         filter = (filter << 16LLU) | (uint64_t)gaussianFilter5x5[10];

         filter_offset = filter_offset + VXLIB_GAUSSIANFILTER3x3_COEFF_SIZE;

         *(uint64_t *) ((uint8_t *) pBlock + filter_offset) = filter;

         // high
         filter        = gaussianFilter5x5[14];
         filter_offset = filter_offset + VXLIB_GAUSSIANFILTER3x3_COEFF_SIZE;

         *(uint64_t *) ((uint8_t *) pBlock + filter_offset) = filter;

         // 4th and 5th row not stored as same as 1 and 2 of coefficient matrix
      }

      // return error for padded version; not implemented
      else {

         status = VXLIB_ERR_NOT_IMPLEMENTED;
      }
   }

   else {
      // return error for any other filter size (only 3x3 supported for now)
      status = VXLIB_ERR_NOT_IMPLEMENTED;
   }

   // store SE and SA params
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET) = se0Params;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET) = se1Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET) = sa0Params;

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* Explicit instantiations for VXLIB_gaussian_init_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

template VXLIB_STATUS VXLIB_gaussian_init_ci<VXLIB_GAUSSIAN_DTYPE_I8U_O8U>(VXLIB_kernelHandle             handle,
                                                                           const VXLIB_bufParams2D_t     *bufParamsIn,
                                                                           const VXLIB_bufParams2D_t     *bufParamsOut,
                                                                           const VXLIB_gaussian_InitArgs *pKerInitArgs);

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_gaussian_exec_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

template <typename dTypeIn, typename dTypeOut>
void VXLIB_gaussian_5x5_exec_ci(VXLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   // get kernel handle
   VXLIB_gaussian_PrivArgs *pKerPrivArgs = (VXLIB_gaussian_PrivArgs *) handle;

   // SE/SA params template
   __SE_TEMPLATE_v1 se0Params;
   __SA_TEMPLATE_v1 sa0Params;

   // create local pointers
   dTypeIn *restrict pInLocal   = (dTypeIn *) pIn;
   dTypeOut *restrict pOutLocal = (dTypeOut *) pOut;

   // define vector types


   uint8_t *pBlock       = pKerPrivArgs->bufPblock;
   size_t   filterOffset = SA0_PARAM_OFFSET + VXLIB_GAUSSIANFILTER3x3_COEFF_SIZE;
   uint8_t  shift        = pKerPrivArgs->pKerInitArgs.shift;

   // get SE/SA params
   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET);
   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET);

   // get coeff from memory
   uint64_t scalarCoeff = *(uint64_t *) ((uint8_t *) pBlock + filterOffset);

   // duplicate scalar coeff to full vector length
   c7x::ulong_vec ulongvCoeff = __vload_dup(&scalarCoeff);

   // reinterpret to 16b element coeffs
   c7x::short_vec vCoeff1_lo = c7x::reinterpret<c7x::short_vec>(ulongvCoeff);

   // Repeat for remaining coefficient vectors
   scalarCoeff          = *(uint64_t *) ((uint8_t *) pBlock + (filterOffset + VXLIB_GAUSSIANFILTER3x3_COEFF_SIZE));
   ulongvCoeff          = __vload_dup(&scalarCoeff);
   c7x::short_vec vCoeff1_hi = c7x::reinterpret<c7x::short_vec>(ulongvCoeff);

   scalarCoeff          = *(uint64_t *) ((uint8_t *) pBlock + (filterOffset + 2 * VXLIB_GAUSSIANFILTER3x3_COEFF_SIZE));
   ulongvCoeff          = __vload_dup(&scalarCoeff);
   c7x::short_vec vCoeff2_lo = c7x::reinterpret<c7x::short_vec>(ulongvCoeff);

   scalarCoeff          = *(uint64_t *) ((uint8_t *) pBlock + (filterOffset + 3 * VXLIB_GAUSSIANFILTER3x3_COEFF_SIZE));
   ulongvCoeff          = __vload_dup(&scalarCoeff);
   c7x::short_vec vCoeff2_hi = c7x::reinterpret<c7x::short_vec>(ulongvCoeff);

   scalarCoeff          = *(uint64_t *) ((uint8_t *) pBlock + (filterOffset + 4 * VXLIB_GAUSSIANFILTER3x3_COEFF_SIZE));
   ulongvCoeff          = __vload_dup(&scalarCoeff);
   c7x::short_vec vCoeff3_lo = c7x::reinterpret<c7x::short_vec>(ulongvCoeff);

   scalarCoeff          = *(uint64_t *) ((uint8_t *) pBlock + (filterOffset + 5 * VXLIB_GAUSSIANFILTER3x3_COEFF_SIZE));
   ulongvCoeff          = __vload_dup(&scalarCoeff);
   c7x::short_vec vCoeff3_hi = c7x::reinterpret<c7x::short_vec>(ulongvCoeff);

   // open streams
   __SE0_OPEN(pInLocal, se0Params);
   __SA0_OPEN(sa0Params);

   size_t filterSize = pKerPrivArgs->pKerInitArgs.filterSize;

   size_t elemCountLoop = c7x::element_count_of<c7x::uchar_hvec>::value;
   size_t wBlocks       = VXLIB_ceilingDiv(pKerPrivArgs->width, (elemCountLoop / 2));
   size_t hBlocks       = pKerPrivArgs->height - filterSize + 1;

   c7x::int_vec shiftVec = (c7x::int_vec) shift;

   c7x::int_vec vResA, vResB, vResC, vResD, vResE, vResABCDE;

   // TODO: nested loop coalescing (NLC compiler pragma)
   for (int32_t wCounter = 0; wCounter < wBlocks; wCounter++) {

      for (int32_t hCounter = 0; hCounter < hBlocks; hCounter++) {
         vResA = __vfir8hw_wvv(vCoeff1_lo, vCoeff1_hi, __SE_REG_0_ADV);
         vResB = __vfir8hw_wvv(vCoeff2_lo, vCoeff2_hi, __SE_REG_0_ADV);
         vResC = __vfir8hw_wvv(vCoeff3_lo, vCoeff3_hi, __SE_REG_0_ADV);
         vResD = __vfir8hw_wvv(vCoeff2_lo, vCoeff2_hi, __SE_REG_0_ADV);
         vResE = __vfir8hw_wvv(vCoeff1_lo, vCoeff1_hi, __SE_REG_0_ADV);

         vResABCDE = vResA + vResB + vResC + vResD + vResE;
         vResABCDE = __shift_right(vResABCDE, shiftVec);

         // saturate from 32b signed to 8b unsigned
         c7x::uint_vec vUResABCDE = c7x::convert<c7x::uint_vec>(vResABCDE);

         __vpred     tmp  = c7x::strm_agen<0, c7x::uint_vec>::get_vpred();
         c7x::uchar_qvec *addr = c7x::strm_agen<0, c7x::uchar_qvec>::get_adv(pOutLocal);
         __vstore_pred_pack_byte(tmp, addr, vUResABCDE);
      }
   }

   __SE0_CLOSE();
   __SA0_CLOSE();
}

// this method performs 3x3 gaussian filter computation
template <typename dTypeIn, typename dTypeOut>
void VXLIB_gaussian_3x3_exec_ci(VXLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   // get kernel handle
   VXLIB_gaussian_PrivArgs *pKerPrivArgs = (VXLIB_gaussian_PrivArgs *) handle;

   // SE/SA params template
   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;
   __SA_TEMPLATE_v1 sa0Params;

   // create local pointers
   dTypeIn *restrict pInLocal   = (dTypeIn *) pIn;
   dTypeOut *restrict pOutLocal = (dTypeOut *) pOut;

   // define vector types
   typedef typename c7x::char_hvec out_hvec;


   uint8_t *pBlock       = pKerPrivArgs->bufPblock;
   size_t   elemCount    = c7x::element_count_of<c7x::short_vec>::value;
   size_t   filterOffset = SA0_PARAM_OFFSET + VXLIB_GAUSSIANFILTER3x3_COEFF_SIZE;

   // get SE/SA params
   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET);
   se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET);
   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET);

   // get coeff from memory
   uint64_t scalarCoeff = *(uint64_t *) ((uint8_t *) pBlock + filterOffset);

   // duplicate scalar coeff to full vector length
   c7x::ulong_vec ulongvCoeff = __vload_dup(&scalarCoeff);

   // reinterpret to 16b element coeffs
   c7x::short_vec vCoeff1 = c7x::reinterpret<c7x::short_vec>(ulongvCoeff);

   // Do similar process to retreive 2nd row of coeffcients
   scalarCoeff       = *(uint64_t *) ((uint8_t *) pBlock + (filterOffset + VXLIB_GAUSSIANFILTER3x3_COEFF_SIZE));
   ulongvCoeff       = __vload_dup(&scalarCoeff);
   c7x::short_vec vCoeff2 = c7x::reinterpret<c7x::short_vec>(ulongvCoeff);

   // open streams
   __SE0_OPEN(pInLocal, se0Params);
   __SE1_OPEN(pInLocal + elemCount, se1Params);
   __SA0_OPEN(sa0Params);

   size_t filterSize = pKerPrivArgs->pKerInitArgs.filterSize;
   size_t wBlocks    = VXLIB_ceilingDiv(pKerPrivArgs->width, elemCount);
   size_t hBlocks    = VXLIB_ceilingDiv((pKerPrivArgs->height - filterSize + 1), filterSize);

   c7x::int_vec vQ_BITS = (c7x::int_vec) Q_BITS;

   c7x::int_vec vResA_lo, vResA_hi, vResB_lo, vResB_hi, vResC_lo, vResC_hi;
   c7x::int_vec vResAB_lo, vResAB_hi, vResBC_lo, vResBC_hi, vResCA_lo, vResCA_hi;
   c7x::int_vec vResABC_lo, vResABC_hi, vResBCA_lo, vResBCA_hi, vResCAB_lo, vResCAB_hi;

   // TODO: nested loop coalescing (NLC compiler pragma)
   for (int32_t wCounter = 0; wCounter < wBlocks; wCounter++) {

      __vfir4hw_vww(vCoeff1, __SE_REG_PAIR_0_ADV, vResA_lo, vResA_hi);
      __vfir4hw_vww(vCoeff2, __SE_REG_PAIR_0, vResB_lo, vResB_hi);

      vResAB_lo = vResA_lo + vResB_lo;
      vResAB_hi = vResA_hi + vResB_hi;
      __vfir4hw_vww(vCoeff1, __SE_REG_PAIR_0_ADV, vResB_lo, vResB_hi);

      // store FIR of row B with Coeffcient 1 for BCA calculation later
      vResBC_lo = vResB_lo;
      vResBC_hi = vResB_hi;

      for (int32_t hCounter = 0; hCounter < hBlocks; hCounter++) {

         // vfir4 row C
         __vfir4hw_vww(vCoeff1, __SE_REG_PAIR_0, vResC_lo, vResC_hi);

         // vertical sum ABC
         vResABC_lo = vResAB_lo + vResC_lo;
         vResABC_hi = vResAB_hi + vResC_hi;

         // adjust ABC by Q factor
         vResABC_lo = __shift_right(vResABC_lo, vQ_BITS);
         vResABC_hi = __shift_right(vResABC_hi, vQ_BITS);

         // predicated pack store ABC, words to bytes (VSTPWPACKB)
         __vpred   tmp  = c7x::strm_agen<0, c7x::char_vec>::get_vpred();
         out_hvec *addr = c7x::strm_agen<0, out_hvec>::get_adv(pOutLocal);
         __vstore_pred_pack_byte_2src(tmp, addr, vResABC_lo, vResABC_hi);

         // store FIR of row C with coefficient 1 for CAB calculation later
         vResCA_lo = vResC_lo;
         vResCA_hi = vResC_hi;

         // vfir4 row C
         __vfir4hw_vww(vCoeff2, __SE_REG_PAIR_0_ADV, vResC_lo, vResC_hi);

         // vertical sum BC
         vResBC_lo += vResC_lo;
         vResBC_hi += vResC_hi;

         __vfir4hw_vww(vCoeff1, __SE_REG_PAIR_0, vResA_lo, vResA_hi);

         vResBCA_lo = vResBC_lo + vResA_lo;
         vResBCA_hi = vResBC_hi + vResA_hi;

         vResBCA_lo = __shift_right(vResBCA_lo, vQ_BITS);
         vResBCA_hi = __shift_right(vResBCA_hi, vQ_BITS);

         // store BCA
         tmp  = c7x::strm_agen<0, c7x::char_vec>::get_vpred();
         addr = c7x::strm_agen<0, out_hvec>::get_adv(pOutLocal);
         __vstore_pred_pack_byte_2src(tmp, addr, vResBCA_lo, vResBCA_hi);

         __vfir4hw_vww(vCoeff2, __SE_REG_PAIR_0, vResA_lo, vResA_hi);

         // add row A with coefficient 2 for CAB calculation
         vResCA_lo += vResA_lo;
         vResCA_hi += vResA_hi;

         // row A with coefficient 1 for next loop iteration of ABC
         __vfir4hw_vww(vCoeff1, __SE_REG_PAIR_0_ADV, vResA_lo, vResA_hi);

         // add row B with coefficient 1 for BCA calculation
         __vfir4hw_vww(vCoeff1, __SE_REG_PAIR_0, vResB_lo, vResB_hi);
         vResBC_lo = vResB_lo;
         vResBC_hi = vResB_hi;

         vResCAB_lo = vResCA_lo + vResB_lo;
         vResCAB_hi = vResCA_hi + vResB_hi;

         vResCAB_lo = __shift_right(vResCAB_lo, vQ_BITS);
         vResCAB_hi = __shift_right(vResCAB_hi, vQ_BITS);

         // row B with coefficient 2 for next loop iteration of ABC
         __vfir4hw_vww(vCoeff2, __SE_REG_PAIR_0_ADV, vResB_lo, vResB_hi);

         // row AB for next loop iteration of ABC
         vResAB_lo = vResA_lo + vResB_lo;
         vResAB_hi = vResA_hi + vResB_hi;

         // store CAB
         tmp  = c7x::strm_agen<0, c7x::char_vec>::get_vpred();
         addr = c7x::strm_agen<0, out_hvec>::get_adv(pOutLocal);
         __vstore_pred_pack_byte_2src(tmp, addr, vResCAB_lo, vResCAB_hi);
      }
   }

   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA0_CLOSE();
}

// this method is the top level module for k x k gaussian filter operation on an input image
template <typename dTypeIn, typename dTypeOut>
VXLIB_STATUS VXLIB_gaussian_exec_ci(VXLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   VXLIB_STATUS status = VXLIB_SUCCESS;

   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_gaussian_PrivArgs *pKerPrivArgs = (VXLIB_gaussian_PrivArgs *) handle;

   size_t filterSize = pKerPrivArgs->pKerInitArgs.filterSize;

   // call 3x3 gaussian filter exec
   if (filterSize == VXLIB_GAUSSIAN_FILTER_3x3) {
      VXLIB_gaussian_3x3_exec_ci<VXLIB_GAUSSIAN_TYPENAME_I8U_O8U>(handle, pIn, pOut);
   }
   else if (filterSize == VXLIB_GAUSSIAN_FILTER_5x5) {
      VXLIB_gaussian_5x5_exec_ci<VXLIB_GAUSSIAN_TYPENAME_I8U_O8U>(handle, pIn, pOut);
   }

   // return errors for other filter sizes; not implemented
   else {
      status = VXLIB_ERR_NOT_IMPLEMENTED;
   }

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* Explicit instantiations for VXLIB_gaussian_exec_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

template VXLIB_STATUS VXLIB_gaussian_exec_ci<VXLIB_GAUSSIAN_TYPENAME_I8U_O8U>(VXLIB_kernelHandle handle,
                                                                              void *restrict pIn,
                                                                              void *restrict pOut);

void VXLIB_gaussian_perfEst(VXLIB_kernelHandle handle, size_t *archCycles, size_t *estCycles)
{
   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_gaussian_PrivArgs *pKerPrivArgs = (VXLIB_gaussian_PrivArgs *) handle;

   // obtain loop count for compute loop
   size_t numBlocks = pKerPrivArgs->numBlocks;
   size_t overheadCycles  = 17; // profiled code before entering compute loop
   size_t iterConst = 5;
   size_t ii        = 7;

   *archCycles = iterConst + numBlocks * ii; // obtained from asm
   *estCycles  = overheadCycles + *archCycles;
}
