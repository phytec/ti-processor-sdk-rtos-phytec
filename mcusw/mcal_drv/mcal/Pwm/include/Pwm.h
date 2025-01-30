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
 *  \file     Pwm.h
 *
 *  \brief    This file contains interface header for PWM MCAL driver
 *
 */

 /**
 *  \defgroup MCAL_PWM_API PWM Driver API
 *
 *    This component provides services for initialization and control of
 *    the PWM unit (pulse width modulation).
 *    The PWM module generates pulses with variable pulse width.
 *    It allows the selection of the duty cycle and the
 *    signal period time.
 *
 *  \sa MCAL_PWM_CFG
 *  \sa MCAL_PWM_IRQ
 *  @{
 */

 /*
 * Below are the global requirements which are met by this PWM
 * driver which can't be mapped to a particular source ID
 */
 /*
  * Design: MCAL-6809,MCAL-7617,MCAL-6772,MCAL-7638,MCAL-6808,MCAL-7571,MCAL-6845,MCAL-7625,MCAL-6826,MCAl-7536,MCAL-6820,MCAL-7570,MCAL-6778,MCAL-7613
  */

/*
 * Below are the PWM module's environment requirements which can't be mapped
 * to this driver
 */
 /*
 * Design: MCAL-6806,MCAL-7610,MCAL-6834,MCAL-7565,MCAL-6837,MCAL-7561
 */
#ifndef PWM_H
#define PWM_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
/*
 * Design: MCAL-6888,MCAL-7551,MCAL-6824,MCAL-7572
 */
#include "Std_Types.h"
#include "Pwm_Cfg.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
/**
 *  \name PWM Driver Module SW Version Info
 *
 *  Defines for PWM Driver version used for compatibility checks
 *  @{
 */
/** \brief Driver Implementation Major Version */
#define PWM_SW_MAJOR_VERSION            (9U)
/** \brief Driver Implementation Minor Version */
#define PWM_SW_MINOR_VERSION            (0U)
/** \brief Driver Implementation Patch Version */
#define PWM_SW_PATCH_VERSION            (1U)
/* @} */



/**
 *  \name PWM Driver Module AUTOSAR Version Info
 *
 *  Defines for PWM Driver AUTOSAR version used for compatibility checks
 *  @{
 */
/** \brief AUTOSAR Major version specification implemented by PWM Driver */
#define PWM_AR_RELEASE_MAJOR_VERSION    (4U)
/** \brief AUTOSAR Minor  version specification implemented by PWM Driver */
#define PWM_AR_RELEASE_MINOR_VERSION    (3U)
/** \brief AUTOSAR Patch version specification implemented by PWM Driver */
#define PWM_AR_RELEASE_REVISION_VERSION (1U)
/* @} */

/**
 *  \name PWM Driver ID Info
 *  @{
 */
/** \brief Texas Instruments Vendor ID */
#define PWM_VENDOR_ID                   ((uint16) 44U)
/** \brief PWM Driver Module ID */
#define PWM_MODULE_ID                   ((uint16) 121U)
/** \brief PWM Driver Instance ID */

/**
 *  \name PWM Error Codes
 *
 *  Error codes returned by PWM functions
 *  @{
 */
/*
 * Design: MCAL-6847,MCAL-7527
 */

/** \brief PWM driver configuration parameter error */
#ifndef PWM_E_INIT_FAILED
#define PWM_E_INIT_FAILED              ((uint8)(0x10U))
#endif

/*
 * Design: MCAL-6833,MCAL-7611
 */

/** \brief PWM driver Not initialized error */
#ifndef PWM_E_UNINIT
#define PWM_E_UNINIT                    ((uint8)(0x11U))
#endif

/*
* Design: MCAL-6872,MCAL-7523
*/


/** \brief PWM driver Invalid param */
#ifndef PWM_E_PARAM_CHANNEL
#define PWM_E_PARAM_CHANNEL             ((uint8)(0x12U))
#endif

