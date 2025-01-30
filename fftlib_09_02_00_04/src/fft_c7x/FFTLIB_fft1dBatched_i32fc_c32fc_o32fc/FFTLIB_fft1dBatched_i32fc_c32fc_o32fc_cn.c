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

#include "FFTLIB_fft1dBatched_i32fc_c32fc_o32fc_cn.h"

/* CODE_SECTION (FFTLIB_fft1dBatched_i32fc_c32fc_o32fc_cn, ".text:ansi") */
/* CODE_SECTION (FFTLIB_fft1dBatched_i32fc_c32fc_o32fc_checkParams_cn, */
/*               ".text:ansi") */

void butterfly4x4 (FFTLIB_F32  point0Re,
                   FFTLIB_F32  point0Im,
                   FFTLIB_F32  point1Re,
                   FFTLIB_F32  point1Im,
                   FFTLIB_F32  point2Re,
                   FFTLIB_F32  point2Im,
                   FFTLIB_F32  point3Re,
                   FFTLIB_F32  point3Im,
                   FFTLIB_F32  x1TwRe,
                   FFTLIB_F32  x1TwIm,
                   FFTLIB_F32  x2TwRe,
                   FFTLIB_F32  x2TwIm,
                   FFTLIB_F32  x3TwRe,
                   FFTLIB_F32  x3TwIm,
                   FFTLIB_F32 *bflyOut)
{
   FFTLIB_F32 x0Re;
   FFTLIB_F32 x0Im;
   FFTLIB_F32 x1Re;
   FFTLIB_F32 x1Im;
   FFTLIB_F32 x2Re;
   FFTLIB_F32 x2Im;
   FFTLIB_F32 x3Re;
   FFTLIB_F32 x3Im;

   x0Re = point0Re + point1Re + point2Re + point3Re;

   x0Im = point0Im + point1Im + point2Im + point3Im;

   x1Re = (point0Re + point1Im - point2Re - point3Im) * x1TwRe -
          (point0Im - point1Re - point2Im + point3Re) * x1TwIm;

   x1Im = (point0Re + point1Im - point2Re - point3Im) * x1TwIm +
          (point0Im - point1Re - point2Im + point3Re) * x1TwRe;

   x2Re = (point0Re - point1Re + point2Re - point3Re) * x2TwRe -
          (point0Im - point1Im + point2Im - point3Im) * x2TwIm;

   x2Im = (point0Re - point1Re + point2Re - point3Re) * x2TwIm +
          (point0Im - point1Im + point2Im - point3Im) * x2TwRe;

   x3Re = (point0Re - point1Im - point2Re + point3Im) * x3TwRe -
          (point0Im + point1Re - point2Im - point3Re) * x3TwIm;

   x3Im = (point0Re - point1Im - point2Re + point3Im) * x3TwIm +
          (point0Im + point1Re - point2Im - point3Re) * x3TwRe;

   bflyOut[0] = x0Re;
   bflyOut[1] = x0Im;
   bflyOut[2] = x1Re;
   bflyOut[3] = x1Im;
   bflyOut[4] = x2Re;
   bflyOut[5] = x2Im;
   bflyOut[6] = x3Re;
   bflyOut[7] = x3Im;
}

uint32_t bitr (uint32_t in, uint32_t maxValue)
{
   uint32_t i;
   uint32_t out;
   uint32_t numBits;

   for (i = 31; i > 0; i--) {
      if ((maxValue >> i) & 1) {
         break;
      }
   }
   numBits = i + 1;

   out = 0;
   for (i = 0; i < numBits; i++) {
      out |= (((in >> i) & 1) << (numBits - 1 - i));
   }

   return (out);
}

