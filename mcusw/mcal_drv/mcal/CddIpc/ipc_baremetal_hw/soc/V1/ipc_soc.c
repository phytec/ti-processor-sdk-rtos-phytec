/*
 *  Copyright (c) Texas Instruments Incorporated 2024
 *  All rights reserved.
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
 *  \file ipc_soc.c
 *
 *  \brief File containing the IPC driver - soc specific implementation.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <CddIpc/ipc_baremetal_hw/ipc.h>
#include <CddIpc/ipc_baremetal_hw/src/ipc_mailbox.h>

#if defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
#include <ti/drv/sciclient/sciclient.h>
#endif

/**
 * \ Dependencies from pdk Sciclient
 */



#define NAVSS_INTRTR_INPUT_MAILBOX0_USER0   (436U)
#define NAVSS_INTRTR_INPUT_MAILBOX1_USER0   (432U)
#define NAVSS_INTRTR_INPUT_MAILBOX2_USER0   (428U)
#define NAVSS_INTRTR_INPUT_MAILBOX3_USER0   (424U)
#define NAVSS_INTRTR_INPUT_MAILBOX4_USER0   (420U)
#define NAVSS_INTRTR_INPUT_MAILBOX5_USER0   (416U)
#define NAVSS_INTRTR_INPUT_MAILBOX6_USER0   (412U)
#define NAVSS_INTRTR_INPUT_MAILBOX7_USER0   (408U)
#define NAVSS_INTRTR_INPUT_MAILBOX8_USER0   (404U)
#define NAVSS_INTRTR_INPUT_MAILBOX9_USER0   (400U)
#define NAVSS_INTRTR_INPUT_MAILBOX10_USER0  (396U)
#define NAVSS_INTRTR_INPUT_MAILBOX11_USER0  (392U)

#define CDD_IPC_START_SEC_VAR_INIT_32
#include "Cdd_Ipc_MemMap.h"

/**
 * \brief Main NavSS512 - Mailbox input line
 */
uint32 g_Navss512MbInput[IPC_MAILBOX_CLUSTER_CNT] =
{
    NAVSS_INTRTR_INPUT_MAILBOX0_USER0,
    NAVSS_INTRTR_INPUT_MAILBOX1_USER0,
    NAVSS_INTRTR_INPUT_MAILBOX2_USER0,
    NAVSS_INTRTR_INPUT_MAILBOX3_USER0,
    NAVSS_INTRTR_INPUT_MAILBOX4_USER0,
    NAVSS_INTRTR_INPUT_MAILBOX5_USER0,
    NAVSS_INTRTR_INPUT_MAILBOX6_USER0,
    NAVSS_INTRTR_INPUT_MAILBOX7_USER0,
    NAVSS_INTRTR_INPUT_MAILBOX8_USER0,
    NAVSS_INTRTR_INPUT_MAILBOX9_USER0,
    NAVSS_INTRTR_INPUT_MAILBOX10_USER0,
    NAVSS_INTRTR_INPUT_MAILBOX11_USER0
};

#define CDD_IPC_STOP_SEC_VAR_INIT_32
#include "Cdd_Ipc_MemMap.h"

#define CDD_IPC_START_SEC_CODE
#include "Cdd_Ipc_MemMap.h"

uint32 Ipc_getNavss512MailboxInputIntr(uint32 clusterId, uint32 userId);
int32_t Ipc_setCoreEventId(uint32 selfId, Ipc_MbConfig* cfg, uint32 intrCnt);
int32_t Ipc_sciclientIrqTranslate(uint16 coreId, uint32 eventId,
        uint16 *procIrq);

