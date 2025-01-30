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

#include "../FFTLIB_fft1dBatched_i32fc_c32fc_o32fc.h"

#define TRACE_ON (0)

#if TRACE_ON
#include "../../../common/printv.h"
#include <stdio.h>
#endif

// CODE_SECTION(FFTLIB_fft1dBatched_i32fc_c32fc_o32fc, ".text:optimized")
// CODE_SECTION(FFTLIB_fft1dBatched_i32fc_c32fc_o32fc_core, ".text:optimized")
// CODE_SECTION(FFTLIB_fft1dBatched_i32fc_c32fc_o32fc_checkParams,
// ".text:optimized")

#define SE_PARAM_BASE         (0x0000)
#define SE_LOOP1_PARAM_OFFSET (SE_PARAM_BASE)
#define SE_LOOP2_PARAM_OFFSET (SE_LOOP1_PARAM_OFFSET + SE_PARAM_SIZE)
#define SE_LOOP3_PARAM_OFFSET (SE_LOOP2_PARAM_OFFSET + SE_PARAM_SIZE)
#define SE_LOOP4_PARAM_OFFSET (SE_LOOP3_PARAM_OFFSET + SE_PARAM_SIZE)
#define SE_LOOP5_PARAM_OFFSET (SE_LOOP4_PARAM_OFFSET + SE_PARAM_SIZE)
#define SE_LOOP6_PARAM_OFFSET (SE_LOOP5_PARAM_OFFSET + SE_PARAM_SIZE)
#define SE_LOOP7_PARAM_OFFSET (SE_LOOP6_PARAM_OFFSET + SE_PARAM_SIZE)
#define SE_TWID_PARAM_OFFSET  (SE_LOOP7_PARAM_OFFSET + SE_PARAM_SIZE)
#define SA_LOOP1_PARAM_OFFSET (SE_TWID_PARAM_OFFSET + SE_PARAM_SIZE)
#define SA_LOOP2_PARAM_OFFSET (SA_LOOP1_PARAM_OFFSET + SA_PARAM_SIZE)
#define SA_LOOP3_PARAM_OFFSET (SA_LOOP2_PARAM_OFFSET + SA_PARAM_SIZE)
#define SA_LOOP4_PARAM_OFFSET (SA_LOOP3_PARAM_OFFSET + SA_PARAM_SIZE)
#define SA_LOOP6_PARAM_OFFSET (SA_LOOP4_PARAM_OFFSET + SA_PARAM_SIZE)

