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
 *  \file     EthUtils_VirtMac.c
 *
 *  \brief    This file contains the eth app virtmac specific utilities implementation
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <string.h>

#include "Eth.h"
#include "EthUtils.h"
#include "Eth_VirtMac.h"
#include "Eth_Rpc.h"
#include "Cdd_Ipc.h"
#include "Cdd_IpcIrq.h"
#if defined (SOC_J721E)
#include "IntRtr_Cfg.h"
#endif
#include "EthUtils_VirtMac.h"

#include <CddIpc/ipc_baremetal_hw/ipc.h>
#include <CddIpc/ipc_baremetal_hw/soc/ipc_soc.h>
#include <hw_include/lldr.h>
#include <hw_include/lld_mailbox.h>

#include "board_utils.h"

#if (STD_ON == ETH_VIRTUALMAC_SUPPORT)
/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

#define ETHVIRTMACAPP_REMOTEETHDEVICE_COMCHID  (CddIpcConf_IpcComChanId_Cdd_IpcMcu20_EthDevice)

#define VQ_BUF_SIZE             (2048)
#define IPC_RPMESSAGE_OBJ_SIZE  (256)
#define RPMSG_DATA_SIZE         (256 * 512 + IPC_RPMESSAGE_OBJ_SIZE)
#if defined (SOC_J7200)
#define IPC_VRING_MEM_SIZE      (0x00800000u)
#elif defined (SOC_J784S4)
#define IPC_VRING_MEM_SIZE      (0x03000000u)
#else
#define IPC_VRING_MEM_SIZE      (0x02000000u)
#endif

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

static uint8 g_vringMemBuf[IPC_VRING_MEM_SIZE] __attribute__ ((section(".bss:ipc_vring_mem"), aligned(8192)));

#if (defined (BUILD_MCU1_0))
static uint32 selfProcId = IPC_MCU1_0;
#else
static uint32 selfProcId = IPC_MCU2_1;
#endif
static uint32 gRemoteProc[] =
{
    IPC_MPU1_0,
    IPC_MCU1_0,
    IPC_MCU1_1,
    IPC_MCU2_0,
#if defined (SOC_J721E)
    IPC_MCU3_0,
    IPC_MCU3_1,
    IPC_C66X_1,
    IPC_C66X_2,
    IPC_C7X_1,
#elif defined (SOC_J784S4)
    IPC_MCU3_0,
    IPC_MCU3_1,
    IPC_MCU4_0,
    IPC_MCU4_1,
    IPC_C7X_1,
    IPC_C7X_2,
    IPC_C7X_3,
    IPC_C7X_4,
#endif
};
static uint32 gNumRemoteProc = sizeof(gRemoteProc) / sizeof(uint32);

/* Indexed list of src ids */
static const uint16 ipc_map_src_id[] =
{
    TISCI_DEV_NAVSS0_MAILBOX_0,
    TISCI_DEV_NAVSS0_MAILBOX_1,
    TISCI_DEV_NAVSS0_MAILBOX_2,
    TISCI_DEV_NAVSS0_MAILBOX_3,
    TISCI_DEV_NAVSS0_MAILBOX_4,
    TISCI_DEV_NAVSS0_MAILBOX_5,
    TISCI_DEV_NAVSS0_MAILBOX_6,
    TISCI_DEV_NAVSS0_MAILBOX_7,
    TISCI_DEV_NAVSS0_MAILBOX_8,
    TISCI_DEV_NAVSS0_MAILBOX_9,
    TISCI_DEV_NAVSS0_MAILBOX_10,
    TISCI_DEV_NAVSS0_MAILBOX_11,
#if defined(SOC_J784S4)
    TISCI_DEV_NAVSS0_MAILBOX1_0,
    TISCI_DEV_NAVSS0_MAILBOX1_1,
    TISCI_DEV_NAVSS0_MAILBOX1_2,
    TISCI_DEV_NAVSS0_MAILBOX1_3,
    TISCI_DEV_NAVSS0_MAILBOX1_4,
    TISCI_DEV_NAVSS0_MAILBOX1_5,
#endif
};

