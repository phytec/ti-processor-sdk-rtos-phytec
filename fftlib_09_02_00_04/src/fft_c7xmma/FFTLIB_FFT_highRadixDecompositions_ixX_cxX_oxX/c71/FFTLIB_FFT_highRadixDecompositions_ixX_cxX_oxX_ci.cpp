/******************************************************************************/
/*!
 * \file FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX.c
 */
/* Copyright (C) 2017 Texas Instruments Incorporated - http://www.ti.com/
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************/

/*******************************************************************************
 *
 * INCLUDES
 *
 ******************************************************************************/

#include "../../../common/c71/FFTLIB_inlines.h"
#include "../../../common/c71/c7x_inlines.h"
#include "../FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_priv.h"

#define SE_PARAM_BASE                  (0x0000)
#define SA_LOGNSTAGE0_TW1_PARAM_OFFSET (SE_PARAM_BASE)
#define SE_LOGNSTAGE0_TW2_PARAM_OFFSET                                         \
   (SA_LOGNSTAGE0_TW1_PARAM_OFFSET + SE_PARAM_SIZE)
#define SA_LOGNSTAGE1_TW1_PARAM_OFFSET                                         \
   (SE_LOGNSTAGE0_TW2_PARAM_OFFSET + SE_PARAM_SIZE)
#define SE_LOGNSTAGE1_TW2_PARAM_OFFSET                                         \
   (SA_LOGNSTAGE1_TW1_PARAM_OFFSET + SE_PARAM_SIZE)
#define SA_LOGNSTAGE2_TW1_PARAM_OFFSET                                         \
   (SE_LOGNSTAGE1_TW2_PARAM_OFFSET + SE_PARAM_SIZE)
#define SE_LOGNSTAGE2_TW2_PARAM_OFFSET                                         \
   (SA_LOGNSTAGE2_TW1_PARAM_OFFSET + SE_PARAM_SIZE)
#define SA_LOGNSTAGE3_TW1_PARAM_OFFSET                                         \
   (SE_LOGNSTAGE2_TW2_PARAM_OFFSET + SE_PARAM_SIZE)
#define SE_LOGNSTAGE3_TW2_PARAM_OFFSET                                         \
   (SA_LOGNSTAGE3_TW1_PARAM_OFFSET + SE_PARAM_SIZE)
#define SE_LOGNSTAGE0_DATA_PARAM_OFFSET                                        \
   (SE_LOGNSTAGE3_TW2_PARAM_OFFSET + SE_PARAM_SIZE)
#define SE_LOGNSTAGE1_DATA_PARAM_OFFSET                                        \
   (SE_LOGNSTAGE0_DATA_PARAM_OFFSET + SE_PARAM_SIZE)
#define SE_LOGNSTAGE2_DATA_PARAM_OFFSET                                        \
   (SE_LOGNSTAGE1_DATA_PARAM_OFFSET + SE_PARAM_SIZE)
#define SE_LOGNSTAGE3_DATA_PARAM_OFFSET                                        \
   (SE_LOGNSTAGE2_DATA_PARAM_OFFSET + SE_PARAM_SIZE)
#define SA_LOGNSTAGE0_OUT_PARAM_OFFSET                                         \
   (SE_LOGNSTAGE3_DATA_PARAM_OFFSET + SE_PARAM_SIZE)
#define SA_LOGNSTAGE1_OUT_PARAM_OFFSET                                         \
   (SA_LOGNSTAGE0_OUT_PARAM_OFFSET + SE_PARAM_SIZE)
#define SA_LOGNSTAGE2_OUT_PARAM_OFFSET                                         \
   (SA_LOGNSTAGE1_OUT_PARAM_OFFSET + SE_PARAM_SIZE)
#define SA_LOGNSTAGE3_OUT_PARAM_OFFSET                                         \
   (SA_LOGNSTAGE2_OUT_PARAM_OFFSET + SE_PARAM_SIZE)
#if 0
#define SA_DFTSTAGE_TW_PARAM_OFFSET                                            \
   (SA_LOGNSTAGE3_OUT_PARAM_OFFSET + SE_PARAM_SIZE)
#define SE_DFTSTAGE_DATA_PARAM_OFFSET                                          \
   (SA_DFTSTAGE_TW_PARAM_OFFSET + SE_PARAM_SIZE)
#else
#define SE_DFTSTAGE_TW_PARAM_OFFSET                                            \
   (SA_LOGNSTAGE3_OUT_PARAM_OFFSET + SE_PARAM_SIZE)
#define SE_DFTSTAGE_DATA_PARAM_OFFSET                                          \
   (SE_DFTSTAGE_TW_PARAM_OFFSET + SE_PARAM_SIZE)
#endif
#define SA_DFTSTAGE_OUT_PARAM_OFFSET                                           \
   (SE_DFTSTAGE_DATA_PARAM_OFFSET + SE_PARAM_SIZE)
#define SA_DFTSTAGE2_OUT_PARAM_OFFSET                                          \
   (SA_DFTSTAGE_OUT_PARAM_OFFSET + SE_PARAM_SIZE)
#define SE_INT_DATA_PARAM_OFFSET   (SA_DFTSTAGE2_OUT_PARAM_OFFSET + SE_PARAM_SIZE)
#define SA_INT_OUT_PARAM_OFFSET    (SE_INT_DATA_PARAM_OFFSET + SE_PARAM_SIZE)
#define SE_DEINT_DATA_PARAM_OFFSET (SA_INT_OUT_PARAM_OFFSET + SE_PARAM_SIZE)
#define SA_DEINT_OUT_PARAM_OFFSET  (SE_DEINT_DATA_PARAM_OFFSET + SE_PARAM_SIZE)
#define PERMCTRLINT_OFFSET         (SA_DEINT_OUT_PARAM_OFFSET + SE_PARAM_SIZE)
#define PERMCTRLDEINT_OFFSET       (PERMCTRLINT_OFFSET + sizeof (uchar64))
#define PERMCTRL64PT_OFFSET        (PERMCTRLDEINT_OFFSET + sizeof (uchar64))
#define DECOMPOSITIONFACTORSTAGE0_OFFSET                                       \
   (PERMCTRL64PT_OFFSET + sizeof (uchar64))

#define MMA_PARAM_BASE              (DECOMPOSITIONFACTORSTAGE0_OFFSET + sizeof (uint32_t))
#define MMA_LOGNSTAGE0_PARAM_OFFSET MMA_PARAM_BASE
#define MMA_LOGNSTAGE1_PARAM_OFFSET                                            \
   (MMA_LOGNSTAGE0_PARAM_OFFSET + FFTLIB_PARAM_SIZE)
#define MMA_LOGNSTAGE2_PARAM_OFFSET                                            \
   (MMA_LOGNSTAGE1_PARAM_OFFSET + FFTLIB_PARAM_SIZE)
#define MMA_LOGNSTAGE3_PARAM_OFFSET                                            \
   (MMA_LOGNSTAGE2_PARAM_OFFSET + FFTLIB_PARAM_SIZE)
#define MMA_DFTSTAGE_PARAM_OFFSET                                              \
   (MMA_LOGNSTAGE3_PARAM_OFFSET + FFTLIB_PARAM_SIZE)

#define MMA_LOGNSTAGE0_CONFIGREG_OFFSET (MMA_LOGNSTAGE0_PARAM_OFFSET)
#define MMA_LOGNSTAGE0_OFFSETREG_OFFSET                                        \
   (MMA_LOGNSTAGE0_CONFIGREG_OFFSET + sizeof (FFTLIB_MMA_CONFIG_REG))
#define MMA_LOGNSTAGE1_CONFIGREG_OFFSET (MMA_LOGNSTAGE1_PARAM_OFFSET)
#define MMA_LOGNSTAGE1_OFFSETREG_OFFSET                                        \
   (MMA_LOGNSTAGE1_CONFIGREG_OFFSET + sizeof (FFTLIB_MMA_CONFIG_REG))
#define MMA_LOGNSTAGE2_CONFIGREG_OFFSET (MMA_LOGNSTAGE2_PARAM_OFFSET)
#define MMA_LOGNSTAGE2_OFFSETREG_OFFSET                                        \
   (MMA_LOGNSTAGE2_CONFIGREG_OFFSET + sizeof (FFTLIB_MMA_CONFIG_REG))
#define MMA_LOGNSTAGE3_CONFIGREG_OFFSET (MMA_LOGNSTAGE3_PARAM_OFFSET)
#define MMA_LOGNSTAGE3_OFFSETREG_OFFSET                                        \
   (MMA_LOGNSTAGE3_CONFIGREG_OFFSET + sizeof (FFTLIB_MMA_CONFIG_REG))
#define MMA_DFTSTAGE_CONFIGREG_OFFSET (MMA_DFTSTAGE_PARAM_OFFSET)
#define MMA_DFTSTAGE_OFFSETREG_OFFSET                                          \
   (MMA_DFTSTAGE_CONFIGREG_OFFSET + sizeof (FFTLIB_MMA_CONFIG_REG))

__attribute__ ((section (".l1dmemory")))
uint8_t FFTLIB_FFT_twData[FFTLIB_MMA_SIZE_16_BIT * FFTLIB_MMA_SIZE_16_BIT * 3 *
                          sizeof (int16_t)];

void FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_perfEst (
    FFTLIB_kernelHandle         handle,
    const FFTLIB_bufParams1D_t *bufParamsX,
    const FFTLIB_bufParams1D_t *bufParamsY,
    const FFTLIB_bufParams1D_t *bufParamsW,
    uint32_t                    fftSize,
    uint32_t                    batchSize,
    uint32_t                    interleave,
    uint64_t                   *archCycles,
    uint64_t                   *estCycles)
{
   // determine the mmaWidth based upon output data type
   uint32_t MMA_SIZE;
   uint32_t numStages, dftSizeCurrStage;
   uint32_t fftSizeModuloPower;
   uint32_t decompositionFactorStage0;
   uint32_t startupCycles, teardownCycles;
   uint32_t overheadCycles, operationCycles;
   uint32_t interleaveComputeCycles, interleaveOperationCycles,
       interleaveOverheadCycles, interleaveStartupCycles,
       interleaveTeardownCycles;
   uint32_t logNStage0ComputeCycles, logNStage0OperationCycles,
       logNStage0OverheadCycles, logNStage0StartupCycles,
       logNStage0TeardownCycles;
   uint32_t logNStage1ComputeCycles, logNStage1OperationCycles,
       logNStage1OverheadCycles, logNStage1StartupCycles,
       logNStage1TeardownCycles;
   uint32_t logNStage2ComputeCycles, logNStage2OperationCycles,
       logNStage2OverheadCycles, logNStage2StartupCycles,
       logNStage2TeardownCycles;
   uint32_t logNStage3ComputeCycles, logNStage3OperationCycles,
       logNStage3OverheadCycles, logNStage3StartupCycles,
       logNStage3TeardownCycles;
   uint32_t dftStageComputeCycles, dftStageOperationCycles,
       dftStageOverheadCycles, dftStageStartupCycles, dftStageTeardownCycles,
       dftStageComputeCyclesPartialPanel, dftStageComputeCyclesTemp;
   uint32_t deinterleaveComputeCycles, deinterleaveOperationCycles,
       deinterleaveOverheadCycles, deinterleaveStartupCycles,
       deinterleaveTeardownCycles;

   if (bufParamsX->data_type == FFTLIB_INT8) {
      MMA_SIZE = FFTLIB_MMA_SIZE_8_BIT;
   }
   else if (bufParamsX->data_type == FFTLIB_INT16) {
      MMA_SIZE = FFTLIB_MMA_SIZE_16_BIT;
   }
   else {
      MMA_SIZE = FFTLIB_MMA_SIZE_32_BIT;
   }

   fftSizeModuloPower = fftSize;
   while (fftSizeModuloPower > MMA_SIZE / 2) {
      fftSizeModuloPower /= MMA_SIZE / 2;
   }
   decompositionFactorStage0 = fftSizeModuloPower;

   dftSizeCurrStage = fftSize;
   dftSizeCurrStage /= decompositionFactorStage0;
   numStages = 1;
   while (dftSizeCurrStage > MMA_SIZE / 2) {
      dftSizeCurrStage /= MMA_SIZE / 2;
      numStages++;
   }

   interleaveStartupCycles   = 0;
   interleaveComputeCycles   = 0;
   interleaveOperationCycles = 0;
   interleaveOverheadCycles  = 0;
   interleaveTeardownCycles  = 0;
   if (interleave == 0) {
      interleaveStartupCycles += 5 + // Load SE config
                                 17; // SE0 open

      interleaveComputeCycles +=
          (fftSize * batchSize * 2) / MMA_SIZE + 3; // interleave loop

      interleaveOperationCycles += interleaveComputeCycles;

      interleaveOverheadCycles += 0;

      interleaveTeardownCycles += 4; // Go to protected mode and SE0 close
   }

   logNStage0StartupCycles   = 0;
   logNStage0ComputeCycles   = 0;
   logNStage0OperationCycles = 0;
   logNStage0OverheadCycles  = 0;
   logNStage0TeardownCycles  = 0;

   logNStage0StartupCycles += 5 +        // load SE config
                              17 +       // SE0 open
                              MMA_SIZE + // load B panel using SE0
                              8;         // 4 row compute loop

   logNStage0OverheadCycles += 1 + // load SE config
                               1 + // SE0 open
                               1;  // load B panel using SE0

   if (fftSize * batchSize * 16 == MMA_SIZE * MMA_SIZE) {

      logNStage0StartupCycles += 1; // if checks

      logNStage0ComputeCycles += MMA_SIZE / 8; // compute and store

      logNStage0OperationCycles += 4 + // load using SA0 latency
                                   logNStage0ComputeCycles +
                                   29; // Store, CMPY latency
   }
   else if (fftSize * batchSize * 8 == MMA_SIZE * MMA_SIZE) {

      logNStage0StartupCycles += 2; // if checks

      logNStage0ComputeCycles += MMA_SIZE / 4; // compute and store

      logNStage0OperationCycles += 4 + // load using SA0 latency
                                   logNStage0ComputeCycles +
                                   29; // Store, CMPY latency
   }
   else if (fftSize * batchSize * 16 == 3 * MMA_SIZE * MMA_SIZE) {

      logNStage0StartupCycles += 4; // if checks

      logNStage0ComputeCycles += (3 * MMA_SIZE) / 8; // compute and store

      logNStage0OperationCycles += 4 + // load using SA0 latency
                                   logNStage0ComputeCycles +
                                   29; // Store, CMPY latency
   }
   else if (fftSize * batchSize * 4 == MMA_SIZE * MMA_SIZE) {

      logNStage0StartupCycles += 5; // if checks

      logNStage0ComputeCycles += MMA_SIZE / 2; // compute and store

      logNStage0OperationCycles += 4 + // load using SA0 latency
                                   logNStage0ComputeCycles +
                                   29; // Store, CMPY latency
   }
   else if (fftSize * batchSize * 16 == 5 * MMA_SIZE * MMA_SIZE) {

      logNStage0StartupCycles += 6; // if checks

      logNStage0ComputeCycles += (5 * MMA_SIZE) / 4; // compute and store

      logNStage0OperationCycles += 4 + // load using SA0 latency
                                   logNStage0ComputeCycles +
                                   29; // Store, CMPY latency
   }
   else if (fftSize * batchSize * 8 == 3 * MMA_SIZE * MMA_SIZE) {

      logNStage0StartupCycles += 7; // if checks

      logNStage0ComputeCycles += (3 * MMA_SIZE) / 2; // compute and store

      logNStage0OperationCycles += 4 + // load using SA0 latency
                                   logNStage0ComputeCycles +
                                   29; // Store, CMPY latency
   }
   else if (fftSize * batchSize * 16 == 7 * MMA_SIZE * MMA_SIZE) {

      logNStage0StartupCycles += 8; // if checks

      logNStage0ComputeCycles += (7 * MMA_SIZE) / 4; // compute and store

      logNStage0OperationCycles += 4 + // load using SA0 latency
                                   logNStage0ComputeCycles +
                                   29; // Store, CMPY latency
   }
   else if (fftSize * batchSize * 2 == MMA_SIZE * MMA_SIZE) {

      logNStage0StartupCycles += 10; // if checks

      logNStage0ComputeCycles += MMA_SIZE * 2; // compute and store

      logNStage0OperationCycles += 4 + // load using SA0 latency
                                   logNStage0ComputeCycles +
                                   29; // Store, CMPY latency
   }
   else {

      logNStage0StartupCycles += 12; // if checks

      logNStage0ComputeCycles +=
          (fftSize * batchSize * 4) / MMA_SIZE; // compute and store

      logNStage0OperationCycles += 4 + // load using SA0 latency
                                   logNStage0ComputeCycles +
                                   29; // Store, CMPY latency
   }

   logNStage0TeardownCycles += 5; // SA/SE close

   logNStage1StartupCycles   = 0;
   logNStage1TeardownCycles  = 0;
   logNStage1ComputeCycles   = 0;
   logNStage1OperationCycles = 0;
   logNStage1OverheadCycles  = 0;
   logNStage2StartupCycles   = 0;
   logNStage2TeardownCycles  = 0;
   logNStage2ComputeCycles   = 0;
   logNStage2OperationCycles = 0;
   logNStage2OverheadCycles  = 0;
   logNStage3StartupCycles   = 0;
   logNStage3TeardownCycles  = 0;
   logNStage3ComputeCycles   = 0;
   logNStage3OperationCycles = 0;
   logNStage3OverheadCycles  = 0;
   if (numStages > 1) {
      logNStage1StartupCycles += 5 +        // load SE config
                                 17 +       // SE0 open
                                 MMA_SIZE + // load B panel using SE0
                                 12 +       // 4 row compute loop
                                 2;         // if checks

      logNStage1OverheadCycles += 1 + // SE0 open
                                  1;  // load B panel using SE0

      logNStage1ComputeCycles += (fftSize * batchSize * 4) / MMA_SIZE;

      logNStage1OperationCycles += 4 + // load using SA0 latency
                                   logNStage1ComputeCycles +
                                   29; // Store, CMPY latency

      logNStage1TeardownCycles += 4; // SA/SE close

      if (numStages > 2) {
         logNStage2StartupCycles += 5 +        // load SE config
                                    17 +       // SE0 open
                                    MMA_SIZE + // load B panel using SE0
                                    12;        // 4 row compute loop

         logNStage2OverheadCycles += 1 + // SE0 open
                                     1;  // load B panel using SE0

         logNStage2ComputeCycles += (fftSize * batchSize * 4) / MMA_SIZE;

         logNStage2OperationCycles += 4 + // load using SA0 latency
                                      logNStage2ComputeCycles +
                                      29; // Store, CMPY latency

         logNStage2TeardownCycles += 4; // SA/SE close

         if (numStages > 3) {
            logNStage3StartupCycles += 5 +        // load SE config
                                       17 +       // SE0 open
                                       MMA_SIZE + // load B panel using SE0
                                       12;        // 4 row compute loop

            logNStage3OverheadCycles += 1 + // SE0 open
                                        1;  // load B panel using SE0

            logNStage3ComputeCycles += (fftSize * batchSize * 4) / MMA_SIZE;

            logNStage3OperationCycles += 4 + // load using SA0 latency
                                         logNStage3ComputeCycles +
                                         29; // Store, CMPY latency

            logNStage3TeardownCycles += 4; // SA/SE close
         }
      }
   }

   dftStageStartupCycles   = 0;
   dftStageTeardownCycles  = 0;
   dftStageComputeCycles   = 0;
   dftStageOperationCycles = 0;
   dftStageOverheadCycles  = 0;

   dftStageStartupCycles += 5 +        // load SE configuration
                            17 +       // SE0 open
                            MMA_SIZE + // load B panel using SE0
                            7 +        // load SA config and open SA
                            7;         // 4 row compute loop

   dftStageOverheadCycles += 1; // load SE config

   if (fftSize * 16 == MMA_SIZE * MMA_SIZE) {
      dftStageStartupCycles += 1; // if checks

      if (batchSize < 8) {

         dftStageStartupCycles += 1; // if checks

         dftStageComputeCycles += 2 * MMA_SIZE; // compute, store and permute

         dftStageOperationCycles += 4 + // load using SA0 latency
                                    dftStageComputeCycles +
                                    40; // store and permute latency
      }
      else if (batchSize == 8) {
         dftStageStartupCycles += 9; // if checks

         dftStageComputeCycles += 2 * MMA_SIZE; // compute, store and permute

         dftStageOperationCycles += 4 + // load using SA0 latency
                                    dftStageComputeCycles +
                                    40; // store and permute latency
      }
      else {
         dftStageStartupCycles += 10; // if checks

         dftStageComputeCycles += (2 * ((batchSize / 8) * 8) * fftSize * 2) /
                                  MMA_SIZE; // compute, store and permute

         dftStageOperationCycles += 4 + // load using SA0 latency
                                    dftStageComputeCycles +
                                    40 + // store and permute latency
                                    9;   // partial panel if check

         if (batchSize & 7u) {
            dftStageComputeCyclesPartialPanel =
                MMA_SIZE * 2; // compute, store and permute

            dftStageComputeCyclesTemp = dftStageComputeCycles;

            dftStageComputeCycles += dftStageComputeCyclesPartialPanel;

            dftStageOperationCycles +=
                4 + // load using SA0 latency
                dftStageComputeCycles -
                dftStageComputeCyclesTemp + // dftStageComputeCyclesPartialPanel(MISRA
                                            // C workaround)
                40; // store and permute latency
         }
      }
   }
   else if (fftSize * 8 == MMA_SIZE * MMA_SIZE) {
      dftStageStartupCycles += 3; // if checks

      if (batchSize < 4) {

         dftStageStartupCycles += 1; // if checks

         dftStageComputeCycles +=
             (6 * MMA_SIZE) / 4; // compute, store and permute

         dftStageOperationCycles += 4 + // load using SA0 latency
                                    dftStageComputeCycles +
                                    34; // store and permute latency
      }
      else if (batchSize == 4) {
         dftStageStartupCycles += 9; // if checks

         dftStageComputeCycles +=
             (6 * MMA_SIZE) / 4; // compute, store and permute

         dftStageOperationCycles += 4 + // load using SA0 latency
                                    dftStageComputeCycles +
                                    38; // store and permute latency
      }
      else {
         dftStageStartupCycles += 10; // if checks

         dftStageComputeCycles += (6 * ((batchSize / 4) * 4) * fftSize * 2) /
                                  (MMA_SIZE * 4); // compute, store and permute

         dftStageOperationCycles += 4 + // load using SA0 latency
                                    dftStageComputeCycles +
                                    38 + // store and permute latency
                                    8;   // partial panel if check

         if (batchSize & 3u) {
            dftStageComputeCyclesPartialPanel =
                (6 * MMA_SIZE) / 4; // compute, store and permute

            dftStageComputeCyclesTemp = dftStageComputeCycles;

            dftStageComputeCycles += dftStageComputeCyclesPartialPanel;

            dftStageOperationCycles +=
                4 + // load using SA0 latency
                dftStageComputeCycles -
                dftStageComputeCyclesTemp + // dftStageComputeCyclesPartialPanel(MISRA
                                            // C workaround)
                38; // store and permute latency
         }
      }
   }
   else if (fftSize * 4 == MMA_SIZE * MMA_SIZE) {
      dftStageStartupCycles += 5; // if checks

      if (batchSize < 2) {

         dftStageStartupCycles += 1; // if checks

         dftStageComputeCycles += MMA_SIZE; // compute, store and permute

         dftStageOperationCycles += 4 + // load using SA0 latency
                                    dftStageComputeCycles +
                                    30; // store and permute latency
      }
      else if (batchSize == 2) {
         dftStageStartupCycles += 10; // if checks

         dftStageComputeCycles += MMA_SIZE; // compute, store and permute

         dftStageOperationCycles += 4 + // load using SA0 latency
                                    dftStageComputeCycles +
                                    30; // store and permute latency
      }
      else {
         dftStageStartupCycles += 13; // if checks

         dftStageComputeCycles += (((batchSize / 2) * 2) * fftSize * 2) /
                                  MMA_SIZE; // compute, store and permute

         dftStageOperationCycles += 4 + // load using SA0 latency
                                    dftStageComputeCycles +
                                    30 + // store and permute latency
                                    4;   // partial panel if check

         if (batchSize & 1u) {
            dftStageComputeCyclesPartialPanel =
                MMA_SIZE; // compute, store and permute

            dftStageComputeCyclesTemp = dftStageComputeCycles;

            dftStageComputeCycles += dftStageComputeCyclesPartialPanel;

            dftStageOperationCycles +=
                4 + // load using SA0 latency
                dftStageComputeCycles -
                dftStageComputeCyclesTemp + // dftStageComputeCyclesPartialPanel(MISRA
                                            // C workaround)
                30; // store and permute latency
         }
      }
   }
   else if (fftSize * 2 == MMA_SIZE * MMA_SIZE) {
      dftStageStartupCycles += 9; // if checks

      dftStageComputeCycles +=
          (fftSize * batchSize * 2) / MMA_SIZE; // compute, store and permute

      dftStageOperationCycles += 4 + // load using SA0 latency
                                 dftStageComputeCycles +
                                 30; // store and permute latency
   }
   else {
      dftStageStartupCycles += 10; // if checks

      dftStageComputeCycles +=
          (fftSize * batchSize * 2) / MMA_SIZE; // compute, store and permute

      dftStageOperationCycles += 4 + // load using SA0 latency
                                 dftStageComputeCycles +
                                 30; // store and permute latency
   }

   dftStageTeardownCycles += 3; // SE/SA close

   deinterleaveStartupCycles   = 0;
   deinterleaveComputeCycles   = 0;
   deinterleaveOperationCycles = 0;
   deinterleaveOverheadCycles  = 0;
   deinterleaveTeardownCycles  = 0;
   if (interleave == 0) {
      deinterleaveStartupCycles += 5 + // Load SE config
                                   17; // SE0 open

      deinterleaveComputeCycles +=
          (fftSize * batchSize * 2) / MMA_SIZE + 3; // deinterleave loop

      deinterleaveOperationCycles += deinterleaveComputeCycles;

      deinterleaveOverheadCycles += 0;

      deinterleaveTeardownCycles += 4; // Go to protected mode and SE0 close
   }

   startupCycles = 6 +  // exec() overhead
                   12 + // kernel function call
                   interleaveStartupCycles + logNStage0StartupCycles +
                   logNStage1StartupCycles + logNStage2StartupCycles +
                   logNStage3StartupCycles + dftStageStartupCycles +
                   deinterleaveStartupCycles;

   teardownCycles = interleaveTeardownCycles + logNStage0TeardownCycles +
                    logNStage1TeardownCycles + logNStage2TeardownCycles +
                    logNStage3TeardownCycles + dftStageTeardownCycles +
                    deinterleaveTeardownCycles + 6; // Function return

   operationCycles = interleaveOperationCycles + logNStage0OperationCycles +
                     logNStage1OperationCycles + logNStage2OperationCycles +
                     logNStage3OperationCycles + dftStageOperationCycles +
                     deinterleaveOperationCycles;

   overheadCycles = interleaveOverheadCycles + logNStage0OverheadCycles +
                    logNStage1OverheadCycles + logNStage2OverheadCycles +
                    logNStage3OverheadCycles + dftStageOverheadCycles +
                    deinterleaveOverheadCycles;

#if FFTLIB_DEBUGPRINT
   printf ("interleaveStartupCycles: %d\n", interleaveStartupCycles);
   printf ("interleaveComputeCycles: %d\n", interleaveComputeCycles);
   printf ("interleaveOperationCycles: %d\n", interleaveOperationCycles);
   printf ("interleaveOverheadCycles: %d\n", interleaveOverheadCycles);
   printf ("interleaveTeardownCycles: %d\n", interleaveTeardownCycles);
   printf ("\n");

   printf ("logNStage0StartupCycles: %d\n", logNStage0StartupCycles);
   printf ("logNStage0ComputeCycles: %d\n", logNStage0ComputeCycles);
   printf ("logNStage0OperationCycles: %d\n", logNStage0OperationCycles);
   printf ("logNStage0OverheadCycles: %d\n", logNStage0OverheadCycles);
   printf ("logNStage0TeardownCycles: %d\n", logNStage0TeardownCycles);
   printf ("\n");

   printf ("logNStage1StartupCycles: %d\n", logNStage1StartupCycles);
   printf ("logNStage1ComputeCycles: %d\n", logNStage1ComputeCycles);
   printf ("logNStage1OperationCycles: %d\n", logNStage1OperationCycles);
   printf ("logNStage1OverheadCycles: %d\n", logNStage1OverheadCycles);
   printf ("logNStage1TeardownCycles: %d\n", logNStage1TeardownCycles);
   printf ("\n");

   printf ("logNStage2StartupCycles: %d\n", logNStage2StartupCycles);
   printf ("logNStage2ComputeCycles: %d\n", logNStage2ComputeCycles);
   printf ("logNStage2OperationCycles: %d\n", logNStage2OperationCycles);
   printf ("logNStage2OverheadCycles: %d\n", logNStage2OverheadCycles);
   printf ("logNStage2TeardownCycles: %d\n", logNStage2TeardownCycles);
   printf ("\n");

   printf ("logNStage3StartupCycles: %d\n", logNStage3StartupCycles);
   printf ("logNStage3ComputeCycles: %d\n", logNStage3ComputeCycles);
   printf ("logNStage3OperationCycles: %d\n", logNStage3OperationCycles);
   printf ("logNStage3OverheadCycles: %d\n", logNStage3OverheadCycles);
   printf ("logNStage3TeardownCycles: %d\n", logNStage3TeardownCycles);
   printf ("\n");

   printf ("dftStageStartupCycles: %d\n", dftStageStartupCycles);
   printf ("dftStageComputeCycles: %d\n", dftStageComputeCycles);
   printf ("dftStageOperationCycles: %d\n", dftStageOperationCycles);
   printf ("dftStageOverheadCycles: %d\n", dftStageOverheadCycles);
   printf ("dftStageTeardownCycles: %d\n", dftStageTeardownCycles);
   printf ("\n");

   printf ("deinterleaveStartupCycles: %d\n", deinterleaveStartupCycles);
   printf ("deinterleaveComputeCycles: %d\n", deinterleaveComputeCycles);
   printf ("deinterleaveOperationCycles: %d\n", deinterleaveOperationCycles);
   printf ("deinterleaveOverheadCycles: %d\n", deinterleaveOverheadCycles);
   printf ("deinterleaveTeardownCycles: %d\n", deinterleaveTeardownCycles);
   printf ("\n");
#endif

   *archCycles = startupCycles + operationCycles + teardownCycles;
   *estCycles =
       startupCycles + operationCycles + overheadCycles + teardownCycles;

   return;
}