FFTLIB_STATUS
FFTLIB_fft1dBatched_i32fc_c32fc_o32fc_cn (FFTLIB_F32 *          pX,
                                          FFTLIB_bufParams1D_t *bufParamsX,
                                          FFTLIB_F32 *          pW,
                                          FFTLIB_bufParams1D_t *bufParamsW,
                                          FFTLIB_F32 *          pY,
                                          FFTLIB_bufParams1D_t *bufParamsY,
                                          uint32_t              numPoints,
                                          uint32_t              numChannels)
{
   FFTLIB_STATUS status = FFTLIB_SUCCESS;
   FFTLIB_F32    bflyOut[4 * 2];
   uint32_t      numPointsPerDft;
   uint32_t      dftIndex;
   uint32_t      dftStartOffset;
   uint32_t      dftPointIndex;
   FFTLIB_F32 *  pWLocal;
   uint32_t      k, l;

#ifdef FFTLIB_CHECK_PARAMS
   status = FFTLIB_fft1dBatched_i32fc_c32fc_o32fc_checkParams_cn (
       pX, bufParamsX, pW, bufParamsW, pY, bufParamsY, numPoints, numChannels);
   if (status == FFTLIB_SUCCESS)
#endif
   {
      numPointsPerDft = numPoints;
      pWLocal         = pW;
      while (numPointsPerDft > 2) {
         for (k = 0; k < numPoints / 4; k++) {
            dftIndex       = k / (numPointsPerDft / 4);
            dftStartOffset = dftIndex * numPointsPerDft;
            dftPointIndex  = k % (numPointsPerDft / 4);
            for (l = 0; l < numChannels; l++) {
               butterfly4x4 (
                   pX[l * numPoints * 2 + dftStartOffset * 2 +
                      2 * dftPointIndex],
                   pX[l * numPoints * 2 + dftStartOffset * 2 +
                      2 * dftPointIndex + 1],
                   pX[l * numPoints * 2 + dftStartOffset * 2 +
                      (numPointsPerDft / 4) * 2 + 2 * dftPointIndex],
                   pX[l * numPoints * 2 + dftStartOffset * 2 +
                      (numPointsPerDft / 4) * 2 + 2 * dftPointIndex + 1],
                   pX[l * numPoints * 2 + dftStartOffset * 2 +
                      (numPointsPerDft / 2) * 2 + 2 * dftPointIndex],
                   pX[l * numPoints * 2 + dftStartOffset * 2 +
                      (numPointsPerDft / 2) * 2 + 2 * dftPointIndex + 1],
                   pX[l * numPoints * 2 + dftStartOffset * 2 +
                      3 * (numPointsPerDft / 4) * 2 + 2 * dftPointIndex],
                   pX[l * numPoints * 2 + dftStartOffset * 2 +
                      3 * (numPointsPerDft / 4) * 2 + 2 * dftPointIndex + 1],
                   pWLocal[dftPointIndex * 2],     /* re of twiddle for x_1 */
                   pWLocal[dftPointIndex * 2 + 1], /* im of twiddle for x_1 */
                   pWLocal[(numPointsPerDft / 4) * 2 +
                           dftPointIndex * 2], /* re of twiddle for x_2 */
                   pWLocal[(numPointsPerDft / 4) * 2 + dftPointIndex * 2 +
                           1], /* re of twiddle for x_2 */
                   pWLocal[(numPointsPerDft / 2) * 2 +
                           dftPointIndex * 2], /* re of twiddle for x_2 */
                   pWLocal[(numPointsPerDft / 2) * 2 + dftPointIndex * 2 +
                           1], /* re of twiddle for x_2 */
                   bflyOut);

               pX[l * numPoints * 2 + dftStartOffset * 2 + 2 * dftPointIndex] =
                   bflyOut[0];
               pX[l * numPoints * 2 + dftStartOffset * 2 + 2 * dftPointIndex +
                  1]                                             = bflyOut[1];
               pX[l * numPoints * 2 + dftStartOffset * 2 +
                  (numPointsPerDft / 4) * 2 + 2 * dftPointIndex] = bflyOut[4];
               pX[l * numPoints * 2 + dftStartOffset * 2 +
                  (numPointsPerDft / 4) * 2 + 2 * dftPointIndex + 1] =
                   bflyOut[5];
               pX[l * numPoints * 2 + dftStartOffset * 2 +
                  (numPointsPerDft / 2) * 2 + 2 * dftPointIndex] = bflyOut[2];
               pX[l * numPoints * 2 + dftStartOffset * 2 +
                  (numPointsPerDft / 2) * 2 + 2 * dftPointIndex + 1] =
                   bflyOut[3];
               pX[l * numPoints * 2 + dftStartOffset * 2 +
                  3 * (numPointsPerDft / 4) * 2 + 2 * dftPointIndex] =
                   bflyOut[6];
               pX[l * numPoints * 2 + dftStartOffset * 2 +
                  3 * (numPointsPerDft / 4) * 2 + 2 * dftPointIndex + 1] =
                   bflyOut[7];
            }
         }
         numPointsPerDft /= 4;
         pWLocal += 3 * numPointsPerDft * 2;
      }

      if (numPointsPerDft == 2) {
         for (k = 0; k < numPoints / 2; k++) {
            for (l = 0; l < numChannels; l++) {
               bflyOut[0] = pX[l * numPoints * 2 + 2 * k * 2] +
                            pX[l * numPoints * 2 + (2 * k + 1) * 2];
               bflyOut[1] = pX[l * numPoints * 2 + 2 * k * 2 + 1] +
                            pX[l * numPoints * 2 + (2 * k + 1) * 2 + 1];
               bflyOut[2] = pX[l * numPoints * 2 + 2 * k * 2] -
                            pX[l * numPoints * 2 + (2 * k + 1) * 2];
               bflyOut[3] = pX[l * numPoints * 2 + 2 * k * 2 + 1] -
                            pX[l * numPoints * 2 + (2 * k + 1) * 2 + 1];

               pX[l * numPoints * 2 + 2 * k * 2]           = bflyOut[0];
               pX[l * numPoints * 2 + 2 * k * 2 + 1]       = bflyOut[1];
               pX[l * numPoints * 2 + (2 * k + 1) * 2]     = bflyOut[2];
               pX[l * numPoints * 2 + (2 * k + 1) * 2 + 1] = bflyOut[3];
            }
         }
      }

      /* Do swap of points between bit-reversed locations */
      for (k = 0; k < numPoints; k++) {
         uint32_t kBitr;

         kBitr = bitr (k, numPoints - 1);
         for (l = 0; l < numChannels; l++) {
            pY[l * numPoints * 2 + 2 * k] = pX[l * numPoints * 2 + 2 * kBitr];
            pY[l * numPoints * 2 + 2 * k + 1] =
                pX[l * numPoints * 2 + 2 * kBitr + 1];
         }
      }
   }

   return (status);
}

FFTLIB_STATUS FFTLIB_fft1dBatched_i32fc_c32fc_o32fc_checkParams_cn (
    FFTLIB_F32 *          pX,
    FFTLIB_bufParams1D_t *bufParamsX,
    FFTLIB_F32 *          pW,
    FFTLIB_bufParams1D_t *bufParamsW,
    FFTLIB_F32 *          pY,
    FFTLIB_bufParams1D_t *bufParamsY,
    uint32_t              numPoints,
    uint32_t              numChannels)
{
   FFTLIB_STATUS status = FFTLIB_SUCCESS;

   if ((pX == NULL) || (pW == NULL) || (pY == NULL)) {
      status = FFTLIB_ERR_NULL_POINTER;
   }
   else if (bufParamsX->dim_x != bufParamsY->dim_x) {
      status = FFTLIB_ERR_INVALID_DIMENSION;
   }
   else if (bufParamsX->dim_x < numPoints * numChannels * 2) {
      /* In general, dim_x == numPoints*numChannels*2. However, optimized
       * kernel requires dim_x to be atleast 64*2. Hence, for small values
       * of numPoints*numChannels, dim_x could be greater than
       * numPoints*numChannels*2                                           */
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
   }
   return (status);
}
