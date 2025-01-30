/**
 * @file  csl_cpgmac_sl.h
 *
 * @brief
 *  API Auxilary header file for Ethernet MAC submodule CSL.
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

#ifndef CSL_CPGMAC_SL_V5_H_
#define CSL_CPGMAC_SL_V5_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "cslr_xge_cpsw.h"


/**
@defgroup CSL_CPGMAC_SL_SYMBOL  CPGMAC_SL Symbols Defined
@ingroup CSL_CPGMAC_SL_API
*/
/**
@defgroup CSL_CPGMAC_SL_DATASTRUCT  CPGMAC_SL Data Structures
@ingroup CSL_CPGMAC_SL_API
*/
/**
@defgroup CSL_CPGMAC_SL_FUNCTION  CPGMAC_SL Functions
@ingroup CSL_CPGMAC_SL_API
*/
/**
@defgroup CSL_CPGMAC_SL_ENUM CPGMAC_SL Enumerated Data Types
@ingroup CSL_CPGMAC_SL_API
*/

/**
@addtogroup CSL_CPGMAC_SL_SYMBOL
@{
*/

/**
@}
*/

/** @addtogroup CSL_CPGMAC_SL_DATASTRUCT
 @{ */

/** Constants for passing parameters to the functions.
 */

/** @brief MAC control register configuration definitions */

/** Enable full duplex mode */
#define    CSL_CPGMAC_SL_MACCONTROL_FULLDUPLEX_EN       (((uint32)1U) << 0u)

/** Enable loopback mode */
#define    CSL_CPGMAC_SL_MACCONTROL_LOOPBACK_EN         (((uint32)1U) << 1u)

/** Enable Rx flow control mode */
#define    CSL_CPGMAC_SL_MACCONTROL_RX_FLOW_EN          (((uint32)1U) << 3u)

/** Enable Tx flow control mode */
#define    CSL_CPGMAC_SL_MACCONTROL_TX_FLOW_EN          (((uint32)1U) << 4u)

/** Enable GMII */
#define    CSL_CPGMAC_SL_MACCONTROL_GMII_EN             (((uint32)1U) << 5u)

/** Enable Tx pacing */
#define    CSL_CPGMAC_SL_MACCONTROL_TX_PACE_EN          (((uint32)1U) << 6u)

/** Enable Gigabit mode */
#define    CSL_CPGMAC_SL_MACCONTROL_GIG_EN              (((uint32)1U) << 7u)

/** Enable Tx short gap */
#define    CSL_CPGMAC_SL_MACCONTROL_TX_SHORT_GAP_EN     (((uint32)1U) << 10u)

/** Enable idle mode */
#define    CSL_CPGMAC_SL_MACCONTROL_CMD_IDLE_EN         (((uint32)1U) << 11u)

/** Enable idle mode */
#define    CSL_CPGMAC_SL_MACCONTROL_CASTAGNOLI_CRC      (((uint32)1U) << 12u)

/** Set IFCTL_A bit to 1 */
#define    CSL_CPGMAC_SL_MACCONTROL_IFCTL_A_EN          (((uint32)1U) << 15u)

/** Set IFCTL_B bit to 1 */
#define    CSL_CPGMAC_SL_MACCONTROL_IFCTL_B_EN          (((uint32)1U) << 16u)

/** Enable forced Gigabit mode */
#define    CSL_CPGMAC_SL_MACCONTROL_GIG_FORCE_EN        (((uint32)1U) << 17u)

/** Enable external control mode */
#define    CSL_CPGMAC_SL_MACCONTROL_EXT_EN              (((uint32)1U) << 18u)

/** Enable external receive flow control mode */
#define    CSL_CPGMAC_SL_MACCONTROL_EXT_EN_RX_FLOW      (((uint32)1U) << 19u)

/** Enable external transmit flow control mode */
#define    CSL_CPGMAC_SL_MACCONTROL_EXT_EN_TX_FLOW      (((uint32)1U) << 20u)

/** Enable Rx copy error frames mode */
#define    CSL_CPGMAC_SL_MACCONTROL_RX_CEF_EN           (((uint32)1U) << 22u)

/** Enable Rx copy short frames mode */
#define    CSL_CPGMAC_SL_MACCONTROL_RX_CSF_EN           (((uint32)1U) << 23u)

/** Enable Rx copy MAC control frames mode */
#define    CSL_CPGMAC_SL_MACCONTROL_RX_CMF_EN           (((uint32)1U) << 24u)

/** @brief
 *
 *  Holds the Sliver submodule's version info.
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
} CSL_CPGMAC_SL_VERSION;

/** @brief
 *
 *  Holds MAC status register contents.
 */
