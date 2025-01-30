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
 *  \file     Gpt.h
 *
 *  \brief    This file contains interface header for GPT MCAL driver
 *
 */

/**
 *  \defgroup MCAL_GPT_API GPT Driver API
 *
 *  The GPT Driver MCAL provides low level access to GPT peripheral
 *
 *  present on AM62xx and TDA4XX
 *  \sa MCAL_GPT_CFG
 *  \sa MCAL_GPT_IRQ
 *  \sa MCAL_GPT_DBG_API
 *  @{
 */

#ifndef GPT_H_
#define GPT_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "Std_Types.h"
#include "Gpt_Cfg.h"
#if (GPT_WAKEUP_FUNCTIONALITY_API == STD_ON)
    /* included for Wakeup handling */
    #include "EcuM_Cbk.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
/*
 * Design : MCAL-6324
 */
/**
 *  \name GPT Driver Module SW Version Info
 *
 *  Defines for GPT Driver version used for compatibility checks
 *  @{
 */
/** \brief Driver Implementation Major Version */
#define GPT_SW_MAJOR_VERSION            (9U)
/** \brief Driver Implementation Minor Version */
#define GPT_SW_MINOR_VERSION            (0U)
/** \brief Driver Implementation Patch Version */
#define GPT_SW_PATCH_VERSION            (1U)
/* @} */

/**
 *  \name GPT Driver Module AUTOSAR Version Info
 *
 *  Defines for GPT Driver AUTOSAR version used for compatibility checks
 *  @{
 */
/** \brief AUTOSAR Major version specification implemented by GPT Driver */
#define GPT_AR_RELEASE_MAJOR_VERSION    (4U)
/** \brief AUTOSAR Minor  version specification implemented by GPT Driver */
#define GPT_AR_RELEASE_MINOR_VERSION    (3U)
/** \brief AUTOSAR Patch version specification implemented by GPT Driver */
#define GPT_AR_RELEASE_REVISION_VERSION (1U)
/* @} */

/**
 *  \name GPT Driver ID Info
 *  @{
 */
/** \brief Texas Instruments Vendor ID */
#define GPT_VENDOR_ID                   ((uint16) 44U)
/** \brief GPT Driver Module ID */
#define GPT_MODULE_ID                   ((uint16) 100U)
/** \brief GPT Driver Instance ID */
#define GPT_INSTANCE_ID                 ((uint8) 0U)
/* @} */

/*
 * GPT channel notification/wakeup function index.
 * This is used store function index into ISR array.
 */
/** \brief GPT continuous mode index */
#define GPT_CH_ISR_MODE_CONTINUOUS     (0U)
/** \brief GPT oneshot mode index */
#define GPT_CH_ISR_MODE_ONESHOT        (1U)
/** \brief GPT continuous wakeup mode index */
#define GPT_CH_ISR_MODE_CONT_WAKEUP    (2U)
/** \brief GPT oneshot wakeup mode index */
#define GPT_CH_ISR_MODE_ONESHOT_WAKEUP (3U)

/**
 *  \name GPT Error Codes
 *
 *  Error codes returned by GPT functions
 *  @{
 */

/*
 * Design : MCAL-6255,MCAL-6197,MCAL-6193
 */

#ifndef GPT_E_UNINIT
/** \brief API service used without module initialization */
#define GPT_E_UNINIT                (0x0AU)
#endif

/*
 * Design : MCAL-6318
 */
#ifndef GPT_E_BUSY
/**
 *  \brief API service called during ongoing process (Timer is already
 *  running)
 */
#define GPT_E_BUSY                  (0x0BU)
#endif

/*
 * Design : MCAL-2514
 */
#ifndef GPT_E_MODE
/** \brief API service called when driver is in wrong mode.
            Used primarily when PreDef Timers are used. Since PreDef Timer
            are not supported in this implementation, this error code is not
            used */
#define GPT_E_MODE                  (0x0CU)
#endif

