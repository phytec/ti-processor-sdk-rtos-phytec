
/*
 *  Copyright (c) Texas Instruments Incorporated 2019-2021
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
 *  \file main_rtos.c
 *
 *  \brief Main file for RTOS builds
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <stdint.h>
#include <stdio.h>
#include "ti/osal/osal.h"
#include "ti/osal/TaskP.h"

#include <ti/board/board.h>
#include <ti/drv/uart/UART.h>
#include <ti/drv/uart/UART_stdio.h>

#include <ti/drv/vhwa/include/vhwa_m2mDof.h>
#include <ti/drv/vhwa/examples/include/vhwa_dof_api.h>
#include "vhwa_dof_cfg.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* Test application stack size */
#define APP_TSK_STACK_MAIN              (64U * 1024U)

/* Set the task priority higher than the default priority (1) */
#define DOF_APP_MAIN_TASKS_PRIORITY     (7U)

/* Set the task priority higher than the default priority (1) */
#define DOF_APP_TEST_TASKS_PRIORITY     (5U)

/* Input Offset between two test cases, used mainly for zebu/qt,
 * so that buffers can be loaded in one shot */
#define VHWA_DOF_TEST_IN_BUF_OFFSET     (8*1024*1024)

/* Output Offset between two test cases, used mainly for zebu/qt,
 * so that buffers can be saved in one shot */
#define VHWA_DOF_TEST_OUT_BUF_OFFSET    (16*1024*1024)

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

static SemaphoreP_Handle waitForTaskCmpl[APP_DOF_MAX_HANDLES];

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static void DofApp_mainTaskFunc(void* a0, void* a1);
static void DofApp_test(void* a0, void* a1);
static int32_t DofApp_init(void);
static void DofApp_run(uint32_t testCnt);
static void DofApp_deInit(void);
static void App_udmaPrint(const char *str);;
static int32_t App_getNum(void);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* Test application stack */
#if defined(SAFERTOS)
static uint8_t  gAppTskStackMain[APP_TSK_STACK_MAIN]
    __attribute__((aligned(APP_TSK_STACK_MAIN)));
static uint8_t  gAppTskStackTest[APP_DOF_MAX_HANDLES][APP_TSK_STACK_MAIN]
    __attribute__((aligned(APP_TSK_STACK_MAIN)));
#else
static uint8_t  gAppTskStackMain[APP_TSK_STACK_MAIN]
    __attribute__((aligned(32)));
static uint8_t  gAppTskStackTest[APP_DOF_MAX_HANDLES][APP_TSK_STACK_MAIN]
    __attribute__((aligned(32)));
#endif

/*
* Application Buffers
*/
static uint64_t gRefSrcBuf      = 0x90000000U;
static uint64_t gCurrSrcBuf     = 0xA0000000U;
static uint64_t gTempSrcBuf     = 0xB0000000U;
static uint64_t gOutBuf         = 0xC0000000U;
static uint64_t gSofSrcBuf      = 0x98000000U;
static uint64_t gInterOddBuf    = 0xD0000000U;
static uint64_t gInterEvenBuf   = 0xE0000000U;
static uint32_t gRefBufFreeIdx = 0u;
static uint32_t gCurrBufFreeIdx = 0u;
static uint32_t gTempBufFreeIdx = 0u;
static uint32_t gSofBufFreeIdx = 0u;
static uint32_t gOutBufFreeIdx = 0u;


static AppDof_TestConfig gAppDofTestCfg[] =
{
    #include <ti/drv/vhwa/examples/include/vhwa_dof_test_cfg.h>
};

static DofApp_TestParams gAppDofObj[] = VHWA_DOF_TIRTOS_CFG;

static struct Udma_DrvObj gDofAppUdmaDrvObj;

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

int main(void)
{
    TaskP_Handle task;
    TaskP_Params taskParams;

    OS_init();

    /* Initialize the task params */
    TaskP_Params_init(&taskParams);

    taskParams.priority     = DOF_APP_MAIN_TASKS_PRIORITY;
    taskParams.stack        = gAppTskStackMain;
    taskParams.stacksize    = sizeof (gAppTskStackMain);

    task = TaskP_create(&DofApp_mainTaskFunc, &taskParams);
    if(NULL == task)
    {
        OS_stop();
    }

    OS_start();    /* does not return */

    return(0);
}

