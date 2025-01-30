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
 *  \file     Icu.h
 *
 *  \brief    This file contains interface header for ICU MCAL driver
 *              to be used by other AUTOSAR MCAL Layers
 *
 */

/**
*  \defgroup MCAL_ICU_API ICU Driver API
*
*    This component provides services for initialization and control of
*    the ICU unit.
*    The ICU module captures and measures signals.
*    signal period time.
*
*  \sa MCAL_ICU_CFG
*  \sa MCAL_ICU_IRQ
*  @{
*/

 /*
 * Below are the global requirements which are met by this PWM
 * driver which can't be mapped to a particular source ID
 */
/*
*Design: MCAL-7113,MCAL-7128,MCAL-7064,MCAL-7118,MCAL-6917,MCAL-6972,MCAL-7091,MCAL-6931,MCAL-7048,MCAL-6960
*/

/*
 * Below are the PWM module's environment requirements which can't be mapped
 * to this driver
 */
/*
*Design: MCAL-7014,MCAL-7101,MCAL-6968,MCAL-7134,MCAL-7082,MCAL-7142,MCAL-7053,MCAL-7004,MCAL-7015,MCAL-7007,MCAL-6933,MCAL-7046,MCAL-6982,MCAL-7001,MCAL-7146,MCAL-6896,MCAL-7055,MCAL-7109,MCAL-7061,MCAL-6993,MCAL-7108,MCAL-7119,MCAL-6997,MCAL-7051,MCAL-6955,MCAL-7120,MCAL-6952,MCAL-7079
*/

