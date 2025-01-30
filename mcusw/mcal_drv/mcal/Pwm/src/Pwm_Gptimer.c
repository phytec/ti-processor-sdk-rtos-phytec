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
 *  \file  Pwm_Gptimer.c
 *
 *  \brief This file contains the IP Specific implementation when PWM MCAL driver
           implemented through General purpose timers.
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
/** \brief Posted mode or non posted mode depening on system clock frequency as
* follows:
* Write/Read Posted
    freq (timer) < freq (OCP)/4
* Write/Read Non posted :
*   freq (timer) > freq (OCP)/4
* Know cases Clock frequency = 32 KHz **/
#define PWM_FREQ_POSTED                 (32000U)
#define PWM_TCLR_CLR_TRG_MSK            (0xFFFF3FFU)
#define PWM_TCLR_SET_TRG_OVF_MSK        (0x1400U)
#define PWM_TCLR_SET_TRG_OVFCMP_MSK     (0x1840U)
#define PWM_TCLR_SCPWM_MSK              (0x80U)
#define PWM_TCLR_SCPWM_BIT_MSK          (7U)
/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */

#ifdef PWM_USE_GPT
inline void Pwm_TimerWaitForWrite(uint32 reg, uint32 baseAddr);
void Pwm_timerPreScalerClkEnable(uint32 baseAddr, uint32 ptv);
void Pwm_timerReset(uint32 baseAddr);
static void Pwm_timerSet(uint32 baseAddr);
#if (PWM_NOTIFICATION_SUPPORTED == STD_ON)
static uint32 Pwm_channelcallback(Pwm_EdgeNotificationType Notification,
                        Pwm_ChannelType ChannelNumber, const Pwm_ChObjType *chObj);
#endif
#endif /*PWM_USE_GPT*/
/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */


/* ========================================================================== */
/*                  Internal Function Definitions                             */
/* ========================================================================== */

#define PWM_START_SEC_CODE
#include "Pwm_MemMap.h"
#ifdef PWM_USE_GPT

/**
 * \brief   Reset the Timer module.
 *
 * \param   baseAddr       Base Address of the Timer Module Register.
 *
 * \return  None.
 *
 **/
void Pwm_timerReset(uint32 baseAddr)
{
    StatusType status;
    TickType   startCount = 0U, tempCount = 0U, elaspsedCount = 0U;
    /* Reset the Timer module */
    HW_WR_FIELD32(baseAddr + TIMER_TIOCP_CFG,
                  TIMER_TIOCP_CFG_SOFTRESET,
                  TIMER_TIOCP_CFG_SOFTRESET_SOFTRESET_VALUE_1);

    status = GetCounterValue(PWM_OS_COUNTER_ID, &startCount);
    if (((StatusType) E_OK) == status)
    {
        /* Stay in the loop until reset is done. */
        do
        {
            /* Design: MCAL-7623 */
            /* Below API can change start time, so use temp variable */
            tempCount = startCount;
            status    = GetElapsedValue( PWM_OS_COUNTER_ID, &tempCount,
                                        &elaspsedCount);
            if ((((StatusType) E_OK) != status) ||
                (elaspsedCount >= PWM_TIMEOUT_DURATION))
            {
                /* PWM_CoverageGap_04: Dynamic Code coverage for this statement 
                 * is not covered because the soft reset ends before timeout
                 */
                break;
            }
        } while (HW_RD_FIELD32(baseAddr + TIMER_TIOCP_CFG,
                               TIMER_TIOCP_CFG_SOFTRESET) ==
                 (uint32) TIMER_TIOCP_CFG_SOFTRESET_SOFTRESET_VALUE_1);
    }
}

