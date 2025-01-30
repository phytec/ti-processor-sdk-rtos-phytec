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
 *  \file     EthVirtmacApp.c
 *
 *  \brief    This file contains the target-side Eth test application code.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ti/csl/arch/r5/interrupt.h>
#include <ti/csl/soc.h>
#include <ti/csl/hw_types.h>
#include <ti/osal/osal.h>
#include <ti/osal/TaskP.h>
#include <ti/osal/SemaphoreP.h>
#include <ti/osal/TimerP.h>
#include <ti/drv/sciclient/sciclient.h>
#include <CddIpc/ipc_baremetal_hw/ipc.h>
#include <CddIpc/ipc_baremetal_hw/soc/ipc_soc.h>
#include <hw_include/lldr.h>
#include <hw_include/lld_mailbox.h>

#include "Eth.h"
#include "Eth_Rpc.h"
#include "Eth_Irq.h"
#include "Eth_VirtMac.h"

#include "Det.h"
#include "Dem.h"
#include "Os.h"
#include "Cdd_Ipc.h"
#include "Cdd_IpcIrq.h"

#include "EthUtils.h"

#if defined (SOC_J721E)
#include "IntRtr_Cfg.h"
#endif

#if (defined (BUILD_MCU1_0) && (defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J784S4)))
#include <ti/drv/sciclient/sciserver_tirtos.h>
#include <ti/drv/sciclient/examples/common/sciclient_appCommon.h>
#endif

#include "EthUtils.h"
#include "EthUtils_Target.h"
#include "EthUtils_VirtMac.h"
#include "board_utils.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

#if (STD_OFF == ETH_VIRTUALMAC_SUPPORT)
#error "ETH_VIRTUALMAC_SUPPORT is required for Ethernet Virtual MAC test app"
#endif
#if (STD_OFF == ETH_VIRTUALMAC_SUBSCRIBEDSTMAC_API)
#error "ETH_VIRTUALMAC_SUBSCRIBEDSTMAC_API is required for Ethernet Virtual MAC test app"
#endif
#if (STD_OFF == ETH_VIRTUALMAC_UNSUBSCRIBEDSTMAC_API)
#error "ETH_VIRTUALMAC_UNSUBSCRIBEDSTMAC_API is required for Ethernet Virtual MAC test app"
#endif
#if (STD_OFF == ETH_VIRTUALMAC_NOTIFYMSGRECEIVED_API)
#error "ETH_VIRTUALMAC_NOTIFYMSGRECEIVED_API is required for Ethernet Virtual MAC test app"
#endif
#if (STD_OFF == ETH_VIRTUALMAC_ADD_UNICAST_MACADDR_API)
#error "ETH_VIRTUALMAC_ADD_UNICAST_MACADDR_API is required for Ethernet Virtual MAC test app"
#endif
#if (STD_OFF == ETH_VIRTUALMAC_DEL_MACADDR_API)
#error "ETH_VIRTUALMAC_DEL_MACADDR_API is required for Ethernet Virtual MAC test app"
#endif
#if (STD_OFF == ETH_VIRTUALMAC_ADD_MCAST_MACADDR_API)
#error "ETH_VIRTUALMAC_ADD_MCAST_MACADDR_API is required for Ethernet Virtual MAC test app"
#endif
#if (STD_OFF == ETH_VIRTUALMAC_ADD_VLAN_API)
#error "ETH_VIRTUALMAC_ADD_VLAN_API is required for Ethernet Virtual MAC test app"
#endif
#if (STD_OFF == ETH_VIRTUALMAC_DEL_VLAN_API)
#error "ETH_VIRTUALMAC_DEL_VLAN_API is required for Ethernet Virtual MAC test app"
#endif

/* Test application stack size */
#define APP_TASK_STACK                  (32U * 1024U)
/**< Stack required for the stack */
#define VIRTETH_PROFILE_DEMO_TASK_NAME  ("VirEth PROFILE")
/**< Task names */
#define VIRTETH_TASK_PRIORITY           (3)
/**< Task Priority Levels */

#if (defined (BUILD_MCU1_0))
#define freertos
#endif

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/* ========================================================================== */
/*                          External Functions                                */
/* ========================================================================== */
#ifdef __cplusplus
extern "C"
{
#endif

extern int EthApp_Main(void);

#ifdef __cplusplus
}
#endif

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */
/**
 * \brief Test basic frame reception
 *
 * Basic DUT frame reception test.  The target side performs these operations:
 * - Send START command
 * - Receive frames until the STOP is detected
 *
 * The number of frames expected to be received is ETH_TEST_ITER_M_COUNT.
 *
 * \return PASS or FAIL  Test result
 */
boolean EthVirtmacApp_test_0001(void);

/**
 * \brief Test basic frame transmission
 *
 * Basic DUT frame transmission test.  The target side performs these operations:
 * - Send START command
 * - Transmit ETH_TEST_ITER_M_COUNT non-VLAN tagged frames without confirmation
 *
 * \return PASS or FAIL  Test result
 */
boolean EthVirtmacApp_test_0002(void);

/**
 * \brief Test external loopback
 *
 * External loopback test.  The target side sends frames to the host, the host
 * receives the frames and sends them back to the target.  The following operations
 * are performed on the target side:
 * - Send START command
 * - Send and receive ETH_TEST_ITER_M_COUNT frames, for each of them:
 *   o A different payload is set
 *   o EtherType and payload are verified on the received frame
 *
 * \return PASS or FAIL  Test result
 */
boolean EthVirtmacApp_test_0003(void);

/**
 * \brief Test default filter operation
 *
 * Test the operation of the default DUT's filter configuration (only frames with
 * DUT's MAC address are accepted, all others are rejected).  The target side
 * performs these operations:
 * - Send START command
 * - Receive frames until the STOP is detected
 *
 * The number of frames expected to be received is ETH_TEST_ITER_M_COUNT / 2 as
 * only one half of the total frames sent by the host have the DUT's MAC address.
 *
 * \return PASS or FAIL  Test result
 */
boolean EthVirtmacApp_test_0004(void);