typedef struct {
    /** Transmit Flow Control Active - When asserted, this bit indicates that
      * the pause time period is being observed for a received pause frame.
      * No new transmissions will begin while this bit is asserted except for the
      * transmission of pause frames.  Any transmission in progress when this bit
      * is asserted will complete.
      */
    uint32      txFlowActive;
    /** Receive Flow Control Active - When asserted, indicates that receive flow
      * control is enabled and triggered
      */
    uint32      rxFlowActive;
    /** External Fullduplex - Value of the EXT_FULLDUPLEX input bit. */
    uint32      extFullDuplexEnabled;
    /** External GIG - This is the value of the EXT_GIG input bit. */
    uint32      extGigabitEnabled;
    /** External Receive Flow Control Enable - This is the value of the
      * EXT_RX_FLOW_EN input bit.
      */
    uint32      extRxFlowEnabled;
    /** External Transmit Flow Control Enable -
      * This is the value of the EXT_TX_FLOW_EN input bit
      */
    uint32      extTxFlowEnabled;
    /** Receive Priority Based Flow Control Active (priority 7 down to 0) */
    uint32      rxPfcFlowAct;
    /** Transmit Priority Based Flow Control Active (priority 7 down to 0) */
    uint32      txPfcFlowAct;
    /** The lowest priority that caused top of receive FIFO flow control trigger
      * since the last write to clear.
      */
    uint32      torfPri;
    /** Top of receive FIFO flow control trigger occurred.  */
    uint32      torf;
    /** Mac Transmit Idle - Both Prempt and Express MAC Transmit in idle state. */
    uint32      macTxIdle;
    /** Express MAC is idle .If IET is not enabled all traffic is express traffic */
    uint32      expressMacIdle;
    /** Prempt MAC is idle */
    uint32      preemptMacIdle;
    /** The Ethernet port (express and preempt (if present)) are in idle state */
    uint32      idle;
} CSL_CPGMAC_SL_MACSTATUS;

/**
@}
*/

/** @addtogroup CSL_CPGMAC_SL_FUNCTION
@{ */


/********************************************************************************
*************** Ethernet Media Access Controller (EMAC) Submodule ***************
********************************************************************************/

/** ============================================================================
 *   @n@b CSL_CPGMAC_SL_enableLoopback
 *
 *   @b Description
 *   @n This function configures the MAC control register to enable lopback mode
 *      for the MAC port specified.
 *
 *   @b Arguments
     @verbatim
        portNum                 MAC port number for which settings must be configured.
 *	 @endverbatim
 *
 *   <b> Return Value </b>
 *	 @n	 None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n XGE_CPSW_PN_MAC_CONTROL_REG_LOOPBACK=1
 *
 *   @b Example
 *   @verbatim
        uint32  portNum = 1;

        CSL_CPGMAC_SL_enableLoopback (portNum);

     @endverbatim
 * =============================================================================
 */
void CSL_CPGMAC_SL_enableLoopback
(CSL_Xge_cpswRegs *hCpswRegs,
    uint32                      portNum
);

/** ============================================================================
 *   @n@b CSL_CPGMAC_SL_enableGMII
 *
 *   @b Description
 *   @n This function configures the MAC control register to enable GMII for the
 *      MAC port specified.
 *
 *   @b Arguments
     @verbatim
        portNum                 MAC port number for which settings must be configured.
 *	 @endverbatim
 *
 *   <b> Return Value </b>
 *	 @n	 None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n XGE_CPSW_PN_MAC_CONTROL_REG_GMII_EN=1
 *
 *   @b Example
 *   @verbatim
        uint32  portNum =   1;

        CSL_CPGMAC_SL_enableGMII (portNum);

     @endverbatim
 * =============================================================================
 */
void CSL_CPGMAC_SL_enableGMII
(CSL_Xge_cpswRegs *hCpswRegs,
    uint32                      portNum
);

/** ============================================================================
 *   @n@b CSL_CPGMAC_SL_disableGMII
 *
 *   @b Description
 *   @n This function configures the MAC control register to disable GMII for the
 *      MAC port specified.
 *
 *   @b Arguments
     @verbatim
        portNum                 MAC port number for which settings must be configured.
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
 *   @n XGE_CPSW_PN_MAC_CONTROL_REG_GMII_EN=0
 *
 *   @b Example
 *   @verbatim
        uint32  portNum =   1;

        CSL_CPGMAC_SL_disableGMII (portNum);

     @endverbatim
 * =============================================================================
 */
void CSL_CPGMAC_SL_disableGMII
(CSL_Xge_cpswRegs *hCpswRegs,
    uint32                      portNum
);

