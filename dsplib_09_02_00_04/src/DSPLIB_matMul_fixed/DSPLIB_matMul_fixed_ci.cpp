/******************************************************************************/
/* Copyright (C) 2017 Texas Instruments Incorporated - http://www.ti.com/
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

#include "../common/c71/DSPLIB_inlines.h"
#include "DSPLIB_matMul_fixed_priv.h"

#define SE_PARAM_BASE (0x0000)
#define SE0_PARAM_OFFSET (SE_PARAM_BASE)
#define SE1_PARAM_OFFSET (SE0_PARAM_OFFSET + SE_PARAM_SIZE)
#define SA0_PARAM_OFFSET (SE1_PARAM_OFFSET + SE_PARAM_SIZE)
#define DSPLIB_MATMUL_FIXED_UNROLL (32)

/***********************************
 *   Initialization
 ***********************************/
template <typename dataTypeIn, typename dataTypeOut>
static inline void DSPLIB_matMul_fixed_PromoteTranspose_init_ci(__SE_TEMPLATE_v1 *se0Params,
                                                                __SE_TEMPLATE_v1 *se1Params);
template <>
inline void DSPLIB_matMul_fixed_PromoteTranspose_init_ci<DSPLIB_MATMAPY_FXD_I16S_O16S>(__SE_TEMPLATE_v1 *se0Params,
                                                                                       __SE_TEMPLATE_v1 *se1Params)
{
   typedef typename c7x::make_full_vector<int16_t>::type vec;
   __SE_VECLEN                                              SE_VECLEN = c7x::se_veclen<vec>::value;
   se0Params->VECLEN                                                  = SE_VECLEN;
   se0Params->TRANSPOSE = __SE_TRANSPOSE_128BIT;
   se1Params->VECLEN    = SE_VECLEN;
   se1Params->TRANSPOSE = __SE_TRANSPOSE_64BIT;
}
template <>
inline void DSPLIB_matMul_fixed_PromoteTranspose_init_ci<DSPLIB_MATMAPY_FXD_I8S_O8S>(__SE_TEMPLATE_v1 *se0Params,
                                                                                     __SE_TEMPLATE_v1 *se1Params)
{
   se0Params->VECLEN    = __SE_VECLEN_32ELEMS;
   se0Params->PROMOTE   = __SE_PROMOTE_2X_SIGNEXT;
   se0Params->TRANSPOSE = __SE_TRANSPOSE_64BIT;
   se1Params->VECLEN    = __SE_VECLEN_32ELEMS;
   se1Params->PROMOTE   = __SE_PROMOTE_2X_SIGNEXT;
   se1Params->TRANSPOSE = __SE_TRANSPOSE_32BIT;
}
template <typename dataTypeIn, typename dataTypeOut>
DSPLIB_STATUS DSPLIB_matMul_fixed_init_ci(DSPLIB_kernelHandle                 handle,
                                               const DSPLIB_bufParams2D_t         *bufParamsIn0,
                                               const DSPLIB_bufParams2D_t         *bufParamsIn1,
                                               const DSPLIB_bufParams2D_t         *bufParamsOut,
                                               const DSPLIB_matMul_fixed_InitArgs *pKerInitArgs)
{

   DSPLIB_STATUS                 status       = DSPLIB_SUCCESS;
   DSPLIB_matMul_fixed_PrivArgs *pKerPrivArgs = (DSPLIB_matMul_fixed_PrivArgs *) handle;

   uint8_t *pBlock = pKerPrivArgs->bufPblock;

   uint32_t M            = pKerPrivArgs->M;
   uint32_t N            = pKerPrivArgs->N;
   uint32_t K            = pKerPrivArgs->K;
   uint32_t MBlocks      = DSPLIB_ceilingDiv(M, 4);  
   uint32_t NBlocks      = DSPLIB_ceilingDiv(N, 32);
   uint32_t KBlocks      = DSPLIB_ceilingDiv(K, 8);
   pKerPrivArgs->MBlocks = MBlocks;
   pKerPrivArgs->NBlocks = NBlocks;
   pKerPrivArgs->KBlocks = KBlocks;
   int32_t strideIn0     = pKerPrivArgs->strideIn0Elements;
   int32_t strideIn1     = pKerPrivArgs->strideIn1Elements;
   int32_t strideOut     = pKerPrivArgs->strideOutElements;

   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;
   __SA_TEMPLATE_v1 sa0Params;
   typedef typename c7x::make_full_vector<dataTypeIn>::type vec;
   __SE_ELETYPE SE_ELETYPE = c7x::se_eletype<vec>::value;

   se0Params = __gen_SE_TEMPLATE_v1();

   se0Params.ICNT0   = K;
   se0Params.ICNT1   = (4u < M) ? 4 : M; // 4;
   se0Params.DIM1    = strideIn0;
   se0Params.ICNT2   = NBlocks;
   se0Params.DIM2    = 0;
   se0Params.ICNT3   = MBlocks;
   se0Params.DIM3    = strideIn0 * 4;
   se0Params.DIMFMT  = __SE_DIMFMT_4D;
   se0Params.ELETYPE = SE_ELETYPE;

   se1Params         = __gen_SE_TEMPLATE_v1();
   se1Params.ICNT0   = 32;
   se1Params.ICNT1   = (8u < K) ? 8 : K; // 8;
   se1Params.DIM1    = strideIn1;
   se1Params.ICNT2   = KBlocks;
   se1Params.DIM2    = strideIn1 * 8;
   se1Params.ICNT3   = NBlocks;
   se1Params.DIM3    = 32;
   se1Params.ICNT4   = MBlocks;
   se1Params.DIM4    = 0;
   se1Params.DIMFMT  = __SE_DIMFMT_5D;
   se1Params.ELETYPE = SE_ELETYPE;

   se1Params.DECDIM2 = __SE_DECDIM_DIM3;
   se1Params.DECDIM2_WIDTH = N;

   DSPLIB_matMul_fixed_PromoteTranspose_init_ci<dataTypeIn, dataTypeOut>(&se0Params, &se1Params);

   sa0Params        = __gen_SA_TEMPLATE_v1();
   sa0Params.ICNT0  = 32;
   sa0Params.ICNT1  = 4;
   sa0Params.DIM1   = strideOut;
   sa0Params.ICNT2  = NBlocks;
   sa0Params.DIM2   = 32;
   sa0Params.ICNT3  = MBlocks;
   sa0Params.DIM3   = 4 * strideOut;
   sa0Params.DIMFMT = __SA_DIMFMT_4D;
   sa0Params.VECLEN = __SA_VECLEN_32ELEMS;

   sa0Params.DECDIM1       = __SA_DECDIM_DIM3;
   sa0Params.DECDIM1SD     = __SA_DECDIMSD_DIM1;
   sa0Params.DECDIM1_WIDTH = M * strideOut;

   sa0Params.DECDIM2       = __SA_DECDIM_DIM2;
   sa0Params.DECDIM2SD     = __SA_DECDIMSD_DIM0;
   sa0Params.DECDIM2_WIDTH = N;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET) = se0Params;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET) = se1Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET) = sa0Params;
   return status;
}

