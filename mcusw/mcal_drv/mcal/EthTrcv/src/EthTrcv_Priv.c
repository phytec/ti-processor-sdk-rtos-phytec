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
 *  \file     EthTrcv_Priv.c
 *
 *  \brief    This file contains Ethernet Transceiver internal driver.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "EthTrcv.h"
#if (STD_ON == ETHTRCV_DEV_ERROR_DETECT)
#include "Det.h"
#endif
#include "Dem.h"
#include "Os.h"
#include "SchM_EthTrcv.h"

#include "Eth.h"
#if (STD_ON == ETH_WRAPPER_ENABLED)
#include "Eth_30_Wrapper.h"
#endif /* (STD_ON == ETH_WRAPPER_ENABLED) */
#include "EthTrcv_Priv.h"
#include "phy/Dp83867.h"
#include "Dp83867_Priv.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
#define DP83869_PHYID                   ((uint32) 0x2000A0F1)

#define ETHTRCV_TICK_MS                 (100U)
#define ETHTRCV_AUTONEG_LINKED_WAIT_S   (4U)
#define ETHTRCV_AUTONEG_RETRY_NUM       (320000U)
#define ETHTRCV_LINKED_RETRY_NUM        (200000U)
#define ETHTRCV_RESET_RETRY_NUM         (10000U)

#define ETHTRCV_DIV_ROUNDUP(val, div)   (((val) + (div) - 1) / (div))

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */

static inline boolean EthTrcv_supportPhyId(uint32 PhyId);

static Std_ReturnType EthTrcv_getSelfAbility(EthTrcv_Trcv *trcv,
                                             uint32 *pCapab,
                                             uint32  isGigCapab);

static Std_ReturnType EthTrcv_getPartnerAbility(EthTrcv_Trcv *trcv,
                                                uint32 *pPartnerAblty,
                                                uint32  isGigCapab);

static Std_ReturnType EthTrcv_updateCapab(EthTrcv_Trcv *trcv,
                                          EthTrcv_ControllerConfigType *cfg);

static Std_ReturnType EthTrcv_waitForAutoNeg(EthTrcv_Trcv *trcv);

Std_ReturnType EthTrcv_readExtReg(EthTrcv_Trcv *trcv,
                                  uint16 reg,
                                  uint16 *val);

Std_ReturnType EthTrcv_writeExtReg(EthTrcv_Trcv *trcv,
                                   uint16 reg,
                                   uint16 val);

Std_ReturnType EthTrcv_rmwExtReg(EthTrcv_Trcv *trcv,
                                 uint16 reg,
                                 uint16 mask,
                                 uint16 val);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                  Internal Function Definitions                             */
/* ========================================================================== */

static inline boolean EthTrcv_supportPhyId(uint32 phyId)
{
    boolean retVal = FALSE;
    if ((phyId == DP83867_PHYID) || (phyId == DP83869_PHYID))
    {
        retVal = TRUE;
    }

    return retVal;
}

static Std_ReturnType EthTrcv_getSelfAbility(EthTrcv_Trcv *trcv,
                                             uint32 *pCapab,
                                             uint32  isGigCapab)
{
    uint16 val;
    Std_ReturnType retVal;

    *pCapab = 0U;

    /* Read the ANAR register (addr 0x04) and get the 10 and
     * 100 Mbps capabilities */
    retVal = EthTrcv_readReg(trcv, ETHTRCV_ANAR, &val);
    if (E_OK == retVal)
    {
        if (ETHTRCV_ANAR_10THALFDUPLEX ==
            (val & ETHTRCV_ANAR_10THALFDUPLEX))
        {
            *pCapab |= ETHERNET_ADV_CAPAB_10_HALF;
        }

        if (ETHTRCV_ANAR_10TFULLDUPLEX ==
            (val & ETHTRCV_ANAR_10TFULLDUPLEX))
        {
            *pCapab |= ETHERNET_ADV_CAPAB_10_FULL;
        }

        if (ETHTRCV_ANAR_100TXHALFDUPLEX ==
            (val & ETHTRCV_ANAR_100TXHALFDUPLEX))
        {
            *pCapab |= ETHERNET_ADV_CAPAB_100_HALF;
        }

        if (ETHTRCV_ANAR_100TXFULLDUPLEX ==
            (val & ETHTRCV_ANAR_100TXFULLDUPLEX))
        {
            *pCapab |= ETHERNET_ADV_CAPAB_100_FULL;
        }
    }

    /* Read the CFG1 register (addr 0x09) and get the 1 Gbps capabilities */
    if ((E_OK == retVal) && (TRUE == isGigCapab))
    {
        retVal = EthTrcv_readReg(trcv, ETHTRCV_CFG1, &val);
        if (E_OK == retVal)
        {
            if (ETHTRCV_CFG1_1000THALFDUPLEX ==
                (val & ETHTRCV_CFG1_1000THALFDUPLEX))
            {
                *pCapab |= ETHERNET_ADV_CAPAB_1000_HALF;
            }

            if (ETHTRCV_CFG1_1000TFULLDUPLEX ==
                (val & ETHTRCV_CFG1_1000TFULLDUPLEX))
            {
                *pCapab |= ETHERNET_ADV_CAPAB_1000_FULL;
            }
        }
    }

    return retVal;
}