#ifndef ICU_H_
#define ICU_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
/*
* Design: MCAL-7045,MCAL-7128,MCAL-7064,MCAL-7118,MCAL-6917,MCAL-6972,MCAL-7091,MCAL-6931,MCAL-7048,MCAL-6960
*/
#include "Std_Types.h"
#include "Icu_Cfg.h"
#include "EcuM_Types.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 *  \name ICU Driver ID Info
 *  @{
 */
/** \brief Texas Instruments Vendor ID */
#define ICU_VENDOR_ID                   ((uint16) 44U)
/** \brief ICU Driver Module ID */
#define ICU_MODULE_ID                   ((uint16) 122U)
/** \brief ICU Driver Instance ID */
#define ICU_INSTANCE_ID                 ((uint8) 0U)
/* @} */

/**
 *  \name ICU Driver Module SW Version Info
 *
 *  Defines for ICU Driver version used for compatibility checks
 *  @{
 */
/** \brief Driver Implementation Major Version */
#define ICU_SW_MAJOR_VERSION            (9U)
/** \brief Driver Implementation Minor Version */
#define ICU_SW_MINOR_VERSION            (0U)
/** \brief Driver Implementation Patch Version */
#define ICU_SW_PATCH_VERSION            (1U)
/* @} */

/**
 *  \name ICU Driver Module AUTOSAR Version Info
 *
 *  Defines for ICU Driver AUTOSAR version used for compatibility checks
 *  @{
 */
/** \brief AUTOSAR Major version specification implemented by ICU Driver */
#define ICU_AR_RELEASE_MAJOR_VERSION    (4U)
/** \brief AUTOSAR Minor  version specification implemented by ICU Driver */
#define ICU_AR_RELEASE_MINOR_VERSION    (3U)
/** \brief AUTOSAR Patch version specification implemented by ICU Driver */
#define ICU_AR_RELEASE_REVISION_VERSION (1U)
/* @} */

/**
 *  \name ICU Service IDs
 *
 *  The Service Id is one of the argument to Det_ReportError function and
 *  is used to identify the source of the error
 *  @{
 */
/** \brief Icu_Init() API Service ID */
#define ICU_INIT_ID                    ((uint8) 0x00U)
/** \brief Icu_DeInit() API Service ID */
#define ICU_DEINIT_ID                  ((uint8) 0x01U)
/** \brief Icu_SetMode() API Service ID */
#define ICU_SETMODE_ID                 ((uint8) 0x02U)
/** \brief Icu_SetActivationCondition() API Service ID */
#define ICU_SETACTIVATIONCONDITION_ID  ((uint8) 0x05U)
/** \brief Icu_DisableNotification() API Service ID */
#define ICU_DISABLENOTIFICATION_ID     ((uint8) 0x06U)
/** \brief Icu_EnableNotification() API Service ID */
#define ICU_ENABLENOTIFICATION_ID      ((uint8) 0x07U)
/** \brief Icu_GetInputState() API Service ID */
#define ICU_GETINPUTSTATE_ID           ((uint8) 0x08U)
/** \brief Icu_StartTimestamp() API Service ID */
#define ICU_STARTTIMESTAMP_ID          ((uint8) 0x09U)
/** \brief Icu_StopTimestamp() API Service ID */
#define ICU_STOPTIMESTAMP_ID           ((uint8) 0x0AU)
/** \brief Icu_GetTimeStampIndex() API Service ID */
#define ICU_GETTIMESTAMPINDEX_ID       ((uint8) 0x0BU)
/** \brief Icu_ResetEdgeCount() API Service ID */
#define ICU_RESETEDGECOUNT_ID          ((uint8) 0x0CU)
/** \brief Icu_EnableEdgeCount() API Service ID */
#define ICU_ENABLEEDGECOUNT_ID         ((uint8) 0x0DU)
/** \brief Icu_EnableEdgeDetection() API Service ID */
#define ICU_ENABLEEDGEDETECTION_ID     ((uint8) 0x16U)
/** \brief Icu_DisableEdgeDetection() API Service ID */
#define ICU_DISABLEEDGEDETECTION_ID    ((uint8) 0x17U)
/** \brief Icu_DisableEdgeCount() API Service ID */
#define ICU_DISABLEEDGECOUNT_ID        ((uint8) 0x0EU)
/** \brief Icu_GetEdgeNumbers() API Service ID */
#define ICU_GETEDGENUMBERS_ID          ((uint8) 0x0FU)
/** \brief Icu_StartSignalMeasurement() API Service ID */
#define ICU_STARTSIGNALMEASUREMENT_ID  ((uint8) 0x13U)
/** \brief Icu_StopSignalMeasurement() API Service ID */
#define ICU_STOPSIGNALMEASUREMENT_ID   ((uint8) 0x14U)
/** \brief Icu_GetTimeElapsed() API Service ID */
#define ICU_GETTIMEELAPSED_ID          ((uint8) 0x10U)
/** \brief Icu_GetDutyCycleValues() API Service ID */
#define ICU_GETDUTYCYCLEVALUES_ID      ((uint8) 0x11U)
/** \brief Icu_GetVersionInfo() API Service ID */
#define ICU_GETVERSIONINFO_ID          ((uint8) 0x12U)
/** \brief Icu_Regesterreadback() API Serviece ID */
#define ICU_SID_REGISTER_READBACK      ((uint8) 0x18U)
/* @} */

/**
 *  \name ICU Error Codes
 *
 *  Error codes returned by ICU functions
 *  @{
 */
/*
*Design: MCAL-7045
*/
/** \brief API service called with invalid pointer */
#define ICU_E_PARAM_POINTER             ((uint8) 0x0AU)
/**
 *  \brief API service called with invalid channel identifier
 *  or channel not configured
 */
#define ICU_E_PARAM_CHANNEL             ((uint8) 0x0BU)
/** \brief API service calleed with an invalid or not feasible activation */
#define ICU_E_PARAM_ACTIVATION          ((uint8) 0x0CU)
/** \brief Init Function Failed */
#define ICU_E_INIT_FAILED               ((uint8) 0x0DU)
/** \brief API service used with invalid Buffer size */
#define ICU_E_PARAM_BUFFER_SIZE         ((uint8) 0x0EU)
/** \brief API service called with invalid mode */
#define ICU_E_PARAM_MODE                ((uint8) 0x0FU)
/** \brief API service calleed without module initialisation */
#define ICU_E_UNINIT                    ((uint8) 0x14U)
/** \brief Icu_StopTimestamp called on channel not started or already stopped */
#define ICU_E_NOT_STARTED               ((uint8) 0x16U)
/** \brief API service called while a running operation */
#define ICU_E_BUSY_OPERATION            ((uint8) 0x16U)
/** \brief API service Icu_Init called when module already initialized */
#define ICU_E_ALREADY_INITIALIZED       ((uint8) 0x17U)
/** \brief API service called when notify interval is invalid */
#define ICU_E_PARAM_NOTIFY_INTERVAL     ((uint8) 0x18U)
/** \brief API service Icu_GetVersionInfo called and parameter is invalid */
#define ICU_E_PARAM_VINFO               ((uint8) 0x19U)
/* @} */

/**
 *  \name ICU Driver State Values
 *
 *  The ICU Driver State Values
 *  @{
 */
/*
*Design: MCAL-7144
*/
/** \brief ICU driver Status uninitialized */
#define ICU_STATUS_UNINIT                   ((uint8)(0U))
/** \brief ICU driver Status Initialized */
#define ICU_STATUS_INIT                     ((uint8)(1U))
/* @} */

/**
 *  \name Macros to define Rising and Falling edge config for capture registers
 *
 */
/** \brief Capture regsiter configuration for FALLING edge  */
#define ICU_FALLING                         ((uint8)(0U))
/** \brief Capture regsiter configuration for RISING edge  */
#define ICU_RISING                          ((uint8)(1U))

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/**
 *  \addtogroup MCAL_ICU_CFG ICU Configuration
 *  @{
 */

/**
 *  \brief This type defines a range of mode type
 */
typedef enum
{
   ICU_MODE_NORMAL,
   /**< Normal Mode */
   ICU_MODE_SLEEP
   /**< Sleep Mode */
} Icu_ModeType;

/**
 *  \brief This type defines a input state of ICU driver
 */
typedef enum
{
   ICU_ACTIVE,
   /**< Module is in active state */
   ICU_IDLE
   /**< Module is in idle state */
} Icu_InputStateType;


/**
 *  \brief This type defines Activation range
 */
typedef enum
{
   ICU_RISING_EDGE,
   /**< Rising Edge Activation type*/
   ICU_FALLING_EDGE,
   /**< Falling Edge Activation type*/
   ICU_BOTH_EDGES
   /**< Both Edge Activation type*/
} Icu_ActivationType;

/**
 *  \brief This type defines Value type
 */
typedef uint32 Icu_ValueType;

/**
 *  \brief This type defines Duty Cycle struct
 */
typedef struct
{
   Icu_ValueType ActiveTime;
   /**< Active time of Signal*/
   Icu_ValueType PeriodTime;
   /**< Period time of Signal*/
} Icu_DutyCycleType;

/**
 *  \brief This type defines return value Icu_GetTimeStampIndex
 */
typedef uint32 Icu_IndexType;

/**
 *  \brief This type defines return value of Icu-Icu_GetEdgeNumbers
 */
typedef uint32 Icu_EdgeNumberType;

/**
 *  \brief This type defines available measurement modes
 */
typedef enum
{
   ICU_MODE_SIGNAL_EDGE_DETECT,
   /**< Signal Edge Detect Mode*/
   ICU_MODE_SIGNAL_MEASUREMENT,
   /**< Signal measurement Mode*/
   ICU_MODE_TIMESTAMP,
   /**< Signal Timestamp Mode*/
   ICU_MODE_EDGE_COUNTER
   /**< Signal Edge counter Mode*/
} Icu_MeasurementModeType;

/**
 *  \brief This type defines Signal Measurement Property range
 */
typedef enum
{
   ICU_LOW_TIME,
   /**< Signal Low time*/
   ICU_HIGH_TIME,
   /**< Signal High time*/
   ICU_PERIOD_TIME,
   /**< Signal Period time*/
   ICU_DUTY_CYCLE
   /**< Signal Duty Cycle time*/
} Icu_SignalMeasurementPropertyType;

/**
 *  \brief This type defines TimeStamp Property range
 */
typedef enum
{
   ICU_LINEAR_BUFFER,
   /**< Linear Buffer to be used in Timestamp*/
   ICU_CIRCULAR_BUFFER
   /**< Circular Buffer to be used in Timestamp*/
} Icu_TimestampBufferType;

/** \brief  Notification callback function pointer  */
typedef void (*Icu_NotifyFuncType)(void);

/**
 *  \brief This type defines Channel type
 */
typedef uint8 Icu_ChannelType;

/**
 *  \brief This type defines Prescaler type
 */
typedef uint32 Icu_ChannelPrescalerType;

/**
 *  \brief ICU Channel Config Structure of Pre-Compile only
 */
typedef struct
{
    Icu_ChannelType      channelId;
    /**< Channel ID of ICU Channel in use */
} Icu_ChannelConfigType_PC;

/**
 *  \brief ICU Config Structure of Pre-Compile only
 */
typedef struct Icu_ConfigType_PC_s
{
    Icu_ChannelConfigType_PC chCfg[ICU_MAX_NUM_CHANNELS];
    /**< Channel configurations */
} Icu_ConfigType_PC;

/**
 *  \brief ICU Channel Config Structure
 */
typedef struct {
   Icu_ActivationType          defaultStartEdge;
   /**< Default activation edge to be used by ICU module */
   Icu_MeasurementModeType     measurementMode;
   /**< Measurement Mode to be used */
   Icu_SignalMeasurementPropertyType signalMeasurementProperty;
   /**< Signal Measurmenet property to be used if mode choosen */
   Icu_NotifyFuncType          notificationHandler;
   /**< Notification handler for notifications */
   Icu_TimestampBufferType     bufferType;
   /**< Buffer Type for Timestamp API mode */
   uint32                      instanceClkMHz;
   /**< Clk src for ICU module in MHz */
   uint32                      prescaler;
   /**< Prescaler value to be used for ICU  module */
} Icu_ChannelConfigType;

/**
 *  \brief ICU Config Structure
 */
typedef struct Icu_ConfigType_s
{
    Icu_ChannelType icuMaxChannel;
    /**< Number of ICU Channel being configured*/
    Icu_ChannelConfigType chCfg[ICU_MAX_NUM_CHANNELS];
    /**< Channel configurations */
} Icu_ConfigType;

#if (STD_ON == ICU_SAFETY_DIAGNOSTIC_API)
/**
 *  \brief Icu register readback structure
 *
 */
typedef struct
{
    /*
     * ECAP related registers
     */
     uint32 ECAP_CNTPHS;
     uint32 ECAP_ECCTL;
     uint32 ECAP_ECINT_EN_FLG;

} Icu_RegisterReadbackType;
#endif  /* #if (STD_ON == ICU_REGISTER_READBACK_API) */
/* @} */



/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */


/**
 *  \brief Service for ICU initialization.
 *
 *  \verbatim
 *  Service name        : Icu_Init
 *  Syntax              : Icu_Init(const Icu_ConfigType* ConfigPtr)
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]     : 0x00
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Non Reentrant
 *  Parameters (in)     : Pointer to the configuration
 *  Parameters (out)    : None
 *  Return value        : void
 *  Description         : Initializes internal variables
 *                        and the ECAP unit of the micro controller.
 *                        The function can be called on task level.
 *  \endverbatim
 */
FUNC(void, ICU_CODE) Icu_Init(
                    P2CONST(Icu_ConfigType, AUTOMATIC, ICU_PBCFG) ConfigPtr);



#if (STD_ON == ICU_DE_INIT_API)
/**
 *  \brief Service for ICU de-initialization.
 *
 *  \verbatim
 *  Service name        : Icu_DeInit
 *  Syntax              : Icu_DeInit(void)
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]     : 0x01
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Non Reentrant
 *  Parameters (in)     : None
 *  Parameters (out)    : None
 *  Return value        : void
 *  Description         :  DeInit ECAP module
 *  \endverbatim
 */
FUNC(void, ICU_CODE) Icu_DeInit( void );
#endif

/**
 *  \brief Service for setting Activation Condition
 *
 *  \verbatim
 *  Service name        : Icu_SetActivationCondition
 *  Syntax              : Icu_SetActivationCondition(Icu_ChannelType Channel,
 *                                              Icu_ActivationType Activation)
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]     : 0x05
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : Icu_ChannelType Channel
 *  Parameters (in)     : Icu_ActivationType Activation
 *  Parameters (out)    : None
 *  Return value        : void
 *  Description         : This service sets the Activation edge for channel
 *                          Type of activation (if supported by hardware)
 *                          ICU_RISING_EDGE, ICU_FALLING_EDGE, ICU_BOTH_EDGES
 *  \endverbatim
 */
FUNC(void, ICU_CODE) Icu_SetActivationCondition( Icu_ChannelType Channel, Icu_ActivationType Activation );

/**
 *  \brief Service for disabling notification
 *
 *  \verbatim
 *  Service name        : Icu_DisableNotification
 *  Syntax              : Icu_DisableNotification(Icu_ChannelType Channel)
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]     : 0x06
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : Icu_ChannelType Channel
 *  Parameters (out)    : None
 *  Return value        : void
 *  Description         : This service disables the notificaiton of a channel
 *  \endverbatim
 */
FUNC(void, ICU_CODE) Icu_DisableNotification( Icu_ChannelType Channel );

/**
 *  \brief Service for enabling notification
 *
 *  \verbatim
 *  Service name        : Icu_EnableNotification
 *  Syntax              : Icu_EnableNotification(Icu_ChannelType Channel)
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]     : 0x07
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : Icu_ChannelType Channel
 *  Parameters (out)    : None
 *  Return value        : void
 *  Description         : This service enables the notificaiton of a channel
 *  \endverbatim
 */
FUNC(void, ICU_CODE) Icu_EnableNotification( Icu_ChannelType Channel );


#if (ICU_GET_INPUT_STATE_API == STD_ON)
/**
 *  \brief Service for getting the ICU input status
 *
 *  \verbatim
 *  Service name        : Icu_GetInputState
 *  Syntax              : Icu_GetInputState(Icu_ChannelType Channel)
 *  Mode                : User Mode (Non-Privileged Mode)
 *  Service ID[hex]     : 0x08
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : Icu_ChannelType Channel
 *  Parameters (out)    : None
 *  Return value        : void
 *  Description         : This service returns the status of the ICU input
 *  \endverbatim
 */
FUNC(Icu_InputStateType, ICU_CODE) Icu_GetInputState( Icu_ChannelType Channel );
#endif

#if (ICU_TIMESTAMP_API == STD_ON)
/**
 *  \brief Service for starts the captuing of timer values on the edges
 *
 *  \verbatim
 *  Service name        : Icu_StartTimestamp
 *  Syntax              : Icu_StartTimestamp(Icu_ChannelType Channel,
 *                                          Icu_ValueType* BufferPtr,
 *                                          uint16 BufferSize,
 *                                          uint16 NotifyInterval)
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]     : 0x09
 *  Sync/Async          : Asynchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : Icu_ChannelType Channel
 *  Parameters (in)     : uint16 BufferSize
 *  Parameters (in)     : uint16 NotifyInterval
 *  Parameters (out)    : Icu_ValueType* BufferPtr
 *  Return value        : void
 *  Description         : This service starts the capturing of timer values on the edges
 *  \endverbatim
 */
FUNC(void, ICU_CODE) Icu_StartTimestamp( Icu_ChannelType Channel, Icu_ValueType* BufferPtr, uint16 BufferSize, uint16 NotifyInterval );

/**
 *  \brief Service for stopping the timestamp measurement
 *
 *  \verbatim
 *  Service name        : Icu_StopTimestamp
 *  Syntax              : Icu_StopTimestamp(Icu_ChannelType Channel)
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]     : 0x0A
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : Icu_ChannelType Channel
 *  Parameters (out)    : None
 *  Return value        : void
 *  Description         : This service stops timestamp measurement of the given channel.
 *  \endverbatim
 */
FUNC(void, ICU_CODE) Icu_StopTimestamp( Icu_ChannelType Channel );

/**
 *  \brief Service for reading the timestamp index of a givem channel
 *
 *  \verbatim
 *  Service name        : Icu_GetTimestampIndex
 *  Syntax              : Icu_GetTimestampIndex(Icu_ChannelType Channel)
 *  Mode                : User Mode (Non-Privileged Mode)
 *  Service ID[hex]     : 0x0B
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : Icu_ChannelType Channel
 *  Parameters (out)    : None
 *  Return value        : void
 *  Description         : This service reads the timestamp index of a given channel.
 *  \endverbatim
 */
FUNC(Icu_IndexType, ICU_CODE) Icu_GetTimestampIndex( Icu_ChannelType Channel );
#endif

#if (ICU_EDGE_COUNT_API == STD_ON)
/**
 *  \brief Service for resets the value of the counted edges to zero.
 *
 *  \verbatim
 *  Service name        : Icu_ResetEdgeCount
 *  Syntax              : Icu_ResetEdgeCount(Icu_ChannelType Channel)
 *  Mode                : User Mode (Non-Privileged Mode)
 *  Service ID[hex]     : 0x0C
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : Icu_ChannelType Channel
 *  Parameters (out)    : None
 *  Return value        : void
 *  Description         : This service resets the value of the counted edges to zero.
 *  \endverbatim
 */
FUNC(void, ICU_CODE) Icu_ResetEdgeCount( Icu_ChannelType Channel );

/**
 *  \brief Service enabling the counting of edges of a given channel.
 *
 *  \verbatim
 *  Service name        : Icu_EnableEdgeCount
 *  Syntax              : Icu_EnableEdgeCount(Icu_ChannelType Channel)
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]     : 0x0D
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : Icu_ChannelType Channel
 *  Parameters (out)    : None
 *  Return value        : void
 *  Description         : This service enables the couting of edges of the given channel.
 *  \endverbatim
 */
FUNC(void, ICU_CODE) Icu_EnableEdgeCount( Icu_ChannelType Channel );

/**
 *  \brief Service for disabling the couting of edges of the given channel.
 *
 *  \verbatim
 *  Service name        : Icu_DisableEdgeCount
 *  Syntax              : Icu_DisableEdgeCount(Icu_ChannelType Channel)
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]     : 0x0E
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : Icu_ChannelType Channel
 *  Parameters (out)    : None
 *  Return value        : void
 *  Description         : This service disables the couting of edges of a given channel.
 *  \endverbatim
 */
FUNC(void, ICU_CODE) Icu_DisableEdgeCount( Icu_ChannelType Channel );

/**
 *  \brief Service for reading the number of counted edges
 *
 *  \verbatim
 *  Service name        : Icu_GetEdgeNumbers
 *  Syntax              : Icu_GetEdgeNumbers(Icu_ChannelType Channel)
 *  Mode                : User Mode (Non-Privileged Mode)
 *  Service ID[hex]     : 0x0F
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : Icu_ChannelType Channel
 *  Parameters (out)    : None
 *  Return value        : void
 *  Description         : This service reads the number of counted edges.
 *  \endverbatim
 */
FUNC(Icu_EdgeNumberType, ICU_CODE) Icu_GetEdgeNumbers( Icu_ChannelType Channel );
#endif

#if (ICU_EDGE_DETECT_API == STD_ON)
/**
 *  \brief Service for enabling/re-enabling the detection of edges of the given channel.
 *
 *  \verbatim
 *  Service name        : Icu_EnableEdgeDetection
 *  Syntax              : Icu_EnableEdgeDetection(Icu_ChannelType Channel)
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]     : 0x16
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : Icu_ChannelType Channel
 *  Parameters (out)    : None
 *  Return value        : void
 *  Description         : This service enables/re-enables the detection of edges of a channel.
 *  \endverbatim
 */
FUNC(void, ICU_CODE) Icu_EnableEdgeDetection( Icu_ChannelType Channel );

/**
 *  \brief Service for disabling the detection of edges of the given channel.
 *
 *  \verbatim
 *  Service name        : Icu_EnableEdgeDetection
 *  Syntax              : Icu_EnableEdgeDetection(Icu_ChannelType Channel)
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]     : 0x17
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : Icu_ChannelType Channel
 *  Parameters (out)    : None
 *  Return value        : void
 *  Description         : This service disables the detection of edges of a channel.
 *  \endverbatim
 */
FUNC(void, ICU_CODE) Icu_DisableEdgeDetection( Icu_ChannelType Channel );
#endif

#if (ICU_SIGNAL_MEASUREMENT_API == STD_ON)
/**
 *  \brief Service for starting the measurement of signals.
 *
 *  \verbatim
 *  Service name        : Icu_StartSignalMeasurement
 *  Syntax              : Icu_StartSignalMeasurement(Icu_ChannelType Channel)
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]     : 0x13
 *  Sync/Async          : Asynchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : Icu_ChannelType Channel
 *  Parameters (out)    : None
 *  Return value        : void
 *  Description         : This service starts the measurement of signals of a given channel.
 *  \endverbatim
 */
FUNC(void, ICU_CODE) Icu_StartSignalMeasurement( Icu_ChannelType Channel );

/**
 *  \brief Service for stopping the measurement of signals.
 *
 *  \verbatim
 *  Service name        : Icu_StopSignalMeasurement
 *  Syntax              : Icu_StopSignalMeasurement(Icu_ChannelType Channel)
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]     : 0x14
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : Icu_ChannelType Channel
 *  Parameters (out)    : None
 *  Return value        : void
 *  Description         : This service stops the measurement of signals of a given channel.
 *  \endverbatim
 */
FUNC(void, ICU_CODE) Icu_StopSignalMeasurement( Icu_ChannelType Channel );
#endif

#if (ICU_GET_TIME_ELAPSED_API == STD_ON)
/**
 *  \brief Service for reading the elasped time for the given channel
 *
 *  \verbatim
 *  Service name        : Icu_GetTimeElapsed
 *  Syntax              : Icu_GetTimeElapsed(Icu_ChannelType Channel)
 *  Mode                : User Mode (Non-Privileged Mode)
 *  Service ID[hex]     : 0x10
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : Icu_ChannelType Channel
 *  Parameters (out)    : None
 *  Return value        : void
 *  Description         : This service reads elasped time for a given channel.
 *  \endverbatim
 */
FUNC (Icu_ValueType, ICU_CODE) Icu_GetTimeElapsed( Icu_ChannelType Channel );
#endif

#if (ICU_GET_DUTY_CYCLE_VALUES_API == STD_ON)
/**
 *  \brief Service for reading the coherent active time and period time of given channel.
 *
 *  \verbatim
 *  Service name        : Icu_GetTimeElapsed
 *  Syntax              : Icu_GetTimeElapsed(Icu_ChannelType Channel,
 *                                           Icu_DutyCycleType* DutyCycleValues)
 *  Mode                : User Mode (Non-Privileged Mode)
 *  Service ID[hex]     : 0x11
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : Icu_ChannelType Channel
 *  Parameters (out)    : Icu_DutyCycleType* DutyCycleValues
 *  Return value        : void
 *  Description         : This service reads active time and period of given channel.
 *  \endverbatim
 */
FUNC (void, ICU_CODE) Icu_GetDutyCycleValues( Icu_ChannelType Channel,
                    Icu_DutyCycleType* DutyCycleValues );
#endif

#if (ICU_GET_VERSION_INFO_API)
/**
 *  \brief This service returns the version information of this module.
 *
 *  \verbatim
 *  Service name        : Icu_GetVersionInfo
 *  Syntax              : Icu_GetVersionInfo(P2VAR(Std_VersionInfoType,
 *                           AUTOMATIC, PWM_APPL_DATA) versioninfo)
 *  Mode                : User Mode (Non-Privileged Mode)
 *  Service ID[hex]     : 0x12
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : Std_VersionInfoType - Version info.
 *  Parameters (out)    : None
 *  Return value        : void
 *  Description         : Returns the version information of this module.
 *                        The function can be called on task level.
 *  \endverbatim
 */
FUNC(void, ICU_CODE) Icu_GetVersionInfo(
        P2VAR(Std_VersionInfoType, AUTOMATIC, sICU_APPL_DATA) VersionInfoPtr);

#endif

#if (ICU_WAKEUP_FUNCTIONALITY_API == STD_ON)
/**
 *  \brief This function checks if a wakeup capable ICU channel is the source for a wakeup event and 
 *   calls the ECU state manager service EcuM_SetWakeupEvent in case of a valid ICU channel wakeup event.
 *
 *  \verbatim
 *  Service name        : Icu_CheckWakeup
 *  Syntax              : void Icu_CheckWakeup(EcuM_WakeupSourceType WakeupSource)
 *  Service ID[hex]     : 0x19
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant(limited according to ICU050)
 *  Parameters (in)     : WakeupSource
 *                        Informatin on wakeup source to be checked.
                          The associated ICU channel can be determined from configuration data
 *  Parameters (inout)  : None
 *  Parameters (out)    : None
 *  Return value        : None
 *  Description         : Checks if a wakeup capable ICU channel is the source for a wakeup event and 
 *                        calls the ECU state manager service EcuM_SetWakeupEvent in case of a valid ICU 
 *                        channel wakeup event.
 *  \endverbatim
 */
FUNC(void, ICU_CODE) Icu_CheckWakeup(EcuM_WakeupSourceType WakeupSource);
#endif /* if(ICU_WAKEUP_FUNCTIONALITY_API == STD_ON) */

#if (ICU_DISABLEWAKEUP_API == STD_ON)
/**
 *  \brief This function disables the wakeup capability of a single ICU channel.
 *
 *  \verbatim
 *  Service name        : Icu_DisableWakeup
 *  Syntax              : void Icu_DisableWakeup(Icu_ChannelType Channel)
 *  Service ID[hex]     : 0x20
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant(limited according to ICU050)
 *  Parameters (in)     : Channel
 *                        Numeric identifier of the ICU channel
 *  Parameters (inout)  : None
 *  Parameters (out)    : None
 *  Return value        : None
 *  Description         : This function disables the wakeup capability of a single ICU channel.
 *  \endverbatim
 */
FUNC(void, ICU_CODE) Icu_DisableWakeup(Icu_ChannelType Channel);
#endif /* if(ICU_DISABLEWAKEUP_API == STD_ON) */

#if (ICU_ENABLEWAKEUP_API == STD_ON)
/**
 *  \brief This function (re-)enables the wakeup capability of the given ICU channel.
 *
 *  \verbatim
 *  Service name        : Icu_EnableWakeup
 *  Syntax              : void Icu_EnableWakeup(Icu_ChannelType Channel)
 *  Service ID[hex]     : 0x21
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant(limited according to ICU050)
 *  Parameters (in)     : Channel
 *                        Numeric identifier of the ICU channel
 *  Parameters (inout)  : None
 *  Parameters (out)    : None
 *  Return value        : None
 *  Description         : This function (re-)enables the wakeup capability of the given ICU channel.
 *  \endverbatim
 */
FUNC(void, ICU_CODE) Icu_EnableWakeup(Icu_ChannelType Channel);
#endif /* if(ICU_ENABLEWAKEUP_API == STD_ON) */

#if (ICU_SETMODE_API == STD_ON)
/**
 *  \brief This function sets the ICU mode.
 *
 *  \verbatim
 *  Service name        : Icu_SetMode
 *  Syntax              : void  Icu_SetMode(Icu_ModeType Mode)
 *  Service ID[hex]     : 0x22
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Non Reentrant
 *  Parameters (in)     : Mode
 *                        ICU_MODE_NORMAL: Normal operation, all used interrupts are enabled 
 *                        according to the notification requests.
 *                        ICU_MODE_SLEEP: Reduced power mode. In sleep mode only those 
 *                        notifications are available which are configured as wakeup capable.
 *  Parameters (inout)  : None
 *  Parameters (out)    : None
 *  Return value        : None
 *  Description         : This function sets the ICU mode.
 *  \endverbatim
 */
FUNC(void, ICU_CODE) Icu_SetMode(Icu_ModeType Mode);
#endif /* if (ICU_SETMODE_API == STD_ON) */

#if (STD_ON == ICU_SAFETY_DIAGNOSTIC_API)
FUNC(Std_ReturnType, ICU_CODE) Icu_RegisterReadback(
  Icu_ChannelType IcuChannel,
    P2VAR(Icu_RegisterReadbackType, AUTOMATIC, ICU_APPL_DATA) RegRbPtr);
#endif

FUNC(Icu_IndexType, ICU_CODE) Icu_GetTimeStampIndex(Icu_ChannelType Channel);


#ifdef __cplusplus
}
#endif
/* @} */

#endif /* ICU_H_ */

/* @} */