template DSPLIB_STATUS
DSPLIB_matMul_fixed_init_ci<DSPLIB_MATMAPY_FXD_I8S_O8S>(DSPLIB_kernelHandle                 handle,
                                                             const DSPLIB_bufParams2D_t         *bufParamsIn0,
                                                             const DSPLIB_bufParams2D_t         *bufParamsIn1,
                                                             const DSPLIB_bufParams2D_t         *bufParamsOut,
                                                             const DSPLIB_matMul_fixed_InitArgs *pKerInitArgs);

template DSPLIB_STATUS
DSPLIB_matMul_fixed_init_ci<DSPLIB_MATMAPY_FXD_I16S_O16S>(DSPLIB_kernelHandle                 handle,
                                                               const DSPLIB_bufParams2D_t         *bufParamsIn0,
                                                               const DSPLIB_bufParams2D_t         *bufParamsIn1,
                                                               const DSPLIB_bufParams2D_t         *bufParamsOut,
                                                               const DSPLIB_matMul_fixed_InitArgs *pKerInitArgs);

/***********************************
 *   Implementation
 ***********************************/

template <typename V> static inline c7x::uchar_vec setMask();
template <> inline c7x::uchar_vec                  setMask<int16_t>()
{
   uint8_t        mask[64] = {0,  1,  2,  3,  8,  9,  10, 11, 4,  5,  6,  7,  12, 13, 14, 15, 32, 33, 34, 35, 40, 41,
                              42, 43, 36, 37, 38, 39, 44, 45, 46, 47, 16, 17, 18, 19, 24, 25, 26, 27, 20, 21, 22, 23,
                              28, 29, 30, 31, 48, 49, 50, 51, 56, 57, 58, 59, 52, 53, 54, 55, 60, 61, 62, 63};
   c7x::uchar_vec vMask    = *stov_ptr(c7x::uchar_vec, (uint8_t *) &mask[0]);
   return vMask;
}