static Std_ReturnType EthTrcv_getPartnerAbility(EthTrcv_Trcv *trcv,
                                                uint32 *pPartnerAblty,
                                                uint32  isGigCapab)
{
    uint16 val;
    Std_ReturnType retVal;

    *pPartnerAblty = 0U;

    /* Read the ANLPAR register (addr 0x05) and get the 10 and
     * 100 Mbps capabilities */
    retVal = EthTrcv_readReg(trcv, ETHTRCV_ANLPAR, &val);
    if (E_OK == retVal)
    {
        if (ETHTRCV_ANLPAR_10THALFDUPLEX ==
            (val & ETHTRCV_ANLPAR_10THALFDUPLEX))
        {
            *pPartnerAblty |= ETHERNET_ADV_CAPAB_10_HALF;
        }

        if (ETHTRCV_ANLPAR_10TFULLDUPLEX ==
            (val & ETHTRCV_ANLPAR_10TFULLDUPLEX))
        {
            *pPartnerAblty |= ETHERNET_ADV_CAPAB_10_FULL;
        }

        if (ETHTRCV_ANLPAR_100TXHALFDUPLEX ==
            (val & ETHTRCV_ANLPAR_100TXHALFDUPLEX))
        {
            *pPartnerAblty |= ETHERNET_ADV_CAPAB_100_HALF;
        }

        if (ETHTRCV_ANLPAR_100TXFULLDUPLEX ==
            (val & ETHTRCV_ANLPAR_100TXFULLDUPLEX))
        {
            *pPartnerAblty |= ETHERNET_ADV_CAPAB_100_FULL;
        }
    }

    /* Read the STS1 register (addr 0x0a) and get the 1 Gbps capabilities */
    if ((E_OK == retVal) && (TRUE == isGigCapab))
    {
        retVal = EthTrcv_readReg(trcv, ETHTRCV_STS1, &val);
        if (E_OK == retVal)
        {
            if (ETHTRCV_STS1_1000THALFDUPLEX ==
                (val & ETHTRCV_STS1_1000THALFDUPLEX))
            {
                *pPartnerAblty |= ETHERNET_ADV_CAPAB_1000_HALF;
            }

            if (ETHTRCV_STS1_1000TFULLDUPLEX ==
                (val & ETHTRCV_STS1_1000TFULLDUPLEX))
            {
                *pPartnerAblty |= ETHERNET_ADV_CAPAB_1000_FULL;
            }
        }
    }

    return retVal;
}

