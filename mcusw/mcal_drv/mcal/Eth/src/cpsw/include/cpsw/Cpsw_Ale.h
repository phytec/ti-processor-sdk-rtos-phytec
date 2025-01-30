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
 *  \file     Cpsw_Ale.h
 *
 *  \brief    This file contains the structure definitions and function
 *            prototypes of the Address Lookup Engine (ALE).
 */

/**
 *  \addtogroup ETH_CPSW_ALE_API
 *  @{
 */

#ifndef CPSWMCAL_ALE_H_
#define CPSWMCAL_ALE_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "csl_cpsw.h"
#include "Cpsw_Types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                                 Macros                                     */
/* ========================================================================== */

/*!
 * \brief Normalize #Enet_MacPort.
 *
 * Macro to normalize #Enet_MacPort. It takes an #Enet_MacPort enum and converts
 * it to a zero-based index.
 */
#define ENET_MACPORT_NORM(n)                  (((uint32)(n)) - ((uint32)ENET_MAC_PORT_FIRST))
/*!
 * \brief De-normalize #Enet_MacPort.
 *
 * Macro to denormalize MAC port number. It takes a zero-based port number and
 * converts it to a #Enet_MacPort enum.
 */
#define ENET_MACPORT_DENORM(n)                (((uint32)(n)) + ((uint32)ENET_MAC_PORT_FIRST))


/** \brief Macro to convert from a port number to a bitmask */
#define ALE_PORT_MASK_BIT(n)                  ((((uint32)1U) << ((uint32)(n))))

/** \brief  ALE table size */
#define ETH_CPSW_ALE_TABLE_SIZE               (64U)

/**
 *  \anchor ALE_ConfigFlags
 *  \name ALE configuration flags
 *
 *  Configuration flags that can be passed to the ALE module when the module
 *  is being opened.
 *
 *  @{
 */

/*! \brief Macro to get the size of an array. */
#define ENET_ARRAYSIZE(x)                     (sizeof(x) / sizeof(x[0]))

/*! \brief Macro to set bit at given bit position. */
#define ENET_BIT(n)                           (((uint32)1U) << (n))

/*! \brief Macro to get bit at given bit position. */
#define ENET_GET_BIT(val, n)                  (((val) & ENET_BIT(n)) >> (n))

/*! \brief Macro to check if bit at given bit position is set. */
#define ENET_IS_BIT_SET(val, n)               (((val) & ENET_BIT(n)) != 0U)

/*! \brief Macro to check if value is not zero. */
#define ENET_NOT_ZERO(val)                    ((uint32)0U != (uint32)(val))

