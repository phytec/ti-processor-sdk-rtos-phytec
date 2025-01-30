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
 *  \file     Eth.h
 *
 *  \brief    This file contains the interface of the Ethernet driver.
 */

/**
 *  \defgroup MCAL_ETH_API Ethernet Driver API
 *
 *  The Ethernet Driver provides low-level access to the two-port Gigabit
 *  Ethernet Switch (MCU_CPSW0) hardware in the Jacinto 7 family.  The
 *  first port (Port 0) is called host port and has a CPPI interface
 *  that interconnects the CPSW peripheral with the rest of the SoC.  The
 *  second port (Port 1) is a MAC port which can be configured in RGMII
 *  or RMII mode.
 *
 *  The Ethernet Driver implements the standardized interface described in
 *  the AUTOSAR Release 4.2.1 Specification of the Ethernet Driver
 *  (AUTOSAR_SWS_EthDriver,  Document ID 430).
 *
 *  The Ethernet Driver is part of the Communication Stack in the AUTOSAR
 *  Basic Software (BSW).
 *
 *  \sa MCAL_ETH_CFG
 *  \sa MCAL_ETH_IRQ
 *  @{
 */

/*
 * Below are the generic requirements met by this Eth driver which can't be
 * mapped to a specific piece of code
 */
/*
 * Design:       ETH_DesignId_001
 * Requirements: MCAL-1519, MCAL-1520, MCAL-1522, MCAL-1523,
 *               MCAL-1525, MCAL-1526, MCAL-1531, MCAL-1614,
 *               MCAL-1635, MCAL-1636, MCAL-1703, MCAL-981
 */

#ifndef ETH_H_
#define ETH_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

/*
 * Design:       ETH_DesignId_001
 * Requirements: MCAL-1635
 *
 * Note: As per "Header file structure" in Ethernet Driver spec:
 * Eth.h --> Eth_Types.h --> Eth_GeneralTypes.h --> ComStack_Types.h
 */