static Std_ReturnType EthTrcv_updateCapab(EthTrcv_Trcv *trcv,
                                          EthTrcv_ControllerConfigType *cfg)
{
    Std_ReturnType retVal;
    uint32 selfCapab, capab;

    retVal = EthTrcv_getSelfAbility(trcv,
                                    &selfCapab,
                                    cfg->isGigCapab);

    retVal |= EthTrcv_getPartnerAbility(trcv,
                                        &cfg->linkPartCapab,
                                        cfg->isGigCapab);

    if (E_OK == retVal)
    {
        capab = selfCapab & cfg->linkPartCapab;

        if (0U != (capab & ETHERNET_ADV_CAPAB_1000_FULL))
        {
            /* 1000 Mbps full-duplex */
            cfg->baudRate   = ETHTRCV_BAUD_RATE_1000MBIT;
            cfg->duplexMode = ETHTRCV_DUPLEX_MODE_FULL;
        }
        else if (0U != (capab & ETHERNET_ADV_CAPAB_1000_HALF))
        {
            /* 1000 Mbps half-duplex */
            cfg->baudRate   = ETHTRCV_BAUD_RATE_1000MBIT;
            cfg->duplexMode = ETHTRCV_DUPLEX_MODE_HALF;
        }
        else if (0U != (capab & ETHERNET_ADV_CAPAB_100_FULL))
        {
            /* 100 Mbps full-duplex */
            cfg->baudRate   = ETHTRCV_BAUD_RATE_100MBIT;
            cfg->duplexMode = ETHTRCV_DUPLEX_MODE_FULL;
        }
        else if (0U != (capab & ETHERNET_ADV_CAPAB_100_HALF))
        {
            /* 100 Mbps half-duplex */
            cfg->baudRate   = ETHTRCV_BAUD_RATE_100MBIT;
            cfg->duplexMode = ETHTRCV_DUPLEX_MODE_HALF;
        }
        else if (0U != (capab & ETHERNET_ADV_CAPAB_10_FULL))
        {
            /* 10 Mbps full-duplex */
            cfg->baudRate   = ETHTRCV_BAUD_RATE_10MBIT;
            cfg->duplexMode = ETHTRCV_DUPLEX_MODE_FULL;
        }
        else if (0U != (capab & ETHERNET_ADV_CAPAB_10_HALF))
        {
            /* 10 Mbps full-duplex */
            cfg->baudRate   = ETHTRCV_BAUD_RATE_10MBIT;
            cfg->duplexMode = ETHTRCV_DUPLEX_MODE_HALF;
        }
        else
        {
            retVal = E_NOT_OK;
        }
    }

    return retVal;
}

static Std_ReturnType EthTrcv_waitForAutoNeg(EthTrcv_Trcv *trcv)
{
    uint16 val;
    Std_ReturnType retVal = E_OK;

    uint64 delayInSec = ETHTRCV_AUTONEG_LINKED_WAIT_S;
    TickType delayInTicks = (TickType)(delayInSec*ETH_OS_COUNTER_FREQ);
    TickType curTick = 0U;
    TickType tempCount = 0U;
    TickType elapsed = 0U;
    StatusType status;
    uint8 success = 0U;

    status = GetCounterValue(ETH_OS_COUNTER_ID, &curTick);

    while ((E_OK == status) && (delayInTicks >= elapsed))
    {
        /* Read the BMSR register (addr 0x01) */
        retVal = EthTrcv_readReg(trcv, ETHTRCV_BMSR, &val);
        if (E_OK != retVal)
        {
            break;
        }

        /* Auto-negotiation is complete when "Auto-Neg Complete" bit
         * is set */
        if (ETHTRCV_BMSR_ANEGCOMPLETE ==
            (val & ETHTRCV_BMSR_ANEGCOMPLETE_MASK))
        {
            success = 1U;
            break;
        }

        tempCount = curTick;
        status = GetElapsedValue(ETH_OS_COUNTER_ID, &tempCount, &elapsed);
    }

    if (!success)
    {
        retVal = E_NOT_OK;
    }

    return retVal;
}

static Std_ReturnType EthTrcv_waitForLinkUp(EthTrcv_Trcv *trcv)
{
    uint16 val;
    Std_ReturnType retVal = E_OK;

    uint64 delayInSec = ETHTRCV_AUTONEG_LINKED_WAIT_S;
    TickType delayInTicks = (TickType)(delayInSec*ETH_OS_COUNTER_FREQ);
    TickType curTick = 0U;
    TickType tempCount = 0U;
    TickType elapsed = 0U;
    StatusType status;
    uint8 success = 0U;

    status = GetCounterValue(ETH_OS_COUNTER_ID, &curTick);

    while ((E_OK == status) && (delayInTicks >= elapsed))
    {
        /* Read the BMSR register (addr 0x01) */
        retVal = EthTrcv_readReg(trcv, ETHTRCV_BMSR, &val);
        if (E_OK != retVal)
        {
            break;
        }

        /* Link is up when "Link Status" bit is set */
        if (ETHTRCV_BMSR_LINKVALID ==
            (val & ETHTRCV_BMSR_LINKSTATUS_MASK))
        {
            success = 1U;
            break;
        }

        tempCount = curTick;
        status = GetElapsedValue(ETH_OS_COUNTER_ID, &tempCount, &elapsed);
    }

    if (!success)
    {
        retVal = E_NOT_OK;
    }

    return retVal;
}

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

#define ETHTRCV_START_SEC_CODE
#include "EthTrcv_MemMap.h"