/* Indexed list of host ids */
static const uint16 ipc_map_host_id[] =
{
    TISCI_HOST_ID_A72_0,
    TISCI_HOST_ID_MCU_0_R5_0,
    TISCI_HOST_ID_MCU_0_R5_2,
    TISCI_HOST_ID_MAIN_0_R5_0,
    TISCI_HOST_ID_MAIN_0_R5_2,
#if defined (SOC_J721E)
    TISCI_HOST_ID_MAIN_1_R5_0,
    TISCI_HOST_ID_MAIN_1_R5_2,
    TISCI_HOST_ID_C6X_0_1,
    TISCI_HOST_ID_C6X_1_1,
    TISCI_HOST_ID_C7X_1,
#elif defined (SOC_J784S4)
    TISCI_HOST_ID_MAIN_1_R5_0,
    TISCI_HOST_ID_MAIN_1_R5_2,
    TISCI_HOST_ID_MAIN_2_R5_0,
    TISCI_HOST_ID_MAIN_2_R5_2,
    TISCI_HOST_ID_C7X_0_0,
    TISCI_HOST_ID_C7X_1_0,
    TISCI_HOST_ID_C7X_2_0,
    TISCI_HOST_ID_C7X_3_0,
#endif
};

/* Indexed list of dst ids */
static const uint16 ipc_map_dst_id[] =
{
    TISCI_DEV_COMPUTE_CLUSTER0_GIC500SS,
    TISCI_DEV_MCU_R5FSS0_CORE0,
    TISCI_DEV_MCU_R5FSS0_CORE1,
    TISCI_DEV_R5FSS0_CORE0,
    TISCI_DEV_R5FSS0_CORE1,
#if defined (SOC_J721E)
    TISCI_DEV_R5FSS1_CORE0,
    TISCI_DEV_R5FSS1_CORE1,
    TISCI_DEV_C66SS0_CORE0,
    TISCI_DEV_C66SS1_CORE0,
    TISCI_DEV_COMPUTE_CLUSTER0_CLEC,
#elif defined (SOC_J784S4)
    TISCI_DEV_R5FSS1_CORE0,
    TISCI_DEV_R5FSS1_CORE1,
    TISCI_DEV_R5FSS2_CORE0,
    TISCI_DEV_R5FSS2_CORE1,
#endif
};

volatile static uint8  expectedSid = 0xFFU;
volatile static uint32 notifyBusyWait = FALSE;

#define ETHAPP_BUSY_WAIT() do {           \
    notifyBusyWait = FALSE;               \
    while(notifyBusyWait == FALSE){       \
        asm ("NOP");                      \
    }                                     \
} while(0)

#define ETHAPP_RELEASE_BUSY_WAIT() do {   \
  notifyBusyWait = TRUE;                  \
} while(0)

/* ========================================================================== */
/*                 Global Function Declarations                             */
/* ========================================================================== */

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */

static void EthUtils_VirtMacDispatchInit(uint8 ctrlIdx);
static void EthUtils_VirtMacrecvMsgNotify(uint8 ctrlIdx);
static uint32 EthUtils_VirtMacipcMailboxClear(uint32 baseAddr, uint32 queueId);
static sint32 EthUtils_VirtMacsciclientIrqRelease(uint16 coreId, uint32 clusterId,
        uint32 userId, uint32 intNumber);
static sint32 EthUtils_VirtMacsciclientIrqSet(uint16 coreId, uint32 clusterId,
        uint32 userId, uint32 intNumber);

static void EthUtils_VirtMacipcMsgFromMcu20Isr(void* notUsed);
static uint32 EthUtils_VirtMacgetCoreId(void);
static void EthUtils_VirtMacipcMbIntRegForMcu20(void);
static void EthUtils_VirtMacipcPrintVersion(void);

static void EthUtils_VirtMacipcInterruptConfig(void);
static void EthUtils_VirtMacipcStartup(void);
static void EthUtils_VirtMacinitIpcCddCfg(struct Cdd_IpcConfigType_s *ipcCddCfg);
static Std_ReturnType EthUtils_VirtMacipcInit(void);

/* ========================================================================== */
/*                 Global Function Definition                                 */
/* ========================================================================== */

Std_ReturnType EthUtils_VirtMacinit(uint8 ctrlIdx)
{
    Std_ReturnType retVal;
    const Eth_ConfigType *ethCfg = (const Eth_ConfigType *)NULL_PTR;

#if (STD_OFF == ETH_PRE_COMPILE_VARIANT)
    ethCfg = (const Eth_ConfigType *)&EthConfigSet_0;
#endif

    EthUtils_VirtMacipcStartup();

    EthUtils_VirtMacipcInit();

    retVal = Eth_VirtMacRpcInit(ethCfg);
    if (E_OK == retVal && TRUE == Eth_IsVirtualMacModeEnable(ctrlIdx))
    {
        /* Wait for ethFw registration */
        ETHAPP_BUSY_WAIT();

        EthUtils_VirtMacDispatchInit(ctrlIdx);
    }

    return retVal;
}

