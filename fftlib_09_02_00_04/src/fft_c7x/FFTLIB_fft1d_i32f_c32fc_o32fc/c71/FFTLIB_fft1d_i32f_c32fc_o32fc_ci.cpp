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

#include "../../../common/c71/FFTLIB_debug.h"
#include "../../FFTLIB_fft1dBatched_i32fc_c32fc_o32fc/FFTLIB_fft1dBatched_i32fc_c32fc_o32fc.h"
#include "../../FFTLIB_fft1d_i32fc_c32fc_o32fc/FFTLIB_fft1d_i32fc_c32fc_o32fc.h"
#include "../FFTLIB_fft1d_i32f_c32fc_o32fc.h"

#define SE_PARAM_BASE         (0x0000)
#define SE0_PARAM_OFFSET      (SE_PARAM_BASE)
#define SE1_PARAM_OFFSET      (SE0_PARAM_OFFSET + SE_PARAM_SIZE)
#define SE1_PARAM_OFFSET_LAST (SE1_PARAM_OFFSET + SE_PARAM_SIZE)
#define SA0_PARAM_OFFSET      (SE1_PARAM_OFFSET_LAST + SE_PARAM_SIZE)
#define SA1_PARAM_OFFSET      (SA0_PARAM_OFFSET + SE_PARAM_SIZE)

typedef typename c7x::cfloat_vec CV;
typedef typename c7x::float_vec  V;

