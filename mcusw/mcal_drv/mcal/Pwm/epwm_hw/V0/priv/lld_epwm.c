/*
 *  Copyright (C) 2024 Texas Instruments Incorporated.
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

/**
 *  \file   csl_epwm.c
 *
 *  \brief  This file contains the CSL layer APIs for EPWM.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
/** \brief This is to disable HW_SYNC_BARRIER for register access */
#define MEM_BARRIER_DISABLE

#include <stdint.h>
#include <hw_include/lld_types.h>
#include <hw_include/hw_types.h>
#include <hw_include/lld_epwm.h>


/* ========================================================================== */
/*                                Macros                                      */
/* ========================================================================== */

/** \brief Time base clock high speed clock divider value 14. */
#define EPWM_TBCTL_HSPCLKDIV_14         (0x000EU)

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

/* None */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

void CSL_epwmTbTimebaseClkCfg(uint32 baseAddr,
                              uint32 tbClk,
                              uint32 moduleClk)
{
    uint32 clkDiv = (moduleClk / tbClk);
    uint32 hspClkDiv = 0U;
    uint32 lspClkDiv = 0U;
    uint32 lspClkDivSetting = 0U;
    uint32 regVal = 0U;

    if (clkDiv > EPWM_TBCTL_HSPCLKDIV_14)
    {
        /* High speed Time-base clock pre scale value */
        hspClkDiv = PWMSS_EPWM_TBCTL_HSPCLKDIV_DIV_14;
        /* Time-base clock pre scale value */
        lspClkDiv = clkDiv/EPWM_TBCTL_HSPCLKDIV_14;

        while(lspClkDiv > 1U)
        {
           lspClkDiv = lspClkDiv >> 1U;
           lspClkDivSetting++;
        }
    }
    else
    {
        /* High speed Time-base clock pre scale value */
        hspClkDiv = clkDiv / 2U;
        /* divide by 1 */
        lspClkDivSetting = PWMSS_EPWM_TBCTL_HSPCLKDIV_DIV_1;
    }

    regVal = HW_RD_REG16((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_TBCTL);
    HW_SET_FIELD32(regVal, PWMSS_EPWM_TBCTL_CLKDIV, lspClkDivSetting);
    HW_SET_FIELD32(regVal, PWMSS_EPWM_TBCTL_HSPCLKDIV, hspClkDiv);
    HW_WR_REG16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_TBCTL),
        (uint16)regVal);
}


void CSL_epwmTbPwmFreqCfg(uint32 baseAddr,
                          uint32 tbClk,
                          uint32 pwmFreq,
                          uint32 counterDir,
                          uint32 enableShadowWrite)
{
    uint32 tbPeriodCount = tbClk / pwmFreq;
    uint32 regVal = 0U;

    regVal = HW_RD_REG16((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_TBCTL);
    HW_SET_FIELD32(regVal, PWMSS_EPWM_TBCTL_PRDLD, enableShadowWrite);
    HW_SET_FIELD32(regVal, PWMSS_EPWM_TBCTL_CTRMODE, counterDir);
    HW_WR_REG16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_TBCTL),
        (uint16)regVal);

    if (CSL_EPWM_TB_COUNTER_DIR_UP_DOWN == counterDir)
    {
        regVal = tbPeriodCount / ((uint32) 2U);
    }
    else
    {
        regVal = tbPeriodCount - ((uint32) 1U);
    }
    HW_WR_REG16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_TBPRD),
        (uint16)regVal);
}


void CSL_epwmTbSyncEnable(uint32 baseAddr,
                          uint32 tbPhsValue,
                          uint32 counterDir)
{
    /* counter direction configuration. */
    HW_WR_FIELD16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_TBCTL),
        PWMSS_EPWM_TBCTL_PHSDIR, (uint16)counterDir);

    /* phase value configuration */
    HW_WR_FIELD16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_TBPHS),
        PWMSS_EPWM_TBPHS, (uint16)tbPhsValue);

    /* Enable sync: Load time base counter with phase register value. */
    HW_WR_FIELD16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_TBCTL),
        PWMSS_EPWM_TBCTL_PHSEN, (uint16)PWMSS_EPWM_TBCTL_PHSEN_LOAD);
}


