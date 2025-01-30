/*
 *  Copyright (c) Texas Instruments Incorporated 2018-2024
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
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
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 *  \file udma_testcases.h
 *
 *  \brief This file defines the test cases for UDMA UT.
 */

#ifndef UDMA_TEST_CASES_H_
#define UDMA_TEST_CASES_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <udma_test.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

#define UDMA_TEST_NUM_TESTCASES         ((sizeof (gUdmaTestCases)) / \
                                         (sizeof (UdmaTestParams)))

#ifndef KB
#define KB                              (1024U)
#endif

#ifndef MB
#define MB                              (KB * KB)
#endif

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/** \brief Defines the various UDMA test cases. */
static UdmaTestParams gUdmaTestCases[] =
{
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3467U,
        .tcName     = UDMA_TEST_MAIN_BC_TCNAME_PREFIX "Blockcpy DDR to DDR in polling mode",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = USE_DEF_LP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_BLK_CPY},
        .testFxnPtr = {&udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_INST_ID_MAIN_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_MAIN_BC | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3473U,
        .tcName     = UDMA_TEST_MAIN_BC_TCNAME_PREFIX "Blockcpy DDR to DDR in interrupt mode",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = USE_DEF_LP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_BLK_CPY},
        .testFxnPtr = {&udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_INST_ID_MAIN_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_INTR_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_MAIN_BC | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
#if (UDMA_SOC_CFG_UDMAP_PRESENT == 1)
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3474U,
        .tcName     = UDMA_TEST_MCU_BC_TCNAME_PREFIX "Blockcpy DDR to DDR in polling mode",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = USE_DEF_LP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_BLK_CPY},
        .testFxnPtr = {&udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_INST_ID_MCU_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_MCU_BC | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3475U,
        .tcName     = UDMA_TEST_MCU_BC_TCNAME_PREFIX "Blockcpy DDR to DDR",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = USE_DEF_LP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_BLK_CPY},
        .testFxnPtr = {&udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_INST_ID_MCU_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_INTR_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_MCU_BC | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
#endif /* #if (UDMA_SOC_CFG_UDMAP_PRESENT == 1) */
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3476U,
        .tcName     = UDMA_TEST_MAIN_BC_TCNAME_PREFIX "Blockcpy DDR to DDR SW global 0 trigger test in polling mode",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = USE_DEF_LP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_BLK_CPY},
        .testFxnPtr = {&udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_INST_ID_MAIN_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_TRIGGER_GLOBAL0},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_MAIN_BC | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3477U,
        .tcName     = UDMA_TEST_MAIN_BC_TCNAME_PREFIX "Blockcpy DDR to DDR SW global 0 trigger test in interrupt mode",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = USE_DEF_LP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_BLK_CPY},
        .testFxnPtr = {&udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_INST_ID_MAIN_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_TRIGGER_GLOBAL0_INTR},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_MAIN_BC),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
#if (UDMA_SOC_CFG_UDMAP_PRESENT == 1)
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3478U,
        .tcName     = UDMA_TEST_MCU_BC_TCNAME_PREFIX "Blockcpy DDR to DDR SW global 0 trigger test in polling mode",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = USE_DEF_LP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_BLK_CPY},
        .testFxnPtr = {&udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_INST_ID_MCU_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_TRIGGER_GLOBAL0},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_MCU_BC),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3479U,
        .tcName     = UDMA_TEST_MCU_BC_TCNAME_PREFIX "Blockcpy DDR to DDR SW global 0 trigger test",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = USE_DEF_LP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_BLK_CPY},
        .testFxnPtr = {&udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_INST_ID_MCU_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_TRIGGER_GLOBAL0_INTR},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_MCU_BC),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