/**
 * \brief Test different filter configurations
 *
 * Test the different modes of the DUT's filter configuration.  The target side performs
 * these operations:
 * - Send START command
 * - Part 1
 *   o Filter is reset to accept only frames with DUT's MAC address
 *   o Receive frames until STOP is detected
 *     - Frame 1: Source MAC address is DUT's. It should be accepted
 *     - Frame 2: Source MAC address is not DUT's. It should be rejected
 *     - Expected to receive ETH_TEST_ITER_M_COUNT / 2 frames
 * - Part 2
 *   o A second unicast MAC address is added to the DUT's filter
 *   o Receive frames until STOP is detected
 *     - Frame 1: Source MAC address is DUT's. It should be accepted
 *     - Frame 2: Source MAC address is second valid MAC address. It should be accepted
 *     - Expected to receive ETH_TEST_ITER_M_COUNT frames
 * - Part 3
 *   o A multicast address is added to the DUT's filter
 *   o Receive frames until STOP is detected
 *     - Frame 1: Source MAC address is DUT's. It should be accepted
 *     - Frame 2: Source MAC address is multicast address. It should be accepted
 *     - Expected to receive ETH_TEST_ITER_M_COUNT frames
 * - Part 4
 *   o The multicast address is removed from the DUT's filter
 *   o Receive frames until STOP is detected
 *     - Frame 1: Source MAC address is DUT's. It should be accepted
 *     - Frame 2: Source MAC address is multicast address. It should be rejected
 *     - Expected to receive ETH_TEST_ITER_M_COUNT / 2 frames
 * - Part 5
 *   o The filter is opened up (promiscuous mode)
 *   o Receive frames until STOP is detected
 *     - Frame 1: Source MAC address is DUT's. It should be accepted
 *     - Frame 2: Source MAC address is not DUT's. It should be accepted
 *     - Expected to receive ETH_TEST_ITER_M_COUNT frames
 * - Part 6
 *   o The filter is reset back to accept only DUT's unicast address
 *   o Receive frames until STOP is detected
 *     - Frame 1: Source MAC address is DUT's. It should be accepted
 *     - Frame 2: Source MAC address is not DUT's. It should be rejected
 *     - Expected to receive ETH_TEST_ITER_M_COUNT / 2 frames
 *
 * \return PASS or FAIL  Test result
 */
boolean EthVirtmacApp_test_0005(void);


/**
 * \brief Test frame transmission with confirmation
 *
 * DUT frame transmission test with TX confirmation.  The target side performs
 * these operations:
 * - Send START command
 * - Transmit ETH_TEST_ITER_M_COUNT non-VLAN tagged frames with confirmation
 *
 * \return PASS or FAIL  Test result
 */
boolean EthVirtmacApp_test_0006(void);

/**
 * \brief Test frame transmission of VLAN tagged frames
 *
 * DUT frame transmission test of VLAN tagged frames.  The target side performs
 * these operations:
 * - Send START command
 * - Transmit ETH_TEST_ITER_M_COUNT VLAN tagged frames with confirmation
 *
 * \return PASS or FAIL  Test result
 */
boolean EthVirtmacApp_test_0007(void);

/**
 * \brief Test frame transmission of different lengths
 *
 * DUT frame transmission test of different frame lengths, including short frames
* (less than 64 octets).  The target side performs these operations:
 * - Send START command
 * - Transmit ETH_TEST_ITER_S_COUNT frames for lengths starting at 10 octets in
 *   increments of 10 octets until 1500 octets.
 *
 * \return PASS or FAIL  Test result
 */
boolean EthVirtmacApp_test_0008(void);

/**
 * \brief Test VLAN tagged frame reception
 *
 * DUT VLAN tagged frame reception test.  The target side performs these operations:
 * - Send START command
 * - Receive frames until the STOP is detected
 *
 * The number of frames expected to be received is ETH_TEST_ITER_M_COUNT.
 *
 * \return PASS or FAIL  Test result
 */
boolean EthVirtmacApp_test_0009(void);

/**
 * \brief Test controller mode change
 *
 * Controller mode change test.  The target performs the following operations:
 * - Send START command
 * - Run ETH_TEST_ITER_S_COUNT iterations of:
 *   o Set controller mode to DOWN state
 *   o Set controller mode to ACTIVE state
 *   o Transmit frames (run test 0002)
 *   o Receive frames (run test 0001)
 *
 * \return PASS or FAIL  Test result
 */
boolean EthVirtmacApp_test_0010(void);

/**
 * \brief Measure DUT transmit throughput
 *
 * Measure DUT transmit throughput over ETH_TEST_ITER_L_COUNT frames. The target
 * performs the following operations:
 * - Get the initial timestamp corresponding to the first frame received
 * - Receive frames until the STOP is detected and get the final timestamp
 * - Compute the transmit throughput from the number of received frames and the
 *   time difference between the two captured timestamps
 *
 * \return PASS or FAIL  Test result
 */
boolean EthVirtmacApp_test_0100(void);

/**
 * \brief Test internal loopback
 *
 * Internal loopback test (MAC).
 * - Send and receive ETH_TEST_ITER_M_COUNT frames, for each of them:
 *   o A different payload is set
 *   o EtherType and payload are verified on the received frame
 *
 * \return PASS or FAIL  Test result
 */
boolean EthVirtmacApp_test_0200(void);

/**
 * \brief Test error recovery process
 *
 * Test error recovery process
 * - Receive error notification from server and start to do recovery process
 *
 * \return PASS or FAIL  Test result
 */
boolean EthVirtmacApp_test_1000(void);

#if defined (freertos)
/**
 * \brief Setup SciServer
 *
 * \return PASS or FAIL of SciServer setup
 */
sint32 SetupSciServer(void);
static uint8_t Sciserver_TaskStack[APP_TASK_STACK] __attribute__((aligned(32)));
static uint8_t VirtEthApp_TaskStack[APP_TASK_STACK] __attribute__((aligned(32)));
#endif

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

static TestFunc testFuncs[] =
{
    &EthVirtmacApp_test_0001,
    &EthVirtmacApp_test_0002,
    &EthVirtmacApp_test_0003,
    &EthVirtmacApp_test_0004,
    &EthVirtmacApp_test_0005,
    &EthVirtmacApp_test_0006,
    &EthVirtmacApp_test_0007,
    &EthVirtmacApp_test_0008,
    &EthVirtmacApp_test_0009,
    &EthVirtmacApp_test_0010,
    &EthVirtmacApp_test_0100,
};