int32_t Ipc_getMailboxInfoTx(uint32 selfId, uint32 remoteId,
                 uint32 *clusterId, uint32 *userId, uint32 *queueId)
{
    int32_t retVal = -1;

    if( (selfId < IPC_MAX_PROCS) &&
        (remoteId < IPC_MAX_PROCS))
    {
        /*CDD_IPC_CoverageGap_12: SelfId and RemoteId are fixed enums which cannot be changed.*/
        Ipc_MailboxInfo   *pMailboxInfo = &g_IPC_MailboxInfo[selfId][remoteId];

        *clusterId = pMailboxInfo->tx.cluster;
        *userId    = pMailboxInfo->tx.user;
        *queueId   = pMailboxInfo->tx.fifo;
        retVal = 0;
    }

    return retVal;
}

int32_t Ipc_getMailboxInfoRx(uint32 selfId, uint32 remoteId,
                 uint32 *clusterId, uint32 *userId, uint32 *queueId)
{
    int32_t retVal = -1;

    if( (selfId < IPC_MAX_PROCS) &&
        (remoteId < IPC_MAX_PROCS))
    {
        /*CDD_IPC_CoverageGap_12: SelfId and RemoteId are fixed enums which cannot be changed.*/
        Ipc_MailboxInfo   *pMailboxInfo = &g_IPC_MailboxInfo[selfId][remoteId];

        *clusterId = pMailboxInfo->rx.cluster;
        *userId    = pMailboxInfo->rx.user;
        *queueId   = pMailboxInfo->rx.fifo;
        retVal = 0;
    }

    return retVal;

}

uintptr_t Ipc_getMailboxBaseAddr(uint32 clusterId)
{
    uintptr_t baseAddr = 0x00000000U;

    if( clusterId < IPC_MAILBOX_CLUSTER_CNT)
    {
        baseAddr = IPC_Mailbox_BasePhyAddr[clusterId];
    }

    return baseAddr;
}

const char* Ipc_getCoreName(uint32 procId)
{
    char*     p = (char*)0;
    uint32 id = procId;

    if(id < IPC_MAX_PROCS)
    {
       p = (char *)g_Ipc_mp_procInfo[id].name;
    }
    return p;
}

uint32 Ipc_getNavss512MailboxInputIntr(uint32 clusterId, uint32 userId)
{
    uint32  mailboxIntrNum = 0U;

    if( (clusterId != MAILBOX_CLUSTER_INVALID) &&
        (clusterId < IPC_MAILBOX_CLUSTER_CNT)  &&
        (userId != MAILBOX_USER_INVALID)       &&
        (userId < IPC_MAILBOX_USER_CNT))
    {
        mailboxIntrNum = g_Navss512MbInput[clusterId] + userId;
    }
    return mailboxIntrNum;
}

#define CDD_IPC_STOP_SEC_CODE
#include "Cdd_Ipc_MemMap.h"

