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
 *  \file     Eth_Types.h
 *
 *  \brief    This file contains Eth type declarations.
 */

/**
 *  \ingroup MCAL_ETH_API
 *  @{
 */

#ifndef ETH_TYPES_H_
#define ETH_TYPES_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "Eth_GeneralTypes.h"
#include "Eth_Cfg.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/** \brief MAC address length in bytes */
#define ETH_MAC_ADDR_LEN                ((uint32) 6U)

/** \brief Total octets in header */
#define ETH_HEADER_LEN                  ((uint16) 14U)

/** \brief Total prioroty queue number */
#define ETH_PRIORITY_QUEUE_NUM          (8U)

/** \brief Invalid stats value (stat not available) */
#define ETH_STATCNT_INVALID             (0xFFFFFFFFU)

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/**
 *  \brief Ethernet buffer state
 *
 *  The state of the Ethernet buffer.
 */
typedef enum
{
    ETH_BUF_STATE_FREE   = 0U,
    /**< Buffer is free for allocation */
    ETH_BUF_STATE_IN_USE = 1U,
    /**< Buffer is in use */
    ETH_BUF_STATE_QUEUED = 2U,
    /**< Buffer has been queued for transmit */
    ETH_BUF_STATE_CONFIRMATION = 3U,
    /**< Buffer has mark that need to confirmation for upper layer */
} Eth_BufStateType;

/**
 *  \brief Ethernet transmit proccess status
 */
typedef enum
{
    ETH_TRANSMITED_BUFFER_DONE = 0U,
    /**< Eth transmited buffer has done */
    ETH_TRANSMITED_BUFFER_PENDING = 1U
    /**< Eth transmited buffer are pending  */
} Eth_TxStatusType;

/**
 *  \brief MII Access State
 *
 *  State of the current MII read/write operation.
 */
typedef struct
{
    boolean isActive;
    /**< MII operation is in progress */
    boolean isRead;
    /**< Read or write operation */
    uint8 ctrlIdx;
    /**< Ethernet controller index */
    uint8 trcvIdx;
    /**< Ethernet transceiver index (PHY address) */
    uint8 regIdx;
    /**< PHY register address */
} Eth_MiiAccessType;

/**
 *  \brief Controller errors
 *
 *  Counters of different types of controller errors.
 */
typedef struct
{
    uint32 rxCRC;
    /**< RX packets with CRC errors */
    uint32 rxOverrun;
    /**< Receive overruns */
    uint32 rxUndersized;
    /**< RX packets that were undersized */
    uint32 rxOversized;
    /**< RX packets that were oversized */
    uint32 rxAlignCode;
    /**< RX packets with align/code errors */
    uint32 txSingleColl;
    /**< TX packets that experienced a single collision */
    uint32 txMultiColl;
    /**< TX packets that experienced multiple colisions */
    uint32 txLateColl;
    /**< TX packets that experienced a late collision */
} Eth_CtrlErrorsType;

/**
 *  \brief Cache write-back invalidate function
 *
 *  Pointer to a function that performs the cache write-back invalidate
 *  operation.  This function is to be called on TX buffers before they
 *  are given to the Ethernet controller hardware.
 */
typedef void (*Eth_CacheWbInv)(uint8 *BufPtr,
                               uint16 LenByte);

/**
 *  \brief Cache write-back function
 *
 *  Pointer to a function that performs the cache write-back operation.
 *  This function is to be called on TX buffers before they are given to
 *  the Ethernet controller hardware.
 */
typedef void (*Eth_CacheWb)(uint8 *BufPtr,
                            uint16 LenByte);

/**
 *  \brief Cache invalidate function
 *
 *  Pointer to a function that performs the cache invalidate operation.
 *  This function is to be called on RX buffers after they have been
 *  retrieved from the Ethernet controller hardware.
 */
typedef void (*Eth_CacheInv)(uint8 *BufPtr,
                             uint16 LenByte);

/** Number of CPSW Type supported */
#define ETH_PORT_COUNT      (((uint8)ETH_PORT_MAC_PORT_LAST) + 1U)

/**
 *  \brief Data Structure listing list of ports.
 *
 *  Variables of this type are used identify port membership for things like
 *  multicast address membership
 */
typedef struct Eth_PortListType_s
{
    uint32  numPorts;
    /**< Number of MAC ports in the port list */
    Eth_PortType ports[ETH_PORT_COUNT];
    /**< MAC ports included in the port list */
} Eth_PortListType;

/**
 *  \brief Data Structure of parameter group for transmit function
 */