void CSL_epwmTbSyncDisable(uint32 baseAddr)
{
    HW_WR_FIELD16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_TBCTL),
        PWMSS_EPWM_TBCTL_PHSEN, (uint16)PWMSS_EPWM_TBCTL_PHSEN_DO_NOT_LOAD);
}


void CSL_epwmTbSetSyncOutMode(uint32 baseAddr, uint32 syncOutMode)
{
    HW_WR_FIELD16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_TBCTL),
        PWMSS_EPWM_TBCTL_SYNCOSEL, (uint16)syncOutMode);
}


void CSL_epwmTbTriggerSwSync(uint32 baseAddr)
{
    HW_WR_FIELD16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_TBCTL),
        PWMSS_EPWM_TBCTL_SWFSYNC,
        (uint16)PWMSS_EPWM_TBCTL_SWFSYNC_FORCE_SYNC);
}


void CSL_epwmTbWriteTbCount(uint32 baseAddr, uint32 tbCount)
{
    HW_WR_FIELD16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_TBCNT),
        PWMSS_EPWM_TBCNT, (uint16)tbCount);
}


uint16 CSL_epwmTbReadTbCount(uint32 baseAddr)
{
    return (HW_RD_FIELD16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_TBCNT),
        PWMSS_EPWM_TBCNT));
}


uint16 CSL_epwmTbGetStatus(uint32 baseAddr, uint32 tbStatusMask)
{
    return ((HW_RD_REG16((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_TBSTS)) &
        ((uint16)tbStatusMask));
}


void CSL_epwmTbStatusClear(uint32 baseAddr, uint32 tbStatusClrMask)
{
    uint32 regVal =
        (uint32)HW_RD_REG16((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_TBSTS);
    regVal |= ((tbStatusClrMask) & (PWMSS_EPWM_TBSTS_CTRMAX_MASK |
        PWMSS_EPWM_TBSTS_SYNCI_MASK));

    HW_WR_REG16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_TBSTS),
        (uint16)regVal);
}


void CSL_epwmTbSetEmulationMode(uint32 baseAddr, uint32 mode)
{
    HW_WR_FIELD16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_TBCTL),
        PWMSS_EPWM_TBCTL_FREE_SOFT, (uint16)mode);
}


uint32 CSL_epwmCounterComparatorCfg(uint32 baseAddr,
                                      uint32 cmpType,
                                      uint32 cmpVal,
                                      uint32 enableShadowWrite,
                                      uint32 shadowToActiveLoadTrigger,
                                      uint32 overwriteShadow)
{
    uint32 status = (uint32)FALSE;
    uint32 regVal =
        (uint32)HW_RD_REG16((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_CMPCTL);

    if (CSL_EPWM_CC_CMP_A == cmpType)
    {
        if ((TRUE == overwriteShadow) ||
            (PWMSS_EPWM_CMPCTL_SHDWAFULL_FIFO_NOT_FULL ==
                HW_GET_FIELD(regVal, PWMSS_EPWM_CMPCTL_SHDWAFULL)))
        {
            HW_SET_FIELD32(regVal,
                PWMSS_EPWM_CMPCTL_SHDWAMODE, enableShadowWrite);
            HW_SET_FIELD32(regVal, PWMSS_EPWM_CMPCTL_LOADAMODE,
                shadowToActiveLoadTrigger);
            HW_WR_REG16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_CMPCTL),
                (uint16)regVal);

            HW_WR_FIELD16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_CMPA),
                PWMSS_EPWM_CMPA, (uint16)cmpVal);

            status = (uint32)TRUE;
        }
    }
    else if (CSL_EPWM_CC_CMP_B == cmpType)
    {
        if ((TRUE == overwriteShadow) ||
            (PWMSS_EPWM_CMPCTL_SHDWBFULL_FIFO_NOT_FULL ==
                HW_GET_FIELD(regVal, PWMSS_EPWM_CMPCTL_SHDWBFULL)))
        {
            HW_SET_FIELD32(regVal,
                PWMSS_EPWM_CMPCTL_SHDWBMODE, enableShadowWrite);
            HW_SET_FIELD32(regVal, PWMSS_EPWM_CMPCTL_LOADBMODE,
                shadowToActiveLoadTrigger);
            HW_WR_REG16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_CMPCTL),
                (uint16)regVal);

            HW_WR_FIELD16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_CMPB),
                PWMSS_EPWM_CMPB, (uint16)cmpVal);

            status = (uint32)TRUE;
        }
    }
    else
    {
        /* This error does not happen because of check done already */
    }

    return status;
}