FFTLIB_STATUS
FFTLIB_fft1d_i32f_c32fc_o32fc_init (FFTLIB_F32           *pX,
                                    FFTLIB_bufParams1D_t *bufParamsX,
                                    FFTLIB_F32           *pW,
                                    FFTLIB_bufParams1D_t *bufParamsW,
                                    FFTLIB_F32           *pXFFT,
                                    FFTLIB_bufParams1D_t *bufParamsXFFT,
                                    FFTLIB_F32           *pSf,
                                    FFTLIB_bufParams1D_t *bufParamsSf,
                                    FFTLIB_F32           *pY,
                                    FFTLIB_bufParams1D_t *bufParamsY,
                                    void                 *pBlock)
{
   FFTLIB_STATUS status = FFTLIB_SUCCESS;

   /* #if defined(FFTLIB_CHECK_PARAMS) || \ */
   /*     defined(FFTLIB_FFT1D_I32FC_C32FC_O32FC_CHECK_PARAMS) */
   /*    status = FFTLIB_fft1d_i32f_c32fc_o32fc_checkParams ( */
   /*        pX, bufParamsX, pW, bufParamsW, pY, bufParamsY, pBlock); */
   /*    if (status == FFTLIB_SUCCESS) */
   /* #endif */
   {

      uint32_t         numPoints;
      __SE_TEMPLATE_v1 se0_param = __gen_SE_TEMPLATE_v1 ();
      __SE_TEMPLATE_v1 se1_param = __gen_SE_TEMPLATE_v1 ();
      __SA_TEMPLATE_v1 sa0_param = __gen_SA_TEMPLATE_v1 ();
      __SA_TEMPLATE_v1 sa1_param = __gen_SA_TEMPLATE_v1 ();

      // se1 params for last loop
      __SE_TEMPLATE_v1     se1_param_last = __gen_SE_TEMPLATE_v1 ();
      FFTLIB_bufParams1D_t bufParamsX_FFT;

      numPoints = bufParamsX->dim_x;

      bufParamsX_FFT.dim_x = numPoints;

      FFTLIB_fft1dBatched_i32fc_c32fc_o32fc_init (
          (FFTLIB_F32 *) pX, &bufParamsX_FFT, (FFTLIB_F32 *) pW,
          &bufParamsX_FFT, (FFTLIB_F32 *) pXFFT, &bufParamsX_FFT,
          (numPoints >> 1), 1, &((uint8_t *) pBlock)[5 * SE_PARAM_SIZE]);

      uint32_t elementCount = c7x::element_count_of<CV>::value;
      uint32_t SEBlocks     = (numPoints >> 1) / elementCount;
      uint32_t offset = (((uint64_t) pSf - (uint64_t) pXFFT)) / sizeof (cfloat);

      se0_param.ICNT0 = elementCount;
      se0_param.ICNT1 = 2;
      se0_param.DIM1  = offset;
      se0_param.ICNT2 = SEBlocks;
      se0_param.DIM2  = elementCount;

      se0_param.ELETYPE = __SE_ELETYPE_32BIT_CMPLX_SWAP;
      se0_param.VECLEN  = c7x::se_veclen<CV>::value;
      se0_param.DIMFMT  = __SE_DIMFMT_3D;
      *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET)) =
          se0_param;

      se1_param.ICNT0 = elementCount;
      se1_param.ICNT1 = 2;
      se1_param.DIM1  = offset + (numPoints >> 1) - 1;
      se1_param.ICNT2 = SEBlocks;
      se1_param.DIM2  = -elementCount;

      se1_param.DIR     = __SE_DIR_DEC;
      se1_param.ELETYPE = __SE_ELETYPE_32BIT_CMPLX_SWAP;
      se1_param.VECLEN  = c7x::se_veclen<CV>::value;
      se1_param.DIMFMT  = __SE_DIMFMT_3D;
      *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET)) =
          se1_param;

      /* sa0_param = (0); */
      sa0_param.ICNT0  = numPoints >> 1;
      sa0_param.VECLEN = c7x::sa_veclen<CV>::value;
      sa0_param.DIMFMT = __SA_DIMFMT_1D;
      *((__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET)) =
          sa0_param;

      se1_param_last.ICNT0   = numPoints >> 1;
      se1_param_last.DIR     = __SE_DIR_DEC;
      se1_param_last.ELETYPE = __SE_ELETYPE_32BIT_CMPLX_NOSWAP;
      se1_param_last.VECLEN  = c7x::se_veclen<CV>::value;
      se1_param_last.DIMFMT  = __SE_DIMFMT_1D;
      *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET_LAST)) =
          se1_param_last;

      sa1_param.ICNT0  = elementCount;
      sa1_param.ICNT1  = (numPoints >> 1) / (elementCount);
      sa1_param.DIM1   = -(elementCount);
      sa1_param.VECLEN = c7x::sa_veclen<CV>::value;
      sa1_param.DIMFMT = __SA_DIMFMT_2D;
      *((__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA1_PARAM_OFFSET)) =
          sa1_param;
   }
   return (status);
}

