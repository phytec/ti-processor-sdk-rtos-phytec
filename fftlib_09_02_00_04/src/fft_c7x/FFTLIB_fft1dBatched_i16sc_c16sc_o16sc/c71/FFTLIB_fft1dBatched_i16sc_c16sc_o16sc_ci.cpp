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
#include <c7x.h>

#define TRACE_ON         (0)
#define VECTOR_BIT_WIDTH 512

/* #if (VECTOR_BIT_WIDTH == 512) */
/* #define CSHORTX               cshort16 */
/* #define AS_CSHORTX            c7x::as_cshort_vec */
/* #define AS_SHORTX             c7x::as_short_vec */
/* #define FFT_SE0ADV_CSHORTX    __SE0ADV (cshort16); */
/* #define FFT_SE1ADV_CSHORTX    c7x::strm_eng<1, CV>::get_adv (); */
/* #define FFT_SA0ADV_CSHORTX(p) __SA0ADV (cshort16, p) */
/* #define FFT_SE_VECLEN_XELEMS  __SE_VECLEN_16ELEMS */
/* #define FFT_SA_VECLEN_XELEMS  __SA_VECLEN_16ELEMS */
/* #define FFT_UINTX             uint16 */
/* #elif (VECTOR_BIT_WIDTH == 128) */
/* #define CSHORTX               cshort4 */
/* #define AS_CSHORTX            __as_cshort4 */
/* #define AS_SHORTX             __as_short8 */
/* #define FFT_SE0ADV_CSHORTX    __SE0ADV (cshort4); */
/* #define FFT_SE1ADV_CSHORTX    __SE1ADV (cshort4); */
/* #define FFT_SA0ADV_CSHORTX(p) __SA0ADV (cshort4, p) */
/* #define FFT_SE_VECLEN_XELEMS  __SE_VECLEN_4ELEMS */
/* #define FFT_SA_VECLEN_XELEMS  __SA_VECLEN_4ELEMS */
/* #define FFT_UINTX             uint4 */
/* #endif */

#if TRACE_ON
#include "../../../common/printv.h"
#include <stdio.h>
#endif

// CODE_SECTION(FFTLIB_fft1dBatched_i16sc_c16sc_o16sc, ".text:optimized")
// CODE_SECTION(FFTLIB_fft1dBatched_i16sc_c16sc_o16sc_core, ".text:optimized")
// CODE_SECTION(FFTLIB_fft1dBatched_i16sc_c16sc_o16sc_checkParams,
// ".text:optimized")

#define NUMPT32_ONLY 0

#define LOOP2THEN4  1 /* LOOP2 fold in LOOP4 */
#define ALIGNED_4PT 1 /* optimization for LOOP4 */
#define ALIGNED_8PT 1 /* optimization for LOOP6 & LOOP7 */

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
      uint32_t         numPointsPerDft;
      uint32_t         seCnt1, seCnt2, seCnt3, seCnt4;
      uint32_t         seCnt6, seCnt7, seCnt8, seCnt9, seCnt10;
      __SE_TEMPLATE_v1 se0_param;
      __SE_TEMPLATE_v1 se1_param;
      __SA_TEMPLATE_v1 sa0_param;

      numPointsPerDft = numPoints;
      seCnt1          = numPoints >> 2;
      seCnt2          = numPoints >> 6;
      seCnt3          = 1;
      seCnt4          = numPoints >> 4;
      seCnt6          = seCnt3 * numChannels;
      seCnt7 =
          (numPoints * numChannels >> 6) > 1 ? numPoints * numChannels >> 6 : 1;
      seCnt8 = numPoints * numChannels > 64 ? numPoints * numChannels : 64;
      seCnt9 =
          (numPoints * numChannels >> 7) > 1 ? numPoints * numChannels >> 7 : 1;
      seCnt10 = numPoints * numChannels > 128 ? numPoints * numChannels : 128;

      /* . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
       */

      /* LOOP1 */

      se0_param         = __gen_SE_TEMPLATE_v1 ();
      se0_param.ICNT0   = 16; /* 16-point vectors processed in one shot */
      se0_param.ICNT1   = 4;
      se0_param.DIM1    = seCnt1; /* 4 quarters(Offsets: 0, N/4, N/2, 3N/4) */
      se0_param.ICNT2   = seCnt2; /* Number of 8-point fetches within each  */
      se0_param.DIM2    = 16;     /* quarter                                */
      se0_param.ICNT3   = seCnt6; /* Number of DFT's  */
      se0_param.DIM3    = numPointsPerDft;
      se0_param.ELETYPE = __SE_ELETYPE_16BIT_CMPLX_SWAP;
      se0_param.VECLEN  = c7x::se_veclen<c7x::cshort_vec>::value;
      se0_param.DIMFMT  = __SE_DIMFMT_4D;
      *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_LOOP1_PARAM_OFFSET)) =
          se0_param;

      se1_param         = __gen_SE_TEMPLATE_v1 ();
      se1_param.ICNT0   = 16; /* 16-point vectors processed in one shot */
      se1_param.ICNT1   = 3;
      se1_param.DIM1    = seCnt1; /* Twiddle factors for x1, x2 and x3 */
      se1_param.ICNT2   = seCnt2; /* Number of 8-point fetches within each  */
      se1_param.DIM2    = 16;     /* quarter                                */
      se1_param.ICNT3   = seCnt6; /* Number of DFT's */
      se1_param.DIM3    = 0;
      se1_param.ELETYPE = __SE_ELETYPE_16BIT_CMPLX_SWAP;
      se1_param.VECLEN  = c7x::se_veclen<c7x::cshort_vec>::value;
      se1_param.DIMFMT  = __SE_DIMFMT_4D;
      *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_TWID_PARAM_OFFSET)) =
          se1_param;

      sa0_param       = __gen_SA_TEMPLATE_v1 ();
      sa0_param.ICNT0 = 16;
      sa0_param.ICNT1 = 4;
      sa0_param.DIM1  = seCnt1; /* Save to each of the 4 quarters */
      sa0_param.ICNT2 = seCnt2; /* Number of 8-point stores within each   */
      sa0_param.DIM2  = 16;     /* quarter                                */
      sa0_param.ICNT3 = seCnt6;
      sa0_param.DIM3  = numPointsPerDft; /* Number of DFT's */

      sa0_param.VECLEN = c7x::sa_veclen<c7x::cshort_vec>::value;
      sa0_param.DIMFMT = __SA_DIMFMT_4D;
      *((__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA_LOOP1_PARAM_OFFSET)) =
          sa0_param;

      /* . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
       */

      /* LOOP2 */

      se0_param       = __gen_SE_TEMPLATE_v1 ();
      se0_param.ICNT0 = 16;
      se0_param.ICNT1 = 4;
      se0_param.DIM1  = 16; /* Process four 16-point DFTs in one shot */
      se0_param.ICNT2 = seCnt7;
      se0_param.DIM2  = 64;

      se0_param.ELETYPE   = __SE_ELETYPE_16BIT_CMPLX_SWAP;
      se0_param.TRANSPOSE = __SE_TRANSPOSE_128BIT;
      se0_param.VECLEN    = c7x::se_veclen<c7x::cshort_vec>::value;
      se0_param.DIMFMT    = __SE_DIMFMT_3D;
      *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_LOOP2_PARAM_OFFSET)) =
          se0_param;

      sa0_param       = __gen_SA_TEMPLATE_v1 ();
      sa0_param.ICNT0 = seCnt8;

      sa0_param.VECLEN = c7x::sa_veclen<c7x::cshort_vec>::value;
      sa0_param.DIMFMT = __SA_DIMFMT_1D;
      *((__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA_LOOP2_PARAM_OFFSET)) =
          sa0_param;

      /* . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
       */

      /* LOOP3 */

      se0_param       = __gen_SE_TEMPLATE_v1 ();
      se0_param.ICNT0 = 32 / 2; /* using both SEs */
      se0_param.ICNT1 = 2;
      se0_param.DIM1  = 32; /* Process two 16-point DFTs in one shot */
      se0_param.ICNT2 = seCnt7;
      se0_param.DIM2  = 64; /* using both SEs */

      se0_param.ELETYPE = __SE_ELETYPE_16BIT_CMPLX_SWAP;
      se0_param.TRANSPOSE =
          __SE_TRANSPOSE_256BIT; /* Using 256BIT transpose required  */
                                 /* 16-byte alignment on pX          */
      se0_param.VECLEN = c7x::se_veclen<c7x::cshort_vec>::value;
      se0_param.DIMFMT = __SE_DIMFMT_3D;
      *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_LOOP3_PARAM_OFFSET)) =
          se0_param;

      sa0_param       = __gen_SA_TEMPLATE_v1 ();
      sa0_param.ICNT0 = seCnt8;

      sa0_param.VECLEN = c7x::sa_veclen<c7x::cshort_vec>::value;
      sa0_param.DIMFMT = __SA_DIMFMT_1D;
      *((__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA_LOOP3_PARAM_OFFSET)) =
          sa0_param;

      /* . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
       */

      /* LOOP4 */

      se0_param = __gen_SE_TEMPLATE_v1 ();
#if (ALIGNED_4PT)
      se0_param.ICNT0     = 16;
      se0_param.ICNT1     = 4;
      se0_param.DIM1      = 16;
      se0_param.TRANSPOSE = __SE_TRANSPOSE_OFF;
#else  /* ALIGNED_4PT */
      /* The following SE configuration may cause sub-optimal
       * tile in SE because second row of tile starts in the
       * middle of first row                                  */
      se0_param.ICNT0     = 4; /* Fetch consecutive four points for DFT  */
      se0_param.ICNT1     = 16;
      se0_param.DIM1      = 4;
      se0_flags.TRANSPOSE = __SE_TRANSPOSE_32BIT;
#endif /* ALIGNED_4PT */
      se0_param.ICNT2 = seCnt7;
      se0_param.DIM2  = 64;

      se0_param.ELETYPE = __SE_ELETYPE_16BIT_CMPLX_SWAP;
      se0_param.VECLEN  = c7x::se_veclen<c7x::cshort_vec>::value;
      se0_param.DIMFMT  = __SE_DIMFMT_3D;
      *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_LOOP4_PARAM_OFFSET)) =
          se0_param;

      sa0_param       = __gen_SA_TEMPLATE_v1 ();
      sa0_param.ICNT0 = seCnt8; /* Input buffer must be at least 64
                                 * elements long even though
                                 * numPoints*numChannels = 16 */

      sa0_param.VECLEN = c7x::sa_veclen<c7x::cshort_vec>::value;
      sa0_param.DIMFMT = __SA_DIMFMT_1D;
      *((__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA_LOOP4_PARAM_OFFSET)) =
          sa0_param;

      /* . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
       */

      /* LOOP5 */

      se0_param       = __gen_SE_TEMPLATE_v1 ();
      se0_param.ICNT0 = seCnt4; /* Fetch consecutive four points for DFT  */
      se0_param.ICNT1 = 16;
      se0_param.DIM1 =
          /* Fetch 16 points separated by           */ seCnt4; /* (numPoints >>
                                                                  4). This fetch
                                                                  pattern   */
      /* can be used for bit reversal           */
      se0_param.TRANSPOSE = __SE_TRANSPOSE_32BIT;

      se0_param.ICNT2 = numChannels;
      se0_param.DIM2  = numPoints;

      se0_param.ELETYPE = __SE_ELETYPE_16BIT_CMPLX_SWAP;
      se0_param.VECLEN  = c7x::se_veclen<c7x::cshort_vec>::value;
      se0_param.DIMFMT  = __SE_DIMFMT_3D;
      *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_LOOP5_PARAM_OFFSET)) =
          se0_param;

      /* . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
       */

      /* LOOP6 */

      se0_param = __gen_SE_TEMPLATE_v1 ();
#if (ALIGNED_8PT)
      se0_param.ICNT0 = 16;
      se0_param.ICNT1 = 8;
      se0_param.DIM1  = 16;

      se0_param.TRANSPOSE = __SE_TRANSPOSE_64BIT;
#else  /* ALIGNED_8PT */
      /* The following SE configuration may cause sub-optimal
       * tile in SE because second row of tile starts in the
       * middle of first row                                  */
      se0_param.ICNT0 = 8;
      se0_param.ICNT1 = 16;
      se0_param.DIM1  = 8;

      se0_param.TRANSPOSE = __SE_TRANSPOSE_32BIT;