#include "Eth_Types.h"
#include "Eth_Cfg.h"
#include "Eth_Packet.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 *  \name Eth Driver Module SW Version Info
 *
 *  Definitions for the Eth Driver version used for compatibility checks
 *  @{
 */
/*
 * Design:       ETH_DesignId_001
 * Requirements: MCAL-1522
 */
/** \brief Driver Implementation Major Version */
#define ETH_SW_MAJOR_VERSION            (9U)
/** \brief Driver Implementation Minor Version */
#define ETH_SW_MINOR_VERSION            (2U)
/** \brief Driver Implementation Patch Version */
#define ETH_SW_PATCH_VERSION            (0U)
/*! @} */

/**
 *  \name Eth Driver Module AUTOSAR Version Info
 *
 *  Definitions for the Etj Driver AUTOSAR version used for compatibility checks
 *  @{
 */
/** \brief AUTOSAR Major version specification implemented by Eth Driver */
#define ETH_AR_RELEASE_MAJOR_VERSION    (4U)
/** \brief AUTOSAR Minor  version specification implemented by Eth Driver */
#define ETH_AR_RELEASE_MINOR_VERSION    (3U)
/** \brief AUTOSAR Patch version specification implemented by Eth Driver */
#define ETH_AR_RELEASE_REVISION_VERSION (1U)
/*! @} */

/**
 *  \name Eth Driver ID Info
 *  @{
 */
/** \brief Texas Instruments Vendor ID */
#define ETH_VENDOR_ID                   ((uint16) 44U)
/** \brief Eth Driver Module ID */
#define ETH_MODULE_ID                   ((uint16) 88U)
/** \brief Eth Driver Instance ID */
#define ETH_INSTANCE_ID                 ((uint8) 0U)
/*! @} */

/**
 *  \anchor Eth_ErrorCodes
 *  \name Eth Error Codes
 *
 *  Error codes returned by Eth functions
 *  @{
 */
/*
 * Design:       ETH_DesignId_003
 * Requirements: MCAL-1530
 */
#ifndef ETH_E_INV_CTRL_IDX
/** \brief Invalid controller index */
#define ETH_E_INV_CTRL_IDX              ((uint8) 0x01U)
#endif
#ifndef ETH_E_UNINIT
/** \brief Eth module was not initialized */
#define ETH_E_UNINIT                    ((uint8) 0x02U)
#endif
#ifndef ETH_E_PARAM_POINTER
/** \brief Invalid pointer in parameter list */
#define ETH_E_PARAM_POINTER             ((uint8) 0x03U)
#endif
#ifndef ETH_E_INV_PARAM
/** \brief Invalid parameter */
#define ETH_E_INV_PARAM                 ((uint8) 0x04U)
#endif
#ifndef ETH_E_INV_MODE
/** \brief Invalid mode */
#define ETH_E_INV_MODE                  ((uint8) 0x05U)
#endif
#ifndef ETH_E_VIRTMAC_APIMISMATCH
/** \brief Mismatch in API version between Eth Driver and ethernet firmware  */
#define ETH_E_VIRTMAC_APIMISMATCH       ((uint8) 0x07U)
#endif
#ifndef ETH_E_VIRTMAC_RPCCMDFAILED
/** @private \brief Virtual MAC RPC command failed */
#define ETH_E_VIRTMAC_RPCCMDFAILED      ((uint8) 0x08U)
#endif
#ifndef ETH_E_VIRTMAC_UNSUPPORTECLIENTNOTIFY
/** @private \brief Ethernet MCAL RPC client received unhandled notify from ethernet firmware server */
#define ETH_E_VIRTMAC_UNSUPPORTECLIENTNOTIFY ((uint8) 0x09U)
#endif
#ifndef ETH_E_VIRTMAC_UNSUPPORTEDSRVCMD
/** @private \brief Rpc Command sent by client not supported by server */
#define ETH_E_VIRTMAC_UNSUPPORTEDSRVCMD       ((uint8) 0x0AU)
#endif
#ifndef ETH_E_BUSY
/** \brief Device or resource is busy */
#define ETH_E_BUSY                      ((uint8) 0x0BU)
#endif

/*! @} */

/**
 *  \anchor Eth_ServiceIds
 *  \name Eth Service Ids
 *
 *  The Service Id is used to identify the source of an error when
 *  reported through the Det_ReportError() function.
 *  @{
 */
/** \brief Eth_Init() API Service ID */
#define ETH_SID_INIT                    ((uint8) 0x01U)

/** \brief Eth_SetControllerMode() API Service ID */
#define ETH_SID_SET_CONTROLLER_MODE     ((uint8) 0x03U)

/** \brief Eth_GetControllerMode() API Service ID */
#define ETH_SID_GET_CONTROLLER_MODE     ((uint8) 0x04U)

/** \brief Eth_WriteMii() API Service ID */
#define ETH_SID_WRITE_MII               ((uint8) 0x05U)

/** \brief Eth_ReadMii() API Service ID */
#define ETH_SID_READ_MII                ((uint8) 0x06U)

/** \brief Eth_GetCounterState() API Service ID */
#define ETH_SID_GET_COUNTER_STATE       ((uint8) 0x07U)

/** \brief Eth_GetPhysAddr() API Service ID */
#define ETH_SID_GET_PHYS_ADDR           ((uint8) 0x08U)

/** \brief Eth_ProvideTxBuffer() API Service ID */
#define ETH_SID_PROVIDE_TX_BUFFER       ((uint8) 0x09U)

/** \brief Eth_MainFunction() API Service ID */
/* REVISIT: Same as below? */
#define ETH_SID_MAIN_FUNCTION           ((uint8) 0x20U)

/** \brief Eth_Transmit() API Service ID */
#define ETH_SID_TRANSMIT                ((uint8) 0x0AU)

/** \brief Eth_Receive() API Service ID */
#define ETH_SID_RECEIVE                 ((uint8) 0x0BU)

/** \brief Eth_TxConfirmation() API Service ID */
#define ETH_SID_TX_CONFIRMATION         ((uint8) 0x0CU)

/** \brief Eth_GetVersionInfo() API Service ID */
#define ETH_SID_GET_VERSION_INFO        ((uint8) 0x0DU)

/** \brief Eth_RxIrqHdlr_<CtrlIdx>() API Service ID */
#define ETH_SID_RX_IRQ_HDLR             ((uint8) 0x10U)

/** \brief Eth_TxIrqHdlr_<CtrlIdx>() API Service ID */
#define ETH_SID_TX_IRQ_HDLR             ((uint8) 0x11U)

/** \brief Eth_UpdatePhysAddrFilter() API Service ID */
#define ETH_SID_UPDATE_PHYS_ADDR_FILTER ((uint8) 0x12U)

/** \brief Eth_SetPhysAddr() API Service ID */
#define ETH_SID_SET_PHYS_ADDR           ((uint8) 0x13U)

/** \brief Eth_GetCounterValues() API Service ID */
#define ETH_SID_GET_COUNTER_VALUES      ((uint8) 0x14U)

/** \brief Eth_GetRxStats() API Service ID */
#define ETH_SID_GET_RX_STATS            ((uint8) 0x15U)

/** \brief Eth_GetTxStats() API Service ID */
#define ETH_SID_GET_TX_STATS            ((uint8) 0x1CU)

/** \brief Eth_GetTxErrorCounterValues() API Service ID */
#define ETH_SID_GET_TXERROR_COUNTERVALUES   ((uint8) 0x1DU)

/** \brief Eth_GetCurrentTime() API Service ID */
#define ETH_SID_GET_CURRENT_TIME        ((uint8) 0x16U)

/** \brief Eth_EnableEgressTimeStamp() API Service ID */
#define ETH_SID_ENABLE_EGRESS_TIMESTAMP ((uint8) 0x17U)

/** \brief Eth_GetEgressTimeStamp() API Service ID */
#define ETH_SID_GET_EGRESS_TIMESTAMP    ((uint8) 0x18U)

/** \brief Eth_GetIngressTimeStamp() API Service ID */
#define ETH_SID_GET_INGRESS_TIMESTAMP   ((uint8) 0x19U)

/** @private \brief Eth_DispatchVirtmacInit() API Service ID */
#define ETH_SID_DISPATCH_VIRTMAC_INIT  ((uint8) 0x30U)

/** @private \brief Eth_DispatchVirtmacDeinit() API Service ID */
#define ETH_SID_DISPATCH_VIRTMAC_DEINIT  ((uint8) 0x31U)

/** @private \brief Eth_NotifyVirtmacMsgRecv() API Service ID */
#define ETH_SID_NOTIFY_VIRTMAC_MSGRECV ((uint8) 0x32U)

/** @private \brief Eth_DispatchVirtmacSubscribeAllTraffic() API Service ID */
#define ETH_SID_DISPATCH_VIRTMAC_SUBSCRIBE_ALLTRAFFIC   ((uint8) 0x33U)

/** @private \brief Eth_DispatchVirtmacUnsubscribeAllTraffic() API Service ID */
#define ETH_SID_DISPATCH_VIRTMAC_UNSUBSCRIBE_ALLTRAFFIC ((uint8) 0x34U)

/** @private \brief Eth_DispatchVirtmacSubscribeDstMac() API Service ID */
#define ETH_SID_DISPATCH_VIRTMAC_SUBSCRIBE_DSTMAC       ((uint8) 0x35U)

/** @private \brief Eth_DispatchVirtmacUnsubscribeDstMac() API Service ID */
#define ETH_SID_DISPATCH_VIRTMAC_UNSUBSCRIBE_DSTMAC     ((uint8) 0x36U)

/** @private \brief Eth_DispatchVirtmacIPv4MacAddrAssociate() API Service ID */
#define ETH_SID_DISPATCH_VIRTMAC_IPV4_MACADDR_ASSOCIATE    ((uint8) 0x37U)

/** @private \brief Eth_DispatchVirtmacIPv4MacAddrDisassociate() API Service ID */
#define ETH_SID_DISPATCH_VIRTMAC_IPV4_MACADDR_DISASSOCIATE ((uint8) 0x38U)

/** @private \brief Eth_DispatchVirtmacAddUnicastMacAddr() API Service ID */
#define ETH_SID_DISPATCH_VIRTMAC_ADD_UNICAST_MACADDR ((uint8) 0x39U)

/** @private \brief Eth_DispatchVirtmacAddMcastMacAddr() API Service ID */
#define ETH_SID_DISPATCH_VIRTMAC_ADD_MCAST_MACADDR ((uint8) 0x3AU)

/** @private \brief Eth_DispatchVirtmacDelMacAddr() API Service ID */
#define ETH_SID_DISPATCH_VIRTMAC_DEL_MACADDR ((uint8) 0x3BU)

/** @private \brief Eth_DispatchVirtmacAddVlan() API Service ID */
#define ETH_SID_DISPATCH_VIRTMAC_ADD_VLAN ((uint8) 0x3CU)

/** @private \brief Eth_DispatchVirtmacDelVlan() API Service ID */
#define ETH_SID_DISPATCH_VIRTMAC_DEL_VLAN ((uint8) 0x3DU)

/** \brief Eth_VirtMacRpcInit() API Service ID */
#define ETH_SID_VIRTMAC_RPC_INIT   ((uint8) 0x3FU)

/** \brief Eth_SetBandwidthLimit() API Service ID */
#define ETH_SID_SET_BANDWIDTH_LIMIT   ((uint8) 0x50U)

/** \brief Eth_GetBandwidthLimit() API Service ID */
#define ETH_SID_GET_BANDWIDTH_LIMIT   ((uint8) 0x51U)

/** \brief Eth_ReleaseRxBuffer() API Service ID */
#define ETH_SID_RELEASE_RX_BUFFER     ((uint8) 0x52U)

/** \brief Eth_ProvideExtTxBuffer() API Service ID */
#define ETH_SID_PROVIDE_EXT_TX_BUFFER ((uint8) 0x53U)

/** \brief Eth_GetTxHeaderPtr() API Service ID */
#define ETH_SID_GET_TX_HEADER_PTR     ((uint8) 0x54U)

/** \brief Eth_GetRxHeaderPtr() API Service ID */
#define ETH_SID_GET_RX_HEADER_PTR     ((uint8) 0x55U)

/** \brief Eth_RegisterReadback() API Service ID */
#define ETH_SID_REGISTER_READBACK    ((uint8) 0x56U)


/*! @} */

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  \brief This function initializes the driver.
 *
 *  \verbatim
 *  Service name      : Eth_Init
 *  Syntax            : void Eth_Init(
 *                          const Eth_ConfigType* CfgPtr
 *                      )
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   : 0x01
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Non Reentrant
 *  Parameters (in)   : CfPtr. Points to the implementation specific structure
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : None
 *  Description       : Initializes the Ethernet Driver.
 *  \endverbatim
 */
FUNC(void, ETH_CODE)
Eth_Init(P2CONST(Eth_ConfigType, AUTOMATIC, ETH_PBCFG) CfgPtr);

/**
 *  \brief This function enables / disables the indexed controller.
 *
 *  \verbatim
 *  Service name      : Eth_SetControllerMode
 *  Syntax            : Std_ReturnType Eth_SetControllerMode(
 *                          uint8 CtrlIdx,
 *                          Eth_ModeType CtrlMode
 *                      )
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   : 0x03
 *  Sync/Async        : Asynchronous
 *  Reentrancy        : Non Reentrant
 *  Parameters (in)   : CtrlIdx. Index of the controller within the context of the
 *                               Ethernet Driver
 *                      CtrlMode.
 *                        ETH_MODE_DOWN: disable the controller
 *                        ETH_MODE_ACTIVE: enable the controller
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : Std_ReturnType
 *                        E_OK: success
 *                        E_NOT_OK: controller mode could not be changed
 *  Description       : Enables / disables the indexed controller.
 *  \endverbatim
 */
FUNC(Std_ReturnType, ETH_CODE)
Eth_SetControllerMode(uint8 CtrlIdx,
                      Eth_ModeType CtrlMode);

#if (STD_ON == ETH_VIRTUALMAC_SUPPORT)
/**
 *  \brief This function enables / disables the indexed controller by Eth virtmac.
 *   This is only used for recovering CPSW process
 *
 *  \verbatim
 *  Service name      : Eth_SetControllerModeByVirtMac
 *  Syntax            : Std_ReturnType Eth_SetControllerModeByVirtMac(
 *                          uint8 CtrlIdx,
 *                          Eth_ModeType CtrlMode
 *                      )
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   : None
 *  Sync/Async        : Asynchronous
 *  Reentrancy        : Non Reentrant
 *  Parameters (in)   : CtrlIdx. Index of the controller within the context of the
 *                               Ethernet Driver
 *                      CtrlMode.
 *                        ETH_MODE_DOWN: disable the controller
 *                        ETH_MODE_ACTIVE: enable the controller
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : Std_ReturnType
 *                        E_OK: success
 *                        E_NOT_OK: controller mode could not be changed
 *  Description       : Enables / disables the indexed controller.
 *  \endverbatim
 */
FUNC(Std_ReturnType, ETH_CODE)
Eth_SetControllerModeByVirtMac(uint8 CtrlIdx,
                      Eth_ModeType CtrlMode);
#endif

/**
 *  \brief This function obtains the state of the indexed controller.
 *
 *  \verbatim
 *  Service name      : Eth_GetControllerMode
 *  Syntax            : Std_ReturnType Eth_GetControllerMode(
 *                          uint8 CtrlIdx,
 *                          Eth_ModeType *CtrlModePtr
 *                      )
 *  Mode              : User Mode (Non-Privileged Mode)
 *  Service ID[hex]   : 0x04
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Non Reentrant
 *  Parameters (in)   : CtrlIdx. Index of the controller within the context of the
 *                               Ethernet Driver
 *  Parameters (inout): CtrlModePtr
 *                        ETH_MODE_DOWN: the controller is disabled
 *                        ETH_MODE_ACTIVE: the controller is enabled
 *  Parameters (out)  : None
 *  Return value      : Std_ReturnType
 *                        E_OK: success
 *                        E_NOT_OK: controller mode could not be obtained
 *  Description       : Obtains the state of the indexed controller.
 *  \endverbatim
 */
FUNC(Std_ReturnType, ETH_CODE)
Eth_GetControllerMode(uint8 CtrlIdx,
                      P2VAR(Eth_ModeType, AUTOMATIC, ETH_APPL_DATA) CtrlModePtr);

/**
 *  \brief This function obtains the physical source address used by the indexed
 *         controller
 *
 *  \verbatim
 *  Service name      : Eth_GetPhysAddr
 *  Syntax            : void Eth_GetPhysAddr(
 *                          uint8 CtrlIdx,
 *                          uint8 *PhysAddrPtr
 *                      )
 *  Mode              : User Mode (Non-Privileged Mode)
 *  Service ID[hex]   : 0x08
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Non Reentrant
 *  Parameters (in)   : CtrlIdx. Index of the controller within the context of the
 *                               Ethernet Driver
 *  Parameters (inout):
 *  Parameters (out)  : PhysAddrPtr. Physical source address (MAC address) in network
 *                                   byte order
 *  Return value      : None
 *  Description       : Obtains the physical source address used by the indexed
 *                      controller.
 *  \endverbatim
 */
FUNC(void, ETH_CODE)
Eth_GetPhysAddr(uint8 CtrlIdx,
                P2VAR(uint8, AUTOMATIC, ETH_APPL_DATA) PhysAddrPtr);

/**
 *  \brief This function sets the physical source address used by the indexed
 *         controller.
 *
 *  \verbatim
 *  Service name      : Eth_SetPhysAddr
 *  Syntax            : void Eth_SetPhysAddr(
 *                          uint8 CtrlIdx,
 *                          const uint8* PhysAddrPtr
 *                      )
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   : 0x13
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Non Reentrant for the same CtrlIdx, reentrant for different
 *  Parameters (in)   : CtrlIdx. Index of the controller within the context of the
 *                               Ethernet Driver
 *                      PhysAddrPtr. Pointer to memory containing the physical source
 *                                   address (MAC address) in network byte order
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : None
 *  Description       : Sets the physical source address used by the indexed
 *                      controller.
 *  \endverbatim
 */
FUNC(void, ETH_CODE)
Eth_SetPhysAddr(uint8 CtrlIdx,
                P2CONST(uint8, AUTOMATIC, ETH_APPL_DATA) PhysAddrPtr);

#if (STD_ON == ETH_UPDATE_PHYS_ADDR_FILTER_API)
/**
 *  \brief This function updates the physical source address to / from the indexed
 *         controller filter.
 *
 *  \verbatim
 *  Service name      : Eth_UpdatePhysAddrFilter
 *  Syntax            : Std_ReturnType Eth_UpdatePhysAddrFilter(
 *                          uint8 CtrlIdx,
 *                          uint8* PhysAddrPtr,
 *                          Eth_FilterActionType Action
 *                      )
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   : 0x12
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Non Reentrant for the same CtrlIdx, reentrant for different
 *  Parameters (in)   : CtrlIdx. Index of the controller within the context of the
 *                               Ethernet Driver
 *                      PhysAddrPtr. Pointer to memory containing the physical source
 *                                   address (MAC address) in network byte order
 *                      Action. Add or remove the address from the Ethernet controllers
 *                              filter
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : Std_ReturnType
 *                        E_OK: filter was successfully changed
 *                        E_NOT_OK: filter could not be changed
 *  Description       : Updates the physical source address to/from the indexed ontroller
 *                      filter. If the Ethernet Controller is not capable to do the
 *                      filtering, the software has to do this.
 *  \endverbatim
 */
FUNC(Std_ReturnType, ETH_CODE)
Eth_UpdatePhysAddrFilter(uint8 CtrlIdx,
                         P2CONST(uint8, AUTOMATIC, ETH_APPL_DATA) PhysAddrPtr,
                         Eth_FilterActionType Action);
#endif /* (STD_ON == ETH_UPDATE_PHYS_ADDR_FILTER_API) */

#if (STD_ON == ETH_ENABLE_MII_API)
/**
 *  \brief This function configures a transceiver register or triggers a
 *         function offered by the receiver Service.
 *
 *  \verbatim
 *  Service name      : Eth_WriteMii
 *  Syntax            : Std_ReturnType Eth_WriteMii(
 *                          uint8 CtrlIdx,
 *                          uint8 TrcvIdx,
 *                          uint8 RegIdx,
 *                          uint16 RegVal
 *                      )
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   : 0x05
 *  Sync/Async        : Asynchronous
 *  Reentrancy        : Non Reentrant
 *  Parameters (in)   : CtrlIdx. Index of the controller within the context of the
 *                               Ethernet Driver
 *                      TrcvIdx. Index of the transceiver on the MII
 *                      RegIdx.Index of the transceiver register on the MII
 *                      RegVal. Value to be written into the indexed registerNone
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : Std_ReturnType
 *                        E_OK: Service accepted
 *                        E_NOT_OK: Service denied
 *                        ETH_E_NO_ACCESS: Ethernet transceiver access failure
 *  Description       : Configures a transceiver register or triggers a function
 *                      offered by the receiver.
 *  \endverbatim
 */
FUNC(Std_ReturnType, ETH_CODE)
Eth_WriteMii(uint8 CtrlIdx,
             uint8 TrcvIdx,
             uint8 RegIdx,
             uint16 RegVal);

/**
 *  \brief This function reads a transceiver register.
 *
 *  \verbatim
 *  Service name      : Eth_ReadMii
 *  Syntax            : Std_ReturnType Eth_ReadMii(
 *                          uint8 CtrlIdx,
 *                          uint8 TrcvIdx,
 *                          uint8 RegIdx,
 *                          uint16* RegValPtr
 *                      )
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   : 0x06
 *  Sync/Async        : Asynchronous
 *  Reentrancy        : Non Reentrant
 *  Parameters (in)   : CtrlIdx. Index of the controller within the context of the
 *                               Ethernet Driver
 *                      TrcvIdx. Index of the transceiver on the MII
 *                      RegIdx. Index of the transceiver register on the MII
 *                      RegVal. Value to be written into the indexed register
 *  Parameters (inout): None
 *  Parameters (out)  : RegValPtr. Filled with the register content of the indexed
 *                                 register
 *  Return value      : Std_ReturnType
 *                        E_OK: Service accepted
 *                        E_NOT_OK: Service denied
 *                        ETH_E_NO_ACCESS: Ethernet transceiver access failure
 *  Description       : Reads a transceiver register.
 *  \endverbatim
 */
FUNC(Std_ReturnType, ETH_CODE)
Eth_ReadMii(uint8 CtrlIdx,
            uint8 TrcvIdx,
            uint8 RegIdx,
            P2VAR(uint16, AUTOMATIC, ETH_APPL_DATA) RegValPtr);
#endif /* (STD_ON == ETH_ENABLE_MII_API) */

#if (STD_ON == ETH_GET_COUNTER_VALUES_API)
/**
 *  \brief This function reads a list with counter values of the
 *         corresponding controller.
 *
 *  \verbatim
 *  Service name      : Eth_GetCounterValues
 *  Syntax            : Std_ReturnType Eth_GetCounterValues(uint8 CtrlIdx,
 *                                                          Eth_CounterType* CounterPtr);
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   : 0x14
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Non Reentrant
 *  Parameters (in)   : CtrlIdx. Index of the controller within the context of the
 *                               Ethernet Driver
 *  Parameters (out)  : CounterPtr. counter values according to IETF RFC 1757, RFC 1643 and
 *                      RFC 2233. Refer to the \ref Eth_CounterType.
 *  Return value      : Std_ReturnType
 *                        E_OK: success
 *                        E_NOT_OK: drop counter could not be obtained
 *  Description       : Reads a list with drop counter values of the corresponding controller.
 *                      The meaning of these values is described at Eth_CounterType
 *  \endverbatim
 */
FUNC(Std_ReturnType, ETH_CODE)
Eth_GetCounterValues(uint8 CtrlIdx,
                     Eth_CounterType* CounterPtr);
#endif /* (STD_ON == ETH_GET_COUNTER_VALUES_API) */

#if (STD_ON == ETH_GET_COUNTER_STATE_API)
/**
 *  \brief Reads the value of a counter specified with its memory offset.
 *
 *  \verbatim
 *  Service name      : Eth_GetCounterState
 *  Syntax            : Std_ReturnType Eth_GetCounterState(uint8 CtrlIdx,
 *                                                         uint16 CtrOffs,
 *                                                         uint32* CtrValPtr);
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   : 0x07
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Non Reentrant
 *  Parameters (in)   : CtrlIdx. Index of the controller within the context of the
 *                               Ethernet Driver
 *  Parameters (in)   : CtrOffs. Memory offset of the counter. The offset is controller specific.
 *  Parameters (out)  : CtrValPtr. Filled with the content of the specified counter
 *  Return value      : Std_ReturnType
 *                        E_OK: success
 *                        E_NOT_OK: specified counter offset not be obtained.
 *  Description       : Reads the value of a counter specified with its memory offset.
 *  \endverbatim
 */
FUNC(Std_ReturnType, ETH_CODE)
Eth_GetCounterState(uint8 CtrlIdx,
                    uint16 CtrOffs,
                    uint32* CtrValPtr);
#endif /* (STD_ON == ETH_GET_COUNTER_STATE_API) */

#if (STD_ON == ETH_GET_RX_STATS_API)
/**
 *  \brief This function reads a list with RX statistics values of the
 *          corresponding controller.
 *
 *  \verbatim
 *  Service name      : Eth_GetRxStats
 *  Syntax            : Std_ReturnType Eth_GetRxStats(
 *                          uint8 CtrlIdx,
 *                          Eth_RxStatsType* RxStats
 *                      )
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   : 0x15
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Non Reentrant
 *  Parameters (in)   : CtrlIdx. Index of the controller within the context of the
 *                               Ethernet Driver
 *  Parameters (inout): None
 *  Parameters (out)  : RxStats. List of values according to IETF RFC 2819 (Remote Network
 *                      Monitoring Management Information Base). Refer to the \ref Eth_RxStatsType.
 *  Return value      : Std_ReturnType
 *                        E_OK: success
 *                        E_NOT_OK: drop counter could not be obtained
 *  Description       :
 *  \endverbatim
 */
FUNC(Std_ReturnType, ETH_CODE)
Eth_GetRxStats(uint8 CtrlIdx,
               P2VAR(Eth_RxStatsType, AUTOMATIC, ETH_APPL_DATA) RxStats);
#endif /* (STD_ON == ETH_GET_RX_STATS_API) */

#if (STD_ON == ETH_GET_TX_STATS_API)
/**
 *  \brief This function reads a list with TX statistics values of the
 *          corresponding controller.
 *
 *  \verbatim
 *  Service name      : Eth_GetTxStats
 *  Syntax            : Std_ReturnType Eth_GetTxStats(
 *                          uint8 CtrlIdx,
 *                          Eth_TxStatsType* TxStats
 *                      )
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   : 0x15
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Non Reentrant
 *  Parameters (in)   : CtrlIdx. Index of the controller within the context of the
 *                               Ethernet Driver
 *  Parameters (inout): None
 *  Parameters (out)  : TxStats. List of values according to IETF RFC 2819 (Remote Network
 *                      Monitoring Management Information Base). Refer to the \ref Eth_TxStatsType.
 *  Return value      : Std_ReturnType
 *                        E_OK: success
 *                        E_NOT_OK: drop counter could not be obtained
 *  Description       :
 *  \endverbatim
 */
FUNC(Std_ReturnType, ETH_CODE)
Eth_GetTxStats(uint8 CtrlIdx,
               P2VAR(Eth_TxStatsType, AUTOMATIC, ETH_APPL_DATA) TxStats);
#endif /* (STD_ON == ETH_GET_TX_STATS_API) */

#if (STD_ON == ETH_GET_TX_ERROR_COUNTERSVALUES_API)
/**
 *  \brief This function reads a list of values to read statistic error counter values
 *          for transmission for corresponding controller.
 *
 *  \verbatim
 *  Service name      : Eth_GetTxErrorCounterValues
 *  Syntax            : Eth_GetTxErrorCounterValues(uint8 CtrlIdx,
 *                            Eth_TxErrorCounterValuesType *TxErrorCounterValues)
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   : 0x15
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Non Reentrant
 *  Parameters (in)   : CtrlIdx. Index of the controller within the context of the
 *                               Ethernet Driver
 *  Parameters (inout): None
 *  Parameters (out)  : TxErrorCounterValues. List of values to read statistic error counter values
 *                      for transmission. Refer to the \ref Eth_TxErrorCounterValuesType.
 *  Return value      : Std_ReturnType
 *                        E_OK: success
 *                        E_NOT_OK: drop counter could not be obtained
 *  Description       :
 *  \endverbatim
 */
FUNC(Std_ReturnType, ETH_CODE)
Eth_GetTxErrorCounterValues(uint8 CtrlIdx,
               P2VAR(Eth_TxErrorCounterValuesType, AUTOMATIC, ETH_APPL_DATA) TxErrorCounterValues);
#endif

#if (STD_ON == ETH_GLOBALTIMESUPPORT_API)
/**
 *  \brief This function returns a time value out of the HW registers.
 *
 *  \verbatim
 *  Service name      : Eth_GetCurrentTime
 *  Syntax            : Std_ReturnType Eth_GetCurrentTime(
 *                          uint8 CtrlIdx,
 *                          Eth_TimeStampQualType *timeQualPtr,
 *                          Eth_TimeStampType *timeStampPtr
 *                      )
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   : 0x16
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Non Reentrant
 *  Parameters (in)   : CtrlIdx. Index of the controller within the context of the
 *                               Ethernet Driver
 *  Parameters (inout): None
 *  Parameters (out)  : timeQualPtr. Quality of HW time stamp, e.g. based on current
 *                                   drift
 *  Return value      : Std_ReturnType
 *                        E_OK: success
 *                        E_NOT_OK: failed
 *  Description       : Returns a time value out of the HW registers according
 *                      to the capability of the HW. Is the HW resolution is lower than
 *                      the Eth_TimeStampType resolution resp. range, than an the
 *                      remaining bits will be filled with 0.
 *  \endverbatim
 */
FUNC(Std_ReturnType, ETH_CODE)
Eth_GetCurrentTime(uint8 CtrlIdx,
                   P2VAR(Eth_TimeStampQualType, AUTOMATIC, ETH_APPL_DATA) timeQualPtr,
                   P2VAR(Eth_TimeStampType, AUTOMATIC, ETH_APPL_DATA) timeStampPtr);

/**
 *  \brief This function activates egress time stamping on a dedicated message
 *         object.
 *
 *  \verbatim
 *  Service name      : Eth_EnableEgressTimeStamp
 *  Syntax            : void Eth_EnableEgressTimeStamp(
 *                          uint8 CtrlIdx,
 *                          Eth_BufIdxType BufIdx
 *                      )
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   : 0x17
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Non Reentrant
 *  Parameters (in)   : CtrlIdx. Index of the controller within the context of the
 *                               Ethernet Driver
 *                      BufIdx. Index of the message buffer, where Application
 *                              expects egress time stamping
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : None
 *  Description       : Activates egress time stamping on a dedicated message
 *                      object.
 *                      Some HW does store once the egress time stamp marker and
 *                      some HW needs it always before transmission. There will be
 *                      no disable functionality, due to the fact, that the message
 *                      type is always "time stamped" by network design.
 *  \endverbatim
 */
FUNC(void, ETH_CODE)
Eth_EnableEgressTimeStamp(uint8 CtrlIdx, Eth_BufIdxType BufIdx);

/**
 *  \brief This function reads back the egress time stamp on a dedicated message
 *         object.
 *
 *  \verbatim
 *  Service name      : Eth_GetEgressTimeStamp
 *  Syntax            : void Eth_GetEgressTimeStamp(
 *                          uint8 CtrlIdx,
 *                          Eth_BufIdxType BufIdx,
 *                          Eth_TimeStampQualType* timeQualPtr,
 *                          Eth_TimeStampType *timeStampPtr
 *                          )
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   : 0x18
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Non Reentrant
 *  Parameters (in)   : CtrlIdx. Index of the controller within the context of the
 *                               Ethernet Driver
 *                      BufIdx. Index of the message buffer, where Application
 *                              expects egress time stamping
 *  Parameters (inout): timeQualPtr. Quality of HW time stamp, e.g. based on current
 *                                   drift
 *                      timeStampPtr. Current time stamp
 *  Parameters (out)  : None
 *  Return value      : None
 *  Description       : Reads back the egress time stamp on a dedicated message
 *                      object.
 *                      It must be called within the TxConfirmation() function.
 *  \endverbatim
 */
FUNC(void, ETH_CODE)
Eth_GetEgressTimeStamp(uint8 CtrlIdx,
                       Eth_BufIdxType BufIdx,
                       P2VAR(Eth_TimeStampQualType, AUTOMATIC, ETH_APPL_DATA) timeQualPtr,
                       P2VAR(Eth_TimeStampType, AUTOMATIC, ETH_APPL_DATA) timeStampPtr);

/**
 *  \brief This function reads back the ingress time stamp on a dedicated
 *         message object.
 *
 *  \verbatim
 *  Service name      : Eth_GetIngressTimeStamp
 *  Syntax            : void Eth_GetIngressTimeStamp(
 *                          uint8 CtrlIdx,
 *                          Eth_DataType* DataPtr,
 *                          Eth_TimeStampQualType* timeQualPtr,
 *                          Eth_TimeStampType *timeStampPtr
 *                      )
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   : 0x19
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Non Reentrant
 *  Parameters (in)   : CtrlIdx. Index of the controller within the context of the
 *                               Ethernet Driver
 *                      DataPtr. Pointer to the message buffer, where Application
 *                               expects ingress time stamping
 *  Parameters (inout): timeQualPtr. Quality of HW time stamp, e.g. based on current
 *                                   drift
 *                      timeStampPtr. Current time stamp
 *  Parameters (out)  : None
 *  Return value      : None
 *  Description       : Reads back the ingress time stamp on a dedicated message
 *                      object.
 *                      It must be called within the RxIndication() function.
 *  \endverbatim
 */
FUNC(void, ETH_CODE)
Eth_GetIngressTimeStamp(uint8 CtrlIdx,
                        P2VAR(Eth_DataType, AUTOMATIC, ETH_APPL_DATA) DataPtr,
                        P2VAR(Eth_TimeStampQualType, AUTOMATIC, ETH_APPL_DATA) timeQualPtr,
                        P2VAR(Eth_TimeStampType, AUTOMATIC, ETH_APPL_DATA) timeStampPtr);

#endif /* (STD_ON == ETH_GLOBALTIMESUPPORT_API) */

/**
 *  \brief This function provides access to a transmit buffer of the specified
 *         controller.
 *
 *  \verbatim
 *  Service name      : Eth_ProvideTxBuffer
 *  Syntax            : BufReq_ReturnType Eth_ProvideTxBuffer(
 *                          uint8 CtrlIdx,
 *                          uint8 Priority,
 *                          uint8* BufIdxPtr,
 *                          uint8** BufPtr,
 *                          uint16* LenBytePtr
 *                      )
 *  Mode              : User Mode (Non-Privileged Mode)
 *  Service ID[hex]   : 0x09
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Non Reentrant
 *  Parameters (in)   : CtrlIdx. Index of the controller within the context of the
 *                               Ethernet Driver
 *  Parameters (in)   : Priority. Frame priority for transmit buffer FIFO selection
 *  Parameters (inout): LenBytePtr
 *                        In: desired length in bytes, out: granted length in bytes
 *  Parameters (out)  : BufIdxPtr. Index to the granted buffer resource. To be used
 *                                 for subsequent requests
 *                      BufPtr. Pointer to the granted buffer
 *  Return value      : BufReq_ReturnType
 *                        BUFREQ_OK: success
 *                        BUFREQ_E_NOT_OK: development error detected
 *                        BUFREQ_E_BUSY: all buffers in use
 *                        BUFREQ_E_OVFL: requested buffer too large
 *  Description       : Provides access to a transmit buffer of the specified
 *                      controller.
 *  \endverbatim
 */
FUNC(BufReq_ReturnType, ETH_CODE)
Eth_ProvideTxBuffer(uint8 CtrlIdx,
                    uint8 Priority,
                    P2VAR(Eth_BufIdxType, AUTOMATIC, ETH_APPL_DAT) BufIdxPtr,
                    P2VAR(uint8, AUTOMATIC, ETH_APPL_DAT) *BufPtr,
                    P2VAR(uint16, AUTOMATIC, ETH_APPL_DAT) LenBytePtr);

/**
 *  \brief This function triggers transmission of a previously filled transmit
 *         buffer.
 *
 *  \verbatim
 *  Service name      : Eth_Transmit
 *  Syntax            : Std_ReturnType Eth_Transmit(
 *                          uint8 CtrlIdx,
 *                          Eth_BufIdxType BufIdx,
 *                          Eth_FrameType FrameType,
 *                          boolean TxConfirmation,
 *                          uint16 LenByte,
 *                          uint8 *PhysAddrPtr)
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   : 0xA
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Non Reentrant
 *  Parameters (in)   : CtrlIdx. Index of the controller within the context of the
 *                               Ethernet Driver
 *                      BufIdx. Index of the buffer resource
 *                      FrameType. Ethernet frame type
 *                      TxConfirmation. Activates transmission confirmation
 *                      LenByte. Data length in byte
 *                      PhysAddrPtr. Physical target address (MAC address) in
 *                                   network byte order
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : Std_ReturnType
 *                        E_OK: success
 *                        E_NOT_OK: transmission failed
 *  Description       : Triggers transmission of a previously filled transmit
 *                      buffer.
 *  \endverbatim
 */
FUNC(Std_ReturnType, ETH_CODE)
Eth_Transmit(uint8 CtrlIdx,
             Eth_BufIdxType BufIdx,
             Eth_FrameType FrameType,
             boolean TxConfirmation,
             uint16 LenByte,
             P2CONST(uint8, AUTOMATIC, ETH_APPL_DAT) PhysAddrPtr);

/**
 *  \brief This function triggers frame reception.
 *
 *  \verbatim
 *  Service name      : Eth_Receive
 *  Syntax            : void Eth_Receive(
 *                          uint8 CtrlIdx,
 *                          uint8 FifoIdx,
 *                          Eth_RxStatusType* RxStatusPtr
 *                      )
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   : 0xB
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Non Reentrant
 *  Parameters (in)   : CtrlIdx. Index of the controller within the context of the
 *                               Ethernet Driver
 *  Parameters (in)   : FifoIdx. Specifies the related fifo
 *  Parameters (inout): None
 *  Parameters (out)  : RxStatusPtr. Indicates whether a frame has been received and
 *                                   if so, whether more frames are available or
 *                                   frames got lost
 *  Return value      : None
 *  Description       : Triggers frame reception.
 *  \endverbatim
 */
FUNC(void, ETH_CODE)
Eth_Receive(uint8 CtrlIdx,
            uint8 FifoIdx,
            P2VAR(Eth_RxStatusType, AUTOMATIC, ETH_APPL_DAT) RxStatusPtr);

/**
 *  \brief This function triggers frame transmission confirmation.
 *
 *  \verbatim
 *  Service name      : Eth_TxConfirmation
 *  Syntax            : void Eth_TxConfirmation(
 *                          uint8 CtrlIdx
 *                      )
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   : 0xC
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Non Reentrant
 *  Parameters (in)   : CtrlIdx. Index of the controller within the context of the
 *                                Ethernet Driver
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : None
 *  Description       : Triggers frame transmission confirmation.
 *  \endverbatim
 */
FUNC(void, ETH_CODE)
Eth_TxConfirmation(uint8 CtrlIdx);

#if (STD_ON == ETH_VERSION_INFO_API)
/**
 *  \brief Function returns the version information of this module.
 *
 *  \verbatim
 *  Service name      : Eth_GetVersionInfo
 *  Syntax            : void Eth_GetVersionInfo(
 *                          Std_VersionInfoType* versioninfo
 *                      )
 *  Mode              : User Mode (Non-Privileged Mode)
 *  Service ID[hex]   : 0xD
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Reentrant
 *  Parameters (in)   : None
 *  Parameters (inout): None
 *  Parameters (out)  : VersionInfoPtr. Pointer to where to store the version
 *                      information of this module
 *  Return value      : None
 *  Description       : Returns the version information of this module.
 *  \endverbatim
 */
FUNC(void, ETH_CODE)
Eth_GetVersionInfo(P2VAR(Std_VersionInfoType, AUTOMATIC, ETH_APPL_DATA) VersionInfo);
#endif  /* (STD_ON == ETH_VERSION_INFO_API) */

/**
 *  \brief The function checks for controller errors and lost frames. Used for
 *         polling state changes. Calls EthIf_CtrlModeIndication when the
 *         controller mode changed.
 *
 *  \verbatim
 *  Service name      : Eth_MainFunction
 *  Syntax            : void Eth_MainFunction(
 *                          void
 *                      )
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   : 0x0A
 *  Description       : The function checks for controller errors and lost frames.
 *                      Used for polling state changes. Calls EthIf_CtrlModeIndication
 *                      when the controller mode changed.
 *  \endverbatim
 */
FUNC(void, ETH_CODE)
Eth_MainFunction(void);

#if (STD_ON == ETH_TRAFFIC_SHAPING_API)
/**
 *  \brief This function set bandwidth for a specific transmitsion queue
 *
 *  \verbatim
 *  Service name      : Eth_Receive
 *  Syntax            : FUNC(Std_ReturnType, ETH_CODE) Eth_SetBandwidthLimit(
                              uint8   CtrlIdx,
                              uint8   QueuePrio,
                              uint32  BandwidthLimit
                        );
 *  Mode              : User Mode (Non-Privileged Mode)
 *  Service ID[hex]   : 0x2C
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Non Reentrant
 *  Parameters (in)   : CtrlIdx. Index of the controller within the context of the
 *                               Ethernet Driver
 *  Parameters (in)   : QueuePrio. Specifies the related fifo
 *                      BandwidthLimit. Bandwidth value
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : Std_ReturnType
 *                        E_OK: success
 *                        E_NOT_OK: configure bandwidth failed
 *  Description       : This function set bandwidth for a specific transmitsion queue.
 *  \endverbatim
 */
FUNC(Std_ReturnType, ETH_CODE) Eth_SetBandwidthLimit(
  uint8   CtrlIdx,
  uint8   QueuePrio,
  uint32  BandwidthLimit);

/**
 *  \brief This function get bandwidth for a specific transmitsion queue
 *
 *  \verbatim
 *  Service name      : Eth_Receive
 *  Syntax            : FUNC(Std_ReturnType, ETH_CODE) Eth_GetBandwidthLimit(
                            uint8              CtrlIdx,
                            uint8              QueuePrio,
                            P2VAR(uint32, AUTOMATIC, ETH_APPL_DAT)  BandwidthLimitPtr
                        );
 *  Mode              : User Mode (None-Privileged Mode)
 *  Service ID[hex]   : 0x2D
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Non Reentrant
 *  Parameters (in)   : CtrlIdx. Index of the controller within the context of the
 *                               Ethernet Driver
 *  Parameters (in)   : QueuePrio. Specifies the related fifo
 *  Parameters (inout): BandwidthLimitPtr Pointer to where to store the currently configured bandwidth limit (in [bit/s])

 *  Return value      : Std_ReturnType
 *                        E_OK: success
 *                        E_NOT_OK: retrive bandwidth failed
 *  Description       : This function get bandwidth for a specific transmitsion queue.
 *  \endverbatim
 */
FUNC(Std_ReturnType, ETH_CODE) Eth_GetBandwidthLimit(
    uint8              CtrlIdx,
    uint8              QueuePrio,
    P2VAR(uint32, AUTOMATIC, ETH_APPL_DAT)  BandwidthLimitPtr);
#endif /* (STD_ON == ETH_TRAFFIC_SHAPING_API) */

#if (STD_ON == ETH_ZERO_COPY_API)
/**
 *  \brief This function releases an RX buffer and prepares the buffer to
 *          be reused during reception.
 *  \verbatim
 *  Service name      : Eth_ReleaseRxBuffer
 *  Syntax            : FUNC(Std_ReturnType, ETH_CODE) Eth_ReleaseRxBuffer(uint8 CtrlIdx,
 *                          P2VAR(Eth_DataType, AUTOMATIC, ETH_APPL_DATA) BufPtr
 *                      );
 *
 *  Mode              : User Mode (None-Privileged Mode)
 *  Service ID[hex]   : 0x30
 *  \param (in)       : CtrlIdx. Index of the controller within the context of the
 *                                   Ethernet Driver
 *  \param (in)       : BufPtr. Pointer to the reception buffer
 *
 *  \return value     : E_NOT_OK : Buffer could not be released. This may only happen with a
 *                                  wrong BufPtr or when DET is enabled.
 *                      E_OK : Buffer successfully released.
 *
 *  \endverbatim
 */
FUNC(Std_ReturnType, ETH_CODE) Eth_ReleaseRxBuffer(uint8 CtrlIdx,
    P2VAR(Eth_DataType, AUTOMATIC, ETH_APPL_DATA) BufPtr);

/**
 *  \brief This function provides an external buffer for frame transmission.
 *          The buffer is supposed to be locked until transmission
 *          confirmation is called.
 *
 *  \verbatim
 *  Service name      : Eth_ProvideExtTxBuffer
 *  Syntax            : FUNC(BufReq_ReturnType, ETH_CODE) Eth_ProvideExtTxBuffer(uint8 CtrlIdx,
                            uint8 Priority,
                            P2VAR(Eth_BufIdxType, AUTOMATIC, ETH_APPL_DATA) BufIdxPtr,
                            P2VAR(uint8, AUTOMATIC, ETH_APPL_DATA) *BufPtr,
                            P2VAR(uint16, AUTOMATIC, ETH_APPL_DATA) LenBytePtr
 *                      );
 *
 *  Mode              : User Mode (None-Privileged Mode)
 *  Service ID[hex]   : 0x31
 *
 *  \param (in)   : CtrlIdx. Index of the controller within the context of the
 *                               Ethernet Driver
 *  \param (in)   : FifoIdx. FIFO selection
 *  \param (inout): LenBytePtr The length of the external buffer
 *  \param (out)  : BufIdxPtr. Index to the granted buffer resource. To be used
 *                                 for subsequent requests
 *                      BufPtr. Pointer to an external buffer location
 *  \return value      : BufReq_ReturnType
 *                        BUFREQ_OK: success
 *                        BUFREQ_E_NOT_OK: development error detected
 *                        BUFREQ_E_BUSY: all buffers in use
 *                        BUFREQ_E_OVFL: requested buffer too large
 *
 *  \endverbatim
 */
FUNC(BufReq_ReturnType, ETH_CODE) Eth_ProvideExtTxBuffer(uint8 CtrlIdx,
    uint8 Priority,
    P2VAR(Eth_BufIdxType, AUTOMATIC, ETH_APPL_DATA) BufIdxPtr,
    P2VAR(Eth_DataType, AUTOMATIC, ETH_APPL_DATA) *BufPtr,
    P2VAR(uint16, AUTOMATIC, ETH_APPL_DATA) LenBytePtr);
#endif /* (STD_ON == ETH_ZERO_COPY_API) */

#if (STD_ON == ETH_HEADER_ACCESS_API)
/**
 *  \brief This function returns the Ethernet header portion of a transmission frame.
 *          This allows the user to overwrite the Ethernet header according to his needs.
 *          Otherwise the header is written by the driver. Call this function after a call to
 *          Eth_ProvideTxBuffer or Eth_ProvideExtTxBuffer.
 *
 *  \verbatim
 *  Service name      : Eth_GetTxHeaderPtr
 *  Syntax            : FUNC(Std_ReturnType, ETH_CODE) Eth_GetTxHeaderPtr(uint8 CtrlIdx,
                            uint8 BufIdx,
                            P2VAR(Eth_DataType, AUTOMATIC, ETH_APPL_DATA) *BufPtr,
                            P2VAR(uint16, AUTOMATIC, ETH_APPL_DATA) LenBytePtr)
 *                      );
 *
 *  Mode              : User Mode (None-Privileged Mode)
 *  Service ID[hex]   : 0x32
 *
 *
 *  \param (in)   : CtrlIdx. Controller index
 *  \param (in)   : BufIdx. Buffer index
 *  \param (out)  : BufPtr. Pointer to buffer area that holds the Ethernet transmission header
 *  \param (inout): LenBytePtr The Ethernet header length. This is always 14 bytes.
 *  \return value      : E_OK : Success
 *                       E_NOT_OK : Either the buffer associated with BufIdx is in a wrong state.
 *
 *  \endverbatim
 */
FUNC(Std_ReturnType, ETH_CODE) Eth_GetTxHeaderPtr(uint8 CtrlIdx,
    uint8 BufIdx,
    P2VAR(Eth_DataType, AUTOMATIC, ETH_APPL_DATA) *BufPtr,
    P2VAR(uint16, AUTOMATIC, ETH_APPL_DATA) LenBytePtr);

/**
 *  \brief This function returns the pointer to the first octet of a received Ethernet frame.
 *          That allows access to the Ethernet header as well as the Ethernet payload.
 *          Call this function after the EthIf_RxIndication function is called.
 *
 *  \verbatim
 *  Service name      : Eth_GetRxHeaderPtr
 *  Syntax            : FUNC(Std_ReturnType, ETH_CODE) Eth_GetRxHeaderPtr(uint8 CtrlIdx,
                            P2VAR(Eth_DataType, AUTOMATIC, ETH_APPL_DATA) *BufPtr,
                            P2VAR(uint16, AUTOMATIC, ETH_APPL_DATA) LenBytePtr)
 *                      );
 *  Mode              : User Mode (None-Privileged Mode)
 *  Service ID[hex]   : 0x33
 *  \param (in)   : CtrlIdx. Controller index
 *  \param (out)  : BufPtr. Pointer to the payload portion of the received Ethernet frame.
 *                          This pointer is passed to the EthIf_RxIndication callback function
 *  \param (inout): LenBytePtr The Ethernet header length. This is always 14 bytes.
 *  \return value      : E_OK : Success
 *                       E_NOT_OK : Either the buffer associated with BufIdx is in a wrong state.
 *
 *  \endverbatim
 */
FUNC(Std_ReturnType, ETH_CODE) Eth_GetRxHeaderPtr(uint8 CtrlIdx,
    P2VAR(Eth_DataType, AUTOMATIC, ETH_APPL_DATA) *BufPtr,
    P2VAR(uint16, AUTOMATIC, ETH_APPL_DATA) LenBytePtr);
#endif /* (STD_ON == ETH_HEADER_ACCESS_API) */

#if (STD_ON == ETH_REGISTER_READBACK_API)
/**
 * \brief This service will readback Eth registers
 *
 * \verbatim
 *
 *  Service name      : Eth_RegisterReadback
 *  Syntax            : Std_ReturnType Eth_RegisterReadback(uint8 CtrlIdx)
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   : 0x56
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Reentrant
 *  Parameters (in)   : CtrlIdx
 *                      - Eth Controller to readback registers configuration.
 *  Parameters (inout): RegRbPtr - Pointer to where to store the readback
 *                      values. If this pointer is NULL_PTR, then the API
 *                      will return E_NOT_OK.
 *  Parameters (out)  : None
 *  Return value      : Std_ReturnType
 *                      - E_OK: Register readback successful.
 *                      - E_NOT_OK: Register readback failed.
 *  Description       : This service will readback Eth registers.
 * \endverbatim
 */

Std_ReturnType Eth_RegisterReadback(
    VAR(uint8, AUTOMATIC)CtrlIdx,
    P2VAR(Eth_RegisterReadbackType, AUTOMATIC, ETH_APPL_DATA) RegRbPtr);
#endif /* (STD_ON == ETH_REGISTER_READBACK_API) */

#ifdef __cplusplus
}
#endif

#endif  /* ETH_H_ */

/*! @} */
