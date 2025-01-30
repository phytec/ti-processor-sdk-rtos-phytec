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
 *  \file     Spi_Mcspi.c
 *
 *  \brief    This file contains SPI MCAL driver functions for the McSPI
 *            peripheral
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "stdint.h"
#include "Spi_Cfg.h"
#include "Spi.h"
#include "Spi_Priv.h"
#include "Dem.h"
#include "Os.h"

/* There are static inline functions in hw_types.h file. Map them as well */
#define SPI_START_SEC_CODE
#include "Spi_MemMap.h"
#include <hw_include/lldr.h>
#define SPI_STOP_SEC_CODE
#include "Spi_MemMap.h"

#include <hw_include/lldr_mcspi.h>



/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/** \brief Total length of FIFO for both TX/RX */
#define MCSPI_FIFO_LENGTH               (128U)

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */
static void Spi_mcspiReadFifo(Spi_ChannelObjType *chObj,
                              uint32              baseAddr,
                              uint32              csNum,
                              uint32               numWordsToRead);
static void Spi_mcspiWriteFifo(Spi_ChannelObjType *chObj,
                               uint32              baseAddr,
                               uint32              csNum,
                               uint32              numWordsToWrite);
static void Spi_mcspiInitiateChunkXfer(const Spi_ChannelObjType *chObj,
                                       const Spi_JobObjType     *jobObj,
                                       uint32                    baseAddr);
static uint32 Spi_mcspiGetTxMask(uint32 csNum);
static uint32 Spi_mcspiGetRxMask(uint32 csNum);
static void Spi_mcspiReset(uint32 baseAddr);
static void Spi_mcspiConfigDataPinDir(
    const Spi_HwUnitObjType *hwUnitObj, const Spi_JobObjType *jobObj);
static inline const uint8 *Spi_mcspiFifoWrite8(uint32 baseAddr, uint32 chNum,
                                       const uint8  *bufPtr,
                                       uint32 transferLength);
static inline const uint16 *Spi_mcspiFifoWrite16(uint32 baseAddr, uint32 chNum,
                                         const uint16 *bufPtr,
                                         uint32 transferLength);
static inline const uint32 *Spi_mcspiFifoWrite32(uint32 baseAddr, uint32 chNum,
                                         const uint32 *bufPtr,
                                         uint32 transferLength);
static inline void Spi_mcspiFifoWriteDefault(uint32 baseAddr, uint32 chNum,
                                   uint32 defaultTxData,
                                   uint32 transferLength);
static inline uint8 *Spi_mcspiFifoRead8(uint32  baseAddr, uint32  chNum,
                                uint8  *bufPtr, uint32  transferLength,
                                uint32  dataWidthBitMask);
static inline uint16 *Spi_mcspiFifoRead16(uint32  baseAddr, uint32  chNum,
                                  uint16  *bufPtr, uint32  transferLength,
                                  uint32  dataWidthBitMask);
static inline uint32 *Spi_mcspiFifoRead32(uint32  baseAddr, uint32  chNum,
                                  uint32  *bufPtr, uint32  transferLength,
                                  uint32  dataWidthBitMask);
static inline void Spi_mcspiFifoReadDiscard(uint32 baseAddr, uint32 chNum,
                                  uint32 transferLength);
static inline uint32 Spi_mcspiGetFifoLength(uint32 baseAddr);
static void Spi_mcspci_continueTxRx_conditons(uint32 baseAddr,uint32 csNum);
/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

#define SPI_START_SEC_CODE
#include "Spi_MemMap.h"

/*
 * Design: MCAL-6398
 */
void Spi_mcspiInit(const Spi_HwUnitObjType *hwUnitObj)
{
    uint32 regVal = 0U;
    uint32 baseAddr = 0U;
    uint32 index = 0U ;
    uint32 csNum = 0U;
    Spi_JobObjType  *jobObj = (Spi_JobObjType *)NULL_PTR;
    Spi_McspiExternalDeviceConfigType *extDevCfg = (Spi_McspiExternalDeviceConfigType *)NULL_PTR;

    baseAddr = hwUnitObj->baseAddr;

    /* Reset McSPI */
    Spi_mcspiReset(baseAddr);

    /* Set sysconfig */
    regVal = ((MCSPI_SYSCONFIG_CLOCKACTIVITY_BOTH <<
               MCSPI_SYSCONFIG_CLOCKACTIVITY_SHIFT) |
              (MCSPI_SYSCONFIG_SIDLEMODE_NO <<
               MCSPI_SYSCONFIG_SIDLEMODE_SHIFT) |
              (MCSPI_SYSCONFIG_ENAWAKEUP_NOWAKEUP <<
               MCSPI_SYSCONFIG_ENAWAKEUP_SHIFT) |
              (MCSPI_SYSCONFIG_AUTOIDLE_OFF <<
               MCSPI_SYSCONFIG_AUTOIDLE_SHIFT));
    LLD_REG32_WR(baseAddr + MCSPI_SYSCONFIG, regVal);

    /* SPI_CoverageGap_06:Dynamic coverage, for fail condition in for loop cannot be achievable,
    * as hwUnitObj or hwUnit id or SPI_MAX_JOB, can be detected and report the
    * DET error inearlier stage itself.
    */
    for (index = 0U; index < SPI_MAX_JOBS; index++)
    {
        jobObj =  &Spi_DrvObj.jobObj[index];
        if (hwUnitObj->hwUnitCfg.hwUnitId == jobObj->jobCfg.hwUnitId)
        {
            csNum     = (uint32)(jobObj->jobCfg_PC.csPin);
            extDevCfg = &jobObj->extDevCfg->mcspi;
            /* Set CS polarity */
            if (SPI_LOW == extDevCfg->csPolarity)
            {
                LLD_REG32_FINS(baseAddr + MCSPI_CHCONF(csNum), MCSPI_CH0CONF_EPOL, MCSPI_CH0CONF_EPOL_ACTIVELOW);
            }
            else
            {
                LLD_REG32_FINS(baseAddr + MCSPI_CHCONF(csNum), MCSPI_CH0CONF_EPOL, MCSPI_CH0CONF_EPOL_ACTIVEHIGH);
            }

            /* Applicable only for single channel master mode only.
               In case of multi-channel, this pin has no effect  */
            LLD_REG32_FINS(baseAddr + MCSPI_CHCONF(csNum), MCSPI_CH0CONF_FORCE, MCSPI_CH0CONF_FORCE_DEASSERT);

            break;
        }
    }

    /* Set master mode - MCAL supports only master mode */
    LLD_REG32_FINS(baseAddr + MCSPI_MODULCTRL, MCSPI_MODULCTRL_MS, MCSPI_MODULCTRL_MS_MASTER);

    return;
}