#endif /* ALIGNED_8PT */
      se0_param.ICNT2 = seCnt9;
      se0_param.DIM2  = 128;

      se0_param.ELETYPE = __SE_ELETYPE_16BIT_CMPLX_SWAP;
      se0_param.VECLEN  = c7x::se_veclen<c7x::cshort_vec>::value;
      se0_param.DIMFMT  = __SE_DIMFMT_3D;
      *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_LOOP6_PARAM_OFFSET)) =
          se0_param;

      sa0_param       = __gen_SA_TEMPLATE_v1 ();
      sa0_param.ICNT0 = seCnt10; /* Input buffer must be at least 128
                                  * elements long even though
                                  * numPoints*numChannels = 32 */

      sa0_param.VECLEN = c7x::sa_veclen<c7x::cshort_vec>::value;
      sa0_param.DIMFMT = __SA_DIMFMT_1D;
      *((__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA_LOOP6_PARAM_OFFSET)) =
          sa0_param;

      /* . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
       */

      /* LOOP7 */

      se0_param = __gen_SE_TEMPLATE_v1 ();

      /* for numPoints == 128, seCnt4 (numPoints >> 4) == 8 -> reduces SE
       * throughput */
      se0_param.ICNT0 = seCnt4;
      se0_param.ICNT1 = 16;
      se0_param.DIM1  = seCnt4; /* (numPoints >>4). This fetch pattern can be
                                   used for bit reversal */

      se0_param.TRANSPOSE = __SE_TRANSPOSE_32BIT;

      se0_param.ICNT2 = numChannels;
      se0_param.DIM2  = numPoints;

      se0_param.ELETYPE = __SE_ELETYPE_16BIT_CMPLX_SWAP;
      se0_param.VECLEN  = c7x::se_veclen<c7x::cshort_vec>::value;
      se0_param.DIMFMT  = __SE_DIMFMT_3D;
      *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_LOOP7_PARAM_OFFSET)) =
          se0_param;

      /* . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
       */
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
   cshort* restrict pXLocal;
   cshort* restrict pYLocal;
   cshort* restrict pWLocal;
   cshort* restrict pY0;
   cshort* restrict pY1;
   cshort* restrict pY2;
   cshort* restrict pY3;
   cshort* restrict pY4;
   cshort* restrict pY5;
   cshort* restrict pY6;
   cshort* restrict pY7;

   typedef typename c7x::cshort_vec             CV;
   typedef CV* CVP;

   typedef typename c7x::short_vec             V;
   typedef V* VP;

   CV            vX_0, vX_N_4, vX_N_2, vX_3N_4;
   CV            vSum1, vSum2, vDiff1, vDiff2;
   CV            vTwX1, vTwX2, vTwX3;
   CV            vX0Temp, vX1Temp, vX2Temp, vX3Temp;
   CV            vX0, vX1, vX2, vX3;
   uint32_t      stageIndex;
   c7x::uint_vec vShift;

   /* #ifdef FFTLIB_CHECK_PARAMS */
   /*     status = FFTLIB_fft1dBatched16x16_checkParams(pX, bufParamsX, */
   /*                                                                pW,
    * bufParamsW, */
   /*                                                                pY,
    * bufParamsY, */
   /*                                                                pShift,
    * bufParamsShift, */
   /*                                                                numPoints,
    * numChannels, */
   /*                                                                pBlock); */
   /*     if( status == FFTLIB_SUCCESS ) */
   /* #endif */
   {
      /* uint32_t loop_profile[4][3] = {0}; */
      uint32_t loop = 0;
      /* uint64_t tsc; */

      numPointsPerDft = numPoints;
      stageIndex      = 0;

      // * SE_LOOP1 - 4D - { 16; 4, numPoints >> 2; numPoints >> 6, 16;
      // 1*numChannels, numPointsPerDft}
      // * SE_TWID  - 4D - { 16; 3, numPoints >> 2; numPoints >> 6, 16;
      // 1*numChannels, 0}
      // * SA_LOOP1 - 4D - { 16; 4, numPoints >> 2; numPoints >> 6, 16;
      // 1*numChannels, numPointsPerDft}

      se0_param =
          *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_LOOP1_PARAM_OFFSET));
      se1_param =
          *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_TWID_PARAM_OFFSET));
      sa0_param =
          *((__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA_LOOP1_PARAM_OFFSET));
      seCnt1 = numPointsPerDft >> 2;
      seCnt2 = numPointsPerDft >> 6;
      seCnt3 = 1;

      pXLocal = (cshort*) pX;
      pWLocal = (cshort*) pW;
      pYLocal = (cshort*) pY;