#ifdef _HOST_BUILD
cint8 __as_cint8 (uchar64 src)
{
   cint8 dst;
   int16 a = __as_int16 (src);

   dst.s[0].i() = a.s[0];
   dst.s[0].r() = a.s[1];
   dst.s[1].i() = a.s[2];
   dst.s[1].r() = a.s[3];
   dst.s[2].i() = a.s[4];
   dst.s[2].r() = a.s[5];
   dst.s[3].i() = a.s[6];
   dst.s[3].r() = a.s[7];
   dst.s[4].i() = a.s[8];
   dst.s[4].r() = a.s[9];
   dst.s[5].i() = a.s[10];
   dst.s[5].r() = a.s[11];
   dst.s[6].i() = a.s[12];
   dst.s[6].r() = a.s[13];
   dst.s[7].i() = a.s[14];
   dst.s[7].r() = a.s[15];

   return dst;
}

cshort16 __as_cshort16 (uchar64 src)
{
   cshort16 dst;
   short32  a   = __as_short32 (src);
   short16  aLo = a.lo();
   short16  aHi = a.hi();

   dst.s[0].i() = aLo.s[0];
   dst.s[0].r() = aLo.s[1];
   dst.s[1].i() = aLo.s[2];
   dst.s[1].r() = aLo.s[3];
   dst.s[2].i() = aLo.s[4];
   dst.s[2].r() = aLo.s[5];
   dst.s[3].i() = aLo.s[6];
   dst.s[3].r() = aLo.s[7];
   dst.s[4].i() = aLo.s[8];
   dst.s[4].r() = aLo.s[9];
   dst.s[5].i() = aLo.s[10];
   dst.s[5].r() = aLo.s[11];
   dst.s[6].i() = aLo.s[12];
   dst.s[6].r() = aLo.s[13];
   dst.s[7].i() = aLo.s[14];
   dst.s[7].r() = aLo.s[15];

   dst.s[8].i() = aHi.s[0];
   dst.s[8].r() = aHi.s[1];
   dst.s[9].i() = aHi.s[2];
   dst.s[9].r() = aHi.s[3];
   dst.s[10].i() = aHi.s[4];
   dst.s[10].r() = aHi.s[5];
   dst.s[11].i() = aHi.s[6];
   dst.s[11].r() = aHi.s[7];
   dst.s[12].i() = aHi.s[8];
   dst.s[12].r() = aHi.s[9];
   dst.s[13].i() = aHi.s[10];
   dst.s[13].r() = aHi.s[11];
   dst.s[14].i() = aHi.s[12];
   dst.s[14].r() = aHi.s[13];
   dst.s[15].i() = aHi.s[14];
   dst.s[15].r() = aHi.s[15];

   return dst;
}
#endif

#ifdef __cplusplus
#pragma FUNC_ALWAYS_INLINE
template <typename dataType>
FFTLIB_STATIC_INLINE void FFTLIB_UTIL_SE0loadVCrotBload (int32_t numRows)
{
   int32_t r;
   uchar64 valB;

   _nassert (numRows > 0);
   for (r = 0; r < numRows; r++) {
      valB = __SE0ADV (uchar64);
      __HWALDB (valB);
      __HWAADV ();
      if (sizeof (dataType) == sizeof (int32_t)) {
         __HWALDB (__as_uchar64 (__crot90 (__as_cint8 (valB))));
         __HWAADV ();
      }
      else {
         __HWALDB (__as_uchar64 (__crot90 (__as_cshort16 (valB))));
         __HWAADV ();
      }
   }

   return;
}
#endif

#ifdef __cplusplus
#pragma FUNC_ALWAYS_INLINE
template <typename dataType>
FFTLIB_STATIC_INLINE void FFTLIB_UTIL_SA0AloadSE0loadVCrotBloadComputeC (
    int32_t numRows,
    FFTLIB_const uint8_t *restrict pSrc)
{
   int32_t r;
   uchar64 valA;
   uchar64 valB;

   _nassert (numRows > 0);
   for (r = 0; r < numRows; r++) {
      valA = *__SA0ADV (uchar64, pSrc);
      valB = __SE0ADV (uchar64);
      __HWALDB (__as_uchar64 (valB));
      __HWAADV ();
      if (sizeof (dataType) == sizeof (int32_t)) {
         __HWALDAB (__as_uchar64 (valA),
                    __as_uchar64 (__crot90 (__as_cint8 (valB))));
         __HWAOP (__MMA_A_LDA);
         __HWAADV ();
      }
      else {
         __HWALDAB (__as_uchar64 (valA),
                    __as_uchar64 (__crot90 (__as_cshort16 (valB))));
         __HWAOP (__MMA_A_LDA);
         __HWAADV ();
      }
   }

   return;
}
#endif

/* ---------------------------------------------------------------- */
/*  MISRAC Rule 8.13(NEEDS.CONST) Deviation: pDst cannot be declared*/
/*  with const qualifier because data is copied into buffer pointed */
/*  by the pointer.                                                 */
/* ---------------------------------------------------------------- */
#ifdef __cplusplus
#pragma FUNC_ALWAYS_INLINE
template <typename dataType>
FFTLIB_STATIC_INLINE void FFTLIB_UTIL_SA0AloadComputeCSE1MultSA1RegStore (
    int32_t numRows,
    FFTLIB_const uint8_t *restrict pSrc,
    uint8_t *restrict pDst)
{
   int32_t r;
   uchar64 valA;
   uchar64 valC;
   __vpred saVpred;

   _nassert (numRows > 0);
   for (r = 0; r < numRows; r++) {
      valA = *__SA0ADV (uchar64, pSrc);
      __HWALDA (__as_uchar64 (valA));
      __HWAOPXFER (__MMA_A_LDA);
      if (sizeof (dataType) == sizeof (int32_t)) {
         valC = __as_uchar64 (__HWARCV (0));
         __HWAADV ();
         saVpred = __SA1_VPRED (uchar64);
         __vstore_pred (
             saVpred, __SA1ADV (uchar64, pDst),
             __as_uchar64 (__cmpy_fx (__as_cint8 (valC),
                                      __as_cint8 (__SE1ADV (uchar64)))));
      }
      else {
         valC = __as_uchar64 (__HWARCV (0));
         __HWAADV ();
         saVpred = __SA1_VPRED (uchar64);
         __vstore_pred (
             saVpred, __SA1ADV (uchar64, pDst),
             __as_uchar64 (__cmpy_fx (__as_cshort16 (valC),
                                      __as_cshort16 (__SE1ADV (uchar64)))));
      }
   }

   return;
}
#endif

/* ---------------------------------------------------------------- */
/*  MISRAC Rule 8.13(NEEDS.CONST) Deviation: pDst cannot be declared*/
/*  with const qualifier because data is copied into buffer pointed */
/*  by the pointer.                                                 */
/* ---------------------------------------------------------------- */
#ifdef __cplusplus
#pragma FUNC_ALWAYS_INLINE
template <typename dataType>
FFTLIB_STATIC_INLINE void
FFTLIB_UTIL_SA0AloadSE0loadVCrotBloadComputeCSE1MultSA1RegStore (
    int32_t numRows,
    FFTLIB_const uint8_t *restrict pSrc,
    uint8_t *restrict pDst)
{
   int32_t r;
   uchar64 valA;
   uchar64 valB;
   uchar64 valC;
   __vpred saVpred;

   _nassert (numRows > 0);
   for (r = 0; r < numRows; r++) {
      valA = *__SA0ADV (uchar64, pSrc);
      valB = __SE0ADV (uchar64);
      __HWALDB (__as_uchar64 (valB));
      __HWAADV ();
      if (sizeof (dataType) == sizeof (int32_t)) {
         __HWALDAB (__as_uchar64 (valA),
                    __as_uchar64 (__crot90 (__as_cint8 (valB))));
         __HWAOPXFER (__MMA_A_LDA);
         valC = __as_uchar64 (__HWARCV (0));
         __HWAADV ();
         saVpred = __SA1_VPRED (uchar64);
         __vstore_pred (
             saVpred, __SA1ADV (uchar64, pDst),
             __as_uchar64 (__cmpy_fx (__as_cint8 (valC),
                                      __as_cint8 (__SE1ADV (uchar64)))));
      }
      else {
         __HWALDAB (__as_uchar64 (valA),
                    __as_uchar64 (__crot90 (__as_cshort16 (valB))));
         __HWAOPXFER (__MMA_A_LDA);
         valC = __as_uchar64 (__HWARCV (0));
         __HWAADV ();
         saVpred = __SA1_VPRED (uchar64);
         __vstore_pred (
             saVpred, __SA1ADV (uchar64, pDst),
             __as_uchar64 (__cmpy_fx (__as_cshort16 (valC),
                                      __as_cshort16 (__SE1ADV (uchar64)))));
      }
   }

   return;
}
#endif

#if 0
#ifdef __cplusplus
#pragma FUNC_ALWAYS_INLINE
FFTLIB_STATIC_INLINE void FFTLIB_UTIL_SA0AloadComputeCPermLlHhSA1Store(
   int32_t numRows,
   uint8_t *restrict pSrc,
   uint8_t *restrict pDst,
   uchar64 permCtrl)
{
   int32_t r;
   uchar64 valA, valC1, valC2;
   int32_t loopCount;
   __vpred saVpred;

   loopCount = numRows >> 1;
   _nassert(loopCount > 0);
   for (r = 0; r < loopCount; r++) {
      valA = *__SA0ADV(uchar64, pSrc);
      __HWALDA(valA);
      __HWAOPXFER(__MMA_A_LDA);
      valC1 = __as_uchar64(__HWARCV(0));
      __HWAADV();
      valA = *__SA0ADV(uchar64, pSrc);
      __HWALDA(valA);
      __HWAOPXFER(__MMA_A_LDA);
      valC2 = __as_uchar64(__HWARCV(0));
      __HWAADV();
      saVpred = __SA1_VPRED(uchar64);
      __vstore_pred(saVpred, __SA1ADV(uchar64, pDst), __permute_low_low(permCtrl, valC2, valC1));
      saVpred = __SA1_VPRED(uchar64);
      __vstore_pred(saVpred, __SA1ADV(uchar64, pDst), __permute_high_high(permCtrl, valC2, valC1));
   }

   return;
}
#endif
#else
#ifdef __cplusplus
#pragma FUNC_ALWAYS_INLINE
FFTLIB_STATIC_INLINE void
FFTLIB_UTIL_SE1AloadComputeCPermLlHhSA1Store (int32_t numRows,
                                              uint8_t *restrict pDst,
                                              uchar64 permCtrl)
{
   int32_t r;
   uchar64 valA, valC1, valC2;
   int32_t loopCount;
   __vpred saVpred;

   loopCount = __shift_right (numRows, 1);
   _nassert (loopCount > 0);
   for (r = 0; r < loopCount; r++) {
      valA = __SE1ADV (uchar64);
      __HWALDA (valA);
      __HWAOPXFER (__MMA_A_LDA);
      valC1 = __as_uchar64 (__HWARCV (0));
      __HWAADV ();
      valA = __SE1ADV (uchar64);
      __HWALDA (valA);
      __HWAOPXFER (__MMA_A_LDA);
      valC2 = __as_uchar64 (__HWARCV (0));
      __HWAADV ();
      saVpred = __SA1_VPRED (uchar64);
      __vstore_pred (saVpred, __SA1ADV (uchar64, pDst),
                     __permute_low_low (permCtrl, valC2, valC1));
      saVpred = __SA1_VPRED (uchar64);
      __vstore_pred (saVpred, __SA1ADV (uchar64, pDst),
                     __permute_high_high (permCtrl, valC2, valC1));
   }

   return;
}
#endif
#endif

#if 0
#ifdef __cplusplus
#pragma FUNC_ALWAYS_INLINE
FFTLIB_STATIC_INLINE void FFTLIB_UTIL_SA0AloadSE0BloadComputeCPermLlHhSA1Store(
   int32_t numRows,
   uint8_t *restrict pSrc,
   uint8_t *restrict pDst,
   uchar64 permCtrl)
{
   int32_t r;
   uchar64 valA, valC1, valC2;
   uchar64 valB;
   int32_t loopCount;
   __vpred saVpred;

   loopCount = numRows >> 1;
   _nassert(loopCount > 0);
   for (r = 0; r < loopCount; r++) {
      valA = *__SA0ADV(uchar64, pSrc);
      valB = __SE0ADV(uchar64);
      __HWALDAB(valA, valB);
      __HWAOPXFER(__MMA_A_LDA);
      valC1 = __as_uchar64(__HWARCV(0));
      __HWAADV();
      valA = *__SA0ADV(uchar64, pSrc);
      valB = __SE0ADV(uchar64);
      __HWALDAB(valA, valB);
      __HWAOPXFER(__MMA_A_LDA);
      valC2 = __as_uchar64(__HWARCV(0));
      __HWAADV();
      saVpred = __SA1_VPRED(uchar64);
      __vstore_pred(saVpred, __SA1ADV(uchar64, pDst), __permute_low_low(permCtrl, valC2, valC1));
      saVpred = __SA1_VPRED(uchar64);
      __vstore_pred(saVpred, __SA1ADV(uchar64, pDst), __permute_high_high(permCtrl, valC2, valC1));
   }

   return;
}
#endif
#else
#ifdef __cplusplus
#pragma FUNC_ALWAYS_INLINE
FFTLIB_STATIC_INLINE void
FFTLIB_UTIL_SE1AloadSE0BloadComputeCPermLlHhSA1Store (int32_t numRows,
                                                      uint8_t *restrict pDst,
                                                      uchar64 permCtrl)
{
   int32_t r;
   uchar64 valA, valC1, valC2;
   uchar64 valB;
   int32_t loopCount;
   __vpred saVpred;

   loopCount = __shift_right (numRows, 1);
   _nassert (loopCount > 0);
   for (r = 0; r < loopCount; r++) {
      valA = __SE1ADV (uchar64);
      valB = __SE0ADV (uchar64);
      __HWALDAB (valA, valB);
      __HWAOPXFER (__MMA_A_LDA);
      valC1 = __as_uchar64 (__HWARCV (0));
      __HWAADV ();
      valA = __SE1ADV (uchar64);
      valB = __SE0ADV (uchar64);
      __HWALDAB (valA, valB);
      __HWAOPXFER (__MMA_A_LDA);
      valC2 = __as_uchar64 (__HWARCV (0));
      __HWAADV ();
      saVpred = __SA1_VPRED (uchar64);
      __vstore_pred (saVpred, __SA1ADV (uchar64, pDst),
                     __permute_low_low (permCtrl, valC2, valC1));
      saVpred = __SA1_VPRED (uchar64);
      __vstore_pred (saVpred, __SA1ADV (uchar64, pDst),
                     __permute_high_high (permCtrl, valC2, valC1));
   }

   return;
}
#endif
#endif

#if 0
#ifdef __cplusplus
#pragma FUNC_ALWAYS_INLINE
FFTLIB_STATIC_INLINE void FFTLIB_UTIL_SA0AloadComputeCPermLlMvLlMvHhSA1Store(
   int32_t numRows,
   uint8_t *restrict pSrc,
   uint8_t *restrict pDst,
   uchar64 permCtrl)
{
   int32_t r;
   uchar64 valA, valC1, valC2, valC3, valC4;
   int32_t loopCount;
   uchar64 valDst1, valDst2, valDst3, valDst4;
   __vpred saVpred;

   loopCount = numRows >> 2;
   _nassert(loopCount > 0);
   for (r = 0; r < loopCount; r++) {
      valA = *__SA0ADV(uchar64, pSrc);
      __HWALDA(valA);
      __HWAOPXFER(__MMA_A_LDA);
      valC1 = __as_uchar64(__HWARCV(0));
      __HWAADV();
      valA = *__SA0ADV(uchar64, pSrc);
      __HWALDA(valA);
      __HWAOPXFER(__MMA_A_LDA);
      valC2 = __as_uchar64(__HWARCV(0));
      __HWAADV();
      valA = *__SA0ADV(uchar64, pSrc);
      __HWALDA(valA);
      __HWAOPXFER(__MMA_A_LDA);
      valC3 = __as_uchar64(__HWARCV(0));
      __HWAADV();
      valA = *__SA0ADV(uchar64, pSrc);
      __HWALDA(valA);
      __HWAOPXFER(__MMA_A_LDA);
      valC4 = __as_uchar64(__HWARCV(0));
      __HWAADV();
      valDst1 = __permute_low_low(permCtrl, valC2, valC1);
      valDst2 = __permute_low_low(permCtrl, valC4, valC3);
      saVpred = __SA1_VPRED(uchar64);
      __vstore_pred(saVpred, __SA1ADV(uchar64, pDst), __vllmv_vvv(valDst2, valDst1));
      saVpred = __SA1_VPRED(uchar64);
      __vstore_pred(saVpred, __SA1ADV(uchar64, pDst), __vhhmv_vvv(valDst2, valDst1));
      valDst3 = __permute_high_high(permCtrl, valC2, valC1);
      valDst4 = __permute_high_high(permCtrl, valC4, valC3);
      saVpred = __SA1_VPRED(uchar64);
      __vstore_pred(saVpred, __SA1ADV(uchar64, pDst), __vllmv_vvv(valDst4, valDst3));
      saVpred = __SA1_VPRED(uchar64);
      __vstore_pred(saVpred, __SA1ADV(uchar64, pDst), __vhhmv_vvv(valDst4, valDst3));
   }

   return;
}
#endif
#else
#ifdef __cplusplus
#pragma FUNC_ALWAYS_INLINE
FFTLIB_STATIC_INLINE void
FFTLIB_UTIL_SE1AloadComputeCPermLlMvLlMvHhSA1Store (int32_t numRows,
                                                    uint8_t *restrict pDst,
                                                    uchar64 permCtrl)
{
   int32_t r;
   uchar64 valA, valC1, valC2, valC3, valC4;
   int32_t loopCount;
   uchar64 valDst1, valDst2, valDst3, valDst4;
   __vpred saVpred;

   loopCount = __shift_right (numRows, 2);
   _nassert (loopCount > 0);
   for (r = 0; r < loopCount; r++) {
      valA = __SE1ADV (uchar64);
      __HWALDA (valA);
      __HWAOPXFER (__MMA_A_LDA);
      valC1 = __as_uchar64 (__HWARCV (0));
      __HWAADV ();
      valA = __SE1ADV (uchar64);
      __HWALDA (valA);
      __HWAOPXFER (__MMA_A_LDA);
      valC2 = __as_uchar64 (__HWARCV (0));
      __HWAADV ();
      valA = __SE1ADV (uchar64);
      __HWALDA (valA);
      __HWAOPXFER (__MMA_A_LDA);
      valC3 = __as_uchar64 (__HWARCV (0));
      __HWAADV ();
      valA = __SE1ADV (uchar64);
      __HWALDA (valA);
      __HWAOPXFER (__MMA_A_LDA);
      valC4 = __as_uchar64 (__HWARCV (0));
      __HWAADV ();
      valDst1 = __permute_low_low (permCtrl, valC2, valC1);
      valDst2 = __permute_low_low (permCtrl, valC4, valC3);
      saVpred = __SA1_VPRED (uchar64);
      __vstore_pred (saVpred, __SA1ADV (uchar64, pDst),
                     __vllmv_vvv (valDst2, valDst1));
      saVpred = __SA1_VPRED (uchar64);
      __vstore_pred (saVpred, __SA1ADV (uchar64, pDst),
                     __vhhmv_vvv (valDst2, valDst1));
      valDst3 = __permute_high_high (permCtrl, valC2, valC1);
      valDst4 = __permute_high_high (permCtrl, valC4, valC3);
      saVpred = __SA1_VPRED (uchar64);
      __vstore_pred (saVpred, __SA1ADV (uchar64, pDst),
                     __vllmv_vvv (valDst4, valDst3));
      saVpred = __SA1_VPRED (uchar64);
      __vstore_pred (saVpred, __SA1ADV (uchar64, pDst),
                     __vhhmv_vvv (valDst4, valDst3));
   }

   return;
}
#endif
#endif

