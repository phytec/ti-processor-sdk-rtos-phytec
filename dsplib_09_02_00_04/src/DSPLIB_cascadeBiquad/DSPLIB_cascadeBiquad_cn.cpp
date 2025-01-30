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
**|         Copyright (c) 2016 Texas Instruments Incorporated                |**
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

#include "DSPLIB_cascadeBiquad_priv.h"

DSPLIB_STATUS DSPLIB_cascadeBiquad_init_cn(DSPLIB_kernelHandle            handle,
                                           DSPLIB_bufParams2D_t          *bufParamsIn,
                                           DSPLIB_bufParams1D_t          *bufParamsFilterCoeff,
                                           DSPLIB_bufParams2D_t          *bufParamsFilterVar,
                                           DSPLIB_bufParams2D_t          *bufParamsOut,
                                           DSPLIB_cascadeBiquad_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;
   return status;
}

template <typename dataType>
DSPLIB_STATUS DSPLIB_cascadeBiquad_exec_cn(DSPLIB_kernelHandle handle,
                                           void *restrict pIn,
                                           void *restrict pFilterCoeff,
                                           void *restrict pFilterVar,
                                           void *restrict pOut)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_cascadeBiquad_exec_cn\n");
#endif

   dataType                      *pInLocal;
   dataType                      *pFilterCoeffLocal = (dataType *) pFilterCoeff;
   dataType                      *pFilterVarLocal   = (dataType *) pFilterVar;
   dataType                      *pOutLocal;
   DSPLIB_cascadeBiquad_PrivArgs *pKerPrivArgs      = (DSPLIB_cascadeBiquad_PrivArgs *) handle;

   uint32_t dataBufferInPitch  = pKerPrivArgs->dataBufferInPitch;
   uint32_t dataBufferOutPitch = pKerPrivArgs->dataBufferOutPitch;
   uint32_t filterVarPitch     = pKerPrivArgs->filterVarPitch;

   uint32_t dataSize    = pKerPrivArgs->initArgs.dataSize;
   uint32_t numStages   = pKerPrivArgs->initArgs.numStages;
   uint32_t numChannels = pKerPrivArgs->initArgs.numChannels;

#if DSPLIB_DEBUGPRINT
   printf("Enter pInLocal %p pFilterCoeffLocal %p pFilterVarLocal %p pOut %p filterVarPitch %d\n", pIn,
          pFilterCoeffLocal, pFilterVarLocal, pOut, filterVarPitch);