int32_t Ipc_setCoreEventId(uint32 selfId, Ipc_MbConfig* cfg, uint32 intrCnt)
{
    int32_t    retVal          = IPC_SOK;
    uint32   outIntrBaseNum  = 0;
    uint32   vimEventBaseNum = 0;
    uint16   proc_irq        = 0;

    /*
     * static variable to used to store the base for first
     * mailbox interrupt register. In subsequent call, it uses
     * the offset of intrCnt from base
     */
    static uint16   start    = 0;
    static uint16   range    = 0;
    uint16   offset   = 0;

    /* Get available CorePack IRQ number from DMSC */
    if( (start == 0U) && (range == 0U))
    {
        retVal = Ipc_getIntNumRange(selfId, &start, &range);
    }

    if((retVal == IPC_SOK) && (range >= 1U))
    {
        /* Allocate the last 5  interrupts for IPC. Note that the IR allocation is
         * static so this needs to be carefully set. Currently first interrupt is
         * used by UDMA and middle one's are used by other modules like CPSW9G so
         * we are using last 5 as a safe option.
         */
        if(range >= 5U)
        {
            offset = 5;
        }
        else
        {
            offset = range;
        }
        vimEventBaseNum = (start + range) - offset;

        /* Translation must happen after this offset */
        retVal = Ipc_sciclientIrqTranslate((uint16)selfId, vimEventBaseNum,
                                           &proc_irq);
        if (LLD_PASS == retVal)
        {
            vimEventBaseNum = proc_irq;
        }

    }
    else
    {
        retVal = IPC_EFAIL;
    }

    switch(selfId)
    {
/*Assuming that the below conditional check is for J7200, require confirmation for the same*/
#if defined (SOC_J721E) || defined (SOC_J7200) ||defined (SOC_J721S2) || defined (SOC_J784S4)
        case IPC_MPU1_0:
            outIntrBaseNum = NAVSS512_MPU1_0_INPUT_MAILBOX_OFFSET;
            break;
        case IPC_MCU1_0:
            outIntrBaseNum = NAVSS512_MCU1R5F0_INPUT_MAILBOX_OFFSET;
            break;
        case IPC_MCU1_1:
            outIntrBaseNum = NAVSS512_MCU1R5F1_INPUT_MAILBOX_OFFSET;
            break;
        case IPC_MCU2_0:
            outIntrBaseNum = NAVSS512_MCU2R5F0_INPUT_MAILBOX_OFFSET;
            break;
        case IPC_MCU2_1:
            outIntrBaseNum = NAVSS512_MCU2R5F1_INPUT_MAILBOX_OFFSET;
            break;
/*Assuming that the above conditional check is for J7200, require confirmation for the same*/
#if defined (SOC_J721E)
        case IPC_MCU3_0:
            outIntrBaseNum = NAVSS512_MCU3R5F0_INPUT_MAILBOX_OFFSET;
            break;
        case IPC_MCU3_1:
            outIntrBaseNum = NAVSS512_MCU3R5F1_INPUT_MAILBOX_OFFSET;
            break;
        case IPC_C66X_1:
            outIntrBaseNum = NAVSS512_C66X1_INPUT_MAILBOX_OFFSET;
            break;
        case IPC_C66X_2:
            outIntrBaseNum = NAVSS512_C66X2_INPUT_MAILBOX_OFFSET;
            break;
        case IPC_C7X_1:
            outIntrBaseNum = NAVSS512_C7X1_INPUT_MAILBOX_OFFSET;
            break;
#endif
#elif defined (SOC_AM62X)
		case IPC_M4FSS0_0:
            outIntrBaseNum = NAVSS512_MCU2M4F0_INPUT_MAILBOX_OFFSET;
            break;
		case IPC_R5FSS0_0:
            outIntrBaseNum = NAVSS512_MCU2R5F1_INPUT_MAILBOX_OFFSET;
            break;
#elif defined (SOC_AM62AX) || defined(SOC_AM62PX) || defined(SOC_J722S)
        case IPC_MCU_R5FSS0_0:
            outIntrBaseNum = NAVSS512_MCU_R5FSS0_0_INPUT_MAILBOX_OFFSET;
            break;
		case IPC_A53SS0_0:
            outIntrBaseNum = NAVSS512_A53SS0_0_INPUT_MAILBOX_OFFSET;
            break;
#endif
        default:
            break;
    }
    cfg->outputIntrNum = outIntrBaseNum + intrCnt;
    cfg->eventId       = vimEventBaseNum + intrCnt;

    return retVal;
}
/*CDD_IPC_CoverageGap_36: Ipc_getMailboxIntrRouterCfg - This API is used in ETH driver, so it will be covered in ETH LDRA
 - MCAL-9188(JIRA task)*/
int32_t Ipc_getMailboxIntrRouterCfg(uint32 selfId, uint32 clusterId,
        uint32 userId, Ipc_MbConfig* cfg, uint32 cnt)
{
    int32_t    retVal         = IPC_SOK;
    uint32   mailboxIntrNum = 0;

    /* Get Navss512 input interrupt number for mailbox */
    mailboxIntrNum = Ipc_getNavss512MailboxInputIntr(clusterId, userId);

    cfg->inputIntrNum  = mailboxIntrNum;
    cfg->priority      = 1U;
    retVal = Ipc_setCoreEventId(selfId, cfg, cnt);

    return retVal;
}



