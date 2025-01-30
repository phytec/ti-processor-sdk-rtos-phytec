/**
 * @file  csl/src/ip/cpsw/V5/csl_cpsw.h
 *
 * @brief
 *  API Auxilary header file for Ethernet switch module CSL.
 *
 *  Contains the different control command and status query functions definations
 *
 *  ============================================================================
 *  @n   (C) Copyright 2024, Texas Instruments, Inc.
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
 *
*/

#ifndef ETH_CSL_CPSW_V5_H_
#define ETH_CSL_CPSW_V5_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "cslr_ale.h"
#include "cslr_xge_cpsw.h"


#ifndef TRUE
#define TRUE    (1)
#define FALSE   (0)
#endif

/* Define CSL_TRUE/CSL_FALSE to go with uint32 */
#ifndef CSL_TRUE
#define CSL_TRUE    (1U)
#endif
#ifndef CSL_FALSE
#define CSL_FALSE   (0U)
#endif

/*! @} */


/** \brief CSL error type */
typedef sint32 CSL_ErrType_t;

/**
 *  \anchor CSL_ErrType_t
 *  \name CSL Error Types
 *
 *  CSL function return error codes.
 *
 *  @{
 */
#define CSL_PASS                        ( (sint32) (0))
#define CSL_EFAIL                       (-(sint32) (1))
#define CSL_EBADARGS                    (-(sint32) (2))
#define CSL_EINVALID_PARAMS             (-(sint32) (3))
#define CSL_ETIMEOUT                    (-(sint32) (4))
#define CSL_EOUT_OF_RANGE               (-(sint32) (5))
#define CSL_EUNSUPPORTED_CMD            (-(sint32) (6))
#define CSL_EUNSUPPORTED_OPS            (-(sint32) (7))
#define CSL_EALLOC                      (-(sint32) (8))
/*! @} */

/** \brief Define NULL if not defined */
#ifndef NULL
#define NULL            ((void*)0)
#endif

#ifndef NULL_PTR
#define NULL_PTR ((void *)0)
#endif

/** @addtogroup CSL_CPSWITCH_DATASTRUCT
 @{ */

/** @brief ALE control register configuration definitions */

/**  Enable Broadcast/Multicast rate limit */
#define    ETH_CSL_CPSW_ALECONTROL_RATELIMIT_EN      (1 << 0u)

/**  MAC auhorization mode enable */
#define    ETH_CSL_CPSW_ALECONTROL_AUTHMODE_EN       (1 << 1u)

/**  VLAN Aware Mode enable */
#define    ETH_CSL_CPSW_ALECONTROL_VLANAWARE_EN      (1 << 2u)

/**  Tx rate limit enable */
#define    ETH_CSL_CPSW_ALECONTROL_RATELIMIT_TX_EN   (1 << 3u)

/**  OUI deny enable */
#define    ETH_CSL_CPSW_ALECONTROL_OUIDENY_EN        (1 << 5u)

/**  VID0 mode enable */
#define    ETH_CSL_CPSW_ALECONTROL_VID0MODE_EN       (1 << 6u)

/**  Learn no VID enable */
#define    ETH_CSL_CPSW_ALECONTROL_LEARN_NO_VID_EN   (1 << 7u)

/**  Age out now enable */
#define    ETH_CSL_CPSW_ALECONTROL_AGEOUT_NOW_EN     (1 << 29u)

/**  Clear table enable */
#define    ETH_CSL_CPSW_ALECONTROL_CLRTABLE_EN       (1 << 30u)

/**  ALE enable */
#define    ETH_CSL_CPSW_ALECONTROL_ALE_EN            (1 << 31u)

/** @brief Port Mask definitions */

/**  Port 0 Enable */
#define    ETH_CSL_CPSW_PORTMASK_PORT0_EN            (1 << 0u)

/**  Port 1 Enable */
#define    ETH_CSL_CPSW_PORTMASK_PORT1_EN            (1 << 1u)

/**  Port 2 Enable */
#define    ETH_CSL_CPSW_PORTMASK_PORT2_EN            (1 << 2u)

/**  Port 3 Enable */
#define    ETH_CSL_CPSW_PORTMASK_PORT3_EN            (1 << 3u)

/**  Port 4 Enable */
#define    ETH_CSL_CPSW_PORTMASK_PORT4_EN            (1 << 4u)

/**  Port 5 Enable */
#define    ETH_CSL_CPSW_PORTMASK_PORT5_EN            (1 << 5u)

/**  Port 6 Enable */
#define    ETH_CSL_CPSW_PORTMASK_PORT6_EN            (1 << 6u)

/**  Port 7 Enable */
#define    ETH_CSL_CPSW_PORTMASK_PORT7_EN            (1 << 7u)

/**  Port 8 Enable */
#define    ETH_CSL_CPSW_PORTMASK_PORT8_EN            (1 << 8u)

/** @brief
 *
 *  Holds the Time sync submodule's version info.
 */
typedef struct {
    /**  Minor version value */
    uint32      minorVer;

    /**  Major version value */
    uint32      majorVer;

    /**  RTL version value */
    uint32      rtlVer;

    /**  Identification value */
    uint32      id;
} ETH_CSL_CPSW_VERSION;

/** @brief
 *
 *  Holds CPSW control register contents.
 */
typedef struct {
    /**  FIFO loopback mode */
    uint32      fifoLb;

    /**  Vlan aware mode */
    uint32      vlanAware;

    /** Port 0 Enable */
    uint32      p0Enable;

    /**  Port 0 Pass Priority Tagged */
    uint32      p0PassPriTag;

    /**  Port 1 Pass Priority Tagged */
    uint32      p1PassPriTag;

    /**  Port 2 Pass Priority Tagged */
    uint32      p2PassPriTag;

    /**  Port 3 Pass Priority Tagged */
    uint32      p3PassPriTag;

    /**  Port 4 Pass Priority Tagged */
    uint32      p4PassPriTag;

    /**  Port 5 Pass Priority Tagged */
    uint32      p5PassPriTag;

    /**  Port 6 Pass Priority Tagged */
    uint32      p6PassPriTag;

    /**  Port 7 Pass Priority Tagged */
    uint32      p7PassPriTag;

    /**  Port 8 Pass Priority Tagged */
    uint32      p8PassPriTag;

    /**  Port 0 Transmit CRC remove */
    uint32      p0TxCrcRemove;

    /**  Port 0 Receive Short Packet Pad
         0 - short packets are dropped
         1 - short packets are padded to 64-bytes (with pad and added CRC)
             if the CRC is not passed in.  Short packets are dropped if the CRC is
             passed (in the Info0 word).
      */
    uint32      p0RxPad;

    /**  Port 0 Pass Received CRC errors */
    uint32      p0RxPassCrcErr;

    /**  Energy Efficient Ethernet enable */
    uint32      eeeEnable;

} ETH_CSL_CPSW_CONTROL;

/** @brief  CPSW_THRU_RATE register
 *
 *  Holds CPSW_THRU_RATE register contents.
 */
typedef struct {
    /** Ethernet Port Switch FIFO receive through rate.
     *  This register value is the maximum throughput of the Ethernet ports
     *  to the crossbar SCR.  The default is one 8-byte word for every
     *  3 VBUSP_GCLK periods maximum.
     *  The minimum value is 2.
     *  This is not a field that is intended to be changed by a user
     */
    uint32      enetRxThruRate;
    /** CPPI FIFO (port 0) receive through rate.
     * This register value is the maximum throughput of the CPPI FIFO (port 0)
     * into the CPSW_NU.
     * The minimum value is 1.
     * This field is not intended to be changed by the user
     */
    uint32      cppiRxThruRate;
} ETH_CSL_CPSW_THRURATE;

/** @brief
 *
 *  Holds CPPI P0 Control register contents.
 */
typedef struct {
    /** Port 0 receive remap thread to DSCP IPV6 priority  */
    uint32      p0RxRemapDscpIpv6;

    /** Port 0 receive remap thread to DSCP IPV6 priority.  */
    uint32      p0RxRemapDscpIpv4;

    /** Port 0 receive remap thread to VLAN.  */
    uint32      p0RxRemapVlan;

    /** Port 0 receive ECC Error Enable
      * This bit must be set to enable receive ECC error operations
      */
    uint32      p0RxEccErrEn;

    /** Port 0 transmit ECC Error Enable
      * This bit must be set to enable transmit ECC error operations
      */
    uint32     p0TxEccErrEn;

    /** Port 0 IPv6 DSCP enable
     *  0 - Ipv6 DSCP priority mapping is disabled
     *  1 - Ipv6 DSCP priority mapping is enabled
     */
    uint32      p0DscpIpv6En;

    /** Port 0 IPv4 DSCP enable
      * 0 - Ipv4 DSCP priority mapping is disabled
      * 1 - Ipv4 DSCP priority mapping is enabled
      */
    uint32      p0DscpIpv4En;

    /** Port 0 Receive (port 0 ingress) Checksum Enable
      *  0 - Port 0 receive checksum is disabled
      *  1 - Port 0 receive checksum is enabled
      */
    uint32      p0RxChksumEn;

} ETH_CSL_CPSW_CPPI_P0_CONTROL;

/** @brief
 *
 *  Holds CPPI_P0_Rx_Gap register contents.
 *  This is applicable only for 2 port switch
 */
typedef struct
{
    /** Receive Gap Count - This is the number of clocks that will in the
     *  gap between received packet on port 0 when a priority has
     *  rxGapEnPri[7-0] set
     */
    uint32 rxGapCnt;

    /** Receive Gap Enable for Priority 7 */
    uint32 rxGapEnPri7;

    /** Receive Gap Enable for Priority 6 */
    uint32 rxGapEnPri6;

    /** Receive Gap Enable for Priority 5 */
    uint32 rxGapEnPri5;

    /** Receive Gap Enable for Priority 4 */
    uint32 rxGapEnPri4;

    /** Receive Gap Enable for Priority 3 */
    uint32 rxGapEnPri3;

    /** Receive Gap Enable for Priority 2 */
    uint32 rxGapEnPri2;

    /** Receive Gap Enable for Priority 1 */
    uint32 rxGapEnPri1;

    /** Receive Gap Enable for Priority 0 */
    uint32 rxGapEnPri0;

} ETH_CSL_CPSW_CPPI_P0_RXGAP;

/** @brief
 *
 *  Holds CPPI_P0_FIFO_Status  register contents.
 *  This is not applicable for 2 port switch
 */
typedef struct
{
    /** Port 0 Transmit FIFO Priority Active for priority 0
      * Indicates whether the corresponding FIFO priority has one or more
      * queued packets on it or not.
      * note: for N=2 this field is always zero (there is no transmit FIFO).
      */
    uint32 p0TxPriActivePri0;

    /** Port 0 Transmit FIFO Priority Active for priority 1
      * Indicates whether the corresponding FIFO priority has one or more
      * queued packets on it or not.
      * note: for N=2 this field is always zero (there is no transmit FIFO).
      */
    uint32 p0TxPriActivePri1;

    /** Port 0 Transmit FIFO Priority Active for priority 2
      * Indicates whether the corresponding FIFO priority has one or more
      * queued packets on it or not.
      * note: for N=2 this field is always zero (there is no transmit FIFO).
      */
    uint32 p0TxPriActivePri2;

    /** Port 0 Transmit FIFO Priority Active for priority 3
      * Indicates whether the corresponding FIFO priority has one or more
      * queued packets on it or not.
      * note: for N=2 this field is always zero (there is no transmit FIFO).
      */
    uint32 p0TxPriActivePri3;

    /** Port 0 Transmit FIFO Priority Active for priority 4
      * Indicates whether the corresponding FIFO priority has one or more
      * queued packets on it or not.
      * note: for N=2 this field is always zero (there is no transmit FIFO).
      */
    uint32 p0TxPriActivePri4;

    /** Port 0 Transmit FIFO Priority Active for priority 5
      * Indicates whether the corresponding FIFO priority has one or more
      * queued packets on it or not.
      * note: for N=2 this field is always zero (there is no transmit FIFO).
      */
    uint32 p0TxPriActivePri5;

    /** Port 0 Transmit FIFO Priority Active for priority 6
      * Indicates whether the corresponding FIFO priority has one or more
      * queued packets on it or not.
      * note: for N=2 this field is always zero (there is no transmit FIFO).
      */
    uint32 p0TxPriActivePri6;

    /** Port 0 Transmit FIFO Priority Active for priority 7
      * Indicates whether the corresponding FIFO priority has one or more
      * queued packets on it or not.
      * note: for N=2 this field is always zero (there is no transmit FIFO).
      */
    uint32 p0TxPriActivePri7;

} ETH_CSL_CPSW_CPPI_P0_FIFOSTATUS;

/** @brief
 *
 *  Holds ETH_CSL_CPSW_CPPI_P0_HOSTBLKSPRI  register contents.
 *  This is not used for 2 port switch
 */
