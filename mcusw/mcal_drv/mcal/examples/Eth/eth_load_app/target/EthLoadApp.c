/*
*
* Copyright (C) 2024 Texas Instruments Incorporated
*
* All rights reserved not granted herein.
*
* Limited License.
*
* Texas Instruments Incorporated grants a world-wide, royalty-free, non-exclusive
* license under copyrights and patents it now or hereafter owns or controls to make,
* have made, use, import, offer to sell and sell ("Utilize") this software subject to the
* terms herein.  With respect to the foregoing patent license, such license is granted
* solely to the extent that any such patent is necessary to Utilize the software alone.
* The patent license shall not apply to any combinations which include this software,
* other than combinations with devices manufactured by or for TI ("TI Devices").
* No hardware patent is licensed hereunder.
*
* Redistributions must preserve existing copyright notices and reproduce this license
* (including the above copyright notice and the disclaimer and (if applicable) source
* code license limitations below) in the documentation and/or other materials provided
* with the distribution
*
* Redistribution and use in binary form, without modification, are permitted provided
* that the following conditions are met:
*
* *       No reverse engineering, decompilation, or disassembly of this software is
* permitted with respect to any software provided in binary form.
*
* *       any redistribution and use are licensed by TI for use only with TI Devices.
*
* *       Nothing shall obligate TI to provide you with source code for the software
* licensed and provided to you in object code.
*
* If software source code is provided to you, modification and redistribution of the
* source code are permitted provided that the following conditions are met:
*
* *       any redistribution and use of the source code, including any resulting derivative
* works, are licensed by TI for use only with TI Devices.
*
* *       any redistribution and use of any object code compiled from the source code
* and any resulting derivative works, are licensed by TI for use only with TI Devices.
*
* Neither the name of Texas Instruments Incorporated nor the names of its suppliers
*
* may be used to endorse or promote products derived from this software without
* specific prior written permission.
*
* DISCLAIMER.
*
* THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY EXPRESS
* OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL TI AND TI'S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
* OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
* OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/

/**
 *  \file     EthApp.c
 *
 *  \brief    This file contains the target-side Eth test application code.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "portmacro.h"

/* AutoSAR specific Header files */
#include "Eth_Cfg.h"
#include "Eth.h"
#include "Eth_Irq.h"
#include "EthTrcv.h"

#include "EthUtils.h"
#include "EthUtils_Target.h"
#include "Eth/src/csl/csl_cpgmac_sl.h"
#include "Eth/src/csl/cslr_xge_cpsw_ss_s.h"
#if (STD_ON == ETH_VIRTUALMAC_SUPPORT)
#include "EthUtils_VirtMac.h"
#endif
#include "os_wrapper.h"
#include "board_utils.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

#define ETHAPP_TASK_HIGH_PRI            (configMAX_PRIORITIES-1)

#define ETHAPP_TASK_DEFAULT_PRI         (configMAX_PRIORITIES-10)

#define ETHAPP_TASK_DEFAULT_STACK_SIZE  (16384U/sizeof(configSTACK_DEPTH_TYPE))

#define ETHAPP_TASK_NUM                 (4U)

#define ETHAPP_TEST_SIZE_MEASURE        (5U)

/* Register to get RGMII speed information */
#define CSL_XGE_CPSW_SS_S_RGMII_STATUS_REG(n)                (0x00000030U + (n)*4u)
#define CSL_XGE_CPSW_SS_S_RGMII_STATUS_REG_SPEED_MASK        (0x00000006U)
#define CSL_XGE_CPSW_SS_S_RGMII_STATUS_REG_SPEED_SHIFT       (0x00000001U)
#define CSL_XGE_CPSW_SS_S_RGMII_STATUS_REG_SPEED_MAX         (0x00000003U)

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

typedef void (*task_fxn)(void*, void*);

typedef struct EthApp_TaskCfgType_s
{
    const char   *name;
    uint8        priority;
    uint32       stackSize;
    uint8        *stackMem;
    void         *handle;
    task_fxn     entry;
} EthApp_TaskCfgType;

