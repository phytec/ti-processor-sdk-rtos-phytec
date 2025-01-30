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
 *  \file     Eth_Priv.h
 *
 *  \brief    This file contains the common declarations and macros used across
 *            all the Ethernet driver files.
 */

#ifndef ETH_PRIV_H_
#define ETH_PRIV_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "Eth.h"
#include "Eth_Packet.h"
#include "Eth_Rpc.h"
#include ETH_ETHIF_CBK_HEADER

#if (STD_ON == ETH_DEV_ERROR_DETECT)
#include "Det.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                                 Macros                                     */
/* ========================================================================== */

/** \brief Host to network byte order conversion for short integer */
#define htons(a)                        ((((a) & 0x00FFU) << 8U) | \
                                         (((a) & 0xFF00U) >> 8U))

/** \brief Host to network byte order conversion for long integer */
#define htonl(a)                        ((((a) & 0xFF000000U) >> 24U) | \
                                         (((a) & 0x00FF0000U) >>  8U) | \
                                         (((a) & 0x0000FF00U) <<  8U) | \
                                         (((a) & 0x000000FFU) << 24U))

/** \brief Network to host byte order conversion for short integer */
#define ntohl(a)                        htonl(a)

/** \brief Network to host byte order conversion for long integer */
#define ntohs(a)                        htons(a)

/** \brief Size of an array */
#define ARRAY_SIZE(x)                   (sizeof((x)) / sizeof(x[0U]))

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/**
 *  \brief Eth driver state to detect and process when error happens
 */
typedef enum Eth_RecoveryState_e
{
    ETH_RECOVERY_STATE_NONE                              = 0x0,
    /**< Recovery state none  */
    ETH_RECOVERY_STATE_ERROR_DETECTED                    = 0x1,
    /**< Receive error detected from EthFw server */
    ETH_RECOVERY_STATE_REQ_SERVER_TEARDONW_COMPLETE      = 0x2,
    /**< Notify server that reseting is completed */
    ETH_RECOVERY_STATE_WAIT_SERVER_ACK_TEARDONW_COMPLETE = 0x3,
    /**< Wait server that acknoleage teardow complete*/    
    ETH_RECOVERY_STATE_WAIT_SERVER_RECOVERY_COMPLETE     = 0x4,
    /**< Wait server to reset corevery hardware complete */
    ETH_RECOVERY_STATE_RESTORING                         = 0x5,
    /**< Restore controller setting */
    ETH_RECOVERY_STATE_MAX                               = 0x6
    /**< Recovery state max */
} Eth_RecoveryState;

/**
 *  \brief Eth driver virtual mac info structure
 *  Configuration related to attached virtual Mac port
 */
typedef struct Eth_VirtualMacInfo_s
{
    uint32 features;
    /**< Virtual MAC port features */
    uint32 token;
    /**< Token given by ETHFW after successful attach */
    uint32 reqId;
    /**< Reuqest id, increase after every command */
    uint32 hostPortRxMtu;
    /**< Max Rx packet size */
    uint32 txMtu[ETHREMOTECFG_PRIORITY_NUM];
    /**< Max Tx packet size per priority */
    uint32 txPSILThreadId;
    /**< Tx Channel Cpsw Peer thread Id */
    uint32 rxFlowStartIdx;
    /**< Flow index base value  */
    uint32 rxFlowIdx;
    /**< Flow index offset for flow used  */
    uint8 macAddress[ETHREMOTECFG_MACADDRLEN];
    /**< Destination Mac Address allocated for the flow */
    EthRemoteCfg_FwVerInfo fwInfo;
    /**< Firmware info exported by ethernet firmware when it attaches to Eth Driver */
    boolean ethFwAttached;
    /**< Flag indicating firmware handshake completed */
    boolean detachResponsePending;
    /**< Flag indicating DETACH completion notification pending */
    uint32 responseTimeoutCnt;
    /**< timeout counter for resending annoucement if fw attached response
     *   is not received from ethernet firmwar */
    uint32 macOnlyPort;
    /**< MAC-only port number. 1-based value for ports in MAC-only mode,
     *   for switch ports */
    volatile uint8 s2CNotifyId;
    /* Server to client notification ID */
    volatile Eth_RecoveryState recoverState;
    /**< Cpsw client recovery state */    
    volatile boolean teardownResponsePending;
    /**< Flag indicating TEARDOWN completion notification pending */    
    Eth_ModeType modeBeforeRecovery;
    /**< Mode of eth controller before start to recovery process */
} Eth_VirtualMacInfo;

