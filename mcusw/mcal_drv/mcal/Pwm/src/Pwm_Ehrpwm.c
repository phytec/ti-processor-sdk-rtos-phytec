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
 *  \file  Pwm_Ehrpwm.c
 *
 *  \brief This file contains the IP Specific implementation when PWM MCAL driver
           implemented through EHRPWM.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "Pwm_Priv.h"
#include "Os.h"
/* There are static inline functions in hw_types.h file. Map them as well */
#define PWM_START_SEC_CODE
#include "Pwm_MemMap.h"
#include <hw_include/hw_types.h>
#define PWM_STOP_SEC_CODE
#include "Pwm_MemMap.h"
/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
#define PWM_TBCTL_DEFAULT       (0xC033U)   /* Default TBCTL Values */
#define PWM_TBPRD_DEFAULT       (0x0000U)
#define PWM_CMPCTRL_DEFAULT     (0x000FU)
#define PWM_CMPA_DEFAULT        (0x0000U)
#define PWM_CMPB_DEFAULT        (0x0000U)
#define PWM_AQCTLA_DEFAULT      (0x0000U)
#define PWM_AQCTLB_DEFAULT      (0x0000U)
#define PWM_AQSFRC_DEFAULT      (0x0000U)
#define PWM_AQCSFRC_DEFAULT     (0x0000U)
#define PWM_ETSEL_DEFAULT       (0x0000U)
#define PWM_ETPS_DEFAULT        (0x0000U)
#define PWM_ETCLR_DEFAULT       (0x0000U)
#define PWM_CMPAHR_DEFAULT      (0x0000U)
#define PWM_TBPHSHR_DEFAULT     (0x0000U)
#define PWM_HRCTL_DEFAULT       (0x0000U)




/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */
#ifdef PWM_USE_EPWM
static void Pwm_AqActionOnCntUpOutputCfg_epwm(uint32 baseAddr,
                                        uint32 outputCh,
                                        uint32 actionQualType);
static void Pwm_AqActionOnCntDwnOutputCfg_epwm(uint32 baseAddr,
                                         uint32 outputCh,
                                         uint32 actionQualType);
static void Pwm_HwSetDefReg_epwm(uint32 baseAddr);
static void Pwm_TimebaseClkCfg_epwm(uint32 baseAddr, uint32 lspClkDiv,
                               uint32 hspClkDiv);
static void Pwm_HwCfgCmpShdwMode_epwm(uint32 baseAddr, uint32 outputCh);
static void Pwm_HighResConfig_epwm(uint32 baseAddr, uint32 outputCh, boolean enableHR);

#endif /*PWM_USE_EPWM*/

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */


/* ========================================================================== */
/*                  Internal Function Definitions                             */
/* ========================================================================== */

#define PWM_START_SEC_CODE
#include "Pwm_MemMap.h"
#ifdef PWM_USE_EPWM

void Pwm_IpInit_epwm(const Pwm_ChObjType *chObj, uint32 baseAddr)
{
    const Pwm_ChannelConfigType *pChannelConfig = &chObj->chCfg;
    const Pwm_ChannelConfigType_PC *channelConfig_PC = &chObj->chCfg_PC;

    /* Set registers to default value as described in Macros section */
    Pwm_HwSetDefReg_epwm(baseAddr);

    /* Time Base Sub Module Config */
    Pwm_TimebaseClkCfg_epwm(baseAddr, pChannelConfig->prescale,
                       pChannelConfig->hsPrescale);

    /* Set Period */
    Pwm_SetTimeBasePeriod_epwm(baseAddr, pChannelConfig->hwPeriod);

    /* Enable/Disable Comparator Shadow Mode */
    Pwm_HwCfgCmpShdwMode_epwm(baseAddr, channelConfig_PC->outputCh);

    Pwm_HighResConfig_epwm(baseAddr, channelConfig_PC->outputCh, pChannelConfig->enableHR);

    return;
}

#if (STD_ON == PWM_DEINIT_API)
void Pwm_IpDeInit_epwm(
    const Pwm_ChObjType *chObj)
{
    uint32 baseAddr = chObj->baseAddr;
    /* Set registers to default value */
    Pwm_HwSetDefReg_epwm(baseAddr);

    Pwm_IpCfgSetIdleState_epwm(chObj);

    /* Disable all Notifications */
    Pwm_EtIntrDisable_epwm(baseAddr);

    Pwm_EtIntrClear_epwm(baseAddr);

    /* Explicit PWM clock disable must not be performed by driver.
     * Refer TRM for more details */
    return;
}
#endif