typedef enum EthApp_TestType_e
{
    ETHAPP_TEST_TX  = 0u,
    ETHAPP_TEST_RX  = 1u,
    ETHAPP_TEST_NUM = 2u,
} EthApp_TestType;

typedef struct EthApp_TestResultType_s
{
    uint32 cpuload;
    uint32 pps;
    uint32 mbps;
} EthApp_TestResultType;

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */

#define ETH_START_SEC_ISR_CODE
#include "Eth_MemMap.h"
static void EthApp_TxIrqHdlr_0_wrapper(void* arg);
static void EthApp_RxIrqHdlr_0_wrapper(void* arg);
static void Eth_TxIrqPacingHdlr_0_wrapper(void* arg);
static void Eth_RxIrqPacingHdlr_0_wrapper(void* arg);
#define ETH_STOP_SEC_ISR_CODE
#include "Eth_MemMap.h"

static void EthApp_interruptConfigCustom(void);

/**
 * \brief Measure DUT transmit throughput
 *
 * Measure DUT transmit throughput over "frames" frames with frame size is "size" 
 * . The target performs the following operations:
 * - Get the initial timestamp corresponding to the first frame received
 * - Receive frames until the STOP is detected and get the final timestamp
 * - Compute the transmit throughput from the number of received frames and the
 *   time difference between the two captured timestamps
 *
 * \return PASS or FAIL  Test result
 */
boolean EthApp_test_0100(uint32 frames, uint32 size);

/* Rx task main function */
void EthApp_rxTask(void *args1, void *args2);

/* Tx task main function */
void EthApp_txTask(void *args1, void *args2);

/* Monitor task main function */
void EthApp_monTask(void *args1, void *args2);

/* Init task main function */
void EthApp_initTask(void *args1, void *args2);

/* ========================================================================== */
/*                          External Functions                                */
/* ========================================================================== */
/* ========================================================================== */
/*                          External Functions                                */
/* ========================================================================== */
#ifdef __cplusplus
extern "C" {
#endif

extern Std_ReturnType Udma_LL_Event_Disable(uint32 eventHardwareNum);
extern int EthApp_Main(void);

#ifdef __cplusplus
}
#endif
/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */


/* Eth application state */
EthApp_StateSet gEthApp;
uint8 gCtrlIdx = ETH_CTRL_ID_0;


volatile uint32 rxPollingTime=50u;
volatile boolean isRunning=TRUE;
volatile boolean isInited=FALSE;
volatile uint32 lastTxConfNum = 0u;

SemaphoreHandle_t gRxSem;
SemaphoreHandle_t gStatsSem;
SemaphoreHandle_t gBucketSem;

StackType_t gMainRxTaskStack[ETHAPP_TASK_DEFAULT_STACK_SIZE]    __attribute__((aligned(32)));
StackType_t gMainTxTaskStack[ETHAPP_TASK_DEFAULT_STACK_SIZE]    __attribute__((aligned(32)));
StackType_t gMainMonTaskStack[ETHAPP_TASK_DEFAULT_STACK_SIZE]   __attribute__((aligned(32)));
StackType_t gMainInitTaskStack[ETHAPP_TASK_DEFAULT_STACK_SIZE]  __attribute__((aligned(32)));

EthApp_TaskCfgType EthApp_taskCfg[ETHAPP_TASK_NUM] =
{
    {
        "RX",
        ETHAPP_TASK_DEFAULT_PRI,
        ETHAPP_TASK_DEFAULT_STACK_SIZE,
        (uint8*)gMainRxTaskStack,
        NULL_PTR,
        &EthApp_rxTask

    },
    {
        "TX",
        ETHAPP_TASK_DEFAULT_PRI,
        ETHAPP_TASK_DEFAULT_STACK_SIZE,
        (uint8*)gMainTxTaskStack,
        NULL_PTR,
        &EthApp_txTask
    },
    {
        "MON",
        ETHAPP_TASK_DEFAULT_PRI,
        ETHAPP_TASK_DEFAULT_STACK_SIZE,
        (uint8*)gMainMonTaskStack,
        NULL_PTR,
        &EthApp_monTask
    },
    {
        "INIT",
        ETHAPP_TASK_HIGH_PRI, /* need to be highest */
        ETHAPP_TASK_DEFAULT_STACK_SIZE,
        (uint8*)gMainInitTaskStack,
        NULL_PTR,
        &EthApp_initTask
    }  
};