void EthUtils_VirtMacRegisterMacAddr(uint8 ctrlIdx, uint8 *macAddr)
{
    Std_ReturnType status;

    expectedSid = ETH_SID_DISPATCH_VIRTMAC_SUBSCRIBE_DSTMAC;
    status = Eth_DispatchVirtmacSubscribeDstMac(ctrlIdx, macAddr);
    (void)status;
    ETHAPP_BUSY_WAIT();
}

void EthUtils_VirtMacUnregisterMacAddr(uint8 ctrlIdx, uint8 *macAddr)
{
    Std_ReturnType status;

    expectedSid = ETH_SID_DISPATCH_VIRTMAC_UNSUBSCRIBE_DSTMAC;
    status = Eth_DispatchVirtmacUnsubscribeDstMac(ctrlIdx, macAddr);
    (void)status;
    ETHAPP_BUSY_WAIT();
}

void EthUtils_VirtMacSubMcastAddr(uint8 ctrlIdx, uint8 *macAddr)
{
    Std_ReturnType status;

    expectedSid = ETH_SID_DISPATCH_VIRTMAC_ADD_MCAST_MACADDR;
    status = Eth_DispatchVirtmacAddMcastAddr(ctrlIdx, macAddr, 0);
    (void)status;
    ETHAPP_BUSY_WAIT();
    (void)ctrlIdx;
}

void EthUtils_VirtMacUnsubMcastAddr(uint8 ctrlIdx, uint8 *macAddr)
{
    Std_ReturnType status;

    expectedSid = ETH_SID_DISPATCH_VIRTMAC_DEL_MACADDR;
    status = Eth_DispatchVirtmacDelAddr(ctrlIdx, macAddr, 0);
    (void)status;
    ETHAPP_BUSY_WAIT();
}

void EthUtils_VirtMacJoinVlan(uint8 ctrlIdx, uint32 vlanId)
{
    Std_ReturnType status;

    expectedSid = ETH_SID_DISPATCH_VIRTMAC_ADD_VLAN;
    status = Eth_DispatchVirtmacAddVlan(ctrlIdx, vlanId);
    (void)status;
    ETHAPP_BUSY_WAIT();
}

void EthUtils_VirtMacLeaveVlan(uint8 ctrlIdx, uint32 vlanId)
{
    Std_ReturnType status;

    expectedSid = ETH_SID_DISPATCH_VIRTMAC_DEL_VLAN;
    status = Eth_DispatchVirtmacDelVlan(ctrlIdx, vlanId);
    (void)status;
    ETHAPP_BUSY_WAIT();
}

void EthVirtmacApp_rpcCmdComplete(uint8 ctrlIdx, uint8 sid, sint32 status)
{
    if ((status == ETHREMOTECFG_CMDSTATUS_OK) && (expectedSid == sid))
    {
        ETHAPP_RELEASE_BUSY_WAIT();
    }
}

void EthVirtmacApp_rpcFwRegistered(uint8 ctrlIdx)
{
    ETHAPP_RELEASE_BUSY_WAIT();
}

void Cdd_IpcNewMessageNotify(void)
{
    EthUtils_VirtMacrecvMsgNotify(0u);
}

void Cdd_IpcNewCtrlMessageNotify(uint32 remoteProcId)
{
    /* None */
}

/* ========================================================================== */
/*                 Internal Function Definition                             */
/* ========================================================================== */

static void EthUtils_VirtMacDispatchInit(uint8 ctrlIdx)
{
    Std_ReturnType status;

    expectedSid = ETH_SID_DISPATCH_VIRTMAC_INIT;
    status = Eth_DispatchVirtmacInit(ctrlIdx);
    (void)status;
    ETHAPP_BUSY_WAIT();
}

static void EthUtils_VirtMacrecvMsgNotify(uint8 ctrlIdx)
{
    Eth_NotifyVirtmacMsgReceived(ctrlIdx);
}

static uint32 EthUtils_VirtMacipcMailboxClear(uint32 baseAddr, uint32 queueId)
{
    uint32 retVal = 0;
    uint32 msg[4];

    retVal = MailboxGetMessage(baseAddr, queueId, msg);

    return retVal;
}