void Pwm_IpCfgDutyCycle_epwm(
                    uint32 baseAddr, uint32 dutycycle, uint32 hwPeriod,
                    uint32 outputCh, boolean enableHR, uint32 sysClk)
{
    uint32 absolute_dutyCycle;
    double duty_cycle_percent;
    uint32 duty_cycle_percent_int;
    double duty_cycle_percent_dec;
    double duty_cycle_period;
    double temp_val;
    uint32 MEP_step = 180U; /*180ps choosen*/

    double highRes_regVal;
    /*Calculate the MEP_SF value*/
    uint16 MEP_step_num = 1U/(sysClk * (MEP_step*0.0000000000010)); /*1/(sysclk * 180ps)*/

    /* Req SWS_Pwm_00059 states that PWM module shall comply below duty cycle
     * scheme.
     *  - 0x0000 means 0%.
     *  - 0x8000 means 100%.
     * AbsoluteDutyCycle = ((uint32)AbsolutePeriodTime * RelativeDutyCycle) >> 15
     */
    /* Calculating absolute duty cycle to be written to register */

    absolute_dutyCycle = ((uint32) hwPeriod * dutycycle) >> 15U;

    /* Calculation for high resolution */
    duty_cycle_percent = (double) (dutycycle/ (double) 32768U);
    /*0x8000 is 32768, calculating percentage from hex*/
    duty_cycle_period = (double) duty_cycle_percent * (double) hwPeriod;
    duty_cycle_percent_int = (uint32) duty_cycle_period;
    duty_cycle_percent_dec = duty_cycle_period - duty_cycle_percent_int;

    if (((uint32)EPWM_OUTPUT_CH_A == outputCh) ||
        ((uint32)EPWM_OUTPUT_CH_BOTH_A_AND_B == outputCh))
    {
        HW_WR_FIELD16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_CMPA),
            PWMSS_EPWM_CMPA, (uint16)absolute_dutyCycle);
            if (TRUE == enableHR)
            {

                temp_val = (double)(duty_cycle_percent_dec * MEP_step_num);
                highRes_regVal = (temp_val * (double)256U) + (double)0x180U;

                /*Write to CMPAHR register*/
                CSL_epwmHrLoadCmpAHrValue2(baseAddr, baseAddr + EHRPWM_BASE_OFFSET,
                                               highRes_regVal,
                                               CSL_EPWM_HR_REG_ACT_LOAD_CNT_ZRO_PULSE);
            }
    }

    if (((uint32)EPWM_OUTPUT_CH_B == outputCh) ||
        ((uint32)EPWM_OUTPUT_CH_BOTH_A_AND_B == outputCh))
    {
        HW_WR_FIELD16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_CMPB),
            PWMSS_EPWM_CMPB, (uint16)absolute_dutyCycle);
    }

    return;
}

void Pwm_IpCfgSetPolarity_epwm(uint16 dutyCycle, uint32 baseAddr,
                     Pwm_OutputStateType polarity, uint32 outputCh)
{
    /* Clear Action qualifiers */
    Pwm_AqSwTriggerContAction_epwm(baseAddr, outputCh,
                              CSL_EPWM_AQ_SW_TRIG_CONT_ACTION_NOEFFECT,
                              CSL_EPWM_AQ_CSFRC_REG_RELOAD_IMMEDIATE);
    Pwm_AqSwTriggerOneTimeAction_epwm(baseAddr, outputCh,
                                 CSL_EPWM_AQ_SW_TRIG_OT_ACTION_DONOTHING);

    /* Set Initial Polarity */
    if (PWM_HIGH == polarity)
    {
        /* Up-down PWM counter implies that we set the output
         * to the configured polarity when the counter is at 0
         * Then we toggle the output when the duty cycle comparevalue is reached
         */
        Pwm_AqActionOnZeroOutputCfg_epwm(baseAddr, outputCh,
                                    CSL_EPWM_AQ_ACTION_HIGH);
        /* Force output once, thereby getting the correct initial state */
        Pwm_AqSwTriggerOneTimeAction_epwm(baseAddr, outputCh,
                                     CSL_EPWM_AQ_SW_TRIG_OT_ACTION_HIGH);
    }
    else
    {
        Pwm_AqActionOnZeroOutputCfg_epwm(baseAddr, outputCh,
                                    CSL_EPWM_AQ_ACTION_LOW);
        /* Force output once, thereby getting the correct initial state */
        Pwm_AqSwTriggerOneTimeAction_epwm(baseAddr, outputCh,
                                     CSL_EPWM_AQ_SW_TRIG_OT_ACTION_LOW);
    }

    /* Handling Duty Cycle Boundary Conditions(0% and 100%) */
    Pwm_HandleDutyBoundry_epwm(dutyCycle, baseAddr, polarity, outputCh);
}