/* Test measurement data */
uint16      gTxPktSizeMesure[] = { 1500u,   512u,     256u,     128u,     64u      };
uint32      gTxIteratorNum[]   = { 640000u, 1280000u, 1280000u, 1280000u, 1280000u };
uint32      gTxNumTest         = sizeof(gTxPktSizeMesure)/sizeof(uint16);

uint16      gRxPktSizeMesure[] = { 1500u,   512u,     256u,     128u,     64u      };
uint32      gRxIteratorNum[]   = { 640000u, 1280000u, 2560000u, 3072000u, 2560000u };
uint32      gRxNumTest         = sizeof(gRxPktSizeMesure)/sizeof(uint16);

TickType_t  gStartTestTick = 0u;
TickType_t  gEndTestTick   = 0u;

EthApp_TestType gTestType = ETHAPP_TEST_TX;
uint16 gTxTestIdx = 0u;
uint16 gRxTestIdx = 0u;

EthApp_TestResultType gTxTestResult[ETHAPP_TEST_SIZE_MEASURE];
EthApp_TestResultType gRxTestResult[ETHAPP_TEST_SIZE_MEASURE];

uint32 gTestSpeed = ETHTRCV_BAUD_RATE_1000MBIT; /* default is 1Gpbs */

volatile sint32 gBucketCount;

/* 
 * A Simple bucket rate limit implementation, this will help to reduce switch context time
 * and shows for better CPU performace
 */
static inline boolean EthApp_bucketIncrease(sint32 count)
{
    boolean isFull = FALSE;

    uintptr_t k = HwiP_disable();

    if(gBucketCount < Eth_GetEgressFifoPacketNum(gEthApp.state[gCtrlIdx].ctrlIdx, 0u))
    {
        gBucketCount += count;
        if(gBucketCount  >= Eth_GetEgressFifoPacketNum(gEthApp.state[gCtrlIdx].ctrlIdx, 0u))
        {
            gBucketCount  = Eth_GetEgressFifoPacketNum(gEthApp.state[gCtrlIdx].ctrlIdx, 0u);
            isFull = TRUE;
        }
    }

    HwiP_restore(k);

    return isFull;
}

static inline boolean EthApp_bucketTryDecrease(sint32 count)
{
    boolean isOk = FALSE;
    sint32 tmp;

    uintptr_t k = HwiP_disable();

    tmp = gBucketCount - count;
    if(tmp >= 0)
    {
        gBucketCount = tmp;
        isOk = TRUE;
    }

    HwiP_restore(k);

    return isOk;
}

