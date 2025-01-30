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

/*******************************************************************************
 *
 * INCLUDES
 *
 ******************************************************************************/

#include "VXLIB_convertDepth_priv.h"

#if defined(_HOST_BUILD)
// clang-format off
const c7x::uchar_vec VX_vperm_data_0_63 = c7x::uchar_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
#else
const c7x::uchar_vec VX_vperm_data_0_63 = c7x::uchar_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
#endif
// clang-format on
/*******************************************************************************
 *
 * DEFINES
 *
 ******************************************************************************/

#define SE_PARAM_BASE (0x0000)
#define SE_SE0_PARAM_OFFSET (SE_PARAM_BASE)
#define SE_SE1_PARAM_OFFSET (SE_SE0_PARAM_OFFSET + SE_PARAM_SIZE)
#define SE_SA0_PARAM_OFFSET (SE_SE1_PARAM_OFFSET + SE_PARAM_SIZE)

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_convertDepth_init_ci                                                                                         */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method initializes the kernel-specific parameters
// mainly, the streaming engine and streaming address generators
template <uint32_t dTypeIn0, uint32_t dTypeOut>
VXLIB_STATUS VXLIB_convertDepth_init_ci(VXLIB_kernelHandle                 handle,
                                        const VXLIB_bufParams2D_t         *bufParamsIn0,
                                        const VXLIB_bufParams2D_t         *bufParamsOut,
                                        const VXLIB_convertDepth_InitArgs *pKerInitArgs)
{
   VXLIB_STATUS     status = VXLIB_SUCCESS;
   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;
   __SA_TEMPLATE_v1 sa0Params;

   __SE_ELETYPE SE_ELETYPE;
   __SE_VECLEN  SE_VECLEN;
   __SA_VECLEN  SA_VECLEN;

   VXLIB_convertDepth_PrivArgs *pKerPrivArgs = (VXLIB_convertDepth_PrivArgs *) handle;
   uint8_t                     *pBlock       = pKerPrivArgs->bufPblock;

   /**********************************************************************/
   /* Prepare SE/SA params for 8->16 bit images                          */
   /**********************************************************************/

   if ((dTypeIn0 == VXLIB_UINT8) && (dTypeOut == VXLIB_INT16)) {
      SE_VECLEN  = c7x::se_veclen<c7x::short_vec>::value;
      SE_ELETYPE = c7x::se_eletype<c7x::uchar_vec>::value;
      SA_VECLEN  = c7x::sa_veclen<c7x::short_vec>::value;

      /**********************************************************************/
      /* SE/SA params for stride != width 8->16 bit image                   */
      /**********************************************************************/

      // SE parameters are the same regardless if image width is > or < SIMD_WIDTH
      if (pKerPrivArgs->sePolicy == VXLIB_CONVERTDEPTH_SE_NARROW ||
          pKerPrivArgs->sePolicy == VXLIB_CONVERTDEPTH_SE_WIDE) {

         size_t elemCount = VXLIB_max_simd<dTypeIn0>::value;
         size_t wBlocks   = (pKerPrivArgs->width + (elemCount - 1)) / (elemCount);
         size_t numBlocks = pKerPrivArgs->height * wBlocks;

         if (pKerPrivArgs->sePolicy == VXLIB_CONVERTDEPTH_SE_WIDE) {
            pKerPrivArgs->numBlocks = numBlocks * 2;
         }
         else {
            pKerPrivArgs->numBlocks = numBlocks;
         }

         /**********************************************************************/
         /* Prepare SE0 template to fetch A matrix                             */
         /**********************************************************************/

         se0Params         = __gen_SE_TEMPLATE_v1();
         se0Params.ICNT0   = bufParamsIn0->dim_x;
         se0Params.ICNT1   = bufParamsIn0->dim_y;
         se0Params.DIMFMT  = __SE_DIMFMT_2D;
         se0Params.PROMOTE = __SE_PROMOTE_2X_ZEROEXT;

         se0Params.DIM1    = pKerPrivArgs->strideIn0Elements;
         se0Params.ELETYPE = SE_ELETYPE;
         se0Params.VECLEN  = SE_VECLEN;

         /**********************************************************************/
         /* Prepare SA0 template to fetch C matrix                             */
         /**********************************************************************/
         sa0Params        = __gen_SA_TEMPLATE_v1();
         sa0Params.ICNT0  = bufParamsOut->dim_x;
         sa0Params.ICNT1  = bufParamsOut->dim_y;
         sa0Params.DIMFMT = __SA_DIMFMT_2D;
         sa0Params.DIM1   = pKerPrivArgs->strideOutElements;
         sa0Params.VECLEN = SA_VECLEN;

         *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
         *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET) = sa0Params;
      }

      /**********************************************************************/
      /* SE/SA params for stride = width 8->16 bit image                    */
      /**********************************************************************/

      else if (pKerPrivArgs->sePolicy == VXLIB_CONVERTDEPTH_SE_1D) {
         /**********************************************************************/
         /* Prepare SE0 template to fetch A matrix                             */
         /**********************************************************************/
         size_t elemCount = VXLIB_max_simd<dTypeIn0>::value;
         size_t numBlocks = (bufParamsIn0->dim_x * bufParamsIn0->dim_y) / (elemCount / 2);
         if (((bufParamsIn0->dim_x * bufParamsIn0->dim_y) % (elemCount / 2)) != 0) {
            numBlocks++;
         }
         pKerPrivArgs->numBlocks = numBlocks;

         se0Params         = __gen_SE_TEMPLATE_v1();
         se0Params.ICNT0   = bufParamsIn0->dim_x * bufParamsIn0->dim_y;
         se0Params.DIMFMT  = __SE_DIMFMT_1D;
         se0Params.PROMOTE = __SE_PROMOTE_2X_ZEROEXT;
         se0Params.ELETYPE = SE_ELETYPE;
         se0Params.VECLEN  = SE_VECLEN;

         /**********************************************************************/
         /* Prepare SA0 template to fetch C matrix                             */
         /**********************************************************************/
         sa0Params        = __gen_SA_TEMPLATE_v1();
         sa0Params.ICNT0  = bufParamsOut->dim_x * bufParamsOut->dim_y;
         sa0Params.DIMFMT = __SA_DIMFMT_1D;
         sa0Params.VECLEN = SA_VECLEN;

         *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
         *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET) = sa0Params;
      }
      else {
        /* Do nothing */
      }
   }

   /**********************************************************************/
   /* Prepare SE/SA params for 16->8 bit images                          */
   /**********************************************************************/
   else {
      SE_VECLEN  = c7x::se_veclen<c7x::short_vec>::value;
      SE_ELETYPE = c7x::se_eletype<c7x::short_vec>::value;
      SA_VECLEN  = c7x::sa_veclen<c7x::uchar_vec>::value;

      /*******************************************************************************/
      /* SE/SA params for stride != width and width > SIMD_WIDTH for 16->8 bit image */
      /*******************************************************************************/

      if (pKerPrivArgs->sePolicy == VXLIB_CONVERTDEPTH_SE_WIDE) {

         size_t elemCount = VXLIB_max_simd<dTypeIn0>::value;
         size_t wBlocks   = (pKerPrivArgs->width + 4 * elemCount - 1) / (4 * elemCount);
         size_t numBlocks = pKerPrivArgs->height * wBlocks;

         if (pKerPrivArgs->width <= 2 * elemCount) {
            pKerPrivArgs->numBlocks = (numBlocks + 1) / 2;
         }
         else {
            pKerPrivArgs->numBlocks = numBlocks * 2;
         }

         /**********************************************************************/
         /* Prepare SE0 template to fetch A matrix                             */
         /**********************************************************************/
         se0Params               = __gen_SE_TEMPLATE_v1();
         se0Params.ICNT0         = elemCount;
         se0Params.ICNT1         = (bufParamsIn0->dim_x + 2 * elemCount - 1) / (2 * elemCount);
         se0Params.ICNT2         = bufParamsIn0->dim_y;
         se0Params.DIMFMT        = __SE_DIMFMT_3D;
         se0Params.DIM1          = 2 * elemCount;
         se0Params.DIM2          = pKerPrivArgs->strideIn0Elements;
         se0Params.DECDIM1       = __SE_DECDIM_DIM1;
         se0Params.DECDIM1_WIDTH = bufParamsIn0->dim_x;
         se0Params.DECDIM2       = __SE_DECDIM_DIM2;
         se0Params.DECDIM2_WIDTH = bufParamsIn0->stride_y * bufParamsIn0->dim_y;
         se0Params.ELETYPE       = SE_ELETYPE;
         se0Params.VECLEN        = SE_VECLEN;

         /**********************************************************************/
         /* Prepare SE1 template to fetch A matrix                             */
         /**********************************************************************/
         se1Params               = __gen_SE_TEMPLATE_v1();
         se1Params.ICNT0         = elemCount;
         se1Params.ICNT1         = (bufParamsIn0->dim_x + 2 * elemCount - 1) / (2 * elemCount);
         se1Params.ICNT2         = bufParamsIn0->dim_y;
         se1Params.DIMFMT        = __SE_DIMFMT_3D;
         se1Params.DIM1          = 2 * elemCount;
         se1Params.DIM2          = pKerPrivArgs->strideIn0Elements;
         se1Params.DECDIM1       = __SE_DECDIM_DIM1;
         se1Params.DECDIM1_WIDTH = bufParamsIn0->dim_x - elemCount;
         se1Params.DECDIM2       = __SE_DECDIM_DIM2;
         se1Params.DECDIM2_WIDTH = bufParamsIn0->stride_y * bufParamsIn0->dim_y;
         se1Params.ELETYPE       = SE_ELETYPE;
         se1Params.VECLEN        = SE_VECLEN;

         /**********************************************************************/
         /* Prepare SA0 template to fetch C matrix                             */
         /**********************************************************************/
         sa0Params        = __gen_SA_TEMPLATE_v1();
         sa0Params.ICNT0  = bufParamsOut->dim_x;
         sa0Params.ICNT1  = bufParamsOut->dim_y;
         sa0Params.DIMFMT = __SA_DIMFMT_2D;

         sa0Params.DIM1   = pKerPrivArgs->strideOutElements;
         sa0Params.VECLEN = SA_VECLEN;

         *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
         *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET) = se1Params;
         *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET) = sa0Params;
      }

      /***********************************************************/
      /* SE/SA params for stride = width and for 16->8 bit image */
      /***********************************************************/
      else if (pKerPrivArgs->sePolicy == VXLIB_CONVERTDEPTH_SE_1D) {

         /*******************************************************************************/
         /* SE params for stride = width and width > SIMD_WIDTH for 16->8 bit image */
         /*******************************************************************************/
         size_t elemCount = VXLIB_max_simd<dTypeIn0>::value;
         if (bufParamsIn0->dim_x > elemCount) {

            size_t numBlocks = (bufParamsIn0->dim_x * bufParamsIn0->dim_y) / (elemCount * 4);
            if (((bufParamsIn0->dim_x * bufParamsIn0->dim_y) % (elemCount * 4)) != 0) {
               numBlocks++;
            }
            pKerPrivArgs->numBlocks = numBlocks;
            uint32_t factor         = (bufParamsIn0->dim_x + 2 * elemCount - 1) / (2 * elemCount);
            /**********************************************************************/
            /* Prepare SE0 template to fetch A matrix                             */
            /**********************************************************************/

            se0Params               = __gen_SE_TEMPLATE_v1();
            se0Params.ICNT0         = elemCount;
            se0Params.ICNT1         = bufParamsIn0->dim_y * factor;
            se0Params.DIMFMT        = __SE_DIMFMT_2D;
            se0Params.DECDIM1       = __SE_DECDIM_DIM1;
            se0Params.DECDIM1_WIDTH = bufParamsIn0->stride_y * bufParamsIn0->dim_y;
            se0Params.DIM1          = 2 * elemCount;
            se0Params.ELETYPE       = SE_ELETYPE;
            se0Params.VECLEN        = SE_VECLEN;

            /**********************************************************************/
            /* Prepare SE1 template to fetch A matrix                             */
            /**********************************************************************/

            se1Params               = __gen_SE_TEMPLATE_v1();
            se1Params.ICNT0         = elemCount;
            se1Params.ICNT1         = bufParamsIn0->dim_y * factor;
            se1Params.DIMFMT        = __SE_DIMFMT_2D;
            se1Params.DECDIM1       = __SE_DECDIM_DIM1;
            se1Params.DECDIM1_WIDTH = bufParamsIn0->stride_y * bufParamsIn0->dim_y;
            se1Params.DIM1          = 2 * elemCount;
            se1Params.ELETYPE       = SE_ELETYPE;
            se1Params.VECLEN        = SE_VECLEN;

            *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET) = se1Params;
         }

         /***************************************************************************/
         /* SE params for stride = width and width < SIMD_WIDTH for 16->8 bit image */
         /***************************************************************************/
         else {

            /**********************************************************************/
            /* Prepare SE0 template to fetch A matrix                             */
            /**********************************************************************/

            se0Params         = __gen_SE_TEMPLATE_v1();
            se0Params.ICNT0   = bufParamsIn0->dim_x * bufParamsIn0->dim_y;
            se0Params.DIMFMT  = __SE_DIMFMT_1D;
            se0Params.ELETYPE = SE_ELETYPE;
            se0Params.VECLEN  = SE_VECLEN;

            size_t numBlocks = (bufParamsIn0->dim_x * bufParamsIn0->dim_y) / (elemCount * 4);
            if (((bufParamsIn0->dim_x * bufParamsIn0->dim_y) % (elemCount * 4)) != 0) {
               numBlocks++;
            }
            pKerPrivArgs->numBlocks = numBlocks;
         }

         /***************************************************************************/
         /* SA params for stride = width and for 16->8 bit image */
         /***************************************************************************/

         // SA params are the same regardless of width < or > SIMD_WIDTH

         sa0Params        = __gen_SA_TEMPLATE_v1();
         sa0Params.ICNT0  = bufParamsOut->dim_x * bufParamsOut->dim_y;
         sa0Params.DIMFMT = __SA_DIMFMT_1D;
         sa0Params.VECLEN = SA_VECLEN;

         *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
         *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET) = sa0Params;
      }

      /*******************************************************************************/
      /* SE/SA params for stride != width and width < SIMD_WIDTH for 16->8 bit image */
      /*******************************************************************************/
      else  {
         size_t elemCount        = VXLIB_max_simd<dTypeIn0>::value;
         size_t wBlocks          = (pKerPrivArgs->width + (elemCount - 1)) / (elemCount);
         size_t numBlocks        = pKerPrivArgs->height * wBlocks;
         pKerPrivArgs->numBlocks = numBlocks;
         /**********************************************************************/
         /* Prepare SE0 template to fetch A matrix                             */
         /**********************************************************************/

         se0Params        = __gen_SE_TEMPLATE_v1();
         se0Params.ICNT0  = bufParamsIn0->dim_x;
         se0Params.ICNT1  = bufParamsIn0->dim_y;
         se0Params.DIMFMT = __SE_DIMFMT_2D;

         se0Params.DIM1    = pKerPrivArgs->strideIn0Elements;
         se0Params.ELETYPE = SE_ELETYPE;
         se0Params.VECLEN  = SE_VECLEN;

         /**********************************************************************/
         /* Prepare SA0 template to fetch C matrix                             */
         /**********************************************************************/
         sa0Params        = __gen_SA_TEMPLATE_v1();
         sa0Params.ICNT0  = bufParamsOut->dim_x;
         sa0Params.ICNT1  = bufParamsOut->dim_y;
         sa0Params.DIMFMT = __SA_DIMFMT_2D;
         sa0Params.DIM1   = pKerPrivArgs->strideOutElements;
         sa0Params.VECLEN = SA_VECLEN;

         *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
         *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET) = sa0Params;
      }

   }

   return status;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* Explicit instantiations for VXLIB_convertDepth_init_ci                                                             */
