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
 *  \file   Pwm_Priv.c
 *
 *  \brief  This file contains the Internal APIs for PWM.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "Pwm_Priv.h"
#include "string.h"
#include "Pwm_Cfg.h"

/* ========================================================================== */
/*                                Macros                                      */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */
 uint32 Pwm_GetBaseAddr(Pwm_ChannelType ChannelNumber);
/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

#define PWM_START_SEC_CODE
#include "Pwm_MemMap.h"

void Pwm_ResetChObj(Pwm_ChObjType *chObj)
{
    chObj->chCfg_PC.channelId  = (Pwm_ChannelType)0U;
#ifdef PWM_USE_EPWM
    chObj->chCfg_PC.outputCh      = 0U;
#endif /*PWM_USE_EPWM*/
    chObj->chCfg.instanceClkHz = 0U;
    chObj->chCfg.dutyCycle     = 0U;
    chObj->chCfg.hwPeriod      = 0U;
    chObj->chCfg.polarity      = PWM_LOW;
    chObj->chCfg.idleState     = PWM_LOW;
    chObj->chCfg.channelClass  = PWM_FIXED_PERIOD;
    chObj->chCfg.prescale      = 0U;
#ifdef PWM_USE_EPWM
    chObj->chCfg.hsPrescale    = 0U;
    chObj->chCfg.enableHR      = FALSE;
#endif /*PWM_USE_EPWM*/
#if (STD_ON == PWM_NOTIFICATION_SUPPORTED)
    chObj->chCfg.notificationHandler = (Pwm_NotifyFuncType)NULL_PTR;
    chObj->channelNotifyActiveFlag   = (uint32) FALSE;
#endif
    chObj->baseAddr = 0U;
    chObj->channelState = PWM_STATUS_UNINIT;
    return;
}

void Pwm_CopyConfig(
    Pwm_ChObjType *chObj, const Pwm_ChannelConfigType *chCfg,
    const Pwm_ChannelConfigType_PC *chCfg_pc)
{
    Pwm_ChannelType channelNumber;
    /* Copying the Config stucture to Channel Object*/
    (void)memcpy(&chObj->chCfg, &chCfg[0U], sizeof(Pwm_ChannelConfigType));

    /*Copying the pre-compile config structure to Channel Object */
    chObj->chCfg_PC.channelId= chCfg_pc->channelId;
#ifdef PWM_USE_EPWM
    chObj->chCfg_PC.outputCh= chCfg_pc->outputCh;
#endif /*PWM_USE_EPWM*/

    channelNumber = chObj->chCfg_PC.channelId;
    /* Store base address of all the configured channels */
    chObj->baseAddr = Pwm_GetBaseAddr(channelNumber);
    /* Init channelForcedIdle var default to FALSE */
    chObj->channelForcedIdle = (boolean) FALSE;
    chObj->channelState = PWM_STATUS_INIT;
    return;
}

FUNC(uint32, PWM_CODE) Pwm_GetBaseAddr(
    Pwm_ChannelType ChannelNumber)
{
    uint32 baseAddr;

#if (PWM_DEV_ERROR_DETECT == STD_ON)
    if (ChannelNumber >= (uint32) PWM_MAX_NUM_CHANNELS)
    {
        (void) Pwm_reportDetError(PWM_SID_INIT, PWM_E_PARAM_CHANNEL);
        baseAddr = 0U;
    }
    else
#endif
    {
        baseAddr = Pwm_HwUnitBaseAddr[ChannelNumber];
    }
    return (baseAddr);
}

void Pwm_SetIdleState(const Pwm_ChObjType *chObj)
{
    /*Call IP specific function to set to idle state */

#ifdef PWM_USE_GPT
    Pwm_IpCfgSetIdleState(chObj);
#endif /*PWM_USE_GPT*/

#ifdef PWM_USE_EPWM
    Pwm_IpCfgSetIdleState_epwm(chObj);
#endif /*PWM_EWPM*/

}

void Pwm_HwUnitInit(Pwm_ChObjType *chObj)
{
    uint32 baseAddr;
    Pwm_ChannelConfigType *pChannelConfig = &chObj->chCfg;

    baseAddr = chObj->baseAddr;

    /* Explicit PWM clock enable must not be performed by driver.
     * Refer TRM for more details */
#ifdef PWM_USE_GPT
    /* Initialize the IP implementing the PWM module*/
    Pwm_IpInit(chObj,baseAddr);
    /* Set Duty Cycle  */
    Pwm_IpCfgDutyCycle(baseAddr, pChannelConfig->dutyCycle, pChannelConfig->hwPeriod);
    /* Set Initial Polarity  */
    Pwm_IpCfgSetPolarity((uint16)pChannelConfig->dutyCycle, baseAddr, pChannelConfig->polarity);
    Pwm_IpEnable(pChannelConfig->instanceClkHz, baseAddr);

#endif /* PWM_USE_GPT*/

#ifdef PWM_USE_EPWM
    Pwm_ChannelConfigType_PC *channelConfig_PC = &chObj->chCfg_PC;

    /* Initialize the IP implementing the PWM module*/
    Pwm_IpInit_epwm(chObj,baseAddr);
    /* Set Duty Cycle  */
    Pwm_IpCfgDutyCycle_epwm(baseAddr, pChannelConfig->dutyCycle, pChannelConfig->hwPeriod,
                                                channelConfig_PC->outputCh, pChannelConfig->enableHR, pChannelConfig->instanceClkHz);
    /* Set Initial Polarity  */
    Pwm_IpCfgSetPolarity_epwm((uint16)pChannelConfig->dutyCycle, baseAddr, pChannelConfig->polarity, channelConfig_PC->outputCh);

#endif /* PWM_USE_EPWM*/

    return ;
}