#endif /* #if (UDMA_SOC_CFG_UDMAP_PRESENT == 1) */
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3480U,
        .tcName     = UDMA_TEST_MCU_BC_TCNAME_PREFIX "Blockcpy circular 1KB DDR to DDR 1KB ICNT1 TR event type test",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_BLK_CPY},
        .testFxnPtr = {&udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_INST_ID_MCU_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_EVENTSIZE_ICNT1},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {1*KB, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {1*KB, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {1*KB, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {1*KB},
        .destBufSize= {1*KB},
        .runFlag    = (UDMA_TEST_RF_MCU_BC),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3481U,
        .tcName     = UDMA_TEST_MCU_BC_TCNAME_PREFIX "Blockcpy circular 1KB DDR to DDR 1MB ICNT2 TR event type test",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_BLK_CPY},
        .testFxnPtr = {&udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_INST_ID_MCU_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_EVENTSIZE_ICNT2},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {1*KB, 1U, 1*KB, 1U}
                      },
        .dicnt      = {
                        {1*KB, 1U, 1*KB, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {1*KB, 1*KB, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {1*KB},
        .destBufSize= {1*MB},
        .runFlag    = (UDMA_TEST_RF_MCU_BC),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3482U,
        .tcName     = UDMA_TEST_MCU_BC_TCNAME_PREFIX "Blockcpy circular 1KB DDR to DDR 1MB ICNT3 TR event type test",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_BLK_CPY},
        .testFxnPtr = {&udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_INST_ID_MCU_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_EVENTSIZE_ICNT3},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {1*KB, 1U, 1*KB, 1U}
                      },
        .dicnt      = {
                        {1*KB, 1U, 1*KB, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {1*KB, 1*KB, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {1*KB},
        .destBufSize= {1*MB},
        .runFlag    = (UDMA_TEST_RF_MCU_BC),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3516U,
        .tcName     = UDMA_TEST_MAIN_BC_TCNAME_PREFIX "Blockcpy MSMC to MSMC in interrupt mode",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = USE_DEF_LP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_BLK_CPY},
        .testFxnPtr = {&udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_INST_ID_MAIN_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_INTR_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {UTILS_MEM_HEAP_ID_MSMC},
        .heapIdDest = {UTILS_MEM_HEAP_ID_MSMC},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_MAIN_BC),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
#if (UDMA_SOC_CFG_UDMAP_PRESENT == 1)
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3483U,
        .tcName     = UDMA_TEST_MCU_BC_TCNAME_PREFIX "Blockcpy MSMC to MSMC in interrupt mode",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = USE_DEF_LP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_BLK_CPY},
        .testFxnPtr = {&udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_INST_ID_MCU_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_INTR_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {UTILS_MEM_HEAP_ID_MSMC},
        .heapIdDest = {UTILS_MEM_HEAP_ID_MSMC},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_MCU_BC),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
#endif /* #if (UDMA_SOC_CFG_UDMAP_PRESENT == 1) */
#if (UDMA_TEST_SOC_OCMC_MEM_PRESENT == 1) 
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3484U,
        .tcName     = UDMA_TEST_MCU_BC_TCNAME_PREFIX "Blockcpy OCMC to OCMC in interrupt mode",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = USE_DEF_LP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_BLK_CPY},
        .testFxnPtr = {&udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_INST_ID_MCU_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_INTR_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {UTILS_MEM_HEAP_ID_INTERNAL},
        .heapIdDest = {UTILS_MEM_HEAP_ID_INTERNAL},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_MCU_BC_INTERNAL_MEM),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
#endif /*#if (UDMA_TEST_SOC_OCMC_MEM_PRESENT == 1)  */
#ifndef UDMA_TEST_SOC_PRESILICON
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3485U,
        .tcName     = UDMA_TEST_MAIN_BC_TCNAME_PREFIX "Blockcpy DDR 1MB to DDR 1MB performance test",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_ENABLE,
        .tcType     = (UDMA_TCT_FULL | UDMA_TCT_FUNCTIONAL | UDMA_TCT_PERFORMANCE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = UDMA_TEST_PERF_LOOP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_BLK_CPY},
        .testFxnPtr = {&udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_INST_ID_MAIN_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_INTR_DEF},
        .qdepth     = {UDMA_TEST_PERF_QDEPTH},
        .icnt       = {
                        {1*KB, 1U, 1*KB, 1U},
                      },
        .dicnt      = {
                        {1*KB, 1U, 1*KB, 1U},
                      },
        .dim        = {
                        {1*KB, 1*KB, 0U},
                      },
        .ddim       = {
                        {1*KB, 1*KB, 0U},
                      },
        .heapIdSrc  = {UTILS_MEM_HEAP_ID_DDR},
        .heapIdDest = {UTILS_MEM_HEAP_ID_DDR},
        .srcBufSize = {1*MB},
        .destBufSize= {1*MB},
        .runFlag    = (UDMA_TEST_RF_MAIN_BC),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3486U,
        .tcName     = UDMA_TEST_MAIN_BC_TCNAME_PREFIX "2D Blockcpy MSMC circular 1KB to DDR 1MB performance test",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_ENABLE,
        .tcType     = (UDMA_TCT_FULL | UDMA_TCT_FUNCTIONAL | UDMA_TCT_PERFORMANCE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = UDMA_TEST_PERF_LOOP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_BLK_CPY},
        .testFxnPtr = {&udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_INST_ID_MAIN_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_INTR_DEF},
        .qdepth     = {UDMA_TEST_DEF_QDEPTH},
        .icnt       = {
                        {1*KB, 1U, 1*KB, 1U}
                      },
        .dicnt      = {
                        {1*KB, 1U, 1*KB, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {1*KB, 1*KB, 0U}
                      },
        .heapIdSrc  = {UTILS_MEM_HEAP_ID_MSMC},
        .heapIdDest = {UTILS_MEM_HEAP_ID_DDR},
        .srcBufSize = {1*KB},
        .destBufSize= {1*MB},
        .runFlag    = (UDMA_TEST_RF_MAIN_BC),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3487U,
        .tcName     = UDMA_TEST_MAIN_BC_TCNAME_PREFIX "2D Blockcpy DDR 1MB to MSMC circular 1KB performance test",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_ENABLE,
        .tcType     = (UDMA_TCT_FULL | UDMA_TCT_FUNCTIONAL | UDMA_TCT_PERFORMANCE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = UDMA_TEST_PERF_LOOP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_BLK_CPY},
        .testFxnPtr = {&udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_INST_ID_MAIN_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_INTR_DEF},
        .qdepth     = {UDMA_TEST_DEF_QDEPTH},
        .icnt       = {
                        {1*KB, 1U, 1*KB, 1U}
                      },
        .dicnt      = {
                        {1*KB, 1U, 1*KB, 1U}
                      },
        .dim        = {
                        {1*KB, 1*KB, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {UTILS_MEM_HEAP_ID_DDR},
        .heapIdDest = {UTILS_MEM_HEAP_ID_MSMC},
        .srcBufSize = {1*MB},
        .destBufSize= {1*KB},
        .runFlag    = (UDMA_TEST_RF_MAIN_BC),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 12918U,
        .tcName     = UDMA_TEST_MAIN_BC_TCNAME_PREFIX "2D Blockcpy OCMC circular 1KB to DDR 1MB performance test",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_ENABLE,
        .tcType     = (UDMA_TCT_FULL | UDMA_TCT_FUNCTIONAL | UDMA_TCT_PERFORMANCE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = UDMA_TEST_PERF_LOOP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_BLK_CPY},
        .testFxnPtr = {&udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_INST_ID_MAIN_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_INTR_DEF},
        .qdepth     = {UDMA_TEST_DEF_QDEPTH},
        .icnt       = {
                        {1*KB, 1U, 1*KB, 1U}
                      },
        .dicnt      = {
                        {1*KB, 1U, 1*KB, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {1*KB, 1*KB, 0U}
                      },
        .heapIdSrc  = {UTILS_MEM_HEAP_ID_INTERNAL},
        .heapIdDest = {UTILS_MEM_HEAP_ID_DDR},
        .srcBufSize = {1*KB},
        .destBufSize= {1*MB},
        .runFlag    = (UDMA_TEST_RF_MAIN_BC),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 12919U,
        .tcName     = UDMA_TEST_MAIN_BC_TCNAME_PREFIX "2D Blockcpy DDR 1MB to OCMC circular 1KB performance test",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_ENABLE,
        .tcType     = (UDMA_TCT_FULL | UDMA_TCT_FUNCTIONAL | UDMA_TCT_PERFORMANCE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = UDMA_TEST_PERF_LOOP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_BLK_CPY},
        .testFxnPtr = {&udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_INST_ID_MAIN_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_INTR_DEF},
        .qdepth     = {UDMA_TEST_DEF_QDEPTH},
        .icnt       = {
                        {1*KB, 1U, 1*KB, 1U}
                      },
        .dicnt      = {
                        {1*KB, 1U, 1*KB, 1U}
                      },
        .dim        = {
                        {1*KB, 1*KB, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {UTILS_MEM_HEAP_ID_DDR},
        .heapIdDest = {UTILS_MEM_HEAP_ID_INTERNAL},
        .srcBufSize = {1*MB},
        .destBufSize= {1*KB},
        .runFlag    = (UDMA_TEST_RF_MAIN_BC),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3488U,
        .tcName     = UDMA_TEST_MAIN_BC_TCNAME_PREFIX "2D Blockcpy MSMC circular 1KB to MSMC circular 1KB performance test",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_ENABLE,
        .tcType     = (UDMA_TCT_FULL | UDMA_TCT_FUNCTIONAL | UDMA_TCT_PERFORMANCE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = UDMA_TEST_PERF_LOOP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_BLK_CPY},
        .testFxnPtr = {&udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_INST_ID_MAIN_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_INTR_DEF},
        .qdepth     = {UDMA_TEST_DEF_QDEPTH},
        .icnt       = {
                        {1*KB, 1U, 1*KB, 1U}
                      },
        .dicnt      = {
                        {1*KB, 1U, 1*KB, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {UTILS_MEM_HEAP_ID_MSMC},
        .heapIdDest = {UTILS_MEM_HEAP_ID_MSMC},
        .srcBufSize = {1*KB},
        .destBufSize= {1*KB},
        .runFlag    = (UDMA_TEST_RF_MAIN_BC),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
#if (UDMA_TEST_SOC_OCMC_MEM_PRESENT == 1) 
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3489U,
        .tcName     = UDMA_TEST_MCU_BC_TCNAME_PREFIX "2D Blockcpy OCMC circular 1KB to OCMC circular 1KB performance test",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_ENABLE,
        .tcType     = (UDMA_TCT_FULL | UDMA_TCT_FUNCTIONAL | UDMA_TCT_PERFORMANCE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = UDMA_TEST_PERF_LOOP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_BLK_CPY},
        .testFxnPtr = {&udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_INST_ID_MCU_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_INTR_DEF},
        .qdepth     = {UDMA_TEST_DEF_QDEPTH},
        .icnt       = {
                        {1*KB, 1U, 1*KB, 1U}
                      },
        .dicnt      = {
                        {1*KB, 1U, 1*KB, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {UTILS_MEM_HEAP_ID_INTERNAL},
        .heapIdDest = {UTILS_MEM_HEAP_ID_INTERNAL},
        .srcBufSize = {1*KB},
        .destBufSize= {1*KB},
        .runFlag    = (UDMA_TEST_RF_MCU_BC_INTERNAL_MEM),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
#endif /* #if (UDMA_TEST_SOC_OCMC_MEM_PRESENT == 1)  */
#if (UDMA_SOC_CFG_UDMAP_PRESENT == 1)
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3490U,
        .tcName     = UDMA_TEST_MCU_BC_TCNAME_PREFIX "Blockcpy DDR 1MB to DDR 1MB performance test",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_ENABLE,
        .tcType     = (UDMA_TCT_FULL | UDMA_TCT_FUNCTIONAL | UDMA_TCT_PERFORMANCE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = UDMA_TEST_PERF_LOOP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_BLK_CPY},
        .testFxnPtr = {&udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_INST_ID_MCU_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_INTR_DEF},
        .qdepth     = {UDMA_TEST_PERF_QDEPTH},
        .icnt       = {
                        {1*KB, 1U, 1*KB, 1U},
                      },
        .dicnt      = {
                        {1*KB, 1U, 1*KB, 1U},
                      },
        .dim        = {
                        {1*KB, 1*KB, 0U},
                      },
        .ddim       = {
                        {1*KB, 1*KB, 0U},
                      },
        .heapIdSrc  = {UTILS_MEM_HEAP_ID_DDR},
        .heapIdDest = {UTILS_MEM_HEAP_ID_DDR},
        .srcBufSize = {1*MB},
        .destBufSize= {1*MB},
        .runFlag    = (UDMA_TEST_RF_MCU_BC),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3491U,
        .tcName     = UDMA_TEST_MCU_BC_TCNAME_PREFIX "2D Blockcpy MSMC circular 1KB to DDR 1MB performance test",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_ENABLE,
        .tcType     = (UDMA_TCT_FULL | UDMA_TCT_FUNCTIONAL | UDMA_TCT_PERFORMANCE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = UDMA_TEST_PERF_LOOP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_BLK_CPY},
        .testFxnPtr = {&udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_INST_ID_MCU_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_INTR_DEF},
        .qdepth     = {UDMA_TEST_DEF_QDEPTH},
        .icnt       = {
                        {1*KB, 1U, 1*KB, 1U}
                      },
        .dicnt      = {
                        {1*KB, 1U, 1*KB, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {1*KB, 1*KB, 0U}
                      },
        .heapIdSrc  = {UTILS_MEM_HEAP_ID_MSMC},
        .heapIdDest = {UTILS_MEM_HEAP_ID_DDR},
        .srcBufSize = {1*KB},
        .destBufSize= {1*MB},
        .runFlag    = (UDMA_TEST_RF_MCU_BC),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3492U,
        .tcName     = UDMA_TEST_MCU_BC_TCNAME_PREFIX "2D Blockcpy DDR 1MB to MSMC circular 1KB performance test",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_ENABLE,
        .tcType     = (UDMA_TCT_FULL | UDMA_TCT_FUNCTIONAL | UDMA_TCT_PERFORMANCE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = UDMA_TEST_PERF_LOOP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_BLK_CPY},
        .testFxnPtr = {&udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_INST_ID_MCU_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_INTR_DEF},
        .qdepth     = {UDMA_TEST_DEF_QDEPTH},
        .icnt       = {
                        {1*KB, 1U, 1*KB, 1U}
                      },
        .dicnt      = {
                        {1*KB, 1U, 1*KB, 1U}
                      },
        .dim        = {
                        {1*KB, 1*KB, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {UTILS_MEM_HEAP_ID_DDR},
        .heapIdDest = {UTILS_MEM_HEAP_ID_MSMC},
        .srcBufSize = {1*MB},
        .destBufSize= {1*KB},
        .runFlag    = (UDMA_TEST_RF_MCU_BC),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3493U,
        .tcName     = UDMA_TEST_MCU_BC_TCNAME_PREFIX "2D Blockcpy MSMC circular 1KB to MSMC circular 1KB performance test",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_ENABLE,
        .tcType     = (UDMA_TCT_FULL | UDMA_TCT_FUNCTIONAL | UDMA_TCT_PERFORMANCE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = UDMA_TEST_PERF_LOOP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_BLK_CPY},
        .testFxnPtr = {&udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_INST_ID_MCU_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_INTR_DEF},
        .qdepth     = {UDMA_TEST_DEF_QDEPTH},
        .icnt       = {
                        {1*KB, 1U, 1*KB, 1U}
                      },
        .dicnt      = {
                        {1*KB, 1U, 1*KB, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {UTILS_MEM_HEAP_ID_MSMC},
        .heapIdDest = {UTILS_MEM_HEAP_ID_MSMC},
        .srcBufSize = {1*KB},
        .destBufSize= {1*KB},
        .runFlag    = (UDMA_TEST_RF_MCU_BC),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
#endif /* #if (UDMA_SOC_CFG_UDMAP_PRESENT == 1) */
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3494U,
        .tcName     = UDMA_TEST_MAIN_BC_TCNAME_PREFIX "Blockcpy DDR 1MB to DDR 1MB from multiple tasks",
        .disableInfo= NULL,
        .printEnable= PRINT_DISABLE,
        .prfEnable  = PRF_ENABLE,
        .tcType     = (UDMA_TCT_FULL | UDMA_TCT_FUNCTIONAL | UDMA_TCT_PERFORMANCE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = UDMA_TEST_LOOP_CNT_MT_SOC,
        .numTasks   = UDMA_TEST_MAX_MAIN_BC_CH,
        .testType   = {UDMA_TT_BLK_CPY, UDMA_TT_BLK_CPY, UDMA_TT_BLK_CPY, UDMA_TT_BLK_CPY},
        .testFxnPtr = {&udmaTestBlkcpyTc, &udmaTestBlkcpyTc, &udmaTestBlkcpyTc, &udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE, PACING_NONE, PACING_NONE, PACING_NONE},
        .numCh      = {1U, 1U, 1U, 1U},
        .instId     = {UDMA_TEST_INST_ID_MAIN_BC, UDMA_TEST_INST_ID_MAIN_BC, UDMA_TEST_INST_ID_MAIN_BC, UDMA_TEST_INST_ID_MAIN_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_INTR_DEF, UDMA_TEST_CH_PRMID_INTR_DEF, UDMA_TEST_CH_PRMID_INTR_DEF, UDMA_TEST_CH_PRMID_INTR_DEF},
        .qdepth     = {UDMA_TEST_PERF_QDEPTH, UDMA_TEST_PERF_QDEPTH, UDMA_TEST_PERF_QDEPTH, UDMA_TEST_PERF_QDEPTH},
        .icnt       = {
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                      },
        .dicnt      = {
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                      },
        .dim        = {
                        {1*KB, 1*KB, 0U},
                        {1*KB, 1*KB, 0U},
                        {1*KB, 1*KB, 0U},
                        {1*KB, 1*KB, 0U},
                      },
        .ddim       = {
                        {1*KB, 1*KB, 0U},
                        {1*KB, 1*KB, 0U},
                        {1*KB, 1*KB, 0U},
                        {1*KB, 1*KB, 0U},
                      },
        .heapIdSrc  = {UTILS_MEM_HEAP_ID_DDR, UTILS_MEM_HEAP_ID_DDR, UTILS_MEM_HEAP_ID_DDR, UTILS_MEM_HEAP_ID_DDR},
        .heapIdDest = {UTILS_MEM_HEAP_ID_DDR, UTILS_MEM_HEAP_ID_DDR, UTILS_MEM_HEAP_ID_DDR, UTILS_MEM_HEAP_ID_DDR},
        .srcBufSize = {1*MB, 1*MB, 1*MB, 1*MB},
        .destBufSize= {1*MB, 1*MB, 1*MB, 1*MB},
        .runFlag    = (UDMA_TEST_RF_MAIN_BC_MT),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
#if (UDMA_SOC_CFG_UDMAP_PRESENT == 1)
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3495U,
        .tcName     = UDMA_TEST_MCU_BC_TCNAME_PREFIX "Blockcpy DDR 1MB to DDR 1MB from multiple tasks",
        .disableInfo= NULL,
        .printEnable= PRINT_DISABLE,
        .prfEnable  = PRF_ENABLE,
        .tcType     = (UDMA_TCT_FULL | UDMA_TCT_FUNCTIONAL | UDMA_TCT_PERFORMANCE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = UDMA_TEST_PERF_LOOP_CNT,
        .numTasks   = UDMA_TEST_MAX_MCU_BC_CH,
        .testType   = {UDMA_TT_BLK_CPY, UDMA_TT_BLK_CPY, UDMA_TT_BLK_CPY, UDMA_TT_BLK_CPY},
        .testFxnPtr = {&udmaTestBlkcpyTc, &udmaTestBlkcpyTc, &udmaTestBlkcpyTc, &udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE, PACING_NONE, PACING_NONE, PACING_NONE},
        .numCh      = {1U, 1U, 1U, 1U},
        .instId     = {UDMA_TEST_INST_ID_MCU_BC, UDMA_TEST_INST_ID_MCU_BC, UDMA_TEST_INST_ID_MCU_BC, UDMA_TEST_INST_ID_MCU_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_INTR_DEF, UDMA_TEST_CH_PRMID_INTR_DEF, UDMA_TEST_CH_PRMID_INTR_DEF, UDMA_TEST_CH_PRMID_INTR_DEF},
        .qdepth     = {UDMA_TEST_PERF_QDEPTH, UDMA_TEST_PERF_QDEPTH, UDMA_TEST_PERF_QDEPTH, UDMA_TEST_PERF_QDEPTH},
        .icnt       = {
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                      },
        .dicnt      = {
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                      },
        .dim        = {
                        {1*KB, 1*KB, 0U},
                        {1*KB, 1*KB, 0U},
                        {1*KB, 1*KB, 0U},
                        {1*KB, 1*KB, 0U},
                      },
        .ddim       = {
                        {1*KB, 1*KB, 0U},
                        {1*KB, 1*KB, 0U},
                        {1*KB, 1*KB, 0U},
                        {1*KB, 1*KB, 0U},
                      },
        .heapIdSrc  = {UTILS_MEM_HEAP_ID_DDR, UTILS_MEM_HEAP_ID_DDR, UTILS_MEM_HEAP_ID_DDR, UTILS_MEM_HEAP_ID_DDR},
        .heapIdDest = {UTILS_MEM_HEAP_ID_DDR, UTILS_MEM_HEAP_ID_DDR, UTILS_MEM_HEAP_ID_DDR, UTILS_MEM_HEAP_ID_DDR},
        .srcBufSize = {1*MB, 1*MB, 1*MB, 1*MB},
        .destBufSize= {1*MB, 1*MB, 1*MB, 1*MB},
        .runFlag    = (UDMA_TEST_RF_MCU_BC_MT),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
#endif /* #if (UDMA_SOC_CFG_UDMAP_PRESENT == 1) */
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3496U,
        .tcName     = UDMA_TEST_MAIN_BC_TCNAME_PREFIX "Blockcpy MSMC to MSMC 1KBx1K (1MB) circular from multiple tasks",
        .disableInfo= NULL,
        .printEnable= PRINT_DISABLE,
        .prfEnable  = PRF_ENABLE,
        .tcType     = (UDMA_TCT_FULL | UDMA_TCT_FUNCTIONAL | UDMA_TCT_PERFORMANCE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = UDMA_TEST_LOOP_CNT_MT_SOC,
        .numTasks   = UDMA_TEST_MAX_MAIN_BC_CH,
        .testType   = {UDMA_TT_BLK_CPY, UDMA_TT_BLK_CPY, UDMA_TT_BLK_CPY, UDMA_TT_BLK_CPY},
        .testFxnPtr = {&udmaTestBlkcpyTc, &udmaTestBlkcpyTc, &udmaTestBlkcpyTc, &udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE, PACING_NONE, PACING_NONE, PACING_NONE},
        .numCh      = {1U, 1U, 1U, 1U},
        .instId     = {UDMA_TEST_INST_ID_MAIN_BC, UDMA_TEST_INST_ID_MAIN_BC, UDMA_TEST_INST_ID_MAIN_BC, UDMA_TEST_INST_ID_MAIN_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_INTR_DEF, UDMA_TEST_CH_PRMID_INTR_DEF, UDMA_TEST_CH_PRMID_INTR_DEF, UDMA_TEST_CH_PRMID_INTR_DEF},
        .qdepth     = {UDMA_TEST_PERF_QDEPTH, UDMA_TEST_PERF_QDEPTH, UDMA_TEST_PERF_QDEPTH, UDMA_TEST_PERF_QDEPTH},
        .icnt       = {
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                      },
        .dicnt      = {
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                      },
        .dim        = {
                        {0U, 0U, 0U},
                        {0U, 0U, 0U},
                        {0U, 0U, 0U},
                        {0U, 0U, 0U},
                      },
        .ddim       = {
                        {0U, 0U, 0U},
                        {0U, 0U, 0U},
                        {0U, 0U, 0U},
                        {0U, 0U, 0U},
                      },
        .heapIdSrc  = {UTILS_MEM_HEAP_ID_MSMC, UTILS_MEM_HEAP_ID_MSMC, UTILS_MEM_HEAP_ID_MSMC, UTILS_MEM_HEAP_ID_MSMC},
        .heapIdDest = {UTILS_MEM_HEAP_ID_MSMC, UTILS_MEM_HEAP_ID_MSMC, UTILS_MEM_HEAP_ID_MSMC, UTILS_MEM_HEAP_ID_MSMC},
        .srcBufSize = {1*KB, 1*KB, 1*KB, 1*KB},
        .destBufSize= {1*KB, 1*KB, 1*KB, 1*KB},
        .runFlag    = (UDMA_TEST_RF_MAIN_BC_MT),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
#if (UDMA_SOC_CFG_UDMAP_PRESENT == 1)
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3497U,
        .tcName     = UDMA_TEST_MCU_BC_TCNAME_PREFIX "Blockcpy MSMC to MSMC circular 1KBx1K (1MB) from multiple tasks",
        .disableInfo= NULL,
        .printEnable= PRINT_DISABLE,
        .prfEnable  = PRF_ENABLE,
        .tcType     = (UDMA_TCT_FULL | UDMA_TCT_FUNCTIONAL | UDMA_TCT_PERFORMANCE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = UDMA_TEST_PERF_LOOP_CNT,
        .numTasks   = UDMA_TEST_MAX_MCU_BC_CH,
        .testType   = {UDMA_TT_BLK_CPY, UDMA_TT_BLK_CPY, UDMA_TT_BLK_CPY, UDMA_TT_BLK_CPY},
        .testFxnPtr = {&udmaTestBlkcpyTc, &udmaTestBlkcpyTc, &udmaTestBlkcpyTc, &udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE, PACING_NONE, PACING_NONE, PACING_NONE},
        .numCh      = {1U, 1U, 1U, 1U},
        .instId     = {UDMA_TEST_INST_ID_MCU_BC, UDMA_TEST_INST_ID_MCU_BC, UDMA_TEST_INST_ID_MCU_BC, UDMA_TEST_INST_ID_MCU_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_INTR_DEF, UDMA_TEST_CH_PRMID_INTR_DEF, UDMA_TEST_CH_PRMID_INTR_DEF, UDMA_TEST_CH_PRMID_INTR_DEF},
        .qdepth     = {UDMA_TEST_PERF_QDEPTH, UDMA_TEST_PERF_QDEPTH, UDMA_TEST_PERF_QDEPTH, UDMA_TEST_PERF_QDEPTH},
        .icnt       = {
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                      },
        .dicnt      = {
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                      },
        .dim        = {
                        {0U, 0U, 0U},
                        {0U, 0U, 0U},
                        {0U, 0U, 0U},
                        {0U, 0U, 0U},
                      },
        .ddim       = {
                        {0U, 0U, 0U},
                        {0U, 0U, 0U},
                        {0U, 0U, 0U},
                        {0U, 0U, 0U},
                      },
        .heapIdSrc  = {UTILS_MEM_HEAP_ID_MSMC, UTILS_MEM_HEAP_ID_MSMC, UTILS_MEM_HEAP_ID_MSMC, UTILS_MEM_HEAP_ID_MSMC},
        .heapIdDest = {UTILS_MEM_HEAP_ID_MSMC, UTILS_MEM_HEAP_ID_MSMC, UTILS_MEM_HEAP_ID_MSMC, UTILS_MEM_HEAP_ID_MSMC},
        .srcBufSize = {1*KB, 1*KB, 1*KB, 1*KB},
        .destBufSize= {1*KB, 1*KB, 1*KB, 1*KB},
        .runFlag    = (UDMA_TEST_RF_MCU_BC_MT),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
#endif /* #if (UDMA_SOC_CFG_UDMAP_PRESENT == 1) */
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3498U,
        .tcName     = UDMA_TEST_MAIN_BC_TCNAME_PREFIX "2D Blockcpy DDR 4MB to MSMC circular 4KB at 20ms pacing for 10 seconds",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_ENABLE,
        .tcType     = (UDMA_TCT_FULL | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = 500U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_BLK_CPY},
        .testFxnPtr = {&udmaTestBlkcpyTc},
        .pacingTime = {DEF_PACING},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_INST_ID_MAIN_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_INTR_DEF},
        .qdepth     = {UDMA_TEST_DEF_QDEPTH},
        .icnt       = {
                        {1*KB, 4U, 1*KB, 1U}
                      },
        .dicnt      = {
                        {1*KB, 4U, 1*KB, 1U}
                      },
        .dim        = {
                        {1*KB, 4*KB, 4*MB}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {UTILS_MEM_HEAP_ID_DDR},
        .heapIdDest = {UTILS_MEM_HEAP_ID_MSMC},
        .srcBufSize = {4U*MB},
        .destBufSize= {4U*KB},
        .runFlag    = (UDMA_TEST_RF_MAIN_BC_PACING),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
#endif /* #ifndef UDMA_TEST_SOC_PRESILICON */
#ifdef UDMA_UTC_ID_MSMC_DRU0
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3499U,
        .tcName     = "DRU Indirect Blockcpy DDR to DDR in polling mode",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = USE_DEF_LP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_DRU_INDIRECT},
        .testFxnPtr = {&udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_INST_ID_MAIN_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DRU_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_DRU | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3500U,
        .tcName     = "DRU Indirect Blockcpy DDR to DDR in interrupt mode",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = USE_DEF_LP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_DRU_INDIRECT},
        .testFxnPtr = {&udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_INST_ID_MAIN_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DRU_INTR_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_DRU | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
#ifndef UDMA_TEST_SOC_PRESILICON    
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3501U,
        .tcName     = "DRU Indirect Blockcpy DDR 1MB to DDR 1MB performance test",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_ENABLE,
        .tcType     = (UDMA_TCT_FULL | UDMA_TCT_FUNCTIONAL | UDMA_TCT_PERFORMANCE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = UDMA_TEST_PERF_LOOP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_DRU_INDIRECT},
        .testFxnPtr = {&udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_INST_ID_MAIN_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DRU_INTR_DEF},
        .qdepth     = {UDMA_TEST_PERF_QDEPTH},
        .icnt       = {
                        {1*KB, 1U, 1*KB, 1U},
                      },
        .dicnt      = {
                        {1*KB, 1U, 1*KB, 1U},
                      },
        .dim        = {
                        {1*KB, 1*KB, 0U},
                      },
        .ddim       = {
                        {1*KB, 1*KB, 0U},
                      },
        .heapIdSrc  = {UTILS_MEM_HEAP_ID_DDR},
        .heapIdDest = {UTILS_MEM_HEAP_ID_DDR},
        .srcBufSize = {1*MB},
        .destBufSize= {1*MB},
        .runFlag    = (UDMA_TEST_RF_DRU),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3502U,
        .tcName     = "DRU Indirect 2D Blockcpy MSMC circular 1KB to DDR 1MB performance test",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_ENABLE,
        .tcType     = (UDMA_TCT_FULL | UDMA_TCT_FUNCTIONAL | UDMA_TCT_PERFORMANCE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = UDMA_TEST_PERF_LOOP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_DRU_INDIRECT},
        .testFxnPtr = {&udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_INST_ID_MAIN_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DRU_INTR_DEF},
        .qdepth     = {UDMA_TEST_DEF_QDEPTH},
        .icnt       = {
                        {1*KB, 1U, 1*KB, 1U}
                      },
        .dicnt      = {
                        {1*KB, 1U, 1*KB, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {1*KB, 1*KB, 0U}
                      },
        .heapIdSrc  = {UTILS_MEM_HEAP_ID_MSMC},
        .heapIdDest = {UTILS_MEM_HEAP_ID_DDR},
        .srcBufSize = {1*KB},
        .destBufSize= {1*MB},
        .runFlag    = (UDMA_TEST_RF_DRU),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3503U,
        .tcName     = "DRU Indirect 2D Blockcpy DDR 1MB to MSMC circular 1KB performance test",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_ENABLE,
        .tcType     = (UDMA_TCT_FULL | UDMA_TCT_FUNCTIONAL | UDMA_TCT_PERFORMANCE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = UDMA_TEST_PERF_LOOP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_DRU_INDIRECT},
        .testFxnPtr = {&udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_INST_ID_MAIN_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DRU_INTR_DEF},
        .qdepth     = {UDMA_TEST_DEF_QDEPTH},
        .icnt       = {
                        {1*KB, 1U, 1*KB, 1U}
                      },
        .dicnt      = {
                        {1*KB, 1U, 1*KB, 1U}
                      },
        .dim        = {
                        {1*KB, 1*KB, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {UTILS_MEM_HEAP_ID_DDR},
        .heapIdDest = {UTILS_MEM_HEAP_ID_MSMC},
        .srcBufSize = {1*MB},
        .destBufSize= {1*KB},
        .runFlag    = (UDMA_TEST_RF_DRU),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3504U,
        .tcName     = "DRU Indirect 2D Blockcpy MSMC circular 1KB to MSMC circular 1KB performance test",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_ENABLE,
        .tcType     = (UDMA_TCT_FULL | UDMA_TCT_FUNCTIONAL | UDMA_TCT_PERFORMANCE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = UDMA_TEST_PERF_LOOP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_DRU_INDIRECT},
        .testFxnPtr = {&udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_INST_ID_MAIN_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DRU_INTR_DEF},
        .qdepth     = {UDMA_TEST_DEF_QDEPTH},
        .icnt       = {
                        {1*KB, 1U, 1*KB, 1U}
                      },
        .dicnt      = {
                        {1*KB, 1U, 1*KB, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {UTILS_MEM_HEAP_ID_MSMC},
        .heapIdDest = {UTILS_MEM_HEAP_ID_MSMC},
        .srcBufSize = {1*KB},
        .destBufSize= {1*KB},
        .runFlag    = (UDMA_TEST_RF_DRU),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3505U,
        .tcName     = "DRU Indirect Blockcpy DDR 1MB to DDR 1MB from multiple tasks",
        .disableInfo= NULL,
        .printEnable= PRINT_DISABLE,
        .prfEnable  = PRF_ENABLE,
        .tcType     = (UDMA_TCT_FULL | UDMA_TCT_FUNCTIONAL | UDMA_TCT_PERFORMANCE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = UDMA_TEST_PERF_LOOP_CNT,
        .numTasks   = UDMA_TEST_MAX_DRU_CH,
        .testType   = {UDMA_TT_DRU_INDIRECT, UDMA_TT_DRU_INDIRECT, UDMA_TT_DRU_INDIRECT, UDMA_TT_DRU_INDIRECT},
        .testFxnPtr = {&udmaTestBlkcpyTc, &udmaTestBlkcpyTc, &udmaTestBlkcpyTc, &udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE, PACING_NONE, PACING_NONE, PACING_NONE},
        .numCh      = {1U, 1U, 1U, 1U},
        .instId     = {UDMA_TEST_INST_ID_MAIN_BC, UDMA_TEST_INST_ID_MAIN_BC, UDMA_TEST_INST_ID_MAIN_BC, UDMA_TEST_INST_ID_MAIN_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DRU_INTR_DEF, UDMA_TEST_CH_PRMID_DRU_INTR_DEF, UDMA_TEST_CH_PRMID_DRU_INTR_DEF, UDMA_TEST_CH_PRMID_DRU_INTR_DEF},
        .qdepth     = {UDMA_TEST_PERF_QDEPTH, UDMA_TEST_PERF_QDEPTH, UDMA_TEST_PERF_QDEPTH, UDMA_TEST_PERF_QDEPTH},
        .icnt       = {
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                      },
        .dicnt      = {
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                      },
        .dim        = {
                        {1*KB, 1*KB, 0U},
                        {1*KB, 1*KB, 0U},
                        {1*KB, 1*KB, 0U},
                        {1*KB, 1*KB, 0U},
                      },
        .ddim       = {
                        {1*KB, 1*KB, 0U},
                        {1*KB, 1*KB, 0U},
                        {1*KB, 1*KB, 0U},
                        {1*KB, 1*KB, 0U},
                      },
        .heapIdSrc  = {UTILS_MEM_HEAP_ID_DDR, UTILS_MEM_HEAP_ID_DDR, UTILS_MEM_HEAP_ID_DDR, UTILS_MEM_HEAP_ID_DDR},
        .heapIdDest = {UTILS_MEM_HEAP_ID_DDR, UTILS_MEM_HEAP_ID_DDR, UTILS_MEM_HEAP_ID_DDR, UTILS_MEM_HEAP_ID_DDR},
        .srcBufSize = {1*MB, 1*MB, 1*MB, 1*MB},
        .destBufSize= {1*MB, 1*MB, 1*MB, 1*MB},
        .runFlag    = (UDMA_TEST_RF_DRU_MT),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3506U,
        .tcName     = "DRU Indirect Blockcpy MSMC to MSMC 1KBx1K (1MB) circular from multiple tasks",
        .disableInfo= NULL,
        .printEnable= PRINT_DISABLE,
        .prfEnable  = PRF_ENABLE,
        .tcType     = (UDMA_TCT_FULL | UDMA_TCT_FUNCTIONAL | UDMA_TCT_PERFORMANCE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = UDMA_TEST_PERF_LOOP_CNT,
        .numTasks   = UDMA_TEST_MAX_DRU_CH,
        .testType   = {UDMA_TT_DRU_INDIRECT, UDMA_TT_DRU_INDIRECT, UDMA_TT_DRU_INDIRECT, UDMA_TT_DRU_INDIRECT},
        .testFxnPtr = {&udmaTestBlkcpyTc, &udmaTestBlkcpyTc, &udmaTestBlkcpyTc, &udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE, PACING_NONE, PACING_NONE, PACING_NONE},
        .numCh      = {1U, 1U, 1U, 1U},
        .instId     = {UDMA_TEST_INST_ID_MAIN_BC, UDMA_TEST_INST_ID_MAIN_BC, UDMA_TEST_INST_ID_MAIN_BC, UDMA_TEST_INST_ID_MAIN_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DRU_INTR_DEF, UDMA_TEST_CH_PRMID_DRU_INTR_DEF, UDMA_TEST_CH_PRMID_DRU_INTR_DEF, UDMA_TEST_CH_PRMID_DRU_INTR_DEF},
        .qdepth     = {UDMA_TEST_PERF_QDEPTH, UDMA_TEST_PERF_QDEPTH, UDMA_TEST_PERF_QDEPTH, UDMA_TEST_PERF_QDEPTH},
        .icnt       = {
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                      },
        .dicnt      = {
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                      },
        .dim        = {
                        {0U, 0U, 0U},
                        {0U, 0U, 0U},
                        {0U, 0U, 0U},
                        {0U, 0U, 0U},
                      },
        .ddim       = {
                        {0U, 0U, 0U},
                        {0U, 0U, 0U},
                        {0U, 0U, 0U},
                        {0U, 0U, 0U},
                      },
        .heapIdSrc  = {UTILS_MEM_HEAP_ID_MSMC, UTILS_MEM_HEAP_ID_MSMC, UTILS_MEM_HEAP_ID_MSMC, UTILS_MEM_HEAP_ID_MSMC},
        .heapIdDest = {UTILS_MEM_HEAP_ID_MSMC, UTILS_MEM_HEAP_ID_MSMC, UTILS_MEM_HEAP_ID_MSMC, UTILS_MEM_HEAP_ID_MSMC},
        .srcBufSize = {1*KB, 1*KB, 1*KB, 1*KB},
        .destBufSize= {1*KB, 1*KB, 1*KB, 1*KB},
        .runFlag    = (UDMA_TEST_RF_DRU_MT),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
#endif /* #ifndef UDMA_TEST_SOC_PRESILICON */
//Enable after adding testcase in Qmetry
#if 0
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 9000U,
        .tcName     = "DRU Blockcpy circular 1KB DDR to DDR 1KB ICNT1 TR event type test",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_DRU_INDIRECT},
        .testFxnPtr = {&udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_INST_ID_MAIN_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DRU_EVENTSIZE_ICNT1},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {1*KB, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {1*KB, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {1*KB, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {1*KB},
        .destBufSize= {1*KB},
        .runFlag    = (UDMA_TEST_RF_DRU),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 9001U,
        .tcName     = "DRU Blockcpy circular 1KB DDR to DDR 1KB ICNT2 TR event type test",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_DRU_INDIRECT},
        .testFxnPtr = {&udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_INST_ID_MAIN_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DRU_EVENTSIZE_ICNT2},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {1*KB, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {1*KB, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {1*KB, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {1*KB},
        .destBufSize= {1*KB},
        .runFlag    = (UDMA_TEST_RF_DRU),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 9002U,
        .tcName     = "DRU Blockcpy circular 1KB DDR to DDR 1KB ICNT3 TR event type test",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_DRU_INDIRECT},
        .testFxnPtr = {&udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_INST_ID_MAIN_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DRU_EVENTSIZE_ICNT3},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {1*KB, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {1*KB, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {1*KB, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {1*KB},
        .destBufSize= {1*KB},
        .runFlag    = (UDMA_TEST_RF_DRU),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 9003U,
        .tcName     = "DRU DDR to DDR SW global 0 trigger test in polling mode",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = USE_DEF_LP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_DRU_INDIRECT},
        .testFxnPtr = {&udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_INST_ID_MAIN_0},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DRU_TRIGGER_GLOBAL0},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_DRU),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 9004U,
        .tcName     = "DRU DDR to DDR SW global 0 trigger test in interrupt mode",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = USE_DEF_LP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_DRU_INDIRECT},
        .testFxnPtr = {&udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_INST_ID_MAIN_0},
        .chPrmId    = {UDMA_TEST_CH_PRMID_TRIGGER_GLOBAL0_INTR},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_DRU),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
#endif /* #if 0 */
#endif  /* #if defined (UDMA_UTC_ID_MSMC_DRU0) */
#if (UDMA_SOC_CFG_PROXY_PRESENT == 1)
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3507U,
        .tcName     = "Proxy queue and dequeue testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 5U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&udmaTestRingProxyTc},
        /* All other below parameters not used in this testcase except ring params */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_NONE,
    },
#endif /* #if (UDMA_SOC_CFG_PROXY_PRESENT == 1) */
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3508U,
        .tcName     = "Ring flush API testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&udmaTestRingFlushTc},
        /* All other below parameters not used in this testcase except ring params */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_NONE,
    },
#if (UDMA_SOC_CFG_RA_NORMAL_PRESENT == 1)    
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3509U,
        .tcName     = "Ring event testcase in interrupt mode",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 10U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&udmaTestRingEventTc},
        /* All other below parameters not used in this testcase except ring params */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_INTR,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3510U,
        .tcName     = "Ring event testcase in event polled mode",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 10U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&udmaTestRingEventTc},
        /* All other below parameters not used in this testcase except ring params */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_POLLED,
    },
#endif /* #if (UDMA_SOC_CFG_RA_NORMAL_PRESENT == 1) */
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3511U,
        .tcName     = "Ring parameter check test",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_NEGATIVE),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&udmaTestRingParamCheckTc},
        /* All other below parameters not used in this testcase except ring params */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_POLLED,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3512U,
        .tcName     = "Ring Utils Mem Size test",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&udmaTestRingUtilsMemSizeTc},
        /* All other below parameters not used in this testcase except ring params */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_POLLED,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3682U,
        .tcName     = "Ring Mem Pointer test",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&udmaTestRingMemPtrTc},
        /* All other below parameters not used in this testcase except ring params */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_POLLED,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3726U,
        .tcName     = "Ring attach and detach testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 5U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&udmaTestRingAttachTc},
        /* All other below parameters not used in this testcase except ring params */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_NONE,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 4644U,
        .tcName     = "Ring reset workaround test",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&udmaTestRingResetTc},
        /* All other below parameters not used in this testcase except ring params */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_AM65XX | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_NONE,
    },
#if (UDMA_SOC_CFG_RA_NORMAL_PRESENT == 1)
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 4693U,
        .tcName     = "Ring prime test",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&udmaTestRingPrimeTc},
        /* All other below parameters not used in this testcase except ring params */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_NONE,
    },
#endif /* #if (UDMA_SOC_CFG_RA_NORMAL_PRESENT == 1) */
#if (UDMA_SOC_CFG_RA_LCDMA_PRESENT == 1) && (UDMA_SOC_CFG_BCDMA_PRESENT == 1)
    {
        /* For LCDMA with Dual ring, 
         * Ring Prime Read checks for Reverse occupancy.
         * In this case, actual transfer should happen to populate
         * reverse occupancy count and successfully do a ring prime read.
         * This testcase, implements block copy using ring prime API's 
         * This tests only Ring Prime API's.
         * Data check and TR Responce checks are NOT carried out.*/
        .enableTest = TEST_ENABLE,
        .tcId       = 8837U,
        .tcName     = "LCDMA Ring Prime Test",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .numCh      = {1U},
        .instId     = {UDMA_TEST_INST_ID_BCDMA_BC},
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&udmaTestRingPrimeLcdmaTc},
        .qdepth     = {500U},
        .pacingTime = {PACING_NONE},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .icnt       = {
                        {16U, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {16U, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_BCDMA_BC | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_NONE,
    },
#endif /* #if (UDMA_SOC_CFG_RA_LCDMA_PRESENT == 1) && (UDMA_SOC_CFG_BCDMA_PRESENT == 1) */
#if (UDMA_SOC_CFG_RING_MON_PRESENT == 1)
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 4636U,
        .tcName     = "Ring Monitor Push and Pop Mode test",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&udmaTestRingMonPushPopTc},
        /* All other below parameters not used in this testcase except ring params */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_NONE,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 4637U,
        .tcName     = "Ring Monitor Low Threshold Mode test",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&udmaTestRingMonLowThresholdTc},
        /* All other below parameters not used in this testcase except ring params */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_NONE,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 4638U,
        .tcName     = "Ring Monitor High Threshold Mode test",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&udmaTestRingMonHighThresholdTc},
        /* All other below parameters not used in this testcase except ring params */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_NONE,
    },
#endif /* #if (UDMA_SOC_CFG_RING_MON_PRESENT == 1) */
#if (UDMA_SOC_CFG_PROXY_PRESENT == 1)
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 4238U,
        .tcName     = "Proxy queue and dequeue testcase with allocated proxy",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_ENABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL | UDMA_TCT_PERFORMANCE),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = UDMA_TEST_PERF_LOOP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&udmaTestProxyPerformanceTc},
        /* All other below parameters not used in this testcase except ring params */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_MCU_PROXY | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_NONE,
    },
#endif /* #if (UDMA_SOC_CFG_PROXY_PRESENT == 1) */
#if ((UDMA_NUM_MAPPED_TX_GROUP + UDMA_NUM_MAPPED_RX_GROUP) > 0)
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 7034U,
        .tcName     = "Mapped Flow attach and detach testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 5U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&udmaTestFlowAttachMappedTc},
        /* All other below parameters not used in this testcase except ring params */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_INST_ID_FLOW},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_FLOW | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_NONE,
    },
#else /* #if ((UDMA_NUM_MAPPED_TX_GROUP + UDMA_NUM_MAPPED_RX_GROUP) > 0) */
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3729U,
        .tcName     = "Flow attach and detach testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 5U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&udmaTestFlowAttachTc},
        /* All other below parameters not used in this testcase except ring params */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_FLOW | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_NONE,
    },
    {
        /* This tests the failure for allocating flows more than actual count.
         * So this is not applicable in case of mapped flows, which allocates only one at a time.
         * Hence the testcase is not appicable for mapped flows. */
        .enableTest = TEST_ENABLE,
        .tcId       = 4154U,
        .tcName     = "Flow alloc and free testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&udmaTestFlowAllocTc},
        /* All other below parameters not used in this testcase except ring params */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_FLOW | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_NONE,
    },
#endif /* #if ((UDMA_NUM_MAPPED_TX_GROUP + UDMA_NUM_MAPPED_RX_GROUP) > 0) */
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3513U,
        .tcName     = "PSIL and PDMA macro verification testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = USE_DEF_LP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&udmaTestPsilMacroTc},
        /* All other below parameters not used in this testcase */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3514U,
        .tcName     = "OSAL params testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = USE_DEF_LP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&udmaTestOsalPrmsTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_INTR_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3515U,
        .tcName     = "TR make utility testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = USE_DEF_LP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&udmaTestTrMakeTc},
        /* All other below parameters not used in this testcase */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_INTR_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3733U,
        .tcName     = "UDMA structure size print testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = USE_DEF_LP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&udmaTestStructSizeTc},
        /* All other below parameters not used in this testcase */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_INTR_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 9595U,
        .tcName     = "Event Disable and Enable Sanity Check Testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&udmaTestEventDisableEnableSanity},
        /* All other below parameters not used in this testcase */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_NONE,
    },
#if (UDMA_SOC_CFG_RA_NORMAL_PRESENT == 1)
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3707U,
        .tcName     = "Out of range flow ID Event testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 100U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&udmaTestEventOutOfRangeFlow},
        /* All other below parameters not used in this testcase */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_NONE,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3905,
        .tcName     = "PDK-3863 bug testcase: Udma_chDequeueTdResponse NULL check",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_FULL | UDMA_TCT_NEGATIVE),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = USE_DEF_LP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_BLK_CPY},
        .testFxnPtr = {&udmaTestBugTcPDK_3863},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
#endif /* #if (UDMA_SOC_CFG_RA_NORMAL_PRESENT == 1) */
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 3965,
        .tcName     = "Channel pause and resume testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_BLK_CPY},
        .testFxnPtr = {&udmaTestBlkcpyPauseResumeTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_INST_ID_MAIN_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_INTR_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_MAIN_BC_PAUSE | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
#if (UDMA_SOC_CFG_UDMAP_PRESENT == 1)
#ifndef UDMA_TEST_SOC_PRESILICON  
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 4100U,
        .tcName     = UDMA_TEST_MAIN_BC_TCNAME_PREFIX "HC Blockcpy DDR 1MB to DDR 1MB performance test",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_ENABLE,
        .tcType     = (UDMA_TCT_FULL | UDMA_TCT_FUNCTIONAL | UDMA_TCT_PERFORMANCE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = UDMA_TEST_PERF_LOOP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_BLK_CPY},
        .testFxnPtr = {&udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_INST_ID_MAIN_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_BLKCPY_HC_INTR_DEF},
        .qdepth     = {UDMA_TEST_PERF_QDEPTH},
        .icnt       = {
                        {1*KB, 1U, 1*KB, 1U},
                      },
        .dicnt      = {
                        {1*KB, 1U, 1*KB, 1U},
                      },
        .dim        = {
                        {1*KB, 1*KB, 0U},
                      },
        .ddim       = {
                        {1*KB, 1*KB, 0U},
                      },
        .heapIdSrc  = {UTILS_MEM_HEAP_ID_DDR},
        .heapIdDest = {UTILS_MEM_HEAP_ID_DDR},
        .srcBufSize = {1*MB},
        .destBufSize= {1*MB},
        .runFlag    = (UDMA_TEST_RF_MAIN_BC_HC),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 4101U,
        .tcName     = UDMA_TEST_MAIN_BC_TCNAME_PREFIX "2D HC Blockcpy MSMC circular 1KB to DDR 1MB performance test",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_ENABLE,
        .tcType     = (UDMA_TCT_FULL | UDMA_TCT_FUNCTIONAL | UDMA_TCT_PERFORMANCE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = UDMA_TEST_PERF_LOOP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_BLK_CPY},
        .testFxnPtr = {&udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_INST_ID_MAIN_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_BLKCPY_HC_INTR_DEF},
        .qdepth     = {UDMA_TEST_DEF_QDEPTH},
        .icnt       = {
                        {1*KB, 1U, 1*KB, 1U}
                      },
        .dicnt      = {
                        {1*KB, 1U, 1*KB, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {1*KB, 1*KB, 0U}
                      },
        .heapIdSrc  = {UTILS_MEM_HEAP_ID_MSMC},
        .heapIdDest = {UTILS_MEM_HEAP_ID_DDR},
        .srcBufSize = {1*KB},
        .destBufSize= {1*MB},
        .runFlag    = (UDMA_TEST_RF_MAIN_BC_HC),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 4102U,
        .tcName     = UDMA_TEST_MAIN_BC_TCNAME_PREFIX "2D HC Blockcpy DDR 1MB to MSMC circular 1KB performance test",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_ENABLE,
        .tcType     = (UDMA_TCT_FULL | UDMA_TCT_FUNCTIONAL | UDMA_TCT_PERFORMANCE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = UDMA_TEST_PERF_LOOP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_BLK_CPY},
        .testFxnPtr = {&udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_INST_ID_MAIN_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_BLKCPY_HC_INTR_DEF},
        .qdepth     = {UDMA_TEST_DEF_QDEPTH},
        .icnt       = {
                        {1*KB, 1U, 1*KB, 1U}
                      },
        .dicnt      = {
                        {1*KB, 1U, 1*KB, 1U}
                      },
        .dim        = {
                        {1*KB, 1*KB, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {UTILS_MEM_HEAP_ID_DDR},
        .heapIdDest = {UTILS_MEM_HEAP_ID_MSMC},
        .srcBufSize = {1*MB},
        .destBufSize= {1*KB},
        .runFlag    = (UDMA_TEST_RF_MAIN_BC_HC),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 4103U,
        .tcName     = UDMA_TEST_MAIN_BC_TCNAME_PREFIX "2D HC Blockcpy MSMC circular 1KB to MSMC circular 1KB performance test",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_ENABLE,
        .tcType     = (UDMA_TCT_FULL | UDMA_TCT_FUNCTIONAL | UDMA_TCT_PERFORMANCE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = UDMA_TEST_PERF_LOOP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_BLK_CPY},
        .testFxnPtr = {&udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_INST_ID_MAIN_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_BLKCPY_HC_INTR_DEF},
        .qdepth     = {UDMA_TEST_DEF_QDEPTH},
        .icnt       = {
                        {1*KB, 1U, 1*KB, 1U}
                      },
        .dicnt      = {
                        {1*KB, 1U, 1*KB, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {UTILS_MEM_HEAP_ID_MSMC},
        .heapIdDest = {UTILS_MEM_HEAP_ID_MSMC},
        .srcBufSize = {1*KB},
        .destBufSize= {1*KB},
        .runFlag    = (UDMA_TEST_RF_MAIN_BC_HC),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 4104U,
        .tcName     = UDMA_TEST_MAIN_BC_TCNAME_PREFIX "HC Blockcpy DDR 1MB to DDR 1MB from multiple tasks",
        .disableInfo= NULL,
        .printEnable= PRINT_DISABLE,
        .prfEnable  = PRF_ENABLE,
        .tcType     = (UDMA_TCT_FULL | UDMA_TCT_FUNCTIONAL | UDMA_TCT_PERFORMANCE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = UDMA_TEST_PERF_LOOP_CNT,
        .numTasks   = UDMA_TEST_MAX_MAIN_BC_HC_CH,
        .testType   = {UDMA_TT_BLK_CPY, UDMA_TT_BLK_CPY, UDMA_TT_BLK_CPY, UDMA_TT_BLK_CPY},
        .testFxnPtr = {&udmaTestBlkcpyTc, &udmaTestBlkcpyTc, &udmaTestBlkcpyTc, &udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE, PACING_NONE, PACING_NONE, PACING_NONE},
        .numCh      = {1U, 1U, 1U, 1U},
        .instId     = {UDMA_TEST_INST_ID_MAIN_BC, UDMA_TEST_INST_ID_MAIN_BC, UDMA_TEST_INST_ID_MAIN_BC, UDMA_TEST_INST_ID_MAIN_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_BLKCPY_HC_INTR_DEF, UDMA_TEST_CH_PRMID_BLKCPY_HC_INTR_DEF, UDMA_TEST_CH_PRMID_BLKCPY_HC_INTR_DEF, UDMA_TEST_CH_PRMID_BLKCPY_HC_INTR_DEF},
        .qdepth     = {UDMA_TEST_PERF_QDEPTH, UDMA_TEST_PERF_QDEPTH, UDMA_TEST_PERF_QDEPTH, UDMA_TEST_PERF_QDEPTH},
        .icnt       = {
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                      },
        .dicnt      = {
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                      },
        .dim        = {
                        {1*KB, 1*KB, 0U},
                        {1*KB, 1*KB, 0U},
                        {1*KB, 1*KB, 0U},
                        {1*KB, 1*KB, 0U},
                      },
        .ddim       = {
                        {1*KB, 1*KB, 0U},
                        {1*KB, 1*KB, 0U},
                        {1*KB, 1*KB, 0U},
                        {1*KB, 1*KB, 0U},
                      },
        .heapIdSrc  = {UTILS_MEM_HEAP_ID_DDR, UTILS_MEM_HEAP_ID_DDR, UTILS_MEM_HEAP_ID_DDR, UTILS_MEM_HEAP_ID_DDR},
        .heapIdDest = {UTILS_MEM_HEAP_ID_DDR, UTILS_MEM_HEAP_ID_DDR, UTILS_MEM_HEAP_ID_DDR, UTILS_MEM_HEAP_ID_DDR},
        .srcBufSize = {1*MB, 1*MB, 1*MB, 1*MB},
        .destBufSize= {1*MB, 1*MB, 1*MB, 1*MB},
        .runFlag    = (UDMA_TEST_RF_MAIN_BC_HC_MT),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 4105U,
        .tcName     = UDMA_TEST_MAIN_BC_TCNAME_PREFIX "HC Blockcpy MSMC to MSMC 1KBx1K (1MB) circular from multiple tasks",
        .disableInfo= NULL,
        .printEnable= PRINT_DISABLE,
        .prfEnable  = PRF_ENABLE,
        .tcType     = (UDMA_TCT_FULL | UDMA_TCT_FUNCTIONAL | UDMA_TCT_PERFORMANCE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = UDMA_TEST_PERF_LOOP_CNT,
        .numTasks   = UDMA_TEST_MAX_MAIN_BC_HC_CH,
        .testType   = {UDMA_TT_BLK_CPY, UDMA_TT_BLK_CPY, UDMA_TT_BLK_CPY, UDMA_TT_BLK_CPY},
        .testFxnPtr = {&udmaTestBlkcpyTc, &udmaTestBlkcpyTc, &udmaTestBlkcpyTc, &udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE, PACING_NONE, PACING_NONE, PACING_NONE},
        .numCh      = {1U, 1U, 1U, 1U},
        .instId     = {UDMA_TEST_INST_ID_MAIN_BC, UDMA_TEST_INST_ID_MAIN_BC, UDMA_TEST_INST_ID_MAIN_BC, UDMA_TEST_INST_ID_MAIN_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_BLKCPY_HC_INTR_DEF, UDMA_TEST_CH_PRMID_BLKCPY_HC_INTR_DEF, UDMA_TEST_CH_PRMID_BLKCPY_HC_INTR_DEF, UDMA_TEST_CH_PRMID_BLKCPY_HC_INTR_DEF},
        .qdepth     = {UDMA_TEST_PERF_QDEPTH, UDMA_TEST_PERF_QDEPTH, UDMA_TEST_PERF_QDEPTH, UDMA_TEST_PERF_QDEPTH},
        .icnt       = {
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                      },
        .dicnt      = {
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                      },
        .dim        = {
                        {0U, 0U, 0U},
                        {0U, 0U, 0U},
                        {0U, 0U, 0U},
                        {0U, 0U, 0U},
                      },
        .ddim       = {
                        {0U, 0U, 0U},
                        {0U, 0U, 0U},
                        {0U, 0U, 0U},
                        {0U, 0U, 0U},
                      },
        .heapIdSrc  = {UTILS_MEM_HEAP_ID_MSMC, UTILS_MEM_HEAP_ID_MSMC, UTILS_MEM_HEAP_ID_MSMC, UTILS_MEM_HEAP_ID_MSMC},
        .heapIdDest = {UTILS_MEM_HEAP_ID_MSMC, UTILS_MEM_HEAP_ID_MSMC, UTILS_MEM_HEAP_ID_MSMC, UTILS_MEM_HEAP_ID_MSMC},
        .srcBufSize = {1*KB, 1*KB, 1*KB, 1*KB},
        .destBufSize= {1*KB, 1*KB, 1*KB, 1*KB},
        .runFlag    = (UDMA_TEST_RF_MAIN_BC_HC_MT),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 4106U,
        .tcName     = UDMA_TEST_MCU_BC_TCNAME_PREFIX "HC Blockcpy DDR 1MB to DDR 1MB performance test",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_ENABLE,
        .tcType     = (UDMA_TCT_FULL | UDMA_TCT_FUNCTIONAL | UDMA_TCT_PERFORMANCE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = UDMA_TEST_PERF_LOOP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_BLK_CPY},
        .testFxnPtr = {&udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_INST_ID_MCU_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_BLKCPY_HC_INTR_DEF},
        .qdepth     = {UDMA_TEST_PERF_QDEPTH},
        .icnt       = {
                        {1*KB, 1U, 1*KB, 1U},
                      },
        .dicnt      = {
                        {1*KB, 1U, 1*KB, 1U},
                      },
        .dim        = {
                        {1*KB, 1*KB, 0U},
                      },
        .ddim       = {
                        {1*KB, 1*KB, 0U},
                      },
        .heapIdSrc  = {UTILS_MEM_HEAP_ID_DDR},
        .heapIdDest = {UTILS_MEM_HEAP_ID_DDR},
        .srcBufSize = {1*MB},
        .destBufSize= {1*MB},
        .runFlag    = (UDMA_TEST_RF_MCU_BC_HC),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 4107U,
        .tcName     = UDMA_TEST_MCU_BC_TCNAME_PREFIX "2D HC Blockcpy MSMC circular 1KB to DDR 1MB performance test",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_ENABLE,
        .tcType     = (UDMA_TCT_FULL | UDMA_TCT_FUNCTIONAL | UDMA_TCT_PERFORMANCE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = UDMA_TEST_PERF_LOOP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_BLK_CPY},
        .testFxnPtr = {&udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_INST_ID_MCU_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_BLKCPY_HC_INTR_DEF},
        .qdepth     = {UDMA_TEST_DEF_QDEPTH},
        .icnt       = {
                        {1*KB, 1U, 1*KB, 1U}
                      },
        .dicnt      = {
                        {1*KB, 1U, 1*KB, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {1*KB, 1*KB, 0U}
                      },
        .heapIdSrc  = {UTILS_MEM_HEAP_ID_MSMC},
        .heapIdDest = {UTILS_MEM_HEAP_ID_DDR},
        .srcBufSize = {1*KB},
        .destBufSize= {1*MB},
        .runFlag    = (UDMA_TEST_RF_MCU_BC_HC),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 4108U,
        .tcName     = UDMA_TEST_MCU_BC_TCNAME_PREFIX "2D HC Blockcpy DDR 1MB to MSMC circular 1KB performance test",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_ENABLE,
        .tcType     = (UDMA_TCT_FULL | UDMA_TCT_FUNCTIONAL | UDMA_TCT_PERFORMANCE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = UDMA_TEST_PERF_LOOP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_BLK_CPY},
        .testFxnPtr = {&udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_INST_ID_MCU_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_BLKCPY_HC_INTR_DEF},
        .qdepth     = {UDMA_TEST_DEF_QDEPTH},
        .icnt       = {
                        {1*KB, 1U, 1*KB, 1U}
                      },
        .dicnt      = {
                        {1*KB, 1U, 1*KB, 1U}
                      },
        .dim        = {
                        {1*KB, 1*KB, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {UTILS_MEM_HEAP_ID_DDR},
        .heapIdDest = {UTILS_MEM_HEAP_ID_MSMC},
        .srcBufSize = {1*MB},
        .destBufSize= {1*KB},
        .runFlag    = (UDMA_TEST_RF_MCU_BC_HC),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 4109U,
        .tcName     = UDMA_TEST_MCU_BC_TCNAME_PREFIX "2D HC Blockcpy MSMC circular 1KB to MSMC circular 1KB performance test",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_ENABLE,
        .tcType     = (UDMA_TCT_FULL | UDMA_TCT_FUNCTIONAL | UDMA_TCT_PERFORMANCE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = UDMA_TEST_PERF_LOOP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_BLK_CPY},
        .testFxnPtr = {&udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_INST_ID_MCU_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_BLKCPY_HC_INTR_DEF},
        .qdepth     = {UDMA_TEST_DEF_QDEPTH},
        .icnt       = {
                        {1*KB, 1U, 1*KB, 1U}
                      },
        .dicnt      = {
                        {1*KB, 1U, 1*KB, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {UTILS_MEM_HEAP_ID_MSMC},
        .heapIdDest = {UTILS_MEM_HEAP_ID_MSMC},
        .srcBufSize = {1*KB},
        .destBufSize= {1*KB},
        .runFlag    = (UDMA_TEST_RF_MCU_BC_HC),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 4110U,
        .tcName     = UDMA_TEST_MCU_BC_TCNAME_PREFIX "HC Blockcpy DDR 1MB to DDR 1MB from multiple tasks",
        .disableInfo= NULL,
        .printEnable= PRINT_DISABLE,
        .prfEnable  = PRF_ENABLE,
        .tcType     = (UDMA_TCT_FULL | UDMA_TCT_FUNCTIONAL | UDMA_TCT_PERFORMANCE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = UDMA_TEST_PERF_LOOP_CNT,
        .numTasks   = UDMA_TEST_MAX_MCU_BC_HC_CH,
        .testType   = {UDMA_TT_BLK_CPY, UDMA_TT_BLK_CPY},
        .testFxnPtr = {&udmaTestBlkcpyTc, &udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE, PACING_NONE},
        .numCh      = {1U, 1U},
        .instId     = {UDMA_TEST_INST_ID_MCU_BC, UDMA_TEST_INST_ID_MCU_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_BLKCPY_HC_INTR_DEF, UDMA_TEST_CH_PRMID_BLKCPY_HC_INTR_DEF},
        .qdepth     = {UDMA_TEST_PERF_QDEPTH, UDMA_TEST_PERF_QDEPTH},
        .icnt       = {
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                      },
        .dicnt      = {
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                      },
        .dim        = {
                        {1*KB, 1*KB, 0U},
                        {1*KB, 1*KB, 0U},
                      },
        .ddim       = {
                        {1*KB, 1*KB, 0U},
                        {1*KB, 1*KB, 0U},
                      },
        .heapIdSrc  = {UTILS_MEM_HEAP_ID_DDR, UTILS_MEM_HEAP_ID_DDR},
        .heapIdDest = {UTILS_MEM_HEAP_ID_DDR, UTILS_MEM_HEAP_ID_DDR},
        .srcBufSize = {1*MB, 1*MB},
        .destBufSize= {1*MB, 1*MB},
        .runFlag    = (UDMA_TEST_RF_MCU_BC_HC_MT),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 4111U,
        .tcName     = UDMA_TEST_MCU_BC_TCNAME_PREFIX "HC Blockcpy MSMC to MSMC 1KBx1K (1MB) circular from multiple tasks",
        .disableInfo= NULL,
        .printEnable= PRINT_DISABLE,
        .prfEnable  = PRF_ENABLE,
        .tcType     = (UDMA_TCT_FULL | UDMA_TCT_FUNCTIONAL | UDMA_TCT_PERFORMANCE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = UDMA_TEST_PERF_LOOP_CNT,
        .numTasks   = UDMA_TEST_MAX_MCU_BC_HC_CH,
        .testType   = {UDMA_TT_BLK_CPY, UDMA_TT_BLK_CPY},
        .testFxnPtr = {&udmaTestBlkcpyTc, &udmaTestBlkcpyTc},
        .pacingTime = {PACING_NONE, PACING_NONE},
        .numCh      = {1U, 1U},
        .instId     = {UDMA_TEST_INST_ID_MCU_BC, UDMA_TEST_INST_ID_MCU_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_BLKCPY_HC_INTR_DEF, UDMA_TEST_CH_PRMID_BLKCPY_HC_INTR_DEF},
        .qdepth     = {UDMA_TEST_PERF_QDEPTH, UDMA_TEST_PERF_QDEPTH},
        .icnt       = {
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                      },
        .dicnt      = {
                        {1*KB, 1U, 1*KB, 1U},
                        {1*KB, 1U, 1*KB, 1U},
                      },
        .dim        = {
                        {0U, 0U, 0U},
                        {0U, 0U, 0U},
                      },
        .ddim       = {
                        {0U, 0U, 0U},
                        {0U, 0U, 0U},
                      },
        .heapIdSrc  = {UTILS_MEM_HEAP_ID_MSMC, UTILS_MEM_HEAP_ID_MSMC},
        .heapIdDest = {UTILS_MEM_HEAP_ID_MSMC, UTILS_MEM_HEAP_ID_MSMC},
        .srcBufSize = {1*KB, 1*KB},
        .destBufSize= {1*KB, 1*KB},
        .runFlag    = (UDMA_TEST_RF_MCU_BC_HC_MT),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
#endif /* #ifndef UDMA_TEST_SOC_PRESILICON   */
#endif /* #if (UDMA_SOC_CFG_UDMAP_PRESENT == 1) */
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 4656U,
        .tcName     = "PDK-4654 bug testcase: UDMA deinit resource check",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_FULL | UDMA_TCT_NEGATIVE),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = USE_DEF_LP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_BLK_CPY},
        .testFxnPtr = {&udmaTestBugTcPDK_4654},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 4841U,
        .tcName     = UDMA_TEST_MAIN_BC_TCNAME_PREFIX "Blockcpy DDR to DDR in interrupt mode chaining test",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = USE_DEF_LP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_BLK_CPY},
        .testFxnPtr = {&udmaTestBlkcpyChainingTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {2U},
        .instId     = {UDMA_TEST_INST_ID_MAIN_BC, UDMA_TEST_INST_ID_MAIN_BC},
        .chPrmId    = {UDMA_TEST_CH_PRMID_INTR_DEF, UDMA_TEST_CH_PRMID_INTR_DEF},
        .qdepth     = {USE_DEF_QDEPTH, USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U},
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U},
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U},
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U},
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID, DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID, DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0, UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0, UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_CHAIN | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
#if (UDMA_SOC_CFG_RA_LCDMA_PRESENT == 1) && (UDMA_SOC_CFG_BCDMA_PRESENT == 1)
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 12498U,
        .tcName     = "BCDMA Channel Paramter Check test",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_NEGATIVE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&udmaTestBcdmaParamCheckTc},
        /* All other below parameters not used in this testcase */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_BCDMA | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_POLLED,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 12499U,
        .tcName     = "BCDMA Channel API's test",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_NEGATIVE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&udmaTestBcdmaChApiTc},
        /* All other below parameters not used in this testcase */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_BCDMA | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_POLLED,
    },
#endif
#if (UDMA_SOC_CFG_RA_LCDMA_PRESENT == 1) && (UDMA_SOC_CFG_PKTDMA_PRESENT == 1)
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 6282U,
        .tcName     = "PKTDMA Channel Paramter Check test",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_NEGATIVE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&udmaTestChPktdmaParamCheckTc},
        /* All other below parameters not used in this testcase */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_POLLED,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 6279U,
        .tcName     = "PKTDMA Channel API's test",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_NEGATIVE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&udmaTestChPktdmaChApiTc},
        /* All other below parameters not used in this testcase */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_POLLED,
    },
#endif /* #if (UDMA_SOC_CFG_RA_LCDMA_PRESENT == 1) */
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13372U,
        .tcName     = "Test UDMA Init function for invalid input parameters",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = USE_DEF_LP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&udmaTestUdmaInitNegTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_INTR_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13373U,
        .tcName     = "RmLock params test for UDMA Init function",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = USE_DEF_LP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&udmaTestUdmaInitRmLockNegTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_INTR_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13374U,
        .tcName     = "PrintLock params test for UDMA Init function",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = USE_DEF_LP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&udmaTestUdmaInitPrintLockNegTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_INTR_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13375U,
        .tcName     = "Test UDMA Init function for invalid RmSetProxy",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = USE_DEF_LP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&udmaTestUdmaInitRmSetProxyNegTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_INTR_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13378U,
        .tcName     = "Test UDMA Params Init function",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = USE_DEF_LP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&udmaTestUdmaPrmsInitNegTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_INTR_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13377U,
        .tcName     = "Test UDMA Deinit function",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = USE_DEF_LP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&udmaTestUdmaDeinitNegTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_INTR_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13376U,
        .tcName     = "Event Registration test for UDMA Init function",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = USE_DEF_LP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&udmaTestUdmaInitEventRegTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_INTR_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13370U,
        .tcName     = "Negative Test for UDMA Utility Functions",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = USE_DEF_LP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&udmaTestUdmaPrinfNegTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13515U,
        .tcName     = "Negative Test for UDMA Utility Functions",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = USE_DEF_LP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&udmaTestUdmaGetTrSizeNegTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13516U,
        .tcName     = "Negative Test for UDMA Utility Functions",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = USE_DEF_LP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&udmaTestUdmaPhyToVirtNegTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13518U,
        .tcName     = "Negative Test for UDMA Utility Functions",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = USE_DEF_LP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&udmaTestUdmaVirtToPhyNegTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13517U,
        .tcName     = "Negative Test for UDMA Utility Functions",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = USE_DEF_LP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&udmaTestUdmaPrinftMutexNegTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13384U,
        .tcName     = "Test UDMA Init and Deinit function when Global Event Reg value is True",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = USE_DEF_LP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&udmaTestUdmaInitSkipEventRegTc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_INTR_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13405U,
        .tcName     = "UDMA flow get num function negative testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaTest_flowGetNum},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_NONE,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13398U,
        .tcName     = "UDMA ring get write index normal function negative testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&udmaTestRingGetWrIdxNormal},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_NONE,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13397U,
        .tcName     = "UDMA ring get Read index function negative testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&udmaTestRingGetRdIdxNormal},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_NONE,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13401U,
        .tcName     = "UDMA ring get ring occupancy function negative testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&udmaTestRingGetRingOccNormal},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_NONE,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13402U,
        .tcName     = "UDMA ring get mode function negative testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&udmaTestRingGetModeNormal},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_NONE,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13404U,
        .tcName     = "UDMA ring get element count function negative testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&udmaTestRingGetElementCntNormal},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_NONE,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13406U,
        .tcName     = "UDMA ring get Mem ptr function negative testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&udmaTestRingGetMemPtrNormal},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_NONE,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13424U,
        .tcName     = "Test UDMA Test ring attach negative test case",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = USE_DEF_LP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaTestRingAttach},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_INTR_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                         {0U, 0U, 0U}
                      },
        .ddim       = {
                         {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13426U,
        .tcName     = "Test UDMA ring get number negative testcases",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = USE_DEF_LP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaTestRingGetNum},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_INTR_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13427U,
        .tcName     = "Test ring Dequeue raw negative testcases",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = USE_DEF_LP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaTestRingDequeueRaw},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_INTR_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13428U,
        .tcName     = "Test ring queue raw negative testcases",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = USE_DEF_LP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaTestRingQueueRaw},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_INTR_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13425U,
        .tcName     = "Test UDMA ring Detach neagtive test cases",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = USE_DEF_LP_CNT,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaTestRingDetach},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_INTR_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_INVALID,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13393U,
        .tcName     = "UDMA ring free function negative testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&udmaTestRingFree},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_NONE,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13386U,
        .tcName     = "UDMA Flow free function negative testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaTest_flowFree},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_NONE,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13392U,
        .tcName     = "UDMA Flow get count function negative testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaTest_flowGetCount},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_NONE,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13549U,
        .tcName     = "UDMA flow params init negative function testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaTest_FlowPrms_init},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_NONE,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13547U,
        .tcName     = "UDMA ring params init negative function testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaTest_RingPrms_init},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_NONE,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13548U,
        .tcName     = "UDMA ring Mon params init negative function testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaTest_RingMonPrms_init},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_NONE,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13554U,
        .tcName     = "UDMA ring alloc negative testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&udmaTestRingAlloc},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_NONE,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13553U,
        .tcName     = "UDMA ring flush raw negative testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaTestRingFlushRaw},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_NONE,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13681U,
        .tcName     = "UDMA proxy alloc negative testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaTest_proxyAllocNeg},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_NONE,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13683U,
        .tcName     = "UDMA proxy config negative testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaTest_proxyConfigNeg},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_NONE,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13682U,
        .tcName     = "UDMA proxy free negative testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaTest_proxyFreeNeg},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_NONE,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13583U,
        .tcName     = "UDMA ChPause negative testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaTestChPauseNeg},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_NONE,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13584U,
        .tcName     = "UDMA ChOpen negative testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_NEGATIVE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaTestChOpenNeg},
        /* All other below parameters not used in this testcase */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_POLLED,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13585U,
        .tcName     = "UDMA ChClose negative testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_NEGATIVE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaTestChCloseNeg},
        /* All other below parameters not used in this testcase */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_POLLED,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13586U,
        .tcName     = "UDMA ChConfigTx negative testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_NEGATIVE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaTestChConfigTxNeg},
        /* All other below parameters not used in this testcase */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_POLLED,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13587U,
        .tcName     = "UdmaTestChGetDefaultFlowHandle negative testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_NEGATIVE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaTestChGetDefaultFlowHandleNeg},
        /* All other below parameters not used in this testcase */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_POLLED,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13588U,
        .tcName     = "UdmaTestChGetTdCqRingHandle negative testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_NEGATIVE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaTestChGetTdCqRingHandleNeg},
        /* All other below parameters not used in this testcase */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_POLLED,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13589U,
        .tcName     = "UdmaTestChGetFqRingNum negative testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_NEGATIVE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaTestChGetFqRingNumNeg},
        /* All other below parameters not used in this testcase */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_POLLED,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13691U,
        .tcName     = "UdmachGetCqRingNum negative testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_NEGATIVE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaTestChGetCqRingNumNeg},
        /* All other below parameters not used in this testcase */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_POLLED,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13693U,
        .tcName     = "Udma chGetTriggerEvent negative testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_NEGATIVE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaTestChGetTriggerEventNeg},
        /* All other below parameters not used in this testcase */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_POLLED,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13694U,
        .tcName     = "Udma chSetSwTrigger negative testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_NEGATIVE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaTestChSetSwTriggerNeg},
        /* All other below parameters not used in this testcase */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_POLLED,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13695U,
        .tcName     = "Udma chSetChaining negative testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_NEGATIVE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaTestChSetChainingNeg},
        /* All other below parameters not used in this testcase */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_POLLED,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13696U,
        .tcName     = "Udma chBreakChaining negative testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_NEGATIVE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaTestChBreakChainingNeg},
        /* All other below parameters not used in this testcase */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_POLLED,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13697U,
        .tcName     = "Udma chGetNumTest negative testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_NEGATIVE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaTestChGetNumNeg},
        /* All other below parameters not used in this testcase */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_POLLED,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13701U,
        .tcName     = "Udma chConfigUtcTest negative testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_NEGATIVE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaTestChConfigUtcTestNeg},
        /* All other below parameters not used in this testcase */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_POLLED,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13703U,
        .tcName     = "Udma chSetSwTrigger negative testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_NEGATIVE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaTestChSetSwTriggerChTypeNeg},
        /* All other below parameters not used in this testcase */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_POLLED,
    },
    #if (UDMA_SOC_CFG_BCDMA_PRESENT == 1)
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13736U,
        .tcName     = "Udma udmaChPause positive testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_NEGATIVE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaTestChPause},
        /* All other below parameters not used in this testcase */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_BCDMA | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_POLLED,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13737U,
        .tcName     = "Udma ChGetDefaultFlowHandle positive testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_NEGATIVE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaTestChGetDefaultFlowHandle},
        /* All other below parameters not used in this testcase */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_BCDMA | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_POLLED,
    },
    #endif
    #if (UDMA_NUM_UTC_INSTANCE > 0)
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13771U,
        .tcName     = "Udma Channel Pause Dru negative testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_NEGATIVE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaTestChPauseDruNeg},
        /* All other below parameters not used in this testcase */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_DRU | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_POLLED,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13772U,
        .tcName     = "Udma channel ConfigUtc Dru negative testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_NEGATIVE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaTestChConfigUtcDruTestNeg},
        /* All other below parameters not used in this testcase */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_DRU | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_POLLED,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13702U,
        .tcName     = "Udma chDisableTest Negative testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_NEGATIVE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaTestChDisableDruNeg},
        /* All other below parameters not used in this testcase */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_DRU | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_POLLED,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13773U,
        .tcName     = "Udma channel SetSwTrigger Dru negative testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_NEGATIVE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaTestChSetSwTriggerDruNeg},
        /* All other below parameters not used in this testcase */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_DRU | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_POLLED,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13686U,
        .tcName     = "UDMA dru get num queue negative testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaTestDruGetNumQueueNeg},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_DRU  | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_NONE,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13687U,
        .tcName     = "UDMA dru queue config negative testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaTestDruQueueConfigNeg},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                       {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_DRU  | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_NONE,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13688U,
        .tcName     = "UDMA dru get trigger addr negative testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaTestDruGetTriggerRegAddrNeg},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_DRU | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_NONE,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13755U,
        .tcName     = "UDMA dru get trigger addr testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaTestDruGetTriggerRegAddr},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_DRU | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_NONE,
    },
    #endif
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13776U,
        .tcName     = "UDMA rmFreeBlkCopyUhcCh testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaRmFreeBlkCopyUhcChTest},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_UHC | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_NONE,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13777U,
        .tcName     = "UDMA rmFreeTxHcCh testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaRmFreeTxHcChTest},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_MCU2_0 | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_NONE,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13778U,
        .tcName     = "UDMA rmFreeTxUhcCh testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaRmFreeTxUhcChTest},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_UHC | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_NONE,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13779U,
        .tcName     = "UDMA rmFreeRxUhcCh testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaRmFreeRxUhcChTest},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_UHC | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_NONE,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13780U,
        .tcName     = "UDMA RmInitPrms_init testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_FUNCTIONAL),
        .dcEnable   = DATA_CHECK_ENABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaRmInitPrmsinitTestNeg},
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_NONE,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13689U,
        .tcName     = "Udma channel GetStats negative testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_NEGATIVE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaChGetStatsTestNeg},
        /* All other below parameters not used in this testcase */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_POLLED,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13690U,
        .tcName     = "Udma channel disable negative testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_NEGATIVE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaChDisableTestNeg},
        /* All other below parameters not used in this testcase */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_POLLED,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13826U,
        .tcName     = "Udma channel disable testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_NEGATIVE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaChDisableTest},
        /* All other below parameters not used in this testcase */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_POLLED,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13692U,
        .tcName     = "Udma channel Resume negative testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_NEGATIVE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaChResumeTestNeg},
        /* All other below parameters not used in this testcase */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_POLLED,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13699U,
        .tcName     = "Udma channel ConfigPdma negative testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_NEGATIVE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaChConfigPdmaTestNeg},
        /* All other below parameters not used in this testcase */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_POLLED,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13834U,
        .tcName     = "Udma channel ConfigPdma testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_NEGATIVE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaChConfigPdmaTest},
        /* All other below parameters not used in this testcase */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_POLLED,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13700U,
        .tcName     = "Udma channel ConfigRx negative testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_NEGATIVE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaChConfigRxTestNeg},
        /* All other below parameters not used in this testcase */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_POLLED,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13698U,
        .tcName     = "Udma channel enable negative testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_NEGATIVE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaChEnableTestNeg},
        /* All other below parameters not used in this testcase */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_POLLED,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13738U,
        .tcName     = "Udma channel GetTdCqRingHandle testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_NEGATIVE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaTestChGetTdCqRingHandle},
        /* All other below parameters not used in this testcase */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_POLLED,
    },
    {
        .enableTest = TEST_ENABLE,
        .tcId       = 13739U,
        .tcName     = "Udma channel GetFqRingNum testcase",
        .disableInfo= NULL,
        .printEnable= PRINT_ENABLE,
        .prfEnable  = PRF_DISABLE,
        .tcType     = (UDMA_TCT_SANITY | UDMA_TCT_NEGATIVE),
        .dcEnable   = DATA_CHECK_DISABLE,
        .loopCnt    = 1U,
        .numTasks   = 1U,
        .testType   = {UDMA_TT_MISC},
        .testFxnPtr = {&UdmaTestChGetFqRingNum},
        /* All other below parameters not used in this testcase */
        .pacingTime = {PACING_NONE},
        .numCh      = {1U},
        .instId     = {UDMA_TEST_DEFAULT_UDMA_INST},
        .chPrmId    = {UDMA_TEST_CH_PRMID_DEF},
        .qdepth     = {USE_DEF_QDEPTH},
        .icnt       = {
                        {UDMA_TEST_DEF_ICNT0, 1U, 1U, 1U}
                      },
        .dicnt      = {
                        {UDMA_TEST_DEF_DICNT0, 1U, 1U, 1U}
                      },
        .dim        = {
                        {0U, 0U, 0U}
                      },
        .ddim       = {
                        {0U, 0U, 0U}
                      },
        .heapIdSrc  = {DEF_HEAP_ID},
        .heapIdDest = {DEF_HEAP_ID},
        .srcBufSize = {UDMA_TEST_DEF_ICNT0},
        .destBufSize= {UDMA_TEST_DEF_DICNT0},
        .runFlag    = (UDMA_TEST_RF_SOC_ALL | UDMA_TEST_RF_CORE_ALL | UDMA_TEST_RF_CFG_DEF | UDMA_TEST_RF_CFG_DYN),
        .ringPrmId  = UDMA_TEST_RING_PRMID_EVENT_POLLED,
    },
};

#ifdef __cplusplus
}
#endif

#endif  /* UDMA_TEST_CASES_H_ */

