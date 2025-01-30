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
 *  \file     EthUtils_Host.c
 *
 *  \brief    This file contains Ud function definition.
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
#include <linux/ethtool.h>
#include <linux/sockios.h>
#include <EthUtils.h>

#include "EthUtils_Host.h"

/* ========================================================================== */
/*                         Global Variable Definitions                        */
/* ========================================================================== */

extern HostApp gHostApp;

/* ========================================================================== */
/*                         Internal Function Declaration                       */
/* ========================================================================== */
static void inline HostApp_processCmd(CtrlFrame *frame);

/* ========================================================================== */
/*                         Function Definition                                */
/* ========================================================================== */
int32_t HostApp_open(const char *ifName)
{
    struct ethtool_cmd edata;
    struct timeval timeout;
    uint32_t size;
    uint32_t i;
    int32_t ret;

    /* Open a raw socket */
    ret = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (ret > 0) {
        gHostApp.sock = ret;
    } else {
        printf("open: failed to open raw socket: %s\n", strerror(errno));
        ret = -errno;
        return ret;
    }

    /* Set a timeout */
    timeout.tv_sec = gHostApp.timeout;
    timeout.tv_usec = 0;
    ret = setsockopt(gHostApp.sock, SOL_SOCKET, SO_RCVTIMEO,
                     (const char *)&timeout, sizeof(timeout));
    if (ret) {
        printf("open: failed to set timeout: %s\n", strerror(errno));
        goto open_err;
    }

    /* Increase the socket buffer size */
    size = HOST_APP_SOCKET_BUFSIZE;
    ret  = setsockopt(gHostApp.sock, SOL_SOCKET, SO_RCVBUF,
                      &size, sizeof(size));
    if (ret) {
        printf("open: failed to socket buffer size: %s\n", strerror(errno));
        goto open_err;
    }

    ret = setsockopt(gHostApp.sock, SOL_SOCKET, SO_BINDTODEVICE,
                      ifName, strlen(ifName));
    if (ret) {
        printf("open: failed to bind to device: %s\n", strerror(errno));
        goto open_err;
    }

    /* Get interface index */
    memset(&gHostApp.ifIdx, 0, sizeof(struct ifreq));
    strncpy(gHostApp.ifIdx.ifr_name, ifName, IFNAMSIZ - 1);
    ret = ioctl(gHostApp.sock, SIOCGIFINDEX, &gHostApp.ifIdx);
    if (ret) {
        printf("open: failed to get interface index: %s\n", strerror(errno));
        goto open_err;
    }

    /* Get interface MAC address */
    memset(&gHostApp.ifMac, 0, sizeof(struct ifreq));
    strncpy(gHostApp.ifMac.ifr_name, ifName, IFNAMSIZ - 1);
    ret = ioctl(gHostApp.sock, SIOCGIFHWADDR, &gHostApp.ifMac);
    if (ret) {
        printf("open: failed to get interface MAC address: %s\n", strerror(errno));
        goto open_err;
    }

    /* Get interface data */
    memset(&gHostApp.ifData, 0, sizeof(struct ifreq));
    strncpy(gHostApp.ifData.ifr_name, ifName, IFNAMSIZ - 1);
    gHostApp.ifData.ifr_data = (char *)&edata;
    edata.cmd = ETHTOOL_GSET;
    ret = ioctl(gHostApp.sock, SIOCETHTOOL, &gHostApp.ifData);
    if (ret) {
        printf("open: failed to get interface data: %s\n", strerror(errno));
        goto open_err;
    }
    gHostApp.speed = ethtool_cmd_speed(&edata);
    gHostApp.duplex = edata.duplex;

    /* Save host's MAC address */
    for (i = 0; i < ETH_HW_ADDR_LEN; i++)
        gHostApp.hwAddr[i] = gHostApp.ifMac.ifr_hwaddr.sa_data[i];

    gHostApp.sockAddr.sll_family  = PF_PACKET;
    gHostApp.sockAddr.sll_ifindex = gHostApp.ifIdx.ifr_ifindex;
    gHostApp.sockAddr.sll_halen = ETH_ALEN;
    memset(gHostApp.sockAddr.sll_addr, 0, ETH_HW_ADDR_LEN);

    /* Bind only to requested interface */
    ret = bind(gHostApp.sock,
               (struct sockaddr *)&gHostApp.sockAddr,
               sizeof(gHostApp.sockAddr));
    if (ret)
    {
        printf("open: failed to bind to '%s': %s\n", ifName, strerror(errno));
        goto open_err;
    }

    printf("Interface    : %s\n", ifName);
    printf("MAC address  : %02x:%02x:%02x:%02x:%02x:%02x\n",
           gHostApp.hwAddr[0] & 0xFF, gHostApp.hwAddr[1] & 0xFF,
           gHostApp.hwAddr[2] & 0xFF, gHostApp.hwAddr[3] & 0xFF,
           gHostApp.hwAddr[4] & 0xFF, gHostApp.hwAddr[5] & 0xFF);
    printf("\n\n");

    return 0;

open_err:
    close(gHostApp.sock);
    gHostApp.sock = -1;
    ret = -errno;
    return ret;
}