typedef struct
{
    /** Host Blocks Per Priority 7
      */
    uint32 p0HostBlksPri7;

    /** Host Blocks Per Priority 6
      */
    uint32 p0HostBlksPri6;

    /** Host Blocks Per Priority 5
      */
    uint32 p0HostBlksPri5;

    /** Host Blocks Per Priority 4
      */
    uint32 p0HostBlksPri4;

    /** Host Blocks Per Priority 3
      */
    uint32 p0HostBlksPri3;

    /** Host Blocks Per Priority 2
      */
    uint32 p0HostBlksPri2;

    /** Host Blocks Per Priority 1
      */
    uint32 p0HostBlksPri1;

    /** Host Blocks Per Priority 0
      */
    uint32 p0HostBlksPri0;

} ETH_CSL_CPSW_CPPI_P0_HOSTBLKSPRI;

/** @brief
 *
 *  Holds flow control register contents.
 */
typedef struct {
    /**  Port 0 flow control enable */
    uint32      p0FlowEnable;

    /**  Port 1 flow control enable */
    uint32      p1FlowEnable;

    /**  Port 2 flow control enable */
    uint32      p2FlowEnable;

    /**  Port 3 flow control enable */
    uint32      p3FlowEnable;

    /**  Port 4 flow control enable */
    uint32      p4FlowEnable;

    /**  Port 5 flow control enable */
    uint32      p5FlowEnable;

    /**  Port 6 flow control enable */
    uint32      p6FlowEnable;

    /**  Port 7 flow control enable */
    uint32      p7FlowEnable;

    /**  Port 8 flow control enable */
    uint32      p8FlowEnable;

} ETH_CSL_CPSW_FLOWCNTL;

/** @brief
 *
 *  Holds the ALE submodule's version info.
 */
typedef struct {
    /**  Minor version value */
    uint32      minorVer;

    /**  Major version value */
    uint32      majorVer;

    /**  RTL version value */
    uint32      rtlVer;

    /**  Identification value */
    uint32      id;
} ETH_CSL_CPSW_ALE_VERSION;

/** @brief
 *
 *  Defines ALE table types support
 */
typedef enum {
    ETH_CSL_CPSW_ALETABLE_TYPE_4PORT,
    ETH_CSL_CPSW_ALETABLE_TYPE_9PORT,
} ETH_CSL_CPSW_ALETABLE_TYPE;

/** @brief
 *
 *  Defines ALE port states
 */
typedef enum {
    ETH_CSL_ALE_PORTSTATE_DISABLED = 0U,
    ETH_CSL_ALE_PORTSTATE_BLOCKED,
    ETH_CSL_ALE_PORTSTATE_LEARN,
    ETH_CSL_ALE_PORTSTATE_FORWARD
} ETH_CSL_CPSW_ALE_PORTSTATE;

/** @brief
 *
 *  Holds the ALE Port control register info.
 */
typedef struct {
    /**  Port state */
    ETH_CSL_CPSW_ALE_PORTSTATE  portState;

    /**  Drop non-VLAN tagged ingress packets?  */
    uint32                  dropUntaggedEnable;

    /**  VLAN ID Ingress check enable */
    uint32                  vidIngressCheckEnable;

    /**  No learn mode enable */
    uint32                  noLearnModeEnable;

    /**  No Source Address Update enable */
    uint32                  noSaUpdateEnable;

    /** MAC only mode enable:
      * When set allows the port to be treated like
      * a Mac port for the host. All traffic received
      * is sent only to the host. The host must direct
      * traffic to this port as the lookup engine will
      * not send traffic to ports with macOnlyEnable
      * and noLearnModeEnable is set.
      * If macOnlyEnable is set and noLearnModeEnable
      * is not set, the host can send non-directed packets
      * to a lookup destination with macOnlyEnable set.
      * It is also possible that the host can broadcast
      * to all ports including Mac Only ports in this mode.
      */
    uint32                  macOnlyEnable;

    /** Disable MAC Authorization Mode for this port
      * @note: This field is only valid when CPSW
      *        MAC authentication is enabled.
      */
    uint32                  macAuthDisable;

    /** Mac Only Copy All Frames:
      * Set: A Mac Only port will transfer all received
      *       good frames to the host.
      * Clear: A Mac Only port will transfer packets to
      *        the host based on ALE destination address
      *        lookup operation.
      */
    uint32                  macOnlyCafEnable;

    /**  Multicast packet rate limit */
    uint32                  mcastLimit;

    /**  Broadcast packet rate limit */
    uint32                  bcastLimit;
    /** Drop Dual VLAN - When set will cause any received
     *  packet with dual VLAN stag followed by ctag to be
     *  dropped     */
    uint32                  dropDualVlan;
    /** Drop Double VLAN - When set cause any received
     *  packet with double VLANs to be dropped. That is if
     *  there are two ctag or two stag fields in the packet it will
     *  be dropped  */
    uint32                  dropDoubleVlan;

} ETH_CSL_CPSW_ALE_PORTCONTROL;

/** @brief
 *
 *  Defines ALE Table Entry types
 */
typedef enum {
    ETH_CSL_ALE_ENTRYTYPE_FREE = 0U,
    ETH_CSL_ALE_ENTRYTYPE_ADDRESS,
    ETH_CSL_ALE_ENTRYTYPE_VLAN,
    ETH_CSL_ALE_ENTRYTYPE_VLANADDRESS
} ETH_CSL_CPSW_ALE_ENTRYTYPE;

/** @brief
 *
 *  ALE Table entry type: MAC ADDRESS
 */
#define  ETH_CSL_ALE_ENTRYTYPE_MAC_ADDR     ETH_CSL_ALE_ENTRYTYPE_ADDRESS

/** @brief
 *
 *  ALE Table entry type: POLICER ENTRY
 */
#define  ETH_CSL_ALE_ENTRYTYPE_POLICER      ETH_CSL_ALE_ENTRYTYPE_VLAN

/** @brief
 *
 *  Defines ALE Unicast types
 */
typedef enum {
    ETH_CSL_ALE_UCASTTYPE_UCAST_NOAGE = 0,
    ETH_CSL_ALE_UCASTTYPE_UCAST_AGENOTOUCH,
    ETH_CSL_ALE_UCASTTYPE_UCAST_OUI,
    ETH_CSL_ALE_UCASTTYPE_UCAST_AGETOUCH
} ETH_CSL_CPSW_ALE_UCASTTYPE;

/** @brief
 *
 *  Defines ALE Address types
 */
typedef enum {
    ETH_CSL_ALE_ADDRTYPE_UCAST = 0,
    ETH_CSL_ALE_ADDRTYPE_MCAST,
    ETH_CSL_ALE_ADDRTYPE_OUI
} ETH_CSL_CPSW_ALE_ADDRTYPE;


/** @brief
 *
 *  Defines ALE Policer Entry  types
 */
typedef enum {
    ETH_CSL_ALE_POLICER_ENTRYTYPE_VLAN = 0,     /** VLAN or Inner VLAN */
    ETH_CSL_ALE_POLICER_ENTRYTYPE_OVLAN,        /** Outer VLAN */
    ETH_CSL_ALE_POLICER_ENTRYTYPE_ETHERTYPE,    /** Ethertype */
    ETH_CSL_ALE_POLICER_ENTRYTYPE_IPV4,         /** IPv4 address */
    ETH_CSL_ALE_POLICER_ENTRYTYPE_IPV6          /** IPv6 address */
} ETH_CSL_CPSW_ALE_POLICER_ENTRYTYPE;

/** @brief
 *
 *  Holds the ALE Multicast Address Table entry
 *  configuration.
 */
typedef struct {
    /**  Multicast address */
    uint8                       macAddress [6];

    /**  Multicast forward state  */
    uint32                      mcastFwdState;

    /**  Supervisory bit enable? */
    uint32                      superEnable;

    /**  Port Mask. */
    uint32                      portMask;

    /** Ignore bits in multicast address */
    uint32                      ignMBits;
} ETH_CSL_CPSW_ALE_MCASTADDR_ENTRY;

/** @brief
 *
 *  Holds the ALE VLAN/Multicast Address Table entry
 *  configuration.
 */
typedef struct {
    /**  Multicast address */
    uint8                       macAddress [6];

    /**  VLAN Id  */
    uint32                      vlanId;

    /**  Multicast forward state  */
    uint32                      mcastFwdState;

    /**  Supervisory bit enable? */
    uint32                      superEnable;

    /**  Port Mask. */
    uint32                      portMask;

    /**  Enable multicast address mask */
    uint32                      ignMBits;
} ETH_CSL_CPSW_ALE_VLANMCASTADDR_ENTRY;

/** @brief
 *
 *  Holds the ALE Unicast Address Table entry
 *  configuration.
 */
typedef struct {
    /**  Unicast address */
    uint8                       macAddress [6];

    /**  Entry ageable or not  */
    uint32                      ageable;

    /**  Entry touched or not  */
    uint32                      touched;


    /**  Secure bit enable?  */
    uint32                      secureEnable;

    /**  Block bit enable? */
    uint32                      blockEnable;

    /**  Port Number to forward matching packets to. */
    uint32                      portNumber;

    /**  Trunk Indicator
      *  0: the port bits in the entry are the port number
      *  1: the port bits in the entry are the trunk  number*/
    uint32                      trunkFlag;

} ETH_CSL_CPSW_ALE_UNICASTADDR_ENTRY;

/** @brief
 *
 *  Holds the ALE OUI Unicast Address Table entry
 *  configuration.
 */
typedef struct {
    /**  OUI Unicast address */
    uint8                       ouiAddress [3];

    /**  Entry ageable or not  */
    uint32                      ageable;

    /**  Entry touched or not  */
    uint32                      touched;
} ETH_CSL_CPSW_ALE_OUIADDR_ENTRY;

/** @brief
 *
 *  Holds the ALE VLAN Unicast Address Table entry
 *  configuration.
 */
typedef struct {
    /**  Unicast address */
    uint8                       macAddress [6];

    /**  VLAN Id  */
    uint32                      vlanId;

    /**  Entry ageable or not  */
    uint32                      ageable;

    /**  Entry touched or not  */
    uint32                      touched;

    /**  Secure bit enable?  */
    uint32                      secureEnable;

    /**  Block bit enable? */
    uint32                      blockEnable;

    /**  Port Number to forward matching packets to. */
    uint32                      portNumber;
    /** Flag indicating if port is part of trunk group */
    uint32                      trunkFlag;
} ETH_CSL_CPSW_ALE_VLANUNICASTADDR_ENTRY;

/** @brief
 *
 *  Holds the ALE (Inner) VLAN Table entry configuration.
 */
typedef struct {
    /**  VLAN Id  */
    uint32                      vlanId;

    /**  VLAN member list */
    uint32                      vlanMemList;

    /**  Unregistered Multicast Flood mask  */
    uint32                      unRegMcastFloodIndex;

    /**  Registered Multicast Flood mask  */
    uint32                      regMcastFloodIndex;
    /**  Unregistered Multicast Flood mask  */
    uint32                      unRegMcastFloodMask;

    /**  Registered Multicast Flood mask  */
    uint32                      regMcastFloodMask;
    /**  Force Untagged Packet Egress. */
    uint32                      forceUntaggedEgress;

    /**  enable check for vlan member list at ingress. */
    uint32                      ingressCheckFlag;

    /**  Port Mask to disable learning on specified port */
    uint32                      noLearnMask;

    /**  enable limit IP nxt header */
    uint32                      limitIPNxtHdr;

    /**  Drop fragmented IP packets */
    uint32                      disallowIPFragmentation;

} ETH_CSL_CPSW_ALE_VLAN_ENTRY;

/** @brief
 *
 *  Holds the ALE Outer VLAN Table entry configuration.
 */
typedef ETH_CSL_CPSW_ALE_VLAN_ENTRY ETH_CSL_CPSW_ALE_OUTER_VLAN_ENTRY;

/** @brief
 *
 *  Holds the ALE Ethertype Table entry configuration.
 */
typedef struct {
    /**  Ethernet Type  */
    uint32                      ethertype;

} ETH_CSL_CPSW_ALE_ETHERTYPE_ENTRY;


/** @brief
 *
 *  Holds the ALE IPv4 Address Table entry
 *  configuration.
 */
typedef struct {
    /**  IPv4 address */
    uint8                       address [4];
    /**  CIDR Mask: Number of least significant bits to ignore in address match */
    uint32                      numLSBIgnore;

} ETH_CSL_CPSW_ALE_IPv4_ENTRY;

/** @brief
 *
 *  Holds the ALE IPv6 Address Table entry
 *  configuration.
 */
typedef struct {
    /**  IPv6 address */
    uint8                       address [16];
    /**  CIDR Mask: Number of least significant bits to ignore in address match */
    uint32                      numLSBIgnore;

} ETH_CSL_CPSW_ALE_IPv6_ENTRY;


#define ETH_CSL_CPSW_NUMALE_ENTRIES_MIN (64)

#define ETH_CSL_CPSW_NUMSTATBLOCKS      (9)



/** @brief Number of statistic blocks.
 *
 *  EMAC has multiple statistics blocks.
 *
 *  STATS0 holds statistics for Host/CPU port (Switch port 0).
 *  STATSn holds statistics for MAC ports (Switch ports n).
 */


/** @brief
 *
 *  Defines ALE RAMDEPTH enums for storing IPv6 sliceIndex entry
 */
