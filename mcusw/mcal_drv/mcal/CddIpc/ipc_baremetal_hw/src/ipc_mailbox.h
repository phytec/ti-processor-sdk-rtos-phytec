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
 *  \file ipc_mailbox.h
 *
 *  \brief Declaration for Mailbox wrapper implementation.
 */

#ifndef IPC_MAILBOX_H_
#define IPC_MAILBOX_H_

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <CddIpc/ipc_baremetal_hw/include/ipc_types.h>
#include "Cdd_Ipc.h"
#include <hw_include/lld_mailbox.h>
/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

#define  MAILBOX_CLUSTER_INVALID   (0xFFU)
#define  MAILBOX_USER_INVALID      (0xFFU)

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

typedef void (*Mailbox_hwiCallback)(uint32* arg1, uint32 arg2);

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/** \brief Initializes mailbox module */
int32_t Ipc_mailboxModuleStartup (void);
void Ipc_mailboxEnable(uintptr_t baseAddr, uint32 userId, uint32 queueId);

int32_t Ipc_mailboxSend(uint32 selfId, uint32 remoteProcId, uint32 val,
                        uint32 timeoutCnt);
int32_t Ipc_mailboxRegister(uint16 selfId, uint16 remoteProcId,
            Mailbox_hwiCallback func, uint32 arg, uint32 timeoutCnt);

/** \brief Handles Interrupts from remote cores.
 *          Expected to be invoked on occurrence of new message in mailbox FIFO
 *
 *  \param  uint32    Valid Remote Processors identifier
 *
 *  \return None
 */
void Ipc_mailboxIsr(uint32 remoteProcId);

uint32 Ipc_mailboxGetStatus(uint32 remoteProcId);

Std_ReturnType IpcRegisterReadBack(uint32 remoteProcId,Cdd_IpcRegRbValues *pRegArgs);

/* ========================================================================== */
/*                       Static Function Definitions                          */
/* ========================================================================== */

/* None */

#ifdef __cplusplus
}
#endif

#endif /* #ifndef IPC_MAILBOX_H_ */