FFTLIB_STATUS
FFTLIB_fft1dBatched_i32fc_c32fc_o32fc_init (FFTLIB_F32           *pX,
                                            FFTLIB_bufParams1D_t *bufParamsX,
                                            FFTLIB_F32           *pW,
                                            FFTLIB_bufParams1D_t *bufParamsW,
                                            FFTLIB_F32           *pY,
                                            FFTLIB_bufParams1D_t *bufParamsY,
                                            uint32_t              numPoints,
                                            uint32_t              numChannels,
                                            void                 *pBlock)
{
   FFTLIB_STATUS status = FFTLIB_SUCCESS;

#if defined(FFTLIB_CHECK_PARAMS) ||                                            \
    defined(FFTLIB_FFT1DBATCHED_I32FC_C32FC_O32FC_CHECK_PARAMS)
   /* status = FFTLIB_fft1dBatched_i32fc_c32fc_o32fc_checkParams ( */
   /*     pX, bufParamsX, pW, bufParamsW, pY, bufParamsY, numPoints,
    * numChannels, */
   /*     pBlock); */
   if (status == FFTLIB_SUCCESS)
#endif
   {
      uint32_t         numPointsPerDft;
      uint32_t         seCnt1, seCnt2, seCnt3, seCnt4;
      uint32_t         seCnt6, seCnt7, seCnt8, seCnt9, seCnt10;
      uint32_t         seCnt11;
      __SE_TEMPLATE_v1 se0_param = __gen_SE_TEMPLATE_v1 ();
      __SE_TEMPLATE_v1 se1_param = __gen_SE_TEMPLATE_v1 ();
      __SA_TEMPLATE_v1 sa0_param = __gen_SA_TEMPLATE_v1 ();

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

      se0_param       = __gen_SE_TEMPLATE_v1 ();
      se0_param.ICNT0 = 8; /* 8-point vectors processed in one shot  */
      se0_param.ICNT1 = 4;
      se0_param.DIM1  = seCnt1; /* 4 quarters(Offsets: 0, N/4, N/2, 3N/4) */
      se0_param.ICNT2 = seCnt2; /* Number of 8-point fetches within each  */
      se0_param.DIM2  = 8;      /* quarter                                */
      se0_param.ICNT3 = seCnt6; /* Number of DFT's for all channels  */
      se0_param.DIM3  = numPointsPerDft;

      se0_param.ELETYPE = __SE_ELETYPE_32BIT_CMPLX_SWAP;
      se0_param.VECLEN  = c7x::se_veclen<c7x::cfloat_vec>::value;
      se0_param.DIMFMT  = __SE_DIMFMT_4D;
      *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_LOOP1_PARAM_OFFSET)) =
          se0_param;

      se1_param       = __gen_SE_TEMPLATE_v1 ();
      se1_param.ICNT0 = 8; /* 8-point vectors processed in one shot  */
      se1_param.ICNT1 = 3;
      se1_param.DIM1  = seCnt1; /* Twiddle factors for x1, x2 and x3      */
      se1_param.ICNT2 = seCnt2; /* Number of 8-point fetches within each  */
      se1_param.DIM2  = 8;      /* quarter                                */
      se1_param.ICNT3 = seCnt6; /* Number of DFT's for all channels       */
      se1_param.DIM3  = 0;

      se1_param.ELETYPE = __SE_ELETYPE_32BIT_CMPLX_SWAP;
      se1_param.VECLEN  = c7x::se_veclen<c7x::cfloat_vec>::value;
      se1_param.DIMFMT  = __SE_DIMFMT_4D;
      *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_TWID_PARAM_OFFSET)) =
          se1_param;

      sa0_param       = __gen_SA_TEMPLATE_v1 ();
      sa0_param.ICNT0 = 8;
      sa0_param.ICNT1 = 4;
      sa0_param.DIM1  = seCnt1; /* Save to each of the 4 quarters         */
      sa0_param.ICNT2 = seCnt2; /* Number of 8-point stores within each   */
      sa0_param.DIM2  = 8;      /* quarter                                */
      sa0_param.ICNT3 = seCnt6;
      sa0_param.DIM3  = numPointsPerDft; /* Number of DFT's for all channels */

      sa0_param.VECLEN = c7x::sa_veclen<c7x::cfloat_vec>::value;
      sa0_param.DIMFMT = __SA_DIMFMT_4D;
      *((__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA_LOOP1_PARAM_OFFSET)) =
          sa0_param;

      se0_param       = __gen_SE_TEMPLATE_v1 ();
      se0_param.ICNT0 = 8; /* Fetch first two quarters               */
      se0_param.ICNT1 = 2;
      se0_param.DIM1  = 16; /* Process two 16-point DFTs in one shot  */
      se0_param.ICNT2 = seCnt7;
      se0_param.DIM2  = 32;

      se0_param.ELETYPE = __SE_ELETYPE_32BIT_CMPLX_SWAP;
      se0_param.TRANSPOSE =
          __SE_TRANSPOSE_256BIT; /* Using 256BIT transpose required  */
                                 /* 16-byte alignment on pX          */
      se0_param.VECLEN = c7x::se_veclen<c7x::cfloat_vec>::value;
      se0_param.DIMFMT = __SE_DIMFMT_3D;
      *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_LOOP2_PARAM_OFFSET)) =
          se0_param;

      sa0_param       = __gen_SA_TEMPLATE_v1 ();
      sa0_param.ICNT0 = seCnt8; /* Input buffer must be at least 32
                                 * elements long even though
                                 * numPoints*numChannels = 16             */

      sa0_param.VECLEN = c7x::sa_veclen<c7x::cfloat_vec>::value;
      sa0_param.DIMFMT = __SA_DIMFMT_1D;
      *((__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA_LOOP2_PARAM_OFFSET)) =
          sa0_param;

      se0_param       = __gen_SE_TEMPLATE_v1 ();
      se0_param.ICNT0 = seCnt8;

      se0_param.ELETYPE = __SE_ELETYPE_32BIT_CMPLX_SWAP;
      se0_param.VECLEN  = c7x::se_veclen<c7x::cfloat_vec>::value;
      se0_param.DIMFMT  = __SE_DIMFMT_1D;
      *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_LOOP3_PARAM_OFFSET)) =
          se0_param;

      sa0_param       = __gen_SA_TEMPLATE_v1 ();
      sa0_param.ICNT0 = seCnt8;

      sa0_param.VECLEN = c7x::sa_veclen<c7x::cfloat_vec>::value;
      sa0_param.DIMFMT = __SA_DIMFMT_1D;
      *((__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA_LOOP3_PARAM_OFFSET)) =
          sa0_param;

      /* The following SE configuration may cause sub-optimal
       * tile in SE because second row of tile starts in the
       * middle of first row                                  */
      se0_param       = __gen_SE_TEMPLATE_v1 ();
      se0_param.ICNT0 = 4;
      se0_param.ICNT1 = 8;
      se0_param.DIM1  = 4;
      se0_param.ICNT2 = seCnt7;
      se0_param.DIM2  = 32;

      se0_param.ELETYPE   = __SE_ELETYPE_32BIT_CMPLX_SWAP;
      se0_param.TRANSPOSE = __SE_TRANSPOSE_64BIT;
      se0_param.VECLEN    = c7x::se_veclen<c7x::cfloat_vec>::value;
      se0_param.DIMFMT    = __SE_DIMFMT_3D;
      *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_LOOP4_PARAM_OFFSET)) =
          se0_param;

      sa0_param       = __gen_SA_TEMPLATE_v1 ();
      sa0_param.ICNT0 = seCnt9; /* Input buffer must be at least 32
                                 * elements long even though
                                 * numPoints*numChannels = 16             */

      sa0_param.VECLEN = c7x::sa_veclen<c7x::cfloat_vec>::value;
      sa0_param.DIMFMT = __SA_DIMFMT_1D;
      *((__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA_LOOP4_PARAM_OFFSET)) =
          sa0_param;

      se0_param       = __gen_SE_TEMPLATE_v1 ();
      se0_param.ICNT0 = seCnt4; /* Fetch consecutive four points for DFT  */
      se0_param.ICNT1 = 8;
      se0_param.DIM1 =
          /* Fetch 8 points separated by            */ seCnt4; /* (numPoints >>
                                                                  3). This fetch
                                                                  pattern   */
      /* can be used for bit reversal           */
      se0_param.ICNT2 = numChannels;
      se0_param.DIM2  = numPoints;

      se0_param.ELETYPE   = __SE_ELETYPE_32BIT_CMPLX_SWAP;
      se0_param.TRANSPOSE = __SE_TRANSPOSE_64BIT;
      se0_param.VECLEN    = c7x::se_veclen<c7x::cfloat_vec>::value;
      se0_param.DIMFMT    = __SE_DIMFMT_3D;
      *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_LOOP5_PARAM_OFFSET)) =
          se0_param;

      se0_param       = __gen_SE_TEMPLATE_v1 ();
      se0_param.ICNT0 = 8;
      se0_param.ICNT1 = 8;
      se0_param.DIM1  = 8;
      se0_param.ICNT2 = seCnt10;
      se0_param.DIM2  = 64;

      se0_param.ELETYPE   = __SE_ELETYPE_32BIT_CMPLX_SWAP;
      se0_param.TRANSPOSE = __SE_TRANSPOSE_64BIT;
      se0_param.VECLEN    = c7x::se_veclen<c7x::cfloat_vec>::value;
      se0_param.DIMFMT    = __SE_DIMFMT_3D;
      *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_LOOP6_PARAM_OFFSET)) =
          se0_param;

      sa0_param       = __gen_SA_TEMPLATE_v1 ();
      sa0_param.ICNT0 = seCnt11; /* Input buffer must be at least 64
                                  * elements long even though
                                  * numPoints*numChannels = 32             */

      sa0_param.VECLEN = c7x::sa_veclen<c7x::cfloat_vec>::value;
      sa0_param.DIMFMT = __SA_DIMFMT_1D;
      *((__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA_LOOP6_PARAM_OFFSET)) =
          sa0_param;

      se0_param       = __gen_SE_TEMPLATE_v1 ();
      se0_param.ICNT0 = seCnt4;
      se0_param.ICNT1 = 8;
      se0_param.DIM1 =
          /* Fetch 8 points separated by            */ seCnt4; /* (numPoints >>
                                                                  3). This fetch
                                                                  pattern   */
      /* can be used for bit reversal           */
      se0_param.ICNT2 = numChannels;
      se0_param.DIM2  = numPoints;

      se0_param.ELETYPE   = __SE_ELETYPE_32BIT_CMPLX_SWAP;
      se0_param.TRANSPOSE = __SE_TRANSPOSE_64BIT;
      se0_param.VECLEN    = c7x::se_veclen<c7x::cfloat_vec>::value;
      se0_param.DIMFMT    = __SE_DIMFMT_3D;
      *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_LOOP7_PARAM_OFFSET)) =
          se0_param;
   }
   return (status);
}