/* Eth application state */
EthApp_StateSet gEthApp;
uint8 gCtrlIdx = ETH_CTRL_ID_0;

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */
#if defined (freertos)
int VirtEthmain(void)
#else
int main(void)
#endif
{
    uint32 i,ci;
    boolean status;
    boolean globalStatus = TRUE;
    Std_ReturnType retVal;
#if (STD_OFF == ETH_PRE_COMPILE_VARIANT)
    Eth_CfgPtr = (Eth_ConfigType *)&EthConfigSet_0;
#endif

#if !defined(freertos)
    BoardUtils_platformInit();
#endif

    /* Print date and time */
    EthUtils_printf("---\r\nEth Application build date/time: %s, %s\r\n", __DATE__, __TIME__);

    EthUtils_printf("Eth Application Starts\r\n");


    /* Initialize the global app structure */
    memset(&gEthApp, 0x0u, sizeof(gEthApp));

    for(ci = 0u; ci < ETH_CTRL_ID_MAX; ++ci)
    {
        gEthApp.state[ci].ctrlIdx = ci;
        gEthApp.state[ci].testIdx = ARRAY_SIZE(testFuncs);
    }

    /* System initialization */
    EthApp_Startup();

    retVal = EthUtils_VirtMacinit(gEthApp.state[gCtrlIdx].ctrlIdx);
    if (E_OK != retVal)
    {
        EthUtils_printf("Failed to initialize virtmac: %d\r\n", retVal);
        return retVal;
    }

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
    if (E_OK != retVal)
    {
        EthUtils_printf("Failed to setup driver: %d\r\n", retVal);
        return retVal;
    }

    EthUtils_VirtMacRegisterMacAddr(gEthApp.state[gCtrlIdx].ctrlIdx, gEthApp.state[gCtrlIdx].macAddr);
    EthUtils_VirtMacSubMcastAddr(gEthApp.state[gCtrlIdx].ctrlIdx, TargetMcastAddr);

    /* Main test section */
    if (E_OK == retVal)
    {
        /* DUT <-> Host Initial Handshake */
        (void)EthApp_waitForHostAll();

#if (STD_ON == ETH_CPSW_RECOVERY_TEST)
        globalStatus = EthVirtmacApp_test_1000();
        (void)i;
        (void)status;
        (void)testFuncs;
#else
        for (i = 0U; i < ARRAY_SIZE(testFuncs); i++)
        {
            EthUtils_printf("\n\n-----------------------------------------------------------\n");
            memset(&gEthApp.state[gCtrlIdx].stats, 0U, sizeof(EthApp_Stats));
            status = testFuncs[i]();
            globalStatus &= status;
            EthUtils_printf("Test Result: %s\n", status?"Pass":"Fail");
            EthUtils_printf("-----------------------------------------------------------\n");

            /* Some packet from last test pending, clear it up */
            EthApp_transmitFlush(gEthApp.state[gCtrlIdx].ctrlIdx);
        }
#endif
    }
    EthUtils_printf("\n");

    EthUtils_VirtMacUnregisterMacAddr(gEthApp.state[gCtrlIdx].ctrlIdx, gEthApp.state[gCtrlIdx].macAddr);
    EthUtils_VirtMacUnsubMcastAddr(gEthApp.state[gCtrlIdx].ctrlIdx, TargetMcastAddr);

    /* Deinitialize the Ethernet controller */
    if (E_OK == retVal)
    {
        retVal = EthApp_deinit(gEthApp.state[gCtrlIdx].ctrlIdx);
        if (E_OK != retVal)
        {
            EthUtils_printf("Failed to deinitialize: %d\n", retVal);
        }
    }

    retVal = EthApp_closeDma();
    if (E_OK != retVal)
    {
        EthUtils_printf("Failed to deinitialize DMA: %d\r\n", retVal);
    }

    EthUtils_printf("ETH Stack Usage: %d bytes\n", AppUtils_GetStackUsage());
    if (AppUtils_EthCheckStackAndSectionCorruption() != E_OK)
    {
        retVal = E_NOT_OK;
        EthUtils_printf("ETH Stack/section corruption!!!\n");
    }

    EthUtils_printf("Final Tests Results: %s\n",
                      (globalStatus == TRUE) ? "PASS" : "FAIL");

    if (E_OK == retVal)
    {
        EthUtils_printf("Eth Application Completed\n");
    }
    else
    {
        EthUtils_printf("Eth Application Failed\n");
    }

    return 0;
}

#if defined (freertos)
static void Taskfnx(void* a0, void* a1)
{
     /* Initialize SCI Client Server */
    SetupSciServer();
}

static void mainTask(void* a0, void* a1)
{
    Std_ReturnType retVal;

    retVal = VirtEthmain();

    if (E_OK == retVal)
    {
        EthUtils_printf("Eth VirtApp task Completed\n");
    }
    else
    {
        EthUtils_printf("Eth VirtApp task Failed\n");
    }
}

int main(void)
{
    char *version_str = NULL;
    char *rmpmhal_version_str = NULL;

    TaskP_Handle VirtEthTask;
    TaskP_Params VirtEthTaskParams;

    TaskP_Handle SciserverTask;
    TaskP_Params SciserverTaskParams;

    /* Relocate FreeRTOS Reset Vectors from BTCM*/
    void _freertosresetvectors (void);
    memcpy((void *)0x0, (void *)_freertosresetvectors, 0x40);

    OS_init();

    /* for UART printing.. */
    BoardUtils_platformInit();

    /* Initialize the task params */
	TaskP_Params_init(&SciserverTaskParams);
	SciserverTaskParams.priority       = 6;
	SciserverTaskParams.stack          = Sciserver_TaskStack;
	SciserverTaskParams.stacksize      = sizeof (Sciserver_TaskStack);

	SciserverTask = TaskP_create(&Taskfnx, &SciserverTaskParams);

    if (NULL == SciserverTask)
    {
        OS_stop();
        return(0);
    }

	/* Initialize the task params */
	TaskP_Params_init(&VirtEthTaskParams);
    //VirtEthTaskParams.instance->name = VIRTETH_PROFILE_DEMO_TASK_NAME;
	VirtEthTaskParams.priority       = VIRTETH_TASK_PRIORITY;
	VirtEthTaskParams.stack          = VirtEthApp_TaskStack;
	VirtEthTaskParams.stacksize      = sizeof (VirtEthApp_TaskStack);

	VirtEthTask = TaskP_create(&mainTask, &VirtEthTaskParams);

    if (NULL == VirtEthTask)
    {
        OS_stop();
        return(0);
    }

    version_str = Sciserver_getVersionStr();
    rmpmhal_version_str = Sciserver_getRmPmHalVersionStr();

    EthUtils_printf("Sciserver Testapp Built On: %s %s\n", __DATE__, __TIME__);
    EthUtils_printf("Sciserver Version: %s\n", version_str);
    EthUtils_printf("RM_PM_HAL Version: %s\n", rmpmhal_version_str);

    OS_start();    /* does not return */

    return(0);
}
#endif

boolean EthVirtmacApp_test_0001(void)
{
    uint32 num = 0U;
    boolean status;

    EthUtils_printf("test_0001: START\n");

    /* Send START cmd */
    EthApp_sendCmd(gEthApp.state[gCtrlIdx].ctrlIdx, CTRL_FRAME_CMD_START);

    /* Receive frames until STOP cmd is detected */
    status = EthApp_receive(gEthApp.state[gCtrlIdx].ctrlIdx, num);
    if (FAIL == status)
    {
        EthUtils_printf("test_0001: failed while receiving frames\n");
    }

    EthUtils_printf("test_0001: received %d of %d frames\n",
                    gEthApp.state[gCtrlIdx].stats.rxExp1PktCnt, ETH_TEST_ITER_M_COUNT);

    if (gEthApp.state[gCtrlIdx].stats.rxExp1PktCnt != ETH_TEST_ITER_M_COUNT)
    {
        printf("test_0001: received frame count mismatch (exp=%d, got=%d)\n",
               ETH_TEST_ITER_M_COUNT, gEthApp.state[gCtrlIdx].stats.rxExp1PktCnt);
        status = FAIL;
    }

    EthUtils_printf("test_0001: END\n");

    return status;
}

boolean EthVirtmacApp_test_0002(void)
{
    uint32 iterations = ETH_TEST_ITER_M_COUNT;
    uint16 len = 1500U;
    boolean txConfirmation = TRUE;
    boolean vlan = FALSE;
    boolean status;

    EthUtils_printf("test_0002: START\n");

    /* Send START cmd */
    EthApp_sendCmd(gEthApp.state[gCtrlIdx].ctrlIdx, CTRL_FRAME_CMD_START);

    /* Transmit frames */
    status = EthApp_transmit(gEthApp.state[gCtrlIdx].ctrlIdx,
                             iterations,
                             len,
                             txConfirmation,
                             vlan,
                             ETH_DEFAULT_TX_PRIORITY,
                             &BcastAddr[0U]);

    EthUtils_printf("test_0002: END\n");

    return status;
}

