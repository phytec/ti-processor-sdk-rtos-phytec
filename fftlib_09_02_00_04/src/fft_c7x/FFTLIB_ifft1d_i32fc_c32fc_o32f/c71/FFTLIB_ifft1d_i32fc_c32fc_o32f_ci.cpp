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
#include "../../FFTLIB_ifft1d_i32fc_c32fc_o32fc/FFTLIB_ifft1d_i32fc_c32fc_o32fc.h"
#include "../FFTLIB_ifft1d_i32fc_c32fc_o32f.h"

#define SE_PARAM_BASE (0x0000)
#define SE0_PARAM_OFFSET (SE_PARAM_BASE)
#define SE1_PARAM_OFFSET (SE0_PARAM_OFFSET + SE_PARAM_SIZE)
#define SE1_PARAM_OFFSET_LAST (SE1_PARAM_OFFSET + SE_PARAM_SIZE)
#define SA0_PARAM_OFFSET (SE1_PARAM_OFFSET_LAST + SE_PARAM_SIZE)
#define SA1_PARAM_OFFSET (SA0_PARAM_OFFSET + SE_PARAM_SIZE)

typedef typename c7x::cfloat_vec CV;
typedef typename c7x::float_vec  V;

FFTLIB_STATUS
FFTLIB_ifft1d_i32fc_c32fc_o32f_init (FFTLIB_F32 *          pX,
                                     FFTLIB_bufParams1D_t *bufParamsX,
                                     FFTLIB_F32 *          pW,
                                     FFTLIB_bufParams1D_t *bufParamsW,
                                     FFTLIB_F32 *          pXFFT,
                                     FFTLIB_bufParams1D_t *bufParamsXFFT,
                                     FFTLIB_F32 *          pSf,
                                     FFTLIB_bufParams1D_t *bufParamsSf,
                                     FFTLIB_F32 *          pY,
                                     FFTLIB_bufParams1D_t *bufParamsY,
                                     void *                pBlock)
{
   FFTLIB_STATUS status = FFTLIB_SUCCESS;

   /* #if defined(FFTLIB_CHECK_PARAMS) || \ */
   /*     defined(FFTLIB_FFT1D_I32FC_C32FC_O32FC_CHECK_PARAMS) */
   /*    status = FFTLIB_ifft1d_i32fc_c32fc_o32f_checkParams ( */
   /*        pX, bufParamsX, pW, bufParamsW, pY, bufParamsY, pBlock); */
   /*    if (status == FFTLIB_SUCCESS) */
   /* #endif */
   {

      uint32_t         numPoints;
      __SE_TEMPLATE_v1 se0_param = __gen_SE_TEMPLATE_v1 ();
      __SA_TEMPLATE_v1 sa0_param = __gen_SA_TEMPLATE_v1 ();

      FFTLIB_bufParams1D_t bufParamsX_FFT;

      numPoints = bufParamsX->dim_x;

      bufParamsX_FFT.dim_x = numPoints;

      FFTLIB_ifft1d_i32fc_c32fc_o32fc_init (
          (FFTLIB_F32 *) pXFFT, &bufParamsX_FFT, (FFTLIB_F32 *) pW,
          &bufParamsX_FFT, (FFTLIB_F32 *) pY, &bufParamsX_FFT,
          &((uint8_t *) pBlock)[5 * SE_PARAM_SIZE]);

      uint32_t elementCount = c7x::element_count_of<CV>::value;
      uint32_t SEBlocks     = (numPoints >> 1) / elementCount;
      uint32_t offset       = numPoints >> 1;

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

      sa0_param.ICNT0  = numPoints >> 1;
      sa0_param.VECLEN = c7x::sa_veclen<CV>::value;
      sa0_param.DIMFMT = __SA_DIMFMT_1D;
      *((__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET)) =
          sa0_param;
   }
   return (status);
}

FFTLIB_STATUS
FFTLIB_ifft1d_i32fc_c32fc_o32f_kernel (FFTLIB_F32 *restrict  pX,
                                       FFTLIB_bufParams1D_t *bufParamsX,
                                       FFTLIB_F32 *restrict  pW,
                                       FFTLIB_bufParams1D_t *bufParamsW,
                                       FFTLIB_F32 *restrict  pXFFT,
                                       FFTLIB_bufParams1D_t *bufParamsXFFT,
                                       FFTLIB_F32 *restrict  pSf,
                                       FFTLIB_bufParams1D_t *bufParamsSf,
                                       FFTLIB_F32 *restrict  pY,
                                       FFTLIB_bufParams1D_t *bufParamsY,
                                       void *                pBlock)
{

   FFTLIB_STATUS status = FFTLIB_SUCCESS;

   uint32_t             numPoints;
   FFTLIB_bufParams1D_t bufParamsX_FFT;

   numPoints            = bufParamsX->dim_x;
   bufParamsX_FFT.dim_x = numPoints;

   /* typedef typename c7x::cfloat_vec CV; */
   /* typedef typename c7x::float_vec  V; */

   __SE_TEMPLATE_v1 se0Params = __gen_SE_TEMPLATE_v1 (); // SE parameter vector

   __SA_TEMPLATE_v1 sa0Params = __gen_SA_TEMPLATE_v1 ();

   se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET);
   sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET);

   /* se1ParamsLast = */
   /*     *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET_LAST); */

   __SE0_OPEN (&(pX[0]), se0Params);
   __SE1_OPEN (&(pSf[0]), se0Params);
   __SA0_OPEN (sa0Params);
   /* __SA1_OPEN (sa1Params); */

   CV var0, var1;
   CV temp0, temp1;
   CV sFA, sFB;
   CV regStore0;

   __vpred tmp;
   CV *    addr;

   uint32_t i            = 0;
   uint32_t elementCount = c7x::element_count_of<CV>::value;

