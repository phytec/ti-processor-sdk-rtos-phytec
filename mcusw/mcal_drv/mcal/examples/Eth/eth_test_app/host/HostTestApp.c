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
 *  \file     HostApp.c
 *
 *  \brief    This file contains the host-side Eth test application code.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>

#include "EthUtils.h"
#include "EthUtils_Host.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */


/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */
/* Unicast header with dst address of DUT's */
EthFrameHeader hdrUcastCtrl = {
    .dstMac    = { 0xf4, 0x84, 0x4c, 0xeb, 0x95, 0x09 },
    .srcMac    = { 0x00 }, /* to be populated later */
    .etherType = htons(ETHERTYPE_EXPERIMENTAL1),
};

/* Unicast header with dst address to be allowed by DUT's filter */
EthFrameHeader hdrUcastVal = {
    .dstMac   = { 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f },
    .srcMac   = { 0x00 }, /* to be populated later */
    .etherType = htons(ETHERTYPE_EXPERIMENTAL2),
};

/* Unicast header with dst address to be rejected by DUT's filter */
EthFrameHeader hdrUcastInv = {
    .dstMac   = { 0x08, 0x00, 0x28, 0x01, 0xf6, 0x7c },
    .srcMac   = { 0x00 }, /* to be populated later */
    .etherType = htons(ETHERTYPE_EXPERIMENTAL2),
};

/* Broadcast header */
EthFrameHeader hdrBcast = {
    .dstMac   = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06},
    .srcMac   = { 0x00 }, /* to be populated later */
    .etherType = htons(ETHERTYPE_EXPERIMENTAL1),
};

/* Multicast header */
EthFrameHeader hdrMcast = {
    .dstMac   = { 0x01, 0x00, 0x5e, 0x0d, 0x0e, 0x0f },
    .srcMac   = { 0x00 }, /* to be populated later */
    .etherType = htons(ETHERTYPE_EXPERIMENTAL2),
};

/* Test control (broadcast) header */
EthFrameHeader hdrCtrl = {
    .dstMac   = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06},
    .srcMac   = { 0x00 }, /* to be populated later */
    .etherType = htons(ETHERTYPE_EXP_CONTROL),
};


/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */

void help(const char *me);

/**
 * \brief Test basic frame reception
 *
 * Basic DUT frame reception test.  The host side performs these operations:
 * - Wait for START command from DUT
 * - Transmit ETH_TEST_ITER_M_COUNT frames
 *
 * \return PASS or FAIL  Test result
 */
bool HostApp_test_0001(void);

/**
 * \brief Test basic frame transmission
 *
 * Basic DUT frame transmission test.  The host side performs these operations:
 * - Wait for START command from DUT
 * - Receive frames until STOP command is detected
 *
 * The number of frames expected to be received is ETH_TEST_ITER_M_COUNT.
 *
 * \return PASS or FAIL  Test result
 */
bool HostApp_test_0002(void);

/**
 * \brief Test external loopback
 *
 * External loopback test.  The host side receives frames from the DUT and
 * sends them back.  The following operations are performed:
 * - Wait for START command from DUT
 * - Loop back all frames received until a STOP command is detected
 *   o One frame is received
 *   o Source and destination MAC addresses are swapped
 *   o The frame is sent back
 *
 * \return PASS or FAIL  Test result
 */
bool HostApp_test_0003(void);

/**
 * \brief Test default filter operation
 *
 * Test the operation of the default DUT's filter configuration (only frames with
 * DUT's MAC address are accepted, all others are rejected).  The host side performs
 * these operations:
 * - Wait for START command from DUT
 * - Transmit ETH_TEST_ITER_M_COUNT frames:
 *   o Frame 1: Source MAC address is DUT's
 *   o Frame 2: Source MAC address is not DUT's
 *
 * Frame 1 is expected to be accepted, frame 2 to be rejected.  The total number of
 * frames received on the DUT's driver is ETH_TEST_ITER_M_COUNT / 2.
 *
 * \return PASS or FAIL  Test result
 */
bool HostApp_test_0004(void);

/**
 * \brief Test different filter configurations
 *
 * Test the different modes of the DUT's filter configuration.  The host side performs
 * these operations:
 * - Wait for START command from DUT
 * - Part 1
 *    o <DUT resets filter>
 *    o Transmit ETH_TEST_ITER_M_COUNT frames:
 *      - Frame 1: Source MAC address is DUT's. It should be accepted
 *      - Frame 2: Source MAC address is not DUT's. It should be rejected
 * - Part 2
 *   o <DUT adds a second valid MAC address>
 *   o Transmit ETH_TEST_ITER_M_COUNT frames:
 *     - Frame 1: Source MAC address is DUT's. It should be accepted
 *     - Frame 2: Source MAC address is second valid MAC address. It should be accepted
 * - Part 3
 *   o <DUT adds a multicast MAC address>
 *   o Transmit ETH_TEST_ITER_M_COUNT frames:
 *     - Frame 1: Source MAC address is DUT's. It should be accepted
 *     - Frame 2: Source MAC address is multicast address. It should be accepted
 * - Part 4
 *   o <DUT removes the multicast MAC address>
 *   o Transmit ETH_TEST_ITER_M_COUNT frames:
 *     - Frame 1: Source MAC address is DUT's. It should be accepted
 *     - Frame 2: Source MAC address is multicast address. It should be rejected
 * - Part 5
 *   o <DUT opens up the filter (promiscuous mode)>
 *   o Transmit ETH_TEST_ITER_M_COUNT frames:
 *     - Frame 1: Source MAC address is DUT's. It should be accepted
 *     - Frame 2: Source MAC address is not DUT's. It should be accepted
 * - Part 6
 *   o <DUT resets filter>
 *   o Transmit ETH_TEST_ITER_M_COUNT frames:
 *     - Frame 1: Source MAC address is DUT's. It should be accepted
 *     - Frame 2: Source MAC address is not DUT's. It should be rejected
 *
 * \return PASS or FAIL  Test result
 */
bool HostApp_test_0005(void);