/**
 * \brief   This API will configure and enable the pre-scaler clock.
 *
 * \param   baseAddr      Base Address of the Timer Module Register.
 * \param   ptv          Pre-scale clock Timer value.
 *
 * 'ptv' can take the following values \n
 *    PWMTIMER_PRESCALER_CLK_DIV_BY_2 - Timer clock divide by 2 \n
 *    PWMTIMER_PRESCALER_CLK_DIV_BY_4 - Timer clock divide by 4 \n
 *    PWMTIMER_PRESCALER_CLK_DIV_BY_8 - Timer clock divide by 8 \n
 *    PWMTIMER_PRESCALER_CLK_DIV_BY_16 - Timer clock divide by 16 \n
 *    PWMTIMER_PRESCALER_CLK_DIV_BY_32 - Timer clock divide by 32 \n
 *    PWMTIMER_PRESCALER_CLK_DIV_BY_64 - Timer clock divide by 64 \n
 *    PWMTIMER_PRESCALER_CLK_DIV_BY_128 - Timer clock divide by 128 \n
 *    PWMTIMER_PRESCALER_CLK_DIV_BY_256 - Timer clock divide by 256 \n
 *
 * \return  None.
 *
 **/
void Pwm_timerPreScalerClkEnable(uint32 baseAddr,
                                           uint32 ptv)
{
    uint32 regVal;
    /* Clear the PTV field of TCLR */
    HW_WR_FIELD32(baseAddr + TIMER_TCLR, TIMER_TCLR_PTV, 0U);
    /* Set the PTV field and enable the pre-scaler clock */
    regVal = HW_RD_REG32(baseAddr + TIMER_TCLR);
    /* Enable Prescale mode */
    HW_SET_FIELD32(regVal, TIMER_TCLR_PRE, TIMER_TCLR_PRE_PRE_VALUE_1);
    /* Configure prescale value : value passed as argument is actually PTV+1*/
    HW_SET_FIELD32(regVal, TIMER_TCLR_PTV, (ptv - 1U));
    HW_WR_REG32(baseAddr + TIMER_TCLR, regVal);
}
/**
 * \brief   This API will initialize the timer
 *
 * \param   baseAddr      Base Address of the Timer Module Register.
 * \param   chObj         Pointer to channel object
 *
 * \return  None.
 *
 **/
void Pwm_timerCounterInit(
    const Pwm_ChObjType *chObj, uint32 baseAddr)
{
    uint16 duty;
    uint32 regVal;
    Pwm_timerSet(baseAddr);
    if ( 0x0U != chObj->chCfg.prescale)
    {
        Pwm_timerPreScalerClkEnable(baseAddr,
                                      chObj->chCfg.prescale);
    }
    else
    {
        (void)TIMERPreScalerClkDisable(baseAddr);
    }
    duty = chObj->chCfg.dutyCycle;
    /* Configure the TCLR[12] for Pulse modulation
     * configure the TCLR[11-10] for TRG to select trigger on overflow*/
    if(PWM_DUTY_50_PERCENT == duty)
    {
        if (PWM_FREQ_POSTED == chObj->chCfg.instanceClkHz)
        {
            /* Wait for previous write to complete */
            Pwm_TimerWaitForWrite(TIMER_WRITE_POST_TCLR, baseAddr);
        }
            /* Configure the Timer for Auto-reload and no compare mode */
        (void)TIMERModeConfigure(baseAddr, TIMER_AUTORLD_NOCMP_ENABLE);
        if (PWM_FREQ_POSTED == chObj->chCfg.instanceClkHz)
        {
            /* Wait for previous write to complete */
            Pwm_TimerWaitForWrite(TIMER_WRITE_POST_TCLR, baseAddr);
        }
        regVal = HW_RD_REG32(baseAddr + TIMER_TCLR);
        /* In TCLR Clear bit 10,11 and set trigger on overflow */
        regVal &= PWM_TCLR_CLR_TRG_MSK ;
        regVal |= PWM_TCLR_SET_TRG_OVF_MSK ;
        if (PWM_FREQ_POSTED == chObj->chCfg.instanceClkHz)
        {
            /* Wait for previous write to complete */
            Pwm_TimerWaitForWrite(TIMER_WRITE_POST_TCLR, baseAddr);
        }
        LLD_REG32_WR((volatile uint32 *)(baseAddr + TIMER_TCLR), regVal);
        if (PWM_FREQ_POSTED == chObj->chCfg.instanceClkHz)
        {
            /* Wait for previous write to complete */
            Pwm_TimerWaitForWrite(TIMER_WRITE_POST_TCLR, baseAddr);
        }
    }
    else /*enable with compare */
    {
        if (PWM_FREQ_POSTED == chObj->chCfg.instanceClkHz)
        {
            /* Wait for previous write to complete */
            Pwm_TimerWaitForWrite(TIMER_WRITE_POST_TCLR, baseAddr);
        }
        (void)TIMERModeConfigure(baseAddr, TIMER_AUTORLD_CMP_ENABLE);
        regVal = HW_RD_REG32(baseAddr + TIMER_TCLR);
        /* TCLR Bit 6 Enable compare mode.  CE
         * Configure the TCLR[11-10] TRG to select  trigger on overflow and match
         * Clear bit 10,11 */
        regVal &= PWM_TCLR_CLR_TRG_MSK ;
        regVal |= PWM_TCLR_SET_TRG_OVFCMP_MSK ;
        if (PWM_FREQ_POSTED == chObj->chCfg.instanceClkHz)
        {
            /* Wait for previous write to complete */
            Pwm_TimerWaitForWrite(TIMER_WRITE_POST_TCLR, baseAddr);
        }
        LLD_REG32_WR((volatile uint32 *)(baseAddr + TIMER_TCLR), regVal);
        if (PWM_FREQ_POSTED == chObj->chCfg.instanceClkHz)
        {
        /* Wait for previous write to complete */
            Pwm_TimerWaitForWrite(TIMER_WRITE_POST_TCLR, baseAddr);
        }
       }

    return;
}

