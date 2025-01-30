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
 *  \file     Eth_GeneralTypes.h
 *
 *  \brief    This file contains general Eth type declarations.
 */

/**
 *  \ingroup MCAL_ETH_API
 *  @{
 */

/* Below are the global requirements which are met by the Eth driver which
 * can't be mapped to a particular source ID */
/*
 * Design:       ETH_DesignId_001
 * Requirements: MCAL-1531, MCAL-1636
 */
/*
 * Requirements: MCAL-1826
 */

#ifndef ETH_GENERALTYPES_H_
#define ETH_GENERALTYPES_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "Std_Types.h"
#include "ComStack_Types.h"

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

/** \brief Max. octets in payload */
#define ETH_DATA_LEN                    ((uint32) 1500U)

/** \brief VLAN tag size */
#define ETH_VLAN_TAG_SIZE               (4U)

/**
 *  \brief Maximum number of CPSW ports supported by the controller.
 *  Note: As of now only one port of the two is supported at the same time.
 */
#define ETH_CPSW_MAX_PORT               ((uint32) 1U)

/**
 *  \anchor DropCount_List
 *  \name Dropcount list
 *
 *  The order of dropcount value is fixed as per spec. These macros should be
 *  used while indexing dropcount array.
 *
 *  @{
 */
/** \brief Number of dropped packets due to buffer overrun */
#define ETH_DRPCNT_BUFFER_OVERRUN_IDX               0U

/** \brief Number of dropped packets due to CRC errors */
#define ETH_DRPCNT_CRC_ERRORS_IDX                   1U

/** \brief Number of undersized packets */
#define ETH_DRPCNT_UNDERSIZE_PACKETS_IDX            2U

/** \brief Number of oversized packets */
#define ETH_DRPCNT_OVERSIZE_PACKETS_IDX             3U

/** \brief Number of aligment errors */
#define ETH_DRPCNT_ALIGNMENT_ERRORS_IDX             4U

/** \brief Number of SQE test error packets */
#define ETH_DRPCNT_SQE_TEST_ERRORS_IDX              5U

/** \brief Number of inbound packets chosen to be discarded */
#define ETH_DRPCNT_IFINDISCARDS_IDX                 6U

/** \brief Number of erroneous inbound packets */
#define ETH_DRPCNT_ERRONEOUS_INBOUND_PACKETS_IDX    7U

/** \brief Number of outbound packets chosen to be discarded */
#define ETH_DRPCNT_IFOUTDISCARDS_IDX                8U

/** \brief Number of erroneous outbound packets */
#define ETH_DRPCNT_ERRONEOUS_OUTBOUND_PACKETS_IDX   9U

/** \brief Number of frames with single collision */
#define ETH_DRPCNT_SINGLE_COLLISION_IDX             10U

/** \brief Number of frames with multiple collisions */
#define ETH_DRPCNT_MULTIPLE_COLLISION_IDX           11U

/** \brief Number of packets with deferred transmission */
#define ETH_DRPCNT_DEFERRED_TRANSMISSION_IDX        12U

/** \brief Number of frames with late colision */
#define ETH_DRPCNT_LATE_COLLISIONS_IDX              13U

/** \brief Number of frames with excessive collisions */
#define ETH_DRPCNT_EXCESSIVE_COLLISIONS_IDX         14U

/** \brief Number of dropped frames due to buffer underrun */
#define ETH_DRPCNT_TX_UNDERRUN_IDX                  15U

/** \brief Number of frames with carrier-sense errors */
#define ETH_DRPCNT_CARRIERSENSE_ERROR_IDX           16U

/** \brief Total number of drop count values */
#define ETH_NUM_DRPCNT_VALUES                       17U

/** \brief Invalid stats value (stat not available) */
#define ETH_STATCNT_INVALID                         0xFFFFFFFFU

/* @} */

/**
 *  \anchor EtherStats_List
 *  \name EtherStats list according to IETF RFC 2819 (Remote Network
 *        Monitoring Management Information Base)
 *
 *  The order of etherstats value is fixed as per spec. These macros
 *  should be used while indexing stats array.
 *
 *  @{
 */
/** \brief Number of drop events */
#define ETH_STATS_DROPEVENTS_IDX                    0U

/** \brief Number of total octets */
#define ETH_STATS_OCTETS_IDX                        1U

/** \brief Number of total packets */
#define ETH_STATS_PKTS_IDX                          2U

/** \brief Number of broadcast packets */
#define ETH_STATS_BROADCASTPKTS_IDX                 3U

/** \brief Number of multicast packets */
#define ETH_STATS_MULTICASTPKTS_IDX                 4U

/** \brief Number of packets with CRC or align errors */
#define ETH_STATS_CRCALIGNERRORS_IDX                5U

/** \brief Number of undersized packets */
#define ETH_STATS_UNDERSIZEPKTS_IDX                 6U

/** \brief Number of oversized packets */
#define ETH_STATS_OVERSIZEPKTS_IDX                  7U

/** \brief Number of fragmented frames */
#define ETH_STATS_FRAGMENTS_IDX                     8U

/** \brief Number of jabber frames */
#define ETH_STATS_JABBERS_IDX                       9U

/** \brief Number of packets that experienced collisions */
#define ETH_STATS_COLLISIONS_IDX                    10U

/** \brief Number of packets of size 64 bytes */
#define ETH_STATS_PKTS64OCTETS_IDX                  11U