void CSL_epwmAqActionOnOutputCfg(uint32 baseAddr,
                                 uint32 pwmOutputCh,
                                 const CSL_EpwmAqActionCfg_t *pCfg)
{
    uint32 regVal = 0U;

    if (CSL_EPWM_OUTPUT_CH_A == pwmOutputCh)
    {
        regVal =
            HW_RD_REG16((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_AQCTLA);
        HW_SET_FIELD32(regVal, PWMSS_EPWM_AQCTLA_CBD, pCfg->cmpBDownAction);
        HW_SET_FIELD32(regVal, PWMSS_EPWM_AQCTLA_CBU, pCfg->cmpBUpAction);
        HW_SET_FIELD32(regVal, PWMSS_EPWM_AQCTLA_CAD, pCfg->cmpADownAction);
        HW_SET_FIELD32(regVal, PWMSS_EPWM_AQCTLA_CAU, pCfg->cmpAUpAction);
        HW_SET_FIELD32(regVal, PWMSS_EPWM_AQCTLA_PRD, pCfg->prdAction);
        HW_SET_FIELD32(regVal, PWMSS_EPWM_AQCTLA_ZRO, pCfg->zeroAction);
        HW_WR_REG16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_AQCTLA),
            (uint16)regVal);
    }
    else if (CSL_EPWM_OUTPUT_CH_B == pwmOutputCh)
    {
        regVal =
            HW_RD_REG16((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_AQCTLB);
        HW_SET_FIELD32(regVal, PWMSS_EPWM_AQCTLB_CBD, pCfg->cmpBDownAction);
        HW_SET_FIELD32(regVal, PWMSS_EPWM_AQCTLB_CBU, pCfg->cmpBUpAction);
        HW_SET_FIELD32(regVal, PWMSS_EPWM_AQCTLB_CAD, pCfg->cmpADownAction);
        HW_SET_FIELD32(regVal, PWMSS_EPWM_AQCTLB_CAU, pCfg->cmpAUpAction);
        HW_SET_FIELD32(regVal, PWMSS_EPWM_AQCTLB_PRD, pCfg->prdAction);
        HW_SET_FIELD32(regVal, PWMSS_EPWM_AQCTLB_ZRO, pCfg->zeroAction);
        HW_WR_REG16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_AQCTLB),
            (uint16)regVal);
    }
    else
    {
        /* This error does not happen because of check done already */
    }
}


void CSL_epwmAqSwTriggerOneTimeAction(uint32 baseAddr,
                                      uint32 pwmOutputCh,
                                      uint32 swTrigAction)
{
    uint32 regVal = 0U;

    if (CSL_EPWM_OUTPUT_CH_A == pwmOutputCh)
    {
        regVal =
            HW_RD_REG16((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_AQSFRC);

        /* Configure the software triggered one-time action for A channel */
        HW_SET_FIELD32(regVal, PWMSS_EPWM_AQSFRC_ACTSFA, swTrigAction);
        /* Initiate single software forced event on A channel output */
        HW_SET_FIELD32(regVal,
            PWMSS_EPWM_AQSFRC_OTSFA, PWMSS_EPWM_AQSFRC_OTSFA_SW_EVENT);

        HW_WR_REG16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_AQSFRC),
            (uint16)regVal);
    }
    else if (CSL_EPWM_OUTPUT_CH_B == pwmOutputCh)
    {
        regVal =
            HW_RD_REG16((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_AQSFRC);

        /* Configure the software triggered one-time action for B channel */
        HW_SET_FIELD32(regVal, PWMSS_EPWM_AQSFRC_ACTSFB, swTrigAction);
        /* Initiate single software forced event on B channel output */
        HW_SET_FIELD32(regVal,
            PWMSS_EPWM_AQSFRC_OTSFB, PWMSS_EPWM_AQSFRC_OTSFB_SW_EVENT);

        HW_WR_REG16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_AQSFRC),
            (uint16)regVal);
    }
    else
    {
        /* This error does not happen because of check done already */
    }
}