void HostApp_close(void)
{
    close(gHostApp.sock);
    gHostApp.sock = -1;
}

void HostApp_send(EthFrame *frame,
                  uint32_t len)
{
    ssize_t sent;

    sent = sendto(gHostApp.sock,
                  frame, len, 0,
                  (struct sockaddr*)&gHostApp.sockAddr,
                  sizeof(struct sockaddr_ll));

    if (sent < 0)
        printf("send: failed to transmit packet: %s\n", strerror(errno));
    else if (sent != len)
        printf("sned: short transmission (req %d, got %d)\n", len, (int)sent);
}

void HostApp_transmit(TestFrame *frames,
                      uint32_t testFramesNum,
                      uint32_t num)
{
    TestFrame *frame;
    uint32_t sent = 0;
    uint32_t delay = 0;

    while (true) {
        frame = &frames[sent % testFramesNum];

        HostApp_send(&frame->frame, frame->len);

        if (gHostApp.veryVerbose)
            EthUtils_printFrame(&frame->frame, HOST_APP_VERBOSE_OCTETS);

        delay = HostApp_getUSleepTime(frame->len);
        usleep(delay);

        if (++sent >= num)
            break;
    }
}

bool HostApp_recv(EthFrame *frame,
                  uint32_t *len)
{
    ssize_t bytes;
    bool status = PASS;

    memset(frame, 0, sizeof(EthFrame));

    bytes = recv(gHostApp.sock, frame, sizeof(EthFrame), 0);
    if (bytes < 0) {
        printf("recv: failed to receive data over raw socket: %s\n", strerror(errno));
        *len = 0;
        status = FAIL;
    } else {
        *len = bytes;
    }

    return status;
}