/**
 * \brief Test bypass configurations
 *
 * Test the different modes of the DUT's filter configuration.  The host side performs
 * these operations:
 * - Wait for START command from DUT
 *   o <DUT opens up the filter (promiscuous mode)>
 *   o Transmit ETH_TEST_ITER_M_COUNT frames:
 *     - Frame 1: Source MAC address is DUT's. It should be accepted
 *     - Frame 2: Source MAC address is not DUT's. It should be accepted
 * \return PASS or FAIL  Test result
 */
bool HostApp_test_0005_1(void);
/**
 * \brief Test frame transmission with confirmation
 *
 * DUT frame transmission test with TX confirmation.  The host side performs
 * these operations:
 * - Wait for START command from DUT
 * - Receive frames until STOP command is detected
 *
 * The number of frames expected to be received is ETH_TEST_ITER_M_COUNT.
 * This test is exactly the same as test_0002 on the host side.
 *
 * \return PASS or FAIL  Test result
 */
bool HostApp_test_0006(void);

/**
 * \brief Test frame transmission of VLAN tagged frames
 *
 * DUT frame transmission test of VLAN tagged packets.  The host side performs
 * these operations:
 * - Wait for START command from DUT
 * - Receive frames until STOP command is detected
 *
 * The number of frames expected to be received is ETH_TEST_ITER_M_COUNT.
 * This test is exactly the same as test_0002 on the host side.
 *
 * \return PASS or FAIL  Test result
 */
bool HostApp_test_0007(void);

/**
 * \brief Test frame transmission of different lengths
 *
 * DUT frame transmission test of different frame lengths, including short frames
 * (less than 64 octets).  The host side performs these operations:
 * - Wait for START command from DUT
 * - Receive frames until STOP command is detected. It's expected to receive
 *   ETH_TEST_ITER_S_COUNT packets of different lengths, starting at 10 octets
 *   in increments of 10 octets until 1500 octets.
 *
 * \return PASS or FAIL  Test result
 */
bool HostApp_test_0008(void);

/**
 * \brief Test VLAN tagged frame reception
 *
 * DUT VLAN tagged frame reception test.  The host side performs these operations:
 * - Wait for START command from DUT
 * - Transmit ETH_TEST_ITER_M_COUNT frames with VLAN tag
 *
 * \return PASS or FAIL  Test result
 */
bool HostApp_test_0009(void);

/**
 * \brief Test controller mode change
 *
 * Controller mode change test.  The host performs the following operations:
 * - Wait for START command from DUT
 * - Run ETH_TEST_ITER_S_COUNT iterations of:
 *   o <DUT sets controller mode to DOWN>
 *   o <DUT sets controller mode to ACTIVE>
 *   o Receive frames (run test 0002)
 *   o Transmit frames (run test 0001)
 *
 * \return PASS or FAIL  Test result
 */
bool HostApp_test_0010(void);

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
bool HostApp_test_0011(void);

/**
 * \brief Measure DUT transmit throughput
 *
 * Measure DUT transmit throughput over frames number and packet lenght is size.
 * The host performs the following operations:
 * - Wait for START command from DUT
 * - Receive frames or until the STOP cmd is detected
 * - Compute the elapsed time, packets per second and Mbps
 *
 * \return PASS or FAIL  Test result
 */
bool HostApp_test_0100(uint32_t frames, uint32_t size);

/**
 * \brief Measure DUT transmit throughput
 *
 * Measure DUT transmit throughput over ETH_TEST_ITER_L_COUNT with size are: 1024,521,128,64 frames.
 * The host performs the following operations:
 * - call HostApp_test_0100 for each frame size
 *
 * \return PASS or FAIL  Test result
 */
bool HostApp_test_0110(void);

/* Measure Rx through*/
bool HostApp_test_0101(void);

/* Egress timestamp test */
bool HostApp_test_0300(void);

/* Ingress timestamp test */
bool HostApp_test_0301(void);

/* Get current time test */
bool HostApp_test_0302(void);

/* Traffic shaping test */
bool HostApp_test_0400(void);

/* Rx QOS test */
bool HostApp_test_0610(void);

/* Tx QOS test */
bool HostApp_test_0611(void);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

TestFunc testFuncs[] = {
    &HostApp_test_0001,
    &HostApp_test_0002,
    &HostApp_test_0003,
    &HostApp_test_0004,
#if (STD_ON == ETH_UPDATE_PHYS_ADDR_FILTER_API)
    &HostApp_test_0005,
#endif /* (STD_ON == ETH_UPDATE_PHYS_ADDR_FILTER_API) */
    &HostApp_test_0006,
    &HostApp_test_0007,
    &HostApp_test_0008,
    &HostApp_test_0009,
    &HostApp_test_0010,
    &HostApp_test_0011,
    &HostApp_test_0300,
    &HostApp_test_0301,
    &HostApp_test_0302,
    &HostApp_test_0110,
    &HostApp_test_0101,
#if (STD_ON == ETH_TRAFFIC_SHAPING_API)
    &HostApp_test_0400,
#endif /* (STD_ON == ETH_TRAFFIC_SHAPING_API) */
    &HostApp_test_0610,
    &HostApp_test_0611,
#if (STD_ON == ETH_UPDATE_PHYS_ADDR_FILTER_API)
    &HostApp_test_0005_1
#endif
};

HostApp gHostApp = {
    .dutAddr = { 0xf4, 0x84, 0x4c, 0xeb, 0x95, 0x09 },
};

static const struct option long_options[] = {
    {"timeout",      1, 0, 't'},      /* time-out */
    {"iface",        1, 0, 'i'},      /* interface number */
    {"select_test",  1, 0, 's'},      /* select test case */
    {"verbose",      0, 0, 'v'},      /* verbose output */
    {"very-verbose", 0, 0, 'V'},      /* very verbose output */
    {"help",         0, 0, 'h'},      /* show help */
};

static const char short_options[] = "t:p:i:s:vh";

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