typedef enum {
    ETH_CSL_ALE_RAMDEPTH_32 = 0,  /** RAMDEPTH is 32  */
    ETH_CSL_ALE_RAMDEPTH_64 = 1,  /** RAMDEPTH is 32  */
    ETH_CSL_ALE_RAMDEPTH_128 = 2,  /** RAMDEPTH is 32  */
} ETH_CSL_CPSW_ALE_RAMDEPTH_E;

/** @brief
 *
 *  Defines ALE Aging Timer Prescale
 */
typedef enum {
    ETH_CSL_ALE_AGT_PRESACLE_1M = 0,  /** 1000,000 (default value) */
    ETH_CSL_ALE_AGT_PRESACLE_1000,    /** 1000 (test value) */
    ETH_CSL_ALE_AGT_PRESACLE_1        /** 1 (test value) */
} ETH_CSL_CPSW_ALE_AGT_PRESCALE_E;

/** @brief
 *
 *  Holds Port Statistics Enable register contents.
 */
typedef struct {
    /**  Port 0 Statistics Enable bit */
    uint32      p0StatEnable;

    /**  Port 1 Statistics Enable bit */
    uint32      p1StatEnable;

    /**  Port 2 Statistics Enable bit */
    uint32      p2StatEnable;

    /**  Port 3 Statistics Enable bit */
    uint32      p3StatEnable;

    /**  Port 4 Statistics Enable bit */
    uint32      p4StatEnable;

    /**  Port 5 Statistics Enable bit */
    uint32      p5StatEnable;

    /**  Port 6 Statistics Enable bit */
    uint32      p6StatEnable;

    /**  Port 7 Statistics Enable bit */
    uint32      p7StatEnable;

    /**  Port 8 Statistics Enable bit */
    uint32      p8StatEnable;

} ETH_CSL_CPSW_PORTSTAT;

/** @brief
 *
 *  Holds Port Time Sync Control register contents.
 */
typedef struct {
    /**  Port Time sync receive Annex D enable bit */
    uint32      tsRxAnnexDEnable;

    /**  Port Time sync receive Annex E enable bit */
    uint32      tsRxAnnexEEnable;

    /**  Port Time sync receive Annex F enable bit */
    uint32      tsRxAnnexFEnable;

    /**  Port Time sync receive VLAN LTYPE 1 enable bit */
    uint32      tsRxVlanLType1Enable;

    /**  Port Time sync receive VLAN LTYPE 2 enable bit */
    uint32      tsRxVlanLType2Enable;

    /**  Port Time sync transmit Annex D enable bit */
    uint32      tsTxAnnexDEnable;

    /**  Port Time sync transmit Annex E enable bit */
    uint32      tsTxAnnexEEnable;

    /**  Port Time sync transmit Annex F enable bit */
    uint32      tsTxAnnexFEnable;

    /**  Port Time sync transmit VLAN LTYPE 1 enable bit */
    uint32      tsTxVlanLType1Enable;

    /**  Port Time sync transmit VLAN LTYPE 2 enable bit */
    uint32      tsTxVlanLType2Enable;

    /**  Port Time sync transmit host timestamp enable bit */
    uint32      tsTxHostEnable;

    /**  Port Time sync transmit and receive LType2 enable bit */
    uint32      tsLType2Enable;

    /**  Port Time sync message type enable bits */
    uint32      tsMsgTypeEnable;

} ETH_CSL_CPSW_TSCNTL;


/** @brief
 *
 *  Holds Port Time Sync Configuration contents.
 */
typedef struct {
    /**  Port Time sync receive Annex D enable */
    uint32      tsRxAnnexDEnable;

    /**  Port Time sync receive Annex E enable */
    uint32      tsRxAnnexEEnable;

    /**  Port Time sync receive Annex F enable */
    uint32      tsRxAnnexFEnable;

    /**  Port Time sync receive VLAN LTYPE 1 enable */
    uint32      tsRxVlanLType1Enable;

    /**  Port Time sync receive VLAN LTYPE 2 enable */
    uint32      tsRxVlanLType2Enable;

    /**  Port Time sync transmit Annex D enable */
    uint32      tsTxAnnexDEnable;

    /**  Port Time sync transmit Annex E enable */
    uint32      tsTxAnnexEEnable;

    /**  Port Time sync transmit Annex F enable */
    uint32      tsTxAnnexFEnable;

    /**  Port Time sync transmit VLAN LTYPE 1 enable */
    uint32      tsTxVlanLType1Enable;

    /**  Port Time sync transmit VLAN LTYPE 2 enable */
    uint32      tsTxVlanLType2Enable;

    /**  Port Time sync transmit host timestamp enable */
    uint32      tsTxHostEnable;

    /**  Port Time sync transmit and receive LType2 enable */
    uint32      tsLType2Enable;

    /**  Port Time sync message type enable bitmap */
    uint32      tsMsgTypeEnable;

    /**  Port Time Sync Unicast IP message enable
         1: all IP address valid
         0: perform multicast address check, only multicast addresses which are enabled are valid
      */
    uint32      tsUniEnable;

    /**  Port Time Sync Destination IP Address 107 enable
         IPv4: 224.0.0.107
         IPv6: FF0M:0:0:0:0:0:0:006B
     */
    uint32      ts107Enable;

    /**  Port Time Sync Destination IP Address 129 enable
         IPv4: 224.0.0.129
         IPv6: FF0M:0:0:0:0:0:0:0181
     */
    uint32      ts129Enable;

    /**  Port Time Sync Destination IP Address 130 enable
         IPv4: 224.0.0.130
         IPv6: FF0M:0:0:0:0:0:0:0182
     */
    uint32      ts130Enable;

    /**  Port Time Sync Destination IP Address 131 enable
         IPv4: 224.0.0.130
         IPv6: FF0M:0:0:0:0:0:0:0183
     */
    uint32      ts131Enable;

    /**  Port Time Sync Destination IP Address 132 enable
         IPv4: 224.0.0.131
         IPv6: FF0M:0:0:0:0:0:0:0184
     */
    uint32      ts132Enable;

    /**  Port Time Sync UDP Destination Port Number 319 enable */
    uint32      ts319Enable;

    /**  Port Time Sync UDP Destination Port Number 320 enable */
    uint32      ts320Enable;

    /**  Port Time sync message IPv6 Multicast Address FF0M enable */
    uint32      tsMcastTypeEnable;

    /**  Port Time Sync Time to Live Non-zero enable */
    uint32      tsTTLNonzeroEnable;

    /**  Port Time Sync LTYPE1 value for Annex F packets */
    uint32      tsLType1;

    /**  Port Time Sync LTYPE2 value for Annex F packets */
    uint32      tsLType2;

    /**  Port Time Sync VLAN LTYPE1 value for both Tx and Rx packets */
    uint32      tsVlanLType1;

    /**  Port Time Sync VLAN LTYPE2 value for both Tx and Rx packets */
    uint32      tsVlanLType2;

    /**  Port Time Sync Sequence ID field byte Offset in PTP message */
    uint32      tsSeqIdOffset;

    /**  Port Time Sync Domain field byte Offset in PTP message */
    uint32      tsDomainOffset;

} ETH_CSL_CPSW_TSCONFIG;


/** @brief
 *
 *  Holds CPSW Port Control contents.
 */
typedef struct {
    /**  Eneregy Efficient Etherent (EEE) Transmit LPI clockstop enable
         for EMAC port only
         1: The GMII or RGMII transmit clock is stopped in the EEE
            LPI state.
         0: The GMII or RGMII transmit clock is not stopped in the
            EEE LPI state.
      */
    uint32      txLpiClkstopEnable;

    /**  IPv6 DSCP to priority mapping enable */
    uint32      dscpIpv6Enable;

    /**  IPv4 DSCP to priority mapping enable */
    uint32      dscpIpv4Enable;

} ETH_CSL_CPSW_PORT_CONTROL;


/** @brief
 *
 *  Holds CPSW Port Rx Rate Limit Configuration for CPPI Port Ingress Rate Limitaion Operation.
 */
typedef struct {
    /**  Number of Rate Limitaion Channels. Rate limited channels must be the highest priority channels.
         For example, if two rate limited channels are required then they must be channel with priority
         7 and 6 respectively. The BW of rate limitation channel is calcualted as
         idleStep/(idleStep + sendStep)*Frequemcy*256 where frequency = the VBUSP_GCLK frequency (350 for 350Mhz)
      */
    uint32      numRLimChans;

    /**  Rate Limitaion Idle Step Array */
    uint32      idleStep[8];

    /**  Rate Limitaion Send Step Array  */
    uint32      sendStep[8];

} ETH_CSL_CPSW_RX_RATE_LIMIT_CONFIG;

/** @brief
 *
 *  Holds CPSW EEE (Energy Efficient Ethernet)  Global Configuration.
 */
typedef struct {
    /**  Energy Efficient Ethernet enable */
    uint32      enable;

    /**  Energy Efficient Ethernet Pre-scale count load value */
    uint32      prescale;

} ETH_CSL_CPSW_EEE_GLOB_CONFIG;

/** @brief
 *
 *  Holds CPSW EEE (Energy Efficient Ethernet)  Per-Port Configuration.
 */
typedef struct {
    /**  EEE Transmit LPI clockstop enable
         for EMAC port only
         1: The GMII or RGMII transmit clock is stopped in the EEE
            LPI state.
         0: The GMII or RGMII transmit clock is not stopped in the
            EEE LPI state.
      */
    uint32      txLpiClkstopEnable;

    /**  EEE Idle to LPI counter load value - After EEE_CLKSTOP_REQ is asserted,
         this value is loaded into the port idle to LPI counter on each clock
         that the port transmit or receive is not idle.  Port enters the LPI
         state when this count decrements to zero.
         This count value should be large relative to switch operations */
    uint32      idle2lpi;

    /**  EEE LPI to wake counter load value - When the port is in the transmit
         LPI state and the EEE_CLKSTOP_REQ signal is deasserted, this value is
         loaded into the port 0 LPI to wake counter.
         Transmit and receive packet operations may begin (resume) when
         the LPI to wake count decrements to zero.  This is the time waited
         before CPPI packet operations begin (resume after wakeup).
         This count value should be large relative to switch operations. */
    uint32      lpi2wake;

} ETH_CSL_CPSW_EEE_PORT_CONFIG;


/** @brief
 *
 *  Holds CPSW EEE (Energy Efficient Ethernet)  Per-Port STATUS.
 */
typedef struct {
    /**  CPSW port wait idle to LPI - asserted when port is counting the IDLE2LPI time */
    uint32      wait_idle2lpi;

    /**  CPSW port receive LPI state - asserted when the port receive is in the LPI state */
    uint32      rxLpi;

    /**  CPSW port transmit LPI state - asserted when the port transmit is in the LPI state */
    uint32      txLpi;

    /**  CPSW port transmit wakeup - asserted when the port transmit is in the transmit
         LPI2WAKE count time. */
    uint32      txWake;

    /**  CPSW port transmit FIFO hold - asserted in the LPI state and
         during the LPI2WAKE count time. */
    uint32      txFifoHold;

    /** CPSW port receive FIFO (switch ingress) is empty - contains no packets */
    uint32      rxFifoEmpty;

    /** CPSW port transmit FIFO (switch egress) is empty - contains no packets */
    uint32      txFifoEmpty;

} ETH_CSL_CPSW_EEE_PORT_STATUS;

/** @brief
 *
 *  Defines ALE Update Bandwidth Control Value:
 *  The upd_bw_ctrl field within ALE control register specifies the rate in which adds,
 *  updates, touches, writes, and aging updates can occur. At frequencies of 350Mhz,
 *  the table update rate should be at its lowest or 5 Million updates per second.
 *  When operating the switch core at frequencies or above, the upd_bw_ctrl can be
 *  programmed more aggressive.
 *  If the upd_bw_ctrl is set but the frequency of the switch subsystem is below the
 *  associated value, ALE will drop packets due to insufficient time to complete
 *  lookup under high traffic loads.
 */
typedef enum {
    ETH_CSL_ALE_UPD_BW_350MHZ_5M = 0,           /** 0 - 350Mhz, 5M */
    ETH_CSL_ALE_UPD_BW_359MHZ_11M,              /** 1 - 359Mhz, 11M */
    ETH_CSL_ALE_UPD_BW_367MHZ_16M,              /** 2 - 367Mhz, 16M */
    ETH_CSL_ALE_UPD_BW_375MHZ_22M,              /** 3 - 375Mhz, 22M */
    ETH_CSL_ALE_UPD_BW_384MHZ_28M,              /** 4 - 384Mhz, 28M */
    ETH_CSL_ALE_UPD_BW_392MHZ_34M,              /** 5 - 392Mhz, 34M */
    ETH_CSL_ALE_UPD_BW_400MHZ_39M,              /** 6 - 400Mhz, 39M */
    ETH_CSL_ALE_UPD_BW_409MHZ_45M               /** 7 - 409Mhz, 45M */
} ETH_CSL_CPSW_ALE_UPD_BW;

/** @brief
 *
 *  Holds CPSW Policer  Global Configuration.
 */
typedef struct {
    /** ALE Policer Default Thread Enable:
        Set: the aleDeafultThread is used for the CPPI transmit thread
             if there are no matching classifiers.
        Clear, the switch default thread is used for the CPPI transmit thread i
        f there are no matching classifiers.
     */

    uint32      defThreadEnable;

    /**  ALE Default Thread */
    uint32      defThread;

} ETH_CSL_CPSW_ALE_POLICER_GLOB_CONFIG;

