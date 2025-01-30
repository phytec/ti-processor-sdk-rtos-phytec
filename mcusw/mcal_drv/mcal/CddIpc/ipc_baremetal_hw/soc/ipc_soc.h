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
 *  \file ipc_soc.h
 *
 *  \brief IPC Low Level Driver SOC specific file.
 */

#ifndef IPC_SOC_TOP_H_
#define IPC_SOC_TOP_H_

#ifdef __cplusplus
extern "C" {
#endif

#define    IPC_INVALID_PROCID (0XFFU)   /**< Invalid Proc ID */


/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#if defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_AM62X) ||  defined(SOC_AM62AX) || defined(SOC_AM62PX) || defined(SOC_J722S)
#include <CddIpc/ipc_baremetal_hw/soc/V1/ipc_soc.h>

#elif defined (SOC_J721S2)
#include <CddIpc/ipc_baremetal_hw/soc/V2/ipc_soc.h>

#elif defined (SOC_J784S4)
#include <CddIpc/ipc_baremetal_hw/soc/V3/ipc_soc.h>

#endif
/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */
int32_t Ipc_getMailboxInfoTx(uint32 selfId, uint32 remoteId,
                 uint32 *clusterId, uint32 *userId, uint32 *queueId);
int32_t Ipc_getMailboxInfoRx(uint32 selfId, uint32 remoteId,
                 uint32 *clusterId, uint32 *userId, uint32 *queueId);
uintptr_t Ipc_getMailboxBaseAddr(uint32 clusterId);
int32_t Ipc_getMailboxIntrRouterCfg(uint32 selfId, uint32 clusterId,
                 uint32 userId, Ipc_MbConfig* cfg, uint32 cnt);
/**
 * \brief Returns the core name for get core id
 *
 * \param procId [IN] Id of desired core.
 *
 * \return name of the given core id
 * */
const char* Ipc_getCoreName(uint32 procId);

/**
 * \brief Returns Core ID based on core build flag
 *
 * \return Code ID of the current core
 **/
/*CDD_IPC_CoverageGap_35: Ipc_getCoreId() - This API is used in ETH driver, so it will be covered in ETH LDRA
 - MCAL-9188(JIRA task)*/
uint32 Ipc_getCoreId(void);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef IPC_SOC_TOP_H_ */
