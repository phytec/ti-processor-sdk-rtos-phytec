/**
 *   Copyright (c) Texas Instruments Incorporated 2019
 *   All rights reserved.
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
 *  \file main_baremetal.c
 *
 *  \brief Main file for baremetal build
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <stdio.h>
#include <stdint.h>
#include <ti/board/board.h>
#include <ti/drv/vhwa/examples/include/vhwa_examples_common.h>
#include <ti/drv/vhwa/examples/include/vhwa_common_crc.h>
#include <ti/drv/vhwa/include/vhwa_m2mViss.h>
#include <ti/drv/vhwa/examples/include/vhwa_viss_test_api.h>
#include "vhwa_viss_cfg.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* Input Offset between two test cases, used mainly for zebu/qt,
 * so that buffers can be loaded in one shot */
#define VHWA_VISS_TEST_IN_BUF_OFFSET     (8*1024*1024)

/* Output Offset between two test cases, used mainly for zebu/qt,
 * so that buffers can be saved in one shot */
#define VHWA_VISS_TEST_OUT_BUF_OFFSET    (16*1024*1024)

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */



/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static void App_udmaPrint(const char *str);
static int32_t AppViss_test(AppViss_TestParams *tPrms);
static int32_t VissApp_init();

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/*
* Application Buffers
*/
static uint64_t gVissTestSrcBuf =
    (uint64_t )(VHWA_EXAMPLE_BUFF_START_ADDR);
static uint64_t gVissTestDestBuf =
    (uint64_t )(VHWA_EXAMPLE_BUFF_START_ADDR + 0x10000000u);

static uint32_t gVissTestSrcBufFreeIdx = 0u;
static uint32_t gVissTestDstBufFreeIdx = 0u;

static uint32_t *gVissTestConfigBuf =
        (uint32_t*) (VHWA_EXAMPLE_CONFIG_BUFF_START_ADDR);
static uint32_t gVissTestConfigBufFreeIdx = 0u;

#if 0
static AppViss_TestConfig gAppVissTestCfg[] =
{
     #include <ti/drv/vhwa/examples/include/vhwa_viss_test_cfg.h>
};

static VissApp_TestParams gAppVissObj[] = VHWA_VISS_TIRTOS_CFG;
#endif

static struct Udma_DrvObj gVissAppUdmaDrvObj;

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

int main(void)
{
    int32_t                 status;
    uint32_t                testCnt;
    Udma_DrvHandle          drvHandle = &gVissAppUdmaDrvObj;
    AppViss_TestParams     *tPrms;

    status = VissApp_init(drvHandle);

    App_startTimer();

    if (FVID2_SOK == status)
    {
        for (testCnt = 0u; testCnt <
                (sizeof(gAppVissTestPrms) / sizeof(AppViss_TestParams));
                testCnt ++)
        {
            tPrms = &gAppVissTestPrms[testCnt];

            gVissTestSrcBufFreeIdx = 0u;
            gVissTestDstBufFreeIdx = 0u;

            App_print (" Starting Test %s\n", tPrms->testName);
            status = AppViss_test(tPrms);

            if (FVID2_SOK != status)
            {
                App_print ("Error Running TestCase: %s\n", tPrms->testName);
                break;
            }
        }
    }

    AppViss_CrcDeinit(drvHandle);

    AppViss_deInit(drvHandle);

    App_print (" Exiting \n");

    return(status);
}

static int32_t VissApp_init()
{
    int32_t        status;
    uint32_t       instId;
    Udma_InitPrms  udmaInitPrms;
    Board_initCfg  boardCfg;
    Udma_DrvHandle drvHandle = &gVissAppUdmaDrvObj;
    Fvid2_InitPrms initPrmsFvid2;

    boardCfg = BOARD_INIT_PINMUX_CONFIG | BOARD_INIT_UART_STDIO;
    Board_init(boardCfg);

    Fvid2InitPrms_init(&initPrmsFvid2);
    initPrmsFvid2.printFxn = App_print;
    status = Fvid2_init(&initPrmsFvid2);

    if (FVID2_SOK != status)
    {
        App_print(" main: FVID2_Init Failed !!!\r\n");
    }

    if (FVID2_SOK == status)
    {
        /* Initialize UDMA and get the handle, it will be used in both CRC layer,
           as well as in the driver */
        /* UDMA driver init */
        instId = UDMA_INST_ID_MAIN_0;
        UdmaInitPrms_init(instId, &udmaInitPrms);
        udmaInitPrms.printFxn = &App_udmaPrint;
        status = Udma_init(drvHandle, &udmaInitPrms);
        if(UDMA_SOK != status)
        {
            App_print("[Error] UDMA init failed!!\n");
            status = FVID2_EFAIL;
        }
    }

    status = AppViss_Init(drvHandle);
    if (FVID2_SOK == status)
    {
        /* Initialize the UDMA channel for CRC */
        status = AppViss_CrcInit(drvHandle);
    }

    return (status);
}

static int32_t AppViss_test(AppViss_TestParams *tPrms)
{
    int32_t     status;
    uint32_t    repCnt;
    Vhwa_M2mVissConfigAppBuff appBuffConfig;

    status = AppViss_Create(tPrms, 0U);
    if (FVID2_SOK != status)
    {
        App_print (" Failed to create\n");
    }

    status = AppViss_GetConfigBufInfo(0U, &appBuffConfig);
    if (FVID2_SOK != status)
    {
        App_print(" VISS_TEST_APP: Failed to Get config buf info\n");
    }
    else
    {
        /* configThroughUdmaFlag should not be changed by application */
        if (true == appBuffConfig.configThroughUdmaFlag)
        {
            /* update config buffer pointer */
            appBuffConfig.bufferPtr = gVissTestConfigBuf
                    + gVissTestConfigBufFreeIdx;

            status = AppViss_SetConfigBufInfo(0U, &appBuffConfig);
            if (FVID2_SOK != status)
            {
                App_print(" VISS_TEST_APP: Failed to Set config buf info\n");
            }
        }
    }

    status = AppViss_SetAllConfig(tPrms, 0U);
    if (FVID2_SOK != status)
    {
        App_print (" Failed to Set all Config\n");
    }

    status = AppViss_AllocBuffers(tPrms, 0U,
        gVissTestSrcBuf, &gVissTestSrcBufFreeIdx,
        gVissTestDestBuf, &gVissTestDstBufFreeIdx);
    if (FVID2_SOK != status)
    {
        App_print (" Failed to Set all Config\n");
    }

    AppViss_PrepareRequest(tPrms, 0U);

    for (repCnt = 0U; repCnt < tPrms->repeatCnt; repCnt ++)
    {
        status = AppViss_SubmitRequest(tPrms, 0U);
        if (FVID2_SOK != status)
        {
            App_print (" Failed to Submit Request\n");
        }

        status = AppViss_WaitForCompRequest(tPrms, 0U);
        if (FVID2_SOK != status)
        {
            App_print (" Failed to Get back Request\n");
        }
        else
        {
            App_print (" Completed RepeatCnt = %d\n", repCnt);
        }
    }

    AppViss_Delete(tPrms, 0U);

    return (status);
}

static void App_udmaPrint(const char *str)
{
    App_print(str);

    return;
}