void CSL_epwmAqSwTriggerContAction(uint32 baseAddr,
                                   uint32 pwmOutputCh,
                                   uint32 swTrigAction,
                                   uint32 activeRegReloadMode)
{
    /* Program AQCSFRC Active Register Reload From Shadow Options */
    HW_WR_FIELD16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_AQSFRC),
        PWMSS_EPWM_AQSFRC_RLDCSF, (uint16)activeRegReloadMode);

    if (CSL_EPWM_OUTPUT_CH_A == pwmOutputCh)
    {
        /* Continuous software forced output on A */
        HW_WR_FIELD16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_AQCSFRC),
            PWMSS_EPWM_AQCSFRC_CSFA, (uint16)swTrigAction);
    }
    else if (CSL_EPWM_OUTPUT_CH_B == pwmOutputCh)
    {
        /* Continuous software forced output on B */
        HW_WR_FIELD16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_AQCSFRC),
            PWMSS_EPWM_AQCSFRC_CSFB, (uint16)swTrigAction);
    }
    else
    {
        /* This error does not happen because of check done already */
    }
}


void CSL_epwmDeadbandCfg(uint32 baseAddr, const CSL_EpwmDeadbandCfg_t *pCfg)
{
    uint32 regVal = 0U;

    /* Configure the input source for dead-band */
    regVal = HW_RD_REG16((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_DBCTL);
    HW_SET_FIELD32(regVal, PWMSS_EPWM_DBCTL_IN_MODE, pCfg->inputMode);

    /* Configure the output mode */
    HW_SET_FIELD32(regVal, PWMSS_EPWM_DBCTL_OUT_MODE, pCfg->outputMode);

    /* Configure the polarity selection */
    HW_SET_FIELD32(regVal, PWMSS_EPWM_DBCTL_POLSEL, pCfg->polaritySelect);
    HW_WR_REG16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_DBCTL),
        (uint16)regVal);

    /* Configure rising edge delay */
    HW_WR_FIELD16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_DBRED),
        PWMSS_EPWM_DBRED_DEL, (uint16)pCfg->risingEdgeDelay);

    /* Configure falling edge delay */
    HW_WR_FIELD16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_DBFED),
        PWMSS_EPWM_DBFED_DEL, (uint16)pCfg->fallingEdgeDelay);
}


void CSL_epwmDeadbandBypass(uint32 baseAddr)
{
    HW_WR_FIELD16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_DBCTL),
        PWMSS_EPWM_DBCTL_OUT_MODE,
        (uint16)PWMSS_EPWM_DBCTL_OUT_MODE_DISABLED);
}


void CSL_epwmChopperCfg(uint32 baseAddr, const CSL_EpwmChopperCfg_t *pCfg)
{
    uint32 freqDivider = pCfg->clkFrequency;
    uint32 regVal =
        (uint32)HW_RD_REG16((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_PCCTL);

    /* Configure chopping clock duty cycle */
    HW_SET_FIELD32(regVal, PWMSS_EPWM_PCCTL_CHPDUTY, pCfg->dutyCycle);

    /* Configure chopping clock frequency */
    if (freqDivider > CSL_EPWM_CHP_CLK_FREQ_DIV_BY_8)
    {
       freqDivider =  CSL_EPWM_CHP_CLK_FREQ_DIV_BY_8;
    }
    HW_SET_FIELD32(regVal, PWMSS_EPWM_PCCTL_CHPFREQ, freqDivider);

    /* Configure pulse width of first pulse */
    HW_SET_FIELD32(regVal, PWMSS_EPWM_PCCTL_OSHTWTH, pCfg->oneShotPulseWidth);

    HW_WR_REG16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_PCCTL), regVal);
}


void CSL_epwmChopperEnable(uint32 baseAddr, uint32 enableChopper)
{
    uint32 regVal =
        (uint32)HW_RD_REG16((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_PCCTL);

    if (TRUE == enableChopper)
    {
        /* Enable chopper sub-module */
        HW_SET_FIELD32(regVal,
            PWMSS_EPWM_PCCTL_CHPEN, PWMSS_EPWM_PCCTL_CHPEN_ENABLE);
    }
    else
    {
        /* Disable chopper sub-module */
        HW_SET_FIELD32(regVal,
            PWMSS_EPWM_PCCTL_CHPEN, PWMSS_EPWM_PCCTL_CHPEN_DISABLE);
    }

    HW_WR_REG16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_PCCTL),
        (uint16)regVal);
}