int main(int argc, char *argv[])
{
    struct timeval timeout;
    const char *action;
    const char *me = argv[0];
    char *ifName = "eth0";
    bool status;
    bool globalStatus = true;
    long int val;
    int32_t option;
    int32_t c;
    int32_t i;
    int32_t ret;
    int32_t ti;

    memset(&gHostApp, 0, sizeof(gHostApp)); 
    gHostApp.verbose = false;
    gHostApp.veryVerbose = false;
    gHostApp.timeout = 120;
    gHostApp.testIdx = 0xFFu;

    while (1) {
        c = getopt_long(argc, argv, short_options, long_options, &option);
        if (c == -1)
            break;

        switch (c) {
        case 't':
            gHostApp.timeout = strtol(optarg, NULL, 0);
            break;
        case 'i':
            ifName = optarg;
            break;
        case 's':
            gHostApp.testIdx = (uint8_t)strtol(optarg, NULL, 0);
            if(gHostApp.testIdx >= ARRAY_SIZE(testFuncs)) {
                printf("Invalid test Id, should be in range [%d,%lu]\n", 0, ARRAY_SIZE(testFuncs)-1);
                return -EINVAL;
            }
            break;
        case 'v':
            gHostApp.verbose = true;
            break;
        case 'V':
            gHostApp.verbose = true;
            gHostApp.veryVerbose = true;
            break;
        case 'h':
            help(me);
            return 0;
        default:
            printf("Invalid option %d\n", option);
            return -EINVAL;
        }
    }

    for(ti = 0u; ti < ETH_APP_TEST_ROUND; ti++)
    {
        ret = HostApp_open(ifName);

        if (ret) {
            printf("Failed to open and init raw socket: %d\n", ret);
            return ret;
        }

        /* Update test packet headers with host's MAC address */
        memcpy(hdrUcastCtrl.srcMac, gHostApp.hwAddr, ETH_HW_ADDR_LEN);
        memcpy(hdrUcastVal.srcMac, gHostApp.hwAddr, ETH_HW_ADDR_LEN);
        memcpy(hdrUcastInv.srcMac, gHostApp.hwAddr, ETH_HW_ADDR_LEN);
        memcpy(hdrBcast.srcMac, gHostApp.hwAddr, ETH_HW_ADDR_LEN);
        memcpy(hdrMcast.srcMac, gHostApp.hwAddr, ETH_HW_ADDR_LEN);
        memcpy(hdrCtrl.srcMac, gHostApp.hwAddr, ETH_HW_ADDR_LEN);
        memcpy(&gHostApp.ctrlFrame.hdr, &hdrCtrl, ETH_HDR_LEN);

        /* DUT <-> Host Initial Handshake */
        HostApp_waitForDUT();

        memcpy(hdrUcastCtrl.dstMac, gHostApp.dutAddr, ETH_HW_ADDR_LEN);

        for (i = 0; i < ARRAY_SIZE(testFuncs); i++) {
            if((gHostApp.testIdx < ARRAY_SIZE(testFuncs)) && (i != gHostApp.testIdx)) {
                continue;
            }

            printf("\n\n-----------------------------------------------------------\n");
            status = testFuncs[i]();
            globalStatus &= status;
            printf("Test Result: %s\n", status?"Pass":"Fail");
            printf("-----------------------------------------------------------\n");
        }

        HostApp_close();
    }

    printf("\nFinal Tests Results: %s\n\n", globalStatus?"Pass":"Fail");

    return 0;
}

void help(const char *me)
{
    printf("Usage: %s [OPTION]...\n"
           "-h, --help              help\n"
           "-i, --iface=<NAME>      interface name (i.e. eth0)\n"
           "-t, --timeout=#         socket time-out (in secs)\n"
           "-v, --verbose           verbose output\n\n",
           me);
}

bool HostApp_test_0001(void)
{
    uint32_t iterations = ETH_TEST_ITER_M_COUNT;
    TestFrame frames[1];
    DataFramePayload *payload;
    uint16_t len;
    uint32_t i;
    int32_t ret;
    bool status = PASS;

    printf("test_0001: START\n");

    /* Unicast packet with DUT's MAC address */
    frames[0].len = 100;
    payload = (DataFramePayload *)frames[0].frame.payload;
    memcpy(&frames[0].frame.hdr, &hdrUcastCtrl, ETH_HDR_LEN);

    /* Wait for DUT to start the test when it's ready */
    HostApp_waitForCmd(CTRL_FRAME_CMD_START);

    /* Transmit one frame per iteration */
    for (i = 0; i < iterations; i++) {
        if (gHostApp.verbose)
            printf("test_0001: iteration: %d of %d\n", i+1, iterations);

        /* Unicast frame with DUT's MAC address */
        len = frames[0].len - ETH_HDR_LEN;
        ret = EthUtils_fillPayload(payload, i % ETH_TEST_NUM_TYPES, len);
        if (ret) {
            printf("test_0001: failed to create test packet: %d\n", ret);
            status = FAIL;
            break;
        }

        /* Transmit one frame */
        HostApp_transmit(frames, ARRAY_SIZE(frames), 1);
    }

    /* Indicate to the DUT that the test is complete */
    HostApp_sendCmd(CTRL_FRAME_CMD_STOP);

    printf("test_0001: transmitted %d of %d frames\n", i, iterations);
    printf("test_0001: END\n");

    return status;
}

bool HostApp_test_0002(void)
{
    uint32_t recvNum = 0;
    bool status = PASS;
    bool st;
    int i;

    printf("test_0002: START\n");

    for(i = 0; i < 2; ++i) {
        /* Wait for DUT to start the test when it's ready */
        HostApp_waitForCmd(CTRL_FRAME_CMD_START);

        /* Receive packets indefinitely until STOP cmd is detected */
        st = HostApp_receive(&recvNum, false);
        if (st == FAIL) {
            printf("test_0002: failed while receiving packets %d\n", recvNum);
            status = FAIL;
        }

        /* Check that all packets were received */
        if (recvNum != ETH_TEST_ITER_M_COUNT) {
            printf("test_0002: received frame count mismatch (exp=%d, got=%d)\n",
                   ETH_TEST_ITER_M_COUNT, recvNum);
            status = FAIL;
        }

        printf("test_0002: received %d of %d frames\n", recvNum, ETH_TEST_ITER_M_COUNT);
    }

    printf("test_0002: END\n");

    return status;
}

