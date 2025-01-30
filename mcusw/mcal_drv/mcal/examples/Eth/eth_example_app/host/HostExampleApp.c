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
#include <signal.h>

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
static void sig_handler(int signo);

static void help(const char *me);

static void HostApp_testMain(void);

HostApp gHostApp = {
    .dutAddr = { 0xf4, 0x84, 0x4c, 0xeb, 0x95, 0x09 },
};

/* ========================================================================== */
/*                 Internal Variable Declarations                             */
/* ========================================================================== */
static const struct option long_options[] = {
    {"timeout",      1, 0, 't'},      /* time-out */
    {"iface",        1, 0, 'i'},      /* interface number */
    {"help",         0, 0, 'h'},      /* show help */
};

static const char short_options[] = "t:i:h";

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */
int main(int argc, char *argv[])
{
    const char *me = argv[0];
    char *ifName = "eth0";
    int32_t option;
    int32_t c;
    int32_t ret;

    gHostApp.verbose = false;
    gHostApp.veryVerbose = false;
    gHostApp.timeout = 10;

    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);

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

    while(true) {
        HostApp_testMain();
    }

    HostApp_close();

    return 0;
}

/* ========================================================================== */
/*                         Internal Function Definitions                      */
/* ========================================================================== */
static void help(const char *me)
{
    printf("Usage: %s [OPTION]...\n"
           "-h, --help              help\n"
           "-i, --iface=<NAME>      interface name (i.e. eth0)\n"
           "-t, --timeout=#         socket time-out (in secs)\n",
           me);
}

static void HostApp_testMain(void)
{
    ssize_t bytes;
    uint8_t tmp[6u];
    ssize_t sent;
    EthFrame ethFrame;

    memset(&ethFrame, 0, sizeof(EthFrame));

    bytes = recv(gHostApp.sock, &ethFrame, sizeof(EthFrame), 0);
    if (bytes < 0) {
        /* timeout */
    } else {
        memcpy(tmp, ethFrame.hdr.srcMac, 6u);
        memcpy(ethFrame.hdr.srcMac,  gHostApp.hwAddr, 6u);
        memcpy(ethFrame.hdr.dstMac, tmp, 6u);

        sent = sendto(gHostApp.sock,
                  &ethFrame, bytes, 0,
                  (struct sockaddr*)&gHostApp.sockAddr,
                  sizeof(struct sockaddr_ll));
        if (sent < 0)
            printf("send: failed to transmit packet: %s\n", strerror(errno));
        else if (sent != bytes)
            printf("sned: short transmission (req %d, got %d)\n", (int)bytes, (int)sent);
    }
}

static void sig_handler(int signo)
{
    if (signo == SIGINT || signo == SIGTERM) {
        printf("Received signal exit program\n");
        close(gHostApp.sock);
        exit(0);
    }
}