static sint32 EthUtils_VirtMacsciclientIrqRelease(uint16 coreId, uint32 clusterId,
        uint32 userId, uint32 intNumber)
{
    sint32                               retVal = IPC_SOK;
    struct tisci_msg_rm_irq_release_req   rmIrqRel;

    rmIrqRel.ia_id                  = 0U;
    rmIrqRel.vint                   = 0U;
    rmIrqRel.global_event           = 0U;
    rmIrqRel.vint_status_bit_index  = 0U;

    rmIrqRel.valid_params   = TISCI_MSG_VALUE_RM_DST_ID_VALID |
                              TISCI_MSG_VALUE_RM_DST_HOST_IRQ_VALID |
                              TISCI_MSG_VALUE_RM_SECONDARY_HOST_VALID;
    rmIrqRel.src_id         = ipc_map_src_id[clusterId];
    rmIrqRel.src_index      = (uint16)userId;
    rmIrqRel.dst_id         = (uint16)ipc_map_dst_id[coreId];
#if defined(BUILD_C7X_1)
    rmIrqRel.dst_host_irq   = (uint16)(intNumber +  IPC_C7X_COMPUTE_CLUSTER_OFFSET);
#else
    rmIrqRel.dst_host_irq   = (uint16)intNumber;
#endif
    rmIrqRel.secondary_host = (uint8)ipc_map_host_id[coreId];

    retVal = Sciclient_rmIrqRelease(&rmIrqRel, IPC_SCICLIENT_TIMEOUT);

    return retVal;
}

static sint32 EthUtils_VirtMacsciclientIrqSet(uint16 coreId, uint32 clusterId,
        uint32 userId, uint32 intNumber)
{
    sint32                           retVal = IPC_SOK;
    struct tisci_msg_rm_irq_set_req   rmIrqReq;
    struct tisci_msg_rm_irq_set_resp  rmIrqResp;

    rmIrqReq.ia_id                  = 0U;
    rmIrqReq.vint                   = 0U;
    rmIrqReq.global_event           = 0U;
    rmIrqReq.vint_status_bit_index  = 0U;

    rmIrqReq.valid_params   = TISCI_MSG_VALUE_RM_DST_ID_VALID |
                              TISCI_MSG_VALUE_RM_DST_HOST_IRQ_VALID |
                              TISCI_MSG_VALUE_RM_SECONDARY_HOST_VALID;
    rmIrqReq.src_id         = ipc_map_src_id[clusterId];
    rmIrqReq.src_index      = (uint16)userId;
    rmIrqReq.dst_id         = (uint16)ipc_map_dst_id[coreId];
#if defined(BUILD_C7X_1)
    rmIrqReq.dst_host_irq   = (uint16)(intNumber +  IPC_C7X_COMPUTE_CLUSTER_OFFSET);
#else
    rmIrqReq.dst_host_irq   = (uint16)intNumber;
#endif
    rmIrqReq.secondary_host = (uint8)ipc_map_host_id[coreId];

    /* Config event */
    retVal = Sciclient_rmIrqSet(&rmIrqReq, &rmIrqResp, IPC_SCICLIENT_TIMEOUT);

    return retVal;
}

static void EthUtils_VirtMacipcMsgFromMcu20Isr(void* notUsed)
{
    /* Invoke MCU 20 Isr handler */
    Cdd_IpcIrqMbxFromMcu_20();
    (void)notUsed;
}

static uint32 EthUtils_VirtMacgetCoreId(void)
{
    uint32 selfId =  IPC_INVALID_PROCID;

#if defined(BUILD_MPU1_0)
    selfId = IPC_MPU1_0;
#elif defined(BUILD_MCU1_0)
    selfId = IPC_MCU1_0;
#elif defined(BUILD_MCU1_1)
    selfId = IPC_MCU1_1;
#elif defined(BUILD_MCU2_0)
    selfId = IPC_MCU2_0;
#elif defined(BUILD_MCU2_1)
    selfId = IPC_MCU2_1;
#elif defined(BUILD_MCU3_0)
    selfId = IPC_MCU3_0;
#elif defined(BUILD_MCU3_1)
    selfId = IPC_MCU3_1;
#elif defined(BUILD_MCU4_0)
    selfId = IPC_MCU4_0;
#elif defined(BUILD_MCU4_1)
    selfId = IPC_MCU4_1;
#elif defined(BUILD_C66X_1)
    selfId = IPC_C66X_1;
#elif defined(BUILD_C66X_2)
    selfId = IPC_C66X_2;
#elif defined(BUILD_C7X_1)
    selfId = IPC_C7X_1;
#elif defined(BUILD_C7X_2)
    selfId = IPC_C7X_2;
#elif defined(BUILD_C7X_3)
    selfId = IPC_C7X_3;
#elif defined(BUILD_C7X_4)
    selfId = IPC_C7X_4;
#else
#error "Unsupported core Id"
#endif

    return (selfId);
}