void CSL_epwmTzTriggerTripAction(uint32 baseAddr,
                                 uint32 tripAction,
                                 uint32 pwmOutputCh)
{
    uint32 regVal =
        (uint32)HW_RD_REG16((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_TZCTL);

    if (CSL_EPWM_OUTPUT_CH_A == pwmOutputCh)
    {
        HW_SET_FIELD32(regVal, PWMSS_EPWM_TZCTL_TZA, tripAction);
    }
    else if (CSL_EPWM_OUTPUT_CH_B == pwmOutputCh)
    {
        HW_SET_FIELD32(regVal, PWMSS_EPWM_TZCTL_TZB, tripAction);
    }
    else
    {
        /* This error does not happen because of check done already */
    }

    HW_WR_REG16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_TZCTL),
        (uint16)regVal);
}


void CSL_epwmTzTripEventEnable(uint32 baseAddr,
                               uint32 tzEventType,
                               uint32 pinNum)
{
    uint32 regVal =
        (uint32)HW_RD_REG16((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_TZSEL);

    if (CSL_EPWM_TZ_EVENT_ONE_SHOT == tzEventType)
    {
        HW_SET_FIELD32(
            regVal, PWMSS_EPWM_TZSEL_OSHTN, ((uint32)1U << pinNum));
    }
    else if (CSL_EPWM_TZ_EVENT_CYCLE_BY_CYCLE == tzEventType)
    {
        HW_SET_FIELD32(
            regVal, PWMSS_EPWM_TZSEL_CBCN, ((uint32) 1U << pinNum));
    }
    else
    {
        /* This error does not happen because of check done already */
    }

    HW_WR_REG16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_TZSEL),
        (uint16)regVal);
}


void CSL_epwmTzTripEventDisable(uint32 baseAddr,
                                uint32 tzEventType,
                                uint32 pinNum)
{
    uint32 regVal =
        (uint32)HW_RD_REG16((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_TZSEL);

    if (CSL_EPWM_TZ_EVENT_ONE_SHOT == tzEventType)
    {
        regVal &= (~((((uint32) 1U << pinNum) << PWMSS_EPWM_TZSEL_OSHTN_SHIFT)
            & (PWMSS_EPWM_TZSEL_OSHTN_MASK)));
    }
    else if (CSL_EPWM_TZ_EVENT_CYCLE_BY_CYCLE == tzEventType)
    {
        regVal &= (~((((uint32) 1U << pinNum) << PWMSS_EPWM_TZSEL_CBCN_SHIFT)
            & (PWMSS_EPWM_TZSEL_CBCN_MASK)));
    }
    else
    {
        /* This error does not happen because of check done already */
    }

    HW_WR_REG16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_TZSEL),
        (uint16)regVal);
}


void CSL_epwmTzIntrEnable(uint32 baseAddr, uint32 tzEventType)
{
    uint32 regVal =
        (uint32)HW_RD_REG16((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_TZEINT);

    if (CSL_EPWM_TZ_EVENT_ONE_SHOT == tzEventType)
    {
        HW_SET_FIELD32(regVal,
            PWMSS_EPWM_TZEINT_OST, PWMSS_EPWM_TZEINT_OST_ENABLE);
    }
    else if (CSL_EPWM_TZ_EVENT_CYCLE_BY_CYCLE == tzEventType)
    {
        HW_SET_FIELD32(regVal,
            PWMSS_EPWM_TZEINT_CBC, PWMSS_EPWM_TZEINT_CBC_ENABLE);
    }
    else
    {
        /* This error does not happen because of check done already */
    }

    HW_WR_REG16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_TZEINT),
        (uint16)regVal);
}


void CSL_epwmTzIntrDisable(uint32 baseAddr, uint32 tzEventType)
{
    uint32 regVal =
        (uint32)HW_RD_REG16((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_TZEINT);

    if (CSL_EPWM_TZ_EVENT_ONE_SHOT == tzEventType)
    {
        HW_SET_FIELD32(regVal,
            PWMSS_EPWM_TZEINT_OST, PWMSS_EPWM_TZEINT_OST_DISABLE);
    }
    else if (CSL_EPWM_TZ_EVENT_CYCLE_BY_CYCLE == tzEventType)
    {
        HW_SET_FIELD32(regVal,
            PWMSS_EPWM_TZEINT_CBC, PWMSS_EPWM_TZEINT_CBC_DISABLE);
    }
    else
    {
        /* This error does not happen because of check done already */
    }

    HW_WR_REG16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_TZEINT),
        (uint16)regVal);
}