boolean EthVirtmacApp_test_0003(void)
{
    uint32 iterations = ETH_TEST_ITER_M_COUNT;
    sint32 i;
    sint32 fillStatus;
    uint16 len = 1500U;
    boolean status = TRUE;
    EthFrame frame;

    EthUtils_printf("test_0003: START\n");

    /* Initialize frame header */
    memcpy(frame.hdr.dstMac, BcastAddr, ETH_MAC_ADDR_LEN);
    Eth_GetPhysAddr(gEthApp.state[gCtrlIdx].ctrlIdx, gEthApp.state[gCtrlIdx].macAddr);

    memcpy(frame.hdr.srcMac, gEthApp.state[gCtrlIdx].macAddr, ETH_MAC_ADDR_LEN);
    frame.hdr.etherType = ETHERTYPE_EXPERIMENTAL1;

    /* Send START cmd */
    EthApp_sendCmd(gEthApp.state[gCtrlIdx].ctrlIdx, CTRL_FRAME_CMD_START);

    /* Send and receive frames */
    for (i = 0U; i < iterations; i++)
    {
        if (TRUE == gEthApp.state[gCtrlIdx].verbose)
        {
            EthUtils_printf("test_0003: iteration %d\n", i);
        }

        gEthApp.state[gCtrlIdx].saveRxFrame = TRUE;
        gEthApp.state[gCtrlIdx].stats.rxPktCnt = 0U;
        gEthApp.state[gCtrlIdx].stats.rxPktIndCnt = 0U;

        fillStatus = EthUtils_fillPayload((DataFramePayload *)frame.payload,
                                          i % ETH_TEST_NUM_TYPES,
                                          len);
        if (ETH_TEST_PKT_SOK != fillStatus)
        {
            EthUtils_printf("test_0003: failed to fill test packet\n");
            status = FAIL;
            break;
        }

        /* Send the frame */
        EthApp_send(gEthApp.state[gCtrlIdx].ctrlIdx, &frame, len, ETH_DEFAULT_TX_PRIORITY);

        /* Wait for Rx frames */
        while (0 == gEthApp.state[gCtrlIdx].stats.rxPktIndCnt)
        {
            if(Eth_GetRxEnableInterrupt(gCtrlIdx) == FALSE)
            {
                EthApp_receiveAllFifo(gEthApp.state[gCtrlIdx].ctrlIdx);
            }
        }

        /* End the test if STOP cmd is detected */
        if (TRUE == EthFrame_isStopCmd(&gEthApp.state[gCtrlIdx].rxFrame))
        {
            break;
        }

        /* Check if EtherType field matches */
        if (frame.hdr.etherType == gEthApp.state[gCtrlIdx].rxFrame.hdr.etherType)
        {
            /* Check if payload matches */
            if (0 != memcmp(&frame.payload, gEthApp.state[gCtrlIdx].rxFrame.payload, len))
            {
                EthUtils_printf("test_0003: payload mismatch\n");
                status = FAIL;
                break;
            }
        }
    }

    /* Send STOP cmd */
    EthApp_sendCmd(gEthApp.state[gCtrlIdx].ctrlIdx, CTRL_FRAME_CMD_STOP);

    EthUtils_printf("test_0003: END\n");

    return status;
}

boolean EthVirtmacApp_test_0004(void)
{
    uint32 num = 0U;
    boolean status;

    EthUtils_printf("test_0004: START\n");

    /* Send START cmd */
    EthApp_sendCmd(gEthApp.state[gCtrlIdx].ctrlIdx, CTRL_FRAME_CMD_START);

    /* Receive frames until STOP cmd is detected */
    status = EthApp_receive(gEthApp.state[gCtrlIdx].ctrlIdx, num);
    if (FAIL == status)
    {
        EthUtils_printf("test_0004: failed while receiving frames\n");
    }

    EthUtils_printf("test_0004: received %d of %d frames\n",
                    gEthApp.state[gCtrlIdx].stats.rxExp1PktCnt, ETH_TEST_ITER_M_COUNT / 2);

    if (gEthApp.state[gCtrlIdx].stats.rxExp1PktCnt != ETH_TEST_ITER_M_COUNT / 2)
    {
        status = FAIL;
    }

    EthUtils_printf("test_0004: END\n");

    return status;
}