#if (!NUMPT32_ONLY)
      while (numPointsPerDft >= 64) {
         seCnt6          = seCnt3 * numChannels;
         se0_param.ICNT1 = 4;
         se0_param.DIM1  = /* 4 quarters(Offsets: 0, N/4, N/2, 3N/4) */ seCnt1;
         se0_param.ICNT2 = seCnt2;
         se0_param.DIM2 =
             /* Number of 8-point fetches within */ 16; /* each quarter */
         se0_param.ICNT3 = seCnt6;
         se0_param.DIM3  = /* Number of DFT's */ numPointsPerDft;
         __SE0_OPEN ((void *) pXLocal, se0_param);

         se1_param.ICNT1 = 3;
         se1_param.DIM1  = /* Twiddle factors for x1, x2 and x3      */ seCnt1;
         se1_param.ICNT2 = /* Number of 8-point fetches within each  */ seCnt2;
         se1_param.DIM2  = /* quarter                                */ 16;
         se1_param.ICNT3 = /* Number of DFT's */ seCnt6;
         se1_param.DIM3  = 0;
         __SE1_OPEN ((void *) pWLocal, se1_param);

         sa0_param.ICNT1 = /* Save to each of the 4 quarters */ 4;
         sa0_param.DIM1  = seCnt1;
         sa0_param.ICNT2 = seCnt2;
         sa0_param.DIM2 =
             /* Number of 8-point stores within */ /* each quarter */ 16;
         sa0_param.ICNT3 = seCnt6;
         sa0_param.DIM3  = /* Number of DFT's */ numPointsPerDft;
         __SA0_OPEN (sa0_param);

         /* Obtain the right shift value for the current stage */
         vShift = __vload_dup ((uint32_t *) (pShift + stageIndex));

         /* tsc = __TSC; */

         for (k = 0; k < numPoints * numChannels; k += 64) {

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

            /* *addr = c7x::as_cshort_vec ( */
            /*     __shift_right_round (c7x::as_short_vec (vX0), vShift)); */
            /* *addr = c7x::as_cshort_vec ( */
            /*     __shift_right_round (c7x::as_short_vec (vX2), vShift)); */
            /* *addr = c7x::as_cshort_vec ( */
            /*     __shift_right_round (c7x::as_short_vec (vX1), vShift)); */
            /* *addr = c7x::as_cshort_vec ( */
            /*     __shift_right_round (c7x::as_short_vec (vX3), vShift)); */

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

         /* loop_profile[loop][0] = (uint32_t) (__TSC - tsc); */
         /* loop_profile[loop][1] = numPointsPerDft; */
         /* loop_profile[loop][2] = 1; // LOOP1 */
         loop++;

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
#endif /* NUMPT32_ONLY */

      vShift = __vload_dup ((uint32_t *) (pShift + stageIndex));
#if (!NUMPT32_ONLY)
      if (numPointsPerDft == 16) {
         // clang-format off
#if __C7X_HOSTEM__
                     static const
                        c7x::uchar_vec vXPermCtrl  = c7x::uchar_vec(0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                            0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                                            0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                                            0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
                                            0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
                                            0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
                                            0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
                                            0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F);

#else
            static const
               c7x::uchar_vec vXPermCtrl  = (c7x::uchar_vec)(0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                            0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                                            0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                                            0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
                                            0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
                                            0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
                                            0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
                                            0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F);
#endif
         // clang-format on

         c7x::uchar_vec vX0X2_01, vX1X3_01, vX0X2_23, vX1X3_23;
         c7x::uchar_vec vXPermOut;

         typedef typename c7x::cshort_qvec             CQV;
         typedef typename c7x::cshort_hvec             CHV;
         typedef CQV* CQVP;

         CQV vTwX1CQV, vTwX2CQV, vTwX3CQV;
         CHV vTwX1CHV, vTwX2CHV, vTwX3CHV;

#if (LOOP2THEN4)
         // clang-format off
#if __C7X_HOSTEM__
                     static const
                        c7x::uchar_vec vXPermCtrl4 = c7x::uchar_vec(0x00, 0x01, 0x02, 0x03, 0x08, 0x09, 0x0A, 0x0B,
                                                0x04, 0x05, 0x06, 0x07, 0x0C, 0x0D, 0x0E, 0x0F,
                                                0x10, 0x11, 0x12, 0x13, 0x18, 0x19, 0x1A, 0x1B,
                                                0x14, 0x15, 0x16, 0x17, 0x1C, 0x1D, 0x1E, 0x1F,
                                                0x20, 0x21, 0x22, 0x23, 0x28, 0x29, 0x2A, 0x2B,
                                                0x24, 0x25, 0x26, 0x27, 0x2C, 0x2D, 0x2E, 0x2F,
                                                0x30, 0x31, 0x32, 0x33, 0x38, 0x39, 0x3A, 0x3B,
                                                0x34, 0x35, 0x36, 0x37, 0x3C, 0x3D, 0x3E, 0x3F);

#else
            static const
                c7x::uchar_vec vXPermCtrl4 = (c7x::uchar_vec)(0x00, 0x01, 0x02, 0x03, 0x08, 0x09, 0x0A, 0x0B,
                                                0x04, 0x05, 0x06, 0x07, 0x0C, 0x0D, 0x0E, 0x0F,
                                                0x10, 0x11, 0x12, 0x13, 0x18, 0x19, 0x1A, 0x1B,
                                                0x14, 0x15, 0x16, 0x17, 0x1C, 0x1D, 0x1E, 0x1F,
                                                0x20, 0x21, 0x22, 0x23, 0x28, 0x29, 0x2A, 0x2B,
                                                0x24, 0x25, 0x26, 0x27, 0x2C, 0x2D, 0x2E, 0x2F,
                                                0x30, 0x31, 0x32, 0x33, 0x38, 0x39, 0x3A, 0x3B,
                                                0x34, 0x35, 0x36, 0x37, 0x3C, 0x3D, 0x3E, 0x3F);
#endif
         // clang-format on
         c7x::uchar_vec vX0X1_01, vX2X3_01, vX0X1_23, vX2X3_23;

// clang-format off
         #if __C7X_HOSTEM__
            static const
               c7x::uchar_vec vXPermCtrl2  = c7x::uchar_vec(
                    0x00, 0x01, 0x02, 0x03, 0x20, 0x21, 0x22, 0x23,
                    0x04, 0x05, 0x06, 0x07, 0x24, 0x25, 0x26, 0x27,
                    0x08, 0x09, 0x0A, 0x0B, 0x28, 0x29, 0x2A, 0x2B,
                    0x0C, 0x0D, 0x0E, 0x0F, 0x2C, 0x2D, 0x2E, 0x2F,
                    0x10, 0x11, 0x12, 0x13, 0x30, 0x31, 0x32, 0x33,
                    0x14, 0x15, 0x16, 0x17, 0x34, 0x35, 0x36, 0x37,
                    0x18, 0x19, 0x1A, 0x1B, 0x38, 0x39, 0x3A, 0x3B,
                    0x1C, 0x1D, 0x1E, 0x1F, 0x3C, 0x3D, 0x3E, 0x3F);
            #else
            static const
                c7x::uchar_vec vXPermCtrl2  = (c7x::uchar_vec)(
                    0x00, 0x01, 0x02, 0x03, 0x20, 0x21, 0x22, 0x23,
                    0x04, 0x05, 0x06, 0x07, 0x24, 0x25, 0x26, 0x27,
                    0x08, 0x09, 0x0A, 0x0B, 0x28, 0x29, 0x2A, 0x2B,
                    0x0C, 0x0D, 0x0E, 0x0F, 0x2C, 0x2D, 0x2E, 0x2F,
                    0x10, 0x11, 0x12, 0x13, 0x30, 0x31, 0x32, 0x33,
                    0x14, 0x15, 0x16, 0x17, 0x34, 0x35, 0x36, 0x37,
                    0x18, 0x19, 0x1A, 0x1B, 0x38, 0x39, 0x3A, 0x3B,
                    0x1C, 0x1D, 0x1E, 0x1F, 0x3C, 0x3D, 0x3E, 0x3F);
#endif

#define PACK2HIGH_INT16( V1, V2) __vpermoow_yvvv( vXPermCtrl2, c7x::as_uchar_vec( V1), c7x::as_uchar_vec( V2))

            #if __C7X_HOSTEM__
                        static const
                           c7x::uchar_vec vXPermCtrl3  = c7x::uchar_vec(
                    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
                    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                    0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
                    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
                    0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
                    0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F);
            #else
                        static const
                c7x::uchar_vec vXPermCtrl3  = (c7x::uchar_vec)(
                    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
                    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                    0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
                    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
                    0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
                    0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F);
#endif
         // clang-format on

#define PACK2LOW__LONG8(V1, V2)                                                \
   __vpermeed_yvvv (vXPermCtrl3, c7x::as_uchar_vec (V1), c7x::as_uchar_vec (V2))
#define PACK2HIGH_LONG8(V1, V2)                                                \
   __vpermood_yvvv (vXPermCtrl3, c7x::as_uchar_vec (V1), c7x::as_uchar_vec (V2))

         uint16 vShift4 = __vload_dup (
             (uint32_t *) (pShift + stageIndex + 1)); /* shift for LOOP4 */

#endif /* LOOP2THEN4 */

         // * SE_LOOP2 - 3D * TRANSPOSE_128BIT - { 16; 4, 16;
         // numPoints*numChannels >> 6, 64}
         // * SA_LOOP2 - 1D - { numPoints*numChannels}

         se0_param = *(
             (__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_LOOP2_PARAM_OFFSET));
         __SE0_OPEN ((void *) pXLocal, se0_param);

         sa0_param = *(
             (__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA_LOOP2_PARAM_OFFSET));
         __SA0_OPEN (sa0_param);

#if __C7X_HOSTEM__
         vTwX1CQV = *((CQVP) pWLocal);
         vTwX1CHV = CHV (vTwX1CQV, vTwX1CQV);
         vTwX1    = CV (vTwX1CHV, vTwX1CHV);

         vTwX2CQV = *((CQVP) (pWLocal + 4));
         vTwX2CHV = CHV (vTwX2CQV, vTwX2CQV);
         vTwX2    = CV (vTwX2CHV, vTwX2CHV);

         vTwX3CQV = *((CQVP) (pWLocal + 8));
         vTwX3CHV = CHV (vTwX3CQV, vTwX3CQV);
         vTwX3    = CV (vTwX3CHV, vTwX3CHV);
#else
         vTwX1CQV = *((CQVP) pWLocal);
         vTwX1CHV = (CHV) (vTwX1CQV, vTwX1CQV);
         vTwX1    = (CV) (vTwX1CHV, vTwX1CHV);

         vTwX2CQV = *((CQVP) (pWLocal + 4));
         vTwX2CHV = (CHV) (vTwX2CQV, vTwX2CQV);
         vTwX2    = (CV) (vTwX2CHV, vTwX2CHV);

         vTwX3CQV = *((CQVP) (pWLocal + 8));
         vTwX3CHV = (CHV) (vTwX3CQV, vTwX3CQV);
         vTwX3    = (CV) (vTwX3CHV, vTwX3CHV);
#endif
         /* tsc = __TSC; */

#if (LOOP2THEN4)
         if (numPoints == 16)
            for (k = 0; k < numPoints * numChannels; k += 64) {
               vX_0    = c7x::strm_eng<0, CV>::get_adv ();
               vX_N_4  = c7x::strm_eng<0, CV>::get_adv ();
               vX_N_2  = c7x::strm_eng<0, CV>::get_adv ();
               vX_3N_4 = c7x::strm_eng<0, CV>::get_adv ();

               vSum1  = vX_0 + vX_N_2;
               vSum2  = vX_N_4 + vX_3N_4;
               vDiff1 = vX_0 - vX_N_2;
               vDiff2 = vX_N_4 - vX_3N_4;

               vX0Temp = vSum1 + vSum2;
               vX1Temp = vDiff1 - __vcrot90h_vv (vDiff2);
               vX2Temp = vSum1 - vSum2;
               vX3Temp = vDiff1 + __vcrot90h_vv (vDiff2);

               vX0 =
                   vX0Temp; /* g0(0..3) | g0'(0..3) | g0"(0..3) | g0"'(0..3) */
               vX1 = __cmpy_fx (
                   vX1Temp,
                   vTwX1); /* g1(0..3) | g1'(0..3) | g1"(0..3) | g1"'(0..3) */
               vX2 = __cmpy_fx (
                   vX2Temp,
                   vTwX2); /* g2(0..3) | g2'(0..3) | g2"(0..3) | g2"'(0..3) */
               vX3 = __cmpy_fx (
                   vX3Temp,
                   vTwX3); /* g3(0..3) | g3'(0..3) | g3"(0..3) | g3"'(0..3) */

               vX0X2_01 = c7x::as_uchar_vec (
                   __pack_low (c7x::as_int_vec (vX2),
                               c7x::as_int_vec (
                                   vX0))); /* g0(0), g2(0), g0(2), g1(2) |
                                         g0'(0), g2'(0), g0'(2), g2'(2) | ... */
               vX1X3_01 = c7x::as_uchar_vec (__pack_low (
                   c7x::as_int_vec (vX3),
                   c7x::as_int_vec (
                       vX1))); /* g1(0), g3(0), g2(2), g3(2) | ... */
               vX0X2_23 = c7x::as_uchar_vec (PACK2HIGH_INT16 (
                   c7x::as_int_vec (vX2),
                   c7x::as_int_vec (
                       vX0))); /* g0(1), g2(1), g0(3), g1(3) | ... */
               vX1X3_23 = c7x::as_uchar_vec (PACK2HIGH_INT16 (
                   c7x::as_int_vec (vX3),
                   c7x::as_int_vec (
                       vX1))); /* g1(1), g3(1), g2(3), g3(3) | ... */

               vXPermOut = __vpermeed_yvvv (
                   vXPermCtrl3, vX1X3_01,
                   vX0X2_01); /* g0(0), g2(0), g1(0), g3(0) | g0'(0), g2'(0),
                                 g1'(0), g3'(0) | g0"(0), g2"(0), g1"(0), g3"(0)
                                 | ... */
               vX_0 = c7x::as_cshort_vec (
                   __shift_right_round (c7x::as_short_vec (vXPermOut), vShift));
               vXPermOut = __vpermeed_yvvv (
                   vXPermCtrl3, vX1X3_23,
                   vX0X2_23); /* g0(1), g2(1), g1(1), g3(1) | g0'(1), g2'(1),
                                 g1'(1), g3'(1) | ... */
               vX_N_4 = c7x::as_cshort_vec (
                   __shift_right_round (c7x::as_short_vec (vXPermOut), vShift));
               vXPermOut = __vpermood_yvvv (
                   vXPermCtrl3, vX1X3_01,
                   vX0X2_01); /* g0(2), g2(2), g1(2), g3(2) | g0'(2), g2'(2),
                                 g1'(2), g3'(2) | ... */
               vX_N_2 = c7x::as_cshort_vec (
                   __shift_right_round (c7x::as_short_vec (vXPermOut), vShift));
               vXPermOut = __vpermood_yvvv (
                   vXPermCtrl3, vX1X3_23,
                   vX0X2_23); /* g0(3), g2(3), g1(3), g3(3) | g0'(3), g2'(3),
                                 g1'(3), g3'(3) | ... */
               vX_3N_4 = c7x::as_cshort_vec (
                   __shift_right_round (c7x::as_short_vec (vXPermOut), vShift));

               vSum1  = vX_0 + vX_N_2;
               vSum2  = vX_N_4 + vX_3N_4;
               vDiff1 = vX_0 - vX_N_2;
               vDiff2 = vX_N_4 - vX_3N_4;

               vX0 = vSum1 + vSum2;
               vX1 = vDiff1 - __vcrot90h_vv (vDiff2);
               vX2 = vSum1 - vSum2;
               vX3 = vDiff1 + __vcrot90h_vv (vDiff2);

#if __C7X_HOSTEM__
               vX0X1_01 = c7x::uchar_vec (c7x::as_uchar_vec (vX0).lo(),
                                          c7x::as_uchar_vec (vX1).lo());
               vX2X3_01 = c7x::uchar_vec (c7x::as_uchar_vec (vX2).lo(),
                                          c7x::as_uchar_vec (vX3).lo());
               vX0X1_23 = c7x::uchar_vec (c7x::as_uchar_vec (vX0).hi(),
                                          c7x::as_uchar_vec (vX1).hi());
               vX2X3_23 = c7x::uchar_vec (c7x::as_uchar_vec (vX2).hi(),
                                          c7x::as_uchar_vec (vX3).hi());
#else
               vX0X1_01 = (c7x::uchar_vec) (c7x::as_uchar_vec (vX0).lo(),
                                            c7x::as_uchar_vec (vX1).lo());
               vX2X3_01 = (c7x::uchar_vec) (c7x::as_uchar_vec (vX2).lo(),
                                            c7x::as_uchar_vec (vX3).lo());
               vX0X1_23 = (c7x::uchar_vec) (c7x::as_uchar_vec (vX0).hi(),
                                            c7x::as_uchar_vec (vX1).hi());
               vX2X3_23 = (c7x::uchar_vec) (c7x::as_uchar_vec (vX2).hi(),
                                            c7x::as_uchar_vec (vX3).hi());

#endif
               /* vXPermOut = __vpermeeq_yvvv (vXPermCtrl4, vX2X3_01, vX0X1_01);
                */
               /* *addr = c7x::as_cshort_vec ( */
               /*     __shift_right_round (c7x::as_short_vec (vXPermOut),
                * vShift4));
                */
               /* vXPermOut = __vpermooq_yvvv (vXPermCtrl4, vX2X3_01, vX0X1_01);
                */
               /* *addr = c7x::as_cshort_vec ( */
               /*     __shift_right_round (c7x::as_short_vec (vXPermOut),
                * vShift4));
                */
               /* vXPermOut = __vpermeeq_yvvv (vXPermCtrl4, vX2X3_23, vX0X1_23);
                */
               /* *addr = c7x::as_cshort_vec ( */
               /*     __shift_right_round (c7x::as_short_vec (vXPermOut),
                * vShift4));
                */
               /* vXPermOut = __vpermooq_yvvv (vXPermCtrl4, vX2X3_23, vX0X1_23);
                */
               /* *addr = c7x::as_cshort_vec ( */
               /*     __shift_right_round (c7x::as_short_vec (vXPermOut),
                * vShift4));
                */
               __vpred tmp;
               CVP     addr;
               tmp       = c7x::strm_agen<0, CV>::get_vpred ();
               addr      = c7x::strm_agen<0, CV>::get_adv (pYLocal);
               vXPermOut = __vpermeeq_yvvv (vXPermCtrl4, vX2X3_01, vX0X1_01);
               __vstore_pred (tmp, addr,
                              c7x::as_cshort_vec (__shift_right_round (
                                  c7x::as_short_vec (vXPermOut), vShift4)));
               vXPermOut = __vpermooq_yvvv (vXPermCtrl4, vX2X3_01, vX0X1_01);
               tmp       = c7x::strm_agen<0, CV>::get_vpred ();
               addr      = c7x::strm_agen<0, CV>::get_adv (pYLocal);
               __vstore_pred (tmp, addr,
                              c7x::as_cshort_vec (__shift_right_round (
                                  c7x::as_short_vec (vXPermOut), vShift4)));
               vXPermOut = __vpermeeq_yvvv (vXPermCtrl4, vX2X3_23, vX0X1_23);
               tmp       = c7x::strm_agen<0, CV>::get_vpred ();
               addr      = c7x::strm_agen<0, CV>::get_adv (pYLocal);
               __vstore_pred (tmp, addr,
                              c7x::as_cshort_vec (__shift_right_round (
                                  c7x::as_short_vec (vXPermOut), vShift4)));
               vXPermOut = __vpermooq_yvvv (vXPermCtrl4, vX2X3_23, vX0X1_23);
               tmp       = c7x::strm_agen<0, CV>::get_vpred ();
               addr      = c7x::strm_agen<0, CV>::get_adv (pYLocal);
               __vstore_pred (tmp, addr,
                              c7x::as_cshort_vec (__shift_right_round (
                                  c7x::as_short_vec (vXPermOut), vShift4)));
            }
         else
#endif /* LOOP2THEN4 */
            for (k = 0; k < numPoints * numChannels; k += 64) {
               vX_0    = c7x::strm_eng<0, CV>::get_adv ();
               vX_N_4  = c7x::strm_eng<0, CV>::get_adv ();
               vX_N_2  = c7x::strm_eng<0, CV>::get_adv ();
               vX_3N_4 = c7x::strm_eng<0, CV>::get_adv ();

               vSum1  = vX_0 + vX_N_2;
               vSum2  = vX_N_4 + vX_3N_4;
               vDiff1 = vX_0 - vX_N_2;
               vDiff2 = vX_N_4 - vX_3N_4;

               vX0Temp = vSum1 + vSum2;
               vX1Temp = vDiff1 - __vcrot90h_vv (vDiff2);
               vX2Temp = vSum1 - vSum2;
               vX3Temp = vDiff1 + __vcrot90h_vv (vDiff2);

               vX0 =
                   vX0Temp; /* g0(0..3) | g0'(0..3) | g0"(0..3) | g0"'(0..3) */
               vX1 = __cmpy_fx (
                   vX1Temp,
                   vTwX1); /* g1(0..3) | g1'(0..3) | g1"(0..3) | g1"'(0..3) */
               vX2 = __cmpy_fx (
                   vX2Temp,
                   vTwX2); /* g2(0..3) | g2'(0..3) | g2"(0..3) | g2"'(0..3) */
               vX3 = __cmpy_fx (
                   vX3Temp,
                   vTwX3); /* g3(0..3) | g3'(0..3) | g3"(0..3) | g3"'(0..3) */

#if __C7X_HOSTEM__
               vX0X2_01 = c7x::uchar_vec (c7x::as_uchar_vec (vX0).lo(),
                                          c7x::as_uchar_vec (vX2).lo());
               vX1X3_01 = c7x::uchar_vec (c7x::as_uchar_vec (vX1).lo(),
                                          c7x::as_uchar_vec (vX3).lo());
               vX0X2_23 = c7x::uchar_vec (c7x::as_uchar_vec (vX0).hi(),
                                          c7x::as_uchar_vec (vX2).hi());
               vX1X3_23 = c7x::uchar_vec (c7x::as_uchar_vec (vX1).hi(),
                                          c7x::as_uchar_vec (vX3).hi());

#else
            vX0X2_01 = (c7x::uchar_vec) (c7x::as_uchar_vec (vX0).lo(),
                                         c7x::as_uchar_vec (vX2).lo());
            vX1X3_01 = (c7x::uchar_vec) (c7x::as_uchar_vec (vX1).lo(),
                                         c7x::as_uchar_vec (vX3).lo());
            vX0X2_23 = (c7x::uchar_vec) (c7x::as_uchar_vec (vX0).hi(),
                                         c7x::as_uchar_vec (vX2).hi());
            vX1X3_23 = (c7x::uchar_vec) (c7x::as_uchar_vec (vX1).hi(),
                                         c7x::as_uchar_vec (vX3).hi());

#endif
               /* vXPermOut = __vpermeeq_yvvv ( */
               /*     vXPermCtrl, */
               /*     vX1X3_01, */
               /*     vX0X2_01); /\* g0(0..3) | g2(0..3) | g1(0..3) | g3(0..3) -
                * 4x */
               /*                   4-pt FFTs (1st set) *\/ */

               /* *addr = c7x::as_cshort_vec ( */
               /*     __shift_right_round (c7x::as_short_vec (vXPermOut),
                * vShift));
                */
               /* vXPermOut = __vpermooq_yvvv ( */
               /*     vXPermCtrl, */
               /*     vX1X3_01, */
               /*     vX0X2_01); /\* g0'(0..3) | g2'(0..3) | g1'(0..3) |
                * g3'(0..3) - */
               /*                   4x 4-pt FFTs (2nd set) *\/ */
               /* *addr = c7x::as_cshort_vec ( */
               /*     __shift_right_round (c7x::as_short_vec (vXPermOut),
                * vShift));
                */
               /* vXPermOut = __vpermeeq_yvvv ( */
               /*     vXPermCtrl, */
               /*     vX1X3_23, */
               /*     vX0X2_23); /\* g0"(0..3) | g2"(0..3) | g1"(0..3) |
                * g3"(0..3) - */
               /*                   4x 4-pt FFTs (3rd set) *\/ */
               /* *addr = c7x::as_cshort_vec ( */
               /*     __shift_right_round (c7x::as_short_vec (vXPermOut),
                * vShift));
                */
               /* vXPermOut = __vpermooq_yvvv ( */
               /*     vXPermCtrl, */
               /*     vX1X3_23, */
               /*     vX0X2_23); /\* g0"'(0..3) | g2"'(0..3) | g1"'(0..3) | */
               /*                   g3"'(0..3) - 4x 4-pt FFTs(4th set) *\/ */
               /* *addr = c7x::as_cshort_vec ( */
               /*     __shift_right_round (c7x::as_short_vec (vXPermOut),
                * vShift));
                */

               __vpred tmp;
               CVP     addr;

               vXPermOut = __vpermeeq_yvvv (
                   vXPermCtrl, vX1X3_01,
                   vX0X2_01); /* g0(0..3) | g2(0..3) | g1(0..3) | g3(0..3) - 4x
                                 4-pt FFTs (1st set) */
               tmp  = c7x::strm_agen<0, CV>::get_vpred ();
               addr = c7x::strm_agen<0, CV>::get_adv (pXLocal);
               __vstore_pred (tmp, addr,
                              c7x::as_cshort_vec (__shift_right_round (
                                  c7x::as_short_vec (vXPermOut), vShift)));
               vXPermOut = __vpermooq_yvvv (
                   vXPermCtrl, vX1X3_01,
                   vX0X2_01); /* g0'(0..3) | g2'(0..3) | g1'(0..3) | g3'(0..3) -
                                 4x 4-pt FFTs (2nd set) */
               tmp  = c7x::strm_agen<0, CV>::get_vpred ();
               addr = c7x::strm_agen<0, CV>::get_adv (pXLocal);
               __vstore_pred (tmp, addr,
                              c7x::as_cshort_vec (__shift_right_round (
                                  c7x::as_short_vec (vXPermOut), vShift)));

               vXPermOut = __vpermeeq_yvvv (
                   vXPermCtrl, vX1X3_23,
                   vX0X2_23); /* g0"(0..3) | g2"(0..3) | g1"(0..3) | g3"(0..3) -
                                 4x 4-pt FFTs (3rd set) */
               tmp  = c7x::strm_agen<0, CV>::get_vpred ();
               addr = c7x::strm_agen<0, CV>::get_adv (pXLocal);
               __vstore_pred (tmp, addr,
                              c7x::as_cshort_vec (__shift_right_round (
                                  c7x::as_short_vec (vXPermOut), vShift)));

               vXPermOut = __vpermooq_yvvv (
                   vXPermCtrl, vX1X3_23,
                   vX0X2_23); /* g0"'(0..3) | g2"'(0..3) | g1"'(0..3) |
                                 g3"'(0..3) - 4x 4-pt FFTs(4th set) */
               tmp  = c7x::strm_agen<0, CV>::get_vpred ();
               addr = c7x::strm_agen<0, CV>::get_adv (pXLocal);
               __vstore_pred (tmp, addr,
                              c7x::as_cshort_vec (__shift_right_round (
                                  c7x::as_short_vec (vXPermOut), vShift)));
            }

         /* loop_profile[loop][0] = (uint32_t) (__TSC - tsc); */
         /* loop_profile[loop][1] = numPointsPerDft; */
         /* loop_profile[loop][2] = 2; // LOOP2 */
         loop++;

         __SA0_CLOSE ();
         __SE0_CLOSE ();
      }
      else
#endif /* NUMPT32_ONLY */
      {
         /* 32-point stage */

         // * using both SEs
         // * SE_LOOP3 - 3D * TRANSPOSE_256BIT - { 32/2; 2, 32;
         // (numPoints*numChannels >> 6)/2, 64}
         // * SA_LOOP3 - 1D - { numPoints*numChannels}

         se0_param = *(
             (__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_LOOP3_PARAM_OFFSET));
         __SE0_OPEN ((void *) pXLocal, se0_param);
         __SE1_OPEN ((void *) (pXLocal + 16), se0_param);

         sa0_param = *(
             (__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA_LOOP3_PARAM_OFFSET));
         __SA0_OPEN (sa0_param);

#if __C7X_HOSTEM__
         vTwX1 = *((CVP) pWLocal);
         vTwX1 = CV (vTwX1.lo(), vTwX1.lo());
         vTwX2 = *((CVP) (pWLocal + 8));
         vTwX2 = CV (vTwX2.lo(), vTwX2.lo());
         vTwX3 = *((CVP) (pWLocal + 16));
         vTwX3 = CV (vTwX3.lo(), vTwX3.lo());
#else
         vTwX1 = *((CVP) pWLocal);
         vTwX1 = (CV) (vTwX1.lo(), vTwX1.lo());
         vTwX2 = *((CVP) (pWLocal + 8));
         vTwX2 = (CV) (vTwX2.lo(), vTwX2.lo());
         vTwX3 = *((CVP) (pWLocal + 16));
         vTwX3 = (CV) (vTwX3.lo(), vTwX3.lo());
#endif

         /* tsc = __TSC; */

         for (k = 0; k < numPoints * numChannels; k += 64) {
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

            /* *addr = c7x::as_cshort_vec
             * (__shift_right_round ( */
            /*     c7x::as_short_vec ((CV) (vX0.lo(), vX2.lo())), vShift)); */
            /* *addr = c7x::as_cshort_vec
             * (__shift_right_round ( */
            /*     c7x::as_short_vec ((CV) (vX1.lo(), vX3.lo())), vShift)); */
            /* *addr = c7x::as_cshort_vec
             * (__shift_right_round ( */
            /*     c7x::as_short_vec ((CV) (vX0.hi(), vX2.hi())), vShift)); */
            /* *addr = c7x::as_cshort_vec
             * (__shift_right_round ( */
            /*     c7x::as_short_vec ((CV) (vX1.hi(), vX3.hi())), vShift)); */

            __vpred tmp;
            CVP     addr;

#if __C7X_HOSTEM__
            tmp  = c7x::strm_agen<0, CV>::get_vpred ();
            addr = c7x::strm_agen<0, CV>::get_adv (pXLocal);
            __vstore_pred (
                tmp, addr,
                c7x::as_cshort_vec (__shift_right_round (
                    c7x::as_short_vec (CV (vX0.lo(), vX2.lo())), vShift)));

            tmp  = c7x::strm_agen<0, CV>::get_vpred ();
            addr = c7x::strm_agen<0, CV>::get_adv (pXLocal);
            __vstore_pred (
                tmp, addr,
                c7x::as_cshort_vec (__shift_right_round (
                    c7x::as_short_vec (CV (vX1.lo(), vX3.lo())), vShift)));

            tmp  = c7x::strm_agen<0, CV>::get_vpred ();
            addr = c7x::strm_agen<0, CV>::get_adv (pXLocal);
            __vstore_pred (
                tmp, addr,
                c7x::as_cshort_vec (__shift_right_round (
                    c7x::as_short_vec (CV (vX0.hi(), vX2.hi())), vShift)));

            tmp  = c7x::strm_agen<0, CV>::get_vpred ();
            addr = c7x::strm_agen<0, CV>::get_adv (pXLocal);
            __vstore_pred (
                tmp, addr,
                c7x::as_cshort_vec (__shift_right_round (
                    c7x::as_short_vec (CV (vX1.hi(), vX3.hi())), vShift)));
#else
            tmp  = c7x::strm_agen<0, CV>::get_vpred ();
            addr = c7x::strm_agen<0, CV>::get_adv (pXLocal);
            __vstore_pred (
                tmp, addr,
                c7x::as_cshort_vec (__shift_right_round (
                    c7x::as_short_vec ((CV) (vX0.lo(), vX2.lo())), vShift)));

            tmp  = c7x::strm_agen<0, CV>::get_vpred ();
            addr = c7x::strm_agen<0, CV>::get_adv (pXLocal);
            __vstore_pred (
                tmp, addr,
                c7x::as_cshort_vec (__shift_right_round (
                    c7x::as_short_vec ((CV) (vX1.lo(), vX3.lo())), vShift)));

            tmp  = c7x::strm_agen<0, CV>::get_vpred ();
            addr = c7x::strm_agen<0, CV>::get_adv (pXLocal);
            __vstore_pred (
                tmp, addr,
                c7x::as_cshort_vec (__shift_right_round (
                    c7x::as_short_vec ((CV) (vX0.hi(), vX2.hi())), vShift)));

            tmp  = c7x::strm_agen<0, CV>::get_vpred ();
            addr = c7x::strm_agen<0, CV>::get_adv (pXLocal);
            __vstore_pred (
                tmp, addr,
                c7x::as_cshort_vec (__shift_right_round (
                    c7x::as_short_vec ((CV) (vX1.hi(), vX3.hi())), vShift)));
#endif
         }

         /* loop_profile[loop][0] = (uint32_t) (__TSC - tsc); */
         /* loop_profile[loop][1] = numPointsPerDft; */
         /* loop_profile[loop][2] = 3; // LOOP3 */
         loop++;

         __SE0_CLOSE ();
         __SE1_CLOSE ();
         __SA0_CLOSE ();
      }

      numPointsPerDft >>= 2;
      pWLocal += numPointsPerDft * 3;
      stageIndex++;
      vShift = __vload_dup ((uint32_t *) (pShift + stageIndex));

#if (!NUMPT32_ONLY)
      if (numPointsPerDft == 4) {
         if (numPoints == 16) {
#if (!LOOP2THEN4)
            // clang-format off
               #if __C7X_HOSTEM__
                               static const
                c7x::uchar_vec vXPermCtrl  = c7x::uchar_vec(0x00, 0x01, 0x02, 0x03, 0x08, 0x09, 0x0A, 0x0B,
                                                0x04, 0x05, 0x06, 0x07, 0x0C, 0x0D, 0x0E, 0x0F,
                                                0x10, 0x11, 0x12, 0x13, 0x18, 0x19, 0x1A, 0x1B,
                                                0x14, 0x15, 0x16, 0x17, 0x1C, 0x1D, 0x1E, 0x1F,
                                                0x20, 0x21, 0x22, 0x23, 0x28, 0x29, 0x2A, 0x2B,
                                                0x24, 0x25, 0x26, 0x27, 0x2C, 0x2D, 0x2E, 0x2F,
                                                0x30, 0x31, 0x32, 0x33, 0x38, 0x39, 0x3A, 0x3B,
                                                0x34, 0x35, 0x36, 0x37, 0x3C, 0x3D, 0x3E, 0x3F);

               #else
                static const
                c7x::uchar_vec vXPermCtrl  = (c7x::uchar_vec)(0x00, 0x01, 0x02, 0x03, 0x08, 0x09, 0x0A, 0x0B,
                                                0x04, 0x05, 0x06, 0x07, 0x0C, 0x0D, 0x0E, 0x0F,
                                                0x10, 0x11, 0x12, 0x13, 0x18, 0x19, 0x1A, 0x1B,
                                                0x14, 0x15, 0x16, 0x17, 0x1C, 0x1D, 0x1E, 0x1F,
                                                0x20, 0x21, 0x22, 0x23, 0x28, 0x29, 0x2A, 0x2B,
                                                0x24, 0x25, 0x26, 0x27, 0x2C, 0x2D, 0x2E, 0x2F,
                                                0x30, 0x31, 0x32, 0x33, 0x38, 0x39, 0x3A, 0x3B,
                                                0x34, 0x35, 0x36, 0x37, 0x3C, 0x3D, 0x3E, 0x3F);
#endif
            // clang-format on

            c7x::uchar_vec vX0X1_01, vX2X3_01, vX0X1_23, vX2X3_23;
            c7x::uchar_vec vXPermOut;

            // ** the following SE configuration may cause sub-optimal tile
            // in SE because second row of tile starts in the middle of first
            // row
            // * SE_LOOP4 - 3D * TRANSPOSE_32BIT - { 4; 16, 4;
            // numPoints*numChannels >> 6, 64}
            // * SA_LOOP4 - 1D - { numPoints*numChannels}

            se0_param =
                *((uint16 *) ((uint8_t *) pBlock + SE_LOOP4_PARAM_OFFSET));
            __SE0_OPEN ((void *) pXLocal, se0_param);

            sa0_param =
                *((uint16 *) ((uint8_t *) pBlock + SA_LOOP4_PARAM_OFFSET));
            __SA0_OPEN (sa0_param);

            /* tsc = __TSC; */

            for (k = 0; k < numChannels << 4; k += 64) {
#if (ALIGNED_4PT)
               CV v0, v1, v2, v3;
               CV v01e, v01o, v23e, v23o;

               v0 =
                   c7x::strm_eng<0, CV>::get_adv (); /* g0(0..3) | g2(0..3) |
                                           g1(0..3) | g3(0..3) - 4x 4-pt FFTs,
                                           from original 16-pt FFT (1st of 4) */
               v1 =
                   c7x::strm_eng<0, CV>::get_adv (); /* g0'(0..3) | g2'(0..3) |
                                           g1'(0..3) | g3'(0..3) - 4x 4-pt FFTs,
                                           from original 16-pt FFT (2nd of 4) */
               v2 =
                   c7x::strm_eng<0, CV>::get_adv (); /* g0"(0..3) | g2"(0..3) |
                                           g1"(0..3) | g3"(0..3) - 4x 4-pt FFTs,
                                           from original 16-pt FFT (3rd of 4) */
               v3 = c7x::strm_eng<0,
                                  CV>::get_adv (); /* g0"'(0..3) | g2"'(0..3) |
                                         g1"'(0..3) | g3"'(0..3) - 4x 4-pt FFTs,
                                         from original 16-pt FFT (4th of 4) */

               v01e = c7x::as_cshort_vec (__pack_consec_low_int (
                   c7x::as_int_vec (v1),
                   c7x::as_int_vec (
                       v0))); /* g0(0), g0(2) | g2(0), g2(2) | g1(0), g1(2) |
                                 g3(0), g3(2) | g0'(0), g0'(2) | ... */
               v01o = c7x::as_cshort_vec (__pack_consec_high_int (
                   c7x::as_int_vec (v1),
                   c7x::as_int_vec (
                       v0))); /* g0(1), g0(3) | g2(1), g2(3) | g1(1), g1(3) |
                                 g3(1), g3(3) | g0'(0), g0'(2) | ... */
               v23e = c7x::as_cshort_vec (__pack_consec_low_int (
                   c7x::as_int_vec (v3),
                   c7x::as_int_vec (v2))); /* g0"(0), g0"(2) | g2"(0), g2"(2) |
                                         g1"(0), g1"(2) | g3"(0), g3"(2) |
                                         g0"'(0), g0"'(2) | ... */
               v23o = c7x::as_cshort_vec (__pack_consec_high_int (
                   c7x::as_int_vec (v3),
                   c7x::as_int_vec (v2))); /* g0"(1), g0"(3) | g2"(1), g2"(3) |
                                         g1"(1), g1"(3) | g3"(1), g3"(3) |
                                         g0"'(1), g0"'(3) | ... */

               vX_0 = c7x::as_cshort_vec (__pack_consec_low_int (
                   c7x::as_int_vec (v23e),
                   c7x::as_int_vec (
                       v01e))); /* g0(0), g2(0), g1(0), g3(0) | g0'(0),
                              g2'(0), g1'(0), g3'(0) | g0"(0),
                              g2"(0), g1"(0), g3"(0) | ... */
               vX_N_4 = c7x::as_cshort_vec (__pack_consec_low_int (
                   c7x::as_int_vec (v23o),
                   c7x::as_int_vec (
                       v01o))); /* g0(1), g2(1), g1(1), g3(1) | g0'(1),
                              g2'(1), g1'(1), g3'(1) | ... */
               vX_N_2 = c7x::as_cshort_vec (__pack_consec_high_int (
                   c7x::as_int_vec (v23e),
                   c7x::as_int_vec (
                       v01e))); /* g0(2), g2(2), g1(2), g3(2) | g0'(2),
                              g2'(2), g1'(2), g3'(2) | ... */
               vX_3N_4 = c7x::as_cshort_vec (__pack_consec_high_int (
                   c7x::as_int_vec (v23o),
                   c7x::as_int_vec (
                       v01o))); /* g0(3), g2(3), g1(3), g3(3) | g0'(3),
                              g2'(3), g1'(3), g3'(3) | ... */
#else                           /* ALIGNED_4PT */
               vX_0     = c7x::strm_eng<0, CV>::get_adv ();
               vX_N_4   = c7x::strm_eng<0, CV>::get_adv ();
               vX_N_2   = c7x::strm_eng<0, CV>::get_adv ();
               vX_3N_4  = c7x::strm_eng<0, CV>::get_adv ();
#endif                          /* ALIGNED_4PT */

               vSum1  = vX_0 + vX_N_2;
               vSum2  = vX_N_4 + vX_3N_4;
               vDiff1 = vX_0 - vX_N_2;
               vDiff2 = vX_N_4 - vX_3N_4;

               vX0 = vSum1 + vSum2;
               vX1 = vDiff1 - __vcrot90h_vv (vDiff2);
               vX2 = vSum1 - vSum2;
               vX3 = vDiff1 + __vcrot90h_vv (vDiff2);

#if __C7X_HOSTEM__
               vX0X1_01 = c7x::uchar_vec (c7x::as_uchar_vec (vX0).lo(),
                                          c7x::as_uchar_vec (vX1).lo());
               vX2X3_01 = c7x::uchar_vec (c7x::as_uchar_vec (vX2).lo(),
                                          c7x::as_uchar_vec (vX3).lo());
               vX0X1_23 = c7x::uchar_vec (c7x::as_uchar_vec (vX0).hi(),
                                          c7x::as_uchar_vec (vX1).hi());
               vX2X3_23 = c7x::uchar_vec (c7x::as_uchar_vec (vX2).hi(),
                                          c7x::as_uchar_vec (vX3).hi());

#else
               vX0X1_01 = (c7x::uchar_vec) (c7x::as_uchar_vec (vX0).lo(),
                                            c7x::as_uchar_vec (vX1).lo());
               vX2X3_01 = (c7x::uchar_vec) (c7x::as_uchar_vec (vX2).lo(),
                                            c7x::as_uchar_vec (vX3).lo());
               vX0X1_23 = (c7x::uchar_vec) (c7x::as_uchar_vec (vX0).hi(),
                                            c7x::as_uchar_vec (vX1).hi());
               vX2X3_23 = (c7x::uchar_vec) (c7x::as_uchar_vec (vX2).hi(),
                                            c7x::as_uchar_vec (vX3).hi());
#endif

               /* vXPermOut = __vpermeeq_yvvv (vXPermCtrl, vX2X3_01,
                * vX0X1_01);
                */
               /* *addr = c7x::as_cshort_vec ( */
               /*     __shift_right_round (c7x::as_short_vec (vXPermOut),
                * vShift));
                */
               /* vXPermOut = __vpermooq_yvvv (vXPermCtrl, vX2X3_01,
                * vX0X1_01);
                */
               /* *addr = c7x::as_cshort_vec ( */
               /*     __shift_right_round (c7x::as_short_vec (vXPermOut),
                * vShift));
                */
               /* vXPermOut = __vpermeeq_yvvv (vXPermCtrl, vX2X3_23,
                * vX0X1_23);
                */
               /* *addr = c7x::as_cshort_vec ( */
               /*     __shift_right_round (c7x::as_short_vec (vXPermOut),
                * vShift));
                */
               /* vXPermOut = __vpermooq_yvvv (vXPermCtrl, vX2X3_23,
                * vX0X1_23);
                */
               /* *addr = c7x::as_cshort_vec ( */
               /*     __shift_right_round (c7x::as_short_vec (vXPermOut),
                * vShift));
                */

               __vpred tmp;
               CVP     addr;
               vXPermOut = __vpermeeq_yvvv (vXPermCtrl, vX2X3_01, vX0X1_01);
               tmp       = c7x::strm_agen<0, CV>::get_vpred ();
               addr      = c7x::strm_agen<0, CV>::get_adv (pYLocal);
               __vstore_pred (tmp, addr,
                              c7x::as_cshort_vec (__shift_right_round (
                                  c7x::as_short_vec (vXPermOut), vShift)));
               vXPermOut = __vpermooq_yvvv (vXPermCtrl, vX2X3_01, vX0X1_01);
               tmp       = c7x::strm_agen<0, CV>::get_vpred ();
               addr      = c7x::strm_agen<0, CV>::get_adv (pYLocal);
               __vstore_pred (tmp, addr,
                              c7x::as_cshort_vec (__shift_right_round (
                                  c7x::as_short_vec (vXPermOut), vShift)));
               vXPermOut = __vpermeeq_yvvv (vXPermCtrl, vX2X3_23, vX0X1_23);
               tmp       = c7x::strm_agen<0, CV>::get_vpred ();
               addr      = c7x::strm_agen<0, CV>::get_adv (pYLocal);
               __vstore_pred (tmp, addr,
                              c7x::as_cshort_vec (__shift_right_round (
                                  c7x::as_short_vec (vXPermOut), vShift)));
               vXPermOut = __vpermooq_yvvv (vXPermCtrl, vX2X3_23, vX0X1_23);
               tmp       = c7x::strm_agen<0, CV>::get_vpred ();
               addr      = c7x::strm_agen<0, CV>::get_adv (pYLocal);
               __vstore_pred (tmp, addr,
                              c7x::as_cshort_vec (__shift_right_round (
                                  c7x::as_short_vec (vXPermOut), vShift)));
            }

            /* loop_profile[loop][0] = (uint32_t) (__TSC - tsc); */
            /* loop_profile[loop][1] = numPointsPerDft; */
            /* loop_profile[loop][2] = 4; // LOOP4 */
            loop++;

            __SE0_CLOSE ();
            __SA0_CLOSE ();
#endif /* LOOP2THEN4 */
         }
         else {
            CV vX0Shifted, vX1Shifted, vX2Shifted, vX3Shifted;

            // * SE_LOOP5 - 3D * TRANSPOSE_32BIT - { numPoints >> 4; 16,
            // numPoints >> 4; numChannels, numPoints}

            /* 4-point stage with bit-reversal */
#if (ALIGNED_4PT)
            if (numPoints == 64)
               se0_param = *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                                                   SE_LOOP4_PARAM_OFFSET));
            else
#endif /* ALIGNED_4PT */
               se0_param = *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                                                   SE_LOOP5_PARAM_OFFSET));
            __SE0_OPEN ((void *) pXLocal, se0_param);

            numLeadingZeros = __norm ((int32_t) (numPoints - 1)) + 1;

            /* pY0 = &pYLocal[0x00000000u]; */
            /* pY1 = &pYLocal[0x40000000u >> numLeadingZeros]; */
            /* pY2 = &pYLocal[0x80000000u >> numLeadingZeros]; */
            /* pY3 = &pYLocal[0xC0000000u >> numLeadingZeros]; */

            pY0 = (cshort*) (pY + 0);
            pY1 = (cshort*) (pY + ((0x40000000u >> numLeadingZeros) << 1));
            pY2 = (cshort*) (pY + ((0x80000000u >> numLeadingZeros) << 1));
            pY3 = (cshort*) (pY + ((0xC0000000u >> numLeadingZeros) << 1));