/** \brief Number of packets of size 65 to 127 bytes */
#define ETH_STATS_PKTS65TO127OCTETS_IDX             12U

/** \brief Number of packets of size 128 to 255 bytes */
#define ETH_STATS_PKTS128TO255OCTETS_IDX            13U

/** \brief Number of packets of size 256 to 511 bytes */
#define ETH_STATS_PKTS256TO511OCTETS_IDX            14U

/** \brief Number of packets of size 512 to 1023 bytes */
#define ETH_STATS_PKTS512TO1023OCTETS_IDX           15U

/** \brief Number of packets of size 1024 to 1518 bytes */
#define ETH_STATS_PKTS1024TO1518OCTETS_IDX          16U

/** \brief Total number of EtherStat values */
#define ETH_NUM_STATS_VALUES                        17U

/* @} */

/** \brief Number of priorities supported by CPSW */
#define ETH_PRIORITY_NUM                            (8U)

/**
 *  \brief Frame type
 *
 *  This type defines the Ethernet frame type used in the Ethernet frame
 *  header.
 */
/*
 * Design:       ETH_DesignId_033
 * Requirements: MCAL-1645
 */
typedef uint16 Eth_FrameType;

/**
 *  \brief Data type
 *
 *  This type defines the Ethernet data type used for data transmission.
 */
/*
 * Design:       ETH_DesignId_033
 * Requirements: MCAL-1646
 */
typedef uint32 Eth_DataType;

/**
 *  \brief Ethernet buffer identifier type
 *
 *  Ethernet buffer identifier type.
 */
/*
 * Design:       ETH_DesignId_033
 * Requirements: MCAL-1660
*/
typedef uint32 Eth_BufIdxType;

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

/**
 *  \brief Application callback to indicate Rpc dispatch command completion
 *
 *  Pointer to a function that is invoked to indicate completion of
 *  RPC dispatch call.
 *  The RPC command is identified by the sid
 */
typedef void (*Eth_RpcCmdComplete)(uint32 CtrlIdx,
                                   uint8 sid,
                                   Std_ReturnType status);


/**
 *  \brief Application callback to indicate Ethernet firmware registered with
 *         the Eth RPC client
 */
typedef void (*Eth_RpcFwRegistered)(uint32 CtrlIdx);

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/**
 *  \brief Eth driver return type
 *
 *  Ethernet Driver specific return type.
 */
/*
 * Design:       ETH_DesignId_033
 * Requirements: MCAL-1642
 */
typedef enum
{
    ETH_OK          = 0x00U,
    /**< Success */
    ETH_E_NOT_OK    = 0x01U,
    /**< General failure */
    ETH_E_NO_ACCESS = 0x02U
    /**< Ethernet hardware access failure */
} Eth_ReturnType;

/**
 *  \brief Controller mode
 *
 *  This type defines the controller modes.
 */
/*
 * Design:       ETH_DesignId_033
 * Requirements: MCAL-1643
 */
typedef enum
{
    ETH_MODE_DOWN   = 0x00U,
    /**< Controller disabled */
    ETH_MODE_ACTIVE = 0x01U
    /**< Controller enabled */
} Eth_ModeType;

/**
 *  \brief State type
 *
 *  Eth driver status supervision used for development error detection.
 */
/*
 * Design:       ETH_DesignId_033
 * Requirements: MCAL-1644
 */
typedef enum
{
    ETH_STATE_UNINIT = 0x00U,
    /**< Driver is not yet configured */
    ETH_STATE_INIT   = 0x01U,
    /**< Driver is configured */
} Eth_StateType;

/**
 *  \brief Receive status type
 *
 *  Used as out parameter in Eth_Receive() indicates whether a frame has been
 *  received and if so, whether more frames are available or frames got lost.
 */
/*
 * Design:       ETH_DesignId_033
 * Requirements: MCAL-1647
 */
typedef enum
{
    ETH_RECEIVED                     = 0x00U,
    /**< Ethernet frame has been received, no further frames available */
    ETH_NOT_RECEIVED                 = 0x01U,
    /**< Ethernet frame has not been received, no further frames available */
    ETH_RECEIVED_MORE_DATA_AVAILABLE = 0x02U
    /**< Ethernet frame has been received, more frames are available */
} Eth_RxStatusType;

/**
 *  \brief Filter action type
 *
 *  The Enumeration Type Eth_FilterActionType describes the action to be
 *  taken for the MAC address given in *PhysAddrPtr.
 */
/*
 * Design:       ETH_DesignId_033
 * Requirements: MCAL-1648
 */
typedef enum
{
    ETH_ADD_TO_FILTER      = 0x00U,
    /**< Add the MAC address to the filter, meaning allow reception */
    ETH_REMOVE_FROM_FILTER = 0x01U
    /**< Remove the MAC address from the filter, meaning reception is blocked
     *   in the lower layer */
} Eth_FilterActionType;

/**
 *  \brief Quality information regarding the evaluated time stamp
 *
 *  Depending on the HW, quality information regarding the evaluated time
 *  stamp might be supported. If not supported, the value shall be always
 *  Valid. For Uncertain and Invalid values, the upper layer shall discard
 *  the time stamp.
 */
/*
 * Design:       ETH_DesignId_033
 * Requirements: MCAL-1662
 */