boolean EthVirtmacApp_test_0005(void)
{
    uint32 num = ETH_TEST_ITER_M_COUNT;
    uint32 acceptCnt = num / 2;
    boolean itStatus;
    boolean status = PASS;

    EthUtils_printf("test_0005: START\n");

    /* Part 1: Receive frames with controller's address, broadcast address */
    gEthApp.state[gCtrlIdx].stats.rxExp1PktCnt = 0U;
    gEthApp.state[gCtrlIdx].stats.rxExp2PktCnt = 0U;
    EthApp_sendCmd(gEthApp.state[gCtrlIdx].ctrlIdx, CTRL_FRAME_CMD_START);

    itStatus = EthApp_receive(gEthApp.state[gCtrlIdx].ctrlIdx, 0U);
    if ((FAIL == itStatus) ||
        (gEthApp.state[gCtrlIdx].stats.rxExp1PktCnt != acceptCnt) ||
        (gEthApp.state[gCtrlIdx].stats.rxExp2PktCnt != 0U))
    {
        gEthApp.state[gCtrlIdx].stats.filterNullRxErr++;
    }

    EthUtils_printf("test_0005:  Null: Exp1: exp %3d, got %3d\n",
                    acceptCnt, gEthApp.state[gCtrlIdx].stats.rxExp1PktCnt);
    EthUtils_printf("test_0005:  Null: Exp2: exp %3d, got %3d\n",
                    0U, gEthApp.state[gCtrlIdx].stats.rxExp2PktCnt);

    /* Part 2: Add unicast address and receive frames with that address */
    EthUtils_VirtMacRegisterMacAddr(gEthApp.state[gCtrlIdx].ctrlIdx, UcastAddr);

    gEthApp.state[gCtrlIdx].stats.rxExp1PktCnt = 0U;
    gEthApp.state[gCtrlIdx].stats.rxExp2PktCnt = 0U;
    EthApp_sendCmd(gEthApp.state[gCtrlIdx].ctrlIdx, CTRL_FRAME_CMD_START);

    itStatus = EthApp_receive(gEthApp.state[gCtrlIdx].ctrlIdx, 0U);
    if ((FAIL == itStatus) ||
        (gEthApp.state[gCtrlIdx].stats.rxExp1PktCnt != acceptCnt) ||
        (gEthApp.state[gCtrlIdx].stats.rxExp2PktCnt != acceptCnt))
    {
        gEthApp.state[gCtrlIdx].stats.filterUcastRxErr++;
    }

    EthUtils_printf("test_0005: Ucast: Exp1: exp %3d, got %3d\n",
                    acceptCnt, gEthApp.state[gCtrlIdx].stats.rxExp1PktCnt);
    EthUtils_printf("test_0005: Ucast: Exp2: exp %3d, got %3d\n",
                    acceptCnt, gEthApp.state[gCtrlIdx].stats.rxExp2PktCnt);

    /* Part 3: Add multicast address and receive frames with that address */
    EthUtils_VirtMacSubMcastAddr(gEthApp.state[gCtrlIdx].ctrlIdx, McastAddr);

    gEthApp.state[gCtrlIdx].stats.rxExp1PktCnt = 0U;
    gEthApp.state[gCtrlIdx].stats.rxExp2PktCnt = 0U;
    EthApp_sendCmd(gEthApp.state[gCtrlIdx].ctrlIdx, CTRL_FRAME_CMD_START);

    itStatus = EthApp_receive(gEthApp.state[gCtrlIdx].ctrlIdx, 0U);
    if ((FAIL == itStatus) ||
        (gEthApp.state[gCtrlIdx].stats.rxExp1PktCnt != acceptCnt) ||
        (gEthApp.state[gCtrlIdx].stats.rxExp2PktCnt != acceptCnt))
    {
        gEthApp.state[gCtrlIdx].stats.filterMcastRxErr++;
    }

    EthUtils_printf("test_0005: Mcast: Exp1: exp %3d, got %3d\n",
                    acceptCnt, gEthApp.state[gCtrlIdx].stats.rxExp1PktCnt);
    EthUtils_printf("test_0005: Mcast: Exp2: exp %3d, got %3d\n",
                    acceptCnt, gEthApp.state[gCtrlIdx].stats.rxExp2PktCnt);

    /* Part 4: Remove multicast address and receive frames with that address */
    EthUtils_VirtMacUnsubMcastAddr(gEthApp.state[gCtrlIdx].ctrlIdx, McastAddr);

    gEthApp.state[gCtrlIdx].stats.rxExp1PktCnt = 0U;
    gEthApp.state[gCtrlIdx].stats.rxExp2PktCnt = 0U;
    EthApp_sendCmd(gEthApp.state[gCtrlIdx].ctrlIdx, CTRL_FRAME_CMD_START);

    itStatus = EthApp_receive(gEthApp.state[gCtrlIdx].ctrlIdx, 0U);
    if ((FAIL == itStatus) ||
        (gEthApp.state[gCtrlIdx].stats.rxExp1PktCnt != acceptCnt) ||
        (gEthApp.state[gCtrlIdx].stats.rxExp2PktCnt != 0U))
    {
        gEthApp.state[gCtrlIdx].stats.filterMcastRxErr++;
    }

    EthUtils_printf("test_0005: Mcast: Exp1: exp %3d, got %3d\n",
                    acceptCnt, gEthApp.state[gCtrlIdx].stats.rxExp1PktCnt);
    EthUtils_printf("test_0005: Mcast: Exp2: exp %3d, got %3d\n",
                    0U, gEthApp.state[gCtrlIdx].stats.rxExp2PktCnt);

    /* Part 5: Add broadcast address and receive/send frames with any address */
    EthUtils_VirtMacRegisterMacAddr(gEthApp.state[gCtrlIdx].ctrlIdx, UcastOpenFilterAddr);

    gEthApp.state[gCtrlIdx].stats.rxExp1PktCnt = 0U;
    gEthApp.state[gCtrlIdx].stats.rxExp2PktCnt = 0U;
    EthApp_sendCmd(gEthApp.state[gCtrlIdx].ctrlIdx, CTRL_FRAME_CMD_START);

    itStatus = EthApp_receive(gEthApp.state[gCtrlIdx].ctrlIdx, 0U);
    if ((FAIL == itStatus) ||
        (gEthApp.state[gCtrlIdx].stats.rxExp1PktCnt != acceptCnt) ||
        (gEthApp.state[gCtrlIdx].stats.rxExp2PktCnt != acceptCnt))
    {
        gEthApp.state[gCtrlIdx].stats.filterBcastRxErr++;
    }

    EthUtils_printf("test_0005: Bcast: Exp1: exp %3d, got %3d\n",
                    acceptCnt, gEthApp.state[gCtrlIdx].stats.rxExp1PktCnt);
    EthUtils_printf("test_0005: Bcast: Exp2: exp %3d, got %3d\n",
                    acceptCnt, gEthApp.state[gCtrlIdx].stats.rxExp2PktCnt);

    /* Part 6: Reset filter and receive frames with controller's address */
    EthUtils_VirtMacUnregisterMacAddr(gEthApp.state[gCtrlIdx].ctrlIdx, UcastAddr);
    EthUtils_VirtMacUnregisterMacAddr(gEthApp.state[gCtrlIdx].ctrlIdx, UcastOpenFilterAddr);

    gEthApp.state[gCtrlIdx].stats.rxExp1PktCnt = 0U;
    gEthApp.state[gCtrlIdx].stats.rxExp2PktCnt = 0U;
    EthApp_sendCmd(gEthApp.state[gCtrlIdx].ctrlIdx, CTRL_FRAME_CMD_START);

    itStatus = EthApp_receive(gEthApp.state[gCtrlIdx].ctrlIdx, 0U);
    if ((FAIL == itStatus) ||
        (gEthApp.state[gCtrlIdx].stats.rxExp1PktCnt != acceptCnt) ||
        (gEthApp.state[gCtrlIdx].stats.rxExp2PktCnt != 0U))
    {
        gEthApp.state[gCtrlIdx].stats.filterNullRxErr++;
    }

    EthUtils_printf("test_0005:  Null: Exp1: exp %3d, got %3d\n",
                    acceptCnt, gEthApp.state[gCtrlIdx].stats.rxExp1PktCnt);
    EthUtils_printf("test_0005:  Null: Exp2: exp %3d, got %3d\n",
                    0U, gEthApp.state[gCtrlIdx].stats.rxExp2PktCnt);

    /* Check error conditions */
    if ((gEthApp.state[gCtrlIdx].stats.filterUcastAddErr > 0U) ||
        (gEthApp.state[gCtrlIdx].stats.filterUcastRemErr > 0U) ||
        (gEthApp.state[gCtrlIdx].stats.filterUcastRxErr > 0U) ||
        (gEthApp.state[gCtrlIdx].stats.filterMcastAddErr > 0U) ||
        (gEthApp.state[gCtrlIdx].stats.filterMcastRemErr > 0U) ||
        (gEthApp.state[gCtrlIdx].stats.filterMcastRxErr > 0U) ||
        (gEthApp.state[gCtrlIdx].stats.filterBcastErr > 0U) ||
        (gEthApp.state[gCtrlIdx].stats.filterBcastRxErr > 0U) ||
        (gEthApp.state[gCtrlIdx].stats.filterNullErr > 0U) ||
        (gEthApp.state[gCtrlIdx].stats.filterNullRxErr > 0U) ||
        (gEthApp.state[gCtrlIdx].stats.detErr > 0U) ||
        (gEthApp.state[gCtrlIdx].stats.demErr > 0U))
    {
        status = FAIL;
    }

    if ((FAIL == status) || (TRUE == gEthApp.state[gCtrlIdx].verbose))
    {
        EthUtils_printf("Filter Test Results:\n");
        EthUtils_printf("  Status:                %s\n", status?"Pass":"Fail");
        EthUtils_printf("  Sent from host:        %d\n", num);
        EthUtils_printf("  Unicast ADD errors:    %d\n", gEthApp.state[gCtrlIdx].stats.filterUcastAddErr);
        EthUtils_printf("  Ucast REMOVE errors:   %d\n", gEthApp.state[gCtrlIdx].stats.filterUcastRemErr);
        EthUtils_printf("  Ucast receive errors:  %d\n", gEthApp.state[gCtrlIdx].stats.filterUcastRxErr);
        EthUtils_printf("  Mcast ADD errors:      %d\n", gEthApp.state[gCtrlIdx].stats.filterMcastAddErr);
        EthUtils_printf("  Mcast REMOVE errors:   %d\n", gEthApp.state[gCtrlIdx].stats.filterMcastRemErr);
        EthUtils_printf("  Mcast receive errors:  %d\n", gEthApp.state[gCtrlIdx].stats.filterMcastRxErr);
        EthUtils_printf("  Bcast ADD errors:      %d\n", gEthApp.state[gCtrlIdx].stats.filterBcastErr);
        EthUtils_printf("  Bcast receive errors:  %d\n", gEthApp.state[gCtrlIdx].stats.filterBcastRxErr);
        EthUtils_printf("  Null ADD errors:       %d\n", gEthApp.state[gCtrlIdx].stats.filterNullErr);
        EthUtils_printf("  Null receive errors:   %d\n", gEthApp.state[gCtrlIdx].stats.filterNullRxErr);
        EthUtils_printf("  DEM errors:            %d\n", gEthApp.state[gCtrlIdx].stats.demErr);
        EthUtils_printf("  DET errors:            %d\n", gEthApp.state[gCtrlIdx].stats.detErr);
        EthUtils_printf("\n");
    }

    EthUtils_printf("test_0005: END\n");

    return status;
}

