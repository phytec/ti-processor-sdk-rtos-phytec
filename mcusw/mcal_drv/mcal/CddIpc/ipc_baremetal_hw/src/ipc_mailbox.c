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
 *  \file ipc_mailbox.c
 *
 *  \brief Implementation of ipc Mailbox interaction
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <CddIpc/ipc_baremetal_hw/ipc.h>
#include <CddIpc/ipc_baremetal_hw/soc/ipc_soc.h>
#include <CddIpc/ipc_baremetal_hw/src/ipc_mailbox.h>
#include <CddIpc/ipc_baremetal_hw/mailbox_hw/V0/mailbox.h>

#if defined(SOC_AM62AX) || defined(SOC_AM62PX) || defined(SOC_J722S)
#include <CddIpc/ipc_baremetal_hw/mailbox_hw/V0/V0_1/hw_mailbox.h>
#endif

#if defined(SOC_AM62X)
#include <CddIpc/ipc_baremetal_hw/mailbox_hw/V0/V0_0/hw_mailbox.h>
#endif

#include <string.h>

#include "ipc_priv.h"

/* Fix Me, need not be 256, can be max remote proc / max proc, i.e. 1 as
    structure which uses this is instantiated max proc times. */
#define  IPC_MBOX_MAXFIFO_CNT    16U
/* Fix Me, need not be 256, can be max remote proc / max proc */
#define  IPC_MBOX_MAXDATA        8U

/* ========================================================================== */
/*                             Local Types                                    */
/* ========================================================================== */

typedef struct Ipc_MailboxFifo_s
{
    int32_t                 cfgNdx;
    Mailbox_hwiCallback     func;
    uint32                arg;
    uint32                queueId;
} Ipc_MailboxFifo;

/* mboxData */
typedef struct Ipc_MailboxData_s
{
    uintptr_t         baseAddr;
    uint32          fifoCnt;
    Ipc_MailboxFifo   fifoTable[IPC_MBOX_MAXFIFO_CNT];
    uint32          noMsgCnt;
    uint32          intCnt;
    uint32 	      userId;

} Ipc_MailboxData;

/* ========================================================================== */
/*                             Globals                                        */
/* ========================================================================== */

#define CDD_IPC_START_SEC_VAR_INIT_32
#include "Cdd_Ipc_MemMap.h"

uint32               g_ipc_mBoxCnt = 0U;

#define CDD_IPC_STOP_SEC_VAR_INIT_32
#include "Cdd_Ipc_MemMap.h"

#define CDD_IPC_START_SEC_VAR_NO_INIT_UNSPECIFIED
#include "Cdd_Ipc_MemMap.h"

Ipc_MailboxData        g_ipc_mBoxData[IPC_MBOX_MAXDATA];

#define CDD_IPC_STOP_SEC_VAR_NO_INIT_UNSPECIFIED
#include "Cdd_Ipc_MemMap.h"

#define CDD_IPC_START_SEC_VAR_NO_INIT_PTR
#include "Cdd_Ipc_MemMap.h"

/**
 * \brief Maps mBoxData allocated to a given Remote Processor
 */
uintptr_t       gIpcRProcIdToMBoxDataMap[IPC_MAX_PROCS];

#define CDD_IPC_STOP_SEC_VAR_NO_INIT_PTR
#include "Cdd_Ipc_MemMap.h"

/* ========================================================================== */
/*                           Function Prototypes                              */
/* ========================================================================== */
void Ipc_mailboxInternalCallback(uintptr_t mboxNdx);

/* ========================================================================== */
/*                             Local Functions                                */
/* ========================================================================== */

/* ========================================================================== */
/*                             Functions                                      */
/* ========================================================================== */

#define CDD_IPC_START_SEC_CODE
#include "Cdd_Ipc_MemMap.h"

int32_t Ipc_mailboxModuleStartup (void)
{
    int32_t  retVal = IPC_SOK;

    memset(gIpcRProcIdToMBoxDataMap, 0, sizeof(gIpcRProcIdToMBoxDataMap));

    return retVal;
}

/**
 *  \brief Enable remote processor interrupt
 */

