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

#include "VXLIB_addWeight_priv.h"
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

#define UNROLL_FACTOR 4
/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_addWeight_init_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method initializes the kernel-specific parameters
// mainly, the streaming engine and streaming address generators
template <uint32_t dTypeIn0, uint32_t dTypeIn1, uint32_t dTypeOut>
VXLIB_STATUS VXLIB_addWeight_init_ci(VXLIB_kernelHandle              handle,
                                     const VXLIB_bufParams2D_t      *bufParamsIn0,
                                     const VXLIB_bufParams2D_t      *bufParamsIn1,
                                     const VXLIB_bufParams2D_t      *bufParamsOut,
                                     const VXLIB_addWeight_InitArgs *pKerInitArgs)
{
   VXLIB_STATUS status = VXLIB_SUCCESS; // assign status to success by default

   // structs to hold SE and SA parameters
   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1();
   __SE_TEMPLATE_v1 se1Params = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa0Params = __gen_SA_TEMPLATE_v1();

   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_addWeight_PrivArgs *pKerPrivArgs = (VXLIB_addWeight_PrivArgs *) handle;

   // obtain image size and compute number of blocks to process
   size_t width_           = pKerPrivArgs->width;
   size_t height_          = pKerPrivArgs->height;
   size_t elemCount_       = VXLIB_max_simd<VXLIB_UINT32>::value;
   size_t wBlocks_         = (width_ + (elemCount_ - 1)) / (elemCount_);
   size_t numBlocks_       = height_ * wBlocks_;
   pKerPrivArgs->numBlocks = numBlocks_;

   uint8_t *pBlock = pKerPrivArgs->bufPblock; // address to retrieve to store SE/SA params

   // TODO: set custom params to promot 4x from se

   // SE/SA parameter containers
   __SE_ELETYPE SE_ELETYPE;
   __SE_VECLEN  SE_VECLEN;
   __SA_VECLEN  SA_VECLEN;

   // assign SE/SA parameters
   SE_ELETYPE = c7x::se_eletype<c7x::uchar_vec>::value;
   SE_VECLEN  = c7x::se_veclen<c7x::uint_vec>::value;
   SA_VECLEN  = c7x::sa_veclen<c7x::uint_vec>::value;

   size_t width  = bufParamsIn0->dim_x;
   size_t height = bufParamsIn0->dim_y;

   size_t strideIn0Elems = pKerPrivArgs->strideIn0Elements;
   size_t strideIn1Elems = pKerPrivArgs->strideIn1Elements;
   size_t strideOutElems = pKerPrivArgs->strideOutElements;

   bool isLinear = false;

   if ((width == strideIn0Elems) && (strideIn0Elems == strideIn1Elems) && (strideIn0Elems == strideOutElems)) {
      isLinear = true;
   }

   // data cannot be accessed in 1D pattern
   if (!isLinear) {

      /***************************************/
      /* Create SE params for reading input0 */
      /***************************************/

      se0Params.ELETYPE = SE_ELETYPE;
      se0Params.VECLEN  = SE_VECLEN;
      se0Params.DIMFMT  = __SE_DIMFMT_2D;
      se0Params.PROMOTE = __SE_PROMOTE_4X_ZEROEXT;

      se0Params.ICNT0 = width;
      se0Params.ICNT1 = height;
      se0Params.DIM1  = strideIn0Elems;

      /***************************************/
      /* Create SE params for reading input1 */
      /***************************************/

      se1Params.ELETYPE = SE_ELETYPE;
      se1Params.VECLEN  = SE_VECLEN;
      se1Params.DIMFMT  = __SE_DIMFMT_2D;
      se1Params.PROMOTE = __SE_PROMOTE_4X_ZEROEXT;

      se1Params.ICNT0 = width;
      se1Params.ICNT1 = height;
      se1Params.DIM1  = strideIn1Elems;

      /****************************************/
      /* Create SA0 params for writing output */
      /****************************************/

      sa0Params.ICNT0  = width;
      sa0Params.ICNT1  = height;
      sa0Params.DIM1   = strideOutElems;
      sa0Params.VECLEN = SA_VECLEN;
      sa0Params.DIMFMT = __SA_DIMFMT_2D;
   }

   // data can be accessed in 1D pattern
   else {

      /***************************************/
      /* Create SE params for reading input0 */
      /***************************************/

      se0Params.ELETYPE = SE_ELETYPE;
      se0Params.VECLEN  = SE_VECLEN;
      se0Params.DIMFMT  = __SE_DIMFMT_1D;
      se0Params.PROMOTE = __SE_PROMOTE_4X_ZEROEXT;

      se0Params.ICNT0 = width * height;

      /***************************************/
      /* Create SE params for reading input1 */
      /***************************************/

      se1Params.ELETYPE = SE_ELETYPE;
      se1Params.VECLEN  = SE_VECLEN;
      se1Params.DIMFMT  = __SE_DIMFMT_1D;
      se1Params.PROMOTE = __SE_PROMOTE_4X_ZEROEXT;

      se1Params.ICNT0 = width * height;

      /****************************************/
      /* Create SA0 params for writing output */
      /****************************************/

      sa0Params.VECLEN = SA_VECLEN;
      sa0Params.DIMFMT = __SA_DIMFMT_1D;

      sa0Params.ICNT0 = width * height;
   }

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
/* Explicit instantiations for VXLIB_addWeight_init_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

template VXLIB_STATUS
VXLIB_addWeight_init_ci<VXLIB_ADDWEIGHT_DTYPE_I8U_I8U_O8U>(VXLIB_kernelHandle              handle,
                                                           const VXLIB_bufParams2D_t      *bufParamsIn0,
                                                           const VXLIB_bufParams2D_t      *bufParamsIn1,
                                                           const VXLIB_bufParams2D_t      *bufParamsOut,
                                                           const VXLIB_addWeight_InitArgs *pKerInitArgs);

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_addWeight_exec_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method performs pixel-wise weighted addition of two input images
template <typename dTypeIn0, typename dTypeIn1, typename dTypeOut>
VXLIB_STATUS VXLIB_addWeight_exec_ci(VXLIB_kernelHandle handle,
                                     void *restrict pIn0,
                                     void *restrict pIn1,
                                     void *restrict pOut,
                                     float alpha)
{
   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_addWeight_PrivArgs *pKerPrivArgs = (VXLIB_addWeight_PrivArgs *) handle;

   // structs to retrieve SE and SA paramters
   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;
   __SA_TEMPLATE_v1 sa0Params;

   // create local pointers
   dTypeIn0 *restrict pInLocal0 = (dTypeIn0 *) pIn0;
   dTypeIn1 *restrict pInLocal1 = (dTypeIn1 *) pIn1;
   dTypeOut *restrict pOutLocal = (dTypeOut *) pOut;

#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_addWeight_exec_ci\n");
#endif

   // address of SE and SA parameters
   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   // num blocks for kernel iteration
   int32_t numBlocks = pKerPrivArgs->numBlocks;

   // define vector type
   /* typedef typename c7x::make_full_vector<dTypeOut>::type vec; */

   // define computation vector type - float
   typedef typename c7x::make_full_vector<float>::type fvec;

   // define 32b se promote vector type - uint
   typedef typename c7x::make_full_vector<uint32_t>::type uvec;

   // define int vec
   typedef typename c7x::make_full_vector<int32_t>::type ivec;

   // retrieve SE and SA parameters
   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET);
   se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET);
   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET);

   // open SEs to fetch Input samples
   __SE0_OPEN(pInLocal0, se0Params);
   __SE1_OPEN(pInLocal1, se1Params);

   // open SA0 to write output samples
   __SA0_OPEN(sa0Params);

   // vectorize alpha scalar input
   fvec vAlpha         = (fvec) alpha;
   fvec vOne           = (fvec) 1.0;
   fvec vOneMinusAlpha = vOne - vAlpha;

   // define sa vector address type based on architecture
   typedef typename std::conditional<__C7X_VEC_SIZE_BITS__ == 512, __uchar16, __uchar8>::type __saType;

   // perform weighted addition of two images
   for (int32_t counter = 0; counter < numBlocks; counter += UNROLL_FACTOR) {

      // fetch first 4th of promoted input vector
      // =================================================
      uvec a0 = c7x::strm_eng<0, uvec>::get_adv();
      uvec b0 = c7x::strm_eng<1, uvec>::get_adv();

      // move to float computation containers
      fvec fa0   = __int_to_float(a0);                  // vintspu (.C, .S, .L)
      fvec fb0   = __int_to_float(b0);                  // vintspu (.C, .S, .L)
      fvec fout0 = vAlpha * fa0 + vOneMinusAlpha * fb0; // vmpysp (.M, .N), vaddsp (.C, .S, .L)

      // move to 32b int container
      ivec out0 = __float_to_int_rtz(fout0); // vsptrunc (.S, .L)

      // convert output to unsigned
      uvec uout0 = c7x::reinterpret<uvec>(out0);

      // predicated pack store (lowest byte of every word) first 4th of output vector
      __vpred   tmp  = c7x::strm_agen<0, uvec>::get_vpred();
      __saType *addr = c7x::strm_agen<0, __saType>::get_adv(pOutLocal);
      __vstore_pred_pack_byte(tmp, addr, uout0); // vstpwsvpackb (.D)

      // fetch second 4th of promoted input vector
      // =================================================
      uvec a1 = c7x::strm_eng<0, uvec>::get_adv();
      uvec b1 = c7x::strm_eng<1, uvec>::get_adv();

      fvec fa1   = __int_to_float(a1);
      fvec fb1   = __int_to_float(b1);
      fvec fout1 = vAlpha * fa1 + vOneMinusAlpha * fb1;

      ivec out1  = __float_to_int_rtz(fout1);
      uvec uout1 = c7x::reinterpret<uvec>(out1);

      // predicated pack store (lowest byte of every word) second 4th of output vector
      tmp  = c7x::strm_agen<0, uvec>::get_vpred();
      addr = c7x::strm_agen<0, __saType>::get_adv(pOutLocal);
      __vstore_pred_pack_byte(tmp, addr, uout1);

      // fetch third 4th of promoted input vector
      // =================================================
      uvec a2 = c7x::strm_eng<0, uvec>::get_adv();
      uvec b2 = c7x::strm_eng<1, uvec>::get_adv();

      fvec fa2   = __int_to_float(a2);
      fvec fb2   = __int_to_float(b2);
      fvec fout2 = vAlpha * fa2 + vOneMinusAlpha * fb2;

      ivec out2  = __float_to_int_rtz(fout2);
      uvec uout2 = c7x::reinterpret<uvec>(out2);

      // predicated pack store (lowest byte of every word) third  4th of output vector
      tmp  = c7x::strm_agen<0, uvec>::get_vpred();
      addr = c7x::strm_agen<0, __saType>::get_adv(pOutLocal);
      __vstore_pred_pack_byte(tmp, addr, uout2);

      // fetch last 4th of promoted input vector
      // =================================================
      uvec a3 = c7x::strm_eng<0, uvec>::get_adv();
      uvec b3 = c7x::strm_eng<1, uvec>::get_adv();

      fvec fa3   = __int_to_float(a3);
      fvec fb3   = __int_to_float(b3);
      fvec fout3 = vAlpha * fa3 + vOneMinusAlpha * fb3;

      ivec out3  = __float_to_int_rtz(fout3);
      uvec uout3 = c7x::reinterpret<uvec>(out3);

      // predicated pack store (lowest byte of every word) last 4th of output vector
      tmp  = c7x::strm_agen<0, uvec>::get_vpred();
      addr = c7x::strm_agen<0, __saType>::get_adv(pOutLocal);
      __vstore_pred_pack_byte(tmp, addr, uout3);
   }

   // close SE/SA
   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA0_CLOSE();

   return VXLIB_SUCCESS;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* Explicit instantiations for VXLIB_addWeight_exec_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

template VXLIB_STATUS VXLIB_addWeight_exec_ci<VXLIB_ADDWEIGHT_TYPENAME_I8U_I8U_O8U>(VXLIB_kernelHandle handle,
                                                                                    void *restrict pIn0,
                                                                                    void *restrict pIn1,

                                                                                    void *restrict pOut,
                                                                                    float alpha);

// performance estimation function
void VXLIB_addWeight_perfEst(VXLIB_kernelHandle handle, size_t *archCycles, size_t *estCycles)
{

   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_addWeight_PrivArgs *pKerPrivArgs = (VXLIB_addWeight_PrivArgs *) handle;

   // compute cycle estimation in similar manner to asm report
   size_t numBlocks = pKerPrivArgs->numBlocks; // iteration count from compute loop
   size_t overheadCnt  = 17;                      // profiled code before compute loop entrance

   // get iteration cycles from asm report
   size_t iterConst = 14; // prologue + epilogue cycle counts
   size_t ii        = 6;  // ii of scheduled loop

   *archCycles = iterConst + numBlocks * ii;
   *estCycles  = overheadCnt + *archCycles;
}