/*
 * Design: MCAL-6734
 */
void Spi_mcspiDeInit(const Spi_HwUnitObjType *hwUnitObj)
{
    /* Reset McSPI */
    Spi_mcspiReset(hwUnitObj->baseAddr);

    return;
}

void Spi_mcspiConfigJob(const Spi_HwUnitObjType *hwUnitObj, Spi_JobObjType *jobObj)
{
    uint32 baseAddr = 0U; 
    uint32 regVal = 0U;
    uint32 csNum = 0U;
    uint32 clkD = 0U;
    uint32 extClk = 0U;
    uint32 txFifoTrigLvl = 0U;
    uint32 rxFifoTrigLvl = 0U;
    Spi_McspiExternalDeviceConfigType *extDevCfg = (Spi_McspiExternalDeviceConfigType *)NULL_PTR;
    uint32 clkModeTemp = 0U;
    uint32 mcspiFifoLength = 0U;

    baseAddr  = hwUnitObj->baseAddr;
    csNum     = (uint32)(jobObj->jobCfg_PC.csPin);
    extDevCfg = &jobObj->extDevCfg->mcspi;

    /*
     * Set Data Pin Directions
     */
    Spi_mcspiConfigDataPinDir(hwUnitObj, jobObj);

    /*
     * Set all CS related functionality
     */
    /* Set 3-pin mode (CS disable) or 4-pin mode CS enable */
    if (((uint32) FALSE) == extDevCfg->csEnable)
    {
        LLD_REG32_FINS(baseAddr + MCSPI_MODULCTRL, MCSPI_MODULCTRL_PIN34, MCSPI_MODULCTRL_PIN34_3PINMODE);
    }
    else
    {
        LLD_REG32_FINS(baseAddr + MCSPI_MODULCTRL, MCSPI_MODULCTRL_PIN34, MCSPI_MODULCTRL_PIN34_4PINMODE);
    }

    if (SPI_CONTINUOUS == extDevCfg->csMode)
    {
        /* In continuous mode, CS is controlled by driver by setting FORCE
         * enable bit just before starting */
        LLD_REG32_FINS(baseAddr + MCSPI_MODULCTRL, MCSPI_MODULCTRL_SINGLE, MCSPI_MODULCTRL_SINGLE_SINGLE);
    }
    else
    {
        /* In single mode, let the hardware decide when to toggle the CS */
        LLD_REG32_FINS(baseAddr + MCSPI_MODULCTRL, MCSPI_MODULCTRL_SINGLE, MCSPI_MODULCTRL_SINGLE_MULTI);
    }

    /* Set CS polarity */
    if (SPI_LOW == extDevCfg->csPolarity)
    {
        LLD_REG32_FINS(baseAddr + MCSPI_CHCONF(csNum), MCSPI_CH0CONF_EPOL, MCSPI_CH0CONF_EPOL_ACTIVELOW);
    }
    else
    {
        LLD_REG32_FINS(baseAddr + MCSPI_CHCONF(csNum), MCSPI_CH0CONF_EPOL, MCSPI_CH0CONF_EPOL_ACTIVEHIGH);
    }

    /* Set the TCS0 field - delay between CS active and first CLK toggling */
    LLD_REG32_FINS(baseAddr + MCSPI_CHCONF(csNum), MCSPI_CH0CONF_TCS0, extDevCfg->csIdleTime);

    /*
     * Set clock configuration
     */
    /* Set divider */
    extClk = (extDevCfg->clkDivider >> ((uint32) 4U));      /* MSB 8-bit */
    clkD   = (extDevCfg->clkDivider & ((uint32) 0x0FU));    /* LSB 4-bit */
    LLD_REG32_FINS(baseAddr + MCSPI_CHCTRL(csNum), MCSPI_CH0CTRL_EXTCLK, extClk);
    LLD_REG32_FINS(baseAddr + MCSPI_CHCONF(csNum), MCSPI_CH0CONF_CLKD, clkD);
    /* Set the clock granularity to 1 clock cycle.*/
    LLD_REG32_FINS(baseAddr + MCSPI_CHCONF(csNum), MCSPI_CH0CONF_CLKG, MCSPI_CH0CONF_CLKG_ONECYCLE);
    /* Set clock mode */
    regVal  = LLD_REG32_RD(baseAddr + MCSPI_CHCONF(csNum));
    regVal &= ~(MCSPI_CH0CONF_PHA_MASK | MCSPI_CH0CONF_POL_MASK);
    clkModeTemp = (uint32) extDevCfg->clkMode;
    regVal |= (clkModeTemp & (MCSPI_CH0CONF_PHA_MASK | MCSPI_CH0CONF_POL_MASK));
    LLD_REG32_WR(baseAddr + MCSPI_CHCONF(csNum), regVal);

    /*
     * Set start bit params
     */
    if (((uint32) FALSE) == extDevCfg->startBitEnable)
    {
        LLD_REG32_FINS(baseAddr + MCSPI_CHCONF(csNum), MCSPI_CH0CONF_SBE, MCSPI_CH0CONF_SBE_DISABLED);
    }
    else
    {
        LLD_REG32_FINS(baseAddr + MCSPI_CHCONF(csNum), MCSPI_CH0CONF_SBE, MCSPI_CH0CONF_SBE_ENABLED);
        LLD_REG32_FINS(baseAddr + MCSPI_CHCONF(csNum), MCSPI_CH0CONF_SBPOL, extDevCfg->startBitLevel);
    }

    /* Set TX/RX mode */
    LLD_REG32_FINS(baseAddr + MCSPI_CHCONF(csNum), MCSPI_CH0CONF_TRM, extDevCfg->txRxMode);

    /*
     * Note: TX/RX FIFO trigger levels are set to half the TX/RX FIFO levels,
     *       so that it is efficient as well as we don't get TX underflow or
     *       RX overflow.
     *       Setting a low value is in-efficient where as setting a high value
     *       leads to overflow/underflow!!
     */
    /* Calculate effective FIFO Trigger level */
    rxFifoTrigLvl = 0U;
    mcspiFifoLength = Spi_mcspiGetFifoLength(baseAddr);
    if (extDevCfg->txRxMode == SPI_TX_RX_MODE_BOTH)
    {
        /* Both TX/RX is used, divide the FIFO equally */
        txFifoTrigLvl = (mcspiFifoLength >> 2U) - 1U;
        rxFifoTrigLvl = (mcspiFifoLength >> 2U) - 1U;
    }
    else
    {
        /* TX only mode, full FIFO is used for TX */
        txFifoTrigLvl = (mcspiFifoLength >> 1U) - 1U;
    }
    jobObj->txFifoTrigLvl = txFifoTrigLvl + 1U; /* To compensate for -1U */
    jobObj->rxFifoTrigLvl = rxFifoTrigLvl + 1U; /* To compensate for -1U */

    return;
}

