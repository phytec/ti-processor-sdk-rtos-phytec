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
 *  \file     EthUtils_Host.h
 *
 *  \brief    This file contains Ud function declaration.
 */


#ifndef ETHUTILS_HOST_H
#define ETHUTILS_HOST_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "EthUtils.h"

#ifdef __cplusplus
extern "C"
{
#endif


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

#define OCTETS_PER_ROW                  8

#define HOST_APP_VERBOSE_OCTETS         16

#define HOST_APP_SOCKET_BUFSIZE         (512 * 1024)

typedef bool (*TestFunc)(void);

#define MAX_RX_PKT                      0xFFFFFFFFU

#define VLAN_VALID(hdr, hv) ((hv)->tp_vlan_tci != 0 || ((hdr)->tp_status & 0x10))
#define VLAN_TPID(hdr, hv) (((hv)->tp_vlan_tpid || \
    ((hdr)->tp_status & TP_STATUS_VLAN_TPID_VALID)) ? (hv)->tp_vlan_tpid : ETH_P_8021Q)

#define HOST_APP_MAX_PRIO_VALUE     8

#define HOST_APP_BASE_DELAY         1000

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */
typedef struct {
    int32_t sock;
    char *ifName;
    uint8_t hwAddr[ETH_HW_ADDR_LEN];
    uint8_t dutAddr[ETH_HW_ADDR_LEN];
    uint32_t timeout;
    uint32_t speed;
    uint8_t duplex;
    struct ifreq ifIdx;
    struct ifreq ifMac;
    struct ifreq ifData;
    struct sockaddr_ll sockAddr;
    bool verbose;
    bool veryVerbose;
    uint8_t testIdx;
    bool gbDUTReady;
	bool gbCmdRecvd;
    EthFrame rxFrame;
	CtrlFrame ctrlFrame;
} HostApp;

typedef struct {
    uint32_t len;
    EthFrame frame;
} TestFrame;


/* ========================================================================== */
/*                 Function Declarations                                      */
/* ========================================================================== */
int32_t HostApp_open(const char *ifName);

void HostApp_close(void);

void HostApp_send(EthFrame *frame, uint32_t len);

void HostApp_transmit(TestFrame *frames, uint32_t testFramesNum,
                      uint32_t num);

bool HostApp_recv(EthFrame *frame,
                  uint32_t *len);

bool HostApp_receive(uint32_t *frames, bool doFlushRecv);

bool HostApp_receiveTput(uint32_t total_frame, uint32_t size);

void HostApp_waitForCmd(uint8_t cmd);

void HostApp_sendCmd(uint8_t cmd);

float HostApp_timeDiff(struct timeval t1,
                       struct timeval t0);

void HostApp_waitForDUT(void);

void HostApp_sendCmdWithAck(uint8_t cmd, uint32_t sendnum);

uint32_t HostApp_getUSleepTime(uint32_t);

#ifdef __cplusplus
}
#endif

#endif