bool HostApp_receive(uint32_t *frames, bool doFlushRecv)
{
    long long total = 0;
    uint32_t len;
    uint32_t i = 0, j;
    int32_t ret;
    bool forever = false;
    bool status = PASS;
    uint32_t flushnum = 0;

    /* Receive packets indefinitely if requested frame count is 0 */
    if (*frames == 0)
        forever = true;

    /* Receive frames */
    while (true) {
        /* Receive one frame */
        status = HostApp_recv(&gHostApp.rxFrame, &len);
        if (status == FAIL) {
            printf("receive: failed to receive packet: %d\n", ret);
            break;
        }

        if (gHostApp.veryVerbose)
            EthUtils_printFrame(&gHostApp.rxFrame, HOST_APP_VERBOSE_OCTETS);

        /* Ignore frames with other EtherTypes */
        if ((gHostApp.rxFrame.hdr.etherType != htons(ETHERTYPE_EXPERIMENTAL1)) &&
            (gHostApp.rxFrame.hdr.etherType != htons(ETHERTYPE_EXPERIMENTAL2)) &&
            (gHostApp.rxFrame.hdr.etherType != htons(ETHERTYPE_EXP_CONTROL)) &&
            (gHostApp.rxFrame.hdr.etherType != htons(ETHERTYPE_VLAN_TAG)))
            continue;

        /* Check if it's a STOP cmd */
        if (EthFrame_isStopCmd(&gHostApp.rxFrame)) {
            if (gHostApp.verbose)
                printf("receive: STOP command received\n");
            break;
        }

        /* Verify frame content */
        if (EthFrame_isVlanTagged(&gHostApp.rxFrame))
            ret = EthUtils_checkVlanPayload((VlanDataFramePayload *)gHostApp.rxFrame.payload);
        else
            ret = EthUtils_checkPayload((DataFramePayload *)gHostApp.rxFrame.payload);

        if (ret != ETH_TEST_PKT_SOK) {
            printf("receive: frame %d is not valid: %d\n", i+1, ret);
            if (gHostApp.veryVerbose)
                EthUtils_printFrame(&gHostApp.rxFrame, len);
            status = FAIL;
        }

        total += len;
        i++;

        if ((!forever) && (i == *frames))
            break;
    }

    /* flushed remaining packets if any */
    if (doFlushRecv)
    {
        flushnum = ETH_TEST_ITER_M_COUNT;
        for (j = i; j < (flushnum+ETH_TEST_ITER_S_COUNT); j++) {
            len = recv(gHostApp.sock, &gHostApp.rxFrame, sizeof(EthFrame), MSG_DONTWAIT);
            if (len < 0) {
                if (EAGAIN != errno && EWOULDBLOCK != errno) break;
                continue;
            }

            /* skip STOP cmd */
            if (EthFrame_isStopCmd(&gHostApp.rxFrame)) {
                continue;
            }

            total += len;
            i++;
        }
    }

    /* Update with the number of frames received */
    *frames = i;

    if (gHostApp.verbose)
        printf("receive: received %d frames, %lld bytes\n", i, total);

    return status;
}

bool HostApp_receiveTput(uint32_t total_frame, uint32_t size)
{
    struct timeval t0, t1;
    float elapsed;
    float pps;
    float mbps;
    long long total = 0;
    ssize_t bytes;
    uint32_t i;
    uint32_t frames = total_frame;
    uint32_t flushnum = 0;

    /* Receive frames until STOP cmd is detected */
    for (i = 0; i < frames; i++) {
        bytes = recv(gHostApp.sock, &gHostApp.rxFrame, sizeof(EthFrame), 0);
        if (bytes < 0) {
            if (EAGAIN != errno && EWOULDBLOCK != errno) {
                printf("receiveTput: failed to received data: %s\n",
                        strerror(errno));
                break;
            }
            continue;
        }

        /* Get the "start" timestamp only after the first frame has been received */
        if (i == 0)
            gettimeofday(&t0, NULL);

        /* Check if it's a STOP cmd */
        if (EthFrame_isStopCmd(&gHostApp.rxFrame)) {
            frames = i;
            break;
        }

        total += bytes;
    }

    /* Get the "end" timestamp after DUT is done sending frames */
    gettimeofday(&t1, NULL);

    /* flushed remaining packets if any */
    flushnum = total_frame == MAX_RX_PKT ? 15000u : total_frame;
    for (i = frames; i < (flushnum+ETH_TEST_ITER_S_COUNT); i++) {
        bytes = recv(gHostApp.sock, &gHostApp.rxFrame, sizeof(EthFrame), MSG_DONTWAIT);
        if (bytes < 0) {
            if (EAGAIN != errno && EWOULDBLOCK != errno) break;
            continue;
        }

        /* skip STOP cmd */
        if (EthFrame_isStopCmd(&gHostApp.rxFrame)) {
            continue;
        }

        frames++;
        total += bytes;
    }

    /* Compute elapsed time, packets per second and Mbps */
    elapsed = HostApp_timeDiff(t1, t0);
    pps = (float)frames / elapsed;
    mbps = (float)total * 8 / elapsed / 1000000;

    /* incase rate limit test, no need to show lost frame*/
    if(total_frame == MAX_RX_PKT) {
    printf("receiveTput-%d: received %d frames in %.2f secs (%.2f frames/s, %.2f Mbps)\n", size,
           frames, elapsed, pps, mbps);
    }
    else {
    printf("receiveTput-%d: received %d frames in %.2f secs (%.2f frames/s, %.2f Mbps) Lost %d\n", size,
           frames, elapsed, pps, mbps, (total_frame-frames));
    }

    return PASS;
}