/*
 * Design: MCAL-6802,MCAL-7616
 */

/** \brief PWM driver Fixed period channel */
#ifndef PWM_E_PERIOD_UNCHANGEABLE
#define PWM_E_PERIOD_UNCHANGEABLE       ((uint8)(0x13U))
#endif

/*
 * Design: MCAL-6856,MCAL-7574
 */
/** \brief PWM driver Already initialized */
#ifndef PWM_E_ALREADY_INITIALIZED
#define PWM_E_ALREADY_INITIALIZED       ((uint8)(0x14U))
#endif

/*
 * Design: MCAL-6847,MCAL-7527
 */
/** \brief PWM driver pointer paramter error */
#ifndef PWM_E_PARAM_POINTER
#define PWM_E_PARAM_POINTER             ((uint8)(0x15U))
#endif

/*
 * Design: MCAL-6879,MCAL-7529
 */
/** \brief RunTime Error: API Pwm_SetPowerState is called
  *        while the PWM module is still in use */
#ifndef PWM_E_NOT_DISENGAGED
#define PWM_E_NOT_DISENGAGED            ((uint8)(0x16U))
#endif
/* @} */

/**
 *  \name PWM Service Ids
 *
 *  The Service Id is one of the argument to Det_ReportError function and
 *  is used to identify the source of the error
 *  @{
 */
/** \brief PWM driver service ID for Initialization API */
#define PWM_SID_INIT                        ((uint8)(0x0U))
/** \brief PWM driver service ID for De-Initialization API */
#define PWM_SID_DEINIT                      ((uint8)(0x1U))
/** \brief PWM driver service ID for set duty cycle API */
#define PWM_SID_SET_DUTY_CYCLE              ((uint8)(0x2U))
/** \brief PWM driver service ID for set duty cycle and period API */
#define PWM_SID_SET_PERIOD_AND_DUTY         ((uint8)(0x3U))
/** \brief PWM driver service ID for set output state to idle API */
#define PWM_SID_SET_OUTPUT_TO_IDLE          ((uint8)(0x4U))
/** \brief PWM driver service ID for get output state API */
#define PWM_SID_GET_OUTPUT_STATE            ((uint8)(0x5U))
/** \brief PWM driver service ID for disable notification API */
#define PWM_SID_DISABLE_NOTIFICATION        ((uint8)(0x6U))
/** \brief PWM driver service ID for Enable notification API */
#define PWM_SID_ENABLE_NOTIFICATION         ((uint8)(0x7U))
/** \brief PWM driver service ID for Get version info API */
#define PWM_SID_GET_VERSION_INFO            ((uint8)(0x8U))
/** \brief PWM driver service ID for Critical register read back API */
#define PWM_SID_REGISTER_READBACK           ((uint8)(0xDU))
/* @} */

/**
 *  \name PWM Driver State Values
 *
 *  The PWM Driver State Values
 *  @{
 */
/** \brief PWM driver Status uninitialized */
#define PWM_STATUS_UNINIT                   ((uint8)(0U))
/** \brief PWM driver Status Initialized */
#define PWM_STATUS_INIT                     ((uint8)(1U))

/* @} */

 /*
  * Design: MCAL-6801,MCAL-7544,MCAL-6880,MCAL-7547
  */
/* Design MCAL-6880,MCAL-7547 states that PWM module shall comply below duty cycle scheme
 *  - 0x0000 means 0%.
 *  - 0x8000 means 100%.
 *    0x8000 gives the highest resolution while allowing 100% duty cycle
 *    to be represented with a 16 bit value
 */
/** \brief 16bit value representing 0% of a period value */
#define PWM_DUTY_0_PERCENT                (0x0U)
/** \brief 16 bit value representing 100% of a period value */
#define PWM_DUTY_100_PERCENT              (0x8000U)
/** \brief 16 bit value representing 50% of a period value */
#define PWM_DUTY_50_PERCENT               (0x4000U)