FFTLIB_STATUS
FFTLIB_fft1d_i32f_c32fc_o32fc_kernel (FFTLIB_F32 *restrict pX,
                                      FFTLIB_bufParams1D_t *bufParamsX,
                                      FFTLIB_F32 *restrict pW,
                                      FFTLIB_bufParams1D_t *bufParamsW,
                                      FFTLIB_F32 *restrict pXFFT,
                                      FFTLIB_bufParams1D_t *bufParamsXFFT,
                                      FFTLIB_F32 *restrict pSf,
                                      FFTLIB_bufParams1D_t *bufParamsSf,
                                      FFTLIB_F32 *restrict pY,
                                      FFTLIB_bufParams1D_t *bufParamsY,
                                      void                 *pBlock)
{

   FFTLIB_STATUS status = FFTLIB_SUCCESS;

   uint32_t             numPoints;
   FFTLIB_bufParams1D_t bufParamsX_FFT;

   numPoints            = bufParamsX->dim_x;
   bufParamsX_FFT.dim_x = numPoints;

   /* typedef typename c7x::cfloat_vec CV; */
   /* typedef typename c7x::float_vec  V; */

   FFTLIB_fft1dBatched_i32fc_c32fc_o32fc_kernel (
       (FFTLIB_F32 *) pX, &bufParamsX_FFT, (FFTLIB_F32 *) pW, &bufParamsX_FFT,
       (FFTLIB_F32 *) pXFFT, &bufParamsX_FFT, (numPoints >> 1), 1,
       &((uint8_t *) pBlock)[5 * SE_PARAM_SIZE]);

   FFTLIB_asm (" MARK 6");

   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1 (); // SE parameter vector
   __SE_TEMPLATE_v1 se1Params = __gen_SE_TEMPLATE_v1 (); // SE parameter vector

   /* __SE_TEMPLATE_v1 se1ParamsLast = __gen_SE_TEMPLATE_v1 (); */
   __SA_TEMPLATE_v1 sa0Params = __gen_SA_TEMPLATE_v1 ();
   __SA_TEMPLATE_v1 sa1Params = __gen_SA_TEMPLATE_v1 ();

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET);
   se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET);
   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET);
   sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA1_PARAM_OFFSET);

   /* se1ParamsLast = */
   /*     *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET_LAST); */

   __SE0_OPEN (&(pXFFT[0]), se0Params);
   __SE1_OPEN (&(pXFFT[numPoints + 2]), se1Params);
   __SA0_OPEN (sa0Params);
   __SA1_OPEN (sa1Params);

   CV var0, var1;
   CV temp0, temp1;
   CV sFA, sFB;
   CV regStore0, regStore1;

   __vpred tmp;
   CV     *addr;

#if defined(_HOST_BUILD)
   c7x::ulong_vec xorVec = (c7x::ulong_vec) (0x0000000080000000);

#else
   c7x::ulong_vec xorVec = (0x0000000080000000);
#endif

   uint32_t i            = 0;
   uint32_t elementCount = c7x::element_count_of<CV>::value;

   /* printf ("\npXFFT Opt below:\n\n"); */
   /* for (i = 0; i < numPoints; i++) { */
   /*    printf ("%.2f, ", pXFFT[i]); */
   /* } */
   /* printf ("\n\n"); */

   /* printf ("\npXFFT:%p, pSf:%p, pSf- pXFFT:%d\n", pXFFT, pSf, */
   /*         ((uint64_t) pSf - (uint64_t) pXFFT)); */

   FFTLIB_asm (" MARK 8");