bool HostApp_test_0003(void)
{
    uint32_t len;
    uint32_t cnt = 0;
    bool status;

    printf("test_0003: START\n");

    /* Wait for DUT to start the test when it's ready */
    HostApp_waitForCmd(CTRL_FRAME_CMD_START);

    /* Loop frames back to the DUT until STOP cmd is received */
    while (true) {
        if (gHostApp.verbose)
            printf("test_0003: iteration %d\n", cnt+1);

        /* Receive one frame */
        len = sizeof(gHostApp.rxFrame);
        status = HostApp_recv(&gHostApp.rxFrame, &len);
        if (status == FAIL) {
            printf("test_0003: failed to receive frame\n");
            HostApp_sendCmd(CTRL_FRAME_CMD_STOP);
            break;
        }

        if (gHostApp.verbose) {
            printf("test_0003: frame received (%d bytes)\n", len);
            if (gHostApp.veryVerbose)
                EthUtils_printFrame(&gHostApp.rxFrame, HOST_APP_VERBOSE_OCTETS);
        }

        /* End the test if STOP cmd is received */
        if (EthFrame_isStopCmd(&gHostApp.rxFrame))
            break;

        /* Swap src and dst MAC addresses */
        memcpy(gHostApp.rxFrame.hdr.dstMac, gHostApp.rxFrame.hdr.srcMac, ETH_HW_ADDR_LEN);
        memcpy(gHostApp.rxFrame.hdr.srcMac, gHostApp.hwAddr, ETH_HW_ADDR_LEN);

        if (gHostApp.verbose) {
            printf("test_0003: frame to be sent (%d bytes)\n", len);
            if (gHostApp.veryVerbose)
                EthUtils_printFrame(&gHostApp.rxFrame, HOST_APP_VERBOSE_OCTETS);
        }

        /* Send the frame back */
        HostApp_send(&gHostApp.rxFrame, len);
        cnt++;

        if (gHostApp.verbose)
            printf("test_0003: iteration %d complete\n", cnt);
    }

    printf("test_0003: looped back %d frames\n", cnt);
    printf("test_0003: END\n");

    return status;
}

bool HostApp_test_0004(void)
{
    uint32_t iterations = ETH_TEST_ITER_M_COUNT / 2;
    TestFrame frames[2];
    DataFramePayload *payload;
    uint16_t len;
    uint32_t i;
    int32_t ret;
    bool status = PASS;

    printf("test_0004: START\n");

    /* Unicast packet with DUT's MAC address */
    frames[0].len = 100;
    memcpy(&frames[0].frame.hdr, &hdrUcastCtrl, ETH_HDR_LEN);

    /* Unicast packet with MAC address to be rejected by DUT's filter */
    frames[1].len = 200;
    memcpy(&frames[1].frame.hdr, &hdrUcastInv, ETH_HDR_LEN);

    /* Wait for DUT to start the test when it's ready */
    HostApp_waitForCmd(CTRL_FRAME_CMD_START);

    /* Transmit two frames per iteration */
    for (i = 0; i < iterations; i++) {
        if (gHostApp.verbose)
            printf("test_0004: iteration: %d of %d\n", i+1, iterations);

        /* Unicast packet with DUT's MAC address */
        payload = (DataFramePayload *)frames[0].frame.payload;
        len = frames[0].len - ETH_HDR_LEN;
        ret = EthUtils_fillPayload(payload, i % ETH_TEST_NUM_TYPES, len);
        if (ret) {
            printf("test_0004: failed to create test packet: %d\n", ret);
            status = FAIL;
            break;
        }

        /* Unicast packet with MAC address to be rejected by DUT's filter */
        payload = (DataFramePayload *)frames[1].frame.payload;
        len = frames[1].len - ETH_HDR_LEN;
        ret = EthUtils_fillPayload(payload, i % ETH_TEST_NUM_TYPES, len);
        if (ret) {
            printf("test_0004: failed to create test packet: %d\n", ret);
            status = FAIL;
            break;
        }

        /* Transmit the two packets */
        HostApp_transmit(frames, ARRAY_SIZE(frames), ARRAY_SIZE(frames));
    }

    /* Indicate to the DUT that the test is complete */
    HostApp_sendCmd(CTRL_FRAME_CMD_STOP);

    printf("test_0004: completed %d of %d iterations\n", i, iterations);
    printf("test_0004: END\n");

    return status;
}

