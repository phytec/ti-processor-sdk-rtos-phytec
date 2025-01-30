/******************************************************************************
 * Copyright (C) 2023 Texas Instruments Incorporated - http://www.ti.com/
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

/**********************************************************************************************************************/
/*                                                                                                                    */
/* INCLUDES                                                                                                           */
/*                                                                                                                    */
/**********************************************************************************************************************/

#include "../common/VXLIB_test.h"
#include "TI_memory.h"
#include "TI_test.h"
#include "VXLIB_minMaxLoc.h"
#include "VXLIB_minMaxLoc_idat.h"
#include "VXLIB_types.h"
#include <vxlib.h>

/**********************************************************************************************************************/
/*                                                                                                                    */
/* Global variables                                                                                                   */
/*                                                                                                                    */
/**********************************************************************************************************************/

#ifdef WIN32
int8_t msmcBuffer[VXLIB_L3_RESULTS_BUFFER_SIZE];
int8_t ddrBuffer[2048 * 1024];
#else
__attribute__((section(".msmcData"), aligned(64))) int8_t msmcBuffer[VXLIB_L3_RESULTS_BUFFER_SIZE];
__attribute__((section(".ddrData"), aligned(64))) int8_t  ddrBuffer[2048 * 1024];

#endif // WIN32

int16_t volatile volatileSum = 0; // use volatile to keep compiler from removing this operation

inline void
TI_fill_minMaxVal(VXLIB_minMaxLoc_testParams_t currPrm, void *pMinVal, void *pMinValCn, void *pMaxVal, void *pMaxValCn)
{

   if (currPrm.dType == VXLIB_UINT8) {
      uint8_t  minValFill = (uint8_t) currPrm.minVal;
      uint8_t  maxValFill = (uint8_t) currPrm.maxVal;
      uint8_t *pFill      = (uint8_t *) pMinVal;
      *pFill              = minValFill;
      pFill               = (uint8_t *) pMinValCn;
      *pFill              = minValFill;
      pFill               = (uint8_t *) pMaxVal;
      *pFill              = maxValFill;
      pFill               = (uint8_t *) pMaxValCn;
      *pFill              = maxValFill;
   }
   else if (currPrm.dType == VXLIB_INT8) {
      int8_t  minValFill = (int8_t) currPrm.minVal;
      int8_t  maxValFill = (int8_t) currPrm.maxVal;
      int8_t *pFill      = (int8_t *) pMinVal;
      *pFill             = minValFill;
      pFill              = (int8_t *) pMinValCn;
      *pFill             = minValFill;
      pFill              = (int8_t *) pMaxVal;
      *pFill             = maxValFill;
      pFill              = (int8_t *) pMaxValCn;
      *pFill             = maxValFill;
   }
   else if (currPrm.dType == VXLIB_UINT16) {
      uint16_t  minValFill = (uint16_t) currPrm.minVal;
      uint16_t  maxValFill = (uint16_t) currPrm.maxVal;
      uint16_t *pFill      = (uint16_t *) pMinVal;
      *pFill               = minValFill;
      pFill                = (uint16_t *) pMinValCn;
      *pFill               = minValFill;
      pFill                = (uint16_t *) pMaxVal;
      *pFill               = maxValFill;
      pFill                = (uint16_t *) pMaxValCn;
      *pFill               = maxValFill;
   }
   else if (currPrm.dType == VXLIB_INT16) {
      int16_t  minValFill = (int16_t) currPrm.minVal;
      int16_t  maxValFill = (int16_t) currPrm.maxVal;
      int16_t *pFill      = (int16_t *) pMinVal;
      *pFill              = minValFill;
      pFill               = (int16_t *) pMinValCn;
      *pFill              = minValFill;
      pFill               = (int16_t *) pMaxVal;
      *pFill              = maxValFill;
      pFill               = (int16_t *) pMaxValCn;
      *pFill              = maxValFill;
   }
}