void Pwm_IpCfgSetIdleState_epwm(const Pwm_ChObjType *chObj)
{
    const Pwm_ChannelConfigType *pChannelConfig = &chObj->chCfg;
    const Pwm_ChannelConfigType_PC *channelConfig_PC = &chObj->chCfg_PC;

    uint32 swTrigAction, baseAddr;

    baseAddr = chObj->baseAddr;
    if (PWM_HIGH == pChannelConfig->idleState)
    {
        swTrigAction = CSL_EPWM_AQ_SW_TRIG_CONT_ACTION_HIGH;
    }
    else
    {
        swTrigAction = CSL_EPWM_AQ_SW_TRIG_CONT_ACTION_LOW;
    }

    Pwm_AqSwTriggerContAction_epwm(baseAddr, channelConfig_PC->outputCh, swTrigAction,
                              CSL_EPWM_AQ_CSFRC_REG_RELOAD_IMMEDIATE);
}

void Pwm_HandleDutyBoundry_epwm(
    uint16 dutyCycle, uint32 baseAddr,
    Pwm_OutputStateType polarity, uint32 outputCh)
{
    uint32 tmpActionVal;

    /* With duty cycle reached, toggle, regardless of polarity
     * on both up and down counter directions */
    Pwm_AqActionOnCntUpOutputCfg_epwm(baseAddr, outputCh,
                                 CSL_EPWM_AQ_ACTION_TOLLGE);
    Pwm_AqActionOnCntDwnOutputCfg_epwm(baseAddr, outputCh,
                                  CSL_EPWM_AQ_ACTION_TOLLGE);

    /* Special handling for 0% and 100% duty to prevent two conflicting events
     * from firing in the HW, so just set the output to constant high or low
     */
    if ((dutyCycle == PWM_DUTY_0_PERCENT) ||
        (dutyCycle == PWM_DUTY_100_PERCENT))
    {
        Pwm_AqActionOnCntUpOutputCfg_epwm(baseAddr, outputCh,
                                     CSL_EPWM_AQ_ACTION_DONOTHING);
        Pwm_AqActionOnCntDwnOutputCfg_epwm(baseAddr, outputCh,
                                     CSL_EPWM_AQ_ACTION_DONOTHING);
        Pwm_AqSwTriggerContAction_epwm(baseAddr, outputCh,
                                  CSL_EPWM_AQ_SW_TRIG_CONT_ACTION_NOEFFECT,
                                  CSL_EPWM_AQ_CSFRC_REG_RELOAD_IMMEDIATE);

        /* Inverse of the configured polarity parameter */
        if (dutyCycle == PWM_DUTY_0_PERCENT)
        {
            tmpActionVal = (polarity == PWM_HIGH ?
                                (uint32)CSL_EPWM_AQ_SW_TRIG_CONT_ACTION_LOW :
                                (uint32)CSL_EPWM_AQ_SW_TRIG_CONT_ACTION_HIGH);
        }
        else /* Equal to the configured polarity parameter */
        {
            tmpActionVal = (polarity == PWM_HIGH ?
                                (uint32)CSL_EPWM_AQ_SW_TRIG_CONT_ACTION_HIGH :
                                (uint32)CSL_EPWM_AQ_SW_TRIG_CONT_ACTION_LOW);
        }
        Pwm_AqSwTriggerContAction_epwm(baseAddr, outputCh, tmpActionVal,
                                  CSL_EPWM_AQ_CSFRC_REG_RELOAD_IMMEDIATE);
    }
}

void Pwm_AqSwTriggerOneTimeAction_epwm(uint32 baseAddr,
                                      uint32 outputCh,
                                      uint32 swTrigAction)
{
    uint32 regVal;

    /* Enable both outputs epwmxA and epwmxB which is same configuration
     * if outputCh == EPWM_OUTPUT_CH_BOTH_A_AND_B */
    if (((uint32)EPWM_OUTPUT_CH_A == outputCh) ||
        ((uint32)EPWM_OUTPUT_CH_BOTH_A_AND_B == outputCh))
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

    if (((uint32)EPWM_OUTPUT_CH_B == outputCh) ||
        ((uint32)EPWM_OUTPUT_CH_BOTH_A_AND_B == outputCh))
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

}

void Pwm_AqActionOnZeroOutputCfg_epwm(
    uint32 baseAddr, uint32 outputCh, uint32 actionQualType)
{
    uint32 regVal;

    /* Enable both outputs epwmxA and epwmxB which is same configuration
     * if outputCh == EPWM_OUTPUT_CH_BOTH_A_AND_B */
    if (((uint32)EPWM_OUTPUT_CH_A == outputCh) ||
        ((uint32)EPWM_OUTPUT_CH_BOTH_A_AND_B == outputCh))
    {
        regVal =
            HW_RD_REG16((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_AQCTLA);
        HW_SET_FIELD32(regVal, PWMSS_EPWM_AQCTLA_ZRO, actionQualType);
        HW_WR_REG16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_AQCTLA),
            (uint16)regVal);
    }

    if (((uint32)EPWM_OUTPUT_CH_B == outputCh) ||
        ((uint32)EPWM_OUTPUT_CH_BOTH_A_AND_B == outputCh))
    {
        regVal =
            HW_RD_REG16((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_AQCTLB);
        HW_SET_FIELD32(regVal, PWMSS_EPWM_AQCTLB_ZRO, actionQualType);
        HW_WR_REG16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_AQCTLB),
            (uint16)regVal);
    }

}