/* ========================================================================== */
/*                         Global Variables Declarations                      */
/* ========================================================================== */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */
/**
 *  \brief Open the CPSW common module driver
 *
 *  Opens the underlying CPSW driver.
 *
 *
 *  \return         none
 */
void Eth_openCpswCommon(void);

/**
 *  \brief Open the CPSW driver for a eth controller
 *
 *  Opens the underlying CPSW driver for a eth controller.
 *
 *  \param ctrlIdx  Eth control idx
 *
 *  \return         E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType Eth_openCpsw(uint8 ctrlIdx);

/**
 *  \brief Closes the CPSW driver
 *
 *  Closes the underlying CPSW driver.
 *
 *  \param ctrlIdx  Eth control idx
 *
 *  \return         E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType Eth_closeCpsw(uint8 ctrlIdx);

/**
 *  \brief Enable CPSW data transfers
 *
 *  Enables the CPSW for data transfer.  Empty CPSW packets are retrieved
 *  from the CPSW driver and filled with valid Eth driver buffers (reception
 *  packets are resubmitted to the driver for future frame reception).
 *  The DMA channels and the host port are enabled.  Frame transfers can
 *  occur at this point.
 *
 *  \param ctrlIdx  Eth control idx
 *
 *  \return         E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType Eth_enableCpsw(uint8 ctrlIdx);

/**
 *  \brief Disable CPSW data transfers
 *
 *  Disables the CPSW from further data transfers.  The host port and DMA
 *  channels are disabled, packets are reclaimed from the CPSW driver.
 *
 *  \param ctrlIdx  Controller index
 *
 *  \return         E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType Eth_disableCpsw(uint8 ctrlIdx);

/**
 *  \brief Process a received packet for a specific ingress Fifo
 *
 *
 *  This function also calls EthIf_RxIndication() on the received packet,
 *  and the packet is ultimately moved to the driver's rxFreeQ.
 *
 *  Only one packet is processed by this function.  The return value is
 *  set accordingly if there are more packets still sitting on the driver's
 *  rxReadyQ
 *
 *  Confirmation of the received packets to the EthIf layer is not performed
 *  by this function.
 *
 *  \param ctrlIdx  Controller index
 *  \param fifoIdx  Fifo index
 *
 *  \return         ETH_RECEIVED if a frame has been received, no further
 *                  frames are available
 *                  ETH_NOT_RECEIVED if no frame has been received, no further
 *                  frames are available
 *                  ETH_RECEIVED_MORE_DATA_AVAILABLE if a frame has been
 *                  received, more frames are available
 */
Eth_RxStatusType Eth_receiveProcess(uint8 ctrlIdx, uint8 fifoIdx);

/**
 *  \brief This function provides access to a transmit buffer of the specified
 *         controller.
 *
 *  \param (in)   : CtrlIdx. Index of the controller within the context of the
 *                               Ethernet Driver
 *  \param (in)   : Priority. Frame priority for transmit buffer FIFO selection
 *  \param (inout): LenBytePtr
 *                        In: desired length in bytes, out: granted length in bytes
 *  \param (out)  : BufIdxPtr. Index to the granted buffer resource. To be used
 *                                 for subsequent requests
 *                      BufPtr. Pointer to the granted buffer
 *  \return value      : BufReq_ReturnType
 *                        BUFREQ_OK: success
 *                        BUFREQ_E_NOT_OK: development error detected
 *                        BUFREQ_E_BUSY: all buffers in use
 *                        BUFREQ_E_OVFL: requested buffer too large
 */
BufReq_ReturnType Eth_provideTxBuffer(uint8 CtrlIdx,
        uint8 Priority,
        Eth_BufIdxType *BufIdxPtr,
        uint8 **BufPtr,
        uint16 *LenBytePtr);