#if 0
#ifdef __cplusplus
#pragma FUNC_ALWAYS_INLINE
FFTLIB_STATIC_INLINE void FFTLIB_UTIL_SA0AloadSE0BloadComputeCPermLlMvLlMvHhSA1Store(
   int32_t numRows,
   uint8_t *restrict pSrc,
   uint8_t *restrict pDst,
   uchar64 permCtrl)
{
   int32_t r;
   uchar64 valA, valC1, valC2, valC3, valC4;
   uchar64 valB;
   int32_t loopCount;
   uchar64 valDst1, valDst2, valDst3, valDst4;
   __vpred saVpred;

   loopCount = numRows >> 2;
   _nassert(loopCount > 0);
   for (r = 0; r < loopCount; r++) {
      valA = *__SA0ADV(uchar64, pSrc);
      valB = __SE0ADV(uchar64);
      __HWALDAB(valA, valB);
      __HWAOPXFER(__MMA_A_LDA);
      valC1 = __as_uchar64(__HWARCV(0));
      __HWAADV();
      valA = *__SA0ADV(uchar64, pSrc);
      valB = __SE0ADV(uchar64);
      __HWALDAB(valA, valB);
      __HWAOPXFER(__MMA_A_LDA);
      valC2 = __as_uchar64(__HWARCV(0));
      __HWAADV();
      valA = *__SA0ADV(uchar64, pSrc);
      valB = __SE0ADV(uchar64);
      __HWALDAB(valA, valB);
      __HWAOPXFER(__MMA_A_LDA);
      valC3 = __as_uchar64(__HWARCV(0));
      __HWAADV();
      valA = *__SA0ADV(uchar64, pSrc);
      valB = __SE0ADV(uchar64);
      __HWALDAB(valA, valB);
      __HWAOPXFER(__MMA_A_LDA);
      valC4 = __as_uchar64(__HWARCV(0));
      __HWAADV();
      valDst1 = __permute_low_low(permCtrl, valC2, valC1);
      valDst2 = __permute_low_low(permCtrl, valC4, valC3);
      saVpred = __SA1_VPRED(uchar64);
      __vstore_pred(saVpred, __SA1ADV(uchar64, pDst), __vllmv_vvv(valDst2, valDst1));
      saVpred = __SA1_VPRED(uchar64);
      __vstore_pred(saVpred, __SA1ADV(uchar64, pDst), __vhhmv_vvv(valDst2, valDst1));
      valDst3 = __permute_high_high(permCtrl, valC2, valC1);
      valDst4 = __permute_high_high(permCtrl, valC4, valC3);
      saVpred = __SA1_VPRED(uchar64);
      __vstore_pred(saVpred, __SA1ADV(uchar64, pDst), __vllmv_vvv(valDst4, valDst3));
      saVpred = __SA1_VPRED(uchar64);
      __vstore_pred(saVpred, __SA1ADV(uchar64, pDst), __vhhmv_vvv(valDst4, valDst3));
   }

   return;
}
#endif
#else
#ifdef __cplusplus
#pragma FUNC_ALWAYS_INLINE
FFTLIB_STATIC_INLINE void
FFTLIB_UTIL_SE1AloadSE0BloadComputeCPermLlMvLlMvHhSA1Store (
    int32_t numRows,
    uint8_t *restrict pDst,
    uchar64 permCtrl)
{
   int32_t r;
   uchar64 valA, valC1, valC2, valC3, valC4;
   uchar64 valB;
   int32_t loopCount;
   uchar64 valDst1, valDst2, valDst3, valDst4;
   __vpred saVpred;

   loopCount = __shift_right (numRows, 2);
   _nassert (loopCount > 0);
   for (r = 0; r < loopCount; r++) {
      valA = __SE1ADV (uchar64);
      valB = __SE0ADV (uchar64);
      __HWALDAB (valA, valB);
      __HWAOPXFER (__MMA_A_LDA);
      valC1 = __as_uchar64 (__HWARCV (0));
      __HWAADV ();
      valA = __SE1ADV (uchar64);
      valB = __SE0ADV (uchar64);
      __HWALDAB (valA, valB);
      __HWAOPXFER (__MMA_A_LDA);
      valC2 = __as_uchar64 (__HWARCV (0));
      __HWAADV ();
      valA = __SE1ADV (uchar64);
      valB = __SE0ADV (uchar64);
      __HWALDAB (valA, valB);
      __HWAOPXFER (__MMA_A_LDA);
      valC3 = __as_uchar64 (__HWARCV (0));
      __HWAADV ();
      valA = __SE1ADV (uchar64);
      valB = __SE0ADV (uchar64);
      __HWALDAB (valA, valB);
      __HWAOPXFER (__MMA_A_LDA);
      valC4 = __as_uchar64 (__HWARCV (0));
      __HWAADV ();
      valDst1 = __permute_low_low (permCtrl, valC2, valC1);
      valDst2 = __permute_low_low (permCtrl, valC4, valC3);
      saVpred = __SA1_VPRED (uchar64);
      __vstore_pred (saVpred, __SA1ADV (uchar64, pDst),
                     __vllmv_vvv (valDst2, valDst1));
      saVpred = __SA1_VPRED (uchar64);
      __vstore_pred (saVpred, __SA1ADV (uchar64, pDst),
                     __vhhmv_vvv (valDst2, valDst1));
      valDst3 = __permute_high_high (permCtrl, valC2, valC1);
      valDst4 = __permute_high_high (permCtrl, valC4, valC3);
      saVpred = __SA1_VPRED (uchar64);
      __vstore_pred (saVpred, __SA1ADV (uchar64, pDst),
                     __vllmv_vvv (valDst4, valDst3));
      saVpred = __SA1_VPRED (uchar64);
      __vstore_pred (saVpred, __SA1ADV (uchar64, pDst),
                     __vhhmv_vvv (valDst4, valDst3));
   }

   return;
}
#endif
#endif

#if 0
#ifdef __cplusplus
#pragma FUNC_ALWAYS_INLINE
FFTLIB_STATIC_INLINE void FFTLIB_UTIL_SA0AloadComputeCPermLlhhPermLlhhMvLlhhSA1Store(
   int32_t numRows,
   uint8_t *restrict pSrc,
   uint8_t *restrict pDst,
   uchar64 permCtrl1,
   uchar64 permCtrl2)
{
   int32_t r;
   uchar64 valA;
   uchar64 valC1, valC2, valC3, valC4;
   uchar64 valC5, valC6, valC7, valC8;
   int32_t loopCount;
   uchar64 valDst1, valDst2, valDst3, valDst4;
   uchar64 valDst12, valDst34;
   __vpred saVpred;

   loopCount = numRows >> 3;
   _nassert(loopCount > 0);
   for (r = 0; r < loopCount; r++) {
      valA = *__SA0ADV(uchar64, pSrc);
      __HWALDA(valA);
      __HWAOPXFER(__MMA_A_LDA);
      valC1 = __as_uchar64(__HWARCV(0));
      __HWAADV();
      valA = *__SA0ADV(uchar64, pSrc);
      __HWALDA(valA);
      __HWAOPXFER(__MMA_A_LDA);
      valC2 = __as_uchar64(__HWARCV(0));
      __HWAADV();
      valA = *__SA0ADV(uchar64, pSrc);
      __HWALDA(valA);
      __HWAOPXFER(__MMA_A_LDA);
      valC3 = __as_uchar64(__HWARCV(0));
      __HWAADV();
      valA = *__SA0ADV(uchar64, pSrc);
      __HWALDA(valA);
      __HWAOPXFER(__MMA_A_LDA);
      valC4 = __as_uchar64(__HWARCV(0));
      __HWAADV();
      valA = *__SA0ADV(uchar64, pSrc);
      __HWALDA(valA);
      __HWAOPXFER(__MMA_A_LDA);
      valC5 = __as_uchar64(__HWARCV(0));
      __HWAADV();
      valA = *__SA0ADV(uchar64, pSrc);
      __HWALDA(valA);
      __HWAOPXFER(__MMA_A_LDA);
      valC6 = __as_uchar64(__HWARCV(0));
      __HWAADV();
      valA = *__SA0ADV(uchar64, pSrc);
      __HWALDA(valA);
      __HWAOPXFER(__MMA_A_LDA);
      valC7 = __as_uchar64(__HWARCV(0));
      __HWAADV();
      valA = *__SA0ADV(uchar64, pSrc);
      __HWALDA(valA);
      __HWAOPXFER(__MMA_A_LDA);
      valC8 = __as_uchar64(__HWARCV(0));
      __HWAADV();
      valDst1 = __permute_low_low(permCtrl1, valC2, valC1);
      valDst2 = __permute_low_low(permCtrl1, valC4, valC3);
      valDst3 = __permute_low_low(permCtrl1, valC6, valC5);
      valDst4 = __permute_low_low(permCtrl1, valC8, valC7);
      valDst12 = __permute_low_low(permCtrl2, valDst2, valDst1);
      valDst34 = __permute_low_low(permCtrl2, valDst4, valDst3);
      saVpred = __SA1_VPRED(uchar64);
      __vstore_pred(saVpred, __SA1ADV(uchar64, pDst), __vllmv_vvv(valDst34, valDst12));
      saVpred = __SA1_VPRED(uchar64);
      __vstore_pred(saVpred, __SA1ADV(uchar64, pDst), __vhhmv_vvv(valDst34, valDst12));
      valDst12 = __permute_high_high(permCtrl2, valDst2, valDst1);
      valDst34 = __permute_high_high(permCtrl2, valDst4, valDst3);
      saVpred = __SA1_VPRED(uchar64);
      __vstore_pred(saVpred, __SA1ADV(uchar64, pDst), __vllmv_vvv(valDst34, valDst12));
      saVpred = __SA1_VPRED(uchar64);
      __vstore_pred(saVpred, __SA1ADV(uchar64, pDst), __vhhmv_vvv(valDst34, valDst12));
      valDst1 = __permute_high_high(permCtrl1, valC2, valC1);
      valDst2 = __permute_high_high(permCtrl1, valC4, valC3);
      valDst3 = __permute_high_high(permCtrl1, valC6, valC5);
      valDst4 = __permute_high_high(permCtrl1, valC8, valC7);
      valDst12 = __permute_low_low(permCtrl2, valDst2, valDst1);
      valDst34 = __permute_low_low(permCtrl2, valDst4, valDst3);
      saVpred = __SA1_VPRED(uchar64);
      __vstore_pred(saVpred, __SA1ADV(uchar64, pDst), __vllmv_vvv(valDst34, valDst12));
      saVpred = __SA1_VPRED(uchar64);
      __vstore_pred(saVpred, __SA1ADV(uchar64, pDst), __vhhmv_vvv(valDst34, valDst12));
      valDst12 = __permute_high_high(permCtrl2, valDst2, valDst1);
      valDst34 = __permute_high_high(permCtrl2, valDst4, valDst3);
      saVpred = __SA1_VPRED(uchar64);
      __vstore_pred(saVpred, __SA1ADV(uchar64, pDst), __vllmv_vvv(valDst34, valDst12));
      saVpred = __SA1_VPRED(uchar64);
      __vstore_pred(saVpred, __SA1ADV(uchar64, pDst), __vhhmv_vvv(valDst34, valDst12));
   }

   return;
}
#endif
#else
#ifdef __cplusplus
#pragma FUNC_ALWAYS_INLINE
FFTLIB_STATIC_INLINE void
FFTLIB_UTIL_SE1AloadComputeCPermLlhhPermLlhhMvLlhhSA1Store (
    int32_t numRows,
    uint8_t *restrict pDst,
    uchar64 permCtrl1,
    uchar64 permCtrl2)
{
   int32_t r;
   uchar64 valA;
   uchar64 valC1, valC2, valC3, valC4;
   uchar64 valC5, valC6, valC7, valC8;
   int32_t loopCount;
   uchar64 valDst1, valDst2, valDst3, valDst4;
   uchar64 valDst12, valDst34;
   __vpred saVpred;

   loopCount = __shift_right (numRows, 3);
   _nassert (loopCount > 0);
   for (r = 0; r < loopCount; r++) {
      valA = __SE1ADV (uchar64);
      __HWALDA (valA);
      __HWAOPXFER (__MMA_A_LDA);
      valC1 = __as_uchar64 (__HWARCV (0));
      __HWAADV ();
      valA = __SE1ADV (uchar64);
      __HWALDA (valA);
      __HWAOPXFER (__MMA_A_LDA);
      valC2 = __as_uchar64 (__HWARCV (0));
      __HWAADV ();
      valA = __SE1ADV (uchar64);
      __HWALDA (valA);
      __HWAOPXFER (__MMA_A_LDA);
      valC3 = __as_uchar64 (__HWARCV (0));
      __HWAADV ();
      valA = __SE1ADV (uchar64);
      __HWALDA (valA);
      __HWAOPXFER (__MMA_A_LDA);
      valC4 = __as_uchar64 (__HWARCV (0));
      __HWAADV ();
      valA = __SE1ADV (uchar64);
      __HWALDA (valA);
      __HWAOPXFER (__MMA_A_LDA);
      valC5 = __as_uchar64 (__HWARCV (0));
      __HWAADV ();
      valA = __SE1ADV (uchar64);
      __HWALDA (valA);
      __HWAOPXFER (__MMA_A_LDA);
      valC6 = __as_uchar64 (__HWARCV (0));
      __HWAADV ();
      valA = __SE1ADV (uchar64);
      __HWALDA (valA);
      __HWAOPXFER (__MMA_A_LDA);
      valC7 = __as_uchar64 (__HWARCV (0));
      __HWAADV ();
      valA = __SE1ADV (uchar64);
      __HWALDA (valA);
      __HWAOPXFER (__MMA_A_LDA);
      valC8 = __as_uchar64 (__HWARCV (0));
      __HWAADV ();
      valDst1  = __permute_low_low (permCtrl1, valC2, valC1);
      valDst2  = __permute_low_low (permCtrl1, valC4, valC3);
      valDst3  = __permute_low_low (permCtrl1, valC6, valC5);
      valDst4  = __permute_low_low (permCtrl1, valC8, valC7);
      valDst12 = __permute_low_low (permCtrl2, valDst2, valDst1);
      valDst34 = __permute_low_low (permCtrl2, valDst4, valDst3);
      saVpred  = __SA1_VPRED (uchar64);
      __vstore_pred (saVpred, __SA1ADV (uchar64, pDst),
                     __vllmv_vvv (valDst34, valDst12));
      saVpred = __SA1_VPRED (uchar64);
      __vstore_pred (saVpred, __SA1ADV (uchar64, pDst),
                     __vhhmv_vvv (valDst34, valDst12));
      valDst12 = __permute_high_high (permCtrl2, valDst2, valDst1);
      valDst34 = __permute_high_high (permCtrl2, valDst4, valDst3);
      saVpred  = __SA1_VPRED (uchar64);
      __vstore_pred (saVpred, __SA1ADV (uchar64, pDst),
                     __vllmv_vvv (valDst34, valDst12));
      saVpred = __SA1_VPRED (uchar64);
      __vstore_pred (saVpred, __SA1ADV (uchar64, pDst),
                     __vhhmv_vvv (valDst34, valDst12));
      valDst1  = __permute_high_high (permCtrl1, valC2, valC1);
      valDst2  = __permute_high_high (permCtrl1, valC4, valC3);
      valDst3  = __permute_high_high (permCtrl1, valC6, valC5);
      valDst4  = __permute_high_high (permCtrl1, valC8, valC7);
      valDst12 = __permute_low_low (permCtrl2, valDst2, valDst1);
      valDst34 = __permute_low_low (permCtrl2, valDst4, valDst3);
      saVpred  = __SA1_VPRED (uchar64);
      __vstore_pred (saVpred, __SA1ADV (uchar64, pDst),
                     __vllmv_vvv (valDst34, valDst12));
      saVpred = __SA1_VPRED (uchar64);
      __vstore_pred (saVpred, __SA1ADV (uchar64, pDst),
                     __vhhmv_vvv (valDst34, valDst12));
      valDst12 = __permute_high_high (permCtrl2, valDst2, valDst1);
      valDst34 = __permute_high_high (permCtrl2, valDst4, valDst3);
      saVpred  = __SA1_VPRED (uchar64);
      __vstore_pred (saVpred, __SA1ADV (uchar64, pDst),
                     __vllmv_vvv (valDst34, valDst12));
      saVpred = __SA1_VPRED (uchar64);
      __vstore_pred (saVpred, __SA1ADV (uchar64, pDst),
                     __vhhmv_vvv (valDst34, valDst12));
   }

   return;
}
#endif
#endif

#if 0
#ifdef __cplusplus
#pragma FUNC_ALWAYS_INLINE
FFTLIB_STATIC_INLINE void FFTLIB_UTIL_SA0AloadSE0BloadComputeCPermLlhhPermLlhhMvLlhhSA1Store(
   int32_t numRows,
   uint8_t *restrict pSrc,
   uint8_t *restrict pDst,
   uchar64 permCtrl1,
   uchar64 permCtrl2)
{
   int32_t r;
   uchar64 valA;
   uchar64 valB;
   uchar64 valC1, valC2, valC3, valC4;
   uchar64 valC5, valC6, valC7, valC8;
   int32_t loopCount;
   uchar64 valDst1, valDst2, valDst3, valDst4;
   uchar64 valDst12, valDst34;
   __vpred saVpred;

   loopCount = numRows >> 3;
   _nassert(loopCount > 0);
   for (r = 0; r < loopCount; r++) {
      valA = *__SA0ADV(uchar64, pSrc);
      valB = __SE0ADV(uchar64);
      __HWALDAB(valA, valB);
      __HWAOPXFER(__MMA_A_LDA);
      valC1 = __as_uchar64(__HWARCV(0));
      __HWAADV();
      valA = *__SA0ADV(uchar64, pSrc);
      valB = __SE0ADV(uchar64);
      __HWALDAB(valA, valB);
      __HWAOPXFER(__MMA_A_LDA);
      valC2 = __as_uchar64(__HWARCV(0));
      __HWAADV();
      valA = *__SA0ADV(uchar64, pSrc);
      valB = __SE0ADV(uchar64);
      __HWALDAB(valA, valB);
      __HWAOPXFER(__MMA_A_LDA);
      valC3 = __as_uchar64(__HWARCV(0));
      __HWAADV();
      valA = *__SA0ADV(uchar64, pSrc);
      valB = __SE0ADV(uchar64);
      __HWALDAB(valA, valB);
      __HWAOPXFER(__MMA_A_LDA);
      valC4 = __as_uchar64(__HWARCV(0));
      __HWAADV();
      valA = *__SA0ADV(uchar64, pSrc);
      valB = __SE0ADV(uchar64);
      __HWALDAB(valA, valB);
      __HWAOPXFER(__MMA_A_LDA);
      valC5 = __as_uchar64(__HWARCV(0));
      __HWAADV();
      valA = *__SA0ADV(uchar64, pSrc);
      valB = __SE0ADV(uchar64);
      __HWALDAB(valA, valB);
      __HWAOPXFER(__MMA_A_LDA);
      valC6 = __as_uchar64(__HWARCV(0));
      __HWAADV();
      valA = *__SA0ADV(uchar64, pSrc);
      valB = __SE0ADV(uchar64);
      __HWALDAB(valA, valB);
      __HWAOPXFER(__MMA_A_LDA);
      valC7 = __as_uchar64(__HWARCV(0));
      __HWAADV();
      valA = *__SA0ADV(uchar64, pSrc);
      valB = __SE0ADV(uchar64);
      __HWALDAB(valA, valB);
      __HWAOPXFER(__MMA_A_LDA);
      valC8 = __as_uchar64(__HWARCV(0));
      __HWAADV();
      valDst1 = __permute_low_low(permCtrl1, valC2, valC1);
      valDst2 = __permute_low_low(permCtrl1, valC4, valC3);
      valDst3 = __permute_low_low(permCtrl1, valC6, valC5);
      valDst4 = __permute_low_low(permCtrl1, valC8, valC7);
      valDst12 = __permute_low_low(permCtrl2, valDst2, valDst1);
      valDst34 = __permute_low_low(permCtrl2, valDst4, valDst3);
      saVpred = __SA1_VPRED(uchar64);
      __vstore_pred(saVpred, __SA1ADV(uchar64, pDst), __vllmv_vvv(valDst34, valDst12));
      saVpred = __SA1_VPRED(uchar64);
      __vstore_pred(saVpred, __SA1ADV(uchar64, pDst), __vhhmv_vvv(valDst34, valDst12));
      valDst12 = __permute_high_high(permCtrl2, valDst2, valDst1);
      valDst34 = __permute_high_high(permCtrl2, valDst4, valDst3);
      saVpred = __SA1_VPRED(uchar64);
      __vstore_pred(saVpred, __SA1ADV(uchar64, pDst), __vllmv_vvv(valDst34, valDst12));
      saVpred = __SA1_VPRED(uchar64);
      __vstore_pred(saVpred, __SA1ADV(uchar64, pDst), __vhhmv_vvv(valDst34, valDst12));
      valDst1 = __permute_high_high(permCtrl1, valC2, valC1);
      valDst2 = __permute_high_high(permCtrl1, valC4, valC3);
      valDst3 = __permute_high_high(permCtrl1, valC6, valC5);
      valDst4 = __permute_high_high(permCtrl1, valC8, valC7);
      valDst12 = __permute_low_low(permCtrl2, valDst2, valDst1);
      valDst34 = __permute_low_low(permCtrl2, valDst4, valDst3);
      saVpred = __SA1_VPRED(uchar64);
      __vstore_pred(saVpred, __SA1ADV(uchar64, pDst), __vllmv_vvv(valDst34, valDst12));
      saVpred = __SA1_VPRED(uchar64);
      __vstore_pred(saVpred, __SA1ADV(uchar64, pDst), __vhhmv_vvv(valDst34, valDst12));
      valDst12 = __permute_high_high(permCtrl2, valDst2, valDst1);
      valDst34 = __permute_high_high(permCtrl2, valDst4, valDst3);
      saVpred = __SA1_VPRED(uchar64);
      __vstore_pred(saVpred, __SA1ADV(uchar64, pDst), __vllmv_vvv(valDst34, valDst12));
      saVpred = __SA1_VPRED(uchar64);
      __vstore_pred(saVpred, __SA1ADV(uchar64, pDst), __vhhmv_vvv(valDst34, valDst12));
   }

   return;
}
#endif
#else
#ifdef __cplusplus
#pragma FUNC_ALWAYS_INLINE
FFTLIB_STATIC_INLINE void
FFTLIB_UTIL_SE1AloadSE0BloadComputeCPermLlhhPermLlhhMvLlhhSA1Store (
    int32_t numRows,
    uint8_t *restrict pDst,
    uchar64 permCtrl1,
    uchar64 permCtrl2)
{
   int32_t r;
   uchar64 valA;
   uchar64 valB;
   uchar64 valC1, valC2, valC3, valC4;
   uchar64 valC5, valC6, valC7, valC8;
   int32_t loopCount;
   uchar64 valDst1, valDst2, valDst3, valDst4;
   uchar64 valDst12, valDst34;
   __vpred saVpred;

   loopCount = __shift_right (numRows, 3);
   _nassert (loopCount > 0);
   for (r = 0; r < loopCount; r++) {
      valA = __SE1ADV (uchar64);
      valB = __SE0ADV (uchar64);
      __HWALDAB (valA, valB);
      __HWAOPXFER (__MMA_A_LDA);
      valC1 = __as_uchar64 (__HWARCV (0));
      __HWAADV ();
      valA = __SE1ADV (uchar64);
      valB = __SE0ADV (uchar64);
      __HWALDAB (valA, valB);
      __HWAOPXFER (__MMA_A_LDA);
      valC2 = __as_uchar64 (__HWARCV (0));
      __HWAADV ();
      valA = __SE1ADV (uchar64);
      valB = __SE0ADV (uchar64);
      __HWALDAB (valA, valB);
      __HWAOPXFER (__MMA_A_LDA);
      valC3 = __as_uchar64 (__HWARCV (0));
      __HWAADV ();
      valA = __SE1ADV (uchar64);
      valB = __SE0ADV (uchar64);
      __HWALDAB (valA, valB);
      __HWAOPXFER (__MMA_A_LDA);
      valC4 = __as_uchar64 (__HWARCV (0));
      __HWAADV ();
      valA = __SE1ADV (uchar64);
      valB = __SE0ADV (uchar64);
      __HWALDAB (valA, valB);
      __HWAOPXFER (__MMA_A_LDA);
      valC5 = __as_uchar64 (__HWARCV (0));
      __HWAADV ();
      valA = __SE1ADV (uchar64);
      valB = __SE0ADV (uchar64);
      __HWALDAB (valA, valB);
      __HWAOPXFER (__MMA_A_LDA);
      valC6 = __as_uchar64 (__HWARCV (0));
      __HWAADV ();
      valA = __SE1ADV (uchar64);
      valB = __SE0ADV (uchar64);
      __HWALDAB (valA, valB);
      __HWAOPXFER (__MMA_A_LDA);
      valC7 = __as_uchar64 (__HWARCV (0));
      __HWAADV ();
      valA = __SE1ADV (uchar64);
      valB = __SE0ADV (uchar64);
      __HWALDAB (valA, valB);
      __HWAOPXFER (__MMA_A_LDA);
      valC8 = __as_uchar64 (__HWARCV (0));
      __HWAADV ();
      valDst1  = __permute_low_low (permCtrl1, valC2, valC1);
      valDst2  = __permute_low_low (permCtrl1, valC4, valC3);
      valDst3  = __permute_low_low (permCtrl1, valC6, valC5);
      valDst4  = __permute_low_low (permCtrl1, valC8, valC7);
      valDst12 = __permute_low_low (permCtrl2, valDst2, valDst1);
      valDst34 = __permute_low_low (permCtrl2, valDst4, valDst3);
      saVpred  = __SA1_VPRED (uchar64);
      __vstore_pred (saVpred, __SA1ADV (uchar64, pDst),
                     __vllmv_vvv (valDst34, valDst12));
      saVpred = __SA1_VPRED (uchar64);
      __vstore_pred (saVpred, __SA1ADV (uchar64, pDst),
                     __vhhmv_vvv (valDst34, valDst12));
      valDst12 = __permute_high_high (permCtrl2, valDst2, valDst1);
      valDst34 = __permute_high_high (permCtrl2, valDst4, valDst3);
      saVpred  = __SA1_VPRED (uchar64);
      __vstore_pred (saVpred, __SA1ADV (uchar64, pDst),
                     __vllmv_vvv (valDst34, valDst12));
      saVpred = __SA1_VPRED (uchar64);
      __vstore_pred (saVpred, __SA1ADV (uchar64, pDst),
                     __vhhmv_vvv (valDst34, valDst12));
      valDst1  = __permute_high_high (permCtrl1, valC2, valC1);
      valDst2  = __permute_high_high (permCtrl1, valC4, valC3);
      valDst3  = __permute_high_high (permCtrl1, valC6, valC5);
      valDst4  = __permute_high_high (permCtrl1, valC8, valC7);
      valDst12 = __permute_low_low (permCtrl2, valDst2, valDst1);
      valDst34 = __permute_low_low (permCtrl2, valDst4, valDst3);
      saVpred  = __SA1_VPRED (uchar64);
      __vstore_pred (saVpred, __SA1ADV (uchar64, pDst),
                     __vllmv_vvv (valDst34, valDst12));
      saVpred = __SA1_VPRED (uchar64);
      __vstore_pred (saVpred, __SA1ADV (uchar64, pDst),
                     __vhhmv_vvv (valDst34, valDst12));
      valDst12 = __permute_high_high (permCtrl2, valDst2, valDst1);
      valDst34 = __permute_high_high (permCtrl2, valDst4, valDst3);
      saVpred  = __SA1_VPRED (uchar64);
      __vstore_pred (saVpred, __SA1ADV (uchar64, pDst),
                     __vllmv_vvv (valDst34, valDst12));
      saVpred = __SA1_VPRED (uchar64);
      __vstore_pred (saVpred, __SA1ADV (uchar64, pDst),
                     __vhhmv_vvv (valDst34, valDst12));
   }

   return;
}
#endif
#endif

