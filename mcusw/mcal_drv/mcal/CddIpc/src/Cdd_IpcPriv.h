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
 *  \file  Cdd_IpcPriv.h
 *
 *  \brief This file defines interfaces / constants required for IPC
 *          implementation
 *
 */

#ifndef CDD_IPC_PRIV_H_
#define CDD_IPC_PRIV_H_

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#if (STD_ON == CDD_IPC_DEV_ERROR_DETECT)
#include "Det.h"
#endif
#include "Dem.h"
#include "SchM_Cdd_Ipc.h"

#include <CddIpc/ipc_baremetal_hw/ipc.h>


/* ========================================================================== */
/*                                 Macros                                     */
/* ========================================================================== */
#define CDD_IPC_RESERVED_END_POINT      (53U)
/**< Reserved End Point, would be used by HLOS (such as QNX, linux) */
#define CDD_IPC_CNTRLMSG_ANNOUNCE       (0x00000000)
/**< Announce control message type */
/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */
typedef struct
{
    uint32      isValid;
    /**< Flag indicating active map, 0 for INVALID */
    uint32      remoteEp;
    /**< Associated Local End Point */
    uint32      commChId;
    /**< Associated Processor ID */
}Cdd_IpcMapProcId2ComId;

/**
 *  \brief CDD Ipc object structure.
 */
typedef struct
{
    uint32              ownCoreId;
    /**< Core ID on which IPC CDD is hosted on / own processor ID */

#if (STD_ON == CDD_IPC_ANNOUNCE_API)
    RPMessage_Handle    ctrlRpmsgHndl;
    /**< Control End point RP MSG Handle */
    uint32              ctrlEp;
    /**< Control End Point */
#endif /* #if (STD_ON == CDD_IPC_ANNOUNCE_API) */

    /* Two tables used, 1 to hold details required for TX and other to hold
        details required for RX. TX will use commChId as index into generated
        channel Config*/
    Cdd_IpcMapProcId2ComId  mapToCommId[CDD_IPC_CORE_ID_MAX]
                                                    [CDD_IPC_MAX_CHANNEL_CFG];
    /**< Details required to map from newMsgCb to associated commChId */
    RPMessage_Handle    rpmsgHndl[CDD_IPC_MAX_CHANNEL_CFG];
    /**< Associated RP MSG Handles, indexed on comm id */

} Cdd_IpcObjType;

/**
 *  \brief Control Message Type
 */
typedef struct
{
    uint32  type;
    /* Type of control message */
} Cdd_IpcCtrlMsgType;

/**
 *  \brief Control Message Type
 */
typedef struct
{
    uint8               name[CDD_IPC_MAX_CTRL_MSG_LEN];
    /**< String */
    uint32              endPt;
    /**< End point annoucing the message */
    Cdd_IpcCtrlMsgType  ctrl;
    /**< Type of control message */
} Cdd_IpcAnnouncement;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */


#ifdef __cplusplus
}
#endif

#endif /* CDD_IPC_PRIV_H_ */
/********************************* End of file ******************************/