typedef enum
{
    ETH_VALID     = 0x00U,
    /**< Valid timestamp */
    ETH_INVALID   = 0x01U,
    /**< Invalid timestamp */
    ETH_UNCERTAIN = 0x02U,
    /**< Uncertain timestamp */
} Eth_TimeStampQualType;

/**
 *  \brief Port identifier
 *
 *  Depending on the CPSW Type (for example CPSW9G) multiple ports
 *  are supported. The specific port is identified using this enum
 */
/*
 * Design:
 * Requirements:
 */
typedef enum
{
    ETH_PORT_HOST_PORT      = 0x00U,
    /**< Host port */
    ETH_PORT_MAC_PORT_1     = 0x01U,
    /**< MAC port 0 */
    ETH_PORT_MAC_PORT_2     = 0x02U,
    /**< MAC port 1 */
    ETH_PORT_MAC_PORT_3    = 0x03U,
    /**< MAC port 2 */
    ETH_PORT_MAC_PORT_4   = 0x04U,
    /**< MAC port 3 */
    ETH_PORT_MAC_PORT_5     = 0x05U,
    /**< MAC port 4 */
    ETH_PORT_MAC_PORT_6     = 0x06U,
    /**< MAC port 5 */
    ETH_PORT_MAC_PORT_7     = 0x07U,
    /**< MAC port 6 */
    ETH_PORT_MAC_PORT_8     = 0x08U,
    /**< MAC port 7 */
    ETH_PORT_MAC_PORT_LAST     = ETH_PORT_MAC_PORT_8,
    /**< Enum indicating last mac port  */
} Eth_PortType;

/** Number of CPSW Type supported */
#define ETH_PORT_COUNT                              (ETH_PORT_MAC_PORT_LAST + 1)


/**
 *  \brief Enet Cpsw Type identifier
 *
 *  Enet CPSW Type enumeration.
 */
/*
 * Design:
 * Requirements:
 */
typedef enum
{
    ETH_ENETTYPE_CPSW2G     = 0x00U,
    /**< CPSW 2-port switch with one host port and 1 external port */
    ETH_ENETTYPE_CPSW9G     = 0x01U,
    /**< CPSW 9-port switch with one host port and 8 external port */
    ETH_ENETTYPE_CPSW5G     = 0x02U,
    /**< CPSW 5-port switch with one host port and 4 external port */
    ETH_ENETTYPE_CPSWLAST   = ETH_ENETTYPE_CPSW5G,
    /**< Enum used to identify the last supported CPSW Type. Used internally */
} Eth_EnetType;


/**
 *  \brief Time stamp type
 *
 *  Variables of this type are used for expressing time stamps including
 *  relative time and absolute calendar time. The absolute time starts
 *  acc. to "[5], Annex C/C1" specification at 1970-01-01.
 *
 *  0 to 281474976710655s
 *  == 3257812230d
 *  [0xFFFF FFFF FFFF]
 *
 *  0 to 999999999ns
 *  [0x3B9A C9FF]
 *  invalid value in nanoseconds: [0x3B9A CA00] to [0x3FFF FFFF]
 *  Bit 30 and 31 reserved, default: 0
 */
/*
 * Design:       ETH_DesignId_033
 * Requirements: MCAL-1663
 */
typedef struct Eth_TimeStampType_s
{
    uint32 nanoseconds;
    /**< Nanoseconds part of the time */
    uint32 seconds;
    /**< 32 bit LSB of the 48 bits Seconds part of the time */
    uint16 secondsHi;
    /**< 16 bit MSB of the 48 bits Seconds part of the time */
} Eth_TimeStampType;

/**
 *  \brief Time interval difference
 *
 *  Variables of this type are used to express time differences in a
 *  usual way. The described "TimeInterval" type referenced in "[5],
 *  chapter 6.3.3.3" will not be used and hereby slightly simplified.
 */
/*
 * Design:       ETH_DesignId_033
 * Requirements: MCAL-1664
 */
typedef struct Eth_TimeIntDiffType_s
{
    Eth_TimeStampType diff;
    /**< Time difference */
    boolean sign;
    /**< Timestamp sign - Positive (True) / negative (False) time */
} Eth_TimeIntDiffType;

/**
 *  \brief Rate ratio type
 *
 *  Variables of this type are used to express frequency ratios.
 */
/*
 * Design:       ETH_DesignId_033
 * Requirements: MCAL-1665
 */
typedef struct Eth_RateRatioType_s
{
    Eth_TimeIntDiffType IngressTimeStampDelta;
    /**< Time difference (IngressTimeStampSync2 - IngressTimeStampSync1) */
    Eth_TimeIntDiffType OriginTimeStampDelta;
    /**< Time difference (OriginTimeStampSync2[FUP2] -
     *   OriginTimeStampSync1[FUP1]) */
} Eth_RateRatioType;

/**
 *  \brief MAC VLAN type
 *
 *  Variables of this type are used for VLAN information.
 */
/*
 * Design:
 * Requirements: MCAL-4839
 */
typedef struct Eth_MacVlanType_s
{
    uint8 MacAddr[6];
    /**< Specifies the MAC address [0..255, 0..255, 0..255, 0..255, 0..255,
     *   0..255] */
    uint16 VlanId;
    /**< Specifies the VLAN address 0..65535 */
    uint8 SwitchPort;
    /**< Port of the switch 0..255 */
    uint16 HashValue;
    /**< Hash value that is generated by the Ethernet switch according the
     *   given MacAddr. If the HashValue is not available the value shall be
     *   set to 0b1111xxxxxxxxxxxx == 0xFxxx (invalid value). If the HashValue
     *   is available the value shall be set to 0b0000xxxxxxxxxxxx == 0x0xxx
     *   (valid value) */
} Eth_MacVlanType;

