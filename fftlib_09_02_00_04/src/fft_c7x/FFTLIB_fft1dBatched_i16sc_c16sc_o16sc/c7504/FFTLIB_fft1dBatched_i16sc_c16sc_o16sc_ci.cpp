/*******************************************************************************
**+--------------------------------------------------------------------------+**
**|                            ****                                          |**
**|                            ****                                          |**
**|                            ******o***                                    |**
**|                      ********_///_****                                   |**
**|                      ***** /_//_/ ****                                   |**
**|                       ** ** (__/ ****                                    |**
**|                           *********                                      |**
**|                            ****                                          |**
**|                            ***                                           |**
**|                                                                          |**
**|         Copyright (c) 2017 Texas Instruments Incorporated                |**
**|                        ALL RIGHTS RESERVED                               |**
**|                                                                          |**
**| Permission to use, copy, modify, or distribute this software,            |**
**| whether in part or in whole, for any purpose is forbidden without        |**
**| a signed licensing agreement and NDA from Texas Instruments              |**
**| Incorporated (TI).                                                       |**
**|                                                                          |**
**| TI makes no representation or warranties with respect to the             |**
**| performance of this computer program, and specifically disclaims         |**
**| any responsibility for any damages, special or consequential,            |**
**| connected with the use of this program.                                  |**
**|                                                                          |**
**+--------------------------------------------------------------------------+**
*******************************************************************************/

#include "../FFTLIB_fft1dBatched_i16sc_c16sc_o16sc.h"

#define TRACE_ON (0)

#if TRACE_ON
#include "../../../common/printv.h"
#include <stdio.h>
#endif

// CODE_SECTION(FFTLIB_fft1d,              ".text:optimized")
// CODE_SECTION(FFTLIB_fft1d_core,         ".text:optimized")
// CODE_SECTION(FFTLIB_fft1d_checkParams,  ".text:optimized")

#define SE_PARAM_BASE         (0x0000)
#define SE_LOOP1_PARAM_OFFSET (SE_PARAM_BASE)
#define SE_LOOP2_PARAM_OFFSET (SE_LOOP1_PARAM_OFFSET + SE_PARAM_SIZE)
#define SE_LOOP3_PARAM_OFFSET (SE_LOOP2_PARAM_OFFSET + SE_PARAM_SIZE)
#define SE_LOOP4_PARAM_OFFSET (SE_LOOP3_PARAM_OFFSET + SE_PARAM_SIZE)
#define SE_LOOP5_PARAM_OFFSET (SE_LOOP4_PARAM_OFFSET + SE_PARAM_SIZE)
#define SE_LOOP6_PARAM_OFFSET (SE_LOOP5_PARAM_OFFSET + SE_PARAM_SIZE)
#define SE_TWID_PARAM_OFFSET  (SE_LOOP6_PARAM_OFFSET + SE_PARAM_SIZE)
#define SA_LOOP1_PARAM_OFFSET (SE_TWID_PARAM_OFFSET + SE_PARAM_SIZE)
#define SA_LOOP2_PARAM_OFFSET (SA_LOOP1_PARAM_OFFSET + SA_PARAM_SIZE)
#define SA_LOOP3_PARAM_OFFSET (SA_LOOP2_PARAM_OFFSET + SA_PARAM_SIZE)
#define SA_LOOP6_PARAM_OFFSET (SA_LOOP3_PARAM_OFFSET + SA_PARAM_SIZE)

