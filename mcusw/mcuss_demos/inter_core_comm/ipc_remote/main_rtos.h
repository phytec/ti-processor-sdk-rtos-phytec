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
 *  \file main_rtos.h
 *
 *  \brief
 *
 *
 */

#ifndef IPC_REMOTE_APP_H_
#define IPC_REMOTE_APP_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

#if defined (SOC_J7200)
#define IPC_REMOTE_APP_VRING_BASE_ADDRESS   (0xA4000000)
/**< Base address of shared memory */
#define IPC_REMOTE_APP_VRING_BUFFER_SIZE    (0x800000)
/**< Size of shared memory */

#elif defined (SOC_J721E)
#define IPC_REMOTE_APP_VRING_BASE_ADDRESS   (0xAA000000)
/**< Base address of shared memory */
#define IPC_REMOTE_APP_VRING_BUFFER_SIZE    (0x1C00000)
/**< Size of shared memory */


#elif defined (SOC_J784S4)
#define IPC_REMOTE_APP_VRING_BASE_ADDRESS   (0xAC000000)
/**< Base address of shared memory */
#define IPC_REMOTE_APP_VRING_BUFFER_SIZE    (0x3000000)
/**< Size of shared memory */


#elif defined (SOC_J721S2)
#define IPC_REMOTE_APP_VRING_BASE_ADDRESS   (0xA8000000)
/**< Base address of shared memory */
#define IPC_REMOTE_APP_VRING_BUFFER_SIZE    (0x1C00000)
/**< Size of shared memory */
#endif /* SOC_J721S2 */

#define IPC_REMOTE_APP_RPMSG_OBJ_SIZE       (256U)
/**< RP Message Object Size */
#define IPC_REMOTE_APP_RPMSG_DATA_SIZE      (256U * (496U + 32U) + \
                                                IPC_REMOTE_APP_RPMSG_OBJ_SIZE)
/**< Message to stored received messages. 256 messages of (496 bytes +
    Size of RPMessage_MsgElem) + space for book-keeping */
#define IPC_REMOTE_APP_VQ_BUF_SIZE          (2048U)
/**< Space required for VirtIO Queue objects, container to hold received
        message pointer */

#define IPC_REMOTE_APP_LOCAL_EP_MCU1_0      (1U)
/**< End point to communicate with */
#define IPC_REMOTE_APP_LOCAL_EP_MCU1_1      (6U)
/**< End point to communicate with */
#define IPC_REMOTE_APP_LOCAL_EP_MPU1_0      (11U)
/**< End point to communicate with */
#define IPC_REMOTE_APP_LOCAL_EP_MCU2_0      (16U)
/**< End point to communicate with */
#define IPC_REMOTE_APP_LOCAL_EP_MCU2_1      (21U)
/**< End point to communicate with */

#define IPC_REMOTE_APP_BLOCKON_ANNOUNCE     (1U)
/**< Flag to configure application to pend on remote end points.
        1 Blocks and 0 do not wait for remote end point */
/* ========================================================================== */
/*                          External Dependencies                             */
/* ========================================================================== */
extern uint32_t IpcRemoteApp_SelfProcId;
/**< Own Proc ID */
extern uint32_t IpcRemoteApp_NumProc;
/**< Number of remote cores */
extern uint32_t IpcRemoteApp_remoteProc[];
/**< Remote Proc ID's */
extern uint8_t IpcRemoteApp_SysVqBuf[IPC_REMOTE_APP_VQ_BUF_SIZE];
/**< VirtIO Queue Object */
extern uint8_t IpcRemoteApp_CtrlTaskMsgBuf[IPC_REMOTE_APP_RPMSG_DATA_SIZE];
/**< Reserved EP / Control EP message buffer */
/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

#ifdef __cplusplus
}
#endif

#endif  /* #define IPC_REMOTE_APP_H_ */