/**
 *  \brief Counter values type
 *
 *  Variables of this type are used to store counter values according to
 *  IETF RFC 1757, RFC 1643 and RFC2233.
 */
/*
 * Design:       ETH_DesignId_022
 * Requirements: MCAL-4840
 */
typedef struct Eth_CounterType_s
{
    uint32 DropPktBufOverrun;
    /**< Dropped packets due to buffer overrun */
    uint32 DropPktCrc;
    /**< Dropped packets due to CRC errors */
    uint32 UndersizePkt;
    /**< Number of undersize packets which were less than 64 octets long
     *   (excluding framing bits, but including FCS octets) and were otherwise
     *   well formed. (see IETF RFC 1757) */
    uint32 OversizePkt;
    /**< Number of oversize packets which are longer than 1518 octets
     *   (excluding framing bits, but including FCS octets) and were otherwise
     *   well formed. (see IETF RFC 1757) */
    uint32 AlgnmtErr;
    /**< Number of alignment errors, i.e. packets which are received and are not
     *   an integral number of octets in length and do not pass the CRC. */
    uint32 SqeTestErr;
    /**< SQE test error according to IETF RFC1643 dot3StatsSQETestErrors */
    uint32 DiscInbdPkt;
    /**< The number of inbound packets which were chosen to be discarded even
     *   though no errors had been detected to prevent their being deliverable
     *   to a higher-layer protocol. One possible reason for discarding such a
     *   packet could be to free up buffer space. (see IETF RFC 2233
     *   ifInDiscards) */
    uint32 ErrInbdPkt;
    /**< Total number of erroneous inbound packets */
    uint32 DiscOtbdPkt;
    /**< The number of outbound packets which were chosen to be discarded even
     *   though no errors had been detected to prevent their being transmitted.
     *   One possible reason for discarding such a packet could be to free up
     *   buffer space.(see IETF RFC 2233 ifOutDiscards) */
    uint32 ErrOtbdPkt;
    /**< Total number of erroneous outbound packets */
    uint32 SnglCollPkt;
    /**<  Single collision frames: A count of successfully transmitted frames
     *    on a particular interface for which transmission is inhibited by
     *    exactly one collision. (see IETF RFC1643
     *    dot3StatsSingleCollisionFrames) */
    uint32 MultCollPkt;
    /**<  Multiple collision frames: A count of successfully transmitted
     *    frames on a particular interface for which transmission is inhibited
     *    by more than one collision. (see IETF RFC1643
     *    dot3StatsMultipleCollisionFrames) */
    uint32 DfrdPkt;
    /**< Number of deferred transmission: A count of frames for which the first
     *   transmission attempt on a particular interface is delayed because the
     *   medium is busy. (see IETF RFC1643 dot3StatsDeferredTransmissions) */
    uint32 LatCollPkt;
    /**< Number of late collisions: The number of times that a collision is
     *   detected on a particular interface later than 512 bittimes into the
     *   transmission of a packet. (see IETF RFC1643 dot3StatsLateCollisions) */
    uint32 HwDepCtr0;
    /**< Hardware dependent counter value */
    uint32 HwDepCtr1;
    /**< Hardware dependent counter value */
    uint32 HwDepCtr2;
    /**< Hardware dependent counter value */
    uint32 HwDepCtr3;
    /**< Hardware dependent counter value */
} Eth_CounterType;

/**
 *  \brief IETF RFC 2819 Statistics type
 *
 *  Variables of this type are used to store statistics values according to
 *  IETF RFC 2819.
 */
/*
 * Design:       ETH_DesignId_023
 * Requirements: MCAL-4841
 */