/*
 * Design: MCAL-6521
 */
void Spi_mcspiConfigCh(const Spi_HwUnitObjType *hwUnitObj, const Spi_JobObjType    *jobObj,
                       Spi_ChannelObjType *chObj)
{
    uint32 baseAddr = 0U;
    uint32 csNum = 0U;
    uint32 txFifoTrigLvl = 0U;
    uint32 rxFifoTrigLvl = 0U;
    uint32 regVal = 0U; 
    uint32 reminder = 0U;
    uint32 effNumWordsTxRx = 0U;

    baseAddr = hwUnitObj->baseAddr;
    csNum    = (uint32)(jobObj->jobCfg_PC.csPin);

    /* Reset counter and other params */
#if ((SPI_CHANNELBUFFERS == SPI_EB) || (SPI_CHANNELBUFFERS == SPI_IB_EB))
    if (SPI_EB == chObj->chCfg.channelBufType)
    {
        chObj->curTxBufPtr = chObj->txBufPtr;
        chObj->curRxBufPtr = chObj->rxBufPtr;
    }
#endif
#if ((SPI_CHANNELBUFFERS == SPI_IB) || (SPI_CHANNELBUFFERS == SPI_IB_EB))
    if (SPI_IB == chObj->chCfg.channelBufType)
    {
        chObj->curTxBufPtr = (const uint8 *) &chObj->txIb[0U];
        chObj->curRxBufPtr = (uint8 *) &chObj->rxIb[0U];
    }
#endif
    chObj->curTxWords  = 0U;
    chObj->curRxWords  = 0U;
    chObj->effTxFifoDepth = jobObj->txFifoTrigLvl / chObj->bufWidth;

    /*
     * Set all the channel parameters
     */
    /* Set wordlength in bits */
    LLD_REG32_FINS(baseAddr + MCSPI_CHCONF(csNum), MCSPI_CH0CONF_WL, ((uint32) chObj->chCfg.dataWidth - 1U));

    /* Start transferring only multiple of FIFO trigger level */
    reminder = (((uint32) chObj->numWordsTxRx) & (chObj->effTxFifoDepth - 1U));

    effNumWordsTxRx = ((uint32) chObj->numWordsTxRx) - reminder;

    rxFifoTrigLvl = jobObj->rxFifoTrigLvl;
    txFifoTrigLvl = jobObj->txFifoTrigLvl;
    /* Handle transfers with less than FIFO level.
     * Set FIFO trigger level and word count to be equal to the
     * reminder word. Otherwise the HW doesn't generating TX
     * empty interrupt if WCNT is less than FIFO trigger level */
    if (effNumWordsTxRx == 0U)
    {
        effNumWordsTxRx = reminder;
        if (rxFifoTrigLvl != 1U)
        {
            /* Set RX level only when RX FIFO is enabled */
            rxFifoTrigLvl = reminder;
        }
        txFifoTrigLvl = reminder;
    }

    /* Set FIFO trigger level and word count */
    regVal  = LLD_REG32_RD(baseAddr + MCSPI_XFERLEVEL);
    regVal &= ~(MCSPI_XFERLEVEL_AFL_MASK | MCSPI_XFERLEVEL_AEL_MASK);
    regVal |= (((rxFifoTrigLvl - 1U) << MCSPI_XFERLEVEL_AFL_SHIFT) & MCSPI_XFERLEVEL_AFL_MASK);
    regVal |= (((txFifoTrigLvl - 1U) << MCSPI_XFERLEVEL_AEL_SHIFT) & MCSPI_XFERLEVEL_AEL_MASK);
    regVal &= ~MCSPI_XFERLEVEL_WCNT_MASK;
    regVal |= ((effNumWordsTxRx << MCSPI_XFERLEVEL_WCNT_SHIFT) & MCSPI_XFERLEVEL_WCNT_MASK);
    LLD_REG32_WR(baseAddr + MCSPI_XFERLEVEL, regVal);

    return;
}