boolean EthVirtmacApp_test_0006(void)
{
    uint32 iterations = ETH_TEST_ITER_M_COUNT;
    uint16 len = 1500U;
    boolean txConfirmation = TRUE;
    boolean vlan = FALSE;
    boolean status;

    EthUtils_printf("test_0006: START\n");

    /* Send START cmd */
    EthApp_sendCmd(gEthApp.state[gCtrlIdx].ctrlIdx, CTRL_FRAME_CMD_START);

    /* Transmit frames with txConfirmation enabled */
    status = EthApp_transmit(gEthApp.state[gCtrlIdx].ctrlIdx,
                             iterations,
                             len,
                             txConfirmation,
                             vlan,
                             ETH_DEFAULT_TX_PRIORITY,
                             &BcastAddr[0U]);

    EthUtils_printf("test_0006: END\n");

    return status;
}

boolean EthVirtmacApp_test_0007(void)
{
    uint32 iterations = ETH_TEST_ITER_M_COUNT;
    uint16 len = 1500U;
    boolean txConfirmation = TRUE;
    boolean vlan = TRUE;
    boolean status;

    EthUtils_printf("test_0007: START\n");

    EthUtils_VirtMacJoinVlan(gEthApp.state[gCtrlIdx].ctrlIdx, ETH_TEST_VLAN_VID);
    /* Send START cmd */
    EthApp_sendCmd(gEthApp.state[gCtrlIdx].ctrlIdx, CTRL_FRAME_CMD_START);

    /* Transmit VLAN tagged frames with txConfirmation enabled */
    status = EthApp_transmit(gEthApp.state[gCtrlIdx].ctrlIdx,
                             iterations,
                             len,
                             txConfirmation,
                             vlan,
                             ETH_DEFAULT_TX_PRIORITY,
                             &BcastAddr[0U]);

    EthUtils_VirtMacLeaveVlan(gEthApp.state[gCtrlIdx].ctrlIdx, ETH_TEST_VLAN_VID);
    EthUtils_printf("test_0007: END\n");

    return status;
}

boolean EthVirtmacApp_test_0008(void)
{
    uint32 iterations = ETH_TEST_ITER_S_COUNT;
    uint16 len;
    boolean txConfirmation = TRUE;
    boolean vlan = FALSE;
    boolean st;
    boolean status = PASS;

    EthUtils_printf("test_0008: START\n");

    /* Send START cmd */
    EthApp_sendCmd(gEthApp.state[gCtrlIdx].ctrlIdx, CTRL_FRAME_CMD_START);

    /* Transmit frames of different lengths */
    for (len = 10U; len <= 1500U; len += 10U)
    {
        st = EthApp_transmit(gEthApp.state[gCtrlIdx].ctrlIdx,
                             iterations,
                             len,
                             txConfirmation,
                             vlan,
                             ETH_DEFAULT_TX_PRIORITY,
                             &BcastAddr[0U]);
        if (st == FAIL)
        {
            EthUtils_printf("test_0008: failed to transmit frames of length %u\n",
                            len);
            status = FAIL;
        }
    }

    EthUtils_printf("test_0008: END\n");

    return status;
}

boolean EthVirtmacApp_test_0009(void)
{
    uint32 num = 0U;
    boolean status;

    EthUtils_printf("test_0009: START\n");

    EthUtils_VirtMacJoinVlan(gEthApp.state[gCtrlIdx].ctrlIdx, ETH_TEST_VLAN_VID);
    /* Send START cmd */
    EthApp_sendCmd(gEthApp.state[gCtrlIdx].ctrlIdx, CTRL_FRAME_CMD_START);

    /* Receive frames until STOP cmd is detected */
    status = EthApp_receive(gEthApp.state[gCtrlIdx].ctrlIdx, num);
    if (FAIL == status)
    {
        EthUtils_printf("test_0009: failed while receiving frames\n");
    }

    EthUtils_printf("test_0009: received %d of %d frames\n",
                    gEthApp.state[gCtrlIdx].stats.rxVlanPktCnt, ETH_TEST_ITER_M_COUNT);

    if (gEthApp.state[gCtrlIdx].stats.rxVlanPktCnt != ETH_TEST_ITER_M_COUNT)
    {
        printf("test_0009: received frame count mismatch (exp=%d, got=%d)\n",
               ETH_TEST_ITER_M_COUNT, gEthApp.state[gCtrlIdx].stats.rxVlanPktCnt);
        status = FAIL;
    }

    EthUtils_VirtMacLeaveVlan(gEthApp.state[gCtrlIdx].ctrlIdx, ETH_TEST_VLAN_VID);
    EthUtils_printf("test_0009: END\n");

    return status;
}