void Pwm_AqSwTriggerContAction_epwm(uint32 baseAddr,
                               uint32 outputCh,
                               uint32 swTrigAction,
                               uint32 activeRegReloadMode)
{
    /* Program AQCSFRC Active Register Reload From Shadow Options */
    HW_WR_FIELD16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_AQSFRC),
        PWMSS_EPWM_AQSFRC_RLDCSF, (uint16)activeRegReloadMode);

    /* Enable both outputs epwmxA and epwmxB which is same configuration
     * if outputCh == EPWM_OUTPUT_CH_BOTH_A_AND_B */
    if (((uint32)EPWM_OUTPUT_CH_A == outputCh) ||
        ((uint32)EPWM_OUTPUT_CH_BOTH_A_AND_B == outputCh))
    {
        /* Continuous software forced output on A */
        HW_WR_FIELD16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_AQCSFRC),
            PWMSS_EPWM_AQCSFRC_CSFA, (uint16)swTrigAction);
    }

    if (((uint32)EPWM_OUTPUT_CH_B == outputCh) ||
        ((uint32)EPWM_OUTPUT_CH_BOTH_A_AND_B == outputCh))
    {
        /* Continuous software forced output on B */
        HW_WR_FIELD16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_AQCSFRC),
            PWMSS_EPWM_AQCSFRC_CSFB, (uint16)swTrigAction);
    }

}

void Pwm_EtIntrCfg_epwm(
    uint32 baseAddr, uint32 intrEvtSrc, uint32 intrPrd)
{
    /* Configure interrupt source */
    HW_WR_FIELD16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_ETSEL),
        PWMSS_EPWM_ETSEL_INTSEL, (uint16)intrEvtSrc);

    /* Configure the interrupt period */
    HW_WR_FIELD16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_ETPS),
        PWMSS_EPWM_ETPS_INTPRD, (uint16)intrPrd);
}

void Pwm_EtIntrEnable_epwm(uint32 baseAddr)
{
    HW_WR_FIELD16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_ETSEL),
        PWMSS_EPWM_ETSEL_INTEN, (uint16)PWMSS_EPWM_ETSEL_INTEN_ENABLE);
}

void Pwm_EtIntrDisable_epwm(uint32 baseAddr)
{
    HW_WR_FIELD16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_ETSEL),
        PWMSS_EPWM_ETSEL_INTEN, (uint16)PWMSS_EPWM_ETSEL_INTEN_DISABLE);
}

void Pwm_EtIntrClear_epwm(uint32 baseAddr)
{
    HW_WR_FIELD16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_ETCLR),
        PWMSS_EPWM_ETCLR_INT, (uint16)PWMSS_EPWM_ETCLR_INT_CLEAR);
}

void Pwm_SetTimeBasePeriod_epwm(
    uint32 baseAddr, uint16 periodVal)
{
    HW_WR_REG16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_TBPRD), periodVal);
}

uint16 Pwm_GetTimeBasePeriod_epwm(uint32 baseAddr)
{
    return (HW_RD_FIELD16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_TBPRD),
        PWMSS_EPWM_TBPRD));
}

/* PWM_RISING_EDGE - Notification will be called when a rising edge occurs
 * on the PWM output signal.
 * PWM_FALLING_EDGE - Notification will be called when a falling edge occurs
 * on the PWM output signal.
 * PWM_BOTH_EDGES - Notification will be called when either a rising edge
 * or falling edge occur on the PWM output signal.
 * Up-Down Counter mode is used so 2 interrupt sources need to be configured
 * depending on the notification type
 */