/* Reduce test packet number by speed */
static inline uint8 EthApp_GetScaleValue(void)
{
    uint8 scale = 1;
    switch (gTestSpeed)
    {
    case ETHTRCV_BAUD_RATE_10MBIT:
        scale = 20u;
        break;
    case ETHTRCV_BAUD_RATE_100MBIT:
        scale = 5u;
        break;
    case ETHTRCV_BAUD_RATE_1000MBIT:
        scale = 1u;
        break;
    default:
        break;
    }

    return scale;
}

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */
int EthApp_CpuloadMain(void)
{
    uint32 ci;
    Std_ReturnType retVal;
#if (STD_OFF == ETH_PRE_COMPILE_VARIANT)
    /* this time configuration is not ready yet */
    Eth_CfgPtr = (const Eth_ConfigType *)&EthConfigSet_0;
#endif

    for(ci = 0u; ci < ETH_CTRL_ID_MAX; ++ci)
    {
        gEthApp.state[ci].ctrlIdx = ci;
    }

    /* System initialization */
    EthApp_Startup();

    /* Register custom IRQ for the app */
    EthApp_interruptConfigCustom();

#if (STD_ON == ETH_VIRTUALMAC_SUPPORT)
    if(TRUE == Eth_IsVirtualMacModeEnable(gEthApp.state[gCtrlIdx].ctrlIdx))
    {
        retVal = EthUtils_VirtMacinit(gEthApp.state[gCtrlIdx].ctrlIdx);
        if (E_OK != retVal)
        {
            EthUtils_printf("Failed to initialize virtmac: %d\r\n", retVal);
            return retVal;
        }
    }
#endif

    /* Print date and time */
    EthUtils_printf("---\r\nEth Application build date/time: %s, %s\r\n", __DATE__, __TIME__);

    EthUtils_printf("Eth Application Starts\r\n");

    retVal = EthApp_initDma();
    if (E_OK != retVal)
    {
        EthUtils_printf("Failed to initialize DMA: %d\r\n", retVal);
        return retVal;
    }

#if (STD_OFF == ETH_PRE_COMPILE_VARIANT)
    Eth_CfgPtr = (Eth_ConfigType *)NULL_PTR;
#endif

    /* Initialize the Eth driver and controller */
    retVal = EthApp_initDriver();
    if (E_OK != retVal)
    {
        EthUtils_printf("Failed to initialize: %d\r\n", retVal);
        return retVal;
    }

    /* Setup the Eth driver and controller */
    retVal = EthApp_initAll();

#if (STD_ON == ETH_VIRTUALMAC_SUPPORT)
    if (E_OK != retVal)
    {
        EthUtils_VirtMacRegisterMacAddr(gEthApp.state[gCtrlIdx].ctrlIdx, gEthApp.macAddr);
        EthUtils_VirtMacSubMcastAddr(gEthApp.state[gCtrlIdx].ctrlIdx, TargetMcastAddr);
    }
#endif

    return 0;
}

#define ETH_START_SEC_ISR_CODE
#include "Eth_MemMap.h"

static void EthApp_TxIrqHdlr_0_wrapper(void* arg)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    uint32 numPktProc = 0u;

    Eth_TxIrqHdlr_0();

    /* calculate how much packet we procces in application */
    numPktProc = gEthApp.state[gCtrlIdx].stats.txConfPktCnt - lastTxConfNum;
    lastTxConfNum = gEthApp.state[gCtrlIdx].stats.txConfPktCnt;

    /* notify tx task if we have enough bucket */
    if(TRUE == EthApp_bucketIncrease(numPktProc))
    {
        xSemaphoreGiveFromISR(gBucketSem, &xHigherPriorityTaskWoken);
        portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
    }
}

static void EthApp_RxIrqHdlr_0_wrapper(void* arg)
{
    /* clear irq and let rx task take care packet */
    Udma_LL_Event_Disable(Eth_GetEventVirtIntrNum(gCtrlIdx, 1u));
}

static void Eth_TxIrqPacingHdlr_0_wrapper(void* arg)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    uint32 numPktProc = 0u;

    Eth_TxIrqPacingHdlr_0();

    /* calculate how much packet we procces in application */
    numPktProc = gEthApp.state[gCtrlIdx].stats.txConfPktCnt - lastTxConfNum;
    lastTxConfNum = gEthApp.state[gCtrlIdx].stats.txConfPktCnt;

    /* notify tx task if we have enough bucket */
    if(TRUE == EthApp_bucketIncrease(numPktProc))
    {
        xSemaphoreGiveFromISR(gBucketSem, &xHigherPriorityTaskWoken);
        portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
    }
}

static void Eth_RxIrqPacingHdlr_0_wrapper(void* arg)
{
    /* nothing do here, Rx task do main job */
}