FFTLIB_STATUS FFTLIB_fft1dBatched_i16sc_c16sc_o16sc_init (
    int16_t              *pX,
    FFTLIB_bufParams1D_t *bufParamsX,
    int16_t              *pW,
    FFTLIB_bufParams1D_t *bufParamsW,
    int16_t              *pY,
    FFTLIB_bufParams1D_t *bufParamsY,
    uint32_t             *pShift,
    FFTLIB_bufParams1D_t *bufParamsShift,
    uint32_t              numPoints,
    uint32_t              numChannels,
    void                 *pBlock)
{
   FFTLIB_STATUS status = FFTLIB_SUCCESS;

#if defined(FFTLIB_CHECK_PARAMS) ||                                            \
    defined(FFTLIB_FFT1DBATCHED_I16SC_C16SC_O16SC_CHECK_PARAMS)
   status = FFTLIB_fft1dBatched_i16sc_c16sc_o16sc_checkParams (
       pX, bufParamsX, pW, bufParamsW, pY, bufParamsY, pShift, bufParamsShift,
       numPoints, numChannels, pBlock);
   if (status == FFTLIB_SUCCESS)
#endif
   {
      uint32_t numPointsPerDft;
      uint32_t seCnt1, seCnt2, seCnt3, seCnt4, seCnt6, seCnt7, seCnt8, seCnt9,
          seCnt10, seCnt11;
      __SE_TEMPLATE_v1 se0_param = __gen_SE_TEMPLATE_v1 ();
      __SE_TEMPLATE_v1 se1_param = __gen_SE_TEMPLATE_v1 ();
      __SA_TEMPLATE_v1 sa0_param = __gen_SA_TEMPLATE_v1 ();

      /* numPoints       = bufParamsX->dim_x >> 1; */
      numPointsPerDft = numPoints;
      seCnt1          = numPoints >> 2;
      seCnt2          = numPoints >> 5;
      seCnt3          = 1;
      seCnt4          = numPoints >> 3;
      seCnt6          = seCnt3 * numChannels;
      seCnt7 =
          (numPoints * numChannels >> 5) > 1 ? numPoints * numChannels >> 5 : 1;
      seCnt8 = numPoints * numChannels;
      seCnt9 = (numPoints * numChannels > 32) ? numPoints * numChannels : 32;
      seCnt10 =
          (numPoints * numChannels >> 6) > 1 ? numPoints * numChannels >> 6 : 1;
      seCnt11 = (numPoints * numChannels > 64) ? numPoints * numChannels : 64;

      /* se0_param = (0); */
      se0_param.ICNT0 = 8; /* 16-point vectors processed in one shot */
      se0_param.ICNT1 = 4;
      se0_param.DIM1  = seCnt1; /* 4 quarters(Offsets: 0, N/4, N/2, 3N/4) */
      se0_param.ICNT2 = seCnt2; /* Number of 8-point fetches within each  */
      se0_param.DIM2  = 8;      /* quarter                                */
      se0_param.ICNT3 = seCnt6; /* Number of DFT's  */
      se0_param.DIM3  = numPointsPerDft;

      se0_param.ELETYPE = __SE_ELETYPE_16BIT_CMPLX_SWAP;
      se0_param.VECLEN  = c7x::se_veclen<c7x::cshort_vec>::value;
      se0_param.DIMFMT  = __SE_DIMFMT_4D;
      /* *((uint16 *)((uint8_t *)pBlock + SE_LOOP1_PARAM_OFFSET)) = se0_param;
       */
      *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_LOOP1_PARAM_OFFSET) =
          se0_param;

      /* se1_param = (0); */
      se1_param.ICNT0 = 8; /* 16-point vectors processed in one shot */
      se1_param.ICNT1 = 3;
      se1_param.DIM1  = seCnt1; /* Twiddle factors for x1, x2 and x3      */
      se1_param.ICNT2 = seCnt2; /* Number of 8-point fetches within each  */
      se1_param.DIM2  = 8;      /* quarter                                */
      se1_param.ICNT3 = seCnt6; /* Number of DFT's                        */
      se1_param.DIM3  = 0;

      se1_param.ELETYPE = __SE_ELETYPE_16BIT_CMPLX_SWAP;
      se1_param.VECLEN  = c7x::se_veclen<c7x::cshort_vec>::value;
      se1_param.DIMFMT  = __SE_DIMFMT_4D;
      /* *((uint16 *)((uint8_t *)pBlock + SE_TWID_PARAM_OFFSET)) = se1_param; */
      *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_TWID_PARAM_OFFSET) =
          se1_param;

      /* sa0_param = (0); */
      sa0_param.ICNT0  = 8;
      sa0_param.ICNT1  = 4;
      sa0_param.DIM1   = seCnt1; /* Save to each of the 4 quarters         */
      sa0_param.ICNT2  = seCnt2; /* Number of 8-point stores within each   */
      sa0_param.DIM2   = 8;      /* quarter                                */
      sa0_param.ICNT3  = seCnt6;
      sa0_param.DIM3   = numPointsPerDft; /* Number of DFT's                 */
      sa0_param.VECLEN = c7x::sa_veclen<c7x::cshort_vec>::value;
      sa0_param.DIMFMT = __SA_DIMFMT_4D;
      /* *((uint16 *)((uint8_t *)pBlock + SA_LOOP1_PARAM_OFFSET)) = sa0_param;
       */
      *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA_LOOP1_PARAM_OFFSET) =
          sa0_param;

      se0_param       = __gen_SE_TEMPLATE_v1 ();
      se0_param.ICNT0 = 8;
      se0_param.ICNT1 = 2;
      se0_param.DIM1  = 16; /* Process four 16-point DFTs in one shot */
      se0_param.ICNT2 = seCnt7;
      se0_param.DIM2  = 32; /* One-fourth the the number of DFT's     */

      se0_param.ELETYPE   = __SE_ELETYPE_16BIT_CMPLX_SWAP;
      se0_param.TRANSPOSE = __SE_TRANSPOSE_128BIT;
      se0_param.VECLEN    = c7x::se_veclen<c7x::cshort_vec>::value;
      se0_param.DIMFMT    = __SE_DIMFMT_3D;
      /* *((uint16 *)((uint8_t *)pBlock + SE_LOOP2_PARAM_OFFSET)) = se0_param;
       */
      *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_LOOP2_PARAM_OFFSET) =
          se0_param;

      sa0_param       = __gen_SA_TEMPLATE_v1 ();
      sa0_param.ICNT0 = seCnt8;

      sa0_param.VECLEN = c7x::sa_veclen<c7x::cshort_vec>::value;
      sa0_param.DIMFMT = __SA_DIMFMT_1D;
      /* *((uint16 *)((uint8_t *)pBlock + SA_LOOP2_PARAM_OFFSET)) = sa0_param;
       */
      *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA_LOOP2_PARAM_OFFSET) =
          sa0_param;

      se0_param       = __gen_SE_TEMPLATE_v1 ();
      se0_param.ICNT0 = seCnt4;
      se0_param.ICNT1 = 8;
      se0_param.DIM1  = seCnt4;
      se0_param.ICNT2 = numChannels;
      se0_param.DIM2  = numPoints;

      se0_param.ELETYPE   = __SE_ELETYPE_16BIT_CMPLX_SWAP;
      se0_param.TRANSPOSE = __SE_TRANSPOSE_32BIT;
      se0_param.VECLEN    = c7x::se_veclen<c7x::cshort_vec>::value;
      se0_param.DIMFMT    = __SE_DIMFMT_3D;
      /* *((uint16 *)((uint8_t *)pBlock + SE_LOOP4_PARAM_OFFSET)) = se0_param;
       */
      *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_LOOP4_PARAM_OFFSET) =
          se0_param;

      /* se0_param = (0); */
      se0_param       = __gen_SE_TEMPLATE_v1 ();
      se0_param.ICNT0 = seCnt4;
      se0_param.ICNT1 = 8;
      se0_param.DIM1  = seCnt4;
      se0_param.ICNT2 = numChannels;
      se0_param.DIM2  = numPoints;

      se0_param.ELETYPE   = __SE_ELETYPE_16BIT_CMPLX_SWAP;
      se0_param.TRANSPOSE = __SE_TRANSPOSE_32BIT;
      se0_param.VECLEN    = c7x::se_veclen<c7x::cshort_vec>::value;
      se0_param.DIMFMT    = __SE_DIMFMT_3D;
      /* *((uint16 *)((uint8_t *)pBlock + SE_LOOP5_PARAM_OFFSET)) = se0_param;
       */
      *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_LOOP5_PARAM_OFFSET) =
          se0_param;

      se0_param       = __gen_SE_TEMPLATE_v1 ();
      se0_param.ICNT0 = 4;
      se0_param.ICNT1 = 8;
      se0_param.DIM1  = 4;
      se0_param.ICNT2 = seCnt7;
      se0_param.DIM2  = 32;

      se0_param.ELETYPE   = __SE_ELETYPE_16BIT_CMPLX_SWAP;
      se0_param.TRANSPOSE = __SE_TRANSPOSE_32BIT;
      se0_param.VECLEN    = c7x::se_veclen<c7x::cshort_vec>::value;
      se0_param.DIMFMT    = __SE_DIMFMT_3D;
      *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_LOOP6_PARAM_OFFSET)) =
          se0_param;

      sa0_param       = __gen_SA_TEMPLATE_v1 ();
      sa0_param.ICNT0 = seCnt9; /* Input buffer must be at least 32
                                 * elements long even though
                                 * numPoints*numChannels = 16             */

      sa0_param.VECLEN = c7x::sa_veclen<c7x::cshort_vec>::value;
      sa0_param.DIMFMT = __SA_DIMFMT_1D;
      *((__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA_LOOP6_PARAM_OFFSET)) =
          sa0_param;
   }
   return (status);
}