Std_ReturnType EthTrcv_reset(EthTrcv_Trcv *trcv)
{
    uint32 retry = ETHTRCV_RESET_RETRY_NUM + 1U;
    uint32 mask;
    uint16 val;
    Std_ReturnType retVal;

    /* Write the BMCR register (addr 0x00) and set the "PHY Reset" bit */
    mask = ETHTRCV_BMCR_RESET_MASK;
    val = ETHTRCV_BMCR_RESET;
    retVal = EthTrcv_rmwReg(trcv, ETHTRCV_BMCR, mask, val);

    /* Wait for reset to complete */
    if (E_OK == retVal)
    {
        while (--retry > 0U)
        {
            retVal = EthTrcv_readReg(trcv, ETHTRCV_BMCR, &val);
            if (E_OK != retVal)
            {
                break;
            }

            if (ETHTRCV_BMCR_RESET !=
                (val & ETHTRCV_BMCR_RESET_MASK))
            {
                break;
            }
        }

        if (0U == retry)
        {
            retVal = E_NOT_OK;
        }
    }

    return retVal;
}

static void EthTrcv_setClkShift(EthTrcv_Trcv *trcv,
                                boolean txShiftEnable,
                                boolean rxShiftEnable)
{
    uint16 val;

    val = DP83867_RGMIICTL_RGMIIEN;
    val |= (txShiftEnable == TRUE) ? DP83867_RGMIICTL_TXCLKDLY : 0U;
    val |= (rxShiftEnable == TRUE) ? DP83867_RGMIICTL_RXCLKDLY : 0U;

    EthTrcv_rmwExtReg(trcv, DP83867_RGMIICTL,
                      DP83867_RGMIICTL_RGMIIEN |
                      DP83867_RGMIICTL_TXCLKDLY |
                      DP83867_RGMIICTL_RXCLKDLY,
                      val);
}

static Std_ReturnType EthTrcv_setTxFifoDepth(EthTrcv_Trcv *trcv,
                                             uint8 depth)
{
    uint16 val = 0U;
    Std_ReturnType retVal = E_OK;

    switch (depth)
    {
        case 3U:
            val = DP83867_PHYCR_TXFIFODEPTH_3B;
            break;

        case 4U:
            val = DP83867_PHYCR_TXFIFODEPTH_4B;
            break;

        case 6U:
            val = DP83867_PHYCR_TXFIFODEPTH_6B;
            break;

        case 8U:
            val = DP83867_PHYCR_TXFIFODEPTH_8B;
            break;

        default:
            retVal = E_NOT_OK;
            break;
    }

    if (E_OK == retVal)
    {
        EthTrcv_rmwReg(trcv, DP83867_PHYCR, DP83867_PHYCR_TXFIFODEPTH_MASK, val);
    }

    return retVal;
}

static Std_ReturnType EthTrcv_setClkDelay(EthTrcv_Trcv *trcv,
                                          uint32 txDelay,
                                          uint32 rxDelay)
{
    uint16 val;
    uint32 delay;
    uint32 delayCtrl;
    Std_ReturnType retVal = E_OK;

    if ((txDelay <= DP83867_RGMIIDCTL_DELAY_MAX) &&
        (rxDelay <= DP83867_RGMIIDCTL_DELAY_MAX))
    {
        /* Avoids wrong value of delayCtrl if txDelay is 0 */
        delay = (txDelay > 0U) ? txDelay : 1U;
        delayCtrl = ETHTRCV_DIV_ROUNDUP(delay, DP83867_RGMIIDCTL_DELAY_STEP) - 1U;
        val = (uint16)((delayCtrl << DP83867_RGMIIDCTL_TXDLYCTRL_OFFSET) &
                       DP83867_RGMIIDCTL_TXDLYCTRL_MASK);

        /* Avoids wrong value of delayCtrl if rxDelay is 0 */
        delay = (rxDelay > 0U) ? rxDelay : 1U;
        delayCtrl = ETHTRCV_DIV_ROUNDUP(delay, DP83867_RGMIIDCTL_DELAY_STEP) - 1U;
        val |= (uint16)((delayCtrl << DP83867_RGMIIDCTL_RXDLYCTRL_OFFSET) &
                        DP83867_RGMIIDCTL_RXDLYCTRL_MASK);

        EthTrcv_writeExtReg(trcv, DP83867_RGMIIDCTL, val);
    }
    else
    {
        retVal = E_NOT_OK;
    }

    return retVal;
}