/*
 * Design : MCAL-6255
 */
#ifndef GPT_E_ALREADY_INITIALIZED
/** \brief API Gpt_Init is called but the module is already initialized. */
#define GPT_E_ALREADY_INITIALIZED   (0x0DU)
#endif

/*
 * Design : MCAL-6243
 */
#ifndef GPT_E_INIT_FAILED
/** \brief API Gpt_Init is called but the module is already initialized. */
#define GPT_E_INIT_FAILED           (0x0EU)
#endif

/*
 * Design : MCAL-6232
 */
#ifndef GPT_E_PARAM_CHANNEL
/** \brief API called with invalid channel. */
#define GPT_E_PARAM_CHANNEL         (0x14U)
#endif

/*
 * Design : MCAL-6285
 */
#ifndef GPT_E_PARAM_VALUE
/** \brief API service called with invalid start timer parameter */
#define GPT_E_PARAM_VALUE           (0x15U)
#endif

/*
 * Design : MCAL-6313
 */
#ifndef GPT_E_PARAM_POINTER
/** \brief API service called with invalid(Null_Ptr) data buffer pointer. */
#define GPT_E_PARAM_POINTER         (0x16U)
#endif
#ifndef GPT_E_PARAM_PREDEF_TIMER
/** \brief API service called with invalid Predef Timer. Not supported in this
    implementation*/
#define GPT_E_PARAM_PREDEF_TIMER    (0x17U)
#endif

/*
 * Design : MCAL-6258
 */
#ifndef GPT_E_PARAM_MODE
/** \brief Gpt_SetMode is called with wrong mode parameter. */
#define GPT_E_PARAM_MODE            (0x1FU)
#endif
#ifndef GPT_E_INVALID_ISR
/** \brief Invalid entry to ISR. */
#define GPT_E_INVALID_ISR           (0x80U)
#endif
/* @} */

/**
 *  \name GPT Service Ids
 *
 *  The Service Id is one of the argument to Det_ReportError function and
 *  is used to identify the source of the error
 *  @{
 */
/** \brief Gpt_GetVersionInfo() API Service ID */
#define GPT_SID_GET_VERSION_INFO    (0x00U)
/** \brief Gpt_Init() API Service ID */
#define GPT_SID_INIT                (0x01U)
/** \brief Gpt_DeInit() API Service ID */
#define GPT_SID_DEINIT              (0x02U)
/** \brief Gpt_GetTimeElapsed() API Service ID */
#define GPT_SID_GET_TIME_ELAPSED    (0x03U)
/** \brief Gpt_GetTimeRemaining() API Service ID */
#define GPT_SID_GET_TIME_REMAINING  (0x04U)
/** \brief Gpt_StartTimer() API Service ID */
#define GPT_SID_START_TIMER         (0x05U)
/** \brief Gpt_StopTimer() API Service ID */
#define GPT_SID_STOP_TIMER          (0x06U)
/** \brief Gpt_EnableNotification() API Service ID */
#define GPT_SID_ENABLE_NOTIFY       (0x07U)
/** \brief Gpt_DisableNotification() API Service ID */
#define GPT_SID_DISABLE_NOTIFY      (0x08U)
/** \brief Gpt_SetMode() API Service ID */
#define GPT_SID_SET_MODE            (0x09U)
/** \brief Gpt_DisableWakeup() API Service ID */
#define GPT_SID_DISABLE_WAKEUP      (0x0AU)
/** \brief Gpt_EnableWakeup() API Service ID */
#define GPT_SID_ENABLE_WAKEUP       (0x0BU)
/** \brief Gpt_CheckWakeup() API Service ID */
#define GPT_SID_CHECK_WAKEUP        (0x0CU)
/** \brief Gpt_GetPredefTimerValue() API Service ID */
#define GPT_SID_GET_PREDEFTIMERVALUE    (0x0DU)
/** \brief GPT Get Hw Unit Obj API Service ID */
#define GPT_SID_GET_GETHWUNITOBJ        (0x0EU)
/** \brief GPT Critical register read back API Service ID */
#define GPT_SID_REGISTER_READBACK       (0x0FU)
/* @} */

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/**
 *  \addtogroup MCAL_GPT_CFG GPT Configuration
 *  @{
 */