typedef struct {
    uint32      trunkBase;
    boolean     trunkEnableDestIP;
    boolean     trunkEnableSrcIP;
    boolean     trunkEnableInnerVLAN;
    boolean     trunkEnablePri;
    boolean     trunkEnableSrc;
    boolean     trunkEnableDst;
} ETH_CSL_CPSW_ALE_CTRL2_TRUNK_CONFIG;


typedef struct {
    boolean     ipPktFltEnableDefNxtHdrLimit;
    boolean     ipPktFltEnableDefNoFrag;
} ETH_CSL_CPSW_ALE_CTRL2_IPPKTFLT_CONFIG;

typedef struct {
    boolean     dropBadLen;
    boolean     noDropSrcMcast;
} ETH_CSL_CPSW_ALE_CTRL2_MALFORMEDFRAME_CONFIG;

#define CSL_ALE_TABLE_POLICER_ENUM2REG(policerType) ((policerType) << 0x1)


/** @brief ALE Policer Entry configuration definitions */
/**  Input EMAC port is used for classification */
#define    ETH_CSL_CPSW_ALE_POLICER_PORT_VALID       (((uint32)1u) << 0u)

/**  VLAN Priority is used for classification */
#define    ETH_CSL_CPSW_ALE_POLICER_PRI_VALID        (((uint32)1u) << 1u)

/**  OUI is used for classification */
#define    ETH_CSL_CPSW_ALE_POLICER_OUI_VALID        (((uint32)1u) << 2u)

/**  Destination MAC is used for classification */
#define    ETH_CSL_CPSW_ALE_POLICER_DST_MAC_VALID    (((uint32)1u) << 3u)

/**  Source MAC is used for classification */
#define    ETH_CSL_CPSW_ALE_POLICER_SRC_MAC_VALID    (((uint32)1u) << 4u)

/**  Outer VLAN ID is used for classification */
#define    ETH_CSL_CPSW_ALE_POLICER_OVLAN_VALID      (((uint32)1u) << 5u)

/**  (Inner) VLAN ID is used for classification */
#define    ETH_CSL_CPSW_ALE_POLICER_VLAN_VALID       (((uint32)1u) << 6u)

/**  Ethertype is used for classification */
#define    ETH_CSL_CPSW_ALE_POLICER_ETHERTYPE_VALID  (((uint32)1u) << 7u)

/**  Source IP address is used for classification */
#define    ETH_CSL_CPSW_ALE_POLICER_SRC_IP_VALID     (((uint32)1u) << 8u)

/**  Destination IP address is used for classification */
#define    ETH_CSL_CPSW_ALE_POLICER_DST_IP_VALID     (((uint32)1u) << 9u)

/**  The specified thread value is used as the CPPI egress thread
     for any packet that matches the classifier  */
#define    ETH_CSL_CPSW_ALE_POLICER_THREAD_VALID     (((uint32)1u) << 10u)

/**  The specified pir_idl_inc value is used  */
#define    ETH_CSL_CPSW_ALE_POLICER_PIR_VALID        (((uint32)1u) << 11u)

/**  The specified cir_idl_inc value is used  */
#define    ETH_CSL_CPSW_ALE_POLICER_CIR_VALID        (((uint32)1u) << 12u)

/**  Input EMAC port is used for classification */
#define    ETH_CSL_CPSW_ALE_POLICER_PORT_TRUNK_VALID (((uint32)1u) << 13u)

/**  The specified egress op is used */
#define    ETH_CSL_CPSW_ALE_POLICER_EGRESSOP_VALID   (((uint32)1u) << 14u)

/** @brief
 *
 *  Holds the ALE Policer Table entry configuration.
 */
typedef struct {
    /**  Configuration control bitmap as defined above */
    uint32                      validBitmap;

    /**  Ingress EMAC port */
    uint32                      port;

    /**  VLAN priority */
    uint32                      pri;

    /**  OUI entry index  */
    uint32                      ouiIdx;

    /**  Destination MAC address entry index  */
    uint32                      dstMacIdx;

    /**  Source MAC address entry index  */
    uint32                      srcMacIdx;

    /**  Outer VLAN entry index  */
    uint32                      ovlanIdx;

    /**  (Inner) VLAN entry index  */
    uint32                      vlanIdx;

    /**  Ethertype entry index  */
    uint32                      ethertypeIdx;

    /**  Source IP address entry index  */
    uint32                      srcIpIdx;

    /**  Destination IP address entry index  */
    uint32                      dstIpIdx;

    /**  CPPI Egress thread upon match  */
    uint32                      thread;

    /** Peak Information Rate Idle Increment Value
      * The number added to the PIR counter every clock cycle. If zero the PIR
      * counter is disabled and packets will never be marked or
      * processed as RED  */
    uint32                      pirIdleIncVal;

    /** Committed Information Rate Idle Increment Value
      * The number added to the CIR counter every clock cycle. If zero the CIR
      * counter is disabled and packets will never be marked or
      * processed as YELLOW  */
    uint32                      cirIdleIncVal;
    /** Egress Op  Value
      * The Egress Operation value allows enabled classifiers with IPSA or IPDA
      * match to use the CPSW Egress Packet Operations Inter VLAN Routing
      * sub functions
      */
    uint32                      egressOp;
    /** Egress trunk index
      * The Egress Trunk Index is the calculated trunk index from the SA,
      * DA or VLAN. If modified to that, InterVLAN routing will work on trunks
      * as well. The DA, SA and VLAN are ignored for trunk generation on
      * InterVLAN Routing so that this field is the index generated from the
      * Egress Op replacements XORed together into a three bit
      * index.
      */
    uint32                      egressTrunkIndex;
    /** TTL check value
     * The TTL Check will cause any packet that fails TTL checks to not
     * be routed to the Inter VLAN Routing sub functions. The packet will
     * be routed to the host it was destined to.
     */
    uint32                      enableTTLCheck;
    /** The Destination Ports is a list of the ports the classified packet will
      * be set to. If a destination is a Trunk, all the port bits for that trunck
      * must be set.
      */
    uint32                      destPortMask;

} ETH_CSL_CPSW_ALE_POLICER_ENTRY;

/** @brief
 *
 *  Holds the EMAC statistics.
 *
 *  The statistics structure is the used to retrieve the current count
 *  of various packet events in the system. These values represent the
 *  delta values from the last time the statistics were read.
 */
typedef struct {
    /** Good Frames Received                      */
    uint32      RxGoodFrames;

    /** Good Broadcast Frames Received            */
    uint32      RxBCastFrames;

    /** Good Multicast Frames Received            */
    uint32      RxMCastFrames;

    /** PauseRx Frames Received                   */
    uint32      RxPauseFrames;

    /** Frames Received with CRC Errors           */
    uint32      RxCRCErrors;

    /** Frames Received with Alignment/Code Errors*/
    uint32      RxAlignCodeErrors;

    /** Oversized Frames Received                 */
    uint32      RxOversized;

    /** Jabber Frames Received                    */
    uint32      RxJabber;

    /** Undersized Frames Received                */
    uint32      RxUndersized;

    /** Rx Frame Fragments Received               */
    uint32      RxFragments;

    /** Rx frames dropped by the ALE              */
    uint32      RxAleDrop;

    /** Rx overrun frames dropped by the ALE      */
    uint32      RxAleOverrunDrop;

    /** Total Received Bytes in Good Frames       */
    uint32      RxOctets;

    /** Good Frames Sent                          */
    uint32      TxGoodFrames;

    /** Good Broadcast Frames Sent                */
    uint32      TxBCastFrames;

    /** Good Multicast Frames Sent                */
    uint32      TxMCastFrames;

    /** PauseTx Frames Sent                       */
    uint32      TxPauseFrames;

    /** Frames Where Transmission was Deferred    */
    uint32      TxDeferred;

    /** Total Frames Sent With Collision          */
    uint32      TxCollision;

    /** Frames Sent with Exactly One Collision    */
    uint32      TxSingleColl;

    /** Frames Sent with Multiple Colisions       */
    uint32      TxMultiColl;

    /** Tx Frames Lost Due to Excessive Collisions*/
    uint32      TxExcessiveColl;

    /** Tx Frames Lost Due to a Late Collision    */
    uint32      TxLateColl;

    /** Rx inter-packet gap errors (10G only)     */
    uint32      RxIpgError;

    /** Tx Frames Lost Due to Carrier Sense Loss  */
    uint32      TxCarrierSLoss;

    /** Total Transmitted Bytes in Good Frames    */
    uint32      TxOctets;

    /** Total Tx&Rx with Octet Size of 64         */
    uint32      Frame64;

    /** Total Tx&Rx with Octet Size of 65 to 127  */
    uint32      Frame65t127;

    /** Total Tx&Rx with Octet Size of 128 to 255 */
    uint32      Frame128t255;

    /** Total Tx&Rx with Octet Size of 256 to 511 */
    uint32      Frame256t511;

    /** Total Tx&Rx with Octet Size of 512 to 1023*/
    uint32      Frame512t1023;

    /** Total Tx&Rx with Octet Size of >=1024     */
    uint32      Frame1024tUp;

    /** Sum of all Octets Tx or Rx on the Network */
    uint32      NetOctets;

    /** Total Rx bottom of FIFO dropped frames    */
    uint32      RxDropBottom;

    /** Total dropped frames due to portmask      */
    uint32      PortmaskFrop;

    /** Total Rx top of FIFO dropped frames       */
    uint32      RxDropTop;

    /** Total dropped frames due to ALE Rate Limiting */
    uint32      AleRateLimitDrop;

    /** Total dropped frames due to ALE VID Ingress   */
    uint32      AleVidDrop;

    /** Total dropped frames due to DA=SA             */
    uint32      AleAddrEqDrop;

    /** Unused Statistics registers                   */
    uint32      Resv1[3];

    /** Total ALE Unknown Unicast frames              */
    uint32      AleUnKnUni;

    /** Total ALE Unknown Unicast byte count          */
    uint32      AleUnKnUniBytes;

    /** Total ALE Unknown Multicast frames            */
    uint32      AleUnKnMulti;

    /** Total ALE Unknown Multicast byte count        */
    uint32      AleUnKnMultiBytes;

    /** Total ALE Unknown Broadcast frames            */
    uint32      AleUnKnBCast;

    /** Total ALE Unknown Broadcast byte count        */
    uint32      AleUnKnBCastBytes;

    /** Total ALE Policer Match frames                */
    uint32      AlePolMatch;

    /** Unused Statistics registers                   */
    uint32      Resv2[46];

    /** Total Tx Memory Protect CRC Error */
    uint32      TxMemProtectErr;

    /** Tx Priority [0-7] Packet Count */
    uint32      TxPriPktCnt[8];

    /** Tx Priority [0-7] Byte Count */
    uint32      TxPriByteCnt[8];

    /** Tx Priority [0-7] Drop packet Count */
    uint32      TxPriDropPktCnt[8];

    /** Tx Priority [0-7] Drop byte Count */
    uint32      TxPriDropByteCnt[8];

} ETH_CSL_CPSW_STATS;

typedef enum {
    ETH_CSL_ALE_POLICER_CONTROL_POLICING_MATCH_MODE_NOMATCH_GREEN,
    ETH_CSL_ALE_POLICER_CONTROL_POLICING_MATCH_MODE_NOMATCH_YELLOW,
    ETH_CSL_ALE_POLICER_CONTROL_POLICING_MATCH_MODE_NOMATCH_RED,
    ETH_CSL_ALE_POLICER_CONTROL_POLICING_MATCH_MODE_NOMATCH_ENTRY0STATE,
} ETH_CSL_CPSW_ALE_POLICER_CONTROL_POLICING_MATCH_MODE;

typedef enum {
    ETH_CSL_ALE_POLICER_CONTROL_YELLOWTHRESH_DROP_PERCENT_100,
    ETH_CSL_ALE_POLICER_CONTROL_YELLOWTHRESH_DROP_PERCENT_50,
    ETH_CSL_ALE_POLICER_CONTROL_YELLOWTHRESH_DROP_PERCENT_33,
    ETH_CSL_ALE_POLICER_CONTROL_YELLOWTHRESH_DROP_PERCENT_25,
    ETH_CSL_ALE_POLICER_CONTROL_YELLOWTHRESH_DROP_PERCENT_20,
    ETH_CSL_ALE_POLICER_CONTROL_YELLOWTHRESH_DROP_PERCENT_17,
    ETH_CSL_ALE_POLICER_CONTROL_YELLOWTHRESH_DROP_PERCENT_14,
    ETH_CSL_ALE_POLICER_CONTROL_YELLOWTHRESH_DROP_PERCENT_13,
} ETH_CSL_CPSW_ALE_POLICER_CONTROL_YELLOWTHRESH;

typedef struct
{
    ETH_CSL_CPSW_ALE_POLICER_CONTROL_POLICING_MATCH_MODE policeMatchMode;
    ETH_CSL_CPSW_ALE_POLICER_CONTROL_YELLOWTHRESH yellowDropThresh;
    uint32 yellowDropEnable;
    uint32 redDropEnable;
    uint32 policingEnable;
    uint32 enablePriorityOR;
    uint32 disableMacPortDefaultThread;
} ETH_CSL_CPSW_ALE_POLICER_CONTROL;