inline void TI_fill_uint32Vals(VXLIB_minMaxLoc_testParams_t currPrm,
                               void                        *pMinCount,
                               void                        *pMinCountCn,
                               void                        *pMaxCount,
                               void                        *pMaxCountCn,
                               void                        *pMinLocCapacity,
                               void                        *pMinLocCapacityCn,
                               void                        *pMaxLocCapacity,
                               void                        *pMaxLocCapacityCn,
                               void                        *pStartX,
                               void                        *pStartY)
{
   if (currPrm.mode > 1) {
      uint32_t *pFill = (uint32_t *) pMinCount;
      *pFill          = currPrm.minCount;
      pFill           = (uint32_t *) pMinCountCn;
      *pFill          = currPrm.minCount;
      pFill           = (uint32_t *) pMaxCount;
      *pFill          = currPrm.maxCount;
      pFill           = (uint32_t *) pMaxCountCn;
      *pFill          = currPrm.maxCount;
   }
   if (currPrm.mode > 2) {
      uint32_t *pFill = (uint32_t *) pMinLocCapacity;
      *pFill          = currPrm.minLocCapacity;
      pFill           = (uint32_t *) pMinLocCapacityCn;
      *pFill          = currPrm.minLocCapacity;
      pFill           = (uint32_t *) pMaxLocCapacity;
      *pFill          = currPrm.maxLocCapacity;
      pFill           = (uint32_t *) pMaxLocCapacityCn;
      *pFill          = currPrm.maxLocCapacity;
      pFill           = (uint32_t *) pStartX;
      *pFill          = currPrm.startX;
      pFill           = (uint32_t *) pStartY;
      *pFill          = currPrm.startY;
   }
}
/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_minMaxLoc_d */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method performs regerssion testing for the VXLIB_minMaxLoc kernel
int VXLIB_minMaxLoc_d(uint32_t *pProfile, uint8_t LevelOfFeedback)
{
   int32_t  tpi;             // iteration variable for test input
   int32_t  currentTestFail; // status of current test case
   int32_t  fail = 0;        // pass/fail flag
   uint32_t repCount;        // iteration variable for repetition count
   uint32_t numReps;         // number of times to repeat test cases
   uint32_t testNum;         // test number

   // input and output buffer parameter variables
   VXLIB_bufParams2D_t bufParamsIn;
   VXLIB_bufParams1D_t bufParamsMinLoc;
   VXLIB_bufParams1D_t bufParamsMaxLoc;

#if defined(EVM_TEST)
   uint32_t k = 0;
#endif

   // performance estimation
   uint64_t archCycles, estCycles;

   // data structure to hold test cases parameters
   VXLIB_minMaxLoc_testParams_t *prm;
   VXLIB_minMaxLoc_testParams_t  currPrm;

   // get total number of test cases
   VXLIB_minMaxLoc_getTestParams(&prm, &test_cases);

   // initialization args for kernel
   VXLIB_minMaxLoc_InitArgs kerInitArgs;

#if VXLIB_DEBUGPRINT
   printf("VXLIB_DEBUGPRINT  VXLIB_minMaxLoc_d CP 0\n");
#endif

   // create kernel's handle
   int32_t            handleSize = VXLIB_minMaxLoc_getHandleSize(&kerInitArgs);
   VXLIB_kernelHandle handle     = malloc(handleSize);

#if VXLIB_DEBUGPRINT
   printf("VXLIB_DEBUGPRINT  VXLIB_minMaxLoc_d CP 1\n");
#endif

   TI_profile_init("VXLIB_minMaxLoc");

#if !defined(EVM_TEST)
   // file IO for benchmarking
   //  FILE *fpOutputCSV = fopen("VXLIB_minMaxLoc.csv", "w+");
   //  fprintf(fpOutputCSV, "Kernel, Test ID, Test type, Bit Width, Parameters, width, height, Arch"
   //                       "cycles, Loki cycles, Loki/Arch cycles, Pass/Fail\n");
#endif // #if defined(EVM_TEST)

   // iterate through test cases
   for (tpi = 0; tpi < test_cases; tpi++) {
      numReps = prm[tpi].numReps;
      testNum = prm[tpi].testID;
      currPrm = prm[tpi];

      for (repCount = 0; repCount < numReps; repCount++) {
         int32_t      status_nat_vs_opt          = TI_TEST_KERNEL_FAIL;
         int32_t      status_ref_vs_opt          = TI_TEST_KERNEL_FAIL;
         int32_t      status_nat_vs_opt_minLoc   = TI_TEST_KERNEL_FAIL;
         int32_t      status_nat_vs_opt_maxLoc   = TI_TEST_KERNEL_FAIL;
         int32_t      status_ref_vs_opt_minLoc   = TI_TEST_KERNEL_FAIL;
         int32_t      status_ref_vs_opt_maxLoc   = TI_TEST_KERNEL_FAIL;
         int32_t      status_nat_vs_opt_minVal   = TI_TEST_KERNEL_FAIL;
         int32_t      status_nat_vs_opt_maxVal   = TI_TEST_KERNEL_FAIL;
         int32_t      status_ref_vs_opt_minVal   = TI_TEST_KERNEL_FAIL;
         int32_t      status_ref_vs_opt_maxVal   = TI_TEST_KERNEL_FAIL;
         int32_t      status_nat_vs_opt_minCount = TI_TEST_KERNEL_FAIL;
         int32_t      status_nat_vs_opt_maxCount = TI_TEST_KERNEL_FAIL;
         int32_t      status_ref_vs_opt_minCount = TI_TEST_KERNEL_FAIL;
         int32_t      status_ref_vs_opt_maxCount = TI_TEST_KERNEL_FAIL;
         VXLIB_STATUS status_init                = VXLIB_SUCCESS;
         VXLIB_STATUS status_opt                 = VXLIB_SUCCESS;
         VXLIB_STATUS status_nat                 = VXLIB_SUCCESS;

         /* Sometimes, depending on certain compile flags, the test will be
          * marked PASS even if no comparison is done. This flag is to detect
          * if a comparison was done or not                                   */
         int32_t comparisonDone = 0;

         currentTestFail = 0;

         /*****************************************/
         /* Assign input output buffer parameters */
         /*****************************************/

         bufParamsIn.data_type = currPrm.dType;
         bufParamsIn.dim_x     = currPrm.width;
         bufParamsIn.dim_y     = currPrm.height;
         bufParamsIn.stride_y  = currPrm.strideIn;

         bufParamsMinLoc.data_type = VXLIB_UINT32;
         bufParamsMinLoc.dim_x     = 2 * currPrm.minLocCapacity; // * VXLIB_sizeof(VXLIB_UINT32);

         bufParamsMaxLoc.data_type = VXLIB_UINT32;
         bufParamsMaxLoc.dim_x     = 2 * currPrm.maxLocCapacity; // * VXLIB_sizeof(VXLIB_UINT32);


         /*****************************************/
         /* Create buffers for input and outputs  */
         /*****************************************/

         size_t inpSize            = bufParamsIn.dim_y * bufParamsIn.stride_y;
         size_t minValSize         = VXLIB_sizeof(currPrm.dType);
         size_t maxValSize         = VXLIB_sizeof(currPrm.dType);
         size_t minCountSize       = VXLIB_sizeof(VXLIB_UINT32);
         size_t maxCountSize       = VXLIB_sizeof(VXLIB_UINT32);
         size_t minLocCapacitySize = VXLIB_sizeof(VXLIB_UINT32);
         size_t maxLocCapacitySize = VXLIB_sizeof(VXLIB_UINT32);
         size_t minLocSize         = bufParamsMinLoc.dim_x * VXLIB_sizeof(bufParamsMinLoc.data_type);
         size_t maxLocSize         = bufParamsMaxLoc.dim_x * VXLIB_sizeof(bufParamsMaxLoc.data_type);
         size_t startXSize         = VXLIB_sizeof(VXLIB_UINT32);
         size_t startYSize         = VXLIB_sizeof(VXLIB_UINT32);
         //  printf("inpSizeBytes: %lu inpSizeEle: %lu\n", inpSize, inpSize / VXLIB_sizeof(currPrm.dType));

         void *pIn = (void *) TI_memalign(VXLIB_L2DATA_ALIGNMENT, inpSize);
         void *pStartX = (void *) TI_memalign(VXLIB_L2DATA_ALIGNMENT, startXSize);
         void *pStartY = (void *) TI_memalign(VXLIB_L2DATA_ALIGNMENT, startYSize);

         void *pMinVal = NULL, *pMinValCn = NULL, *pMaxVal = NULL, *pMaxValCn = NULL;
         void *pMinCount = NULL, *pMaxCount = NULL, *pMinCountCn = NULL, *pMaxCountCn = NULL;
         void *pMinLocCapacity = NULL, *pMinLocCapacityCn = NULL;
         void *pMaxLocCapacity = NULL, *pMaxLocCapacityCn = NULL;
         void *pMinLoc = NULL, *pMaxLoc = NULL, *pMinLocCn = NULL, *pMaxLocCn = NULL;

         if (currPrm.outputDataLocation == VXLIB_TEST_OUTPUT_HEAP) {
            pMinVal   = (void *) TI_memalign(VXLIB_L2DATA_ALIGNMENT, minValSize);
            pMaxVal   = (void *) TI_memalign(VXLIB_L2DATA_ALIGNMENT, maxValSize);
            pMinValCn = (void *) malloc(minValSize);
            pMaxValCn = (void *) malloc(maxValSize);
            if (currPrm.mode > 1) {
               pMinCount   = (void *) TI_memalign(VXLIB_L2DATA_ALIGNMENT, minCountSize);
               pMaxCount   = (void *) TI_memalign(VXLIB_L2DATA_ALIGNMENT, maxCountSize);
               pMinCountCn = (void *) malloc(minCountSize);
               pMaxCountCn = (void *) malloc(maxCountSize);

               if (currPrm.mode > 2) {
                  pMinLocCapacity   = (void *) TI_memalign(VXLIB_L2DATA_ALIGNMENT, minLocCapacitySize);
                  pMaxLocCapacity   = (void *) TI_memalign(VXLIB_L2DATA_ALIGNMENT, maxLocCapacitySize);
                  pMinLoc           = (void *) TI_memalign(VXLIB_L2DATA_ALIGNMENT, minLocSize);
                  pMaxLoc           = (void *) TI_memalign(VXLIB_L2DATA_ALIGNMENT, maxLocSize);
                  pMinLocCapacityCn = (void *) malloc(minLocCapacitySize);
                  pMaxLocCapacityCn = (void *) malloc(maxLocCapacitySize);
                  pMinLocCn         = (void *) malloc(minLocSize);
                  pMaxLocCn         = (void *) malloc(maxLocSize);
               }
            }
         }
         else if (currPrm.outputDataLocation == VXLIB_TEST_OUTPUT_MSMC) {
            pMinVal = (void *) TI_memalign(VXLIB_L2DATA_ALIGNMENT, minValSize);
            pMaxVal = (void *) TI_memalign(VXLIB_L2DATA_ALIGNMENT, maxValSize);
            if (currPrm.mode > 1) {
               pMinCount = (void *) TI_memalign(VXLIB_L2DATA_ALIGNMENT, minCountSize);
               pMaxCount = (void *) TI_memalign(VXLIB_L2DATA_ALIGNMENT, maxCountSize);

               if (currPrm.mode > 2) {
                  pMinLocCapacity = (void *) TI_memalign(VXLIB_L2DATA_ALIGNMENT, minLocCapacitySize);
                  pMaxLocCapacity = (void *) TI_memalign(VXLIB_L2DATA_ALIGNMENT, maxLocCapacitySize);

                  size_t usedMSMCBytes = 0;
                  pMinLoc              = (void *) (msmcBuffer + usedMSMCBytes);
                  usedMSMCBytes += minLocSize;
                  pMaxLoc = (void *) (msmcBuffer + usedMSMCBytes);
                  usedMSMCBytes += maxLocSize;
               }
            }
            size_t usedDDRBytes = 0;
            pMinValCn           = (void *) (ddrBuffer + usedDDRBytes);
            usedDDRBytes += minValSize;
            pMaxValCn = (void *) (ddrBuffer + usedDDRBytes);
            usedDDRBytes += maxValSize;
            if (currPrm.mode > 1) {
               pMinCountCn = (void *) (ddrBuffer + usedDDRBytes);
               usedDDRBytes += minCountSize;
               pMaxCountCn = (void *) (ddrBuffer + usedDDRBytes);
               usedDDRBytes += maxCountSize;

               if (currPrm.mode > 2) {
                  pMinLocCapacityCn = (void *) (ddrBuffer + usedDDRBytes);
                  usedDDRBytes += minLocCapacitySize;
                  pMaxLocCapacityCn = (void *) (ddrBuffer + usedDDRBytes);
                  usedDDRBytes += maxLocCapacitySize;
                  pMinLocCn = (void *) (ddrBuffer + usedDDRBytes);
                  usedDDRBytes += minLocSize;
                  pMaxLocCn = (void *) (ddrBuffer + usedDDRBytes);
                  usedDDRBytes += maxLocSize;
               }
            }
         }

         // only run the test if the buffer allocations fit in the heap

         if (pIn && pMinVal && pMinValCn && pMaxVal && pMaxValCn &&
             ((currPrm.mode == 1) ||
              (currPrm.mode > 1 && pMinCount && pMinCountCn && pMaxCount && pMaxCountCn &&
               ((currPrm.mode == 2) || (currPrm.mode > 2 && pMaxLoc && pMaxLocCn && pMinLoc && pMinLocCn))))) {

            // fill buffers
            TI_fillBuffer(prm[tpi].testPattern, 0, pIn, prm[tpi].staticIn, bufParamsIn.dim_x, bufParamsIn.dim_y,
                          bufParamsIn.stride_y, VXLIB_sizeof(currPrm.dType), testPatternString);

            TI_fill_minMaxVal(currPrm, pMinVal, pMinValCn, pMaxVal, pMaxValCn);

            TI_fill_uint32Vals(currPrm, pMinCount, pMinCountCn, pMaxCount, pMaxCountCn, pMinLocCapacity,
                               pMinLocCapacityCn, pMaxLocCapacity, pMaxLocCapacityCn, pStartX, pStartY);

#if VXLIB_DEBUGPRINT
            printf("VXLIB_DEBUGPRINT  VXLIB_minMaxLoc_d CP 0\n");
#endif

#if VXLIB_DEBUGPRINT
            printf("VXLIB_DEBUGPRINT  VXLIB_minMaxLoc_d CP 1 status_init %d\n", status_init);
#endif
            // check parameters for initializing the kernel and initialize kernel if successful
            status_init = VXLIB_minMaxLoc_init_checkParams(handle, &bufParamsIn, &bufParamsMinLoc, &bufParamsMaxLoc,
                                                           &kerInitArgs);
            if (status_init == VXLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_INIT);
               VXLIB_asm(" MARK 0");
               kerInitArgs.funcStyle = VXLIB_FUNCTION_OPTIMIZED;
               status_init =
                   VXLIB_minMaxLoc_init(handle, &bufParamsIn, &bufParamsMinLoc, &bufParamsMaxLoc, &kerInitArgs);
               VXLIB_asm(" MARK 1");
               TI_profile_stop();
            }
            else {
               printf("status_init failed: %d", status_init);
            }

            // check execution parameters and call optimized kernel if sucessful
            status_opt =
                VXLIB_minMaxLoc_exec_checkParams(handle, pIn, pMinVal, pMaxVal, pMinCount, pMaxCount, pMinLocCapacity,
                                                 pMaxLocCapacity, pMinLoc, pMaxLoc, pStartX, pStartY);
            if (status_opt == VXLIB_SUCCESS) {
               TI_profile_start(TI_PROFILE_KERNEL_OPT);
               VXLIB_asm(" MARK 2");
               status_opt = VXLIB_minMaxLoc_exec(handle, pIn, pMinVal, pMaxVal, pMinCount, pMaxCount, pMinLocCapacity,
                                                 pMaxLocCapacity, pMinLoc, pMaxLoc, pStartX, pStartY);
               VXLIB_asm(" MARK 3");
               TI_profile_stop();
            }
            else {
               printf("status_opt failed: %d", status_opt);
            }

#if defined(EVM_TEST)
            uint32_t k = 0;
            // the following for loop is to call kernel repeatedly so as to train the branch predictor
            for (k = 0; k < (VXLIB_WARM_REPS); k++) {
               TI_fill_minMaxVal(currPrm, pMinVal, pMinValCn, pMaxVal, pMaxValCn);
               TI_fill_uint32Vals(currPrm, pMinCount, pMinCountCn, pMaxCount, pMaxCountCn, pMinLocCapacity,
                                  pMinLocCapacityCn, pMaxLocCapacity, pMaxLocCapacityCn, pStartX, pStartY);

               // run warm instruction cache test
               TI_profile_clear_cycle_count_single(TI_PROFILE_KERNEL_OPT_WARM);
               TI_profile_start(TI_PROFILE_KERNEL_OPT_WARM);
               VXLIB_asm(" MARK 4");
               status_opt = VXLIB_minMaxLoc_exec(handle, pIn, pMinVal, pMaxVal, pMinCount, pMaxCount, pMinLocCapacity,
                                                 pMaxLocCapacity, pMinLoc, pMaxLoc, pStartX, pStartY);
               VXLIB_asm(" MARK 5");
               TI_profile_stop();
            }

            // get output to L1D
            int16_t outSum  = 0;
            int8_t *pInTemp = (int8_t *) pIn; // treat output as bytes to be data type agnostic
            for (k = 0; k < bufParamsIn.dim_x; k++) {
               outSum += *pInTemp;
               pInTemp++;
            }

            // dummy store of outSum to insure that the compiler does not remove it.
            volatileSum = outSum;
            TI_fill_minMaxVal(currPrm, pMinVal, pMinValCn, pMaxVal, pMaxValCn);
            TI_fill_uint32Vals(currPrm, pMinCount, pMinCountCn, pMaxCount, pMaxCountCn, pMinLocCapacity,
                               pMinLocCapacityCn, pMaxLocCapacity, pMaxLocCapacityCn, pStartX, pStartY);

            // run warm instruction cache test
            TI_profile_start(TI_PROFILE_KERNEL_OPT_WARMWRB);

            VXLIB_asm(" MARK 6");
            status_opt = VXLIB_minMaxLoc_exec(handle, pIn, pMinVal, pMaxVal, pMinCount, pMaxCount, pMinLocCapacity,
                                              pMaxLocCapacity, pMinLoc, pMaxLoc, pStartX, pStartY);
            VXLIB_asm(" MARK 7");
            TI_profile_stop();
#endif // if defined(EVM_TEST)

            /**************************************/
            /* Test natural C code for the kernel */
            /**************************************/

            // initialize the kernel function style to natural C
            kerInitArgs.funcStyle = VXLIB_FUNCTION_NATC;

            // call init once again to use the natural C variant
            VXLIB_minMaxLoc_init(handle, &bufParamsIn, &bufParamsMinLoc, &bufParamsMaxLoc, &kerInitArgs);

            TI_profile_start(TI_PROFILE_KERNEL_CN);
            VXLIB_asm(" MARK 8");
            status_nat =
                VXLIB_minMaxLoc_exec(handle, pIn, pMinValCn, pMaxValCn, pMinCountCn, pMaxCountCn, pMinLocCapacityCn,
                                     pMaxLocCapacityCn, pMinLocCn, pMaxLocCn, pStartX, pStartY);
            VXLIB_asm(" MARK 9");
            TI_profile_stop();
#if VXLIB_DEBUGPRINT
            printf("VXLIB_DEBUGPRINT  VXLIB_minMaxLoc_d CP 6 status_nat %d\n", status_nat);
#endif

            /*********************************************************************/
            /* Compare results with natural C output and static reference output */
            /*********************************************************************/
            VXLIB_DEBUGPRINTFN(0, "%s", " NATC VS OPT COMPARISON \n");
            VXLIB_DEBUGPRINTFN(0, "%s", "Compare Min / Max Values\n");
            status_nat_vs_opt_minVal = TI_compare_mem_2D((void *) pMinVal, (void *) pMinValCn, 0, 0, 1, 1,
                                                         VXLIB_sizeof(currPrm.dType), VXLIB_sizeof(currPrm.dType));

            status_nat_vs_opt_maxVal = TI_compare_mem_2D((void *) pMaxVal, (void *) pMaxValCn, 0, 0, 1, 1,
                                                         VXLIB_sizeof(currPrm.dType), VXLIB_sizeof(currPrm.dType));

            if (currPrm.mode > 1) {
               VXLIB_DEBUGPRINTFN(0, "%s", "Compare minCount\n");
               status_nat_vs_opt_minCount = TI_compare_mem_2D((void *) pMinCount, (void *) pMinCountCn, 0, 0, 1, 1,
                                                              VXLIB_sizeof(VXLIB_UINT32), VXLIB_sizeof(VXLIB_UINT32));

               VXLIB_DEBUGPRINTFN(0, "%s", "Compare maxCount\n");
               status_nat_vs_opt_maxCount = TI_compare_mem_2D((void *) pMaxCount, (void *) pMaxCountCn, 0, 0, 1, 1,
                                                              VXLIB_sizeof(VXLIB_UINT32), VXLIB_sizeof(VXLIB_UINT32));

               if (currPrm.mode > 2) {
                  if (status_nat_vs_opt_minCount == TI_TEST_KERNEL_PASS) {
                     VXLIB_DEBUGPRINTFN(0, "%s", "Compare minLoc\n");
                     uint32_t *minCountResult    = (uint32_t *) pMinCount;
                     uint32_t  compareMinLocSize = 2 * (currPrm.minLocCapacity);
                     if (compareMinLocSize > (2 * (*minCountResult))) {
                        compareMinLocSize = (2 * (*minCountResult));
                     }
                     status_nat_vs_opt_minLoc = TI_compare_mem_2D((void *) pMinLoc, (void *) pMinLocCn, 0, 0,
                                                                  compareMinLocSize, 1, 0, VXLIB_sizeof(VXLIB_UINT32));
                  }
                  if (status_nat_vs_opt_maxCount == TI_TEST_KERNEL_PASS) {
                     VXLIB_DEBUGPRINTFN(0, "%s", "Compare maxLoc\n");
                     uint32_t *maxCountResult    = (uint32_t *) pMaxCount;
                     uint32_t  compareMaxLocSize = 2 * (currPrm.maxLocCapacity);
                     if (compareMaxLocSize > (2 * (*maxCountResult))) {
                        compareMaxLocSize = (2 * (*maxCountResult));
                     }
                     status_nat_vs_opt_maxLoc = TI_compare_mem_2D((void *) pMaxLoc, (void *) pMaxLocCn, 0, 0,
                                                                  compareMaxLocSize, 1, 0, VXLIB_sizeof(VXLIB_UINT32));
                  }
               }
               else {
                  status_nat_vs_opt_minLoc = TI_TEST_KERNEL_PASS;
                  status_nat_vs_opt_maxLoc = TI_TEST_KERNEL_PASS;
               }
            }
            else {
               status_nat_vs_opt_minCount = TI_TEST_KERNEL_PASS;
               status_nat_vs_opt_maxCount = TI_TEST_KERNEL_PASS;
               status_nat_vs_opt_minLoc   = TI_TEST_KERNEL_PASS;
               status_nat_vs_opt_maxLoc   = TI_TEST_KERNEL_PASS;
            }

            comparisonDone = 1;

            if (currPrm.staticIn != NULL) {
               VXLIB_DEBUGPRINTFN(0, "%s", " STATIC REV VS OPT COMPARISON \n");
               VXLIB_DEBUGPRINTFN(0, "%s", "Compare Min / Max Values\n");
               status_ref_vs_opt_minVal =
                   TI_compare_mem((void *) pMinVal, (void *) currPrm.staticMinVal, VXLIB_sizeof(currPrm.dType));
               status_ref_vs_opt_maxVal =
                   TI_compare_mem((void *) pMaxVal, (void *) currPrm.staticMaxVal, VXLIB_sizeof(currPrm.dType));
               comparisonDone = 1;
               if (currPrm.mode > 1) {
                  VXLIB_DEBUGPRINTFN(0, "%s", "Compare minCount\n");
                  status_ref_vs_opt_minCount =
                      TI_compare_mem((void *) pMinCount, (void *) currPrm.staticMinCount, VXLIB_sizeof(VXLIB_UINT32));
                  VXLIB_DEBUGPRINTFN(0, "%s", "Compare maxCount\n");
                  status_ref_vs_opt_maxCount =
                      TI_compare_mem((void *) pMaxCount, (void *) currPrm.staticMaxCount, VXLIB_sizeof(VXLIB_UINT32));

                  if (currPrm.mode > 2) {
                     if (status_ref_vs_opt_minCount == TI_TEST_KERNEL_PASS) {
                        VXLIB_DEBUGPRINTFN(0, "%s", "Compare minLoc\n");
                        uint32_t *minCountResult    = (uint32_t *) pMinCount;
                        uint32_t  compareMinLocSize = 2 * (currPrm.minLocCapacity);
                        if (compareMinLocSize > (2 * (*minCountResult))) {
                           compareMinLocSize = (2 * (*minCountResult));
                        }
                        status_ref_vs_opt_minLoc =
                            TI_compare_mem_2D((void *) pMinLoc, (void *) currPrm.staticMinLoc, 0, 0, compareMinLocSize,
                                              1, 0, VXLIB_sizeof(VXLIB_UINT32));
                     }
                     if (status_ref_vs_opt_maxCount == TI_TEST_KERNEL_PASS) {
                        VXLIB_DEBUGPRINTFN(0, "%s", "Compare maxLoc\n");
                        uint32_t *maxCountResult    = (uint32_t *) pMaxCount;
                        uint32_t  compareMaxLocSize = 2 * (currPrm.maxLocCapacity);
                        if (compareMaxLocSize > (2 * (*maxCountResult))) {
                           compareMaxLocSize = (2 * (*maxCountResult));
                        }
                        status_ref_vs_opt_maxLoc =
                            TI_compare_mem_2D((void *) pMaxLoc, (void *) currPrm.staticMaxLoc, 0, 0, compareMaxLocSize,
                                              1, 0, VXLIB_sizeof(VXLIB_UINT32));
                     }
                  }
                  else {
                     status_ref_vs_opt_minLoc = TI_TEST_KERNEL_PASS;
                     status_ref_vs_opt_maxLoc = TI_TEST_KERNEL_PASS;
                  }
               }
               else {
                  status_ref_vs_opt_minCount = TI_TEST_KERNEL_PASS;
                  status_ref_vs_opt_maxCount = TI_TEST_KERNEL_PASS;
                  status_ref_vs_opt_minLoc   = TI_TEST_KERNEL_PASS;
                  status_ref_vs_opt_maxLoc   = TI_TEST_KERNEL_PASS;
               }
            }
            else {
               // set to pass since it wasn't supposed to run.
               status_ref_vs_opt          = TI_TEST_KERNEL_PASS;
               status_ref_vs_opt_minVal   = TI_TEST_KERNEL_PASS;
               status_ref_vs_opt_maxVal   = TI_TEST_KERNEL_PASS;
               status_ref_vs_opt_minCount = TI_TEST_KERNEL_PASS;
               status_ref_vs_opt_maxCount = TI_TEST_KERNEL_PASS;
               status_ref_vs_opt_minLoc   = TI_TEST_KERNEL_PASS;
               status_ref_vs_opt_maxLoc   = TI_TEST_KERNEL_PASS;
            }

            status_nat_vs_opt =
                ((status_nat_vs_opt_minVal == TI_TEST_KERNEL_FAIL) ||
                 (status_nat_vs_opt_maxVal == TI_TEST_KERNEL_FAIL) ||
                 (status_nat_vs_opt_minCount == TI_TEST_KERNEL_FAIL) ||
                 (status_nat_vs_opt_maxCount == TI_TEST_KERNEL_FAIL) ||
                 (status_nat_vs_opt_minLoc == TI_TEST_KERNEL_FAIL) || (status_nat_vs_opt_maxLoc == TI_TEST_KERNEL_FAIL))
                    ? TI_TEST_KERNEL_FAIL
                    : TI_TEST_KERNEL_PASS;

            status_ref_vs_opt =
                ((status_ref_vs_opt_minVal == TI_TEST_KERNEL_FAIL) ||
                 (status_ref_vs_opt_maxVal == TI_TEST_KERNEL_FAIL) ||
                 (status_ref_vs_opt_minCount == TI_TEST_KERNEL_FAIL) ||
                 (status_ref_vs_opt_maxCount == TI_TEST_KERNEL_FAIL) ||
                 (status_ref_vs_opt_minLoc == TI_TEST_KERNEL_FAIL) || (status_ref_vs_opt_maxLoc == TI_TEST_KERNEL_FAIL))
                    ? TI_TEST_KERNEL_FAIL
                    : TI_TEST_KERNEL_PASS;

            VXLIB_DEBUGPRINTFN(0, "status_nat_vs_opt: %d status_ref_vs_opt: %d\n", status_nat_vs_opt,
                               status_ref_vs_opt);

            // set the 'fail' flag based on test vector comparison results
            currentTestFail =
                ((status_nat_vs_opt == TI_TEST_KERNEL_FAIL) || (status_ref_vs_opt == TI_TEST_KERNEL_FAIL) ||
                 (status_opt != VXLIB_SUCCESS) || (status_nat != VXLIB_SUCCESS) || (status_init != VXLIB_SUCCESS) ||
                 (comparisonDone == 0) || (currentTestFail == 1))
                    ? 1
                    : 0;

            fail = ((fail == 1) || (currentTestFail == 1)) ? 1 : 0;
#if VXLIB_DEBUGPRINT
            printf("VXLIB_DEBUGPRINT  VXLIB_minMaxLoc_d CP 8 fail %d\n", fail);
#endif

            // update profiling information
            pProfile[3 * tpi]     = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT);
            pProfile[3 * tpi + 1] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARM);
            pProfile[3 * tpi + 2] = (int32_t) TI_profile_get_cycles(TI_PROFILE_KERNEL_OPT_WARMWRB);

            sprintf(desc, "%s | R | %u | C | %u | mode | %u | dType | %u |", testPatternString, currPrm.height,
                    currPrm.width, currPrm.mode, currPrm.dType);

#if !defined(EVM_TEST)
            // write to CSV, must happen prior to write to screen because TI_profile_formula_add clears values in
            // counters
            /* fprintf(fpOutputCSV, "add, %d, %d, %d, %lu, %lu, %lu, %lu, %.2f, %d\n", testNum, currPrm.testPattern,
             */
            /*         VXLIB_sizeof(bufParamsIn0.data_type) * 8, currPrm.width, currPrm.height, archCycles, */
            /*         cycles[TI_PROFILE_KERNEL_OPT], */
            /*         ((VXLIB_F32) cycles[TI_PROFILE_KERNEL_OPT]) / ((VXLIB_F32) archCycles), !currentTestFail); */
#endif // #if !defined(EVM_TEST)
            VXLIB_minMaxLoc_perfEst(handle, &bufParamsIn, &bufParamsMinLoc, &bufParamsMaxLoc, &kerInitArgs, &archCycles, &estCycles);
            TI_profile_add_test(testNum++, currPrm.width * currPrm.height, archCycles, estCycles, currentTestFail, desc);
         }
         else {
            sprintf(desc, "width =%u, height = %u", currPrm.width, currPrm.height);
            TI_profile_skip_test(desc);

         } // end of memory allocation successful?

         /*************************************/
         /* Free buffers for each test vector */
         /*************************************/

         if (currPrm.outputDataLocation == VXLIB_TEST_OUTPUT_HEAP) {
            free(pMinValCn);
            free(pMaxValCn);
            TI_align_free(pMinVal);
            TI_align_free(pMaxVal);
            if (currPrm.mode > 1) {
               free(pMinCountCn);
               free(pMaxCountCn);
               TI_align_free(pMinCount);
               TI_align_free(pMaxCount);
               if (currPrm.mode > 2) {
                  free(pMinLocCn);
                  free(pMaxLocCn);
                  free(pMinLocCapacityCn);
                  free(pMaxLocCapacityCn);
                  TI_align_free(pMinLoc);
                  TI_align_free(pMaxLoc);
                  TI_align_free(pMinLocCapacity);
                  TI_align_free(pMaxLocCapacity);
               }
            }
         }
         else {
            TI_align_free(pMinVal);
            TI_align_free(pMaxVal);
            if (currPrm.mode > 1) {
               TI_align_free(pMinCount);
               TI_align_free(pMaxCount);
               if (currPrm.mode > 2) {
                  TI_align_free(pMinLocCapacity);
                  TI_align_free(pMaxLocCapacity);
               }
            }
         }
         TI_align_free(pIn);
         TI_align_free(pStartX);
         TI_align_free(pStartY);
      } // end repetitions
   }    // end idat test cases

   free(handle);

   /* Close results CSV */
   /* fclose(fpOutputCSV); */

   return fail;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* coverage_test_main */