/*
 * Design : MCAL-6368
 */
/** \brief Type describing the Gpt channel */
typedef uint32 Gpt_ChannelType;


/*
 * Design : MCAL-6282
 */
/** \brief  Type describing the timeout value  */
typedef uint32 Gpt_ValueType;


/*
 * Design : MCAL-6349
 */
/** \brief  List of process modes  */
typedef enum
{
    GPT_MODE_NORMAL = 0U,
    /**< Normal operation mode of the GPT */
    GPT_MODE_SLEEP = 1U,
    /**< Operation for reduced power operation mode. In sleep mode only wakeup
     *   capable channels are available.*/
    GPT_MODE_INVALID = 0xFFU
    /**< Mode end marker */
} Gpt_ModeType;


/** \brief Type for GPT Predef Timers */
typedef enum
{
   GPT_PREDEF_TIMER_1US_16BIT = 0U,
   /**< GPT Predef Timer with tick duration 1µs and range 16bit*/
   GPT_PREDEF_TIMER_1US_24BIT = 1U,
   /**< GPT Predef Timer with tick duration 1µs and range 24bit*/
   GPT_PREDEF_TIMER_1US_32BIT = 2U,
   /**< GPT Predef Timer with tick duration 1µs and range 32bit*/
   GPT_PREDEF_TIMER_100US_32BIT = 3U
   /**< GPT Predef Timer with tick duration 100µs and range 32bit*/   
} Gpt_PredefTimerType;


/**
 *  \brief  List of Prescale values for General purpose timer channels
 *          The timer counter is prescaled with the value 2^(PTV+1).
 *          Example: PTV = 3, counter increases value (if started) after 16
 *          functional clock periods
 *  Note: Not applicable to GPT Predef timers.
 */
typedef enum
{
    GPT_PRESCALER_CLK_DIV_BY_2 = 0x0U,
    /**< Value used to divide timer clock by 2 */
    GPT_PRESCALER_CLK_DIV_BY_4 = 0x1U,
    /**< Value used to divide timer clock by 4 */
    GPT_PRESCALER_CLK_DIV_BY_8 = 0x2U,
    /**< Value used to divide timer clock by 8 */
    GPT_PRESCALER_CLK_DIV_BY_16 = 0x3U,
    /**< Value used to divide timer clock by 16 */
    GPT_PRESCALER_CLK_DIV_BY_32 = 0x4U,
    /**< Value used to divide timer clock by 32 */
    GPT_PRESCALER_CLK_DIV_BY_64 = 0x5U,
    /**< Value used to divide timer clock by 64 */
    GPT_PRESCALER_CLK_DIV_BY_128 = 0x6U,
    /**< Value used to divide timer clock by 128 */
    GPT_PRESCALER_CLK_DIV_BY_256 = 0x7U,
    /**< Value used to divide timer clock by 256 */
    GPT_PRESCALER_NO_PRESCALE = 0xFFU,
    /**< Prescale disabled */
} Gpt_PrescaleValueType;


/*
 * Design : MCAL-6221
 */
/** \brief  Notification callback function pointer  */
typedef void (*Gpt_NotifyType)(void);

/** \brief  GPT channel mode macros.  */
typedef enum
{
    GPT_CH_MODE_CONTINUOUS = 0U,
    /**< GPT initialized */
    GPT_CH_MODE_ONESHOT
    /**< Normal operation mode of the GPT */
} Gpt_ChannelMode;

/**
 * \brief  GPT channel state
 *         GPT will be in one of this state during its lifetime.
 */