#if (STD_ON == PWM_NOTIFICATION_SUPPORTED)
void PWM_NotifySetIntMode(
    const Pwm_ChObjType *chObj, Pwm_EdgeNotificationType Notification)
{
    uint32 baseAddr, outputCh;
    uint32 intrEvtSrcCntrInc, intrEvtSrcCntrDec;

    baseAddr = chObj->baseAddr;
    outputCh = chObj->chCfg_PC.outputCh;

    /* Set interrupt event sources based on output type & counter direction */
    if (((uint32)EPWM_OUTPUT_CH_A == outputCh) ||
        ((uint32)EPWM_OUTPUT_CH_BOTH_A_AND_B == outputCh))
    {
        /* Set interrupt event sources of output A, if dual output is enabled
         * as interrupt is per instance and not per output */
        intrEvtSrcCntrInc  = (uint32)CSL_EPWM_ET_INTR_EVT_CNT_EQ_CMPA_INC;
        intrEvtSrcCntrDec  = (uint32)CSL_EPWM_ET_INTR_EVT_CNT_EQ_CMPA_DEC;
    }
    if((uint32)EPWM_OUTPUT_CH_B == outputCh)
    {
        intrEvtSrcCntrInc  = (uint32)CSL_EPWM_ET_INTR_EVT_CNT_EQ_CMPB_INC;
        intrEvtSrcCntrDec  = (uint32)CSL_EPWM_ET_INTR_EVT_CNT_EQ_CMPB_DEC;
    }

    /* Configured to generate an interrupt on every event */
    switch (Notification)
    {
        case PWM_FALLING_EDGE:
            if (PWM_HIGH == chObj->chCfg.polarity)
            {
                Pwm_EtIntrCfg_epwm(baseAddr, intrEvtSrcCntrInc,
                              CSL_EPWM_ET_INTR_PERIOD_FIRST_EVT);
            }
            else
            {
                Pwm_EtIntrCfg_epwm(baseAddr, intrEvtSrcCntrDec,
                              CSL_EPWM_ET_INTR_PERIOD_FIRST_EVT);
            }
            break;
        case PWM_RISING_EDGE:
            if (PWM_HIGH == chObj->chCfg.polarity)
            {
                Pwm_EtIntrCfg_epwm(baseAddr, intrEvtSrcCntrDec,
                              CSL_EPWM_ET_INTR_PERIOD_FIRST_EVT);
            }
            else
            {
                Pwm_EtIntrCfg_epwm(baseAddr, intrEvtSrcCntrInc,
                              CSL_EPWM_ET_INTR_PERIOD_FIRST_EVT);
            }
            break;
        case PWM_BOTH_EDGES:
                Pwm_EtIntrCfg_epwm(baseAddr, intrEvtSrcCntrInc,
                              CSL_EPWM_ET_INTR_PERIOD_FIRST_EVT);
                Pwm_EtIntrCfg_epwm(baseAddr, intrEvtSrcCntrDec,
                            CSL_EPWM_ET_INTR_PERIOD_FIRST_EVT);
            break;
        default:
            break;
    }
}
#endif

static void Pwm_AqActionOnCntUpOutputCfg_epwm(uint32 baseAddr,
                                        uint32 outputCh,
                                        uint32 actionQualType)
{
    uint32 regVal;

    /* Enable both outputs epwmxA and epwmxB which is same configuration
     * if outputCh == EPWM_OUTPUT_CH_BOTH_A_AND_B */
    if (((uint32)EPWM_OUTPUT_CH_A == outputCh) ||
        ((uint32)EPWM_OUTPUT_CH_BOTH_A_AND_B == outputCh))
    {
        regVal =
            HW_RD_REG16((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_AQCTLA);
        HW_SET_FIELD32(regVal, PWMSS_EPWM_AQCTLA_CAU, actionQualType);
        HW_WR_REG16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_AQCTLA),
            (uint16)regVal);
    }

    if (((uint32)EPWM_OUTPUT_CH_B == outputCh) ||
        ((uint32)EPWM_OUTPUT_CH_BOTH_A_AND_B == outputCh))
    {
        regVal =
            HW_RD_REG16((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_AQCTLB);
        HW_SET_FIELD32(regVal, PWMSS_EPWM_AQCTLB_CBU, actionQualType);
        HW_WR_REG16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_AQCTLB),
            (uint16)regVal);
    }

}

static void Pwm_HwCfgCmpShdwMode_epwm(
    uint32 baseAddr, uint32 outputCh)
{
    uint32 enableShadowWrite;
    uint32 overwriteShadow;
    uint32 shadowToActiveLoadTrigger;
    uint32 regVal;

   /*
    * For updates at the end of period, leave default shadowing of CMPA
    * On, and write to the real register on period clock event.
    */
#if (PWM_DUTYCYCLE_UPDATED_ENDPERIOD == STD_ON)
    enableShadowWrite = (uint32)CSL_EPWM_SHADOW_REG_CTRL_ENABLE;
    shadowToActiveLoadTrigger = (uint32)CSL_EPWM_CC_CMP_LOAD_MODE_CNT_EQ_ZERO;
    overwriteShadow = (uint32)TRUE;
#else
    /* For Non Shadow mode, disable shadowing of the CMPA/CMPB register. */
    enableShadowWrite = (uint32)CSL_EPWM_SHADOW_REG_CTRL_DISABLE;
    overwriteShadow = FALSE;
    shadowToActiveLoadTrigger =
                           (uint32)CSL_EPWM_CC_CMP_LOAD_MODE_CNT_EQ_ZERO_OR_PRD;
#endif

    /*TI_INSPECTED 91 D : MISRAC_2012_R.17.7
     * "Reason - Tool Issue, regVal is used in this function only " */
    regVal =
        (uint32)HW_RD_REG16((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_CMPCTL);

    if (((uint32)EPWM_OUTPUT_CH_A == outputCh) ||
        ((uint32)EPWM_OUTPUT_CH_BOTH_A_AND_B == outputCh))
    {
        HW_SET_FIELD32(regVal,
            PWMSS_EPWM_CMPCTL_SHDWAMODE, enableShadowWrite);
        /*PWM_CoverageGap_05: Dynamic coverage is not possible as for
         * below HW_GET_FIELD as its dependent on hardware. 
         */
        if (((uint32) TRUE == overwriteShadow) ||
            (PWMSS_EPWM_CMPCTL_SHDWAFULL_FIFO_NOT_FULL ==
                HW_GET_FIELD(regVal, PWMSS_EPWM_CMPCTL_SHDWAFULL)))
        {
            HW_SET_FIELD32(regVal, PWMSS_EPWM_CMPCTL_LOADAMODE,
                shadowToActiveLoadTrigger);
        }
        HW_WR_REG16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_CMPCTL),
            (uint16)regVal);
    }

    if (((uint32)EPWM_OUTPUT_CH_B == outputCh) ||
        ((uint32)EPWM_OUTPUT_CH_BOTH_A_AND_B == outputCh))
    {
        HW_SET_FIELD32(regVal,
            PWMSS_EPWM_CMPCTL_SHDWBMODE, enableShadowWrite);
        /*PWM_CoverageGap_06: Dynamic coverage is not possible as for
         * below HW_GET_FIELD as its dependent on hardware. 
         */
        if (((uint32) TRUE == overwriteShadow) ||
            (PWMSS_EPWM_CMPCTL_SHDWBFULL_FIFO_NOT_FULL ==
                HW_GET_FIELD(regVal, PWMSS_EPWM_CMPCTL_SHDWBFULL)))
        {
            HW_SET_FIELD32(regVal, PWMSS_EPWM_CMPCTL_LOADBMODE,
                shadowToActiveLoadTrigger);
        }
        HW_WR_REG16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_CMPCTL),
            (uint16)regVal);
    }

    return;
}