/*                                                                                                                    */
/**********************************************************************************************************************/

template VXLIB_STATUS
VXLIB_convertDepth_init_ci<VXLIB_UINT8, VXLIB_INT16>(VXLIB_kernelHandle                 handle,
                                                     const VXLIB_bufParams2D_t         *bufParamsIn0,
                                                     const VXLIB_bufParams2D_t         *bufParamsOut,
                                                     const VXLIB_convertDepth_InitArgs *pKerInitArgs);

template VXLIB_STATUS
VXLIB_convertDepth_init_ci<VXLIB_INT16, VXLIB_UINT8>(VXLIB_kernelHandle                 handle,
                                                     const VXLIB_bufParams2D_t         *bufParamsIn0,
                                                     const VXLIB_bufParams2D_t         *bufParamsOut,
                                                     const VXLIB_convertDepth_InitArgs *pKerInitArgs);

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_convertDepth_calc                                                                                            */
/*                                                                                                                    */
/**********************************************************************************************************************/
template <typename dTypeIn0, typename dTypeOut>
inline void VXLIB_convertDepth_wrap_wide(dTypeOut *pOut, size_t size, uint8_t shift)
{
}

template <> inline void VXLIB_convertDepth_wrap_wide<int16_t, uint8_t>(uint8_t *pOut, size_t size, uint8_t shift)
{
   typedef typename c7x::make_full_vector<int16_t>::type vecIn;
   typedef typename c7x::make_full_vector<uint8_t>::type vecOut;

   for (uint32_t counter = 0; counter < size; counter++) {
      vecIn a0 = c7x::strm_eng<0, vecIn>::get_adv();
      vecIn b0 = c7x::strm_eng<1, vecIn>::get_adv();
      vecIn a1 = c7x::strm_eng<0, vecIn>::get_adv();
      vecIn b1 = c7x::strm_eng<1, vecIn>::get_adv();

      vecIn a0Shift = __shift_right(a0, c7x::int_vec(shift));
      vecIn b0Shift = __shift_right(b0, c7x::int_vec(shift));
      vecIn a1Shift = __shift_right(a1, c7x::int_vec(shift));
      vecIn b1Shift = __shift_right(b1, c7x::int_vec(shift));

      vecOut a = c7x::reinterpret<vecOut>(__permute_even_even_char(
          VX_vperm_data_0_63, c7x::as_uchar_vec(b0Shift), c7x::as_uchar_vec(a0Shift)));
      vecOut b = c7x::reinterpret<vecOut>(__permute_even_even_char(
          VX_vperm_data_0_63, c7x::as_uchar_vec(b1Shift), c7x::as_uchar_vec(a1Shift)));

      __vpred tmp0  = c7x::strm_agen<0, vecOut>::get_vpred();
      vecOut *addr0 = c7x::strm_agen<0, vecOut>::get_adv(pOut);
      __vstore_pred(tmp0, addr0, a);
      __vpred tmp1  = c7x::strm_agen<0, vecOut>::get_vpred();
      vecOut *addr1 = c7x::strm_agen<0, vecOut>::get_adv(pOut);
      __vstore_pred(tmp1, addr1, b);
   }
}