typedef enum
{
    GPT_UNINITIALIZED = 0U,
    /**< GPT is uninitialized */
    GPT_INITIALIZED,
    /**< GPT is initialized */
    GPT_RUNNING,
    /**< GPT is running */
    GPT_STOPPED,
    /**< GPT is stopped */
    GPT_EXPIRED
    /**< GPT is expired */
} Gpt_ChannelStateType;


/*
 * Design : MCAL-6362,MCAL-6332,MCAL-6334
 */
/** \brief  Configuration per channel */
typedef struct
{
    Gpt_ChannelMode         channelMode;
    /**< Channel mode */
    uint32                  tickValueMax;
    /**< Maximum value in ticks, the timer channel is able to count. With the
     *   next tick, the timer rolls over to zero
     */
    uint8                   enableWakeupFlag;
    /**< Enables wakeup capability of MCU for a channel */
    Gpt_NotifyType          fnPtrNotifyFunction;
    /**< Function pointer to callback function (for non-wakeup notification) */
#if (STD_ON == GPT_REPORT_WAKEUP_SOURCE) && (STD_ON == \
                                                 GPT_WAKEUP_FUNCTIONALITY_API)
    EcuM_WakeupSourceType   wakeupSourceRef;
    /**< Channel wakeup source, in case the wakeup-capability is true this value
     *   is transmitted to the EcuState Manager */
#endif
    uint32                  prescale;
    /**< The input GPT clock will be divided by this value */
} Gpt_ChannelConfigType;

/**
 *  \brief SPI sequence config structure parameters Pre-Compile only
 */
typedef struct Gpt_ChannelConfigType_PC_s
{
    Gpt_ChannelType         channelId;
    /**< Channel Id */
} Gpt_ChannelConfigType_PC;


/*
 * Design : MCAL-6344
 */
/** \brief  global configuration of the driver  */
typedef struct Gpt_ConfigType_s
{
    P2CONST(Gpt_ChannelConfigType, AUTOMATIC, GPT_CONST) ChannelCfgPtr;
    /**<  Pointer to Channel list */
    uint32 channelCount;
    /**<  Number of channels configured  */
} Gpt_ConfigType;

#if (STD_ON == GPT_REGISTER_READBACK_API)
/**
 *  \brief GPT register readback structure
 */
typedef struct
{
    /*
     * GPT related registers
     */
    uint32 gptRev;
    /**< IP revision identifier */
    uint32 gptTtgr;
    /**< Reg read always 0xFFFFFFFF */
    uint32 gptTimerSynCtrl;
    /**< Timer synchronous interface control register */
    uint32 gptTiocpCfg;
    /**< CBASS0 Configuration register */
    uint32 gptTclr;
    /**< Timer control register */
} Gpt_RegisterReadbackType;

#endif  /* #if (STD_ON == GPT_REGISTER_READBACK_API) */

/* @} */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

#if (STD_ON == GPT_VERSION_INFO_API)
/**
 *  \brief This service returns the version information of this module.
 *
 *  \verbatim
 *  Service name        : Gpt_GetVersionInfo
 *  Syntax              : void Gpt_GetVersionInfo(Std_VersionInfoType*
 *                                                          VersionInfoPtr)
 *  Mode                : User Mode (Non-Privileged Mode)
 *  Service ID[hex]     : 0x00
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : None
 *  Parameters (inout)  : None
 *  Parameters (out)    : VersionInfoPtr - Pointer to where to store the version
 *                        information of this module
 *  Return value        : None
 *  Description         : Returns the version information of this module
 *  \endverbatim
 */
FUNC(void,
     GPT_CODE) Gpt_GetVersionInfo(P2VAR(Std_VersionInfoType, AUTOMATIC,
                                        GPT_APPL_DATA) VersionInfoPtr);
#endif /*(STD_ON == GPT_VERSION_INFO_API)*/