static void Pwm_TimebaseClkCfg_epwm(
    uint32 baseAddr, uint32 lspClkDiv, uint32 hspClkDiv)
{
    uint32 regVal;

    regVal = HW_RD_REG16((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_TBCTL);
#if (PWM_PERIOD_UPDATED_ENDPERIOD == STD_ON)
    HW_SET_FIELD32(regVal, (uint32)PWMSS_EPWM_TBCTL_PRDLD,
                                       (uint32)CSL_EPWM_SHADOW_REG_CTRL_ENABLE);
#else
    HW_SET_FIELD32(regVal, (uint32)PWMSS_EPWM_TBCTL_PRDLD,
                                      (uint32)CSL_EPWM_SHADOW_REG_CTRL_DISABLE);
#endif

    /* UP_DOWN Counter Mode is selected */
    HW_SET_FIELD32(regVal, (uint32)PWMSS_EPWM_TBCTL_CTRMODE,
                                       (uint32)CSL_EPWM_TB_COUNTER_DIR_UP_DOWN);
    HW_WR_REG16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_TBCTL),
        (uint16)regVal);

    /* Initialize Prescalers */
    regVal = HW_RD_REG16((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_TBCTL);
    HW_SET_FIELD32(regVal, PWMSS_EPWM_TBCTL_CLKDIV, lspClkDiv);
    HW_SET_FIELD32(regVal, PWMSS_EPWM_TBCTL_HSPCLKDIV, hspClkDiv);
    HW_WR_REG16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_TBCTL),
        (uint16)regVal);

    /* Set Emulation Mode (Free Run Mode) */
    Pwm_TbSetEmulationMode_epwm(baseAddr, EPWM_TB_EMU_MODE_FREE_RUN);

}

static void Pwm_AqActionOnCntDwnOutputCfg_epwm(uint32 baseAddr,
                                          uint32 outputCh,
                                          uint32 actionQualType)
{
    uint32 regVal;

    /* Enable both outputs epwmxA and epwmxB which is same configuration
     * if outputCh == EPWM_OUTPUT_CH_BOTH_A_AND_B */
    if (((uint32)EPWM_OUTPUT_CH_A == outputCh) ||
        ((uint32)EPWM_OUTPUT_CH_BOTH_A_AND_B == outputCh))
    {
        regVal =
            HW_RD_REG16((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_AQCTLA);
        HW_SET_FIELD32(regVal, PWMSS_EPWM_AQCTLA_CAD, actionQualType);
        HW_WR_REG16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_AQCTLA),
            (uint16)regVal);
    }

    if (((uint32)EPWM_OUTPUT_CH_B == outputCh) ||
        ((uint32)EPWM_OUTPUT_CH_BOTH_A_AND_B == outputCh))
    {
        regVal =
            HW_RD_REG16((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_AQCTLB);
        HW_SET_FIELD32(regVal, PWMSS_EPWM_AQCTLB_CBD, actionQualType);
        HW_WR_REG16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_AQCTLB),
            (uint16)regVal);
    }

}

