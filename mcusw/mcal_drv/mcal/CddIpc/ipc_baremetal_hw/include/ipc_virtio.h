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
 *  \ingroup DRV_IPC_MODULE
 *  \defgroup DRV_IPC_VIRTIO_MODULE IPC Driver VirtIO configuration
 *            This is documentation for VirtIO configurations used by IPC
 *
 *  @{
 */

/**
 *  \file ipc_virtio.h
 *
 *  \brief VirtIO Interface for application.
 */

#ifndef IPC_VIRTIO_H_
#define IPC_VIRTIO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <CddIpc/ipc_baremetal_hw/soc/ipc_soc.h>
#include <CddIpc/ipc_baremetal_hw/include/ipc_types.h>
/**
 *  \brief Parameter structure for creating VirtIO table for each core
 *  combinations.
 */
typedef struct Ipc_VirtIoParams_s
{
    void      *vqObjBaseAddr;
    /**< Base address for storing VQ Object */

    uint32  vqBufSize;
    /**< Size of the Buffer for storing VQ objects */

    void      *vringBaseAddr;
    /**<  Base address for Shared VRing for all cores */

    uint32  vringBufSize;
    /**< Buffer Size. Recommemded Size is 0x40000 * NumOfCores */

    uint32  timeoutCnt;
    /**< timeoutCnt. Set 0xFFFFFFFF for wait forever */
} Ipc_VirtIoParams;




/**
 * \brief Returns local memory for Virtio objects for one core-pair
 */
uint32 Ipc_getVqObjMemoryRequiredPerCore(void);

/**
 *  \brief      Initailize the Virtio module
 * 
 *  \return      #IPC_SOK or #IPC_EFAIL
 */
int32_t Ipc_initVirtIO(Ipc_VirtIoParams *vqParam);


/**
 *  \brief     Loads the resource table. If the remote
 *             core A72, and has valid radource table
 *             then, it will wait for Linux to be ready
 *             use address from resource table.
 *
 *  \param rsctable [IN] Pointer of resource table
 *
 *  \return      #IPC_SOK or #IPC_EFAIL
 */
int32_t Ipc_loadResourceTable(void *rsctable);

/**
 *  \brief     Gets the address of the trace buffer.
 *
 *  \return    Address of the trace buffer.
 */
void * Ipc_getResourceTraceBufPtr(void);
void* Ipc_allocVirtio(void);
void Virtio_isr(uint32* msg, uint32 priv);


/**
 * \brief  Checks if remote is ready
 *
 * \param procId [IN] Id of remote core
 *
 * \return  1 if remote proc is is not A72 or resource table is null
 *            or linux vdev status is 0x7
 *          0 if linux vdev status is not 0x7
 *
 */
uint8 Ipc_isRemoteReady(uint16 procId);
uint8 Ipc_isRemoteVirtioCreated(uint32 remoteId);

/**
 * \brief Creates Virtio late when Linux is ready
 *
 * \param procId  [IN] Id of remote processor
 *
 * \return #IPC_SOK or #IPC_EFAIL
 */

int32_t Ipc_physToVirt(uint32 pa, uintptr_t *va);
int8_t Ipc_virtToPhys(uintptr_t va, uint32 *pa);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef IPC_VIRTIO_H_ */

/* @} */