/**
 *  \brief This service initializes all the configured Gpt channels. This will
 *         set the state of the each channel to"initialized", also set the Gpt
 *         driver state to "Normal".
 *
 *  \verbatim
 *  Service name        : Gpt_Init
 *  Syntax              : void Gpt_Init(const Gpt_ConfigType* CfgPtr)
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]     : 0x01
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Non Reentrant
 *  Parameters (in)     : Gpt_ConfigType* CfgPtr - configuration structure
 *                        for initializing the module.
 *  Parameters (inout)  : None
 *  Parameters (out)    : None
 *  Return value        : None
 *  Description         : Initializes the GPT driver.
 *  \endverbatim
 */
FUNC(void, GPT_CODE) Gpt_Init(P2CONST(Gpt_ConfigType, AUTOMATIC,
                                      GPT_CONST) CfgPtr);

#if (STD_ON == GPT_DEINIT_API)
/**
 *  \brief This service deinitializes the RTI used by Gpt driver to the power on
 *         reset state.
 *         The Gpt driver state is changed to "Uninitialized" state".
 *         All the channel registers are cleared to stop the timer channels.
 *         API will disable all interrupt notifications, wakeup interrupts.
 *
 *  \verbatim
 *  Service name        : Gpt_DeInit
 *  Syntax              : void Gpt_DeInit(void)
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]     : 0x02
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Non Reentrant
 *  Parameters (in)     : None
 *  Parameters (inout)  : None
 *  Parameters (out)    : None
 *  Return value        : None
 *  Description         : Deinitializes the GPT driver.
 *  \endverbatim
 */
FUNC(void, GPT_CODE) Gpt_DeInit(void);
#endif /*(STD_ON == GPT_DEINIT_API)*/

#if (STD_ON == GPT_TIME_ELAPSED_API)
/**
 *  \brief Gpt_GetTimeElapsed will return the time elapsed for channel which is
 *         referenced. The user can configure the channel in two modes, One-shot
 *         and Continuous mode. In one shot mode, if the timer is in stopped
 *         state, the function will return time value at the moment of stopping.
 *         If the timer is expired, the function will return the target time
 *         configured for the channel. In Continuous Mode - The elapsed time
 *         value will be the value relative to last occurrence.
 *
 *  \verbatim
 *  Service name        : Gpt_GetTimeElapsed
 *  Syntax              : Gpt_ValueType Gpt_GetTimeElapsed(Gpt_ChannelType
 *                                                                      Channel)
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]     : 0x03
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : Channel - ChannelId of the Gpt channel.
 *  Parameters (inout)  : None
 *  Parameters (out)    : None
 *  Return value        : Gpt_ValueType Elapsed timer value (in number of ticks)
 *  Description         : Returns the time already elapsed.
 *  \endverbatim
 */
FUNC(Gpt_ValueType, GPT_CODE) Gpt_GetTimeElapsed(Gpt_ChannelType Channel);
#endif /*(STD_ON == GPT_TIME_ELAPSED_API)*/

#if (STD_ON == GPT_TIME_REMAINING_API)
/**
 *  \brief Gpt_GetTimeRemaining will return the timer value remaining until the
 *         target time will be reached next time. The remaining time is target
 *         time minus time already elapsed. In one shot mode, if the timer is in
 *         stopped state, the function will return remaining time value at the
 *         moment of stopping. If the timer is expired, the function will
 *         return 0.
 *
 *  \verbatim
 *  Service name        : Gpt_GetTimeRemaining
 *  Syntax              : Gpt_ValueType Gpt_GetTimeRemaining(Gpt_ChannelType
 *                                                                      Channel)
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]     : 0x04
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : Channel - ChannelId of the Gpt channel.
 *  Parameters (inout)  : None
 *  Parameters (out)    : None
 *  Return value        : Gpt_ValueType Remaining timer value (in number o
 *                        ticks)
 *  Description         : Returns the time remaining until the target time is
 *                        reached.
 *  \endverbatim
 */