typedef struct Eth_TxParamsGroupType_s
{
    uint8 ctrlIdx;
    /**< Controller index */
    Eth_BufIdxType bufIdx;
    /**< Transmit buffer index */
    Eth_FrameType frameType;
    /**< Transmit Eth type */
    boolean txConfirmation;
    /**< Enable/disable tx confirmation */
    uint16 lenByte;
    /**< Length of transmit packet */
    P2CONST(uint8, AUTOMATIC, ETH_APPL_DAT) physAddrPtr;
    /**< Destination mac address */
} Eth_TxParamsGroupType;

#if (STD_ON == ETH_REGISTER_READBACK_API)
/**
 * \brief  Register Readback Structure
 *
 * Structure to read back key Eth control registers.
 *
 * The Safety API to readback Eth register vales will populate this structure
 */
typedef struct
{
    uint32 EthReadBackReg_ALE_CONTROL;
    /**< ALE_CONTROL Register  */
    uint32 EthReadBackReg_I0_ALE_PORTCTL0;
    /**< I0_ALE_PORTCTL0 Register  */
    uint32 EthReadBackReg_I0_ALE_PORTCTL0_PORT_0;
    /**< PORT 0 I0_ALE_PORTCTL0 Register  */
    uint32 EthReadBackReg_I0_ALE_PORTCTL0_PORT_N;
    /**< PORT N I0_ALE_PORTCTL0 Register  */
    uint32 EthReadBackReg_THREADMAPDEF;
    /**< ALE control Register  */
    uint32 EthReadBackReg_ALE_UVLAN_MEMBER;
    /**< ALE_UVLAN_MEMBER Register  */
    uint32 EthReadBackReg_ALE_UVLAN_URCAST;
    /**< ALE_UVLAN_URCAST Register  */
    uint32 EthReadBackReg_ALE_UVLAN_RMCAST;
    /**< ALE_UVLAN_URCAST Register  */
    uint32 EthReadBackReg_ALE_UVLAN_UNTAG;
    /**< ALE_UVLAN_UNTAG Register  */
    uint32 EthReadBackReg_CPTS_CONTROL;
    /**< CPTS_CONTROL Register  */
    uint32 EthReadBackReg_PN_MAC_CONTROL_REG;
    /**< PN_MAC_CONTROL_REG Register  */
    uint32 EthReadBackReg_PN_RX_MAXLEN_REG;
    /**< PN_RX_MAXLEN_REG Register  */
    uint32 EthReadBackReg_USER_INT_MASK_CLEAR_REG;
    /**< USER_INT_MASK_CLEAR_REG Register  */
    uint32 EthReadBackReg_USER_GROUP_USER_ACCESS_REG;
    /**< USER_GROUP_USER_ACCESS_REG Register  */
    uint32 EthReadBackReg_POLL_EN_REG;
    /**< POLL_EN_REG Register  */
    uint32 EthReadBackReg_MDIO_CONTROL_REG;
    /**< MDIO_CONTROL_REG Register  */
    uint32 EthReadBackReg_PORT_CONTROL_REG;
    /**< PORT_CONTROL_REG Register  */
    uint32 EthReadBackReg_P0_CONTROL_REG;
    /**< P0_CONTROL_REG Register  */
    uint32 EthReadBackReg_PN_TS_CTL_REG;
    /**< PN_TS_CTL_REG Register  */
    uint32 EthReadBackReg_PN_TS_SEQ_LTYPE_REG;
    /**< PN_TS_SEQ_LTYPE_REG Register  */
    uint32 EthReadBackReg_PN_TS_VLAN_LTYPE_REG;
    /**< PN_TS_VLAN_LTYPE_REG Register  */
    uint32 EthReadBackReg_PN_TS_CTL_LTYPE2_REG;
    /**< PN_TS_CTL_LTYPE2_REG Register  */
    uint32 EthReadBackReg_PN_TS_CTL2_REG;
    /**< PN_TS_CTL2_REG Register  */
    uint32 EthReadBackReg_PN_PRI_CIR_REG[ETH_PRIORITY_QUEUE_NUM];
    /**< PN_PRI_CIR_REG Register  */
    uint32 EthReadBackReg_STAT_PORT_EN_REG;
    /**< STAT_PORT_EN_REG Register  */
} Eth_RegisterReadbackType;
#endif /* (STD_ON == ETH_REGISTER_READBACK_API) */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */

#ifdef __cplusplus
}
#endif

#endif  /* ETH_TYPES_H_ */

/*! @} */