#define ETH_STOP_SEC_ISR_CODE
#include "Eth_MemMap.h"

static void EthApp_interruptConfigCustom(void)
{
    Std_ReturnType retVal = E_OK;

    /* RX DMA completion interrupt */
    if(Eth_GetRxEnableInterrupt(gCtrlIdx) == TRUE)
    {
        retVal = BoardUtils_registerInterrupt(
                            Eth_GetRxEventCoreIntrNum(gEthApp.state[gCtrlIdx].ctrlIdx),
                            &EthApp_RxIrqHdlr_0_wrapper);
        if (E_OK != retVal)
        {
            EthUtils_printf("%s: failed to register RX DMA interrupt\n", __func__);
        }
    }

    if(Eth_GetTxEnableInterrupt(gCtrlIdx) == TRUE && E_OK == retVal)
    {
        retVal = BoardUtils_registerInterrupt(
                            Eth_GetTxEventCoreIntrNum(gEthApp.state[gCtrlIdx].ctrlIdx),
                            &EthApp_TxIrqHdlr_0_wrapper);
        if (E_OK != retVal)
        {
            EthUtils_printf("%s: failed to register TX DMA interrupt\n", __func__);
        }
    }

    if(TRUE == Eth_GetRxIrqPacingEnable(gCtrlIdx) && E_OK == retVal)
    {
        retVal = BoardUtils_registerInterrupt(
                            ETH_RX_IRQ_PACING_HW_TIMER_IRQ_NUM,
                            &Eth_RxIrqPacingHdlr_0_wrapper);
        if (E_OK != retVal)
        {
            EthUtils_printf("%s: failed to register Rx Hw Timer interrupt\n", __func__);
        }
    }

    if(TRUE == Eth_GetTxIrqPacingEnable(gCtrlIdx) && E_OK == retVal)
    {
        retVal = BoardUtils_registerInterrupt(
                            ETH_TX_IRQ_PACING_HW_TIMER_IRQ_NUM,
                            &Eth_TxIrqPacingHdlr_0_wrapper);
        if (E_OK != retVal)
        {
            EthUtils_printf("%s: failed to register Tx Hw Timer interrupt\n", __func__);
        }
    }
}

boolean EthApp_test_0100(uint32 frames, uint32 size)
{
    Eth_BufIdxType bufIdx;
    boolean txConfirmation = TRUE;
    uint8 *bufPtr;
    uint32 iterations = frames;
    uint32 i;
    uint16 len = size;
    uint8 priority = 0U;
    uint32 step;

    /* Send START cmd */
    EthApp_sendCmd(gEthApp.state[gCtrlIdx].ctrlIdx, CTRL_FRAME_CMD_START);

    /* wait a bit to flush all control packet */
    vTaskDelay(100);

    while(iterations > 0)
    {
        uint32 count = Eth_GetEgressFifoPacketNum(gEthApp.state[gCtrlIdx].ctrlIdx, 0u);        /* proccess max bucket size at a time */
        boolean is_OK = EthApp_bucketTryDecrease(count);

        while(is_OK == FALSE)                          /* is bucket enough? */
        {
            xSemaphoreTake(gBucketSem, portMAX_DELAY); /* wait until bucket is full */
            is_OK = EthApp_bucketTryDecrease(count);   /* retry */
        }

        step = (count > iterations) ? iterations : count;
        iterations -= count;

        for(i = 0; i < step; ++i)
        {
            (void)EthApp_provideTxBuffer(gEthApp.state[gCtrlIdx].ctrlIdx,
                                           priority,
                                           &bufIdx,
                                           &bufPtr,
                                           &len);

            (void)Eth_Transmit(gEthApp.state[gCtrlIdx].ctrlIdx,
                                           bufIdx,
                                           (Eth_FrameType)ETHERTYPE_EXPERIMENTAL3,
                                           txConfirmation,
                                           len,
                                           BcastAddr);
        }
    }

    if(gTestSpeed != ETHTRCV_BAUD_RATE_1000MBIT)
    {
        vTaskDelay(40);
    }

    /* Send STOP cmd */
    EthApp_sendCmd(gEthApp.state[gCtrlIdx].ctrlIdx, CTRL_FRAME_CMD_STOP);
    return PASS;
}

