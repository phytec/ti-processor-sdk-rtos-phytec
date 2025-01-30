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

#include "FFTLIB_fft1d_i32f_c32fc_o32fc_cn.h"

FFTLIB_STATUS
FFTLIB_fft1d_i32f_c32fc_o32fc_cn(FFTLIB_F32           *pX,
                                 FFTLIB_bufParams1D_t *bufParamsX,
                                 FFTLIB_F32           *pW,
                                 FFTLIB_bufParams1D_t *bufParamsW,
                                 FFTLIB_F32           *pXFFT,
                                 FFTLIB_bufParams1D_t *bufParamsXFFT,
                                 FFTLIB_F32           *pSf,
                                 FFTLIB_bufParams1D_t *bufParamsSf,
                                 FFTLIB_F32           *pY,
                                 FFTLIB_bufParams1D_t *bufParamsY)
{
   FFTLIB_STATUS        status = FFTLIB_SUCCESS;
   uint32_t             numPoints;
   FFTLIB_bufParams1D_t bufParamsX_FFT;

   /* #ifdef FFTLIB_CHECK_PARAMS */
   /*    status = FFTLIB_fft1d_i32f_c32fc_o32fc_checkParams_cn ( */
   /*        pX, bufParamsX, pW, bufParamsW, pY, bufParamsY); */
   /*    if (status == FFTLIB_SUCCESS) */
   /* #endif */
   {

      numPoints            = bufParamsX->dim_x;
      bufParamsX_FFT.dim_x = numPoints;

      FFTLIB_fft1dBatched_i32fc_c32fc_o32fc_cn(pX, &bufParamsX_FFT, pW, &bufParamsX_FFT, pXFFT, &bufParamsX_FFT,
                                               numPoints >> 1, 1);

      uint32_t i = 0;

      FFTLIB_F32 temp0Real;
      FFTLIB_F32 temp0Imag;
      FFTLIB_F32 temp1Real;
      FFTLIB_F32 temp1Imag;

      FFTLIB_F32 temp2Real;
      FFTLIB_F32 temp2Imag;
      FFTLIB_F32 temp3Real;
      FFTLIB_F32 temp3Imag;

      FFTLIB_F32 var0Real;
      FFTLIB_F32 var0Imag;
      FFTLIB_F32 var1Real;
      FFTLIB_F32 var1Imag;

      /* printf ("\npY natC FFT below:\n\n"); */
      /* for (i = 0; i < numPoints; i++) { */
      /*    printf ("%.2f, ", pY[i]); */
      /* } */
      /* printf ("\n\n"); */

      // we dont reach numPoints + 2 * (numPoints >> 2) in loop
      i        = (numPoints >> 2);
      var0Real = pXFFT[2 * i];
      var0Imag = pXFFT[2 * i + 1];
      var1Real = pXFFT[(numPoints) -2 * (i)];
      var1Imag = pXFFT[(numPoints) - (2 * i) + 1];

      temp0Real = var0Real * pSf[2 * i] - (var0Imag * pSf[2 * i + 1]);
      temp0Imag = var0Real * pSf[2 * i + 1] + (var0Imag * pSf[2 * i]);

      temp1Real = var1Real * pSf[2 * i + numPoints] + var1Imag * pSf[2 * i + numPoints + 1];
      temp1Imag = var1Real * pSf[2 * i + numPoints + 1] - var1Imag * pSf[2 * i + numPoints];

      pY[2 * i]                           = temp0Real + temp1Real;
      pY[2 * i + 1]                       = temp0Imag + temp1Imag;
      pY[(numPoints * 2) - (2 * (i))]     = pY[2 * i];
      pY[(numPoints * 2) - (2 * (i)) + 1] = pY[2 * i + 1] * -1;

      // similarly for i = numPoints >> 1
      i = (numPoints >> 1);

      pY[2 * i]     = pXFFT[0] - pXFFT[1];
      pY[2 * i + 1] = 0;

      for (i = 0; i < numPoints >> 1; i += 1) {
         /* for (i = 0; i < 16; i += 1) { */
         /* printf ("\nIteration %d\n", i); */
         var0Real = pXFFT[2 * i]; // [1]
         var0Imag = pXFFT[2 * i + 1];
         var1Real = pXFFT[(numPoints) -2 * (i)];
         var1Imag = pXFFT[(numPoints) - (2 * i) + 1];

         /* printf ("var0Real = %0.2f, var0Imag = %0.2f\n", var0Real, var0Imag);
          */
         /* printf ("var1Real = %0.2f, var1Imag = %0.2f\n", var1Real, var1Imag);
          */
         // A*X
         /* temp0Real = pY[2 * i] * pSf[2 * i] - (pY[2 * i + 1] * pSf[2 * i +
          * 1]); */
         /* temp0Imag = pY[2 * i] * pSf[2 * i + 1] + (pY[2 * i + 1] * pSf[2 *
          * i]); */

         temp0Real = var0Real * pSf[2 * i] - (var0Imag * pSf[2 * i + 1]);
         temp0Imag = var0Real * pSf[2 * i + 1] + (var0Imag * pSf[2 * i]);

         /* printf ("temp0Real = %0.2f, temp0Imag = %0.2f\n", temp0Real, */
         /*         temp0Imag); */
         /* printf ("temp2Real = %0.2f, temp2Imag = %0.2f\n", temp2Real, */
         /*         temp2Imag); */
         // B*X'[N-k]
         if (!i) {
            temp1Real = pXFFT[0] * pSf[2 * numPoints - 2 * i - 2] + pXFFT[1] * pSf[2 * numPoints - 2 * i - 2 + 1];
            temp1Imag = pXFFT[0] * pSf[2 * numPoints - 2 * i - 2] - pXFFT[1] * pSf[2 * numPoints - 2 * i - 2 + 1];
         }
         else {
            /* temp1Real = */
            /*     pY[numPoints - (2 * (i))] * pSf[2 * i + numPoints] + */
            /*     pY[numPoints - (2 * (i)) + 1] * pSf[2 * i + numPoints + 1];
             */
            /* temp1Imag = pY[numPoints - (2 * (i))] * pSf[2 * i + numPoints +
             * 1] - */
            /*             pY[numPoints - (2 * (i)) + 1] * pSf[2 * i +
             * numPoints]; */
            temp1Real = var1Real * pSf[2 * numPoints - 2 * i - 2] + var1Imag * pSf[2 * numPoints - 2 * i - 2 + 1];
            temp1Imag = var1Real * pSf[2 * numPoints - 2 * i - 2 + 1] - var1Imag * pSf[2 * numPoints - 2 * i - 2];
         }

         /* printf ("psFReal = %0.2f, psFImag = %0.2f\n", */
         /*         pSf[2 * numPoints - 2 * i - 2], */
         /*         pSf[2 * numPoints - 2 * i - 2 + 1]); */

         /* printf ("temp1Real = %0.2f, temp1Imag = %0.2f\n", temp1Real, */
         /*         temp1Imag); */

         /* printf ("temp3Real = %0.2f, temp3Imag = %0.2f\n", temp3Real, */
         /*         temp3Imag); */

         pY[2 * i]                           = temp0Real + temp1Real;
         pY[2 * i + 1]                       = temp0Imag + temp1Imag;
         pY[(numPoints * 2) - (2 * (i))]     = pY[2 * i];
         pY[(numPoints * 2) - (2 * (i)) + 1] = pY[2 * i + 1] * -1;
      }

      /* printf ("pY natC below:\n"); */
      /* for (i = 0; i < 2 * numPoints; i += 2) { */
      /*    printf ("%d: %.2f, %.2f\n", i >> 1, pY[i], pY[i + 1]); */
      /* } */

      return (status);
   }
}

FFTLIB_STATUS
FFTLIB_fft1d_i32f_c32fc_o32fc_checkParams_cn(FFTLIB_F32           *pX,
                                             FFTLIB_bufParams1D_t *bufParamsX,
                                             FFTLIB_F32           *pW,
                                             FFTLIB_bufParams1D_t *bufParamsW,
                                             FFTLIB_F32           *pY,
                                             FFTLIB_bufParams1D_t *bufParamsY)
{
   FFTLIB_STATUS status = FFTLIB_SUCCESS;

   if ((pX == NULL) || (pW == NULL) || (pY == NULL)) {
      status = FFTLIB_ERR_NULL_POINTER;
   }
   else if (bufParamsX->dim_x != bufParamsW->dim_x || bufParamsX->dim_x != bufParamsY->dim_x) {
      status = FFTLIB_ERR_INVALID_DIMENSION;
   }
   else if ((bufParamsX->data_type != FFTLIB_FLOAT32) || (bufParamsW->data_type != FFTLIB_FLOAT32) ||
            (bufParamsY->data_type != FFTLIB_FLOAT32)) {
      status = FFTLIB_ERR_INVALID_TYPE;
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