FFTLIB_STATUS FFTLIB_fft1dBatched_i16sc_c16sc_o16sc_kernel (
    int16_t              *pX,
    FFTLIB_bufParams1D_t *bufParamsX,
    int16_t              *pW,
    FFTLIB_bufParams1D_t *bufParamsW,
    int16_t              *pY,
    FFTLIB_bufParams1D_t *bufParamsY,
    uint32_t             *pShift,
    FFTLIB_bufParams1D_t *bufParamsShift,
    uint32_t              numPoints,
    uint32_t              numChannels,
    void                 *pBlock)
{
   uint32_t         k, l;
   FFTLIB_STATUS    status = FFTLIB_SUCCESS;
   uint32_t         numPointsPerDft;
   uint32_t         numLeadingZeros;
   uint32_t         offsetBitReverse;
   uint32_t         seCnt1, seCnt2, seCnt3, seCnt6;
   __SE_TEMPLATE_v1 se0_param = __gen_SE_TEMPLATE_v1 ();
   __SE_TEMPLATE_v1 se1_param = __gen_SE_TEMPLATE_v1 ();
   __SA_TEMPLATE_v1 sa0_param = __gen_SA_TEMPLATE_v1 ();
   cshort *restrict pXLocal;
   cshort *restrict pYLocal;
   cshort *restrict pWLocal;
   cshort *restrict pY0;
   cshort *restrict pY1;
   cshort *restrict pY2;
   cshort *restrict pY3;
   cshort *restrict pY4;
   cshort *restrict pY5;
   cshort *restrict pY6;
   cshort *restrict pY7;

   typedef typename c7x::cshort_vec CV;
   typedef CV                      *CVP;

   typedef typename c7x::short_vec V;
   typedef V                      *VP;

   CV            vX_0, vX_N_4, vX_N_2, vX_3N_4;
   CV            vSum1, vSum2, vDiff1, vDiff2;
   CV            vTwX1, vTwX2, vTwX3;
   CV            vX0Temp, vX1Temp, vX2Temp, vX3Temp;
   CV            vX0, vX1, vX2, vX3;
   CV            vX_0_1, vX_N_4_1, vX_N_2_1, vX_3N_4_1;
   CV            vSum1_1, vSum2_1, vDiff1_1, vDiff2_1;
   CV            vX0_1, vX1_1, vX2_1, vX3_1;
   CV            vX0_2PtDft_1, vX0_2PtDft_2;
   CV            vX1_2PtDft_1, vX1_2PtDft_2;
   CV            vX2_2PtDft_1, vX2_2PtDft_2;
   CV            vX3_2PtDft_1, vX3_2PtDft_2;
   CV            vX01_lo, vX23_lo, vX01_hi, vX23_hi;
   cshort        twTemp;
   uint32_t      stageIndex;
   c7x::uint_vec vShift;

#if defined(FFTLIB_CHECK_PARAMS) ||                                            \
    defined(FFTLIB_FFT1DBATCHED_I16SC_C16SC_O16SC_CHECK_PARAMS)
   status = FFTLIB_fft1dBatched_i16sc_c16sc_o16sc_checkParams (
       pX, bufParamsX, pW, bufParamsW, pY, bufParamsY, pShift, bufParamsShift,
       numPoints, numChannels, pBlock);
   if (status == FFTLIB_SUCCESS)
#endif
   {
      /* numPoints       = bufParamsX->dim_x >> 1; */
      numPointsPerDft = numPoints;
      stageIndex      = 0;

      se0_param =
          *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_LOOP1_PARAM_OFFSET));
      se1_param =
          *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_TWID_PARAM_OFFSET));
      sa0_param =
          *((__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA_LOOP1_PARAM_OFFSET));

      seCnt1 = numPointsPerDft >> 2;
      seCnt2 = numPointsPerDft >> 5;
      seCnt3 = 1;

      pXLocal = (cshort *) pX;
      pWLocal = (cshort *) pW;
      pYLocal = (cshort *) pY;

      while (numPointsPerDft >= 32) {
         seCnt6          = seCnt3 * numChannels;
         se0_param.ICNT1 = 4;
         se0_param.DIM1  = seCnt1; /* 4 quarters(Offsets: 0, N/4, N/2, 3N/4) */
         se0_param.ICNT2 = seCnt2;
         se0_param.DIM2  = 8;
         /* Number of 8-point fetches within       */ /* each quarter */
         se0_param.ICNT3 = seCnt6;
         se0_param.DIM3  = numPointsPerDft; /* Number of DFT's   */
         __SE0_OPEN ((void *) pXLocal, se0_param);

         se1_param.ICNT1 = 3; /* Twiddle factors for x1, x2, and x3      */
         se1_param.DIM1  = seCnt1;
         se1_param.ICNT2 = seCnt2; /* Number of 8-point fetches within each  */
         se1_param.DIM2  = 8;      /* quarter                                */
         se1_param.ICNT3 = seCnt6; /* Number of DFT's                        */
         se1_param.DIM3  = 0;
         __SE1_OPEN ((void *) pWLocal, se1_param);

         sa0_param.ICNT1 = 4;
         sa0_param.DIM1  = seCnt1; /* Save to each of the 4 quarters         */
         sa0_param.ICNT2 = seCnt2;
         sa0_param.DIM2  = 8;
         /* Number of 8-point stores within        */ /* each quarter */
         sa0_param.ICNT3 = seCnt6;
         sa0_param.DIM3  = numPointsPerDft;
         /* Number of DFT's                        */
         __SA0_OPEN (sa0_param);

         /* Obtain the right shift value for the current stage */
         vShift = __vload_dup ((uint32_t *) (pShift + stageIndex));

         for (k = 0; k < numChannels * numPoints; k += 32) {

            vX_0    = c7x::strm_eng<0, CV>::get_adv ();
            vX_N_4  = c7x::strm_eng<0, CV>::get_adv ();
            vX_N_2  = c7x::strm_eng<0, CV>::get_adv ();
            vX_3N_4 = c7x::strm_eng<0, CV>::get_adv ();

            vSum1  = vX_0 + vX_N_2;
            vSum2  = vX_N_4 + vX_3N_4;
            vDiff1 = vX_0 - vX_N_2;
            vDiff2 = vX_N_4 - vX_3N_4;

            vTwX1 = c7x::strm_eng<1, CV>::get_adv ();
            vTwX2 = c7x::strm_eng<1, CV>::get_adv ();
            vTwX3 = c7x::strm_eng<1, CV>::get_adv ();

            vX0Temp = vSum1 + vSum2;
            vX1Temp = vDiff1 - __vcrot90h_vv (vDiff2);
            vX2Temp = vSum1 - vSum2;
            vX3Temp = vDiff1 + __vcrot90h_vv (vDiff2);

            vX0 = vX0Temp;
            vX1 = __cmpy_fx (vX1Temp, vTwX1);
            vX2 = __cmpy_fx (vX2Temp, vTwX2);
            vX3 = __cmpy_fx (vX3Temp, vTwX3);

            /* __SA0ADV(CV, pXLocal) = */
            /*     c7x::as_cshort_vec(__shift_right_round(__as_short32(vX0),
             * vShift)); */
            /* __SA0ADV(CV, pXLocal) = */
            /*     c7x::as_cshort_vec(__shift_right_round(__as_short32(vX2),
             * vShift)); */
            /* __SA0ADV(CV, pXLocal) = */
            /*     c7x::as_cshort_vec(__shift_right_round(__as_short32(vX1),
             * vShift)); */
            /* __SA0ADV(CV, pXLocal) = */
            /*     c7x::as_cshort_vec(__shift_right_round(__as_short32(vX3),
             * vShift)); */
            __vpred tmp;
            CVP     addr;

            tmp  = c7x::strm_agen<0, CV>::get_vpred ();
            addr = c7x::strm_agen<0, CV>::get_adv (pXLocal);
            __vstore_pred (tmp, addr,
                           c7x::as_cshort_vec (__shift_right_round (
                               c7x::as_short_vec (vX0), vShift)));
            tmp  = c7x::strm_agen<0, CV>::get_vpred ();
            addr = c7x::strm_agen<0, CV>::get_adv (pXLocal);
            __vstore_pred (tmp, addr,
                           c7x::as_cshort_vec (__shift_right_round (
                               c7x::as_short_vec (vX2), vShift)));
            tmp  = c7x::strm_agen<0, CV>::get_vpred ();
            addr = c7x::strm_agen<0, CV>::get_adv (pXLocal);
            __vstore_pred (tmp, addr,
                           c7x::as_cshort_vec (__shift_right_round (
                               c7x::as_short_vec (vX1), vShift)));
            tmp  = c7x::strm_agen<0, CV>::get_vpred ();
            addr = c7x::strm_agen<0, CV>::get_adv (pXLocal);
            __vstore_pred (tmp, addr,
                           c7x::as_cshort_vec (__shift_right_round (
                               c7x::as_short_vec (vX3), vShift)));
         }
         __SA0_CLOSE ();
         __SE0_CLOSE ();
         __SE1_CLOSE ();

         numPointsPerDft >>= 2;
         pWLocal += numPointsPerDft * 3;
         seCnt1 >>= 2;
         seCnt2 >>= 2;
         seCnt3 <<= 2;
         stageIndex++;
      }

      vShift = __vload_dup ((uint32_t *) (pShift + stageIndex));
      if (numPointsPerDft == 16) {

         c7x::uchar_vec vX0X2_01, vX1X3_01, vX0X2_23, vX1X3_23;
         c7x::uchar_vec vXPermOut;

         typedef typename c7x::cshort_qvec CQV;
         typedef typename c7x::cshort_hvec CHV;
         typedef CQV                      *CQVP;
         /* typedef CHV* CHVP; */

         CQV vTwX1CShortQvec, vTwX2CShortQvec, vTwX3CShortQvec;
         CHV vTwX1CShortHvec, vTwX2CShortHvec, vTwX3CShortHvec;

         se0_param = *(
             (__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_LOOP2_PARAM_OFFSET));
         __SE0_OPEN ((void *) pXLocal, se0_param);
         __SE1_OPEN ((void *) (pXLocal + 8), se0_param);

         sa0_param = *(
             (__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA_LOOP2_PARAM_OFFSET));
         __SA0_OPEN (sa0_param);

#if __C7X_HOSTEM__
         vTwX1 = *((CVP) pWLocal);
         vTwX1 = CV (vTwX1.lo (), vTwX1.lo ());
         vTwX2 = *((CVP) (pWLocal + 4));
         vTwX2 = CV (vTwX2.lo (), vTwX2.lo ());
         vTwX3 = *((CVP) (pWLocal + 8));
         vTwX3 = CV (vTwX3.lo (), vTwX3.lo ());
#else
         vTwX1 = *((CVP) pWLocal);
         vTwX1 = (CV) (vTwX1.lo (), vTwX1.lo ());
         vTwX2 = *((CVP) (pWLocal + 4));
         vTwX2 = (CV) (vTwX2.lo (), vTwX2.lo ());
         vTwX3 = *((CVP) (pWLocal + 8));
         vTwX3 = (CV) (vTwX3.lo (), vTwX3.lo ());

#endif

         for (k = 0; k < numChannels * numPoints; k += 32) {
            vX_0    = c7x::strm_eng<0, CV>::get_adv ();
            vX_N_4  = c7x::strm_eng<0, CV>::get_adv ();
            vX_N_2  = c7x::strm_eng<1, CV>::get_adv ();
            vX_3N_4 = c7x::strm_eng<1, CV>::get_adv ();

            vSum1  = vX_0 + vX_N_2;
            vSum2  = vX_N_4 + vX_3N_4;
            vDiff1 = vX_0 - vX_N_2;
            vDiff2 = vX_N_4 - vX_3N_4;

            vX0Temp = vSum1 + vSum2;
            vX1Temp = vDiff1 - __vcrot90h_vv (vDiff2);
            vX2Temp = vSum1 - vSum2;
            vX3Temp = vDiff1 + __vcrot90h_vv (vDiff2);

            vX0 = vX0Temp;
            vX1 = __cmpy_fx (vX1Temp, vTwX1);
            vX2 = __cmpy_fx (vX2Temp, vTwX2);
            vX3 = __cmpy_fx (vX3Temp, vTwX3);
#if __C7X_HOSTEM__
            __vpred tmp = c7x::strm_agen<0, CV>::get_vpred ();
            CVP     addr;
            addr = c7x::strm_agen<0, CV>::get_adv (pXLocal);
            __vstore_pred (
                tmp, addr,
                c7x::as_cshort_vec (__shift_right_round (
                    c7x::as_short_vec (CV (vX0.lo (), vX2.lo ())), vShift)));
            tmp  = c7x::strm_agen<0, CV>::get_vpred ();
            addr = c7x::strm_agen<0, CV>::get_adv (pXLocal);
            __vstore_pred (
                tmp, addr,
                c7x::as_cshort_vec (__shift_right_round (
                    c7x::as_short_vec (CV (vX1.lo (), vX3.lo ())), vShift)));
            tmp  = c7x::strm_agen<0, CV>::get_vpred ();
            addr = c7x::strm_agen<0, CV>::get_adv (pXLocal);
            __vstore_pred (
                tmp, addr,
                c7x::as_cshort_vec (__shift_right_round (
                    c7x::as_short_vec (CV (vX0.hi (), vX2.hi ())), vShift)));
            tmp  = c7x::strm_agen<0, CV>::get_vpred ();
            addr = c7x::strm_agen<0, CV>::get_adv (pXLocal);
            __vstore_pred (
                tmp, addr,
                c7x::as_cshort_vec (__shift_right_round (
                    c7x::as_short_vec (CV (vX1.hi (), vX3.hi ())), vShift)));
#else
            __vpred tmp = c7x::strm_agen<0, CV>::get_vpred ();
            CVP     addr;
            addr = c7x::strm_agen<0, CV>::get_adv (pXLocal);
            __vstore_pred (
                tmp, addr,
                c7x::as_cshort_vec (__shift_right_round (
                    c7x::as_short_vec (__vllmv_vvv (c7x::as_uchar_vec (vX2),
                                                    c7x::as_uchar_vec (vX0))),
                    vShift)));
            tmp  = c7x::strm_agen<0, CV>::get_vpred ();
            addr = c7x::strm_agen<0, CV>::get_adv (pXLocal);
            __vstore_pred (
                tmp, addr,
                c7x::as_cshort_vec (__shift_right_round (
                    c7x::as_short_vec (__vllmv_vvv (c7x::as_uchar_vec (vX3),
                                                    c7x::as_uchar_vec (vX1))),
                    vShift)));
            tmp  = c7x::strm_agen<0, CV>::get_vpred ();
            addr = c7x::strm_agen<0, CV>::get_adv (pXLocal);
            __vstore_pred (
                tmp, addr,
                c7x::as_cshort_vec (__shift_right_round (
                    c7x::as_short_vec (__vhhmv_vvv (c7x::as_uchar_vec (vX2),
                                                    c7x::as_uchar_vec (vX0))),
                    vShift)));
            tmp  = c7x::strm_agen<0, CV>::get_vpred ();
            addr = c7x::strm_agen<0, CV>::get_adv (pXLocal);
            __vstore_pred (
                tmp, addr,
                c7x::as_cshort_vec (__shift_right_round (
                    c7x::as_short_vec (__vhhmv_vvv (c7x::as_uchar_vec (vX3),
                                                    c7x::as_uchar_vec (vX1))),
                    vShift)));
#endif
         }
         __SA0_CLOSE ();
         __SE0_CLOSE ();
         __SE1_CLOSE ();

         numPointsPerDft >>= 2;
         pWLocal += numPointsPerDft * 3;
         stageIndex++;
         vShift = __vload_dup ((uint32_t *) (pShift + stageIndex));
         /* uint32_t p = 0; */

         /* for (p = 0; p < numPoints * numChannels * 2; p++) { */
         /*    printf ("%d, ", ((short *) pXLocal)[p]); */
         /* } */
      }
      else { // no 32-point stage for C7504
      }

      if (numPointsPerDft == 4) {

         if (numPoints == 16) {
// clang-format off
#if __C7X_HOSTEM__
                c7x::uchar_vec vXPermCtrl  = c7x::uchar_vec(0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                                0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                                                0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                                                0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F);
#else
                c7x::uchar_vec vXPermCtrl  = (c7x::uchar_vec)(0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                                0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                                                0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                                                0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F);
#endif
            // clang-format on

            se0_param = *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                                                SE_LOOP6_PARAM_OFFSET));
            __SE0_OPEN ((void *) pXLocal, se0_param);

            sa0_param = *((__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                                                SA_LOOP6_PARAM_OFFSET));
            __SA0_OPEN (sa0_param);

            for (k = 0; k < numChannels << 4; k += 32) {
               vX_0    = c7x::strm_eng<0, CV>::get_adv ();
               vX_N_4  = c7x::strm_eng<0, CV>::get_adv ();
               vX_N_2  = c7x::strm_eng<0, CV>::get_adv ();
               vX_3N_4 = c7x::strm_eng<0, CV>::get_adv ();

               vSum1  = vX_0 + vX_N_2;
               vSum2  = vX_N_4 + vX_3N_4;
               vDiff1 = vX_0 - vX_N_2;
               vDiff2 = vX_N_4 - vX_3N_4;

               vX0 = vSum1 + vSum2;
               vX1 = vDiff1 - __vcrot90h_vv (vDiff2);
               vX2 = vSum1 - vSum2;
               vX3 = vDiff1 + __vcrot90h_vv (vDiff2);

               vX01_lo = c7x::as_cshort_vec (
                   __vpermll_yvvv (vXPermCtrl, c7x::as_uchar_vec (vX1),
                                   c7x::as_uchar_vec (vX0)));
               vX23_lo = c7x::as_cshort_vec (
                   __vpermll_yvvv (vXPermCtrl, c7x::as_uchar_vec (vX3),
                                   c7x::as_uchar_vec (vX2)));
               vX01_hi = c7x::as_cshort_vec (
                   __vpermhh_yvvv (vXPermCtrl, c7x::as_uchar_vec (vX1),
                                   c7x::as_uchar_vec (vX0)));
               vX23_hi = c7x::as_cshort_vec (
                   __vpermhh_yvvv (vXPermCtrl, c7x::as_uchar_vec (vX3),
                                   c7x::as_uchar_vec (vX2)));

               __vpred tmp = c7x::strm_agen<0, CV>::get_vpred ();
               CVP     addr;
               addr = c7x::strm_agen<0, CV>::get_adv (pYLocal);
               __vstore_pred (tmp, addr, vX01_lo);

               tmp  = c7x::strm_agen<0, CV>::get_vpred ();
               addr = c7x::strm_agen<0, CV>::get_adv (pYLocal);
               __vstore_pred (tmp, addr, vX23_lo);

               tmp  = c7x::strm_agen<0, CV>::get_vpred ();
               addr = c7x::strm_agen<0, CV>::get_adv (pYLocal);
               __vstore_pred (tmp, addr, vX01_hi);

               tmp  = c7x::strm_agen<0, CV>::get_vpred ();
               addr = c7x::strm_agen<0, CV>::get_adv (pYLocal);
               __vstore_pred (tmp, addr, vX23_hi);
            }
            __SE0_CLOSE ();
            __SA0_CLOSE ();
         }

         else {
            /* 4-point stage with bit-reversal */
            se0_param = *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                                                SE_LOOP4_PARAM_OFFSET));
            __SE0_OPEN ((void *) pXLocal, se0_param);

            numLeadingZeros = __norm ((int32_t) (numPoints - 1)) + 1;

            pY0 = (cshort *) (pY + 0);
            pY1 = (cshort *) (pY + ((0x40000000u >> numLeadingZeros) << 1));
            pY2 = (cshort *) (pY + ((0x80000000u >> numLeadingZeros) << 1));
            pY3 = (cshort *) (pY + ((0xC0000000u >> numLeadingZeros) << 1));