template <> inline c7x::uchar_vec setMask<int8_t>()
{
   uint8_t        mask[64] = {0,  1,  4,  5,  8,  9,  12, 13, 2,  3,  6,  7,  10, 11, 14, 15, 32, 33, 36, 37, 40, 41,
                              44, 45, 34, 35, 38, 39, 42, 43, 46, 47, 16, 17, 20, 21, 24, 25, 28, 29, 18, 19, 22, 23,
                              26, 27, 30, 31, 48, 49, 52, 53, 56, 57, 60, 61, 50, 51, 54, 55, 58, 59, 62, 63};
   c7x::uchar_vec vMask    = *stov_ptr(c7x::uchar_vec, (uint8_t *) &mask[0]);
   return vMask;
}

template <typename V, __SE_REG, __SE_REG> static inline void vecMulAcc(V outSum0, V outSum1);

template <>
inline void vecMulAcc<c7x::long_vec &, __SE_REG_0, __SE_REG_1_ADV>(c7x::long_vec &outSum0, c7x::long_vec &outSum1)
{
   c7x::long_vec vOut0, vOut1;
   __vmatmpyhd_vvw(__SE_REG_0, __SE_REG_1_ADV, vOut0, vOut1);
   outSum0 += vOut0;
   outSum1 += vOut1;
}

template <>
inline void vecMulAcc<c7x::long_vec &, __SE_REG_0_ADV, __SE_REG_1_ADV>(c7x::long_vec &outSum0, c7x::long_vec &outSum1)
{
   c7x::long_vec vOut0, vOut1;
   __vmatmpyhd_vvw(__SE_REG_0_ADV, __SE_REG_1_ADV, vOut0, vOut1);
   outSum0 += vOut0;
   outSum1 += vOut1;
}

template <>
inline void vecMulAcc<c7x::int_vec &, __SE_REG_0, __SE_REG_1_ADV>(c7x::int_vec &outSum0, c7x::int_vec &outSum1)
{
   c7x::int_vec vOut = __matmpy(__SE_REG_0, __SE_REG_1_ADV);
   outSum0 += vOut;
}

template <>
inline void vecMulAcc<c7x::int_vec &, __SE_REG_0_ADV, __SE_REG_1_ADV>(c7x::int_vec &outSum0, c7x::int_vec &outSum1)
{
   c7x::int_vec vOut = __matmpy(__SE_REG_0_ADV, __SE_REG_1_ADV);
   outSum0 += vOut;
}

template <typename V> static inline void vecShiftRight(V vec0, V vec1, V shiftVec);

template <>
inline void vecShiftRight<c7x::long_vec &>(c7x::long_vec &vec0, c7x::long_vec &vec1, c7x::long_vec &shiftVec)
{
   vec0 = __shift_right(vec0, shiftVec);
   vec1 = __shift_right(vec1, shiftVec);
}

template <> inline void vecShiftRight<c7x::int_vec &>(c7x::int_vec &vec0, c7x::int_vec &vec1, c7x::int_vec &shiftVec)
{
   vec0 = __shift_right(vec0, shiftVec);
}

template <typename V> static inline void resetVec(V vec0, V vec1);

template <> inline void resetVec<c7x::long_vec &>(c7x::long_vec &vec0, c7x::long_vec &vec1)
{
   vec0 = (c7x::long_vec) 0;
   vec1 = (c7x::long_vec) 0;
}

template <> inline void resetVec<c7x::int_vec &>(c7x::int_vec &vec0, c7x::int_vec &vec1) { vec0 = (c7x::int_vec) 0; }

template <typename V> static inline void packAlternate(V vec0, V vec1, V vec2, V vec3, V vecOut0, V vecOut1);