void Ipc_mailboxEnable(uintptr_t baseAddr, uint32 userId, uint32 queueId)
{
    MailboxEnableNewMsgInt(baseAddr, userId, queueId);
}

/**
 *  \brief Disables remote processor interrupt
 */

int32_t Ipc_mailboxSend(uint32 selfId, uint32 remoteProcId, uint32 val,
                        uint32 timeoutCnt)
{
    /*CDD_IPC_CoverageGap_32: No default value is provided for cluster id.*/
    int32_t   status = 0;
    uint32  clusterId;
    uint32  userId;
    uint32  queueId;
    uintptr_t baseAddr;
    uint32  cnt = timeoutCnt;

#ifndef IPC_ONE_CONTEXT_FOR_HISRGATE_HWIGATE
    Ipc_Object      *pObj  = NULL;
    Ipc_OsalPrms    *pOsal = NULL;
    uintptr_t        key   = 0U;

    pObj = getIpcObjInst(0U);
    pOsal = &pObj->initPrms.osalPrms;
#endif /* IPC_ONE_CONTEXT_FOR_HISRGATE_HWIGATE */

    Ipc_getMailboxInfoTx(selfId, remoteProcId,
        &clusterId, &userId, &queueId);
/*CDD_IPC_CoverageGap_32: No default value is provided for cluster id.*/
    if( (clusterId != MAILBOX_CLUSTER_INVALID) && (queueId < 16U) )
    {
        uint32    retVal;

        baseAddr = Ipc_getMailboxBaseAddr(clusterId);

        /* In case of baremetal, the HISR protection and Interrupt disable would
            same. This function is invoked by Virtio_kick() to let other cores
            know that there is new message available.
            Virtio_kick is called under HISR protection
            which means that we should not invoke protection again. As recursive
            interrupt disable is not recommended */
#ifndef IPC_ONE_CONTEXT_FOR_HISRGATE_HWIGATE
        if (NULL != pOsal->disableAllIntr)
        {
            key = pOsal->disableAllIntr();
        }
#endif /* IPC_ONE_CONTEXT_FOR_HISRGATE_HWIGATE */

        do
        {
            retVal = MailboxSendMessage(baseAddr, queueId, val);
            cnt--;
        } while( (cnt != 0U) && (retVal == MESSAGE_INVALID));

        if(MESSAGE_INVALID == retVal)
        {
#ifdef DEBUG_PRINT
            SystemP_printf("Ipc_mailboxSend : BaseAddr 0x%x queue %d value %d failed\n",
                baseAddr,  queueId, val);
#endif
            status = IPC_EFAIL;
        }

#if DEBUG_PRINT
        SystemP_printf("Ipc_mailboxSend : BaseAddr 0x%x queue %d value %d\n",
            baseAddr,  queueId, val);
#endif

#ifndef IPC_ONE_CONTEXT_FOR_HISRGATE_HWIGATE
        if (NULL != pOsal->restoreAllIntr)
        {
            pOsal->restoreAllIntr(key);
        }
#endif /* IPC_ONE_CONTEXT_FOR_HISRGATE_HWIGATE */

    }

    return status;
}