boolean EthVirtmacApp_test_0010(void)
{
    uint32 iterations = ETH_TEST_ITER_S_COUNT;
    uint32 i;
    boolean status = PASS;
    Std_ReturnType retVal = E_OK;

    EthUtils_printf("test_0010: START\n");

    /* Send START cmd */
    EthApp_sendCmd(gEthApp.state[gCtrlIdx].ctrlIdx, CTRL_FRAME_CMD_START);

    for (i = 0U; i < iterations; i++)
    {
        if (TRUE == gEthApp.state[gCtrlIdx].verbose)
        {
            EthUtils_printf("test_0010: iteration %d\n", i+1);
        }

        if (E_OK == retVal)
        {
            uint8 macAddress[ETH_MAC_ADDR_LEN];

            Eth_GetPhysAddr(gEthApp.state[gCtrlIdx].ctrlIdx, macAddress);
            EthUtils_VirtMacUnregisterMacAddr(gEthApp.state[gCtrlIdx].ctrlIdx, macAddress);
            EthUtils_VirtMacUnsubMcastAddr(gEthApp.state[gCtrlIdx].ctrlIdx, TargetMcastAddr);
        }

        /* Set controller to down state */
        retVal = Eth_SetControllerMode(gEthApp.state[gCtrlIdx].ctrlIdx, ETH_MODE_DOWN);
        if (E_OK != retVal)
        {
            EthUtils_printf("test_0010: failed to set down mode\n");
            gEthApp.state[gCtrlIdx].stats.ctrlModeErr++;
        }
        else
        {
            gEthApp.state[gCtrlIdx].stats.ctrlModeActCnt++;
        }

        Eth_MainFunction();

        /* Set controller back to active state */
        retVal = Eth_SetControllerMode(gEthApp.state[gCtrlIdx].ctrlIdx, ETH_MODE_ACTIVE);
        if (E_OK != retVal)
        {
            EthUtils_printf("test_0010: failed to set active mode\n");
            gEthApp.state[gCtrlIdx].stats.ctrlModeErr++;
        }
        else
        {
            gEthApp.state[gCtrlIdx].stats.ctrlModeDownCnt++;
        }

        Eth_MainFunction();

        /* Check error conditions */
        if ((gEthApp.state[gCtrlIdx].stats.ctrlModeDownCnt == 0U) ||
            (gEthApp.state[gCtrlIdx].stats.ctrlModeActCnt == 0U) ||
            (gEthApp.state[gCtrlIdx].stats.ctrlModeErr > 0U) ||
            (gEthApp.state[gCtrlIdx].stats.ctrlModeIndDownCnt != gEthApp.state[gCtrlIdx].stats.ctrlModeDownCnt) ||
            (gEthApp.state[gCtrlIdx].stats.ctrlModeIndActCnt != gEthApp.state[gCtrlIdx].stats.ctrlModeActCnt) ||
            (gEthApp.state[gCtrlIdx].stats.ctrlModeIndErr > 0U) ||
            (gEthApp.state[gCtrlIdx].stats.detErr > 0U) ||
            (gEthApp.state[gCtrlIdx].stats.demErr > 0U))
        {
            status = FAIL;
        }

        if ((FAIL == status) || (TRUE == gEthApp.state[gCtrlIdx].verbose))
        {
            EthUtils_printf("Controller Mode Test Results:\n");
            EthUtils_printf("  Status:                   %s\n", status?"Pass":"Fail");
            EthUtils_printf("  Requested:                %d\n", iterations);
            EthUtils_printf("  Changes to DOWN mode:     %d\n", gEthApp.state[gCtrlIdx].stats.ctrlModeDownCnt);
            EthUtils_printf("  Changes to ACTIVE mode:   %d\n", gEthApp.state[gCtrlIdx].stats.ctrlModeActCnt);
            EthUtils_printf("  Mode change errors:       %d\n", gEthApp.state[gCtrlIdx].stats.ctrlModeErr);
            EthUtils_printf("  Indicated DOWN changes:   %d\n", gEthApp.state[gCtrlIdx].stats.ctrlModeIndDownCnt);
            EthUtils_printf("  Indicated ACTIVE changes: %d\n", gEthApp.state[gCtrlIdx].stats.ctrlModeIndActCnt);
            EthUtils_printf("  Indication errors:        %d\n", gEthApp.state[gCtrlIdx].stats.ctrlModeIndErr);
            EthUtils_printf("\n");
        }

        if (FAIL == status)
        {
            EthUtils_printf("test_0010: failed to change controller mode\n");
        }

        if (E_OK == retVal)
        {
            uint8 macAddress[ETH_MAC_ADDR_LEN];

            Eth_GetPhysAddr(gEthApp.state[gCtrlIdx].ctrlIdx, macAddress);
            EthUtils_VirtMacRegisterMacAddr(gEthApp.state[gCtrlIdx].ctrlIdx, gEthApp.state[gCtrlIdx].macAddr);
            EthUtils_VirtMacSubMcastAddr(gEthApp.state[gCtrlIdx].ctrlIdx, TargetMcastAddr);
        }

        /* Transmit frames */
        status = EthVirtmacApp_test_0002();
        if (FAIL == status)
        {
            EthUtils_printf("test_0010: failed to transmit frames\n");
        }

        /* Receive frames */
        status = EthVirtmacApp_test_0001();
        if (FAIL == status)
        {
            EthUtils_printf("test_0010: failed to receive frames\n");
        }
    }

    EthUtils_printf("test_0010: completed %d of %d iterations\n", i, iterations);
    EthUtils_printf("test_0010: END\n");

    return status;
}

boolean EthVirtmacApp_test_0100(void)
{
    Eth_BufIdxType bufIdx;
    BufReq_ReturnType bufStatus;
    boolean txConfirmation = FALSE;
    uint8 *bufPtr;
    uint32 iterations = ETH_TEST_ITER_L_COUNT;
    uint32 retryCnt;
    uint32 i;
    uint16 len = 1500U;
    Std_ReturnType retVal;
    /* QoS is not supported in current MCAL Eth, so set to 0 */
    uint8 priority = 0U;

    EthUtils_printf("test_0100: START\n");

    /* Send START cmd */
    EthApp_sendCmd(gEthApp.state[gCtrlIdx].ctrlIdx, CTRL_FRAME_CMD_START);

    /* Send frames */
    for (i = 0U; i < iterations; i++)
    {
        /* Request one buffer */
        bufStatus = EthApp_provideTxBuffer(gEthApp.state[gCtrlIdx].ctrlIdx,
                                                  priority,
                                                  &bufIdx,
                                                  &bufPtr,
                                                  &len);

        /* Transmit it as is (only header is updated) */
        if (BUFREQ_OK == bufStatus)
        {
            retryCnt = txConfirmation ? 0U : ETHAPP_TRANSMIT_RETRY_COUNT;

            do
            {
                retVal = Eth_Transmit(gEthApp.state[gCtrlIdx].ctrlIdx,
                                      bufIdx,
                                      (Eth_FrameType)ETHERTYPE_EXPERIMENTAL1,
                                      txConfirmation,
                                      len,
                                      BcastAddr);
                if (E_OK == retVal)
                {
                    break;
                }

                if (retryCnt > 0U)
                {
                    EthApp_delay(1U);
                    retryCnt--;
                }
                else
                {
                    break;
                }
            }
            while (TRUE);

            if (E_OK != retVal)
            {
                EthUtils_printf("test_0100: failed to send buffer\n");
            }
        }
        else
        {
            EthUtils_printf("test_0100: failed to get TX buffer: %d\n",
                            (int)bufStatus);
        }
    }

    /* Send STOP cmd */
    EthApp_sendCmd(gEthApp.state[gCtrlIdx].ctrlIdx, CTRL_FRAME_CMD_STOP);

    EthUtils_printf("test_0100: %d frames sent\n", i);
    EthUtils_printf("test_0100: END\n");

    return PASS;
}