typedef struct
{
    uint32 polClrallHit;
    uint32 polClrallRedhit;
    uint32 polClrallYellowhit;
    uint32 polClrselAll;
    uint32 polTestIdx;
} ETH_CSL_CPSW_ALE_POLICER_TEST_CONTROL;

typedef struct
{
    uint32  polHit        ;
    uint32  polRedhit     ;
    uint32  polYellowhit  ;
} ETH_CSL_CPSW_ALE_POLICER_HSTAT;

/** @brief
 *
 *  Holds CPSW priority type register contents.
 */
typedef struct {
    /**  Port 0 Priority Escalation Enable bit */
    uint32      port0PriorityTypeEscalateEnable;

    /**  Port 1 Priority Escalation Enable bit */
    uint32      port1PriorityTypeEscalateEnable;

    /**  Port 2 Priority Escalation Enable bit */
    uint32      port2PriorityTypeEscalateEnable;

    /**  Port 3 Priority Escalation Enable bit */
    uint32      port3PriorityTypeEscalateEnable;

    /**  Port 4 Priority Escalation Enable bit */
    uint32      port4PriorityTypeEscalateEnable;

    /**  Port 5 Priority Escalation Enable bit */
    uint32      port5PriorityTypeEscalateEnable;

    /**  Port 6 Priority Escalation Enable bit */
    uint32      port6PriorityTypeEscalateEnable;

    /**  Port 7 Priority Escalation Enable bit */
    uint32      port7PriorityTypeEscalateEnable;

    /**  Port 8 Priority Escalation Enable bit */
    uint32      port8PriorityTypeEscalateEnable;

    /** Escalate priority load value
      * When a port is in escalate priority, this is the number of
      * higher priority packets sent before the next lower priority
      * is allowed to send a packet.
      * Escalate priority allows lower priority packets to be sent at a fixed rate
      * relative to the next higher priority.
      * The min value of esc_pri_ld_val = 2
      */
    uint32      escPriLoadVal;
} ETH_CSL_CPSW_PTYPE;

/** @brief
 *
 *  Holds the Port intervlan configuration info.
 */
typedef struct {
    /**  Destination mac address to be replaced */
    uint8                       dstMacAddress [6];
    /**  Source mac address to be replaced */
    uint8                       srcMacAddress [6];
    /** Decrement Time To Live -
      * When set, the Time To Live (TTL) field in the header is decremented:
      *  IPV4 - Decrement the TTL byte and update the Header Checksum
      *  IPV6 - Decrement the Hop Limit.
      *  note: When this bit is set, the ALE should be configured to send any
      *  IPv4/6 packet with a zero or one TTL field to the host.
      *  When this bit is cleared the TTL/Hop Limit fields are not checked or
      *  modified.
      */
    uint32                      decrementTtl;
    /** Destination VLAN Force Untagged Egress - When set, this bit indicates
      * that the VLAN should be removed on egress for the routed packet.
      */
    uint32                      destForceUntaggedEgress;
    /** Replace Destination Address and Source Address - When set this bit
      * indicates that the routed packet destination address should be replaced
      * by da[47:0] and the source address should be replaced by sa[47:0]
      */
    uint32                      replaceDaSa;
    /** Replace VLAN ID - When set this bit indicates that the VLAN ID
      * should be replaced for the routed packet
      */
    uint32                      replaceVid;
    /** VLAN ID to be set on outgoing packets */
    uint32                      vid;
} ETH_CSL_CPSW_INTERVLANCFG;

/** @brief
 *
 *  Holds the Enet_Pn_FIFO_Status register contents
 */
typedef struct {
    /** EST RAM active buffer  .
      * Indicates the active 64-word fetch buffer when pn_est_onebuf is
      * cleared to zero.
      * Indicates the fetch ram address MSB when pn_est_onebuf set to one.
      */
    uint32 estBufAct;
    /** EST Address Error
      * Indicates that the fetch ram was read again after the previous maximum
      * buffer address read (the previous fetch from the maximum address is
      * reused).
      */
    uint32 estAddErr;
    /** EST Fetch Count Error
      * Indicates that insufficient clocks were programmed into the fetch count
      * and that another fetch was commanded before the previous fetch
      * finished.
      */
    uint32 estCntErr;
    /** EST transmit mac allow
      * Bus that indicates the actual priorities assigned to the express queue
      * (and inversely the priorities assigned to the prempt queue).
      * The pn_mac_prempt[7:0] field in the Enet_Pn_IET_Control register
      * indicates which priorities should be assigned to the express/prempt
      * queues.  The switch between queues happens only when the priority is
      * empty and the actual assignment is shown in this field.
      */
    uint32 txExpressMacAllow;
    /** EST Transmit Priority Active
      * Bus that indicates which priorities have packets (non-empty) at the
      * time of the register read.
      */
    uint32 txPriActive;
} CSL_CPGMAC_SL_FIFOSTATUS;



/**
@}
*/


/** @addtogroup CSL_CPSWITCH_FUNCTION
@{ */

/********************************************************************************
************************* Ethernet Switch (CPSW) Submodule **********************
********************************************************************************/


/** ============================================================================
 *   @n@b ETH_CSL_CPSW_nGF_getCpswVersionInfo
 *
 *   @b Description
 *   @n This function retrieves the CPSW identification and version information.
 *
 *   @b Arguments
     @verbatim
        pVersionInfo        ETH_CSL_CPSW_VERSION structure that needs to be populated
                            with the version info read from the hardware.
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Reads
 *   @n XGE_CPSW_CPSW_ID_VER_REG_CPSW_5GF_MINOR_VER,
 *      XGE_CPSW_CPSW_ID_VER_REG_CPSW_5GF_MAJ_VER,
 *      XGE_CPSW_CPSW_ID_VER_REG_CPSW_5GF_RTL_VER,
 *      XGE_CPSW_CPSW_ID_VER_REG_CPSW_5GF_IDENT
 *
 *   @b Example
 *   @verbatim
        ETH_CSL_CPSW_VERSION    versionInfo;

        ETH_CSL_CPSW_getCpswVersionInfo (&versionInfo);

     @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_getCpswVersionInfo (CSL_Xge_cpswRegs *hCpswRegs,
    ETH_CSL_CPSW_VERSION*       pVersionInfo
);


/** ============================================================================
 *   @n@b ETH_CSL_CPSW_enablePort0
 *
 *   @b Description
 *   @n This function configures the CPSW control register to enable the Port 0.
 *
 *   @b Arguments
 *   @n None
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n XGE_CPSW_CONTROL_REG_P0_ENABLE=1
 *
 *   @b Example
 *   @verbatim

        ETH_CSL_CPSW_enablePort0 ();

     @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_enablePort0 (CSL_Xge_cpswRegs *hCpswRegs);


/** ============================================================================
 *   @n@b ETH_CSL_CPSW_disablePort0
 *
 *   @b Description
 *   @n This function configures the CPSW control register to disable the Port 0.
 *
 *   @b Arguments
 *   @n None
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n XGE_CPSW_CONTROL_REG_P0_ENABLE=0
 *
 *   @b Example
 *   @verbatim

        ETH_CSL_CPSW_disablePort0 ();

     @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_disablePort0 (CSL_Xge_cpswRegs *hCpswRegs);


/** ============================================================================
 *   @n@b ETH_CSL_CPSW_getCpswControlReg
 *
 *   @b Description
 *   @n This function retrieves the contents of the CPSW Control register.
 *
 *   @b Arguments
     @verbatim
        pControlRegInfo     ETH_CSL_CPSW_CONTROL structure that needs to be populated
                            with the control register contents.
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Reads
 *   @n XGE_CPSW_CONTROL_REG_VLAN_AWARE,
 *      XGE_CPSW_CONTROL_REG_P0_ENABLE,
 *      XGE_CPSW_CONTROL_REG_P0_PASS_PRI_TAGGED,
 *      XGE_CPSW_CONTROL_REG_P1_PASS_PRI_TAGGED,
 *      XGE_CPSW_CONTROL_REG_P0_RX_PAD,
 *      XGE_CPSW_CONTROL_REG_P0_RX_PASS_CRC_ERR,
 *      XGE_CPSW_CONTROL_REG_EEE_ENABLE
 *
 *
 *   @b Example
 *   @verbatim
        ETH_CSL_CPSW_CONTROL    controlRegInfo;

        ETH_CSL_CPSW_getCpswControlReg (&controlRegInfo);

     @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_getCpswControlReg (CSL_Xge_cpswRegs *hCpswRegs,
    ETH_CSL_CPSW_CONTROL*   pControlRegInfo
);


/** ============================================================================
 *   @n@b ETH_CSL_CPSW_setCpswControlReg
 *
 *   @b Description
 *   @n This function populates the contents of the CPSW Control register.
 *
 *   @b Arguments
     @verbatim
        pControlRegInfo     ETH_CSL_CPSW_CONTROL structure that holds the values
                            that need to be configured to the CPSW control register.
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  CPSW control register modified with values provided.
 *
 *   @b Writes
 *   @n XGE_CPSW_CONTROL_REG_VLAN_AWARE,
 *      XGE_CPSW_CONTROL_REG_P0_ENABLE,
 *      XGE_CPSW_CONTROL_REG_P0_PASS_PRI_TAGGED,
 *      XGE_CPSW_CONTROL_REG_P1_PASS_PRI_TAGGED,
 *      XGE_CPSW_CONTROL_REG_P1_PASS_PRI_TAGGED,
 *      XGE_CPSW_CONTROL_REG_P0_TX_CRC_REMOVE,
 *      XGE_CPSW_CONTROL_REG_P0_RX_PAD,
 *      XGE_CPSW_CONTROL_REG_P0_RX_PASS_CRC_ERR,
 *      XGE_CPSW_CONTROL_REG_EEE_ENABLE
 *
 *   @b Example
 *   @verbatim
        ETH_CSL_CPSW_CONTROL    controlRegInfo;

        controlRegInfo.vlanAware    =   0;
        ...

        ETH_CSL_CPSW_setCpswControlReg (&controlRegInfo);

     @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_setCpswControlReg (CSL_Xge_cpswRegs *hCpswRegs,
    ETH_CSL_CPSW_CONTROL*   pControlRegInfo
);


/** ============================================================================
 *   @n@b ETH_CSL_CPSW_getPortStatsEnableReg
 *
 *   @b Description
 *   @n This function retrieves the contents of the CPSW Port Statistics
 *      Enable register.
 *
 *   @b Arguments
     @verbatim
        pPortStatsCfg       CSL_XGE_CPSW_PORTSTAT structure that needs to be populated
                            with the port statistics enable register contents.
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Reads
 *   @n XGE_CPSW_STAT_PORT_EN_REG_P0_STAT_EN,
 *      XGE_CPSW_STAT_PORT_EN_REG_P1_STAT_EN,
 *
 *   @b Example
 *   @verbatim
        ETH_CSL_CPSW_PORTSTAT       portStatsCfg;

        ETH_CSL_CPSW_getPortStatsEnableReg (&portStatsCfg);

     @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_getPortStatsEnableReg (CSL_Xge_cpswRegs *hCpswRegs,
    ETH_CSL_CPSW_PORTSTAT*  pPortStatsCfg
);


/** ============================================================================
 *   @n@b ETH_CSL_CPSW_setPortStatsEnableReg
 *
 *   @b Description
 *   @n This function sets up the contents of the CPSW Port Statistics
 *      Enable register.
 *
 *   @b Arguments
     @verbatim
        pPortStatsCfg       ETH_CSL_CPSW_PORTSTAT structure that contains the values
                            to be used to setup port statistics enable register.
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n XGE_CPSW_STAT_PORT_EN_REG_P0_STAT_EN,
 *      XGE_CPSW_STAT_PORT_EN_REG_P1_STAT_EN,
 *
 *   @b Example
 *   @verbatim
        ETH_CSL_CPSW_PORTSTAT       portStatsCfg;

        portStatsCfg.p0StatEnable  =   1;
        portStatsCfg.p1StatEnable  =   1;
        ...

        ETH_CSL_CPSW_setPortStatsEnableReg (&portStatsCfg);

     @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_setPortStatsEnableReg (CSL_Xge_cpswRegs *hCpswRegs,
    ETH_CSL_CPSW_PORTSTAT*  pPortStatsCfg
);


/** ============================================================================
 *   @n@b ETH_CSL_CPSW_setPortRxMaxLen
 *
 *   @b Description
 *   @n This function sets up the Port Receive Maximum length register.
 *
 *   @b Arguments
     @verbatim
        portNum             CPSW port number for which the Receive Maximum Length
                            must be retrieved.
        rxMaxLen            Maximum receive frame length to configure.
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n XGE_CPSW_P0_RX_MAXLEN_REG_RX_MAXLEN
 *      XGE_CPSW_PN_RX_MAXLEN_REG_RX_MAXLEN
 *
 *   @b Example
 *   @verbatim
 *      uint32      portNum, rxMaxLen;

        portNum     =   1;
        rxMaxLen    =   1518;

        ETH_CSL_CPSW_setPortRxMaxLen (portNum, rxMaxLen);

     @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_setPortRxMaxLen (CSL_Xge_cpswRegs *hCpswRegs,
    uint32                  portNum,
    uint32                  rxMaxLen
);


/** ============================================================================
 *   @n@b ETH_CSL_CPSW_setPortMACAddress
 *
 *   @b Description
 *   @n This function sets up the source MAC address the Tx Pause Frame corresponding to the
 *      CPSW port number specified.
 *
 *   @b Arguments
     @verbatim
        portNum                 CPSW port number for which the source MAC address
                                must be setup. (1-8)
        pMacAddress             6 byte Source MAC address to configure.
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  The input parameter 'pMacAddres' is expected to be 6 bytes long.
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n XGE_CPSW_PN_SA_L_REG_MACSRCADDR_7_0,
 *      XGE_CPSW_PN_SA_L_REG_MACSRCADDR_15_8,
 *      XGE_CPSW_PN_SA_H_REG_MACSRCADDR_23_16,
 *      XGE_CPSW_PN_SA_H_REG_MACSRCADDR_31_24,
 *      XGE_CPSW_PN_SA_H_REG_MACSRCADDR_39_32,
 *      XGE_CPSW_PN_SA_H_REG_MACSRCADDR_47_40
 *
 *   @b Example
 *   @verbatim
 *      uint8   macAddress [6], portNum;

        portNum         =   1;
        macAddress [0]  =   0x01;
        macAddress [1]  =   0x02;
        macAddress [2]  =   0x03;
        macAddress [3]  =   0x04;
        macAddress [4]  =   0x05;
        macAddress [5]  =   0x06;

        ETH_CSL_CPSW_setPortMACAddress (portNum, macAddress);

     @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_setPortMACAddress (CSL_Xge_cpswRegs *hCpswRegs,
    uint32                      portNum,
    uint8*                      pMacAddress
);


/** ============================================================================
 *   @n@b ETH_CSL_CPSW_setPortTimeSyncConfig
 *
 *   @b Description
 *   @n This function sets up the contents of Time sync control registers
 *      corresponding to the CPSW port number specified per user configuration.
 *
 *   @b Arguments
     @verbatim
        portNum                 CPSW port number for which the registers must be
                                configured.
        pTimeSyncConfig         ETH_CSL_CPSW_TSCONFIG containing settings for time
                                sync control registers.
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n XGE_CPSW_PN_TS_CTL_REG_TS_RX_ANNEX_F_EN,
 *      XGE_CPSW_PN_TS_CTL_REG_TS_RX_VLAN_LTYPE1_EN,
 *      XGE_CPSW_PN_TS_CTL_REG_TS_RX_VLAN_LTYPE2_EN,
 *      XGE_CPSW_PN_TS_CTL_REG_TS_RX_ANNEX_D_EN,
 *      XGE_CPSW_PN_TS_CTL_REG_TS_TX_ANNEX_F_EN,
 *      XGE_CPSW_PN_TS_CTL_REG_TS_TX_VLAN_LTYPE1_EN,
 *      XGE_CPSW_PN_TS_CTL_REG_TS_TX_VLAN_LTYPE2_EN,
 *      XGE_CPSW_PN_TS_CTL_REG_TS_TX_ANNEX_D_EN,
 *      XGE_CPSW_PN_TS_CTL_REG_TS_LTYPE2_EN,
 *      XGE_CPSW_PN_TS_CTL_REG_TS_RX_ANNEX_E_EN,
 *      XGE_CPSW_PN_TS_CTL_REG_TS_TX_ANNEX_E_EN,
 *      XGE_CPSW_PN_TS_CTL_REG_TS_TX_HOST_TS_EN,
 *      XGE_CPSW_PN_TS_CTL_REG_TS_MSG_TYPE_EN
 *
 *      XGE_CPSW_PN_TS_SEQ_LTYPE_REG_TS_LTYPE1,
 *      XGE_CPSW_PN_TS_SEQ_LTYPE_REG_TS_SEQ_ID_OFFSET,
 *
 *      XGE_CPSW_PN_TS_VLAN_LTYPE_REG_TS_VLAN_LTYPE1,
 *      XGE_CPSW_PN_TS_VLAN_LTYPE_REG_TS_VLAN_LTYPE2,
 *
 *      XGE_CPSW_PN_TS_CTL_LTYPE2_REG_TS_LTYPE2,
 *      XGE_CPSW_PN_TS_CTL_LTYPE2_REG_TS_107,
 *      XGE_CPSW_PN_TS_CTL_LTYPE2_REG_TS_129,
 *      XGE_CPSW_PN_TS_CTL_LTYPE2_REG_TS_130,
 *      XGE_CPSW_PN_TS_CTL_LTYPE2_REG_TS_131,
 *      XGE_CPSW_PN_TS_CTL_LTYPE2_REG_TS_132,
 *      XGE_CPSW_PN_TS_CTL_LTYPE2_REG_TS_319,
 *      XGE_CPSW_PN_TS_CTL_LTYPE2_REG_TS_330,
 *      XGE_CPSW_PN_TS_CTL_LTYPE2_REG_TS_TTL_NONZERO,
 *      XGE_CPSW_PN_TS_CTL_LTYPE2_REG_TS_UNI_EN,
 *
 *      XGE_CPSW_PN_TS_CTL2_REG_TS_MCAST_TYPE_EN,
 *      XGE_CPSW_PN_TS_CTL2_REG_TS_DOMAIN_OFFSET
 *
 *   @b Example
 *   @verbatim
 *      uint32              portNum;
        ETH_CSL_CPSW_TSCONFIG   tsConfig;

        portNum =   1;

        tsConfig.tsRxVlanLType1Enable   =   0;
        tsConfig.tsRxVlanLType2Enable   =   0;
        ...

        ETH_CSL_CPSW_setPortTimeSyncConfig (portNum, &tsConfig);

     @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_setPortTimeSyncConfig (CSL_Xge_cpswRegs *hCpswRegs,
    uint32                  portNum,
    ETH_CSL_CPSW_TSCONFIG*        pTimeSyncConfig
);




/********************************************************************************
*************************  Statistics (STATS) Submodule *************************
********************************************************************************/

