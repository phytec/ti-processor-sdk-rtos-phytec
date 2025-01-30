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

#include "FFTLIB_ifft1d_i32fc_c32fc_o32f_cn.h"

FFTLIB_STATUS
FFTLIB_ifft1d_i32fc_c32fc_o32f_cn (FFTLIB_F32 *          pX,
                                   FFTLIB_bufParams1D_t *bufParamsX,
                                   FFTLIB_F32 *          pW,
                                   FFTLIB_bufParams1D_t *bufParamsW,
                                   FFTLIB_F32 *          pXFFT,
                                   FFTLIB_bufParams1D_t *bufParamsXFFT,
                                   FFTLIB_F32 *          pSf,
                                   FFTLIB_bufParams1D_t *bufParamsSf,
                                   FFTLIB_F32 *          pY,
                                   FFTLIB_bufParams1D_t *bufParamsY)
{
   FFTLIB_STATUS        status = FFTLIB_SUCCESS;
   uint32_t             numPoints;
   FFTLIB_bufParams1D_t bufParamsX_FFT;

   /* #ifdef FFTLIB_CHECK_PARAMS */
   /*    status = FFTLIB_ifft1d_i32fc_c32fc_o32f_checkParams_cn ( */
   /*        pX, bufParamsX, pW, bufParamsW, pY, bufParamsY); */
   /*    if (status == FFTLIB_SUCCESS) */
   /* #endif */

   numPoints            = bufParamsX->dim_x;
   bufParamsX_FFT.dim_x = numPoints;
   uint32_t i           = 0;
   float    var0Real    = 0.0f;
   float    var0Imag    = 0.0f;
   float    temp0Real   = 0.0f;
   float    temp0Imag   = 0.0f;

   float var1Real  = 0.0f;
   float var1Imag  = 0.0f;
   float temp1Real = 0.0f;
   float temp1Imag = 0.0f;

   uint32_t N_2 = numPoints >> 1;

   /* printf ("\nDebug!!\n"); */
   for (i = 0; i < N_2; i++) {
      var0Real  = pX[2 * i];
      var0Imag  = pX[2 * i + 1];
      temp0Real = var0Real * pSf[2 * i] - (var0Imag * pSf[2 * i + 1]);
      temp0Imag = var0Real * pSf[2 * i + 1] + (var0Imag * pSf[2 * i]);

      /* printf ("var0Real = %0.2f, var0Imag = %0.2f\n", var0Real, var0Imag);
       */
      /* printf ("temp0Real = %0.2f, temp0Imag = %0.2f\n", temp0Real,
       * temp0Imag); */

      var1Real  = pX[2 * i + numPoints];
      var1Imag  = pX[2 * i + 1 + numPoints];
      temp1Real = var1Real * pSf[2 * i + numPoints] -
                  (var1Imag * pSf[2 * i + 1 + (numPoints)]);
      temp1Imag = var1Real * pSf[2 * i + 1 + numPoints] +
                  (var1Imag * pSf[2 * i + numPoints]);

      /* printf ("var1Real = %0.2f, var1Imag = %0.2f\n", var1Real, var1Imag);
       */
      /* printf ("temp1Real = %0.2f, temp1Imag = %0.2f\n", temp1Real,
       * temp1Imag); */

      pXFFT[2 * i]     = temp0Real + temp1Real;
      pXFFT[2 * i + 1] = temp0Imag + temp1Imag;
   }

   /* printf ("\nXFFT_cn:\n"); */
   /* for (i = 0; i < numPoints; i++) { */
   /*    printf ("%.2f ", pXFFT[i]); */
   /* } */

   FFTLIB_ifft1d_i32fc_c32fc_o32fc_cn (pXFFT, &bufParamsX_FFT, pW,
                                       &bufParamsX_FFT, pY, &bufParamsX_FFT);

   return (status);
}

FFTLIB_STATUS
FFTLIB_ifft1d_i32fc_c32fc_o32f_checkParams_cn (FFTLIB_F32 *          pX,
                                               FFTLIB_bufParams1D_t *bufParamsX,
                                               FFTLIB_F32 *          pW,
                                               FFTLIB_bufParams1D_t *bufParamsW,
                                               FFTLIB_F32 *          pY,
                                               FFTLIB_bufParams1D_t *bufParamsY)
{
   FFTLIB_STATUS status = FFTLIB_SUCCESS;

   /* if ((pX == NULL) || (pW == NULL) || (pY == NULL)) { */
   /*    status = FFTLIB_ERR_NULL_POINTER; */
   /* } */
   /* else if (bufParamsX->dim_x != bufParamsW->dim_x || */
   /*          bufParamsX->dim_x != bufParamsY->dim_x) { */
   /*    status = FFTLIB_ERR_INVALID_DIMENSION; */
   /* } */
   /* else if ((bufParamsX->data_type != FFTLIB_FLOAT32) || */
   /*          (bufParamsW->data_type != FFTLIB_FLOAT32) || */
   /*          (bufParamsY->data_type != FFTLIB_FLOAT32)) { */
   /*    status = FFTLIB_ERR_INVALID_TYPE; */
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