template <> inline void VXLIB_convertDepth_wrap_wide<uint8_t, int16_t>(int16_t *pOut, size_t size, uint8_t shift)
{
   typedef typename c7x::make_full_vector<int16_t>::type vecOut;

   for (uint32_t counter = 0; counter < size; counter++) {
      vecOut a0      = c7x::strm_eng<0, vecOut>::get_adv();
      vecOut a0Shift = __shift_left(a0, c7x::int_vec(shift));

      __vpred tmp0  = c7x::strm_agen<0, vecOut>::get_vpred();
      vecOut *addr0 = c7x::strm_agen<0, vecOut>::get_adv(pOut);
      __vstore_pred(tmp0, addr0, a0Shift);
   }
}

template <typename dTypeIn0, typename dTypeOut>
inline void VXLIB_convertDepth_sat_wide(dTypeOut *pOut, size_t size, uint8_t shift)
{
}

template <> inline void VXLIB_convertDepth_sat_wide<int16_t, uint8_t>(uint8_t *pOut, size_t size, uint8_t shift)
{
   typedef typename c7x::make_full_vector<int16_t>::type vecIn;
   typedef typename c7x::make_full_vector<uint8_t>::type vecOut;

   vecIn vecZero      = (vecIn) 0;
   vecIn vecUINT8_MAX = (vecIn) UINT8_MAX;

   for (uint32_t counter = 0; counter < size; counter++) {
      vecIn a0 = c7x::strm_eng<0, vecIn>::get_adv();
      vecIn b0 = c7x::strm_eng<1, vecIn>::get_adv();
      vecIn a1 = c7x::strm_eng<0, vecIn>::get_adv();
      vecIn b1 = c7x::strm_eng<1, vecIn>::get_adv();

      vecIn a0Shift = __shift_right(a0, c7x::int_vec(shift));
      vecIn b0Shift = __shift_right(b0, c7x::int_vec(shift));
      vecIn a1Shift = __shift_right(a1, c7x::int_vec(shift));
      vecIn b1Shift = __shift_right(b1, c7x::int_vec(shift));

      __vpred cmp_max_a0 = __cmp_gt_pred(vecUINT8_MAX, a0Shift);
      a0Shift            = __select(cmp_max_a0, a0Shift, vecUINT8_MAX);
      __vpred cmp_max_b0 = __cmp_gt_pred(vecUINT8_MAX, b0Shift);
      b0Shift            = __select(cmp_max_b0, b0Shift, vecUINT8_MAX);
      __vpred cmp_max_a1 = __cmp_gt_pred(vecUINT8_MAX, a1Shift);
      a1Shift            = __select(cmp_max_a1, a1Shift, vecUINT8_MAX);
      __vpred cmp_max_b1 = __cmp_gt_pred(vecUINT8_MAX, b1Shift);
      b1Shift            = __select(cmp_max_b1, b1Shift, vecUINT8_MAX);

      __vpred cmp_min_a0 = __cmp_gt_pred(a0Shift, vecZero);
      a0Shift            = __select(cmp_min_a0, a0Shift, vecZero);
      __vpred cmp_min_b0 = __cmp_gt_pred(b0Shift, vecZero);
      b0Shift            = __select(cmp_min_b0, b0Shift, vecZero);
      __vpred cmp_min_a1 = __cmp_gt_pred(a1Shift, vecZero);
      a1Shift            = __select(cmp_min_a1, a1Shift, vecZero);
      __vpred cmp_min_b1 = __cmp_gt_pred(b1Shift, vecZero);
      b1Shift            = __select(cmp_min_b1, b1Shift, vecZero);

      vecOut a = c7x::reinterpret<vecOut>(__permute_even_even_char(
          VX_vperm_data_0_63, c7x::as_uchar_vec(b0Shift), c7x::as_uchar_vec(a0Shift)));
      vecOut b = c7x::reinterpret<vecOut>(__permute_even_even_char(
          VX_vperm_data_0_63, c7x::as_uchar_vec(b1Shift), c7x::as_uchar_vec(a1Shift)));

      __vpred tmp0  = c7x::strm_agen<0, vecOut>::get_vpred();
      vecOut *addr0 = c7x::strm_agen<0, vecOut>::get_adv(pOut);
      __vstore_pred(tmp0, addr0, a);
      __vpred tmp1  = c7x::strm_agen<0, vecOut>::get_vpred();
      vecOut *addr1 = c7x::strm_agen<0, vecOut>::get_adv(pOut);
      __vstore_pred(tmp1, addr1, b);
   }
}