void Spi_mcspiStart(const Spi_HwUnitObjType *hwUnitObj, const Spi_JobObjType *jobObj,
                    uint32 isIntrMode)
{
    uint32 baseAddr = 0U;
    uint32 csNum = 0U;
    uint32 intrMask = 0U;
    uint32 csIntrMask = 0U;
    Spi_McspiExternalDeviceConfigType *extDevCfg = (Spi_McspiExternalDeviceConfigType *)NULL_PTR;

    baseAddr  = hwUnitObj->baseAddr;
    csNum     = (uint32)(jobObj->jobCfg_PC.csPin);
    extDevCfg = &jobObj->extDevCfg->mcspi;

    /* Clear all previous interrupt status */
    Spi_mcspiClearAllIrqStatus(baseAddr);

    /* Enable TX and RX FIFO */
    LLD_REG32_FINS(baseAddr + MCSPI_CHCONF(csNum), MCSPI_CH0CONF_FFEW, MCSPI_CH0CONF_FFEW_FFENABLED);
    if (extDevCfg->txRxMode == SPI_TX_RX_MODE_BOTH)
    {
        LLD_REG32_FINS(baseAddr + MCSPI_CHCONF(csNum), MCSPI_CH0CONF_FFER, MCSPI_CH0CONF_FFER_FFENABLED);
    }

    if (((uint32) TRUE) == isIntrMode)
    {
        /* Enable EOW, TX and RX interrupts */
        intrMask   = LLD_REG32_RD(baseAddr + MCSPI_IRQENABLE);
        intrMask  |= MCSPI_IRQSTATUS_EOW_MASK;
        csIntrMask = Spi_mcspiGetCsIntrMask(csNum, extDevCfg->txRxMode);
        intrMask  |= csIntrMask;
        LLD_REG32_WR(baseAddr + MCSPI_IRQENABLE, intrMask);
    }

    /* Set force mode */
    if (SPI_CONTINUOUS == extDevCfg->csMode)
    {
        LLD_REG32_FINS(baseAddr + MCSPI_CHCONF(csNum), MCSPI_CH0CONF_FORCE, MCSPI_CH0CONF_FORCE_ASSERT);
    }

    /* Enable channel */
    LLD_REG32_FINS(baseAddr + MCSPI_CHCTRL(csNum), MCSPI_CH0CTRL_EN, MCSPI_CH0CTRL_EN_ACT);

    /*
     * Note: Once the channel is enabled, we will get the TX almost empty
     *       interrupt. No data transfer is required here!!
     */

}

void Spi_mcspiReStart(const Spi_HwUnitObjType *hwUnitObj, const Spi_JobObjType *jobObj)
{
    uint32 baseAddr = 0U;
    uint32 csNum = 0U;

    baseAddr = hwUnitObj->baseAddr;
    csNum    = (uint32)(jobObj->jobCfg_PC.csPin);

    /* Disable channel and re-enable so that new word count takes effect */
    LLD_REG32_FINS(baseAddr + MCSPI_CHCTRL(csNum), MCSPI_CH0CTRL_EN, MCSPI_CH0CTRL_EN_NACT);
    LLD_REG32_FINS(baseAddr + MCSPI_CHCTRL(csNum), MCSPI_CH0CTRL_EN, MCSPI_CH0CTRL_EN_ACT);

    return;
}


/*
 * Design: MCAL-6376,MCAL-6446,MCAL-6621,MCAL-6600
 */
Spi_JobResultType Spi_mcspiContinueTxRx(const Spi_HwUnitObjType *hwUnitObj,
                                        const Spi_JobObjType *jobObj,
                                        Spi_ChannelObjType *chObj)
{
    uint32 baseAddr = 0U;
    uint32 csNum = 0U;
    uint32 intrStatus = 0U;
    uint32 txEmptyMask = 0U;
    uint32 rxFullMask = 0U;
    Spi_JobResultType jobResult = SPI_JOB_PENDING;
    Spi_McspiExternalDeviceConfigType *extDevCfg = (Spi_McspiExternalDeviceConfigType *)NULL_PTR;
    uint32 numWordsToRead = 0U;
    uint32 numWordsToWrite = 0U;

    baseAddr    = hwUnitObj->baseAddr;
    extDevCfg   = &jobObj->extDevCfg->mcspi;
    csNum       = (uint32)(jobObj->jobCfg_PC.csPin);
    txEmptyMask = Spi_mcspiGetTxMask(csNum);
    rxFullMask  = Spi_mcspiGetRxMask(csNum);

    /* Get interrupt status */
    intrStatus = LLD_REG32_RD(baseAddr + MCSPI_IRQSTATUS);

    /* clear interrupt status */
    LLD_REG32_WR(baseAddr + MCSPI_IRQSTATUS, intrStatus);

    /*
     * Read out all RX data first in case RX mode is enabled.
     */
    if (extDevCfg->txRxMode == SPI_TX_RX_MODE_BOTH)
    {
        if ((intrStatus & rxFullMask) == rxFullMask)
        {
            /* RX is full, read RX FIFO level, TX and RX Fifo depth remains same */
            numWordsToRead = (uint32) ((uint32) chObj->numWordsTxRx -
                                       (uint32) chObj->curRxWords);
            if (numWordsToRead > chObj->effTxFifoDepth)
            {
                numWordsToRead = chObj->effTxFifoDepth; /* Limit to FIFO depth */
            }
            Spi_mcspiReadFifo(chObj, baseAddr, csNum, numWordsToRead);
        }
    }

    /*
     * Handle TX events
     */
    if ((intrStatus & txEmptyMask) == txEmptyMask)
    {
        numWordsToWrite = (uint32) ((uint32) chObj->numWordsTxRx -
                                    (uint32) chObj->curTxWords);
        if (numWordsToWrite > chObj->effTxFifoDepth)
        {
            numWordsToWrite = chObj->effTxFifoDepth; /* Limit to FIFO depth */
        }
        Spi_mcspiWriteFifo(chObj, baseAddr, csNum, numWordsToWrite);
    }

    /*
     * TX/RX is complete
     */
    if ((intrStatus & MCSPI_IRQSTATUS_EOW_MASK) == MCSPI_IRQSTATUS_EOW_MASK)
    {
        if (chObj->numWordsTxRx == chObj->curTxWords)
        {
			Spi_mcspci_continueTxRx_conditons(baseAddr,csNum);

            /* read the last data if any from Rx FIFO. */
            if ((extDevCfg->txRxMode != SPI_TX_RX_MODE_TX_ONLY) &&
                (chObj->numWordsTxRx != chObj->curRxWords))
            {
                /* This is a corner case. EOW is set at the end of transmission.
                    * the reception is not complete by the time we are processing EOW.
                    * Read the remaining bytes.
                    */
                Spi_mcspiReadFifo(chObj, baseAddr, csNum, 
                                 (uint32)(((uint32)chObj->numWordsTxRx) - 
                                           ((uint32)chObj->curRxWords)));
            }

            /* Clear all previous interrupt status */
            Spi_mcspiClearAllIrqStatus(baseAddr);
            /* This channel transfer is complete */
            jobResult = SPI_JOB_OK;
        }
        else
        {
            /* Initiate the reminder transfer */
            Spi_mcspiInitiateChunkXfer(chObj, jobObj, baseAddr);
        }
    }

    return (jobResult);
}