FUNC(Gpt_ValueType, GPT_CODE) Gpt_GetTimeRemaining(Gpt_ChannelType Channel);
#endif /*(STD_ON == GPT_CFG_TIME_REMAINING_API)*/

/**
 *  \brief Gpt_StartTimer will start the selected timer channel with defined
 *         target time. If the timer channel is enabled for interrupt
 *         notification, then interrupt notification will be triggered after
 *         expiration of the selected timer channel. In one shot mode, if the
 *         timer is expired then the channel will be stopped in interrupt
 *         subroutine. The selected channel will be moved to "Running" state
 *         after calling this function.
 *
 *  \verbatim
 *  Service name        : Gpt_StartTimer
 *  Syntax              : void Gpt_StartTimer(Gpt_ChannelType Channel,
 *                                            Gpt_ValueType Value)
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]     : 0x05
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : Channel - ChannelId of the Gpt channel.
 *                        value - Target time in number of ticks.
 *  Parameters (inout)  : None
 *  Parameters (out)    : None
 *  Return value        : None
 *  Description         : Starts a timer channel.
 *  \endverbatim
 */
FUNC(void, GPT_CODE) Gpt_StartTimer(Gpt_ChannelType Channel,
                                    Gpt_ValueType Value);

/**
 *  \brief Gpt_StopTimer will stop the selected timer channel.This will clear
 *         all the registers corresponding to the selected channel. The state of
 *         the timer channel will be changed to "Stopped".If the channel is in
 *         state "Initialized","Expired","Stopped" before calling this function,
 *         the function will be left without any action.
 *
 *  \verbatim
 *  Service name        : Gpt_StopTimer
 *  Syntax              : void Gpt_StopTimer( Gpt_ChannelType Channel )
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]     : 0x06
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant (but not for the same timer channel)
 *  Parameters (in)     : Channel: Numeric identifier of the GPT channel.
 *  Parameters (inout)  : None
 *  Parameters (out)    : None
 *  Return value        : None
 *  Description         : Stops a timer channel.
 *  \endverbatim
 */
FUNC(void, GPT_CODE) Gpt_StopTimer(Gpt_ChannelType Channel);

#if (STD_ON == GPT_ENABLE_DISABLE_NOTIFICATION_API)
/**
 *  \brief Gpt_EnableNotification will enable the interrupt notification for the
 *         selected channel. The SETINT bit in RTI Set interrupt register will
 *         be Set to enable the Compare interrupt. The interrupt is triggered
 *         when Free running counter value matches with compare register value.
 *
 *  \verbatim
 *  Service name        : Gpt_EnableNotification
 *  Syntax              : void Gpt_EnableNotification( Gpt_ChannelType Channel)
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]     : 0x07
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant (but not for the same timer channel)
 *  Parameters (in)     : Channel: Numeric identifier of the GPT channel.
 *  Parameters (inout)  : None
 *  Parameters (out)    : None
 *  Return value        : None
 *  Description         : Enables the interrupt notification for a channel
 *                        relevant in normal mode).
 *  \endverbatim
 */
FUNC(void, GPT_CODE) Gpt_EnableNotification(Gpt_ChannelType Channel);
#endif /*(STD_ON == GPT_ENABLE_DISABLE_NOTIFICATION_API)*/

#if (STD_ON == GPT_ENABLE_DISABLE_NOTIFICATION_API)
/**
 *  \brief Gpt_DisableNotification will disable the compare interrupt
 *    notification for the selected channel. The SETINT bit in RTI Set interrupt
 *    register will be cleared to disable the Compare interrupt. The
 *    interrupt is triggered when Free running counter value matches with
 *    compare register value.
 *
 *  \verbatim
 *  Service name        : Gpt_DisableNotification
 *  Syntax              : void Gpt_DisableNotification( Gpt_ChannelType Channel)
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]     : 0x08
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant (but not for the same timer channel)
 *  Parameters (in)     : Channel: Numeric identifier of the GPT channel.
 *  Parameters (inout)  : None
 *  Parameters (out)    : None
 *  Return value        : None
 *  Description         : Disables the interrupt notification for a channel
 *                        relevant in normal mode).
 *  \endverbatim
 */