#endif

   dataType b11, b12, a11, a12; /* Cascade-1 coeffs   */
   dataType b21, b22, a21, a22; /* Cascade-2 coeffs   */
   dataType b31, b32, a31, a32; /* Cascade-3 coeffs   */
   dataType b41, b42, a41, a42; /* Cascade-4 coeffs   */
   dataType b51, b52, a51, a52; /* Cascade-5 coeffs   */
   dataType b61, b62, a61, a62; /* Cascade-6 coeffs   */
   dataType b71, b72, a71, a72; /* Cascade-7 coeffs   */
   dataType b10, b20, b30, b40, b50, b60, b70;
   dataType vd0, vd1, vd2, vd3, vd4, vd5, vd6, vd7, vd8, vd9, vd10, vd11, vd12, vd13;

   float coeff0 = pFilterCoeffLocal[0];
   b10          = coeff0; // pFilterCoeffLocal[0];
   dataType r   = (dataType) (1 / coeff0);
   b11          = pFilterCoeffLocal[1] * r;
   b12          = pFilterCoeffLocal[2] * r;
   a11          = -pFilterCoeffLocal[3];
   a12          = -pFilterCoeffLocal[4];

   if (numStages >= 2) {
      b20 = pFilterCoeffLocal[5];
      r   = (dataType) (1 / b20);
      b21 = pFilterCoeffLocal[6] * r;
      b22 = pFilterCoeffLocal[7] * r;
      a21 = -pFilterCoeffLocal[8];
      a22 = -pFilterCoeffLocal[9];
   }
   else {
      // if second stage is unused, create coefficients of unity transfer function
      b20 = 1.0;
      b21 = 0;
      b22 = 0;
      a21 = 0;
      a22 = 0;
   }

   if (numStages >= 3) {
      b30 = pFilterCoeffLocal[10];
      r   = (dataType) (1 / b30);
      b31 = pFilterCoeffLocal[11] * r;
      b32 = pFilterCoeffLocal[12] * r;
      a31 = -pFilterCoeffLocal[13];
      a32 = -pFilterCoeffLocal[14];
   }
   else {
      // if third stage is unused, create coefficients of unity transfer function
      b30 = 1.0;
      b31 = 0;
      b32 = 0;
      a31 = 0;
      a32 = 0;
   }

   if (numStages >= 4) {
      b40 = pFilterCoeffLocal[15];
      r   = (dataType) (1 / b40);
      b41 = pFilterCoeffLocal[16] * r;
      b42 = pFilterCoeffLocal[17] * r;
      a41 = -pFilterCoeffLocal[18];
      a42 = -pFilterCoeffLocal[19];
   }
   else {
      // if third stage is unused, create coefficients of unity transfer function
      b40 = 1.0;
      b41 = 0;
      b42 = 0;
      a41 = 0;
      a42 = 0;
   }

   if (numStages >= 5) {
      b50 = pFilterCoeffLocal[20];
      r   = (dataType) (1 / b50);
      b51 = pFilterCoeffLocal[21] * r;
      b52 = pFilterCoeffLocal[22] * r;
      a51 = -pFilterCoeffLocal[23];
      a52 = -pFilterCoeffLocal[24];
   }
   else {
      // if third stage is unused, create coefficients of unity transfer function
      b50 = 1.0;
      b51 = 0;
      b52 = 0;
      a51 = 0;
      a52 = 0;
   }

   if (numStages >= 6) {
      b60 = pFilterCoeffLocal[25];
      r   = (dataType) (1 / b60);
      b61 = pFilterCoeffLocal[26] * r;
      b62 = pFilterCoeffLocal[27] * r;
      a61 = -pFilterCoeffLocal[28];
      a62 = -pFilterCoeffLocal[29];
   }
   else {
      // if third stage is unused, create coefficients of unity transfer function
      b60 = 1.0;
      b61 = 0;
      b62 = 0;
      a61 = 0;
      a62 = 0;
   }

   if (numStages >= 7) {
      b70 = pFilterCoeffLocal[30];
      r   = (dataType) (1 / b70);
      b71 = pFilterCoeffLocal[31] * r;
      b72 = pFilterCoeffLocal[32] * r;
      a71 = -pFilterCoeffLocal[33];
      a72 = -pFilterCoeffLocal[34];
   }
   else {
      // if third stage is unused, create coefficients of unity transfer function
      b70 = 1.0;
      b71 = 0;
      b72 = 0;
      a71 = 0;
      a72 = 0;
   }
   /* Get the filter states into corresponding regs */

   dataType outGain = b10 * b20 * b30 * b40 * b50 * b60 * b70; // Final output gain rescales based on b0 term
   dataType input1, output01, output02, output03, output04, output05, output06, output07;
   dataType t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13; /* Filter temp regs  */
   dataType output;

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT a11 %f a12 %f b11 %f b12 %f b21 %f b22 %f b31 %f b32 %f\n", a11, a12, b11, b12, b21, b22,
          b31, b32);
   printf("DSPLIB_DEBUGPRINT a41 %f a42 %f b41 %f b42 %f b51 %f b52 %f b61 %f b62 %f\n", a41, a42, b41, b42, b51, b52,
          b61, b62);
   printf("DSPLIB_DEBUGPRINT b10 %f b20 %f b30 %f b40 %f b50 %f b60 %f b70 %f\n", b10, b20, b30, b40, b50, b60, b70);
   printf("DSPLIB_DEBUGPRINT outGain %f\n", outGain);
