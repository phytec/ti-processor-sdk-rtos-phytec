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
#include <ti/drv/vhwa/include/vhwa_m2mFlexConnect.h>

#include <ti/drv/vhwa/examples/include/vhwa_viss_test_api.h>
#include <ti/drv/vhwa/examples/vhwa_viss_test/vhwa_viss_cfg.h>
#include <ti/drv/vhwa/examples/include/vhwa_flexconnect_api.h>
#include <ti/drv/vhwa/examples/vhwa_flexconnect_test/vhwa_flexconnect_cfg.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */



/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static void App_udmaPrint(const char *str);
static int32_t AppFc_test(AppFc_TestParams *tPrms);
static int32_t FcApp_init();

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/*
* Application Buffers
*/
static uint64_t gFcTestSrcBuf =
    (uint64_t )(0xA0000000);
static uint64_t gFcTestDestBuf =
    (uint64_t )(0xC0000000u);

static uint32_t gFcTestSrcBufFreeIdx = 0u;
static uint32_t gFcTestDstBufFreeIdx = 0u;

static struct Udma_DrvObj gFcAppUdmaDrvObj;

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

int main(void)
{
    int32_t                 status;
    uint32_t                testCnt;
    Udma_DrvHandle          drvHandle = &gFcAppUdmaDrvObj;
    AppFc_TestParams     *tPrms;

    status = FcApp_init();

    App_startTimer();

    if (FVID2_SOK == status)
    {
        for (testCnt = 0u; testCnt <
                (sizeof(gAppFcTestPrms) / sizeof(AppFc_TestParams));
                testCnt ++)
        {
            tPrms = &gAppFcTestPrms[testCnt];

            gFcTestSrcBufFreeIdx = 0u;
            gFcTestDstBufFreeIdx = 0u;

            App_print (" Starting Test %s\n", tPrms->testName);
            status = AppFc_test(tPrms);

            if (FVID2_SOK != status)
            {
                App_print ("Error Running TestCase: %s\n", tPrms->testName);
                break;
            }
        }
    }

    AppFc_CrcDeinit(drvHandle);

    AppFc_deInit(drvHandle);

    App_print (" Exiting \n");

    return(status);
}

static int32_t FcApp_init()
{
    int32_t        status;
    uint32_t       instId;
    Udma_InitPrms  udmaInitPrms;
    Udma_DrvHandle drvHandle = &gFcAppUdmaDrvObj;
    Fvid2_InitPrms initPrmsFvid2;
    Board_initCfg  boardCfg;

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

    status = AppFc_Init(drvHandle);
    if (FVID2_SOK == status)
    {
        /* Initialize the UDMA channel for CRC */
        status = AppFc_CrcInit(drvHandle);
    }

    return (status);
}

static int32_t AppFc_test(AppFc_TestParams *tPrms)
{
    int32_t     status;
    uint32_t    repCnt;

    status = AppFc_Create(tPrms, 0U);
    if (FVID2_SOK != status)
    {
        App_print (" Failed to create\n");
    }

    status = AppFc_SetAllConfig(tPrms, 0U);
    if (FVID2_SOK != status)
    {
        App_print (" Failed to Set all Config\n");
    }

    status = AppFc_AllocBuffers(tPrms, 0U,
        gFcTestSrcBuf,  &gFcTestSrcBufFreeIdx,
        gFcTestDestBuf, &gFcTestDstBufFreeIdx);
    if (FVID2_SOK != status)
    {
        App_print (" Failed to Alloc Buffer\n");
    }

    for (repCnt = 0U; repCnt < tPrms->repeatCnt; repCnt ++)
    {


        AppFc_PrepareRequest(tPrms, 0U, repCnt);

        status = AppFc_SubmitRequest(tPrms, 0U);
        if (FVID2_SOK != status)
        {
            App_print (" Failed to Submit Request\n");
        }

        status = AppFc_WaitForCompRequest(tPrms, 0U);
        if (FVID2_SOK != status)
        {
            App_print (" Failed to Get back Request\n");
        }
        else
        {
            App_print (" Completed RepeatCnt = %d\n", repCnt);
        }
    }

    AppFc_Delete(tPrms, 0U);

    return (status);
}

static void App_udmaPrint(const char *str)
{
    App_print(str);

    return;
}