/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/**
 *  \addtogroup MCAL_PWM_CFG PWM Configuration
 *  @{
 */
/*
 * Design: MCAL-6775,MCAL-7543
 */
/** \brief Possible output states of a PWM channel */
typedef enum
{
    PWM_LOW = 0U,
    /**< LOW  state */
    PWM_HIGH
    /**< HIGH state */
} Pwm_OutputStateType;

 /*
  * Design: MCAL-6836,MCAL-7576
  */
/** \brief Possible edge notification  */
typedef enum
{
    PWM_RISING_EDGE  = 1U,
    /**< Rising edges issue notification  */
    PWM_FALLING_EDGE = 2U,
    /**< Falling edges issue notification */
    PWM_BOTH_EDGES   = 3U
    /**< Both edges issue notification    */
} Pwm_EdgeNotificationType;

/*
 * Design: MCAL-6763,MCAL-7587
 */
/** \brief typedef of period value */
typedef uint32 Pwm_PeriodType;

/*
 * Design: MCAL-6883,MCAL-7505
 */
/** \brief PWM Channel type.  */
typedef uint32 Pwm_ChannelType;

/** \brief typedef of frequency value (Frequency in Hz) */
typedef uint32 Pwm_FrequencyType;

/*
 * Design: MCAL-6796,MCAL-7589,MCAL-7597
 */
/** \brief Period type for pwm channels.
 * Fixed periods prohibit changes to the period length with Pwm_SetPeriod( ).
 * PWM_FIXED_PERIOD_SHIFTED type is not supported due to hardware restrictions
 * of the PWM unit.
 */
typedef enum
{
    PWM_FIXED_PERIOD = 0U,
    /**< Period is immutable                      */
    PWM_FIXED_PERIOD_SHIFTED,
    /**< Period is immutable with phase shift     */
    PWM_VARIABLE_PERIOD
    /**< Period is variable                       */
} Pwm_ChannelClassType;

/** \brief EPWM outputs in a single epwm channel. */
typedef enum Pwm_epwmOutputCh
{
    EPWM_OUTPUT_CH_A  = 0U,
    /**< Output channel A. */
    EPWM_OUTPUT_CH_B  = 1U,
    /**< Output channel B. */
    EPWM_OUTPUT_CH_BOTH_A_AND_B = 2U
    /**< Both Output channel A and B */
} Pwm_epwmOutputCh_t;

/** \brief  Notification callback function pointer  */
typedef void (*Pwm_NotifyFuncType)(void);
/** \brief  Pwm channel configuration */
typedef struct
{
    Pwm_ChannelType      channelId;
#ifdef PWM_USE_EPWM
    uint32               outputCh;
    /**< Output Select EPWMxA or EPWMxB or both(EPWMxA and EPWMxB) */
#endif /* PWM_USE_EPWM*/
} Pwm_ChannelConfigType_PC;

/** \brief Pwm configuration structure */
typedef struct Pwm_ConfigType_PC_s
{
    Pwm_ChannelConfigType_PC chCfg[PWM_NUM_CHANNELS];
    /**< Channel configurations */
} Pwm_ConfigType_PC;