static void DofApp_mainTaskFunc(void* a0, void* a1)
{
    int32_t             status;
    bool                done = UFALSE;
    int32_t             testIdx;

    status = DofApp_init();

    App_print("\n Starting DOF Test ..\n");

    App_startTimer();

    if (FVID2_SOK == status)
    {
        while(!done)
        {
            App_print("\n Enter Test to Run (Enter '-1' to exit)  \n");
            testIdx = App_getNum();

            switch(testIdx)
            {
                case -1:
                    done = UTRUE;
                    break;

                default:
                    if((uint32_t)testIdx < (sizeof(gAppDofObj)/ sizeof(DofApp_TestParams)))
                    {
                        DofApp_run(testIdx);
                    }
                    else
                    {
                        App_print("Invalid Test ID. Enter Again!!\n");
                    }
                    break;
            }
        }
    }

    App_print (" Exiting \n");
    DofApp_deInit();

    return;
}

void DofApp_run(uint32_t testCnt)
{
    uint32_t            hndlCnt;
    TaskP_Handle        task[APP_DOF_MAX_HANDLES];
    TaskP_Params        taskParams[APP_DOF_MAX_HANDLES];
    DofApp_TestParams  *tObj = NULL;

    tObj = &gAppDofObj[testCnt];

    gRefBufFreeIdx = 0u;
    gCurrBufFreeIdx = 0u;
    gTempBufFreeIdx = 0u;
    gSofBufFreeIdx = 0u;
    gOutBufFreeIdx = 0u;

    if (UTRUE == tObj->isEnableTest)
    {
        App_print ("\n Starting Test %s\n", tObj->testName);

        for (hndlCnt = 0U; hndlCnt < tObj->numHandles;
                hndlCnt ++)
        {
            /* Initialize the task params */
            TaskP_Params_init(&taskParams[hndlCnt]);

            /* Set the task priority higher than the
                default priority (1) */
            taskParams[hndlCnt].priority  = DOF_APP_TEST_TASKS_PRIORITY;
            taskParams[hndlCnt].stack     = gAppTskStackTest[hndlCnt];
            taskParams[hndlCnt].stacksize = sizeof (gAppTskStackTest[hndlCnt]);
            taskParams[hndlCnt].arg0      = (void*)tObj;
            taskParams[hndlCnt].arg1      = (void*)hndlCnt;

            /* Start a new Task */
            task[hndlCnt] = TaskP_create(&DofApp_test,
                                            &taskParams[hndlCnt]);
            if(NULL == task[hndlCnt])
            {
                App_print (" Task%d create failed... \n", hndlCnt);
                OS_stop();
            }
        }

        /* Wait for all tasks to get completed */
        for (hndlCnt = 0U; hndlCnt < tObj->numHandles; hndlCnt ++)
        {
            SemaphoreP_pend(waitForTaskCmpl[hndlCnt],
                SemaphoreP_WAIT_FOREVER);

            App_print (" Task%d Completed \n", hndlCnt);
            /* Delete Task */
            if (NULL != task[hndlCnt])
            {
                TaskP_delete(&task[hndlCnt]);
            }
        }
    }
    else
    {
        App_print ("\n Test %d - %s is Disabled \n", testCnt, tObj->testName);
    }

    return;
}