/**
 *  \brief This function triggers transmission of a previously filled transmit
 *         buffer.
 *
 *  \param  (in)   : txParamPtr. Transmit parameter pointer
 *  \param  (inout): None
 *  \param  (out)  : None
 *  \return value      : Std_ReturnType
 *                        E_OK: success
 *                        E_NOT_OK: transmission failed
 */
Std_ReturnType Eth_transmit(Eth_TxParamsGroupType *txParamsGroupPtr);

/**
 *  \brief This function proccess transmission done for a specific fifo index
 *         of a previsously transmited buffer.
 *
 *  \param  (in)   : CtrlIdx. Index of the controller within the context of the
 *                   Ethernet Driver
 *  \param  (in)   : fifoIdx. Index of Egress fifo
 *  \param  (out)  : None
 *  \return value  : Eth_TxStatusType
 */
Eth_TxStatusType Eth_txProcess(uint8 ctrlIdx,
    uint8 fifoIdx);

/**
 *  \brief This function returns a time value out of the HW registers.
 *
 *  \param  (in)   : CtrlIdx. Index of the controller within the context of the
 *                            Ethernet Driver
 *  \param  (inout): None
 *  \param  (out)  : timeQualPtr. Quality of HW time stamp, e.g. based on current
 *                                drift
 *  \return value      : Std_ReturnType
 *                        E_OK: success
 *                        E_NOT_OK: failed
 */
Std_ReturnType Eth_getCurrentTime(uint8 CtrlIdx,
    Eth_TimeStampQualType *timeQualPtr,
    Eth_TimeStampType *timeStampPtr);

/**
 *  \brief This function reads back the ingress time stamp on a dedicated
 *         message object.
 *
 *  \param  (in)   : CtrlIdx. Index of the controller within the context of the
 *                            Ethernet Driver
 *                   DataPtr. Pointer to the message buffer, where Application
 *                            expects ingress time stamping
 *  \param  (inout): timeQualPtr. Quality of HW time stamp, e.g. based on current
 *                                drift
 *                   timeStampPtr. Current time stamp
 *  \param  (out)  : None
 *  \return value  : None
 */
void Eth_getIngressTimeStamp(uint8 CtrlIdx,
    Eth_DataType *DataPtr,
    Eth_TimeStampQualType *timeQualPtr,
    Eth_TimeStampType *timeStampPtr);

/**
 *  \brief This function activates egress time stamping on a dedicated message
 *         object.
 *
 *  \param  (in)   : CtrlIdx. Index of the controller within the context of the
 *                               Ethernet Driver
 *                      BufIdx. Index of the message buffer, where Application
 *                              expects egress time stamping
 *  \param  (inout): None
 *  \param  (out)  : None
 *  \return value      : None
 */
void Eth_enableEgressTimeStamp(uint8 CtrlIdx, Eth_BufIdxType BufIdx);

/**
 *  \brief This function reads back the egress time stamp on a dedicated message
 *         object.
 *
 *  \param  (in)   : CtrlIdx. Index of the controller within the context of the
 *                               Ethernet Driver
 *                      BufIdx. Index of the message buffer, where Application
 *                              expects egress time stamping
 *  \param  (inout): timeQualPtr. Quality of HW time stamp, e.g. based on current
 *                                   drift
 *                      timeStampPtr. Current time stamp
 *  \param  (out)  : None
 *  \return value      : None
 */
void Eth_getEgressTimeStamp(uint8 CtrlIdx,
    Eth_BufIdxType BufIdx,
    Eth_TimeStampQualType *timeQualPtr,
    Eth_TimeStampType *timeStampPtr);

#if (STD_ON == ETH_TRAFFIC_SHAPING_API)
/**
 *  \brief This function set bandwidth for a specific transmitsion queue
 *
 *  \param (in)   : CtrlIdx. Index of the controller within the context of the
 *                               Ethernet Driver
 *  \param (in)   : QueuePrio. Specifies the related fifo
 *                      BandwidthLimit. Bandwidth value
 *  \param (inout): None
 *  \param (out)  : None
 *  \return value      : Std_ReturnType
 *                        E_OK: success
 *                        E_NOT_OK: configure bandwidth failed
 */