FUNC(void, GPT_CODE) Gpt_DisableNotification(Gpt_ChannelType Channel);
#endif /*(STD_ON == GPT_ENABLE_DISABLE_NOTIFICATION_API)*/

#if (STD_ON == GPT_REPORT_WAKEUP_SOURCE) && (STD_ON == \
                                                 GPT_WAKEUP_FUNCTIONALITY_API)
/**
 *  \brief Gpt_SetMode will set the operation mode of the Gpt driver to the
 *   given set mode parameter. If the parameter mode = Normal, then the function
 *   will enable the interrupt notifications for all the channels which are
 *   configured for notification and notification is enabled. If the
 *   Mode = Sleep, the function will enable the wakeup interrupts for all
 *   channels which are configured for wakeup and disable all other channels.
 *   All the timer channels which are running will be stopped in this mode.
 *
 *  \verbatim
 *  Service name        : Gpt_SetMode
 *  Syntax              : void Gpt_SetMode( Gpt_ModeType Mode )
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]     : 0x09
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Non Reentrant
 *  Parameters (in)     : Mode GPT_MODE_NORMAL: Normal operation mode of the GPT
 * driver.
 *                             GPT_MODE_SLEEP: Sleep mode of the GPT driver
 * (wakeup capable).
 *  Parameters (inout)  : None
 *  Parameters (out)    : None
 *  Return value        : None
 *  Description         : Sets the operation mode of the GPT.
 *  \endverbatim
 */
FUNC(void, GPT_CODE) Gpt_SetMode(Gpt_ModeType Mode);
#endif /*(STD_ON == GPT_WAKEUP_FUNCTIONALITY_API)*/

/*
 * Design : MCAL-6242
 */

#if (STD_ON == GPT_REPORT_WAKEUP_SOURCE) && (STD_ON == \
                                                 GPT_WAKEUP_FUNCTIONALITY_API)
/**
 *  \brief Gpt_DisableWakeup will disable the wakeup interrupt of the referenced
 *   channel. The function will save the attribute "wakeup disable" of the
 *   channel.
 *
 *  \verbatim
 *  Service name        : Gpt_DisableWakeup
 *  Syntax              : void Gpt_DisableWakeup( Gpt_ChannelType Channel )
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]     : 0x0A
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant (but not for the same timer channel)
 *  Parameters (in)     : Channel - Numeric identifier of the GPT channel.
 *  Parameters (inout)  : None
 *  Parameters (out)    : None
 *  Return value        : None
 *  Description         : Disables the wakeup interrupt of a channel (relevant
 *                        in sleep mode).
 *  \endverbatim
 */
FUNC(void, GPT_CODE) Gpt_DisableWakeup(Gpt_ChannelType Channel);
#endif /*(STD_ON == GPT_WAKEUP_FUNCTIONALITY_API)*/

/*
 * Design : MCAL-6242
 */

#if (STD_ON == GPT_REPORT_WAKEUP_SOURCE) && (STD_ON == \
                                                 GPT_WAKEUP_FUNCTIONALITY_API)
/**
 *  \brief Gpt_EnableWakeup will enable the wakeup interrupt of the referenced
 *         channel. The function will save the attribute "wakeup enable" of the
 *         channel which only affects the wakeup interrupt when the driver is in
 *          Sleep mode.
 *
 *  \verbatim
 *  Service name        : Gpt_EnableWakeup
 *  Syntax              : void Gpt_EnableWakeup( Gpt_ChannelType Channel )
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]     : 0x0B
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant (but not for the same timer channel)
 *  Parameters (in)     : Channel - Numeric identifier of the GPT channel.
 *  Parameters (inout)  : None
 *  Parameters (out)    : None
 *  Return value        : None
 *  Description         : Enables the wakeup interrupt of a channel (relevant in
 *                        sleep mode).
 *  \endverbatim
 */