static void DofApp_test(void* a0, void* a1)
{
    int32_t                 status = FVID2_SOK;
    uint32_t                hndlIdx = (uint32_t )a1;
    uint32_t                repCnt, pyrCnt;
    uint64_t                timeCount = 0;
    DofApp_TestParams      *tObj = (DofApp_TestParams *)a0;

    status = AppDof_Create(tObj, hndlIdx);
    if (FVID2_SOK != status)
    {
        App_print(" DOF_TEST_APP: Create Failed for %d\n", hndlIdx);
        status = FVID2_EFAIL;
    }

    if (FVID2_SOK == status)
    {
        status = AppDof_SetParams(tObj, hndlIdx);
        if (FVID2_SOK != status)
        {
            App_print(" DOF_TEST_APP: SetParams Failed \n");
            status = FVID2_EFAIL;
            return ;
        }
    }

    if(FVID2_SOK == status)
    {
        status = AppDof_SetConfScoreParam(tObj, hndlIdx);
        if (FVID2_SOK != status)
        {
            App_print("DOF_TEST_APP: SetConfScoreParam Failed \n");
        }
    }

    if (FVID2_SOK == status)
    {
        tObj->buffAddr.interOddBuff = gInterOddBuf;
        tObj->buffAddr.interEvenBuff = gInterEvenBuf;

        tObj->buffAddr.refBuff = gRefSrcBuf + gRefBufFreeIdx;
        tObj->buffAddr.currBuff = gCurrSrcBuf + gCurrBufFreeIdx;
        tObj->buffAddr.tempBuff = gTempSrcBuf + gTempBufFreeIdx;
        tObj->buffAddr.sofBuff = gSofSrcBuf + gSofBufFreeIdx;
        tObj->buffAddr.outBuff = gOutBuf + gOutBufFreeIdx;
        tObj->buffAddr.refBuffIdx = &gRefBufFreeIdx;
        tObj->buffAddr.currBuffIdx = &gCurrBufFreeIdx;
        tObj->buffAddr.tempBuffIdx = &gTempBufFreeIdx;
        tObj->buffAddr.sofBuffIdx = &gSofBufFreeIdx;
        tObj->buffAddr.outBuffIdx = &gOutBufFreeIdx;

        AppDof_AllocBuffers(tObj, 0U, &tObj->buffAddr);

        App_print(" Load buffers and press any key to continue\n");
        UART_getc();

        if(tObj->isPerformanceTest)
        {
            timeCount = TimerP_getTimeInUsecs();
        }

        for (repCnt = 0u; (repCnt < tObj->repeatCnt) &&
                (FVID2_SOK == status); repCnt ++)
        {
            AppDof_PrepareRequest(tObj, hndlIdx);

            for (pyrCnt = tObj->testCfg[hndlIdx]->tPyrLvl; pyrCnt > 0; pyrCnt--)
            {
                /* Submit Request*/
                status = AppDof_SubmitRequest(tObj, hndlIdx);

                if (FVID2_SOK == status)
                {
                    /* Wait for Request Completion */
                    status = AppDof_WaitForComplRequest(tObj, hndlIdx);
                    if (FVID2_SOK != status)
                    {
                        App_print (" Failed to wait for request completion \n");
                    }
                }
                else
                {
                    App_print (" Failed to Submit Request \n");
                }
            }
        }

        if(tObj->isPerformanceTest)
        {
            timeCount = TimerP_getTimeInUsecs() - timeCount;
            App_print ("Performance:\n\t FrameCount: %d: Time in uSec: %d\n",
                        tObj->repeatCnt, timeCount);
            App_print ("\t MPix/s: %d.%d\n",
                (uint32_t)(((uint64_t)tObj->testCfg[hndlIdx]->width
                   *(uint64_t)tObj->testCfg[hndlIdx]->height
                   *(uint64_t)tObj->repeatCnt)/timeCount),
                 (uint32_t)((((uint64_t)tObj->testCfg[hndlIdx]->width
                   *(uint64_t)tObj->testCfg[hndlIdx]->height
                   *(uint64_t)tObj->repeatCnt*(uint64_t)100)/timeCount)%100));
        }
        if (FVID2_SOK == status)
        {
            App_print (" Completed HandleCnt %d RepeatCnt %d\n",
                hndlIdx, repCnt);
        }
    }

    AppDof_Delete(tObj, hndlIdx);

    SemaphoreP_post(waitForTaskCmpl[hndlIdx]);
}

static int32_t DofApp_init(void)
{
    int32_t                 status;
    uint32_t                instId;
    uint32_t                hndlCnt;
    Udma_DrvHandle          drvHandle = &gDofAppUdmaDrvObj;
    Udma_InitPrms           udmaInitPrms;
    Board_initCfg           boardCfg;
    SemaphoreP_Params       params[APP_DOF_MAX_HANDLES];
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

    status = AppDof_Init(drvHandle);
    if (FVID2_SOK == status)
    {
        /* Initialize the UDMA channel for CRC */
        status = AppDof_CrcInit(drvHandle);
    }

    if (FVID2_SOK == status)
    {
        for (hndlCnt = 0U; hndlCnt < APP_DOF_MAX_HANDLES; hndlCnt ++)
        {
            SemaphoreP_Params_init(&params[hndlCnt]);
            params[hndlCnt].mode = SemaphoreP_Mode_BINARY;
            waitForTaskCmpl[hndlCnt] =
                SemaphoreP_create(0U, &params[hndlCnt]);
            if (NULL == waitForTaskCmpl[hndlCnt])
            {
                App_print (" Could not create Semaphore \n");
                status = FVID2_EFAIL;
            }
        }
    }

    return (status);
}

static void DofApp_deInit()
{
    int32_t         status;
    uint32_t        cnt;
    Udma_DrvHandle  drvHandle = &gDofAppUdmaDrvObj;

    Vhwa_m2mDofDeInit();

    AppDof_CrcDeinit(drvHandle);

    status = Udma_deinit(drvHandle);
    if(UDMA_SOK != status)
    {
        App_print("[Error] UDMA deinit failed!!\n");
    }

    for (cnt = 0U; cnt < APP_DOF_MAX_HANDLES; cnt ++)
    {
        if (NULL != waitForTaskCmpl[cnt])
        {
            SemaphoreP_delete(waitForTaskCmpl[cnt]);
        }
    }

    Fvid2_deInit(NULL);
}

static void App_udmaPrint(const char *str)
{
    App_print(str);

    return;
}

int32_t App_getNum(void)
{
    int32_t num;

    UART_scanFmt("%d", &num);

    return (num);
}