uint16 CSL_epwmTzEventStatus(uint32 baseAddr, uint32 eventMask)
{
    return ((HW_RD_REG16((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_TZFLG)) &
        ((uint16)eventMask));
}


void CSL_epwmTzEventStatusClear(uint32 baseAddr, uint32 eventMask)
{
    uint32 regVal = 0U;
    regVal = (eventMask) & (PWMSS_EPWM_TZCLR_OST_MASK |
        PWMSS_EPWM_TZCLR_CBC_MASK | PWMSS_EPWM_TZCLR_INT_MASK);
    HW_WR_REG16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_TZCLR), regVal);
}


void CSL_epwmTzTriggerSwEvent(uint32 baseAddr, uint32 tzEventType)
{
    uint32 regVal =
        (uint32)HW_RD_REG16((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_TZFRC);

    if (CSL_EPWM_TZ_EVENT_ONE_SHOT == tzEventType)
    {
        HW_SET_FIELD32(regVal,
            PWMSS_EPWM_TZFRC_OST, PWMSS_EPWM_TZFRC_OST_FORCE_TRIP);
    }
    else if (CSL_EPWM_TZ_EVENT_CYCLE_BY_CYCLE == tzEventType)
    {
        HW_SET_FIELD32(regVal,
            PWMSS_EPWM_TZFRC_CBC, PWMSS_EPWM_TZFRC_CBC_FORCE_TRIP);
    }
    else
    {
        /* This error does not happen because of check done already */
    }

    HW_WR_REG16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_TZFRC),
        (uint16)regVal);
}


void CSL_epwmEtIntrCfg(uint32 baseAddr, uint32 intrEvtSrc, uint32 intrPrd)
{
    /* Configure interrupt source */
    HW_WR_FIELD16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_ETSEL),
        PWMSS_EPWM_ETSEL_INTSEL, (uint16)intrEvtSrc);

    /* Configure the interrupt period */
    HW_WR_FIELD16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_ETPS),
        PWMSS_EPWM_ETPS_INTPRD, (uint16)intrPrd);
}

void CSL_epwmEtIntrEnable(uint32 baseAddr)
{
    HW_WR_FIELD16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_ETSEL),
        PWMSS_EPWM_ETSEL_INTEN, (uint16)PWMSS_EPWM_ETSEL_INTEN_ENABLE);
}

void CSL_epwmEtIntrDisable(uint32 baseAddr)
{
    HW_WR_FIELD16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_ETSEL),
        PWMSS_EPWM_ETSEL_INTEN, (uint16)PWMSS_EPWM_ETSEL_INTEN_DISABLE);
}

uint16 CSL_epwmEtIntrStatus(uint32 baseAddr)
{
    return (HW_RD_FIELD16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_ETFLG),
        PWMSS_EPWM_ETFLG_INT));
}

void CSL_epwmEtIntrClear(uint32 baseAddr)
{
    HW_WR_FIELD16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_ETCLR),
        PWMSS_EPWM_ETCLR_INT, (uint16)PWMSS_EPWM_ETCLR_INT_CLEAR);
}

void CSL_epwmEtIntrTrigger(uint32 baseAddr)
{
    HW_WR_FIELD16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_ETFRC),
        PWMSS_EPWM_ETFRC_INT, (uint16)PWMSS_EPWM_ETFRC_INT_GENERATE_INTR);
}

uint16 CSL_epwmEtGetEventCount(uint32 baseAddr)
{
    return (HW_RD_FIELD16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_ETPS),
        PWMSS_EPWM_ETPS_INTCNT));
}