#if 0
#ifdef __cplusplus
#pragma FUNC_ALWAYS_INLINE
FFTLIB_STATIC_INLINE void FFTLIB_UTIL_SA0AloadComputeCPermLlhhPermLlhhMvLlhhSA2Store(
   int32_t numRows,
   uint8_t *restrict pSrc,
   uint8_t *restrict pDst,
   uchar64 permCtrl1,
   uchar64 permCtrl2)
{
   int32_t r;
   uchar64 valA;
   uchar64 valC1, valC2, valC3, valC4;
   uchar64 valC5, valC6, valC7, valC8;
   int32_t loopCount;
   uchar64 valDst1, valDst2, valDst3, valDst4;
   uchar64 valDst12, valDst34;
   __vpred saVpred;

   loopCount = numRows >> 3;
   _nassert(loopCount > 0);
   for (r = 0; r < loopCount; r++) {
      valA = *__SA0ADV(uchar64, pSrc);
      __HWALDA(valA);
      __HWAOPXFER(__MMA_A_LDA);
      valC1 = __as_uchar64(__HWARCV(0));
      __HWAADV();
      valA = *__SA0ADV(uchar64, pSrc);
      __HWALDA(valA);
      __HWAOPXFER(__MMA_A_LDA);
      valC2 = __as_uchar64(__HWARCV(0));
      __HWAADV();
      valA = *__SA0ADV(uchar64, pSrc);
      __HWALDA(valA);
      __HWAOPXFER(__MMA_A_LDA);
      valC3 = __as_uchar64(__HWARCV(0));
      __HWAADV();
      valA = *__SA0ADV(uchar64, pSrc);
      __HWALDA(valA);
      __HWAOPXFER(__MMA_A_LDA);
      valC4 = __as_uchar64(__HWARCV(0));
      __HWAADV();
      valA = *__SA0ADV(uchar64, pSrc);
      __HWALDA(valA);
      __HWAOPXFER(__MMA_A_LDA);
      valC5 = __as_uchar64(__HWARCV(0));
      __HWAADV();
      valA = *__SA0ADV(uchar64, pSrc);
      __HWALDA(valA);
      __HWAOPXFER(__MMA_A_LDA);
      valC6 = __as_uchar64(__HWARCV(0));
      __HWAADV();
      valA = *__SA0ADV(uchar64, pSrc);
      __HWALDA(valA);
      __HWAOPXFER(__MMA_A_LDA);
      valC7 = __as_uchar64(__HWARCV(0));
      __HWAADV();
      valA = *__SA0ADV(uchar64, pSrc);
      __HWALDA(valA);
      __HWAOPXFER(__MMA_A_LDA);
      valC8 = __as_uchar64(__HWARCV(0));
      __HWAADV();
      valDst1 = __permute_low_low(permCtrl1, valC2, valC1);
      valDst2 = __permute_low_low(permCtrl1, valC4, valC3);
      valDst3 = __permute_low_low(permCtrl1, valC6, valC5);
      valDst4 = __permute_low_low(permCtrl1, valC8, valC7);
      valDst12 = __permute_low_low(permCtrl2, valDst2, valDst1);
      valDst34 = __permute_low_low(permCtrl2, valDst4, valDst3);
      saVpred = __SA2_VPRED(uchar64);
      __vstore_pred(saVpred, __SA2ADV(uchar64, pDst), __vllmv_vvv(valDst34, valDst12));
      saVpred = __SA2_VPRED(uchar64);
      __vstore_pred(saVpred, __SA2ADV(uchar64, pDst), __vhhmv_vvv(valDst34, valDst12));
      valDst12 = __permute_high_high(permCtrl2, valDst2, valDst1);
      valDst34 = __permute_high_high(permCtrl2, valDst4, valDst3);
      saVpred = __SA2_VPRED(uchar64);
      __vstore_pred(saVpred, __SA2ADV(uchar64, pDst), __vllmv_vvv(valDst34, valDst12));
      saVpred = __SA2_VPRED(uchar64);
      __vstore_pred(saVpred, __SA2ADV(uchar64, pDst), __vhhmv_vvv(valDst34, valDst12));
      valDst1 = __permute_high_high(permCtrl1, valC2, valC1);
      valDst2 = __permute_high_high(permCtrl1, valC4, valC3);
      valDst3 = __permute_high_high(permCtrl1, valC6, valC5);
      valDst4 = __permute_high_high(permCtrl1, valC8, valC7);
      valDst12 = __permute_low_low(permCtrl2, valDst2, valDst1);
      valDst34 = __permute_low_low(permCtrl2, valDst4, valDst3);
      saVpred = __SA2_VPRED(uchar64);
      __vstore_pred(saVpred, __SA2ADV(uchar64, pDst), __vllmv_vvv(valDst34, valDst12));
      saVpred = __SA2_VPRED(uchar64);
      __vstore_pred(saVpred, __SA2ADV(uchar64, pDst), __vhhmv_vvv(valDst34, valDst12));
      valDst12 = __permute_high_high(permCtrl2, valDst2, valDst1);
      valDst34 = __permute_high_high(permCtrl2, valDst4, valDst3);
      saVpred = __SA2_VPRED(uchar64);
      __vstore_pred(saVpred, __SA2ADV(uchar64, pDst), __vllmv_vvv(valDst34, valDst12));
      saVpred = __SA2_VPRED(uchar64);
      __vstore_pred(saVpred, __SA2ADV(uchar64, pDst), __vhhmv_vvv(valDst34, valDst12));
   }

   return;
}
#endif
#else
#ifdef __cplusplus
#pragma FUNC_ALWAYS_INLINE
FFTLIB_STATIC_INLINE void
FFTLIB_UTIL_SE1AloadComputeCPermLlhhPermLlhhMvLlhhSA2Store (
    int32_t numRows,
    uint8_t *restrict pDst,
    uchar64 permCtrl1,
    uchar64 permCtrl2)
{
   int32_t r;
   uchar64 valA;
   uchar64 valC1, valC2, valC3, valC4;
   uchar64 valC5, valC6, valC7, valC8;
   int32_t loopCount;
   uchar64 valDst1, valDst2, valDst3, valDst4;
   uchar64 valDst12, valDst34;
   __vpred saVpred;

   loopCount = __shift_right (numRows, 3);
   _nassert (loopCount > 0);
   for (r = 0; r < loopCount; r++) {
      valA = __SE1ADV (uchar64);
      __HWALDA (valA);
      __HWAOPXFER (__MMA_A_LDA);
      valC1 = __as_uchar64 (__HWARCV (0));
      __HWAADV ();
      valA = __SE1ADV (uchar64);
      __HWALDA (valA);
      __HWAOPXFER (__MMA_A_LDA);
      valC2 = __as_uchar64 (__HWARCV (0));
      __HWAADV ();
      valA = __SE1ADV (uchar64);
      __HWALDA (valA);
      __HWAOPXFER (__MMA_A_LDA);
      valC3 = __as_uchar64 (__HWARCV (0));
      __HWAADV ();
      valA = __SE1ADV (uchar64);
      __HWALDA (valA);
      __HWAOPXFER (__MMA_A_LDA);
      valC4 = __as_uchar64 (__HWARCV (0));
      __HWAADV ();
      valA = __SE1ADV (uchar64);
      __HWALDA (valA);
      __HWAOPXFER (__MMA_A_LDA);
      valC5 = __as_uchar64 (__HWARCV (0));
      __HWAADV ();
      valA = __SE1ADV (uchar64);
      __HWALDA (valA);
      __HWAOPXFER (__MMA_A_LDA);
      valC6 = __as_uchar64 (__HWARCV (0));
      __HWAADV ();
      valA = __SE1ADV (uchar64);
      __HWALDA (valA);
      __HWAOPXFER (__MMA_A_LDA);
      valC7 = __as_uchar64 (__HWARCV (0));
      __HWAADV ();
      valA = __SE1ADV (uchar64);
      __HWALDA (valA);
      __HWAOPXFER (__MMA_A_LDA);
      valC8 = __as_uchar64 (__HWARCV (0));
      __HWAADV ();
      valDst1  = __permute_low_low (permCtrl1, valC2, valC1);
      valDst2  = __permute_low_low (permCtrl1, valC4, valC3);
      valDst3  = __permute_low_low (permCtrl1, valC6, valC5);
      valDst4  = __permute_low_low (permCtrl1, valC8, valC7);
      valDst12 = __permute_low_low (permCtrl2, valDst2, valDst1);
      valDst34 = __permute_low_low (permCtrl2, valDst4, valDst3);
      saVpred  = __SA2_VPRED (uchar64);
      __vstore_pred (saVpred, __SA2ADV (uchar64, pDst),
                     __vllmv_vvv (valDst34, valDst12));
      saVpred = __SA2_VPRED (uchar64);
      __vstore_pred (saVpred, __SA2ADV (uchar64, pDst),
                     __vhhmv_vvv (valDst34, valDst12));
      valDst12 = __permute_high_high (permCtrl2, valDst2, valDst1);
      valDst34 = __permute_high_high (permCtrl2, valDst4, valDst3);
      saVpred  = __SA2_VPRED (uchar64);
      __vstore_pred (saVpred, __SA2ADV (uchar64, pDst),
                     __vllmv_vvv (valDst34, valDst12));
      saVpred = __SA2_VPRED (uchar64);
      __vstore_pred (saVpred, __SA2ADV (uchar64, pDst),
                     __vhhmv_vvv (valDst34, valDst12));
      valDst1  = __permute_high_high (permCtrl1, valC2, valC1);
      valDst2  = __permute_high_high (permCtrl1, valC4, valC3);
      valDst3  = __permute_high_high (permCtrl1, valC6, valC5);
      valDst4  = __permute_high_high (permCtrl1, valC8, valC7);
      valDst12 = __permute_low_low (permCtrl2, valDst2, valDst1);
      valDst34 = __permute_low_low (permCtrl2, valDst4, valDst3);
      saVpred  = __SA2_VPRED (uchar64);
      __vstore_pred (saVpred, __SA2ADV (uchar64, pDst),
                     __vllmv_vvv (valDst34, valDst12));
      saVpred = __SA2_VPRED (uchar64);
      __vstore_pred (saVpred, __SA2ADV (uchar64, pDst),
                     __vhhmv_vvv (valDst34, valDst12));
      valDst12 = __permute_high_high (permCtrl2, valDst2, valDst1);
      valDst34 = __permute_high_high (permCtrl2, valDst4, valDst3);
      saVpred  = __SA2_VPRED (uchar64);
      __vstore_pred (saVpred, __SA2ADV (uchar64, pDst),
                     __vllmv_vvv (valDst34, valDst12));
      saVpred = __SA2_VPRED (uchar64);
      __vstore_pred (saVpred, __SA2ADV (uchar64, pDst),
                     __vhhmv_vvv (valDst34, valDst12));
   }

   return;
}
#endif
#endif

#if 0
#ifdef __cplusplus
#pragma FUNC_ALWAYS_INLINE
FFTLIB_STATIC_INLINE void FFTLIB_UTIL_SA0AloadComputeCPermLlMvLlMvHhSA2Store(
   int32_t numRows,
   uint8_t *restrict pSrc,
   uint8_t *restrict pDst,
   uchar64 permCtrl)
{
   int32_t r;
   uchar64 valA, valC1, valC2, valC3, valC4;
   int32_t loopCount;
   uchar64 valDst1, valDst2, valDst3, valDst4;
   __vpred saVpred;

   loopCount = numRows >> 2;
   _nassert(loopCount > 0);
   for (r = 0; r < loopCount; r++) {
      valA = *__SA0ADV(uchar64, pSrc);
      __HWALDA(valA);
      __HWAOPXFER(__MMA_A_LDA);
      valC1 = __as_uchar64(__HWARCV(0));
      __HWAADV();
      valA = *__SA0ADV(uchar64, pSrc);
      __HWALDA(valA);
      __HWAOPXFER(__MMA_A_LDA);
      valC2 = __as_uchar64(__HWARCV(0));
      __HWAADV();
      valA = *__SA0ADV(uchar64, pSrc);
      __HWALDA(valA);
      __HWAOPXFER(__MMA_A_LDA);
      valC3 = __as_uchar64(__HWARCV(0));
      __HWAADV();
      valA = *__SA0ADV(uchar64, pSrc);
      __HWALDA(valA);
      __HWAOPXFER(__MMA_A_LDA);
      valC4 = __as_uchar64(__HWARCV(0));
      __HWAADV();
      valDst1 = __permute_low_low(permCtrl, valC2, valC1);
      valDst2 = __permute_low_low(permCtrl, valC4, valC3);
      saVpred = __SA2_VPRED(uchar64);
      __vstore_pred(saVpred, __SA2ADV(uchar64, pDst), __vllmv_vvv(valDst2, valDst1));
      saVpred = __SA2_VPRED(uchar64);
      __vstore_pred(saVpred, __SA2ADV(uchar64, pDst), __vhhmv_vvv(valDst2, valDst1));
      valDst3 = __permute_high_high(permCtrl, valC2, valC1);
      valDst4 = __permute_high_high(permCtrl, valC4, valC3);
      saVpred = __SA2_VPRED(uchar64);
      __vstore_pred(saVpred, __SA2ADV(uchar64, pDst), __vllmv_vvv(valDst4, valDst3));
      saVpred = __SA2_VPRED(uchar64);
      __vstore_pred(saVpred, __SA2ADV(uchar64, pDst), __vhhmv_vvv(valDst4, valDst3));
   }

   return;
}
#endif
#else
#ifdef __cplusplus
#pragma FUNC_ALWAYS_INLINE
FFTLIB_STATIC_INLINE void
FFTLIB_UTIL_SE1AloadComputeCPermLlMvLlMvHhSA2Store (int32_t numRows,
                                                    uint8_t *restrict pDst,
                                                    uchar64 permCtrl)
{
   int32_t r;
   uchar64 valA, valC1, valC2, valC3, valC4;
   int32_t loopCount;
   uchar64 valDst1, valDst2, valDst3, valDst4;
   __vpred saVpred;

   loopCount = __shift_right (numRows, 2);
   _nassert (loopCount > 0);
   for (r = 0; r < loopCount; r++) {
      valA = __SE1ADV (uchar64);
      __HWALDA (valA);
      __HWAOPXFER (__MMA_A_LDA);
      valC1 = __as_uchar64 (__HWARCV (0));
      __HWAADV ();
      valA = __SE1ADV (uchar64);
      __HWALDA (valA);
      __HWAOPXFER (__MMA_A_LDA);
      valC2 = __as_uchar64 (__HWARCV (0));
      __HWAADV ();
      valA = __SE1ADV (uchar64);
      __HWALDA (valA);
      __HWAOPXFER (__MMA_A_LDA);
      valC3 = __as_uchar64 (__HWARCV (0));
      __HWAADV ();
      valA = __SE1ADV (uchar64);
      __HWALDA (valA);
      __HWAOPXFER (__MMA_A_LDA);
      valC4 = __as_uchar64 (__HWARCV (0));
      __HWAADV ();
      valDst1 = __permute_low_low (permCtrl, valC2, valC1);
      valDst2 = __permute_low_low (permCtrl, valC4, valC3);
      saVpred = __SA2_VPRED (uchar64);
      __vstore_pred (saVpred, __SA2ADV (uchar64, pDst),
                     __vllmv_vvv (valDst2, valDst1));
      saVpred = __SA2_VPRED (uchar64);
      __vstore_pred (saVpred, __SA2ADV (uchar64, pDst),
                     __vhhmv_vvv (valDst2, valDst1));
      valDst3 = __permute_high_high (permCtrl, valC2, valC1);
      valDst4 = __permute_high_high (permCtrl, valC4, valC3);
      saVpred = __SA2_VPRED (uchar64);
      __vstore_pred (saVpred, __SA2ADV (uchar64, pDst),
                     __vllmv_vvv (valDst4, valDst3));
      saVpred = __SA2_VPRED (uchar64);
      __vstore_pred (saVpred, __SA2ADV (uchar64, pDst),
                     __vhhmv_vvv (valDst4, valDst3));
   }

   return;
}
#endif
#endif

#if 0
#ifdef __cplusplus
#pragma FUNC_ALWAYS_INLINE
FFTLIB_STATIC_INLINE void FFTLIB_UTIL_SA0AloadComputeCPermLlHhSA2Store(
   int32_t numRows,
   uint8_t *restrict pSrc,
   uint8_t *restrict pDst,
   uchar64 permCtrl)
{
   int32_t r;
   uchar64 valA, valC1, valC2;
   int32_t loopCount;
   __vpred saVpred;

   loopCount = numRows >> 1;
   _nassert(loopCount > 0);
   for (r = 0; r < loopCount; r++) {
      valA = *__SA0ADV(uchar64, pSrc);
      __HWALDA(valA);
      __HWAOPXFER(__MMA_A_LDA);
      valC1 = __as_uchar64(__HWARCV(0));
      __HWAADV();
      valA = *__SA0ADV(uchar64, pSrc);
      __HWALDA(valA);
      __HWAOPXFER(__MMA_A_LDA);
      valC2 = __as_uchar64(__HWARCV(0));
      __HWAADV();
      saVpred = __SA2_VPRED(uchar64);
      __vstore_pred(saVpred, __SA2ADV(uchar64, pDst), __permute_low_low(permCtrl, valC2, valC1));
      saVpred = __SA2_VPRED(uchar64);
      __vstore_pred(saVpred, __SA2ADV(uchar64, pDst), __permute_high_high(permCtrl, valC2, valC1));
   }

   return;
}
#endif
#else
#ifdef __cplusplus
#pragma FUNC_ALWAYS_INLINE
FFTLIB_STATIC_INLINE void
FFTLIB_UTIL_SE1AloadComputeCPermLlHhSA2Store (int32_t numRows,
                                              uint8_t *restrict pDst,
                                              uchar64 permCtrl)
{
   int32_t r;
   uchar64 valA, valC1, valC2;
   int32_t loopCount;
   __vpred saVpred;

   loopCount = __shift_right (numRows, 1);
   _nassert (loopCount > 0);
   for (r = 0; r < loopCount; r++) {
      valA = __SE1ADV (uchar64);
      __HWALDA (valA);
      __HWAOPXFER (__MMA_A_LDA);
      valC1 = __as_uchar64 (__HWARCV (0));
      __HWAADV ();
      valA = __SE1ADV (uchar64);
      __HWALDA (valA);
      __HWAOPXFER (__MMA_A_LDA);
      valC2 = __as_uchar64 (__HWARCV (0));
      __HWAADV ();
      saVpred = __SA2_VPRED (uchar64);
      __vstore_pred (saVpred, __SA2ADV (uchar64, pDst),
                     __permute_low_low (permCtrl, valC2, valC1));
      saVpred = __SA2_VPRED (uchar64);
      __vstore_pred (saVpred, __SA2ADV (uchar64, pDst),
                     __permute_high_high (permCtrl, valC2, valC1));
   }

   return;
}
#endif
#endif