#pragma MUST_ITERATE(8, , 8)
#pragma UNROLL(8)
   for (i = 0; i < (numPoints >> 1) / (elementCount); i++) {
      var0 = c7x::strm_eng<0, CV>::get_adv ();
      var1 = c7x::strm_eng<0, CV>::get_adv ();

      sFA = c7x::strm_eng<1, CV>::get_adv ();
      sFB = c7x::strm_eng<1, CV>::get_adv ();

      temp0 = (__complex_multiply (var0, c7x::as_cfloat_vec (sFA)));
      temp1 = (__complex_multiply (var1, c7x::as_cfloat_vec (sFB)));

      regStore0 = temp0 + temp1;
      tmp       = c7x::strm_agen<0, CV>::get_vpred ();
      addr      = c7x::strm_agen<0, CV>::get_adv (&pXFFT[0]);
      __vstore_pred (tmp, addr, regStore0);
   }

   __SA0_CLOSE ();
   __SE0_CLOSE ();
   __SE1_CLOSE ();

   /* printf ("\nXFFT_ci:\n"); */
   /* for (i = 0; i < numPoints; i++) { */
   /*    printf ("%.2f ", pXFFT[i]); */
   /* } */

   FFTLIB_ifft1d_i32fc_c32fc_o32fc_kernel (
       (FFTLIB_F32 *) pXFFT, &bufParamsX_FFT, (FFTLIB_F32 *) pW,
       &bufParamsX_FFT, (FFTLIB_F32 *) pY, &bufParamsX_FFT,
       &((uint8_t *) pBlock)[5 * SE_PARAM_SIZE]);

   return status;
}

#if (!defined(FFTLIB_REMOVE_CHECK_PARAMS) &&                                   \
     !defined(FFTLIB_FFT1D_I32FC_C32FC_O32FC_REMOVE_CHECK_PARAMS)) ||          \
    (defined(FFTLIB_CHECK_PARAMS)) ||                                          \
    (defined(FFTLIB_FFT1D_I32FC_C32FC_O32FC_CHECK_PARAMS))

FFTLIB_STATUS
FFTLIB_ifft1d_i32fc_c32fc_o32f_checkParams (FFTLIB_F32 *          pX,
                                            FFTLIB_bufParams1D_t *bufParamsX,
                                            FFTLIB_F32 *          pW,
                                            FFTLIB_bufParams1D_t *bufParamsW,
                                            FFTLIB_F32 *          pY,
                                            FFTLIB_bufParams1D_t *bufParamsY,
                                            void *                pBlock)
{
   FFTLIB_STATUS status = FFTLIB_SUCCESS;

   /* if ((pX == NULL) || (pW == NULL) || (pY == NULL)) { */
   /*    status = FFTLIB_ERR_NULL_POINTER; */
   /* } */
   /* else if (bufParamsX->dim_x != bufParamsW->dim_x || */
   /*          bufParamsX->dim_x != bufParamsY->dim_x) { */
   /*    status = FFTLIB_ERR_INVALID_DIMENSION; */
   /* } */
   /* else if (bufParamsX->dim_x < 64 * 2) { /\* Minimum number of points is 64
    * *\/ */
   /*    status = FFTLIB_ERR_INVALID_DIMENSION; */
   /* } */
   /* else if ((bufParamsX->data_type != FFTLIB_FLOAT32) || */
   /*          (bufParamsW->data_type != FFTLIB_FLOAT32) || */
   /*          (bufParamsY->data_type != FFTLIB_FLOAT32)) { */
   /*    status = FFTLIB_ERR_INVALID_TYPE; */
   /* } */
   /* else if (((uint64_t) pX) & 0xFu) { /\* pX must be 16-byte aligned for a
    * *\/ */
   /*    status = FFTLIB_ERR_NOT_ALIGNED_PTRS_STRIDES; /\* streaming engine */
   /*                                                     configuration *\/ */
   /* } */
   /* else { */
   /*    /\* Check if number of pts is a power of 2 *\/ */
   /*    uint32_t k = 0; */
   /*    while (k < 32) { */
   /*       if (bufParamsX->dim_x & (1u << k)) { */
   /*          break; */
   /*       } */
   /*       k++; */
   /*    } */
   /*    if ((1u << k) != bufParamsX->dim_x) { */
   /*       status = FFTLIB_ERR_INVALID_DIMENSION; */
   /*    } */
   /* } */
   return (status);
}

#endif
