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
#include <linux/sockios.h>
#include <linux/ethtool.h>

#include "EthUtils.h"
#include "EthUtils_Host.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
#define HOSTAPP_TEST_RX  0
#define HOSTAPP_TEST_TX  1
#define HOSTAPP_TEST_NUM 2
#define HOSTAPP_TEST_IDX_NUM 5

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */


/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */

void help(const char *me);


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
bool HostApp_test_0100_wrapper(uint8_t testIdx);

/* Measure Rx through*/
bool HostApp_test_0101(uint8_t testIdx);

/* Get interface link speed */
int HostApp_getInterfaceSpeed(char *ifname);

/* Reduce number of test packet base on link speed */
int HostApp_GetScaleValue(void);

/* ========================================================================== */
/*                            Global Variables                                */
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

HostApp gHostApp = {
    .dutAddr = { 0xf4, 0x84, 0x4c, 0xeb, 0x95, 0x09 },
};


/* ========================================================================== */
/*                            Internalariables                                */
/* ========================================================================== */

static const struct option long_options[] = {
    {"timeout",      1, 0, 't'},      /* time-out */
    {"iface",        1, 0, 'i'},      /* interface number */
    {"verbose",      0, 0, 'v'},      /* verbose output */
    {"very-verbose", 0, 0, 'V'},      /* very verbose output */
    {"help",         0, 0, 'h'},      /* show help */
};

static const char short_options[] = "t:p:i:v:h";

/* Test data */
static uint16_t   gTxTestIdx         = 0u;
static uint16_t   gTxPktSizeMesure[] = { 1500u,   512u,     256u,     128u,     64u      };
static uint32_t   gTxIteratorNum[]   = { 640000u, 1280000u, 2560000u, 3072000u, 2560000u };
static uint32_t   gTxDelayNum[]   =    { 0u,      0u,       40u,      40u,      30u      };
static uint16_t   gRxTestIdx         = 0u;
static uint16_t   gRxPktSizeMesure[] = { 1500u,   512u,     256u,     128u,     64u      };
static uint32_t   gRxIteratorNum[]   = { 640000u, 1280000u, 1280000u, 1280000u, 1280000u };
static uint32_t   gTestSpeed = 0xFFFF;

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
    int32_t ret;
    uint8_t testType, testId;

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

    for(testType = 0u; testType < HOSTAPP_TEST_NUM; ++testType)
    {
        for(testId = 0u; testId < HOSTAPP_TEST_IDX_NUM; ++testId)
        {

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
            HostApp_waitForCmd(CTRL_FRAME_CMD_READY);
            
            memcpy(hdrUcastCtrl.dstMac, gHostApp.dutAddr, ETH_HW_ADDR_LEN);

            gTestSpeed = HostApp_getInterfaceSpeed(ifName);
            printf("Link speed %d\n", gTestSpeed);

            if(testType == HOSTAPP_TEST_TX)
            {
                HostApp_test_0101(testId);
            }
            else
            {
                HostApp_test_0100_wrapper(testId);
            }
        }
    }

    HostApp_close();

    printf("\nTest Result: %s\n\n", globalStatus?"Pass":"Fail");

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

bool HostApp_test_0100_wrapper(uint8_t testIdx)
{
    bool status;

    printf("test_0100: START %d\n", testIdx);

    status = HostApp_test_0100(gRxIteratorNum[testIdx]/HostApp_GetScaleValue(),
        gRxPktSizeMesure[testIdx]);

    printf("test_0100: DONE %d\n", testIdx);

    return status;
}

bool HostApp_test_0101(uint8_t testIdx)
{
    uint32_t iterations = ETH_TEST_ITER_L_COUNT;
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
    int j,k;

    EthUtils_printf("test_0101: START %d\n", testIdx);

    si = testIdx;
    {
        /* Unicast packet with DUT's MAC address */
        frames[0].len = gTxPktSizeMesure[si];
        payload = (DataFramePayload *)frames[0].frame.payload;
        memcpy(&frames[0].frame.hdr, &hdrUcastCtrl, ETH_HDR_LEN);
        frames[0].frame.hdr.etherType = htons(ETHERTYPE_EXPERIMENTAL3);

        iterations = gTxIteratorNum[si]/HostApp_GetScaleValue();

        /* Wait for DUT to start the test when it's ready */
        HostApp_waitForCmd(CTRL_FRAME_CMD_START);

        gettimeofday(&t0, NULL);

        /* Transmit one frame per iteration */
        for (i = 0; i < iterations; i++) {
            if (gHostApp.verbose)
                EthUtils_printf("test_0101: iteration: %d of %d\n", i+1, iterations);

            /* Transmit one frame */
            HostApp_send(&frames[0].frame, frames[0].len);
            if((gTxDelayNum[si] > 0u) && (i % gTxDelayNum[si] == 0))
            {
                if(gTxDelayNum[si] < 10)
                {
                    /* need a bigger sleep in big packet size (or small slowing down interval) */
                    usleep(15);
                }
                else
                {
                    usleep(1);   
                }
            }
        }

        /* Indicate to the DUT that the test is complete */
        HostApp_sendCmd(CTRL_FRAME_CMD_STOP);

        /* Get the "end" timestamp after DUT is done sending frames */
        gettimeofday(&t1, NULL);

        /* Indicate to the DUT that the test is complete, try couple times to prevent lost of STOP packet */
        for(j=0; j<20; ++j) {
            usleep(10000);
            for(k=0; k<10; ++k) {
                HostApp_sendCmd(CTRL_FRAME_CMD_STOP);
            }
        }
    
       /* Compute elapsed time, packets per second and Mbps */
       elapsed = HostApp_timeDiff(t1, t0);
       pps = (float)i / elapsed;
       mbps = (float)i*frames[0].len * 8 / elapsed / 1000000;
   
       EthUtils_printf("transmitTput: TX side transmitted %d frames in %.2f secs (%.2f frames/s, %.2f Mbps)\n",
           i, elapsed, pps, mbps);
    }

    printf("test_0101: END %d\n", testIdx);

    return status;
}

int HostApp_getInterfaceSpeed(char *ifname)
{
    int sock;
    struct ifreq ifr;
    struct ethtool_cmd edata;
    int rc;

    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock < 0) {
        return 0xFFFF;
    }
    strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));
    ifr.ifr_data = (char*)&edata;
    edata.cmd = ETHTOOL_GSET;

    rc = ioctl(sock, SIOCETHTOOL, &ifr);
    if (rc < 0) {
        close(sock);
        return 0xFFFF;
    }
    close(sock);
    return edata.speed;
}

int HostApp_GetScaleValue(void)
{
    int scale = 1;
    if(gTestSpeed == 100)
    {
        scale = 5;
    }
    return scale;
}