/** \brief  Pwm channel configuration */
typedef struct
{
    uint32               instanceClkHz;
    /**< System clock frequency */
    uint16               dutyCycle;
    /**< Initial dutycycle */
    uint32               hwPeriod;
    /**< Number of HW Unit ticks value which sets initial period */
    Pwm_OutputStateType  polarity;
    /**< Initial polarity */
    Pwm_OutputStateType  idleState;
    /**< Initial idle state */
    Pwm_ChannelClassType channelClass;
    /**< Fixed or Variable period */
    uint32               prescale;
    /**< Prescaler, part of PWM HW time-base clock
      *  Range - 0 <= prescale <= 7,  TBCLK = SYSCLK / (prescale)
      *  0x0 = /1 (No prescale)
      *  0x1 = /2
      *  0x2 = /4
      *  0x3 = /8
      *  0x4 = /16
      *  0x5 = /32
      *  0x6 = /64
      *  0x7 = /128
      */
#ifdef PWM_USE_EPWM
    uint32               hsPrescale;
    /**< High-Speed Prescaler, part for PWM HW time-base clock
    *  Range - 0 <= prescale <= 7,  TBCLK = SYSCLK / (hsPrescale x prescale)
    *  0x0 = /1 (No prescale)
    *  0x1 = /2
    *  0x2 = /4
    *  0x3 = /6
    *  0x4 = /8
    *  0x5 = /10
    *  0x6 = /12
    *  0x7 = /14
    */
    boolean             enableHR;
    /**< Enable High Resolution Capacbility */
#endif /*PWM_USE_EPWM*/
#if (PWM_NOTIFICATION_SUPPORTED == STD_ON)
    Pwm_NotifyFuncType   notificationHandler;
    /**< Notification handler */
#endif
} Pwm_ChannelConfigType;

/*
 * Design: MCAL-6885,MCAL-7533,MCAL-6799,MCAL-7535
 */
/** \brief Pwm configuration structure */
typedef struct Pwm_ConfigType_s
{
    Pwm_ChannelConfigType chCfg[PWM_NUM_CHANNELS];
    /**< Channel configurations */
} Pwm_ConfigType;

#if (STD_ON == PWM_REGISTER_READBACK_API)
/**
 *  \brief PWM register readback structure
 */
typedef struct
{
#ifdef PWM_USE_GPT
    /*
     * GPT related registers
     */
    uint32 pwmRev;
    /**< IP revision identifier */
    uint32 pwmTtgr;
    /**< Reg read always 0xFFFFFFFF */
    uint32 pwmTimerSynCtrl;
    /**< Timer synchronous interface control register */
#endif
    /*
     * EPWM related registers
     */
#ifdef PWM_USE_EPWM
    uint16 pwmTbCtl;
    /**< Time base control register */
    uint16 pwmTbPhs;
    /**< Time base counter phase register */
    uint16 pwmTbCnt;
    /**< Time base counter register */
    uint16 pwmAqCtlA;
    /**< Action Qualifier Control Register for Output A */
    uint16 pwmAqCtlB;
    /**< Action Qualifier Control Register for Output B */
    uint16 pwmDbCtl;
    /** < Dead Band Generator Control Register */
    uint16 pwmTzSel;
    /** < Trip-zone Select Register */
    uint16 pwmTzCtl;
    /** < Trip-zone Control Register */
    uint16 pwmPcCtl;
    /** < PWM Chopper Control Register */
    uint16 pwmTbsts;
    /** < Time-Base Status Register  */
    uint16 pwmTbprd;
    /** < Time-Base Period Register */
    uint16 pwmAqsfrc;
    /** < Action Qualifier Software Force Register */
    uint16 pwmAqcsfrc;
    /* < Action Qualifier Continuous Software Force Register */
    uint16 pwmDbred;
    /* < Dead Band Generator Rising Edge Delay Count Register */
    uint16 pwmDbfed;
    /* < Dead Band Generator Falling Edge Delay Count Register */
    uint16 pwmEtsel;
    /* < Event Trigger Selection Registe */

#endif
} Pwm_RegisterReadbackType;


#endif  /* #if (STD_ON == PWM_REGISTER_READBACK_API) */

/* @} */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  \brief Service for PWM initialization.
 *
 *  \verbatim
 *  Service name        : Pwm_Init
 *  Syntax              : Pwm_Init(P2CONST(Pwm_ConfigType,
 *                                  AUTOMATIC, PWM_PBCFG) ConfigPtr)
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]     : 0x00
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Non Reentrant
 *  Parameters (in)     : Pointer to the configuration
 *  Parameters (out)    : None
 *  Return value        : void
 *  Description         : Initializes internal variables
 *                        and the PWM unit of the micro controller.
 *                        The function can be called on task level.
 *  \endverbatim
 */