template <typename dTypeIn0, typename dTypeOut>
inline void VXLIB_convertDepth_sat_narrow(dTypeOut *pOut, size_t size, uint8_t shift)
{
}

template <> inline void VXLIB_convertDepth_sat_narrow<int16_t, uint8_t>(uint8_t *pOut, size_t size, uint8_t shift)
{
   typedef typename c7x::make_full_vector<int16_t>::type vecIn;
   typedef typename c7x::make_full_vector<uint8_t>::type vecOut;

   vecIn vecZero      = (vecIn) 0;
   vecIn vecUINT8_MAX = (vecIn) UINT8_MAX;

   for (uint32_t counter = 0; counter < size; counter++) {
      vecIn a0 = c7x::strm_eng<0, vecIn>::get_adv();

      vecIn a0Shift = (__shift_right(a0, c7x::int_vec(shift)));

      __vpred cmp_max_a0 = __cmp_gt_pred(vecUINT8_MAX, a0Shift);
      a0Shift            = __select(cmp_max_a0, a0Shift, vecUINT8_MAX);
      __vpred cmp_min_a0 = __cmp_gt_pred(a0Shift, vecZero);
      a0Shift            = __select(cmp_min_a0, a0Shift, vecZero);

      vecOut a0Low = c7x::reinterpret<vecOut>(__permute_even_even_char(
          VX_vperm_data_0_63, c7x::as_uchar_vec(vecZero), c7x::as_uchar_vec(a0Shift)));

      __vpred tmp0  = c7x::strm_agen<0, vecOut>::get_vpred();
      vecOut *addr0 = c7x::strm_agen<0, vecOut>::get_adv(pOut);
      __vstore_pred(tmp0, addr0, a0Low);
   }
}

