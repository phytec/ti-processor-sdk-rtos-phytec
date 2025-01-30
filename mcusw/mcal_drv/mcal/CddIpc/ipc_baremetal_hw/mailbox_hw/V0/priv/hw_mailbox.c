/*
 *  Copyright (C) 2024 Texas Instruments Incorporated - http://www.ti.com/
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
 *
 */
/**
 *  \file  hw_mailbox.c
 *
 *  \brief Mailbox Device Abstraction Layer APIs
 *
 *   This file contains the device abstraction layer APIs for the
 *   mailbox module. These are used for IPC communication.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
/** \brief This is to disable HW_SYNC_BARRIER for register access */
#define MEM_BARRIER_DISABLE

#include "stdint.h"
#include <hw_include/lld_mailbox.h>

#if defined(SOC_AM62X)
#include <hw_include/soc/am62x/src/lldr_soc_baseaddress.h>
#include <CddIpc/ipc_baremetal_hw/mailbox_hw/V0/mailbox.h>
#include <CddIpc/ipc_baremetal_hw/mailbox_hw/V0/V0_0/hw_mailbox.h>
#endif

#if defined(SOC_AM62AX) || defined(SOC_AM62PX) || defined(SOC_J722S)
#include <CddIpc/ipc_baremetal_hw/mailbox_hw/V0/mailbox.h>
#include <CddIpc/ipc_baremetal_hw/mailbox_hw/V0/V0_1/hw_mailbox.h>
#endif

/* ========================================================================== */
/*                          Function Definitions                             */
/* ========================================================================== */

#define CDD_IPC_START_SEC_CODE
#include "Cdd_Ipc_MemMap.h"

void MailboxReset(uint32 baseAddr)
{
    /*    Start the soft reset sequence    */
    /* write SOFTRESET field */
    HW_WR_FIELD32(baseAddr + MAILBOX_SYSCONFIG, MAILBOX_SYSCONFIG_SOFTRESET,
                  MAILBOX_SYSCONFIG_SOFTRESET_B1);

    /*    Wait till the reset is complete    */
    while (MAILBOX_SYSCONFIG_SOFTRESET_B1 ==
           HW_RD_FIELD32(baseAddr + MAILBOX_SYSCONFIG,
                         MAILBOX_SYSCONFIG_SOFTRESET))
    {
        /* Do nothing - Busy wait */
    }

    return;
}
uint32 MailboxGetMessage(uint32 baseAddr, uint32 queueId,
                           uint32 *msgPtr)
{
    uint32 msgCount;
    uint32 retval;

    msgCount = MailboxGetMessageCount(baseAddr, queueId);

    if (msgCount > 0U)
    {
        /*    Read message    */
        *msgPtr = HW_RD_REG32(baseAddr + MAILBOX_MESSAGE(queueId));
        retval  = MESSAGE_VALID;
    }
    else
    {
        /*    Queue empty*/
        retval = MESSAGE_INVALID;
    }

    return retval;
}

void MailboxReadMessage(uint32 baseAddr, uint32 queueId,
                        uint32 *msgPtr)
{
        /*    Read message    */
        *msgPtr = HW_RD_REG32(baseAddr + MAILBOX_MESSAGE(queueId));
}

uint32 MailboxSendMessage(uint32 baseAddr, uint32 queueId, uint32 msg)
{
    uint32 fifoFull;
    uint32 retval;

    /* Read the FIFO Status */
    fifoFull = HW_RD_REG32(baseAddr + MAILBOX_FIFOSTATUS(queueId));
    if (fifoFull == 0U)
    {
        /* FIFO not full write msg */
        MailboxWriteMessage(baseAddr, queueId, msg);
        retval = MESSAGE_VALID;
    }
    else
    {
        retval = MESSAGE_INVALID;
    }

    return retval;
}

void MailboxEnableNewMsgInt(uint32 baseAddr, uint32 userId,
                            uint32 queueId)
{
    /* Set the NewMsgEnable field - Writing zero has no effect */
    HW_WR_REG32(baseAddr + MAILBOX_IRQENABLE_SET(userId), (uint32) 0x1 <<
                (queueId * 2U));
}