void EthApp_receiveTput(uint32 frames, uint16 pktLen, uint32 *pps, uint32 *mbps)
{
    float32 elapsed;
    uint32  framesOk;
    uint64  byteCnt;

    elapsed = (float32)(gEndTestTick-gStartTestTick)/1000;

    framesOk =  gEthApp.state[gCtrlIdx].stats.rxExp3PktCnt;
    byteCnt =  gEthApp.state[gCtrlIdx].stats.rxExp3BytesCnt;

    /* Compute elapsed time, packets per second and Mbps */
    *pps = (uint32)framesOk / elapsed;
    *mbps = (uint32)((byteCnt * 8) / (elapsed * 1000000));
}

void EthApp_transmitTput(uint32 frames, uint16 pktLen, uint32 *pps, uint32 *mbps)
{
    float32 elapsed;
    uint32  framesOk;
    uint64  byteCnt;

    elapsed = (float32)(gEndTestTick-gStartTestTick)/1000;

    framesOk =  gEthApp.state[gCtrlIdx].stats.txConfPktCnt;
    byteCnt =  (uint64)(gEthApp.state[gCtrlIdx].stats.txConfPktCnt * (pktLen + 14u));

    /* Compute elapsed time, packets per second and Mbps */
    *pps = (uint32)framesOk / elapsed;
    *mbps = (uint32)((byteCnt * 8) / (elapsed * 1000000));
}

void EthApp_monShowStats()
{
    if(gTestType == ETHAPP_TEST_TX)
    {
        gTxTestResult[gTxTestIdx].cpuload = OsUtils_getCpuload(EthApp_taskCfg[1u].handle);
        EthApp_transmitTput(gTxIteratorNum[gTxTestIdx]/EthApp_GetScaleValue(), gTxPktSizeMesure[gTxTestIdx],
            &gTxTestResult[gTxTestIdx].pps, &gTxTestResult[gTxTestIdx].mbps);

        EthUtils_printf("Tx throughput test size %4d Done in %2ds load %d\n", gTxPktSizeMesure[gTxTestIdx],
            (gEndTestTick-gStartTestTick)/1000, gTxTestResult[gTxTestIdx].cpuload );
    }
    else
    {
        gRxTestResult[gRxTestIdx].cpuload = OsUtils_getCpuload(EthApp_taskCfg[0u].handle);
        EthApp_receiveTput(gRxIteratorNum[gRxTestIdx]/EthApp_GetScaleValue(), gRxPktSizeMesure[gRxTestIdx],
            &gRxTestResult[gRxTestIdx].pps, &gRxTestResult[gRxTestIdx].mbps);

        EthUtils_printf("Rx throughput test size %4d Done in %2ds load %d\n", gRxPktSizeMesure[gRxTestIdx],
            (gEndTestTick-gStartTestTick)/1000, gRxTestResult[gRxTestIdx].cpuload);
    }
}

void EthApp_TestFunc(uint8 testType, uint8 testId)
{
    gTestType = (EthApp_TestType)testType;

    EthApp_sendCmd(gEthApp.state[gCtrlIdx].ctrlIdx, CTRL_FRAME_CMD_READY);

    gStartTestTick = xTaskGetTickCount();

    if(gTestType == ETHAPP_TEST_TX)
    {
        gEthApp.state[gCtrlIdx].runTest = TRUE;
        gEthApp.state[gCtrlIdx].stats.txConfPktCnt = 0u;
        gTxTestIdx = testId;
        (void)EthApp_test_0100(gTxIteratorNum[gTxTestIdx]/EthApp_GetScaleValue(), gTxPktSizeMesure[gTxTestIdx]);
    }
    else
    {
        gEthApp.state[gCtrlIdx].runTest = TRUE;
        gEthApp.state[gCtrlIdx].stats.rxPktCnt       = 0u;
        gEthApp.state[gCtrlIdx].stats.rxExp3PktCnt   = 0u;
        gEthApp.state[gCtrlIdx].stats.rxExp3BytesCnt = 0u;
        gRxTestIdx = testId;

        EthApp_sendCmd(gEthApp.state[gCtrlIdx].ctrlIdx, CTRL_FRAME_CMD_START);
        /* wait rx test done */
        xSemaphoreTake((SemaphoreHandle_t)gEthApp.state[gCtrlIdx].testDoneNotifySem, portMAX_DELAY);
    }

    gEndTestTick = xTaskGetTickCount();
    xSemaphoreGive(gStatsSem);

    /* Wait a little bit to let Mon task finish its job */
    vTaskDelay(1000);
}