template <>
inline void packAlternate<c7x::long_vec &>(c7x::long_vec &vec0,
                                           c7x::long_vec &vec1,
                                           c7x::long_vec &vec2,
                                           c7x::long_vec &vec3,
                                           c7x::long_vec &vecOut0,
                                           c7x::long_vec &vecOut1)
{
   vecOut0 = c7x::as_long_vec(__vpackw_vvv(c7x::as_int_vec(vec2), c7x::as_int_vec(vec0)));
   vecOut1 = c7x::as_long_vec(__vpackw_vvv(c7x::as_int_vec(vec3), c7x::as_int_vec(vec1)));
}

template <>
inline void packAlternate<c7x::int_vec &>(c7x::int_vec &vec0,
                                          c7x::int_vec &vec1,
                                          c7x::int_vec &vec2,
                                          c7x::int_vec &vec3,
                                          c7x::int_vec &vecOut0,
                                          c7x::int_vec &vecOut1)
{
   vecOut0 = c7x::as_int_vec(__vpackl2_vvv(c7x::as_short_vec(vec2), c7x::as_short_vec(vec0)));
}

template <typename V, typename W>
static inline void
vecPermutePack(V vec1, V vec2, V vec3, V vec4, V vec5, V vec6, V vec7, V vec8, W pOutLocal, c7x::uchar_vec vMask);

template <>
inline void vecPermutePack<c7x::long_vec &, int16_t *>(c7x::long_vec &out1,
                                                       c7x::long_vec &out2,
                                                       c7x::long_vec &out3,
                                                       c7x::long_vec &out4,
                                                       c7x::long_vec &out5,
                                                       c7x::long_vec &out6,
                                                       c7x::long_vec &out7,
                                                       c7x::long_vec &out8,
                                                       int16_t       *pOutLocal,
                                                       c7x::uchar_vec vMask)
{
   c7x::long_vec out9, out10, out11, out12, out13, out14, out15, out16;

   out9  = c7x::as_long_vec(__permute_low_low(vMask, c7x::as_uchar_vec(out3), c7x::as_uchar_vec(out1)));
   out10 = c7x::as_long_vec(__permute_low_low(vMask, c7x::as_uchar_vec(out4), c7x::as_uchar_vec(out2)));
   out11 = c7x::as_long_vec(__permute_low_low(vMask, c7x::as_uchar_vec(out7), c7x::as_uchar_vec(out5)));
   out12 = c7x::as_long_vec(__permute_low_low(vMask, c7x::as_uchar_vec(out8), c7x::as_uchar_vec(out6)));
   out13 = c7x::as_long_vec(__permute_high_high(vMask, c7x::as_uchar_vec(out3), c7x::as_uchar_vec(out1)));
   out14 = c7x::as_long_vec(__permute_high_high(vMask, c7x::as_uchar_vec(out4), c7x::as_uchar_vec(out2)));
   out15 = c7x::as_long_vec(__permute_high_high(vMask, c7x::as_uchar_vec(out7), c7x::as_uchar_vec(out5)));
   out16 = c7x::as_long_vec(__permute_high_high(vMask, c7x::as_uchar_vec(out8), c7x::as_uchar_vec(out6)));

   out1 = c7x::long_vec(out9.lo(), out11.lo());
   out2 = c7x::long_vec(out10.lo(), out12.lo());
   out3 = c7x::long_vec(out13.lo(), out15.lo());
   out4 = c7x::long_vec(out14.lo(), out16.lo());
   out5 = c7x::long_vec(out9.hi(), out11.hi());
   out6 = c7x::long_vec(out10.hi(), out12.hi());
   out7 = c7x::long_vec(out13.hi(), out15.hi());
   out8 = c7x::long_vec(out14.hi(), out16.hi());
}

template <>
inline void vecPermutePack<c7x::int_vec &, int8_t *>(c7x::int_vec  &out1,
                                                     c7x::int_vec  &out2,
                                                     c7x::int_vec  &out3,
                                                     c7x::int_vec  &out4,
                                                     c7x::int_vec  &out5,
                                                     c7x::int_vec  &out6,
                                                     c7x::int_vec  &out7,
                                                     c7x::int_vec  &out8,
                                                     int8_t        *pOutLocal,
                                                     c7x::uchar_vec vMask)
{
   c7x::int_vec out9, out11, out13, out15;

   out9  = c7x::as_int_vec(__permute_low_low(vMask, c7x::as_uchar_vec(out3), c7x::as_uchar_vec(out1)));
   out11 = c7x::as_int_vec(__permute_low_low(vMask, c7x::as_uchar_vec(out7), c7x::as_uchar_vec(out5)));
   out13 = c7x::as_int_vec(__permute_high_high(vMask, c7x::as_uchar_vec(out3), c7x::as_uchar_vec(out1)));
   out15 = c7x::as_int_vec(__permute_high_high(vMask, c7x::as_uchar_vec(out7), c7x::as_uchar_vec(out5)));

   out1 = c7x::int_vec(out9.lo(), out11.lo());
   out3 = c7x::int_vec(out13.lo(), out15.lo());
   out5 = c7x::int_vec(out9.hi(), out11.hi());
   out7 = c7x::int_vec(out13.hi(), out15.hi());
}

