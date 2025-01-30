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
 *  \file     Eth_VirtMac.c
 *
 *  \brief    This file contains the implementation of the main APIs
 *            of the Ethernet driver.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "Eth.h"
#include "Eth_VirtMac.h"
#include "Dem.h"
#if (STD_ON == ETH_DEV_ERROR_DETECT)
#include "Det.h"
#endif
#include "Eth_Rpc.h"
#include "Eth_RpcPriv.h"
#if (STD_ON == ETH_VIRTUALMAC_SUPPORT)
#include <string.h>
#include "Cdd_Ipc.h"
#endif

/* ========================================================================== */
/*                  Global Variable Declaration                             */
/* ========================================================================== */
#define ETH_START_SEC_VAR_INIT_8
#include "Eth_MemMap.h"
extern volatile VAR(Eth_StateType, ETH_VAR_CLEARED) Eth_DrvStatus;
#define ETH_STOP_SEC_VAR_INIT_8
#include "Eth_MemMap.h"

#define  ETH_START_SEC_VAR_NO_INIT_UNSPECIFIED
#include "Eth_MemMap.h"
Eth_VirtualMacInfo  Eth_VirtMacInfo[ETH_CTRL_ID_MAX];
#define  ETH_STOP_SEC_VAR_NO_INIT_UNSPECIFIED
#include "Eth_MemMap.h"

/* ========================================================================== */
/*                  Internal Function Declaration                             */
/* ========================================================================== */
#if (STD_ON == ETH_VIRTUALMAC_SUPPORT)
static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkVirtualmacErrors(uint8 ctrlIdx, uint8 apiId);

static FUNC(void, ETH_CODE)
Eth_virtMacProcessState(uint8 ctrlIdx);
#endif /* (STD_ON == ETH_VIRTUALMAC_SUPPORT) */

/* ========================================================================== */
/*                  Global Function Definitions                               */
/* ========================================================================== */

#if (STD_ON == ETH_VIRTUALMAC_SUPPORT)
FUNC(Std_ReturnType, ETH_CODE)
Eth_VirtMacRpcInit(P2CONST(Eth_ConfigType, AUTOMATIC, ETH_PBCFG) CfgPtr)
{
    Std_ReturnType retVal = E_OK;
    UCHAR *announceMsg = ((UCHAR *)ETHREMOTECFG_AUTOSAR_REMOTE_SERVICE_NAME);
    uint8 ctrlIdx = 0;

#if (STD_ON == ETH_PRE_COMPILE_VARIANT)
    if (NULL_PTR != CfgPtr)
    {
    #if (STD_ON == ETH_DEV_ERROR_DETECT)
        Eth_reportDetError(ETH_SID_INIT, ETH_E_INV_PARAM);
    #endif
        retVal = E_NOT_OK;
    }
#else
    if (NULL_PTR == CfgPtr)
    {
    #if (STD_ON == ETH_DEV_ERROR_DETECT)
        Eth_reportDetError(ETH_SID_INIT, ETH_E_INV_PARAM);
    #endif
        retVal = E_NOT_OK;
    }
    else
    {
        Eth_CfgPtr = (Eth_ConfigType *)CfgPtr;
    }
#endif

    if (E_OK == retVal)
    {
        for (ctrlIdx = 0u; ctrlIdx < ETH_CTRL_ID_MAX; ++ctrlIdx)
        {
#if (STD_ON == ETH_DEV_ERROR_DETECT)
            if ((Cdd_IpcIsInitDone() != TRUE) ||
                (Cdd_IpcGetMaxMsgSize(Eth_VirtMacGetEthFwRpcComChannelId(ctrlIdx)) <
                 ETHREMOTECFG_IPC_MSG_PAYLOAD_SIZE))
            {
                retVal = E_NOT_OK;
            }
#endif
            if (retVal == E_OK)
            {
                memset(&Eth_VirtMacInfo[ctrlIdx], 0, sizeof(Eth_VirtualMacInfo));
                Eth_VirtMacInfo[ctrlIdx].token = ETHREMOTECFG_TOKEN_NONE;
                Eth_VirtMacInfo[ctrlIdx].s2CNotifyId  = ETHREMOTECFG_NOTIFY_LAST;
                Eth_VirtMacInfo[ctrlIdx].teardownResponsePending = FALSE;
                retVal = Cdd_IpcAnnounce(announceMsg, Eth_VirtMacGetEthFwRpcComChannelId(ctrlIdx));
            }
        }
    }

    return retVal;
}
#endif