template <typename dTypeIn0, typename dTypeOut>
inline void VXLIB_convertDepth_wrap_narrow(dTypeOut *pOut, size_t size, uint8_t shift)
{
}

template <> inline void VXLIB_convertDepth_wrap_narrow<uint8_t, int16_t>(int16_t *pOut, size_t size, uint8_t shift)
{
   typedef typename c7x::make_full_vector<int16_t>::type vecOut;

   for (uint32_t counter = 0; counter < size; counter++) {
      vecOut a0      = c7x::strm_eng<0, vecOut>::get_adv();
      vecOut a0Shift = __shift_left(a0, c7x::int_vec(shift));

      __vpred tmp0  = c7x::strm_agen<0, vecOut>::get_vpred();
      vecOut *addr0 = c7x::strm_agen<0, vecOut>::get_adv(pOut);
      __vstore_pred(tmp0, addr0, a0Shift);
   }
}

template <> inline void VXLIB_convertDepth_wrap_narrow<int16_t, uint8_t>(uint8_t *pOut, size_t size, uint8_t shift)
{
   typedef typename c7x::make_full_vector<int16_t>::type vecIn;
   typedef typename c7x::make_full_vector<uint8_t>::type vecOut;

   vecIn zero = (vecIn) 0;

   for (uint32_t counter = 0; counter < size; counter++) {
      vecIn a0 = c7x::strm_eng<0, vecIn>::get_adv();

      vecIn a0Shift = (__shift_right(a0, c7x::int_vec(shift)));

      vecOut a0Low = c7x::reinterpret<vecOut>(__permute_even_even_char(
          VX_vperm_data_0_63, c7x::as_uchar_vec(zero), c7x::as_uchar_vec(a0Shift)));

      __vpred tmp0  = c7x::strm_agen<0, vecOut>::get_vpred();
      vecOut *addr0 = c7x::strm_agen<0, vecOut>::get_adv(pOut);
      __vstore_pred(tmp0, addr0, a0Low);
   }
}

