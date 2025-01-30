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


#include "FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_priv.h"

FFTLIB_STATUS FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_init_cn(
   FFTLIB_kernelHandle handle,
   FFTLIB_bufParams1D_t *bufParamsX,
   FFTLIB_bufParams1D_t *bufParamsW,
   FFTLIB_bufParams1D_t *bufParamsY,
   FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_InitArgs *pKerInitArgs)
{
   FFTLIB_STATUS status = FFTLIB_SUCCESS;
   
   return status;
}

template <typename dataType, typename accumulateType, uint32_t MMA_SIZE>
FFTLIB_STATUS FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_exec_cn(
   FFTLIB_kernelHandle handle,
   void *restrict pX,
   const void *restrict pW,
   void *restrict pY)
{
   uint32_t h, i, j, k, l, m, n;
   uint32_t mCount, iCount, hCount;
   accumulateType yTempRe, yTempIm;
   accumulateType yProd;
   FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_PrivArgs *pKerPrivArgs = 
      (FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_PrivArgs *)handle;
   uint32_t fftSize = pKerPrivArgs->initArgs.fftSize;
   uint32_t batchSize = pKerPrivArgs->initArgs.batchSize;
   uint32_t shift0 = pKerPrivArgs->initArgs.shiftVector[0];
   uint32_t shift1 = pKerPrivArgs->initArgs.shiftVector[1];
   uint32_t shift2 = pKerPrivArgs->initArgs.shiftVector[2];
   uint32_t shift3 = pKerPrivArgs->initArgs.shiftVector[3];
   uint32_t dftShift = pKerPrivArgs->initArgs.shiftVector[4];
   uint32_t interleave = pKerPrivArgs->initArgs.interleave;
   uint32_t dftSizeInStage;
   uint32_t fftSizeModuloPower, decompositionFactorStage0;
   const dataType *pW1LogNStages;
   const dataType *pW2LogNStages;
   const dataType *pWDftStage;
   dataType *pSrc, *pDst;
   dataType wVal;
   dataType dVal;
   accumulateType yTemp;
   uint32_t logNStageCnt;
   uint32_t nonMmaMultShift;

   nonMmaMultShift = sizeof(dataType)*8-1;

   pSrc = (dataType *)pX;
   pDst = (dataType *)pY;
   if (interleave == 0) {
      for (i = 0; i < batchSize; i++) {
         for (j = 0; j < fftSize; j++) {
            pDst[i*fftSize*2+j*2] = pSrc[i*fftSize*2+j];
            pDst[i*fftSize*2+j*2+1] = pSrc[i*fftSize*2+fftSize+j];
         }
      }
      pSrc = (dataType *)pY;
      pDst = (dataType *)pX;
   }

   /* The FFT algorithm works by decomposing to smaller size DFT's stage
    * after stage.
    *    In all but the first stage, the decomposition factor is MMA_SIZE/2.
    * This decomposition factor is chosen so that each row of the decomposition 
    * matrix would fit one full vector length. The first stage would also use
    * a decomposition factor of MMA_SIZE/2 if fftSize is a power of MMA_SIZE/2.
    * In case fftSize is not a power of MMA_SIZE/2, the decomposition factor for
    * the first stage is less than MMA_SIZE/2.                                */
   fftSizeModuloPower = fftSize;
   while (fftSizeModuloPower > MMA_SIZE/2) {
      fftSizeModuloPower /= MMA_SIZE/2;
   }
   decompositionFactorStage0 = fftSizeModuloPower;
   
   dftSizeInStage = fftSize;
   logNStageCnt = 0;
   pW1LogNStages = (const dataType *)pKerPrivArgs->pW;
   pW2LogNStages = (const dataType *)pW;

   /***************************************************************************/
   /* Log N stage 0                                                           */
   /***************************************************************************/
   for (j = 0; j < batchSize; j++) {
      for (n = 0; n < (fftSize*2)/(MMA_SIZE*decompositionFactorStage0); n++) {
         for (k = 0; k < MMA_SIZE/2; k++) {
            for (l = 0; l < decompositionFactorStage0; l++) {
               yTempRe = 0;
               yTempIm = 0;
               for (m = 0; m < decompositionFactorStage0; m++) {
                  yProd = FFTLIB_UTIL_mult(
                          pW1LogNStages[l*MMA_SIZE+m*2],
                          pSrc[j*fftSize*2+(n*MMA_SIZE+k*2)+(m*fftSize*2)/decompositionFactorStage0+1]);
                  yTempIm = yTempIm + yProd;

                  yProd = FFTLIB_UTIL_mult(
                          pW1LogNStages[l*MMA_SIZE+m*2+1],
                          pSrc[j*fftSize*2+(n*MMA_SIZE+k*2)+(m*fftSize*2)/decompositionFactorStage0]);
                  yTempIm = yTempIm + yProd;

                  yProd = FFTLIB_UTIL_mult(
                          pW1LogNStages[l*MMA_SIZE+m*2],
                          pSrc[j*fftSize*2+(n*MMA_SIZE+k*2)+(m*fftSize*2)/decompositionFactorStage0]);
                  yTempRe = yTempRe + yProd;

                  dVal = FFTLIB_UTIL_negate(pSrc[j*fftSize*2+(n*MMA_SIZE+k*2)+(m*fftSize*2)/decompositionFactorStage0+1]);
                  yProd = FFTLIB_UTIL_mult(
                          pW1LogNStages[l*MMA_SIZE+m*2+1],
                          dVal);
                  yTempRe = yTempRe + yProd;
               }
               yProd = FFTLIB_UTIL_mult(
                          pW2LogNStages[n*MMA_SIZE*decompositionFactorStage0+k*2+l*MMA_SIZE],
                          FFTLIB_UTIL_shiftAndRound<accumulateType, dataType>(yTempRe, shift0));
               yTemp = yProd;
               yProd = FFTLIB_UTIL_mult(
                          pW2LogNStages[n*MMA_SIZE*decompositionFactorStage0+k*2+l*MMA_SIZE+1],
                          FFTLIB_UTIL_shiftAndRound<accumulateType, dataType>(yTempIm, shift0));
               yTemp = yTemp+yProd;
               pDst[j*fftSize*2+(n*MMA_SIZE+k*2)+(l*fftSize*2)/decompositionFactorStage0] =
                  FFTLIB_UTIL_shiftAndRound<accumulateType, dataType>(yTemp, nonMmaMultShift);

               wVal = FFTLIB_UTIL_negate(pW2LogNStages[n*MMA_SIZE*decompositionFactorStage0+k*2+l*MMA_SIZE]);
               yProd = FFTLIB_UTIL_mult(
                          wVal,
                          FFTLIB_UTIL_shiftAndRound<accumulateType, dataType>(yTempIm, shift0));
               yTemp = yProd;
               yProd = FFTLIB_UTIL_mult(
                          pW2LogNStages[n*MMA_SIZE*decompositionFactorStage0+k*2+l*MMA_SIZE+1],
                          FFTLIB_UTIL_shiftAndRound<accumulateType, dataType>(yTempRe, shift0));
               yTemp = yTemp+yProd;
               pDst[j*fftSize*2+(n*MMA_SIZE+k*2)+(l*fftSize*2)/decompositionFactorStage0+1] =
                  FFTLIB_UTIL_shiftAndRound<accumulateType, dataType>(yTemp, nonMmaMultShift);
            }
         }
      }
   }
   pSrc = (interleave == 0) ? (dataType *)pX : (dataType *)pY;
   pDst = (interleave == 0) ? (dataType *)pY : (dataType *)pX;
   pW2LogNStages += dftSizeInStage*2;
   dftSizeInStage /= decompositionFactorStage0;
   logNStageCnt++;
   pW1LogNStages += MMA_SIZE*MMA_SIZE/2;

   if (dftSizeInStage > MMA_SIZE/2) {
   /***************************************************************************/
   /* Log N stage 1                                                           */
   /***************************************************************************/
      for (j = 0; j < batchSize; j++) {
         for (k = 0; k < fftSize/dftSizeInStage; k++) {
            for (l = 0; l < (dftSizeInStage*4)/(MMA_SIZE*MMA_SIZE); l++) {
               for (i = 0; i < MMA_SIZE/2; i++) {
                  for (m = 0; m < MMA_SIZE/2; m++) {
                     yTempRe = 0;
                     yTempIm = 0;
                     for (n = 0; n < MMA_SIZE/2; n++) {
                        yProd = FFTLIB_UTIL_mult(
                                pW1LogNStages[m*MMA_SIZE+n*2],
                                pSrc[j*fftSize*2+k*dftSizeInStage*2+(l*MMA_SIZE+i*2)+n*(dftSizeInStage*4)/MMA_SIZE]);
                        yTempIm = yTempIm + yProd;

                        yProd = FFTLIB_UTIL_mult(
                                pW1LogNStages[m*MMA_SIZE+n*2+1],
                                pSrc[j*fftSize*2+k*dftSizeInStage*2+(l*MMA_SIZE+i*2)+n*(dftSizeInStage*4)/MMA_SIZE+1]);
                        yTempIm = yTempIm + yProd;

                        yProd = FFTLIB_UTIL_mult(
                                pW1LogNStages[m*MMA_SIZE+n*2],
                                pSrc[j*fftSize*2+k*dftSizeInStage*2+(l*MMA_SIZE+i*2)+n*(dftSizeInStage*4)/MMA_SIZE+1]);
                        yTempRe = yTempRe + yProd;
                        dVal = FFTLIB_UTIL_negate(pSrc[j*fftSize*2+k*dftSizeInStage*2+(l*MMA_SIZE+i*2)+n*(dftSizeInStage*4)/MMA_SIZE]);
                        yProd = FFTLIB_UTIL_mult(
                                pW1LogNStages[m*MMA_SIZE+n*2+1],
                                dVal);
                        yTempRe = yTempRe + yProd;
                     }
                     yProd = FFTLIB_UTIL_mult(
                                pW2LogNStages[l*MMA_SIZE*MMA_SIZE/2+i*2+m*MMA_SIZE],
                                FFTLIB_UTIL_shiftAndRound<accumulateType, dataType>(yTempRe, shift1));
                     yTemp = yProd;
                     yProd = FFTLIB_UTIL_mult(
                                pW2LogNStages[l*MMA_SIZE*MMA_SIZE/2+i*2+m*MMA_SIZE+1],
                                FFTLIB_UTIL_shiftAndRound<accumulateType, dataType>(yTempIm, shift1));
                     yTemp = yTemp+yProd;
                     pDst[j*fftSize*2+k*dftSizeInStage*2+l*MMA_SIZE+i*2+m*(dftSizeInStage*4)/MMA_SIZE] =
                        FFTLIB_UTIL_shiftAndRound<accumulateType, dataType>(yTemp, nonMmaMultShift);

                     wVal = FFTLIB_UTIL_negate(pW2LogNStages[l*MMA_SIZE*MMA_SIZE/2+i*2+m*MMA_SIZE]);
                     yProd = FFTLIB_UTIL_mult(
                                wVal,
                                FFTLIB_UTIL_shiftAndRound<accumulateType, dataType>(yTempIm, shift1));
                     yTemp = yProd;
                     yProd = FFTLIB_UTIL_mult(
                                pW2LogNStages[l*MMA_SIZE*MMA_SIZE/2+i*2+m*MMA_SIZE+1],
                                FFTLIB_UTIL_shiftAndRound<accumulateType, dataType>(yTempRe, shift1));
                     yTemp = yTemp+yProd;
                                
                     pDst[j*fftSize*2+k*dftSizeInStage*2+l*MMA_SIZE+i*2+m*(dftSizeInStage*4)/MMA_SIZE+1] =
                        FFTLIB_UTIL_shiftAndRound<accumulateType, dataType>(yTemp, nonMmaMultShift);
                  }
               }
            }
         }
      }
      pSrc = (interleave == 0) ? (dataType *)pY : (dataType *)pX;
      pDst = (interleave == 0) ? (dataType *)pX : (dataType *)pY;
      pW2LogNStages += dftSizeInStage*2;
      dftSizeInStage /= MMA_SIZE/2;
      logNStageCnt++;

      if (dftSizeInStage > MMA_SIZE/2) {
      /*********************************************************************/
      /* Log N stage 2                                                     */
      /*********************************************************************/
         for (j = 0; j < batchSize; j++) {
            for (k = 0; k < fftSize/dftSizeInStage; k++) {
               for (l = 0; l < (dftSizeInStage*4)/(MMA_SIZE*MMA_SIZE); l++) {
                  for (i = 0; i < MMA_SIZE/2; i++) {
                     for (m = 0; m < MMA_SIZE/2; m++) {
                        yTempRe = 0;
                        yTempIm = 0;
                        for (n = 0; n < MMA_SIZE/2; n++) {
                           yProd = FFTLIB_UTIL_mult(
                                   pW1LogNStages[m*MMA_SIZE+n*2],
                                   pSrc[j*fftSize*2+k*dftSizeInStage*2+(l*MMA_SIZE+i*2)+n*(dftSizeInStage*4)/MMA_SIZE]);
                           yTempIm = yTempIm + yProd;

                           yProd = FFTLIB_UTIL_mult(
                                   pW1LogNStages[m*MMA_SIZE+n*2+1],
                                   pSrc[j*fftSize*2+k*dftSizeInStage*2+(l*MMA_SIZE+i*2)+n*(dftSizeInStage*4)/MMA_SIZE+1]);
                           yTempIm = yTempIm + yProd;

                           yProd = FFTLIB_UTIL_mult(
                                   pW1LogNStages[m*MMA_SIZE+n*2],
                                   pSrc[j*fftSize*2+k*dftSizeInStage*2+(l*MMA_SIZE+i*2)+n*(dftSizeInStage*4)/MMA_SIZE+1]);
                           yTempRe = yTempRe + yProd;
                           dVal = FFTLIB_UTIL_negate(pSrc[j*fftSize*2+k*dftSizeInStage*2+(l*MMA_SIZE+i*2)+n*(dftSizeInStage*4)/MMA_SIZE]);
                           yProd = FFTLIB_UTIL_mult(
                                   pW1LogNStages[m*MMA_SIZE+n*2+1],
                                   dVal);
                           yTempRe = yTempRe + yProd;
                        }
                        yProd = FFTLIB_UTIL_mult(
                                   pW2LogNStages[l*MMA_SIZE*MMA_SIZE/2+i*2+m*MMA_SIZE],
                                   FFTLIB_UTIL_shiftAndRound<accumulateType, dataType>(yTempRe, shift2));
                        yTemp = yProd;
                        yProd = FFTLIB_UTIL_mult(
                                   pW2LogNStages[l*MMA_SIZE*MMA_SIZE/2+i*2+m*MMA_SIZE+1],
                                   FFTLIB_UTIL_shiftAndRound<accumulateType, dataType>(yTempIm, shift2));
                        yTemp = yTemp+yProd;
                        pDst[j*fftSize*2+k*dftSizeInStage*2+l*MMA_SIZE+i*2+m*(dftSizeInStage*4)/MMA_SIZE] =
                           FFTLIB_UTIL_shiftAndRound<accumulateType, dataType>(yTemp, nonMmaMultShift);

                        wVal = FFTLIB_UTIL_negate(pW2LogNStages[l*MMA_SIZE*MMA_SIZE/2+i*2+m*MMA_SIZE]);
                        yProd = FFTLIB_UTIL_mult(
                                   wVal,
                                   FFTLIB_UTIL_shiftAndRound<accumulateType, dataType>(yTempIm, shift2));
                        yTemp = yProd;
                        yProd = FFTLIB_UTIL_mult(
                                   pW2LogNStages[l*MMA_SIZE*MMA_SIZE/2+i*2+m*MMA_SIZE+1],
                                   FFTLIB_UTIL_shiftAndRound<accumulateType, dataType>(yTempRe, shift2));
                        yTemp = yTemp+yProd;
                                
                        pDst[j*fftSize*2+k*dftSizeInStage*2+l*MMA_SIZE+i*2+m*(dftSizeInStage*4)/MMA_SIZE+1] =
                           FFTLIB_UTIL_shiftAndRound<accumulateType, dataType>(yTemp, nonMmaMultShift);
                     }
                  }
               }
            }
         }
         pSrc = (interleave == 0) ? (dataType *)pX : (dataType *)pY;
         pDst = (interleave == 0) ? (dataType *)pY : (dataType *)pX;
         pW2LogNStages += dftSizeInStage*2;
         dftSizeInStage /= MMA_SIZE/2;
         logNStageCnt++;

         if (dftSizeInStage > MMA_SIZE/2) {
         /*********************************************************************/
         /* Log N stage 3                                                     */
         /*********************************************************************/
            for (j = 0; j < batchSize; j++) {
               for (k = 0; k < fftSize/dftSizeInStage; k++) {
                  for (l = 0; l < (dftSizeInStage*4)/(MMA_SIZE*MMA_SIZE); l++) {
                     for (i = 0; i < MMA_SIZE/2; i++) {
                        for (m = 0; m < MMA_SIZE/2; m++) {
                           yTempRe = 0;
                           yTempIm = 0;
                           for (n = 0; n < MMA_SIZE/2; n++) {
                              yProd = FFTLIB_UTIL_mult(
                                      pW1LogNStages[m*MMA_SIZE+n*2],
                                      pSrc[j*fftSize*2+k*dftSizeInStage*2+(l*MMA_SIZE+i*2)+n*(dftSizeInStage*4)/MMA_SIZE]);
                              yTempIm = yTempIm + yProd;

                              yProd = FFTLIB_UTIL_mult(
                                      pW1LogNStages[m*MMA_SIZE+n*2+1],
                                      pSrc[j*fftSize*2+k*dftSizeInStage*2+(l*MMA_SIZE+i*2)+n*(dftSizeInStage*4)/MMA_SIZE+1]);
                              yTempIm = yTempIm + yProd;

                              yProd = FFTLIB_UTIL_mult(
                                      pW1LogNStages[m*MMA_SIZE+n*2],
                                      pSrc[j*fftSize*2+k*dftSizeInStage*2+(l*MMA_SIZE+i*2)+n*(dftSizeInStage*4)/MMA_SIZE+1]);
                              yTempRe = yTempRe + yProd;
                              dVal = FFTLIB_UTIL_negate(pSrc[j*fftSize*2+k*dftSizeInStage*2+(l*MMA_SIZE+i*2)+n*(dftSizeInStage*4)/MMA_SIZE]);
                              yProd = FFTLIB_UTIL_mult(
                                      pW1LogNStages[m*MMA_SIZE+n*2+1],
                                      dVal);
                              yTempRe = yTempRe + yProd;
                           }
                           yProd = FFTLIB_UTIL_mult(
                                      pW2LogNStages[l*MMA_SIZE*MMA_SIZE/2+i*2+m*MMA_SIZE],
                                      FFTLIB_UTIL_shiftAndRound<accumulateType, dataType>(yTempRe, shift3));
                           yTemp = yProd;
                           yProd = FFTLIB_UTIL_mult(
                                      pW2LogNStages[l*MMA_SIZE*MMA_SIZE/2+i*2+m*MMA_SIZE+1],
                                      FFTLIB_UTIL_shiftAndRound<accumulateType, dataType>(yTempIm, shift3));
                           yTemp = yTemp+yProd;
                           pDst[j*fftSize*2+k*dftSizeInStage*2+l*MMA_SIZE+i*2+m*(dftSizeInStage*4)/MMA_SIZE] =
                              FFTLIB_UTIL_shiftAndRound<accumulateType, dataType>(yTemp, nonMmaMultShift);

                           wVal = FFTLIB_UTIL_negate(pW2LogNStages[l*MMA_SIZE*MMA_SIZE/2+i*2+m*MMA_SIZE]);
                           yProd = FFTLIB_UTIL_mult(
                                      wVal,
                                      FFTLIB_UTIL_shiftAndRound<accumulateType, dataType>(yTempIm, shift3));
                           yTemp = yProd;
                           yProd = FFTLIB_UTIL_mult(
                                      pW2LogNStages[l*MMA_SIZE*MMA_SIZE/2+i*2+m*MMA_SIZE+1],
                                      FFTLIB_UTIL_shiftAndRound<accumulateType, dataType>(yTempRe, shift3));
                           yTemp = yTemp+yProd;
                                   
                           pDst[j*fftSize*2+k*dftSizeInStage*2+l*MMA_SIZE+i*2+m*(dftSizeInStage*4)/MMA_SIZE+1] =
                              FFTLIB_UTIL_shiftAndRound<accumulateType, dataType>(yTemp, nonMmaMultShift);
                        }
                     }
                  }
               }
            }
            pSrc = (interleave == 0) ? (dataType *)pY : (dataType *)pX;
            pDst = (interleave == 0) ? (dataType *)pX : (dataType *)pY;
            pW2LogNStages += dftSizeInStage*2;
            dftSizeInStage /= MMA_SIZE/2;
            logNStageCnt++;
         }
      }
   }

#if 0
   pWDftStage = pW1LogNStages+MMA_SIZE*MMA_SIZE/2;
#else
   pWDftStage = pW2LogNStages;
#endif

   /* The following loop structure accomplishes two things:
    *  (1) calculates DFT of sets of 16 consecutive points
    *  (2) permutes the data to arrange it in radix-16 digit 
    *      reversal sequence.
    *
    *  The following mCount, iCount and hCount variables account for 
    *  different cases such as fftSize == MMA_SIZE*MMA_SIZE/4,
    *  MMA_SIZE*MMA_SIZE/4 < fftSize < MMA_SIZE*MMA_SIZE*MMA_SIZE/8,
    *  fftSize > MMA_SIZE*MMA_SIZE*MMA_SIZE/8.                       */
   mCount = (fftSize*2)/(MMA_SIZE*decompositionFactorStage0) > MMA_SIZE/2?
            MMA_SIZE/2 : (fftSize*2)/(MMA_SIZE*decompositionFactorStage0);
   iCount = (fftSize*2)/(MMA_SIZE*decompositionFactorStage0) > MMA_SIZE/2?
            ((fftSize*4)/(MMA_SIZE*MMA_SIZE*decompositionFactorStage0) > MMA_SIZE/2 ?
             MMA_SIZE/2 : (fftSize*4)/(MMA_SIZE*MMA_SIZE*decompositionFactorStage0) ) : 1;
   hCount = (fftSize*4)/(MMA_SIZE*MMA_SIZE*decompositionFactorStage0) > MMA_SIZE/2 ?
            (fftSize*8)/(MMA_SIZE*MMA_SIZE*MMA_SIZE*decompositionFactorStage0) : 1;
            
   for (j = 0; j < batchSize; j++) {
      for (k = 0; k < decompositionFactorStage0; k++) {
         for (m = 0; m < mCount; m++) {
            for (i = 0; i < iCount; i++) {
               for (h = 0; h < hCount; h++) {
                  for (l = 0; l < MMA_SIZE/2; l++) {
                     yTempRe = 0;
                     yTempIm = 0;
                     for (n = 0; n < MMA_SIZE/2; n++) {
                        yProd = FFTLIB_UTIL_mult(
                                   pWDftStage[l*2*MMA_SIZE+n*2],
                                   pSrc[j*fftSize*2+k*(fftSize*2)/decompositionFactorStage0+m*(fftSize*4)/(decompositionFactorStage0*MMA_SIZE)+i*(fftSize*8)/(decompositionFactorStage0*MMA_SIZE*MMA_SIZE)+h*MMA_SIZE+n*2]);
                        yTempRe = yTempRe + yProd;

                        yProd = FFTLIB_UTIL_mult(
                                   pWDftStage[l*2*MMA_SIZE+n*2+1],
                                   pSrc[j*fftSize*2+k*(fftSize*2)/decompositionFactorStage0+m*(fftSize*4)/(decompositionFactorStage0*MMA_SIZE)+i*(fftSize*8)/(decompositionFactorStage0*MMA_SIZE*MMA_SIZE)+h*MMA_SIZE+n*2+1]);
                        yTempRe = yTempRe + yProd;

                        yProd = FFTLIB_UTIL_mult(
                                   pWDftStage[(l*2+1)*MMA_SIZE+n*2],
                                   pSrc[j*fftSize*2+k*(fftSize*2)/decompositionFactorStage0+m*(fftSize*4)/(decompositionFactorStage0*MMA_SIZE)+i*(fftSize*8)/(decompositionFactorStage0*MMA_SIZE*MMA_SIZE)+h*MMA_SIZE+n*2]);
                        yTempIm = yTempIm + yProd;

                        yProd = FFTLIB_UTIL_mult(
                                   pWDftStage[(l*2+1)*MMA_SIZE+n*2+1],
                                   pSrc[j*fftSize*2+k*(fftSize*2)/decompositionFactorStage0+m*(fftSize*4)/(decompositionFactorStage0*MMA_SIZE)+i*(fftSize*8)/(decompositionFactorStage0*MMA_SIZE*MMA_SIZE)+h*MMA_SIZE+n*2+1]);
                        yTempIm = yTempIm + yProd;
                     }

                     pDst[j*fftSize*2+l*decompositionFactorStage0*mCount*iCount*hCount*2+h*decompositionFactorStage0*mCount*iCount*2+i*decompositionFactorStage0*mCount*2+m*decompositionFactorStage0*2+k*2] =
                        FFTLIB_UTIL_shiftAndRound<accumulateType, dataType>(yTempRe, dftShift);
                     pDst[j*fftSize*2+l*decompositionFactorStage0*mCount*iCount*hCount*2+h*decompositionFactorStage0*mCount*iCount*2+i*decompositionFactorStage0*mCount*2+m*decompositionFactorStage0*2+k*2+1] =
                        FFTLIB_UTIL_shiftAndRound<accumulateType, dataType>(yTempIm, dftShift);
                  }
               }
            }
         }
      }
   }

   if (interleave == 0) {
      pSrc = ((logNStageCnt & 1u) != 0) ? (dataType *)pY : (dataType *)pX;
      pDst = ((logNStageCnt & 1u) != 0) ? (dataType *)pX : (dataType *)pY;

      for (i = 0; i < batchSize; i++) {
         for (j = 0; j < fftSize; j++) {
            pDst[i*fftSize*2+j] = pSrc[i*fftSize*2+j*2];
            pDst[i*fftSize*2+fftSize+j] = pSrc[i*fftSize*2+j*2+1];
         }
      }
   }
   if (logNStageCnt & 1u) {
      pSrc = (dataType *)pX;
      pDst = (dataType *)pY;
      for (j = 0; j < fftSize*batchSize*2; j++) {
         pDst[j] = pSrc[j];
      }
   }

   return FFTLIB_SUCCESS;
}

// explicit instantiation for the different data type versions
/* Check the template parameter values here */
template FFTLIB_STATUS FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_exec_cn<int16_t, int64_t, FFTLIB_MMA_SIZE_16_BIT>(
   FFTLIB_kernelHandle handle,
   void *restrict pX,
   const void *restrict pW,
   void *restrict pY);

/* Check the template parameter values here */
template FFTLIB_STATUS FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_exec_cn<int32_t, FFTLIB_int128_t, FFTLIB_MMA_SIZE_32_BIT>(
   FFTLIB_kernelHandle handle,
   void *restrict pX,
   const void *restrict pW,
   void *restrict pY);