static void Spi_mcspci_continueTxRx_conditons(uint32 baseAddr, uint32 csNum)
{
	
	TickType startCount = 0U;
    uint32 tempCount = 0U;
    uint32 elapsedCount = 0U;
	uint32 chStat = 0U;
	StatusType  status = E_OK;
	
	status = GetCounterValue(SPI_OS_COUNTER_ID, &startCount);

	if (((StatusType) E_OK) == status)
	{
		
	do{ 
		/* Below API can change start time, so use temp variable */
		tempCount = startCount;
		status = GetElapsedValue(SPI_OS_COUNTER_ID,&tempCount,&elapsedCount);
		if ((((StatusType) E_OK) != status) ||
		   (elapsedCount >= SPI_TIMEOUT_DURATION))
		{
		   /* timeout */
		   break;
		}
		/* Wait for end of transfer*/
		chStat = LLD_REG32_RD(baseAddr + MCSPI_CHSTAT(csNum));
	  }while ((chStat & MCSPI_CH0STAT_EOT_MASK) == 0U);
	}			 
}

Spi_JobResultType Spi_mcspiXferJob(const Spi_HwUnitObjType *hwUnitObj, Spi_JobObjType *jobObj)
{
    uint32              index = 0U;
    Spi_ChannelType     chId = 0U;
    Spi_ChannelObjType *chObj = (Spi_ChannelObjType *)NULL_PTR;
    Spi_JobResultType   jobResult = SPI_JOB_OK;

    /* Transfer all the channels */
    for (index = 0U; index < jobObj->jobCfg.channelPerJob; index++)
    {
        chId  = jobObj->jobCfg.channelList[jobObj->curChIdx];
        chObj = Spi_getCurrChannelObj(chId);

        /* Configure and start the channel */
        Spi_mcspiConfigCh(hwUnitObj, jobObj, chObj);
        if (0U == index)
        {
            /* First channel */
            Spi_mcspiStart(hwUnitObj, jobObj, (uint32) FALSE);
        }
        else
        {
            /* Non-first channel - just restart is sufficient */
            Spi_mcspiReStart(hwUnitObj, jobObj);
        }

        /* Busy loop till channel transfer is completed */
        do
        {
            jobResult = Spi_mcspiContinueTxRx(hwUnitObj, jobObj, chObj);
        }
        while (SPI_JOB_PENDING == jobResult);

        /* Move to next channel */
        jobObj->curChIdx++;
    }

    Spi_mcspiStop(hwUnitObj, jobObj);

    return (jobResult);
}

void Spi_mcspiStop(const Spi_HwUnitObjType *hwUnitObj, const Spi_JobObjType *jobObj)
{
    uint32 baseAddr = 0U;
    uint32 csNum = 0U;
    uint32 intrMask = 0U;
    uint32 csIntrMask = 0U;
    Spi_McspiExternalDeviceConfigType *extDevCfg = (Spi_McspiExternalDeviceConfigType *)NULL_PTR;

    baseAddr  = hwUnitObj->baseAddr;
    extDevCfg = &jobObj->extDevCfg->mcspi;
    csNum     = (uint32)(jobObj->jobCfg_PC.csPin);

    /* Deassert CS */
    if (SPI_CONTINUOUS == extDevCfg->csMode)
    {
        LLD_REG32_FINS(baseAddr + MCSPI_CHCONF(csNum), MCSPI_CH0CONF_FORCE, MCSPI_CH0CONF_FORCE_DEASSERT);
    }

    /* Disable channel */
    LLD_REG32_FINS(baseAddr + MCSPI_CHCTRL(csNum), MCSPI_CH0CTRL_EN, MCSPI_CH0CTRL_EN_NACT);

    /* Disable EOW, TX and RX interrupts */
    intrMask   = LLD_REG32_RD(baseAddr + MCSPI_IRQENABLE);
    intrMask  &= ~(MCSPI_IRQSTATUS_EOW_MASK);
    csIntrMask = Spi_mcspiGetCsIntrMask(csNum, extDevCfg->txRxMode);
    intrMask  &= ~(csIntrMask);
    LLD_REG32_WR(baseAddr + MCSPI_IRQENABLE, intrMask);

    /* Disable TX and RX FIFO - This is required so that next CS can
     * use the FIFO - done as per McSPI spec */
    LLD_REG32_FINS(baseAddr + MCSPI_CHCONF(csNum), MCSPI_CH0CONF_FFEW, MCSPI_CH0CONF_FFEW_FFDISABLED);
    LLD_REG32_FINS(baseAddr + MCSPI_CHCONF(csNum), MCSPI_CH0CONF_FFER, MCSPI_CH0CONF_FFER_FFDISABLED);

    /* Clear all previous interrupt status */
    Spi_mcspiClearAllIrqStatus(baseAddr);

}

void Spi_mcspiClearAllIrqStatus(uint32 baseAddr)
{
    /* Clear all previous interrupt status */
    LLD_REG32_FINS(baseAddr + MCSPI_SYST, MCSPI_SYST_SSB, MCSPI_SYST_SSB_OFF);
    LLD_REG32_WR(baseAddr + MCSPI_IRQSTATUS, MCSPI_IRQSTATUS_CLEAR_ALL);

    return;
}

void Spi_mcspiDisableAllIntr(uint32 baseAddr)
{
    uint32 intrMask = 0U;

    /* Disable all interrupts */
    intrMask   = LLD_REG32_RD(baseAddr + MCSPI_IRQENABLE);
    intrMask  &= ~(MCSPI_IRQSTATUS_CLEAR_ALL);
    LLD_REG32_WR(baseAddr + MCSPI_IRQENABLE, intrMask);

    return;
}