void EthApp_TestFuncAll()
{
    uint8 testType;
    uint8 testId;
    for(testType = 0u; testType < ETHAPP_TEST_NUM; ++testType)
    {
        if(testType == ETHAPP_TEST_RX)
        {
            rxPollingTime = 1u;
            xSemaphoreGive(gRxSem);
            vTaskDelay(100);
        }

        for(testId = 0u; testId < ETHAPP_TEST_SIZE_MEASURE; ++testId)
        {
            OsUtils_resetCpuload();

            EthApp_TestFunc(testType, testId);
        }
    }  
}

void EthApp_txTask(void *args1, void *args2)
{
    /* only start test after init done */
    while(isInited == FALSE);

    EthApp_TestFuncAll();

    isRunning = FALSE;
    xSemaphoreGive(gStatsSem);
    vTaskDelay(5000);

    /* Exit freertos */
    exit(0);

    (void)args1;
    (void)args2;
}

void EthApp_rxTask(void *args1, void *args2)
{
    Eth_RxStatusType rxStatus = ETH_NOT_RECEIVED;

    /* only start test after init done */
    while(isInited == FALSE);

    for(;;)
    {
        xSemaphoreTake(gRxSem, rxPollingTime);
        if(isRunning == FALSE)
        {
            break;
        }        
        if(TRUE == Eth_GetRxIrqPacingEnable(gCtrlIdx))
        {
            Eth_RxIrqPacingHdlr_0();
        }
        else
        {
            rxStatus = ETH_RECEIVED_MORE_DATA_AVAILABLE;
            while(rxStatus == ETH_RECEIVED_MORE_DATA_AVAILABLE)
            {
                Eth_Receive(0u, 0u, &rxStatus);
            }
        }
    }

    vTaskDelete(NULL);
    (void)args1;
    (void)args2;
}

void EthApp_monTask(void *args1, void *args2)
{
    uint8 idx;

    for(;;)
    {
        xSemaphoreTake(gStatsSem, portMAX_DELAY);
        if(isRunning == FALSE)
        {
            break;
        }
        EthApp_monShowStats();
    }

    /* Show test result */
    EthUtils_printf("\nPerformance result - %s link\n\n",
        (gTestSpeed == 0U) ? "10Mbps": ((gTestSpeed == 1U) ? "100Mbps":"1Gbps"));
    EthUtils_printf("----------------------------------------------------------------------\n");
    EthUtils_printf("|Pkt Size | TX throughput(Mbps) | TX packet rate(Pps) | Cpu load(%%) |\n");
    EthUtils_printf("---------------------------------------------------------------------\n");

    for(idx = 0u; idx < ETHAPP_TEST_SIZE_MEASURE; ++idx)
    {
        EthUtils_printf("|%8d |%20d |%20d |%12d |\n",
          gTxPktSizeMesure[idx], gTxTestResult[idx].mbps, gTxTestResult[idx].pps, gTxTestResult[idx].cpuload);
        EthUtils_printf("---------------------------------------------------------------------\n");
    }
    
    EthUtils_printf("----------------------------------------------------------------------\n");
    EthUtils_printf("|Pkt Size | RX throughput(Mbps) | RX packet rate(Pps) | Cpu load(%%) |\n");
    EthUtils_printf("---------------------------------------------------------------------\n");

    for(idx = 0u; idx < ETHAPP_TEST_SIZE_MEASURE; ++idx)
    {
        EthUtils_printf("|%8d |%20d |%20d |%12d |\n",
          gRxPktSizeMesure[idx], gRxTestResult[idx].mbps, gRxTestResult[idx].pps, gRxTestResult[idx].cpuload);
        EthUtils_printf("---------------------------------------------------------------------\n");
    }

    vTaskDelete(NULL);
    (void)args1;
    (void)args2;
}