int32_t Ipc_mailboxRegister(uint16 selfId, uint16 remoteProcId,
           Mailbox_hwiCallback func, uint32 arg, uint32 timeoutCnt)
{
    int32_t               retVal = IPC_SOK;
    uint32              clusterId;
    uint32              userId;
    uint32              queueId;
    uintptr_t             baseAddr;
    uint32             n;
    Ipc_MailboxData      *mbox = NULL;
    Ipc_Object           *pObj  = NULL;
    Ipc_OsalPrms         *pOsal = NULL;

    pObj = getIpcObjInst(0U);
    pOsal = &pObj->initPrms.osalPrms;

    Ipc_getMailboxInfoRx(selfId, remoteProcId,
        &clusterId, &userId, &queueId);

    if( (clusterId != MAILBOX_CLUSTER_INVALID) && (IPC_MAX_PROCS > remoteProcId))
    {
        baseAddr = Ipc_getMailboxBaseAddr(clusterId);
    }
    else
    {
        SystemP_printf("Ipc_Mailbox_register : failed Invalid cluster..\n");
        retVal = IPC_EFAIL;
    }

    if(retVal == IPC_SOK)
    {
        for(n = 0; n < g_ipc_mBoxCnt; n++)
        {
            if((baseAddr == g_ipc_mBoxData[n].baseAddr) && (userId == g_ipc_mBoxData[n].userId))
            {
                break;
            }

        }

        /* Get the MailBox Data */
        mbox = &g_ipc_mBoxData[n];

        if(n == g_ipc_mBoxCnt)
        {
            /* Could not find one, this is new entry */
            mbox->baseAddr = baseAddr;
            mbox->fifoCnt  = 0;
            mbox->userId   = userId;

            if (NULL != pOsal->registerIntr)
            {
                /* Do not clear the mailbox, other cores could have already sent messages */

#ifdef MAILBOX_INTERRUPT_MODE
                {
                    Ipc_MbConfig cfg;

                    MailboxClrNewMsgStatus(baseAddr, userId, queueId);

                    /* Register Mailbox interrupt now... */
                    if (retVal == IPC_SOK)
                    {
                        pObj->interruptHandle = pOsal->registerIntr(
                                &cfg,Ipc_mailboxInternalCallback,
                                (uintptr_t)mbox);
                    }

                }
#endif /* MAILBOX_INTERRUPT_MODE */
	    }

            g_ipc_mBoxCnt++;
        }

        /* Add the fifo data for the remoteProcId. */
        mbox->fifoTable[mbox->fifoCnt].cfgNdx  = n;
        mbox->fifoTable[mbox->fifoCnt].func    = func;
        mbox->fifoTable[mbox->fifoCnt].arg     = arg;
        mbox->fifoTable[mbox->fifoCnt].queueId = queueId;
        gIpcRProcIdToMBoxDataMap[remoteProcId] = (uintptr_t)mbox;

        mbox->fifoCnt++;

#ifdef DEBUG_PRINT
        SystemP_printf("Ipc_MB(%d): Self %d Remote %d (c%d,u%d,q%d) arg %d,total %d\n",
                mbox->fifoCnt, selfId, remoteProcId, clusterId, userId, queueId, arg,
                g_ipc_mBoxCnt);
#endif
    }

    return retVal;
}

/** \brief Low Level Mailbox ISR for a given remote processor */
void Ipc_mailboxIsr(uint32 remoteProcId)
{
    uintptr_t mBoxData = 0U;

    if (IPC_MAX_PROCS > remoteProcId)
    {
        mBoxData = gIpcRProcIdToMBoxDataMap[remoteProcId];
        if (0U != mBoxData)
        {
            Ipc_mailboxInternalCallback(mBoxData);
        }
        else
        {
          /* Do Nothing */
        }
    }
    else
    {
      /* Do Nothing */
    }

    return;
}

/*!
 *  ======== Ipc_mailboxInternalCallback ========
 */

void Ipc_mailboxInternalCallback(uintptr_t arg)
{
    uint32              n;
    Ipc_MailboxData      *mbox;
    uint32              msg[4];
    Ipc_MailboxFifo      *fifo;

    mbox = (Ipc_MailboxData *)arg;
    if(mbox != NULL)
    {
        mbox->intCnt++;

        for(n = 0; n < mbox->fifoCnt; n++)
        {
            fifo = &mbox->fifoTable[n];

            if(0U != MailboxGetRawNewMsgStatus(mbox->baseAddr, mbox->userId, fifo->queueId))
            {
                if( MailboxGetMessageCount(mbox->baseAddr, fifo->queueId) > 0U)
                {
                    /* Get the message from Mailbox fifo */
                    MailboxGetMessage(mbox->baseAddr, fifo->queueId, msg);

                    /* Clear new message status of Mailbox */
                    MailboxClrNewMsgStatus(mbox->baseAddr, mbox->userId,
                            fifo->queueId);

                    /* Call the function with arg */
                    (mbox->fifoTable[n].func)(msg, fifo->arg);
                }
                else
                {
                /*CDD_IPC_CoverageGap_34: There is only one msg in the queue always so it does not excute the else part. */
                    /* Clear new message status of Mailbox */
                    MailboxClrNewMsgStatus(mbox->baseAddr, mbox->userId, fifo->queueId);
                }
            }
        }
        if(n == mbox->fifoCnt)
        {
            mbox->noMsgCnt++;
        }
    }
}