FFTLIB_STATUS
FFTLIB_fft1dBatched_i32fc_c32fc_o32fc_kernel (FFTLIB_F32           *pX,
                                              FFTLIB_bufParams1D_t *bufParamsX,
                                              FFTLIB_F32           *pW,
                                              FFTLIB_bufParams1D_t *bufParamsW,
                                              FFTLIB_F32           *pY,
                                              FFTLIB_bufParams1D_t *bufParamsY,
                                              uint32_t              numPoints,
                                              uint32_t              numChannels,
                                              void                 *pBlock)
{
   uint32_t      k, l;
   FFTLIB_STATUS status = FFTLIB_SUCCESS;
   uint32_t      numPointsPerDft;
   uint32_t      numLeadingZeros;
   uint32_t      offsetBitReverse;
   uint32_t      seCnt1, seCnt2, seCnt3, seCnt6;

   __SE_TEMPLATE_v1 se0_param;
   __SE_TEMPLATE_v1 se1_param;
   __SA_TEMPLATE_v1 sa0_param;

   cfloat* restrict pXLocal;
   cfloat* restrict pYLocal;
   cfloat* restrict pWLocal;
   cfloat* restrict pY0;
   cfloat* restrict pY1;
   cfloat* restrict pY2;
   cfloat* restrict pY3;
   cfloat* restrict pY4;
   cfloat* restrict pY5;
   cfloat* restrict pY6;
   cfloat* restrict pY7;

   typedef typename c7x::cfloat_vec             CV;
   typedef CV* CVP;

   /* typedef typename c7x::float_vec V; */
   /* typedef V* VP; */

   CV     vX_0, vX_N_4, vX_N_2, vX_3N_4;
   CV     vSum1, vSum2, vDiff1, vDiff2;
   CV     vTwX1, vTwX2, vTwX3;
   CV     vX0Temp, vX1Temp, vX2Temp, vX3Temp;
   CV     vX0, vX1, vX2, vX3;
   CV     vX_0_1, vX_N_4_1, vX_N_2_1, vX_3N_4_1;
   CV     vSum1_1, vSum2_1, vDiff1_1, vDiff2_1;
   CV     vX0_1, vX1_1, vX2_1, vX3_1;
   CV     vX0_2PtDft_1, vX0_2PtDft_2;
   CV     vX1_2PtDft_1, vX1_2PtDft_2;
   CV     vX2_2PtDft_1, vX2_2PtDft_2;
   CV     vX3_2PtDft_1, vX3_2PtDft_2;
   CV     vX01_lo, vX23_lo, vX01_hi, vX23_hi;
   cfloat twTemp;

#ifdef FFTLIB_CHECK_PARAMS
   /* status = FFTLIB_fft1dBatched_i32fc_c32fc_o32fc_checkParams ( */
   /*     pX, bufParamsX, pW, bufParamsW, pY, bufParamsY, numPoints,
    * numChannels, */
   /*     pBlock); */
   if (status == FFTLIB_SUCCESS)
#endif
   {
      numPointsPerDft = numPoints;

      se0_param =
          *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_LOOP1_PARAM_OFFSET));
      se1_param =
          *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_TWID_PARAM_OFFSET));
      sa0_param =
          *((__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA_LOOP1_PARAM_OFFSET));
      seCnt1 = numPointsPerDft >> 2;
      seCnt2 = numPointsPerDft >> 5;
      seCnt3 = 1;

      pXLocal = (cfloat*) pX;
      pWLocal = (cfloat*) pW;
      pYLocal = (cfloat*) pY;

      while (numPointsPerDft >= 64) {

         seCnt6          = seCnt3 * numChannels;
         se0_param.ICNT1 = 4;
         se0_param.DIM1  = seCnt1; /* 4 quarters(Offsets: 0, N/4, N/2, 3N/4) */
         se0_param.ICNT2 = seCnt2;
         se0_param.DIM2 = 8; /* Number of 8-point fetches within each quarter */
         se0_param.ICNT3 = seCnt6;
         se0_param.DIM3 =
             numPointsPerDft; /* Number of DFT's for all channels    */
         __SE0_OPEN ((void *) pXLocal, se0_param);

         se1_param.ICNT1 = 3;
         se1_param.DIM1  = seCnt1; /* Twiddle factors for x1, x2 and x3      */
         se1_param.ICNT2 =
             seCnt2; /* Number of 8-point fetches within each  quarter*/
         se1_param.DIM2  = 8;
         se1_param.ICNT3 = seCnt6; /* Number of DFT's for all channels       */
         se1_param.DIM3  = 0;
         __SE1_OPEN ((void *) pWLocal, se1_param);

         sa0_param.ICNT1 = 4;
         sa0_param.DIM1  = seCnt1; /* Save to each of the 4 quarters         */
         sa0_param.ICNT2 = seCnt2;
         sa0_param.DIM2  = 8;
         /* Number of 8-point stores within each quarter */
         sa0_param.ICNT3 = seCnt6;
         sa0_param.DIM3  = numPointsPerDft; /* Number of DFT's */
         __SA0_OPEN (sa0_param);

         /* Loop is unrolled twice for better optimization */
         for (k = 0; k < numPoints * numChannels; k += 64) {

            /* First iteration of loop unroll */
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
            vX1Temp = vDiff1 - __vcrot90sp_vv (vDiff2);
            vX2Temp = vSum1 - vSum2;
            vX3Temp = vDiff1 + __vcrot90sp_vv (vDiff2);

            vX0 = vX0Temp;
            vX1 = __complex_multiply (vX1Temp, vTwX1);
            vX2 = __complex_multiply (vX2Temp, vTwX2);
            vX3 = __complex_multiply (vX3Temp, vTwX3);

            __vpred tmp;
            CVP     addr;

            tmp  = c7x::strm_agen<0, CV>::get_vpred ();
            addr = c7x::strm_agen<0, CV>::get_adv (pXLocal);
            __vstore_pred (tmp, addr, vX0);

            tmp  = c7x::strm_agen<0, CV>::get_vpred ();
            addr = c7x::strm_agen<0, CV>::get_adv (pXLocal);
            __vstore_pred (tmp, addr, vX2);

            tmp  = c7x::strm_agen<0, CV>::get_vpred ();
            addr = c7x::strm_agen<0, CV>::get_adv (pXLocal);
            __vstore_pred (tmp, addr, vX1);

            tmp  = c7x::strm_agen<0, CV>::get_vpred ();
            addr = c7x::strm_agen<0, CV>::get_adv (pXLocal);
            __vstore_pred (tmp, addr, vX3);

            /* Second iteration of loop unroll */
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
            vX1Temp = vDiff1 - __vcrot90sp_vv (vDiff2);
            vX2Temp = vSum1 - vSum2;
            vX3Temp = vDiff1 + __vcrot90sp_vv (vDiff2);

            vX0 = vX0Temp;
            vX1 = __complex_multiply (vX1Temp, vTwX1);
            vX2 = __complex_multiply (vX2Temp, vTwX2);
            vX3 = __complex_multiply (vX3Temp, vTwX3);

            tmp  = c7x::strm_agen<0, CV>::get_vpred ();
            addr = c7x::strm_agen<0, CV>::get_adv (pXLocal);
            __vstore_pred (tmp, addr, vX0);

            tmp  = c7x::strm_agen<0, CV>::get_vpred ();
            addr = c7x::strm_agen<0, CV>::get_adv (pXLocal);
            __vstore_pred (tmp, addr, vX2);

            tmp  = c7x::strm_agen<0, CV>::get_vpred ();
            addr = c7x::strm_agen<0, CV>::get_adv (pXLocal);
            __vstore_pred (tmp, addr, vX1);

            tmp  = c7x::strm_agen<0, CV>::get_vpred ();
            addr = c7x::strm_agen<0, CV>::get_adv (pXLocal);
            __vstore_pred (tmp, addr, vX3);
         }
         __SA0_CLOSE ();
         __SE0_CLOSE ();
         __SE1_CLOSE ();

         numPointsPerDft >>= 2;
         pWLocal += numPointsPerDft * 3;
         seCnt1 >>= 2;
         seCnt2 >>= 2;
         seCnt3 <<= 2;
      }

      if (numPointsPerDft == 16) {
         /* 16-point stage */
         se0_param = *(
             (__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_LOOP2_PARAM_OFFSET));
         __SE0_OPEN ((void *) pXLocal, se0_param);
         __SE1_OPEN ((void *) (pXLocal + 8), se0_param);

         sa0_param = *(
             (__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA_LOOP2_PARAM_OFFSET));
         __SA0_OPEN (sa0_param);

         vTwX1 = *((CVP) pWLocal);
         vTwX2 = *((CVP) (pWLocal + 4));
         vTwX3 = *((CVP) (pWLocal + 8));

#if __C7X_HOSTEM__
         vTwX1 = CV (vTwX1.lo(), vTwX1.lo());
         vTwX2 = CV (vTwX2.lo(), vTwX2.lo());
         vTwX3 = CV (vTwX3.lo(), vTwX3.lo());
#else
         vTwX1 = (CV) (vTwX1.lo(), vTwX1.lo());
         vTwX2 = (CV) (vTwX2.lo(), vTwX2.lo());
         vTwX3 = (CV) (vTwX3.lo(), vTwX3.lo());
#endif

         for (k = 0; k < numPoints * numChannels; k += 32) {
            vX_0    = c7x::strm_eng<0, CV>::get_adv ();
            vX_N_4  = c7x::strm_eng<0, CV>::get_adv ();
            vX_N_2  = c7x::strm_eng<1, CV>::get_adv ();
            vX_3N_4 = c7x::strm_eng<1, CV>::get_adv ();

            vSum1  = vX_0 + vX_N_2;
            vSum2  = vX_N_4 + vX_3N_4;
            vDiff1 = vX_0 - vX_N_2;
            vDiff2 = vX_N_4 - vX_3N_4;

            vX0Temp = vSum1 + vSum2;
            vX1Temp = vDiff1 - __vcrot90sp_vv (vDiff2);
            vX2Temp = vSum1 - vSum2;
            vX3Temp = vDiff1 + __vcrot90sp_vv (vDiff2);

            vX0 = vX0Temp;
            vX1 = __complex_multiply (vX1Temp, vTwX1);
            vX2 = __complex_multiply (vX2Temp, vTwX2);
            vX3 = __complex_multiply (vX3Temp, vTwX3);

#if __C7X_HOSTEM__
            __vpred tmp = c7x::strm_agen<0, CV>::get_vpred ();
            CVP     addr;
            addr = c7x::strm_agen<0, CV>::get_adv (pXLocal);
            __vstore_pred (tmp, addr, CV (vX0.lo(), vX2.lo()));

            tmp  = c7x::strm_agen<0, CV>::get_vpred ();
            addr = c7x::strm_agen<0, CV>::get_adv (pXLocal);
            __vstore_pred (tmp, addr, CV (vX1.lo(), vX3.lo()));

            tmp  = c7x::strm_agen<0, CV>::get_vpred ();
            addr = c7x::strm_agen<0, CV>::get_adv (pXLocal);
            __vstore_pred (tmp, addr, CV (vX0.hi(), vX2.hi()));

            tmp  = c7x::strm_agen<0, CV>::get_vpred ();
            addr = c7x::strm_agen<0, CV>::get_adv (pXLocal);
            __vstore_pred (tmp, addr, CV (vX1.hi(), vX3.hi()));
#else
            __vpred tmp = c7x::strm_agen<0, CV>::get_vpred ();
            CVP     addr;
            addr = c7x::strm_agen<0, CV>::get_adv (pXLocal);
            __vstore_pred (tmp, addr, (CV) (vX0.lo(), vX2.lo()));

            tmp  = c7x::strm_agen<0, CV>::get_vpred ();
            addr = c7x::strm_agen<0, CV>::get_adv (pXLocal);
            __vstore_pred (tmp, addr, (CV) (vX1.lo(), vX3.lo()));

            tmp  = c7x::strm_agen<0, CV>::get_vpred ();
            addr = c7x::strm_agen<0, CV>::get_adv (pXLocal);
            __vstore_pred (tmp, addr, (CV) (vX0.hi(), vX2.hi()));

            tmp  = c7x::strm_agen<0, CV>::get_vpred ();
            addr = c7x::strm_agen<0, CV>::get_adv (pXLocal);
            __vstore_pred (tmp, addr, (CV) (vX1.hi(), vX3.hi()));
#endif
         }
         __SA0_CLOSE ();
         __SE0_CLOSE ();
         __SE1_CLOSE ();
      }
      else {
         /* 32-point stage */
         se0_param = *(
             (__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_LOOP3_PARAM_OFFSET));
         __SE0_OPEN ((void *) pXLocal, se0_param);

         sa0_param = *(
             (__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA_LOOP3_PARAM_OFFSET));
         __SA0_OPEN (sa0_param);

         vTwX1 = *((CVP) pWLocal);
         vTwX2 = *((CVP) (pWLocal + 8));
         vTwX3 = *((CVP) (pWLocal + 16));

         for (k = 0; k < numPoints * numChannels; k += 32) {
            vX_0    = c7x::strm_eng<0, CV>::get_adv ();
            vX_N_4  = c7x::strm_eng<0, CV>::get_adv ();
            vX_N_2  = c7x::strm_eng<0, CV>::get_adv ();
            vX_3N_4 = c7x::strm_eng<0, CV>::get_adv ();

            vSum1  = vX_0 + vX_N_2;
            vSum2  = vX_N_4 + vX_3N_4;
            vDiff1 = vX_0 - vX_N_2;
            vDiff2 = vX_N_4 - vX_3N_4;

            vX0Temp = vSum1 + vSum2;
            vX1Temp = vDiff1 - __vcrot90sp_vv (vDiff2);
            vX2Temp = vSum1 - vSum2;
            vX3Temp = vDiff1 + __vcrot90sp_vv (vDiff2);

            vX0 = vX0Temp;
            vX1 = __complex_multiply (vX1Temp, vTwX1);
            vX2 = __complex_multiply (vX2Temp, vTwX2);
            vX3 = __complex_multiply (vX3Temp, vTwX3);

            __vpred tmp = c7x::strm_agen<0, CV>::get_vpred ();
            CVP     addr;
            addr = c7x::strm_agen<0, CV>::get_adv (pXLocal);
            __vstore_pred (tmp, addr, vX0);

            tmp  = c7x::strm_agen<0, CV>::get_vpred ();
            addr = c7x::strm_agen<0, CV>::get_adv (pXLocal);
            __vstore_pred (tmp, addr, vX2);

            tmp  = c7x::strm_agen<0, CV>::get_vpred ();
            addr = c7x::strm_agen<0, CV>::get_adv (pXLocal);
            __vstore_pred (tmp, addr, vX1);

            tmp  = c7x::strm_agen<0, CV>::get_vpred ();
            addr = c7x::strm_agen<0, CV>::get_adv (pXLocal);
            __vstore_pred (tmp, addr, vX3);
         }
         __SE0_CLOSE ();
         __SA0_CLOSE ();
      }

      numPointsPerDft >>= 2;
      pWLocal += numPointsPerDft * 3;

      if (numPointsPerDft == 4) {
         /* 4-point stage with bit-reversal */

         if (numPoints == 16) {
// clang-format off
#if __C7X_HOSTEM__
                c7x::uchar_vec vXPermCtrl  = c7x::uchar_vec(0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                                0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                                                0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                                                0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
                                                0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
                                                0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
                                                0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
                                                0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F);
#else
                c7x::uchar_vec vXPermCtrl  = (c7x::uchar_vec)(0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                                0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                                                0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                                                0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
                                                0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
                                                0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
                                                0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
                                                0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F);
#endif
            // clang-format on

            se0_param = *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                                                SE_LOOP4_PARAM_OFFSET));
            __SE0_OPEN ((void *) pXLocal, se0_param);

            sa0_param = *((__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                                                SA_LOOP4_PARAM_OFFSET));
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
               vX1 = vDiff1 - __vcrot90sp_vv (vDiff2);
               vX2 = vSum1 - vSum2;
               vX3 = vDiff1 + __vcrot90sp_vv (vDiff2);

               vX01_lo = c7x::as_cfloat_vec (
                   __vpermll_yvvv (vXPermCtrl, c7x::as_uchar_vec (vX1),
                                   c7x::as_uchar_vec (vX0)));
               vX23_lo = c7x::as_cfloat_vec (
                   __vpermll_yvvv (vXPermCtrl, c7x::as_uchar_vec (vX3),
                                   c7x::as_uchar_vec (vX2)));
               vX01_hi = c7x::as_cfloat_vec (
                   __vpermhh_yvvv (vXPermCtrl, c7x::as_uchar_vec (vX1),
                                   c7x::as_uchar_vec (vX0)));
               vX23_hi = c7x::as_cfloat_vec (
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
            se0_param = *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                                                SE_LOOP5_PARAM_OFFSET));
            __SE0_OPEN ((void *) pXLocal, se0_param);

            numLeadingZeros = __norm ((int32_t) (numPoints - 1)) + 1;

            /* pY0 = &pYLocal[0x00000000u]; */
            /* pY1 = &pYLocal[0x40000000u >> numLeadingZeros]; */
            /* pY2 = &pYLocal[0x80000000u >> numLeadingZeros]; */
            /* pY3 = &pYLocal[0xC0000000u >> numLeadingZeros]; */

            pY0 = (cfloat*) (pY + 0);
            pY1 = (cfloat*) (pY + ((0x40000000u >> numLeadingZeros) << 1));
            pY2 = (cfloat*) (pY + ((0x80000000u >> numLeadingZeros) << 1));
            pY3 = (cfloat*) (pY + ((0xC0000000u >> numLeadingZeros) << 1));