#endif

   for (uint32_t numCh = 0; numCh < numChannels; numCh++) {
      pInLocal  = (dataType *) pIn + numCh;
      pOutLocal = (dataType *) pOut + numCh;
      vd0       = pFilterVarLocal[0];
      vd1       = pFilterVarLocal[filterVarPitch];
      if (numStages >= 2) {
         vd2 = pFilterVarLocal[2 * filterVarPitch];
         vd3 = pFilterVarLocal[3 * filterVarPitch];
      }
      else {
         vd2 = 0;
         vd3 = 0;
      }
      if (numStages >= 3) {
         vd4 = pFilterVarLocal[4 * filterVarPitch];
         vd5 = pFilterVarLocal[5 * filterVarPitch];
      }
      else {
         vd4 = 0;
         vd5 = 0;
      }
      if (numStages >= 4) {
         vd6 = pFilterVarLocal[6 * filterVarPitch];
         vd7 = pFilterVarLocal[7 * filterVarPitch];
      }
      else {
         vd6 = 0;
         vd7 = 0;
      }
      if (numStages >= 5) {
         vd8 = pFilterVarLocal[8 * filterVarPitch];
         vd9 = pFilterVarLocal[9 * filterVarPitch];
      }
      else {
         vd8 = 0;
         vd9 = 0;
      }
      if (numStages >= 6) {
         vd10 = pFilterVarLocal[10 * filterVarPitch];
         vd11 = pFilterVarLocal[11 * filterVarPitch];
      }
      else {
         vd10 = 0;
         vd11 = 0;
      }
      if (numStages >= 7) {
         vd12 = pFilterVarLocal[12 * filterVarPitch];
         vd13 = pFilterVarLocal[13 * filterVarPitch];
      }
      else {
         vd12 = 0;
         vd13 = 0;
      }

#if DSPLIB_DEBUGPRINT
      printf("DSPLIB_DEBUGPRINT numCh %d: pFilterVarLocal %p vd0 %f vd1 %f vd2 %f vd3 %f vd4 %f vd5 %f\n", numCh,
             pFilterVarLocal, vd0, vd1, vd2, vd3, vd4, vd5);
      printf("DSPLIB_DEBUGPRINT vd6 %f vd7 %f vd8 %f vd9 %f vd10 %f vd11 %f vd12 %f vd13 %f\n", vd6, vd7, vd8, vd9,
             vd10, vd11, vd12, vd13);
#endif

      for (uint32_t samp = 0; samp < dataSize; samp++) {
         input1   = *(pInLocal); //, *in2++);
         pInLocal = (dataType *) pInLocal + dataBufferInPitch;
         // Channel 1 stage 1

         output01 = input1 + vd0;
         t1       = (((b11 + a11) * input1) + vd1);
         t0       = (a12 * vd0);
         vd0      = ((a11 * vd0) + t1);
         vd1      = (((b12 + a12) * input1) + t0);

         // Channel 1 stage 2
         output02 = (output01 + vd2);
         t3       = (((b21 + a21) * output01) + vd3); //(b21 + a21) * output1 + d3 ;
         t2       = (a22 * vd2);
         vd2      = ((a21 * vd2) + t3);
         vd3      = (((b22 + a22) * output01) + t2);

         // Channel 1 stage 3
         output03 = (output02 + vd4);
         t5       = (((b31 + a31) * output02) + vd5);
         t4       = (a32 * vd4);
         vd4      = ((a31 * vd4) + t5);
         vd5      = (((b32 + a32) * output02) + t4);

         // Channel 1 stage 4
         output04 = output03 + vd6;
         t7       = (((b41 + a41) * output03) + vd7);
         t6       = (a42 * vd6);
         vd6      = ((a41 * vd6) + t7);
         vd7      = (((b42 + a42) * output03) + t6);

         // Channel 1 stage 5
         output05 = (output04 + vd8);
         t9       = (((b51 + a51) * output04) + vd9); //(b21 + a21) * output1 + d3 ;
         t8       = (a52 * vd8);
         vd8      = ((a51 * vd8) + t9);
         vd9      = (((b52 + a52) * output04) + t8);

         // Channel 1 stage 6
         output06 = (output05 + vd10);
         t11      = (((b61 + a61) * output05) + vd11);
         t10      = (a62 * vd10);
         vd10     = ((a61 * vd10) + t11);
         vd11     = (((b62 + a62) * output05) + t10);

         // Channel 1 stage 7
         output07 = (output06 + vd12);
         t13      = (((b71 + a71) * output06) + vd13);
         t12      = (a72 * vd12);
         vd12     = ((a71 * vd12) + t13);
         vd13     = (((b72 + a72) * output06) + t12);

#if DSPLIB_DEBUGPRINT
//      if (samp == 0) {
//          printf("DSPLIB_DEBUGPRINT samp %d: vd0 %f vd1 %f vd2 %f vd3 %f vd4 %f vd5 %f\n", samp, vd0, vd1, vd2, vd3,
//          vd4, vd5); printf("DSPLIB_DEBUGPRINT samp %d: t1 %f t0 %f t3 %f t2 %f t5 %f t4 %f\n", samp, t1, t0, t3, t2,
//          t5, t4);
//      }
#endif

         output = output07 * outGain;

         // Output samples per channel
         *(dataType *) pOutLocal = output;
         pOutLocal               = (dataType *) pOutLocal + dataBufferOutPitch;
#if DSPLIB_DEBUGPRINT
         //         if (samp == 0)
         printf("DSPLIB_DEBUGPRINT numCh %d samp %d input1 %f output01 %f output02 %f output03 %f output07 %f output "
                "%f outGain %f pInLocal %p pOutLocal %p pOutLocal %f dataBufferOutPitch %d\n",
                numCh, samp, input1, output01, output02, output03, output07, output, outGain, pInLocal, pOutLocal,
                *(dataType *) pOutLocal, dataBufferOutPitch);
#endif
      }

      *(pFilterVarLocal)                  = vd0;
      *(pFilterVarLocal + filterVarPitch) = vd1;
      if (numStages >= 2) {
         *(pFilterVarLocal + 2 * filterVarPitch) = vd2;
         *(pFilterVarLocal + 3 * filterVarPitch) = vd3;
      }
      if (numStages >= 3) {
         *(pFilterVarLocal + 4 * filterVarPitch) = vd4;
         *(pFilterVarLocal + 5 * filterVarPitch) = vd5;
      }
      if (numStages >= 4) {
         *(pFilterVarLocal + 6 * filterVarPitch) = vd6;
         *(pFilterVarLocal + 7 * filterVarPitch) = vd7;
      }
      if (numStages >= 5) {
         *(pFilterVarLocal + 8 * filterVarPitch) = vd8;
         *(pFilterVarLocal + 9 * filterVarPitch) = vd9;
      }
      if (numStages >= 6) {
         *(pFilterVarLocal + 10 * filterVarPitch) = vd10;
         *(pFilterVarLocal + 11 * filterVarPitch) = vd11;
      }
      if (numStages >= 7) {
         *(pFilterVarLocal + 12 * filterVarPitch) = vd12;
         *(pFilterVarLocal + 13 * filterVarPitch) = vd13;
      }
      pFilterVarLocal++;
   }
   return (status);
}

// explicit instantiation for the different data type versions
template DSPLIB_STATUS DSPLIB_cascadeBiquad_exec_cn<float>(DSPLIB_kernelHandle handle,
                                                           void *restrict pIn,
                                                           void *restrict pFilterCoeff,
                                                           void *restrict pFilterVar,
                                                           void *restrict pOut);