#if (STD_ON == ETH_VIRTUALMAC_SUPPORT)
/*
 * Design:
 * Requirements:
 */
FUNC(Std_ReturnType, ETH_CODE)
Eth_DispatchVirtmacInit(uint8 CtrlIdx)
{
    Std_ReturnType retVal = E_OK;

#if (STD_ON == ETH_DEV_ERROR_DETECT)
    if (CtrlIdx >= ETH_CTRL_ID_MAX)
    {
        Eth_reportDetError(ETH_SID_DISPATCH_VIRTMAC_INIT, ETH_E_INV_CTRL_IDX);
    }
    else
    {
        retVal = E_OK;
    }
    if ((Eth_VirtMacInfo[CtrlIdx].token != ETHREMOTECFG_TOKEN_NONE) ||
        (Eth_VirtMacInfo[CtrlIdx].detachResponsePending == TRUE))
    {
        /* Detach not done . Eth_DispatchVirtmacDeinit not invoked or RESPONSE not yet received */
        Eth_reportDetError(ETH_SID_DISPATCH_VIRTMAC_INIT, ETH_E_UNINIT);
        retVal = E_NOT_OK;
    }
#endif

    if (E_OK == retVal)
    {
        retVal = Eth_RpcSendExtendedAttachReq(CtrlIdx);
    }
    return retVal;
}
#endif /* (STD_ON == ETH_VIRTUALMAC_SUPPORT) */

#if (STD_ON == ETH_VIRTUALMAC_SUPPORT)
/*
 * Design:
 * Requirements:
 */
FUNC(Std_ReturnType, ETH_CODE)
Eth_DispatchVirtmacDeinit(uint8 CtrlIdx)
{
    Std_ReturnType retVal = E_OK;

#if (STD_ON == ETH_DEV_ERROR_DETECT)
    retVal = Eth_checkVirtualmacErrors(CtrlIdx, ETH_SID_DISPATCH_VIRTMAC_DEINIT);
    if (Eth_VirtMacInfo[CtrlIdx].token == ETHREMOTECFG_TOKEN_NONE)
    {
        /* Detach not done . Eth_DispatchVirtmacDeinit not invoked or RESPONSE not yet received */
        Eth_reportDetError(ETH_SID_DISPATCH_VIRTMAC_DEINIT, ETH_E_UNINIT);
        retVal = E_NOT_OK;
    }
#endif

    if (E_OK == retVal)
    {
        retVal = Eth_RpcSendDetachReq(CtrlIdx);
    }
    return retVal;
}
#endif /* (STD_ON == ETH_VIRTUALMAC_SUPPORT) */


#if ((STD_ON == ETH_VIRTUALMAC_SUPPORT) && (STD_ON == ETH_VIRTUALMAC_NOTIFYMSGRECEIVED_API))
/*
 * Design:
 * Requirements:
 */
FUNC(Std_ReturnType, ETH_CODE)
Eth_NotifyVirtmacMsgReceived(uint8 CtrlIdx)
{
    Std_ReturnType retVal = E_OK;

#if (STD_ON == ETH_DEV_ERROR_DETECT)
    if (CtrlIdx >= ETH_CTRL_ID_MAX)
    {
        Eth_reportDetError(ETH_SID_NOTIFY_VIRTMAC_MSGRECV, ETH_E_INV_CTRL_IDX);
    }
    else
    {
        retVal = E_OK;
    }
    #endif

    if (E_OK == retVal)
    {
        retVal = Eth_RpcRecvMsg(CtrlIdx, ETH_SID_NOTIFY_VIRTMAC_MSGRECV);
    }
    return retVal;
}
#endif /* ((STD_ON == ETH_VIRTUALMAC_SUPPORT) && (STD_ON == ETH_VIRTUALMAC_NOTIFYMSGRECEIVED_API)) */

#if ((STD_ON == ETH_VIRTUALMAC_SUPPORT) && (STD_ON == ETH_VIRTUALMAC_SUBSCRIBEALLTRAFFIC_API))
/*
 * Design:
 * Requirements:
 */