template <typename dataTypeIn> static inline void writeOutSA0(dataTypeIn *pOut, c7x::short_vec v1, c7x::short_vec v2);

template <> inline void writeOutSA0(int16_t *pOut, c7x::short_vec v1, c7x::short_vec v2)
{
   __vpred         tmp      = c7x::strm_agen<0, c7x::short_vec>::get_vpred();
   c7x::short_vec *storevec = c7x::strm_agen<0, c7x::short_vec>::get_adv(pOut);
   __vstore_pred_interleave(tmp, storevec, v1, v2);
}

template <> inline void writeOutSA0(int8_t *pOut, c7x::short_vec v1, c7x::short_vec v2)
{
   __vpred         tmp      = c7x::strm_agen<0, c7x::short_vec>::get_vpred();
   c7x::char_hvec *storevec = c7x::strm_agen<0, c7x::char_hvec>::get_adv(pOut);
   __vstore_pred_packl(tmp, storevec, v1);
}

template <typename dataTypeIn, typename dataTypeOut>
DSPLIB_STATUS DSPLIB_matMul_fixed_exec_ci(DSPLIB_kernelHandle handle,
                                               void *restrict pIn0,
                                               void *restrict pIn1,
                                               void *restrict pOut)
{
#if ENABLE_PROFILE
   uint64_t start        = __TSC;
   uint64_t overhead     = __TSC - start;
   uint64_t loopCycle    = 0;
   uint64_t accloopCycle = 0;
   uint64_t count        = 0;
#endif

   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_matMul_fixed_PrivArgs *pKerPrivArgs = (DSPLIB_matMul_fixed_PrivArgs *) handle;
   uint8_t                      *pBlock       = pKerPrivArgs->bufPblock;
   int32_t                       MBlocks      = pKerPrivArgs->MBlocks;
   int32_t                       NBlocks      = pKerPrivArgs->NBlocks;
   int32_t                       KBlocks      = pKerPrivArgs->KBlocks;
   int32_t                       qs           = pKerPrivArgs->qs;

   dataTypeIn *pIn0Local = (dataTypeIn *) pIn0;
   dataTypeIn *pIn1Local = (dataTypeIn *) pIn1;
   dataTypeIn *pOutLocal = (dataTypeIn *) pOut;

   __SE_TEMPLATE_v1 se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET);
   __SE_TEMPLATE_v1 se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET);
   __SA_TEMPLATE_v1 sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET);

   __SE0_OPEN(pIn0Local, se0Params);
   __SE1_OPEN(pIn1Local, se1Params);
   __SA0_OPEN(sa0Params);

   c7x::uchar_vec vMask = setMask<dataTypeIn>();

   dataTypeOut shiftVec = (dataTypeOut) qs;
   dataTypeOut zeroVec  = (dataTypeOut) 0;
   dataTypeOut outSum0 = zeroVec, outSum1 = zeroVec, outSum2 = zeroVec, outSum3 = zeroVec;
   dataTypeOut outSum4 = zeroVec, outSum5 = zeroVec, outSum6 = zeroVec, outSum7 = zeroVec;
   dataTypeOut outSum8 = zeroVec, outSum9 = zeroVec, outSum10 = zeroVec, outSum11 = zeroVec;
   dataTypeOut outSum12 = zeroVec, outSum13 = zeroVec, outSum14 = zeroVec, outSum15 = zeroVec;
   dataTypeOut out1, out2, out3, out4, out5, out6, out7, out8;

   for (int32_t l = 0; l < MBlocks * NBlocks; l++) {
#if ENABLE_PROFILE
      start = __TSC;
#endif
      for (int32_t p = 0; p < KBlocks; p++) {
         vecMulAcc<dataTypeOut &, __SE_REG_0, __SE_REG_1_ADV>(outSum0, outSum1);
         vecMulAcc<dataTypeOut &, __SE_REG_0, __SE_REG_1_ADV>(outSum2, outSum3);
         vecMulAcc<dataTypeOut &, __SE_REG_0, __SE_REG_1_ADV>(outSum4, outSum5);
         vecMulAcc<dataTypeOut &, __SE_REG_0, __SE_REG_1_ADV>(outSum6, outSum7);
         vecMulAcc<dataTypeOut &, __SE_REG_0, __SE_REG_1_ADV>(outSum8, outSum9);
         vecMulAcc<dataTypeOut &, __SE_REG_0, __SE_REG_1_ADV>(outSum10, outSum11);
         vecMulAcc<dataTypeOut &, __SE_REG_0, __SE_REG_1_ADV>(outSum12, outSum13);
         vecMulAcc<dataTypeOut &, __SE_REG_0_ADV, __SE_REG_1_ADV>(outSum14, outSum15);
      }
#if ENABLE_PROFILE
      loopCycle = __TSC - start;
      accloopCycle += loopCycle;
      printf("|INNER LOOP CNT : %d| %lu |\n", count++, loopCycle);
#endif

      vecShiftRight<dataTypeOut &>(outSum0, outSum1, shiftVec);
      vecShiftRight<dataTypeOut &>(outSum2, outSum3, shiftVec);
      vecShiftRight<dataTypeOut &>(outSum4, outSum5, shiftVec);
      vecShiftRight<dataTypeOut &>(outSum6, outSum7, shiftVec);
      vecShiftRight<dataTypeOut &>(outSum8, outSum9, shiftVec);
      vecShiftRight<dataTypeOut &>(outSum10, outSum11, shiftVec);
      vecShiftRight<dataTypeOut &>(outSum12, outSum13, shiftVec);
      vecShiftRight<dataTypeOut &>(outSum14, outSum15, shiftVec);

      packAlternate<dataTypeOut &>(outSum0, outSum1, outSum2, outSum3, out1, out2);
      packAlternate<dataTypeOut &>(outSum4, outSum5, outSum6, outSum7, out3, out4);
      packAlternate<dataTypeOut &>(outSum8, outSum9, outSum10, outSum11, out5, out6);
      packAlternate<dataTypeOut &>(outSum12, outSum13, outSum14, outSum15, out7, out8);

      vecPermutePack<dataTypeOut &, dataTypeIn *>(out1, out2, out3, out4, out5, out6, out7, out8, pOutLocal, vMask);

      writeOutSA0(pOutLocal, c7x::as_short_vec(out1), c7x::as_short_vec(out2));
      writeOutSA0(pOutLocal, c7x::as_short_vec(out5), c7x::as_short_vec(out6));
      writeOutSA0(pOutLocal, c7x::as_short_vec(out3), c7x::as_short_vec(out4));
      writeOutSA0(pOutLocal, c7x::as_short_vec(out7), c7x::as_short_vec(out8));

      resetVec<dataTypeOut &>(outSum0, outSum1);
      resetVec<dataTypeOut &>(outSum2, outSum3);
      resetVec<dataTypeOut &>(outSum4, outSum5);
      resetVec<dataTypeOut &>(outSum6, outSum7);
      resetVec<dataTypeOut &>(outSum8, outSum9);
      resetVec<dataTypeOut &>(outSum10, outSum11);
      resetVec<dataTypeOut &>(outSum12, outSum13);
      resetVec<dataTypeOut &>(outSum14, outSum15);
   }

#if ENABLE_PROFILE
   printf("|CORE LOOP IN16_T | %lu |\n", accloopCycle);
#endif

   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA0_CLOSE();
   return (status);
}

template DSPLIB_STATUS DSPLIB_matMul_fixed_exec_ci<DSPLIB_MATMAPY_FXD_I16S_O16S>(DSPLIB_kernelHandle handle,
                                                                                      void *restrict pIn0,
                                                                                      void *restrict pIn1,
                                                                                      void *restrict pOut);

template DSPLIB_STATUS DSPLIB_matMul_fixed_exec_ci<DSPLIB_MATMAPY_FXD_I8S_O8S>(DSPLIB_kernelHandle handle,
                                                                                    void *restrict pIn0,
                                                                                    void *restrict pIn1,
                                                                                    void *restrict pOut);
