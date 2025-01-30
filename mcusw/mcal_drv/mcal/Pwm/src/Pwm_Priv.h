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
 *  \file     Pwm_Priv.h
 *
 *  \brief    This file contains the common declarations and macros used across
 *            all the PWM MCAL driver files.
 *
 */

#ifndef PWM_PRIV_H_
#define PWM_PRIV_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "Pwm.h"
#include "Pwm_Cfg.h"



/* IP Specific include */
#include <hw_include/lld_timer.h>
#include <hw_include/lld_epwm.h>
#include <hw_include/soc.h>
#if (STD_ON == PWM_DEV_ERROR_DETECT)
#include "Det.h"
#endif
#ifdef __cplusplus
extern "C"
{
#endif
/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

#define PWMTIMER_MAX_RESOLUTION    (0xFFFFFFFFU)
#define PWMTICKMAXVALUE            (0xF0000000U)

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/** \brief Pwm configuration structure internal to driver */
typedef struct
{
    boolean               channelForcedIdle;
    /**< PWM channel forced idle status array */
    uint32                baseAddr;
    /**< Base address of the PWM Channel */
    Pwm_ChannelConfigType chCfg;
    /**< Channel configurations */
    Pwm_ChannelConfigType_PC chCfg_PC;
    /**< ChannelId configuration in PC */
    uint32               channelNotifyActiveFlag;
    /**<  Variable indicate the notification status for each channel */
    Pwm_EdgeNotificationType Notification;
    /**<  Variable indicate the type of notification  each channel */
    uint8 channelState;
    /**< Variable to indicate the state of the channel */
} Pwm_ChObjType;

extern Pwm_ChObjType Pwm_ChObj[PWM_MAX_NUM_CHANNELS];
/* Pwm_ChObj is not static storage class, as this requires to be accessed by
    debug interface provided.  */
/* ========================================================================== */
/*                         GLOBAL VARIABLES                                   */
/* ========================================================================== */

extern volatile uint8 Pwm_DrvStatus;
extern volatile Pwm_OutputStateType Pwm_Intrflag[PWM_MAX_NUM_CHANNELS];

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */
#if (PWM_SET_PERIOD_AND_DUTY_API==STD_ON) || \
    (PWM_SET_DUTY_CYCLE_API == STD_ON)
/**
 * \brief   This API will set the duty cycle for a PWM Channel
 *
 * \param   chObj      Pointer to HW channel object.
            DutyCycle  Duty cycle
 *
 * \return  None.
 *
 **/
void Pwm_SetDutyCycle_Internal(Pwm_ChObjType *chObj, uint16 DutyCycle);
#endif

/**
 * \brief   This API will report Det Error
 *
 * \param   apiId       api id
            errorId     error id
 *
 * \return
 *
 **/
#if (STD_ON == PWM_DEV_ERROR_DETECT)
void Pwm_reportDetError(uint8 apiId, uint8 errorId);
#endif /*PWM_DEV_ERROR_DETECT*/

/**
 * \brief   This API will reset PWM s/w channel object.
 *
 * \param   chObj       Pointer to channel object.
 *
 * \return  None.
 *
 **/
void Pwm_ResetChObj(Pwm_ChObjType *chObj);

/**
 * \brief   This API will copy all channel configuration into local PWM Object
 *
 * \param   chObj      Pointer to driver object.
 * \param   CfgPtr     Pointer to configuration object sent by application.
 * \param   CfgPtr_PC  Pointer to configuration object Pre_Comp sent by application.
 *
 * \return  None.
 *
 **/
void Pwm_CopyConfig(Pwm_ChObjType *chObj, const Pwm_ChannelConfigType * chCfg,
                    const Pwm_ChannelConfigType_PC * chCfg_pc);

/**
 * \brief   This API will initialize PWM channel object associated with logical
 *          channel.
 *
 * \param   chObj      Pointer to channel object.
 *
 * \return  None.
 *
 **/
void Pwm_HwUnitInit(Pwm_ChObjType *chObj);

/**
 * \brief   This API will get the base address of the PWM .
 *
 * \param   ChannelNumber       channel number
 *
 * \return  None.
 *
 **/
uint32 Pwm_GetBaseAddr(Pwm_ChannelType ChannelNumber);

/**
 * \brief   This wrapper API will set channel to Idle state .
 *
 * \param   chObj       channel object
 *
 * \return  None.
 *
 **/
void Pwm_SetIdleState(const Pwm_ChObjType *chObj);

#if (STD_ON == PWM_REGISTER_READBACK_API)
void Pwm_IpRegisterReadback(uint32 baseAddr,
           Pwm_RegisterReadbackType *RegRbPtr);
void Pwm_IpRegisterReadback_epwm(uint32 baseAddr,
          Pwm_RegisterReadbackType *RegRbPtr);
#endif /*PWM_REGISTER_READBACK_API*/

#if (STD_ON == PWM_NOTIFICATION_SUPPORTED)
void Pwm_ChannelNotificationISR(Pwm_ChannelType ChannelNumber);
void Pwm_ChannelNotificationISR_epwm(Pwm_ChannelType ChannelNumber);
#endif /*PWM_NOTIFICATION_SUPPORTED*/

/*EPWM related APIs*/
/**
 * \brief   This API will initialize the EPWM for PWM signal generation.
 *
 * \param   chObj     Pointer to EPWM channel object to store initialized
 *                    value.
 * \param   baseAddr  Address of the EPWM channel being used
 * \return  None.
 **/
void Pwm_IpInit_epwm(const Pwm_ChObjType *chObj, uint32 baseAddr);

#if (STD_ON == PWM_DEINIT_API)
/**
 * \brief   This API will deinitialize the EPWM.
 *
 * \param   chObj     Pointer to EPWM HW object.
 *
 * \return  None.
 **/
void Pwm_IpDeInit_epwm( const Pwm_ChObjType *chObj);
#endif /*PWM_DEINIT_API*/

void Pwm_AqActionOnZeroOutputCfg_epwm(
    uint32 baseAddr, uint32 outputCh, uint32 actionQualType);
void Pwm_HandleDutyBoundry_epwm(
    uint16 dutyCycle, uint32 baseAddr, Pwm_OutputStateType polarity,
    uint32 outputCh);

/**
 * \brief   This API will set the duty cycle w.r.t EPWM.
 *
 * \param   baseAddr        base address of EPWM
 * \param   dutycycle       duty cycle in HEX
 * \param   hwperiod        period of pwm signal in ticks
 * \param   outputCh        output channel path selected
 * \param   enableHR        switch to enable/disable hrpwm
 * \param   sysClk          current system clock selection
 *
 * \return  None.
 **/
void Pwm_IpCfgDutyCycle_epwm(
       uint32 baseAddr, uint32 dutycycle, uint32 hwPeriod,
       uint32 outputCh, boolean enableHR, uint32 sysClk);

/**
* \brief   This API will set the polarity w.r.t EPWM.
*
* \param   dutycycle       duty cycle in HEX
* \param   baseAddr        base address of EPWM
* \param   polarity        polarity of pwm signal
* \param   outputCh        output channel path selected
*
* \return  None.
**/
void Pwm_IpCfgSetPolarity_epwm(uint16 dutyCycle, uint32 baseAddr,
                   Pwm_OutputStateType polarity, uint32 outputCh);

/**
* \brief   This API will set the channel to idle state
*
* \param   chObj           Channel object
*
* \return  None.
**/
void Pwm_IpCfgSetIdleState_epwm(const Pwm_ChObjType *chObj);
/**
 * \brief   This API triggers the SW forced single event on the PWM output.
 *
 * \details This can be used for testing the AQ sub-module. Every call to this
 *          API will trigger a single event.
 *
 * \param   baseAddr      Base Address of PWMSS instance used.
 * \param   outputCh   PWM Output channel type either A or B
 *          'outputCh' can take values from the following enum
 *          - #CSL_EpwmOutputCh_t.
 * \param   swTrigAction  Action that needs to be taken on the PWM output.
 *          'swTrigAction' can take one of the following enum values
 *          - #CSL_EpwmAqSwTrigOtAction_t.
 */
void Pwm_AqSwTriggerOneTimeAction_epwm(uint32 baseAddr,
                                  uint32 outputCh,
                                  uint32 swTrigAction);

/**
 * \brief   This API forces output value continuously on PWM output channel.
 *          The output can be forced to low or high.
 *
 * \param   baseAddr      Base Address of PWMSS instance used.
 * \param   outputCh   PWM Output channel type either A or B
 *          'outputCh' can take values from the following enum
 *          - #CSL_EpwmOutputCh_t.
 * \param   swTrigAction             Value to be forced on the output
 *          This parameter can take values from the following enum
 *          - #CSL_EpwmAqSwTrigContAction_t
 * \param   activeRegReloadMode  Shadow to active reg load trigger.
 *          This parameter can take values from the following enum
 *          - #CSL_EpwmAqCsfrcRegReload_t
 */
void Pwm_AqSwTriggerContAction_epwm(uint32 baseAddr,
                               uint32 outputCh,
                               uint32 swTrigAction,
                               uint32 activeRegReloadMode);

uint16 Pwm_GetTimeBasePeriod_epwm(uint32 baseAddr);
void Pwm_SetTimeBasePeriod_epwm(uint32 baseAddr, uint16 periodVal);
/**
 * \brief   This API configures the Event Trigger sub-module. This API
 *          configures the interrupt source and interrupt period.
 *
 * \param   baseAddr    Base Address of PWMSS instance used.
 * \param   intrEvtSrc  Event source which triggers interrupt.
 *          This parameter can take values from the following enum
 *          - #CSL_EpwmEtIntrEvt_t.
 * \param   intrPrd     prescalar value(This determines how may selected events
 *                      need to occur before an interrupt is generated).
 *          This parameter can take values from the following enum
 *          - #CSL_EpwmEtIntrPeriod_t.
 */
void Pwm_EtIntrCfg_epwm(uint32 baseAddr,
                       uint32 intrEvtSrc,
                       uint32 intrPrd);

/**
 * \brief   This API enables the ePWM Event interrupt.
 *
 * \param   baseAddr    Base Address of PWMSS instance used.
 */
void Pwm_EtIntrEnable_epwm(uint32 baseAddr);

/**
 * \brief   This API disables the ePWM Event interrupt.
 *
 * \param   baseAddr    Base Address of PWMSS instance used.
 */
void Pwm_EtIntrDisable_epwm(uint32 baseAddr);

/**
 * \brief   This API clears the interrupt. This will clear the interrupt flag
 *          bit and enable further interrupts pulses to be generated.
 *
 * \param   baseAddr    Base Address of PWMSS instance used.
 */
void Pwm_EtIntrClear_epwm(uint32 baseAddr);

/**
 * \brief  This API configures emulation mode. This setting determines
 *         the behaviour of Timebase counter during emulation (debugging).
 *
 * \param   baseAddr      Base Address of PWMSS instance used.
 * \param   mode          Emulation mode.
 *          'mode' can take values from the following enum
 *          - #CSL_EpwmTbEmuMode_t.
 */
#if (STD_ON == PWM_NOTIFICATION_SUPPORTED)
void PWM_NotifySetIntMode(
 const Pwm_ChObjType *chObj, Pwm_EdgeNotificationType Notification);
#endif /*PWM_NOTIFICATION_SUPPORTED*/

/**
* \brief  This API configures emulation mode. This setting determines
*         the behaviour of Timebase counter during emulation (debugging).
*
* \param   baseAddr      Base Address of PWMSS instance used.
* \param   mode          Emulation mode.
*          'mode' can take values from the following enum
*          - #CSL_EpwmTbEmuMode_t.
*/
void Pwm_TbSetEmulationMode_epwm(uint32 baseAddr, uint32 mode);


/* GPT related APIs */
void Pwm_IpInit(const Pwm_ChObjType *chObj, uint32 baseAddr);
void Pwm_timerCounterInit(const Pwm_ChObjType *chObj, uint32 baseAddr);

#if (STD_ON == PWM_DEINIT_API)
void Pwm_IpDeInit( const Pwm_ChObjType *chObj);
#endif /*PWM_DEINIT_API*/

void Pwm_IpCfgDutyCycle(
   uint32 baseAddr, uint32 dutycycle, uint32 hwPeriod);
void Pwm_IpCfgSetPolarity(uint16 dutyCycle, uint32 baseAddr,
                   Pwm_OutputStateType polarity);

void Pwm_IpCfgSetIdleState(const Pwm_ChObjType *chObj);
void Pwm_IpEnable(uint32 instanceClkHz, uint32 baseAddr);
void Pwm_IntDisableAll(Pwm_ChannelType pwmChannel);
void Pwm_IntrClear(Pwm_ChannelType pwmChannel);

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef PWM_PRIV_H_ */
