/*
*
* Copyright (c) 2024 Texas Instruments Incorporated
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

/* AutoSAR specific Header files */
#include "Eth.h"
#include "Eth_Irq.h"
#include "EthTrcv.h"

#include "Det.h"
#include "Dem.h"
#include "Os.h"

#include "EthUtils.h"
#include "EthUtils_Target.h"
#include "board_utils.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

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

/* Test cases */

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
boolean EthApp_test_0001(void);

/**
 * \brief Test basic frame transmission
 *
 * Basic DUT frame transmission test.  The target side performs these operations:
 * - Send START command
 * - Transmit ETH_TEST_ITER_M_COUNT non-VLAN tagged frames without confirmation
 *
 * \return PASS or FAIL  Test result
 */
boolean EthApp_test_0002(void);

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
boolean EthApp_test_0003(void);

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
boolean EthApp_test_0004(void);

#if (STD_ON == ETH_UPDATE_PHYS_ADDR_FILTER_API)
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
 * - Part 6
 *   o The filter is reset back to accept only DUT's unicast address
 *   o Receive frames until STOP is detected
 *     - Frame 1: Source MAC address is DUT's. It should be accepted
 *     - Frame 2: Source MAC address is not DUT's. It should be rejected
 *     - Expected to receive ETH_TEST_ITER_M_COUNT / 2 frames
 *
 * \return PASS or FAIL  Test result
 */
boolean EthApp_test_0005(void);

/**
 * \brief Test broadcast filter configurations(bypass mode)
 *  This test should be the last data path test case, since bypass mode is applied
 *  for all controller
 *
 * Test the different modes of the DUT's filter configuration.  The target side performs
 * the operations:
 *   o The filter is opened up (promiscuous mode)
 *   o Receive frames until STOP is detected
 *     - Frame 1: Source MAC address is DUT's. It should be accepted
 *     - Frame 2: Source MAC address is not DUT's. It should be accepted
 *     - Expected to receive ETH_TEST_ITER_M_COUNT frames
 * \return PASS or FAIL  Test result
 */
boolean EthApp_test_0005_1(void);

#endif /* (STD_ON == ETH_UPDATE_PHYS_ADDR_FILTER_API) */

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
boolean EthApp_test_0006(void);

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
boolean EthApp_test_0007(void);

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
boolean EthApp_test_0008(void);

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
boolean EthApp_test_0009(void);

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
boolean EthApp_test_0010(void);

/**
 * \brief Test multicast frame transmission
 *
 * Multicast frame transmission test.  The target side performs these operations:
 * - Send START command
 * - Transmit ETH_TEST_ITER_M_COUNT non-VLAN tagged multicast frames without
 *   confirmation
 *
 * \return PASS or FAIL  Test result
 */
boolean EthApp_test_0011(void);

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

/**
 * \brief Measure DUT transmit throughput
 *
 * Measure DUT transmit throughput for multiple frame size. The target
 * performs the following operations:
 * - with each frame size and frame number call EthApp_test_0010
 *   to calculate throughput for that frame szie
 *
 * \return PASS or FAIL  Test result
 */
boolean EthApp_test_0110(void);

/**
 * \brief Measure DUT receive throughput
 *
 * Measure DUT receive throughput.
 * . The target performs the following operations:
 * - Get the initial timestamp corresponding to the first frame received
 * - Receive frames until the STOP is detected and get the final timestamp
 * - Compute the receive throughput from the number of received frames and the
 *   time difference between the two captured timestamps
 *
 * \return PASS or FAIL  Test result
 */
boolean EthApp_test_0101(void);

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
boolean EthApp_test_0200(void);

/* Egress timestamp test */
boolean EthApp_test_0300(void);

/* Ingress timestamp test */
boolean EthApp_test_0301(void);

/* Get current time test */
boolean EthApp_test_0302(void);

#if (STD_ON == ETH_TRAFFIC_SHAPING_API)
/* Traffic shaping test */
boolean EthApp_test_0400(void);
#endif /* (STD_ON == ETH_TRAFFIC_SHAPING_API) */

#if (STD_ON == ETHTRCV_GETLINKSTATE_API)
/**
 * \brief Test MDIO Operation
 *
 * Stress testing of MDIO Read Operation
 *
 * \return PASS or FAIL  Test result
 */
boolean EthApp_test_0500(void);
#endif /* (STD_ON == ETHTRCV_GETLINKSTATE_API) */

/* Tx QOS test */
boolean EthApp_test_0610(void);

/* Rx QOS test */
boolean EthApp_test_0611(void);

#if (STD_ON == ETH_REGISTER_READBACK_API)
/**
 * \brief Test readback Operation
 *
 * \return PASS or FAIL  Test result
 */
boolean EthApp_test_0700(void);
#endif /* (STD_ON == ETH_REGISTER_READBACK_API) */



/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

#define  ETH_START_SEC_VAR_NO_INIT_UNSPECIFIED_128
#include "Eth_MemMap.h"

extern VAR(uint8, ETH_VAR_NO_INIT_128) EthApp_ExtTxBuffer[1536U];

#define  ETH_STOP_SEC_VAR_NO_INIT_UNSPECIFIED_128
#include "Eth_MemMap.h"

static TestFunc testFuncs[] =
{
    &EthApp_test_0001,
    &EthApp_test_0002,
    &EthApp_test_0003,
    &EthApp_test_0004,
#if (STD_ON == ETH_UPDATE_PHYS_ADDR_FILTER_API)
    &EthApp_test_0005,
#endif
    &EthApp_test_0006,
    &EthApp_test_0007,
    &EthApp_test_0008,
    &EthApp_test_0009,
    &EthApp_test_0010,
    &EthApp_test_0011,
    &EthApp_test_0300,
    &EthApp_test_0301,
    &EthApp_test_0302,
    &EthApp_test_0110,
    &EthApp_test_0101,
#if (STD_ON == ETH_TRAFFIC_SHAPING_API)
    &EthApp_test_0400,
#endif
    &EthApp_test_0610,
    &EthApp_test_0611,
#if (STD_ON == ETH_UPDATE_PHYS_ADDR_FILTER_API)
    &EthApp_test_0005_1,
#endif
#if (STD_ON == ETHTRCV_GETLINKSTATE_API)
    &EthApp_test_0500,
#endif
#if (STD_ON == ETH_REGISTER_READBACK_API)
    &EthApp_test_0700,
#endif
};

/* Eth application state */
EthApp_StateSet gEthApp;
uint8 gCtrlIdx = ETH_CTRL_ID_0;

/* ========================================================================== */
/*                           External Variables                               */
/* ========================================================================== */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */
int EthApp_Main(void)
{
    uint32 i,ti,ci;
    boolean status;
    boolean globalStatus = TRUE;
    Std_ReturnType retVal;
#if (STD_OFF == ETH_PRE_COMPILE_VARIANT)
    Eth_CfgPtr = (const Eth_ConfigType *)&EthConfigSet_0;
#endif

    /* Initialize the global app structure */
    memset(&gEthApp, 0x0u, sizeof(gEthApp));

    for(ci = 0u; ci < ETH_CTRL_ID_MAX; ++ci)
    {
        gEthApp.state[ci].ctrlIdx = ci;
        gEthApp.state[ci].testIdx = ARRAY_SIZE(testFuncs);
    }

    /* Print date and time */
    EthUtils_printf("---\r\nEth Application build date/time: %s, %s\r\n", __DATE__, __TIME__);

    EthUtils_printf("Eth Application Starts\r\n");

    /* System initialization */
    EthApp_Startup();

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

    /* Main test section */
    if (E_OK == retVal)
    {
        for(ti = 0u; ti < ETH_APP_TEST_ROUND; ti++)
        {
            /* DUT <-> Host Initial Handshake */
            status = EthApp_waitForHostAll();
            globalStatus &= status;
            if (PASS != status)
                break;
            for (i = 0U; i < ARRAY_SIZE(testFuncs); i++)
            {
                if((gEthApp.state[gCtrlIdx].testIdx < ARRAY_SIZE(testFuncs)) && (i != gEthApp.state[gCtrlIdx].testIdx))
                {
                    continue;
                }
                EthUtils_printf("\r\n\r\n-----------------------------------------------------------\r\n");
                for(ci = 0; ci < ETH_CTRL_ID_MAX; ++ci)
                {
                    gCtrlIdx = ci;
                    memset(&gEthApp.state[gCtrlIdx].stats, 0U, sizeof(EthApp_Stats));
                    status = testFuncs[i]();
                    globalStatus &= status;
                    EthUtils_printf("Test Result: %s %d\r\n", status?"Pass":"Fail", ti);
                    EthUtils_printf("-----------------------------------------------------------\r\n");
                    /* Some packet from last test pending, clear it up */
                    EthApp_transmitFlush(gEthApp.state[gCtrlIdx].ctrlIdx);
                }
            }
            if(status != 1)
                break;
            EthApp_delay(2000U);
        }
    }
    EthUtils_printf("\r\n");

    /* Show statistics */
    if (E_OK == retVal)
    {
        retVal = EthApp_showStatsAll();
        if (E_OK != retVal)
        {
            EthUtils_printf("main: failed to show statistics\r\n");
        }
    }

    /* Deinitialize the Ethernet controller */
    if (E_OK == retVal)
    {
        retVal = EthApp_deinitAll();
        if (E_OK != retVal)
        {
            EthUtils_printf("Failed to deinitialize: %d\r\n", retVal);
        }
    }


    retVal = EthApp_closeDma();
    if (E_OK != retVal)
    {
        EthUtils_printf("Failed to deinitialize DMA: %d\r\n", retVal);
    }


    EthUtils_printf("ETH Stack Usage: %d bytes\r\n", AppUtils_GetStackUsage());
    if (AppUtils_EthCheckStackAndSectionCorruption() != E_OK)
    {
        retVal = E_NOT_OK;
        EthUtils_printf("ETH Stack/section corruption!!!\r\n");
    }

    EthUtils_printf("Final Tests Results: %s\r\n",
                    (globalStatus == TRUE) ? "PASS" : "FAIL");

    if (E_OK == retVal)
    {
        EthUtils_printf("Eth Application Completed\r\n");
    }
    else
    {
        EthUtils_printf("Eth Application Failed\r\n");
    }

    return retVal;
}