Std_ReturnType Eth_setBandwidthLimit(uint8 CtrlIdx, uint8 QueuePrio, uint32  BandwidthLimit);

/**
 *  \brief This function get bandwidth for a specific transmitsion queue
 *
 *  \param (in)   : CtrlIdx. Index of the controller within the context of the
 *                               Ethernet Driver
 *  \param (in)   : QueuePrio. Specifies the related fifo
 *  \param (inout): BandwidthLimitPtr Pointer to where to store the currently configured bandwidth limit (in [bit/s])

 *  \return value      : Std_ReturnType
 *                        E_OK: success
 *                        E_NOT_OK: retrive bandwidth failed
 */
Std_ReturnType Eth_getBandwidthLimit(uint8 CtrlIdx, uint8 QueuePrio, uint32* BandwidthLimitPtr);
#endif /* (STD_ON == ETH_TRAFFIC_SHAPING_API) */

#if (STD_ON == ETH_ZERO_COPY_API)
/**
 *  \brief This function releases an RX buffer and prepares the buffer to be reused during reception
 *
 *  \param (in)   : CtrlIdx  Controller index
 *  \param (in)   : bufPtr   Pointer to the reception buffer.

 *  \return value      : E_NOT_OK : Buffer could not be released. This may only happen with a
 *                                  wrong BufPtr.
 *                       E_OK : Buffer successfully released
 */
Std_ReturnType Eth_releaseRxBuffer(uint8 ctrlIdx,
    Eth_DataType *bufPtr);

/**
 *  \brief This function provides an external buffer for frame transmission.
 *          The buffer is supposed to be locked until transmission
 *          confirmation is called.
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
 */
BufReq_ReturnType Eth_provideExtTxBuffer(uint8 CtrlIdx,
    uint8 FifoIdx,
    Eth_BufIdxType *BufIdxPtr,
    Eth_DataType **BufPtr,
    uint16 *LenBytePtr);
#endif /* (STD_ON == ETH_ZERO_COPY_API) */

#if (STD_ON == ETH_HEADER_ACCESS_API)
/**
 *  \brief This function returns the Ethernet header portion of a transmission frame.
 *          This allows the user to overwrite the Ethernet header according to his needs.
 *          Otherwise the header is written by the driver. Call this function after a call to
 *          Eth_ProvideTxBuffer or Eth_ProvideExtTxBuffer.
 *
 *  \param (in)   : CtrlIdx. Controller index
 *  \param (in)   : BufIdx. Buffer index
 *  \param (out)  : BufPtr. Pointer to buffer area that holds the Ethernet transmission header
 *  \param (inout): LenBytePtr The Ethernet header length. This is always 14 bytes.
 *  \return value      : E_OK : Success
 *                       E_NOT_OK : Either the buffer associated with BufIdx is in a wrong state.
 */
Std_ReturnType Eth_getTxHeaderPtr(uint8 CtrlIdx,
    uint8 BufIdx,
    Eth_DataType **BufPtr,
    uint16 *LenBytePtr);

/**
 *  \brief This function returns the pointer to the first octet of a received Ethernet frame.
 *          That allows access to the Ethernet header as well as the Ethernet payload.
 *          Call this function after the EthIf_RxIndication function is called.
 *
 *  \param (in)   : CtrlIdx. Controller index
 *  \param (out)  : BufPtr. Pointer to the payload portion of the received Ethernet frame.
 *                          This pointer is passed to the EthIf_RxIndication callback function
 *  \param (inout): LenBytePtr The Ethernet header length. This is always 14 bytes.
 *  \return value      : E_OK : Success
 *                       E_NOT_OK : Either the buffer associated with BufIdx is in a wrong state.
 */
Std_ReturnType Eth_getRxHeaderPtr(uint8 CtrlIdx,
    Eth_DataType **BufPtr,
    uint16 *LenBytePtr);
#endif /* (STD_ON == ETH_HEADER_ACCESS_API) */