static void Pwm_HwSetDefReg_epwm(uint32 baseAddr)
{
    /*
     * Init all registers to default value
     */
    HW_WR_REG16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_TBCTL),
        (uint16)PWM_TBCTL_DEFAULT);
    HW_WR_REG16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_TBPRD),
        (uint16)PWM_TBPRD_DEFAULT);
    HW_WR_REG16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_CMPCTL),
        (uint16)PWM_CMPCTRL_DEFAULT);
    HW_WR_REG16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_CMPA),
        (uint16)PWM_CMPA_DEFAULT);
    HW_WR_REG16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_CMPB),
        (uint16)PWM_CMPB_DEFAULT);
    HW_WR_REG16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_AQCTLA),
        (uint16)PWM_AQCTLA_DEFAULT);
    HW_WR_REG16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_AQCTLB),
        (uint16)PWM_AQCTLB_DEFAULT);
    HW_WR_REG16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_AQSFRC),
        (uint16)PWM_AQSFRC_DEFAULT);
    HW_WR_REG16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_AQCSFRC),
        (uint16)PWM_AQCSFRC_DEFAULT);
    HW_WR_REG16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_ETSEL),
        (uint16)PWM_ETSEL_DEFAULT);
    HW_WR_REG16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_ETPS),
        (uint16)PWM_ETPS_DEFAULT);
    HW_WR_REG16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_ETCLR),
        (uint16)PWM_ETCLR_DEFAULT);
    HW_WR_REG16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_CMPAHR),
        (uint16)PWM_CMPAHR_DEFAULT);
    HW_WR_REG16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_TBPHSHR),
    (uint16)PWM_TBPHSHR_DEFAULT);
    HW_WR_REG16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_HRCTL),
        (uint16)PWM_HRCTL_DEFAULT);

    return;
}

void Pwm_TbSetEmulationMode_epwm(uint32 baseAddr, uint32 mode)
{
    HW_WR_FIELD16(((baseAddr + PWMSS_EPWM_OFFSET) + PWMSS_EPWM_TBCTL),
        PWMSS_EPWM_TBCTL_FREE_SOFT, (uint16)mode);
}

void Pwm_HighResConfig_epwm(uint32 baseAddr, uint32 outputCh, boolean enableHR)
{
    /**
     * High resolution feature is supported only on PWM A channel. If channel
     * is A then proceed with High Resolution processing.
     */
    if (enableHR == TRUE) /*only is HR enable is selected */
    {
        if (((uint32)EPWM_OUTPUT_CH_A == outputCh) ||
            ((uint32)EPWM_OUTPUT_CH_BOTH_A_AND_B == outputCh))
        {

            CSL_epwmHighResolutionCfg(
                    baseAddr + EHRPWM_BASE_OFFSET, /* Base addres of EHRPWM*/
                        PWMSS_EPWM_HRCTL_DELBUSSEL_SEL_CMPAHR,
                            PWMSS_EPWM_HRCTL_DELMODE_DELAY_FALLING_EDGE);
        }
        else
        {
            /* Disable High Resolution Feature */
            CSL_epwmHighResolutionDisable(baseAddr + EHRPWM_BASE_OFFSET);
        }
    }
    else
    {
        /* Disable High Resolution Feature */
        CSL_epwmHighResolutionDisable(baseAddr + EHRPWM_BASE_OFFSET);
    }

    return;
}