#ifdef CL7X_HE_CSHORT_PTR_BUG
            short *myPY0 = (short *) pY0;
            short *myPY1 = (short *) pY1;
            short *myPY2 = (short *) pY2;
            short *myPY3 = (short *) pY3;
#endif

            for (l = 0; l < numChannels; l++) {
               for (k = 0; k < numPoints >> 3; k += 4) {
                  offsetBitReverse = __bit_reverse (k) >> numLeadingZeros;

                  vX_0    = c7x::strm_eng<0, CV>::get_adv ();
                  vX_N_4  = c7x::strm_eng<0, CV>::get_adv ();
                  vX_N_2  = c7x::strm_eng<0, CV>::get_adv ();
                  vX_3N_4 = c7x::strm_eng<0, CV>::get_adv ();

                  vSum1  = vX_0 + vX_N_2;
                  vSum2  = vX_N_4 + vX_3N_4;
                  vDiff1 = vX_0 - vX_N_2;
                  vDiff2 = vX_N_4 - vX_3N_4;

                  vX0 = vSum1 + vSum2;
                  vX1 = vDiff1 - __vcrot90h_vv (vDiff2);
                  vX2 = vSum1 - vSum2;
                  vX3 = vDiff1 + __vcrot90h_vv (vDiff2);

                  __vstore_reverse_bit (
                      (CVP) (pY0 + offsetBitReverse),
                      c7x::as_cshort_vec (__shift_right_round (
                          c7x::as_short_vec (vX0), vShift)));
                  __vstore_reverse_bit (
                      (CVP) (pY1 + offsetBitReverse),
                      c7x::as_cshort_vec (__shift_right_round (
                          c7x::as_short_vec (vX1), vShift)));
                  __vstore_reverse_bit (
                      (CVP) (pY2 + offsetBitReverse),
                      c7x::as_cshort_vec (__shift_right_round (
                          c7x::as_short_vec (vX2), vShift)));
                  __vstore_reverse_bit (
                      (CVP) (pY3 + offsetBitReverse),
                      c7x::as_cshort_vec (__shift_right_round (
                          c7x::as_short_vec (vX3), vShift)));
               }
#ifdef CL7X_HE_CSHORT_PTR_BUG
               myPY0 += (numPoints << 1);
               myPY1 += (numPoints << 1);
               myPY2 += (numPoints << 1);
               myPY3 += (numPoints << 1);

               pY0 = (cshort *) myPY0;
               pY1 = (cshort *) myPY1;
               pY2 = (cshort *) myPY2;
               pY3 = (cshort *) myPY3;

#else
               pY0 += numPoints;
               pY1 += numPoints;
               pY2 += numPoints;
               pY3 += numPoints;
#endif
            }
            __SE0_CLOSE ();
         }
      }

      else {
         /* 4-point stage followed by 2-point stage with bit-reversal */
         se0_param = *(
             (__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_LOOP5_PARAM_OFFSET));
         __SE0_OPEN ((void *) pXLocal, se0_param);

         numLeadingZeros = __norm ((int32_t) (numPoints - 1)) + 1;

#if __C7X_HOSTEM__
         pWLocal += 1;
         twTemp = *pWLocal;
         vTwX1  = CV (twTemp, twTemp, twTemp, twTemp, twTemp, twTemp, twTemp,
                      twTemp);
#else
         pWLocal += 1;
         twTemp = *pWLocal;
         vTwX1  = (CV) (twTemp, twTemp, twTemp, twTemp, twTemp, twTemp, twTemp,
                       twTemp);
#endif

#if __C7X_HOSTEM__
         pWLocal += 2;
         twTemp = *pWLocal;
         vTwX2  = CV (twTemp, twTemp, twTemp, twTemp, twTemp, twTemp, twTemp,
                      twTemp);
#else
         pWLocal += 2;
         twTemp = *pWLocal;
         vTwX2  = (CV) (twTemp, twTemp, twTemp, twTemp, twTemp, twTemp, twTemp,
                       twTemp);
#endif

#if __C7X_HOSTEM__
         pWLocal += 2;
         twTemp = *pWLocal;
         vTwX3  = CV (twTemp, twTemp, twTemp, twTemp, twTemp, twTemp, twTemp,
                      twTemp);
#else
         pWLocal += 2;
         twTemp = *pWLocal;
         vTwX3  = (CV) (twTemp, twTemp, twTemp, twTemp, twTemp, twTemp, twTemp,
                       twTemp);
#endif

         /* pY0 = &pYLocal[0x00000000u]; */
         /* pY1 = &pYLocal[0x80000000u >> numLeadingZeros]; */
         /* pY2 = &pYLocal[0x20000000u >> numLeadingZeros]; */
         /* pY3 = &pYLocal[0xA0000000u >> numLeadingZeros]; */
         /* pY4 = &pYLocal[0x40000000u >> numLeadingZeros]; */
         /* pY5 = &pYLocal[0xC0000000u >> numLeadingZeros]; */
         /* pY6 = &pYLocal[0x60000000u >> numLeadingZeros]; */
         /* pY7 = &pYLocal[0xE0000000u >> numLeadingZeros]; */

         pY0 = (cshort *) (pY + 0x00000000u);
         pY1 = (cshort *) (pY + ((0x80000000u >> numLeadingZeros) << 1));
         pY2 = (cshort *) (pY + ((0x20000000u >> numLeadingZeros) << 1));
         pY3 = (cshort *) (pY + ((0xA0000000u >> numLeadingZeros) << 1));
         pY4 = (cshort *) (pY + ((0x40000000u >> numLeadingZeros) << 1));
         pY5 = (cshort *) (pY + ((0xC0000000u >> numLeadingZeros) << 1));
         pY6 = (cshort *) (pY + ((0x60000000u >> numLeadingZeros) << 1));
         pY7 = (cshort *) (pY + ((0xE0000000u >> numLeadingZeros) << 1));

#ifdef CL7X_HE_CSHORT_PTR_BUG
         short *myPY0 = (short *) pY0;
         short *myPY1 = (short *) pY1;
         short *myPY2 = (short *) pY2;
         short *myPY3 = (short *) pY3;
         short *myPY4 = (short *) pY4;
         short *myPY5 = (short *) pY5;
         short *myPY6 = (short *) pY6;
         short *myPY7 = (short *) pY7;
#endif

         for (l = 0; l < numChannels; l++) {
            for (k = 0; k < (numPoints >> 3); k += 8) {
               offsetBitReverse = __bit_reverse (k) >> numLeadingZeros;

               /* Interleaving vX_x and vX_x_1 in SE fetches optimizes
                * for SE tiling in transpose mode                      */
               vX_0      = c7x::strm_eng<0, CV>::get_adv ();
               vX_0_1    = c7x::strm_eng<0, CV>::get_adv ();
               vX_N_4    = c7x::strm_eng<0, CV>::get_adv ();
               vX_N_4_1  = c7x::strm_eng<0, CV>::get_adv ();
               vX_N_2    = c7x::strm_eng<0, CV>::get_adv ();
               vX_N_2_1  = c7x::strm_eng<0, CV>::get_adv ();
               vX_3N_4   = c7x::strm_eng<0, CV>::get_adv ();
               vX_3N_4_1 = c7x::strm_eng<0, CV>::get_adv ();

               vSum1  = vX_0 + vX_N_2;
               vSum2  = vX_N_4 + vX_3N_4;
               vDiff1 = vX_0 - vX_N_2;
               vDiff2 = vX_N_4 - vX_3N_4;

               vX0 = vSum1 + vSum2;
               vX1 = vDiff1 - __vcrot90h_vv (vDiff2);
               vX2 = vSum1 - vSum2;
               vX3 = vDiff1 + __vcrot90h_vv (vDiff2);

               vSum1_1  = vX_0_1 + vX_N_2_1;
               vSum2_1  = vX_N_4_1 + vX_3N_4_1;
               vDiff1_1 = vX_0_1 - vX_N_2_1;
               vDiff2_1 = vX_N_4_1 - vX_3N_4_1;

               vX0Temp = vSum1_1 + vSum2_1;
               vX1Temp = vDiff1_1 - __vcrot90h_vv (vDiff2_1);
               vX2Temp = vSum1_1 - vSum2_1;
               vX3Temp = vDiff1_1 + __vcrot90h_vv (vDiff2_1);

               vX0_1 = vX0Temp;
               vX1_1 = __cmpy_fx (vX1Temp, vTwX1);
               vX2_1 = __cmpy_fx (vX2Temp, vTwX2);
               vX3_1 = __cmpy_fx (vX3Temp, vTwX3);

               vX0_2PtDft_1 = vX0 + vX0_1;
               vX0_2PtDft_2 = vX0 - vX0_1;
               vX1_2PtDft_1 = vX1 + vX1_1;
               vX1_2PtDft_2 = vX1 - vX1_1;
               vX2_2PtDft_1 = vX2 + vX2_1;
               vX2_2PtDft_2 = vX2 - vX2_1;
               vX3_2PtDft_1 = vX3 + vX3_1;
               vX3_2PtDft_2 = vX3 - vX3_1;

               __vstore_reverse_bit (
                   (CVP) (pY0 + offsetBitReverse),
                   c7x::as_cshort_vec (__shift_right_round (
                       c7x::as_short_vec (vX0_2PtDft_1), vShift)));
               __vstore_reverse_bit (
                   (CVP) (pY1 + offsetBitReverse),
                   c7x::as_cshort_vec (__shift_right_round (
                       c7x::as_short_vec (vX0_2PtDft_2), vShift)));
               __vstore_reverse_bit (
                   (CVP) (pY2 + offsetBitReverse),
                   c7x::as_cshort_vec (__shift_right_round (
                       c7x::as_short_vec (vX1_2PtDft_1), vShift)));
               __vstore_reverse_bit (
                   (CVP) (pY3 + offsetBitReverse),
                   c7x::as_cshort_vec (__shift_right_round (
                       c7x::as_short_vec (vX1_2PtDft_2), vShift)));
               __vstore_reverse_bit (
                   (CVP) (pY4 + offsetBitReverse),
                   c7x::as_cshort_vec (__shift_right_round (
                       c7x::as_short_vec (vX2_2PtDft_1), vShift)));
               __vstore_reverse_bit (
                   (CVP) (pY5 + offsetBitReverse),
                   c7x::as_cshort_vec (__shift_right_round (
                       c7x::as_short_vec (vX2_2PtDft_2), vShift)));
               __vstore_reverse_bit (
                   (CVP) (pY6 + offsetBitReverse),
                   c7x::as_cshort_vec (__shift_right_round (
                       c7x::as_short_vec (vX3_2PtDft_1), vShift)));
               __vstore_reverse_bit (
                   (CVP) (pY7 + offsetBitReverse),
                   c7x::as_cshort_vec (__shift_right_round (
                       c7x::as_short_vec (vX3_2PtDft_2), vShift)));
            }
#ifdef CL7X_HE_CSHORT_PTR_BUG
            myPY0 += (numPoints << 1);
            myPY1 += (numPoints << 1);
            myPY2 += (numPoints << 1);
            myPY3 += (numPoints << 1);
            myPY4 += (numPoints << 1);
            myPY5 += (numPoints << 1);
            myPY6 += (numPoints << 1);
            myPY7 += (numPoints << 1);

            pY0 = (cshort *) myPY0;
            pY1 = (cshort *) myPY1;
            pY2 = (cshort *) myPY2;
            pY3 = (cshort *) myPY3;
            pY4 = (cshort *) myPY4;
            pY5 = (cshort *) myPY5;
            pY6 = (cshort *) myPY6;
            pY7 = (cshort *) myPY7;

#else
            pY0 += numPoints;
            pY1 += numPoints;
            pY2 += numPoints;
            pY3 += numPoints;
            pY4 += numPoints;
            pY5 += numPoints;
            pY6 += numPoints;
            pY7 += numPoints;
#endif
         }
         __SE0_CLOSE ();
      }
   }
   return (status);
}