/*                                                                                                                    */
/**********************************************************************************************************************/

int coverage_test_main()
{
   int32_t                  testNum         = 1000;
   int32_t                  currentTestFail = 0;
   VXLIB_STATUS             status_init;
   VXLIB_STATUS             status_opt = VXLIB_ERR_FAILURE;
   VXLIB_bufParams2D_t      bufParamsIn;
   VXLIB_bufParams1D_t      bufParamsMinLoc, bufParamsMaxLoc;
   VXLIB_minMaxLoc_InitArgs kerInitArgs;
   int32_t                  handleSize = VXLIB_minMaxLoc_getHandleSize(&kerInitArgs);
   VXLIB_kernelHandle       handle     = malloc(handleSize);
   int                      fail       = 0;

   bufParamsIn.data_type = VXLIB_UINT8;

   while (testNum <= 1005) {
      currentTestFail = 0;
      switch (testNum) {

      case 1000:
         status_init =
             VXLIB_minMaxLoc_init_checkParams(NULL, &bufParamsIn, &bufParamsMinLoc, &bufParamsMaxLoc, &kerInitArgs);
         currentTestFail = (status_init != VXLIB_ERR_NULL_POINTER) ? 1 : 0;

         status_init = VXLIB_minMaxLoc_init_checkParams(handle, NULL, &bufParamsMinLoc, &bufParamsMaxLoc, &kerInitArgs);
         currentTestFail = ((currentTestFail == 1) || (status_init != VXLIB_ERR_NULL_POINTER)) ? 1 : 0;

         status_init     = VXLIB_minMaxLoc_init_checkParams(handle, &bufParamsIn, NULL, &bufParamsMaxLoc, &kerInitArgs);
         currentTestFail = ((currentTestFail == 1) || (status_init != VXLIB_ERR_NULL_POINTER)) ? 1 : 0;

         status_init     = VXLIB_minMaxLoc_init_checkParams(handle, &bufParamsIn, &bufParamsMinLoc, NULL, &kerInitArgs);
         currentTestFail = ((currentTestFail == 1) || (status_init != VXLIB_ERR_NULL_POINTER)) ? 1 : 0;

         status_init = VXLIB_minMaxLoc_init_checkParams(handle, &bufParamsIn, &bufParamsMinLoc, &bufParamsMaxLoc, NULL);
         currentTestFail = ((currentTestFail == 1) || (status_init != VXLIB_ERR_NULL_POINTER)) ? 1 : 0;

         break;
      case 1001:
         bufParamsIn.data_type = VXLIB_UINT8;
         bufParamsIn.dim_x     = 8;
         bufParamsIn.stride_y  = 4;
         status_init =
             VXLIB_minMaxLoc_init_checkParams(handle, &bufParamsIn, &bufParamsMinLoc, &bufParamsMaxLoc, &kerInitArgs);
         currentTestFail = (status_init != VXLIB_ERR_INVALID_DIMENSION) ? 1 : 0;

         currentTestFail = ((currentTestFail == 1) || (status_init != VXLIB_ERR_INVALID_DIMENSION)) ? 1 : 0;
         break;

      case 1002:
         bufParamsIn.data_type = VXLIB_FLOAT32;
         bufParamsIn.dim_x     = 8;
         bufParamsIn.dim_y     = 8;
         bufParamsIn.stride_y  = 32;
         status_init =
             VXLIB_minMaxLoc_init_checkParams(handle, &bufParamsIn, &bufParamsMinLoc, &bufParamsMaxLoc, &kerInitArgs);
         currentTestFail = (status_init != VXLIB_ERR_INVALID_TYPE) ? 1 : 0;

         kerInitArgs.funcStyle = VXLIB_FUNCTION_NATC;
         status_init     = VXLIB_minMaxLoc_init(handle, &bufParamsIn, &bufParamsMinLoc, &bufParamsMaxLoc, &kerInitArgs);
         currentTestFail = ((currentTestFail == 1) || (status_init != VXLIB_ERR_INVALID_TYPE)) ? 1 : 0;

         kerInitArgs.funcStyle = VXLIB_FUNCTION_OPTIMIZED;
         status_init     = VXLIB_minMaxLoc_init(handle, &bufParamsIn, &bufParamsMinLoc, &bufParamsMaxLoc, &kerInitArgs);
         currentTestFail = ((currentTestFail == 1) || (status_init != VXLIB_ERR_INVALID_TYPE)) ? 1 : 0;

         break;
      case 1003:
         bufParamsIn.data_type = VXLIB_FLOAT16;

         status_init     = VXLIB_minMaxLoc_init(handle, &bufParamsIn, &bufParamsMinLoc, &bufParamsMaxLoc, &kerInitArgs);
         currentTestFail = ((status_init != VXLIB_ERR_INVALID_TYPE)) ? 1 : 0;

         bufParamsIn.data_type = VXLIB_UINT24;
         status_init     = VXLIB_minMaxLoc_init(handle, &bufParamsIn, &bufParamsMinLoc, &bufParamsMaxLoc, &kerInitArgs);
         currentTestFail = ((currentTestFail == 1) || (status_init != VXLIB_ERR_INVALID_TYPE)) ? 1 : 0;

         bufParamsIn.data_type = VXLIB_INT32;
         status_init     = VXLIB_minMaxLoc_init(handle, &bufParamsIn, &bufParamsMinLoc, &bufParamsMaxLoc, &kerInitArgs);
         currentTestFail = ((currentTestFail == 1) || (status_init != VXLIB_ERR_INVALID_TYPE)) ? 1 : 0;

         bufParamsIn.data_type = VXLIB_UINT32;
         status_init     = VXLIB_minMaxLoc_init(handle, &bufParamsIn, &bufParamsMinLoc, &bufParamsMaxLoc, &kerInitArgs);
         currentTestFail = ((currentTestFail == 1) || (status_init != VXLIB_ERR_INVALID_TYPE)) ? 1 : 0;

         bufParamsIn.data_type = VXLIB_FLOAT32;
         status_init     = VXLIB_minMaxLoc_init(handle, &bufParamsIn, &bufParamsMinLoc, &bufParamsMaxLoc, &kerInitArgs);
         currentTestFail = ((currentTestFail == 1) || (status_init != VXLIB_ERR_INVALID_TYPE)) ? 1 : 0;

         bufParamsIn.data_type = VXLIB_INT64;
         status_init     = VXLIB_minMaxLoc_init(handle, &bufParamsIn, &bufParamsMinLoc, &bufParamsMaxLoc, &kerInitArgs);
         currentTestFail = ((currentTestFail == 1) || (status_init != VXLIB_ERR_INVALID_TYPE)) ? 1 : 0;

         bufParamsIn.data_type = VXLIB_UINT64;
         status_init     = VXLIB_minMaxLoc_init(handle, &bufParamsIn, &bufParamsMinLoc, &bufParamsMaxLoc, &kerInitArgs);
         currentTestFail = ((currentTestFail == 1) || (status_init != VXLIB_ERR_INVALID_TYPE)) ? 1 : 0;

         bufParamsIn.data_type = VXLIB_FLOAT64;
         status_init     = VXLIB_minMaxLoc_init(handle, &bufParamsIn, &bufParamsMinLoc, &bufParamsMaxLoc, &kerInitArgs);
         currentTestFail = ((currentTestFail == 1) || (status_init != VXLIB_ERR_INVALID_TYPE)) ? 1 : 0;

         bufParamsIn.data_type = VXLIB_INT128;
         status_init     = VXLIB_minMaxLoc_init(handle, &bufParamsIn, &bufParamsMinLoc, &bufParamsMaxLoc, &kerInitArgs);
         currentTestFail = ((currentTestFail == 1) || (status_init != VXLIB_ERR_INVALID_TYPE)) ? 1 : 0;

         bufParamsIn.data_type = VXLIB_UINT128;
         status_init     = VXLIB_minMaxLoc_init(handle, &bufParamsIn, &bufParamsMinLoc, &bufParamsMaxLoc, &kerInitArgs);
         currentTestFail = ((currentTestFail == 1) || (status_init != VXLIB_ERR_INVALID_TYPE)) ? 1 : 0;

         bufParamsIn.data_type = 14;
         status_init     = VXLIB_minMaxLoc_init(handle, &bufParamsIn, &bufParamsMinLoc, &bufParamsMaxLoc, &kerInitArgs);
         currentTestFail = ((currentTestFail == 1) || (status_init != VXLIB_ERR_INVALID_TYPE)) ? 1 : 0;
         break;

      case 1004: {
         void *pIn     = (void *) malloc(4);
         void *pMinVal = (void *) malloc(4);
         void *pMaxVal = (void *) malloc(4);
         status_init = VXLIB_minMaxLoc_exec_checkParams(NULL, pIn, pMinVal, pMaxVal, NULL, NULL, NULL, NULL, NULL, NULL,
                                                        NULL, NULL);
         currentTestFail = (status_init != VXLIB_ERR_NULL_POINTER) ? 1 : 0;

         status_init = VXLIB_minMaxLoc_exec_checkParams(handle, NULL, pMinVal, pMaxVal, NULL, NULL, NULL, NULL, NULL,
                                                        NULL, NULL, NULL);
         currentTestFail = ((currentTestFail == 1) || (status_init != VXLIB_ERR_NULL_POINTER)) ? 1 : 0;

         status_init = VXLIB_minMaxLoc_exec_checkParams(handle, pIn, NULL, pMaxVal, NULL, NULL, NULL, NULL, NULL, NULL,
                                                        NULL, NULL);
         currentTestFail = ((currentTestFail == 1) || (status_init != VXLIB_ERR_NULL_POINTER)) ? 1 : 0;

         status_init = VXLIB_minMaxLoc_exec_checkParams(handle, pIn, pMinVal, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                                        NULL, NULL);
         currentTestFail = ((currentTestFail == 1) || (status_init != VXLIB_ERR_NULL_POINTER)) ? 1 : 0;

         free(pIn);
         free(pMinVal);
         free(pMaxVal);
      } break;

      case 1005: {
         uint8_t  pIn[16]        = {5, 6, 7, 8, 1, 2, 3, 4, 5, 6, 7, 8, 1, 2, 3, 4};
         uint8_t  minVal         = 1;
         uint8_t  maxVal         = 8;
         uint32_t minCount       = 3;
         uint32_t maxCount       = 3;
         uint32_t minLocCapacity = 4;
         uint32_t maxLocCapacity = 4;
         uint32_t minLoc[12]      = {0};
         uint32_t maxLoc[12]      = {0};
         uint32_t startX         = 0;
         uint32_t startY         = 0;

         bufParamsIn.data_type = VXLIB_UINT8;
         bufParamsIn.dim_x     = 4;
         bufParamsIn.dim_y     = 4;
         bufParamsIn.stride_y  = 4;

         bufParamsMinLoc.data_type = VXLIB_UINT32;
         bufParamsMinLoc.dim_x     = 8;

         bufParamsMaxLoc.data_type = VXLIB_UINT32;
         bufParamsMaxLoc.dim_x     = 8;

         kerInitArgs.funcStyle = VXLIB_FUNCTION_OPTIMIZED;
         status_init =
             VXLIB_minMaxLoc_init_checkParams(handle, &bufParamsIn, &bufParamsMinLoc, &bufParamsMaxLoc, &kerInitArgs);

         if (status_init == VXLIB_SUCCESS) {
            status_init = VXLIB_minMaxLoc_init(handle, &bufParamsIn, &bufParamsMinLoc, &bufParamsMaxLoc, &kerInitArgs);
         }

         if (status_init == VXLIB_SUCCESS) {
            status_opt =
                VXLIB_minMaxLoc_exec_checkParams(handle, pIn, &minVal, &maxVal, &minCount, &maxCount, &minLocCapacity,
                                                 &maxLocCapacity, minLoc, maxLoc, &startX, &startY);
         }

         if (status_opt == VXLIB_SUCCESS) {
            status_opt = VXLIB_minMaxLoc_exec(handle, pIn, &minVal, &maxVal, &minCount, &maxCount, &minLocCapacity,
                                              &maxLocCapacity, minLoc, maxLoc, &startX, &startY);
         }

         currentTestFail = ((status_opt != VXLIB_SUCCESS)) ? 1 : 0;



      } break;

      default:
         break;
      }

      fail = ((fail == 1) || (currentTestFail == 1)) ? 1 : 0;
      sprintf(desc, "%s", "COVERAGE TEST");
      TI_profile_add_test(testNum++, 0, 0, 0, currentTestFail, desc);
   }

   free(handle);
   return fail;
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* test_main */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method calls the testbench function
int test_main(uint32_t *pProfile)
{
#if !defined(_HOST_BUILD)
   if (TI_cache_init()) {
      TI_memError("VXLIB_minMaxLoc");
      return 1;
   }
   else
#else
   printf("_HOST_BUILD is defined.\n");
#endif
   {
      return VXLIB_minMaxLoc_d(&pProfile[0], 0);
   }
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* main */
/*                                                                                                                    */
/**********************************************************************************************************************/

#if !defined(__ONESHOTTEST)
int main()
{
   int fail = 1;

   uint32_t profile[256 * 3] = {0};

   VXLIB_TEST_init();

   fail = test_main(&profile[0]);

#if !defined(NO_PRINTF)
   if (fail == 0)
      printf("Test Pass!\n");
   else
      printf("Test Fail!\n");

   int i;
   for (i = 0; i < test_cases; i++) {
      printf("Test %4d: Cold Cycles = %8d, Warm Cycles = %8d, Warm Cycles WRB = %8d\n", i + 1, profile[3 * i],
             profile[3 * i + 1] / (VXLIB_WARM_REPS), profile[3 * i + 2]);
   }
#endif

   fail = coverage_test_main();
   if (fail == 0) {
      printf("Coverage Test Pass!\n");
   }
   else {
      printf("Coverage Test Fail!!\n");
   }
   return fail;
}
#endif
