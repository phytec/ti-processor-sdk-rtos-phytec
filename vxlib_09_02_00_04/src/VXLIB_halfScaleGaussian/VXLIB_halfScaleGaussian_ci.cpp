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

#include "VXLIB_halfScaleGaussian_priv.h"
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

// clang-format off
#if defined(_HOST_BUILD)
#if __C7X_VEC_SIZE_BYTES__ == 64
const c7x::uchar_vec VXLIB_vperm_data =
      c7x::uchar_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
#elif __C7X_VEC_SIZE_BYTES__ == 32
const c7x::uchar_vec VXLIB_vperm_data = 
      c7x::uchar_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
#endif
//Add
#else  // not host build.....
#if __C7X_VEC_SIZE_BYTES__ == 64
const c7x::uchar_vec VXLIB_vperm_data = c7x::uchar_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
#elif __C7X_VEC_SIZE_BYTES__ == 32
const c7x::uchar_vec VXLIB_vperm_data = c7x::uchar_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
#endif 
#endif
// clang-format on

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_halfScaleGaussian_init_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method initializes the kernel-specific parameters
// mainly, the streaming engine and streaming address generators
template <uint32_t dTypeIn, uint32_t dTypeOut>
VXLIB_STATUS VXLIB_halfScaleGaussian_init_ci(VXLIB_kernelHandle                      handle,
                                             const VXLIB_bufParams2D_t              *bufParamsIn,
                                             const VXLIB_bufParams2D_t              *bufParamsOut,
                                             const VXLIB_halfScaleGaussian_InitArgs *pKerInitArgs)
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
   VXLIB_halfScaleGaussian_PrivArgs *pKerPrivArgs = (VXLIB_halfScaleGaussian_PrivArgs *) handle;

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

   if (filterSize == VXLIB_HALF_SCALE_GAUSSIAN_FILTER_5x5) {
      int16_t gaussianFilter5x5[15] = {1, 4, 6, 4, 1, 4, 16, 24, 16, 4, 6, 24, 36, 24, 6};

      // call 5x5 gaussian init non-padded implementation
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
         se0Params.DIM2  = stride * 2;
         se0Params.ICNT2 = (height - filterSize + 1) / 2;

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
         sa0Params.ICNT0 = elemCountOut / 2;

         // 1: vertical iterations (1 store per convolution window)
         sa0Params.DIM1  = pKerPrivArgs->strideOutElements; // SA DIM jumps are given in num elements
         sa0Params.ICNT1 = bufParamsOut->dim_y;

         // 2: horizontal iterations
         sa0Params.DIM2  = elemCountOut / 2;
         sa0Params.ICNT2 = VXLIB_ceilingDiv(bufParamsOut->dim_x, (elemCountOut / 2));

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
/* Explicit instantiations for VXLIB_halfScaleGaussian_init_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

template VXLIB_STATUS VXLIB_halfScaleGaussian_init_ci<VXLIB_HALF_SCALE_GAUSSIAN_DTYPE_I8U_O8U>(
    VXLIB_kernelHandle                      handle,
    const VXLIB_bufParams2D_t              *bufParamsIn,
    const VXLIB_bufParams2D_t              *bufParamsOut,
    const VXLIB_halfScaleGaussian_InitArgs *pKerInitArgs);

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_halfScaleGaussian_exec_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

template <typename dTypeIn, typename dTypeOut>
void VXLIB_halfScaleGaussian_5x5_exec_ci(VXLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   // get kernel handle
   VXLIB_halfScaleGaussian_PrivArgs *pKerPrivArgs = (VXLIB_halfScaleGaussian_PrivArgs *) handle;

   // SE/SA params template
   __SE_TEMPLATE_v1 se0Params;
   __SA_TEMPLATE_v1 sa0Params;

   // create local pointers
   dTypeIn *restrict pInLocal   = (dTypeIn *) pIn;
   dTypeOut *restrict pOutLocal = (dTypeOut *) pOut;

   uint8_t *pBlock       = pKerPrivArgs->bufPblock;
   size_t   filterOffset = SA0_PARAM_OFFSET + VXLIB_GAUSSIANFILTER3x3_COEFF_SIZE;

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
   size_t wBlocks       = VXLIB_ceilingDiv(pKerPrivArgs->widthIn, (elemCountLoop / 2));
   size_t hBlocks       = (pKerPrivArgs->heightIn - filterSize + 1) / 2;

   c7x::int_vec vResA, vResB, vResC, vResD, vResE, vRes1ABCDE;

   uint8_t shift    = 8;
   c7x::int_vec shiftVec = (c7x::int_vec) shift;

   // TODO: nested loop coalescing (NLC compiler pragma)
   for (int32_t wCounter = 0; wCounter < wBlocks; wCounter++) {

      for (int32_t hCounter = 0; hCounter < hBlocks; hCounter++) {
         vResA = __vfir8hw_wvv(vCoeff1_lo, vCoeff1_hi, __SE_REG_0_ADV);
         vResB = __vfir8hw_wvv(vCoeff2_lo, vCoeff2_hi, __SE_REG_0_ADV);
         vResC = __vfir8hw_wvv(vCoeff3_lo, vCoeff3_hi, __SE_REG_0_ADV);
         vResD = __vfir8hw_wvv(vCoeff2_lo, vCoeff2_hi, __SE_REG_0_ADV);
         vResE = __vfir8hw_wvv(vCoeff1_lo, vCoeff1_hi, __SE_REG_0_ADV);

         vRes1ABCDE = vResA + vResB + vResC + vResD + vResE;
         vRes1ABCDE = __shift_right(vRes1ABCDE, shiftVec);

         // saturate from 32b signed to 8b unsigned
         c7x::uint_vec vURes1ABCDE = c7x::convert<c7x::uint_vec>(vRes1ABCDE);

         c7x::uint_vec zero = (c7x::uint_vec) 0;
         // permute
         c7x::uint_vec vUResABCDE = c7x::as_uint_vec(__permute_even_even_int(
             VXLIB_vperm_data, c7x::as_uchar_vec(zero), c7x::as_uchar_vec(vURes1ABCDE)));

         __vpred     tmp  = c7x::strm_agen<0, c7x::uint_vec>::get_vpred();
         c7x::uchar_qvec *addr = c7x::strm_agen<0, c7x::uchar_qvec>::get_adv(pOutLocal);
         __vstore_pred_pack_byte(tmp, addr, vUResABCDE);
      }
   }

   __SE0_CLOSE();
   __SA0_CLOSE();
}

// this method is the top level module for 5 x 5 half scale gaussian filter operation on an input image
template <typename dTypeIn, typename dTypeOut>
VXLIB_STATUS VXLIB_halfScaleGaussian_exec_ci(VXLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   VXLIB_STATUS status = VXLIB_SUCCESS;

   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_halfScaleGaussian_PrivArgs *pKerPrivArgs = (VXLIB_halfScaleGaussian_PrivArgs *) handle;

   size_t filterSize = pKerPrivArgs->pKerInitArgs.filterSize;

   // call 5x5 half scale gaussian filter exec
   if (filterSize == VXLIB_HALF_SCALE_GAUSSIAN_FILTER_5x5) {
      VXLIB_halfScaleGaussian_5x5_exec_ci<VXLIB_HALF_SCALE_GAUSSIAN_TYPENAME_I8U_O8U>(handle, pIn, pOut);
   }

   // return errors for other filter sizes; not implemented
   else {
      status = VXLIB_ERR_NOT_IMPLEMENTED;
   }

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* Explicit instantiations for VXLIB_halfScaleGaussian_exec_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

template VXLIB_STATUS
VXLIB_halfScaleGaussian_exec_ci<VXLIB_HALF_SCALE_GAUSSIAN_TYPENAME_I8U_O8U>(VXLIB_kernelHandle handle,
                                                                            void *restrict pIn,
                                                                            void *restrict pOut);

void VXLIB_halfScaleGaussian_perfEst(VXLIB_kernelHandle handle, size_t *archCycles, size_t *estCycles)
{
   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_halfScaleGaussian_PrivArgs *pKerPrivArgs = (VXLIB_halfScaleGaussian_PrivArgs *) handle;

   // obtain loop count for compute loop
   size_t numBlocks = pKerPrivArgs->numBlocks;
   size_t overheadCycles  = 17; // profiled code before entering compute loop
   size_t iterConst = 5;
   size_t ii        = 7;

   *archCycles = iterConst + numBlocks * ii; // obtained from asm
   *estCycles  = overheadCycles + *archCycles;
}