void ETH_CSL_CPSW_getPortStats (CSL_Xge_cpswRegs *hCpswRegs,
    uint32                  portNum,
    ETH_CSL_CPSW_STATS*         pCpswStats
);


/********************************************************************************
********************  Address Lookup Engine (ALE) Submodule *********************
********************************************************************************/


/** ============================================================================
 *   @n@b ETH_CSL_CPSW_isAleVlanAwareEnabled
 *
 *   @b Description
 *   @n This function indicates if ALE is programmed to be VLAN aware.
 *
 *   @b Arguments
 *   @n None
 *
 *   <b> Return Value </b>
 *   @n  TRUE                   ALE VLAN aware. ALE drops packets if VLAN not found.
 *   @n  FALSE                  ALE not VLAN aware. Floods if VLAN not found.
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Reads
 *   @n ALE_ALE_CONTROL_ALE_VLAN_AWARE
 *
 *   @b Example
 *   @verbatim
        if (ETH_CSL_CPSW_isAleVlanAwareEnabled () == TRUE)
        {
            // ALE VLAN aware
        }
        else
        {
            // ALE not VLAN aware
        }
     @endverbatim
 * =============================================================================
 */
uint32 ETH_CSL_CPSW_isAleVlanAwareEnabled (CSL_AleRegs *hCpswAleRegs);

/** ============================================================================
 *   @n@b CSL_CPSW_isAleBypassEnabled
 *
 *   @b Description
 *   @n This function indicates if ALE is programmed to be in bypass mode.
 *
 *   @b Arguments
 *   @n None
 *
 *   <b> Return Value </b>
 *   @n  TRUE                   ALE bypass mode enabled.
 *   @n  FALSE                  ALE bypass mode disabled.
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Reads
 *   @n ALE_ALE_CONTROL_ENABLE_BYPASS
 *
 *   @b Example
 *   @verbatim
        if (CSL_CPSW_isAleBypassEnabled () == TRUE)
        {
            // ALE bypass mode on
        }
        else
        {
            // ALE bypass mode off
        }
     @endverbatim
 * =============================================================================
 */
uint32 CSL_CPSW_isAleBypassEnabled (CSL_AleRegs *hCpswAleRegs);

/** ============================================================================
 *   @n@b ETH_CSL_CPSW_enableAleBypass
 *
 *   @b Description
 *   @n This function configures the ALE control register to enable bypass mode.
 *
 *   @b Arguments
 *   @n None
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n ALE_ALE_CONTROL_ENABLE_BYPASS=1
 *
 *   @b Example
 *   @verbatim

        ETH_CSL_CPSW_enableAleBypass ();

     @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_enableAleBypass (CSL_AleRegs *hCpswAleRegs);


/** ============================================================================
 *   @n@b ETH_CSL_CPSW_disableAleBypass
 *
 *   @b Description
 *   @n This function configures the ALE control register to disable bypass mode.
 *
 *   @b Arguments
 *   @n None
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n ALE_ALE_CONTROL_ENABLE_BYPASS=0
 *
 *   @b Example
 *   @verbatim

        ETH_CSL_CPSW_disableAleBypass ();

     @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_disableAleBypass (CSL_AleRegs *hCpswAleRegs);


/** ============================================================================
 *   @n@b ETH_CSL_CPSW_clearAleTable
 *
 *   @b Description
 *   @n This function initiates a full ALE table cleanup. The ALE hardware
 *      clears all table entries.
 *
 *   @b Arguments
 *   @n None
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n ALE_ALE_CONTROL_CLEAR_TABLE=1
 *
 *   @b Example
 *   @verbatim

        ETH_CSL_CPSW_clearAleTable ();

     @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_clearAleTable (CSL_AleRegs *hCpswAleRegs);


/** ============================================================================
 *   @n@b ETH_CSL_CPSW_disableAle
 *
 *   @b Description
 *   @n This function configures the ALE control register to disable ALE processing.
 *
 *   @b Arguments
 *   @n None
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n ALE_ALE_CONTROL_ENABLE_ALE=0
 *
 *   @b Example
 *   @verbatim

        ETH_CSL_CPSW_disableAle ();

     @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_disableAle (CSL_AleRegs *hCpswAleRegs);


/** ============================================================================
 *   @n@b ETH_CSL_CPSW_getAleControlReg
 *
 *   @b Description
 *   @n This function retrieves the contents of the ALE control register.
 *
 *   @b Arguments
 *   @n None
 *
 *   <b> Return Value </b>
 *   @n  >=0                ALE control register contents.
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Reads
 *   @n ALE_CONTROL_REG
 *
 *   @b Example
 *   @verbatim
        uint32      aleCtrlVal;

        aleCtrlVal  =   ETH_CSL_CPSW_getAleControlReg ();

     @endverbatim
 * =============================================================================
 */
uint32 ETH_CSL_CPSW_getAleControlReg (CSL_AleRegs *hCpswAleRegs);