FUNC(Std_ReturnType, ETH_CODE)
Eth_DispatchVirtmacSubscribeAllTraffic(uint8 CtrlIdx)
{
    Std_ReturnType retVal = E_OK;

#if (STD_ON == ETH_DEV_ERROR_DETECT)
    retVal = Eth_checkVirtualmacErrors(CtrlIdx, ETH_SID_DISPATCH_VIRTMAC_SUBSCRIBE_ALLTRAFFIC);
#endif

    if (E_OK == retVal)
    {
        retVal = Eth_RpcSendRegisterDefaultReq(CtrlIdx);
    }
    return retVal;
}
#endif /* ((STD_ON == ETH_VIRTUALMAC_SUPPORT) && (STD_ON == ETH_VIRTUALMAC_SUBSCRIBEALLTRAFFIC_API)) */

#if ((STD_ON == ETH_VIRTUALMAC_SUPPORT) && (STD_ON == ETH_VIRTUALMAC_UNSUBSCRIBEALLTRAFFIC_API))
/*
 * Design:
 * Requirements:
 */
FUNC(Std_ReturnType, ETH_CODE)
Eth_DispatchVirtmacUnsubscribeAllTraffic(uint8 CtrlIdx)
{
    Std_ReturnType retVal = E_OK;

#if (STD_ON == ETH_DEV_ERROR_DETECT)
    retVal = Eth_checkVirtualmacErrors(CtrlIdx, ETH_SID_DISPATCH_VIRTMAC_UNSUBSCRIBE_ALLTRAFFIC);
#endif

    if (E_OK == retVal)
    {
        retVal = Eth_RpcSendUnregisterDefaultReq(CtrlIdx);
    }
    return retVal;
}
#endif /* ((STD_ON == ETH_VIRTUALMAC_SUPPORT) && (STD_ON == ETH_VIRTUALMAC_SUBSCRIBEALLTRAFFIC_API)) */


#if ((STD_ON == ETH_VIRTUALMAC_SUPPORT) && (STD_ON == ETH_VIRTUALMAC_SUBSCRIBEDSTMAC_API))
/*
 * Design:
 * Requirements:
 */
FUNC(Std_ReturnType, ETH_CODE)
Eth_DispatchVirtmacSubscribeDstMac(uint8 CtrlIdx, uint8 *macAddress)
{
    Std_ReturnType retVal = E_OK;

#if (STD_ON == ETH_DEV_ERROR_DETECT)
    retVal = Eth_checkVirtualmacErrors(CtrlIdx, ETH_SID_DISPATCH_VIRTMAC_SUBSCRIBE_DSTMAC);
#endif

    if (E_OK == retVal)
    {
        retVal = Eth_RpcSendRegisterMacReq(CtrlIdx, macAddress);
    }
    return retVal;
}
#endif /* ((STD_ON == ETH_VIRTUALMAC_SUPPORT) && (STD_ON == ETH_VIRTUALMAC_SUBSCRIBEDSTMAC_API)) */

#if ((STD_ON == ETH_VIRTUALMAC_SUPPORT) && (STD_ON == ETH_VIRTUALMAC_UNSUBSCRIBEDSTMAC_API))
/*
 * Design:
 * Requirements:
 */
FUNC(Std_ReturnType, ETH_CODE)
Eth_DispatchVirtmacUnsubscribeDstMac(uint8 CtrlIdx, uint8 *macAddress)
{
    Std_ReturnType retVal = E_OK;

#if (STD_ON == ETH_DEV_ERROR_DETECT)
    retVal = Eth_checkVirtualmacErrors(CtrlIdx, ETH_SID_DISPATCH_VIRTMAC_UNSUBSCRIBE_DSTMAC);
#endif

    if (E_OK == retVal)
    {
        retVal = Eth_RpcSendUnregisterMacReq(CtrlIdx, macAddress);
    }
    return retVal;
}
#endif /* ((STD_ON == ETH_VIRTUALMAC_SUPPORT) && (STD_ON == ETH_VIRTUALMAC_UNSUBSCRIBEDSTMAC_API)) */


#if ((STD_ON == ETH_VIRTUALMAC_SUPPORT) && (STD_ON == ETH_VIRTUALMAC_ASSOCIATEIPV4MACADDR_API))
/*
 * Design:
 * Requirements:
 */
