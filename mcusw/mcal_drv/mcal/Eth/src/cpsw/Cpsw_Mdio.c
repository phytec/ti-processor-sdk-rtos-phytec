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
 *  \file     Cpsw_Mdio.c
 *
 *  \brief    This file contains MDIO related functionality.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "cpsw/Cpsw_Types.h"
#include "cpsw/Cpsw_Mdio.h"
#include "Os.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 *  \anchor MDIO Clause22 Bit definitions
 *  \name MDIO Clause22 Bit definitions
 *
 *  @{
 */
/** \brief Preamble length (32 bits) */
#define CPSW_MDIO_PRE32_LEN      (32)

/** \brief Start (2 bits) and Opcode (2 bits) field length */
#define CPSW_MDIO_ST_OP_LEN      (4)

/** \brief PHY address field length (5 bits) */
#define CPSW_MDIO_PA5_LEN        (5)

/** \brief REGADR (5 bits). PHY register address field length */
#define CPSW_MDIO_RA5_LEN        (5)

/** \brief Turn-around field length (2 bits) */
#define CPSW_MDIO_TA_LEN         (2)

/** \brief Data field length (16 bits) */
#define CPSW_MDIO_D16_LEN        (16)
/*! @} */

/**
 *  \anchor MDIO Function (Clause22)
 *  \name MDIO Function (Clause22)
 *
 *  Standard MDIO Functions for Clause22 (aka Article 22)
 *  i.e. Function = ST + OpCode
 *  Where:
 *   - ST (2 bits), Start of Frame (01 for Clause22)
 *   - OpCode (2 bits),
 *     - 01 for WRITE Operation
 *     - 10 for READ Operation
 *  @{
 */
/** \brief MDIO Write Function for Clause22 */
#define CPSW_MDIO_WRITE_FUNC    0x05U

/** \brief MDIO Read Function for Clause22 */
#define CPSW_MDIO_READ_FUNC     0x06U
/*! @} */

/**
 *  \anchor MDIO Turn-Around Bits (Clause22)
 *  \name MDIO Turn-Around Bits (Clause22)
 *
 *  Standard MDIO turn-around bits for Clause-22 (aka Article 22)
 *   - 10 for WRITE Operation
 *  @{
 */
/** \brief MDIO Write Function for Clause22 */
#define CPSW_MDIO_TA_BITS       0x02U
/*! @} */

/**
 *  \anchor MDIO Preamble Bits (Clause22)
 *  \name MDIO Preamble Bits (Clause22)
 *
 *  Standard MDIO preamble bits for Clause-22 (aka Article 22) which
 *  consists of 32 ones sent by the MAC on the MDIO line.
 *  @{
 */
/** \brief MDIO Preamble Bits for Clause22 */
#define CPSW_MDIO_PRE32_BITS    0xFFFFFFFFU
/*! @} */

/**
 *  \anchor PHY_Registers
 *  \name PHY registers
 *
 *  Standard PHY register address and definitions.
 *
 *  @{
 */
/** \brief Basic Mode Control Register (BMCR) */
#define CPSW_MDIO_BMCR          (0x00U)

/** \brief Basic Mode Status Register (BMSR) */
#define CPSW_MDIO_BMSR          (0x01U)

/** \brief BMSR[2] LINK STATUS bit-field mask */
#define CPSW_MDIO_BMSR_LINKSTS  (1U << 2U)
/*! @} */

/** \brief Invalid Data Value to be set when Read Operation Failed */
#define CPSW_MDIO_INV_DATA_VAL  (0xFFFFU)

/** \brief one second in unit of microsecond */
#define CPSW_MDIO_SEC_TO_NSEC   (1000000000ULL)

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/**
 * \brief MDIO bit banging delay in terms of nanosecond.
 *        Applicable only when in Manual MDIO Operating Mode
 */
static uint32 mdcHalfPeriodNsec = 0U;

/* ========================================================================== */
/*                  Internal Function Declarations                            */
/* ========================================================================== */

/**
 *  \brief Get base address of CPSW MDIO register set
 *
 *  \return  Base address of CPSW MDIO register set
 */
static inline CSL_mdioHandle CpswMdio_getRegAddr(void);

/**
 * \brief Write MDIO manual interface register.
 *
 * \return None
 */
static inline void CpswMdio_setManualIf(CSL_mdioHandle hMdioRegs,
                                        uint32 val);

/**
 * \brief Read MDIO manual interface register.
 *
 * \return Register value
 */
static inline uint32 CpswMdio_getManualIf(CSL_mdioHandle hMdioRegs);

/**
 * \brief Read MDIO PIN Register Value
 *
 * \return MDIO PIN field value
 */
static inline uint32 CpswMdio_readMdi(CSL_mdioHandle hMdioRegs);

/**
 * \brief Clear Poll Enable Mask
 *
 * \return None
 */
static inline void CpswMdio_clearPollEnableMask(CSL_mdioHandle hMdioRegs);

/**
 * \brief Enableds MDIO Manual Mode
 *
 * \return None
 */
static void CpswMdio_enableManualMode(CSL_mdioHandle hMdioRegs);

/**
 * \brief Perfom a cpu delay
 *
 * \return None
 */
static void CpswMdio_delayNsec(uint32 delayInNsec);

/**
 * \brief Toggle MDC Clock for Reg Write.
 *
 * \return None
 */
static void CpswMdio_swToggleMdclkWrite(uint8 ctrlIdx,
                                        uint32 *manualIf);

/**
 * \brief Toggle MDC Clock for Reg Read.
 *
 * \return None
 */
static void CpswMdio_swToggleMdclkRead(uint8 ctrlIdx,
                                       uint32 *manualIf);

/**
 * \brief Sends MDIO field value.
 *
 * \return None
 */
static void CpswMdio_swFieldSend(uint8 ctrlIdx,
                                 sint32 len,
                                 uint32 val,
                                 uint32 *manualIf);

/**
 * \brief Receives MDIO field value (up to 16 bits).
 *
 * \return Read value
 */
static uint16 CpswMdio_swFieldRecv(uint8 ctrlIdx,
                                   sint32 len,
                                   uint32 *manualIf);

/**
 * \brief Discards MDIO field value.
 *
 * \return Error value
 */
static uint16 CpswMdio_swFieldDiscard(uint8 ctrlIdx,
                                      sint32 len,
                                      uint32 *manualIf);

/**
 * \brief MDIO Manual Phy Reg Read (Clause 22)
 *
 * \return E_OK if Phy Read is successful else E_NOT_OK
 */
static Std_ReturnType CpswMdio_manualPhyRegRead22(uint8 ctrlIdx,
                                                  uint32 phyAddr,
                                                  uint32 regNum,
                                                  uint16 *val);

/**
 * \brief MDIO Manual Phy Reg Write (Clause 22)
 *
 * \return None
 */
static void CpswMdio_manualPhyRegWrite22(uint8 ctrlIdx,
                                         uint32 phyAddr,
                                         uint32 regNum,
                                         uint16 val);

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

void CpswMdio_open(void)
{
    CSL_mdioHandle hMdioRegs = CpswMdio_getRegAddr();
    uint32 cppiClkFreqHz = Eth_Cpsw_GetCppiClockFreq();
    uint32 busFreq = Eth_Cpsw_GetMdioBusClockFreq();
    uint32 retVal = CPSW_SOK;
    uint32 clkdiv;

    /* sanity check */
    if (busFreq != 0U)
    {
        clkdiv = (cppiClkFreqHz / busFreq) - 1U;
        if (clkdiv > 65535U)
        {
            retVal = CPSW_EINVALID_PARAMS;
        }
    }
    else
    {
        retVal = CPSW_EBADARGS;
    }

    if (CPSW_SOK == retVal)
    {
        if ((ETH_MDIO_OPMODE_MANUAL == Eth_Cpsw_GetMdioOpMode()) ||
            (FALSE == Eth_Cpsw_GetMdioEnableInterrupt()))
        {
            /* Disable MDIO user command complete interrupt for user group 0 */
            CSL_REG32_FINS(&hMdioRegs->USER_INT_MASK_CLEAR_REG,
                           MDIO_USER_INT_MASK_CLEAR_REG_USERINTMASKCLR, 1U);
        }
        else
        {
            /* Enable MDIO user command complete interrupt for user group 0 */
            CSL_REG32_FINS(&hMdioRegs->USER_INT_MASK_SET_REG,
                           MDIO_USER_INT_MASK_SET_REG_USERINTMASKSET, 1U);
        }

        /* Clear the USER_ACCESS_REG */
        CSL_REG32_WR(&hMdioRegs->USER_GROUP[0U].USER_ACCESS_REG, 0U);

        if (ETH_MDIO_OPMODE_MANUAL == Eth_Cpsw_GetMdioOpMode())
        {
            /* compute for MDC Half Period */
            mdcHalfPeriodNsec = (uint32)((CPSW_MDIO_SEC_TO_NSEC/busFreq)/2U);

            /* enable mdio manual mode */
            CpswMdio_clearPollEnableMask(hMdioRegs);
            CpswMdio_enableManualMode(hMdioRegs);
        }
        else
        {
            /* Set the MDIO clock divider */
            CSL_MDIO_setClkDivVal(hMdioRegs, (uint16)clkdiv);

            /* Enable the state machine */
            CSL_MDIO_enableStateMachine(hMdioRegs);
        }
    }
}

void CpswMdio_close(void)
{
    /* Disable the state machine */
    CSL_MDIO_disableStateMachine(CpswMdio_getRegAddr());
}

boolean CpswMdio_isPhyAlive(uint8 ctrlIdx, uint32 phyAddr)
{
    boolean isAlive = FALSE;
    if (ETH_MDIO_OPMODE_MANUAL == Eth_Cpsw_GetMdioOpMode())
    {
        Std_ReturnType retVal;
        uint16 bmsrVal;

        retVal = CpswMdio_manualPhyRegRead22(ctrlIdx,
                                             phyAddr,
                                             CPSW_MDIO_BMSR,
                                             &bmsrVal);
        if (E_OK == retVal)
        {
            isAlive = TRUE;
        }
    }
    else
    {
        isAlive = (CSL_MDIO_isPhyAlive(CpswMdio_getRegAddr(), phyAddr) != 0U) ?
                  TRUE : FALSE;
    }
    return isAlive;
}

void CpswMdio_writePhyReg(uint8 ctrlIdx,
                          uint32 phyAddr,
                          uint32 reg,
                          uint16 val)
{
    CpswMdio_manualPhyRegWrite22(ctrlIdx, phyAddr, reg, val);
}

Std_ReturnType CpswMdio_readPhyReg(uint8 ctrlIdx,
                                   uint32 phyAddr,
                                   uint32 reg,
                                   uint16 *val)
{
    return CpswMdio_manualPhyRegRead22(ctrlIdx, phyAddr, reg, val);
}

Std_ReturnType CpswMdio_initiatePhyRegWrite(uint32 phyAddr,
                                            uint32 reg,
                                            uint16 val)
{
    CSL_mdioHandle hMdioRegs = CpswMdio_getRegAddr();
    Std_ReturnType retVal = E_OK;
    uint32 userAccess = 0U;

    if (0U == CSL_REG32_FEXT(&hMdioRegs->USER_GROUP[0U].USER_ACCESS_REG,
                             MDIO_USER_GROUP_USER_ACCESS_REG_GO))
    {
        CSL_FINS(userAccess,
                 MDIO_USER_GROUP_USER_ACCESS_REG_GO,
                 CSL_MDIO_USER_GROUP_USER_ACCESS_REG_GO_EN_0x1);

        CSL_FINS(userAccess,
                 MDIO_USER_GROUP_USER_ACCESS_REG_WRITE,
                 CSL_MDIO_USER_GROUP_USER_ACCESS_REG_WRITE);

        CSL_FINS(userAccess,
                 MDIO_USER_GROUP_USER_ACCESS_REG_PHYADR,
                 phyAddr);

        CSL_FINS(userAccess,
                 MDIO_USER_GROUP_USER_ACCESS_REG_REGADR,
                 reg);

        CSL_FINS(userAccess,
                 MDIO_USER_GROUP_USER_ACCESS_REG_DATA,
                 (uint32)val);

        CSL_REG32_WR(&hMdioRegs->USER_GROUP[0U].USER_ACCESS_REG, userAccess);
    }
    else
    {
        retVal = E_NOT_OK;
    }

    return retVal;
}

Std_ReturnType CpswMdio_initiatePhyRegRead(uint32 phyAddr, uint32 reg)
{
    CSL_mdioHandle hMdioRegs = CpswMdio_getRegAddr();
    Std_ReturnType retVal = E_OK;
    uint32 userAccess = 0U;

    if (0U == CSL_REG32_FEXT(&hMdioRegs->USER_GROUP[0U].USER_ACCESS_REG,
                             MDIO_USER_GROUP_USER_ACCESS_REG_GO))
    {
        CSL_FINS(userAccess,
                 MDIO_USER_GROUP_USER_ACCESS_REG_GO,
                 CSL_MDIO_USER_GROUP_USER_ACCESS_REG_GO_EN_0x1);

        CSL_FINS(userAccess,
                 MDIO_USER_GROUP_USER_ACCESS_REG_WRITE,
                 CSL_MDIO_USER_GROUP_USER_ACCESS_REG_READ);

        CSL_FINS(userAccess,
                 MDIO_USER_GROUP_USER_ACCESS_REG_PHYADR,
                 phyAddr);

        CSL_FINS(userAccess,
                MDIO_USER_GROUP_USER_ACCESS_REG_REGADR,
                reg);

        CSL_REG32_WR(&hMdioRegs->USER_GROUP[0U].USER_ACCESS_REG, userAccess);
    }
    else
    {
        retVal = E_NOT_OK;
    }

    return retVal;
}

Std_ReturnType CpswMdio_getPhyRegVal(uint8 ctrlIdx,
                                     uint16 *val)
{
    CSL_mdioHandle hMdioRegs = CpswMdio_getRegAddr();
    Std_ReturnType retVal = E_OK;

    if (CSL_MDIO_USER_GROUP_USER_ACCESS_REG_ACK_PASS ==
        CSL_REG32_FEXT(&hMdioRegs->USER_GROUP[0U].USER_ACCESS_REG,
                       MDIO_USER_GROUP_USER_ACCESS_REG_ACK))
    {
        *val = (uint16)CSL_REG32_FEXT(&hMdioRegs->USER_GROUP[0U].USER_ACCESS_REG,
                                      MDIO_USER_GROUP_USER_ACCESS_REG_DATA);
    }
    else
    {
        retVal = E_NOT_OK;
    }

    return retVal;
}

void CpswMdio_clearUserInt(void)
{
    /* Clear the User Command Complete interrupt */
    CSL_REG32_FINS(&CpswMdio_getRegAddr()->USER_INT_MASKED_REG,
                   MDIO_USER_INT_MASKED_REG_USERINTMASKED,
                   1U);
}

boolean CpswMdio_isPhyRegReadWritePending(void)
{
    boolean retVal = TRUE;

    if (0U == CSL_REG32_FEXT(&CpswMdio_getRegAddr()->USER_GROUP[0U].USER_ACCESS_REG,
                             MDIO_USER_GROUP_USER_ACCESS_REG_GO))
    {
        retVal = FALSE;
    }

    return retVal;
}

/* ========================================================================== */
/*                  Internal Function Definitions                             */
/* ========================================================================== */

static inline CSL_mdioHandle CpswMdio_getRegAddr(void)
{
    return (CSL_mdioHandle)Eth_Cpsw_GetMdioRegAddr();
}

static inline void CpswMdio_setManualIf(CSL_mdioHandle hMdioRegs,
                                        uint32 val)
{
    hMdioRegs->MANUAL_IF_REG = val;
}

static inline uint32 CpswMdio_getManualIf(CSL_mdioHandle hMdioRegs)
{
    return hMdioRegs->MANUAL_IF_REG;
}

static inline uint32 CpswMdio_readMdi(CSL_mdioHandle hMdioRegs)
{
    return CSL_FEXT(hMdioRegs->MANUAL_IF_REG, MDIO_MANUAL_IF_REG_MDIO_PIN);
}

static inline void CpswMdio_clearPollEnableMask(CSL_mdioHandle hMdioRegs)
{
    /* Due to a hardware limitation, bit 31 must always be set */
    hMdioRegs->POLL_EN_REG = (((uint32)1U) << 31U);
}

static void CpswMdio_enableManualMode(CSL_mdioHandle hMdioRegs)
{
    CSL_FINS(hMdioRegs->POLL_REG, MDIO_POLL_REG_STATECHANGEMODE, ((uint32)1U));
    CSL_FINS(hMdioRegs->POLL_REG, MDIO_POLL_REG_MANUALMODE, ((uint32)1U));
}

static void CpswMdio_delayNsec(uint32 delayInNsec)
{
#ifdef CPSW_MDIO_DELAY_ENABLE
    TickType delayInTicks = (TickType)(((uint64)delayInNsec*ETH_OS_COUNTER_FREQ)/
                            CPSW_MDIO_SEC_TO_NSEC);
    TickType curTick = 0U;
    TickType tempCount = 0U;
    TickType elapsed = 0U;
    StatusType status;

    status = GetCounterValue(ETH_OS_COUNTERctrlIdx, &curTick);

    while ((E_OK == status) && (delayInTicks >= elapsed))
    {
        NOP50;
        tempCount = curTick;
        status = GetElapsedValue(ETH_OS_COUNTER_ID, &tempCount, &elapsed);
    }
#endif
}

static void CpswMdio_swToggleMdclkWrite(uint8 ctrlIdx,
                                        uint32 *manualIf)
{
    CSL_mdioHandle hMdioRegs = CpswMdio_getRegAddr();

    /* MDIO clock falling edge */
    CSL_FINS(*manualIf, MDIO_MANUAL_IF_REG_MDIO_MDCLK_O, 0U);
    CpswMdio_setManualIf(hMdioRegs, *manualIf);

    /* MDIO clock delay on low */
    Eth_GetMdioWriteLowDelayNsec(ctrlIdx);

    /* MDIO clock rising edge */
    CSL_FINS(*manualIf, MDIO_MANUAL_IF_REG_MDIO_MDCLK_O, 1U);
    CpswMdio_setManualIf(hMdioRegs, *manualIf);

    /* MDIO clock delay on high */
    Eth_GetMdioWriteHighDelayNsec(ctrlIdx);
}

static void CpswMdio_swToggleMdclkRead(uint8 ctrlIdx,
                                       uint32 *manualIf)
{
    CSL_mdioHandle hMdioRegs = CpswMdio_getRegAddr();

    /* MDIO clock falling edge */
    CSL_FINS(*manualIf, MDIO_MANUAL_IF_REG_MDIO_MDCLK_O, 0U);
    CpswMdio_setManualIf(hMdioRegs, *manualIf);

    /* MDIO clock delay on low */
    Eth_GetMdioReadLowDelayNsec(ctrlIdx);

    /* MDIO clock rising edge */
    CSL_FINS(*manualIf, MDIO_MANUAL_IF_REG_MDIO_MDCLK_O, 1U);
    CpswMdio_setManualIf(hMdioRegs, *manualIf);

    /* MDIO clock delay on high */
    Eth_GetMdioReadHighDelayNsec(ctrlIdx);
}

static void CpswMdio_swFieldSend(uint8 ctrlIdx,
                                 sint32 len,
                                 uint32 val,
                                 uint32 *manualIf)
{
    CSL_mdioHandle hMdioRegs = CpswMdio_getRegAddr();
    uint32 mdo;
    sint32 i;

    for (i = (len - 1); i >= 0; i--)
    {
        mdo = (val >> i) & 1U;

        /* Drive MDIO data line at MDIO clock falling edge */
        CSL_FINS(*manualIf, MDIO_MANUAL_IF_REG_MDIO_PIN, mdo);
        CSL_FINS(*manualIf, MDIO_MANUAL_IF_REG_MDIO_MDCLK_O, 0U);
        CpswMdio_setManualIf(hMdioRegs, *manualIf);

        /* MDIO write clock delay on low */
        Eth_GetMdioWriteLowDelayNsec(ctrlIdx);

        /* MDIO clock rising edge */
        CSL_FINS(*manualIf, MDIO_MANUAL_IF_REG_MDIO_MDCLK_O, 1U);
        CpswMdio_setManualIf(hMdioRegs, *manualIf);

        /* MDIO write clock delay on high */
        Eth_GetMdioWriteHighDelayNsec(ctrlIdx);
    }
}

static uint16 CpswMdio_swFieldRecv(uint8 ctrlIdx,
                                   sint32 len,
                                   uint32 *manualIf)
{
    CSL_mdioHandle hMdioRegs = CpswMdio_getRegAddr();
    uint32 mdi;
    uint16 val;
    sint32 i;

    val = 0U;

    for (i = (len - 1); i >= 0; i--)
    {
        mdi = CpswMdio_readMdi(hMdioRegs);
        val |= ((uint16)mdi & 1U) << i;
        /* MDIO read with clock delay */
        CpswMdio_swToggleMdclkRead(ctrlIdx, manualIf);
    }

    return val;
}

static uint16 CpswMdio_swFieldDiscard(uint8 ctrlIdx,
                                      sint32 len,
                                      uint32 *manualIf)
{
    sint32 i;

    for (i = (len - 1); i >= 0; i--)
    {
        /* MDIO write clock delay */
        CpswMdio_swToggleMdclkWrite(ctrlIdx, manualIf);
    }

    return CPSW_MDIO_INV_DATA_VAL;
}


/**
 * \brief MDIO Manual Phy Reg Read (Clause 22)
 */
static Std_ReturnType CpswMdio_manualPhyRegRead22(uint8 ctrlIdx,
                                                  uint32 phyAddr,
                                                  uint32 regNum,
                                                  uint16 *val)
{
    CSL_mdioHandle hMdioRegs = CpswMdio_getRegAddr();
    Std_ReturnType retVal = E_OK;
    uint32 manualIf;
    uint32 ack;

    manualIf = CpswMdio_getManualIf(hMdioRegs);

    /* Disable PHY interrupt driver */
    CSL_FINS(manualIf, MDIO_MANUAL_IF_REG_MDIO_MDCLK_O, 0U);
    CpswMdio_setManualIf(hMdioRegs, manualIf);

    /* Enable our drive capability */
    CSL_FINS(manualIf, MDIO_MANUAL_IF_REG_MDIO_OE, 1U);
    CpswMdio_setManualIf(hMdioRegs, manualIf);

    /* Send 32-bit preamble */
    CpswMdio_swFieldSend(ctrlIdx, CPSW_MDIO_PRE32_LEN, CPSW_MDIO_PRE32_BITS, &manualIf);

    /* Issue clause 22 MII read function {0,1,1,0} */
    CpswMdio_swFieldSend(ctrlIdx, CPSW_MDIO_ST_OP_LEN, CPSW_MDIO_READ_FUNC, &manualIf);

    /* Send the device number MSB first */
    CpswMdio_swFieldSend(ctrlIdx, CPSW_MDIO_PA5_LEN, phyAddr, &manualIf);

    /* Send the register number MSB first */
    CpswMdio_swFieldSend(ctrlIdx, CPSW_MDIO_RA5_LEN, regNum, &manualIf);

    /* Send turn-around cycles */
    CSL_FINS(manualIf, MDIO_MANUAL_IF_REG_MDIO_OE, 0U);
    CpswMdio_swToggleMdclkWrite(ctrlIdx, &manualIf);

    /* Get PHY ack */
    ack = CpswMdio_readMdi(hMdioRegs);
    CpswMdio_swToggleMdclkRead(ctrlIdx, &manualIf);

    if (ack == 0U) /* If acked read the data */
    {
        *val = CpswMdio_swFieldRecv(ctrlIdx, CPSW_MDIO_D16_LEN, &manualIf);
        retVal = E_OK;
    }
    else
    {
        *val = CpswMdio_swFieldDiscard(ctrlIdx, CPSW_MDIO_D16_LEN, &manualIf);
        retVal = E_NOT_OK;
    }

    /* Give time for pull-up to work */
    CSL_FINS(manualIf, MDIO_MANUAL_IF_REG_MDIO_MDCLK_O, 0U);
    CpswMdio_setManualIf(hMdioRegs, manualIf);
    CpswMdio_setManualIf(hMdioRegs, manualIf);
    CpswMdio_setManualIf(hMdioRegs, manualIf);

    /* Re-enable PHY interrupt function */
    CpswMdio_swToggleMdclkWrite(ctrlIdx, &manualIf);

    return retVal;
}

/**
 * \brief MDIO Manual Phy Reg Write (Clause 22)
 */
static void CpswMdio_manualPhyRegWrite22(uint8 ctrlIdx,
                                         uint32 phyAddr,
                                         uint32 regNum,
                                         uint16 val)
{
    CSL_mdioHandle hMdioRegs = CpswMdio_getRegAddr();
    uint32 manualIf;

    manualIf = CpswMdio_getManualIf(hMdioRegs);

    /* Disable PHY interrupt driver */
    CSL_FINS(manualIf, MDIO_MANUAL_IF_REG_MDIO_MDCLK_O, 0U);
    CpswMdio_setManualIf(hMdioRegs, manualIf);

    /* Enable our drive capability */
    CSL_FINS(manualIf, MDIO_MANUAL_IF_REG_MDIO_OE, 1U);
    CpswMdio_setManualIf(hMdioRegs, manualIf);

    /* Send 32-bit preamble */
    CpswMdio_swFieldSend(ctrlIdx, CPSW_MDIO_PRE32_LEN, CPSW_MDIO_PRE32_BITS, &manualIf);

    /* Issue clause 22 MII write function {0,1,0,1} */
    CpswMdio_swFieldSend(ctrlIdx, CPSW_MDIO_ST_OP_LEN, CPSW_MDIO_WRITE_FUNC, &manualIf);

    /* Send the device number MSB first */
    CpswMdio_swFieldSend(ctrlIdx, CPSW_MDIO_PA5_LEN, phyAddr, &manualIf);

    /* Send the register number MSB first */
    CpswMdio_swFieldSend(ctrlIdx, CPSW_MDIO_RA5_LEN, regNum, &manualIf);

    /* Send turn-around cycles */
    CpswMdio_swFieldSend(ctrlIdx, CPSW_MDIO_TA_LEN, CPSW_MDIO_TA_BITS, &manualIf);

    /* Send Register data MSB first */
    CpswMdio_swFieldSend(ctrlIdx, CPSW_MDIO_D16_LEN, val, &manualIf);

    CSL_FINS(manualIf, MDIO_MANUAL_IF_REG_MDIO_OE, 0U);
    CpswMdio_setManualIf(hMdioRegs, manualIf);

    /* Give time for pull-up to work */
    CSL_FINS(manualIf, MDIO_MANUAL_IF_REG_MDIO_MDCLK_O, 0U);
    CpswMdio_setManualIf(hMdioRegs, manualIf);
    CpswMdio_setManualIf(hMdioRegs, manualIf);
    CpswMdio_setManualIf(hMdioRegs, manualIf);

    /* Re-enable PHY interrupt function */
    CpswMdio_swToggleMdclkWrite(ctrlIdx, &manualIf);
}