/** ============================================================================
 *   @n@b ETH_CSL_CPSW_setAleControlReg
 *
 *   @b Description
 *   @n This function sets up the contents of the ALE control register.
 *
 *   @b Arguments
     @verbatim
        aleCtrlVal          Value to be configured to the ALE control register.
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n ALE_CONTROL_REG
 *
 *   @b Example
 *   @verbatim
 *      uint32          aleCtrlVal = 0;

        aleCtrlVal      =   ETH_CSL_CPSW_getAleControlReg ();
        aleCtrlVal      |=  CSL_XGE_CPSW_ALECONTROL_CLRTABLE_EN;

        ETH_CSL_CPSW_setAleControlReg (&aleCtrlRegInfo);

     @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_setAleControlReg
(CSL_AleRegs *hCpswAleRegs,
    uint32                      aleCtrlVal
);

/** ============================================================================
 *   @n@b ETH_CSL_CPSW_getAleCOSPolicerEntry
 *
 *   @b Description
 *   @n This function reads the ALE Policer table entry for the index specified and
 *      fills the output parameter structure with Policer configuration
 *      read from the hardware.
 *      This function only reads data releate to COS mapping in ALE policer entry.
 *
 *   @b Arguments
     @verbatim
        index                   ALE Policer table index to be read.
        pPolCfg                 ALE Policer entry contents read.
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n ALE_POLICETBLCTL_POL_TBL_IDX
 *      ALE_POLICETBLCTL_WRITE_ENABLE
 *
 *   @b Reads
 *   @n ALE_POLICECFG0_PORT_MEN,
 *      ALE_POLICECFG0_PRI_MEN,
 *      ALE_THREADMAPVAL_THREAD_EN,
 *      ALE_THREADMAPVAL_THREADVAL
 *
 *   @b Example
 *   @verbatim
        Uint32                                  index;
        ETH_CSL_CPSW_ALE_POLICER_ENTRY          polCfg;

        index   =   0;

         // Read Policer Entry config from hardware
         ETH_CSL_CPSW_getAleCOSPolicerEntry (index, &polCfg);

     @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_getAleCOSPolicerEntry
(CSL_AleRegs *hCpswAleRegs,
    uint32                              index,
    ETH_CSL_CPSW_ALE_POLICER_ENTRY*         pPolCfg
);

/** ============================================================================
 *   @n@b ETH_CSL_CPSW_setAleCOSPolicerEntry
 *
 *   @b Description
 *   @n This function sets up the ALE COS table entry for the index specified with
 *      COS configuration specified here.
 *
 *   @b Arguments
     @verbatim
        index                   ALE table index.
        pPolCfg                 ALE entry contents to be configured.
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n ALE_POLICECFG0_PRI_VAL,
 *      ALE_THREADMAPVAL_THREAD_EN,
 *      ALE_THREADMAPVAL_THREADVAL
 *
 *   @b Example
 *   @verbatim
        uint32                                  index;
        ETH_CSL_CPSW_ALE_POLICER_ENTRY          polCfg;

        index   =   0;
        polEntry.pri = priority;
        polEntry.thread = threadId;
        ...

        // Add ALE Policer entry
        ETH_CSL_CPSW_setAleCOSPolicerEntry (index, &polCfg);

     @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_setAleCOSPolicerEntry
(CSL_AleRegs *hCpswAleRegs,
    uint32                              index,
    ETH_CSL_CPSW_ALE_POLICER_ENTRY*         pPolCfg
);


void ETH_CSL_CPSW_disableAlePolicerThread(CSL_AleRegs *hCpswAleRegs,
                                                    uint32           index);

/** ============================================================================
 *   @n@b ETH_CSL_CPSW_getAleUnkownVlanReg
 *
 *   @b Description
 *   @n This function retrieves the contents of the ALE Unknown VLAN and etc registers.
 *
 *   @b Arguments
     @verbatim
        pUnVlanMemList          Unknown VLAN member list.
        pUnMcastFloodMask       Unknown VLAN Multicast flood mask.
        pUnRegMcastFloodMask    Unknown VLAN Registered Multicast Flood mask.
        pUnForceUntagEgress     Unknown VLAN Force Untagged Egress.
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Reads
 *   @n ALE_UNKNOWN_VLAN_REG_UNKNOWN_LIST,
 *      ALE_UNKNOWN_MCAST_FLOOD_REG_MASK,
 *      ALE_UNKNOWN_REG_MCAST_FLOOD_REG_MASK,
 *      ALE_ALE_UVLAN_UNTAG_UVLAN_FORCE_UNTAGGED_EGRESS
 *
 *   @b Example
 *   @verbatim
        uint32      unVlanMemList, unMcastFloodMask, unRegMcastFloodMask, unForceUntagEgress;

        ETH_CSL_CPSW_getAleUnkownVlanReg (&unVlanMemList,
                                      &unMcastFloodMask,
                                      &unRegMcastFloodMask,
                                      &unForceUntagEgress);

     @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_getAleUnkownVlanReg
(CSL_AleRegs *hCpswAleRegs,
    uint32*                        pUnVlanMemList,
    uint32*                        pUnMcastFloodMask,
    uint32*                        pUnRegMcastFloodMask,
    uint32*                        pUnForceUntagEgress
);


/** ============================================================================
 *   @n@b ETH_CSL_CPSW_setAleUnkownVlanReg
 *
 *   @b Description
 *   @n This function sets up the contents of the ALE Unknown VLAN and etc. register.
 *
 *   @b Arguments
     @verbatim
        unVlanMemList           Unknown VLAN member list.
        unMcastFloodMask        Unknown VLAN Multicast flood mask.
        unRegMcastFloodMask     Unknown VLAN Registered Multicast Flood mask.
        unForceUntagEgress      Unknown VLAN Force Untagged Egress.
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n ALE_UNKNOWN_VLAN_REG_UNKNOWN_LIST,
 *      ALE_UNKNOWN_MCAST_FLOOD_REG_MASK,
 *      ALE_UNKNOWN_REG_MCAST_FLOOD_REG_MASK,
 *      ALE_ALE_UVLAN_UNTAG_UVLAN_FORCE_UNTAGGED_EGRESS
 *
 *   @b Example
 *   @verbatim
        uint32      unVlanMemList, unMcastFloodMask, unRegMcastFloodMask, unForceUntagEgress;

        unVlanMemList           =   0;
        unMcastFloodMask        =   3;
        unRegMcastFloodMask     =   0;
        unForceUntagEgress      =   0;

        ETH_CSL_CPSW_setAleUnkownVlanReg (unVlanMemList,
                                      unMcastFloodMask,
                                      unRegMcastFloodMask,
                                      unForceUntagEgress);

     @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_setAleUnkownVlanReg
(CSL_AleRegs *hCpswAleRegs,
    uint32                      unVlanMemList,
    uint32                      unMcastFloodMask,
    uint32                      unRegMcastFloodMask,
    uint32                      unForceUntagEgress
);


/** ============================================================================
 *   @n@b ETH_CSL_CPSW_getAleTableEntry
 *
 *   @b Description
 *   @n This function retrieves an ALE table entry corresponding to the
 *      ALE entry index specified in 'index' input parameter. The ALE
 *      entry values corresponding to the ALE_TBLW0, ALE_TBLW1 and
 *      ALE_TBLW2 registers are returned in 'pAleInfoWd0', 'pAleInfoWd1', 'pAleInfoWd2'
 *      output parameters.
 *
 *   @b Arguments
     @verbatim
        index                   ALE table index to be read.
        pAleInfoWd0             Contents of ALE Table Word 0 Register (ALE_TBLW0).
        pAleInfoWd1             Contents of ALE Table Word 1 Register (ALE_TBLW1).
        pAleInfoWd2             Contents of ALE Table Word 2 Register (ALE_TBLW2).
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n ALE_ALE_TBLCTL_TABLEIDX,
 *      ALE_ALE_TBLCTL_TABLEWR=0
 *
 *   @b Reads
 *   @n ALE_ALE_TBLW0_TABLEWRD0,
 *      ALE_ALE_TBLW1_TABLEWRD1,
 *      ALE_ALE_TBLW2_TABLEWRD2
 *
 *   @b Example
 *   @verbatim
        uint32      index, info0, info1, info2;

        index   =   0;

        ETH_CSL_CPSW_getAleTableEntry (index,
                                   &info0,
                                   &info1,
                                   &info2);

     @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_getAleTableEntry
(CSL_AleRegs *hCpswAleRegs,
    uint32                      index,
    uint32*                     pAleInfoWd0,
    uint32*                     pAleInfoWd1,
    uint32*                     pAleInfoWd2
);


/** ============================================================================
 *   @n@b ETH_CSL_CPSW_getALEEntryType
 *
 *   @b Description
 *   @n This function returns the ALE entry type for any given ALE table
 *      entry index.
 *
 *   @b Arguments
     @verbatim
        hCpswAleRegs            ALE register overlay
        index                   ALE table index to be read.
        aleType                 ALE type(4-Port/9-Port)
 *   @endverbatim
 *
 *   <b> Return Value </b>  ETH_CSL_CPSW_ALE_ENTRYTYPE
 *   @n  ALE_ENTRYTYPE_FREE             ALE entry is free.
 *   @n  ALE_ENTRYTYPE_ADDRESS          ALE entry contains a unicast/multicast address.
 *   @n  ALE_ENTRYTYPE_VLAN             ALE entry contains a VLAN.
 *   @n  ALE_ENTRYTYPE_VLANADDRESS      ALE entry contains a VLAN and a unicast/multicast address.
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n ALE_ALE_TBLCTL_TABLEIDX,
 *      ALE_ALE_TBLCTL_TABLEWR=0
 *
 *   @b Reads
 *   @n ALE_TABLE_WORD1_REG
 *
 *   @b Example
 *   @verbatim
 *      uint32      index = 0;
        if (ETH_CSL_CPSW_getALEEntryType () == ALE_ENTRYTYPE_FREE)
        {
            // ALE entry free
        }
     @endverbatim
 * =============================================================================
 */
ETH_CSL_CPSW_ALE_ENTRYTYPE ETH_CSL_CPSW_getALEEntryType(CSL_AleRegs *hCpswAleRegs,
                                                uint32      index,
                                                ETH_CSL_CPSW_ALETABLE_TYPE aleType);