#ifdef CL7X_HE_CFLOAT_PTR_BUG
            short *myPY0 = (short *) pY0;
            short *myPY1 = (short *) pY1;
            short *myPY2 = (short *) pY2;
            short *myPY3 = (short *) pY3;
#endif

#if (ALIGNED_4PT)
            if (numPoints == 64)
               for (l = 0; l < numChannels; l++) {
                  for (k = 0; k < numPoints >> 4; k += 4) {
                     offsetBitReverse = __bit_reverse (k) >> numLeadingZeros;

                     CV v0, v1, v2, v3;
                     CV v01e, v01o, v23e, v23o;

                     v0 = c7x::strm_eng<0, CV>::get_adv (); /* g0(0..3) |
                                                  g2(0..3) | g1(0..3) | g3(0..3)
                                                  - 4x 4-pt FFTs, from original
                                                  16-pt FFT (1st of 4) */
                     v1 = c7x::strm_eng<0,
                                        CV>::get_adv ();    /* g0'(0..3) |
                                       g2'(0..3) | g1'(0..3) | g3'(0..3) - 4x 4-pt
                                       FFTs, from original 16-pt FFT (2nd of 4) */
                     v2 = c7x::strm_eng<0, CV>::get_adv (); /* g0"(0..3) |
                                       g2"(0..3) | g1"(0..3) |    g3"(0..3) - 4x
                                       4-pt FFTs, from    original 16-pt FFT
                                       (3rd of 4) */
                     v3 = c7x::strm_eng<0, CV>::get_adv (); /* g0"'(0..3) |
                                       g2"'(0..3) | g1"'(0..3) | g3"'(0..3) - 4x
                                       4-pt FFTs, from original 16-pt FFT (4th
                                       of 4) */

                     v01e = c7x::as_cshort_vec (__pack_consec_low (
                         c7x::as_int_vec (v1),
                         c7x::as_int_vec (v0))); /* g0(0), g0(2) | g2(0), g2(2)
                                               | g1(0), g1(2) | g3(0), g3(2) |
                                               g0'(0), g0'(2) | ... */
                     v01o = c7x::as_cshort_vec (__pack_consec_high (
                         c7x::as_int_vec (v1),
                         c7x::as_int_vec (v0))); /* g0(1), g0(3) | g2(1), g2(3)
                                               | g1(1), g1(3) | g3(1), g3(3) |
                                               g0'(0), g0'(2) | ... */
                     v23e = c7x::as_cshort_vec (__pack_consec_low (
                         c7x::as_int_vec (v3),
                         c7x::as_int_vec (
                             v2))); /* g0"(0), g0"(2) | g2"(0), g2"(2) |
                                  g1"(0), g1"(2) | g3"(0), g3"(2) |
                                  g0"'(0), g0"'(2) | ... */
                     v23o = c7x::as_cshort_vec (__pack_consec_high (
                         c7x::as_int_vec (v3),
                         c7x::as_int_vec (
                             v2))); /* g0"(1), g0"(3) | g2"(1), g2"(3) |
                                  g1"(1), g1"(3) | g3"(1), g3"(3) |
                                  g0"'(1), g0"'(3) | ... */

                     vX_0 = c7x::as_cshort_vec (__pack_consec_low (
                         c7x::as_int_vec (v23e),
                         c7x::as_int_vec (
                             v01e))); /* g0(0), g2(0), g1(0), g3(0) | g0'(0),
                                         g2'(0), g1'(0), g3'(0) | g0"(0),
                                         g2"(0), g1"(0), g3"(0) | ... */
                     vX_N_4 = c7x::as_cshort_vec (__pack_consec_low (
                         c7x::as_int_vec (v23o),
                         c7x::as_int_vec (
                             v01o))); /* g0(1), g2(1), g1(1), g3(1) | g0'(1),
                                         g2'(1), g1'(1), g3'(1) | ... */
                     vX_N_2 = c7x::as_cshort_vec (__pack_consec_high (
                         c7x::as_int_vec (v23e),
                         c7x::as_int_vec (
                             v01e))); /* g0(2), g2(2), g1(2), g3(2) | g0'(2),
                                         g2'(2), g1'(2), g3'(2) | ... */
                     vX_3N_4 = c7x::as_cshort_vec (__pack_consec_high (
                         c7x::as_int_vec (v23o),
                         c7x::as_int_vec (
                             v01o))); /* g0(3), g2(3), g1(3), g3(3) | g0'(3),
                                         g2'(3), g1'(3), g3'(3) | ... */

                     vSum1  = vX_0 + vX_N_2;
                     vSum2  = vX_N_4 + vX_3N_4;
                     vDiff1 = vX_0 - vX_N_2;
                     vDiff2 = vX_N_4 - vX_3N_4;

                     vX0 = vSum1 + vSum2;
                     vX1 = vDiff1 - __vcrot90h_vv (vDiff2);
                     vX2 = vSum1 - vSum2;
                     vX3 = vDiff1 + __vcrot90h_vv (vDiff2);

                     vX0Shifted = c7x::as_cshort_vec (
                         __shift_right_round (c7x::as_short_vec (vX0), vShift));
                     vX1Shifted = c7x::as_cshort_vec (
                         __shift_right_round (c7x::as_short_vec (vX1), vShift));
                     vX2Shifted = c7x::as_cshort_vec (
                         __shift_right_round (c7x::as_short_vec (vX2), vShift));
                     vX3Shifted = c7x::as_cshort_vec (
                         __shift_right_round (c7x::as_short_vec (vX3), vShift));

                     __vstore_reverse_bit ((CVP) (pY0 + offsetBitReverse),
                                           vX0Shifted);
                     __vstore_reverse_bit ((CVP) (pY1 + offsetBitReverse),
                                           vX1Shifted);
                     __vstore_reverse_bit ((CVP) (pY2 + offsetBitReverse),
                                           vX2Shifted);
                     __vstore_reverse_bit ((CVP) (pY3 + offsetBitReverse),
                                           vX3Shifted);
                  }
#ifdef CL7X_HE_CFLOAT_PTR_BUG
                  myPY0 += (numPoints << 1);
                  myPY1 += (numPoints << 1);
                  myPY2 += (numPoints << 1);
                  myPY3 += (numPoints << 1);

                  pY0 = (cshort*) myPY0;
                  pY1 = (cshort*) myPY1;
                  pY2 = (cshort*) myPY2;
                  pY3 = (cshort*) myPY3;

#else
                  pY0 += numPoints;
                  pY1 += numPoints;
                  pY2 += numPoints;
                  pY3 += numPoints;
#endif
               }
            else
