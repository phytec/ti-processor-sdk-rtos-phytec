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

#include "FFTLIB_fft1dBatched_i16sc_c16sc_o16sc_cn.h"

/* CODE_SECTION (FFTLIB_fft1dBatched_i16sc_c16sc_o16sc_cn, ".text:ansi") */
/* CODE_SECTION (FFTLIB_fft1dBatched_i16sc_c16sc_o16sc_checkParams_cn, */
/*               ".text:ansi") */

/* Logic of scaling here is similar to that in implementation of
 * VCMPYR1HH instruction                                         */
static int16_t satAndScale (int64_t x)
{
   int64_t xTemp;

   xTemp = (x + 0x4000L) << 1;
   if (xTemp > 0x7FFF0000L) {
      xTemp = 0x7FFF0000L;
   }
   if (xTemp < -0x80000000L) {
      xTemp = -0x80000000L;
   }
   return ((int16_t) (xTemp >> 16));
}

int16_t crot90HRe (int16_t re, int16_t im)
{
   if (im == ((int16_t) 0x8000)) {
      /* Saturate like VCROT90H instruction does */
      return 0x7FFF;
   }
   else {
      return -im;
   }
}

int16_t crot90HIm (int16_t re, int16_t im) { return re; }

void shiftRightAndRound (int16_t *pBuff, uint32_t numElements, uint32_t shift)
{
   uint32_t k;
   int16_t  shiftOutBitMask;

   shiftOutBitMask = 1u << (shift - 1);
   for (k = 0; k < numElements; k++) {
      pBuff[k] = (pBuff[k] & shiftOutBitMask) != 0 ? (pBuff[k] >> shift) + 1
                                                   : (pBuff[k] >> shift);
   }
}

void butterflyNoTw (int16_t  point0Re,
                    int16_t  point0Im,
                    int16_t  point1Re,
                    int16_t  point1Im,
                    int16_t  point2Re,
                    int16_t  point2Im,
                    int16_t  point3Re,
                    int16_t  point3Im,
                    int16_t *bflyOut)
{
   int16_t sum1Re, sum2Re, diff1Re, diff2Re;
   int16_t sum1Im, sum2Im, diff1Im, diff2Im;
   int16_t x0Re, x1Re, x2Re, x3Re;
   int16_t x0Im, x1Im, x2Im, x3Im;

   /* Kernel is implemented with intermediate at 16-bit.
    * So, we always do one operation at a time and store
    * result in a 16-bit variable                        */
   sum1Re  = point0Re + point2Re;
   sum1Im  = point0Im + point2Im;
   sum2Re  = point1Re + point3Re;
   sum2Im  = point1Im + point3Im;
   diff1Re = point0Re - point2Re;
   diff1Im = point0Im - point2Im;
   diff2Re = point1Re - point3Re;
   diff2Im = point1Im - point3Im;

   x0Re = sum1Re + sum2Re;
   x0Im = sum1Im + sum2Im;
   x1Re = diff1Re - crot90HRe (diff2Re, diff2Im);
   x1Im = diff1Im - crot90HIm (diff2Re, diff2Im);
   x2Re = sum1Re - sum2Re;
   x2Im = sum1Im - sum2Im;
   x3Re = diff1Re + crot90HRe (diff2Re, diff2Im);
   x3Im = diff1Im + crot90HIm (diff2Re, diff2Im);

   bflyOut[0] = x0Re;
   bflyOut[1] = x0Im;
   bflyOut[2] = x1Re;
   bflyOut[3] = x1Im;
   bflyOut[4] = x2Re;
   bflyOut[5] = x2Im;
   bflyOut[6] = x3Re;
   bflyOut[7] = x3Im;
}