#if 1
#ifdef __cplusplus
#pragma FUNC_ALWAYS_INLINE
FFTLIB_STATIC_INLINE void FFTLIB_UTIL_SE1AloadSE0BloadComputeC (int32_t numRows)
{
   int32_t r;
   uchar64 valA;
   uchar64 valB;

   _nassert (numRows > 0);
   for (r = 0; r < numRows; r++) {
      valA = __SE1ADV (uchar64);
      valB = __SE0ADV (uchar64);
      __HWALDAB (__as_uchar64 (valA), __as_uchar64 (valB));
      __HWAOP (__MMA_A_LDA);
      __HWAADV ();
   }

   return;
}
#endif
#endif

template <uint32_t dataType>
FFTLIB_STATUS FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_getSizes (
    const FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_InitArgs *pKerInitArgs,
    FFTLIB_bufParams1D_t                                          *bufParamsX,
    FFTLIB_bufParams1D_t                                          *bufParamsW,
    FFTLIB_bufParams1D_t                                          *bufParamsY)
{
   uint32_t      fftSize   = pKerInitArgs->fftSize;
   uint32_t      batchSize = pKerInitArgs->batchSize;
   uint32_t      MMA_SIZE;
   int32_t       wSize;
   uint32_t      dftSizeCurrStage;
   uint32_t      fftSizeModuloPower, decompositionFactorStage0;
   FFTLIB_STATUS status = FFTLIB_SUCCESS;

   if (dataType == FFTLIB_INT32) {
      MMA_SIZE = FFTLIB_MMA_SIZE_32_BIT;
   }
   else if (dataType == FFTLIB_INT16) {
      MMA_SIZE = FFTLIB_MMA_SIZE_16_BIT;
   }
   else {
      status = FFTLIB_ERR_NOT_IMPLEMENTED;
   }

   if (status == FFTLIB_SUCCESS) {
      wSize              = 0;
      fftSizeModuloPower = fftSize;
      while (fftSizeModuloPower > MMA_SIZE / 2) {
         fftSizeModuloPower /= MMA_SIZE / 2;
      }
      decompositionFactorStage0 = fftSizeModuloPower;

      dftSizeCurrStage = fftSize;
      wSize += dftSizeCurrStage * 2;
      dftSizeCurrStage /= decompositionFactorStage0;
      while (dftSizeCurrStage > MMA_SIZE / 2) {
         wSize += dftSizeCurrStage * 2;
         dftSizeCurrStage /= MMA_SIZE / 2;
      }
#if 1
      wSize += MMA_SIZE * MMA_SIZE;
#endif

      bufParamsX->dim_x = fftSize * batchSize * 2;
      bufParamsY->dim_x = fftSize * batchSize * 2;
      bufParamsW->dim_x = wSize;
   }

   return status;
}

template FFTLIB_STATUS
FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_getSizes<FFTLIB_INT16> (
    const FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_InitArgs *pKerInitArgs,
    FFTLIB_bufParams1D_t                                          *bufParamsX,
    FFTLIB_bufParams1D_t                                          *bufParamsW,
    FFTLIB_bufParams1D_t                                          *bufParamsY);
template FFTLIB_STATUS
FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_getSizes<FFTLIB_INT32> (
    const FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_InitArgs *pKerInitArgs,
    FFTLIB_bufParams1D_t                                          *bufParamsX,
    FFTLIB_bufParams1D_t                                          *bufParamsW,
    FFTLIB_bufParams1D_t                                          *bufParamsY);

template <typename dataType>
FFTLIB_STATUS FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_twGen (
    const FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_InitArgs *pKerInitArgs,
    void *restrict pW,
    const FFTLIB_bufParams1D_t *bufParamsW)
{
   FFTLIB_STATUS        status = FFTLIB_SUCCESS;
   uint32_t             j, k, l;
   const FFTLIB_D64     PI = 3.141592654;
   FFTLIB_D64           twF2sScale;
   FFTLIB_bufParams1D_t bufParamsXLocal, bufParamsYLocal, bufParamsWLocal;
   uint32_t             fftSize = pKerInitArgs->fftSize;
   uint32_t             MMA_SIZE;
   dataType            *pW1LogNStages, *pW2LogNStages, *pWDftStage;
   uint32_t             dftSizeCurrStage;
   uint32_t             fftSizeModuloPower, decompositionFactorStage0;

   if (sizeof (dataType) == 4) {
      MMA_SIZE = FFTLIB_MMA_SIZE_32_BIT;
      status   = FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_getSizes<
          FFTLIB_INT32> (pKerInitArgs, &bufParamsXLocal, &bufParamsWLocal,
                         &bufParamsYLocal);
   }
   else if (sizeof (dataType) == 2) {
      MMA_SIZE = FFTLIB_MMA_SIZE_16_BIT;
      status   = FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_getSizes<
          FFTLIB_INT16> (pKerInitArgs, &bufParamsXLocal, &bufParamsWLocal,
                         &bufParamsYLocal);
   }
   else {
      status = FFTLIB_ERR_NOT_IMPLEMENTED;
   }

   if (bufParamsW->dim_x != bufParamsWLocal.dim_x) {
      status = FFTLIB_ERR_INVALID_DIMENSION;
   }

   if (status == FFTLIB_SUCCESS) {
      twF2sScale =
          ((uint32_t) 1 << ((uint32_t) (sizeof (dataType) * 8 - 2))) - 1;
      twF2sScale += ((uint32_t) 1 << ((uint32_t) (sizeof (dataType) * 8 - 2)));
      twF2sScale += 0.5;

      fftSizeModuloPower = fftSize;
      while (fftSizeModuloPower > MMA_SIZE / 2) {
         fftSizeModuloPower /= MMA_SIZE / 2;
      }
      decompositionFactorStage0 = fftSizeModuloPower;

      pW1LogNStages = (dataType *) &FFTLIB_FFT_twData[0];

      for (k = 0; k < MMA_SIZE / 2; k++) {
         for (l = 0; l < MMA_SIZE; l++) {
            pW1LogNStages[k * MMA_SIZE + l] = 0;
         }
      }
      for (j = 0; j < MMA_SIZE / (2 * decompositionFactorStage0); j++) {
         for (k = 0; k < decompositionFactorStage0; k++) {
            for (l = 0; l < decompositionFactorStage0; l++) {
               pW1LogNStages[j * decompositionFactorStage0 * MMA_SIZE +
                             j * decompositionFactorStage0 * 2 + k * MMA_SIZE +
                             l * 2] =
                   FFTLIB_UTIL_cos_i64f_oxX<dataType> (
                       (2 * PI * k * l) / decompositionFactorStage0,
                       twF2sScale);
               pW1LogNStages[j * decompositionFactorStage0 * MMA_SIZE +
                             j * decompositionFactorStage0 * 2 + k * MMA_SIZE +
                             l * 2 + 1] =
                   -FFTLIB_UTIL_sin_i64f_oxX<dataType> (
                       (2 * PI * k * l) / decompositionFactorStage0,
                       twF2sScale);
            }
         }
      }

      pW1LogNStages += MMA_SIZE * MMA_SIZE / 2;
      for (k = 0; k < MMA_SIZE / 2; k++) {
         for (l = 0; l < MMA_SIZE / 2; l++) {
            pW1LogNStages[k * MMA_SIZE + l * 2] =
                FFTLIB_UTIL_cos_i64f_oxX<dataType> (
                    (2 * PI * k * l * 2) / MMA_SIZE, twF2sScale);
            pW1LogNStages[k * MMA_SIZE + l * 2 + 1] =
                -FFTLIB_UTIL_sin_i64f_oxX<dataType> (
                    (2 * PI * k * l * 2) / MMA_SIZE, twF2sScale);
         }
      }

      pW2LogNStages    = (dataType *) pW;
      dftSizeCurrStage = fftSize;
      for (j = 0;
           j < (dftSizeCurrStage * 2) / (MMA_SIZE * decompositionFactorStage0);
           j++) {
         for (l = 0; l < decompositionFactorStage0; l++) {
            for (k = 0; k < MMA_SIZE / 2; k++) {
               pW2LogNStages[j * MMA_SIZE * decompositionFactorStage0 +
                             l * MMA_SIZE + k * 2 + 1] =
                   FFTLIB_UTIL_cos_i64f_oxX<dataType> (
                       (2 * PI * (j * MMA_SIZE / 2 + k) * l) / dftSizeCurrStage,
                       twF2sScale);
               pW2LogNStages[j * MMA_SIZE * decompositionFactorStage0 +
                             l * MMA_SIZE + k * 2] =
                   -FFTLIB_UTIL_sin_i64f_oxX<dataType> (
                       (2 * PI * (j * MMA_SIZE / 2 + k) * l) / dftSizeCurrStage,
                       twF2sScale);
            }
         }
      }
      pW2LogNStages += dftSizeCurrStage * 2;
      dftSizeCurrStage /= decompositionFactorStage0;

      while (dftSizeCurrStage > MMA_SIZE / 2) {
         for (j = 0; j < (dftSizeCurrStage * 4) / (MMA_SIZE * MMA_SIZE); j++) {
            for (l = 0; l < MMA_SIZE / 2; l++) {
               for (k = 0; k < MMA_SIZE / 2; k++) {
                  pW2LogNStages[j * MMA_SIZE * MMA_SIZE / 2 + l * MMA_SIZE +
                                k * 2 + 1] =
                      FFTLIB_UTIL_cos_i64f_oxX<dataType> (
                          (2 * PI * (j * MMA_SIZE / 2 + k) * l) /
                              dftSizeCurrStage,
                          twF2sScale);
                  pW2LogNStages[j * MMA_SIZE * MMA_SIZE / 2 + l * MMA_SIZE +
                                k * 2] =
                      -FFTLIB_UTIL_sin_i64f_oxX<dataType> (
                          (2 * PI * (j * MMA_SIZE / 2 + k) * l) /
                              dftSizeCurrStage,
                          twF2sScale);
               }
            }
         }
         pW2LogNStages += dftSizeCurrStage * 2;
         dftSizeCurrStage /= MMA_SIZE / 2;
      }

#if 0
      pWDftStage = pW1LogNStages + MMA_SIZE*MMA_SIZE/2;
#else
      pWDftStage  = pW2LogNStages;
#endif
      for (k = 0; k < MMA_SIZE / 2; k++) {
         for (l = 0; l < MMA_SIZE / 2; l++) {
            pWDftStage[k * 2 * MMA_SIZE + l * 2] =
                FFTLIB_UTIL_sin_i64f_oxX<dataType> (
                    (2 * PI * k * l * 2) / MMA_SIZE, twF2sScale);
            pWDftStage[k * 2 * MMA_SIZE + l * 2 + 1] =
                FFTLIB_UTIL_cos_i64f_oxX<dataType> (
                    (2 * PI * k * l * 2) / MMA_SIZE, twF2sScale);
            pWDftStage[(k * 2 + 1) * MMA_SIZE + l * 2] =
                FFTLIB_UTIL_cos_i64f_oxX<dataType> (
                    (2 * PI * k * l * 2) / MMA_SIZE, twF2sScale);
            pWDftStage[(k * 2 + 1) * MMA_SIZE + l * 2 + 1] =
                -FFTLIB_UTIL_sin_i64f_oxX<dataType> (
                    (2 * PI * k * l * 2) / MMA_SIZE, twF2sScale);
         }
      }
   }

   return status;
}

template FFTLIB_STATUS
FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_twGen<int16_t> (
    const FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_InitArgs *pKerInitArgs,
    void *restrict pW,
    const FFTLIB_bufParams1D_t *bufParamsW);

template FFTLIB_STATUS
FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_twGen<int32_t> (
    const FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_InitArgs *pKerInitArgs,
    void *restrict pW,
    const FFTLIB_bufParams1D_t *bufParamsW);