#if (STD_ON == SPI_REGISTER_READBACK_API)
void Spi_mcspiRegReadback(
    const Spi_HwUnitObjType *hwUnitObj, Spi_RegisterReadbackType *RegRbPtr)
{
    uint32 baseAddr = 0U;

    baseAddr = hwUnitObj->baseAddr;
    RegRbPtr->mcspiHlRev      	  = LLD_REG32_RD(baseAddr + MCSPI_HL_REV);
    RegRbPtr->mcspiHlHwInfo    	  = LLD_REG32_RD(baseAddr + MCSPI_HL_HWINFO);
    RegRbPtr->mcspiHlSysConfig 	  = LLD_REG32_RD(baseAddr + MCSPI_HL_SYSCONFIG);
    RegRbPtr->mcspiRev         	  = LLD_REG32_RD(baseAddr + MCSPI_REVISION);
    RegRbPtr->mcspiSysStatus   	  = LLD_REG32_RD(baseAddr + MCSPI_SYSSTATUS);
    RegRbPtr->mcspiSyst           = LLD_REG32_RD(baseAddr + MCSPI_SYST);
    RegRbPtr->mcspiModulctrl   	  = LLD_REG32_RD(baseAddr + MCSPI_MODULCTRL);
    RegRbPtr->mcspiSysConfig      = LLD_REG32_RD(baseAddr + MCSPI_SYSCONFIG);
    RegRbPtr->mcspiCh0config      = LLD_REG32_RD(baseAddr + MCSPI_CH0CONF);
    RegRbPtr->mcspiCh1config      = LLD_REG32_RD(baseAddr + MCSPI_CH1CONF);
    RegRbPtr->mcspiCh2config      = LLD_REG32_RD(baseAddr + MCSPI_CH2CONF);
    RegRbPtr->mcspiCh3config      = LLD_REG32_RD(baseAddr + MCSPI_CH3CONF);
    RegRbPtr->mcspiIrqenable  	  = LLD_REG32_RD(baseAddr + MCSPI_IRQENABLE);
}
#endif  /* #if (STD_ON == SPI_REGISTER_READBACK_API) */

/*
 * Design: MCAL-6533,MCAL-6454,MCAL-6697,MCAL-6524,MCAL-6627
 */
static void Spi_mcspiReadFifo(Spi_ChannelObjType *chObj, uint32 baseAddr, uint32 csNum, uint32 numWordsToRead)
{
    if(NULL_PTR != chObj->curRxBufPtr)
    {
        if(1U == chObj->bufWidth)
        {
            chObj->curRxBufPtr = Spi_mcspiFifoRead8(baseAddr, csNum, chObj->curRxBufPtr, numWordsToRead,
                                                    chObj->dataWidthBitMask);
        }
        else if(2U == chObj->bufWidth)
        {
            chObj->curRxBufPtr = (uint8 *) Spi_mcspiFifoRead16(baseAddr, csNum, (uint16 *) chObj->curRxBufPtr,
                                                               numWordsToRead, chObj->dataWidthBitMask);
        }
        else
        {
            chObj->curRxBufPtr = (uint8 *) Spi_mcspiFifoRead32(baseAddr, csNum, (uint32 *) chObj->curRxBufPtr,
                                     numWordsToRead, chObj->dataWidthBitMask);
        }
    }
    else
    {
        /* NULL RX pointer provided. Read and discard data */
        Spi_mcspiFifoReadDiscard(baseAddr, csNum, numWordsToRead);
    }
    chObj->curRxWords += numWordsToRead;

}
/*
 * Design: MCAL-6533,MCAL-6454,MCAL-6697,MCAL-6414,MCAL-6584
 */
static void Spi_mcspiWriteFifo(Spi_ChannelObjType *chObj, uint32 baseAddr, uint32 csNum, uint32 numWordsToWrite)
{
    if(NULL_PTR != chObj->curTxBufPtr)
    {
        if(1U == chObj->bufWidth)
        {
            chObj->curTxBufPtr = Spi_mcspiFifoWrite8(baseAddr, csNum, chObj->curTxBufPtr,
                                                     numWordsToWrite);
        }
        else if(2U == chObj->bufWidth)
        {
            chObj->curTxBufPtr = (const uint8 *) Spi_mcspiFifoWrite16(baseAddr, csNum, (const uint16 *) chObj->curTxBufPtr,
                                                                      numWordsToWrite);
        }
        else
        {
            chObj->curTxBufPtr = (const uint8 *) Spi_mcspiFifoWrite32(baseAddr, csNum, (const uint32 *) chObj->curTxBufPtr,
                                                                      numWordsToWrite);
        }
    }
    else
    {
        /* NULL TX pointer provided. Use default data */
        Spi_mcspiFifoWriteDefault(baseAddr, csNum, chObj->chCfg.defaultTxData, numWordsToWrite);
    }
    chObj->curTxWords += (Spi_NumberOfDataType) numWordsToWrite;

}

static void Spi_mcspiInitiateChunkXfer(const Spi_ChannelObjType *chObj, const Spi_JobObjType *jobObj,
                                       uint32 baseAddr)
{
    uint32 reminder = 0U;
    uint32 regVal = 0U;
    uint32 csNum = 0U;

    csNum = (uint32)(jobObj->jobCfg_PC.csPin);

    /* Disable channel so that new settings takes effect */
    LLD_REG32_FINS(baseAddr + MCSPI_CHCTRL(csNum), MCSPI_CH0CTRL_EN, MCSPI_CH0CTRL_EN_NACT);

    /* Set FIFO trigger level and word count to be equal to the
     * reminder word. Otherwise the HW doesn't generating TX
     * empty interrupt if WCNT is less than FIFO trigger level */
    reminder = (((uint32) chObj->numWordsTxRx) & (chObj->effTxFifoDepth - 1U));

    regVal   = LLD_REG32_RD(baseAddr + MCSPI_XFERLEVEL);
    regVal  &= ~(MCSPI_XFERLEVEL_AFL_MASK | MCSPI_XFERLEVEL_AEL_MASK);
    if (jobObj->rxFifoTrigLvl != 1U)
    {
        regVal |= (((reminder - 1U) << MCSPI_XFERLEVEL_AFL_SHIFT) & MCSPI_XFERLEVEL_AFL_MASK);
    }
    regVal |= (((reminder - 1U) << MCSPI_XFERLEVEL_AEL_SHIFT) & MCSPI_XFERLEVEL_AEL_MASK);
    regVal &= ~MCSPI_XFERLEVEL_WCNT_MASK;
    regVal |= ((reminder << MCSPI_XFERLEVEL_WCNT_SHIFT) & MCSPI_XFERLEVEL_WCNT_MASK);
    LLD_REG32_WR(baseAddr + MCSPI_XFERLEVEL, regVal);

    /* Enable channel */
    LLD_REG32_FINS(baseAddr + MCSPI_CHCTRL(csNum), MCSPI_CH0CTRL_EN, MCSPI_CH0CTRL_EN_ACT);

}

