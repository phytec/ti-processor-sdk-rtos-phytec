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
 *  \file     EthTrcv_Priv.h
 *
 *  \brief    This file contains the common declarations and macros used across
 *            all the Ethernet Transceiver driver files.
 */

#ifndef ETHTRCV_PRIV_H_
#define ETHTRCV_PRIV_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

/* None */

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 *  \anchor PHY_Registers
 *  \name PHY registers
 *
 *  Standard PHY register address and definitions.
 *
 *  @{
 */

/** \brief Basic Mode Control Register (BMCR) */
#define ETHTRCV_BMCR                    (0x00U)

/** \brief Basic Mode Status Register (BMSR) */
#define ETHTRCV_BMSR                    (0x01U)

/** \brief PHY Identifier Register #1 (PHYIDR1) */
#define ETHTRCV_PHYIDR1                 (0x02U)

/** \brief PHY Identifier Register #2 (PHYIDR2) */
#define ETHTRCV_PHYIDR2                 (0x03U)

/** \brief Auto-Neg Advertisement Register (ANAR) */
#define ETHTRCV_ANAR                    (0x04U)

/** \brief Auto-Neg Link Partner Abilitiy Register (ANLPAR) */
#define ETHTRCV_ANLPAR                  (0x05U)

/** \brief Auto-Neg Expansion Register (ANER) */
#define ETHTRCV_ANER                    (0x06U)

/** \brief Auto-Neg NP TX Register (ANNPTR) */
#define ETHTRCV_ANNPTR                  (0x07U)

/** \brief Auto-Neg NP RX Register (ANNPRR) */
#define ETHTRCV_ANNPRR                  (0x08U)

/** \brief 1000BASE-T Control Register (CFG1 or 1KTCR) */
#define ETHTRCV_CFG1                    (0x09U)

/** \brief 1000BASE-T Status Register (STS1 or 1KSTSR) */
#define ETHTRCV_STS1                    (0x0AU)

/** \brief MMD Access Control Register */
#define ETHTRCV_MMD_CR                  (0x0DU)

/** \brief MMD Access Data Register */
#define ETHTRCV_MMD_DR                  (0x0EU)

/** \brief BMCR[15] RESET bit-field mask */
#define ETHTRCV_BMCR_RESET_MASK         (0x8000U)

/** \brief BMCR[15] RESET value */
#define ETHTRCV_BMCR_RESET              (0x8000U)

/** \brief BMCR[14] LOOPBACK bit-field mask */
#define ETHTRCV_BMCR_LOOPBACK_MASK      (0x4000U)

/** \brief BMCR[14] LOOPBACK value */
#define ETHTRCV_BMCR_LOOPBACK           (0x4000U)

/** \brief BMCR[12] AUTO-NEGOTIATION ENABLE bit-field mask */
#define ETHTRCV_BMCR_ANEGENABLE_MASK    (0x1000U)

/** \brief BMCR[12] AUTO-NEGOTIATION ENABLE value */
#define ETHTRCV_BMCR_ANEGENABLE         (0x1000U)

/** \brief BMCR[11] POWER DOWN bit-field mask */
#define ETHTRCV_BMCR_POWERDOWN_MASK     (0x0800U)

/** \brief BMCR[11] POWER DOWN value */
#define ETHTRCV_BMCR_POWERDOWN          (0x0800U)

/** \brief BMCR[9] RESTART AUTO-NEGOTIATION bit-field mask */
#define ETHTRCV_BMCR_RESTARTANEG_MASK   (0x0200U)

/** \brief BMCR[9] RESTART AUTO-NEGOTIATION value */
#define ETHTRCV_BMCR_RESTARTANEG        (0x0200U)

/** \brief BMCR[8] DUPLEX MODE bit-field mask */
#define ETHTRCV_BMCR_DUPLEXMODE_MASK    (0x0100U)

/** \brief BMCR[8] DUPLEX MODE half-duplex */
#define ETHTRCV_BMCR_HALFDUPLEX         (0x0000U)

/** \brief BMCR[8] DUPLEX MODE full-duplex */
#define ETHTRCV_BMCR_FULLDUPLEX         (0x0100U)

/** \brief BMCR[13,6] SPEED SELECTION bit-field */
#define ETHTRCV_BMCR_SPEED_MASK         (0x2040U)

/** \brief BMCR[13,6] SPEED SELECTION 10 Mbps */
#define ETHTRCV_BMCR_SPEED_10           (0x0000U)

/** \brief BMCR[13,6] SPEED SELECTION 100 Mbps*/
#define ETHTRCV_BMCR_SPEED_100          (0x2000U)

/** \brief BMCR[13,6] SPEED SELECTION 1000 Mbps */
#define ETHTRCV_BMCR_SPEED_1000         (0x0040U)

/** \brief BMSR[5] AUTO-NEGOTIATION COMPLETE bit-field mask */
#define ETHTRCV_BMSR_ANEGCOMPLETE_MASK  (0x0020U)

/** \brief BMSR[5] AUTO-NEGOTIATION COMPLETE value */
#define ETHTRCV_BMSR_ANEGCOMPLETE       (0x0020U)

/** \brief BMSR[2] LINK STATUS bit-field mask */
#define ETHTRCV_BMSR_LINKSTATUS_MASK    (0x0004U)

/** \brief BMSR[2] LINK STATUS value */
#define ETHTRCV_BMSR_LINKVALID          (0x0004U)

/** \brief ANAR[8:7] 100BASE-TX DUPLEXITY bit-field mask */
#define ETHTRCV_ANAR_100TXDUPLEX_MASK   (0x0180U)

/** \brief ANAR[8:7] 100BASE-TX DUPLEXITY full-duplex */
#define ETHTRCV_ANAR_100TXFULLDUPLEX    (0x0100U)

/** \brief ANAR[8:7] 100BASE-TX DUPLEXITY half-duplex */
#define ETHTRCV_ANAR_100TXHALFDUPLEX    (0x0080U)

/** \brief ANAR[6:5] 10BASE-T DUPLEXITY bit-field mask */
#define ETHTRCV_ANAR_10TDUPLEX_MASK     (0x0060U)

/** \brief ANAR[6:5] 10BASE-T DUPLEXITY full-duplex */
#define ETHTRCV_ANAR_10TFULLDUPLEX      (0x0040U)

/** \brief ANAR[6:5] 10BASE-T DUPLEXITY half-duplex */
#define ETHTRCV_ANAR_10THALFDUPLEX      (0x0020U)

/** \brief ANLPAR[8:7] 100BASE-TX DUPLEXITY bit-field mask */
#define ETHTRCV_ANLPAR_100TXDUPLEX_MASK (0x0180U)

/** \brief ANLPAR[8:7] 100BASE-TX DUPLEXITY full-duplex */
#define ETHTRCV_ANLPAR_100TXFULLDUPLEX  (0x0100U)

/** \brief ANLPAR[8:7] 100BASE-TX DUPLEXITY half-duplex */
#define ETHTRCV_ANLPAR_100TXHALFDUPLEX  (0x0080U)

/** \brief ANLPAR[6:5] 10BASE-T DUPLEXITY bit-field mask */
#define ETHTRCV_ANLPAR_10TDUPLEX_MASK   (0x0060U)

/** \brief ANLPAR[6:5] 10BASE-T DUPLEXITY full-duplex */
#define ETHTRCV_ANLPAR_10TFULLDUPLEX    (0x0040U)

/** \brief ANLPAR[6:5] 10BASE-T DUPLEXITY half-duplex */
#define ETHTRCV_ANLPAR_10THALFDUPLEX    (0x0020U)

/** \brief CFG1[9:8] 1000BASE-T DUPLEX bit-field mask */
#define ETHTRCV_CFG1_1000TDUPLEX_MASK   (0x0300U)

/** \brief CFG1[9:8] 1000BASE-T DUPLEX full-duplex */
#define ETHTRCV_CFG1_1000TFULLDUPLEX    (0x0200U)

/** \brief CFG1[9:8] 1000BASE-T DUPLEX half-duplex */
#define ETHTRCV_CFG1_1000THALFDUPLEX    (0x0100U)

/** \brief STS1[11:10] 1000BASE-T DUPLEX bit-field mask */
#define ETHTRCV_STS1_1000TDUPLEX_MASK   (0x0C00U)

/** \brief STS1[11:10] 1000BASE-T DUPLEX full-duplex */
#define ETHTRCV_STS1_1000TFULLDUPLEX    (0x0800U)

/** \brief STS1[11:10] 1000BASE-T DUPLEX half-duplex */
#define ETHTRCV_STS1_1000THALFDUPLEX    (0x0400U)

/** \brief MMD CR [15:14] Address Function */
#define ETHTRCV_MMD_CR_ADDR             (0x0000U)

/** \brief MMD CR [15:14] Data R/W without Post-Increment */
#define ETHTRCV_MMD_CR_DATA_NOPOSTINC   (0x4000U)

/** \brief MMD CR [15:14] Data R/W with Post-Increment */
#define ETHTRCV_MMD_CR_DATA_POSTINC_RW  (0x8000U)

/** \brief MMD CR [15:14] Data R/W with Post-Increment for writes
 *  and no Post-Increment for reads */
#define ETHTRCV_MMD_CR_DATA_POSTINC_W   (0xC000U)

/** \brief MMD CR [4:0] Device Address */
#define ETHTRCV_MMD_CR_DEVADDR          (0x001FU)

/*! @} */

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/**
 *  \brief  Ethernet transceiver
 *
 *  Ethernet transceiver in the context of the EthTrcv driver.
 */
typedef struct EthTrcv_Trcv_s
{
    uint8 trcvIdx;
    /**< Transceived index */
    uint8 ctrlIdx;
    /**< Controller index */
    uint8 miiIdx;
    /**< MII index (PHY address) */
    boolean modeChanged;
    /**< Mode has changed */
    EthTrcv_ModeType mode;
    /**< Current transceiver mode */
    EthTrcv_ControllerConfigType ethTrcvCfg;
    /**< Saved EthTrcv driver configuration information */
    boolean isInit;
    /**< Transceiver init state */
} EthTrcv_Trcv;

/**
 *  \brief Ethernet transceiver driver
 *
 *  Ethernet transceiver driver.
 */
typedef struct EthTrcv_Drv_s
{
    EthTrcv_Trcv trcv[ETHTRCV_MAX_CONTROLLER];
    /**< Transceivers supported by the EthTrcv driver */
} EthTrcv_Drv;

/* ========================================================================== */
/*                         Global Variables Declarations                      */
/* ========================================================================== */

extern VAR(volatile boolean, ETHTRCV_VAR_CLEARED) mdioRdCmdComplete;
extern VAR(volatile boolean, ETHTRCV_VAR_CLEARED) mdioWrCmdComplete;
extern VAR(volatile uint16, ETHTRCV_VAR_CLEARED) mdioRdVal;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  \brief Reset the Ethernet transceiver
 *
 *  Resets the Ethernet transceiver and waits for reset to complete.  Time
 *  out can happen if the reset completion is not detected.
 *
 *  \param trcv     Transceiver handle
 *
 *  \return E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType EthTrcv_reset(EthTrcv_Trcv *trcv);

/**
 *  \brief Configure the Ethernet transceiver (extended configuration)
 *
 *  Perform the extended configuration of the Ethernet transceiver
 *  (non IEEE-standard registers).
 *
 *  \param trcv     Transceiver handle
 *  \param cfg      Transceiver extended configuration
 *
 *  \return E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType EthTrcv_config(EthTrcv_Trcv *trcv,
                              const void *extendedCfg);

/**
 *  \brief Enable loopback mode
 *
 *  Sets the transceiver in loopback mode.
 *
 *  \param trcv     Transceiver handle
 *  \param enable   Enable or disable loopback
 *
 *  \return E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType EthTrcv_enableLoopback(EthTrcv_Trcv *trcv,
                                      boolean enable);

/**
 *  \brief Configure and start auto-negotiation
 *
 *  Configures and starts the auto-negotiation process.
 *
 *  \param trcv     Transceiver handle
 *  \param cfg      Configuration information
 *
 *  \return E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType EthTrcv_configAndStartANeg(EthTrcv_Trcv *trcv,
                                          EthTrcv_ControllerConfigType *cfg);

/**
 *  \brief Check if link is valid
 *
 *  Checks if the link is in valid state by querying the status
 *  reported in the BMSR register.
 *
 *  \param trcv     Transceiver handle
 *
 *  \return         TRUE is valid, FALSE otherwise
 */
boolean EthTrcv_isLinkValid(EthTrcv_Trcv *trcv);

/**
 *  \brief Read a transceiver register
 *
 *  Reads a PHY register.
 *
 *  \param trcv     Transceiver handle
 *  \param reg      Register address
 *  \param val      Value variable pointer
 *
 *  \return E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType EthTrcv_readReg(EthTrcv_Trcv *trcv,
                               uint8 reg,
                               uint16 *val);

/**
 *  \brief Write a transceiver register
 *
 *  Writes a PHY register.
 *
 *  \param trcv     Transceiver handle
 *  \param reg      Register address
 *  \param val      Value
 *
 *  \return E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType EthTrcv_writeReg(EthTrcv_Trcv *trcv,
                                uint8 reg,
                                uint16 val);

/**
 *  \brief Read-modify-write a transceiver register
 *
 *  Reads, modifies and writes a PHY register.  A mask is applied to the read
 *  value before the new value is set.  The updated value is then written
 *  back to the same PHY register.
 *
 *  \param trcv     Transceiver handle
 *  \param reg      Register address
 *  \param mask     Bit mask to be applied to the read value
 *  \param val      New value
 *
 *  \return E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType EthTrcv_rmwReg(EthTrcv_Trcv *trcv,
                              uint8 reg,
                              uint16 mask,
                              uint16 val);

/**
 *  \brief Check if transceiver can be accessed
 *
 *  Checks if the transceiver can be accessed. A Diagnostics Event Manager
 *  (DEM) is raised if the transceiver can't be accessed.
 *
 *  \param trcv     Transceiver handle
 *
 *  \return E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType EthTrcv_checkTransceiverAccess(EthTrcv_Trcv *trcv);

void EthTrcv_dumpRegs(EthTrcv_Trcv *trcv);

#ifdef __cplusplus
}
#endif

#endif /* ETHTRCV_PRIV_H_ */