static Std_ReturnType EthTrcv_setOutputImpedance(EthTrcv_Trcv *trcv,
                                                 uint32 impedance)
{
    Std_ReturnType retVal = E_OK;
    uint32 val;

    if ((impedance >= DP83867_IOMUXCFG_IOIMPEDANCE_MIN) &&
        (impedance <= DP83867_IOMUXCFG_IOIMPEDANCE_MAX))
    {
        val = (DP83867_IOMUXCFG_IOIMPEDANCE_MAX - impedance) *
              DP83867_IOMUXCFG_IOIMPEDANCE_MASK;
        val = (val + DP83867_IOMUXCFG_IOIMPEDANCE_RANGE / 2) /
              DP83867_IOMUXCFG_IOIMPEDANCE_RANGE;

        EthTrcv_rmwExtReg(trcv, DP83867_IOMUXCFG,
                          DP83867_IOMUXCFG_IOIMPEDANCE_MASK,
                          val);
    }
    else
    {
        retVal = E_NOT_OK;
    }

    return retVal;
}

static uint32 EthTrcv_getId(EthTrcv_Trcv *trcv)
{
    Std_ReturnType retVal;
    uint16 phyId1;
    uint16 phyId2;

    retVal = EthTrcv_readReg(trcv, ETHTRCV_PHYIDR1, &phyId1);
    if (E_OK == retVal)
    {
        retVal = EthTrcv_readReg(trcv, ETHTRCV_PHYIDR2, &phyId2);
    }

    if (E_OK != retVal)
    {
        phyId1 = 0U;
        phyId2 = 0U;
    }

    return ((phyId1 << 16U) | phyId2);
}

Std_ReturnType EthTrcv_config(EthTrcv_Trcv *trcv,
                              const void *extendedCfg)
{
    Std_ReturnType retVal = E_OK;
    uint32 phyId;

    phyId = EthTrcv_getId(trcv);

    if (TRUE == EthTrcv_supportPhyId(phyId))
    {
        const Dp83867_ConfigType *cfg =
            (const Dp83867_ConfigType *)extendedCfg;

        /* Enable/disable RGMII clock shift and clock delay */
        EthTrcv_setClkShift(trcv, cfg->txClkShiftEnable, cfg->rxClkShiftEnable);

        /* Set RGMII clock delay */
        retVal = EthTrcv_setClkDelay(trcv, cfg->txDelayInPs, cfg->rxDelayInPs);

        /* Set TX FIFO depth */
        if (E_OK == retVal)
        {
            retVal = EthTrcv_setTxFifoDepth(trcv, cfg->txFifoDepth);
        }

        /* Set output impedance */
        if (E_OK == retVal)
        {
            retVal = EthTrcv_setOutputImpedance(trcv,
                                                cfg->outputImpedanceInMilliOhms);
        }
    }
    else
    {
        /* PHY not supported by this driver */
        retVal = E_NOT_OK;
    }

    return retVal;
}

Std_ReturnType EthTrcv_enableLoopback(EthTrcv_Trcv *trcv,
                                      boolean enable)
{
    uint16 mask;
    uint16 val;
    Std_ReturnType retVal;

    mask = ETHTRCV_BMCR_LOOPBACK_MASK | ETHTRCV_BMCR_ANEGENABLE_MASK;

    /* Disable auto-negotiation when loopback is enabled */
    if (TRUE == enable)
    {
        val = ETHTRCV_BMCR_LOOPBACK;
    }
    else
    {
        val = ETHTRCV_BMCR_ANEGENABLE;
    }

    /* Write the BMCR register (addr 0x00) and set the "Loopback" bit
     * to enter into loopback mode */
    retVal = EthTrcv_rmwReg(trcv, ETHTRCV_BMCR, mask, val);

    return retVal;
}