boolean EthApp_test_0001(void)
{
    uint32 num = 0U;
    boolean status;

    EthUtils_printf("test_0001: START\r\n");

    /* Send START cmd */
    EthApp_sendCmd(gEthApp.state[gCtrlIdx].ctrlIdx, CTRL_FRAME_CMD_START);

    /* Receive frames until STOP cmd is detected */
    status = EthApp_receive(gEthApp.state[gCtrlIdx].ctrlIdx, num);
    if (FAIL == status)
    {
        EthUtils_printf("test_0001: failed while receiving frames\r\n");
    }

    EthUtils_printf("test_0001: received %d of %d frames\r\n",
                    gEthApp.state[gCtrlIdx].stats.rxExp1PktCnt, ETH_TEST_ITER_M_COUNT);

    if (gEthApp.state[gCtrlIdx].stats.rxExp1PktCnt != ETH_TEST_ITER_M_COUNT)
    {
        EthUtils_printf("test_0001: received frame count mismatch (exp=%d, got=%d)\r\n",
               ETH_TEST_ITER_M_COUNT, gEthApp.state[gCtrlIdx].stats.rxExp1PktCnt);
        status = FAIL;
    }

    EthUtils_printf("test_0001: END\r\n");

    return status;
}

boolean EthApp_test_0002(void)
{
    uint32 iterations = ETH_TEST_ITER_M_COUNT;
    uint16 len = 1500U;
    boolean txConfirmation = TRUE;
    boolean vlan = FALSE;
    boolean status = PASS;
    uint8 i;

    EthUtils_printf("test_0002: START\r\n");

    /* Do 2 transmit test, 1 for standard and 1 for zero mempy if enabled */
    for (i = 0; i < 2; ++i)
    {
        /* Send START cmd */
        EthApp_sendCmd(gEthApp.state[gCtrlIdx].ctrlIdx, CTRL_FRAME_CMD_START);

#if ETH_ZERO_COPY_API == STD_ON
        gEthApp.state[gCtrlIdx].useExtTxBuffer = (boolean)i;
#endif

        /* Transmit frames */
        status &= EthApp_transmit(gEthApp.state[gCtrlIdx].ctrlIdx,
                                 iterations,
                                 len,
                                 txConfirmation,
                                 vlan,
                                 ETH_DEFAULT_TX_PRIORITY,
                                 &BcastAddr[0U]);
    }

    EthUtils_printf("test_0002: END\r\n");

#if ETH_ZERO_COPY_API == STD_ON
    gEthApp.state[gCtrlIdx].useExtTxBuffer = FALSE;
#endif

    return status;
}