FUNC(Std_ReturnType, ETH_CODE)
Eth_DispatchVirtmacAssociateIPv4Macaddr(uint8 CtrlIdx, uint8 *ipv4Address, uint8 *macAddress)
{
    Std_ReturnType retVal = E_OK;

#if (STD_ON == ETH_DEV_ERROR_DETECT)
    retVal = Eth_checkVirtualmacErrors(CtrlIdx, ETH_SID_DISPATCH_VIRTMAC_IPV4_MACADDR_ASSOCIATE);
#endif

    if (E_OK == retVal)
    {
        retVal = Eth_RpcSendIPv4RegisterMacReq(CtrlIdx, ipv4Address, macAddress);
    }
    return retVal;
}
#endif /* ((STD_ON == ETH_VIRTUALMAC_SUPPORT) && (STD_ON == ETH_VIRTUALMAC_ASSOCIATEIPV4MACADDR_API)) */

#if ((STD_ON == ETH_VIRTUALMAC_SUPPORT) && (STD_ON == ETH_VIRTUALMAC_DISASSOCIATEIPV4MACADDR_API))
/*
 * Design:
 * Requirements:
 */
FUNC(Std_ReturnType, ETH_CODE)
Eth_DispatchVirtmacDisassociateIPv4Macaddr(uint8 CtrlIdx, uint8 *ipv4Address)
{
    Std_ReturnType retVal = E_OK;

#if (STD_ON == ETH_DEV_ERROR_DETECT)
    retVal = Eth_checkVirtualmacErrors(CtrlIdx, ETH_SID_DISPATCH_VIRTMAC_IPV4_MACADDR_ASSOCIATE);
#endif

    if (E_OK == retVal)
    {
        retVal = Eth_RpcSendIPv4UnregisterMacReq(CtrlIdx, ipv4Address);
    }
    return retVal;
}
#endif /* ((STD_ON == ETH_VIRTUALMAC_SUPPORT) && (STD_ON == ETH_VIRTUALMAC_DISASSOCIATEIPV4MACADDR_API)) */

#if ((STD_ON == ETH_VIRTUALMAC_SUPPORT) && (STD_ON == ETH_VIRTUALMAC_ADD_MCAST_MACADDR_API))
/*
 * Design:
 * Requirements:
 */
FUNC(Std_ReturnType, ETH_CODE)
Eth_DispatchVirtmacAddMcastAddr(uint8 CtrlIdx, uint8 *macAddress, uint32 vlanId)
{
    Std_ReturnType retVal = E_OK;

#if (STD_ON == ETH_DEV_ERROR_DETECT)
    retVal = Eth_checkVirtualmacErrors(CtrlIdx, ETH_SID_DISPATCH_VIRTMAC_ADD_MCAST_MACADDR);
#endif

    if (E_OK == retVal)
    {
        retVal = Eth_RpcSendAddMulticastEntry(CtrlIdx, macAddress, vlanId);
    }
    return retVal;
}
#endif /* ((STD_ON == ETH_VIRTUALMAC_SUPPORT) && (STD_ON == ETH_VIRTUALMAC_ADD_MCAST_MACADDR_API)) */

#if ((STD_ON == ETH_VIRTUALMAC_SUPPORT) && (STD_ON == ETH_VIRTUALMAC_DEL_MACADDR_API))
/*
 * Design:
 * Requirements:
 */
FUNC(Std_ReturnType, ETH_CODE)
Eth_DispatchVirtmacDelAddr(uint8 CtrlIdx, uint8 *macAddress, uint32 vlanId)
{
    Std_ReturnType retVal = E_OK;

#if (STD_ON == ETH_DEV_ERROR_DETECT)
    retVal = Eth_checkVirtualmacErrors(CtrlIdx, ETH_SID_DISPATCH_VIRTMAC_DEL_MACADDR);
#endif

    if (E_OK == retVal)
    {
        retVal = Eth_RpcSendDelMulticastEntry(CtrlIdx, macAddress, vlanId);
    }
    return retVal;
}
#endif /* ((STD_ON == ETH_VIRTUALMAC_SUPPORT) && (STD_ON == ETH_VIRTUALMAC_DISASSOCIATEIPV4MACADDR_API)) */

#if ((STD_ON == ETH_VIRTUALMAC_SUPPORT) && (STD_ON == ETH_VIRTUALMAC_ADD_VLAN_API))
/*
 * Design:
 * Requirements:
 */