/**
 * \brief   This API will initialize the timer for PWM signal generation.
 *
 * \param   chObj     Pointer to Timer channel object to store initialized
 *                    value.
 *
 * \return  None.
 **/


void Pwm_IpInit(const Pwm_ChObjType *chObj, uint32 baseAddr)
{
    /* Reset timer configuration */
    (void)TIMERResetConfigure(baseAddr, TIMER_TSICR_SFT_MASK);
    /* Reset timer module */
    Pwm_timerReset(baseAddr);
    /* Enable posted/non posted mode of timer operation. */
    if (PWM_FREQ_POSTED == chObj->chCfg.instanceClkHz)
    {
        (void)TIMERPostedModeConfig(baseAddr, TIMER_POSTED);
    }
    else
    {
        (void)TIMERPostedModeConfig(baseAddr, TIMER_NONPOSTED);
    }
    if (PWM_FREQ_POSTED == chObj->chCfg.instanceClkHz)
    {
        /* Wait for previous write to complete */
        Pwm_TimerWaitForWrite(TIMER_WRITE_POST_TCLR, baseAddr);
    }
    /* Set timer EMU mode to Emulation free */
    (void)TIMEREmuModeConfigure(baseAddr, TIMER_FREE);
    if (PWM_FREQ_POSTED == chObj->chCfg.instanceClkHz)
    {
        /* Wait for previous write to complete */
        Pwm_TimerWaitForWrite(TIMER_WRITE_POST_TCLR, baseAddr);
    }
    /* Keep channel disabled till explicitly enable by Init or  set duty APIs */
    (void)TIMERDisable(baseAddr);
    /* To implement Auto-reload/continuous timer as TCRR is reloaded
     * with the Timer Load Register (TLDR) value after a counting overflow
     **/
    Pwm_timerCounterInit(chObj, baseAddr);
    return;
}

/**
 * \brief   This API will deinitialize the timer.
 *
 * \param   chObj     Pointer to Timer HW object.
 *
 * \return  None.
 **/

#if (STD_ON == PWM_DEINIT_API)
void Pwm_IpDeInit(
    const Pwm_ChObjType *chObj)
{
    uint32 baseAddr = chObj->baseAddr;
     /*
     * Design: MCAL-6875,MCAL-7559,MCAL-7620
     */
    Pwm_IpCfgSetIdleState(chObj);
    /* Disable timer module */
    (void)TIMERDisable(baseAddr);
    (void)TIMERCounterSet(baseAddr, 0U);
    (void)TIMERReloadSet(baseAddr, 0U);
    (void)TIMERPreScalerClkDisable(baseAddr);
    /*
    * Design: MCAL-6840,MCAL-7637
    */
    (void)TIMERIntDisable(baseAddr,(TIMER_IRQENABLE_CLR_OVF_EN_FLAG_MASK |
                            TIMER_IRQENABLE_CLR_MAT_EN_FLAG_MASK));
    return;
}
#endif