void MailboxEnableQueueNotFullInt(uint32 baseAddr, uint32 userId,
                                  uint32 queueId)
{
    /* Set the FIFO Not Full field - Writing zero has no effect */
    HW_WR_REG32(baseAddr + MAILBOX_IRQENABLE_SET(userId), (uint32) 0x2 <<
                (queueId * 2U));
}

void MailboxDisableNewMsgInt(uint32 baseAddr, uint32 userId,
                             uint32 queueId)
{
    /* Set the NewMsgEnable field - Writing zero has no effect */
    HW_WR_REG32(baseAddr + MAILBOX_IRQENABLE_CLR(userId), (uint32) 0x1 <<
                (queueId * 2U));
}

void MailboxDisableQueueNotFullInt(uint32 baseAddr, uint32 userId,
                                   uint32 queueId)
{
    /* Set the FIFO Not Full field - Writing zero has no effect */
    HW_WR_REG32(baseAddr + MAILBOX_IRQENABLE_CLR(userId), (uint32) 0x2 <<
                (queueId * 2U));
}

void MailboxClrNewMsgStatus(uint32 baseAddr, uint32 userId,
                            uint32 queueId)
{
    /* Set the NewMsgEnable field - Writing zero has no effect */
    HW_WR_REG32(baseAddr + MAILBOX_IRQSTATUS_CLR(userId), (uint32) 0x1 <<
                (queueId * 2U));
}

void MailboxClrQueueNotFullStatus(uint32 baseAddr, uint32 userId,
                                  uint32 queueId)
{
    /* Set the NewMsgEnable field - Writing zero has no effect */
    HW_WR_REG32(baseAddr + MAILBOX_IRQSTATUS_CLR(userId), (uint32) 0x2 <<
                (queueId * 2U));
}

uint32 MailboxGetRawNewMsgStatus(uint32 baseAddr,
                                   uint32 userId,
                                   uint32 queueId)
{
    uint32 regVal;

    /* Read the IRQSTATUSRAW */
    regVal = HW_RD_REG32(baseAddr + MAILBOX_IRQSTATUS_RAW(userId));

    /* Mask & read the NewMsgStatus for given queueId */
    regVal &= (uint32) 0x1 << (queueId * 2U);

    return (regVal >> (queueId * 2U));
}

uint32 MailboxGetRawQueueNotFullStatus(uint32 baseAddr,
                                         uint32 userId,
                                         uint32 queueId)
{
    uint32 regVal;

    /* Read the IRQSTATUSRAW */
    regVal = HW_RD_REG32(baseAddr + MAILBOX_IRQSTATUS_RAW(userId));

    /* Mask & read the FIFO Not full for given queueId */
    regVal &= (uint32) 0x2 << (queueId * 2U);

    return (regVal >> (queueId * 2U));
}

uint32 MailboxGetIrqEnableStatus(uint32 baseAddr,
                                   uint32 userId,
                                   uint32 queueId)
{
    uint32 regVal;

    /* Read the IRQENABLESET */
    regVal = HW_RD_REG32(baseAddr + MAILBOX_IRQENABLE_SET(userId));

    /* Mask & read the FIFO Not full for given queueId */
    regVal &= (uint32) 0x1 << (queueId * 2U);

    return (regVal);
}

uint32 MailboxGetMessageCount(uint32 baseAddr,
                                uint32 queueId)
{
    /* Return message count */
    return (HW_RD_REG32(baseAddr + MAILBOX_MSGSTATUS(queueId)));
}

void MailboxWriteMessage(uint32 baseAddr, uint32 queueId, uint32 msg)
{
    /* Write mailbox message */
    HW_WR_REG32(baseAddr + MAILBOX_MESSAGE(queueId), msg);
}

void MailboxWriteEOI(uint32 baseAddr, uint32 value)
{
    /* Write mailbox message */
    HW_WR_REG32(baseAddr + MAILBOX_IRQ_EOI, value);
}

#define CDD_IPC_STOP_SEC_CODE
#include "Cdd_Ipc_MemMap.h"

/********************************* End of file ******************************/