#if (PWM_SET_PERIOD_AND_DUTY_API==STD_ON) || \
    (PWM_SET_DUTY_CYCLE_API == STD_ON)
 void Pwm_SetDutyCycle_Internal(
    Pwm_ChObjType *chObj, uint16 DutyCycle)
{
#ifdef PWM_USE_GPT
    uint32 period;
    uint32 baseAddr;
    Pwm_OutputStateType polarity;
    baseAddr = chObj->baseAddr;
    polarity = chObj->chCfg.polarity;

    /* Updating Init time dutyCycle which is used to enable notifications,
     * if the duty cycle is not 0% or 100% */
    chObj->chCfg.dutyCycle = DutyCycle;

    period   = (uint32) chObj->chCfg.hwPeriod;
    /* Update DutyCycle */
    if ((PWM_DUTY_0_PERCENT != DutyCycle) && (PWM_DUTY_100_PERCENT != DutyCycle))
    {
        Pwm_timerCounterInit(chObj, baseAddr);
        Pwm_IpCfgDutyCycle(baseAddr, DutyCycle, period);
    }
    /*
     * Design: MCAL-6839,MCAL-7509,MCAL-6751,MCAL-7511,MCAL-7599
     */
    /* Special handling for 0% and 100% duty to prevent two conflicting events
     * from firing in the HW, so just set the output to constant high or low
     */
    Pwm_IpCfgSetPolarity(DutyCycle, baseAddr, polarity);
    /* Re-activate channel if output was forced to idle */
    /*
    * Design: MCAL-6776,MCAL-7546,MCAL-6863,MCAL-7530,MCAL-6787,MCAL-7512
    */
    if ((PWM_DUTY_0_PERCENT != DutyCycle) && (PWM_DUTY_100_PERCENT != DutyCycle))
    {
        (void)TIMEREnable(baseAddr);
        if ((boolean)TRUE == chObj->channelForcedIdle)
        {
            chObj->channelForcedIdle = (boolean)FALSE;
        }
    }
#endif /*PWM_USE_GPT*/

#ifdef PWM_USE_EPWM

    uint32 period;

    uint32 baseAddr, outputCh, sysClk;
    boolean enableHR;
    Pwm_OutputStateType polarity;

    baseAddr = chObj->baseAddr;
    outputCh = chObj->chCfg_PC.outputCh;
    polarity = chObj->chCfg.polarity;
    enableHR = chObj->chCfg.enableHR;
    sysClk   = chObj->chCfg.instanceClkHz;

    /* Reactivate channel if output was forced to idle */
    /*TI_INSPECTED 331 S: MISRAC_2012_7.2
     * "Reason - External Package errors like Compiler which are
     *  not part of MCAL cannot be fixed." */
    /*TI_INSPECTED 434 S: MISRAC_2012_10.3
             * "Reason - Tool Issue Not able to analyse boolean type" */
    if ((boolean)TRUE == chObj->channelForcedIdle)
    {
        /* Disable SW Forced Action qualifiers */
        Pwm_AqSwTriggerContAction_epwm(baseAddr, outputCh,
            CSL_EPWM_AQ_SW_TRIG_CONT_ACTION_NOEFFECT,
            CSL_EPWM_AQ_CSFRC_REG_RELOAD_IMMEDIATE);
        Pwm_AqSwTriggerOneTimeAction_epwm(baseAddr, outputCh,
            CSL_EPWM_AQ_SW_TRIG_OT_ACTION_DONOTHING);
        chObj->channelForcedIdle = (boolean)FALSE;
    }

    /* Updating Init time dutyCycle which is used to enable notifications,
     * if the duty cycle is not 0% or 100% */
    chObj->chCfg.dutyCycle = DutyCycle;

    period   = (uint32) Pwm_GetTimeBasePeriod_epwm(baseAddr);

    /* Update DutyCycle */
    Pwm_IpCfgDutyCycle_epwm(baseAddr, DutyCycle, period, outputCh, enableHR, sysClk);

     /* Special handling for 0% and 100% duty to prevent two conflicting events
     * from firing in the HW, so just set the output to constant high or low
     */
    Pwm_IpCfgSetPolarity_epwm(DutyCycle, baseAddr, polarity, outputCh);

    #endif /*PWM_USE_EPWM*/


}
#endif

#if (STD_ON == PWM_DEV_ERROR_DETECT)
void Pwm_reportDetError(uint8 apiId, uint8 errorId)
{
    (void) Det_ReportError(PWM_MODULE_ID, PWM_INSTANCE_ID, apiId, errorId);
    return;
}
#endif  /* #if (STD_ON == PWM_DEV_ERROR_DETECT) */

#define PWM_STOP_SEC_CODE
#include "Pwm_MemMap.h"