Std_ReturnType EthTrcv_configAndStartANeg(EthTrcv_Trcv *trcv,
                                          EthTrcv_ControllerConfigType *cfg)
{
    uint16 mask;
    uint16 val;
    Std_ReturnType retVal = E_NOT_OK;

    /* Enable auto-negotiation */
    mask = ETHTRCV_BMCR_ANEGENABLE_MASK;
    val = ETHTRCV_BMCR_ANEGENABLE;
    retVal = EthTrcv_rmwReg(trcv, ETHTRCV_BMCR, mask, val);

    /* Set 10 and 100 Mbps auto-negotiation capabilities */
    if (E_OK == retVal)
    {
        mask = ETHTRCV_ANAR_100TXDUPLEX_MASK |
               ETHTRCV_ANAR_10TDUPLEX_MASK;
        val = 0U;

        if (0U != (cfg->advertiseCapab & ETHERNET_ADV_CAPAB_100_HALF))
        {
            val |= ETHTRCV_ANAR_100TXHALFDUPLEX;
        }

        if (0U != (cfg->advertiseCapab & ETHERNET_ADV_CAPAB_100_FULL))
        {
            val |= ETHTRCV_ANAR_100TXFULLDUPLEX;
        }

        if (0U != (cfg->advertiseCapab & ETHERNET_ADV_CAPAB_10_HALF))
        {
            val |= ETHTRCV_ANAR_10THALFDUPLEX;
        }

        if (0U != (cfg->advertiseCapab & ETHERNET_ADV_CAPAB_10_FULL))
        {
            val |= ETHTRCV_ANAR_10TFULLDUPLEX;
        }

        retVal = EthTrcv_rmwReg(trcv, ETHTRCV_ANAR, mask, val);
    }

    /* Set 1 Gbps auto-negotiation capabilities */
    if ((E_OK == retVal) && (TRUE == cfg->isGigCapab))
    {
        mask = ETHTRCV_CFG1_1000TDUPLEX_MASK;
        val = 0U;

        if (0U != (cfg->advertiseCapab & ETHERNET_ADV_CAPAB_1000_HALF))
        {
            val |= ETHTRCV_CFG1_1000THALFDUPLEX;
        }

        if (0U != (cfg->advertiseCapab & ETHERNET_ADV_CAPAB_1000_FULL))
        {
            val |= ETHTRCV_CFG1_1000TFULLDUPLEX;
        }

        retVal = EthTrcv_rmwReg(trcv, ETHTRCV_CFG1, mask, val);
    }

    /* Start auto-negotiation */
    if (E_OK == retVal)
    {
        mask = ETHTRCV_BMCR_RESTARTANEG_MASK;
        val = ETHTRCV_BMCR_RESTARTANEG;
        retVal = EthTrcv_rmwReg(trcv, ETHTRCV_BMCR, mask, val);
    }

    /* Wait for auto-negotiation to complete */
    if (E_OK == retVal)
    {
        retVal = EthTrcv_waitForAutoNeg(trcv);
    }

    /* Wait for link up */
    if (E_OK == retVal)
    {
        retVal = EthTrcv_waitForLinkUp(trcv);
    }

    /* Update link capabilities from transceiver's and partner's */
    if (E_OK == retVal)
    {
        retVal = EthTrcv_updateCapab(trcv, cfg);
    }

    return retVal;
}

boolean EthTrcv_isLinkValid(EthTrcv_Trcv *trcv)
{
    uint16 val;
    boolean valid = FALSE;
    Std_ReturnType retVal;

    /* Read the BMSR register (addr 0x01) */
    retVal = EthTrcv_readReg(trcv, ETHTRCV_BMSR, &val);
    if (E_OK == retVal)
    {
        /* Determine status from "Link Status" field */
        if (ETHTRCV_BMSR_LINKVALID ==
            (val & ETHTRCV_BMSR_LINKSTATUS_MASK))
        {
            valid = TRUE;
        }
    }

    return valid;
}

#if (STD_ON == ETH_WRAPPER_ENABLED)
Std_ReturnType EthTrcv_readReg(EthTrcv_Trcv *trcv,
                               uint8 reg,
                               uint16 *val)
{
    Std_ReturnType retVal = E_NOT_OK;

    /* Read the PHY register */
#if (STD_ON == ETH_ENABLE_MII_API)
    retVal = Eth_30_Wrapper_ReadMii(trcv->ctrlIdx, trcv->miiIdx, reg, val);
#endif

    /* Eth_30_Wrapper_ReadMii() already wait for MDIO command completed ISR
     * No need to wait here
     */

    return retVal;
}

Std_ReturnType EthTrcv_writeReg(EthTrcv_Trcv *trcv,
                                uint8 reg,
                                uint16 val)
{
    Std_ReturnType retVal = E_NOT_OK;

#if (STD_ON == ETH_ENABLE_MII_API)
    retVal = Eth_30_Wrapper_WriteMii(trcv->ctrlIdx, trcv->miiIdx, reg, val);
#endif

    /* Eth_30_Wrapper_WriteMii() already wait for MDIO command completed ISR
     * No need to wait here
     */

    return retVal;
}

#else /* (STD_ON == ETH_WRAPPER_ENABLED) */