FUNC(Std_ReturnType, ETH_CODE)
Eth_DispatchVirtmacAddVlan(uint8 CtrlIdx, uint32 vlanId)
{
    Std_ReturnType retVal = E_OK;

#if (STD_ON == ETH_DEV_ERROR_DETECT)
    retVal = Eth_checkVirtualmacErrors(CtrlIdx, ETH_SID_DISPATCH_VIRTMAC_ADD_VLAN);
#endif

    if (E_OK == retVal)
    {
        retVal = Eth_RpcSendAddVlan(CtrlIdx, vlanId);
    }
    return retVal;
}
#endif /* ((STD_ON == ETH_VIRTUALMAC_SUPPORT) && (STD_ON == ETH_VIRTUALMAC_ADD_VLAN_API)) */

#if ((STD_ON == ETH_VIRTUALMAC_SUPPORT) && (STD_ON == ETH_VIRTUALMAC_DEL_VLAN_API))
/*
 * Design:
 * Requirements:
 */
FUNC(Std_ReturnType, ETH_CODE)
Eth_DispatchVirtmacDelVlan(uint8 CtrlIdx, uint32 vlanId)
{
    Std_ReturnType retVal = E_OK;

#if (STD_ON == ETH_DEV_ERROR_DETECT)
    retVal = Eth_checkVirtualmacErrors(CtrlIdx, ETH_SID_DISPATCH_VIRTMAC_DEL_VLAN);
#endif

    if (E_OK == retVal)
    {
        retVal = Eth_RpcSendDelVlan(CtrlIdx, vlanId);
    }
    return retVal;
}
#endif /* ((STD_ON == ETH_VIRTUALMAC_SUPPORT) && (STD_ON == ETH_VIRTUALMAC_DEL_VLAN_API)) */

#if (STD_ON == ETH_VIRTUALMAC_SUPPORT)
FUNC(void, ETH_CODE)
Eth_VirtMac_MainFunction(uint8 ctrlIdx)
{
    Std_ReturnType retVal = E_OK;

    if (FALSE == Eth_VirtMacInfo[ctrlIdx].ethFwAttached)
    {
        Eth_VirtMacInfo[ctrlIdx].responseTimeoutCnt++;
        if (Eth_VirtMacInfo[ctrlIdx].responseTimeoutCnt >= ETH_VIRTUALMAC_FWINFO_TIMEOUT)
        {
            retVal = Eth_RpcAnnounce(ctrlIdx);

            if (E_OK == retVal)
            {
                Eth_VirtMacInfo[ctrlIdx].responseTimeoutCnt = 0;
            }
        }
    }

    if (TRUE == Eth_VirtMacGetEthPollRecvMsgInEthMain(ctrlIdx))
    {
        /* Process all queued receive msgs . If there are no
         * queued msgs,Eth_RpcRecvMsg will return E_NOT_OK.
         * This is not an error condition and just indicates
         * no recv msgs are queued.
         */
        do {
            retVal = Eth_RpcRecvMsg(ctrlIdx, ETH_SID_MAIN_FUNCTION);
        } while (retVal == E_OK);
    }

    if (TRUE == Eth_VirtMacInfo[ctrlIdx].ethFwAttached)
    {
        Eth_virtMacProcessState(ctrlIdx);
    }
}
#endif /* ((STD_ON == ETH_VIRTUALMAC_SUPPORT) */

/* ========================================================================== */
/*                  Internal Function Definitions                             */
/* ========================================================================== */

#if (STD_ON == ETH_VIRTUALMAC_SUPPORT)
static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkVirtualmacErrors(uint8 ctrlIdx, uint8 apiId)
{
    Std_ReturnType retVal = E_NOT_OK;

    if (ETH_STATE_INIT != Eth_DrvStatus)
    {
        Eth_reportDetError(apiId, ETH_E_UNINIT);
    }
    else if (ctrlIdx >= ETH_CTRL_ID_MAX)
    {
        Eth_reportDetError(apiId, ETH_E_INV_CTRL_IDX);
    }
    else
    {
        retVal = E_OK;
    }

    return retVal;
}
#endif /* (STD_ON == ETH_VIRTUALMAC_SUPPORT) */