#ifdef CL7X_HE_CFLOAT_PTR_BUG
            float *myPY0 = (float *) pY0;
            float *myPY1 = (float *) pY1;
            float *myPY2 = (float *) pY2;
            float *myPY3 = (float *) pY3;
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
                  vX1 = vDiff1 - __vcrot90sp_vv (vDiff2);
                  vX2 = vSum1 - vSum2;
                  vX3 = vDiff1 + __vcrot90sp_vv (vDiff2);

                  /* __vstore_reverse_bit ((CVP) &pY0[offsetBitReverse],
                   */
                  /*                       vX0); */
                  /* __vstore_reverse_bit ((CVP) &pY1[offsetBitReverse],
                   */
                  /*                       vX1); */
                  /* __vstore_reverse_bit ((CVP) &pY2[offsetBitReverse],
                   */
                  /*                       vX2); */
                  /* __vstore_reverse_bit ((CVP) &pY3[offsetBitReverse],
                   */
                  /*                       vX3); */

                  __vstore_reverse_bit ((CVP) (pY0 + offsetBitReverse), vX0);
                  __vstore_reverse_bit ((CVP) (pY1 + offsetBitReverse), vX1);
                  __vstore_reverse_bit ((CVP) (pY2 + offsetBitReverse), vX2);
                  __vstore_reverse_bit ((CVP) (pY3 + offsetBitReverse), vX3);
               }