uint32 Spi_mcspiGetCsIntrMask(uint32 csNum, Spi_TxRxMode txRxMode)
{
    uint32 csIntrMask = 0U;

    if ((uint32)SPI_CS0 == csNum)
    {
        csIntrMask |= (uint32) MCSPI_IRQENABLE_TX0_EMPTY_ENABLE_MASK;
        if (SPI_TX_RX_MODE_BOTH == txRxMode)
        {
            csIntrMask |= (uint32) MCSPI_IRQENABLE_RX0_FULL_ENABLE_MASK;        }
    }
    else if ((uint32)SPI_CS1 == csNum)
    {
        csIntrMask |= (uint32) MCSPI_IRQENABLE_TX1_EMPTY_ENABLE_MASK;
        if (SPI_TX_RX_MODE_BOTH == txRxMode)
        {
            csIntrMask |= (uint32) MCSPI_IRQENABLE_RX1_FULL_ENABLE_MASK;
        }
    }
    else if ((uint32)SPI_CS2 == csNum)
    {
        csIntrMask |= (uint32) MCSPI_IRQENABLE_TX2_EMPTY_ENABLE_MASK;
        if (SPI_TX_RX_MODE_BOTH == txRxMode)
        {
            csIntrMask |= (uint32) MCSPI_IRQENABLE_RX2_FULL_ENABLE_MASK;
        }
    }
    else
    {
        csIntrMask |= (uint32) MCSPI_IRQENABLE_TX3_EMPTY_ENABLE_MASK;
        if (SPI_TX_RX_MODE_BOTH == txRxMode)
        {
            csIntrMask |= (uint32) MCSPI_IRQENABLE_RX3_FULL_ENABLE_MASK;
        }
    }

    return (csIntrMask);
}

static uint32 Spi_mcspiGetTxMask(uint32 csNum)
{
    uint32 txEmptyMask = 0U;

    if ((uint32)SPI_CS0 == csNum)
    {
        txEmptyMask = (uint32) MCSPI_IRQSTATUS_TX0_EMPTY_MASK;
    }
    else if ((uint32)SPI_CS1 == csNum)
    {
        txEmptyMask = (uint32) MCSPI_IRQSTATUS_TX1_EMPTY_MASK;
    }
    else if ((uint32)SPI_CS2 == csNum)
    {
        txEmptyMask = (uint32) MCSPI_IRQSTATUS_TX2_EMPTY_MASK;
    }
    else
    {
        txEmptyMask = (uint32) MCSPI_IRQSTATUS_TX3_EMPTY_MASK;
    }

    return (txEmptyMask);
}

static uint32 Spi_mcspiGetRxMask(uint32 csNum)
{
    uint32 rxFullMask = 0U;

    if ((uint32)SPI_CS0 == csNum)
    {
        rxFullMask = (uint32) MCSPI_IRQSTATUS_RX0_FULL_MASK;
    }
    else if ((uint32)SPI_CS1 == csNum)
    {
        rxFullMask = (uint32) MCSPI_IRQSTATUS_RX1_FULL_MASK;
    }
    else if ((uint32)SPI_CS2 == csNum)
    {
        rxFullMask = (uint32) MCSPI_IRQSTATUS_RX2_FULL_MASK;
    }
    else
    {
        rxFullMask = (uint32) MCSPI_IRQSTATUS_RX3_FULL_MASK;
    }

    return (rxFullMask);
}

static void Spi_mcspiReset(uint32 baseAddr)
{
    StatusType status = 0U;
    TickType startCount = 0U;
    TickType tempCount = 0U;
    TickType elaspsedCount = 0U;

    /* Set the SOFTRESET field of MCSPI_SYSCONFIG register. */
    LLD_REG32_FINS(baseAddr + MCSPI_SYSCONFIG, MCSPI_SYSCONFIG_SOFTRESET, MCSPI_SYSCONFIG_SOFTRESET_ON);

    status = GetCounterValue(SPI_OS_COUNTER_ID, &startCount);
    if (((StatusType) E_OK) == status)
    {
        /* Stay in the loop until reset is done. */
        while ((MCSPI_SYSSTATUS_RESETDONE_MASK & LLD_REG32_RD(baseAddr + MCSPI_SYSSTATUS)) 
                != MCSPI_SYSSTATUS_RESETDONE_MASK)
        {
            /* SPI_CoverageGap_08:Dynamic Code coverage for this statement is not covered
             * because the hardware reset ends before timeout
             */
            /* Below API can change start time, so use temp variable */
            tempCount = startCount;
            status    = GetElapsedValue(SPI_OS_COUNTER_ID, &tempCount, &elaspsedCount);
            if ((((StatusType) E_OK) != status) || (elaspsedCount >= SPI_TIMEOUT_DURATION))
            {
                /* timeout */
#if (SPI_E_HARDWARE_ERROR != SPI_DEM_NO_EVENT)
                (void)Dem_SetEventStatus(SPI_E_HARDWARE_ERROR, DEM_EVENT_STATUS_FAILED);
#endif
                break;
            }
        }
    }

}

static void Spi_mcspiConfigDataPinDir(const Spi_HwUnitObjType *hwUnitObj, const Spi_JobObjType *jobObj)
{
    uint32 baseAddr = 0U;
    uint32 csNum = 0U;
    Spi_McspiExternalDeviceConfigType *extDevCfg = (Spi_McspiExternalDeviceConfigType *)NULL_PTR;

    baseAddr  = hwUnitObj->baseAddr;
    csNum     = (uint32)(jobObj->jobCfg_PC.csPin);
    extDevCfg = &jobObj->extDevCfg->mcspi;

    /* Enable Rx Data Line IS */
    LLD_REG32_FINS(baseAddr + MCSPI_CHCONF(csNum), MCSPI_CH0CONF_IS,
                   extDevCfg->receptionLineEnable);

    /* Enable Tx Data Lines DPE0/DPE1 */
    LLD_REG32_FINS(baseAddr + MCSPI_CHCONF(csNum), MCSPI_CH0CONF_DPE0,
                  ((uint32)extDevCfg->transmissionLineEnable & (uint32)0x1U));
    LLD_REG32_FINS(baseAddr + MCSPI_CHCONF(csNum), MCSPI_CH0CONF_DPE1,
                  (((uint32)extDevCfg->transmissionLineEnable & 
                   (uint32)0x2U) >> 1U));

    return;
}