#if (STD_ON == ETH_VIRTUALMAC_SUPPORT)
static FUNC(void, ETH_CODE) Eth_virtMacProcessState(uint8 ctrlIdx)
{
    Std_ReturnType retVal = E_OK;

    switch(Eth_VirtMacInfo[ctrlIdx].recoverState)
    {
        case ETH_RECOVERY_STATE_NONE:
        {
            if(ETHREMOTECFG_NOTIFY_HWERROR == Eth_VirtMacInfo[ctrlIdx].s2CNotifyId)
            {
                Eth_VirtMacInfo[ctrlIdx].recoverState = ETH_RECOVERY_STATE_ERROR_DETECTED;
                Eth_VirtMacInfo[ctrlIdx].s2CNotifyId = ETHREMOTECFG_NOTIFY_LAST;
            }
            break;
        }
        case ETH_RECOVERY_STATE_ERROR_DETECTED:
        {
            /* Save current controller mode */
            Eth_GetControllerMode(ctrlIdx, &Eth_VirtMacInfo[ctrlIdx].modeBeforeRecovery);

            /* Perform controller down to teardown process  */
            retVal |= Eth_SetControllerModeByVirtMac(ctrlIdx, ETH_MODE_DOWN);

            /* Unpair PSIL */
            if(NULL_PTR != Eth_VirtMacGetDmaTxChannelUnpairAll(ctrlIdx))
            {
                retVal |= Eth_VirtMacGetDmaTxChannelUnpairAll(ctrlIdx)(ctrlIdx);
            }
            /* Reset flow */
            if(NULL_PTR != Eth_VirtMacGetDmaFlowResetAll(ctrlIdx))
            {
                retVal |= Eth_VirtMacGetDmaFlowResetAll(ctrlIdx)(ctrlIdx);
            }

            if(E_OK == retVal)
            {
                Eth_VirtMacInfo[ctrlIdx].recoverState = ETH_RECOVERY_STATE_REQ_SERVER_TEARDONW_COMPLETE;
            }
            break;
        }
        case ETH_RECOVERY_STATE_REQ_SERVER_TEARDONW_COMPLETE:
        {
            Eth_VirtMacInfo[ctrlIdx].teardownResponsePending = TRUE;

            retVal = Eth_RpcSendResetComplete(ctrlIdx);
            if(E_OK == retVal)
            {
                Eth_VirtMacInfo[ctrlIdx].recoverState = ETH_RECOVERY_STATE_WAIT_SERVER_ACK_TEARDONW_COMPLETE;
            }
            else
            {
                Eth_VirtMacInfo[ctrlIdx].teardownResponsePending = FALSE;
            }
            break;
        }
        case ETH_RECOVERY_STATE_WAIT_SERVER_ACK_TEARDONW_COMPLETE:
        {
            if(Eth_VirtMacInfo[ctrlIdx].teardownResponsePending == FALSE)
            {
                Eth_VirtMacInfo[ctrlIdx].recoverState = ETH_RECOVERY_STATE_WAIT_SERVER_RECOVERY_COMPLETE;
            }
            break;
        }        
        case ETH_RECOVERY_STATE_WAIT_SERVER_RECOVERY_COMPLETE:
        {
            if(ETHREMOTECFG_NOTIFY_HWRECOVERY_COMPLETE == Eth_VirtMacInfo[ctrlIdx].s2CNotifyId)
            {
                Eth_VirtMacInfo[ctrlIdx].recoverState = ETH_RECOVERY_STATE_RESTORING;
                Eth_VirtMacInfo[ctrlIdx].s2CNotifyId = ETHREMOTECFG_NOTIFY_LAST;
            }
            break;
        }
        case ETH_RECOVERY_STATE_RESTORING:
        {
            /* Pair PSIL */
            if(NULL_PTR != Eth_VirtMacGetDmaTxChannelPairAll(ctrlIdx))
            {
                retVal |= Eth_VirtMacGetDmaTxChannelPairAll(ctrlIdx)(ctrlIdx);
            }
            /* Reconfig flow */
            if(NULL_PTR != Eth_VirtMacGetDmaFlowCfgAll(ctrlIdx))
            {
                retVal |= Eth_VirtMacGetDmaFlowCfgAll(ctrlIdx)(ctrlIdx);
            }

            /* Restore controller mode */
            retVal |= Eth_SetControllerModeByVirtMac(ctrlIdx, Eth_VirtMacInfo[ctrlIdx].modeBeforeRecovery);

            if(E_OK == retVal)
            {
                Eth_VirtMacInfo[ctrlIdx].recoverState = ETH_RECOVERY_STATE_NONE;
            }
            break;
        }
        default:
            break;
    }
}
#endif /* (STD_ON == ETH_VIRTUALMAC_SUPPORT) */