#if (!defined(FFTLIB_REMOVE_CHECK_PARAMS) &&                                   \
     !defined(FFTLIB_FFT1DBATCHED_I16SC_C16SC_O16SC_REMOVE_CHECK_PARAMS)) ||   \
    (defined(FFTLIB_CHECK_PARAMS)) ||                                          \
    (defined(FFTLIB_FFT1DBATCHED_I16SC_C16SC_O16SC_CHECK_PARAMS))
FFTLIB_STATUS FFTLIB_fft1dBatched_i16sc_c16sc_o16sc_checkParams (
    int16_t              *pX,
    FFTLIB_bufParams1D_t *bufParamsX,
    int16_t              *pW,
    FFTLIB_bufParams1D_t *bufParamsW,
    int16_t              *pY,
    FFTLIB_bufParams1D_t *bufParamsY,
    uint32_t             *pShift,
    FFTLIB_bufParams1D_t *bufParamsShift,
    uint32_t              numPoints,
    uint32_t              numChannels,
    void                 *pBlock)

{
   FFTLIB_STATUS status = FFTLIB_SUCCESS;

   if ((pX == NULL) || (pW == NULL) || (pY == NULL) || (pShift == NULL) ||
       (pBlock == NULL)) {
      status = FFTLIB_ERR_NULL_POINTER;
   }
   /* else if (bufParamsX->dim_x != bufParamsW->dim_x || */
   /*          bufParamsX->dim_x != bufParamsY->dim_x) { */
   /*    status = FFTLIB_ERR_INVALID_DIMENSION; */
   /* } */
   /* else if (bufParamsX->dim_x < 64 * 2) { /\* Minimum number of points is 64
    * *\/ */
   /*    status = FFTLIB_ERR_INVALID_DIMENSION; */
   /* } */
   /* else if ((bufParamsX->data_type != FFTLIB_INT16) || */
   /*          (bufParamsW->data_type != FFTLIB_INT16) || */
   /*          (bufParamsY->data_type != FFTLIB_INT16)) { */
   /*    status = FFTLIB_ERR_INVALID_TYPE; */
   /* } */
   /* else if (bufParamsShift->data_type != FFTLIB_UINT32) { */
   /*    status = FFTLIB_ERR_INVALID_TYPE; */
   /* } */
   /* else if (((uint64_t) pX) & 0xFu) { /\* pX must be 16-byte aligned for a
    * *\/ */
   /*    status = FFTLIB_ERR_NOT_ALIGNED_PTRS_STRIDES; /\* streaming engine */
   /*                                                     configuration *\/ */
   /* } */
   /* else if (((1u << (bufParamsShift->dim_x * 2)) * 2 != */
   /*           bufParamsX->dim_x) && /\* check for powers of 4 *\/ */
   /*          ((1u << (bufParamsShift->dim_x * 2)) * 2 * 2 != */
   /*           bufParamsX->dim_x)) { /\* check for powers of 2 *\/ */
   /*    status = FFTLIB_ERR_INVALID_DIMENSION; */
   /* } */
   return (status);
}

#endif