boolean EthApp_test_0003(void)
{
    uint32 iterations = ETH_TEST_ITER_M_COUNT;
    uint32 i;
    sint32 fillStatus;
    uint16 len = 1500U;
    boolean status = TRUE;
    EthFrame frame;

    EthUtils_printf("test_0003: START\r\n");

    /* Initialize frame header */
    memcpy(frame.hdr.dstMac, BcastAddr, ETH_MAC_ADDR_LEN);
    memcpy(frame.hdr.srcMac, gEthApp.state[gCtrlIdx].macAddr, ETH_MAC_ADDR_LEN);
    frame.hdr.etherType = ETHERTYPE_EXPERIMENTAL1;

    /* Send START cmd */
    EthApp_sendCmd(gEthApp.state[gCtrlIdx].ctrlIdx, CTRL_FRAME_CMD_START);

    /* Send and receive frames */
    for (i = 0U; i < iterations; i++)
    {
        if (TRUE == gEthApp.state[gCtrlIdx].verbose)
        {
            EthUtils_printf("test_0003: iteration %d\r\n", i);
        }

        gEthApp.state[gCtrlIdx].saveRxFrame = TRUE;
        gEthApp.state[gCtrlIdx].stats.rxPktCnt = 0U;
        gEthApp.state[gCtrlIdx].stats.rxPktIndCnt = 0U;

        fillStatus = EthUtils_fillPayload((DataFramePayload *)frame.payload,
                                          i % ETH_TEST_NUM_TYPES,
                                          len);
        if (ETH_TEST_PKT_SOK != fillStatus)
        {
            EthUtils_printf("test_0003: failed to fill test packet\r\n");
            status = FAIL;
            break;
        }

        /* Send the frame */
        EthApp_send(gEthApp.state[gCtrlIdx].ctrlIdx, &frame, len, ETH_DEFAULT_TX_PRIORITY);

        /* Wait for Rx frames */
        while (0 == gEthApp.state[gCtrlIdx].stats.rxPktIndCnt)
        {
            if(FALSE == Eth_GetRxEnableInterrupt(gCtrlIdx))
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
        if (frame.hdr.etherType != gEthApp.state[gCtrlIdx].rxFrame.hdr.etherType)
        {
            EthUtils_printf("test_0003: EtherType mismatch (exp=0x%04x, got=0x%04x)\r\n",
                            frame.hdr.etherType & 0xFFFFU,
                            gEthApp.state[gCtrlIdx].rxFrame.hdr.etherType & 0xFFFFU);
            status = FAIL;
            break;
        }

        /* Check if payload matches */
        if (0 != memcmp(&frame.payload, gEthApp.state[gCtrlIdx].rxFrame.payload, len))
        {
            EthUtils_printf("test_0003: payload mismatch\r\n");
            status = FAIL;
            break;
        }
    }

    /* Send STOP cmd */
    EthApp_sendCmd(gEthApp.state[gCtrlIdx].ctrlIdx, CTRL_FRAME_CMD_STOP);

    EthUtils_printf("test_0003: END\r\n");

    return status;
}

boolean EthApp_test_0004(void)
{
    uint32 num = 0U;
    boolean status;

    EthUtils_printf("test_0004: START\r\n");

    /* Send START cmd */
    EthApp_sendCmd(gEthApp.state[gCtrlIdx].ctrlIdx, CTRL_FRAME_CMD_START);

    /* Receive frames until STOP cmd is detected */
    status = EthApp_receive(gEthApp.state[gCtrlIdx].ctrlIdx, num);
    if (FAIL == status)
    {
        EthUtils_printf("test_0004: failed while receiving frames\r\n");
    }

    EthUtils_printf("test_0004: received %d of %d frames\r\n",
                    gEthApp.state[gCtrlIdx].stats.rxExp1PktCnt, ETH_TEST_ITER_M_COUNT / 2);

    if (gEthApp.state[gCtrlIdx].stats.rxExp1PktCnt != ETH_TEST_ITER_M_COUNT / 2)
    {
        status = FAIL;
    }

    EthUtils_printf("test_0004: END\r\n");

    return status;
}

#if (STD_ON == ETH_UPDATE_PHYS_ADDR_FILTER_API)

boolean EthApp_test_0005(void)
{
    uint32 num = ETH_TEST_ITER_M_COUNT;
    uint32 acceptCnt = num / 2;
    boolean itStatus;
    boolean status = PASS;
    Std_ReturnType retVal;

    EthUtils_printf("test_0005: START\r\n");

    /* Part 1: Reset filter and receive frames with controller's address */
    retVal = Eth_UpdatePhysAddrFilter(gEthApp.state[gCtrlIdx].ctrlIdx, NullAddr, ETH_ADD_TO_FILTER);
    if (E_OK != retVal)
    {
        gEthApp.state[gCtrlIdx].stats.filterNullErr++;
    }
    /* Subscribe to spl mcast address used by host app to send mcast frames.
     * HostApp sends mcast frames with non-broadcast address so that
     * broadcast frames in network from Host PC/other machines on network
     * get routed to target test app inadvertently
     */
    if (E_OK == retVal)
    {
        retVal = Eth_UpdatePhysAddrFilter(gEthApp.state[gCtrlIdx].ctrlIdx, TargetMcastAddr, ETH_ADD_TO_FILTER);
    }


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

    EthUtils_printf("test_0005:  Null: Exp1: exp %3d, got %3d\r\n",
                    acceptCnt, gEthApp.state[gCtrlIdx].stats.rxExp1PktCnt);
    EthUtils_printf("test_0005:  Null: Exp2: exp %3d, got %3d\r\n",
                    0U, gEthApp.state[gCtrlIdx].stats.rxExp2PktCnt);

    /* Part 2: Add unicast address and receive frames with that address */
    retVal = Eth_UpdatePhysAddrFilter(gEthApp.state[gCtrlIdx].ctrlIdx, UcastAddr, ETH_ADD_TO_FILTER);
    if (E_OK != retVal)
    {
        gEthApp.state[gCtrlIdx].stats.filterUcastAddErr++;
    }

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

    EthUtils_printf("test_0005: Ucast: Exp1: exp %3d, got %3d\r\n",
                    acceptCnt, gEthApp.state[gCtrlIdx].stats.rxExp1PktCnt);
    EthUtils_printf("test_0005: Ucast: Exp2: exp %3d, got %3d\r\n",
                    acceptCnt, gEthApp.state[gCtrlIdx].stats.rxExp2PktCnt);

    /* Part 3: Add multicast address and receive frames with that address */
    retVal = Eth_UpdatePhysAddrFilter(gEthApp.state[gCtrlIdx].ctrlIdx, McastAddr, ETH_ADD_TO_FILTER);
    if (E_OK != retVal)
    {
        gEthApp.state[gCtrlIdx].stats.filterMcastAddErr++;
    }

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

    EthUtils_printf("test_0005: Mcast: Exp1: exp %3d, got %3d\r\n",
                    acceptCnt, gEthApp.state[gCtrlIdx].stats.rxExp1PktCnt);
    EthUtils_printf("test_0005: Mcast: Exp2: exp %3d, got %3d\r\n",
                    acceptCnt, gEthApp.state[gCtrlIdx].stats.rxExp2PktCnt);

    /* Part 4: Remove multicast address and receive frames with that address */
    retVal = Eth_UpdatePhysAddrFilter(gEthApp.state[gCtrlIdx].ctrlIdx, McastAddr, ETH_REMOVE_FROM_FILTER);
    if (E_OK != retVal)
    {
        gEthApp.state[gCtrlIdx].stats.filterMcastRemErr++;
    }

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

    EthUtils_printf("test_0005: Mcast: Exp1: exp %3d, got %3d\r\n",
                    acceptCnt, gEthApp.state[gCtrlIdx].stats.rxExp1PktCnt);
    EthUtils_printf("test_0005: Mcast: Exp2: exp %3d, got %3d\r\n",
                    0U, gEthApp.state[gCtrlIdx].stats.rxExp2PktCnt);

    /* Part 6: Reset filter and receive frames with controller's address */
    retVal = Eth_UpdatePhysAddrFilter(gEthApp.state[gCtrlIdx].ctrlIdx, NullAddr, ETH_ADD_TO_FILTER);
    if (E_OK != retVal)
    {
        gEthApp.state[gCtrlIdx].stats.filterNullErr++;
    }
    /* Subscribe to spl mcast address used by host app to send mcast frames.
     * HostApp sends mcast frames with non-broadcast address so that
     * broadcast frames in network from Host PC/other machines on network
     * get routed to target test app inadvertently
     */
    if (E_OK == retVal)
    {
        retVal = Eth_UpdatePhysAddrFilter(gEthApp.state[gCtrlIdx].ctrlIdx, TargetMcastAddr, ETH_ADD_TO_FILTER);
    }
    if (E_OK != retVal)
    {
        gEthApp.state[gCtrlIdx].stats.filterNullErr++;
    }


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

    EthUtils_printf("test_0005:  Null: Exp1: exp %3d, got %3d\r\n",
                    acceptCnt, gEthApp.state[gCtrlIdx].stats.rxExp1PktCnt);
    EthUtils_printf("test_0005:  Null: Exp2: exp %3d, got %3d\r\n",
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
        EthUtils_printf("Filter Test Results:\r\n");
        EthUtils_printf("  Status:                %s\r\n", status?"Pass":"Fail");
        EthUtils_printf("  Sent from host:        %d\r\n", num);
        EthUtils_printf("  Unicast ADD errors:    %d\r\n", gEthApp.state[gCtrlIdx].stats.filterUcastAddErr);
        EthUtils_printf("  Ucast REMOVE errors:   %d\r\n", gEthApp.state[gCtrlIdx].stats.filterUcastRemErr);
        EthUtils_printf("  Ucast receive errors:  %d\r\n", gEthApp.state[gCtrlIdx].stats.filterUcastRxErr);
        EthUtils_printf("  Mcast ADD errors:      %d\r\n", gEthApp.state[gCtrlIdx].stats.filterMcastAddErr);
        EthUtils_printf("  Mcast REMOVE errors:   %d\r\n", gEthApp.state[gCtrlIdx].stats.filterMcastRemErr);
        EthUtils_printf("  Mcast receive errors:  %d\r\n", gEthApp.state[gCtrlIdx].stats.filterMcastRxErr);
        EthUtils_printf("  Bcast ADD errors:      %d\r\n", gEthApp.state[gCtrlIdx].stats.filterBcastErr);
        EthUtils_printf("  Bcast receive errors:  %d\r\n", gEthApp.state[gCtrlIdx].stats.filterBcastRxErr);
        EthUtils_printf("  Null ADD errors:       %d\r\n", gEthApp.state[gCtrlIdx].stats.filterNullErr);
        EthUtils_printf("  Null receive errors:   %d\r\n", gEthApp.state[gCtrlIdx].stats.filterNullRxErr);
        EthUtils_printf("  DEM errors:            %d\r\n", gEthApp.state[gCtrlIdx].stats.demErr);
        EthUtils_printf("  DET errors:            %d\r\n", gEthApp.state[gCtrlIdx].stats.detErr);
        EthUtils_printf("\r\n");
    }

    EthUtils_printf("test_0005: END\r\n");

    return status;
}

boolean EthApp_test_0005_1(void)
{
    uint32 num = ETH_TEST_ITER_M_COUNT;
    uint32 acceptCnt = num / 2;
    boolean itStatus;
    boolean status = PASS;
    Std_ReturnType retVal;

    EthUtils_printf("test_0005_1: START\r\n");


    /* Add broadcast address and receive/send frames with any address */
    retVal = Eth_UpdatePhysAddrFilter(gEthApp.state[gCtrlIdx].ctrlIdx, BcastAddr, ETH_ADD_TO_FILTER);
    if (E_OK != retVal)
    {
        gEthApp.state[gCtrlIdx].stats.filterBcastErr++;
    }

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

    EthUtils_printf("test_0005: Bcast: Exp1: exp %3d, got %3d\r\n",
                    gEthApp.state[gCtrlIdx].stats.rxExp1PktCnt, acceptCnt);
    EthUtils_printf("test_0005: Bcast: Exp2: exp %3d, got %3d\r\n",
                    gEthApp.state[gCtrlIdx].stats.rxExp2PktCnt, acceptCnt);

    /* Check error conditions */
    if (((gEthApp.state[gCtrlIdx].stats.filterBcastErr > 0U) ||
        (gEthApp.state[gCtrlIdx].stats.filterBcastRxErr > 0U)))
    {
        status = FAIL;
    }

    if ((FAIL == status) || (TRUE == gEthApp.state[gCtrlIdx].verbose))
    {
        EthUtils_printf("Filter Test Results:\r\n");
        EthUtils_printf("  Status:                %s\r\n", status?"Pass":"Fail");
        EthUtils_printf("  Sent from host:        %d\r\n", num);
        EthUtils_printf("  Bcast ADD errors:      %d\r\n", gEthApp.state[gCtrlIdx].stats.filterBcastErr);
        EthUtils_printf("  Bcast receive errors:  %d\r\n", gEthApp.state[gCtrlIdx].stats.filterBcastRxErr);
        EthUtils_printf("  DEM errors:            %d\r\n", gEthApp.state[gCtrlIdx].stats.demErr);
        EthUtils_printf("  DET errors:            %d\r\n", gEthApp.state[gCtrlIdx].stats.detErr);
        EthUtils_printf("\r\n");
    }

    EthUtils_printf("test_0005: END\r\n");

    return status;
}
#endif /* (STD_ON == ETH_UPDATE_PHYS_ADDR_FILTER_API) */

boolean EthApp_test_0006(void)
{
    uint32 iterations = ETH_TEST_ITER_M_COUNT;
    uint16 len = 1500U;
    boolean txConfirmation = TRUE;
    boolean vlan = FALSE;
    boolean status;

    EthUtils_printf("test_0006: START\r\n");

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

    EthUtils_printf("test_0006: END\r\n");

    return status;
}

boolean EthApp_test_0007(void)
{
    uint32 iterations = ETH_TEST_ITER_M_COUNT;
    uint16 len = 1500U;
    boolean txConfirmation = TRUE;
    boolean vlan = TRUE;
    boolean status;

    EthUtils_printf("test_0007: START\r\n");

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

    EthUtils_printf("test_0007: END\r\n");

    return status;
}

boolean EthApp_test_0008(void)
{
    uint32 iterations = ETH_TEST_ITER_S_COUNT;
    uint16 len;
    boolean txConfirmation = TRUE;
    boolean vlan = FALSE;
    boolean st;
    boolean status = PASS;

    EthUtils_printf("test_0008: START\r\n");

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
            EthUtils_printf("test_0008: failed to transmit frames of length %u\r\n",
                            len);
            status = FAIL;
        }

        /* Add delay per test case */
        EthApp_delay(10U);
    }

    EthUtils_printf("test_0008: END\r\n");

    return status;
}