Std_ReturnType EthTrcv_readReg(EthTrcv_Trcv *trcv,
                               uint8 reg,
                               uint16 *val)
{
    Std_ReturnType retVal = E_OK;
    uint32 timeoutCnt = 0u;

    mdioRdCmdComplete = FALSE;

    /* Read the PHY register */
#if (STD_ON == ETH_ENABLE_MII_API)
    retVal = Eth_ReadMii(trcv->ctrlIdx, trcv->miiIdx, reg, val);
#endif

    if(E_OK == retVal)
    {
        while(FALSE == mdioRdCmdComplete)
        {
            if(ETHTRCV_TIMEOUT_DURATION == timeoutCnt)
            {
#if (ETHTRCV_E_HARDWARE_ERROR != ETHTRCV_DEM_NO_EVENT)
                (void)Dem_SetEventStatus(ETHTRCV_E_HARDWARE_ERROR,
                                      DEM_EVENT_STATUS_FAILED);
#endif
                retVal = E_NOT_OK;
                break;
            }
            timeoutCnt++;
        }
    }

    if (E_OK == retVal)
    {
        *val = mdioRdVal;
    }

    return retVal;
}

Std_ReturnType EthTrcv_writeReg(EthTrcv_Trcv *trcv,
                                uint8 reg,
                                uint16 val)
{
    Std_ReturnType retVal = E_OK;
    uint32 timeoutCnt = 0u;

    mdioWrCmdComplete = FALSE;

    /* Write the PHY register */
#if (STD_ON == ETH_ENABLE_MII_API)
    retVal = Eth_WriteMii(trcv->ctrlIdx, trcv->miiIdx, reg, val);
#endif

    if(E_OK == retVal)
    {
        while(FALSE == mdioWrCmdComplete)
        {
            if(ETHTRCV_TIMEOUT_DURATION == timeoutCnt)
            {
#if (ETHTRCV_E_HARDWARE_ERROR != ETHTRCV_DEM_NO_EVENT)
                (void)Dem_SetEventStatus(ETHTRCV_E_HARDWARE_ERROR,
                                      DEM_EVENT_STATUS_FAILED);
#endif
                retVal = E_NOT_OK;
                break;
            }
            timeoutCnt++;
        }
    }

    return retVal;
}

#endif /* (STD_ON == ETH_WRAPPER_ENABLED) */

Std_ReturnType EthTrcv_rmwReg(EthTrcv_Trcv *trcv,
                              uint8 reg,
                              uint16 mask,
                              uint16 val)
{
    Std_ReturnType retVal;
    uint16 tmp;

    retVal = EthTrcv_readReg(trcv, reg, &tmp);
    if (E_OK == retVal)
    {
        val &= mask;
        tmp &= ~mask;
        retVal = EthTrcv_writeReg(trcv, reg, tmp | val);
    }

    return retVal;
}

Std_ReturnType EthTrcv_readExtReg(EthTrcv_Trcv *trcv,
                                  uint16 reg,
                                  uint16 *val)
{
    uint16 devad = ETHTRCV_MMD_CR_DEVADDR;
    Std_ReturnType retVal;

    retVal = EthTrcv_writeReg(trcv, ETHTRCV_MMD_CR,
                              devad | ETHTRCV_MMD_CR_ADDR);
    if (E_OK == retVal)
    {
        retVal = EthTrcv_writeReg(trcv, ETHTRCV_MMD_DR, reg);
    }

    if (E_OK == retVal)
    {
        EthTrcv_writeReg(trcv, ETHTRCV_MMD_CR,
                         devad | ETHTRCV_MMD_CR_DATA_NOPOSTINC);
    }

    if (E_OK == retVal)
    {
        retVal = EthTrcv_readReg(trcv, ETHTRCV_MMD_DR, val);
    }

    return retVal;
}

Std_ReturnType EthTrcv_writeExtReg(EthTrcv_Trcv *trcv,
                                   uint16 reg,
                                   uint16 val)
{
    uint16 devad = ETHTRCV_MMD_CR_DEVADDR;
    Std_ReturnType retVal;

    retVal = EthTrcv_writeReg(trcv, ETHTRCV_MMD_CR,
                              devad | ETHTRCV_MMD_CR_ADDR);
    if (E_OK == retVal)
    {
        retVal = EthTrcv_writeReg(trcv, ETHTRCV_MMD_DR, reg);
    }

    if (E_OK == retVal)
    {
        EthTrcv_writeReg(trcv, ETHTRCV_MMD_CR,
                         devad | ETHTRCV_MMD_CR_DATA_NOPOSTINC);
    }

    if (E_OK == retVal)
    {
        retVal = EthTrcv_writeReg(trcv, ETHTRCV_MMD_DR, val);
    }

    return retVal;
}