template <uint32_t dataType>
FFTLIB_STATUS FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_init_ci (
    FFTLIB_kernelHandle                                            handle,
    const FFTLIB_bufParams1D_t                                    *bufParamsX,
    const FFTLIB_bufParams1D_t                                    *bufParamsW,
    const FFTLIB_bufParams1D_t                                    *bufParamsY,
    const FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_InitArgs *pKerInitArgs)
{
   FFTLIB_STATUS         status = FFTLIB_SUCCESS;
   __SE_TEMPLATE_v1      seParams;
   __SA_TEMPLATE_v1      saParams;
   FFTLIB_MMA_CONFIG_REG mmaConfig;
   __HWA_OFFSET_REG      mmaOffset;
   FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_PrivArgs *pKerPrivArgs =
       (FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_PrivArgs *) handle;
   uint8_t              *pBlock    = pKerPrivArgs->bufPblock;
   uint32_t              fftSize   = pKerInitArgs->fftSize;
   uint32_t              batchSize = pKerInitArgs->batchSize;
   uint32_t              shift0    = pKerPrivArgs->initArgs.shiftVector[0];
   uint32_t              shift1    = pKerPrivArgs->initArgs.shiftVector[1];
   uint32_t              shift2    = pKerPrivArgs->initArgs.shiftVector[2];
   uint32_t              shift3    = pKerPrivArgs->initArgs.shiftVector[3];
   uint32_t              dftShift  = pKerPrivArgs->initArgs.shiftVector[4];
   uint32_t              MMA_SIZE;
   __SE_ELETYPE          SE_ELETYPE, SE_ELETYPE_CMPLX;
   __SE_VECLEN           SE_VECLEN, SE_VECLEN_CMPLX;
   FFTLIB_MMA_CONFIG_REG mmaConfigDefault;
   uint32_t              dftSizeInStage;
   uint32_t              fftSizeModuloPower, decompositionFactorStage0;
   int32_t               logNStageCnt;
   uchar64               permCtrlInt, permCtrlDeint, permCtrl64Pt;

   if (dataType == FFTLIB_INT16) {
      MMA_SIZE         = FFTLIB_MMA_SIZE_16_BIT;
      SE_ELETYPE       = __SE_ELETYPE_16BIT;
      SE_ELETYPE_CMPLX = __SE_ELETYPE_16BIT_CMPLX_SWAP;
      SE_VECLEN        = __SE_VECLEN_32ELEMS;
      SE_VECLEN_CMPLX  = __SE_VECLEN_16ELEMS;
      mmaConfigDefault = configRegisterStruct_i16s_i16s_o16s;
#ifndef _HOST_BUILD
      permCtrlInt =
          (uchar64) (0x00, 0x01, 0x20, 0x21, 0x02, 0x03, 0x22, 0x23, 0x04, 0x05,
                     0x24, 0x25, 0x06, 0x07, 0x26, 0x27, 0x08, 0x09, 0x28, 0x29,
                     0x0A, 0x0B, 0x2A, 0x2B, 0x0C, 0x0D, 0x2C, 0x2D, 0x0E, 0x0F,
                     0x2E, 0x2F, 0x10, 0x11, 0x30, 0x31, 0x12, 0x13, 0x32, 0x33,
                     0x14, 0x15, 0x34, 0x35, 0x16, 0x17, 0x36, 0x37, 0x18, 0x19,
                     0x38, 0x39, 0x1A, 0x1B, 0x3A, 0x3B, 0x1C, 0x1D, 0x3C, 0x3D,
                     0x1E, 0x1F, 0x3E, 0x3F);
      permCtrlDeint =
          (uchar64) (0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
                     0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13,
                     0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D,
                     0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
                     0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31,
                     0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B,
                     0x3C, 0x3D, 0x3E, 0x3F);
      permCtrl64Pt =
          (uchar64) (0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
                     0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x20, 0x21, 0x22, 0x23,
                     0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D,
                     0x2E, 0x2F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                     0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x30, 0x31,
                     0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B,
                     0x3C, 0x3D, 0x3E, 0x3F);
#else
      permCtrlInt = uchar64 (
          0x00, 0x01, 0x20, 0x21, 0x02, 0x03, 0x22, 0x23, 0x04, 0x05, 0x24,
          0x25, 0x06, 0x07, 0x26, 0x27, 0x08, 0x09, 0x28, 0x29, 0x0A, 0x0B,
          0x2A, 0x2B, 0x0C, 0x0D, 0x2C, 0x2D, 0x0E, 0x0F, 0x2E, 0x2F, 0x10,
          0x11, 0x30, 0x31, 0x12, 0x13, 0x32, 0x33, 0x14, 0x15, 0x34, 0x35,
          0x16, 0x17, 0x36, 0x37, 0x18, 0x19, 0x38, 0x39, 0x1A, 0x1B, 0x3A,
          0x3B, 0x1C, 0x1D, 0x3C, 0x3D, 0x1E, 0x1F, 0x3E, 0x3F);
      permCtrlDeint = uchar64 (
          0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A,
          0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
          0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20,
          0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B,
          0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36,
          0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F);
      permCtrl64Pt = uchar64 (
          0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A,
          0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25,
          0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x10,
          0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B,
          0x1C, 0x1D, 0x1E, 0x1F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36,
          0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F);
#endif
      //   *stov_ptr(uchar64, (uint8_t *)pBlock + PERMCTRLINT_OFFSET) =
      //   permCtrlInt; *stov_ptr(uchar64, (uint8_t *)pBlock +
      //   PERMCTRLDEINT_OFFSET) = permCtrlDeint; *stov_ptr(uchar64, (uint8_t
      //   *)pBlock + PERMCTRL64PT_OFFSET) = permCtrl64Pt;
      *((uchar64*) ((uint8_t *) pBlock + PERMCTRLINT_OFFSET)) = permCtrlInt;
      *((uchar64*) ((uint8_t *) pBlock + PERMCTRLDEINT_OFFSET)) =
          permCtrlDeint;
      *((uchar64*) ((uint8_t *) pBlock + PERMCTRL64PT_OFFSET)) =
          permCtrl64Pt;
   }
   else if (dataType == FFTLIB_INT32) {
      MMA_SIZE         = FFTLIB_MMA_SIZE_32_BIT;
      SE_ELETYPE       = __SE_ELETYPE_32BIT;
      SE_ELETYPE_CMPLX = __SE_ELETYPE_32BIT_CMPLX_SWAP;
      SE_VECLEN        = __SE_VECLEN_16ELEMS;
      SE_VECLEN_CMPLX  = __SE_VECLEN_8ELEMS;
      mmaConfigDefault = configRegisterStruct_i32s_i32s_o32s;
#ifndef _HOST_BUILD
      permCtrlInt =
          (uchar64) (0x00, 0x01, 0x02, 0x03, 0x20, 0x21, 0x22, 0x23, 0x04, 0x05,
                     0x06, 0x07, 0x24, 0x25, 0x26, 0x27, 0x08, 0x09, 0x0A, 0x0B,
                     0x28, 0x29, 0x2A, 0x2B, 0x0C, 0x0D, 0x0E, 0x0F, 0x2C, 0x2D,
                     0x2E, 0x2F, 0x10, 0x11, 0x12, 0x13, 0x30, 0x31, 0x32, 0x33,
                     0x14, 0x15, 0x16, 0x17, 0x34, 0x35, 0x36, 0x37, 0x18, 0x19,
                     0x1A, 0x1B, 0x38, 0x39, 0x3A, 0x3B, 0x1C, 0x1D, 0x1E, 0x1F,
                     0x3C, 0x3D, 0x3E, 0x3F);
      permCtrlDeint =
          (uchar64) (0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
                     0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13,
                     0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D,
                     0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
                     0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31,
                     0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B,
                     0x3C, 0x3D, 0x3E, 0x3F);
#else
      permCtrlInt = uchar64 (
          0x00, 0x01, 0x02, 0x03, 0x20, 0x21, 0x22, 0x23, 0x04, 0x05, 0x06,
          0x07, 0x24, 0x25, 0x26, 0x27, 0x08, 0x09, 0x0A, 0x0B, 0x28, 0x29,
          0x2A, 0x2B, 0x0C, 0x0D, 0x0E, 0x0F, 0x2C, 0x2D, 0x2E, 0x2F, 0x10,
          0x11, 0x12, 0x13, 0x30, 0x31, 0x32, 0x33, 0x14, 0x15, 0x16, 0x17,
          0x34, 0x35, 0x36, 0x37, 0x18, 0x19, 0x1A, 0x1B, 0x38, 0x39, 0x3A,
          0x3B, 0x1C, 0x1D, 0x1E, 0x1F, 0x3C, 0x3D, 0x3E, 0x3F);
      permCtrlDeint = uchar64 (
          0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A,
          0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
          0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20,
          0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B,
          0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36,
          0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F);
#endif
      //   *stov_ptr(uchar64, (uint8_t *)pBlock + PERMCTRLINT_OFFSET) =
      //   permCtrlInt; *stov_ptr(uchar64, (uint8_t *)pBlock +
      //   PERMCTRLDEINT_OFFSET) = permCtrlDeint;
      *((uchar64*) ((uint8_t *) pBlock + PERMCTRLINT_OFFSET)) = permCtrlInt;
      *((uchar64*) ((uint8_t *) pBlock + PERMCTRLDEINT_OFFSET)) =
          permCtrlDeint;
      /* permCtrl64Pt is not relevant for FFTLIB_INT32 data type              */
   }
   else {
      status = FFTLIB_ERR_NOT_IMPLEMENTED;
   }

   if (status == FFTLIB_SUCCESS) {
      fftSizeModuloPower = fftSize;
      while (fftSizeModuloPower > MMA_SIZE / 2) {
         fftSizeModuloPower /= MMA_SIZE / 2;
      }
      decompositionFactorStage0 = fftSizeModuloPower;
      *((uint32_t *) ((uint8_t *) pBlock + DECOMPOSITIONFACTORSTAGE0_OFFSET)) =
          decompositionFactorStage0;

      logNStageCnt   = 0;
      dftSizeInStage = fftSize;

      /**********************************************************************/
      /* Prepare SE to fetch data for interleaving                          */
      /**********************************************************************/
      seParams = __gen_SE_TEMPLATE_v1 ();

      seParams.ICNT0 = MMA_SIZE;
      seParams.ICNT1 = 2;
      seParams.DIM1  = fftSize;
      seParams.ICNT2 = fftSize / MMA_SIZE;
      seParams.DIM2  = MMA_SIZE;
      seParams.ICNT3 = batchSize;
      seParams.DIM3  = fftSize * 2;

      seParams.ELETYPE = SE_ELETYPE;
      seParams.VECLEN  = SE_VECLEN;
      seParams.DIMFMT  = __SE_DIMFMT_4D;

      *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_INT_DATA_PARAM_OFFSET)) =
          seParams;

      /**********************************************************************/
      /* Prepare SA template to store output after interleaving             */
      /**********************************************************************/
      saParams = __gen_SA_TEMPLATE_v1 ();

      saParams.ICNT0 =
          fftSize * batchSize * 2 * FFTLIB_sizeof (bufParamsY->data_type);

      saParams.VECLEN = __SA_VECLEN_64ELEMS;
      saParams.DIMFMT = __SA_DIMFMT_1D;

      *((__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA_INT_OUT_PARAM_OFFSET)) =
          saParams;

      /**********************************************************************/
      /* Prepare SE to fetch input data into B matrix for the logN-stage 0  */
      /**********************************************************************/
      seParams = __gen_SE_TEMPLATE_v1 ();

      seParams.ICNT0 = MMA_SIZE / 2;

      seParams.ICNT1 = decompositionFactorStage0;
      seParams.DIM1  = fftSize / decompositionFactorStage0;

      seParams.ICNT2 = (fftSize * 2) / (MMA_SIZE * decompositionFactorStage0);
      seParams.DIM2  = MMA_SIZE / 2;

      seParams.ICNT3 = batchSize;
      seParams.DIM3  = fftSize;

      /* Dummy dimension to facilitate extra loads in the loops in code */
      seParams.ICNT4 = 100;
      seParams.DIM4  = 0;

      seParams.ELETYPE = SE_ELETYPE_CMPLX;
      seParams.VECLEN  = SE_VECLEN_CMPLX;
      seParams.DIMFMT  = __SE_DIMFMT_5D;

      *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                              SE_LOGNSTAGE0_DATA_PARAM_OFFSET)) = seParams;

      /**********************************************************************/
      /* Prepare SA to fetch twiddle factors(1) for logN-stages             */
      /**********************************************************************/
      saParams = __gen_SA_TEMPLATE_v1 ();

      saParams.ICNT0 =
          MMA_SIZE * (MMA_SIZE / 2) * FFTLIB_sizeof (bufParamsY->data_type);

      /* Possibly one extra panel load, but doesn't harm                    */
      saParams.ICNT1 = (fftSize * batchSize * 4) / (MMA_SIZE * MMA_SIZE) + 1;
      saParams.DIM1  = 0;

      /* Dummy dimension to facilitate extra loads in the loops in code     */
      saParams.ICNT2 = 100;
      saParams.DIM2  = 0;

      saParams.VECLEN = __SA_VECLEN_64ELEMS;
      saParams.DIMFMT = __SA_DIMFMT_3D;

      *((__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                              SA_LOGNSTAGE0_TW1_PARAM_OFFSET)) = saParams;

      /**********************************************************************/
      /* Prepare SE to fetch twiddle factors(2) for logN-stage 0            */
      /**********************************************************************/
      seParams = __gen_SE_TEMPLATE_v1 ();

      seParams.ICNT0 = fftSize * 2;
      seParams.ICNT1 = batchSize;
      seParams.DIM1  = 0;

      seParams.ELETYPE = SE_ELETYPE;
      seParams.VECLEN  = SE_VECLEN;
      seParams.DIMFMT  = __SE_DIMFMT_2D;

      *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                              SE_LOGNSTAGE0_TW2_PARAM_OFFSET)) = seParams;

      /**********************************************************************/
      /* Prepare SA to store output from logN-stage 0                       */
      /**********************************************************************/
      saParams = __gen_SA_TEMPLATE_v1 ();

      saParams.ICNT0 = MMA_SIZE * FFTLIB_sizeof (bufParamsY->data_type);
      saParams.ICNT1 = decompositionFactorStage0;
      saParams.DIM1  = ((fftSize * 2) / decompositionFactorStage0) *
                      FFTLIB_sizeof (bufParamsY->data_type);
      saParams.ICNT2 = (fftSize * 2) / (decompositionFactorStage0 * MMA_SIZE);
      saParams.DIM2  = MMA_SIZE * FFTLIB_sizeof (bufParamsY->data_type);
      saParams.ICNT3 = batchSize;
      saParams.DIM3  = fftSize * 2 * FFTLIB_sizeof (bufParamsY->data_type);

      saParams.VECLEN = __SA_VECLEN_64ELEMS;
      saParams.DIMFMT = __SA_DIMFMT_4D;

      *((__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                              SA_LOGNSTAGE0_OUT_PARAM_OFFSET)) = saParams;

      /**********************************************************************/
      /* Prepare MMA for logN-stage 0                                       */
      /**********************************************************************/
      mmaConfig = mmaConfigDefault;

      /* The number of computes is half the number of loads into B panel. So,
       * while the B_STM switches the B panel every MMA_SIZE
       * cycles, the C_STM switches the B panel every MMA_SIZE/2
       * cycles                                                               */
      mmaConfig.C_BSWPER = MMA_SIZE / 2;

      mmaConfig.C_CWSWPER  = 4;
      mmaConfig.C_CRSWPER  = 4;
      mmaConfig.C_CRRSTPER = 4;
      mmaConfig.C_CWRSTPER = 4;

      mmaConfig.X_SHIFT    = shift0;
      mmaConfig.X_CSWPER   = 4;
      mmaConfig.X_CRRSTPER = 4;

      mmaOffset = offsetRegStruct_zeros;
      *((FFTLIB_MMA_CONFIG_REG *) ((uint8_t *) pBlock +
                                   MMA_LOGNSTAGE0_CONFIGREG_OFFSET)) =
          mmaConfig;
      *((__HWA_OFFSET_REG *) ((uint8_t *) pBlock +
                              MMA_LOGNSTAGE0_OFFSETREG_OFFSET)) = mmaOffset;

      logNStageCnt++;
      dftSizeInStage /= decompositionFactorStage0;

      if (dftSizeInStage > MMA_SIZE / 2) {
         /***************************************************************************/
         /* Prepare SE to fetch input data into A matrix for the logN-stage 1 */
         /***************************************************************************/
         seParams = __gen_SE_TEMPLATE_v1 ();

         seParams.ICNT0 = MMA_SIZE;

         seParams.ICNT1 = MMA_SIZE / 2;
         seParams.DIM1  = (dftSizeInStage * 4) / MMA_SIZE;

         seParams.ICNT2 = (dftSizeInStage * 4) / (MMA_SIZE * MMA_SIZE);
         seParams.DIM2  = MMA_SIZE;

         seParams.ICNT3 = fftSize / dftSizeInStage;
         seParams.DIM3  = dftSizeInStage * 2;

         seParams.ICNT4 = batchSize;
         seParams.DIM4  = fftSize * 2;

         /* Dummy dimension to facilitate an extra load of 4 rows because of
          * structure of loops */
         seParams.ICNT5 = 100;
         seParams.DIM5  = 0;

         seParams.ELETYPE = SE_ELETYPE;
         seParams.VECLEN  = SE_VECLEN;
         seParams.DIMFMT  = __SE_DIMFMT_6D;

         *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                                 SE_LOGNSTAGE1_DATA_PARAM_OFFSET)) = seParams;

         /**********************************************************************/
         /* Prepare SA to fetch twiddle factors(1) for logN-stages */
         /**********************************************************************/
         saParams = __gen_SA_TEMPLATE_v1 ();

         saParams.ICNT0 =
             MMA_SIZE * (MMA_SIZE / 2) * FFTLIB_sizeof (bufParamsY->data_type);
         saParams.ICNT1 = (fftSize * batchSize * 4) / (MMA_SIZE * MMA_SIZE);
         saParams.DIM1  = 0;

         /* Dummy dimension to facilitate extra loads in the loops in code */
         saParams.ICNT2 = 100;
         saParams.DIM2  = 0;

         saParams.VECLEN = __SA_VECLEN_64ELEMS;
         saParams.DIMFMT = __SA_DIMFMT_3D;

         *((__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                                 SA_LOGNSTAGE1_TW1_PARAM_OFFSET)) = saParams;

         /**********************************************************************/
         /* Prepare SE to fetch twiddle factors(2) for logN-stage 1 */
         /**********************************************************************/
         seParams = __gen_SE_TEMPLATE_v1 ();

         seParams.ICNT0 = dftSizeInStage * 2;

         seParams.ICNT1 = fftSize / dftSizeInStage;
         seParams.DIM1  = 0;

         seParams.ICNT2 = batchSize;
         seParams.DIM2  = 0;

         seParams.ELETYPE = SE_ELETYPE;
         seParams.VECLEN  = SE_VECLEN;
         seParams.DIMFMT  = __SE_DIMFMT_3D;

         *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                                 SE_LOGNSTAGE1_TW2_PARAM_OFFSET)) = seParams;

         /**********************************************************************/
         /* Prepare SA to store output from logN-stage 1 */
         /**********************************************************************/
         saParams = __gen_SA_TEMPLATE_v1 ();

         saParams.ICNT0 = MMA_SIZE * FFTLIB_sizeof (bufParamsY->data_type);

         saParams.ICNT1 = MMA_SIZE / 2;
         saParams.DIM1  = ((dftSizeInStage * 4) / MMA_SIZE) *
                         FFTLIB_sizeof (bufParamsY->data_type);

         saParams.ICNT2 = (dftSizeInStage * 4) / (MMA_SIZE * MMA_SIZE);
         saParams.DIM2  = MMA_SIZE * FFTLIB_sizeof (bufParamsY->data_type);

         saParams.ICNT3 = fftSize / dftSizeInStage;
         saParams.DIM3 =
             dftSizeInStage * 2 * FFTLIB_sizeof (bufParamsY->data_type);

         saParams.ICNT4 = batchSize;
         saParams.DIM4  = fftSize * 2 * FFTLIB_sizeof (bufParamsY->data_type);

         saParams.VECLEN = __SA_VECLEN_64ELEMS;
         saParams.DIMFMT = __SA_DIMFMT_5D;

         *((__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                                 SA_LOGNSTAGE1_OUT_PARAM_OFFSET)) = saParams;

         /**********************************************************************/
         /* Prepare MMA for logN-stage 1 */
         /**********************************************************************/
         mmaConfig.X_SHIFT = shift1;
         *((FFTLIB_MMA_CONFIG_REG *) ((uint8_t *) pBlock +
                                      MMA_LOGNSTAGE1_CONFIGREG_OFFSET)) =
             mmaConfig;
         *((__HWA_OFFSET_REG *) ((uint8_t *) pBlock +
                                 MMA_LOGNSTAGE1_OFFSETREG_OFFSET)) = mmaOffset;

         logNStageCnt++;
         dftSizeInStage /= MMA_SIZE / 2;

         if (dftSizeInStage > MMA_SIZE / 2) {
            /***************************************************************************/
            /* Prepare SE to fetch input data into A matrix for the logN-stage 2
             */
            /***************************************************************************/
            seParams = __gen_SE_TEMPLATE_v1 ();

            seParams.ICNT0 = MMA_SIZE;

            seParams.ICNT1 = MMA_SIZE / 2;
            seParams.DIM1  = (dftSizeInStage * 4) / MMA_SIZE;

            seParams.ICNT2 = (dftSizeInStage * 4) / (MMA_SIZE * MMA_SIZE);
            seParams.DIM2  = MMA_SIZE;

            seParams.ICNT3 = fftSize / dftSizeInStage;
            seParams.DIM3  = dftSizeInStage * 2;

            seParams.ICNT4 = batchSize;
            seParams.DIM4  = fftSize * 2;

            /* Dummy dimension to facilitate an extra load of 4 rows because of
             * structure of loops */
            seParams.ICNT5 = 100;
            seParams.DIM5  = 0;

            seParams.ELETYPE = SE_ELETYPE;
            seParams.VECLEN  = SE_VECLEN;
            seParams.DIMFMT  = __SE_DIMFMT_6D;

            *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                                    SE_LOGNSTAGE2_DATA_PARAM_OFFSET)) =
                seParams;

            /**********************************************************************/
            /* Prepare SA to fetch twiddle factors(1) for logN-stages */
            /**********************************************************************/
            saParams = __gen_SA_TEMPLATE_v1 ();

            saParams.ICNT0 = MMA_SIZE * (MMA_SIZE / 2) *
                             FFTLIB_sizeof (bufParamsY->data_type);
            saParams.ICNT1 = (fftSize * batchSize * 4) / (MMA_SIZE * MMA_SIZE);
            saParams.DIM1  = 0;

            /* Dummy dimension to facilitate extra loads in the loops in code */
            saParams.ICNT2 = 100;
            saParams.DIM2  = 0;

            saParams.VECLEN = __SA_VECLEN_64ELEMS;
            saParams.DIMFMT = __SA_DIMFMT_3D;

            *((__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                                    SA_LOGNSTAGE2_TW1_PARAM_OFFSET)) = saParams;

            /**********************************************************************/
            /* Prepare SE to fetch twiddle factors(2) for logN-stage 2 */
            /**********************************************************************/
            seParams = __gen_SE_TEMPLATE_v1 ();

            seParams.ICNT0 = dftSizeInStage * 2;

            seParams.ICNT1 = fftSize / dftSizeInStage;
            seParams.DIM1  = 0;

            seParams.ICNT2 = batchSize;
            seParams.DIM2  = 0;

            seParams.ELETYPE = SE_ELETYPE;
            seParams.VECLEN  = SE_VECLEN;
            seParams.DIMFMT  = __SE_DIMFMT_3D;

            *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                                    SE_LOGNSTAGE2_TW2_PARAM_OFFSET)) = seParams;

            /**********************************************************************/
            /* Prepare SA to store output from logN-stage 2 */
            /**********************************************************************/
            saParams = __gen_SA_TEMPLATE_v1 ();

            saParams.ICNT0 = MMA_SIZE * FFTLIB_sizeof (bufParamsY->data_type);

            saParams.ICNT1 = MMA_SIZE / 2;
            saParams.DIM1  = ((dftSizeInStage * 4) / MMA_SIZE) *
                            FFTLIB_sizeof (bufParamsY->data_type);

            saParams.ICNT2 = (dftSizeInStage * 4) / (MMA_SIZE * MMA_SIZE);
            saParams.DIM2  = MMA_SIZE * FFTLIB_sizeof (bufParamsY->data_type);

            saParams.ICNT3 = fftSize / dftSizeInStage;
            saParams.DIM3 =
                dftSizeInStage * 2 * FFTLIB_sizeof (bufParamsY->data_type);

            saParams.ICNT4 = batchSize;
            saParams.DIM4 = fftSize * 2 * FFTLIB_sizeof (bufParamsY->data_type);

            saParams.VECLEN = __SA_VECLEN_64ELEMS;
            saParams.DIMFMT = __SA_DIMFMT_5D;

            *((__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                                    SA_LOGNSTAGE2_OUT_PARAM_OFFSET)) = saParams;

            /**********************************************************************/
            /* Prepare MMA for logN-stage 2 */
            /**********************************************************************/
            mmaConfig.X_SHIFT = shift2;
            *((FFTLIB_MMA_CONFIG_REG *) ((uint8_t *) pBlock +
                                         MMA_LOGNSTAGE2_CONFIGREG_OFFSET)) =
                mmaConfig;
            *((__HWA_OFFSET_REG *) ((uint8_t *) pBlock +
                                    MMA_LOGNSTAGE2_OFFSETREG_OFFSET)) =
                mmaOffset;

            logNStageCnt++;
            dftSizeInStage /= MMA_SIZE / 2;

            if (dftSizeInStage > MMA_SIZE / 2) {
               /***************************************************************************/
               /* Prepare SE to fetch input data into A matrix for the
                * logN-stage 3       */
               /***************************************************************************/
               seParams = __gen_SE_TEMPLATE_v1 ();

               seParams.ICNT0 = MMA_SIZE;

               seParams.ICNT1 = MMA_SIZE / 2;
               seParams.DIM1  = (dftSizeInStage * 4) / MMA_SIZE;

               seParams.ICNT2 = (dftSizeInStage * 4) / (MMA_SIZE * MMA_SIZE);
               seParams.DIM2  = MMA_SIZE;

               seParams.ICNT3 = fftSize / dftSizeInStage;
               seParams.DIM3  = dftSizeInStage * 2;

               seParams.ICNT4 = batchSize;
               seParams.DIM4  = fftSize * 2;

               /* Dummy dimension to facilitate an extra load of 4 rows because
                * of structure of loops */
               seParams.ICNT5 = 100;
               seParams.DIM5  = 0;

               seParams.ELETYPE = SE_ELETYPE;
               seParams.VECLEN  = SE_VECLEN;
               seParams.DIMFMT  = __SE_DIMFMT_6D;

               *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                                       SE_LOGNSTAGE3_DATA_PARAM_OFFSET)) =
                   seParams;

               /**********************************************************************/
               /* Prepare SA to fetch twiddle factors(1) for logN-stages */
               /**********************************************************************/
               saParams = __gen_SA_TEMPLATE_v1 ();

               saParams.ICNT0 = MMA_SIZE * (MMA_SIZE / 2) *
                                FFTLIB_sizeof (bufParamsY->data_type);
               saParams.ICNT1 =
                   (fftSize * batchSize * 4) / (MMA_SIZE * MMA_SIZE);
               saParams.DIM1 = 0;

               /* Dummy dimension to facilitate extra loads in the loops in code
                */
               saParams.ICNT2 = 100;
               saParams.DIM2  = 0;

               saParams.VECLEN = __SA_VECLEN_64ELEMS;
               saParams.DIMFMT = __SA_DIMFMT_3D;

               *((__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                                       SA_LOGNSTAGE3_TW1_PARAM_OFFSET)) =
                   saParams;

               /**********************************************************************/
               /* Prepare SE to fetch twiddle factors(2) for logN-stage 3 */
               /**********************************************************************/
               seParams = __gen_SE_TEMPLATE_v1 ();

               seParams.ICNT0 = dftSizeInStage * 2;

               seParams.ICNT1 = fftSize / dftSizeInStage;
               seParams.DIM1  = 0;

               seParams.ICNT2 = batchSize;
               seParams.DIM2  = 0;

               seParams.ELETYPE = SE_ELETYPE;
               seParams.VECLEN  = SE_VECLEN;
               seParams.DIMFMT  = __SE_DIMFMT_3D;

               *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                                       SE_LOGNSTAGE3_TW2_PARAM_OFFSET)) =
                   seParams;

               /**********************************************************************/
               /* Prepare SA to store output from logN-stage 3 */
               /**********************************************************************/
               saParams = __gen_SA_TEMPLATE_v1 ();

               saParams.ICNT0 =
                   MMA_SIZE * FFTLIB_sizeof (bufParamsY->data_type);

               saParams.ICNT1 = MMA_SIZE / 2;
               saParams.DIM1  = ((dftSizeInStage * 4) / MMA_SIZE) *
                               FFTLIB_sizeof (bufParamsY->data_type);

               saParams.ICNT2 = (dftSizeInStage * 4) / (MMA_SIZE * MMA_SIZE);
               saParams.DIM2 = MMA_SIZE * FFTLIB_sizeof (bufParamsY->data_type);

               saParams.ICNT3 = fftSize / dftSizeInStage;
               saParams.DIM3 =
                   dftSizeInStage * 2 * FFTLIB_sizeof (bufParamsY->data_type);

               saParams.ICNT4 = batchSize;
               saParams.DIM4 =
                   fftSize * 2 * FFTLIB_sizeof (bufParamsY->data_type);

               saParams.VECLEN = __SA_VECLEN_64ELEMS;
               saParams.DIMFMT = __SA_DIMFMT_5D;

               *((__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                                       SA_LOGNSTAGE3_OUT_PARAM_OFFSET)) =
                   saParams;

               /**********************************************************************/
               /* Prepare MMA for logN-stage 3 */
               /**********************************************************************/
               mmaConfig.X_SHIFT = shift3;
               *((FFTLIB_MMA_CONFIG_REG *) ((uint8_t *) pBlock +
                                            MMA_LOGNSTAGE3_CONFIGREG_OFFSET)) =
                   mmaConfig;
               *((__HWA_OFFSET_REG *) ((uint8_t *) pBlock +
                                       MMA_LOGNSTAGE3_OFFSETREG_OFFSET)) =
                   mmaOffset;

               logNStageCnt++;
            }
         }
      }

      /**********************************************************************/
      /* Prepare SE to fetch input data into B matrix for the DFT stage     */
      /**********************************************************************/
      seParams = __gen_SE_TEMPLATE_v1 ();

      seParams.ICNT0 = MMA_SIZE;

      seParams.ICNT1 = decompositionFactorStage0;
      seParams.DIM1  = (fftSize * 2) / decompositionFactorStage0;

      if (logNStageCnt == 4) {
         seParams.ICNT2 = MMA_SIZE / 2;
         seParams.DIM2 = (fftSize * 4) / (decompositionFactorStage0 * MMA_SIZE);
         seParams.ICNT3 = MMA_SIZE / 2;
         seParams.DIM3 =
             (fftSize * 8) / (decompositionFactorStage0 * MMA_SIZE * MMA_SIZE);
         seParams.ICNT4 = MMA_SIZE / 2;
         seParams.DIM4  = (fftSize * 16) / (decompositionFactorStage0 *
                                           MMA_SIZE * MMA_SIZE * MMA_SIZE);
      }
      else if (logNStageCnt == 3) {
         seParams.ICNT2 = MMA_SIZE / 2;
         seParams.DIM2 = (fftSize * 4) / (decompositionFactorStage0 * MMA_SIZE);
         seParams.ICNT3 = MMA_SIZE / 2;
         seParams.DIM3 =
             (fftSize * 8) / (decompositionFactorStage0 * MMA_SIZE * MMA_SIZE);
         seParams.ICNT4 = 1;
         seParams.DIM4  = (fftSize * 16) / (decompositionFactorStage0 *
                                           MMA_SIZE * MMA_SIZE * MMA_SIZE);
      }
      else if (logNStageCnt == 2) {
         seParams.ICNT2 = MMA_SIZE / 2;
         seParams.DIM2 = (fftSize * 4) / (decompositionFactorStage0 * MMA_SIZE);
         seParams.ICNT3 = 1;
         seParams.DIM3 =
             (fftSize * 8) / (decompositionFactorStage0 * MMA_SIZE * MMA_SIZE);
         seParams.ICNT4 = 1;
         seParams.DIM4  = (fftSize * 16) / (decompositionFactorStage0 *
                                           MMA_SIZE * MMA_SIZE * MMA_SIZE);
      }
      else {
         seParams.ICNT2 = 1;
         seParams.DIM2 = (fftSize * 4) / (decompositionFactorStage0 * MMA_SIZE);
         seParams.ICNT3 = 1;
         seParams.DIM3 =
             (fftSize * 8) / (decompositionFactorStage0 * MMA_SIZE * MMA_SIZE);
         seParams.ICNT4 = 1;
         seParams.DIM4  = (fftSize * 16) / (decompositionFactorStage0 *
                                           MMA_SIZE * MMA_SIZE * MMA_SIZE);
      }

      /* We overprovision dimension 5 to facilitate extra loops in code */
      seParams.ICNT5 = 100 * batchSize;
      seParams.DIM5  = fftSize * 2;

      seParams.DECDIM1_WIDTH = batchSize * fftSize * 2;

      seParams.DECDIM1 = __SE_DECDIM_DIM5;
      seParams.ELETYPE = SE_ELETYPE;
      seParams.VECLEN  = SE_VECLEN;
      seParams.DIMFMT  = __SE_DIMFMT_6D;

      *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                              SE_DFTSTAGE_DATA_PARAM_OFFSET)) = seParams;

#if 0
      /**********************************************************************/
      /* Prepare SA to fetch DFT matrix into A vectors for DFT stage        */
      /**********************************************************************/
      saParams  = __gen_SA_TEMPLATE_v1();

      saParams.ICNT0 =                               MMA_SIZE*MMA_SIZE*                              FFTLIB_sizeof(bufParamsY->data_type);
      if (fftSize*16 == MMA_SIZE*MMA_SIZE) {
         /* Round up to higher multiple of 8 number of channels so that the last
          * partial panel is also processed.                                     */
         saParams.ICNT1 =  (batchSize+7)/8;
         saParams.DIM1 =  0;
      } else if (fftSize*8 == MMA_SIZE*MMA_SIZE) {
         /* Round up to higher multiple of 4 number of channels so that the last
          * partial panel is also processed.                                     */
         saParams.ICNT1 =  (batchSize+3)/4;
         saParams.DIM1 =  0;
      } else if (fftSize*4 == MMA_SIZE*MMA_SIZE) {
         /* Round up to even number of channels so that the last partial panel
          * is also processed.                                                   */
         saParams.ICNT1 =  (batchSize+1)/2;
         saParams.DIM1 =  0;
      } else {
         saParams.ICNT1 =                                       (fftSize*batchSize*2)/(MMA_SIZE*MMA_SIZE);
         saParams.DIM1 =                                       0;
      }

      saParams.VECLEN    = __SA_VECLEN_64ELEMS;
      saParams.DIMFMT    = __SA_DIMFMT_2D;

      *((__SA_TEMPLATE_v1*)((uint8_t *)pBlock + SA_DFTSTAGE_TW_PARAM_OFFSET)) = saParams;
#else
      /**********************************************************************/
      /* Prepare SE to fetch DFT matrix into A vectors for DFT stage        */
      /**********************************************************************/
      seParams = __gen_SE_TEMPLATE_v1 ();

      seParams.ICNT0 = MMA_SIZE * MMA_SIZE;
      if (fftSize * 16 == MMA_SIZE * MMA_SIZE) {
         /* Round up to higher multiple of 8 number of channels so that the last
          * partial panel is also processed. */
         seParams.ICNT1 = (batchSize + 7) / 8;
         seParams.DIM1  = 0;
      }
      else if (fftSize * 8 == MMA_SIZE * MMA_SIZE) {
         /* Round up to higher multiple of 4 number of channels so that the last
          * partial panel is also processed. */
         seParams.ICNT1 = (batchSize + 3) / 4;
         seParams.DIM1  = 0;
      }
      else if (fftSize * 4 == MMA_SIZE * MMA_SIZE) {
         /* Round up to even number of channels so that the last partial panel
          * is also processed. */
         seParams.ICNT1 = (batchSize + 1) / 2;
         seParams.DIM1  = 0;
      }
      else {
         seParams.ICNT1 = (fftSize * batchSize * 2) / (MMA_SIZE * MMA_SIZE);
         seParams.DIM1  = 0;
      }

      seParams.ELETYPE = SE_ELETYPE;
      seParams.VECLEN  = SE_VECLEN;
      seParams.DIMFMT  = __SE_DIMFMT_2D;

      *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                              SE_DFTSTAGE_TW_PARAM_OFFSET)) = seParams;
