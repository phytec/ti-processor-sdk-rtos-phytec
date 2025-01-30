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

#include <ti/drv/vhwa/include/vhwa_m2mViss.h>
#include <ti/drv/vhwa/examples/include/vhwa_viss_test_api.h>
#include "vhwa_viss_cfg.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* Test application stack size */
#define APP_VISS_TSK_STACK_MAIN          (64U * 1024U)

/* Set the task priority higher than the default priority (1) */
#define APP_VISS_MAIN_TASKS_PRIORITY     (7U)

/* Set the task priority higher than the default priority (1) */
#define APP_VISS_TEST_TASKS_PRIORITY     (5U)

/* Input Offset between two test cases, used mainly for zebu/qt,
 * so that buffers can be loaded in one shot */
#define VHWA_VISS_TEST_IN_BUF_OFFSET     (8*1024*1024)

/* Output Offset between two test cases, used mainly for zebu/qt,
 * so that buffers can be saved in one shot */
#define VHWA_VISS_TEST_OUT_BUF_OFFSET    (16*1024*1024)

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

static SemaphoreP_Handle waitForTaskCmpl[VHWA_M2M_VISS_MAX_HANDLES];

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static void VissApp_mainTaskFunc(void* a0, void* a1);
static void VissApp_test(void* a0, void* a1);
static int32_t VissApp_init(void);
static void VissApp_run(uint32_t testCnt);
static void VissApp_deInit(void);
static void App_udmaPrint(const char *str);
static int32_t App_getNum(void);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* Test application stack */
#if defined(SAFERTOS)
static uint8_t  gAppTskStackMain[APP_VISS_TSK_STACK_MAIN]
    __attribute__((aligned(APP_VISS_TSK_STACK_MAIN))) = { 0 };
static uint8_t  gAppTskStackTest[VHWA_M2M_VISS_MAX_HANDLES][APP_VISS_TSK_STACK_MAIN]
    __attribute__((aligned(APP_VISS_TSK_STACK_MAIN))) = { 0 };
#else
static uint8_t  gAppTskStackMain[APP_VISS_TSK_STACK_MAIN]
    __attribute__((aligned(32)));
static uint8_t  gAppTskStackTest[VHWA_M2M_VISS_MAX_HANDLES][APP_VISS_TSK_STACK_MAIN]
    __attribute__((aligned(32)));
#endif

/*
* Application Buffers
*/
static uint64_t gVissTestSrcBuf =
    (uint64_t )(VHWA_EXAMPLE_BUFF_START_ADDR);
static uint64_t gVissTestDestBuf =
    (uint64_t )(VHWA_EXAMPLE_BUFF_START_ADDR + 0x10000000u);

static uint32_t *gVissTestConfigBuf =
        (uint32_t*) (VHWA_EXAMPLE_CONFIG_BUFF_START_ADDR);
static uint32_t gVissTestConfigBufFreeIdx = 0u;

static uint32_t gVissTestSrcBufFreeIdx = 0u;
static uint32_t gVissTestDstBufFreeIdx = 0u;

static SemaphoreP_Handle gAllocLock = {NULL};

extern AppViss_TestParams gAppVissTestPrms[];

static struct Udma_DrvObj gVissAppUdmaDrvObj;

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

    taskParams.priority     = APP_VISS_MAIN_TASKS_PRIORITY;
    taskParams.stack        = gAppTskStackMain;
    taskParams.stacksize    = sizeof (gAppTskStackMain);

    task = TaskP_create(&VissApp_mainTaskFunc, &taskParams);
    if(NULL == task)
    {
        OS_stop();
    }

    OS_start();    /* does not return */

    return(0);
}