typedef struct Eth_RxStatsType_s
{
    uint32 RxStatsDropEvents;
    /**< The total number of events in which packets were dropped by the probe
     *   due to lack of resources. Also described in IETF RFC 2819 MIB
     *   etherStatsDropEvents. */
    uint32 RxStatsOctets;
    /**< The total number of octets of data (including those in bad packets)
     *   received on the network (excluding framing bits but including FCS
     *   octets). Also described in IETF RFC 2819 MIB etherStatsOctets. */
    uint32 RxStatsPkts;
    /**< The total number of packets (including bad packets, broadcast packets,
     *   and multicast packets) received. Also described in IETF RFC 2819 MIB
     *   etherStatsPkts */
    uint32 RxStatsBroadcastPkts;
    /**< The total number of good packets received that were directed to the
     *   broadcast address. Also described in IETF RFC 2819 MIB
     *   etherStatsBroadcastPkts */
    uint32 RxStatsMulticastPkts;
    /**< The total number of good packets received that were directed to a
     *   multicast address. Also described in IETF RFC 2819 MIB
     *   etherStatsMulticastPkts. */
    uint32 RxStatsCrcAlignErrors;
    /**< The total number of packets received that had a length of between 64
     *   and 1518 octets that had either a bad Frame Check Sequence (FCS) with
     *   an integral number of octets (FCS Error) or a bad FCS with a
     *   non-integral number of octets (Alignment Error). Also described in IETF
     *   RFC 2819 MIB etherStatsCRCAlignErrors */
    uint32 RxStatsUndersizePkts;
    /**< The total number of packets received that were less than 64 octets long
     *   (excluding framing bits, but including FCS octets) and were otherwise
     *   well formed. Also described in IETF RFC 2819 MIB
     *   etherStatsUndersizePkts. */
    uint32 RxStatsOversizePkts;
    /**< The total number of packets received that were longer than 1518 octets
     *   (excluding framing bits, but including FCS octets) and were otherwise
     *   well formed. Also described in IETF RFC 2819 MIB
     *   etherStatsOversizePkts. */
    uint32 RxStatsFragments;
    /**< The total number of packets received that were less than 64 octets in
     *   length (excluding framing bits but including FCS octets) and had either
     *   a bad Frame Check Sequence (FCS) with an integral number of octets
     *   (FCS Error) or a bad FCS with a non-integral number of octets (Alignment
     *   Error). Also described in IETF RFC 2819 MIB etherStatsFragments. */
    uint32 RxStatsJabbers;
    /**< The total number of packets received that were longer than1518 octets,
     *   and had either a bad Frame Check Sequence (FCS) with an integral number
     *   of octets (FCS Error) or a bad FCS with a nonintegral number of octets
     *   (Alignment Error). Also described in IETF RFC 2819 MIB
     *   etherStatsJabbers. */
     uint32 RxStatsCollisions;
     /**< The best estimate of the total number of collisions on this Ethernet
      *   segment. Also described in IETF RFC 2819 MIB etherStatsCollisions */
     uint32 RxStatsPkts64Octets;
     /**< The total number of packets (including bad packets) received that
      *   were 64 octets in length. Also described in IETF RFC 2819 MIB
      *   etherStatsPkts64Octets */
     uint32 RxStatsPkts65to127Octets;
     /**< The total number of packets (including bad packets) received that
      *   were between 65 and 127 octets in length. Also described in IETF
      *   RFC 2819 MIB etherStatsPkts65to127Octets */
     uint32 RxStatsPkts128to255Octets;
     /**< The total number of packets (including bad packets) received that
      *   were between 128 and 255 octets in length. Also described in IETF
      *   RFC 2819 MIB etherStatsPkts128to255Octets */
     uint32 RxStatsPkts256to511Octets;
     /**< The total number of packets (including bad packets) received that
      *   were between 256 and 511 octets in length. Also described in IETF
      *   RFC 2819 MIB etherStatsPkts256to511Octets */
     uint32 RxStatsPkts512to1023Octets;
     /**< The total number of packets (including bad packets) received that
      *   were between 512 and 1023 octets in length. Also described in IETF
      *   RFC 2819 MIB etherStatsPkts512to1023Octets */
     uint32 RxStatsPkts1024to1518Octets;
     /**< The total number of packets (including bad packets) received that
      *   were between 1024 and 1518 octets in length. Also described in IETF
      *   RFC 2819 MIB etherStatsPkts1024to1518Octets */
     uint32 RxUnicastFrames;
     /**< The number of subnetwork-unicast packets delivered to a higher-layer
      *   protocol. Also described in IETF RFC1213 MIB ifInUcastPkts */
} Eth_RxStatsType;

/**
 *  \brief IETF RFC 1213 Transmission Statistics type
 *
 *  Variables of this type are used to store transmission statistics according
 *  to IETF RFC 2819.
 */
/*
 * Design:       ETH_DesignId_040
 * Requirements: MCAL-4842
 */
typedef struct Eth_TxStatsType_s
{
    uint32 TxNumberOfOctets;
    /**< The total number of octets transmitted out of the interface, including
     *   framing characters. Also described in IETF RFC1213 MIB ifOutOctets. */
    uint32 TxNUcastPkts;
    /**< The total number of packets that higherlevel protocols requested be
     *   transmitted to a non-unicast (i.e., a subnetworkbroadcast or
     *   subnetwork-multicast) address, including those that were discarded or
     *   not sent. Also described in IETF RFC1213 MIB ifOutNUcastPkts */
    uint32 TxUniCastPkts;
    /**< The total number of packets that higherlevel protocols requested be
     *   transmitted to a subnetwork-unicast address, including those that were
     *   discarded or not sent. Also described in IETF RFC1213 MIB
     *   ifOutUcastPkts */
} Eth_TxStatsType;

/**
 *  \brief IETF RFC 1213 and RFC 1643 Transmission Error Counters type
 *
 *  Variables of this type are used to store transmission error counters
 *  according to IETF RFC 1231 and RFC 1643.
 */
/*
 * Design:       ETH_DesignId_041
 * Requirements: MCAL-4843
 */