void HostApp_sendCmd(uint8_t cmd)
{
    EthFrame *frame = (EthFrame *)&gHostApp.ctrlFrame;

    gHostApp.ctrlFrame.payload.cmd = cmd;
    memset(gHostApp.ctrlFrame.payload.data.payload, 0, sizeof(gHostApp.ctrlFrame.payload.data));

    if (gHostApp.verbose) {
        switch (cmd) {
        case CTRL_FRAME_CMD_READY:
            printf("sendCmd: READY cmd\n");
            break;
        case CTRL_FRAME_CMD_START:
            printf("sendCmd: START cmd\n");
            break;
        case CTRL_FRAME_CMD_STOP:
            printf("sendCmd: STOP cmd\n");
            break;
        case CTRL_FRAME_CMD_READY_SYN_ACK:
            printf("sendCmd: READY SYN ACK cmd\n");
            break;
        default:
            printf("sendCmd: invalid cmd\n");
            break;
        }
    }

    if(cmd == CTRL_FRAME_CMD_READY_SYN_ACK) {
        gHostApp.ctrlFrame.payload.data.payload[0] = gHostApp.testIdx;
    }

    HostApp_send(frame, sizeof(gHostApp.ctrlFrame));
}

void HostApp_waitForCmd(uint8_t cmd)
{
    CtrlFrame *frame = (CtrlFrame*)&gHostApp.rxFrame;
    ssize_t bytes;

    /* Receive frames until a command frame is detected */
    while (true) {
        /* Receive one frame */
        memset(frame, 0, sizeof(EthFrame));
        bytes = recv(gHostApp.sock, frame, sizeof(EthFrame), 0);
        if (bytes < 0) {
            if (errno == EAGAIN) {
                continue;
            } else {
                /* Exit the application since host and DUT are no longer in sync */
                printf("waitForCmd: errno %d: %s\n", errno, strerror(errno));
                exit(1);
            }
        }

        /* Check if it's a control frame and take action depending on the type */
        if ((frame->hdr.etherType == ntohs(ETHERTYPE_EXP_CONTROL)) &&
            (frame->payload.cmd == cmd)) {
            HostApp_processCmd(frame);
            break;
        }
    }
}

float HostApp_timeDiff(struct timeval t1,
                       struct timeval t0)
{
    struct timeval temp;
    float elapsed;

    if ((t1.tv_usec - t0.tv_usec) < 0) {
        temp.tv_sec = t1.tv_sec - t0.tv_sec-1;
        temp.tv_usec = 1000000UL + t1.tv_usec - t0.tv_usec;
    } else {
        temp.tv_sec = t1.tv_sec - t0.tv_sec;
        temp.tv_usec = t1.tv_usec - t0.tv_usec;
    }

    elapsed = (float)temp.tv_sec + (float)temp.tv_usec/1000000;

    return elapsed;
}