/**
 * \brief   This API will set the duty cycle w.r.t timer.
 *
 * \param   baseAddr        base address of timer
 * \param   dutycycle       duty cycle in HEX
 * \param   hwperiod        period of pwm signal in ticks
 *
 * \return  None.
 **/
void Pwm_IpCfgDutyCycle(
    uint32 baseAddr, uint32 dutycycle, uint32 hwPeriod)
{
    uint32 absolute_dutyCycle;
    uint32 TLDR_VALUE             =        0U;
    uint32 TMAR_VALUE             =        0U;
     /* Design MCAL-7547,MCAL-6880,MCAL-7643,MCAL-7519,MCAL-7585 */
     /*states that PWM module shall comply below duty cycle
     * scheme.
     *  - 0x0000 means 0%.
     *  - 0x8000 means 100%.
     * AbsoluteDutyCycle = ((uint32)AbsolutePeriodTime * RelativeDutyCycle) >> 15
     */
    /* Calculating absolute duty cycle in ticks */
    absolute_dutyCycle = (uint32)(hwPeriod * ((dutycycle *100U) >> 15U) / 100U);
    /* Calculating Reload and Compare register values  */
    if (PWM_DUTY_50_PERCENT == dutycycle)
    {
        TLDR_VALUE = ((hwPeriod) / 2U);
    }
    else
    {
        TLDR_VALUE = (hwPeriod);
        TMAR_VALUE = (TLDR_VALUE- absolute_dutyCycle)- 1U;
        TMAR_VALUE = 0xFFFFFFFFU - TMAR_VALUE;
        (void)TIMERCompareSet(baseAddr,TMAR_VALUE);
    }
    TLDR_VALUE = 0xFFFFFFFFU - (TLDR_VALUE- 1U);
    (void)TIMERCounterSet(baseAddr, TLDR_VALUE);
    (void)TIMERReloadSet(baseAddr, TLDR_VALUE);
    return;
}
/**
 * \brief   This API will set the polarity w.r.t timer.
 *
 * \param   dutycycle       duty cycle in HEX
 * \param   baseAddr        base address of timer
 * \param   polarity        polarity of pwm signal
 *
 * \return  None.
 *
 * Design: MCAL-7591
 **/
void Pwm_IpCfgSetPolarity(uint16 dutyCycle, uint32 baseAddr,
                     Pwm_OutputStateType polarity)
{
    uint32 regVal;
    /* Set Initial Polarity */
    if (PWM_LOW == polarity)
    {
        /*Configure the TCLR[7] SCPWM 1U to set initial polarity to HIGH*/
        regVal = HW_RD_REG32(baseAddr + TIMER_TCLR);
        regVal |= PWM_TCLR_SCPWM_MSK ;
        LLD_REG32_WR((volatile uint32 *)(baseAddr + TIMER_TCLR), regVal);
    }
    else
    {
        /*Configure the TCLR[7] SCPWM to 0U to set initial polarity to LOW*/
        regVal = HW_RD_REG32(baseAddr + TIMER_TCLR);
        regVal &= ~(PWM_TCLR_SCPWM_MSK) ;
        LLD_REG32_WR((volatile uint32 *)(baseAddr + TIMER_TCLR), regVal);
    }
    /*
    * Design: MCAL-6779,MCAL-7635,MCA-6789,MCAL-7598,MCAL-6839,MCAL-7509
    */
    /* Handle boundary conditions of 0% */
    /* Handle boundary conditions of 100% */
    if (PWM_DUTY_0_PERCENT == dutyCycle)
    {
        /* For 0% Dutycycle Output is inverse of configured polarity parameter
         * disable timer */
        (void)TIMERDisable(baseAddr);

    }
    else if (PWM_DUTY_100_PERCENT == dutyCycle)
    {
        /* For 100% Dutycycle Output is equal configured polarity parameter
         * disable timer */
        regVal = HW_RD_REG32(baseAddr + TIMER_TCLR);
        regVal ^= ((uint32)1U<< PWM_TCLR_SCPWM_BIT_MSK);
        LLD_REG32_WR((volatile uint32 *)(baseAddr + TIMER_TCLR), regVal);
        (void)TIMERDisable(baseAddr);
    }
    else
    {
     /* MISRA C Compliance */
    }
}