#pragma MUST_ITERATE(8, , 8)
#pragma UNROLL(8)
   for (i = 0; i < (numPoints >> 1) / (elementCount); i++) {
      var0 = c7x::strm_eng<0, CV>::get_adv ();
      var1 = c7x::strm_eng<1, CV>::get_adv ();

      sFA = c7x::strm_eng<0, CV>::get_adv ();
      sFB = c7x::strm_eng<1, CV>::get_adv ();

      temp0 = (__complex_multiply (var0, c7x::as_cfloat_vec (sFA)));
      temp1 = (__complex_conjugate_multiply (var1, c7x::as_cfloat_vec (sFB)));

      regStore0 = temp0 + temp1;
      tmp       = c7x::strm_agen<0, CV>::get_vpred ();
      addr      = c7x::strm_agen<0, CV>::get_adv (&pY[0]);
      __vstore_pred (tmp, addr, regStore0);

      regStore1 =
          c7x::as_cfloat_vec ((c7x::as_ulong_vec (regStore0)) ^ (xorVec));

      tmp  = c7x::strm_agen<1, CV>::get_vpred ();
      addr = c7x::strm_agen<1, CV>::get_adv (
          &pY[2 * numPoints + 2 - (elementCount << 1)]);
      __vstore_pred (tmp, addr, (__reverse (regStore1)));
   }
   FFTLIB_asm (" MARK 9");

   __SA0_CLOSE ();
   __SA1_CLOSE ();
   /* __SA2_CLOSE (); */
   __SE0_CLOSE ();
   __SE1_CLOSE ();

   /* __SE1_OPEN (&(pY[numPoints]), se1ParamsLast); */
   /* __SA0_OPEN (sa0Params); */

   /* FFTLIB_asm (" MARK 10"); */
   /* FFTLIB_UNROLL (2) */
   /* for (i = 0; i < (numPoints >> 1) / (elementCount); i++) { */
   /*    var1 = c7x::strm_eng<1, CV>::get_adv (); */
   /*    var1 = c7x::as_cfloat_vec ((c7x::as_ulong_vec (var1)) ^ (xorVec)); */

   /*    tmp  = c7x::strm_agen<0, V>::get_vpred (); */
   /*    addr = c7x::strm_agen<0, V>::get_adv (&pY[2 + numPoints]); */

   /*    __vstore_pred (tmp, addr, c7x::as_float_vec (var1)); */
   /* } */
   /* FFTLIB_asm (" MARK 11"); */

   /* __SE1_CLOSE (); */
   /* __SA0_CLOSE (); */

   /*********************************/
   /* Write pY[0] and pY[numPoints] */
   /*********************************/

   cfloat var2, temp2, temp3;

   // pY[0]
   var2 = ((cfloat *) pXFFT)[0];

   temp2 = __complex_multiply (var2, (((cfloat *) pSf)[0]));
   temp3 =
       __complex_conjugate_multiply (var2, (((cfloat *) pSf)[numPoints - 1]));

   ((cfloat *) pY)[0] = temp2 + temp3;
   /* printf ("%.2f, %.2f\n", var2.r (), var2.i ()); */

   // pY[numPoints]
   pY[numPoints]     = pXFFT[0] - pXFFT[1];
   pY[numPoints + 1] = 0;

   FFTLIB_asm (" MARK 7");

   /* printf ("pY Opt below:\n"); */
   /* for (i = 0; i < 2 * numPoints; i += 2) { */
   /*    printf ("%d: %.2f, %.2f\n", i >> 1, pY[i], pY[i + 1]); */
   /* } */
   /* printf ("\n\n"); */

   return status;
}

FFTLIB_STATUS
FFTLIB_fft1d_i32f_c32fc_o32fc_checkParams (FFTLIB_F32           *pX,
                                           FFTLIB_bufParams1D_t *bufParamsX,
                                           FFTLIB_F32           *pW,
                                           FFTLIB_bufParams1D_t *bufParamsW,
                                           FFTLIB_F32           *pY,
                                           FFTLIB_bufParams1D_t *bufParamsY,
                                           void                 *pBlock)
{
   FFTLIB_STATUS status = FFTLIB_SUCCESS;

   if ((pX == NULL) || (pW == NULL) || (pY == NULL)) {
      status = FFTLIB_ERR_NULL_POINTER;
   }
   else if (bufParamsX->dim_x != bufParamsW->dim_x ||
            bufParamsX->dim_x != bufParamsY->dim_x) {
      status = FFTLIB_ERR_INVALID_DIMENSION;
   }
   else if (bufParamsX->dim_x < 64 * 2) { /* Minimum number of points is 64
                                           */
      status = FFTLIB_ERR_INVALID_DIMENSION;
   }
   else if ((bufParamsX->data_type != FFTLIB_FLOAT32) ||
            (bufParamsW->data_type != FFTLIB_FLOAT32) ||
            (bufParamsY->data_type != FFTLIB_FLOAT32)) {
      status = FFTLIB_ERR_INVALID_TYPE;
   }
   else if (((uint64_t) pX) & 0xFu) { /* pX must be 16-byte aligned for a
                                       */
      status = FFTLIB_ERR_NOT_ALIGNED_PTRS_STRIDES; /* streaming engine
                                                       configuration */
   }
   else {
      /* Check if number of pts is a power of 2 */
      uint32_t k = 0;
      while (k < 32) {
         if (bufParamsX->dim_x & (1u << k)) {
            break;
         }
         k++;
      }
      if ((1u << k) != bufParamsX->dim_x) {
         status = FFTLIB_ERR_INVALID_DIMENSION;
      }
   }
   return (status);
}