boolean EthApp_test_0009(void)
{
    uint32 num = 0U;
    boolean status;

    EthUtils_printf("test_0009: START\r\n");

    /* Send START cmd */
    EthApp_sendCmd(gEthApp.state[gCtrlIdx].ctrlIdx, CTRL_FRAME_CMD_START);

    /* Receive frames until STOP cmd is detected */
    status = EthApp_receive(gEthApp.state[gCtrlIdx].ctrlIdx, num);
    if (FAIL == status)
    {
        EthUtils_printf("test_0009: failed while receiving frames\r\n");
    }

    EthUtils_printf("test_0009: received %d of %d frames\r\n",
                    gEthApp.state[gCtrlIdx].stats.rxVlanPktCnt, ETH_TEST_ITER_M_COUNT);

    if (gEthApp.state[gCtrlIdx].stats.rxVlanPktCnt != ETH_TEST_ITER_M_COUNT)
    {
        EthUtils_printf("test_0009: received frame count mismatch (exp=%d, got=%d)\r\n",
               ETH_TEST_ITER_M_COUNT, gEthApp.state[gCtrlIdx].stats.rxVlanPktCnt);
        status = FAIL;
    }

    EthUtils_printf("test_0009: END\r\n");

    return status;
}

const char* EthApp_GetCtrlModeString(Eth_ModeType ControllerMode)
{
    if (ETH_MODE_DOWN == ControllerMode)
    {
        return "DOWN";
    }
    else if (ETH_MODE_ACTIVE == ControllerMode)
    {
        return "ACTIVE";
    }
    else
    {
        return "INVALID";
    }
}

boolean EthApp_test_0010(void)
{
    uint32 iterations = ETH_TEST_ITER_S_COUNT;
    uint32 i;
    boolean status = PASS;
    Std_ReturnType retVal;
    Eth_ModeType CtrlMode;

#if (ETH_CTRL_ID_MAX == 2U)
    /* This test to make sure 1 controller can work while others in DOWN state */
    uint8 otherIdx = (gEthApp.state[gCtrlIdx].ctrlIdx == 0u) ? 1u:0u;
    EthApp_SetControllerMode(otherIdx, ETH_MODE_DOWN);
#endif

    EthUtils_printf("test_0010: START\r\n");


    /* Send START cmd */
    EthApp_sendCmd(gEthApp.state[gCtrlIdx].ctrlIdx, CTRL_FRAME_CMD_START);

    for (i = 0U; i < iterations; i++)
    {
        if (TRUE == gEthApp.state[gCtrlIdx].verbose)
        {
            EthUtils_printf("test_0010: iteration %d\r\n", i+1);
        }

        EthUtils_printf("Set CtrlMode DOWN\r\n");
        /* Set controller to down state */
        retVal = EthApp_SetControllerMode(gEthApp.state[gCtrlIdx].ctrlIdx, ETH_MODE_DOWN);
        if (E_OK != retVal)
        {
            EthUtils_printf("test_0010: failed to set down mode\r\n");
            gEthApp.state[gCtrlIdx].stats.ctrlModeErr++;
        }
        else
        {
            gEthApp.state[gCtrlIdx].stats.ctrlModeActCnt++;
        }

        /*Get controller to down state */
        retVal = EthApp_GetControllerMode(gEthApp.state[gCtrlIdx].ctrlIdx, &CtrlMode);
        if (E_OK != retVal)
        {
            EthUtils_printf("test_0010: failed to get control mode\r\n");
            gEthApp.state[gCtrlIdx].stats.ctrlModeErr++;
        }
        else
        {
            EthUtils_printf("Get CtrlMode %s\r\n", EthApp_GetCtrlModeString(CtrlMode));
        }

        Eth_MainFunction();

        EthUtils_printf("Set CtrlMode ACTIVE\r\n");
        /* Set controller back to active state */
        retVal = EthApp_SetControllerMode(gEthApp.state[gCtrlIdx].ctrlIdx, ETH_MODE_ACTIVE);
        if (E_OK != retVal)
        {
            EthUtils_printf("test_0010: failed to set active mode\r\n");
            gEthApp.state[gCtrlIdx].stats.ctrlModeErr++;
        }
        else
        {
            gEthApp.state[gCtrlIdx].stats.ctrlModeDownCnt++;
        }

        /*Get controller to down state */
        retVal = EthApp_GetControllerMode(gEthApp.state[gCtrlIdx].ctrlIdx, &CtrlMode);
        if (E_OK != retVal)
        {
            EthUtils_printf("test_0010: failed to get down mode\r\n");
            gEthApp.state[gCtrlIdx].stats.ctrlModeErr++;
        }
        else
        {
            EthUtils_printf("Get CtrlMode %s\r\n", EthApp_GetCtrlModeString(CtrlMode));
        }

        Eth_MainFunction();

#if (STD_OFF == ETH_WRAPPER_ENABLED)
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
#endif
        if ((FAIL == status) || (TRUE == gEthApp.state[gCtrlIdx].verbose))
        {
            EthUtils_printf("Controller Mode Test Results:\r\n");
            EthUtils_printf("  Status:                   %s\r\n", status?"Pass":"Fail");
            EthUtils_printf("  Requested:                %d\r\n", iterations);
            EthUtils_printf("  Changes to DOWN mode:     %d\r\n", gEthApp.state[gCtrlIdx].stats.ctrlModeDownCnt);
            EthUtils_printf("  Changes to ACTIVE mode:   %d\r\n", gEthApp.state[gCtrlIdx].stats.ctrlModeActCnt);
            EthUtils_printf("  Mode change errors:       %d\r\n", gEthApp.state[gCtrlIdx].stats.ctrlModeErr);
            EthUtils_printf("  Indicated DOWN changes:   %d\r\n", gEthApp.state[gCtrlIdx].stats.ctrlModeIndDownCnt);
            EthUtils_printf("  Indicated ACTIVE changes: %d\r\n", gEthApp.state[gCtrlIdx].stats.ctrlModeIndActCnt);
            EthUtils_printf("  Indication errors:        %d\r\n", gEthApp.state[gCtrlIdx].stats.ctrlModeIndErr);
            EthUtils_printf("\r\n");
        }

        if (FAIL == status)
        {
            EthUtils_printf("test_0010: failed to change controller mode\r\n");
            break;
        }

        /* Transmit frames */
        status = EthApp_test_0002();
        if (FAIL == status)
        {
            EthUtils_printf("test_0010: failed to transmit frames\r\n");
        }

        /* Receive frames */
        status = EthApp_test_0001();
        if (FAIL == status)
        {
            EthUtils_printf("test_0010: failed to receive frames\r\n");
        }
    }

    EthUtils_printf("test_0010: completed %d of %d iterations\r\n", i, iterations);
    EthUtils_printf("test_0010: END\r\n");

#if (ETH_CTRL_ID_MAX == 2U)
    EthApp_SetControllerMode(otherIdx, ETH_MODE_ACTIVE);
#endif

    return status;
}

boolean EthApp_test_0011(void)
{
    uint32 iterations = ETH_TEST_ITER_M_COUNT;
    uint16 len = 1500U;
    boolean txConfirmation = TRUE;
    boolean vlan = FALSE;
    boolean status = true;

    EthUtils_printf("test_0011: START\r\n");

    /* Send START cmd */
    EthApp_sendCmd(gEthApp.state[gCtrlIdx].ctrlIdx, CTRL_FRAME_CMD_START);

    /* Transmit frames */
    status = EthApp_transmit(gEthApp.state[gCtrlIdx].ctrlIdx,
                             iterations,
                             len,
                             txConfirmation,
                             vlan,
                             ETH_DEFAULT_TX_PRIORITY,
                             &TxMcastAddr[0U]);

    EthUtils_printf("test_0011: END\r\n");

    return status;
}