/**
 *  \brief Check controller access
 *
 *  Checks if the Ethernet controller hardware is accessible and calls appropriate
 *  Diagnostic Event Manager(DEM) events.
 *
 * Fail - When access to the Ethernet Controller fails extended production error with event
 *        status DEM_EVENT_STATUS_PREFAILED is returned to  DEM.
 * Pass - When access to the Ethernet Controller succeeds extended production error with event
 *        status DEM_EVENT_STATUS_PREPASSED is returned to  DEM.
 *
 *  \param ctrlIdx  eth controller index
 *
 *  \return         E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType Eth_checkControllerAccess(uint8 ctrlIdx);

/**
 *  \brief Check controller errors
 *
 *  Checks for the following controller errors from the statistics
 *  reported by CPSW:
 *  - Receive packets with CRC errors
 *  - Receive packets with RX overrun
 *  - Receive frames lost
 *  - Receive undersized packets
 *  - Receive oversized packets
 *  - Receive packets with alignment errors
 *  - Transmit packets that experienced a single collision
 *  - Transmit packets that experienced multiple collisions
 *  - Transmit packets that experienced a late collision
 *
 */
void Eth_checkControllerErrors(uint8 ctrlIdx);

/**
 *  \brief Get Ethernet RX statistics as per RFC2819
 *
 *  Get the Ethernet RX statistics as per RFC2819. Refer to the
 *  \ref Eth_RxStatsType.
 *
 *  \param RxStats  Rx Statistics structure pointer.
 */
void Eth_getRxStatsRFC2819(uint8 ctrlIdx, Eth_RxStatsType *RxStats);

/**
 *  \brief Get Ethernet TX statistics as per RFC1213
 *
 *  Get the Ethernet TX statistics as per RFC1213. Refer to the
 *  \ref Eth_TxStatsType.
 *
 *  \param TxStats  Tx Statistics structure pointer.
 */
void Eth_getTxStatsRFC1213(uint8 ctrlIdx, Eth_TxStatsType *TxStats);

/**
 *  \brief Get Ethernet TX Error statistics
 *
 *  Get the Ethernet TX Error statistics as per IETF RFC1213 and RFC1643. Refer to the
 *  \ref Eth_TxStatsType.
 *
 *  \param TxErrorCounterValues  Tx Error Statistics structure pointer.
 */
void Eth_getTxErrorCounterValues(uint8 ctrlIdx, Eth_TxErrorCounterValuesType *TxErrorCounterValues);
/**
 *  \brief Get counter statistics
 *
 *  Get the frame count statistics.  The Count filled by this
 *  function includes hardware dependent counts. Refer to the
 *  \ref Eth_CounterType
 *
 *  \param CounterPtr Pointer to Eth_CounterType structure array pointer.
 */
void Eth_getCounterValues(uint8 ctrlIdx, Eth_CounterType* CounterPtr);

#if (STD_ON == ETH_GET_COUNTER_STATE_API)
/**
 *  \brief Get counter statistics with the given offset
 *
 *  Get the frame count statistics with the given offset.
 *
 *  \param drv       Eth driver handle
 *  \param CtrOffs   Memory offset of the counter based on CpswStats_Stats
 *                   memory offset
 *  \param CtrValPtr Pointer to the Counter Value output.
 *
 *  \return         E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType Eth_getCounterState(uint8 ctrlIdx, uint16 CtrOffs, uint32 *CtrValPtr);
#endif /* #if (STD_ON == ETH_GET_COUNTER_STATE_API) */

/**
 *  \brief Check if a MAC address is a broadcast address
 *
 *  Checks if a MAC address is a broadcast address (FF:FF:FF:FF:FF:FF).
 *
 *  \param addr     MAC address
 *
 *  \return         TRUE if it's a broadcast address, FALSE otherwise
 */
boolean Eth_isBcastMacAddr(const uint8 *addr);

/**
 *  \brief Check if a MAC address is a multicast address
 *
 *  Checks if a MAC address is a multicast address.
 *
 *  \param addr     MAC address
 *
 *  \return         TRUE if it's a multicast address, FALSE otherwise
 */
boolean Eth_isMcastMacAddr(const uint8 *addr);

