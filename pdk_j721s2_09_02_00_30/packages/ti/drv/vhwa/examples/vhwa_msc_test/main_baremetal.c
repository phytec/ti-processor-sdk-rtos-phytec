/**
 *   Copyright (c) Texas Instruments Incorporated 2018
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
 *  \brief Main file for Non OS build
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/board/board.h>

#include <ti/drv/vhwa/include/vhwa_m2mMsc.h>
#include <ti/drv/vhwa/examples/include/vhwa_msc_api.h>
#include "vhwa_msc_cfg.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* Input Offset between two test cases, used mainly for zebu/qt,
 * so that buffers can be loaded in one shot */
#define VHWA_MSC_TEST_IN_BUF_OFFSET     (8*1024*1024)

/* Output Offset between two test cases, used mainly for zebu/qt,
 * so that buffers can be saved in one shot */
#define VHWA_MSC_TEST_OUT_BUF_OFFSET    (16*1024*1024)

/* Offset for Each Input buffer in DDR */
#define APP_MSC_IN_BUFF_OFFSET      (0x800000u)
/* Offset for Each Output buffer in DDR */
#define APP_MSC_OUT_BUFF_OFFSET     (0x800000u)

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */
/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */
static void App_MscTest(App_MscTestParams *tObj);
static int32_t App_MscInit(void);
static void App_MscDeInit(void);
static void App_UdmaPrint(const char *str);;

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/*
* Application Buffers
*/
static uint64_t gMscTestSrcBuf =
    (uint64_t )(VHWA_EXAMPLE_BUFF_START_ADDR);
static uint64_t gMscTestDestBuf =
    (uint64_t )(VHWA_EXAMPLE_BUFF_START_ADDR + 0x10000000u);

static uint32_t gMscTestSrcBufFreeIdx = 0u;
static uint32_t gMscTestDstBufFreeIdx = 0u;

int32_t  gSpCoeffSets[][MSC_MAX_SP_COEFF_SET][MSC_MAX_TAP] = {
    {
        {16, 48, 128, 48, 16},
        {32, 32, 128, 32, 32}
    },
};

int32_t  gMpCoeffSets[][MSC_MAX_MP_COEFF_SET][MSC_MAX_TAP * 32U] = {
    #include "coeff.txt"
};

Msc_Coeff gCoefTbl[] = {
    {
        {gSpCoeffSets[0][0], gSpCoeffSets[0][1]},
        {gMpCoeffSets[0][0], gMpCoeffSets[0][1],
         gMpCoeffSets[0][2], gMpCoeffSets[0][3]}
    }
};

static App_MscTestCfg gAppMscTestCfg[] =
{
    #include <ti/drv/vhwa/examples/include/vhwa_msc_test_cfg.h>
};

static App_MscTestParams gAppMscObj[] = VHWA_MSC_TIRTOS_CFG;

static struct Udma_DrvObj gMscAppUdmaDrvObj;

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

int main(void)
{
    int32_t                 status;
    uint32_t                testCnt;
    App_MscTestParams      *tObj = NULL;

    status = App_MscInit();

    App_startTimer();

    if (FVID2_SOK == status)
    {
        for (testCnt = 0u; testCnt <
                (sizeof(gAppMscObj)/ sizeof(App_MscTestParams)); testCnt ++)
        {
            tObj = &gAppMscObj[testCnt];

            gMscTestSrcBufFreeIdx = VHWA_MSC_TEST_IN_BUF_OFFSET * testCnt;
            gMscTestDstBufFreeIdx = VHWA_MSC_TEST_OUT_BUF_OFFSET * testCnt;

            /* Only single Testcases are supported */
            if ((UTRUE == tObj->isEnableTest) && (1U == tObj->numHandles))
            {
                App_print (" Starting Test %s\n", tObj->testName);
                App_MscTest(tObj);
            }
        }
    }

    App_print (" Exiting \n");
    App_MscDeInit();

    return (0);
}

static void App_MscTest(App_MscTestParams  *tObj)
{
    int32_t                 status = FVID2_SOK;
    uint32_t                repCnt;
    uint32_t                inFrmSize, outFrmSize;

    status = AppMsc_Create(tObj, 0);
    if (FVID2_SOK != status)
    {
        App_print(" MSC_TEST_APP: Create Failed\n");
        status = FVID2_EFAIL;
    }

    status = AppMsc_SetCoeff(tObj, 0, &gCoefTbl[0]);
    if (FVID2_SOK != status)
    {
        App_print(" MSC_TEST_APP: SetCoeff Failed\n");
        status = FVID2_EFAIL;
    }


    if (FVID2_SOK == status)
    {
        status = AppMsc_SetParams(tObj, 0);
        if (FVID2_SOK != status)
        {
            App_print(" MSC_TEST_APP: SetParams Failed \n");
            status = FVID2_EFAIL;
        }
    }

    if (FVID2_SOK == status)
    {
        AppMsc_AllocBuffers(tObj, 0,
            gMscTestSrcBuf+gMscTestSrcBufFreeIdx, &inFrmSize,
            APP_MSC_IN_BUFF_OFFSET,
            gMscTestDestBuf+gMscTestDstBufFreeIdx, &outFrmSize,
            APP_MSC_OUT_BUFF_OFFSET);

        /* Move Buffer Index */
        gMscTestSrcBufFreeIdx += inFrmSize;
        gMscTestDstBufFreeIdx += outFrmSize;
    }

    for (repCnt = 0u; (repCnt < tObj->repeatCnt) &&
            (FVID2_SOK == status); repCnt ++)
    {
        AppMsc_PrepareRequest(tObj, 0);

        /* Submit Request*/
        status = AppMsc_SubmitRequest(tObj, 0);

        if (FVID2_SOK == status)
        {
            /* Wait for Request Completion */
            status = AppMsc_WaitForComplRequest(tObj, 0);
            if (FVID2_SOK != status)
            {
                App_print (" Failed to wait for request completion \n");
            }
        }
        else
        {
            App_print (" Failed to Submit Request \n");
        }

        if (FVID2_SOK != status)
        {
            break;
        }
        else
        {
            App_print (" Completed RepeatCnt %d\n",
                        repCnt);
        }
    }

    AppMsc_Delete(tObj, 0);

}

static int32_t App_MscInit(void)
{
    int32_t                 status;
    uint32_t                instId;
    Udma_DrvHandle          drvHandle = &gMscAppUdmaDrvObj;
    Udma_InitPrms           udmaInitPrms;
    Board_initCfg           boardCfg;
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
        udmaInitPrms.printFxn = &App_UdmaPrint;
        status = Udma_init(drvHandle, &udmaInitPrms);
        if(UDMA_SOK != status)
        {
            App_print("[Error] UDMA init failed!!\n");
            status = FVID2_EFAIL;
        }
    }

    status = AppMsc_Init(drvHandle);
    if (FVID2_SOK == status)
    {
        /* Initialize the UDMA channel for CRC */
        status = AppMsc_CrcInit(drvHandle);
    }

    return (status);
}

static void App_MscDeInit()
{
    int32_t         status;
    Udma_DrvHandle  drvHandle = &gMscAppUdmaDrvObj;

    AppMsc_DeInit();

    AppMsc_CrcDeinit(drvHandle);

    status = Udma_deinit(drvHandle);
    if(UDMA_SOK != status)
    {
        App_print("[Error] UDMA deinit failed!!\n");
    }

    Fvid2_deInit(NULL);
}

static void App_UdmaPrint(const char *str)
{
    App_print(str);

    return;
}