void HostApp_waitForDUT(void)
{
    /* Emulate TCP's Initial Handshake to make sure both side's interface is
     * up and running.
     *  [DUT]      [HOST]
     *    |--- READY->|
     *    |<-- SACK --|
     *    |--- ACK -->|
     */
    printf("Waiting for DUT..\n");
    do {
        HostApp_waitForCmd(CTRL_FRAME_CMD_READY);
        printf("Recv Ready CMD, sending READY SYN ACK...\n");
        HostApp_sendCmd(CTRL_FRAME_CMD_READY_SYN_ACK);
        printf("Waiting for DUT's READY ACK command...\n");
        HostApp_waitForCmd(CTRL_FRAME_CMD_READY_ACK);
    } while(!gHostApp.gbDUTReady);
}

void HostApp_sendCmdWithAck(uint8_t cmd, uint32_t sendnum)
{
    CtrlFrame *frame = (CtrlFrame*)&gHostApp.rxFrame;
    ssize_t bytes;
    uint32_t i;

    /* Emulate TCP's 3-way Handshake to make sure both side's interface is
     * on sync
     *  [HOST]      [DUT]
     *    |--- CMD -->|
     *    |<-- SACK --|
     *    |--- ACK -->|
     */

    gHostApp.gbCmdRecvd =  false;
    do {
        for(i=0; i<sendnum; ++i) {
            HostApp_sendCmd(cmd);
            usleep(10000);
        }
        memset(frame, 0, sizeof(EthFrame));
        bytes = recv(gHostApp.sock, frame, sizeof(EthFrame), 0);
        if (bytes < 0) {
            if (errno == EAGAIN) {
                continue;
            } else {
                /* Exit the application since host and DUT are no longer
                 * in sync */
                printf("waitForCmd: errno %d: %s\n", errno, strerror(errno));
                close(gHostApp.sock);
                exit(1);
            }
        }

        /* Check if it's a control frame and take action depending on the
         * type */
        if ((frame->hdr.etherType == ntohs(ETHERTYPE_EXP_CONTROL))) {
            HostApp_processCmd(frame);
        }
    } while(!gHostApp.gbCmdRecvd);

    HostApp_sendCmd(CTRL_FRAME_CMD_RECV_ACK);
}

uint32_t HostApp_getUSleepTime(uint32_t frameSize)
{
    uint32_t delay = HOST_APP_BASE_DELAY;
    delay = delay * (frameSize+20)/(gHostApp.speed*1000/8)+1;
    if (delay < HOST_APP_BASE_DELAY)
    {
        delay = HOST_APP_BASE_DELAY;
    }
    if (!gHostApp.duplex)
    {
        delay *= 2;
    }
    return delay;
}

/* ========================================================================== */
/*                         Interla Variable Definition                        */
/* ========================================================================== */
static void inline HostApp_processCmd(CtrlFrame *frame)
{
    switch (frame->payload.cmd) {
        case CTRL_FRAME_CMD_START:
            if (gHostApp.verbose)
                printf("waitForCmd: START cmd received\n");
            break;
        case CTRL_FRAME_CMD_READY:
            memcpy(gHostApp.dutAddr, frame->hdr.srcMac, ETH_HW_ADDR_LEN);
            break;
        case CTRL_FRAME_CMD_READY_ACK:
            memcpy(gHostApp.dutAddr, frame->hdr.srcMac, ETH_HW_ADDR_LEN);
            printf("DUT detected: %02x:%02x:%02x:%02x:%02x:%02x\n",
                   gHostApp.dutAddr[0] & 0xFF, gHostApp.dutAddr[1] & 0xFF,
                   gHostApp.dutAddr[2] & 0xFF, gHostApp.dutAddr[3] & 0xFF,
                   gHostApp.dutAddr[4] & 0xFF, gHostApp.dutAddr[5] & 0xFF);
            gHostApp.gbDUTReady =  true;
            break;
        case CTRL_FRAME_CMD_RECV_SYN_ACK:
            gHostApp.gbCmdRecvd =  true;
            break;
        default:
            printf("WARN: Unknown cmd (0x%02X) received\n",
                    frame->payload.cmd);
            break;
    }
}