boolean EthApp_test_0100(uint32 frames, uint32 size)
{
    Eth_BufIdxType bufIdx;
    BufReq_ReturnType bufStatus;
    boolean txConfirmation = FALSE;
    uint8 *bufPtr;
    uint32 iterations = frames;
    uint32 retryCnt;
    uint32 i;
    uint16 len = size;
    Std_ReturnType retVal;

    EthUtils_printf("test_0100 frames %d size %d: START\r\n", frames, size);

    /* Send START cmd */
    EthApp_sendCmd(gEthApp.state[gCtrlIdx].ctrlIdx, CTRL_FRAME_CMD_START);

    /* Send frames */
    for (i = 0U; i < iterations; i++)
    {
        /* Request one buffer */
        bufStatus = EthApp_provideTxBuffer(gEthApp.state[gCtrlIdx].ctrlIdx,
                                           ETH_DEFAULT_TX_PRIORITY,
                                           &bufIdx,
                                           &bufPtr,
                                           &len);

        /* Transmit it as is (only header is updated) */
        if (BUFREQ_OK == bufStatus)
        {
            retryCnt = txConfirmation ? 0U : ETHAPP_TRANSMIT_RETRY_COUNT;

            do
            {
                retVal = EthApp_Transmit(gEthApp.state[gCtrlIdx].ctrlIdx,
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
                EthUtils_printf("test_0100: failed to send buffer\r\n");
            }
            else
            {
                if(FALSE == Eth_GetTxEnableInterrupt(gCtrlIdx))
                {
                    EthApp_TxConfirmationInt(gEthApp.state[gCtrlIdx].ctrlIdx);
                }
            }
        }
        else
        {
            EthUtils_printf("test_0100: failed to get TX buffer: %d\r\n",
                            (int)bufStatus);
        }
    }

    /* Send STOP cmd 10x to prevent lost,
     * receiver app will handle flushing the extra STOP cmd */
    for (i = 0U; i < ETH_TEST_ITER_S_COUNT; i++)
    {
        EthApp_sendCmd(gEthApp.state[gCtrlIdx].ctrlIdx, CTRL_FRAME_CMD_STOP);
    }

    EthUtils_printf("test_0100: %d frames size %d\r\n", i, size);
    EthUtils_printf("test_0100: END\r\n");

    return PASS;
}

boolean EthApp_test_0110(void)
{
    uint16  txPktSizeMesure[] = { 1500u,   512u,    256u,    128u,    64u };
    uint32  iterator_num[]   =  { 150000u, 150000u, 150000u, 150000u, 150000u };
    boolean status = PASS;
    uint32 i;

    EthUtils_printf("test_0110: START\r\n");
    for(i = 0; i< sizeof(txPktSizeMesure)/sizeof(uint16); ++i)
    {
        (void)EthApp_test_0100(iterator_num[i], txPktSizeMesure[i]);
        EthApp_transmitFlush(gEthApp.state[gCtrlIdx].ctrlIdx);
    }

    EthUtils_printf("test_0110: DONE\r\n");
    return status;
}

boolean EthApp_test_0200(void)
{
    uint32 iterations = ETH_TEST_ITER_M_COUNT;
    uint32 i;
    sint32 fillStatus;
    uint16 len = 1500U;
    boolean status = TRUE;
    EthFrame frame;

    EthUtils_printf("test_0200: START\r\n");

    /* Initialize frame header */
    memcpy(frame.hdr.dstMac, BcastAddr, ETH_MAC_ADDR_LEN);
    memcpy(frame.hdr.srcMac, gEthApp.state[gCtrlIdx].macAddr, ETH_MAC_ADDR_LEN);
    frame.hdr.etherType = ETHERTYPE_EXPERIMENTAL1;

    /* Send and receive frames */
    for (i = 0U; i < iterations; i++)
    {
        if (TRUE == gEthApp.state[gCtrlIdx].verbose)
        {
            EthUtils_printf("test_0200: iteration %d\r\n", i);
        }

        gEthApp.state[gCtrlIdx].saveRxFrame = TRUE;
        gEthApp.state[gCtrlIdx].stats.rxPktCnt = 0U;
        gEthApp.state[gCtrlIdx].stats.rxPktIndCnt = 0U;

        fillStatus = EthUtils_fillPayload((DataFramePayload *)frame.payload,
                                          i % ETH_TEST_NUM_TYPES,
                                          len);
        if (ETH_TEST_PKT_SOK != fillStatus)
        {
            EthUtils_printf("test_0200: failed to fill test packet\r\n");
            status = FAIL;
            break;
        }

        /* Send the frame */
        EthApp_send(gEthApp.state[gCtrlIdx].ctrlIdx, &frame, len, ETH_DEFAULT_TX_PRIORITY);

        /* Wait for Rx frames */
        while (0 == gEthApp.state[gCtrlIdx].stats.rxPktIndCnt)
        {
            if(FALSE == Eth_GetRxEnableInterrupt(gCtrlIdx))
            {
                EthApp_receiveAllFifo(gEthApp.state[gCtrlIdx].ctrlIdx);
            }
        }

        /* Check if EtherType field matches */
        if (frame.hdr.etherType != gEthApp.state[gCtrlIdx].rxFrame.hdr.etherType)
        {
            EthUtils_printf("test_0200: EtherType mismatch (exp=0x%04x, got=0x%04x)\r\n",
                            frame.hdr.etherType & 0xFFFFU,
                            gEthApp.state[gCtrlIdx].rxFrame.hdr.etherType & 0xFFFFU);
            status = FAIL;
            break;
        }

        /* Check if payload matches */
        if (0 != memcmp(&frame.payload, gEthApp.state[gCtrlIdx].rxFrame.payload, len))
        {
            EthUtils_printf("test_0200: payload mismatch\r\n");
            status = FAIL;
            break;
        }
    }

    EthUtils_printf("test_0200: completed %d of %d iterations\r\n", i, iterations);
    EthUtils_printf("test_0200: END\r\n");

    return status;
}

boolean EthApp_test_0300(void)
{
    uint32 i,j,iterations = ETH_TEST_ITER_S_COUNT;
    uint32 delays_msec[ETH_TEST_ITER_S_COUNT] =
              { 31u, 62u, 125u, 250u, 500u, 750u, 1000u, 2000u, 5000u, 1000u };
    uint16 len = 1500;
    boolean status = PASS;
    BufReq_ReturnType bufStatus;
    Eth_BufIdxType bufIdx;
    sint64 diffTs = 0;
    uint8 *bufPtr;

    EthUtils_printf("test_0300: START\r\n");

    /* Send START cmd */
    EthApp_sendCmd(gEthApp.state[gCtrlIdx].ctrlIdx, CTRL_FRAME_CMD_START);

    gEthApp.state[gCtrlIdx].saveTxTime = 1u;
    gEthApp.state[gCtrlIdx].txFrameCnt = 0u;
    for(i = 0; i < iterations; ++i)
    {
        gEthApp.state[gCtrlIdx].saveTxTimeStamp[i].seconds      = 0U;
        gEthApp.state[gCtrlIdx].saveTxTimeStamp[i].secondsHi    = 0U;
        gEthApp.state[gCtrlIdx].saveTxTimeStamp[i].nanoseconds  = 0U;
    }

    for(i = 0; i < iterations; ++i)
    {
        bufStatus = EthApp_provideTxBuffer(gEthApp.state[gCtrlIdx].ctrlIdx,
                                               ETH_DEFAULT_TX_PRIORITY,
                                               &bufIdx,
                                               &bufPtr,
                                               &len);

        if(BUFREQ_OK == bufStatus)
        {
            EthUtils_fillPayload((DataFramePayload *)bufPtr,
                                    i % ETH_TEST_NUM_TYPES,
                                    len);

            EthApp_EnableEgressTimeStamp(gEthApp.state[gCtrlIdx].ctrlIdx, bufIdx);

            (void)EthApp_Transmit(gEthApp.state[gCtrlIdx].ctrlIdx,
                               bufIdx,
                               (Eth_FrameType)ETHERTYPE_EXPERIMENTAL1,
                               TRUE,
                               len,
                               BcastAddr);

            if(FALSE == Eth_GetTxEnableInterrupt(gCtrlIdx))
            {
                EthApp_TxConfirmationInt(gEthApp.state[gCtrlIdx].ctrlIdx);
            }

            if (1000U < delays_msec[i])
            {
                for (j = 0; j < (delays_msec[i]/1000U); j++)
                {
                    EthApp_delay(1000U);
                }
                EthApp_delay((delays_msec[i]%1000U));
            }
            else
            {
                EthApp_delay(delays_msec[i]);
            }
        }
    }

    /* Send STOP cmd */
    EthApp_sendCmd(gEthApp.state[gCtrlIdx].ctrlIdx, CTRL_FRAME_CMD_STOP);

    for(i = 0; i < iterations; ++i)
    {
        if (i == 0)
        {
            EthUtils_printf("Egress timestamp %d Ts: %9u %2u %2u\r\n", i,
                gEthApp.state[gCtrlIdx].saveTxTimeStamp[i].nanoseconds,
                gEthApp.state[gCtrlIdx].saveTxTimeStamp[i].seconds,
                gEthApp.state[gCtrlIdx].saveTxTimeStamp[i].secondsHi);
        }
        else
        {
            diffTs = TS_DIFF64NS(gEthApp.state[gCtrlIdx].saveTxTimeStamp[i], gEthApp.state[gCtrlIdx].saveTxTimeStamp[i-1]);
            EthUtils_printf("Egress timestamp %d Ts: %9u %2u %2u [Exp: %.9f Actual: %.9f]\r\n", i,
                gEthApp.state[gCtrlIdx].saveTxTimeStamp[i].nanoseconds,
                gEthApp.state[gCtrlIdx].saveTxTimeStamp[i].seconds,
                gEthApp.state[gCtrlIdx].saveTxTimeStamp[i].secondsHi,
                ((float)delays_msec[i-1]/1000),
                ((float)diffTs/SEC_TO_NS));
        }
    }

    gEthApp.state[gCtrlIdx].saveTxTime = 0u;
    EthUtils_printf("test_0300: END\r\n");

    return status;
}

boolean EthApp_test_0301(void)
{
    uint32 delays_msec[ETH_TEST_ITER_S_COUNT] =
              { 31u, 62u, 125u, 250u, 500u, 750u, 1000u, 2000u, 5000u, 1000u };
    uint32 num = 0U;
    boolean status;
    sint64 diffTs = 0;
    int i;

    EthUtils_printf("test_0301: START\r\n");

    /* Send START cmd */
    EthApp_sendCmd(gEthApp.state[gCtrlIdx].ctrlIdx, CTRL_FRAME_CMD_START);

    gEthApp.state[gCtrlIdx].saveRxTime = 1u;
    gEthApp.state[gCtrlIdx].rxFrameCnt = 0u;
    for(i = 0; i < ETH_TEST_ITER_S_COUNT; ++i)
    {
        gEthApp.state[gCtrlIdx].saveRxTimeStamp[i].seconds      = 0U;
        gEthApp.state[gCtrlIdx].saveRxTimeStamp[i].secondsHi    = 0U;
        gEthApp.state[gCtrlIdx].saveRxTimeStamp[i].nanoseconds  = 0U;
    }

    /* Receive frames until STOP cmd is detected */
    status = EthApp_receive(gEthApp.state[gCtrlIdx].ctrlIdx, num);
    if (FAIL == status)
    {
        EthUtils_printf("test_0301: failed while receiving frames\r\n");
    }

    EthUtils_printf("test_0301: received %d of %d frames\r\n",
                    gEthApp.state[gCtrlIdx].stats.rxExp1PktCnt, ETH_TEST_ITER_S_COUNT);

    if (gEthApp.state[gCtrlIdx].stats.rxExp1PktCnt != ETH_TEST_ITER_S_COUNT)
    {
        EthUtils_printf("test_0301: received frame count mismatch (exp=%d, got=%d)\r\n",
               ETH_TEST_ITER_S_COUNT, gEthApp.state[gCtrlIdx].stats.rxExp1PktCnt);
        status = FAIL;
    }

    for(i = 0; i < ETH_TEST_ITER_S_COUNT; ++i)
    {
        if (i == 0)
        {
            EthUtils_printf("Ingress timestamp %d Ts: %9u %2u %2u\r\n", i,
                    gEthApp.state[gCtrlIdx].saveRxTimeStamp[i].nanoseconds,
                    gEthApp.state[gCtrlIdx].saveRxTimeStamp[i].seconds,
                    gEthApp.state[gCtrlIdx].saveRxTimeStamp[i].secondsHi);
        }
        else
        {
            diffTs = TS_DIFF64NS(gEthApp.state[gCtrlIdx].saveRxTimeStamp[i], gEthApp.state[gCtrlIdx].saveRxTimeStamp[i-1]);
            EthUtils_printf("Ingress timestamp %d Ts: %9u %2u %2u [Exp: %.9f Actual: %.9f]\r\n", i,
                    gEthApp.state[gCtrlIdx].saveRxTimeStamp[i].nanoseconds,
                    gEthApp.state[gCtrlIdx].saveRxTimeStamp[i].seconds,
                    gEthApp.state[gCtrlIdx].saveRxTimeStamp[i].secondsHi,
                    ((float)delays_msec[i-1]/1000),
                    ((float)diffTs/SEC_TO_NS));
        }
    }
    gEthApp.state[gCtrlIdx].saveRxTime = 0u;

    EthUtils_printf("test_0301: END\r\n");

    return status;
}

boolean EthApp_test_0302(void)
{
    uint32 i,iterations = ETH_TEST_ITER_S_COUNT;
    uint16 len = 1500;
    boolean status = PASS;
    Eth_TimeStampQualType timeQual = ETH_INVALID;
    Eth_TimeStampType tsVal = {};
    EthFrame frame;

    EthUtils_printf("test_0302: START\r\n");

    /* Initialize frame header */
    memcpy(frame.hdr.dstMac, BcastAddr, ETH_MAC_ADDR_LEN);
    memcpy(frame.hdr.srcMac, gEthApp.state[gCtrlIdx].macAddr, ETH_MAC_ADDR_LEN);
    frame.hdr.etherType = ETHERTYPE_EXPERIMENTAL1;

    /* Send START cmd */
    EthApp_sendCmd(gEthApp.state[gCtrlIdx].ctrlIdx, CTRL_FRAME_CMD_START);

    for(i = 0; i < iterations; ++i)
    {
        (void)EthUtils_fillPayload((DataFramePayload *)frame.payload,
                                          i % ETH_TEST_NUM_TYPES,
                                          len);

        /* Send the frame */
        EthApp_send(gEthApp.state[gCtrlIdx].ctrlIdx, &frame, len, ETH_DEFAULT_TX_PRIORITY);

        EthApp_delay(1000u);

        EthApp_GetCurrentTime(gEthApp.state[gCtrlIdx].ctrlIdx, &timeQual, &tsVal);

        EthUtils_printf("Current timestamp %d Ts: %9u %2u %2u\r\n",
                timeQual,
                tsVal.nanoseconds,
                tsVal.seconds,
                tsVal.secondsHi);
    }

    /* Send STOP cmd */
    EthApp_sendCmd(gEthApp.state[gCtrlIdx].ctrlIdx, CTRL_FRAME_CMD_STOP);

    EthUtils_printf("test_0302: END\r\n");

    return status;
}

#if (STD_ON == ETH_TRAFFIC_SHAPING_API)
boolean EthApp_test_0400_Helper(uint16 type,
                                uint8 pcp,
                                uint16 vid,
                                uint16 etherType)
{
    Eth_BufIdxType bufIdx;
    BufReq_ReturnType bufStatus;
    boolean txConfirmation = FALSE;
    uint8 *bufPtr;
    uint32 iterations = 15000;
    uint32 retryCnt;
    uint32 i;
    uint16 len = 1500U;
    Std_ReturnType retVal = E_NOT_OK;

    /* Send frames */
    for (i = 0U; i < iterations; i++)
    {
        /* Request one buffer */
        bufStatus = EthApp_provideTxBuffer(gEthApp.state[gCtrlIdx].ctrlIdx,
                                           ETH_DEFAULT_TX_PRIORITY,
                                           &bufIdx,
                                           &bufPtr,
                                           &len);

        /* Transmit it as is (only header is updated) */
        if (BUFREQ_OK == bufStatus)
        {
            EthUtils_fillVlanHdr((VlanDataFramePayload *)bufPtr, type, len, pcp, vid, etherType);

            retryCnt = txConfirmation ? 0U : ETHAPP_TRANSMIT_RETRY_COUNT;

            do
            {
                retVal = EthApp_Transmit(gEthApp.state[gCtrlIdx].ctrlIdx,
                                      bufIdx,
                                      (Eth_FrameType)ETHERTYPE_VLAN_TAG,
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
                EthUtils_printf("test_0400: failed to send buffer\r\n");
            }
            else
            {
                if(FALSE == Eth_GetTxEnableInterrupt(gCtrlIdx))
                {
                    EthApp_TxConfirmationInt(gEthApp.state[gCtrlIdx].ctrlIdx);
                }
            }
        }
        else
        {
            EthUtils_printf("test_0400: failed to get TX buffer: %d\r\n",
                            (int)bufStatus);
        }
    }

    return PASS;
}

#define TEST_PRIO_NUM                   (8u)
#define TEST_BANDWIDTH_PRIO_0_BPS       (10000000) /*10mbps*/
#define TEST_BANDWIDTH_PRIO_1_BPS       (20000000) /*20mbps*/
#define TEST_BANDWIDTH_PRIO_2_BPS       (30000000) /*30mbps*/
#define TEST_BANDWIDTH_PRIO_3_BPS       (40000000) /*40mbps*/
#define TEST_BANDWIDTH_PRIO_4_BPS       (50000000) /*50mbps*/
#define TEST_BANDWIDTH_PRIO_5_BPS       (60000000) /*60mbps*/
#define TEST_BANDWIDTH_PRIO_6_BPS       (70000000) /*70mbps*/
#define TEST_BANDWIDTH_PRIO_7_BPS       (80000000) /*80mbps*/

uint32 Test_RateLimit[TEST_PRIO_NUM] = {
    TEST_BANDWIDTH_PRIO_0_BPS,
    TEST_BANDWIDTH_PRIO_1_BPS,
    TEST_BANDWIDTH_PRIO_2_BPS,
    TEST_BANDWIDTH_PRIO_3_BPS,
    TEST_BANDWIDTH_PRIO_4_BPS,
    TEST_BANDWIDTH_PRIO_5_BPS,
    TEST_BANDWIDTH_PRIO_6_BPS,
    TEST_BANDWIDTH_PRIO_7_BPS,
};

boolean EthApp_test_0400(void)
{
    uint32 bandwidth;
    int i;

    for(i = 0U; i < TEST_PRIO_NUM; ++i)
    {
        Eth_SetBandwidthLimit(gEthApp.state[gCtrlIdx].ctrlIdx, i, Test_RateLimit[i]);
    }

    for(i = TEST_PRIO_NUM-1; i >= 0; i--)
    {
        EthUtils_printf("test_0400: START %d\r\n", i);

        /* Send START cmd */
        EthApp_sendCmd(gEthApp.state[gCtrlIdx].ctrlIdx, CTRL_FRAME_CMD_START);

        EthApp_test_0400_Helper(ETH_TEST_TYPE_PATTERN_1, i, ETH_TEST_VLAN_VID, ETHERTYPE_EXPERIMENTAL1);

        /* Send STOP cmd */
        EthApp_sendCmd(gEthApp.state[gCtrlIdx].ctrlIdx, CTRL_FRAME_CMD_STOP);

        Eth_GetBandwidthLimit(gEthApp.state[gCtrlIdx].ctrlIdx, i, &bandwidth);
        EthUtils_printf("test_0400 bandwidth %d: END\r\n", bandwidth);

        /* Flush pending rx packet */
        EthApp_flushRecvQueue(gEthApp.state[gCtrlIdx].ctrlIdx);
    }

    /* Reset bandwidth for all priorities queues */
    for(i = 0; i < TEST_PRIO_NUM; ++i)
    {
        Eth_SetBandwidthLimit(gEthApp.state[gCtrlIdx].ctrlIdx, i, 0U);
    }

    return PASS;
}
#endif /* (STD_ON == ETH_TRAFFIC_SHAPING_API) */

static boolean EthApp_receiveTputHelper(uint32 num)
{
    boolean status = PASS;

    /* Reset test counters */
    gEthApp.state[gCtrlIdx].stats.rxPktCnt       = 0U;
    gEthApp.state[gCtrlIdx].stats.rxExp3BytesCnt = 0U;
    gEthApp.state[gCtrlIdx].stats.rxExp3PktCnt   = 0U;
    gEthApp.state[gCtrlIdx].stats.rxExp1PktCnt   = 0U;
    gEthApp.state[gCtrlIdx].stats.rxExp2PktCnt   = 0U;
    gEthApp.state[gCtrlIdx].stats.rxVlanPktCnt   = 0U;
    gEthApp.state[gCtrlIdx].stats.rxPktNotRecv   = 0U;
    gEthApp.state[gCtrlIdx].stats.rxPktIndCnt    = 0U;
    gEthApp.state[gCtrlIdx].stats.rxEtherTypeErr = 0U;
    gEthApp.state[gCtrlIdx].stats.rxPayloadErr   = 0U;
    gEthApp.state[gCtrlIdx].stats.rxCtrlIdxErr   = 0U;
    gEthApp.state[gCtrlIdx].stats.detErr         = 0U;
    gEthApp.state[gCtrlIdx].stats.demErr         = 0U;

    gEthApp.state[gCtrlIdx].runTest = TRUE;

    /* Wait for Rx frames */
    while (gEthApp.state[gCtrlIdx].runTest)
    {
        if(FALSE == Eth_GetRxEnableInterrupt(gCtrlIdx))
        {
            EthApp_receiveAllFifo(gCtrlIdx);
        }
    }

    return status;
}

boolean EthApp_receiveTput(uint32 frames, uint16 pktLen)
{
    TickType t0 = 0U;
    TickType elapsed_tick = 0U;
    float32 elapsed = 0U;
    float32 pps = 0U;
    float32 mbps = 0U;
    uint32 rxframes = 0U;
    uint32 rxbytecnt = 0U;

    GetCounterValue(0, &t0);

    (void)EthApp_receiveTputHelper(frames);

    /* Get the "end" timestamp after DUT is done sending frames */
    GetElapsedValue(0, &t0, &elapsed_tick);
    elapsed = (float32)elapsed_tick/(1000U*OS_TIME_SCALE_VALUE);

    /* Send SYN ACK to notify Host that we received the STOP cmd */
    EthApp_notifyCmdRecv(gEthApp.state[gCtrlIdx].ctrlIdx);

    rxframes =  gEthApp.state[gCtrlIdx].stats.rxExp3PktCnt;
    rxbytecnt =  gEthApp.state[gCtrlIdx].stats.rxExp3BytesCnt;

    /* Compute elapsed time, packets per second and Mbps */
    pps = (float32)rxframes / elapsed;
    mbps = (float32)rxbytecnt * 8 / elapsed / 1000000;

    EthUtils_printf("receiveTput-%d: received %d frames in %.2f secs (%.2f frames/s, %.2f Mbps) Lost %d\r\n",
           pktLen, rxframes, elapsed, pps, mbps, (frames-rxframes));;

    return PASS;
}

boolean EthApp_test_0101(void)
{
    uint32 frames = 150000;
    uint16 rxPktSizeMesure[] = { 1500u,   512u,    256u,    128u,    64u };
    uint32 iterator_num[]    = { 150000u, 150000u, 150000u, 300000u, 300000u };
    boolean status;
    uint32 i;

    /* set rx interupt to lower than tick irq, to get correct tick */
    BoardUtils_setClockPIsrPrio(0u);

    EthUtils_printf("test_0101: START\r\n");
    for(i = 0; i< sizeof(rxPktSizeMesure)/sizeof(uint16); ++i)
    {
        frames = iterator_num[i];

        /* Send START cmd */
        EthApp_sendCmd(gEthApp.state[gCtrlIdx].ctrlIdx, CTRL_FRAME_CMD_START);

        /* Receive packets and measure the throughput */
        status = EthApp_receiveTput(frames, rxPktSizeMesure[i]);
        if (status == FAIL)
            EthUtils_printf("test_0101: failed to get receive throughput\n");

        /* Host may send multiple STOP packet, so try to clear that packet */
        EthApp_flushRecvQueue(gEthApp.state[gCtrlIdx].ctrlIdx);
    }
    EthUtils_printf("test_0101: END\r\n");

    /* reset tick irq to orignal value */
    BoardUtils_setClockPIsrPrio(15u);

    return status;
}

#if (STD_ON == ETHTRCV_GETLINKSTATE_API)
boolean EthApp_test_0500(void)
{
    Std_ReturnType retVal = E_OK;
    uint8 trcvIdx;
    uint32 iterations = 100;
    uint32 i;

    EthUtils_printf("test_0500: START\r\n");

    for(trcvIdx = 0u; trcvIdx < ETHTRCV_MAX_CONTROLLER; ++trcvIdx)
    {
        for (i = 0U; i < iterations; i++)
        {
            /* Get Tranceiver Link State */
            retVal = EthApp_getTranceiverLinkState(trcvIdx);
            if (E_OK == retVal)
            {
                /* check Link State every 1 sec */
                EthApp_delay(1000U);
            }
            else
            {
                EthUtils_printf("test_0500: Read Transceiver Link State Failed\r\n");
                break;
            }
        }
    }

    EthUtils_printf("test_0500: END\r\n");
    return (E_OK == retVal) ? PASS : FAIL;
}
#endif /* (STD_ON == ETHTRCV_GETLINKSTATE_API) */

void EthApp_test_0610_Helper(uint16 type,
    uint8 pcp,
    uint16 vid,
    uint16 etherType)
{
    Eth_BufIdxType bufIdx;
    BufReq_ReturnType bufStatus;
    boolean txConfirmation = FALSE;
    uint8 *bufPtr;
    uint16 len = 1500U;

    bufStatus = Eth_ProvideTxBuffer(gEthApp.state[gCtrlIdx].ctrlIdx,
                                    pcp,
                                    &bufIdx,
                                    &bufPtr,
                                    &len);

    /* Transmit it as is (only header is updated) */
    if (BUFREQ_OK == bufStatus)
    {
        EthUtils_fillVlanHdr((VlanDataFramePayload *)bufPtr, type, len, pcp, vid, etherType);

        EthApp_Transmit(gEthApp.state[gCtrlIdx].ctrlIdx,
                                  bufIdx,
                                  (Eth_FrameType)ETHERTYPE_VLAN_TAG,
                                  txConfirmation,
                                  len,
                                  BcastAddr);
    }
    else
    {
        if(FALSE == Eth_GetTxEnableInterrupt(gCtrlIdx))
        {
            EthApp_TxConfirmationInt(gEthApp.state[gCtrlIdx].ctrlIdx);
        }
    }
}

boolean EthApp_test_0610(void)
{
    uint32 iter = 0u;
    uint32 pktNum = 1500000u;

    EthUtils_printf("test_0610: START\r\n");

    /* Send START cmd */
    EthApp_sendCmd(gEthApp.state[gCtrlIdx].ctrlIdx, CTRL_FRAME_CMD_START);

    for(iter = 0; iter < pktNum; ++iter)
    {
        EthApp_test_0610_Helper(ETH_TEST_TYPE_PATTERN_1, 7u,
                ETH_TEST_VLAN_VID, ETHERTYPE_EXPERIMENTAL1);

        EthApp_test_0610_Helper(ETH_TEST_TYPE_PATTERN_1, 0u,
                ETH_TEST_VLAN_VID, ETHERTYPE_EXPERIMENTAL1);
    }

    /* Send STOP cmd */
    EthApp_sendCmd(gEthApp.state[gCtrlIdx].ctrlIdx, CTRL_FRAME_CMD_STOP);

    EthUtils_printf("test_0610: END\r\n");

    return PASS;
}

boolean EthApp_receiveTputQosHelper(uint8 ctrlIdx, uint32 num)
{
    boolean status = PASS;
    TickType t0 = 0U;
    TickType elapsed_tick = 0U;
    float32 elapsed = 0U;
    float32 pps = 0U;
    float32 mbps = 0U;
    uint8 i = 0U;
    uint32 rxframes = 0U;
    uint32 rxbytecnt = 0U;

    GetCounterValue(0, &t0);

    gEthApp.state[gCtrlIdx].runTest = TRUE;

    /* Wait for Rx frames */
    while (gEthApp.state[gCtrlIdx].runTest)
    {
        if(FALSE == Eth_GetRxEnableInterrupt(gCtrlIdx))
        {
            EthApp_receiveAllFifo(ctrlIdx);
        }
    }

    GetElapsedValue(0, &t0, &elapsed_tick);
    elapsed = (float32)elapsed_tick/(1000U*OS_TIME_SCALE_VALUE);

    for(i = 0u; i < ETH_PRIORITY_QUEUE_NUM; ++i)
    {
        if(gEthApp.state[gCtrlIdx].stats.qosPacketCnt[i] > 0u)
        {
            rxframes =  gEthApp.state[gCtrlIdx].stats.qosPacketCnt[i];
            rxbytecnt =  gEthApp.state[gCtrlIdx].stats.qosBytesCnt[i];

            /* Compute elapsed time, packets per second and Mbps */
            pps = (float32)rxframes / elapsed;
            mbps = (float32)rxbytecnt * 8u / elapsed / 1000000;

            EthUtils_printf("receiveQosTput-%d: received %d frames in %.2f secs (%.2f frames/s, %.2f Mbps)\r\n",
               i, rxframes, elapsed, pps, mbps);
        }
    }

    /* Send SYN ACK to notify Host that we received the STOP cmd */
    EthApp_notifyCmdRecv(gEthApp.state[gCtrlIdx].ctrlIdx);

    return status;
}

boolean EthApp_test_0611(void)
{
    uint32_t frames = 15000;
    bool status = PASS;

    gEthApp.state[gCtrlIdx].qosTest = TRUE;
    EthUtils_printf("test_0611: START\n");

    /* Wait for DUT to start the test when it's ready */
    EthApp_sendCmd(gEthApp.state[gCtrlIdx].ctrlIdx, CTRL_FRAME_CMD_START);

    /* Receive packets and measure the throughput */
    status = EthApp_receiveTputQosHelper(gEthApp.state[gCtrlIdx].ctrlIdx, frames);
    if (status == FAIL)
        EthUtils_printf("test_0611: failed to get receive throughput\n");

    EthUtils_printf("test_0611: END\n");

    /* Cleanup pending rx packet */
    EthApp_flushRecvQueue(gEthApp.state[gCtrlIdx].ctrlIdx);

    gEthApp.state[gCtrlIdx].qosTest = FALSE;

    return status;
}

#if (STD_ON == ETH_REGISTER_READBACK_API)
boolean EthApp_test_0700(void)
{
    bool status = PASS;
    Eth_RegisterReadbackType RegRb;
    Eth_RegisterReadbackType *RegRbPtr = &RegRb;

    if(Eth_RegisterReadback(gEthApp.state[gCtrlIdx].ctrlIdx, RegRbPtr) == E_OK)
    {
        EthUtils_printf("EthReadBackReg_ALE_CONTROL 0x%.8x\n",            RegRbPtr->EthReadBackReg_ALE_CONTROL);
        EthUtils_printf("EthReadBackReg_I0_ALE_PORTCTL0_PORT_0 0x%.8x\n", RegRbPtr->EthReadBackReg_I0_ALE_PORTCTL0_PORT_0);
        EthUtils_printf("EthReadBackReg_I0_ALE_PORTCTL0_PORT_N 0x%.8x\n", RegRbPtr->EthReadBackReg_I0_ALE_PORTCTL0_PORT_N);
        EthUtils_printf("EthReadBackReg_THREADMAPDEF 0x%.8x\n",           RegRbPtr->EthReadBackReg_THREADMAPDEF);
        EthUtils_printf("EthReadBackReg_ALE_UVLAN_MEMBER 0x%.8x\n",       RegRbPtr->EthReadBackReg_ALE_UVLAN_MEMBER);
        EthUtils_printf("EthReadBackReg_ALE_UVLAN_URCAST 0x%.8x\n",       RegRbPtr->EthReadBackReg_ALE_UVLAN_URCAST);
        EthUtils_printf("EthReadBackReg_ALE_UVLAN_RMCAST 0x%.8x\n",       RegRbPtr->EthReadBackReg_ALE_UVLAN_RMCAST);
        EthUtils_printf("EthReadBackReg_ALE_UVLAN_UNTAG 0x%.8x\n",        RegRbPtr->EthReadBackReg_ALE_UVLAN_UNTAG);
        EthUtils_printf("EthReadBackReg_CPTS_CONTROL 0x%.8x\n",           RegRbPtr->EthReadBackReg_CPTS_CONTROL);
        EthUtils_printf("EthReadBackReg_PN_MAC_CONTROL_REG 0x%.8x\n",     RegRbPtr->EthReadBackReg_PN_MAC_CONTROL_REG);
        EthUtils_printf("EthReadBackReg_PN_RX_MAXLEN_REG 0x%.8x\n",       RegRbPtr->EthReadBackReg_PN_RX_MAXLEN_REG);
        EthUtils_printf("EthReadBackReg_USER_INT_MASK_CLEAR_REG 0x%.8x\n",    RegRbPtr->EthReadBackReg_USER_INT_MASK_CLEAR_REG);
        EthUtils_printf("EthReadBackReg_USER_GROUP_USER_ACCESS_REG 0x%.8x\n", RegRbPtr->EthReadBackReg_USER_GROUP_USER_ACCESS_REG);
        EthUtils_printf("EthReadBackReg_POLL_EN_REG 0x%.8x\n",                RegRbPtr->EthReadBackReg_POLL_EN_REG);
        EthUtils_printf("EthReadBackReg_MDIO_CONTROL_REG 0x%.8x\n",           RegRbPtr->EthReadBackReg_MDIO_CONTROL_REG);
        EthUtils_printf("EthReadBackReg_PORT_CONTROL_REG 0x%.8x\n",           RegRbPtr->EthReadBackReg_PORT_CONTROL_REG);
        EthUtils_printf("EthReadBackReg_P0_CONTROL_REG 0x%.8x\n",             RegRbPtr->EthReadBackReg_P0_CONTROL_REG);
        EthUtils_printf("EthReadBackReg_PN_TS_CTL_REG 0x%.8x\n",              RegRbPtr->EthReadBackReg_PN_TS_CTL_REG);
        EthUtils_printf("EthReadBackReg_PN_TS_SEQ_LTYPE_REG 0x%.8x\n",        RegRbPtr->EthReadBackReg_PN_TS_SEQ_LTYPE_REG);
        EthUtils_printf("EthReadBackReg_PN_TS_VLAN_LTYPE_REG 0x%.8x\n",       RegRbPtr->EthReadBackReg_PN_TS_VLAN_LTYPE_REG);
        EthUtils_printf("EthReadBackReg_PN_TS_CTL_LTYPE2_REG 0x%.8x\n",       RegRbPtr->EthReadBackReg_PN_TS_CTL_LTYPE2_REG);
        EthUtils_printf("EthReadBackReg_PN_TS_CTL2_REG 0x%.8x\n",    RegRbPtr->EthReadBackReg_PN_TS_CTL2_REG);
        EthUtils_printf("EthReadBackReg_PN_PRI_CIR_REG[0] 0x%.8x\n", RegRbPtr->EthReadBackReg_PN_PRI_CIR_REG[0u]);
        EthUtils_printf("EthReadBackReg_PN_PRI_CIR_REG[1] 0x%.8x\n", RegRbPtr->EthReadBackReg_PN_PRI_CIR_REG[1U]);
        EthUtils_printf("EthReadBackReg_PN_PRI_CIR_REG[2] 0x%.8x\n", RegRbPtr->EthReadBackReg_PN_PRI_CIR_REG[2U]);
        EthUtils_printf("EthReadBackReg_PN_PRI_CIR_REG[3] 0x%.8x\n", RegRbPtr->EthReadBackReg_PN_PRI_CIR_REG[3U]);
        EthUtils_printf("EthReadBackReg_PN_PRI_CIR_REG[4] 0x%.8x\n", RegRbPtr->EthReadBackReg_PN_PRI_CIR_REG[4U]);
        EthUtils_printf("EthReadBackReg_PN_PRI_CIR_REG[5] 0x%.8x\n", RegRbPtr->EthReadBackReg_PN_PRI_CIR_REG[5U]);
        EthUtils_printf("EthReadBackReg_PN_PRI_CIR_REG[6] 0x%.8x\n", RegRbPtr->EthReadBackReg_PN_PRI_CIR_REG[6U]);
        EthUtils_printf("EthReadBackReg_PN_PRI_CIR_REG[7] 0x%.8x\n", RegRbPtr->EthReadBackReg_PN_PRI_CIR_REG[7U]);
        EthUtils_printf("EthReadBackReg_STAT_PORT_EN_REG 0x%.8x\n",  RegRbPtr->EthReadBackReg_STAT_PORT_EN_REG);
    }
    else
    {
        status = FAIL;
    }

    return status;
}
#endif /* (STD_ON == ETH_REGISTER_READBACK_API) */


#if (STD_ON == ETH_VIRTUALMAC_SUPPORT)
void EthVirtmacApp_rpcCmdComplete (uint8 controllerIdx,
                                   uint8 sid,
                                   sint32 status)
{
    /* nothing */
}

void EthVirtmacApp_rpcFwRegistered (uint8 controllerIdx)
{
    /* nothing */
}
#endif