#endif /* ALIGNED_4PT */
               for (l = 0; l < numChannels; l++) {
                  for (k = 0; k < numPoints >> 4; k += 4) {
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

                     vX0Shifted = c7x::as_cshort_vec (
                         __shift_right_round (c7x::as_short_vec (vX0), vShift));
                     vX1Shifted = c7x::as_cshort_vec (
                         __shift_right_round (c7x::as_short_vec (vX1), vShift));
                     vX2Shifted = c7x::as_cshort_vec (
                         __shift_right_round (c7x::as_short_vec (vX2), vShift));
                     vX3Shifted = c7x::as_cshort_vec (
                         __shift_right_round (c7x::as_short_vec (vX3), vShift));

                     __vstore_reverse_bit ((CVP) (pY0 + offsetBitReverse),
                                           vX0Shifted);
                     __vstore_reverse_bit ((CVP) (pY1 + offsetBitReverse),
                                           vX1Shifted);
                     __vstore_reverse_bit ((CVP) (pY2 + offsetBitReverse),
                                           vX2Shifted);
                     __vstore_reverse_bit ((CVP) (pY3 + offsetBitReverse),
                                           vX3Shifted);
                  }
#ifdef CL7X_HE_CFLOAT_PTR_BUG
                  myPY0 += (numPoints << 1);
                  myPY1 += (numPoints << 1);
                  myPY2 += (numPoints << 1);
                  myPY3 += (numPoints << 1);

                  pY0 = (cshort*) myPY0;
                  pY1 = (cshort*) myPY1;
                  pY2 = (cshort*) myPY2;
                  pY3 = (cshort*) myPY3;

#else
               pY0 += numPoints;
               pY1 += numPoints;
               pY2 += numPoints;
               pY3 += numPoints;
#endif
               }

            /* loop_profile[loop][0] = (uint32_t) (__TSC - tsc); */
            /* loop_profile[loop][1] = numPointsPerDft; */
            /* loop_profile[loop][2] = 5; // LOOP5 */
            loop++;

            __SE0_CLOSE ();
         }
      }
      else