#ifdef CL7X_HE_CFLOAT_PTR_BUG
               myPY0 += (numPoints << 1);
               myPY1 += (numPoints << 1);
               myPY2 += (numPoints << 1);
               myPY3 += (numPoints << 1);

               pY0 = (cfloat*) myPY0;
               pY1 = (cfloat*) myPY1;
               pY2 = (cfloat*) myPY2;
               pY3 = (cfloat*) myPY3;

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

#if __C7X_HOSTEM__
         pWLocal += 1;
         twTemp = *pWLocal;
         vTwX1  = CV (twTemp, twTemp, twTemp, twTemp, twTemp, twTemp, twTemp,
                      twTemp);
         pWLocal += 2;
         twTemp = *pWLocal;
         vTwX2  = CV (twTemp, twTemp, twTemp, twTemp, twTemp, twTemp, twTemp,
                      twTemp);
         pWLocal += 2;
         twTemp = *pWLocal;
         vTwX3  = CV (twTemp, twTemp, twTemp, twTemp, twTemp, twTemp, twTemp,
                      twTemp);
#else
         pWLocal += 1;
         twTemp = *pWLocal;
         vTwX1  = (CV) (twTemp, twTemp, twTemp, twTemp, twTemp, twTemp, twTemp,
                       twTemp);
         pWLocal += 2;
         twTemp = *pWLocal;
         vTwX2  = (CV) (twTemp, twTemp, twTemp, twTemp, twTemp, twTemp, twTemp,
                       twTemp);
         pWLocal += 2;
         twTemp = *pWLocal;
         vTwX3  = (CV) (twTemp, twTemp, twTemp, twTemp, twTemp, twTemp, twTemp,
                       twTemp);