#endif

      /**********************************************************************/
      /* Prepare SA to store output (full MMA panels) from DFT stage        */
      /**********************************************************************/
      saParams = __gen_SA_TEMPLATE_v1 ();

      saParams.ICNT0 = MMA_SIZE * FFTLIB_sizeof (bufParamsY->data_type);

      if (fftSize * 16 == MMA_SIZE * MMA_SIZE) {
         saParams.ICNT1 = 8;
         saParams.DIM1  = fftSize * 2 * FFTLIB_sizeof (bufParamsY->data_type);
         saParams.ICNT2 = MMA_SIZE / 8;
         saParams.DIM2  = MMA_SIZE * FFTLIB_sizeof (bufParamsY->data_type);
         /* ICNT3 should actually be batchSize/8, but we overprovision the
          * dimension 3. This is harmless because the loop does not run
          * beyond the correct dimension anyway. This is just to avoid a
          * warning in batchSize < 8 case when batchSize/8 would amount
          * to 0. In this case, the SA is open, but is never used.         */
         saParams.ICNT3 = batchSize;
         saParams.DIM3  = fftSize * 16 * FFTLIB_sizeof (bufParamsY->data_type);
         saParams.ICNT4 = 1;
         saParams.DIM4  = fftSize * 2 * FFTLIB_sizeof (bufParamsY->data_type);
      }
      else if (fftSize * 8 == MMA_SIZE * MMA_SIZE) {
         saParams.ICNT1 = 4;
         saParams.DIM1  = fftSize * 2 * FFTLIB_sizeof (bufParamsY->data_type);
         saParams.ICNT2 = MMA_SIZE / 4;
         saParams.DIM2  = MMA_SIZE * FFTLIB_sizeof (bufParamsY->data_type);
         /* ICNT3 should actually be batchSize/4, but we overprovision the
          * dimension 3. This is harmless because the loop does not run
          * beyond the correct dimension anyway. This is just to avoid a
          * warning in batchSize < 4 case when batchSize/4 would amount
          * to 0. In this case, the SA is open, but is never used.         */
         saParams.ICNT3 = batchSize;
         saParams.DIM3  = fftSize * 8 * FFTLIB_sizeof (bufParamsY->data_type);
         saParams.ICNT4 = 1;
         saParams.DIM4  = fftSize * 2 * FFTLIB_sizeof (bufParamsY->data_type);
      }
      else if (fftSize * 4 == MMA_SIZE * MMA_SIZE) {
         saParams.ICNT1 = 2;
         saParams.DIM1  = fftSize * 2 * FFTLIB_sizeof (bufParamsY->data_type);
         saParams.ICNT2 = MMA_SIZE / 2;
         saParams.DIM2  = MMA_SIZE * FFTLIB_sizeof (bufParamsY->data_type);
         /* ICNT3 should actually be batchSize/2, but we overprovision the
          * dimension 3. This is harmless because the loop does not run
          * beyond the correct dimension anyway. This is just to avoid a
          * warning in batchSize < 2 case when batchSize/2 would amount
          * to 0. In this case, the SA is open, but is never used.         */
         saParams.ICNT3 = batchSize;
         saParams.DIM3  = fftSize * 4 * FFTLIB_sizeof (bufParamsY->data_type);
         saParams.ICNT4 = 1;
         saParams.DIM4  = fftSize * 2 * FFTLIB_sizeof (bufParamsY->data_type);
      }
      else {
         saParams.ICNT1 = 2;
         saParams.DIM1  = MMA_SIZE * FFTLIB_sizeof (bufParamsY->data_type);
         saParams.ICNT2 = MMA_SIZE / 2;
         saParams.DIM2 =
             ((fftSize * 4) / MMA_SIZE) * FFTLIB_sizeof (bufParamsY->data_type);
         saParams.ICNT3 = (fftSize * 2) / (MMA_SIZE * MMA_SIZE);
         saParams.DIM3  = MMA_SIZE * 2 * FFTLIB_sizeof (bufParamsY->data_type);
         saParams.ICNT4 = batchSize;
         saParams.DIM4  = fftSize * 2 * FFTLIB_sizeof (bufParamsY->data_type);
      }

      saParams.VECLEN = __SA_VECLEN_64ELEMS;
      saParams.DIMFMT = __SA_DIMFMT_5D;

      *((__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                              SA_DFTSTAGE_OUT_PARAM_OFFSET)) = saParams;

      /**********************************************************************/
      /* Prepare SA to store output (last partial MMA panel) from DFT stage */
      /**********************************************************************/
      saParams = __gen_SA_TEMPLATE_v1 ();

      saParams.ICNT0 = MMA_SIZE * FFTLIB_sizeof (bufParamsY->data_type);
      if (fftSize * 16 == MMA_SIZE * MMA_SIZE) {
         saParams.ICNT1 = 8;
         saParams.DIM1  = fftSize * 2 * FFTLIB_sizeof (bufParamsY->data_type);
         saParams.ICNT2 = MMA_SIZE / 8;
         saParams.DIM2  = MMA_SIZE * FFTLIB_sizeof (bufParamsY->data_type);
         if ((batchSize & 7u) == 0) {
            saParams.DECDIM1_WIDTH =
                8 * fftSize * 2 * FFTLIB_sizeof (bufParamsY->data_type);
         }
         else {
            saParams.DECDIM1_WIDTH = (batchSize & 7u) * fftSize * 2 *
                                     FFTLIB_sizeof (bufParamsY->data_type);
         }
      }
      else if (fftSize * 8 == MMA_SIZE * MMA_SIZE) {
         saParams.ICNT1 = 4;
         saParams.DIM1  = fftSize * 2 * FFTLIB_sizeof (bufParamsY->data_type);
         saParams.ICNT2 = MMA_SIZE / 4;
         saParams.DIM2  = MMA_SIZE * FFTLIB_sizeof (bufParamsY->data_type);
         if ((batchSize & 3u) == 0) {
            saParams.DECDIM1_WIDTH =
                4 * fftSize * 2 * FFTLIB_sizeof (bufParamsY->data_type);
         }
         else {
            saParams.DECDIM1_WIDTH = (batchSize & 3u) * fftSize * 2 *
                                     FFTLIB_sizeof (bufParamsY->data_type);
         }
      }
      else {
         saParams.ICNT1 = 2;
         saParams.DIM1  = fftSize * 2 * FFTLIB_sizeof (bufParamsY->data_type);
         saParams.ICNT2 = MMA_SIZE / 2;
         saParams.DIM2  = MMA_SIZE * FFTLIB_sizeof (bufParamsY->data_type);
         saParams.DECDIM1_WIDTH =
             fftSize * 2 * FFTLIB_sizeof (bufParamsY->data_type);
      }

      saParams.VECLEN  = __SA_VECLEN_64ELEMS;
      saParams.DIMFMT  = __SA_DIMFMT_3D;
      saParams.DECDIM1 = __SA_DECDIM_DIM1;

      *((__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                              SA_DFTSTAGE2_OUT_PARAM_OFFSET)) = saParams;

      /**********************************************************************/
      /* Prepare MMA for DFT stage                                          */
      /**********************************************************************/
      mmaConfig = mmaConfigDefault;

      mmaConfig.B_ORDER    = __MMA_B_CONFIG_COL;
      mmaConfig.C_CWSWPER  = 4;
      mmaConfig.C_CRSWPER  = 4;
      mmaConfig.C_CRRSTPER = 4;
      mmaConfig.C_CWRSTPER = 4;

      mmaConfig.X_SHIFT    = dftShift;
      mmaConfig.X_CSWPER   = 4;
      mmaConfig.X_CRRSTPER = 4;

      mmaOffset = offsetRegStruct_zeros;
      *((FFTLIB_MMA_CONFIG_REG *) ((uint8_t *) pBlock +
                                   MMA_DFTSTAGE_CONFIGREG_OFFSET)) = mmaConfig;
      *((__HWA_OFFSET_REG *) ((uint8_t *) pBlock +
                              MMA_DFTSTAGE_OFFSETREG_OFFSET))      = mmaOffset;

      /**********************************************************************/
      /* Prepare SE to fetch data for deinterleaving                          */
      /**********************************************************************/
      seParams = __gen_SE_TEMPLATE_v1 ();

      seParams.ICNT0 = batchSize * fftSize * 2;

      seParams.ELETYPE = SE_ELETYPE;
      seParams.VECLEN  = SE_VECLEN;
      seParams.DIMFMT  = __SE_DIMFMT_1D;

      *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                              SE_DEINT_DATA_PARAM_OFFSET)) = seParams;

      /**********************************************************************/
      /* Prepare SA template to store output after deinterleaving             */
      /**********************************************************************/
      saParams = __gen_SA_TEMPLATE_v1 ();

      saParams.ICNT0 = MMA_SIZE * FFTLIB_sizeof (bufParamsY->data_type);
      saParams.ICNT1 = 2;
      saParams.DIM1  = fftSize * FFTLIB_sizeof (bufParamsY->data_type);
      saParams.ICNT2 = fftSize / MMA_SIZE;
      saParams.DIM2  = MMA_SIZE * FFTLIB_sizeof (bufParamsY->data_type);
      saParams.ICNT3 = batchSize;
      saParams.DIM3  = fftSize * 2 * FFTLIB_sizeof (bufParamsY->data_type);

      saParams.VECLEN = __SA_VECLEN_64ELEMS;
      saParams.DIMFMT = __SA_DIMFMT_4D;

      *((__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA_DEINT_OUT_PARAM_OFFSET)) =
          saParams;
   }

   return (status);
}

template FFTLIB_STATUS
FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_init_ci<FFTLIB_INT16> (
    FFTLIB_kernelHandle         handle,
    const FFTLIB_bufParams1D_t *bufParamsX,
    const FFTLIB_bufParams1D_t *bufParamsW,
    const FFTLIB_bufParams1D_t *bufParamsY,
    const FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_InitArgs
        *pKerInitArgs);

template FFTLIB_STATUS
FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_init_ci<FFTLIB_INT32> (
    FFTLIB_kernelHandle         handle,
    const FFTLIB_bufParams1D_t *bufParamsX,
    const FFTLIB_bufParams1D_t *bufParamsW,
    const FFTLIB_bufParams1D_t *bufParamsY,
    const FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_InitArgs
        *pKerInitArgs);

/*******************************************************************************
 *
 * EXECUTION FUNCTIONS
 *
 ******************************************************************************/

template <typename dataType, uint32_t MMA_SIZE>
FFTLIB_STATUS FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_exec_ci (
    FFTLIB_kernelHandle handle,
    void *restrict pX,
    const void *restrict pW,
    void *restrict pY)
{
   FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_PrivArgs *pKerPrivArgs =
       (FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_PrivArgs *) handle;
   uint32_t         fftSize    = pKerPrivArgs->initArgs.fftSize;
   uint32_t         batchSize  = pKerPrivArgs->initArgs.batchSize;
   uint32_t         interleave = pKerPrivArgs->initArgs.interleave;
   uint8_t         *pBlock     = pKerPrivArgs->bufPblock;
   int32_t          loopCount;
   uint32_t         dftSizeInStage;
   uint32_t         decompositionFactorStage0;
   __SE_TEMPLATE_v1 seParamsDataLogNStage0;
#if 0
   __SA_TEMPLATE_v1 saParamsTwDftStage;
#else
   __SE_TEMPLATE_v1 seParamsTwDftStage;
#endif
   __SE_TEMPLATE_v1      seParamsDataDftStage;
   __SA_TEMPLATE_v1      saParamsOutDftStage;
   __SA_TEMPLATE_v1      saParamsOutDftStage2;
   __SA_TEMPLATE_v1      saParamsTw1LogNStage0;
   __SE_TEMPLATE_v1      seParamsTw2LogNStage0;
   __SA_TEMPLATE_v1      saParamsOutLogNStage0;
   __SE_TEMPLATE_v1      seParamsDataLogNStage1;
   __SA_TEMPLATE_v1      saParamsTw1LogNStage1;
   __SE_TEMPLATE_v1      seParamsTw2LogNStage1;
   __SA_TEMPLATE_v1      saParamsOutLogNStage1;
   __SE_TEMPLATE_v1      seParamsDataInt;
   __SA_TEMPLATE_v1      saParamsOutInt;
   __SE_TEMPLATE_v1      seParamsDataDeint;
   __SA_TEMPLATE_v1      saParamsOutDeint;
   FFTLIB_MMA_CONFIG_REG mmaConfigLogNStages;
   __HWA_OFFSET_REG      mmaOffsetLogNStages;
   FFTLIB_MMA_CONFIG_REG mmaConfigDftStage;
   __HWA_OFFSET_REG      mmaOffsetDftStage;
   dataType             *pSrc, *pDst;
   const dataType       *pW1LogNStage, *pW2LogNStage, *pWDftStage;
   uchar64               permCtrlInt;
   uchar64               permCtrl64Pt;
   uchar64               permCtrlDeint;
   uint32_t              k;

   pSrc = (dataType *) pX;
   pDst = (dataType *) pY;

   /**************************************************************************/
   /* Interleaving step                                                    */
   /**************************************************************************/
   if (interleave == 0) {
      uchar64 inRe, inIm;
      uchar64 in0, in1;

      seParamsDataInt = *(
          (__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_INT_DATA_PARAM_OFFSET));
      saParamsOutInt = *(
          (__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA_INT_OUT_PARAM_OFFSET));
      permCtrlInt = *((uchar64*) ((uint8_t *) pBlock + PERMCTRLINT_OFFSET));

      __SE0_OPEN ((void *) pSrc, seParamsDataInt);
      __SA0_OPEN (saParamsOutInt);

      _nassert (fftSize * batchSize >= MMA_SIZE);
      for (k = 0; k < fftSize * batchSize; k += MMA_SIZE) {
         inRe = __SE0ADV (uchar64);
         inIm = __SE0ADV (uchar64);
         in0  = __permute_low_low (permCtrlInt, inIm, inRe);
         in1  = __permute_high_high (permCtrlInt, inIm, inRe);

         *__SA0ADV (uchar64, (char *) pDst) = in0;
         *__SA0ADV (uchar64, (char *) pDst) = in1;
      }

      __SE0_CLOSE ();
      __SA0_CLOSE ();

      pSrc = (dataType *) pY;
      pDst = (dataType *) pX;
   }

   /* The FFT algorithm works by decomposing to smaller size DFT's stage
    * after stage.
    *    In all but the first stage, the decomposition factor is MMA_SIZE/2.
    * This decomposition factor is chosen so that each row of the decomposition
    * matrix would fit one full vector length. The first stage would also use
    * a decomposition factor of MMA_SIZE/2 if fftSize is a power of MMA_SIZE/2.
    * In case fftSize is not a power of MMA_SIZE/2, the decomposition factor for
    * the first stage is less than MMA_SIZE/2.                                */

   /**************************************************************************/
   /* Log N Stage 0                                                          */
   /**************************************************************************/
   {
      pW1LogNStage = (const dataType *) pKerPrivArgs->pW;
      pW2LogNStage = (const dataType *) pW;
      permCtrlInt  = *((uchar64*) ((uint8_t *) pBlock + PERMCTRLINT_OFFSET));
      permCtrl64Pt =
          *((uchar64*) ((uint8_t *) pBlock + PERMCTRL64PT_OFFSET));
      dftSizeInStage = fftSize;

      seParamsDataLogNStage0 =
          *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                                  SE_LOGNSTAGE0_DATA_PARAM_OFFSET));
      __SE0_OPEN ((void *) pSrc, seParamsDataLogNStage0);

      seParamsTw2LogNStage0 =
          *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                                  SE_LOGNSTAGE0_TW2_PARAM_OFFSET));
      __SE1_OPEN ((const void *) pW2LogNStage, seParamsTw2LogNStage0);

      decompositionFactorStage0 = *(
          (uint32_t *) ((uint8_t *) pBlock + DECOMPOSITIONFACTORSTAGE0_OFFSET));

      mmaConfigLogNStages =
          *((FFTLIB_MMA_CONFIG_REG *) ((uint8_t *) pBlock +
                                       MMA_LOGNSTAGE0_CONFIGREG_OFFSET));
      mmaOffsetLogNStages =
          *((__HWA_OFFSET_REG *) ((uint8_t *) pBlock +
                                  MMA_LOGNSTAGE0_OFFSETREG_OFFSET));
      __HWAOPEN (mmaConfigLogNStages, mmaOffsetLogNStages,
                 __MMA_OPEN_FSM_RESET);
      __HWAADV ();

      saParamsTw1LogNStage0 =
          *((__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                                  SA_LOGNSTAGE0_TW1_PARAM_OFFSET));
      __SA0_OPEN (saParamsTw1LogNStage0);
      saParamsOutLogNStage0 =
          *((__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                                  SA_LOGNSTAGE0_OUT_PARAM_OFFSET));
      __SA1_OPEN (saParamsOutLogNStage0);

      FFTLIB_UTIL_SE0loadVCrotBload<dataType> (MMA_SIZE / 2);

      {
         FFTLIB_UTIL_SA0AloadSE0loadVCrotBloadComputeC<dataType> (
             4, (FFTLIB_const uint8_t *) pW1LogNStage);

         if (batchSize * fftSize * 16 == MMA_SIZE * MMA_SIZE) {
            /* This case is not relevant for 32-bit data type */
            /* 16-bit resolution: FFT_SIZE*BATCH_SIZE == 64 */
            FFTLIB_UTIL_SA0AloadComputeCSE1MultSA1RegStore<dataType> (
                MMA_SIZE / 8, (FFTLIB_const uint8_t *) pW1LogNStage,
                (uint8_t *) pDst);
         }
         else if (batchSize * fftSize * 8 == MMA_SIZE * MMA_SIZE) {
            /* 16-bit resolution: FFT_SIZE*BATCH_SIZE == 128 */
            FFTLIB_UTIL_SA0AloadComputeCSE1MultSA1RegStore<dataType> (
                MMA_SIZE / 4, (FFTLIB_const uint8_t *) pW1LogNStage,
                (uint8_t *) pDst);
         }
         else if (batchSize * fftSize * 16 == 3 * MMA_SIZE * MMA_SIZE) {
            /* 16-bit resolution: FFT_SIZE*BATCH_SIZE == 192 */
            FFTLIB_UTIL_SA0AloadComputeCSE1MultSA1RegStore<dataType> (
                (3 * MMA_SIZE) / 8, (FFTLIB_const uint8_t *) pW1LogNStage,
                (uint8_t *) pDst);
         }
         else if (batchSize * fftSize * 4 == MMA_SIZE * MMA_SIZE) {
            /* 16-bit resolution: FFT_SIZE*BATCH_SIZE == 256 */
            FFTLIB_UTIL_SA0AloadComputeCSE1MultSA1RegStore<dataType> (
                MMA_SIZE / 2, (FFTLIB_const uint8_t *) pW1LogNStage,
                (uint8_t *) pDst);
         }
         else if (batchSize * fftSize * 16 == 5 * MMA_SIZE * MMA_SIZE) {
            /* 16-bit resolution: FFT_SIZE*BATCH_SIZE == 320 */
            FFTLIB_UTIL_SA0AloadSE0loadVCrotBloadComputeCSE1MultSA1RegStore<
                dataType> ((5 * MMA_SIZE) / 8,
                           (FFTLIB_const uint8_t *) pW1LogNStage,
                           (uint8_t *) pDst);
         }
         else if (batchSize * fftSize * 8 == MMA_SIZE * MMA_SIZE * 3) {
            /* 16-bit resolution: FFT_SIZE*BATCH_SIZE == 384 */
            FFTLIB_UTIL_SA0AloadSE0loadVCrotBloadComputeCSE1MultSA1RegStore<
                dataType> ((MMA_SIZE * 3) / 4,
                           (FFTLIB_const uint8_t *) pW1LogNStage,
                           (uint8_t *) pDst);
         }
         else if (batchSize * fftSize * 16 == 7 * MMA_SIZE * MMA_SIZE) {
            /* 16-bit resolution: FFT_SIZE*BATCH_SIZE == 448 */
            FFTLIB_UTIL_SA0AloadSE0loadVCrotBloadComputeCSE1MultSA1RegStore<
                dataType> ((7 * MMA_SIZE) / 8,
                           (FFTLIB_const uint8_t *) pW1LogNStage,
                           (uint8_t *) pDst);
         }
         else if (batchSize * fftSize * 2 == MMA_SIZE * MMA_SIZE) {
            /* 16-bit resolution: FFT_SIZE*BATCH_SIZE == 512 */
            FFTLIB_UTIL_SA0AloadSE0loadVCrotBloadComputeCSE1MultSA1RegStore<
                dataType> (MMA_SIZE, (FFTLIB_const uint8_t *) pW1LogNStage,
                           (uint8_t *) pDst);
         }
         else {
            loopCount = (batchSize * fftSize * 2) / MMA_SIZE;
            _nassert (loopCount > 0);
            /* TODO OPT: How to convey loopCount > MMA_SIZE/2 */
            FFTLIB_UTIL_SA0AloadSE0loadVCrotBloadComputeCSE1MultSA1RegStore<
                dataType> (loopCount, (FFTLIB_const uint8_t *) pW1LogNStage,
                           (uint8_t *) pDst);
         }
      }

      __SE0_CLOSE ();
      __SE1_CLOSE ();
      __SA1_CLOSE ();
      __SA0_CLOSE ();

      pSrc = (interleave == 0) ? (dataType *) pX : (dataType *) pY;
      pDst = (interleave == 0) ? (dataType *) pY : (dataType *) pX;
      pW2LogNStage += dftSizeInStage * 2;
      dftSizeInStage /= decompositionFactorStage0;
      pW1LogNStage += MMA_SIZE * MMA_SIZE / 2;
   }

   if (dftSizeInStage <= MMA_SIZE / 2) {
      /* We want output of DFT stage to be in pY buffer. When there is only
       * one logNStage, we use the same source and destination buffer for DFT
       * stage.
       *    In general, we need to use different source and destination buffers
       * if we write data out of sequence compared to the data in read buffer
       * because if we use the same source and destination buffers in this case,
       * we would overwrite data before it is read.  When there is only one
       * logNStage, we process data sequentially except for within a
       * MMA_SIZE*MMA_SIZE/2 block. SE anyway prefetches more than the size of
       * a block into its tile memory, so that there is no danger of
       * overwriting the data before it is read.                              */
      pDst = (interleave == 0) ? (dataType *) pX : (dataType *) pY;
   }
   else {

      /**************************************************************************/
      /* Log N Stage 1 */
      /**************************************************************************/
      {
         seParamsDataLogNStage1 =
             *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                                     SE_LOGNSTAGE1_DATA_PARAM_OFFSET));
         __SE0_OPEN ((void *) pSrc, seParamsDataLogNStage1);

         seParamsTw2LogNStage1 =
             *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                                     SE_LOGNSTAGE1_TW2_PARAM_OFFSET));
         __SE1_OPEN ((const void *) pW2LogNStage, seParamsTw2LogNStage1);

         saParamsTw1LogNStage1 =
             *((__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                                     SA_LOGNSTAGE1_TW1_PARAM_OFFSET));
         __SA0_OPEN (saParamsTw1LogNStage1);

         saParamsOutLogNStage1 =
             *((__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                                     SA_LOGNSTAGE1_OUT_PARAM_OFFSET));
         __SA1_OPEN (saParamsOutLogNStage1);

         mmaConfigLogNStages =
             *((FFTLIB_MMA_CONFIG_REG *) ((uint8_t *) pBlock +
                                          MMA_LOGNSTAGE1_CONFIGREG_OFFSET));
         mmaOffsetLogNStages =
             *((__HWA_OFFSET_REG *) ((uint8_t *) pBlock +
                                     MMA_LOGNSTAGE1_OFFSETREG_OFFSET));
         __HWAOPEN (mmaConfigLogNStages, mmaOffsetLogNStages,
                    __MMA_OPEN_FSM_RESET);
         __HWAADV ();

         FFTLIB_UTIL_SE0loadVCrotBload<dataType> (MMA_SIZE / 2);

         FFTLIB_UTIL_SA0AloadSE0loadVCrotBloadComputeC<dataType> (
             4, (FFTLIB_const uint8_t *) pW1LogNStage);

         /* We split the two cases below so that compiler does not use a
          * suboptimal case for the first case (seen with 32-bit data type) */
         if (fftSize * batchSize * 2 == MMA_SIZE * MMA_SIZE) {
            FFTLIB_UTIL_SA0AloadSE0loadVCrotBloadComputeCSE1MultSA1RegStore<
                dataType> (MMA_SIZE, (FFTLIB_const uint8_t *) pW1LogNStage,
                           (uint8_t *) pDst);
         }
         else {
            loopCount = (batchSize * fftSize * 2) / MMA_SIZE;
            _nassert (loopCount > 0);
            /* TODO OPT: How to convey loopCount > MMA_SIZE/2 */
            FFTLIB_UTIL_SA0AloadSE0loadVCrotBloadComputeCSE1MultSA1RegStore<
                dataType> (loopCount, (FFTLIB_const uint8_t *) pW1LogNStage,
                           (uint8_t *) pDst);
         }

         __SE0_CLOSE ();
         __SE1_CLOSE ();
         __SA1_CLOSE ();
         __SA0_CLOSE ();

         pSrc = (interleave == 0) ? (dataType *) pY : (dataType *) pX;
         pDst = (interleave == 0) ? (dataType *) pX : (dataType *) pY;
         pW2LogNStage += dftSizeInStage * 2;
         dftSizeInStage /= MMA_SIZE / 2;
      }

      /**************************************************************************/
      /* Log N Stage 2 */
      /**************************************************************************/
      if (dftSizeInStage > MMA_SIZE / 2) {

         if (dftSizeInStage * 4 <= MMA_SIZE * MMA_SIZE) {
            /* We want the output of DFT stage to be in pY buffer. To facilitate
             * that we keep the source and destination buffers the same (pX) in
             * this stage. In general, we need to use different source and
             * destination buffers if we write data out of sequence compared to
             * the data in read buffer because if we use the same source and
             * destination buffers in this case, we would overwrite data before
             * it is read.  For this stage, we process the data sequentially
             * except for within a MMA_SIZE*MMA_SIZE/4 block. SE anyway
             * prefetches more than the size of a block into its tile memory, so
             * that there is no danger of overwriting the data before it is
             * read. */
            pDst = (interleave == 0) ? (dataType *) pY : (dataType *) pX;
         }

         seParamsDataLogNStage1 =
             *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                                     SE_LOGNSTAGE2_DATA_PARAM_OFFSET));
         __SE0_OPEN ((void *) pSrc, seParamsDataLogNStage1);

         seParamsTw2LogNStage1 =
             *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                                     SE_LOGNSTAGE2_TW2_PARAM_OFFSET));
         __SE1_OPEN ((const void *) pW2LogNStage, seParamsTw2LogNStage1);

         saParamsTw1LogNStage1 =
             *((__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                                     SA_LOGNSTAGE2_TW1_PARAM_OFFSET));
         __SA0_OPEN (saParamsTw1LogNStage1);

         saParamsOutLogNStage1 =
             *((__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                                     SA_LOGNSTAGE2_OUT_PARAM_OFFSET));
         __SA1_OPEN (saParamsOutLogNStage1);

         mmaConfigLogNStages =
             *((FFTLIB_MMA_CONFIG_REG *) ((uint8_t *) pBlock +
                                          MMA_LOGNSTAGE2_CONFIGREG_OFFSET));
         mmaOffsetLogNStages =
             *((__HWA_OFFSET_REG *) ((uint8_t *) pBlock +
                                     MMA_LOGNSTAGE2_OFFSETREG_OFFSET));
         __HWAOPEN (mmaConfigLogNStages, mmaOffsetLogNStages,
                    __MMA_OPEN_FSM_RESET);
         __HWAADV ();

         FFTLIB_UTIL_SE0loadVCrotBload<dataType> (MMA_SIZE / 2);

         FFTLIB_UTIL_SA0AloadSE0loadVCrotBloadComputeC<dataType> (
             4, (FFTLIB_const uint8_t *) pW1LogNStage);

         loopCount = (batchSize * fftSize * 2) / MMA_SIZE;
         _nassert (loopCount > 0);
         /* TODO OPT: How to convey loopCount > MMA_SIZE/2 */
         FFTLIB_UTIL_SA0AloadSE0loadVCrotBloadComputeCSE1MultSA1RegStore<
             dataType> (loopCount, (FFTLIB_const uint8_t *) pW1LogNStage,
                        (uint8_t *) pDst);

         __SE0_CLOSE ();
         __SE1_CLOSE ();
         __SA1_CLOSE ();
         __SA0_CLOSE ();

         /* pSrc and pDst are set here assuming this is the last Log N stage.
          * If there is Log N Stage 3, they are changed within that code      */
         pSrc = (interleave == 0) ? (dataType *) pY : (dataType *) pX;
         pDst = (interleave == 0) ? (dataType *) pX : (dataType *) pY;

         pW2LogNStage += dftSizeInStage * 2;
         dftSizeInStage /= MMA_SIZE / 2;

         /**************************************************************************/
         /* Log N Stage 3 */
         /**************************************************************************/
         if (dftSizeInStage > MMA_SIZE / 2) {

            pSrc = (interleave == 0) ? (dataType *) pX : (dataType *) pY;
            pDst = (interleave == 0) ? (dataType *) pY : (dataType *) pX;

            seParamsDataLogNStage1 =
                *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                                        SE_LOGNSTAGE3_DATA_PARAM_OFFSET));
            __SE0_OPEN ((void *) pSrc, seParamsDataLogNStage1);

            seParamsTw2LogNStage1 =
                *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                                        SE_LOGNSTAGE3_TW2_PARAM_OFFSET));
            __SE1_OPEN ((const void *) pW2LogNStage, seParamsTw2LogNStage1);

            saParamsTw1LogNStage1 =
                *((__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                                        SA_LOGNSTAGE3_TW1_PARAM_OFFSET));
            __SA0_OPEN (saParamsTw1LogNStage1);

            saParamsOutLogNStage1 =
                *((__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                                        SA_LOGNSTAGE3_OUT_PARAM_OFFSET));
            __SA1_OPEN (saParamsOutLogNStage1);

            mmaConfigLogNStages =
                *((FFTLIB_MMA_CONFIG_REG *) ((uint8_t *) pBlock +
                                             MMA_LOGNSTAGE3_CONFIGREG_OFFSET));
            mmaOffsetLogNStages =
                *((__HWA_OFFSET_REG *) ((uint8_t *) pBlock +
                                        MMA_LOGNSTAGE3_OFFSETREG_OFFSET));
            __HWAOPEN (mmaConfigLogNStages, mmaOffsetLogNStages,
                       __MMA_OPEN_FSM_RESET);
            __HWAADV ();

            FFTLIB_UTIL_SE0loadVCrotBload<dataType> (MMA_SIZE / 2);

            FFTLIB_UTIL_SA0AloadSE0loadVCrotBloadComputeC<dataType> (
                4, (FFTLIB_const uint8_t *) pW1LogNStage);

            loopCount = (batchSize * fftSize * 2) / MMA_SIZE;
            _nassert (loopCount > 0);
            /* TODO OPT: How to convey loopCount > MMA_SIZE/2 */
            FFTLIB_UTIL_SA0AloadSE0loadVCrotBloadComputeCSE1MultSA1RegStore<
                dataType> (loopCount, (FFTLIB_const uint8_t *) pW1LogNStage,
                           (uint8_t *) pDst);

            __SE0_CLOSE ();
            __SE1_CLOSE ();
            __SA1_CLOSE ();
            __SA0_CLOSE ();

            pSrc = (interleave == 0) ? (dataType *) pY : (dataType *) pX;
            pDst = (interleave == 0) ? (dataType *) pX : (dataType *) pY;
            pW2LogNStage += dftSizeInStage * 2;
         }
      }
   }

   /**************************************************************************/
   /* DFT stage                                                              */
   /**************************************************************************/
   {
      seParamsDataDftStage =
          *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                                  SE_DFTSTAGE_DATA_PARAM_OFFSET));
      __SE0_OPEN ((void *) pSrc, seParamsDataDftStage);

      mmaConfigDftStage =
          *((FFTLIB_MMA_CONFIG_REG *) ((uint8_t *) pBlock +
                                       MMA_DFTSTAGE_CONFIGREG_OFFSET));
      mmaOffsetDftStage =
          *((__HWA_OFFSET_REG *) ((uint8_t *) pBlock +
                                  MMA_DFTSTAGE_OFFSETREG_OFFSET));
      __HWAOPEN (mmaConfigDftStage, mmaOffsetDftStage, __MMA_OPEN_FSM_RESET);
      __HWAADV ();