/** ============================================================================
 *   @n@b ETH_CSL_CPSW_getAleMcastAddrEntry
 *
 *   @b Description
 *   @n This function reads the ALE table entry for the index specified and
 *      fills the output parameter structure with Multicast address configuration
 *      read from the hardware.
 *
 *   @b Arguments
     @verbatim
        hCpswAleRegs            ALE register overlay
        index                   ALE table index to be read.
        pMcastAddrCfg           ALE entry contents read.
        aleType                 ALE type(4-Port/9-Port)
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n ALE_ALE_TBLCTL_TABLEIDX,
 *      ALE_ALE_TBLCTL_TABLEWR=0
 *
 *   @b Reads
 *   @n ALE_TABLE_WORD0,
 *      ALE_TABLE_WORD1,
 *      ALE_TABLE_WORD2
 *
 *   @b Example
 *   @verbatim
        uint32                     index;
        ETH_CSL_ALE_MCASTADDR_ENTRY    mcastAddrCfg;

        index   =   0;

        if (ETH_CSL_CPSW_getALEEntryType (index) == ALE_ENTRYTYPE_ADDRESS)
        {
            // ALE entry has an address

            if (ETH_CSL_CPSW_getALEAddressType (index) ==  ALE_ADDRTYPE_MCAST)
            {
                // Read Multicast address config from hardware
                ETH_CSL_CPSW_getAleMcastAddrEntry (index, &mcastAddrCfg);
            }
        }

     @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_getAleMcastAddrEntry(CSL_AleRegs    *hCpswAleRegs,
                                   uint32         index,
                                   ETH_CSL_CPSW_ALE_MCASTADDR_ENTRY* pMcastAddrCfg,
                                   ETH_CSL_CPSW_ALETABLE_TYPE aleType);


/** ============================================================================
 *   @n@b ETH_CSL_CPSW_setAleMcastAddrEntry
 *
 *   @b Description
 *   @n This function sets up the ALE table entry for the index specified with
 *      Multicast address configuration specified here.
 *
 *   @b Arguments
     @verbatim
        hCpswAleRegs            ALE register overlay
        index                   ALE table index.
        pMcastAddrCfg           ALE entry contents to be configured.
        aleType                 ALE type(4-Port/9-Port)
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n ALE_ALE_TBLCTL_TABLEIDX,
 *      ALE_ALE_TBLCTL_TABLEWR=1
 *      ALE_TABLE_WORD0,
 *      ALE_TABLE_WORD1,
 *      ALE_TABLE_WORD2
 *
 *   @b Example
 *   @verbatim
        uint32                              index;
        ETH_CSL_CPSW_ALE_MCASTADDR_ENTRY    mcastAddrCfg;

        index   =   0;
        mcastAddrCfg.macAddress [0] = 0x00;
        mcastAddrCfg.macAddress [1] = 0x01;
        ...

        if (ETH_CSL_CPSW_getALEEntryType (index) == ALE_ENTRYTYPE_FREE)
        {
            // ALE entry is free

            // Add Multicast address entry
            ETH_CSL_CPSW_setAleMcastAddrEntry (index, &mcastAddrCfg);
        }

     @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_setAleMcastAddrEntry(CSL_AleRegs *hCpswAleRegs,
                                   uint32                       index,
                                   ETH_CSL_CPSW_ALE_MCASTADDR_ENTRY*  pMcastAddrCfg,
                                   ETH_CSL_CPSW_ALETABLE_TYPE aleType);

/** ============================================================================
 *   @n@b ETH_CSL_CPSW_getAleVlanMcastAddrEntry
 *
 *   @b Description
 *   @n This function reads the ALE table entry for the index specified and
 *      fills the output parameter structure with VLAN Multicast address configuration
 *      read from the hardware.
 *
 *   @b Arguments
     @verbatim
        hCpswAleRegs            ALE register overlay
        index                   ALE table index to be read.
        pVlanMcastAddrCfg       ALE entry contents read.
        aleType                 ALE type(4-Port/9-Port)
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n ALE_ALE_TBLCTL_TABLEIDX,
 *      ALE_ALE_TBLCTL_TABLEWR=0
 *
 *   @b Reads
 *   @n ALE_TABLE_WORD0,
 *      ALE_TABLE_WORD1,
 *      ALE_TABLE_WORD2
 *
 *   @b Example
 *   @verbatim
        uint32                                  index;
        ETH_CSL_CPSW_ALE_VLANMCASTADDR_ENTRY    vlanMcastAddrCfg;

        index   =   0;

        if (ETH_CSL_CPSW_getALEEntryType (index) == ALE_ENTRYTYPE_VLANADDRESS)
        {
            // ALE entry has a VLAN address

            if (ETH_CSL_CPSW_getALEAddressType (index) ==  ALE_ADDRTYPE_MCAST)
            {
                // Read VLAN Multicast address config from hardware
                ETH_CSL_CPSW_getAleVlanMcastAddrEntry (index, &vlanMcastAddrCfg);
            }
        }

     @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_getAleVlanMcastAddrEntry(CSL_AleRegs *hCpswAleRegs,
                                       uint32      index,
                                       ETH_CSL_CPSW_ALE_VLANMCASTADDR_ENTRY*   pVlanMcastAddrCfg,
                                       ETH_CSL_CPSW_ALETABLE_TYPE aleType);

/** ============================================================================
 *   @n@b ETH_CSL_CPSW_setAleVlanMcastAddrEntry
 *
 *   @b Description
 *   @n This function sets up the ALE table entry for the index specified with
 *      VLAN Multicast address configuration specified here.
 *
 *   @b Arguments
     @verbatim
        hCpswAleRegs            ALE register overlay
        index                   ALE table index.
        pVlanMcastAddrCfg       ALE entry contents to be configured.
        aleType                 ALE type(4-Port/9-Port)
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n ALE_ALE_TBLCTL_TABLEIDX,
 *      ALE_ALE_TBLCTL_TABLEWR=1
 *      ALE_TABLE_WORD0,
 *      ALE_TABLE_WORD1,
 *      ALE_TABLE_WORD2
 *
 *   @b Example
 *   @verbatim
        uint32                                  index;
        ETH_CSL_CPSW_ALE_VLANMCASTADDR_ENTRY    vlanMcastAddrCfg;

        index   =   0;
        vlanMcastAddrCfg.macAddress [0] = 0x00;
        vlanMcastAddrCfg.macAddress [1] = 0x01;
        ...

        if (ETH_CSL_CPSW_getALEEntryType (index) == ALE_ENTRYTYPE_FREE)
        {
            // ALE entry is free

            // Add VLAN Multicast address entry
            ETH_CSL_CPSW_setAleVlanMcastAddrEntry (index, &vlanMcastAddrCfg);
        }

     @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_setAleVlanMcastAddrEntry(CSL_AleRegs *hCpswAleRegs,
                           uint32      index,
                           ETH_CSL_CPSW_ALE_VLANMCASTADDR_ENTRY* pVlanMcastAddrCfg,
                           ETH_CSL_CPSW_ALETABLE_TYPE aleType);

/** ============================================================================
 *   @n@b ETH_CSL_CPSW_setAleUnicastAddrEntry
 *
 *   @b Description
 *   @n This function sets up the ALE table entry for the index specified with
 *      unicast address configuration specified here.
 *
 *   @b Arguments
     @verbatim
        hCpswAleRegs            ALE register overlay
        index                   ALE table index.
        pUcastAddrCfg           ALE entry contents to be configured.
        aleType                 ALE type(4-Port/9-Port)
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n ALE_ALE_TBLCTL_TABLEIDX,
 *      ALE_ALE_TBLCTL_TABLEWR=1
 *      ALE_TABLE_WORD0,
 *      ALE_TABLE_WORD1,
 *      ALE_TABLE_WORD2
 *
 *   @b Example
 *   @verbatim
        uint32                                  index;
        ETH_CSL_CPSW_ALE_UNICASTADDR_ENTRY      ucastAddrCfg;

        index   =   0;
        ucastAddrCfg.macAddress [0] = 0x00;
        ucastAddrCfg.macAddress [1] = 0x01;
        ...

        if (ETH_CSL_CPSW_getALEEntryType (index) == ALE_ENTRYTYPE_FREE)
        {
            // ALE entry is free

            // Add Unicast address entry
            ETH_CSL_CPSW_setAleUnicastAddrEntry (index, &ucastAddrCfg);
        }

     @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_setAleUnicastAddrEntry(CSL_AleRegs *hCpswAleRegs,
                                     uint32      index,
                                     ETH_CSL_CPSW_ALE_UNICASTADDR_ENTRY* pUcastAddrCfg,
                                     ETH_CSL_CPSW_ALETABLE_TYPE aleType);

/** ============================================================================
 *   @n@b ETH_CSL_CPSW_setAleVlanUnicastAddrEntry
 *
 *   @b Description
 *   @n This function sets up the ALE table entry for the index specified with
 *      VLAN unicast address configuration specified here.
 *
 *   @b Arguments
     @verbatim
        hCpswAleRegs            ALE register overlay
        index                   ALE table index.
        pVlanUcastAddrCfg       ALE entry contents to be configured.
        aleType                 ALE type(4-Port/9-Port)
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n ALE_ALE_TBLCTL_TABLEIDX,
 *      ALE_ALE_TBLCTL_TABLEWR=1
 *      ALE_TABLE_WORD0,
 *      ALE_TABLE_WORD1,
 *      ALE_TABLE_WORD2
 *
 *   @b Example
 *   @verbatim
        uint32                                  index;
        ETH_CSL_CPSW_ALE_VLANUNICASTADDR_ENTRY  vlanUcastAddrCfg;

        index   =   0;
        vlanUcastAddrCfg.macAddress [0] = 0x00;
        vlanUcastAddrCfg.macAddress [1] = 0x01;
        ...

        if (ETH_CSL_CPSW_getALEEntryType (index) == ALE_ENTRYTYPE_FREE)
        {
            // ALE entry is free

            // Add VLAN Unicast address entry
            ETH_CSL_CPSW_setAleVlanUnicastAddrEntry (index, &vlanUcastAddrCfg);
        }

     @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_setAleVlanUnicastAddrEntry(CSL_AleRegs *hCpswAleRegs,
                         uint32      index,
                         ETH_CSL_CPSW_ALE_VLANUNICASTADDR_ENTRY* pVlanUcastAddrCfg,
                         ETH_CSL_CPSW_ALETABLE_TYPE aleType);


/** ============================================================================
 *   @n@b ETH_CSL_CPSW_clearAleEntry
 *
 *   @b Description
 *   @n This function clears the ALE entry corresponding to the index
 *      specified
 *
 *   @b Arguments
     @verbatim
        index                   ALE table index.
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n ALE_ALE_TBLCTL_TABLEIDX,
 *      ALE_ALE_TBLCTL_TABLEWR=1
 *      ALE_TABLE_WORD0=0,
 *      ALE_TABLE_WORD1=0,
 *      ALE_TABLE_WORD2=0
 *
 *   @b Example
 *   @verbatim
        uint32                              index;

        index   =   0;

        ETH_CSL_CPSW_clearAleEntry (index);
     @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_clearAleEntry
(CSL_AleRegs *hCpswAleRegs,
    uint32                                  index
);


/** ============================================================================
 *   @n@b ETH_CSL_CPSW_getAlePortControlReg
 *
 *   @b Description
 *   @n This function retrieves the contents of ALE Port control register
 *      corresponding to the port number specified.
 *
 *   @b Arguments
     @verbatim
        portNo                  Port number for which the ALE port control register
                                must be read.
        pPortControlInfo        ETH_CSL_CPSW_ALE_PORTCONTROL structure that needs to be
                                filled with Port control register info read from
                                the hardware.
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Reads
 *   @n ALE_I0_ALE_PORTCTL0_I0_REG_P0_PORTSTATE,
 *      ALE_I0_ALE_PORTCTL0_I0_REG_P0_DROP_UN_TAGGED,
 *      ALE_I0_ALE_PORTCTL0_I0_REG_P0_VID_INGRESS_CHECK,
 *      ALE_I0_ALE_PORTCTL0_I0_REG_P0_NO_LEARN,
 *      ALE_I0_ALE_PORTCTL0_I0_REG_P0_NO_SA_UPDATE,
 *      ALE_I0_ALE_PORTCTL0_I0_REG_P0_MACONLY,
 *      ALE_I0_ALE_PORTCTL0_I0_REG_P0_DIS_PAUTHMOD,
 *      ALE_I0_ALE_PORTCTL0_I0_REG_P0_MACONLY_CAF,
 *      ALE_I0_ALE_PORTCTL0_I0_REG_P0_MCAST_LIMIT,
 *      ALE_I0_ALE_PORTCTL0_I0_REG_P0_BCAST_LIMIT
 *
 *   @b Example
 *   @verbatim
        uint32                          index;
        ETH_CSL_CPSW_ALE_PORTCONTROL    portControlInfo;

        index   =   0;

        ETH_CSL_CPSW_getAlePortControlReg (index, &portControlInfo);

     @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_getAlePortControlReg
(CSL_AleRegs *hCpswAleRegs,
    uint32                      portNo,
    ETH_CSL_CPSW_ALE_PORTCONTROL*   pPortControlInfo
);


/** ============================================================================
 *   @n@b ETH_CSL_CPSW_setAlePortControlReg
 *
 *   @b Description
 *   @n This function sets up the contents of ALE Port control register
 *      corresponding to the port number specified.
 *
 *   @b Arguments
     @verbatim
        portNo                  Port number for which the ALE port control register
                                must be configured.
        pPortControlInfo        ETH_CSL_CPSW_ALE_PORTCONTROL structure that contains
                                port control register settings to be written.
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n ALE_I0_ALE_PORTCTL0_I0_REG_P0_PORTSTATE,
 *      ALE_I0_ALE_PORTCTL0_I0_REG_P0_DROP_UN_TAGGED,
 *      ALE_I0_ALE_PORTCTL0_I0_REG_P0_VID_INGRESS_CHECK,
 *      ALE_I0_ALE_PORTCTL0_I0_REG_P0_NO_LEARN,
 *      ALE_I0_ALE_PORTCTL0_I0_REG_P0_NO_SA_UPDATE,
 *      ALE_I0_ALE_PORTCTL0_I0_REG_P0_MACONLY,
 *      ALE_I0_ALE_PORTCTL0_I0_REG_P0_DIS_PAUTHMOD,
 *      ALE_I0_ALE_PORTCTL0_I0_REG_P0_MACONLY_CAF,
 *      ALE_I0_ALE_PORTCTL0_I0_REG_P0_MCAST_LIMIT,
 *      ALE_I0_ALE_PORTCTL0_I0_REG_P0_BCAST_LIMIT
 *
 *   @b Example
 *   @verbatim
        uint32                          index;
        ETH_CSL_CPSW_ALE_PORTCONTROL    portControlInfo;

        index   =   0;
        portControlInfo.portState   =   ALE_PORTSTATE_FORWARD |
                                        ALE_PORTSTATE_LEARN;

        ETH_CSL_CPSW_setAlePortControlReg (index, &portControlInfo);

     @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_setAlePortControlReg
(CSL_AleRegs *hCpswAleRegs,
    uint32                      portNo,
    ETH_CSL_CPSW_ALE_PORTCONTROL*   pPortControlInfo
);


/** ============================================================================
 *   @n@b ETH_CSL_CPSW_setAlePolicerGlobConfig
 *
 *   @b Description
 *   @n This function sets up the contents of the CPSW ALE Policer/Classifier
 *      global registers
 *      per user-specified ALE Policer Global Configuration.
 *
 *   @b Arguments
     @verbatim
        pGlobConfig         ETH_CSL_CPSW_ALE_POLICER_GLOB_CONFIG structure that holds the values
                            that need to be configured to the ALE Policer global control
                            registers.
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  CPSW ALE Policer Global control register modified with values provided.
 *
 *   @b Writes
 *   @n ALE_THREADMAPDEF_DEFTHREAD_EN,
 *      ALE_THREADMAPDEF_DEFTHREADVAL
 *
 *   @b Example
 *   @verbatim
        ETH_CSL_CPSW_ALE_POLICER_GLOB_CONFIG    globConfig;

        globConfig.defThreadEnable  =   1;
        ...

        ETH_CSL_CPSW_setAlePolicerGlobConfig (&globConfig);

     @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_setAlePolicerGlobConfig (CSL_AleRegs *hCpswAleRegs,
    ETH_CSL_CPSW_ALE_POLICER_GLOB_CONFIG*   pGlobConfig
);


void ETH_CSL_CPSW_setPriCirEir(CSL_Xge_cpswRegs *hCpswRegs,uint32 portNo, uint32 pri, uint32 cir, uint32 eir);


void ETH_CSL_CPSW_getPriCirEir(CSL_Xge_cpswRegs *hCpswRegs,uint32 portNo, uint32 pri, uint32 *cir, uint32 *eir);

/** ============================================================================
 *   @n@b ETH_CSL_CPSW_setCppiP0Control
 *
 *   @b Description
 *   @n This function sets the P0_CONTROL_REG register contents.
 *
 *   @b Arguments
 *    @verbatim
 *       pCppiP0ControlCfg   P0_CONTROL_REG configuration structure
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  none
 *
 * =============================================================================
 */
void  ETH_CSL_CPSW_setCppiP0Control(CSL_Xge_cpswRegs *hCpswRegs,
                                ETH_CSL_CPSW_CPPI_P0_CONTROL *pCppiP0ControlCfg);

/** ============================================================================
 *   @n@b ETH_CSL_CPSW_setP0HostBlksPri
 *
 *   @b Description
 *   @n This function sets CPPI_P0_Host_Blks_Pri  register
 *      This function is not applicable to two port switch
 *
 *   @b Arguments
 *    @verbatim
 *       pCppiHostBlksPri  CPPI_P0_Host_Blks_Pri register configuration
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  none
 *
 * =============================================================================
 */
void ETH_CSL_CPSW_setP0HostBlksPri(CSL_Xge_cpswRegs *hCpswRegs,
                               ETH_CSL_CPSW_CPPI_P0_HOSTBLKSPRI *pCppiHostBlksPri);

#ifdef __cplusplus
}
#endif

#endif

/**
@}
*/