#endif

         if (numPoints == 32) {
// clang-format off
            #if __C7X_HOSTEM__
                c7x::uchar_vec vXPermCtrl  = c7x::uchar_vec(0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                                0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                                                0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                                                0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
                                                0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
                                                0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
                                                0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
                                                0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F);
                #else
                c7x::uchar_vec vXPermCtrl  = (c7x::uchar_vec)(0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                                0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                                                0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                                                0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
                                                0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
                                                0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
                                                0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
                                                0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F);
                #endif
            // clang-format on
            CV vX01_2PtDft_1_lo, vX23_2PtDft_1_lo, vX01_2PtDft_2_lo,
                vX23_2PtDft_2_lo;
            CV vX01_2PtDft_1_hi, vX23_2PtDft_1_hi, vX01_2PtDft_2_hi,
                vX23_2PtDft_2_hi;

            se0_param = *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                                                SE_LOOP6_PARAM_OFFSET));
            __SE0_OPEN ((void *) pXLocal, se0_param);

            sa0_param = *((__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                                                SA_LOOP6_PARAM_OFFSET));
            __SA0_OPEN (sa0_param);

            for (k = 0; k < numChannels << 5; k += 64) {
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
               vX1 = vDiff1 - __vcrot90sp_vv (vDiff2);
               vX2 = vSum1 - vSum2;
               vX3 = vDiff1 + __vcrot90sp_vv (vDiff2);

               vSum1_1  = vX_0_1 + vX_N_2_1;
               vSum2_1  = vX_N_4_1 + vX_3N_4_1;
               vDiff1_1 = vX_0_1 - vX_N_2_1;
               vDiff2_1 = vX_N_4_1 - vX_3N_4_1;

               vX0Temp = vSum1_1 + vSum2_1;
               vX1Temp = vDiff1_1 - __vcrot90sp_vv (vDiff2_1);
               vX2Temp = vSum1_1 - vSum2_1;
               vX3Temp = vDiff1_1 + __vcrot90sp_vv (vDiff2_1);

               vX0_1 = vX0Temp;
               vX1_1 = __complex_multiply (vX1Temp, vTwX1);
               vX2_1 = __complex_multiply (vX2Temp, vTwX2);
               vX3_1 = __complex_multiply (vX3Temp, vTwX3);

               vX0_2PtDft_1 = vX0 + vX0_1;
               vX0_2PtDft_2 = vX0 - vX0_1;
               vX1_2PtDft_1 = vX1 + vX1_1;
               vX1_2PtDft_2 = vX1 - vX1_1;
               vX2_2PtDft_1 = vX2 + vX2_1;
               vX2_2PtDft_2 = vX2 - vX2_1;
               vX3_2PtDft_1 = vX3 + vX3_1;
               vX3_2PtDft_2 = vX3 - vX3_1;

               /* Permute to obtain bit-reversal order */
               vX01_2PtDft_1_lo = c7x::as_cfloat_vec (
                   __vpermll_yvvv (vXPermCtrl, c7x::as_uchar_vec (vX1_2PtDft_1),
                                   c7x::as_uchar_vec (vX0_2PtDft_1)));
               vX23_2PtDft_1_lo = c7x::as_cfloat_vec (
                   __vpermll_yvvv (vXPermCtrl, c7x::as_uchar_vec (vX3_2PtDft_1),
                                   c7x::as_uchar_vec (vX2_2PtDft_1)));
               vX01_2PtDft_2_lo = c7x::as_cfloat_vec (
                   __vpermll_yvvv (vXPermCtrl, c7x::as_uchar_vec (vX1_2PtDft_2),
                                   c7x::as_uchar_vec (vX0_2PtDft_2)));
               vX23_2PtDft_2_lo = c7x::as_cfloat_vec (
                   __vpermll_yvvv (vXPermCtrl, c7x::as_uchar_vec (vX3_2PtDft_2),
                                   c7x::as_uchar_vec (vX2_2PtDft_2)));
               vX01_2PtDft_1_hi = c7x::as_cfloat_vec (
                   __vpermhh_yvvv (vXPermCtrl, c7x::as_uchar_vec (vX1_2PtDft_1),
                                   c7x::as_uchar_vec (vX0_2PtDft_1)));
               vX23_2PtDft_1_hi = c7x::as_cfloat_vec (
                   __vpermhh_yvvv (vXPermCtrl, c7x::as_uchar_vec (vX3_2PtDft_1),
                                   c7x::as_uchar_vec (vX2_2PtDft_1)));
               vX01_2PtDft_2_hi = c7x::as_cfloat_vec (
                   __vpermhh_yvvv (vXPermCtrl, c7x::as_uchar_vec (vX1_2PtDft_2),
                                   c7x::as_uchar_vec (vX0_2PtDft_2)));
               vX23_2PtDft_2_hi = c7x::as_cfloat_vec (
                   __vpermhh_yvvv (vXPermCtrl, c7x::as_uchar_vec (vX3_2PtDft_2),
                                   c7x::as_uchar_vec (vX2_2PtDft_2)));

               __vpred tmp = c7x::strm_agen<0, CV>::get_vpred ();
               CVP     addr;
               addr = c7x::strm_agen<0, CV>::get_adv (pYLocal);
               __vstore_pred (tmp, addr, vX01_2PtDft_1_lo);

               tmp  = c7x::strm_agen<0, CV>::get_vpred ();
               addr = c7x::strm_agen<0, CV>::get_adv (pYLocal);
               __vstore_pred (tmp, addr, vX23_2PtDft_1_lo);

               tmp  = c7x::strm_agen<0, CV>::get_vpred ();
               addr = c7x::strm_agen<0, CV>::get_adv (pYLocal);
               __vstore_pred (tmp, addr, vX01_2PtDft_2_lo);

               tmp  = c7x::strm_agen<0, CV>::get_vpred ();
               addr = c7x::strm_agen<0, CV>::get_adv (pYLocal);
               __vstore_pred (tmp, addr, vX23_2PtDft_2_lo);

               tmp  = c7x::strm_agen<0, CV>::get_vpred ();
               addr = c7x::strm_agen<0, CV>::get_adv (pYLocal);
               __vstore_pred (tmp, addr, vX01_2PtDft_1_hi);

               tmp  = c7x::strm_agen<0, CV>::get_vpred ();
               addr = c7x::strm_agen<0, CV>::get_adv (pYLocal);
               __vstore_pred (tmp, addr, vX23_2PtDft_1_hi);

               tmp  = c7x::strm_agen<0, CV>::get_vpred ();
               addr = c7x::strm_agen<0, CV>::get_adv (pYLocal);
               __vstore_pred (tmp, addr, vX01_2PtDft_2_hi);

               tmp  = c7x::strm_agen<0, CV>::get_vpred ();
               addr = c7x::strm_agen<0, CV>::get_adv (pYLocal);
               __vstore_pred (tmp, addr, vX23_2PtDft_2_hi);
            }
            __SE0_CLOSE ();
            __SA0_CLOSE ();
         }
         else {
            se0_param = *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                                                SE_LOOP7_PARAM_OFFSET));
            __SE0_OPEN ((void *) pXLocal, se0_param);

            numLeadingZeros = __norm ((int32_t) (numPoints - 1)) + 1;

            /* pY0 = &pYLocal[0x00000000u]; */
            /* pY1 = &pYLocal[0x80000000u >> numLeadingZeros]; */
            /* pY2 = &pYLocal[0x20000000u >> numLeadingZeros]; */
            /* pY3 = &pYLocal[0xA0000000u >> numLeadingZeros]; */
            /* pY4 = &pYLocal[0x40000000u >> numLeadingZeros]; */
            /* pY5 = &pYLocal[0xC0000000u >> numLeadingZeros]; */
            /* pY6 = &pYLocal[0x60000000u >> numLeadingZeros]; */
            /* pY7 = &pYLocal[0xE0000000u >> numLeadingZeros]; */

            pY0 = (cfloat*) (pY + (0x00000000u));
            pY1 = (cfloat*) (pY + ((0x80000000u >> numLeadingZeros) << 1));
            pY2 = (cfloat*) (pY + ((0x20000000u >> numLeadingZeros) << 1));
            pY3 = (cfloat*) (pY + ((0xA0000000u >> numLeadingZeros) << 1));
            pY4 = (cfloat*) (pY + ((0x40000000u >> numLeadingZeros) << 1));
            pY5 = (cfloat*) (pY + ((0xC0000000u >> numLeadingZeros) << 1));
            pY6 = (cfloat*) (pY + ((0x60000000u >> numLeadingZeros) << 1));
            pY7 = (cfloat*) (pY + ((0xE0000000u >> numLeadingZeros) << 1));