/** \brief Field EXTract macro. We can't use CSL macro as it appends shift/mask with CSL_*/
#define ENET_FEXT(reg, PER_REG_FIELD)                                        \
    (((reg) & PER_REG_FIELD##_MASK) >> PER_REG_FIELD##_SHIFT)

/** \brief Field INSert macro. We can't use CSL macro as it appends shift/mask with CSL_*/
#define ENET_FINS(reg, PER_REG_FIELD, val)                                   \
    ((reg) = ((reg) & ~PER_REG_FIELD##_MASK)                                 \
      | (((val) << PER_REG_FIELD##_SHIFT) & PER_REG_FIELD##_MASK))

/*! \brief Macro to perform round-up division. */
#define ENET_DIV_ROUNDUP(val, div)            (((val) + (div) - 1) / (div))

/*! \brief Version field is not supported. */
#define ENET_VERSION_NONE                     (0xFFFFFFFFU)

/*! \brief MAC address length in bytes/octets. */
#define ENET_MAC_ADDR_LEN                     (6U)

/*! \brief Organization Unique Id (OUI) address length in bytes/octets. */
#define ENET_OUI_ADDR_LEN                     (3U)

/*! \brief IPv4 address length in bytes/octets. */
#define ENET_IPv4_ADDR_LEN                    (4U)

/*! \brief IPv6 address length in bytes/octets. */
#define ENET_IPv6_ADDR_LEN                    (16U)

/*! \brief Packet priority. */
#define ENET_PRI_NUM                          (8U)

/*! \brief Lowest packet priority. */
#define ENET_PRI_MIN                          (0U)

/*! \brief Lowest packet priority. */
#define ENET_PRI_MAX                          (ENET_PRI_NUM - 1U)

/*! \brief Type of Service (ToS) priority. */
#define ENET_TOS_PRI_NUM                      (64U)

/*! \brief Lowest ToS priority. */
#define ENET_TOS_PRI_MIN                      (0U)

/*! \brief Highest ToS priority. */
#define ENET_TOS_PRI_MAX                      (ENET_TOS_PRI_NUM - 1U)

/*! \brief Maximum value for VLAN ID. */
#define ETH_ENET_VLAN_ID_MAX                  (4095U)

/*! \brief EtherType value for PTP over Ethernet Annex F (IEEE 802.3). */
#define ENET_ETHERTYPE_PTP                    (0x88F7U)

/** \brief ALE enable flag */
#define ALE_FLAG_ENABLE                       (0x0001U)

/** \brief ALE bypass mode flag */
#define ALE_FLAG_BYPASS                       (0x0002U)

/** \brief ALE VLAN aware mode flag */
#define ALE_FLAG_VLANAWARE                    (0x0004U)

/*! \brief Helper macro to create IOCTL commands for ALE module. */
#define ETH_CPSW_ALE_PUBLIC_IOCTL(x)          (((uint32)(ETH_ENET_IOCTL_TYPE_PUBLIC)) | \
                                               ((uint32)(ETH_ENET_IOCTL_FDB_BASE)) |    \
                                               ((uint32)(ETH_ENET_IOCTL_PER_CPSW)) |    \
                                               ((uint32)(ETH_ENET_IOCTL_MIN(x))))

/*! \brief Number of switch ports in the subsystem. */
#define ETH_CPSW_ALE_NUM_PORTS                    (9U)

/*! \brief Number of external ports in the subsystem. */
#define ETH_CPSW_ALE_NUM_MAC_PORTS                (ETH_CPSW_ALE_NUM_PORTS - 1U)

/*! \brief Maximum IP next header whitelist. */
#define ETH_CPSW_ALE_MAX_WHITELIST_IP_NXT_HDR     (4U)

/*! \brief Mask value for the all the ports. */
#define ETH_CPSW_ALE_ALL_PORTS_MASK               ((uint32)((1U << ETH_CPSW_ALE_NUM_PORTS) - 1U))

/*! \brief Mask value for the all MAC ports. */
#define ETH_CPSW_ALE_ALL_MACPORTS_MASK            ((uint32)(((1U << ETH_CPSW_ALE_NUM_PORTS) - 1U) - 1U))

/*! \brief Mask value for the host port. */
#define ETH_CPSW_ALE_HOST_PORT_MASK               (ENET_BIT(0))

/*! \brief Port number for the host port. */
#define ETH_CPSW_ALE_HOST_PORT_NUM                (0U)

/*! \brief Base index for MAC ports in ALE context */
#define ETH_CPSW_ALE_MACPORT_BASE                 (((uint32)1U))

/*! \brief Macro to convert MAC port (#Enet_MacPort) to ALE port number */
#define ETH_CPSW_ALE_MACPORT_TO_ALEPORT(macPortNum)      (ENET_MACPORT_NORM(macPortNum) + ETH_CPSW_ALE_MACPORT_BASE)

/*! \brief Macro to convert ALE port number to MAC port number(#Enet_MacPort) */
#define ETH_CPSW_ALE_ALEPORT_TO_MACPORT(alePortNum)      (ENET_MACPORT_DENORM(alePortNum - ETH_CPSW_ALE_MACPORT_BASE))

/*! \brief Macro to convert MAC port (#Enet_MacPort) to ALE port mask. */
#define ETH_CPSW_ALE_MACPORT_TO_PORTMASK(macPort)        (ENET_BIT(ETH_CPSW_ALE_MACPORT_TO_ALEPORT(macPort)))

/*! \brief Maximum number of ignore bits in multicast address. */
#define ETH_CPSW_ALE_MCAST_IGN_BITS_MAX           (10U)

/*! \brief Disable peak bit rate. */
#define ETH_CPSW_ALE_PEAKBITRATE_DISABLE          (0U)

/*! \brief Disable commit bit rate. */
#define ETH_CPSW_ALE_COMMITBITRATE_DISABLE        (0U)

/*! \brief ALE invalid thread id. */
#define ETH_CPSW_ALE_THREADID_INVALID             (~0U)


/*! \brief IOCTL type bit offset. */
#define ETH_ENET_IOCTL_TYPE_OFFSET                (24U)

/*! \brief IOCTL hardware peripheral bit offset. */
#define ETH_ENET_IOCTL_PER_OFFSET                 (16U)

/*! \brief IOCTL major number bit offset. */
#define ETH_ENET_IOCTL_MAJOR_OFFSET               (8U)

/*! \brief IOCTL minor number bit offset. */
#define ETH_ENET_IOCTL_MINOR_OFFSET               (0U)


/*! \brief Helper macro to get the IOCTL type (public/private). */
#define ETH_ENET_IOCTL_GET_TYPE(x)                (((uint32)(x)) & 0xFF000000U)

/*! \brief Helper macro to get the peripheral type. */
#define ETH_ENET_IOCTL_GET_PER(x)                 (((uint32)(x)) & 0x00FF0000U)

/*! \brief Helper macro to get the IOCTL major number. */
#define ETH_ENET_IOCTL_GET_MAJ(x)                 (((uint32)(x)) & 0x0000FF00U)

/*! \brief Helper macro to get the IOCTL major number. */
#define ETH_ENET_IOCTL_GET_MIN(x)                 (((uint32)(x)) & 0x000000FFU)

/*! \brief Helper macro to set the IOCTL type. */
#define ETH_ENET_IOCTL_TYPE(x)                    (((uint32)(x)) << ETH_ENET_IOCTL_TYPE_OFFSET)

/*! \brief Helper macro to set the IOCTL type. */
#define ETH_ENET_IOCTL_PER(x)                     (((uint32)(x)) << ETH_ENET_IOCTL_PER_OFFSET)

/*! \brief Helper macro to set the IOCTL major number. */
#define ETH_ENET_IOCTL_MAJ(x)                     (((uint32)(x)) << ETH_ENET_IOCTL_MAJOR_OFFSET)

/*! \brief Helper macro to set the IOCTL minor number. */
#define ETH_ENET_IOCTL_MIN(x)                     (((uint32)(x)) << ETH_ENET_IOCTL_MINOR_OFFSET)

/*! @} */

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/*!
 * \brief IOCTL types.
 */
enum Eth_Enet_IoctlType_e
{
    /*! Public IOCTL type (default type if omitted) */
    ETH_ENET_IOCTL_TYPE_PUBLIC  = ETH_ENET_IOCTL_TYPE(1U),

    /*! Private IOCTL type */
    ETH_ENET_IOCTL_TYPE_PRIVATE = ETH_ENET_IOCTL_TYPE(2U),
};

/*!
 * \brief IOCTL peripheral types.
 */
enum Eth_Enet_IoctlPer_e
{
    /*! Generic IOCTLs */
    ETH_ENET_IOCTL_PER_GENERIC = ETH_ENET_IOCTL_PER(0U),

    /*! CPSW specific IOCTL (agnostic of number of CPSW ports) */
    ETH_ENET_IOCTL_PER_CPSW = ETH_ENET_IOCTL_PER(1U),

    /*! ICSSG specific IOCTL */
    ETH_ENET_IOCTL_PER_ICSSG = ETH_ENET_IOCTL_PER(2U),

    /*! GMAC specific IOCTL */
    ETH_ENET_IOCTL_PER_GMAC = ETH_ENET_IOCTL_PER(3U),
};

/*!
 * \brief IOCTL base number
 */
enum Eth_Enet_IoctlMajor_e
{
    /*! Enet peripheral IOCTL base */
    ETH_ENET_IOCTL_PER_BASE      = ETH_ENET_IOCTL_MAJ(1U),

    /*! FDB module IOCTL base */
    ETH_ENET_IOCTL_FDB_BASE      = ETH_ENET_IOCTL_MAJ(2U),

    /*! TimeSync module IOCTL base */
    ETH_ENET_IOCTL_TIMESYNC_BASE = ETH_ENET_IOCTL_MAJ(3U),

    /*! Host port module IOCTL base */
    ETH_ENET_IOCTL_HOSTPORT_BASE = ETH_ENET_IOCTL_MAJ(4U),

    /*! MAC port module IOCTL base */
    ETH_ENET_IOCTL_MACPORT_BASE  = ETH_ENET_IOCTL_MAJ(5U),

    /*! MDIO module IOCTL base */
    ETH_ENET_IOCTL_MDIO_BASE     = ETH_ENET_IOCTL_MAJ(6U),

    /*! Statistics module IOCTL base */
    ETH_ENET_IOCTL_STATS_BASE    = ETH_ENET_IOCTL_MAJ(7U),

    /*! PHY module IOCTL base */
    ETH_ENET_IOCTL_PHY_BASE      = ETH_ENET_IOCTL_MAJ(9U),

    /*! Resource Manager module IOCTL base */
    ETH_ENET_IOCTL_RM_BASE       = ETH_ENET_IOCTL_MAJ(10U),

    /*! Resource Manager module IOCTL base */
    ETH_ENET_IOCTL_TAS_BASE       = ETH_ENET_IOCTL_MAJ(11U),
};

/*!
 * \brief ALE IOCTL commands.
 */
typedef enum Eth_CpswAle_Ioctl_e
{
    /*!
     * \brief Dump ALE table entries.
     *
     * Print current entries in the ALE table.
     *
     * IOCTL params:
     * -  inArgs: None
     * - outArgs: None
     *
     * Calling context: Task
     */
    ETH_CPSW_ALE_IOCTL_DUMP_TABLE = ETH_CPSW_ALE_PUBLIC_IOCTL(0U),

    /*!
     * \brief Add unicast address entry.
     *
     * IOCTL params:
     * -  inArgs: #Eth_CpswAle_SetUcastEntryInArgs
     * - outArgs: uint32_t
     *
     * Calling context: Task
     */
    ETH_CPSW_ALE_IOCTL_ADD_UCAST = ETH_CPSW_ALE_PUBLIC_IOCTL(1U),

    /*!
     * \brief Add multicast address entry.
     *
     * IOCTL params:
     * -  inArgs: #Eth_CpswAle_SetMcastEntryInArgs
     * - outArgs: uint32_t
     *
     * Calling context: Task
     */
    ETH_CPSW_ALE_IOCTL_ADD_MCAST = ETH_CPSW_ALE_PUBLIC_IOCTL(2U),

    /*!
     * \brief Add inner/outer VLAN entry.
     *
     * IOCTL params:
     * -  inArgs: #Eth_CpswAle_VlanEntryInfo
     * - outArgs: uint32_t
     *
     * Calling context: Task
     */
    ETH_CPSW_ALE_IOCTL_ADD_VLAN = ETH_CPSW_ALE_PUBLIC_IOCTL(3U),

    /*!
     * \brief Add OUI address entry.
     * IOCTL params:
     * -  inArgs: #CpswAle_OuiEntryInfo
     * - outArgs: uint32_t
     *
     * Calling context: Task
     */
    ETH_CPSW_ALE_IOCTL_ADD_OUI = ETH_CPSW_ALE_PUBLIC_IOCTL(4U),

    /*!
     * \brief Add IPv4 address.
     *
     * IOCTL params:
     * -  inArgs: #CpswAle_IPv4EntryInfo
     * - outArgs: uint32_t
     *
     * Calling context: Task
     */
    ETH_CPSW_ALE_IOCTL_ADD_IPV4ADDR = ETH_CPSW_ALE_PUBLIC_IOCTL(5U),

    /*!
     * \brief Add IPv6 address.
     *
     * IOCTL params:
     * -  inArgs: #CpswAle_IPv6EntryInfo
     * - outArgs: uint32_t
     *
     * Calling context: Task
     */
    ETH_CPSW_ALE_IOCTL_ADD_IPV6ADDR = ETH_CPSW_ALE_PUBLIC_IOCTL(6U),

    /*!
     * \brief Add EtherType entry.
     *
     * IOCTL params:
     * -  inArgs: uint16_t
     * - outArgs: uint32_t
     *
     * Calling context: Task
     */
    ETH_CPSW_ALE_IOCTL_ADD_ETHERTYPE = ETH_CPSW_ALE_PUBLIC_IOCTL(7U),

    /*!
     * \brief Return entry info for given unicast address.
     *
     * IOCTL params:
     * -  inArgs: #Eth_CpswAle_MacAddrInfo
     * - outArgs: #Eth_CpswAle_GetUcastEntryOutArgs
     * Calling context: Task
     */
    ETH_CPSW_ALE_IOCTL_LOOKUP_UCAST = ETH_CPSW_ALE_PUBLIC_IOCTL(8U),

    /*!
     * \brief Return entry info for given multicast address.
     *
     * IOCTL params:
     * -  inArgs: #Eth_CpswAle_GetMcastEntryInArgs
     * - outArgs: #Eth_CpswAle_GetMcastEntryOutArgs
     *
     * Calling context: Task
     */
    ETH_CPSW_ALE_IOCTL_LOOKUP_MCAST = ETH_CPSW_ALE_PUBLIC_IOCTL(9U),

    /*!
     * \brief Return entry info for given VLAN id.
     *
     * IOCTL params:
     * -  inArgs: #Eth_CpswAle_VlanIdInfo
     * - outArgs: #CpswAle_GetVlanEntryOutArgs
     *
     * Calling context: Task
     */
    ETH_CPSW_ALE_IOCTL_LOOKUP_VLAN = ETH_CPSW_ALE_PUBLIC_IOCTL(10U),

    /*!
     * \brief Delete entry info for given unicast/multicast address.
     *
     * IOCTL params:
     * -  inArgs: #Eth_CpswAle_MacAddrInfo
     * - outArgs: None
     *
     * Calling context: Task
     */
    ETH_CPSW_ALE_IOCTL_REMOVE_ADDR = ETH_CPSW_ALE_PUBLIC_IOCTL(11U),

    /*!
     * \brief Delete entry info for given VLAN id.
     *
     * IOCTL params:
     * -  inArgs: #Eth_CpswAle_VlanIdInfo
     * - outArgs: None
     *
     * Calling context: Task
     */
    ETH_CPSW_ALE_IOCTL_REMOVE_VLAN = ETH_CPSW_ALE_PUBLIC_IOCTL(12U),

    /*!
     * \brief Delete entry info for given OUI address.
     *
     * IOCTL params:
     * -  inArgs: #CpswAle_OuiEntryInfo
     * - outArgs: None
     *
     * Calling context: Task
     */
    ETH_CPSW_ALE_IOCTL_REMOVE_OUI = ETH_CPSW_ALE_PUBLIC_IOCTL(13U),

    /*!
     * \brief Delete entry info for given IPv4 address.
     *
     * IOCTL params:
     * -  inArgs: #CpswAle_IPv4EntryInfo
     * - outArgs: None
     *
     * Calling context: Task
     */
    ETH_CPSW_ALE_IOCTL_REMOVE_IPV4ADDR = ETH_CPSW_ALE_PUBLIC_IOCTL(14U),

    /*!
     * \brief Delete entry info for given IPv6 address.
     *
     * IOCTL params:
     * -  inArgs: #CpswAle_IPv6EntryInfo
     * - outArgs: None
     *
     * Calling context: Task
     */
    ETH_CPSW_ALE_IOCTL_REMOVE_IPV6ADDR = ETH_CPSW_ALE_PUBLIC_IOCTL(15U),

    /*!
     * \brief Delete entry info for given EtherType.
     *
     * IOCTL params:
     * -  inArgs: uint16_t
     * - outArgs: None
     *
     * Calling context: Task
     */
    ETH_CPSW_ALE_IOCTL_REMOVE_ETHERTYPE = ETH_CPSW_ALE_PUBLIC_IOCTL(16U),

    /*!
     * \brief Delete all learned entries for a given port.
     *
     * Returns the number of learned entries that were successfully removed.
     *
     * IOCTL params:
     * -  inArgs: uint32_t
     * - outArgs: uint32_t
     *
     * Calling context: Task
     */
    ETH_CPSW_ALE_IOCTL_REMOVE_LEARNED_ENTRIES = ETH_CPSW_ALE_PUBLIC_IOCTL(17U),

    /*!
     * \brief Delete all entries.
     *
     * IOCTL params:
     * -  inArgs: None
     * - outArgs: None
     *
     * Calling context: Task
     */
    ETH_CPSW_ALE_IOCTL_REMOVE_ALL_ENTRIES = ETH_CPSW_ALE_PUBLIC_IOCTL(18U),

    /*!
     * \brief Age all entries now.
     *
     * IOCTL params:
     * -  inArgs: None
     * - outArgs: None
     *
     * Calling context:ISR/SWI/Task
     */
    ETH_CPSW_ALE_IOCTL_AGE_ALL_ENTRIES = ETH_CPSW_ALE_PUBLIC_IOCTL(19U),

    /*!
     * \brief Set host port RX filter.
     *
     * IOCTL params:
     * -  inArgs: CpswAle_RxFilter
     * - outArgs: None
     *
     * Calling context: Task
     */
    ETH_CPSW_ALE_IOCTL_SET_RX_FILTER = ETH_CPSW_ALE_PUBLIC_IOCTL(20U),

    /*!
     * \brief Get current host port Rx filter
     *
     * IOCTL params:
     * -  inArgs: None
     * - outArgs: #CpswAle_RxFilter
     *
     * Calling context: Task
     */
    ETH_CPSW_ALE_IOCTL_GET_RX_FILTER = ETH_CPSW_ALE_PUBLIC_IOCTL(21U),

    /*!
     * \brief Set port state.
     *
     * IOCTL params:
     * -  inArgs: #CpswAle_SetPortStateInArgs
     * - outArgs: None
     * Calling context: ISR/SWI/Task
     */
    ETH_CPSW_ALE_IOCTL_SET_PORT_STATE = ETH_CPSW_ALE_PUBLIC_IOCTL(22U),

    /*!
     * \brief Get port state.
     *
     * IOCTL params:
     * -  inArgs: uint32_t
     * - outArgs: CpswAle_PortState
     *
     * Calling context: ISR/SWI/Task
     */
    ETH_CPSW_ALE_IOCTL_GET_PORT_STATE = ETH_CPSW_ALE_PUBLIC_IOCTL(23U),

    /*!
     * \brief Get MAC addresses reachable on given port.
     *
     * IOCTL params:
     * -  inArgs: #CpswAle_GetPortMacAddrInArgs
     * - outArgs: #CpswAle_GetPortMacAddrOutArgs
     *
     * Calling context: Task
     */
    ETH_CPSW_ALE_IOCTL_GET_PORT_MACADDR = ETH_CPSW_ALE_PUBLIC_IOCTL(24U),

    /*!
     * \brief Set default thread configuration.
     *
     * IOCTL params:
     * -  inArgs: #CpswAle_DfltThreadCfg
     * - outArgs: None
     *
     * Calling context:Task
     */
    ETH_CPSW_ALE_IOCTL_SET_DEFAULT_THREADCFG = ETH_CPSW_ALE_PUBLIC_IOCTL(25U),

    /*!
     * \brief Get default thread configuration.
     *
     * IOCTL params:
     * -  inArgs: None
     * - outArgs: #CpswAle_DfltThreadCfg
     *
     * Calling context:Task
     */
    ETH_CPSW_ALE_IOCTL_GET_DEFAULT_THREADCFG = ETH_CPSW_ALE_PUBLIC_IOCTL(26U),

    /*!
     * \brief Set port mirroring configuration.
     *
     * IOCTL params:
     * -  inArgs: #CpswAle_PortMirroringCfg
     * - outArgs: None
     *
     * Calling context: Task
     */
    ETH_CPSW_ALE_IOCTL_SET_PORT_MIRROR_CFG = ETH_CPSW_ALE_PUBLIC_IOCTL(27U),

    /*!
     * \brief Disable port mirror (match mirror, destination port mirror
     *        and source port mirror).
     *
     * IOCTL params:
     * -  inArgs: None
     * - outArgs: None
     *
     * Calling context: Task
     */
    ETH_CPSW_ALE_IOCTL_DISABLE_PORT_MIRROR = ETH_CPSW_ALE_PUBLIC_IOCTL(28U),

    /*!
     * \brief Set port trunking configuration.
     *
     * IOCTL params:
     * -  inArgs: #CpswAle_TrunkCfg
     * - outArgs: None
     *
     * Calling context: Task
     */
    ETH_CPSW_ALE_IOCTL_SET_TRUNK_CFG = ETH_CPSW_ALE_PUBLIC_IOCTL(29U),

    /*!
     * \brief Enable OAM loopback for ports in the given port mask.
     *
     * IOCTL params:
     * -  inArgs: uint32_t
     * - outArgs: None
     *
     * Calling context: Task
     */
    ETH_CPSW_ALE_IOCTL_SET_OAMLPBK_CFG = ETH_CPSW_ALE_PUBLIC_IOCTL(30U),

    /*!
     * \brief Set broadcast/multicast rate limit configuration.
     *
     * IOCTL params:
     * -  inArgs: #CpswAle_SetBcastMcastRateLimitInArgs
     * - outArgs: None
     *
     * Calling context: Task
     */
    ETH_CPSW_ALE_IOCTL_SET_BCAST_MCAST_LIMIT = ETH_CPSW_ALE_PUBLIC_IOCTL(31U),

    /*!
     * \brief Get configured broadcast/multicast rate limit configuration.
     *
     * IOCTL params:
     * -  inArgs: None
     * - outArgs: #CpswAle_GetBcastMcastRateLimitOutArgs
     *
     * Calling context: Task
     */
    ETH_CPSW_ALE_IOCTL_GET_BCAST_MCAST_LIMIT = ETH_CPSW_ALE_PUBLIC_IOCTL(32U),

    /*!
     * \brief Disable broadcast/multicast rate limit.
     *
     * IOCTL params:
     * -  inArgs: None
     * - outArgs: None
     *
     * Calling context: Task
     */
    ETH_CPSW_ALE_IOCTL_DISABLE_BCAST_MCAST_LIMIT = ETH_CPSW_ALE_PUBLIC_IOCTL(33U),

    /*!
     * \brief Set policer/classifier entry.
     *
     * IOCTL params:
     * -  inArgs: #CpswAle_SetPolicerEntryInArgs
     * - outArgs: #CpswAle_SetPolicerEntryOutArgs
     *
     * Calling context: Task
     */
    ETH_CPSW_ALE_IOCTL_SET_POLICER = ETH_CPSW_ALE_PUBLIC_IOCTL(34U),

    /*!
     * \brief Get policer/classifier entry info.
     *
     * IOCTL params:
     * -  inArgs: #CpswAle_PolicerMatchParams
     * - outArgs: #CpswAle_PolicerEntryOutArgs
     *
     * Calling context: Task
     */
    ETH_CPSW_ALE_IOCTL_GET_POLICER = ETH_CPSW_ALE_PUBLIC_IOCTL(35U),

    /*!
     * \brief Delete policer/classifier entry.
     *
     * IOCTL params:
     * -  inArgs: #CpswAle_PolicerMatchParams
     * - outArgs: None
     *
     * Calling context: Task
     */
    ETH_CPSW_ALE_IOCTL_DEL_POLICER = ETH_CPSW_ALE_PUBLIC_IOCTL(36U),

    /*!
     * \brief Dump ALE policer entries.
     *
     * Print ALE policy entries.
     *
     * IOCTL params:
     * -  inArgs: None
     * - outArgs: None
     *
     * Calling context: Task
     */
    ETH_CPSW_ALE_IOCTL_DUMP_POLICER_ENTRIES = ETH_CPSW_ALE_PUBLIC_IOCTL(37U),

    /*!
     * \brief Get ALE policer statistics.
     *
     * IOCTL params:
     * -  inArgs: #CpswAle_GetPolicerStatsInArgs
     * - outArgs: #CpswAle_GetPolicerStatsOutArgs
     *
     * Calling context: Task
     */
    ETH_CPSW_ALE_IOCTL_GET_POLICER_STATS = ETH_CPSW_ALE_PUBLIC_IOCTL(38U),

    /*!
     * \brief Set thread id for given classifier/policer.
     *
     * IOCTL params:
     * -  inArgs: #CpswAle_SetPolicerThreadCfgInArgs
     * - outArgs: #CpswAle_PolicerEntryOutArgs
     *
     * Calling context: Task
     */
    ETH_CPSW_ALE_IOCTL_SET_POLICER_THREADCFG = ETH_CPSW_ALE_PUBLIC_IOCTL(39U),

    /*!
     * \brief Configure policer global settings.
     * IOCTL params:
     * -  inArgs: #CpswAle_PolicerGlobalCfg
     * - outArgs: None
     *
     * Calling context: Task
     */
    ETH_CPSW_ALE_IOCTL_SET_POLICER_GLOBAL_CFG = ETH_CPSW_ALE_PUBLIC_IOCTL(40U),

    /*!
     * \brief Configure policer global settings.
     *
     * IOCTL params:
     * -  inArgs: None
     * - outArgs: #CpswAle_PolicerGlobalCfg
     *
     * Calling context: Task
     */
    ETH_CPSW_ALE_IOCTL_GET_POLICER_GLOBAL_CFG = ETH_CPSW_ALE_PUBLIC_IOCTL(41U),

    /*!
     * \brief Delete all policer entries and associated ALE entry with the
     *        given thread id.
     *
     * IOCTL params:
     * -  inArgs: uint32_t
     * - outArgs: None
     *
     * Calling context: Task
     */
    ETH_CPSW_ALE_IOCTL_DEL_ALL_POLICER_THREADID = ETH_CPSW_ALE_PUBLIC_IOCTL(42U),

    /*!
     * \brief Blacklist classifier to host port.
     *
     * ALE supports feature to allow packets matching policer match criteria
     * that is destined for host port to be dropped.
     *
     * IOCTL params:
     * -  inArgs: #CpswAle_PolicerMatchParams
     * - outArgs: #CpswAle_PolicerEntryOutArgs
     *
     * Calling context: Task
     */
    ETH_CPSW_ALE_IOCTL_BLOCK_CLASSIFIER_HOSTPORT = ETH_CPSW_ALE_PUBLIC_IOCTL(43U),

    /*!
     * \brief Set InterVLAN routing configuration.
     *
     * IOCTL params:
     * -  inArgs: #CpswAle_SetInterVlanCfgInArgs
     * - outArgs: #CpswAle_PolicerEntryOutArgs
     *
     * Calling context: Task
     */
    ETH_CPSW_ALE_IOCTL_SET_INTERVLAN_CFG = ETH_CPSW_ALE_PUBLIC_IOCTL(44U),

    /*!
     * \brief Get InterVLAN routing configuration.
     *
     * IOCTL params:
     * -  inArgs: None
     * - outArgs: None
     *
     * Calling context: Task
     */
    ETH_CPSW_ALE_IOCTL_GET_INTERVLAN_CFG = ETH_CPSW_ALE_PUBLIC_IOCTL(45U),

    /*!
     * \brief Enable promiscuous mode on a MAC port.
     *
     * Enables promiscuous mode on an ALE port configured in MAC-only mode
     * by enabling the copy-all-frames functionality on that port.
     *
     * IOCTL params:
     * -  inArgs: Enet_MacPort
     * - outArgs: None
     *
     * Calling context: Task
     */
    ETH_CPSW_ALE_IOCTL_ENABLE_PROMISC_MODE = ETH_CPSW_ALE_PUBLIC_IOCTL(46U),

    /*!
     * \brief Disable promiscuous mode on a MAC port.
     *
     * Disables promiscuous mode on an ALE port configured in MAC-only mode
     * by disabling the copy-all-frames functionality on that port.
     *
     * IOCTL params:
     * -  inArgs: Enet_MacPort
     * - outArgs: None
     *
     * Calling context: Task
     */
    ETH_CPSW_ALE_IOCTL_DISABLE_PROMISC_MODE = ETH_CPSW_ALE_PUBLIC_IOCTL(47U),
} Eth_CpswAle_Ioctl;

/**
 *  \brief ALE Unicast entry type
 *
 *  Type of the ALE unicast entry.
 */
typedef enum
{
    ETH_CPSW_ALE_UCASTTYPE_NOAGE      = 0U,
    /**< Unicast address that is not ageable */
    ETH_CPSW_ALE_UCASTTYPE_AGENOTOUCH = 1U,
    /**< Ageable unicast address that has not been touched */
    ETH_CPSW_ALE_UCASTTYPE_OUI        = 2U,
    /**< OUI address */
    ETH_CPSW_ALE_UCASTTYPE_AGETOUCH   = 3U
    /**< Ageable unicast address that has been touched */
} Eth_CpswAle_UcastType;

/**
 *  \brief ALE port state
 *
 *  The state of an ALE port used for lookup operations.
 */
typedef enum
{
    CPSWMCAL_ALE_PORTSTATE_DISABLED = 0U,
    /**< Disabled state */
    CPSWMCAL_ALE_PORTSTATE_BLOCKED  = 1U,
    /**< Blocked state */
    CPSWMCAL_ALE_PORTSTATE_LEARN    = 2U,
    /**< Learning state */
    CPSWMCAL_ALE_PORTSTATE_FORWARD  = 3U
    /**< Forwarding state */
} Eth_CpswMcalAle_PortState;

/*!
 * \brief Forward state level.
 *
 * Indicates the port state(s) required for the received port on a destination
 * address lookup in order for the multicast packet to be forwarded to the
 * transmit port(s).
 *
 * A transmit port must be in the Forwarding state in order to forward the packet.
 * If the transmit port mask has multiple set bits then each forward decision is
 * independent of the other transmit port(s) forward decision.
 */
typedef enum Eth_CpswAle_FwdStateLevel_e
{
    /*! Forwarding */
    ETH_CPSW_ALE_FWDSTLVL_FWD  = 0U,

    /*! Blocking/Forwarding/Learning */
    ETH_CPSW_ALE_FWDSTLVL_BLK_FWD_LRN,

    /*! Forwarding/Learning */
    ETH_CPSW_ALE_FWDSTLVL_FWD_LRN,
} Eth_CpswAle_FwdStateLevel;

/**
 *  \brief ALE port configuration
 *
 *  Configuration information for an ALE port.
 */
typedef struct
{
    Eth_CpswMcalAle_PortState state;
    /**< Port state used for lookup operations */
} Eth_CpswMcalAle_PortConfig;

/**
 *  \brief ALE module configuration
 *
 *  Configuration information for the ALE module.
 */
typedef struct
{
    uint32 flags;
    /**< ALE configuration flags \ref ALE_ConfigFlags */
    Eth_CpswMcalAle_PortConfig portCfg[CPSW_PORT_MAX];
    /**< Per-port configuration information */
} Eth_CpswMcalAle_Config;

/**
 *  \brief ALE module
 *
 *  This is an internal/private driver structure and should not be used
 *  or modified by the caller.
 */
typedef struct
{
    CSL_AleRegs *regs;
    /**< CPSW ALE register overlay */
    boolean enable;
    /**< Enable state */
    boolean bypass;
    /**< Bypass state */
    boolean isInit;
    /**< Module initialization state */
    Eth_CpswMcalAle_Config cfg;
    /**< Saved configuration information */
    ETH_CSL_CPSW_ALETABLE_TYPE aleTableType;
     /**< Ale table type */
} Eth_CpswAle;

typedef struct Eth_CpswAle_MacAddrInfo_s
{
    /*! 48-bit unicast/multicast MAC address */
    uint8 addr[ENET_MAC_ADDR_LEN];

    /*! VLAN ID associated with the entry. vlanId of 0 indicates do not use VLAN */
    uint32 vlanId;
} Eth_CpswAle_MacAddrInfo;

/*!
 * \brief Unicast entry type info apart from MAC address that can be set.
 */
typedef struct Eth_CpswAle_UcastEntryInfo_s
{
    /*! Port number that the packet with a unicast destination address is
     *  forwarded to */
    uint32 portNum;

    /*! Indicates that packet with a matching source or destination address
     *  should be dropped */
    boolean blocked;

    /*! Indicates that packet with a matching source address should be dropped,
     *  if the received port number is not equal to the port number in the
     *  table entry */
    boolean secure;

    /*! Indicates that this address can be received in port state other than
     *  Forwarding state */
    boolean super;

    /*! Indicates if the unicast entry is subject to aging */
    boolean ageable;

    /*! Indicates if the portNum refers to trunk number of port number */
    boolean trunk;
} Eth_CpswAle_UcastEntryInfo;

/*!
 * \brief Input args for #ETH_CPSW_ALE_IOCTL_ADD_UCAST command.
 */
typedef struct Eth_CpswAle_SetUcastEntryInArgs_s
{
    /*! Address info to be added */
    Eth_CpswAle_MacAddrInfo addr;

    /*! Unicast entry additional info to be added */
    Eth_CpswAle_UcastEntryInfo info;
} Eth_CpswAle_SetUcastEntryInArgs;

/*!
 * \brief Multicast entry type info apart from MAC address.
 */
typedef struct Eth_CpswAle_McastEntryInfo_s
{
    /*! Supervisory flag which indicates that this address can be received in port
     *  state other than Forwarding state */
    boolean super;

    /*! Forward state level */
    Eth_CpswAle_FwdStateLevel fwdState;

    /*! Port bit mask that is returned with a found multicast destination address */
    uint32 portMask;

    /*! Number of bits in the address to ignore. Up to 10 least significant bits
     *  can be ignored to form a multicast address range */
    uint32 numIgnBits;
} Eth_CpswAle_McastEntryInfo;

/*!
 * \brief Input args for #ETH_CPSW_ALE_IOCTL_ADD_MCAST command.
 */
typedef struct Eth_CpswAle_SetMcastEntryInArgs_s
{
    /*! Multicast MAC address to be added */
    Eth_CpswAle_MacAddrInfo addr;

    /*! Additional info to be added for multicast entry */
    Eth_CpswAle_McastEntryInfo info;
} Eth_CpswAle_SetMcastEntryInArgs;

/*!
 * \brief Double tagging VLAN type.
 */
typedef enum Eth_Enet_VlanTagType_e
{
    /*! Inner or customer tag */
    ETH_ENET_VLAN_TAG_TYPE_INNER = 0U,

    /*! Outer or service tag */
    ETH_ENET_VLAN_TAG_TYPE_OUTER,
} Eth_Enet_VlanTagType;

/*!
 * \brief VLAN tag.
 */
typedef struct Eth_Enet_VlanTag_s
{
    /*! Tag protocol identifier (TPID) */
    uint16 tpid;

    /*! Priority code point (PCP) */
    uint8 pcp;

    /*! Drop elegible indicator (DEI) */
    uint8 dei;

    /*! VLAN id (VID) */
    uint16 vlanId;

    /*! Whether is an inner or outer VLAN tag */
    Eth_Enet_VlanTagType tagType;
} Eth_Enet_VlanTag;

/*!
 * \brief VLAN id info.
 */
typedef struct Eth_CpswAle_VlanIdInfo_s
{
    /*! VLAN Id */
    uint32 vlanId;

    /*! VLAN type is either outer or inner VLAN */
    Eth_Enet_VlanTagType tagType;
} Eth_CpswAle_VlanIdInfo;

/*!
 * \brief VLAN entry type info.
 */
typedef struct Eth_CpswAle_VlanEntryInfo_s
{
    /*! VLAN id / VLAN type to be added */
    Eth_CpswAle_VlanIdInfo vlanIdInfo;

    /*! Port member mask for the VLAN entry being added */
    uint32 vlanMemberList;

    /*! Unregistered multicast flood mask - Mask used for multicast when the
     *  multicast address is not found */
    uint32 unregMcastFloodMask;

    /*! Registered multicast flood mask - Mask used for multicast when the
     *  multicast address is found */
    uint32 regMcastFloodMask;

    /*! Force untagged egress bit flags - Causes the packet VLAN tag to be
     *  removed on egress */
    uint32 forceUntaggedEgressMask;

    /*! VLAN No Learn Mask - When a bit is set in this mask, a packet with an
     *  unknown source address received on the associated port will not be
     *  learned (i.e. When a VLAN packet is received and the source address is
     *  not in the table, the source address will not be added to the table) */
    uint32 noLearnMask;

    /*! VLAN Ingress Check - When set, if the receive port is not a member of
     *  this VLAN then the packet is dropped */
    boolean vidIngressCheck;

    /*! Limit IP NXT hdr field - When set IP packets only with configured
     *  NXTHDR will be allowed @sa CpswAle_IPPktSecurityCfg */
    boolean limitIPNxtHdr;

    /*! VLAN No IPv4 Fragmented frames Control - Causes IPv4 fragmented IP
     *  frames to be dropped */
    boolean disallowIPFrag;
} Eth_CpswAle_VlanEntryInfo;

/* ========================================================================== */
/*                         Global Variables Declarations                      */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  \brief ALE module open function
 *
 *  Opens the ALE module and performs an initial configuration according to
 *  the config structure being passed.  The ALE table is cleared after this
 *  function returns.
 *
 *  This function must be called before any other CpswAle_*() function.
 *
 *  \return none
 */
void CpswAle_open(void);

/**
 *  \brief ALE module close function
 *
 *  Clears all ALE table entries, disables the ALE hardware and closes the
 *  ALE module.
 *
 *  No CpswAle_*() functions must be called after this function.
 *
 */
void CpswAle_close(void);

/**
 *  \brief Enable or disable the bypass mode
 *
 *  Enables or disables the bypass mode.  In bypass mode, packet reception
 *  traffic is forwarded only to the host port, while packet transmission
 *  traffic is processed as in normal mode.
 *
 *  \param enable   TRUE: enable bypass, FALSE: disable bypass
 */
void CpswAle_setBypass(boolean enable);

/**
 *  \brief Get bypass mode of ALE
 *
 *  \param None
 *  \return TRUE if bypass is enabled else FALSE
 */
boolean CpswAle_getBypass(void);


/**
 *  \brief Clear the ALE table
 *
 *  Clear all entries in the ALE table.
 *
 *  \param ale      Eth_CpswAle handle which is allocated by the caller
 */
void CpswAle_clearTable(void);

/**
 *  \brief Set the port state
 *
 *  Sets the state of the given port, which is used for lookup operations.
 *
 *  \param port     Port number
 *  \param state    Port state
 */
void CpswAle_setPortState(CpswPort_Num port,
                          Eth_CpswMcalAle_PortState state);

/**
 *  \brief Add a unicast address entry
 *
 *  Adds a unicast address entry to the ALE table.  Duplicated address
 *  entries are not permitted, so if an entry already exists for the given
 *  address, that entry is updated with the parameters of this function.
 *  Otherwise, a new entry is added.
 *
 *  \param port     Port number
 *  \param addr     Unicast address
 *  \param vlanId   VLAN ID
 *  \param blocked  Indicates if a packet with matching source or destination
 *                  address should be blocked or not
 *  \param secure   Indicates if a packet with matching source address should
 *                  be dropped if the port doesn't match
 *
 *  \return \ref Std_ReturnType
 */
Std_ReturnType CpswAle_addUcastAddr(CpswPort_Num port,
                                    const uint8 *addr,
                                    uint32 vlanId,
                                    uint32 blocked,
                                    uint32 secure);

/**
 *  \brief Remove a unicast address entry
 *
 *  Removes a unicast address entry to the ALE table.
 *
 *  \param addr     Unicast address
 *  \param vlanId   VLAN ID
 *
 *  \return \ref Std_ReturnType
 */
Std_ReturnType CpswAle_removeUcastAddr(const uint8 *addr,
                                       uint32 vlanId);

/**
 *  \brief Add a multicast address entry
 *
 *  Adds a multicast address entry to the ALE table.  Duplicated address
 *  entries are not permitted, so if an entry already exists for the given
 *  address, the port mask of that entry is updated (logical 'or' operation)
 *  with the passed port mask.  Otherwise, a new entry is added.
 *
 *  \param addr     Multicast address
 *  \param vlanId   VLAN ID
 *  \param portMask Port mask
 *  \param super    Indicates if a packet with matching destination address
 *                  is a supervisory packet
 *  \param state    State of the received port(s) on a destination address
 *                  lookup for the packet to be forwarded to the transmit
 *                  ports
 *
 *  \return \ref Std_ReturnType
 */
Std_ReturnType CpswAle_addMcastAddr(const uint8 *addr,
                                    uint32 vlanId,
                                    uint32 portMask,
                                    uint32 super,
                                    Eth_CpswMcalAle_PortState state);

/**
 *  \brief Remove a multicast address entry
 *
 *  Removes a multicast address entry to the ALE table.  The port mask of
 *  ALE entry is updated by removing the port mask being passed.
 *
 *  \param addr     Multicast address
 *  \param vlanId   VLAN ID
 *  \param portMask Port mask
 *
 *  \return \ref Std_ReturnType
 */
Std_ReturnType CpswAle_removeMcastAddr(const uint8 *addr,
                                       uint32 vlanId,
                                       uint32 portMask);

/**
 *  \brief Remove an ALE entry
 *
 *  Removes an ALE entry with a matching address and VLAN ID.
 *
 *  \param addr     Multicast address
 *  \param vlanId   VLAN ID
 *
 *  \return \ref Std_ReturnType
 */
Std_ReturnType CpswAle_removeAddr(const uint8 *addr,
                                  uint32 vlanId);

/**
 *  \brief Set unknown VLAN member list
 *
 *  Sets the unknown VLAN member list mask.
 *
 *  \param portMask Port mask
 */
void CpswAle_setUnknownVlan(uint32 portMask);

/**
 *  \brief Set unknown VLAN registered multicast flood mask
 *
 *  Sets the unknown VLAN registered multicast flood mask.
 *
 *  \param portMask Port mask
 */
void CpswAle_setUnknownRegMcastFlood(uint32 portMask);

/**
 *  \brief Set policer entry for mapping priority to receive threadId
 *
 *
 *  \param policerIdx      policer index
 *  \param priority        priority of packet
 *  \param threadId        ingress receive thread
 *
 *  \return none
 */
void CpswAle_setPolicerPriorityThreadMapping(uint8 policerIdx,
                                             uint8 priority,
                                             uint32 threadId,
                                             uint8 portId);

/**
 *  \brief Disable thread mapping for policer entry
 *
 *
 *  \param policerIdx      policer index
 *
 *  \return none
 */
void CpswAle_deletePolicerThreadMapping(uint8 policerIdx);

/* ========================================================================== */
/*                        Deprecated Function Declarations                    */
/* ========================================================================== */

/* None */

#ifdef __cplusplus
}
#endif

#endif /* CPSWMCAL_ALE_H_ */

/*! @} */