FUNC(void, GPT_CODE) Gpt_EnableWakeup(Gpt_ChannelType Channel);
#endif /*(STD_ON == GPT_WAKEUP_FUNCTIONALITY_API)*/

/*
 * Design : MCAL-6242
 */

#if (STD_ON == GPT_REPORT_WAKEUP_SOURCE) && (STD_ON == \
                                                 GPT_WAKEUP_FUNCTIONALITY_API)
/**
 *  \brief Gpt_CheckWakeup will check if wakeup capable timer channel is source
 *         for a wakeup event and call EcuM_SetWakeupEvent to indicate the valid
 *         timer wakeup event to the EcuM.
 *
 *  \verbatim
 *  Service name        : Gpt_CheckWakeup
 *  Syntax              : void Gpt_CheckWakeup
 *                                  ( EcuM_WakeupSourceType wakeupSource )
 *  Mode                : User Mode (Non-Privileged Mode)
 *  Service ID[hex]     : 0x0C
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant.
 *  Parameters (in)     : WakeupSource -  Information on wakeup source to be
 *                        checked. The associated GPT channel can be determined
 *                        from configuration data
 *  Parameters (inout)  : None
 *  Parameters (out)    : None
 *  Return value        : None
 *  Description         : Checks if a wakeup capable GPT channel is the source
 *                        for a wakeup event and calls the ECU state manager
 *                        service EcuM_SetWakeupEvent in case of a valid GPT
 *                        channel wakeup event.
 *  \endverbatim
 */
FUNC(void, GPT_CODE) Gpt_CheckWakeup(EcuM_WakeupSourceType WakeupSource);
#endif /*(STD_ON == GPT_WAKEUP_FUNCTIONALITY_API)*/

#if (STD_ON == GPT_REGISTER_READBACK_API)
/**
 *  \brief This function reads the important registers of the hardware unit
 *  and returns the value in the structure.
 *
 *  This API should be called after Gpt_Init is called. Otherwise this API will
 *  return E_NOT_OK.
 *
 *  This API could be used to readback the register contents after Gpt_Init
 *  and then the readback value could be compared during GPT execution to
 *  check the correctness of the HW unit. Since this API is used for this
 *  purpose, the register returned are the ones which doesn't change after init
 *  based on conv or channel config.
 *
 *  \verbatim
 *  Service name        : Gpt_RegisterReadback
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : GptChannel - Numeric identifier of the GPT channel.
 *                        If this is invalid, then the API will
 *                        return E_NOT_OK.
 *  Parameters (inout)  : RegRbPtr - Pointer to where to store the readback
 *                        values. If this pointer is NULL_PTR, then the API
 *                        will return E_NOT_OK.
 *  Return value        : Std_ReturnType
 *                        E_OK: Register read back has been done
 *                        E_NOT_OK: Register read back failed
 *  \endverbatim
 */
FUNC(Std_ReturnType, GPT_CODE) Gpt_RegisterReadback(
    Gpt_ChannelType GptChannel,
    P2VAR(Gpt_RegisterReadbackType, AUTOMATIC, GPT_APPL_DATA) RegRbPtr);
#endif  /* #if (STD_ON == GPT_REGISTER_READBACK_API) */

#if (GPT_PREDEF_TIMER_DISABLED == GPT_PREDEF_TIMER_TYPE)
/* Function not implemented due to hardware limitation
   Dummy Function*/
FUNC(Std_ReturnType, GPT_CODE) Gpt_GetPredefTimerValue (
     Gpt_PredefTimerType PredefTimer,uint32* TimeValuePtr);
#endif

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef GPT_H_ */

/* @} */