int32_t CSL_epwmHighResolutionCfg(uint32 baseAddr,
                               uint32 ctrlMode,
                               uint32 mepCtrlEdge)
{
#if defined (PWMSS_EPWM_HRCTL)
    uint32 regVal =
        (uint32)HW_RD_REG16((baseAddr + (uint32)PWMSS_EPWM_OFFSET) + (uint32)PWMSS_EPWM_HRCTL);
    HW_SET_FIELD32(regVal, PWMSS_EPWM_HRCTL_DELBUSSEL, ctrlMode);
    HW_SET_FIELD32(regVal, PWMSS_EPWM_HRCTL_DELMODE, mepCtrlEdge);
    HW_WR_REG16(((baseAddr + (uint32)PWMSS_EPWM_OFFSET) + (uint32)PWMSS_EPWM_HRCTL),
        (uint16)regVal);
    return (LLD_PASS);
#else
    return (LLD_EUNSUPPORTED_CMD);
#endif
}


void CSL_epwmHrLoadTbPhaseHrValue(uint32 baseAddr, uint32 tbPhsHighResVal)
{
    HW_WR_FIELD16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_TBPHSHR),
        PWMSS_EPWM_TBPHSHR_TBPHSH, (uint16)tbPhsHighResVal);
}


int32_t CSL_epwmHrLoadCmpAHrValue(uint32 baseAddr,
                               uint32 compAHighResVal,
                               uint32 ShadowToActiveLoadTrigger)
{
    int32_t     retVal;

    HW_WR_FIELD16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_CMPAHR),
        PWMSS_EPWM_CMPAHR, (uint16)compAHighResVal);

#if defined (PWMSS_EPWM_HRCTL)
    #if defined (CSL_HRPWM_HRCTL)
        /* HRCTL is at a different base address and hence this API is not supported
           as only EPWM base address can be passed for this API */
        retVal = LLD_EUNSUPPORTED_CMD;
    #else
        HW_WR_FIELD16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_HRCTL),
            PWMSS_EPWM_HRCTL_PULSESEL, (uint16)ShadowToActiveLoadTrigger);
        retVal = LLD_PASS;
    #endif
#else
    retVal = LLD_EUNSUPPORTED_CMD;
#endif
    return (retVal);
}


int32_t CSL_epwmHighResolutionDisable(uint32 baseAddr)
{
#if defined (PWMSS_EPWM_HRCTL)
    HW_WR_FIELD16(((baseAddr + (uint32)PWMSS_EPWM_OFFSET) + (uint32)PWMSS_EPWM_HRCTL),
        PWMSS_EPWM_HRCTL_DELMODE, (uint16)PWMSS_EPWM_HRCTL_DELMODE_NO_DELAY);
    return (LLD_PASS);
#else
    return (LLD_EUNSUPPORTED_CMD);
#endif
}


int32_t CSL_epwmClockEnable(uint32 baseAddr)
{
    int32_t retVal = LLD_PASS;
#if defined (PWMSS_CLK_CFG_EPWM_CLK_ENABLE)
    HW_WR_FIELD32((baseAddr + CSL_PWMSS_CLKCONFIG),
        CSL_PWMSS_CLKCONFIG_EPWMCLK_EN,
        PWMSS_CLK_CFG_EPWM_CLK_ENABLE);
#else
    /* The clock control is SoC specific operation.
     * Example, for AM65xx, it is in WAKUP Control and for K2G it is in bootCfg.
     * Hence this implementation should be done from application.
     * Supporting this API for earlier SOCs such as AM572x, TDA parts for
     * backwards compatibility
     * the ePWM IP. This file is meant to update only ePWM registers
     */
     retVal = LLD_EUNSUPPORTED_CMD;
#endif
	return (retVal);
}


int32_t CSL_epwmClockDisable(uint32 baseAddr)
{
    int32_t retVal = LLD_PASS;
#if defined (PWMSS_CLK_CFG_EPWM_CLK_STOP)
    HW_WR_FIELD32((baseAddr + CSL_PWMSS_CLKCONFIG),
        CSL_PWMSS_CLKCONFIG_EPWMCLKSTOP_REQ,
        PWMSS_CLK_CFG_EPWM_CLK_STOP);
#else
    /* The clock control is SoC specific operation.
     * Example, for AM65xx, it is in WAKUP Control and for K2G it is in bootCfg.
     * Hence this implementation should be done from application.
     * Supporting this API for earlier SOCs such as AM572x, TDA parts for
     * backwards compatibility
     * the ePWM IP. This file is meant to update only ePWM registers
     */
     retVal = LLD_EUNSUPPORTED_CMD;
#endif
	return (retVal);
}