FUNC(void, PWM_CODE) Pwm_Init(
        P2CONST(Pwm_ConfigType, AUTOMATIC, PWM_PBCFG) CfgPtr);

#if (STD_ON == PWM_DEINIT_API)
/**
 *  \brief Service for PWM de-initialization.
 *
 *  \verbatim
 *  Service name        : Pwm_DeInit
 *  Syntax              : Pwm_DeInit(void)
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]     : 0x01
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Non Reentrant
 *  Parameters (in)     : None
 *  Parameters (out)    : None
 *  Return value        : void
 *  Description         : After DeInit output states should be in idle level
 *                       (it is configurable whether reset state is restored or
 *                        if channel output states are really set to idle).
 *                        The function can be called on task level.
 *  \endverbatim
 */
FUNC(void, PWM_CODE) Pwm_DeInit(void);
#endif

#if (STD_ON == PWM_SET_DUTY_CYCLE_API)
/**
 *  \brief Service for setting Duty Cycle
 *
 *  \verbatim
 *  Service name        : Pwm_SetDutyCycle
 *  Syntax              : Pwm_SetDutyCycle(Pwm_ChannelType Channel,
 *                                         uint16 DutyCycle)
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]     : 0x02
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : Pwm_ChannelType Channel
 *  Parameters (in)     : uint16 DutyCycle
 *  Parameters (out)    : None
 *  Return value        : void
 *  Description         : This service sets the duty cycle for a channel
 *                        if DutyCycle is 0%, set output to inverse of polarity
 *                        if DutyCycle is >= 100%, set output to polarity
 *                        if 0% < DutyCycle < 100%, set output accordingly
 *                        The function can be called on task level.
 *  \endverbatim
 */
FUNC(void, PWM_CODE) Pwm_SetDutyCycle(
    Pwm_ChannelType ChannelNumber, uint16 DutyCycle);
#endif

#if (STD_ON == PWM_SET_PERIOD_AND_DUTY_API)
/**
 *  \brief Service for setting Period and Duty Cycle
 *
 *  \verbatim
 *  Service name        : Pwm_SetPeriodAndDuty
 *  Syntax              : Pwm_SetPeriodAndDuty(Pwm_ChannelType Channel,
 *                                  Pwm_PeriodType Period, uint16 DutyCycle)
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]     : 0x03
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : Pwm_ChannelType Channel
 *  Parameters (in)     : Pwm_PeriodType Period
 *  Parameters (in)     : uint16 DutyCycle
 *  Parameters (out)    : None
 *  Return value        : void
 *  Description         : This service sets period and duty cycle for a channel
 *                        if DutyCycle is 0%, set output to inverse of polarity
 *                        if DutyCycle is >= 100%, set output to polarity
 *                        if 0% < DutyCycle < 100%, set output accordingly
 *                        The function can be called on task level.
 *  \endverbatim
 */
FUNC(void, PWM_CODE) Pwm_SetPeriodAndDuty(
    Pwm_ChannelType ChannelNumber, Pwm_PeriodType Period, uint16 DutyCycle);
#endif

#if (STD_ON == PWM_SET_OUTPUT_TO_IDLE_API)
/**
 *  \brief Service to set the output of a channel immediately to idle.
 *
 *  \verbatim
 *  Service name        : Pwm_SetOutputToIdle
 *  Syntax              : Pwm_SetOutputToIdle(Pwm_ChannelType Channel)
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]     : 0x04
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : Pwm_ChannelType Channel
 *  Parameters (out)    : None
 *  Return value        : void
 *  Description         : This service sets the output of a channel
 *                        immediately to idle. The idle output level
 *                        is configured in driver configuration.
 *                        The function can be called on task level.
 *  \endverbatim
 */
FUNC(void, PWM_CODE) Pwm_SetOutputToIdle(Pwm_ChannelType ChannelNumber);
#endif

 /* Function not implmented due to hardware limitation
    Dummy Function*/