bool HostApp_test_0005(void)
{
    TestFrame frames[2];
    DataFrame *frame;
    uint32_t num = ETH_TEST_ITER_M_COUNT;
    uint16_t len;
    bool status = PASS;

    printf("test_0005: START\n");

    len = frames[0].len = 1500;
    frame = (DataFrame *)&frames[0].frame;
    memcpy(&frame->hdr, &hdrUcastCtrl, ETH_HDR_LEN);
    EthUtils_fillPayload(&frame->payload,
                         ETH_TEST_TYPE_PATTERN_1,
                         len - ETH_HDR_LEN);

    len = frames[1].len = 1500;
    frame = (DataFrame *)&frames[1].frame;

    /* Part 1: DUT's filter has been reset. Only packets with DUT's address
     *         should be accepted */
    memcpy(&frame->hdr, &hdrUcastInv, ETH_HDR_LEN);
    EthUtils_fillPayload(&frame->payload,
                         ETH_TEST_TYPE_PATTERN_2,
                         len - ETH_HDR_LEN);

    HostApp_waitForCmd(CTRL_FRAME_CMD_START);
    HostApp_transmit(frames, ARRAY_SIZE(frames), num);
    HostApp_sendCmd(CTRL_FRAME_CMD_STOP);

    /* Part 2: An unicast address has been added to the DUT's filter. Packets
     *         with new address as well as DUT's address should be accepted */
    memcpy(&frame->hdr, &hdrUcastVal, ETH_HDR_LEN);
    EthUtils_fillPayload(&frame->payload,
                         ETH_TEST_TYPE_PATTERN_3,
                         len - ETH_HDR_LEN);

    HostApp_waitForCmd(CTRL_FRAME_CMD_START);
    HostApp_transmit(frames, ARRAY_SIZE(frames), num);
    HostApp_sendCmd(CTRL_FRAME_CMD_STOP);

    /* Part 3: A multicast address has been added to the DUT's filter. Packets
     *         with the multicast address as well as DUT's address should be
     *         accepted */
    memcpy(&frame->hdr, &hdrMcast, ETH_HDR_LEN);
    EthUtils_fillPayload(&frame->payload,
                         ETH_TEST_TYPE_PATTERN_2,
                         len - ETH_HDR_LEN);

    HostApp_waitForCmd(CTRL_FRAME_CMD_START);
    HostApp_transmit(frames, ARRAY_SIZE(frames), num);
    HostApp_sendCmd(CTRL_FRAME_CMD_STOP);

    /* Part 4: The multicast address has been removed from the DUT's filter.
     *         Only packets with DUT's address should be accepted */
    memcpy(&frame->hdr, &hdrMcast, ETH_HDR_LEN);
    EthUtils_fillPayload(&frame->payload,
                         ETH_TEST_TYPE_PATTERN_3,
                         len - ETH_HDR_LEN);

    HostApp_waitForCmd(CTRL_FRAME_CMD_START);
    HostApp_transmit(frames, ARRAY_SIZE(frames), num);
    HostApp_sendCmd(CTRL_FRAME_CMD_STOP);

    /* Part 6: DUT's filter has been reset. Only packets with DUT's address
     *         should be accepted */
    memcpy(&frame->hdr, &hdrUcastInv, ETH_HDR_LEN);
    EthUtils_fillPayload(&frame->payload,
                         ETH_TEST_TYPE_PATTERN_4,
                         len - ETH_HDR_LEN);

    HostApp_waitForCmd(CTRL_FRAME_CMD_START);
    HostApp_transmit(frames, ARRAY_SIZE(frames), num);
    HostApp_sendCmd(CTRL_FRAME_CMD_STOP);

    printf("test_0005: END\n");

    return PASS;
}

bool HostApp_test_0005_1(void)
{
    TestFrame frames[2];
    DataFrame *frame;
    uint32_t num = ETH_TEST_ITER_M_COUNT;
    uint16_t len;
    bool status = PASS;

    printf("test_0005_1: START\n");

    len = frames[0].len = 1500;
    frame = (DataFrame *)&frames[0].frame;
    memcpy(&frame->hdr, &hdrUcastCtrl, ETH_HDR_LEN);
    EthUtils_fillPayload(&frame->payload,
                         ETH_TEST_TYPE_PATTERN_1,
                         len - ETH_HDR_LEN);

    len = frames[1].len = 1500;
    frame = (DataFrame *)&frames[1].frame;


    /* Part 5: DUT's filter is open, all packets should be accepted */
    memcpy(&frame->hdr, &hdrUcastInv, ETH_HDR_LEN);
    EthUtils_fillPayload(&frame->payload,
                         ETH_TEST_TYPE_PATTERN_2,
                         len - ETH_HDR_LEN);

    HostApp_waitForCmd(CTRL_FRAME_CMD_START);
    HostApp_transmit(frames, ARRAY_SIZE(frames), num);
    HostApp_sendCmd(CTRL_FRAME_CMD_STOP);

    printf("test_0005_1: END\n");

    return PASS;
}

bool HostApp_test_0006(void)
{
    uint32_t recvNum = 0;
    bool status;

    printf("test_0006: START\n");

    /* Wait for DUT to start the test when it's ready */
    HostApp_waitForCmd(CTRL_FRAME_CMD_START);

    /* Receive packets indefinitely until STOP cmd is detected */
    status = HostApp_receive(&recvNum, false);
    if (status == FAIL)
        printf("test_0006: failed while receiving packets\n");

    /* Check that all packets were received */
    if (recvNum != ETH_TEST_ITER_M_COUNT) {
        printf("test_0006: received frame count mismatch (exp=%d, got=%d)\n",
               ETH_TEST_ITER_M_COUNT, recvNum);
        status = FAIL;
    }

    printf("test_0006: received %d of %d frames\n", recvNum, ETH_TEST_ITER_M_COUNT);
    printf("test_0006: END\n");

    return status;
}

bool HostApp_test_0007(void)
{
    uint32_t recvNum = 0;
    bool st;
    bool status = PASS;

    printf("test_0007: START\n");

    /* Wait for DUT to start the test when it's ready */
    HostApp_waitForCmd(CTRL_FRAME_CMD_START);

    /* Receive packets indefinitely until STOP cmd is detected */
    st = HostApp_receive(&recvNum, false);
    if (st == FAIL) {
        printf("test_0007: failed while receiving packets\n");
        status = FAIL;
    }

    /* Check that all packets were received */
    if (recvNum != ETH_TEST_ITER_M_COUNT) {
        printf("test_0007: received frame count mismatch (exp=%d, got=%d)\n",
               ETH_TEST_ITER_M_COUNT, recvNum);
        status = FAIL;
    }

    printf("test_0007: received %d of %d frames\n", recvNum, ETH_TEST_ITER_M_COUNT);
    printf("test_0007: END\n");

    return status;
}

bool HostApp_test_0008(void)
{
    uint32_t recvNum;
    uint16_t len;
    bool st;
    bool status = PASS;

    printf("test_0008: START\n");

    /* Wait for DUT to start the test when it's ready */
    HostApp_waitForCmd(CTRL_FRAME_CMD_START);

    /* Receive packets indefinitely until STOP cmd is detected */
    for (len = 10; len <= 1500; len += 10) {
        recvNum = 0;
        st = HostApp_receive(&recvNum, false);
        if (st == FAIL) {
            printf("test_0008: failed while receiving packets\n");
            status = FAIL;
        }

        /* Check that all packets were received */
        if (recvNum != ETH_TEST_ITER_S_COUNT) {
            printf("test_0008: received frame count mismatch (exp=%d, got=%d)\n",
                   ETH_TEST_ITER_S_COUNT, recvNum);
            status = FAIL;
        }
    }

    printf("test_0008: END\n");

    return status;
}