void EthApp_initTask(void *args1, void *args2)
{
    EthApp_CpuloadMain();

    if(FALSE == Eth_IsVirtualMacModeEnable(gEthApp.state[gCtrlIdx].ctrlIdx))
    {
        volatile uint32 rgmiiStatusReg;
        uint32 regVal = 0u;

        /* Link speed will automatic get from partner */
        regVal = CSL_CPGMAC_SL_getMacControlReg((CSL_Xge_cpswRegs*)Eth_Cpsw_GetCtrlRegAddr(),
            Eth_GetMacPortNum(gEthApp.state[gCtrlIdx].ctrlIdx)-1u);
        regVal |= CSL_CPGMAC_SL_MACCONTROL_EXT_EN;
        CSL_CPGMAC_SL_setMacControlReg((CSL_Xge_cpswRegs*)Eth_Cpsw_GetCtrlRegAddr(),
            Eth_GetMacPortNum(gEthApp.state[gCtrlIdx].ctrlIdx)-1u, regVal);

        /* Get link speed in RGMII register */
        rgmiiStatusReg = (Eth_Cpsw_GetCtrlRegAddr() - CPSW_NU_OFFSET) +
            CSL_XGE_CPSW_SS_S_RGMII_STATUS_REG(Eth_GetMacPortNum(gEthApp.state[gCtrlIdx].ctrlIdx)-1);

        gTestSpeed = CSL_FEXT((*(uint32*)rgmiiStatusReg), XGE_CPSW_SS_S_RGMII_STATUS_REG_SPEED);
    }

    EthUtils_printf("Link Speed %s\n",  (gTestSpeed == 0U) ? "10Mbps": 
        ((gTestSpeed == 1U) ? "100Mbps":"1Gbps"));

    gBucketCount = Eth_GetEgressFifoPacketNum(gEthApp.state[gCtrlIdx].ctrlIdx, 0u);

    OsUtils_resetCpuload();

    isInited=TRUE;

    vTaskDelay(portMAX_DELAY);
    (void)args1;
    (void)args2;
}

int EthApp_Main(void)
{
    uint8 taskIdx;

    OsUtils_init();

    gBucketSem = xSemaphoreCreateBinary();
    gRxSem = xSemaphoreCreateBinary();
    gEthApp.state[gCtrlIdx].testDoneNotifySem = (void*)xSemaphoreCreateBinary();
    gStatsSem = xSemaphoreCreateBinary();

    configASSERT(gRxSem != NULL);
    configASSERT(gEthApp.state[gCtrlIdx].testDoneNotifySem != NULL);
    configASSERT(gStatsSem != NULL);

    for(taskIdx = 0u; taskIdx < ETHAPP_TASK_NUM; ++taskIdx)
    {
        EthApp_taskCfg[taskIdx].handle = OsUtils_createTask(
            EthApp_taskCfg[taskIdx].name,
            EthApp_taskCfg[taskIdx].priority,
            EthApp_taskCfg[taskIdx].stackSize,
            EthApp_taskCfg[taskIdx].stackMem,
            EthApp_taskCfg[taskIdx].entry
            );
        configASSERT(EthApp_taskCfg[taskIdx].handle != NULL);
    }

    /* Start the scheduler to start the tasks executing. */
    vTaskStartScheduler();

    return 0;
}