/**
 *  \brief Check if a MAC address is a null address
 *
 *  Checks if a MAC address is a null address (00:00:00:00:00:00).
 *
 *  \param addr     MAC address
 *
 *  \return         TRUE if it's a null address, FALSE otherwise
 */
boolean Eth_isNullMacAddr(const uint8 *addr);

/**
 *  \brief Set the MAC address to a CPSW MAC port
 *
 *  Sets the MAC address to the CPSW MAC port corresponding to the
 *  controller index.
 *
 *  \param addr     MAC address
 *
 *  \return         E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType Eth_setMacAddr(uint8 ctrlIdx,
                              const uint8 *addr);

/**
 *  \brief Enable promiscuous mode
 *
 *  Enable the promiscuous mode by clearing the ALE table and setting
 *  the ALE in bypass mode.  While in bypass mode, all packets received
 *  in all ports are exclusively forwarded to the host port.
 *
 *  \param ctrlIdx  Controller index
 *
 *  \return         E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType Eth_setPromiscuousMode(uint8 ctrlIdx);

/**
 *  \brief Disable promiscuous mode
 *
 *  Disable the promiscuous mode by setting ALE in normal, non-bypass
 *  mode and adding a unicast rule for the controller's MAC address
 *  and a multicast rule for the broadcast address.
 *
 *  \param ctrlIdx  Controller index
 *  \param addr     Controller's MAC address
 *
 *  \return         E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType Eth_setNonPromiscuousMode(uint8 ctrlIdx, uint8 *addr);

/**
 *  \brief Replace unicast address
 *
 *  Replaces the given unicast address with a new unicast address.
 *  The ALE entry for the old address is removed and a new entry
 *  is added with the new address.
 *
 *  \param oldAddr  Old unicast address
 *  \param newAddr  New unicast address
 *
 *  \return         E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType Eth_replaceUcastAddr(uint8 ctrlIdx,
                                    const uint8 *oldAddr,
                                    const uint8 *newAddr);


/**
 *  \brief Do filter action for a MAC address to ALE
 *
 *  Do filter action for a  ALE entry for the provided MAC address.  The mac entry
 *  can be a unicast or multicast rule, depending on the type of MAC
 *  address being passed.
 *
 *  \param addr     MAC address
  * \param Action     Add/Delete mac address
 *
 *  \return         E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType Eth_doFilterAddr(uint8 ctrlIdx,
                                 const uint8 *addr, Eth_FilterActionType Action);

#if (STD_ON == ETH_ENABLE_MII_API)
/**
 *  \brief Trigger a PHY register read synchronous
 *
 *  Initiates the PHY register read operation.  The operation is completed
 *  when the MDIO interrupt occurs.
 *
 *  The MDIO interrupt handler is responsible for passing the read value to
 *  to the EthTrcv driver through the EthTrcv_ReadMiiIndication() call.
 *
 *  \param ctrlIdx  Controller index
 *  \param trcvIdx  Transceiver index (PHY address)
 *  \param regIdx   Register address
  * \param regVal   Read value
 *
 *  \return         E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType Eth_triggerMiiReadSync(uint8 ctrlIdx,
                                  uint8 trcvIdx,
                                  uint8 regIdx,
                                  uint16 *regVal);

/**
 *  \brief Trigger a PHY register read asynchronous
 *
 *  Initiates the PHY register read operation.  The operation is completed
 *  when the MDIO interrupt occurs.
 *
 *  The MDIO interrupt handler is responsible for passing the read value to
 *  to the EthTrcv driver through the EthTrcv_ReadMiiIndication() call.
 *
 *  \param ctrlIdx  Controller index
 *  \param trcvIdx  Transceiver index (PHY address)
 *  \param regIdx   Register address
 *  \param regVal   Read value
 *
 *  \return         E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType Eth_triggerMiiReadAsync(uint8 ctrlIdx,
                                  uint8 trcvIdx,
                                  uint8 regIdx,
                                  uint16 *regVal);
/**
 *  \brief Trigger a PHY register write synchronous
 *
 *  Initiates the PHY register write operation.  The operation is completed
 *  when the MDIO interrupt occurs.
 *
 *  The MDIO interrupt handler is responsible for indicating the completion
 *  of the register write to the EthTrcv driver through the
 *  EthTrcv_WriteMiiIndication() call.
 *
 *  \param ctrlIdx  Controller index
 *  \param trcvIdx  Transceiver index (PHY address)
 *  \param regIdx   Register address
 *  \param regVal   Register value to be written
 *
 *  \return         E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType Eth_triggerMiiWriteSync(uint8 ctrlIdx,
                                   uint8 trcvIdx,
                                   uint8 regIdx,
                                   uint16 regVal);

/**
 *  \brief Trigger a PHY register write asynchronous
 *
 *  Initiates the PHY register write operation.  The operation is completed
 *  when the MDIO interrupt occurs.
 *
 *  The MDIO interrupt handler is responsible for indicating the completion
 *  of the register write to the EthTrcv driver through the
 *  EthTrcv_WriteMiiIndication() call.
 *
 *  \param ctrlIdx  Controller index
 *  \param trcvIdx  Transceiver index (PHY address)
 *  \param regIdx   Register address
 *  \param regVal   Register value to be written
 *
 *  \return         E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType Eth_triggerMiiWriteAsync(uint8 ctrlIdx,
                                   uint8 trcvIdx,
                                   uint8 regIdx,
                                   uint16 regVal);
/**
 *  \brief Get the value read from a PHY register
 *
 *  Gets the value read from a PHY register after the MDIO interrupt has
 *  occurred indicating the completion of the read operation.
 *
 *  \param ctrlIdx  Controller index
 *  \param regVal   Register value read
 *
 *  \return         E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType Eth_getMiiReadVal(uint8 ctrlIdx,
                                 uint16 *regVal);

/**
 *  \brief Clear MDIO interrupt
 *
 *  Clears the MDIO interrupt.
 *
 *  \param ctrlIdx  Controller index
 *
 *  \return         E_OK if successful, E_NOT_OK otherwise
 */
