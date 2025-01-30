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

#include "VXLIB_normL2_priv.h"
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
/* VXLIB_normL2_init_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method initializes the kernel-specific parameters
// mainly, the streaming engine and streaming address generators
template <uint32_t dType>
inline size_t VXLIB_normL2_SE0SE1SA1_init(VXLIB_kernelHandle         handle,
                                          __SE_TEMPLATE_v1 *         se0Params,
                                          __SE_TEMPLATE_v1 *         se1Params,
                                          __SA_TEMPLATE_v1 *         sa0Params,
                                          const VXLIB_bufParams2D_t *bufParamsIn0,
                                          const VXLIB_bufParams2D_t *bufParamsIn1,
                                          const VXLIB_bufParams2D_t *bufParamsOut);
template <>
inline size_t VXLIB_normL2_SE0SE1SA1_init<VXLIB_INT8>(VXLIB_kernelHandle         handle,
                                                      __SE_TEMPLATE_v1 *         se0Params,
                                                      __SE_TEMPLATE_v1 *         se1Params,
                                                      __SA_TEMPLATE_v1 *         sa0Params,
                                                      const VXLIB_bufParams2D_t *bufParamsIn0,
                                                      const VXLIB_bufParams2D_t *bufParamsIn1,
                                                      const VXLIB_bufParams2D_t *bufParamsOut)
{
   VXLIB_normL2_PrivArgs *pKerPrivArgs = (VXLIB_normL2_PrivArgs *) handle;
   size_t                 numBlocks    = 0;

   // obtain image size and compute number of blocks to process
   size_t width  = pKerPrivArgs->width;
   size_t height = pKerPrivArgs->height;

   size_t strideIn0Elems = bufParamsIn0->stride_y / VXLIB_sizeof(bufParamsIn0->data_type);
   size_t strideIn1Elems = bufParamsIn1->stride_y / VXLIB_sizeof(bufParamsIn1->data_type);
   size_t strideOutElems = bufParamsOut->stride_y / VXLIB_sizeof(bufParamsOut->data_type);
   size_t elemCount      = VXLIB_max_simd<VXLIB_INT8>::value;

   int32_t nTiles          = (width + (elemCount - 1)) / elemCount;

   numBlocks = VXLIB_SE0SE1SA0_init<VXLIB_I8S_I8S_O8U_DTYPE>(se0Params, se1Params, sa0Params, bufParamsIn0,
                                                             bufParamsIn1, bufParamsOut);

   if ((width == strideIn0Elems) && (strideIn0Elems == strideIn1Elems) && (strideIn0Elems == strideOutElems)) {
      se0Params->VECLEN  = c7x::se_veclen<c7x::short_vec>::value;
      se0Params->PROMOTE = __SE_PROMOTE_2X_SIGNEXT;
      se1Params->VECLEN  = c7x::se_veclen<c7x::short_vec>::value;
      se1Params->PROMOTE = __SE_PROMOTE_2X_SIGNEXT;
   }
   else {
      se0Params->ICNT0         = elemCount;
      se0Params->DIM1          = elemCount;
      se0Params->ICNT1         = nTiles;
      se0Params->ICNT2         = height;
      se0Params->DIM2          = strideIn0Elems;
      se0Params->DIMFMT        = __SE_DIMFMT_3D;
      se0Params->DECDIM1       = __SE_DECDIM_DIM1;
      se0Params->DECDIM1SD     = __SE_DECDIMSD_DIM0;
      se0Params->VECLEN        = c7x::se_veclen<c7x::short_vec>::value;
      se0Params->DECDIM1_WIDTH = width;
      se0Params->PROMOTE       = __SE_PROMOTE_2X_SIGNEXT;

      se1Params->ICNT0         = elemCount;
      se1Params->DIM1          = elemCount;
      se1Params->ICNT1         = nTiles;
      se1Params->ICNT2         = height;
      se1Params->DIM2          = strideIn1Elems;
      se1Params->DIMFMT        = __SE_DIMFMT_3D;
      se1Params->DECDIM1       = __SE_DECDIM_DIM1;
      se1Params->DECDIM1SD     = __SE_DECDIMSD_DIM0;
      se1Params->VECLEN        = c7x::se_veclen<c7x::short_vec>::value;
      se1Params->DECDIM1_WIDTH = width;
      se1Params->PROMOTE       = __SE_PROMOTE_2X_SIGNEXT;
   }
   return numBlocks;
}

template <>
inline size_t VXLIB_normL2_SE0SE1SA1_init<VXLIB_INT16>(VXLIB_kernelHandle         handle,
                                                       __SE_TEMPLATE_v1 *         se0Params,
                                                       __SE_TEMPLATE_v1 *         se1Params,
                                                       __SA_TEMPLATE_v1 *         sa0Params,
                                                       const VXLIB_bufParams2D_t *bufParamsIn0,
                                                       const VXLIB_bufParams2D_t *bufParamsIn1,
                                                       const VXLIB_bufParams2D_t *bufParamsOut)
{
   return VXLIB_SE0SE1SA0_init<VXLIB_INT16, VXLIB_INT16, VXLIB_UINT16>(se0Params, se1Params, sa0Params, bufParamsIn0,
                                                                       bufParamsIn1, bufParamsOut);
}
template <uint32_t dTypeIn, uint32_t dTypeOut>
VXLIB_STATUS VXLIB_normL2_init_ci(VXLIB_kernelHandle           handle,
                                  const VXLIB_bufParams2D_t *  bufParamsIn0,
                                  const VXLIB_bufParams2D_t *  bufParamsIn1,
                                  const VXLIB_bufParams2D_t *  bufParamsOut,
                                  const VXLIB_normL2_InitArgs *pKerInitArgs)
{
   VXLIB_STATUS status = VXLIB_SUCCESS; // assign status to success by default

   // structs to hold SE and SA parameters
   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1();
   __SE_TEMPLATE_v1 se1Params = __gen_SE_TEMPLATE_v1();
   __SA_TEMPLATE_v1 sa0Params = __gen_SA_TEMPLATE_v1();

   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_normL2_PrivArgs *pKerPrivArgs = (VXLIB_normL2_PrivArgs *) handle;
   uint8_t *              pBlock       = pKerPrivArgs->bufPblock; // address to retrieve to store SE/SA params

   // set SE0, SE1, and SA0 params
   pKerPrivArgs->numBlocks = VXLIB_normL2_SE0SE1SA1_init<dTypeIn>(handle, &se0Params, &se1Params, &sa0Params,
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
/* Explicit instantiations for VXLIB_normL2_init_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/
template VXLIB_STATUS VXLIB_normL2_init_ci<VXLIB_NORML2_DTYPE_I16S_O16U>(VXLIB_kernelHandle           handle,
                                                                         const VXLIB_bufParams2D_t *  bufParamsIn0,
                                                                         const VXLIB_bufParams2D_t *  bufParamsIn1,
                                                                         const VXLIB_bufParams2D_t *  bufParamsOut,
                                                                         const VXLIB_normL2_InitArgs *pKerInitArgs);

template VXLIB_STATUS VXLIB_normL2_init_ci<VXLIB_NORML2_DTYPE_I8S_O8U>(VXLIB_kernelHandle           handle,
                                                                       const VXLIB_bufParams2D_t *  bufParamsIn0,
                                                                       const VXLIB_bufParams2D_t *  bufParamsIn1,
                                                                       const VXLIB_bufParams2D_t *  bufParamsOut,
                                                                       const VXLIB_normL2_InitArgs *pKerInitArgs);
/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_normL2_exec_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

template <typename vec> static inline vec getSqrt(vec a)
{
   vec     zeroVec  = (vec) 0;
   vec     Half     = (vec) 0.5f;
   vec     OneP5    = (vec) 1.5f;
   __vpred zeroPred = __cmp_eq_pred(zeroVec, a);
   vec     nonZero  = (vec) 1e-12f;

   a = __select(zeroPred, nonZero, a);

   vec x = __recip_sqrt(a);

   vec tmp  = x * a;
   vec tmp1 = OneP5 - tmp * x * Half;
   vec y    = x * tmp1;

   tmp  = y * a;
   tmp1 = OneP5 - tmp * y * Half;
   y    = a * y * tmp1;

   return (y);
}
// this method performs pixel-wise bitwise normL2 of two input images
template <typename dTypeIn, typename dTypeOut>
VXLIB_STATUS
VXLIB_normL2_exec_ci(VXLIB_kernelHandle handle, void *restrict pIn0, void *restrict pIn1, void *restrict pOut)
{
   VXLIB_STATUS status = VXLIB_SUCCESS; // assign status to success by default
   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_normL2_PrivArgs *pKerPrivArgs = (VXLIB_normL2_PrivArgs *) handle;

   // structs to retrieve SE and SA paramters
   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;
   __SA_TEMPLATE_v1 sa0Params;

   // create local pointers
   dTypeIn *restrict  pInLocal0 = (dTypeIn *) pIn0;
   dTypeIn *restrict  pInLocal1 = (dTypeIn *) pIn1;
   dTypeOut *restrict pOutLocal = (dTypeOut *) pOut;

#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_normL2_exec_ci\n");
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

   c7x::int_vec  sqrVec00, sqrVec01, sqrVec10, sqrVec11;
   c7x::int_vec  outVal0, outVal1;
   c7x::uint_vec sumVec0, sumVec1;

   c7x::float_vec sumfVec0, sumfVec1, sqrtVec0, sqrtVec1;
   for (uint32_t counter = 0; counter < numBlocks; counter++) {

      inVec val0 = c7x::strm_eng<0, inVec>::get_adv(); // fetch input 0 via SE0
      inVec val1 = c7x::strm_eng<1, inVec>::get_adv(); // fetch input 1 via SE1

      __vmpyhw_vvw(val0, val0, sqrVec00, sqrVec01);
      __vmpyhw_vvw(val1, val1, sqrVec10, sqrVec11);

      sumVec0 = c7x::as_uint_vec(sqrVec00) + c7x::as_uint_vec(sqrVec10);
      sumVec1 = c7x::as_uint_vec(sqrVec01) + c7x::as_uint_vec(sqrVec11);

      sumfVec0 = __int_to_float(sumVec0);
      sumfVec1 = __int_to_float(sumVec1);

      sqrtVec0 = getSqrt(sumfVec0);
      sqrtVec1 = getSqrt(sumfVec1);

      outVal0 = __float_to_int(sqrtVec0);
      outVal1 = __float_to_int(sqrtVec1);

      // interleave store.
      __vpred tmp  = c7x::strm_agen<0, outVec>::get_vpred();        // generate predicate
      outVec *addr = c7x::strm_agen<0, outVec>::get_adv(pOutLocal); // generate address
      __vstore_pred_interleave(tmp, addr, c7x::as_ushort_vec(outVal0), c7x::as_ushort_vec(outVal1)); // store value
   }

   // close SE/SA
   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA0_CLOSE();

   return status;
}

template VXLIB_STATUS VXLIB_normL2_exec_ci<VXLIB_NORML2_TYPENAME_I16S_O16U>(VXLIB_kernelHandle handle,
                                                                            void *restrict     pIn0,
                                                                            void *restrict     pIn1,
                                                                            void *restrict     pOut);
template <>
VXLIB_STATUS VXLIB_normL2_exec_ci<VXLIB_NORML2_TYPENAME_I8S_O8U>(VXLIB_kernelHandle handle,
                                                                 void *restrict     pIn0,
                                                                 void *restrict     pIn1,
                                                                 void *restrict     pOut)
{
   VXLIB_STATUS status = VXLIB_SUCCESS; // assign status to success by default
   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_normL2_PrivArgs *pKerPrivArgs = (VXLIB_normL2_PrivArgs *) handle;

   // structs to retrieve SE and SA paramters
   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;
   __SA_TEMPLATE_v1 sa0Params;

   // create local pointers
   int8_t *restrict  pInLocal0 = (int8_t *) pIn0;
   int8_t *restrict  pInLocal1 = (int8_t *) pIn1;
   uint8_t *restrict pOutLocal = (uint8_t *) pOut;

#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_normL2_exec_ci\n");
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

   typedef typename c7x::make_full_vector<uint8_t>::type outVec;

   c7x::short_vec  sqrVec0, sqrVec1;
   c7x::ushort_vec outVal00, outVal01, outVal10, outVal11, outVal0, outVal1;
   c7x::ushort_vec sumVec0, sumVec1;

   c7x::float_vec sumfVec00, sumfVec01, sumfVec10, sumfVec11, sqrtVec00, sqrtVec01, sqrtVec10, sqrtVec11;

   for (uint32_t counter = 0; counter < numBlocks; counter++) {

      c7x::short_vec val0 = c7x::strm_eng<0, c7x::short_vec>::get_adv(); // fetch input 0 via SE0
      c7x::short_vec val1 = c7x::strm_eng<1, c7x::short_vec>::get_adv(); // fetch input 1 via SE1

      sqrVec0 = (val0 * val0);
      sqrVec1 = (val1 * val1);

      sumVec0 = c7x::as_ushort_vec(sqrVec0) + c7x::as_ushort_vec(sqrVec1);

      sumfVec00 = __low_half_int_to_float(sumVec0);
      sumfVec01 = __high_half_int_to_float(sumVec0);

      val0 = c7x::strm_eng<0, c7x::short_vec>::get_adv();
      val1 = c7x::strm_eng<1, c7x::short_vec>::get_adv();

      sqrVec0 = (val0 * val0);
      sqrVec1 = (val1 * val1);

      sumVec1 = c7x::as_ushort_vec(sqrVec0) + c7x::as_ushort_vec(sqrVec1);

      sumfVec10 = __low_half_int_to_float(sumVec1);
      sumfVec11 = __high_half_int_to_float(sumVec1);

      sqrtVec00 = getSqrt(sumfVec00);
      sqrtVec01 = getSqrt(sumfVec01);
      sqrtVec10 = getSqrt(sumfVec10);
      sqrtVec11 = getSqrt(sumfVec11);

      outVal00 = c7x::as_ushort_vec(__vspinthpk_vv(sqrtVec00));
      outVal01 = c7x::as_ushort_vec(__vspinthpk_vv(sqrtVec01));

      outVal0 = c7x::as_ushort_vec(__vpackw_vvv(c7x::as_uint_vec(outVal01), c7x::as_uint_vec(outVal00)));

      outVal10 = c7x::as_ushort_vec(__vspinthpk_vv(sqrtVec10));
      outVal11 = c7x::as_ushort_vec(__vspinthpk_vv(sqrtVec11));

      outVal1 = c7x::as_ushort_vec(__vpackw_vvv(c7x::as_uint_vec(outVal11), c7x::as_uint_vec(outVal10)));

      //  pack and store.
      __vpred tmp  = c7x::strm_agen<0, outVec>::get_vpred();        // generate predicate
      outVec *addr = c7x::strm_agen<0, outVec>::get_adv(pOutLocal); // generate address
      __vstore_pred_packl_2src(tmp, addr, outVal0, outVal1);        // store value
   }

   // close SE/SA
   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA0_CLOSE();

   return status;
}
void VXLIB_normL2_perfEst(VXLIB_kernelHandle                handle,
                               const VXLIB_bufParams2D_t        *bufParamsIn0,
                               const VXLIB_bufParams2D_t        *bufParamsIn1,
                               const VXLIB_bufParams2D_t        *bufParamsOut,
                               const VXLIB_normL2_InitArgs *pKerInitArgs,
                               size_t                           *archCycles,
                               size_t                           *estCycles)
{

   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_normL2_PrivArgs *pKerPrivArgs = (VXLIB_normL2_PrivArgs *) handle;

   const uint32_t dTypeIn0 = bufParamsIn0->data_type;
   const uint32_t dTypeIn1 = bufParamsIn1->data_type;
   const uint32_t dTypeOut = bufParamsOut->data_type;

   // obtain loop count for compute loop
   size_t numBlocks   = pKerPrivArgs->numBlocks;
   size_t overheadCnt = 110; // profiled code before entering compute loop
   size_t iterConst;
   size_t ii;

   if ((dTypeIn0 == VXLIB_INT8 && dTypeIn1 == VXLIB_INT8 && dTypeOut == VXLIB_UINT8)) {
      iterConst = 56;
      ii        = 20;
   }
   else {
      iterConst = 56;
      ii        = 11;
   }
   *archCycles = iterConst + numBlocks * ii;
   *estCycles  = *archCycles + overheadCnt;
}

/* ======================================================================== */
/*  End of file:  VXLIB_normL2_ci.cpp                                       */
/* ======================================================================== */