typedef struct Eth_TxErrorCounterValuesType_s
{
    uint32 TxDroppedNoErrorPkts;
    /**< Number of outbound packets which were chosen to be discarded even
     *   though no errors had been detected to prevent their being transmitted.
     *   One possible reason for discarding such a packet could be to free up
     *   buffer space. Also described in IETF RFC1213 MIB ifOutDiscards */
    uint32 TxDroppedErrorPkts;
    /**< Transmitted because of errors. Also described in IETF RFC1213 MIB
     *   ifOutErrors */
    uint32 TxDeferredTrans;
    /**< A count of frames for which the first transmission attempt on a
     *   particular interface is delayed because the medium is busy. The count
     *   represented by an instance of this object does not include frames
     *   involved in collisions. Also described in IETF RFC1643 MIB
     *   dot3StatsDeferredTransmissions */
    uint32 TxSingleCollision;
    /**< A count of successfully transmitted frames on a particular interface
     *   for which transmission is inhibited by exactly one collision. A frame
     *   that is counted by an instance of this object is also counted by the
     *   corresponding instance of either the TxUniCastPkts and TxNUcastPkts
     *   and is not counted by the corresponding instance of the
     *   TxMultipleCollision object. Also described in IETF RFC1643 MIB
     *   dot3StatsSingleCollisionFrames */
    uint32 TxMultipleCollision;
    /**< A count of successfully transmitted frames on a particular interface
     *   for which transmission is inhibited by more than one collision. A frame
     *   that is counted by an instance of this object is also counted by the
     *   corresponding instance of either the TxUniCastPkts and TxNUcastPkts
     *   and is not counted by the corresponding instance of the
     *   TxSingleCollision object. Also described in IETF RFC1643 MIB
     *   dot3StatsMultipleCollisionFrames. */
    uint32 TxLateCollision;
    /**< The number of times that a collision is detected on a particular
     *   interface later than 512 bit-times into the transmission of a packet.
     *   Five hundred and twelve bit-times corresponds to 51.2 microseconds on
     *   a 10 Mbit/s system. A (late) collision included in a count represented
     *   by an instance of this object is also considered as a (generic)
     *   collision for purposes of other collision-related statistics. Also
     *   described in IETF RFC1643 MIB dot3StatsLateCollisions */
    uint32 TxExcessiveCollison;
    /**< A count of frames for which transmission on a particular interface
     *   fails due to excessive collisions. Also described in IETF RFC1643 MIB
     *   dot3StatsExcessiveCollisions */
} Eth_TxErrorCounterValuesType;

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
 * \brief Speed types
 *
 *  Speed of the Ethernet interface.
 */
typedef enum
{
    ETH_SPEED_10   = 0x00U,
    /**< 10 Mbps operation */
    ETH_SPEED_100  = 0x01U,
    /**< 100 Mbps operation */
    ETH_SPEED_1000 = 0x02U
    /**< 1000 Mbps operation */
} Eth_SpeedType;

/**
 *  \brief  Type/Speed/Duplex connection type
 *
 *  Ethernet connections based on the type (MII, RMII, RGMII), speed
 *  (10Mbps, 100Mbps, 1Gbps) and duplexity (half, full).
 */
typedef enum
{
    ETH_MAC_CONN_TYPE_RMII_10               = 0x01U,
    /**< MAC connection type for 10Mbps RMII mode */
    ETH_MAC_CONN_TYPE_RMII_100              = 0x02U,
    /**< MAC connection type for 100Mbps RMII mode */
    ETH_MAC_CONN_TYPE_RGMII_FORCE_100_HALF  = 0x03U,
    /**< MAC connection type for forced half-duplex 100Mbps RGMII mode */
    ETH_MAC_CONN_TYPE_RGMII_FORCE_100_FULL  = 0x04U,
    /**< MAC connection type for forced full-duplex 100Mbps RGMII mode */
    ETH_MAC_CONN_TYPE_RGMII_FORCE_1000_FULL = 0x05U,
    /**< MAC connection type for forced full-duplex 1000Mbps RGMII mode */
    ETH_MAC_CONN_TYPE_RGMII_DETECT_INBAND   = 0x06U,
    /**< MAC connection type for RGMII inband detection mode
     *   (speed determined based on received RGMII Rx clock) */
} Eth_MacConnectionType;

/**
 *  \brief Duplexity type
 *
 *  Enumerates duplicity configuration modes.
 */
typedef enum
{
    ETH_DUPLEX_MODE_HALF = 0x00U,
    /**< Half duplex mode. */
    ETH_DUPLEX_MODE_FULL = 0x01U
    /**< Full duplex mode. */
} Eth_DuplexModeType;

/**
 *  \brief Eth driver virtual mac configuration data
 *
 *  Configuration related to virtual MAC configuration.
 */
typedef struct Eth_VirtualMacConfigType_s
{
    uint32 ethfwRpcComChId;
    /**< CddIpc Communication channel */
    Eth_RpcCmdComplete rpcCmdComplete;
    /**< Callback to inidcate completion of prior Eth_dispatchVirtmacXXX API */
    boolean pollRecvMsgInEthMain;
    /**< Flag to enable Eth Rpc recv msg polling in Eth_main */
    Eth_RpcFwRegistered fwRegisteredCb;
    /**< App Callback invoked on ethernet firmware registration is done  */
} Eth_VirtualMacConfigType;


/**
 *  \brief Eth driver configuration data
 *
 *  Implementation specific structure of the post build configuration.
 */
/*
 * Design:       ETH_DesignId_033
 * Requirements: MCAL-1641
 */