void Eth_clearMdioInt(uint8 ctrlIdx);
#endif /* (STD_ON == ETH_ENABLE_MII_API) */

/**
 *  \brief Read the PHY MAC address
 *
 *  Gets the PHY MAC address from the MMR registers MAC_ID0 and MAC_ID1.
 *
 *  \param addr     MAC address pointer
 */
void Eth_readMacAddr(uint8 ctrlIdx, uint8 *addr);

/**
 *  \brief Get fifo index and fifo buffer index from input bufferIdx
 *
 *  \param bufferIdx     input buffer index
 *  \param FifoIdx       pointer to fifo index
 *  \param fifoBuffIdx   pointer to fifo buffer index
 *
 *  \return none
 */
void Eth_getFifoFromBufferIdx(Eth_BufIdxType bufferIdx, uint8 *FifoIdx, uint16 *fifoBuffIdx);


#if (STD_ON == ETH_REGISTER_READBACK_API)
/**
 * \brief Get value of configuration value
 *
 *  \param ctrlIdx    Eth Controller to readback registers configuration.
 *  \param regRbPtr   Pointer to where to store the readback values.
 *
 *  \return none
 */

void Eth_registerReadback(uint8 ctrlIdx,
                          Eth_RegisterReadbackType *regRbPtr);
#endif  /* (STD_ON == ETH_REGISTER_READBACK_API) */

/**
 *  \brief Report DET error
 *
 *  Reports an Ethernet module error to the Development Error Tracer (DET).
 *
 *  \param apiId    API Id \ref Eth_ServiceIds
 *  \param errorId  Error Id \ref Eth_ErrorCodes
 */
#if (STD_ON == ETH_DEV_ERROR_DETECT)
LOCAL_INLINE void Eth_reportDetError(uint8 apiId, uint8 errorId)
{
    (void) Det_ReportError(ETH_MODULE_ID, ETH_INSTANCE_ID, apiId, errorId);
}
#endif

/* ========================================================================== */
/*                        Deprecated Function Declarations                    */
/* ========================================================================== */

/* None */

#ifdef __cplusplus
}
#endif

#endif /* ETH_PRIV_H_ */