static void EthUtils_VirtMacipcMbIntRegForMcu20(void)
{
    sint32 retVal;
    uint32 selfId, remoteProcId;
    uint32 clusterId;
    uint32 userId;
    uint32 queueId;
    Ipc_MbConfig cfg;
    uint32 baseAddr;

    selfId = EthUtils_VirtMacgetCoreId();
    remoteProcId = IPC_MCU2_0;
    Ipc_getMailboxInfoRx(selfId, remoteProcId, &clusterId, &userId, &queueId);
    baseAddr = Ipc_getMailboxBaseAddr(clusterId);

    /* Clear Mailbox cluster queue */
    EthUtils_VirtMacipcMailboxClear(baseAddr, queueId);
    MailboxClrNewMsgStatus(baseAddr, userId, queueId);

    /* Get the Interrupt Configuration */
    Ipc_getMailboxIntrRouterCfg(selfId, clusterId, userId, &cfg, 0);

    /* Release the resource first */
    retVal = EthUtils_VirtMacsciclientIrqRelease(selfId, clusterId, userId, cfg.eventId);

    uint32 timeout_cnt = 10;
    do
    {
        retVal = EthUtils_VirtMacsciclientIrqSet(selfId, clusterId, userId, cfg.eventId);
        if(retVal != 0)
        {
            EthUtils_printf("Failed to register irq through sciclient...%x\n", retVal);
        }
        timeout_cnt--;
    }while((retVal != 0) && (timeout_cnt > 0));

    if(timeout_cnt == 0)
    {
        retVal = IPC_EFAIL;
    }


    retVal = BoardUtils_registerInterrupt(
                            cfg.eventId,
                            &EthUtils_VirtMacipcMsgFromMcu20Isr);
    if (E_OK != retVal)
    {
        EthUtils_printf("%s: failed to register Cdd Ipc interrupt\n", __func__);
    }

    return;
}

/** \brief prints of the version of this implementation */
static void EthUtils_VirtMacipcPrintVersion(void)
{
    Std_VersionInfoType versioninfo;

    Cdd_IpcGetVersionInfo(&versioninfo);
    EthUtils_printf( " \n");
    EthUtils_printf(" CDD IPC MCAL Version Info\n");
    EthUtils_printf("---------------------\n");
    EthUtils_printf(" Vendor ID           : %d\n",
                                                versioninfo.vendorID);
    EthUtils_printf(" Module ID           : %d\n",
                                                versioninfo.moduleID);
    EthUtils_printf(" SW Major Version    : %d\n",
                                                versioninfo.sw_major_version);
    EthUtils_printf(" SW Minor Version    : %d\n",
                                                versioninfo.sw_minor_version);
    EthUtils_printf(" SW Patch Version    : %d\n",
                                                versioninfo.sw_patch_version);
    EthUtils_printf( " \n");

}

/** \brief Interrupt from mailbox for all cores registrations */
static void EthUtils_VirtMacipcInterruptConfig(void)
{
    EthUtils_VirtMacipcMbIntRegForMcu20 ();

    return;
}

/** \brief Start up sequence : Program the interrupt muxes / priorities */
static void EthUtils_VirtMacipcStartup(void)
{

    EthUtils_VirtMacipcPrintVersion();

    EthUtils_VirtMacipcInterruptConfig();

    /* Initialize memory sections  */
    AppUtils_CddIpcSectionInit();

}

static void EthUtils_VirtMacinitIpcCddCfg(struct Cdd_IpcConfigType_s *ipcCddCfg)
{
    uint32 numProc = gNumRemoteProc;
    uint32 i;

    ipcCddCfg->coreIds.numProcs = gNumRemoteProc;
    ipcCddCfg->coreIds.ownProcID = selfProcId;
    for (i = 0; i < gNumRemoteProc; i++)
    {
        ipcCddCfg->coreIds.remoteProcID[i] = gRemoteProc[i];
    }
    ipcCddCfg->vertIoCfg.vertIoRingAddr = (void *)g_vringMemBuf;
    ipcCddCfg->vertIoCfg.vertIoRingSize = sizeof(g_vringMemBuf);
    ipcCddCfg->vertIoCfg.vertIoObjSize = numProc * Ipc_getVqObjMemoryRequiredPerCore();
}

static Std_ReturnType EthUtils_VirtMacipcInit(void)
{
    struct Cdd_IpcConfigType_s *ipcCddCfg = (struct Cdd_IpcConfigType_s *)&CddIpcConfiguraions_PC;

    EthUtils_VirtMacinitIpcCddCfg(ipcCddCfg);

    Cdd_IpcInit();

    return E_OK;
}
#endif