/**
 * \brief   This API will set the channel to idle state
 *
 * \param   chObj           Channel object
 *
 * \return  None.
 **/
void Pwm_IpCfgSetIdleState(const Pwm_ChObjType *chObj)
{
    const Pwm_ChannelConfigType *pChannelConfig = &chObj->chCfg;
    uint32  baseAddr;
    uint32 regVal = 0U;
    baseAddr = chObj->baseAddr;
    if (PWM_HIGH == pChannelConfig->idleState)
    {
        /* Configure the TCLR[7] SCPWM 1U to set initial polarity to HIGH */
        regVal = HW_RD_REG32(baseAddr + TIMER_TCLR);
        regVal |= PWM_TCLR_SCPWM_MSK ;
        /* stop timer */
        regVal &= 0xFFFFFFFEU;
        LLD_REG32_WR((volatile uint32 *)(baseAddr + TIMER_TCLR), regVal);
    }
    else
    {
        /* Configure the TCLR[7] SCPWM to 0U to set initial polarity to LOW*/
        regVal = HW_RD_REG32(baseAddr + TIMER_TCLR);
        regVal &= ~(PWM_TCLR_SCPWM_MSK) ;
        /* stop timer*/
        regVal &= 0xFFFFFFFEU;
        LLD_REG32_WR((volatile uint32 *)(baseAddr + TIMER_TCLR), regVal);
    }
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
 * Design: MCAL-6852,MCAL-6760,MCAL-6831
 *
 **/
void Pwm_IpRegisterReadback(uint32 baseAddr,
            Pwm_RegisterReadbackType *RegRbPtr)
{
    RegRbPtr->pwmRev          = HW_RD_REG32(baseAddr + TIMER_TIDR);
    RegRbPtr->pwmTtgr         = HW_RD_REG32(baseAddr + TIMER_TTGR);
    RegRbPtr->pwmTimerSynCtrl = HW_RD_REG32(baseAddr + TIMER_TSICR);
}

#endif
/**
 * \brief   This API will wait for Timer synchronous interface control register
 *          to be clear to indicate that write/read is completed in posted mode
 *
 * \param   reg               Read/write posted mode selection
 * \param   baseAddr          Base address
 *
 * \return  None.
 **/
void Pwm_TimerWaitForWrite(uint32 reg, uint32 baseAddr)
{
    if (0U != HW_RD_FIELD32(baseAddr + TIMER_TSICR, TIMER_TSICR_POSTED))
    {
        while ((uint32) 0U != (reg & TIMERWritePostedStatusGet(baseAddr)))
        {
            /* Do nothing - Busy wait */
        }
    }
}

/**
 * \brief   This API will enable the timer
 *
 * \param   baseAddr          Base address
 *
 * \return  None.
 **/
void Pwm_IpEnable(uint32 instanceClkHz, uint32 baseAddr)
{
    if (PWM_FREQ_POSTED == instanceClkHz)
    {
        /* Wait for previous write to complete */
        Pwm_TimerWaitForWrite(TIMER_WRITE_POST_TCLR, baseAddr);
    }
    (void)TIMEREnable(baseAddr);
}
/**
 * \brief   This API will clear interrupts on a channel
 *
 * \param   pwmChannel          channel id
 *
 * \return  None.
 **/
void Pwm_IntrClear(Pwm_ChannelType pwmChannel)
{
    uint32 baseAddr = Pwm_GetBaseAddr(pwmChannel);

    (void)TIMERIntStatusClear(baseAddr, (TIMER_INT_MAT_IT_FLAG|TIMER_INT_OVF_IT_FLAG));
}
/**
 * \brief   This API will disable all interrupts on a channel
 *
 * \param   baseAddr          Base address
 *
 * \return  None.
 **/
void Pwm_IntDisableAll(Pwm_ChannelType pwmChannel)
{
    uint32 baseAddr = Pwm_GetBaseAddr(pwmChannel);

    (void)TIMERIntDisable(baseAddr, (TIMER_IRQENABLE_CLR_OVF_EN_FLAG_MASK |
                            TIMER_IRQENABLE_CLR_MAT_EN_FLAG_MASK));
}

static void Pwm_timerSet(uint32 baseAddr)
{
    (void)TIMERCounterSet(baseAddr, (PWMTIMER_MAX_RESOLUTION - PWMTICKMAXVALUE));
    (void)TIMERReloadSet(baseAddr, (PWMTIMER_MAX_RESOLUTION - PWMTICKMAXVALUE));
}


#endif /* PWM_USE_GPT*/

#define PWM_STOP_SEC_CODE
#include "Pwm_MemMap.h"

#define PWM_START_SEC_ISR_CODE
#include "Pwm_MemMap.h"
#ifdef PWM_USE_GPT

/*******************************************************************************
 *   Function Name : Pwm_ChannelNotificationISR
 ******************************************************************************/
/*! \Description: Interrupt Subroutine for Notification
 *
 *  \context    Function is called from interrupt level
 ******************************************************************************/
#if (PWM_NOTIFICATION_SUPPORTED == STD_ON)
FUNC(void, PWM_CODE_FAST) Pwm_ChannelNotificationISR(
    Pwm_ChannelType ChannelNumber)
{
    Pwm_NotifyFuncType pwmNotification = (Pwm_NotifyFuncType)NULL_PTR;
    uint32 callback = FALSE;
#if (STD_ON == PWM_DEV_ERROR_DETECT)
    if (ChannelNumber >= (uint32)PWM_MAX_NUM_CHANNELS)
    {
        /*Invalid channel */
        (void)Pwm_reportDetError(PWM_SID_ENABLE_NOTIFICATION, PWM_E_PARAM_CHANNEL);
    }
#endif
        Pwm_ChObjType *chObj = &Pwm_ChObj[ChannelNumber];
        Pwm_EdgeNotificationType Notification = chObj->Notification;
        uint32 baseAddr = chObj->baseAddr;
        uint32 regVal;
        regVal = TIMERIntEnableGet(baseAddr);
        /* On Interrupt Toggle the flag */
        switch(Pwm_Intrflag[ChannelNumber])
        {
            case PWM_HIGH:
            Pwm_Intrflag[ChannelNumber] =PWM_LOW;
            break;
            case PWM_LOW:
            Pwm_Intrflag[ChannelNumber] =PWM_HIGH;
            break;
            default:
            Pwm_Intrflag[ChannelNumber] =PWM_LOW;
            break;
        }
        /*
         * Design: MCAL-6762,MCAL-7622
         */
        if (PWM_FREQ_POSTED == chObj->chCfg.instanceClkHz)
        {
            /* Wait for previous write to complete */
            Pwm_TimerWaitForWrite(TIMER_WRITE_POST_TCLR, baseAddr);
        }
        /* Reset the interrupt flag associated to the notification*/
        (void)TIMERIntStatusClear(baseAddr, regVal);
        callback = Pwm_channelcallback(Notification,ChannelNumber, chObj);
        if ((chObj->chCfg.dutyCycle != PWM_DUTY_0_PERCENT) &&
            (chObj->chCfg.dutyCycle != PWM_DUTY_100_PERCENT))
        {
            /*
             * Design: MCAL-6810,MCAL-7630
             */
            /* Call Notification */
            if (((Pwm_NotifyFuncType)NULL_PTR!= chObj->chCfg.notificationHandler) &&
                (TRUE == callback))
            {
                pwmNotification = chObj->chCfg.notificationHandler;
                /* Based on edge notification */
                pwmNotification();

            }

        }
}

static uint32 Pwm_channelcallback(Pwm_EdgeNotificationType Notification,
                    Pwm_ChannelType ChannelNumber, const Pwm_ChObjType *chObj)
{
    uint32 callback = FALSE;
    switch (Notification)
        {
            case PWM_FALLING_EDGE:
                if (chObj->chCfg.polarity == Pwm_Intrflag[ChannelNumber])
                {
                    callback =  TRUE;
                }
                else
                {
                    callback = FALSE;
                }
                break;
            case PWM_RISING_EDGE:
            case PWM_BOTH_EDGES:
            default:
                if (chObj->chCfg.polarity != Pwm_Intrflag[ChannelNumber])
                {
                    callback =  TRUE;
                }
                else
                {
                    callback =  FALSE;
                }
                break;
        }

    return callback;
}
#endif

#endif /*PWM_USE_GPT*/
#define PWM_STOP_SEC_ISR_CODE
#include "Pwm_MemMap.h"

/* eof */