bool HostApp_test_0009(void)
{
    uint32_t iterations = ETH_TEST_ITER_M_COUNT;
    TestFrame frames[1];
    VlanDataFramePayload *payload;
    uint16_t len;
    uint32_t i;
    int32_t ret;
    bool status = PASS;

    printf("test_0009: START\n");

    /* Unicast packet with DUT's MAC address */
    frames[0].len = 100;
    payload = (VlanDataFramePayload *)frames[0].frame.payload;
    memcpy(&frames[0].frame.hdr, &hdrUcastCtrl, ETH_HDR_LEN);
    frames[0].frame.hdr.etherType = htons(ETHERTYPE_VLAN_TAG);

    /* Wait for DUT to start the test when it's ready */
    HostApp_waitForCmd(CTRL_FRAME_CMD_START);

    /* Transmit one frame per iteration */
    for (i = 0; i < iterations; i++) {
        if (gHostApp.verbose)
            printf("test_0009: iteration: %d of %d\n", i+1, iterations);

        /* Unicast frame with DUT's MAC address */
        len = frames[0].len - ETH_HDR_LEN;
        ret = EthUtils_fillVlanPayload(payload,
                                       i % ETH_TEST_NUM_TYPES,
                                       len,
                                       ETH_TEST_VLAN_PCP,
                                       ETH_TEST_VLAN_VID,
                                       ETHERTYPE_EXPERIMENTAL1);
        if (ret) {
            printf("test_0009: failed to create test packet: %d\n", ret);
            status = FAIL;
            break;
        }

        /* Transmit one frame */
        HostApp_transmit(frames, ARRAY_SIZE(frames), 1);
    }

    /* Indicate to the DUT that the test is complete */
    HostApp_sendCmd(CTRL_FRAME_CMD_STOP);

    printf("test_0009: transmitted %d of %d frames\n", i, iterations);
    printf("test_0009: END\n");

    return status;
}

bool HostApp_test_0010(void)
{
    uint32_t iterations = ETH_TEST_ITER_S_COUNT;
    uint32_t i;
    bool status = PASS;
    bool st;

    printf("test_0010: START\n");

    /* Wait for DUT to start the test when it's ready */
    HostApp_waitForCmd(CTRL_FRAME_CMD_START);

    /* Transmit and receive packets after DUT's mode change */
    for (i = 0; i < iterations; i++) {
        if (gHostApp.verbose)
            printf("test_0010: iteration: %d\n", i+1);

        /* Receive packets */
        st = HostApp_test_0002();
        if (st == FAIL) {
            printf("test_0010: failed to receive packets\n");
            status = FAIL;
        }

        /* Transmit packets */
        st = HostApp_test_0001();
        if (st == FAIL) {
            printf("test_0010: failed to transmit packets\n");
            status = FAIL;
        }
    }

    printf("test_0010: completed %d of %d iterations\n", i, iterations);
    printf("test_0010: END\n");

    return status;
}

bool HostApp_test_0011(void)
{
    uint32_t recvNum = 0;
    bool st;
    bool status = PASS;

    printf("test_0011: START\n");

    /* Wait for DUT to start the test when it's ready */
    HostApp_waitForCmd(CTRL_FRAME_CMD_START);

    /* Receive packets indefinitely until STOP cmd is detected */
    st = HostApp_receive(&recvNum, false);
    if (st == FAIL) {
        printf("test_0011: failed while receiving packets\n");
        status = FAIL;
    }

    /* Check that all packets were received */
    if (recvNum != ETH_TEST_ITER_M_COUNT) {
        printf("test_0011: received frame count mismatch (exp=%d, got=%d)\n",
               ETH_TEST_ITER_M_COUNT, recvNum);
        status = FAIL;
    }

    printf("test_0011: received %d of %d frames\n", recvNum, ETH_TEST_ITER_M_COUNT);
    printf("test_0011: END\n");

    return status;
}

bool HostApp_test_0100(uint32_t frames, uint32_t size)
{
    bool status;

    /* Wait for DUT to start the test when it's ready */
    HostApp_waitForCmd(CTRL_FRAME_CMD_START);

    /* Receive packets and measure the throughput */
    status = HostApp_receiveTput(frames, size);
    if (status == FAIL)
        printf("test_0100: failed to get receive throughput\n");

    return status;
}

bool HostApp_test_0110(void)
{
    uint16_t txPktSizeMesure[] = { 1500u,   512u,    256u,    128u,    64u };
    uint32_t iterator_num[]    = { 150000u, 150000u, 150000u, 150000u, 150000u };
    bool status;
    uint32_t si;

    printf("test_0110: START\n");

    for(si = 0; si< sizeof(txPktSizeMesure)/sizeof(unsigned short); ++si)
    {
        status = HostApp_test_0100(iterator_num[si], txPktSizeMesure[si]);
    }

    printf("test_0110: DONE\n");

    return status;
}


