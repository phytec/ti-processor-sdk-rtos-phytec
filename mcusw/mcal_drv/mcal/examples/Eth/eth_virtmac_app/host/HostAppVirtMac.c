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
 * \brief Measure DUT transmit throughput
 *
 * Measure DUT transmit throughput over ETH_TEST_ITER_L_COUNT frames.
 * The host performs the following operations:
 * - Wait for START command from DUT
 * - Receive ETH_TEST_ITER_L_COUNT or until the STOP cmd is detected
 * - Compute the elapsed time, packets per second and Mbps
 *
 * \return PASS or FAIL  Test result
 */
bool HostApp_test_0100(void);

/**
 * \brief Test error recovery process
 *
 * Test error recovery process
 * - Receive error notification from server and start to do recovery process
 *
 * \return PASS or FAIL  Test result
 */
bool HostApp_test_1000(void);

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
    &HostApp_test_0100,
};

HostApp gHostApp = {
    .dutAddr = { 0xf4, 0x84, 0x4c, 0xeb, 0x95, 0x09 },
};

EthFrame rxFrame;

CtrlFrame ctrlFrame;

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
    .dstMac   = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
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
/*                            Internal Variables                              */
/* ========================================================================== */

static const struct option long_options[] = {
    {"timeout",      1, 0, 't'},      /* time-out */
    {"iface",        1, 0, 'i'},      /* interface number */
    {"verbose",      0, 0, 'v'},      /* verbose output */
    {"very-verbose", 0, 0, 'V'},      /* very verbose output */
    {"help",         0, 0, 'h'},      /* show help */
};

static const char short_options[] = "t:p:i:vh";

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

    gHostApp.verbose = false;
    gHostApp.veryVerbose = false;
    gHostApp.timeout = 10;

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

    /* Wait for DUT */
    printf("Waiting for DUT..\n");
    /* DUT <-> Host Initial Handshake */
    HostApp_waitForDUT();

    memcpy(hdrUcastCtrl.dstMac, gHostApp.dutAddr, ETH_HW_ADDR_LEN);

#if (STD_ON == ETH_CPSW_RECOVERY_TEST)
    status = HostApp_test_1000();
    globalStatus &= status;
#else
    for (i = 0; i < ARRAY_SIZE(testFuncs); i++) {
        printf("\n\n-----------------------------------------------------------\n");
        status = testFuncs[i]();
        globalStatus &= status;
        printf("Test Result: %s\n", status?"Pass":"Fail");
        printf("-----------------------------------------------------------\n");
    }
#endif

    HostApp_close();

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
    bool st;
    bool status = PASS;

    printf("test_0002: START\n");

    /* Wait for DUT to start the test when it's ready */
    HostApp_waitForCmd(CTRL_FRAME_CMD_START);

    /* Receive packets indefinitely until STOP cmd is detected */
    st = HostApp_receive(&recvNum, false);
    if (st == FAIL) {
        printf("test_0002: failed while receiving packets\n");
        status = FAIL;
    }

    /* Check that all packets were received */
    if (recvNum != ETH_TEST_ITER_M_COUNT) {
        printf("test_0002: received frame count mismatch (exp=%d, got=%d)\n",
               ETH_TEST_ITER_M_COUNT, recvNum);
        status = FAIL;
    }

    printf("test_0002: received %d of %d frames\n", recvNum, ETH_TEST_ITER_M_COUNT);
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
        len = sizeof(rxFrame);
        status = HostApp_recv(&rxFrame, &len);
        if (status == FAIL) {
            printf("test_0003: failed to receive frame\n");
            HostApp_sendCmd(CTRL_FRAME_CMD_STOP);
            break;
        }

        if (gHostApp.verbose) {
            printf("test_0003: frame received (%d bytes)\n", len);
            if (gHostApp.veryVerbose)
                EthUtils_printFrame(&rxFrame, HOST_APP_VERBOSE_OCTETS);
        }

        /* End the test if STOP cmd is received */
        if (EthFrame_isStopCmd(&rxFrame))
            break;

        /* Swap src and dst MAC addresses */
        memcpy(rxFrame.hdr.dstMac, rxFrame.hdr.srcMac, ETH_HW_ADDR_LEN);
        memcpy(rxFrame.hdr.srcMac, gHostApp.hwAddr, ETH_HW_ADDR_LEN);

        if (gHostApp.verbose) {
            printf("test_0003: frame to be sent (%d bytes)\n", len);
            if (gHostApp.veryVerbose)
                EthUtils_printFrame(&rxFrame, HOST_APP_VERBOSE_OCTETS);
        }

        /* Send the frame back */
        HostApp_send(&rxFrame, len);
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

    /* Part 5: DUT's filter is open, all packets should be accepted */
    memcpy(&frame->hdr, &hdrUcastInv, ETH_HDR_LEN);
    EthUtils_fillPayload(&frame->payload,
                         ETH_TEST_TYPE_PATTERN_4,
                         len - ETH_HDR_LEN);

    HostApp_waitForCmd(CTRL_FRAME_CMD_START);
    HostApp_transmit(frames, ARRAY_SIZE(frames), num);
    HostApp_sendCmd(CTRL_FRAME_CMD_STOP);

    /* Part 5: DUT's filter has been reset. Only packets with DUT's address
     *         should be accepted */
    memcpy(&frame->hdr, &hdrUcastInv, ETH_HDR_LEN);
    EthUtils_fillPayload(&frame->payload,
                         ETH_TEST_TYPE_PATTERN_2,
                         len - ETH_HDR_LEN);

    HostApp_waitForCmd(CTRL_FRAME_CMD_START);
    HostApp_transmit(frames, ARRAY_SIZE(frames), num);
    HostApp_sendCmd(CTRL_FRAME_CMD_STOP);

    printf("test_0005: END\n");

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

bool HostApp_test_0100(void)
{
    uint32_t frames = 150000;
    bool status;

    printf("test_0100: START\n");

    /* Wait for DUT to start the test when it's ready */
    HostApp_waitForCmd(CTRL_FRAME_CMD_START);

    /* Receive packets and measure the throughput */
    status = HostApp_receiveTput(frames, 1500u);
    if (status == FAIL)
        printf("test_0100: failed to get receive throughput\n");

    printf("test_0100: END\n");

    return status;
}

bool HostApp_test_1000(void)
{
    bool status = true;
    int i;

    for(i = 0; i < 50; ++i) {
        status &= HostApp_test_0001();
        status &= HostApp_test_0002();
    }

    return status;
}