#ifdef CL7X_HE_CFLOAT_PTR_BUG
            float *myPY0 = (float *) pY0;
            float *myPY1 = (float *) pY1;
            float *myPY2 = (float *) pY2;
            float *myPY3 = (float *) pY3;
            float *myPY4 = (float *) pY4;
            float *myPY5 = (float *) pY5;
            float *myPY6 = (float *) pY6;
            float *myPY7 = (float *) pY7;
#endif

            for (l = 0; l < numChannels; l++) {
               for (k = 0; k < numPoints >> 3; k += 8) {
                  offsetBitReverse = __bit_reverse (k) >> numLeadingZeros;

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
                  vX1 = vDiff1 - __vcrot90sp_vv (vDiff2);
                  vX2 = vSum1 - vSum2;
                  vX3 = vDiff1 + __vcrot90sp_vv (vDiff2);

                  vSum1_1  = vX_0_1 + vX_N_2_1;
                  vSum2_1  = vX_N_4_1 + vX_3N_4_1;
                  vDiff1_1 = vX_0_1 - vX_N_2_1;
                  vDiff2_1 = vX_N_4_1 - vX_3N_4_1;

                  vX0Temp = vSum1_1 + vSum2_1;
                  vX1Temp = vDiff1_1 - __vcrot90sp_vv (vDiff2_1);
                  vX2Temp = vSum1_1 - vSum2_1;
                  vX3Temp = vDiff1_1 + __vcrot90sp_vv (vDiff2_1);

                  vX0_1 = vX0Temp;
                  vX1_1 = __complex_multiply (vX1Temp, vTwX1);
                  vX2_1 = __complex_multiply (vX2Temp, vTwX2);
                  vX3_1 = __complex_multiply (vX3Temp, vTwX3);

                  vX0_2PtDft_1 = vX0 + vX0_1;
                  vX0_2PtDft_2 = vX0 - vX0_1;
                  vX1_2PtDft_1 = vX1 + vX1_1;
                  vX1_2PtDft_2 = vX1 - vX1_1;
                  vX2_2PtDft_1 = vX2 + vX2_1;
                  vX2_2PtDft_2 = vX2 - vX2_1;
                  vX3_2PtDft_1 = vX3 + vX3_1;
                  vX3_2PtDft_2 = vX3 - vX3_1;

                  __vstore_reverse_bit ((CVP) (pY0 + offsetBitReverse),
                                        vX0_2PtDft_1);
                  __vstore_reverse_bit ((CVP) (pY1 + offsetBitReverse),
                                        vX0_2PtDft_2);
                  __vstore_reverse_bit ((CVP) (pY2 + offsetBitReverse),
                                        vX1_2PtDft_1);
                  __vstore_reverse_bit ((CVP) (pY3 + offsetBitReverse),
                                        vX1_2PtDft_2);
                  __vstore_reverse_bit ((CVP) (pY4 + offsetBitReverse),
                                        vX2_2PtDft_1);
                  __vstore_reverse_bit ((CVP) (pY5 + offsetBitReverse),
                                        vX2_2PtDft_2);
                  __vstore_reverse_bit ((CVP) (pY6 + offsetBitReverse),
                                        vX3_2PtDft_1);
                  __vstore_reverse_bit ((CVP) (pY7 + offsetBitReverse),
                                        vX3_2PtDft_2);
               }

#ifdef CL7X_HE_CFLOAT_PTR_BUG
               myPY0 += (numPoints << 1);
               myPY1 += (numPoints << 1);
               myPY2 += (numPoints << 1);
               myPY3 += (numPoints << 1);
               myPY4 += (numPoints << 1);
               myPY5 += (numPoints << 1);
               myPY6 += (numPoints << 1);
               myPY7 += (numPoints << 1);

               pY0 = (cfloat*) myPY0;
               pY1 = (cfloat*) myPY1;
               pY2 = (cfloat*) myPY2;
               pY3 = (cfloat*) myPY3;
               pY4 = (cfloat*) myPY4;
               pY5 = (cfloat*) myPY5;
               pY6 = (cfloat*) myPY6;
               pY7 = (cfloat*) myPY7;

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
   }

   return (status);
}