#endif /* NUMPT32_ONLY */
      {
         /* 4-point stage followed by 2-point stage with bit-reversal */
         cshort twTemp;
         CV     vX0_2PtDft_1, vX0_2PtDft_2;
         CV     vX1_2PtDft_1, vX1_2PtDft_2;
         CV     vX2_2PtDft_1, vX2_2PtDft_2;
         CV     vX3_2PtDft_1, vX3_2PtDft_2;
         CV     vX_0_1, vX_N_4_1, vX_N_2_1, vX_3N_4_1;
         CV     vSum1_1, vSum2_1, vDiff1_1, vDiff2_1;
         CV     vX0_1, vX1_1, vX2_1, vX3_1;
#if __C7X_HOSTEM__
         pWLocal += 1;
         twTemp = *pWLocal;
         vTwX1  = CV (twTemp, twTemp, twTemp, twTemp, twTemp, twTemp, twTemp,
                      twTemp, twTemp, twTemp, twTemp, twTemp, twTemp, twTemp,
                      twTemp, twTemp);
         pWLocal += 2;
         twTemp = *pWLocal;
         vTwX2  = CV (twTemp, twTemp, twTemp, twTemp, twTemp, twTemp, twTemp,
                      twTemp, twTemp, twTemp, twTemp, twTemp, twTemp, twTemp,
                      twTemp, twTemp);
         pWLocal += 2;
         twTemp = *pWLocal;
         vTwX3  = CV (twTemp, twTemp, twTemp, twTemp, twTemp, twTemp, twTemp,
                      twTemp, twTemp, twTemp, twTemp, twTemp, twTemp, twTemp,
                      twTemp, twTemp);

#else
         pWLocal += 1;
         twTemp = *pWLocal;
         vTwX1  = (CV) (twTemp, twTemp, twTemp, twTemp, twTemp, twTemp, twTemp,
                       twTemp, twTemp, twTemp, twTemp, twTemp, twTemp, twTemp,
                       twTemp, twTemp);
         pWLocal += 2;
         twTemp = *pWLocal;
         vTwX2  = (CV) (twTemp, twTemp, twTemp, twTemp, twTemp, twTemp, twTemp,
                       twTemp, twTemp, twTemp, twTemp, twTemp, twTemp, twTemp,
                       twTemp, twTemp);
         pWLocal += 2;
         twTemp = *pWLocal;
         vTwX3  = (CV) (twTemp, twTemp, twTemp, twTemp, twTemp, twTemp, twTemp,
                       twTemp, twTemp, twTemp, twTemp, twTemp, twTemp, twTemp,
                       twTemp, twTemp);
#endif

#if (!NUMPT32_ONLY)
         if (numPoints == 32)
#endif /* NUMPT32_ONLY */
         {
            // clang-format off
               #if __C7X_HOSTEM__
                               static const
                c7x::uchar_vec vXPermCtrl  = c7x::uchar_vec(0x00, 0x01, 0x02, 0x03, 0x08, 0x09, 0x0A, 0x0B,
                                                0x04, 0x05, 0x06, 0x07, 0x0C, 0x0D, 0x0E, 0x0F,
                                                0x10, 0x11, 0x12, 0x13, 0x18, 0x19, 0x1A, 0x1B,
                                                0x14, 0x15, 0x16, 0x17, 0x1C, 0x1D, 0x1E, 0x1F,
                                                0x20, 0x21, 0x22, 0x23, 0x28, 0x29, 0x2A, 0x2B,
                                                0x24, 0x25, 0x26, 0x27, 0x2C, 0x2D, 0x2E, 0x2F,
                                                0x30, 0x31, 0x32, 0x33, 0x38, 0x39, 0x3A, 0x3B,
                                                0x34, 0x35, 0x36, 0x37, 0x3C, 0x3D, 0x3E, 0x3F);

#else
                static const
                c7x::uchar_vec vXPermCtrl  = (c7x::uchar_vec)(0x00, 0x01, 0x02, 0x03, 0x08, 0x09, 0x0A, 0x0B,
                                                0x04, 0x05, 0x06, 0x07, 0x0C, 0x0D, 0x0E, 0x0F,
                                                0x10, 0x11, 0x12, 0x13, 0x18, 0x19, 0x1A, 0x1B,
                                                0x14, 0x15, 0x16, 0x17, 0x1C, 0x1D, 0x1E, 0x1F,
                                                0x20, 0x21, 0x22, 0x23, 0x28, 0x29, 0x2A, 0x2B,
                                                0x24, 0x25, 0x26, 0x27, 0x2C, 0x2D, 0x2E, 0x2F,
                                                0x30, 0x31, 0x32, 0x33, 0x38, 0x39, 0x3A, 0x3B,
                                                0x34, 0x35, 0x36, 0x37, 0x3C, 0x3D, 0x3E, 0x3F);
#endif
            // clang-format on

            c7x::uchar_vec vX01_2PtDft_1_lo, vX23_2PtDft_1_lo, vX01_2PtDft_2_lo,
                vX23_2PtDft_2_lo;
            c7x::uchar_vec vX01_2PtDft_1_hi, vX23_2PtDft_1_hi, vX01_2PtDft_2_hi,
                vX23_2PtDft_2_hi;
            CV             vX0X1X2X3_0, vX0X1X2X3_1, vX0X1X2X3_2, vX0X1X2X3_3;
            CV             vX0X1X2X3_4, vX0X1X2X3_5, vX0X1X2X3_6, vX0X1X2X3_7;
            c7x::uchar_vec vXPermOut;

            // ALIGNED_8PT
            // * SE_LOOP6 - 3D * TRANSPOSE_64BIT - { 16; 8, 16;
            // numPoints*numChannels >> 7, 128}
            // * SA_LOOP6 - 1D - { numPoints*numChannels}

            se0_param = *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                                                SE_LOOP6_PARAM_OFFSET));
            __SE0_OPEN ((void *) pXLocal, se0_param);

            sa0_param = *((__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                                                SA_LOOP6_PARAM_OFFSET));
            __SA0_OPEN (sa0_param);

            /* tsc = __TSC; */

            for (k = 0; k < numChannels << 5; k += 128) {
#if (ALIGNED_8PT)
               // clang-format off
                  #if __C7X_HOSTEM__
                  static const
                        c7x::uchar_vec vXPermCtrl2  = c7x::uchar_vec(
                            0x00, 0x01, 0x02, 0x03, 0x20, 0x21, 0x22, 0x23,
                            0x04, 0x05, 0x06, 0x07, 0x24, 0x25, 0x26, 0x27,
                            0x08, 0x09, 0x0A, 0x0B, 0x28, 0x29, 0x2A, 0x2B,
                            0x0C, 0x0D, 0x0E, 0x0F, 0x2C, 0x2D, 0x2E, 0x2F,
                            0x10, 0x11, 0x12, 0x13, 0x30, 0x31, 0x32, 0x33,
                            0x14, 0x15, 0x16, 0x17, 0x34, 0x35, 0x36, 0x37,
                            0x18, 0x19, 0x1A, 0x1B, 0x38, 0x39, 0x3A, 0x3B,
                            0x1C, 0x1D, 0x1E, 0x1F, 0x3C, 0x3D, 0x3E, 0x3F);
                  #else
                    static const
                        c7x::uchar_vec vXPermCtrl2  = (c7x::uchar_vec)(
                            0x00, 0x01, 0x02, 0x03, 0x20, 0x21, 0x22, 0x23,
                            0x04, 0x05, 0x06, 0x07, 0x24, 0x25, 0x26, 0x27,
                            0x08, 0x09, 0x0A, 0x0B, 0x28, 0x29, 0x2A, 0x2B,
                            0x0C, 0x0D, 0x0E, 0x0F, 0x2C, 0x2D, 0x2E, 0x2F,
                            0x10, 0x11, 0x12, 0x13, 0x30, 0x31, 0x32, 0x33,
                            0x14, 0x15, 0x16, 0x17, 0x34, 0x35, 0x36, 0x37,
                            0x18, 0x19, 0x1A, 0x1B, 0x38, 0x39, 0x3A, 0x3B,
                            0x1C, 0x1D, 0x1E, 0x1F, 0x3C, 0x3D, 0x3E, 0x3F);
#endif
               // clang-format on
#define PACK_HIGH_INT16(V1, V2)                                                \
   __vpermoow_yvvv (vXPermCtrl2, c7x::as_uchar_vec (V1), c7x::as_uchar_vec (V2))

               CV v0, v1, v2, v3, v4, v5, v6, v7;

               v0 = c7x::strm_eng<0, CV>::get_adv ();
               v1 = c7x::strm_eng<0, CV>::get_adv ();
               v2 = c7x::strm_eng<0, CV>::get_adv ();
               v3 = c7x::strm_eng<0, CV>::get_adv ();
               v4 = c7x::strm_eng<0, CV>::get_adv ();
               v5 = c7x::strm_eng<0, CV>::get_adv ();
               v6 = c7x::strm_eng<0, CV>::get_adv ();
               v7 = c7x::strm_eng<0, CV>::get_adv ();

               vX_0 = c7x::as_cshort_vec (
                   __pack_low (c7x::as_int_vec (v4), c7x::as_int_vec (v0)));
               vX_0_1 = c7x::as_cshort_vec (PACK_HIGH_INT16 (
                   c7x::as_int_vec (v4), c7x::as_int_vec (v0)));
               vX_N_4 = c7x::as_cshort_vec (
                   __pack_low (c7x::as_int_vec (v5), c7x::as_int_vec (v1)));
               vX_N_4_1 = c7x::as_cshort_vec (PACK_HIGH_INT16 (
                   c7x::as_int_vec (v5), c7x::as_int_vec (v1)));
               vX_N_2   = c7x::as_cshort_vec (
                     __pack_low (c7x::as_int_vec (v6), c7x::as_int_vec (v2)));
               vX_N_2_1 = c7x::as_cshort_vec (PACK_HIGH_INT16 (
                   c7x::as_int_vec (v6), c7x::as_int_vec (v2)));
               vX_3N_4  = c7x::as_cshort_vec (
                    __pack_low (c7x::as_int_vec (v7), c7x::as_int_vec (v3)));
               vX_3N_4_1 = c7x::as_cshort_vec (PACK_HIGH_INT16 (
                   c7x::as_int_vec (v7), c7x::as_int_vec (v3)));
#else  /* ALIGNED_8PT */
               vX_0      = c7x::strm_eng<0, CV>::get_adv ();
               vX_0_1    = c7x::strm_eng<0, CV>::get_adv ();
               vX_N_4    = c7x::strm_eng<0, CV>::get_adv ();
               vX_N_4_1  = c7x::strm_eng<0, CV>::get_adv ();
               vX_N_2    = c7x::strm_eng<0, CV>::get_adv ();
               vX_N_2_1  = c7x::strm_eng<0, CV>::get_adv ();
               vX_3N_4   = c7x::strm_eng<0, CV>::get_adv ();
               vX_3N_4_1 = c7x::strm_eng<0, CV>::get_adv ();
#endif /* ALIGNED_8PT */

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

#if __C7X_HOSTEM__
               vX01_2PtDft_1_lo =
                   c7x::uchar_vec (c7x::as_uchar_vec (vX0_2PtDft_1).lo(),
                                   c7x::as_uchar_vec (vX1_2PtDft_1).lo());
               vX23_2PtDft_1_lo =
                   c7x::uchar_vec (c7x::as_uchar_vec (vX2_2PtDft_1).lo(),
                                   c7x::as_uchar_vec (vX3_2PtDft_1).lo());
               vX01_2PtDft_2_lo =
                   c7x::uchar_vec (c7x::as_uchar_vec (vX0_2PtDft_2).lo(),
                                   c7x::as_uchar_vec (vX1_2PtDft_2).lo());
               vX23_2PtDft_2_lo =
                   c7x::uchar_vec (c7x::as_uchar_vec (vX2_2PtDft_2).lo(),
                                   c7x::as_uchar_vec (vX3_2PtDft_2).lo());
               vX01_2PtDft_1_hi =
                   c7x::uchar_vec (c7x::as_uchar_vec (vX0_2PtDft_1).hi(),
                                   c7x::as_uchar_vec (vX1_2PtDft_1).hi());
               vX23_2PtDft_1_hi =
                   c7x::uchar_vec (c7x::as_uchar_vec (vX2_2PtDft_1).hi(),
                                   c7x::as_uchar_vec (vX3_2PtDft_1).hi());
               vX01_2PtDft_2_hi =
                   c7x::uchar_vec (c7x::as_uchar_vec (vX0_2PtDft_2).hi(),
                                   c7x::as_uchar_vec (vX1_2PtDft_2).hi());
               vX23_2PtDft_2_hi =
                   c7x::uchar_vec (c7x::as_uchar_vec (vX2_2PtDft_2).hi(),
                                   c7x::as_uchar_vec (vX3_2PtDft_2).hi());
#else
               vX01_2PtDft_1_lo =
                   (c7x::uchar_vec) (c7x::as_uchar_vec (vX0_2PtDft_1).lo(),
                                     c7x::as_uchar_vec (vX1_2PtDft_1).lo());
               vX23_2PtDft_1_lo =
                   (c7x::uchar_vec) (c7x::as_uchar_vec (vX2_2PtDft_1).lo(),
                                     c7x::as_uchar_vec (vX3_2PtDft_1).lo());
               vX01_2PtDft_2_lo =
                   (c7x::uchar_vec) (c7x::as_uchar_vec (vX0_2PtDft_2).lo(),
                                     c7x::as_uchar_vec (vX1_2PtDft_2).lo());
               vX23_2PtDft_2_lo =
                   (c7x::uchar_vec) (c7x::as_uchar_vec (vX2_2PtDft_2).lo(),
                                     c7x::as_uchar_vec (vX3_2PtDft_2).lo());
               vX01_2PtDft_1_hi =
                   (c7x::uchar_vec) (c7x::as_uchar_vec (vX0_2PtDft_1).hi(),
                                     c7x::as_uchar_vec (vX1_2PtDft_1).hi());
               vX23_2PtDft_1_hi =
                   (c7x::uchar_vec) (c7x::as_uchar_vec (vX2_2PtDft_1).hi(),
                                     c7x::as_uchar_vec (vX3_2PtDft_1).hi());
               vX01_2PtDft_2_hi =
                   (c7x::uchar_vec) (c7x::as_uchar_vec (vX0_2PtDft_2).hi(),
                                     c7x::as_uchar_vec (vX1_2PtDft_2).hi());
               vX23_2PtDft_2_hi =
                   (c7x::uchar_vec) (c7x::as_uchar_vec (vX2_2PtDft_2).hi(),
                                     c7x::as_uchar_vec (vX3_2PtDft_2).hi());
#endif

               vXPermOut   = __vpermeeq_yvvv (vXPermCtrl, vX23_2PtDft_1_lo,
                                              vX01_2PtDft_1_lo);
               vX0X1X2X3_0 = c7x::as_cshort_vec (
                   __shift_right_round (c7x::as_short_vec (vXPermOut), vShift));
               vXPermOut   = __vpermeeq_yvvv (vXPermCtrl, vX23_2PtDft_2_lo,
                                              vX01_2PtDft_2_lo);
               vX0X1X2X3_1 = c7x::as_cshort_vec (
                   __shift_right_round (c7x::as_short_vec (vXPermOut), vShift));
               vXPermOut   = __vpermooq_yvvv (vXPermCtrl, vX23_2PtDft_1_lo,
                                              vX01_2PtDft_1_lo);
               vX0X1X2X3_2 = c7x::as_cshort_vec (
                   __shift_right_round (c7x::as_short_vec (vXPermOut), vShift));
               vXPermOut   = __vpermooq_yvvv (vXPermCtrl, vX23_2PtDft_2_lo,
                                              vX01_2PtDft_2_lo);
               vX0X1X2X3_3 = c7x::as_cshort_vec (
                   __shift_right_round (c7x::as_short_vec (vXPermOut), vShift));
               vXPermOut   = __vpermeeq_yvvv (vXPermCtrl, vX23_2PtDft_1_hi,
                                              vX01_2PtDft_1_hi);
               vX0X1X2X3_4 = c7x::as_cshort_vec (
                   __shift_right_round (c7x::as_short_vec (vXPermOut), vShift));
               vXPermOut   = __vpermeeq_yvvv (vXPermCtrl, vX23_2PtDft_2_hi,
                                              vX01_2PtDft_2_hi);
               vX0X1X2X3_5 = c7x::as_cshort_vec (
                   __shift_right_round (c7x::as_short_vec (vXPermOut), vShift));
               vXPermOut   = __vpermooq_yvvv (vXPermCtrl, vX23_2PtDft_1_hi,
                                              vX01_2PtDft_1_hi);
               vX0X1X2X3_6 = c7x::as_cshort_vec (
                   __shift_right_round (c7x::as_short_vec (vXPermOut), vShift));
               vXPermOut   = __vpermooq_yvvv (vXPermCtrl, vX23_2PtDft_2_hi,
                                              vX01_2PtDft_2_hi);
               vX0X1X2X3_7 = c7x::as_cshort_vec (
                   __shift_right_round (c7x::as_short_vec (vXPermOut), vShift));

               /* *addr = vX0X1X2X3_0; */
               /* *addr = vX0X1X2X3_1; */
               /* *addr = vX0X1X2X3_2; */
               /* *addr = vX0X1X2X3_3; */
               /* *addr = vX0X1X2X3_4; */
               /* *addr = vX0X1X2X3_5; */
               /* *addr = vX0X1X2X3_6; */
               /* *addr = vX0X1X2X3_7; */

               __vpred tmp;
               CVP     addr;
               tmp  = c7x::strm_agen<0, CV>::get_vpred ();
               addr = c7x::strm_agen<0, CV>::get_adv (pYLocal);
               __vstore_pred (tmp, addr, vX0X1X2X3_0);

               tmp  = c7x::strm_agen<0, CV>::get_vpred ();
               addr = c7x::strm_agen<0, CV>::get_adv (pYLocal);
               __vstore_pred (tmp, addr, vX0X1X2X3_1);

               tmp  = c7x::strm_agen<0, CV>::get_vpred ();
               addr = c7x::strm_agen<0, CV>::get_adv (pYLocal);
               __vstore_pred (tmp, addr, vX0X1X2X3_2);

               tmp  = c7x::strm_agen<0, CV>::get_vpred ();
               addr = c7x::strm_agen<0, CV>::get_adv (pYLocal);
               __vstore_pred (tmp, addr, vX0X1X2X3_3);

               tmp  = c7x::strm_agen<0, CV>::get_vpred ();
               addr = c7x::strm_agen<0, CV>::get_adv (pYLocal);
               __vstore_pred (tmp, addr, vX0X1X2X3_4);

               tmp  = c7x::strm_agen<0, CV>::get_vpred ();
               addr = c7x::strm_agen<0, CV>::get_adv (pYLocal);
               __vstore_pred (tmp, addr, vX0X1X2X3_5);

               tmp  = c7x::strm_agen<0, CV>::get_vpred ();
               addr = c7x::strm_agen<0, CV>::get_adv (pYLocal);
               __vstore_pred (tmp, addr, vX0X1X2X3_6);

               tmp  = c7x::strm_agen<0, CV>::get_vpred ();
               addr = c7x::strm_agen<0, CV>::get_adv (pYLocal);
               __vstore_pred (tmp, addr, vX0X1X2X3_7);
            }

            /* loop_profile[loop][0] = (uint32_t) (__TSC - tsc); */
            /* loop_profile[loop][1] = numPointsPerDft; */
            /* loop_profile[loop][2] = 6; // LOOP6 */
            loop++;

            __SE0_CLOSE ();
            __SA0_CLOSE ();
         }