void Ipc_mailboxEnableNewMsgInt(uint32 selfId, uint32 remoteProcId)
{
    uint32  clusterId;
    uint32  userId;
    uint32  queueId;
    uintptr_t baseAddr;

    if ((IPC_MAX_PROCS > selfId) && (IPC_MAX_PROCS > remoteProcId))
    {
        Ipc_getMailboxInfoRx(selfId, remoteProcId, &clusterId, &userId,
                                &queueId);

        baseAddr = Ipc_getMailboxBaseAddr(clusterId);

        Ipc_mailboxEnable(baseAddr, userId, queueId);
    }
    else
    {
      /* Do Nothing */
    }
}

/** \returns status of mailbox for a given remote processor */
uint32 Ipc_mailboxGetStatus(uint32 remoteProcId)
{
    uintptr_t mBoxData = 0U;
    Ipc_MailboxData      *mbox;
    Ipc_MailboxFifo      *fifo;
    uint32 retVal = 0;

    if ( remoteProcId < IPC_MAX_PROCS )
    {
        mBoxData = gIpcRProcIdToMBoxDataMap[remoteProcId];
        if (0U != mBoxData)
        {
          mbox = (Ipc_MailboxData *)mBoxData;
          if(mbox != NULL)
          {
              for(uint32 n = 0; n < mbox->fifoCnt; n++)
              {
                  fifo = &mbox->fifoTable[n];
                  retVal = MailboxGetRawNewMsgStatus(mbox->baseAddr, mbox->userId, fifo->queueId);
                }
            }
            else
            {
              /* Do Nothing */
            }
        }
        else
        {
          /* Do Nothing */
        }
    }
    else
    {
      /* Do Nothing */
    }

    return retVal;
}

Std_ReturnType IpcRegisterReadBack(uint32 remoteProcId,Cdd_IpcRegRbValues *pRegArgs)
{
  uint32              clusterId;
  uint32              userId;
  uint32              queueId;
  uintptr_t             baseAddr;

  Std_ReturnType retVal = E_NOT_OK;

  uint32 selfId = Ipc_mpGetSelfId();
  Ipc_getMailboxInfoRx(selfId, remoteProcId,
      &clusterId, &userId, &queueId);
  baseAddr = Ipc_getMailboxBaseAddr(clusterId);

 /* Read configuration registers value */
 if (pRegArgs != NULL_PTR)
 {
   pRegArgs ->cddipcRev       = HW_RD_REG32(baseAddr + MAILBOX_REVISION);
   pRegArgs ->cddipcsyscfg    = HW_RD_REG32(baseAddr + MAILBOX_SYSCONFIG);
   pRegArgs ->cddipceoi       = HW_RD_REG32(baseAddr + MAILBOX_IRQ_EOI);
   pRegArgs ->cddipcmsg       = HW_RD_REG32(baseAddr + MAILBOX_MSGSTATUS(queueId));
   pRegArgs ->cddipcfifosts   = HW_RD_REG32(baseAddr + MAILBOX_FIFOSTATUS(queueId));
   pRegArgs ->cddipcmsgsts    = HW_RD_REG32(baseAddr + MAILBOX_MSGSTATUS(queueId));
   pRegArgs ->cddipcirqrawsts = HW_RD_REG32(baseAddr + MAILBOX_IRQSTATUS_RAW(userId));
   pRegArgs ->cddipcclrsts    = HW_RD_REG32(baseAddr + MAILBOX_IRQSTATUS_CLR(userId));
   pRegArgs ->cddipcenbclr    = HW_RD_REG32(baseAddr + MAILBOX_IRQENABLE_CLR(userId));
   pRegArgs ->cddipcenbset    = HW_RD_REG32(baseAddr + MAILBOX_IRQENABLE_SET(userId));
   retVal = E_OK;
 }
 else
 {
   /* Do Nothing */
 }

  return retVal;

}

#define CDD_IPC_STOP_SEC_CODE
#include "Cdd_Ipc_MemMap.h"