static inline const uint8 *Spi_mcspiFifoWrite8(uint32 baseAddr, uint32 chNum,
                                               const uint8  *bufPtr,
                                               uint32 transferLength)
{
    uint32 index = 0U;
    uint32 txData = 0U;

    /* Write the data in TX FIFO for 8-bit transfer */
    for(index = 0U; index < transferLength; index++)
    {
        txData = *bufPtr;
        LLD_REG32_WR(baseAddr + MCSPI_CHTX(chNum), txData);
        bufPtr++;
    }

    return (bufPtr);
}

static inline const uint16 *Spi_mcspiFifoWrite16(uint32 baseAddr, uint32 chNum,
                                                 const uint16 *bufPtr,
                                                 uint32 transferLength)
{
    uint32 index = 0U;
    uint32 txData = 0U;

    /* Write the data in TX FIFO for 16-bit transfer */
    for(index = 0U; index < transferLength; index++)
    {
        txData = *bufPtr;
        LLD_REG32_WR(baseAddr + MCSPI_CHTX(chNum), txData);
        bufPtr++;
    }

    return (bufPtr);
}

static inline const uint32 *Spi_mcspiFifoWrite32(uint32 baseAddr, uint32 chNum,
                                                 const uint32 *bufPtr,
                                                 uint32 transferLength)
{
    uint32 index = 0U;
    uint32 txData = 0U;

    /* Write the data in TX FIFO for 32-bit transfer */
    for(index = 0U; index < transferLength; index++)
    {
        txData = *bufPtr;
        LLD_REG32_WR(baseAddr + MCSPI_CHTX(chNum), txData);
        bufPtr++;
    }

    return (bufPtr);
}

static inline void Spi_mcspiFifoWriteDefault(uint32 baseAddr, uint32 chNum,
                                             uint32 defaultTxData,
                                             uint32 transferLength)
{
    uint32 index = 0U;

    /* Write default data to TX FIFO */
    for(index = 0U; index < transferLength; index++)
    {
        LLD_REG32_WR(baseAddr + MCSPI_CHTX(chNum), defaultTxData);
    }

}

static inline uint8 *Spi_mcspiFifoRead8(uint32  baseAddr, uint32  chNum,
                                        uint8  *bufPtr, uint32  transferLength,
                                        uint32  dataWidthBitMask)
{
    uint32 index = 0U;
    uint32 rxData = 0U;

    /* Read the data from RX FIFO for 8-bit transfer */
    for(index = 0U; index < transferLength; index++)
    {
        rxData = LLD_REG32_RD(baseAddr + MCSPI_CHRX(chNum));
        rxData &= dataWidthBitMask;         /* Clear unused bits */
        *bufPtr = (uint8) rxData;
        bufPtr++;
    }

    return (bufPtr);
}

static inline uint16 *Spi_mcspiFifoRead16(uint32  baseAddr, uint32  chNum,
                                          uint16  *bufPtr, uint32  transferLength,
                                          uint32  dataWidthBitMask)
{
    uint32 index = 0U;
    uint32 rxData = 0U;

    /* Read the data from RX FIFO for 16-bit transfer */
    for(index = 0U; index < transferLength; index++)
    {
        rxData = LLD_REG32_RD(baseAddr + MCSPI_CHRX(chNum));
        rxData &= dataWidthBitMask;         /* Clear unused bits */
        *bufPtr = (uint16) rxData;
        bufPtr++;
    }

    return (bufPtr);
}

static inline uint32 *Spi_mcspiFifoRead32(uint32  baseAddr, uint32  chNum,
                                          uint32  *bufPtr, uint32  transferLength,
                                          uint32  dataWidthBitMask)
{
    uint32 index = 0U;
    uint32 rxData = 0U;

    /* Read the data from RX FIFO for 32-bit transfer */
    for(index = 0U; index < transferLength; index++)
    {
        rxData = LLD_REG32_RD(baseAddr + MCSPI_CHRX(chNum));
        rxData &= dataWidthBitMask;         /* Clear unused bits */
        *bufPtr = (uint32) rxData;
        bufPtr++;
    }

    return (bufPtr);
}

static inline void Spi_mcspiFifoReadDiscard(uint32 baseAddr, uint32 chNum,
                                            uint32 transferLength)
{
    uint32 index = 0U;
    volatile uint32 rxData = 0U;

    /* Read the data from RX FIFO and discard it */
    for(index = 0U; index < transferLength; index++)
    {
        rxData = LLD_REG32_RD(baseAddr + MCSPI_CHRX(chNum));
        (void) rxData;
    }

    return;
}


static inline uint32 Spi_mcspiGetFifoLength(uint32 baseAddr)
{
    uint32 fifoNumByte = 0U; 
    uint32 retFifoSize = 0U;

    fifoNumByte = HW_RD_FIELD32(baseAddr + MCSPI_HL_HWINFO,
                                MCSPI_HL_HWINFO_FFNBYTE);
    switch(fifoNumByte)
    {
        case MCSPI_HL_HWINFO_FFNBYTE_FF16BYTES:
             retFifoSize = 16U;
             break;
        case MCSPI_HL_HWINFO_FFNBYTE_FF32BYTES:
             retFifoSize = 32U;
             break;
        case MCSPI_HL_HWINFO_FFNBYTE_FF64BYTES:
             retFifoSize = 64U;
             break;
        case MCSPI_HL_HWINFO_FFNBYTE_FF128BYTES:
             retFifoSize = 128U;
             break;
        case MCSPI_HL_HWINFO_FFNBYTE_FF256BYTES:
             retFifoSize = 256U;
             break;
        default:
            /* By default keeping it to 128 bytes */
             retFifoSize = MCSPI_FIFO_LENGTH;
             break;
    }

    return retFifoSize;
}

#define SPI_STOP_SEC_CODE
#include "Spi_MemMap.h"