#if (!defined(FFTLIB_REMOVE_CHECK_PARAMS) &&                                   \
     !defined(FFTLIB_FFT1DBATCHED_I32FC_C32FC_O32FC_REMOVE_CHECK_PARAMS)) ||   \
    (defined(FFTLIB_CHECK_PARAMS)) ||                                          \
    (defined(FFTLIB_FFT1DBATCHED_I32FC_C32FC_O32FC_CHECK_PARAMS))

FFTLIB_STATUS FFTLIB_fft1dBatched_i32fc_c32fc_o32fc_checkParams (
    FFTLIB_F32           *pX,
    FFTLIB_bufParams1D_t *bufParamsX,
    FFTLIB_F32           *pW,
    FFTLIB_bufParams1D_t *bufParamsW,
    FFTLIB_F32           *pY,
    FFTLIB_bufParams1D_t *bufParamsY,
    uint32_t              numPoints,
    uint32_t              numChannels,
    void                 *pBlock)
{
   FFTLIB_STATUS status = FFTLIB_SUCCESS;

   if ((pX == NULL) || (pW == NULL) || (pY == NULL) || (pBlock == NULL)) {
      status = FFTLIB_ERR_NULL_POINTER;
   }
   else if (bufParamsX->dim_x != bufParamsY->dim_x) {
      status = FFTLIB_ERR_INVALID_DIMENSION;
   }
   else if (bufParamsX->dim_x < numPoints * numChannels * 2) {
      /* In general, dim_x == numPoints*numChannels*2. However,
       * optimized kernel requires dim_x to be atleast 64*2. Hence, for
       * small values of numPoints*numChannels, dim_x could be greater
       * than numPoints*numChannels*2 */
      status = FFTLIB_ERR_INVALID_DIMENSION;
   }
   else if (bufParamsX->dim_x < 64 * 2) {
      status = FFTLIB_ERR_INVALID_DIMENSION;
   }
   else if (bufParamsW->dim_x != numPoints * 2) {
      status = FFTLIB_ERR_INVALID_DIMENSION;
   }
   else if ((bufParamsX->data_type != FFTLIB_FLOAT32) ||
            (bufParamsW->data_type != FFTLIB_FLOAT32) ||
            (bufParamsY->data_type != FFTLIB_FLOAT32)) {
      status = FFTLIB_ERR_INVALID_TYPE;
   }
   else if (((uint64_t) pX) & 0xFu) { /* pX must be 16-byte aligned for a */
      status = FFTLIB_ERR_NOT_ALIGNED_PTRS_STRIDES; /* streaming engine
                                                       configuration */
   }
   else {
      /* Check if number of pts is a power of 2 */
      uint32_t k = 0;
      while (k < 32) {
         if (numPoints & (1u << k)) {
            break;
         }
         k++;
      }
      if ((1u << k) != numPoints) {
         status = FFTLIB_ERR_INVALID_DIMENSION;
      }

      if ((numChannels != 1) && (numChannels != 2) && (numChannels != 4) &&
          (numChannels != 8) && (numChannels != 16)) {
         status = FFTLIB_ERR_INVALID_DIMENSION;
      }
   }
   return (status);
}

#endif