typedef struct Eth_ConfigType_s
{
    uint32 ctrlIdx;
    /**< Controller index */
    uint32 udmaInstId;
    /**< UDMA Instance Id. Based on core in which the EthDriver is running
     *  Udma Instance should be chosen
     *  - UDMA_INST_ID_MCU_0 for cores in MCU domain
     *  - UDMA_INST_ID_MAIN_0 for cores in Main domain
     */
    Eth_EnetType enetType;
    /**< CPSW Type identifier */
    Eth_PortType macPort;
    /**< In native MAC mode ethDriver supports control of only one MAC port */
    uint32 dmaTxChIntrNum;
    /**< DMA TX channel interrupt number */
    uint32 dmaRxChIntrNum;
    /**< DMA RX channel interrupt number */
    uint32 mdioBusFreq;
    /**< MDIO bus clock (MDCLK) frequency (in Hz) */
    Eth_MacConnectionType connType;
    /**< MII connection type */
    boolean loopback;
    /**< Loopback enable */
    boolean enableCacheOps;
    /**< Packet memory is cacheable */
    Eth_CacheWbInv cacheWbInv;
    /**< Cache write-back invalidate function */
    Eth_CacheWb cacheWb;
    /**< Cache write-back function */
    Eth_CacheInv cacheInv;
    /**< Cache invalidate function */
    boolean enableVirtualMac;
    /**< Enable Virtual MAC mode of operation .Support only for CPSW9G */
    Eth_VirtualMacConfigType virtualMacCfg;
    /**< Virtual MAC config.Should be populated if enableVirtualMac is true */
} Eth_ConfigType;

/* Eth Transceiver types */

/**
 *  \brief Transceiver mode
 *
 *  This type defines the transceiver modes
 */
/* Requirements: MCAL-1828 */
typedef enum
{
    ETHTRCV_MODE_DOWN   = 0x00U,
    /**< Transceiver disabled */
    ETHTRCV_MODE_ACTIVE = 0x01U
    /**< Transceiver enabled */
} EthTrcv_ModeType;

/**
 *  \brief Transceiver link state
 *
 *  This type defines the Ethernet link state. The link state changes after an
 *  Ethernet cable gets plugged in and the transceivers on both ends negotiated
 *  the transmission parameters (i.e. baud rate and duplex mode).
 */
/* Requirements: MCAL-1829 */
typedef enum
{
    ETHTRCV_LINK_STATE_DOWN   = 0x00U,
    /**< No physical Ethernet connection established */
    ETHTRCV_LINK_STATE_ACTIVE = 0x01U
    /**< Physical Ethernet connection established */
} EthTrcv_LinkStateType;

/**
 *  \brief Transceiver state
 *
 *  Status supervision used for Development Error Detection.
 */
/* Requirements: MCAL-1830 */
typedef enum
{
    ETHTRCV_STATE_UNINIT = 0x00U,
    /**< Driver is not yet configured */
    ETHTRCV_STATE_INIT   = 0x01U,
    /**< Driver is configured */
    ETHTRCV_STATE_ACTIVE = 0x02U
    /**< Driver is active */
} EthTrcv_StateType;

/**
 *  \brief Transceiver baud rate
 *
 *  This type defines the Ethernet baud rate. The baud rate gets either
 *  negotiated between the connected transceivers or has to be configured.
 */
/* Requirements: MCAL-1831 */
typedef enum
{
    ETHTRCV_BAUD_RATE_10MBIT   = 0x00U,
    /**< 10MBIT Ethernet connection */
    ETHTRCV_BAUD_RATE_100MBIT  = 0x01U,
    /**< 100MBIT Ethernet connection */
    ETHTRCV_BAUD_RATE_1000MBIT = 0x02U
    /**< 1000MBIT Ethernet connection */
} EthTrcv_BaudRateType;

/**
 *  \brief Transceiver duplex mode
 *
 *  This type defines the Ethernet duplex mode. The duplex mode gets either
 *  negotiated between the connected transceivers or has to be configured.
 */
/* Requirements: MCAL-1832 */
typedef enum
{
    ETHTRCV_DUPLEX_MODE_HALF = 0x00U,
    /**< Half duplex Ethernet connection */
    ETHTRCV_DUPLEX_MODE_FULL = 0x01U
    /**< Full duplex Ethernet connection */
} EthTrcv_DuplexModeType;

/**
 *  \brief Transceiver wake-up mode
 *
 *  This type controls the transceiver wake up modes and/or clears the
 *  wake-up reason.
 */
/* Requirements: MCAL-1842 */
typedef enum
{
    ETHTRCV_WUM_DISABLE = 0x00U,
    /**< Transceiver wake up disabled */
    ETHTRCV_WUM_ENABLE  = 0x01U,
    /**< Transceiver wake up enabled */
    ETHTRCV_WUM_CLEAR   = 0x02U
    /**< Transceiver wake up reason cleared */
} EthTrcv_WakeupModeType;

/**
 *  \brief Transceiver wake-up reason type
 *
 *  This type defines the transceiver wake up reasons.
 */
/* Requirements: MCAL-1843 */
typedef enum
{
    ETHTRCV_WUR_NONE     = 0x00U,
    /**< No wake up reason detected */
    ETHTRCV_WUR_GENERAL  = 0x01U,
    /**< General wake up detected, no distinct reason supported
     *   by hardware */
    ETHTRCV_WUR_BUS      = 0x02U,
    /**< Bus wake up detected. Available if supported by hardware */
    ETHTRCV_WUR_INTERNAL = 0x03U,
    /**< Internal wake up detected. Available if supported by hardware */
    ETHTRCV_WUR_RESET    = 0x04U,
    /**< Reset wake up detected. Available if supported by hardware */
    ETHTRCV_WUR_POWER_ON = 0x05U,
    /**< Power on wake up detected. Available if supported by hardware */
    ETHTRCV_WUR_PIN      = 0x06U,
    /**< Pin wake up detected. Available if supported by hardware */
    ETHTRCV_WUR_SYSERR   = 0x07U
    /**< System error wake up detected. Available if supported by hardware */
} EthTrcv_WakeupReasonType;