/** ============================================================================
 *   @n@b CSL_CPGMAC_SL_getMacControlReg
 *
 *   @b Description
 *   @n This function retrieves the contents of MAC control register in entirety.
 *
 *   @b Arguments
     @verbatim
        portNum                 MAC port number for which the API must
                                retrieve the settings.
 *	 @endverbatim
 *
 *   <b> Return Value </b>
 *	 @n	 >=0                MAC control register contents.
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Reads
 *   @n CPGMAC_SL_MACCONTROL_REG
 *
 *   @b Example
 *   @verbatim
        uint32          macControlVal, portNum = 1;

        macControlVal   =   CSL_CPGMAC_SL_getMacControlReg (portNum);

     @endverbatim
 * =============================================================================
 */
uint32 CSL_CPGMAC_SL_getMacControlReg
(CSL_Xge_cpswRegs *hCpswRegs,
    uint32                      portNum
);

/** ============================================================================
 *   @n@b CSL_CPGMAC_SL_setMacControlReg
 *
 *   @b Description
 *   @n This function configures the contents of MAC control register in entirety.
 *
 *   @b Arguments
     @verbatim
        portNum             MAC port number for which the reset must be performed.
        macControlRegVal    value to be configured to the MAC control register.
 *	 @endverbatim
 *
 *   <b> Return Value </b>
 *	 @n	 None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n CPGMAC_SL_MACCONTROL_REG
 *
 *   @b Example
 *   @verbatim
        uint32          macControlVal, portNum = 1;

        macControlVal   =   CSL_CPGMAC_SL_getMacControlReg ();

        // Enable full duplex, giagbit mode too
        macControlVal   |=  CSL_CPGMAC_SL_MACCONTROL_FULLDUPLEX_EN |
                            CSL_CPGMAC_SL_MACCONTROL_GMII_EN;

        CSL_CPGMAC_SL_setMacControlReg (macControlVal, portNum);

     @endverbatim
 * =============================================================================
 */
void CSL_CPGMAC_SL_setMacControlReg
(CSL_Xge_cpswRegs *hCpswRegs,
    uint32                  portNum,
    uint32                  macControlRegVal
);

/** ============================================================================
 *   @n@b CSL_CPGMAC_SL_resetMac
 *
 *   @b Description
 *   @n This function issues a software reset to the MAC for the MAC port number
 *      specified.
 *
 *   @b Arguments
     @verbatim
        portNum             MAC port number for which the reset must be performed.
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
 *   @n XGE_CPSW_PN_MAC_SOFT_RESET_REG_SOFT_RESET=1
 *
 *   @b Example
 *   @verbatim
        uint32 portNum  =   1;

        CSL_CPGMAC_SL_resetMac (portNum);

     @endverbatim
 * =============================================================================
 */
void CSL_CPGMAC_SL_resetMac
(CSL_Xge_cpswRegs *hCpswRegs,
    uint32                      portNum
);

/** ============================================================================
 *   @n@b CSL_CPGMAC_SL_isMACResetDone
 *
 *   @b Description
 *   @n This function reads the MAC Soft Reset register to check if the software
 *      reset operation has completed.
 *
 *   @b Arguments
     @verbatim
        portNum                 MAC port number for which the API must
                                retrieve the settings.
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  TRUE               MAC Soft reset done.
 *   @n  FALSE              MAC Soft reset not yet completed.
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Reads
 *   @n XGE_CPSW_PN_MAC_SOFT_RESET_REG_SOFT_RESET
 *
 *   @b Example
 *   @verbatim
        uint32  portNum = 1;

        CSL_CPGMAC_SL_resetMac (portNum);

        // Wait until MAC software reset completes
        while (!CSL_CPGMAC_SL_isMACResetDone (portNum));

     @endverbatim
 * =============================================================================
 */
uint32 CSL_CPGMAC_SL_isMACResetDone
(CSL_Xge_cpswRegs *hCpswRegs,
    uint32                      portNum
);

/** ============================================================================
 *   @n@b CSL_CPGMAC_SL_setRxMaxLen
 *
 *   @b Description
 *   @n This function sets up the Receive maximum frame length in Receive Maximum
 *      Length register for the MAC port number specified.
 *
 *   @b Arguments
     @verbatim
        portNum             MAC port number for which the settings must be configured.
        rxMaxLen            the Receive maximum frame length to be configered into
                            the Receive Maximum Length register
 *	 @endverbatim
 *
 *   <b> Return Value </b>
 *	 @n	 None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n XGE_CPSW_PN_RX_MAXLEN_REG_RX_MAXLEN
 *
 *   @b Example
 *   @verbatim
        uint32          mtu, portNum = 1;

        mtu =   1518;

        CSL_CPGMAC_SL_setRxMaxLen (portNum, mtu);

     @endverbatim
 * =============================================================================
 */
uint32 CSL_CPGMAC_SL_setRxMaxLen
(CSL_Xge_cpswRegs *hCpswRegs,
    uint32                  portNum,
    uint32                  rxMaxLen
);

#ifdef __cplusplus
}
#endif

#endif

/**
@}
*/