template <typename dTypeIn0, typename dTypeOut>
inline void VXLIB_convertDepth_wrap_narrow_1D(dTypeOut *pOut, size_t size, uint8_t shift)
{
}

template <> inline void VXLIB_convertDepth_wrap_narrow_1D<int16_t, uint8_t>(uint8_t *pOut, size_t size, uint8_t shift)
{
   typedef typename c7x::make_full_vector<int16_t>::type vecIn;
   typedef typename c7x::make_full_vector<uint8_t>::type vecOut;

   for (uint32_t counter = 0; counter < size; counter++) {
      vecIn a0 = c7x::strm_eng<0, vecIn>::get_adv();
      vecIn a1 = c7x::strm_eng<0, vecIn>::get_adv();
      vecIn a2 = c7x::strm_eng<0, vecIn>::get_adv();
      vecIn a3 = c7x::strm_eng<0, vecIn>::get_adv();

      vecIn a0Shift = (__shift_right(a0, c7x::int_vec(shift)));
      vecIn a1Shift = (__shift_right(a1, c7x::int_vec(shift)));
      vecIn a2Shift = (__shift_right(a2, c7x::int_vec(shift)));
      vecIn a3Shift = (__shift_right(a3, c7x::int_vec(shift)));

      vecOut a0Low = c7x::reinterpret<vecOut>(__permute_even_even_char(
          VX_vperm_data_0_63, c7x::as_uchar_vec(a1Shift), c7x::as_uchar_vec(a0Shift)));
      vecOut a1Low = c7x::reinterpret<vecOut>(__permute_even_even_char(
          VX_vperm_data_0_63, c7x::as_uchar_vec(a3Shift), c7x::as_uchar_vec(a2Shift)));

      __vpred tmp0  = c7x::strm_agen<0, vecOut>::get_vpred();
      vecOut *addr0 = c7x::strm_agen<0, vecOut>::get_adv(pOut);
      __vstore_pred(tmp0, addr0, a0Low);
      __vpred tmp1  = c7x::strm_agen<0, vecOut>::get_vpred();
      vecOut *addr1 = c7x::strm_agen<0, vecOut>::get_adv(pOut);
      __vstore_pred(tmp1, addr1, a1Low);
   }
}

template <typename dTypeIn0, typename dTypeOut>
inline void VXLIB_convertDepth_sat_narrow_1D(dTypeOut *pOut, size_t size, uint8_t shift)
{
}