/**
 *  \brief Describes the possible PHY test modes
 *
 *  This type describes the possible PHY test modes
 */
/* Requirements: MCAL-4821 */
typedef enum
{
    ETHTRCV_PHYTESTMODE_NONE = 0x00U,
    /**< normal operation */
    ETHTRCV_PHYTESTMODE_1    = 0x01U,
    /**< test transmitter droop */
    ETHTRCV_PHYTESTMODE_2    = 0x02U,
    /**< test master timing jitter */
    ETHTRCV_PHYTESTMODE_3    = 0x03U,
    /**< test slave timing jitter */
    ETHTRCV_PHYTESTMODE_4    = 0x04U,
    /**< test transmitter distortion */
    ETHTRCV_PHYTESTMODE_5    = 0x05U,
    /**< test power spectral density (PSD) mask */
} EthTrcv_PhyTestModeType;

/**
 *  \brief Describes the possible PHY loopback modes
 *
 *  This type describes the possible PHY loopback modes
 */
/* Requirements: MCAL-4893 */
typedef enum
{
    ETHTRCV_PHYLOOPBACK_NONE       = 0x00U,
    /**< normal operation */
    ETHTRCV_PHYLOOPBACK_INTERNAL   = 0x01U,
    /**< internal operation */
    ETHTRCV_PHYLOOPBACK_EXTERNAL   = 0x02U,
    /**< external loopback */
    ETHTRCV_PHYLOOPBACK_REMOTE     = 0x03U,
    /**< remote loopback */
} EthTrcv_PhyLoopbackModeType;

/**
 *  \brief Describes the possible PHY transmit modes
 *
 *  This type describes the possible PHY transmit modes
 */
/* Requirements: MCAL-4895 */
typedef enum
{
    ETHTRCV_PHYTXMODE_NORMAL          = 0x00U,
    /**< normal operation */
    ETHTRCV_PHYTXMODE_TX_OFF          = 0x01U,
    /**< transmitter disabled */
    ETHTRCV_PHYTXMODE_SCRAMBLER_OFF   = 0x02U,
    /**< scrambler disabled */
} EthTrcv_PhyTxModeType;

/**
 *  \brief Describes the results of the cable diagnostics
 *
 *  This type describes the results of the cable diagnostics
 */
/* Requirements: MCAL-4897 */
typedef enum
{
    ETHTRCV_CABLEDIAG_OK      = 0x00U,
    /**< Cable diagnostic ok */
    ETHTRCV_CABLEDIAG_ERROR   = 0x01U,
    /**< Cable diagnostic failed */
    ETHTRCV_CABLEDIAG_SHORT   = 0x02U,
    /**< Short circuit detected */
    ETHTRCV_CABLEDIAG_OPEN    = 0x03U,
    /**< Open circuit detected */
} EthTrcv_CableDiagResultType;

/**
 *  \brief Advertisement configurations
 */
/* Requirements: MCAL-1764 */
typedef enum
{
    ETHERNET_ADV_CAPAB_10_HALF   = 0x01U,
    /**< 10 Mbps half operation */
    ETHERNET_ADV_CAPAB_10_FULL   = 0x02U,
    /**< 10 Mbps full operation */
    ETHERNET_ADV_CAPAB_100_HALF  = 0x04U,
    /**< 100 Mbps half operation */
    ETHERNET_ADV_CAPAB_100_FULL  = 0x08U,
    /**< 100 Mbps full operation */
    ETHERNET_ADV_CAPAB_1000_HALF = 0x10U,
    /**< 1000 Mbps half operation */
    ETHERNET_ADV_CAPAB_1000_FULL = 0x20U
    /**< 1000 Mbps full operation */
} EthTrcv_AdvCapabType;

/**
 *  \brief Implementation specific structure of the post build configuration.
 */
/* Requirements: MCAL-1827 */
typedef struct EthTrcv_ConfigType_s
{
    uint8 ctrlIdx;
    /**< Index of the controller */
    uint8 trcvIdx;
    /**< Index of the transceiver */
    uint8 miiIdx;
    /**< PHY address */
    uint32 macType;
    /**< PHY interface type */
    boolean enableAutoNeg;
    /**< PHY auto-negotiation enable */
    uint32 advertiseCapab;
    /**< PHY advertisement capability */
    uint32 linkPartCapab;
    /**< Link partner capability */
    boolean isGigCapab;
    /**< PHY gigabit capability */
    EthTrcv_BaudRateType baudRate;
    /**< PHY operating speed */
    EthTrcv_DuplexModeType duplexMode;
    /**< PHY operating duplexity */
    boolean loopback;
    /**< PHY loopback enable. For test purpose only */
    uint32 phyId;
    /**< PHY identification as per IEEE registers PHYIDR1 and PHYIDR2.
     *   It's the concatenation of OUI, model and revision numbers */
    void *extendedConfig;
    /**< PHY specific configuration (chosen by driver based on PHY ID) */
} EthTrcv_ConfigType;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */

#ifdef __cplusplus
}
#endif

#endif  /* ETH_GENERALTYPES_H_ */

/* @} */