#if (STD_ON == PWM_REGISTER_READBACK_API)
/**
 * \brief   This API will read critical registers of the timer
 *
 * \param   baseAddr           Base address
 * \param   RegRbPtr           Pointer to structure of crtical registers
 *
 * \return  None.
 *
 * Design: MCAL-7540,MCAL-7513,MCAL-7553,MCAL-7557,MCAL-MCAL-7578
 *
**/
void Pwm_IpRegisterReadback_epwm(uint32 baseAddr,
            Pwm_RegisterReadbackType *RegRbPtr)
{
    RegRbPtr->pwmTbCtl  =  HW_RD_REG16((baseAddr + PWMSS_EPWM_OFFSET) +
                                        PWMSS_EPWM_TBCTL);
    RegRbPtr->pwmTbPhs  =  HW_RD_REG16((baseAddr + PWMSS_EPWM_OFFSET) +
                                        PWMSS_EPWM_TBPHS);
    RegRbPtr->pwmTbCnt  =  HW_RD_REG16((baseAddr + PWMSS_EPWM_OFFSET) +
                                        PWMSS_EPWM_TBCNT);
    RegRbPtr->pwmAqCtlA =  HW_RD_REG16((baseAddr +PWMSS_EPWM_OFFSET) +
                                        PWMSS_EPWM_AQCTLA);
    RegRbPtr->pwmAqCtlB =  HW_RD_REG16((baseAddr + PWMSS_EPWM_OFFSET) +
                                        PWMSS_EPWM_AQCTLB);
    RegRbPtr->pwmDbCtl  =  HW_RD_REG16((baseAddr + PWMSS_EPWM_OFFSET) +
                                        PWMSS_EPWM_DBCTL);
    RegRbPtr->pwmTzSel  =  HW_RD_REG16((baseAddr + PWMSS_EPWM_OFFSET) +
                                        PWMSS_EPWM_TZSEL);
    RegRbPtr->pwmTzCtl  =  HW_RD_REG16((baseAddr + PWMSS_EPWM_OFFSET) +
                                        PWMSS_EPWM_TZCTL);
    RegRbPtr->pwmPcCtl  =  HW_RD_REG16((baseAddr + PWMSS_EPWM_OFFSET) +
                                        PWMSS_EPWM_PCCTL);
    RegRbPtr->pwmTbsts  =  HW_RD_REG16((baseAddr + PWMSS_EPWM_OFFSET) +
                                        PWMSS_EPWM_TBSTS);
    RegRbPtr->pwmTbprd  =  HW_RD_REG16((baseAddr + PWMSS_EPWM_OFFSET) +
                                        PWMSS_EPWM_TBPRD);
    RegRbPtr->pwmAqsfrc  =  HW_RD_REG16((baseAddr + PWMSS_EPWM_OFFSET) +
                                        PWMSS_EPWM_AQSFRC);
    RegRbPtr->pwmAqcsfrc =  HW_RD_REG16((baseAddr +PWMSS_EPWM_OFFSET) +
                                        PWMSS_EPWM_AQCSFRC);
    RegRbPtr->pwmDbred =  HW_RD_REG16((baseAddr + PWMSS_EPWM_OFFSET) +
                                        PWMSS_EPWM_DBRED);
    RegRbPtr->pwmDbfed  =  HW_RD_REG16((baseAddr + PWMSS_EPWM_OFFSET) +
                                        PWMSS_EPWM_DBFED);
    RegRbPtr->pwmEtsel  =  HW_RD_REG16((baseAddr + PWMSS_EPWM_OFFSET) +
                                        PWMSS_EPWM_ETSEL);
}
#endif /*PWM_REGISTER_READBACK_API*/

#endif /*PWM_USE_EPWM*/


#define PWM_STOP_SEC_CODE
#include "Pwm_MemMap.h"

#define PWM_START_SEC_ISR_CODE
/*TI_INSPECTED 338 S : MISRAC_2012_D_20_1
 * "Reason - Required to comply with AUTOSAR memmap spec " */
#include "Pwm_MemMap.h"

#if (STD_ON == PWM_NOTIFICATION_SUPPORTED)
/*******************************************************************************
 *   Function Name : Pwm_ChannelNotificationISR
 ******************************************************************************/
/*! \Description: Interrupt Subroutine for Notification
 *
 *  \context    Function is called from interrupt level
 ******************************************************************************/
FUNC(void, PWM_CODE) Pwm_ChannelNotificationISR_epwm(
    Pwm_ChannelType ChannelNumber)
{
    /*TI_INSPECTED 93 S : MISRAC_2012_R.11.1
     * "Reason - Pointer typecast required here to match
     *           destination data type" */
    /*TI_INSPECTED 95 S : MISRAC_2012_R.11.4
     * "Reason - Pointer typecast required here since same element is
     * required to hold different types" */
    /*TI_INSPECTED 606 S : MISRAC_2012_R.11.1
     * "Reason - Pointer typecast required here to match
     *           destination data type" */
    Pwm_NotifyFuncType pwmNotification = (Pwm_NotifyFuncType)NULL;
    Pwm_ChObjType *chObj = &Pwm_ChObj[ChannelNumber];
    /* PWM_CoverageGap_07: Dynamic coverage is not possible
     * as chobj != NULL is verified in earlier stages.
     */ 

    if (NULL != (void *)chObj)
    {
        Pwm_EtIntrClear_epwm(chObj->baseAddr);
        /* PWM_CoverageGap_08: Dynamic coverage is not possible the condition is 
         * possible both condition should be true, if either condition fails its wont
         * execute hence MC/DC can be checked only for both true conditions. 
         */
        if ((chObj->chCfg.dutyCycle != PWM_DUTY_0_PERCENT) &&
                (chObj->chCfg.dutyCycle != PWM_DUTY_100_PERCENT))
        {
            /* Call Notification */
         /*PWM_CoverageGap_09: Dynamic coverage for false condition cannot be
          *achievable as its Executed when there is no duty cycle is assigned
          *as its verified in earlier stages itself.
          */
            if ((Pwm_NotifyFuncType)NULL_PTR != chObj->chCfg.notificationHandler)
            {
                pwmNotification = chObj->chCfg.notificationHandler;
                pwmNotification();
            }
        }
    }
}

#endif /*(STD_ON == PWM_NOTIFICATION_SUPPORTED)*/

#define PWM_STOP_SEC_ISR_CODE
/*TI_INSPECTED 338 S : MISRAC_2012_D_20_1
 * "Reason - Required to comply with AUTOSAR memmap spec " */
#include "Pwm_MemMap.h"

/* eof */