static void VissApp_mainTaskFunc(void* a0, void* a1)
{
    int32_t             status;
    bool                done = UFALSE;
    int32_t             testIdx;

    status = VissApp_init();

    App_print("\n Starting VISS Test ..\n");

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
                    if((uint32_t)testIdx < (sizeof(gAppVissTestPrms)/ sizeof(AppViss_TestParams)))
                    {
                        VissApp_run(testIdx);
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
    VissApp_deInit();

    return;
}

void VissApp_run(uint32_t testCnt)
{
    uint32_t            hndlCnt;
    TaskP_Handle        task[VHWA_M2M_VISS_MAX_HANDLES];
    TaskP_Params        taskParams[VHWA_M2M_VISS_MAX_HANDLES];
    AppViss_TestParams *tPrms = NULL;
        
    tPrms = &gAppVissTestPrms[testCnt];

    gVissTestSrcBufFreeIdx = 0U;
    gVissTestDstBufFreeIdx = 0U;

    if (UTRUE == tPrms->isEnableTest)
    {
        App_print ("\n Starting Test %s\n", tPrms->testName);

        for (hndlCnt = 0U; hndlCnt < tPrms->numHandles;
                hndlCnt ++)
        {
            /* Initialize the task params */
            TaskP_Params_init(&taskParams[hndlCnt]);

            /* Set the task priority higher than the
                default priority (1) */
            taskParams[hndlCnt].priority  = APP_VISS_TEST_TASKS_PRIORITY;
            taskParams[hndlCnt].stack     = gAppTskStackTest[hndlCnt];
            taskParams[hndlCnt].stacksize = sizeof (gAppTskStackTest[hndlCnt]);
            taskParams[hndlCnt].arg0      = (void*)tPrms;
            taskParams[hndlCnt].arg1      = (void*)hndlCnt;

            /* Start a new Task */
            task[hndlCnt] = TaskP_create(&VissApp_test,
                                            &taskParams[hndlCnt]);
            if(NULL == task[hndlCnt])
            {
                App_print (" Task%d create failed... \n", hndlCnt);
                OS_stop();
            }
        }

        /* Wait for all tesks to get completed */
        for (hndlCnt = 0U; hndlCnt < tPrms->numHandles; hndlCnt ++)
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
        App_print ("\n Test %d - %s is Disabled \n", testCnt, tPrms->testName);
    }

    return;
}

static void VissApp_test(void* a0, void* a1)
{
    int32_t                 status = FVID2_SOK;
    uint32_t                hndlIdx = (uint32_t )a1;
    uint32_t                repCnt;
    uint64_t                timeCount;
    uint64_t                perf;
    AppViss_TestParams     *tPrms = (AppViss_TestParams *)a0;
    uint32_t                inFrmSize, outFrmSize;
    Vhwa_M2mVissConfigAppBuff appBuffConfig;

    status = AppViss_Create(tPrms, hndlIdx);
    if (FVID2_SOK != status)
    {
        App_print(" VISS_TEST_APP: Create Failed for %d\n", hndlIdx);
        status = FVID2_EFAIL;
    }

    if (FVID2_SOK == status)
    {
        SemaphoreP_pend(gAllocLock, SemaphoreP_WAIT_FOREVER);

        status = AppViss_GetConfigBufInfo(hndlIdx, &appBuffConfig);
        if (FVID2_SOK != status)
        {
            App_print(" VISS_TEST_APP: Failed to Get config buf info\n");
            status = FVID2_EFAIL;
        }
        else
        {
            /* configThroughUdmaFlag should not be changed by application */
            if (true == appBuffConfig.configThroughUdmaFlag)
            {
                /* update config buffer pointer */
                appBuffConfig.bufferPtr = gVissTestConfigBuf
                        + gVissTestConfigBufFreeIdx;

                status = AppViss_SetConfigBufInfo(hndlIdx, &appBuffConfig);
                if (FVID2_SOK != status)
                {
                    App_print(
                            " VISS_TEST_APP: Failed to Set config buf info\n");
                    status = FVID2_EFAIL;
                }
                else
                {
                    /* Move config Buffer Index in word size */
                    gVissTestConfigBufFreeIdx += (appBuffConfig.length / 4);
                }
            }
        }

        SemaphoreP_post(gAllocLock);
    }

    if (FVID2_SOK == status)
    {
        status = AppViss_SetAllConfig(tPrms, hndlIdx);
        if (FVID2_SOK != status)
        {
            App_print (" VISS_TEST_APP: Failed to Set all Config\n");
            status = FVID2_EFAIL;
        }
    }

    if (FVID2_SOK == status)
    {
        SemaphoreP_pend(gAllocLock, SemaphoreP_WAIT_FOREVER);

        status = AppViss_AllocBuffers(tPrms, hndlIdx,
            gVissTestSrcBuf+gVissTestSrcBufFreeIdx, &inFrmSize,
            gVissTestDestBuf+gVissTestDstBufFreeIdx, &outFrmSize);

        /* Move Buffer Index */
        gVissTestSrcBufFreeIdx += inFrmSize;
        gVissTestDstBufFreeIdx += outFrmSize;

        SemaphoreP_post(gAllocLock);

        if (FVID2_SOK != status)
        {
            App_print (" VISS_TEST_APP: Failed to Set all Config\n");
        }
    }

    AppViss_PrepareRequest(tPrms, hndlIdx);

    App_print(" Load buffers and press any key to continue\n");
    UART_getc();

    if(tPrms->isPerformanceTest)
    {
        timeCount = TimerP_getTimeInUsecs();
    }

    if (FVID2_SOK == status)
    {
        for (repCnt = 0U; repCnt < tPrms->repeatCnt; repCnt ++)
        {
            status = AppViss_SubmitRequest(tPrms, hndlIdx);
            if (FVID2_SOK != status)
            {
                App_print (" VISS_TEST_APP: Failed to Submit Request\n");
                break;
            }
            else
            {
                status = AppViss_WaitForCompRequest(tPrms, hndlIdx);
                if (FVID2_SOK != status)
                {
                    App_print (" VISS_TEST_APP: Failed to Get back Request\n");
                    break;
                }
            }
        }

        if(tPrms->isPerformanceTest)
        {
            timeCount = TimerP_getTimeInUsecs() - timeCount;
            App_print ("Performance:\n\t FrameCount: %d: Time in uSec: %d\n",
                        tPrms->repeatCnt, timeCount);

            perf = (uint64_t)tPrms->testCfg[hndlIdx]->vissPrms.inFmt.width
                   *(uint64_t)tPrms->testCfg[hndlIdx]->vissPrms.inFmt.height
                   *(uint64_t)tPrms->repeatCnt;

            App_print ("\t MPix/s: %d.%d\n",
                (uint32_t)(perf/timeCount),
                 (uint32_t)(((perf*(uint64_t)100)/timeCount)%100));
        }

        if (FVID2_SOK == status)
        {
            App_print (" Completed HandleCnt %d RepeatCnt %d\n",
                    hndlIdx, repCnt);
        }
    }

    AppViss_Delete(tPrms, hndlIdx);

    SemaphoreP_post(waitForTaskCmpl[hndlIdx]);
}

static int32_t VissApp_init(void)
{
    int32_t        status;
    uint32_t       instId, hndlCnt;
    Udma_InitPrms  udmaInitPrms;
    Board_initCfg  boardCfg;
    Udma_DrvHandle drvHandle = &gVissAppUdmaDrvObj;
    SemaphoreP_Params params[VHWA_M2M_VISS_MAX_HANDLES];
    SemaphoreP_Params       semAllocPrms;
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

    if (FVID2_SOK == status)
    {
        for (hndlCnt = 0U; hndlCnt < VHWA_M2M_VISS_MAX_HANDLES; hndlCnt ++)
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

        SemaphoreP_Params_init(&semAllocPrms);
        semAllocPrms.mode = SemaphoreP_Mode_BINARY;

        gAllocLock = SemaphoreP_create(1U, &semAllocPrms);
        if (NULL == gAllocLock)
        {
            App_print("[Error] Sem create failed!!\n");
            status = FVID2_EFAIL;
        }
    }
    return (status);
}

static void VissApp_deInit()
{
    int32_t         status;
    uint32_t        cnt;
    Udma_DrvHandle  drvHandle = &gVissAppUdmaDrvObj;

    Vhwa_m2mVissDeInit();

    AppViss_CrcDeinit(drvHandle);

    status = Udma_deinit(drvHandle);
    if(UDMA_SOK != status)
    {
        App_print("[Error] UDMA deinit failed!!\n");
    }

    for (cnt = 0U; cnt < VHWA_M2M_VISS_MAX_HANDLES; cnt ++)
    {
        if (NULL != waitForTaskCmpl[cnt])
        {
            SemaphoreP_delete(waitForTaskCmpl[cnt]);
        }
    }

    if (gAllocLock != NULL)
    {
        SemaphoreP_delete(gAllocLock);
        gAllocLock = NULL;
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