bool HostApp_test_0101(void)
{
    uint32_t iterations = ETH_TEST_ITER_L_COUNT;
    uint16_t rxPktSizeMesure[] = { 1500u,   512u,    256u,    128u,    64u     };
    uint32_t iterator_num[]    = { 150000u, 150000u, 150000u, 300000u, 300000u };
    uint32_t delay[]           = { 0u,      0u,      40u,     40u,     30u     };
    TestFrame frames[1];
    DataFramePayload *payload;
    uint32_t i, si;
    int32_t ret;
    bool status = PASS;
    struct timeval t0, t1;
    float elapsed;
    float pps;
    float mbps;
    long long total = 0;
    ssize_t bytes;

    EthUtils_printf("test_0101: START\n");

    for(si = 0; si< sizeof(rxPktSizeMesure)/sizeof(unsigned short); ++si)
    {
        /* Unicast packet with DUT's MAC address */
        frames[0].len = rxPktSizeMesure[si];
        payload = (DataFramePayload *)frames[0].frame.payload;
        memcpy(&frames[0].frame.hdr, &hdrUcastCtrl, ETH_HDR_LEN);
        frames[0].frame.hdr.etherType = htons(ETHERTYPE_EXPERIMENTAL3);

        iterations = iterator_num[si];

        /* Wait for DUT to start the test when it's ready */
        HostApp_waitForCmd(CTRL_FRAME_CMD_START);

        gettimeofday(&t0, NULL);

        /* Transmit one frame per iteration */
        for (i = 0; i < iterations; i++) {
            if (gHostApp.verbose)
                EthUtils_printf("test_0101: iteration: %d of %d\n", i+1, iterations);

            /* Transmit one frame */
            HostApp_send(&frames[0].frame, frames[0].len);
            if((delay[si] > 0u) && (i % delay[si] == 0))
            usleep(1);
        }

        /* Get the "end" timestamp after DUT is done sending frames */
        gettimeofday(&t1, NULL);

        /* Send STOP command and wait for acknowledgement from DUT
         * - Send multiple times (ETH_TEST_ITER_S_COUNT) to account for
         *   possible packet drop
         * - This is OK since DUT will flush the receive queue before
         *   proceeding to the next test case
         */
        HostApp_sendCmdWithAck(CTRL_FRAME_CMD_STOP, ETH_TEST_ITER_S_COUNT);

        /* Compute elapsed time, packets per second and Mbps */
        elapsed = HostApp_timeDiff(t1, t0);
        pps = (float)i / elapsed;
        mbps = (float)i*frames[0].len * 8 / elapsed / 1000000;

        EthUtils_printf("transmitTput: TX side transmitted %d frames in %.2f secs (%.2f frames/s, %.2f Mbps)\n",
                        i, elapsed, pps, mbps);
    }

    printf("test_0101: END\n");

    return status;
}

bool HostApp_test_0300(void)
{
    uint32_t i,iterations = ETH_TEST_ITER_S_COUNT;
    bool status = PASS;
    uint32_t recvNum;
    bool st;

    EthUtils_printf("test_0300: START\r\n");

    HostApp_waitForCmd(CTRL_FRAME_CMD_START);

    /* Receive packets indefinitely until STOP cmd is detected */
    st = HostApp_receive(&recvNum, false);
    if (st == FAIL) {
        printf("test_0300: failed while receiving packets\n");
        status = FAIL;
    }

    /* Check that all packets were received */
    if (recvNum != ETH_TEST_ITER_S_COUNT) {
        printf("test_0300: received frame count mismatch (exp=%d, got=%d)\n",
               ETH_TEST_ITER_S_COUNT, recvNum);
        status = FAIL;
    }

    EthUtils_printf("test_0300: END\r\n");

    return status;
}

bool HostApp_test_0301(void)
{
    uint32_t iterations = ETH_TEST_ITER_S_COUNT;
    uint32_t delays_usec[ETH_TEST_ITER_S_COUNT] = {
           31250u,   62500u,  125000u,  250000u,  500000u,
          750000u, 1000000u, 2000000u, 5000000u, 1000000u
    };
    TestFrame frames[1];
    DataFramePayload *payload;
    uint16_t len;
    uint32_t i;
    int32_t ret;
    bool status = PASS;
    bool st;

    printf("test_0301: START\n");

    /* Unicast packet with DUT's MAC address */
    frames[0].len = 100;
    payload = (DataFramePayload *)frames[0].frame.payload;
    memcpy(&frames[0].frame.hdr, &hdrUcastCtrl, ETH_HDR_LEN);

    /* Wait for DUT to start the test when it's ready */
    HostApp_waitForCmd(CTRL_FRAME_CMD_START);

    /* Transmit one frame per iteration */
    for (i = 0; i < iterations; i++) {
        if (gHostApp.verbose)
            printf("test_0301: iteration: %d of %d\n", i+1, iterations);

        /* Unicast frame with DUT's MAC address */
        len = frames[0].len - ETH_HDR_LEN;
        ret = EthUtils_fillPayload(payload, i % ETH_TEST_NUM_TYPES, len);
        if (ret) {
            printf("test_0301: failed to create test packet: %d\n", ret);
            status = FAIL;
            break;
        }

        /* Transmit one frame */
        HostApp_transmit(frames, ARRAY_SIZE(frames), 1);
        usleep(delays_usec[i]);
    }

    /* Indicate to the DUT that the test is complete */
    HostApp_sendCmd(CTRL_FRAME_CMD_STOP);

    printf("test_0301: transmitted %d of %d frames\n", i, iterations);
    printf("test_0301: END\n");
}

bool HostApp_test_0302(void)
{
    uint32_t i,iterations = ETH_TEST_ITER_S_COUNT;
    bool status = PASS;
    uint32_t recvNum;
    bool st;

    EthUtils_printf("test_0302: START\r\n");

    HostApp_waitForCmd(CTRL_FRAME_CMD_START);

    /* Receive packets indefinitely until STOP cmd is detected */
    st = HostApp_receive(&recvNum, false);
    if (st == FAIL) {
        printf("test_0302: failed while receiving packets\n");
        status = FAIL;
    }

    /* Check that all packets were received */
    if (recvNum != ETH_TEST_ITER_S_COUNT) {
        printf("test_0302: received frame count mismatch (exp=%d, got=%d)\n",
               ETH_TEST_ITER_S_COUNT, recvNum);
        status = FAIL;
    }

    EthUtils_printf("test_0302: END\r\n");

    return status;
}

bool HostApp_test_0400(void)
{
    uint32_t frames = 15000;
    bool status = PASS;
    int i = 0;

    for(i = 7; i >= 0; i--)
    {
        printf("test_0400: START %d\n", i);
        /* Wait for DUT to start the test when it's ready */
        HostApp_waitForCmd(CTRL_FRAME_CMD_START);

        /* Receive packets and measure the throughput */
        status = HostApp_receiveTput(MAX_RX_PKT, frames);
        if (status == FAIL)
            printf("test_0400: failed to get receive throughput\n");

        printf("test_0400: END\n");
    }

    return status;
}