void butterfly (int16_t  point0Re,
                int16_t  point0Im,
                int16_t  point1Re,
                int16_t  point1Im,
                int16_t  point2Re,
                int16_t  point2Im,
                int16_t  point3Re,
                int16_t  point3Im,
                int16_t  x1TwRe,
                int16_t  x1TwIm,
                int16_t  x2TwRe,
                int16_t  x2TwIm,
                int16_t  x3TwRe,
                int16_t  x3TwIm,
                int16_t *bflyOut)
{
   int16_t sum1Re, sum2Re, diff1Re, diff2Re;
   int16_t sum1Im, sum2Im, diff1Im, diff2Im;
   int16_t x0Re, x0Im, x1Re, x1Im;
   int16_t x2Re, x2Im, x3Re, x3Im;
   int16_t x0TempRe, x0TempIm;
   int16_t x1TempRe, x1TempIm;
   int16_t x2TempRe, x2TempIm;
   int16_t x3TempRe, x3TempIm;
   int64_t x0LongRe, x0LongIm;
   int64_t x1LongRe, x1LongIm;
   int64_t x2LongRe, x2LongIm;
   int64_t x3LongRe, x3LongIm;

   sum1Re  = point0Re + point2Re;
   sum1Im  = point0Im + point2Im;
   sum2Re  = point1Re + point3Re;
   sum2Im  = point1Im + point3Im;
   diff1Re = point0Re - point2Re;
   diff1Im = point0Im - point2Im;
   diff2Re = point1Re - point3Re;
   diff2Im = point1Im - point3Im;

   x0TempRe = sum1Re + sum2Re;
   x0TempIm = sum1Im + sum2Im;
   x1TempRe = diff1Re - crot90HRe (diff2Re, diff2Im);
   x1TempIm = diff1Im - crot90HIm (diff2Re, diff2Im);
   x2TempRe = sum1Re - sum2Re;
   x2TempIm = sum1Im - sum2Im;
   x3TempRe = diff1Re + crot90HRe (diff2Re, diff2Im);
   x3TempIm = diff1Im + crot90HIm (diff2Re, diff2Im);

   /* Multiplication and saturation logic uses int64_t types
    * to keep logic similar to VCMPYR1HH instruction         */
   x0LongRe = ((int64_t) x0TempRe) << 15;
   x0LongIm = ((int64_t) x0TempIm) << 15;
   x1LongRe = (int64_t) (x1TempRe * x1TwRe) - (int64_t) (x1TempIm * x1TwIm);
   x1LongIm = (int64_t) (x1TempRe * x1TwIm) + (int64_t) (x1TempIm * x1TwRe);
   x2LongRe = (int64_t) (x2TempRe * x2TwRe) - (int64_t) (x2TempIm * x2TwIm);
   x2LongIm = (int64_t) (x2TempRe * x2TwIm) + (int64_t) (x2TempIm * x2TwRe);
   x3LongRe = (int64_t) (x3TempRe * x3TwRe) - (int64_t) (x3TempIm * x3TwIm);
   x3LongIm = (int64_t) (x3TempRe * x3TwIm) + (int64_t) (x3TempIm * x3TwRe);

   x0Re = satAndScale (x0LongRe);
   x0Im = satAndScale (x0LongIm);
   x1Re = satAndScale (x1LongRe);
   x1Im = satAndScale (x1LongIm);
   x2Re = satAndScale (x2LongRe);
   x2Im = satAndScale (x2LongIm);
   x3Re = satAndScale (x3LongRe);
   x3Im = satAndScale (x3LongIm);

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
FFTLIB_fft1dBatched_i16sc_c16sc_o16sc_cn (int16_t *             pX,
                                          FFTLIB_bufParams1D_t *bufParamsX,
                                          int16_t *             pW,
                                          FFTLIB_bufParams1D_t *bufParamsW,
                                          int16_t *             pY,
                                          FFTLIB_bufParams1D_t *bufParamsY,
                                          uint32_t *            pShift,
                                          FFTLIB_bufParams1D_t *bufParamsShift,
                                          uint32_t              numPoints,
                                          uint32_t              numChannels)
{
   FFTLIB_STATUS status = FFTLIB_SUCCESS;
   int16_t       bflyOut[4 * 2];
   uint32_t      numPointsPerDft;
   uint32_t      dftIndex;
   uint32_t      dftStartOffset;
   uint32_t      dftPointIndex;
   int16_t *     pWLocal;
   uint32_t      k, l;
   uint32_t      shift;
   uint32_t      stageIndex;

#ifdef FFTLIB_CHECK_PARAMS
   status =
       FFTLIB_fft1dBatched_i16sc_c16sc_o16sc_checkParams_cn (pX,
                                                             bufParamsX,
                                                             pW,
                                                             bufParamsW,
                                                             pY,
                                                             bufParamsY,
                                                             pShift,
                                                             bufParamsShift,
                                                             numPoints,
                                                             numChannels);
   if (status == FFTLIB_SUCCESS)
#endif
   {
      numPointsPerDft = numPoints;
      pWLocal         = pW;
      stageIndex      = 0;
      while (numPointsPerDft > 2) {
         shift = pShift[stageIndex];
         for (k = 0; k < numPoints / 4; k++) {
            dftIndex       = k / (numPointsPerDft / 4);
            dftStartOffset = dftIndex * numPointsPerDft;
            dftPointIndex  = k % (numPointsPerDft / 4);
            for (l = 0; l < numChannels; l++) {
               if ((numPointsPerDft == 4) ||
                   ((numPointsPerDft == 8) && (dftPointIndex == 0))) {
                  /* For this case, the twiddle factors are all 1's. In this
                   * (fixed point) variant, multiplication by 1 and scaling down
                   * can introduce errors. So, like in the kernel
                   * implementation, we skip the multiplication by 1's */
                  butterflyNoTw (
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
                      bflyOut);
               }
               else {
                  butterfly (
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
                      pWLocal[dftPointIndex * 2], /* re of twiddle for x_1 */
                      pWLocal[dftPointIndex * 2 +
                              1], /* im of twiddle for x_1 */
                      pWLocal[(numPointsPerDft / 4) * 2 +
                              dftPointIndex * 2], /* re of twiddle for x_2 */
                      pWLocal[(numPointsPerDft / 4) * 2 + dftPointIndex * 2 +
                              1], /* re of twiddle for x_2 */
                      pWLocal[(numPointsPerDft / 2) * 2 +
                              dftPointIndex * 2], /* re of twiddle for x_2 */
                      pWLocal[(numPointsPerDft / 2) * 2 + dftPointIndex * 2 +
                              1], /* re of twiddle for x_2 */
                      bflyOut);
               }

               /* Only one shift that is applied for 8-point and the final
                * 2-point stage combined. */
               if (numPointsPerDft != 8)
                  shiftRightAndRound (bflyOut, 8, shift);

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
         stageIndex++;
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

               /* One shift amount for 8-point and 2-point stage combined */
               shiftRightAndRound (bflyOut, 4, shift);

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

FFTLIB_STATUS FFTLIB_fft1dBatched_i16sc_c16sc_o16sc_checkParams_cn (
    int16_t *             pX,
    FFTLIB_bufParams1D_t *bufParamsX,
    int16_t *             pW,
    FFTLIB_bufParams1D_t *bufParamsW,
    int16_t *             pY,
    FFTLIB_bufParams1D_t *bufParamsY,
    uint32_t *            pShift,
    FFTLIB_bufParams1D_t *bufParamsShift,
    uint32_t              numPoints,
    uint32_t              numChannels)
{

   FFTLIB_STATUS status = FFTLIB_SUCCESS;

   if ((pX == NULL) || (pW == NULL) || (pY == NULL) || (pShift == NULL)) {
      status = FFTLIB_ERR_NULL_POINTER;
   }
   else if (bufParamsX->dim_x != bufParamsY->dim_x) {
      status = FFTLIB_ERR_INVALID_DIMENSION;
   }
   else if (bufParamsX->dim_x < numPoints * numChannels * 2) {
      /* In general, dim_x == numPoints*numChannels*2. However, optimized
       * kernel requires dim_x to be atleast 128*2. Hence, for small values
       * of numPoints*numChannels, dim_x could be greater than
       * numPoints*numChannels*2                                           */
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
   else if (((1u << (bufParamsShift->dim_x * 2)) * 2 !=
             bufParamsW->dim_x) && /* check for powers of 4 */
            ((1u << (bufParamsShift->dim_x * 2)) * 2 * 2 !=
             bufParamsW->dim_x)) { /* check for powers of 2 */
      status = FFTLIB_ERR_INVALID_DIMENSION;
   }
   return (status);
}