boolean EthVirtmacApp_test_0200(void)
{
    uint32 iterations = ETH_TEST_ITER_M_COUNT;
    uint32 i;
    sint32 fillStatus;
    uint16 len = 1500U;
    boolean status = TRUE;
    EthFrame frame;

    EthUtils_printf("test_0200: START\n");

    /* Initialize frame header */
    memcpy(frame.hdr.dstMac, BcastAddr, ETH_MAC_ADDR_LEN);
    Eth_GetPhysAddr(gEthApp.state[gCtrlIdx].ctrlIdx, gEthApp.state[gCtrlIdx].macAddr);
    memcpy(frame.hdr.srcMac, gEthApp.state[gCtrlIdx].macAddr, ETH_MAC_ADDR_LEN);
    frame.hdr.etherType = ETHERTYPE_EXPERIMENTAL1;

    /* Send and receive frames */
    for (i = 0U; i < iterations; i++)
    {
        if (TRUE == gEthApp.state[gCtrlIdx].verbose)
        {
            EthUtils_printf("test_0200: iteration %d\n", i);
        }

        gEthApp.state[gCtrlIdx].saveRxFrame = TRUE;
        gEthApp.state[gCtrlIdx].stats.rxPktCnt = 0U;
        gEthApp.state[gCtrlIdx].stats.rxPktIndCnt = 0U;

        fillStatus = EthUtils_fillPayload((DataFramePayload *)frame.payload,
                                          i % ETH_TEST_NUM_TYPES,
                                          len);
        if (ETH_TEST_PKT_SOK != fillStatus)
        {
            EthUtils_printf("test_0200: failed to fill test packet\n");
            status = FAIL;
            break;
        }

        /* Send the frame */
        EthApp_send(gEthApp.state[gCtrlIdx].ctrlIdx, &frame, len, ETH_DEFAULT_TX_PRIORITY);

        /* Wait for Rx frames */
        while (0 == gEthApp.state[gCtrlIdx].stats.rxPktIndCnt)
        {
            if(Eth_GetRxEnableInterrupt(gCtrlIdx) == FALSE)
            {
                EthApp_receiveAllFifo(gEthApp.state[gCtrlIdx].ctrlIdx);
            }
        }

        /* Check if EtherType field matches */
        if (frame.hdr.etherType != gEthApp.state[gCtrlIdx].rxFrame.hdr.etherType)
        {
            EthUtils_printf("test_0200: EtherType mismatch (exp=0x%04x, got=0x%04x)\n",
                            frame.hdr.etherType & 0xFFFFU,
                            gEthApp.state[gCtrlIdx].rxFrame.hdr.etherType & 0xFFFFU);
            status = FAIL;
            break;
        }

        /* Check if payload matches */
        if (0 != memcmp(&frame.payload, gEthApp.state[gCtrlIdx].rxFrame.payload, len))
        {
            EthUtils_printf("test_0200: payload mismatch\n");
            status = FAIL;
            break;
        }
    }

    EthUtils_printf("test_0200: completed %d of %d iterations\n", i, iterations);
    EthUtils_printf("test_0200: END\n");

    return status;
}

boolean EthVirtmacApp_test_1000(void)
{
    uint32 num_interation = 50u;
    uint32 idx;
    boolean status = TRUE;
    uint32 cnt = 1u;
    Eth_ModeType lastCtrMode;
    Eth_ModeType currCtrMode;

    for(idx = 0u; idx < num_interation; ++idx)
    {
        status &= EthVirtmacApp_test_0001();
        status &= EthVirtmacApp_test_0002();

        /* Unregister default mac */
        EthUtils_VirtMacUnregisterMacAddr(gEthApp.state[gCtrlIdx].ctrlIdx, gEthApp.state[gCtrlIdx].macAddr);
        EthUtils_VirtMacUnsubMcastAddr(gEthApp.state[gCtrlIdx].ctrlIdx, TargetMcastAddr);

        EthUtils_printf("Start ERROR process\n");

        lastCtrMode = ETH_MODE_ACTIVE;
        currCtrMode = ETH_MODE_ACTIVE;
        cnt = 1u;
        while(TRUE)
        {
            Eth_VirtMac_MainFunction(gEthApp.state[gCtrlIdx].ctrlIdx);
            Eth_GetControllerMode(gEthApp.state[gCtrlIdx].ctrlIdx, &currCtrMode);

            if(lastCtrMode == ETH_MODE_DOWN && currCtrMode == ETH_MODE_ACTIVE)
            {
                /* This means recovery process was done (controller state move from DOWN -> UP */
                EthUtils_printf("Recorvery done\n");
                break;
            }

            if(lastCtrMode != currCtrMode)
            {
                lastCtrMode = currCtrMode;
            }

            if((cnt++ % 10) == 0)
            {
                EthUtils_printf("Recorvery in progress\n");
                cnt = 1u;
            }
            EthApp_delay(100u);
        }

        /* Reregister default MAC address */
        EthUtils_VirtMacRegisterMacAddr(gEthApp.state[gCtrlIdx].ctrlIdx, gEthApp.state[gCtrlIdx].macAddr);
        EthUtils_VirtMacSubMcastAddr(gEthApp.state[gCtrlIdx].ctrlIdx, TargetMcastAddr);

        EthApp_delay(4000u);
    }

    return status;
}


#if defined (freertos)
sint32 SetupSciServer(void)
{
	sint32 ret = CSL_PASS;
    Sciserver_TirtosCfgPrms_t appPrms;
    Sciclient_ConfigPrms_t clientPrms;

    appPrms.taskPriority[SCISERVER_TASK_USER_LO] = 4;
    appPrms.taskPriority[SCISERVER_TASK_USER_HI] = 5;

    /* Sciclient needs to be initialized before Sciserver. Sciserver depends on
     * Sciclient API to execute message forwarding */
    ret = Sciclient_configPrmsInit(&clientPrms);

	if (ret == CSL_PASS)
    {
        ret = Sciclient_boardCfgParseHeader(
            (uint8_t *) SCISERVER_COMMON_X509_HEADER_ADDR,
            &clientPrms.inPmPrms, &clientPrms.inRmPrms);
    }

    if (ret == CSL_PASS)
    {
        ret = Sciclient_init(&clientPrms);
    }

    if (ret == CSL_PASS)
    {
        ret = Sciserver_tirtosInit(&appPrms);
    }

    if (ret == CSL_PASS)
    {
        EthUtils_printf("Starting Sciserver..... PASSED\n");
    }
    else
    {
        EthUtils_printf("Starting Sciserver..... FAILED\n");
    }

	return ret;
}
#endif