bool HostApp_receiveTputQos(uint32_t total_frame, uint32_t size)
{
    bool status = PASS;
    struct timeval t0, t1;
    float elapsed;
    float pps;
    float mbps;
    long long total[8u] = {0,0,0,0,0,0,0,0};
    uint32_t frames[8u] = {0,0,0,0,0,0,0,0};
    ssize_t bytes;
    uint32_t i;
    EthFrame lrxFrame;
    uint32_t qos;
    struct sockaddr_ll  from;
    struct iovec        iov;
    struct msghdr       msg;
    struct cmsghdr      *cmsg;
    union {
        struct cmsghdr  cmsg;
        char        buf[CMSG_SPACE(sizeof(struct tpacket_auxdata))];
    } cmsg_buf;
    int val = 1;

    if(setsockopt(gHostApp.sock,SOL_PACKET, PACKET_AUXDATA, &val, sizeof(val)) < 0) {
         printf("failed to set aux option, so we can not get vlan header \n");
         return FAIL;
    }

    /* Get the "start" timestamp only after the first frame has been received */
    gettimeofday(&t0, NULL);

    while(true)
    {
        msg.msg_name        = &from;
        msg.msg_namelen     = sizeof(from);
        msg.msg_control     = &cmsg_buf;
        msg.msg_controllen  = sizeof(cmsg_buf);
        msg.msg_flags       = 0;
        iov.iov_len         = sizeof(lrxFrame);
        iov.iov_base        = (uint8_t *)&lrxFrame;
        msg.msg_iov         = &iov;
        msg.msg_iovlen      = 1;

        bytes = recvmsg(gHostApp.sock, &msg, MSG_TRUNC);

        /* Check if it's a STOP cmd */
        if (EthFrame_isStopCmd((EthFrame*)&lrxFrame)) {
            break;
        }

        qos = 8u;
        for (cmsg = CMSG_FIRSTHDR(&msg); cmsg; cmsg = CMSG_NXTHDR(&msg, cmsg)) {
            struct tpacket_auxdata *aux;
            size_t len;
            struct vlan_tag *tag;

            if (cmsg->cmsg_len < CMSG_LEN(sizeof(struct tpacket_auxdata)) ||
                cmsg->cmsg_level != SOL_PACKET ||
                cmsg->cmsg_type != PACKET_AUXDATA) {
                continue;
            }

            aux = (struct tpacket_auxdata *)CMSG_DATA(cmsg);
            if (!VLAN_VALID(aux, aux)) {
                continue;
            }

            qos = aux->tp_vlan_tci >> 13U;
            if(gHostApp.verbose) {
                printf("bytes %d tpid %x tci %x cos %d\n", (uint32_t)bytes,
                    VLAN_TPID(aux, aux), aux->tp_vlan_tci, aux->tp_vlan_tci >> 13U);
            }
        }

        if(qos >= 8){
            status = FAIL;
            printf("receiveTput: invalid qos: %x\n", qos);
        }
        else {
            total[qos] += bytes;
            frames[qos]++;
        }
    }

    /* Get the "end" timestamp after DUT is done sending frames */
    gettimeofday(&t1, NULL);
    elapsed = HostApp_timeDiff(t1, t0);

    for(i=0; i<8; ++i) {
        /* Compute elapsed time, packets per second and Mbps */
        pps = (float)frames[i] / elapsed;
        mbps = (float)total[i] * 8 / elapsed / 1000000;

        printf("receiveTput-COS %d: received %d frames in %.2f secs (%.2f frames/s, %.2f Mbps)\n",
            i, frames[i], elapsed, pps, mbps);
    }

    return status;
}

bool HostApp_test_0610(void)
{
    uint32_t frames = 15000;
    bool status = PASS;

    printf("test_0610: START\n");

    /* Wait for DUT to start the test when it's ready */
    HostApp_waitForCmd(CTRL_FRAME_CMD_START);

    /* Receive packets and measure the throughput */
    status = HostApp_receiveTputQos(MAX_RX_PKT, frames);
    if (status == FAIL)
        printf("test_0610: failed to get receive throughput\n");

    printf("test_0610: END\n");

    return status;
}

bool HostApp_test_0611(void)
{
    uint8_t pcp = 0;
    uint32_t iterations = 150000u;
    TestFrame frames[2];
    VlanDataFramePayload *payload;
    int vid = ETH_TEST_VLAN_VID;
    int i;

    printf("test_0611: START\r\n");

    /* Unicast packet with DUT's MAC address */
    frames[0].len = 1500u;
    pcp = 0u;
    payload = (VlanDataFramePayload *)frames[0].frame.payload;
    memcpy(&frames[0].frame.hdr, &hdrUcastCtrl, ETH_HDR_LEN);
    frames[0].frame.hdr.etherType = htons(ETHERTYPE_VLAN_TAG);
    payload->tci = htons((pcp << 13) | vid);
    payload->etherType = htons(ETHERTYPE_EXPERIMENTAL3);

    /* Unicast packet with DUT's MAC address */
    pcp = 7u;
    frames[1].len = 1500u;
    payload = (VlanDataFramePayload *)frames[1].frame.payload;
    memcpy(&frames[1].frame.hdr, &hdrUcastCtrl, ETH_HDR_LEN);
    frames[1].frame.hdr.etherType = htons(ETHERTYPE_VLAN_TAG);
    payload->tci = htons((pcp << 13) | vid);
    payload->etherType = htons(ETHERTYPE_EXPERIMENTAL3);

    /* Wait for DUT to start the test when it's ready */
    HostApp_waitForCmd(CTRL_FRAME_CMD_START);

    for (i = 0; i < iterations; i++) {
        if (gHostApp.verbose)
            printf("test_0611: iteration: %d of %d\n", i+1, iterations);

        /* Transmit one frame */
        HostApp_send(&frames[0].frame, frames[0].len);
        HostApp_send(&frames[1].frame, frames[1].len);
    }

    /* Send STOP command and wait for acknowledgement from DUT
     * - Send multiple times (ETH_TEST_ITER_S_COUNT) to account for
     *   possible packet drop
     * - This is OK since DUT will flush the receive queue before
     *   proceeding to the next test case
     */
    HostApp_sendCmdWithAck(CTRL_FRAME_CMD_STOP, ETH_TEST_ITER_S_COUNT);

    printf("test_0611: END\r\n");


    return PASS;
}