Std_ReturnType EthTrcv_rmwExtReg(EthTrcv_Trcv *trcv,
                                 uint16 reg,
                                 uint16 mask,
                                 uint16 val)
{
    uint16 devad = ETHTRCV_MMD_CR_DEVADDR;
    uint16 data;
    Std_ReturnType retVal;

    EthTrcv_writeReg(trcv, ETHTRCV_MMD_CR,
                     devad | ETHTRCV_MMD_CR_ADDR);
    EthTrcv_writeReg(trcv, ETHTRCV_MMD_DR, reg);
    EthTrcv_writeReg(trcv, ETHTRCV_MMD_CR,
                     devad | ETHTRCV_MMD_CR_DATA_NOPOSTINC);
    retVal = EthTrcv_readReg(trcv, ETHTRCV_MMD_DR, &data);

    if (E_OK == retVal)
    {
        data = (data & ~mask) | (val & mask);
        EthTrcv_writeReg(trcv, ETHTRCV_MMD_CR,
                         devad | ETHTRCV_MMD_CR_DATA_NOPOSTINC);
        EthTrcv_writeReg(trcv, ETHTRCV_MMD_DR, data);
    }

    return retVal;
}

Std_ReturnType EthTrcv_checkTransceiverAccess(EthTrcv_Trcv *trcv)
{
    uint16 val;
    Std_ReturnType retVal;

    /* Check if Ethernet transceiver is present or not by checking
     * if BMSR register read goes through */
    retVal = EthTrcv_readReg(trcv, ETHTRCV_BMSR, &val);
    if (E_OK != retVal)
    {
#if (ETHTRCV_E_ACCESS != ETHTRCV_DEM_NO_EVENT)
        (void)Dem_SetEventStatus(ETHTRCV_E_ACCESS,
                              DEM_EVENT_STATUS_PREFAILED);
#endif
    }

    return retVal;
}

#if defined(ETHTRCV_DEBUG)
extern void AppUtils_printf(const char *pcString, ...);

void EthTrcv_dumpRegs(EthTrcv_Trcv *trcv)
{
    uint16 val;

    EthTrcv_readReg(trcv, ETHTRCV_BMCR, &val);
    AppUtils_printf("BMCR    = 0x%04x\n", val & 0xFFFFU);

    EthTrcv_readReg(trcv, ETHTRCV_BMSR, &val);
    AppUtils_printf("BMSR    = 0x%04x\n", val & 0xFFFFU);

    EthTrcv_readReg(trcv, ETHTRCV_PHYIDR1, &val);
    AppUtils_printf("PHYIDR1 = 0x%04x\n", val & 0xFFFFU);

    EthTrcv_readReg(trcv, ETHTRCV_PHYIDR2, &val);
    AppUtils_printf("PHYIDR2 = 0x%04x\n", val & 0xFFFFU);

    EthTrcv_readReg(trcv, ETHTRCV_ANAR, &val);
    AppUtils_printf("ANAR    = 0x%04x\n", val & 0xFFFFU);

    EthTrcv_readReg(trcv, ETHTRCV_ANLPAR, &val);
    AppUtils_printf("ANLPAR  = 0x%04x\n", val & 0xFFFFU);

    EthTrcv_readReg(trcv, ETHTRCV_ANER, &val);
    AppUtils_printf("ANER    = 0x%04x\n", val & 0xFFFFU);

    EthTrcv_readReg(trcv, ETHTRCV_ANNPTR, &val);
    AppUtils_printf("ANNPTR  = 0x%04x\n", val & 0xFFFFU);

    EthTrcv_readReg(trcv, ETHTRCV_ANNPRR, &val);
    AppUtils_printf("ANNPRR  = 0x%04x\n", val & 0xFFFFU);

    EthTrcv_readReg(trcv, ETHTRCV_CFG1, &val);
    AppUtils_printf("CFG1    = 0x%04x\n", val & 0xFFFFU);

    EthTrcv_readReg(trcv, ETHTRCV_STS1, &val);
    AppUtils_printf("STS1    = 0x%04x\n", val & 0xFFFFU);
}
#endif

#define ETHTRCV_STOP_SEC_CODE
#include "EthTrcv_MemMap.h"