FUNC(Pwm_OutputStateType, PWM_CODE) Pwm_GetOutputState(
    Pwm_ChannelType ChannelNumber);


#if (STD_ON == PWM_NOTIFICATION_SUPPORTED)
/**
 *  \brief Service to disable the Edge Notification
 *
 *  \verbatim
 *  Service name        : Pwm_DisableNotification
 *  Syntax              : Pwm_DisableNotification(Pwm_ChannelType ChannelNumber)
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]     : 0x06
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : Pwm_ChannelType ChannelNumber
 *  Parameters (out)    : None
 *  Return value        : None
 *  Description         : Disables the Edge Notification
 *                        This service deactivates the Edge Notification for the
 *                        pwm channel specified by the ChannelNumber Parameter.
 *                        The function can be called on task level.
 *  \endverbatim
 */
FUNC(void, PWM_CODE) Pwm_DisableNotification(Pwm_ChannelType ChannelNumber);
#endif

#if (STD_ON == PWM_NOTIFICATION_SUPPORTED)
/**
 *  \brief Service to enable the Edge Notification
 *
 *  \verbatim
 *  Service name        : Pwm_EnableNotification
 *  Syntax              : Pwm_EnableNotification (Pwm_ChannelType Channel,
 *                        Pwm_EdgeNotificationType Notification)
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]     : 0x07
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : Pwm_ChannelType ChannelNumber
 *  Parameters (in)     : Pwm_EdgeNotificationType  Notification
 *  Parameters (out)    : None
 *  Return value        : None
 *  Description         : Enables the Edge Notification
 *                        This service enables the notification of the chosen
 *                        edges for the Pwm channel specified by the
 *                        ChannelNumber.The function can be called on task level
 *  \endverbatim
 */
FUNC(void, PWM_CODE) Pwm_EnableNotification(
    Pwm_ChannelType ChannelNumber, Pwm_EdgeNotificationType Notification);
#endif

#if (STD_ON == PWM_VERSION_INFO_API)
/**
 *  \brief This service returns the version information of this module.
 *
 *  \verbatim
 *  Service name        : Pwm_GetVersionInfo
 *  Syntax              : Pwm_GetVersionInfo(P2VAR(Std_VersionInfoType,
 *                           AUTOMATIC, PWM_APPL_DATA) versioninfo)
 *  Mode                : User Mode (Non-Privileged Mode)
 *  Service ID[hex]     : 0x08
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : Std_VersionInfoType - Version info.
 *  Parameters (out)    : None
 *  Return value        : void
 *  Description         : Returns the version information of this module.
 *                        The function can be called on task level.
 *  \endverbatim
 */

FUNC(void, PWM_CODE) Pwm_GetVersionInfo(
    P2VAR(Std_VersionInfoType, AUTOMATIC, PWM_APPL_DATA) versioninfo);
#endif

#if(STD_ON == PWM_REGISTER_READBACK_API)
/**
 *  \brief This function reads the important registers of the hardware unit
 *         and returns the value in the structure.
 *
 *
 *  \verbatim
 *  Service name        : Pwm_RegisterReadback(Pwm_ChannelType ChannelNumber,
 *                                          Pwm_RegisterReadbackType *RegRbPtr)
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Service ID[hex]     : 0x0D
 *  Parameters (in)     : Pwm_ChannelType ChannelNumber
 *  Parameters (inout)  : RegRbPtr - Pointer to where to store the readback
 *                        values. If this pointer is NULL, then the API
 *                        will return E_NOT_OK.
 *  Description         : This API is used to check the status of critical
                          registers which donot change during program execution.
 *  Return value        : Std_ReturnType
 *                        E_OK: Register read back has been done successfully
 *                        E_NOT_OK: Register read back failed
 *  \endverbatim
 */
Std_ReturnType Pwm_RegisterReadback(
    Pwm_ChannelType ChannelNumber, Pwm_RegisterReadbackType *RegRbPtr);
#endif

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef PWM_H_ */

/* @} */