#if 0
      pWDftStage = pW1LogNStage + MMA_SIZE*MMA_SIZE/2;
      saParamsTwDftStage = *((__SA_TEMPLATE_v1*)((uint8_t *)pBlock + SA_DFTSTAGE_TW_PARAM_OFFSET));
      __SA0_OPEN(saParamsTwDftStage);
#else
      pWDftStage = pW2LogNStage;
      seParamsTwDftStage =
          *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                                  SE_DFTSTAGE_TW_PARAM_OFFSET));
      __SE1_OPEN ((const void *) pWDftStage, seParamsTwDftStage);
#endif

      FFTLIB_UTIL_SE0Bload (MMA_SIZE);

      saParamsOutDftStage =
          *((__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                                  SA_DFTSTAGE_OUT_PARAM_OFFSET));
      __SA1_OPEN (saParamsOutDftStage);
      saParamsOutDftStage2 =
          *((__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                                  SA_DFTSTAGE2_OUT_PARAM_OFFSET));
      __SA2_OPEN (saParamsOutDftStage2);

#if 0
      FFTLIB_UTIL_SA0AloadSE0BloadComputeC(4,
                                           (uint8_t *) pWDftStage
#else
      FFTLIB_UTIL_SE1AloadSE0BloadComputeC (4
#endif
                                           );

                                           if (fftSize * 16 ==
                                               MMA_SIZE * MMA_SIZE) {
                                              /* This case is not relevant for
                                               * 32-bit data type */
                                              if (batchSize < 8) {
#if 0
            FFTLIB_UTIL_SA0AloadComputeCPermLlhhPermLlhhMvLlhhSA2Store(MMA_SIZE,
                                                                       (uint8_t *) pWDftStage,
#else
            FFTLIB_UTIL_SE1AloadComputeCPermLlhhPermLlhhMvLlhhSA2Store (
                MMA_SIZE,
#endif
                                                                       (uint8_t *)pDst,
                                                                       permCtrlInt,
                                                                       permCtrl64Pt);
                                              }
                                              else if (batchSize == 8) {
#if 0
            FFTLIB_UTIL_SA0AloadComputeCPermLlhhPermLlhhMvLlhhSA1Store(MMA_SIZE,
                                                                       (uint8_t *) pWDftStage,
#else
            FFTLIB_UTIL_SE1AloadComputeCPermLlhhPermLlhhMvLlhhSA1Store (
                MMA_SIZE,
#endif
                                                                       (uint8_t *)pDst,
                                                                       permCtrlInt,
                                                                       permCtrl64Pt);
                                              }
                                              else {
                                                 /* We process nearest lower
                                                  * multiple of 8 number of
                                                  * channels in this loop */
                                                 loopCount =
                                                     (((batchSize / 8) * 8) *
                                                      fftSize * 2) /
                                                     MMA_SIZE;
                                                 _nassert (loopCount > 0);
#if 0
            FFTLIB_UTIL_SA0AloadSE0BloadComputeCPermLlhhPermLlhhMvLlhhSA1Store(loopCount,
                                                                               (uint8_t *) pWDftStage,
#else
            FFTLIB_UTIL_SE1AloadSE0BloadComputeCPermLlhhPermLlhhMvLlhhSA1Store (
                loopCount,
#endif
                                                                               (uint8_t *)pDst,
                                                                               permCtrlInt,
                                                                               permCtrl64Pt);
                                                                               pDst +=
                                                                                   loopCount *
                                                                                   MMA_SIZE;

                                                                               if (batchSize &
                                                                                   7u) {
                                                                                  /* If the number of channels is not a multiple of 8, we process the
                                                                                   * remainder channels here                                        */
#if 0
               FFTLIB_UTIL_SA0AloadComputeCPermLlhhPermLlhhMvLlhhSA2Store(MMA_SIZE,
                                                                          (uint8_t *) pWDftStage,
#else
               FFTLIB_UTIL_SE1AloadComputeCPermLlhhPermLlhhMvLlhhSA2Store (
                   MMA_SIZE,
#endif
                                                                          (uint8_t *)pDst,
                                                                          permCtrlInt,
                                                                          permCtrl64Pt);
                                                                               }
                                              }
                                           }
                                           else if (fftSize * 8 ==
                                                    MMA_SIZE * MMA_SIZE) {
                                              if (batchSize < 4) {
#if 0
            FFTLIB_UTIL_SA0AloadComputeCPermLlMvLlMvHhSA2Store(MMA_SIZE,
                                                               (uint8_t *) pWDftStage,
#else
            FFTLIB_UTIL_SE1AloadComputeCPermLlMvLlMvHhSA2Store (
                MMA_SIZE,
#endif
                                                               (uint8_t *)pDst,
                                                               permCtrlInt);
                                              }
                                              else if (batchSize == 4) {
#if 0
            FFTLIB_UTIL_SA0AloadComputeCPermLlMvLlMvHhSA1Store(MMA_SIZE,
                                                               (uint8_t *) pWDftStage,
#else
            FFTLIB_UTIL_SE1AloadComputeCPermLlMvLlMvHhSA1Store (
                MMA_SIZE,
#endif
                                                               (uint8_t *)pDst,
                                                               permCtrlInt);
                                              }
                                              else {
                                                 /* We process nearest lower
                                                  * multiple of 4 number of
                                                  * channels in this loop */
                                                 loopCount =
                                                     (((batchSize / 4) * 4) *
                                                      fftSize * 2) /
                                                     MMA_SIZE;
                                                 _nassert (loopCount > 0);
#if 0
            FFTLIB_UTIL_SA0AloadSE0BloadComputeCPermLlMvLlMvHhSA1Store(loopCount,
                                                                       (uint8_t *) pWDftStage,
#else
            FFTLIB_UTIL_SE1AloadSE0BloadComputeCPermLlMvLlMvHhSA1Store (
                loopCount,
#endif
                                                                       (uint8_t *)pDst,
                                                                       permCtrlInt);
                                                                       pDst +=
                                                                           loopCount *
                                                                           MMA_SIZE;

                                                                       if (batchSize &
                                                                           3u) {
                                                                          /* If
                                                                           * the
                                                                           * number
                                                                           * of
                                                                           * channels
                                                                           * is
                                                                           * odd,
                                                                           * we
                                                                           * process
                                                                           * the
                                                                           * last
                                                                           * channel
                                                                           * here*/
#if 0
               FFTLIB_UTIL_SA0AloadComputeCPermLlMvLlMvHhSA2Store(MMA_SIZE,
                                                                  (uint8_t *) pWDftStage,
#else
               FFTLIB_UTIL_SE1AloadComputeCPermLlMvLlMvHhSA2Store (
                   MMA_SIZE,
#endif
                                                                  (uint8_t *)pDst,
                                                                  permCtrlInt);
                                                                       }
                                              }
                                           }
                                           else if (fftSize * 4 ==
                                                    MMA_SIZE * MMA_SIZE) {
                                              if (batchSize < 2) {
#if 0
            FFTLIB_UTIL_SA0AloadComputeCPermLlHhSA2Store(MMA_SIZE,
                                                         (uint8_t *) pWDftStage,
#else
            FFTLIB_UTIL_SE1AloadComputeCPermLlHhSA2Store (MMA_SIZE,
#endif
                                                         (uint8_t *)pDst,
                                                         permCtrlInt);
                                              }
                                              else if (batchSize == 2) {
#if 0
            FFTLIB_UTIL_SA0AloadComputeCPermLlHhSA1Store(MMA_SIZE,
                                                         (uint8_t *) pWDftStage,
#else
            FFTLIB_UTIL_SE1AloadComputeCPermLlHhSA1Store (MMA_SIZE,
#endif
                                                         (uint8_t *)pDst,
                                                         permCtrlInt);
                                              }
                                              else {
                                                 /* We process nearest lower
                                                  * multiple of 2 number of
                                                  * channels in this loop */
                                                 loopCount =
                                                     (((batchSize / 2) * 2) *
                                                      fftSize * 2) /
                                                     MMA_SIZE;

                                                 /* We split the three cases
                                                  * below so that compiler does
                                                  * not use a suboptimal case
                                                  * for the first two cases
                                                  * (seen with 32-bit data
                                                  * type) */
                                                 if (loopCount == MMA_SIZE) {
#if 0
               FFTLIB_UTIL_SA0AloadSE0BloadComputeCPermLlHhSA1Store(MMA_SIZE,
                                                                    (uint8_t *) pWDftStage,
#else
               FFTLIB_UTIL_SE1AloadSE0BloadComputeCPermLlHhSA1Store (
                   MMA_SIZE,
#endif

                                                                    (uint8_t *)pDst,
                                                                    permCtrlInt);
                                                 }
                                                 else if (loopCount ==
                                                          MMA_SIZE * 2) {
#if 0
               FFTLIB_UTIL_SA0AloadSE0BloadComputeCPermLlHhSA1Store(MMA_SIZE*2,
                                                                    (uint8_t *) pWDftStage,
#else
               FFTLIB_UTIL_SE1AloadSE0BloadComputeCPermLlHhSA1Store (
                   MMA_SIZE * 2,
#endif
                                                                    (uint8_t *)pDst,
                                                                    permCtrlInt);
                                                 }
                                                 else {
                                                    _nassert (loopCount > 0);
#if 0
               FFTLIB_UTIL_SA0AloadSE0BloadComputeCPermLlHhSA1Store(loopCount,
                                                                    (uint8_t *) pWDftStage,
#else
               FFTLIB_UTIL_SE1AloadSE0BloadComputeCPermLlHhSA1Store (
                   loopCount,
#endif
                                                                    (uint8_t *)pDst,
                                                                    permCtrlInt);
                                                 }
                                                 pDst += loopCount * MMA_SIZE;

                                                 if (batchSize & 1u) {
                                                    /* If the number of channels
                                                     * is odd, we process the
                                                     * last channel here*/
#if 0
               FFTLIB_UTIL_SA0AloadComputeCPermLlHhSA2Store(MMA_SIZE,
                                                            (uint8_t *) pWDftStage,
#else
               FFTLIB_UTIL_SE1AloadComputeCPermLlHhSA2Store (MMA_SIZE,
#endif
                                                            (uint8_t *)pDst,
                                                            permCtrlInt);
                                                 }
                                              }
                                           }
                                           else if (fftSize * 2 ==
                                                    MMA_SIZE * MMA_SIZE) {
                                              /* 16-bit resolution: FFT_SIZE ==
                                               * 512 points */
                                              if (batchSize == 1) {
#if 0
            FFTLIB_UTIL_SA0AloadComputeCPermLlHhSA1Store(MMA_SIZE,
                                                         (uint8_t *) pWDftStage,
#else
            FFTLIB_UTIL_SE1AloadComputeCPermLlHhSA1Store (MMA_SIZE,
#endif
                                                         (uint8_t *)pDst,
                                                         permCtrlInt);
                                              }
                                              else if (batchSize == 2) {
                                                 /* We added this extra case so
                                                  * that compiler does not
                                                  * handle it
                                                  * suboptimally (seen with
                                                  * 32-bit data type) */
#if 0
            FFTLIB_UTIL_SA0AloadSE0BloadComputeCPermLlHhSA1Store(MMA_SIZE*2,
                                                                 (uint8_t *) pWDftStage,
#else
            FFTLIB_UTIL_SE1AloadSE0BloadComputeCPermLlHhSA1Store (
                MMA_SIZE * 2,
#endif
                                                                 (uint8_t *)pDst,
                                                                 permCtrlInt);
                                              }
                                              else {
                                                 loopCount =
                                                     (batchSize * fftSize * 2) /
                                                     MMA_SIZE;
                                                 _nassert (loopCount > 0);
#if 0
            FFTLIB_UTIL_SA0AloadSE0BloadComputeCPermLlHhSA1Store(loopCount,
                                                                 (uint8_t *) pWDftStage,
#else
            FFTLIB_UTIL_SE1AloadSE0BloadComputeCPermLlHhSA1Store (
                loopCount,
#endif
                                                                 (uint8_t *)pDst,
                                                                 permCtrlInt);
                                              }
                                           }
                                           else {
                                              if (fftSize * batchSize ==
                                                  MMA_SIZE * MMA_SIZE) {
                                                 /* We added this extra case so
                                                  * that compiler does not
                                                  * handle it
                                                  * suboptimally (seen with
                                                  * 32-bit data type) */
#if 0
            FFTLIB_UTIL_SA0AloadSE0BloadComputeCPermLlHhSA1Store(MMA_SIZE*2,
                                                                 (uint8_t *) pWDftStage,
#else
            FFTLIB_UTIL_SE1AloadSE0BloadComputeCPermLlHhSA1Store (
                MMA_SIZE * 2,
#endif
                                                                 (uint8_t *)pDst,
                                                                 permCtrlInt);
                                              }
                                              else {
                                                 loopCount =
                                                     (batchSize * fftSize * 2) /
                                                     MMA_SIZE;
                                                 _nassert (loopCount > 0);
                                                 /* TODO OPT: How to convey
                                                  * loopCount > MMA_SIZE/2 */
#if 0
            FFTLIB_UTIL_SA0AloadSE0BloadComputeCPermLlHhSA1Store(loopCount,
                                                                 (uint8_t *) pWDftStage,
#else
            FFTLIB_UTIL_SE1AloadSE0BloadComputeCPermLlHhSA1Store (
                loopCount,
#endif
                                                                 (uint8_t *)pDst,
                                                                 permCtrlInt);
                                              }
                                           }
                                           __SE0_CLOSE ();
#if 0
      __SA0_CLOSE();
#else
      __SE1_CLOSE ();
#endif
                                           __SA1_CLOSE ();
                                           __SA2_CLOSE ();

                                           pSrc = (dataType *) pX;
                                           pDst = (dataType *) pY;
   }

   /**************************************************************************/
   /* Deinterleaving step                                                    */
   /**************************************************************************/
   if (interleave == 0) {
      uchar64 out0, out1;
      uchar64 outRe, outIm;

      seParamsDataDeint = *((__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                                                  SE_DEINT_DATA_PARAM_OFFSET));
      saParamsOutDeint  = *((__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock +
                                                 SA_DEINT_OUT_PARAM_OFFSET));
      permCtrlDeint =
          *((uchar64*) ((uint8_t *) pBlock + PERMCTRLDEINT_OFFSET));

      __SE0_OPEN ((void *) pSrc, seParamsDataDeint);
      __SA0_OPEN (saParamsOutDeint);

      _nassert (fftSize * batchSize >= MMA_SIZE);
      for (k = 0; k < fftSize * batchSize; k += MMA_SIZE) {
         out0 = __SE0ADV (uchar64);
         out1 = __SE0ADV (uchar64);
         outRe =
             c7x_permute_even_even_xxx<dataType> (permCtrlDeint, out1, out0);
         outIm = c7x_permute_odd_odd_xxx<dataType> (permCtrlDeint, out1, out0);

         *__SA0ADV (uchar64, (char *) pDst) = outRe;
         *__SA0ADV (uchar64, (char *) pDst) = outIm;
      }

      __SE0_CLOSE ();
      __SA0_CLOSE ();
   }

   return FFTLIB_SUCCESS;
}

template FFTLIB_STATUS
FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_exec_ci<int16_t,
                                                       FFTLIB_MMA_SIZE_16_BIT> (
    FFTLIB_kernelHandle handle,
    void *restrict pX,
    const void *restrict pW,
    void *restrict pY);

template FFTLIB_STATUS
FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_exec_ci<int32_t,
                                                       FFTLIB_MMA_SIZE_32_BIT> (
    FFTLIB_kernelHandle handle,
    void *restrict pX,
    const void *restrict pW,
    void *restrict pY);