#if (!NUMPT32_ONLY)
         else {
            CV vX0_2PtDft_1_shifted, vX0_2PtDft_2_shifted;
            CV vX1_2PtDft_1_shifted, vX1_2PtDft_2_shifted;
            CV vX2_2PtDft_1_shifted, vX2_2PtDft_2_shifted;
            CV vX3_2PtDft_1_shifted, vX3_2PtDft_2_shifted;

            // * SE_LOOP7 - 3D * TRANSPOSE_32BIT - { numPoints >> 4; 16,
            // numPoints >> 4; numChannels, numPoints}

#if (ALIGNED_8PT)
            if (numPoints == 128)
               se0_param = *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                                                   SE_LOOP6_PARAM_OFFSET));
            else
#endif /* ALIGNED_8PT */
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

            pY0 = (cshort*) (pY + 0x00000000u);
            pY1 = (cshort*) (pY + ((0x80000000u >> numLeadingZeros) << 1));
            pY2 = (cshort*) (pY + ((0x20000000u >> numLeadingZeros) << 1));
            pY3 = (cshort*) (pY + ((0xA0000000u >> numLeadingZeros) << 1));
            pY4 = (cshort*) (pY + ((0x40000000u >> numLeadingZeros) << 1));
            pY5 = (cshort*) (pY + ((0xC0000000u >> numLeadingZeros) << 1));
            pY6 = (cshort*) (pY + ((0x60000000u >> numLeadingZeros) << 1));
            pY7 = (cshort*) (pY + ((0xE0000000u >> numLeadingZeros) << 1));