template <> inline void VXLIB_convertDepth_sat_narrow_1D<int16_t, uint8_t>(uint8_t *pOut, size_t size, uint8_t shift)
{
   typedef typename c7x::make_full_vector<int16_t>::type vecIn;
   typedef typename c7x::make_full_vector<uint8_t>::type vecOut;

   vecIn vecZero      = (vecIn) 0;
   vecIn vecUINT8_MAX = (vecIn) UINT8_MAX;

   for (uint32_t counter = 0; counter < size; counter++) {
      vecIn a0 = c7x::strm_eng<0, vecIn>::get_adv();
      vecIn a1 = c7x::strm_eng<0, vecIn>::get_adv();
      vecIn a2 = c7x::strm_eng<0, vecIn>::get_adv();
      vecIn a3 = c7x::strm_eng<0, vecIn>::get_adv();

      vecIn a0Shift = (__shift_right(a0, c7x::int_vec(shift)));
      vecIn a1Shift = (__shift_right(a1, c7x::int_vec(shift)));
      vecIn a2Shift = (__shift_right(a2, c7x::int_vec(shift)));
      vecIn a3Shift = (__shift_right(a3, c7x::int_vec(shift)));

      __vpred cmp_max_a0 = __cmp_gt_pred(vecUINT8_MAX, a0Shift);
      a0Shift            = __select(cmp_max_a0, a0Shift, vecUINT8_MAX);
      __vpred cmp_max_a1 = __cmp_gt_pred(vecUINT8_MAX, a1Shift);
      a1Shift            = __select(cmp_max_a1, a1Shift, vecUINT8_MAX);
      __vpred cmp_max_a2 = __cmp_gt_pred(vecUINT8_MAX, a2Shift);
      a2Shift            = __select(cmp_max_a2, a2Shift, vecUINT8_MAX);
      __vpred cmp_max_a3 = __cmp_gt_pred(vecUINT8_MAX, a3Shift);
      a3Shift            = __select(cmp_max_a3, a3Shift, vecUINT8_MAX);

      __vpred cmp_min_a0 = __cmp_gt_pred(a0Shift, vecZero);
      a0Shift            = __select(cmp_min_a0, a0Shift, vecZero);
      __vpred cmp_min_a1 = __cmp_gt_pred(a1Shift, vecZero);
      a1Shift            = __select(cmp_min_a1, a1Shift, vecZero);
      __vpred cmp_min_a2 = __cmp_gt_pred(a2Shift, vecZero);
      a2Shift            = __select(cmp_min_a2, a2Shift, vecZero);
      __vpred cmp_min_a3 = __cmp_gt_pred(a3Shift, vecZero);
      a3Shift            = __select(cmp_min_a3, a3Shift, vecZero);

      vecOut a0Low = c7x::reinterpret<vecOut>(__permute_even_even_char(
          VX_vperm_data_0_63, c7x::as_uchar_vec(a1Shift), c7x::as_uchar_vec(a0Shift)));
      vecOut a1Low = c7x::reinterpret<vecOut>(__permute_even_even_char(
          VX_vperm_data_0_63, c7x::as_uchar_vec(a3Shift), c7x::as_uchar_vec(a2Shift)));

      __vpred tmp0  = c7x::strm_agen<0, vecOut>::get_vpred();
      vecOut *addr0 = c7x::strm_agen<0, vecOut>::get_adv(pOut);
      __vstore_pred(tmp0, addr0, a0Low);
      __vpred tmp1  = c7x::strm_agen<0, vecOut>::get_vpred();
      vecOut *addr1 = c7x::strm_agen<0, vecOut>::get_adv(pOut);
      __vstore_pred(tmp1, addr1, a1Low);
   }
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_convertDepth_exec_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method performs pixel-wise absolute difference of two input images
template <typename dTypeIn0, typename dTypeOut>
VXLIB_STATUS VXLIB_convertDepth_exec_ci(VXLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut)
{
   VXLIB_convertDepth_PrivArgs *pKerPrivArgs = (VXLIB_convertDepth_PrivArgs *) handle;

#if VXLIB_DEBUGPRINT
   printf("Enter VXLIB_convertDepth_exec_ci\n");
#endif

   uint8_t *pBlock    = pKerPrivArgs->bufPblock;
   size_t   numBlocks = pKerPrivArgs->numBlocks;
   uint8_t  shift     = pKerPrivArgs->shift;

   // int16 is used in both convertDepth types due to SE promote for 8->16 bit
   size_t SIMD_WIDTH = c7x::max_simd<int16_t>::value;
   dTypeOut *restrict pOutLocal;

   /**************************************************************************/
   /* Open appropriate SE/SA                                                 */
   /**************************************************************************/

   // For images with widths larger than SIMD_WIDTH pixels and for 16->8 bit conversion, utilize 2 SEs
   // Ex: For C7100/C7120 if the width of the image is larger than 32 pixels
   if (pKerPrivArgs->width > SIMD_WIDTH && pKerPrivArgs->data_type == VXLIB_INT16) {
      __SE_TEMPLATE_v1 se0Params;
      __SE_TEMPLATE_v1 se1Params;
      __SA_TEMPLATE_v1 sa0Params;

      se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
      se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);
      sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);

      dTypeIn0 *restrict pInLocal0 = (dTypeIn0 *) pIn0;
      dTypeIn0 *restrict pInLocal1 = (dTypeIn0 *) pIn0 + SIMD_WIDTH;
      pOutLocal                    = (dTypeOut *) pOut;

      // Input samples
      __SE0_OPEN(pInLocal0, se0Params);
      __SE1_OPEN(pInLocal1, se1Params);

      // Output samples
      __SA0_OPEN(sa0Params);
   }
   // For images with widths of <= SIMD_WIDTH or for 8->16 bit conversion, utilize 1 SE
   else {
      __SE_TEMPLATE_v1 se0Params;
      __SA_TEMPLATE_v1 sa0Params;

      se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
      sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);

      dTypeIn0 *restrict pInLocal0 = (dTypeIn0 *) pIn0;
      pOutLocal                    = (dTypeOut *) pOut;

      // Input samples
      __SE0_OPEN(pInLocal0, se0Params);

      // Output samples
      __SA0_OPEN(sa0Params);
   }

   /*************************************************************************/
   /* Determining calculation loop depending on sePolicy and overflowPolicy */
   /*************************************************************************/

   /**************************************************************************/
   /* Loops for wrap behavior for 16->8 images and all 8->16 images          */
   /**************************************************************************/
   if (pKerPrivArgs->overFlowPolicy == VXLIB_WRAP && pKerPrivArgs->sePolicy == VXLIB_CONVERTDEPTH_SE_NARROW) {
      VXLIB_convertDepth_wrap_narrow<dTypeIn0, dTypeOut>(pOutLocal, numBlocks, shift);
   }
   else if (pKerPrivArgs->overFlowPolicy == VXLIB_WRAP && pKerPrivArgs->sePolicy == VXLIB_CONVERTDEPTH_SE_WIDE) {
      VXLIB_convertDepth_wrap_wide<dTypeIn0, dTypeOut>(pOutLocal, numBlocks, shift);
   }
   else if (pKerPrivArgs->overFlowPolicy == VXLIB_WRAP && pKerPrivArgs->sePolicy == VXLIB_CONVERTDEPTH_SE_1D) {
      if (pKerPrivArgs->width <= SIMD_WIDTH && pKerPrivArgs->data_type == VXLIB_INT16) {
         VXLIB_convertDepth_wrap_narrow_1D<dTypeIn0, dTypeOut>(pOutLocal, numBlocks, shift);
      }
      else if (pKerPrivArgs->width <= SIMD_WIDTH && pKerPrivArgs->data_type == VXLIB_UINT8) {
         VXLIB_convertDepth_wrap_narrow<dTypeIn0, dTypeOut>(pOutLocal, numBlocks, shift);
      }
      else {
         VXLIB_convertDepth_wrap_wide<dTypeIn0, dTypeOut>(pOutLocal, numBlocks, shift);
      }
   }

   /**************************************************************************/
   /* Loops for saturate behavior for 16->8 images                           */
   /**************************************************************************/
   else if (pKerPrivArgs->overFlowPolicy == VXLIB_SATURATE && pKerPrivArgs->sePolicy == VXLIB_CONVERTDEPTH_SE_1D) {
      if (pKerPrivArgs->width <= SIMD_WIDTH) {
         VXLIB_convertDepth_sat_narrow_1D<dTypeIn0, dTypeOut>(pOutLocal, numBlocks, shift);
      }
      else {
         VXLIB_convertDepth_sat_wide<dTypeIn0, dTypeOut>(pOutLocal, numBlocks, shift);
      }
   }
   else if (pKerPrivArgs->overFlowPolicy == VXLIB_SATURATE && pKerPrivArgs->sePolicy == VXLIB_CONVERTDEPTH_SE_WIDE) {
      VXLIB_convertDepth_sat_wide<dTypeIn0, dTypeOut>(pOutLocal, numBlocks, shift);
   }
   else if (pKerPrivArgs->overFlowPolicy == VXLIB_SATURATE && pKerPrivArgs->sePolicy == VXLIB_CONVERTDEPTH_SE_NARROW) {
      VXLIB_convertDepth_sat_narrow<dTypeIn0, dTypeOut>(pOutLocal, numBlocks, shift);
   }
   else
   {
      /* Nothing to do here.*/
   }

   /**************************************************************************/
   /* Close appropriate SE/SA                                                */
   /**************************************************************************/
   if (pKerPrivArgs->width > SIMD_WIDTH && pKerPrivArgs->data_type == VXLIB_INT16) {
      __SE0_CLOSE();
      __SE1_CLOSE();
      __SA0_CLOSE();
   }
   else {
      __SE0_CLOSE();
      __SA0_CLOSE();
   }

   return VXLIB_SUCCESS;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* Explicit instantiations for VXLIB_convertDepth_exec_ci */
/*                                                                                                                    */
/**********************************************************************************************************************/

template VXLIB_STATUS
VXLIB_convertDepth_exec_ci<uint8_t, int16_t>(VXLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);

template VXLIB_STATUS
VXLIB_convertDepth_exec_ci<int16_t, uint8_t>(VXLIB_kernelHandle handle, void *restrict pIn0, void *restrict pOut);
