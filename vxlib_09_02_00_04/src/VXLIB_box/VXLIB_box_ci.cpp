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

#include "VXLIB_box_priv.h"
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

#define VXLIB_BOXFILTER3x3_COEFF_SIZE (sizeof(uint64_t) * 8)

#define Q_BITS 15
#define Q_FACTOR (1U << (uint32_t)Q_BITS)
/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_box_init_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method initializes the kernel-specific parameters
// mainly, the streaming engine and streaming address generators
template <uint32_t dTypeIn, uint32_t dTypeOut>
VXLIB_STATUS VXLIB_box_init_ci(VXLIB_kernelHandle         handle,
                               const VXLIB_bufParams2D_t *bufParamsIn,
                               const VXLIB_bufParams2D_t *bufParamsOut,
                               const VXLIB_box_InitArgs  *pKerInitArgs)
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
   VXLIB_box_PrivArgs *pKerPrivArgs = (VXLIB_box_PrivArgs *) handle;

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

   // declare filter coefficients
   float   spCoeff  = 1.0 / (filterSize * filterSize);
   int16_t fixCoeff = (int16_t)(spCoeff * Q_FACTOR); // convert to Q15 fixed point value

   // setup se/sa for k x k implementation
   if (filterSize == 3) {

      // call 3x3 box init non-padded implementation
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

         // store box filter 3x3 coefficients

         /* 0                     64b */
         /* +-----------------------+ */
         /* | 1/9 | 1/9 | 1/9 |  0  | */
         /* +-----------------------+ */

         uint64_t filter = fixCoeff;
         filter          = (filter << 16) | (uint64_t)fixCoeff;
         filter          = (filter << 16) | (uint64_t)fixCoeff;

         size_t filter_offset = SA0_PARAM_OFFSET + VXLIB_BOXFILTER3x3_COEFF_SIZE;

         *(uint64_t *) ((uint8_t *) pBlock + filter_offset) = filter;
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
/* Explicit instantiations for VXLIB_box_init_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

template VXLIB_STATUS VXLIB_box_init_ci<VXLIB_BOX_DTYPE_I8U_O8U>(VXLIB_kernelHandle         handle,
                                                                 const VXLIB_bufParams2D_t *bufParamsIn,
                                                                 const VXLIB_bufParams2D_t *bufParamsOut,
                                                                 const VXLIB_box_InitArgs  *pKerInitArgs);

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_box_exec_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method performs 3x3 box filter computation
template <typename dTypeIn, typename dTypeOut>
void VXLIB_box_3x3_exec_ci(VXLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{

   // get kernel handle
   VXLIB_box_PrivArgs *pKerPrivArgs = (VXLIB_box_PrivArgs *) handle;

   // SE/SA params template
   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;
   __SA_TEMPLATE_v1 sa0Params;

   // create local pointers
   dTypeIn *restrict pInLocal   = (dTypeIn *) pIn;
   dTypeOut *restrict pOutLocal = (dTypeOut *) pOut;

   // define vector types
   typedef typename c7x::char_hvec out_hvec_t;
   typedef typename c7x::int_vec   int_vec_t;
   typedef typename c7x::short_vec short_vec_t;
   typedef typename c7x::ulong_vec ulong_vec_t;
   typedef typename c7x::char_vec  char_vec_t;

   uint8_t *pBlock       = pKerPrivArgs->bufPblock;
   size_t   elemCount    = c7x::element_count_of<short_vec_t>::value;
   size_t   filterOffset = SA0_PARAM_OFFSET + VXLIB_BOXFILTER3x3_COEFF_SIZE;

   // get SE/SA params
   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET);
   se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET);
   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET);

   // get coeff from memory
   uint64_t scalarCoeff = *(uint64_t *) ((uint8_t *) pBlock + filterOffset);

   // duplicate scalar coeff to full vector length
   ulong_vec_t ulongvCoeff = __vload_dup(&scalarCoeff);

   // reinterpret to 16b element coeffs
   short_vec_t vCoeff = c7x::reinterpret<short_vec_t>(ulongvCoeff);

   // open streams
   __SE0_OPEN(pInLocal, se0Params);
   __SE1_OPEN(pInLocal + elemCount, se1Params);
   __SA0_OPEN(sa0Params);

   size_t filterSize = pKerPrivArgs->pKerInitArgs.filterSize;
   size_t wBlocks    = VXLIB_ceilingDiv(pKerPrivArgs->width, elemCount);
   size_t hBlocks    = VXLIB_ceilingDiv((pKerPrivArgs->height - filterSize + 1), filterSize);

   int_vec_t vQ_BITS = (int_vec_t) Q_BITS;

   int_vec_t vResA_lo, vResA_hi, vResB_lo, vResB_hi, vResC_lo, vResC_hi;
   int_vec_t vResAB_lo, vResAB_hi, vResBC_lo, vResBC_hi, vResCA_lo, vResCA_hi;
   int_vec_t vResABC_lo, vResABC_hi, vResBCA_lo, vResBCA_hi, vResCAB_lo, vResCAB_hi;

   // TODO: nested loop coalescing (NLC compiler pragma)
   for (int32_t wCounter = 0; wCounter < wBlocks; wCounter++) {

      __vfir4hw_vww(vCoeff, __SE_REG_PAIR_0_ADV, vResA_lo, vResA_hi);
      __vfir4hw_vww(vCoeff, __SE_REG_PAIR_0_ADV, vResB_lo, vResB_hi);

      vResAB_lo = vResA_lo + vResB_lo;
      vResAB_hi = vResA_hi + vResB_hi;

      for (int32_t hCounter = 0; hCounter < hBlocks; hCounter++) {

         // vfir4 row C
         __vfir4hw_vww(vCoeff, __SE_REG_PAIR_0_ADV, vResC_lo, vResC_hi);

         // vertical sum BC
         vResBC_lo = vResB_lo + vResC_lo;
         vResBC_hi = vResB_hi + vResC_hi;

         // vertical sum ABC
         vResABC_lo = vResAB_lo + vResC_lo;
         vResABC_hi = vResAB_hi + vResC_hi;

         // adjust ABC by Q factor
         vResABC_lo = __shift_right(vResABC_lo, vQ_BITS);
         vResABC_hi = __shift_right(vResABC_hi, vQ_BITS);

         // predicated pack store ABC, words to bytes (VSTPWPACKB)
         __vpred   tmp  = c7x::strm_agen<0, char_vec_t>::get_vpred();
         out_hvec_t *addr = c7x::strm_agen<0, out_hvec_t>::get_adv(pOutLocal);
         __vstore_pred_pack_byte_2src(tmp, addr, vResABC_lo, vResABC_hi);

         __vfir4hw_vww(vCoeff, __SE_REG_PAIR_0_ADV, vResA_lo, vResA_hi);

         vResCA_lo = vResC_lo + vResA_lo;
         vResCA_hi = vResC_hi + vResA_hi;

         vResBCA_lo = vResBC_lo + vResA_lo;
         vResBCA_hi = vResBC_hi + vResA_hi;

         vResBCA_lo = __shift_right(vResBCA_lo, vQ_BITS);
         vResBCA_hi = __shift_right(vResBCA_hi, vQ_BITS);

         // store BCA
         tmp  = c7x::strm_agen<0, char_vec_t>::get_vpred();
         addr = c7x::strm_agen<0, out_hvec_t>::get_adv(pOutLocal);
         __vstore_pred_pack_byte_2src(tmp, addr, vResBCA_lo, vResBCA_hi);

         __vfir4hw_vww(vCoeff, __SE_REG_PAIR_0_ADV, vResB_lo, vResB_hi);

         vResAB_lo = vResA_lo + vResB_lo;
         vResAB_hi = vResA_hi + vResB_hi;

         vResCAB_lo = vResCA_lo + vResB_lo;
         vResCAB_hi = vResCA_hi + vResB_hi;

         vResCAB_lo = __shift_right(vResCAB_lo, vQ_BITS);
         vResCAB_hi = __shift_right(vResCAB_hi, vQ_BITS);

         // store CAB
         tmp  = c7x::strm_agen<0, char_vec_t>::get_vpred();
         addr = c7x::strm_agen<0, out_hvec_t>::get_adv(pOutLocal);
         __vstore_pred_pack_byte_2src(tmp, addr, vResCAB_lo, vResCAB_hi);
      }
   }

   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA0_CLOSE();
}

// this method is the top level module for k x k box filter operation on an input image
template <typename dTypeIn, typename dTypeOut>
VXLIB_STATUS VXLIB_box_exec_ci(VXLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{

   VXLIB_STATUS status = VXLIB_SUCCESS;

   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_box_PrivArgs *pKerPrivArgs = (VXLIB_box_PrivArgs *) handle;

   size_t filterSize = pKerPrivArgs->pKerInitArgs.filterSize;

   // call 3x3 box filter exec
   if (filterSize == 3) {
      VXLIB_box_3x3_exec_ci<VXLIB_BOX_TYPENAME_I8U_O8U>(handle, pIn, pOut);
   }

   // return errors for other filter sizes; not implemented
   else {
      status = VXLIB_ERR_NOT_IMPLEMENTED;
   }

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* Explicit instantiations for VXLIB_box_exec_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

template VXLIB_STATUS
VXLIB_box_exec_ci<VXLIB_BOX_TYPENAME_I8U_O8U>(VXLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

void VXLIB_box_perfEst(VXLIB_kernelHandle handle, size_t *archCycles, size_t *estCycles)
{

   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_box_PrivArgs *pKerPrivArgs = (VXLIB_box_PrivArgs *) handle;

   // obtain loop count for compute loop
   size_t numBlocks = pKerPrivArgs->numBlocks;
   size_t overheadCnt  = 17; // profiled code before entering compute loop
   size_t iterConst = 5;
   size_t ii        = 7;

   *archCycles = iterConst + numBlocks * ii; // obtained from asm
   *estCycles  = overheadCnt + *archCycles;
}