#ifdef CL7X_HE_CFLOAT_PTR_BUG
            short *myPY0 = (short *) pY0;
            short *myPY1 = (short *) pY1;
            short *myPY2 = (short *) pY2;
            short *myPY3 = (short *) pY3;
            short *myPY4 = (short *) pY4;
            short *myPY5 = (short *) pY5;
            short *myPY6 = (short *) pY6;
            short *myPY7 = (short *) pY7;
#endif

#if (ALIGNED_8PT)
            if (numPoints == 128)
               for (l = 0; l < numChannels; l++) {
                  for (k = 0; k < numPoints >> 4; k += 8) {
                     offsetBitReverse = __bit_reverse (k) >> numLeadingZeros;

                     // clang-format off
                        #if __C7X_HOSTEM__
                        static const
                            c7x::uchar_vec vXPermCtrl2  = c7x::uchar_vec(
                                0x00, 0x01, 0x02, 0x03, 0x20, 0x21, 0x22, 0x23,
                                0x04, 0x05, 0x06, 0x07, 0x24, 0x25, 0x26, 0x27,
                                0x08, 0x09, 0x0A, 0x0B, 0x28, 0x29, 0x2A, 0x2B,
                                0x0C, 0x0D, 0x0E, 0x0F, 0x2C, 0x2D, 0x2E, 0x2F,
                                0x10, 0x11, 0x12, 0x13, 0x30, 0x31, 0x32, 0x33,
                                0x14, 0x15, 0x16, 0x17, 0x34, 0x35, 0x36, 0x37,
                                0x18, 0x19, 0x1A, 0x1B, 0x38, 0x39, 0x3A, 0x3B,
                                0x1C, 0x1D, 0x1E, 0x1F, 0x3C, 0x3D, 0x3E, 0x3F);
                        #else
                        static const
                            c7x::uchar_vec vXPermCtrl2  = (c7x::uchar_vec)(
                                0x00, 0x01, 0x02, 0x03, 0x20, 0x21, 0x22, 0x23,
                                0x04, 0x05, 0x06, 0x07, 0x24, 0x25, 0x26, 0x27,
                                0x08, 0x09, 0x0A, 0x0B, 0x28, 0x29, 0x2A, 0x2B,
                                0x0C, 0x0D, 0x0E, 0x0F, 0x2C, 0x2D, 0x2E, 0x2F,
                                0x10, 0x11, 0x12, 0x13, 0x30, 0x31, 0x32, 0x33,
                                0x14, 0x15, 0x16, 0x17, 0x34, 0x35, 0x36, 0x37,
                                0x18, 0x19, 0x1A, 0x1B, 0x38, 0x39, 0x3A, 0x3B,
                                0x1C, 0x1D, 0x1E, 0x1F, 0x3C, 0x3D, 0x3E, 0x3F);
#endif

                        ///#define PACK_HIGH_INT16( V1, V2) __vpermoow_yvvv( vXPermCtrl2, c7x::as_uchar_vec( V1), c7x::as_uchar_vec( V2))
                     // clang-format on

                     CV v0, v1, v2, v3, v4, v5, v6, v7;

                     v0 = c7x::strm_eng<0, CV>::get_adv ();
                     v1 = c7x::strm_eng<0, CV>::get_adv ();
                     v2 = c7x::strm_eng<0, CV>::get_adv ();
                     v3 = c7x::strm_eng<0, CV>::get_adv ();
                     v4 = c7x::strm_eng<0, CV>::get_adv ();
                     v5 = c7x::strm_eng<0, CV>::get_adv ();
                     v6 = c7x::strm_eng<0, CV>::get_adv ();
                     v7 = c7x::strm_eng<0, CV>::get_adv ();

                     vX_0      = c7x::as_cshort_vec (__pack_low (
                              c7x::as_int_vec (v4), c7x::as_int_vec (v0)));
                     vX_0_1    = c7x::as_cshort_vec (PACK_HIGH_INT16 (
                            c7x::as_int_vec (v4), c7x::as_int_vec (v0)));
                     vX_N_4    = c7x::as_cshort_vec (__pack_low (
                            c7x::as_int_vec (v5), c7x::as_int_vec (v1)));
                     vX_N_4_1  = c7x::as_cshort_vec (PACK_HIGH_INT16 (
                          c7x::as_int_vec (v5), c7x::as_int_vec (v1)));
                     vX_N_2    = c7x::as_cshort_vec (__pack_low (
                            c7x::as_int_vec (v6), c7x::as_int_vec (v2)));
                     vX_N_2_1  = c7x::as_cshort_vec (PACK_HIGH_INT16 (
                          c7x::as_int_vec (v6), c7x::as_int_vec (v2)));
                     vX_3N_4   = c7x::as_cshort_vec (__pack_low (
                           c7x::as_int_vec (v7), c7x::as_int_vec (v3)));
                     vX_3N_4_1 = c7x::as_cshort_vec (PACK_HIGH_INT16 (
                         c7x::as_int_vec (v7), c7x::as_int_vec (v3)));

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

                     vX0_2PtDft_1_shifted =
                         c7x::as_cshort_vec (__shift_right_round (
                             c7x::as_short_vec (vX0_2PtDft_1), vShift));
                     vX0_2PtDft_2_shifted =
                         c7x::as_cshort_vec (__shift_right_round (
                             c7x::as_short_vec (vX0_2PtDft_2), vShift));
                     vX1_2PtDft_1_shifted =
                         c7x::as_cshort_vec (__shift_right_round (
                             c7x::as_short_vec (vX1_2PtDft_1), vShift));
                     vX1_2PtDft_2_shifted =
                         c7x::as_cshort_vec (__shift_right_round (
                             c7x::as_short_vec (vX1_2PtDft_2), vShift));
                     vX2_2PtDft_1_shifted =
                         c7x::as_cshort_vec (__shift_right_round (
                             c7x::as_short_vec (vX2_2PtDft_1), vShift));
                     vX2_2PtDft_2_shifted =
                         c7x::as_cshort_vec (__shift_right_round (
                             c7x::as_short_vec (vX2_2PtDft_2), vShift));
                     vX3_2PtDft_1_shifted =
                         c7x::as_cshort_vec (__shift_right_round (
                             c7x::as_short_vec (vX3_2PtDft_1), vShift));
                     vX3_2PtDft_2_shifted =
                         c7x::as_cshort_vec (__shift_right_round (
                             c7x::as_short_vec (vX3_2PtDft_2), vShift));

                     __vstore_reverse_bit ((CVP) (pY0 + offsetBitReverse),
                                           vX0_2PtDft_1_shifted);
                     __vstore_reverse_bit ((CVP) (pY1 + offsetBitReverse),
                                           vX0_2PtDft_2_shifted);
                     __vstore_reverse_bit ((CVP) (pY2 + offsetBitReverse),
                                           vX1_2PtDft_1_shifted);
                     __vstore_reverse_bit ((CVP) (pY3 + offsetBitReverse),
                                           vX1_2PtDft_2_shifted);
                     __vstore_reverse_bit ((CVP) (pY4 + offsetBitReverse),
                                           vX2_2PtDft_1_shifted);
                     __vstore_reverse_bit ((CVP) (pY5 + offsetBitReverse),
                                           vX2_2PtDft_2_shifted);
                     __vstore_reverse_bit ((CVP) (pY6 + offsetBitReverse),
                                           vX3_2PtDft_1_shifted);
                     __vstore_reverse_bit ((CVP) (pY7 + offsetBitReverse),
                                           vX3_2PtDft_2_shifted);
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

                  pY0 = (cshort*) myPY0;
                  pY1 = (cshort*) myPY1;
                  pY2 = (cshort*) myPY2;
                  pY3 = (cshort*) myPY3;
                  pY4 = (cshort*) myPY4;
                  pY5 = (cshort*) myPY5;
                  pY6 = (cshort*) myPY6;
                  pY7 = (cshort*) myPY7;

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
            else
#endif /* ALIGNED_8PT */
               for (l = 0; l < numChannels; l++) {
                  for (k = 0; k < numPoints >> 4; k += 8) {
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

                     vX0_2PtDft_1_shifted =
                         c7x::as_cshort_vec (__shift_right_round (
                             c7x::as_short_vec (vX0_2PtDft_1), vShift));
                     vX0_2PtDft_2_shifted =
                         c7x::as_cshort_vec (__shift_right_round (
                             c7x::as_short_vec (vX0_2PtDft_2), vShift));
                     vX1_2PtDft_1_shifted =
                         c7x::as_cshort_vec (__shift_right_round (
                             c7x::as_short_vec (vX1_2PtDft_1), vShift));
                     vX1_2PtDft_2_shifted =
                         c7x::as_cshort_vec (__shift_right_round (
                             c7x::as_short_vec (vX1_2PtDft_2), vShift));
                     vX2_2PtDft_1_shifted =
                         c7x::as_cshort_vec (__shift_right_round (
                             c7x::as_short_vec (vX2_2PtDft_1), vShift));
                     vX2_2PtDft_2_shifted =
                         c7x::as_cshort_vec (__shift_right_round (
                             c7x::as_short_vec (vX2_2PtDft_2), vShift));
                     vX3_2PtDft_1_shifted =
                         c7x::as_cshort_vec (__shift_right_round (
                             c7x::as_short_vec (vX3_2PtDft_1), vShift));
                     vX3_2PtDft_2_shifted =
                         c7x::as_cshort_vec (__shift_right_round (
                             c7x::as_short_vec (vX3_2PtDft_2), vShift));

                     __vstore_reverse_bit ((CVP) (pY0 + offsetBitReverse),
                                           vX0_2PtDft_1_shifted);
                     __vstore_reverse_bit ((CVP) (pY1 + offsetBitReverse),
                                           vX0_2PtDft_2_shifted);
                     __vstore_reverse_bit ((CVP) (pY2 + offsetBitReverse),
                                           vX1_2PtDft_1_shifted);
                     __vstore_reverse_bit ((CVP) (pY3 + offsetBitReverse),
                                           vX1_2PtDft_2_shifted);
                     __vstore_reverse_bit ((CVP) (pY4 + offsetBitReverse),
                                           vX2_2PtDft_1_shifted);
                     __vstore_reverse_bit ((CVP) (pY5 + offsetBitReverse),
                                           vX2_2PtDft_2_shifted);
                     __vstore_reverse_bit ((CVP) (pY6 + offsetBitReverse),
                                           vX3_2PtDft_1_shifted);
                     __vstore_reverse_bit ((CVP) (pY7 + offsetBitReverse),
                                           vX3_2PtDft_2_shifted);
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

                  pY0 = (cshort*) myPY0;
                  pY1 = (cshort*) myPY1;
                  pY2 = (cshort*) myPY2;
                  pY3 = (cshort*) myPY3;
                  pY4 = (cshort*) myPY4;
                  pY5 = (cshort*) myPY5;
                  pY6 = (cshort*) myPY6;
                  pY7 = (cshort*) myPY7;

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

            /* loop_profile[loop][0] = (uint32_t) (__TSC - tsc); */
            /* loop_profile[loop][1] = numPointsPerDft; */
            /* loop_profile[loop][2] = 7; // LOOP7 */
            loop++;

            __SE0_CLOSE ();
         }
#endif /* NUMPT32_ONLY */
      }

      /* uint32_t ll; */
      /* printf( "numPoints = %d; LOOP, NP, cyc:", numPoints); */
      /* for( ll = 0; ll < loop; ll++) */
      /*     printf( " %d, %d, %d;", loop_profile[ll][2],
       * loop_profile[ll][1], loop_profile[ll][0]); */
      /* printf( "\n"); */
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
   else if (bufParamsX->dim_x != bufParamsY->dim_x) {
      status = FFTLIB_ERR_INVALID_DIMENSION;
   }
   else if (bufParamsX->dim_x < numPoints * numChannels * 2) {
      /* In general, dim_x == numPoints*numChannels*2. However, optimized
       * kernel requires dim_x to be atleast 128*2. Hence, for small
       * values of numPoints*numChannels, dim_x could be greater than
       * numPoints*numChannels*2 */
      status = FFTLIB_ERR_INVALID_DIMENSION;
   }
   else if (bufParamsX->dim_x < 128 * 2) {
      status = FFTLIB_ERR_INVALID_DIMENSION;
   }
   else if (bufParamsW->dim_x != numPoints * 2) {
      status = FFTLIB_ERR_INVALID_DIMENSION;
   }
   else if ((bufParamsX->data_type != FFTLIB_INT16) ||
            (bufParamsW->data_type != FFTLIB_INT16) ||
            (bufParamsY->data_type != FFTLIB_INT16)) {
      status = FFTLIB_ERR_INVALID_TYPE;
   }
   else if (bufParamsShift->data_type != FFTLIB_UINT32) {
      status = FFTLIB_ERR_INVALID_TYPE;
   }
   else if (((uint64_t) pX) & 0xFu) { /* pX must be 16-byte aligned for a */
      status = FFTLIB_ERR_NOT_ALIGNED_PTRS_STRIDES; /* streaming engine
                                                       configuration */
   }
   else if (((1u << (bufParamsShift->dim_x * 2)) * 2 !=
             bufParamsW->dim_x) && /* check for powers of 4 */
            ((1u << (bufParamsShift->dim_x * 2)) * 2 * 2 !=
             bufParamsW->dim_x)) { /* check for powers of 2 */
      status = FFTLIB_ERR_INVALID_DIMENSION;
   }
   else {
      /* if ((numChannels != 1) && (numChannels != 2) && (numChannels !=
       * 4)
       * &&
       */
      /*     (numChannels != 8) && (numChannels != 16)) { */
      /*    status = FFTLIB_ERR_INVALID_DIMENSION; */
      /* } */
   }

   return (status);
}

#endif
