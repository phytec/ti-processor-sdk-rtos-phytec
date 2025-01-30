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
 *  \file     Adc.h
 *
 *  \brief    This file contains interface header for ADC MCAL driver
 *
 */

/**
 *  \defgroup MCAL_ADC_API ADC Handler and Driver API
 *
 *  The ADC Driver MCAL provides low level access to ADC peripheral
 *  present on Device.<br>
 *  The ADC Driver implements a standardized interface specified in the
 *  AUTOSAR_SWS_ADCDriver document.<br>
 *
 *  Only the following combinations of modes are supported:
 *      - One-shot,   Software Trigger,          Single    Access Mode
 *      - Continuous, Software Trigger, Circular Single    Access Mode
 *      - Continuous, Software Trigger, Linear   Single    Access Mode
 *        (similar to one-shot mode)
 *      - Continuous, Software Trigger, Circular Streaming Access Mode
 *      - Continuous, Software Trigger, Linear   Streaming Access Mode
 *
 *  Following combinations of modes are not supported:
 *      - All hardware trigger modes
 *      - One-shot,   Software Trigger, Stream Access (Linear + Circular) Mode
 *
 *  @sa MCAL_ADC_CFG
 *  @{
 */

/*
 * Below are the global design requirements which are met by this ADC
 * driver which can't be mapped to a particular source ID
 */
/*
 * Design: MCAL-5723,MCAL-5861
 */

/*
 * Below are the ADC's module environment design requirements which can't
 * be mapped to this driver
 */
/*
 * Design: MCAL-5805,MCAL-5708,MCAL-5739,MCAL-5748
 */

 /*
  * Note: MCAL-5700,MCAL-5834 are prerequisites which are done by SBL/GEL files.
  */


#ifndef ADC_H_
#define ADC_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "Std_Types.h"
#include "Adc_Cfg.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 *  \name ADC Driver Module SW Version Info
 *
 *  Defines for ADC Driver version used for compatibility checks
 *  @{
 */
/** \brief Driver Implementation Major Version */
#define ADC_SW_MAJOR_VERSION            (9U)
/** \brief Driver Implementation Minor Version */
#define ADC_SW_MINOR_VERSION            (0U)
/** \brief Driver Implementation Patch Version */
#define ADC_SW_PATCH_VERSION            (1U)

/* @} */

/**
 *  \name ADC Driver Module AUTOSAR Version Info
 *
 *  Defines for ADC Driver AUTOSAR version used for compatibility checks
 *  @{
 */
/** \brief AUTOSAR Major version specification implemented by ADC Driver */
#define ADC_AR_RELEASE_MAJOR_VERSION    (4U)
/** \brief AUTOSAR Minor  version specification implemented by ADC Driver */
#define ADC_AR_RELEASE_MINOR_VERSION    (3U)
/** \brief AUTOSAR Patch version specification implemented by ADC Driver */
#define ADC_AR_RELEASE_REVISION_VERSION (1U)
/* @} */

/**
 *  \name ADC Driver ID Info
 *  @{
 */
/** \brief Texas Instruments Vendor ID */
#define ADC_VENDOR_ID                   ((uint16) 44U)
/** \brief ADC Driver Module ID */
#define ADC_MODULE_ID                   ((uint16) 123U)
/** \brief ADC Driver Instance ID */
#define ADC_INSTANCE_ID                 ((uint8) 0U)
/* @} */
/**
 *  \brief The default channel resolution supported by the current ADC hardware.
 *  No other value is supported by the hardware.
 */
#define ADC_DEF_CHANNEL_RESOLUTION      ((uint8) 12U)

/**
 *  \brief Number of MCAL channels - in terms of ADC HW, this represents the
 *  number of hardware steps.
 *  Note: In terms of ADC IP number of steps is 16.
 *        Due to AUTOSAR specification, the channel ID's should be unique i.e
 *        each channel Id and actual HW channel should have one to one mapping.
 *        So ADC_NUM_CHANNEL is fixed to 9 as number of actual HW channels
 *        available is also 9.
 *
 */
#define ADC_NUM_CHANNEL                 (10U)
/** \brief Minimum value of channel ID */
#define ADC_MIN_CHANNEL_ID              (0U)
/** \brief Maximum value of channel ID */
#define ADC_MAX_CHANNEL_ID              (ADC_NUM_CHANNEL - 1U)

/**
 *  \brief Number of actual HW channels - in terms of ADC HW, this represents
 *  the actual channel input to the ADC module.
 *  Note: This is a fixed value as per the ADC module and can't be changed.
 */
#define ADC_NUM_HW_CHANNEL              (10U)
/** \brief Minimum value of HW channel ID */
#define ADC_MIN_HW_CHANNEL_ID           (0U)
/** \brief Maximum value of HW channel ID */
#define ADC_MAX_HW_CHANNEL_ID           (ADC_NUM_HW_CHANNEL - 1U)

/** \brief Minimum value of open delay */
#define ADC_MIN_OPEN_DELAY              (0x00U)
/** \brief Maximum value of open delay */
#define ADC_MAX_OPEN_DELAY              (0x3FFFFU)
/** \brief Minimum value of sample delay */
#define ADC_MIN_SAMPLE_DELAY            (0x00U)
/** \brief Maximum value of sample delay */
#define ADC_MAX_SAMPLE_DELAY            (0xFFU)
/** \brief Minimum value of range */
#define ADC_MIN_RANGE                   (0x00U)
/** \brief Maximum value of range */
#define ADC_MAX_RANGE                   (0xFFFU)

/*
 *Design: MCAL-5687
 */
/**
 *  \name ADC Error Codes
 *
 *  Error codes returned by ADC functions
 *  @{
 */
#ifndef ADC_E_UNINIT
/** \brief API service used without module initiali-zation */
#define ADC_E_UNINIT                    ((uint8) 0x0AU)
#endif
#ifndef ADC_E_BUSY
/** \brief API service called during ongoing process */
#define ADC_E_BUSY                      ((uint8) 0x0BU)
#endif
#ifndef ADC_E_IDLE
/** \brief API service called while no conversion is ongoing. */
#define ADC_E_IDLE                      ((uint8) 0x0CU)
#endif
#ifndef ADC_E_ALREADY_INITIALIZED
/**
 *  \brief API Adc_Init service called while the ADC driver has already been
 *  initialized
 */
#define ADC_E_ALREADY_INITIALIZED       ((uint8) 0x0DU)
#endif
#ifndef ADC_E_PARAM_CONFIG
/** \brief API service called with incorrect configuration parameter. */
#define ADC_E_PARAM_CONFIG              ((uint8) 0x0EU)
#endif
#ifndef ADC_E_PARAM_POINTER
/** \brief API servcie called with invalid data buffer pointer. */
#define ADC_E_PARAM_POINTER             ((uint8) 0x14U)
#endif
#ifndef ADC_E_PARAM_GROUP
/** \brief API servcie called with invalid group ID. */
#define ADC_E_PARAM_GROUP               ((uint8) 0x15U)
#endif
#ifndef ADC_E_WRONG_CONV_MODE
/**
 *  \brief API servcie called on a group with conversion mode configured as
 *  continuous.
 */
#define ADC_E_WRONG_CONV_MODE           ((uint8) 0x16U)
#endif
#ifndef ADC_E_WRONG_TRIGG_SRC
/** \brief API servcie called for group with wrong trigger source */
#define ADC_E_WRONG_TRIGG_SRC           ((uint8) 0x17U)
#endif
#ifndef ADC_E_NOTIF_CAPABILITY
/**
 *  \brief Enable/disable notification function for a group whose configuration
 *  set has no notification available.
 */
#define ADC_E_NOTIF_CAPABILITY          ((uint8) 0x18U)
#endif
#ifndef ADC_E_BUFFER_UNINIT
/** \brief Conversion started and result buffer pointer is not initialized. */
#define ADC_E_BUFFER_UNINIT             ((uint8) 0x19U)
#endif
#ifndef ADC_E_NOT_DISENGAGED
/** \brief One or more ADC group/channel not in IDLE state. */
#define ADC_E_NOT_DISENGAGED            ((uint8) 0x1AU)
#endif
#ifndef ADC_E_POWER_STATE_NOT_SUPPORTED
/** \brief Unsupported power state request. */
#define ADC_E_POWER_STATE_NOT_SUPPORTED ((uint8) 0x1BU)
#endif
#ifndef ADC_E_TRANSITION_NOT_POSSIBLE
/** \brief Requested power state can not be reached directly. */
#define ADC_E_TRANSITION_NOT_POSSIBLE   ((uint8) 0x1CU)
#endif
#ifndef ADC_E_PERIPHERAL_NOT_PREPARED
/** \brief ADC not prepared for target power state. */
#define ADC_E_PERIPHERAL_NOT_PREPARED   ((uint8) 0x1DU)
#endif
/* @} */

/** \brief Numeric ID of an ADC channel */
/*
 *Design: MCAL-5852
 */
typedef uint8   Adc_ChannelType;

/** \brief Numeric ID of an ADC channel group */
/*
 * Design: MCAL-5702
 */
typedef uint8   Adc_GroupType;

/**
 * \brief Type for reading the converted values of a channel group (raw,
 *  without further scaling, right aligned).
 *
 *  Note: Even though the resolution for this ADC HW is 10-bit, we set this
 *  to 32-bit integer as the register read access is 32-bit.
 */
/*
 * Design: MCAL-5881,MCAL-5856
 */
typedef uint32  Adc_ValueGroupType;

/** \brief Type of clock prescaler factor. */
/*
 * Design: MCAL-5709
 */
typedef uint32  Adc_PrescaleType;

/**
 *  \brief Type of conversion time, i.e. the time during which the sampled
 *  analogue value is converted into digital representation.
 */
/*
 * Design: MCAL-5822
 */
typedef uint16  Adc_ConversionTimeType;

/**
 *  \brief Type of sampling time, i.e. the time during which the value is
 *  sampled (in clockcycles).
 */
/*
 * Design: MCAL-5855
 */
typedef uint16  Adc_SamplingTimeType;

/** \brief Type of channel resolution in number of bits. */
/*
 * Design: MCAL-5800
 */
typedef uint8   Adc_ResolutionType;

/** \brief Priority level of the channel. Lowest priority is 0. */
/*
 * Design: MCAL-5712
 */
typedef uint8   Adc_GroupPriorityType;

/** \brief Type of assignment of channels to a channel group. */
/*
 * Design: MCAL-5817
 */
typedef uint32  Adc_GroupDefType;

/**
 *  \brief Type for configuring the number of group conversions in streaming
 *   access mode (in single access mode, parameter is 1).
 */
/*
 * Design: MCAL-5826
 */
typedef uint16  Adc_StreamNumSampleType;

/** \brief Type for the value of the ADC module embedded timer. */
/*
 * Design: MCAL-5858
 */
typedef uint8   Adc_HwTriggerTimerType;

/**
 *  \brief Specifies the identification (ID) for a ADC Hardware microcontroller
 *  peripheral (unit)
 */
typedef uint8   Adc_HWUnitType;

#if (STD_ON == ADC_INTERNAL_DIAGNOSTIC_DEBUG_MODE_API)
/**
 *  \brief Specifies the Reference select for functional internal diagnostic
 *  debug mode.
 */
typedef uint8   Adc_RefSelType;
#endif

/**
 *  \name ADC Service Ids
 *
 *  The Service Id is one of the argument to Det_ReportError function and
 *  is used to identify the source of the error
 *  @{
 */
/** \brief Adc_Init() API Service ID */
#define ADC_SID_INIT                        ((uint8) 0x00U)
/** \brief Adc_DeInit() API Service ID */
#define ADC_SID_DEINIT                      ((uint8) 0x01U)
/** \brief Adc_StartGroupConversion() API Service ID */
#define ADC_SID_START_GROUP_CONVERSION      ((uint8) 0x02U)
/** \brief Adc_StopGroupConversion() API Service ID */
#define ADC_SID_STOP_GROUP_CONVERSION       ((uint8) 0x03U)
/** \brief Adc_ReadGroup() API Service ID */
#define ADC_SID_READ_GROUP                  ((uint8) 0x04U)
/** \brief Adc_EnableHardwareTrigger() API Service ID */
#define ADC_SID_ENABLE_HARDWARE_TRIGGER     ((uint8) 0x05U)
/** \brief Adc_DisableHardwareTrigger() API Service ID */
#define ADC_SID_DISABLE_HARDWARE_TRIGGER    ((uint8) 0x06U)
/** \brief Adc_EnableGroupNotification() API Service ID */
#define ADC_SID_ENABLE_GROUP_NOTIFICATION   ((uint8) 0x07U)
/** \brief Adc_DisableGroupNotification() API Service ID */
#define ADC_SID_DISABLE_GROUP_NOTIFICATION  ((uint8) 0x08U)
/** \brief Adc_GetGroupStatus() API Service ID */
#define ADC_SID_GET_GROUP_STATUS            ((uint8) 0x09U)
/** \brief Adc_GetVersionInfo() API Service ID */
#define ADC_SID_GET_VERSION_INFO            ((uint8) 0x0AU)
/** \brief Adc_GetStreamLastPointer() API Service ID */
#define ADC_SID_GET_STREAM_LAST_POINTER     ((uint8) 0x0BU)
/** \brief Adc_SetupResultBuffer() API Service ID */
#define ADC_SID_SETUP_RESULT_BUFFER         ((uint8) 0x0CU)
/** \brief Adc_SetPowerState() API Service ID */
#define ADC_SID_SET_POWER_STATE             ((uint8) 0x10U)
/** \brief Adc_GetCurrentPowerState() API Service ID */
#define ADC_SID_GET_CURRENT_POWER_STATE     ((uint8) 0x11U)
/** \brief Adc_GetTargetPowerState() API Service ID */
#define ADC_SID_GET_TARGET_POWER_STATE      ((uint8) 0x12U)
/** \brief Adc_PreparePowerState() API Service ID */
#define ADC_SID_PREPARE_POWER_STATE         ((uint8) 0x13U)
/** \brief Adc_Main_PowerTransitionManager() API Service ID */
#define ADC_SID_MAIN_POWER_TRANSITION_MANAGER       ((uint8) 0x14U)
/** \brief IoHwAb_AdcNotification() API Service ID */
#define ADC_SID_IOHWAB_NOTIFICATION                 ((uint8) 0x20U)
/** \brief IoHwAb_Adc_NotifyReadyForPowerState() API Service ID */
#define ADC_SID_IOHWAB_NOTIFY_READY_FOR_POWER_STATE ((uint8) 0x70U)
/* @} */

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/**
 *  \addtogroup MCAL_ADC_CFG ADC Configuration
 *  @{
 */

/** \brief Typedef for group end notification function pointer */
typedef P2FUNC(void, ADC_APPL_CODE, Adc_GroupEndNotifyType)(void);

/**
 *  \brief Current status of the conversion of the requested ADC Channel group
 */
/*
 * Design: MCAL-5811
 */
typedef enum
{
    ADC_IDLE,
    /**< The conversion of the specified group has not been started.
     *   No result is available */
    ADC_BUSY,
    /**< The conversion of the specified group has been started and is still
     *   going on. So far no result is available */
    ADC_COMPLETED,
    /**< A conversion round (which is not the final one) of the specified group
     *  has been finished. A result is available for all channels of the
     *  group */
    ADC_STREAM_COMPLETED
    /**< The result buffer is completely filled. For each channel of the
     *   selected group the number of samples to be acquired is available */
} Adc_StatusType;

/**
 *  \brief Type for configuring the trigger source for an ADC Channel group
 */
/*
 * Design: MCAL-5836
 */
typedef enum
{
    ADC_TRIGG_SRC_SW,
    /**< Group is triggered by a software API call */
    ADC_TRIGG_SRC_HW
    /**< Group is triggered by a hardware event
     *   Note: This feature is not supported in this driver */
} Adc_TriggerSourceType;

/**
 *  \brief Type for configuring the conversion mode of an ADC Channel group
 */
/*
 * Design: MCAL-5735
 */
typedef enum
{
    ADC_CONV_MODE_ONESHOT,
    /**< Exactly one conversion of each channel in an ADC channel group is
     *   performed after the configured trigger event */
    ADC_CONV_MODE_CONTINUOUS
    /**< Repeated conversions of each ADC channel in an ADC channel
     *   group are performed */
} Adc_GroupConvModeType;

/**
 *  \brief Type for configuring the streaming access mode buffer type
 */
/*
 * Design: MCAL-5742
 */
typedef enum
{
    ADC_STREAM_BUFFER_LINEAR,
    /**< The ADC Driver stops the conversion as soon as the stream buffer
     *   is full (number of samples reached) */
    ADC_STREAM_BUFFER_CIRCULAR
    /**< The ADC Driver continues the conversion even if the stream buffer is
     *   full (number of samples reached) by wrapping around the stream buffer
     *   itself */
} Adc_StreamBufferModeType;

/**
 *  \brief Type for configuring the access mode to group conversion results
 */
/*
 * Design: MCAL-5769
 */
typedef enum
{
    ADC_ACCESS_MODE_SINGLE,
    /**< Single value access mode */
    ADC_ACCESS_MODE_STREAMING
    /**< Streaming access mode */
} Adc_GroupAccessModeType;

/**
 *  \brief Type for configuring on which edge of the hardware trigger signal
 *  the driver should react, i.e. start the conversion.
 *
 *  Note: This feature is not supported in this driver
 */
/*
 * Design: MCAL-5871
 */
typedef enum
{
    ADC_HW_TRIG_RISING_EDGE,
    /**< React on the rising edge of the hardware trigger signal */
    ADC_HW_TRIG_FALLING_EDGE,
    /**< React on the falling edge of the hardware trigger signal */
    ADC_HW_TRIG_BOTH_EDGES
    /**< React on both edges of the hardware trigger signal */
} Adc_HwTriggerSignalType;

/**
 *  \brief Type for configuring the prioritization mechanism.
 */
/*
 * Design: MCAL-5807
 */
typedef enum
{
    ADC_PRIORITY_NONE,
    /**< Priority mechanism is not available */
    ADC_PRIORITY_HW,
    /**< Hardware priority mechanism is available only */
    ADC_PRIORITY_HW_SW
    /**< Hardware and software priority mechanism is available */
} Adc_PriorityImplementationType;

/**
 *  \brief Replacement mechanism, which is used on ADC group level, if a group
 *  conversion is interrupted by a group which has a higher priority.
 */
/*
 * Design: MCAL-5732
 */
typedef enum
{
    ADC_GROUP_REPL_ABORT_RESTART,
    /**< Abort/Restart mechanism is used on group level, if a group is
     *   interrupted by a higher priority group */
    ADC_GROUP_REPL_SUSPEND_RESUME,
    /**< Suspend/Resume mechanism is used on group level, if a group is
     *   interrupted by a higher priority group.
     *   Note: The implementation is same as abort/restart i.e. when a group
     *   is started again, previous intermediate results are discarded and
     *   the driver always starts from first channel of the group */
} Adc_GroupReplacementType;

/**
 *  \brief In case of active limit checking: defines which conversion values
 *  are taken into account related to the boardes defined with
 *  AdcChannelLowLimit and AdcChannelHighLimit.
 *
 *  Note: This feature is not supported yet.
 */
/*
 * Design: MCAL-5703
 */
typedef enum
{
    ADC_RANGE_UNDER_LOW,
    /**< Range below low limit - low limit value included */
    ADC_RANGE_BETWEEN,
    /**< Range between low limit and high limit - high limit value included */
    ADC_RANGE_OVER_HIGH,
    /**< Range above high limit */
    ADC_RANGE_ALWAYS,
    /**< Complete range - independent from channel limit settings */
    ADC_RANGE_NOT_UNDER_LOW,
    /**< Range above low limit */
    ADC_RANGE_NOT_BETWEEN,
    /**< Range above high limit or below low limit - low limit value included */
    ADC_RANGE_NOT_OVER_HIGH
    /**< Range below high limit - high limit value included */
} Adc_ChannelRangeSelectType;

/**
 *  \brief Type for alignment of ADC raw results in ADC result buffer
 *  (left/right alignment).
 */
/*
 * Design: MCAL-5843
 */
typedef enum
{
    ADC_ALIGN_LEFT,
    /**< Left alignment.
     *   Note: This feature is not supported in the ADC hardware */
    ADC_ALIGN_RIGHT
    /**< Right alignment */
} Adc_ResultAlignmentType;

/**
 *  \brief Power state currently active or set as target power state.
 */
/*
 * Design: MCAL-5829
 */
typedef enum
{
    ADC_FULL_POWER,
    /**< Full Power (0) */
    ADC_ZERO_POWER
    /**< Power modes with decreasing power consumptions */
} Adc_PowerStateType;

/**
 *  \brief Result of the requests related to power state transitions.
 */
/*
 * Design: MCAL-5705
 */
typedef enum
{
    ADC_SERVICE_ACCEPTED,
    /**< Power state change executed */
    ADC_NOT_INIT,
    /**< ADC Module not initialized */
    ADC_SEQUENCE_ERROR,
    /**< Wrong API call sequence */
    ADC_HW_FAILURE,
    /**< The HW module has a failure which prevents it to enter the required
     *   power state */
    ADC_POWER_STATE_NOT_SUPP,
    /**< ADC Module does not support the requested power state */
    ADC_TRANS_NOT_POSSIBLE,
    /**< ADC Module cannot transition directly from the current power state
     *   to the requested power state or the HW peripheral is still busy */
} Adc_PowerStateRequestResultType;

/**
 * \brief  Enum to number of samplings to average.
 *         ADC allows user to program the number of samplings to average.
 */
typedef enum
{
    ADC_AVERAGING_MODE_NONE,
    /**< Disable averaging */
    ADC_AVERAGING_MODE_2_SAMPLES,
    /**< Set number of samplings to average to 2 */
    ADC_AVERAGING_MODE_4_SAMPLES,
    /**< Set number of samplings to average to 4 */
    ADC_AVERAGING_MODE_8_SAMPLES,
    /**< Set number of samplings to average to 8 */
    ADC_AVERAGING_MODE_16_SAMPLES
    /**< Set number of samplings to average to 16 */
} Adc_AveragingMode;

/**
 * \brief  Structure containing parameters for ADC MCAL channel configuration.
 *         In term of ADC hardware, this represents the step configuration.
 *         There are ADC_NUM_CHANNEL steps in the ADC hardware and each step
 *         could be mapped to an actual hardware input channel.
 */
typedef struct
{
    uint32              hwChannelId;
    /**< The hardware channel number from which input is given.
     *   Valid values: ADC_MIN_HW_CHANNEL_ID to ADC_MAX_HW_CHANNEL_ID
     *   0x0 = Channel 1
     *   0x1 = Channel 2
     *   0x2 = Channel 3
     *   0x3 = Channel 4
     *   0x4 = Channel 5
     *   0x5 = Channel 6
     *   0x6 = Channel 7
     *   0x7 = Channel 8
     *   0x8 = VREFP
     *   0x9 = VREFN
     */
    uint32              openDelay;
    /**< Number of ADC clock cycles to wait after applying the step
     *   configuration registers and before sending the start of ADC conversion.
     *   Valid values: ADC_MIN_OPEN_DELAY to ADC_MAX_OPEN_DELAY */
    uint32              sampleDelay;
    /**< Number of ADC clock cycles to hold SOC high.
     *   The actual delay is +1 of this value.
     *   Valid values: ADC_MIN_SAMPLE_DELAY to ADC_MAX_SAMPLE_DELAY */
    uint32              rangeCheckEnable;
    /**< Option to enable range check per channel.
     *   Enabled if it is TRUE
     *   Disabled if it is FALSE
     *
     *   Note: Since there are no standard MCAL API for this feature, this is
     *   not supported by the ADC driver. So set this to FALSE */
    Adc_AveragingMode   averagingMode;
    /**< Option for averaging the sampled data.
     *   Refer enum #Adc_AveragingMode */
} Adc_ChannelConfigType;

/**
 *  \brief ADC Group configuration structure.
 */
/*
 *Design: MCAL-5865,MCAL-5764,MCAL-5756,MCAL-5889,MCAL-5801,MCAL-5883,MCAL-5845,MCAL-5814,MCAL-5757,MCAL-5768
 */
typedef struct
{
    Adc_GroupType                   groupId;
    /**< Group ID - This should be same as that of the index in to the
     *   groupCfg[] array */
    Adc_GroupPriorityType           groupPriority;
    /**< Group priority */
    Adc_HWUnitType                  hwUnitId;
    /**< HWUnit associated with this group */
    Adc_GroupEndNotifyType          Adc_GroupEndNotification;
    /**< Group end notification callback fxn pointer */

    Adc_StreamNumSampleType         streamNumSamples;
    /**< Contains how many samples fit into result buffer */
    Adc_ResolutionType              resolution;
    /**< Group resolution - This is not configurable and should be set to
     *   ADC_DEF_CHANNEL_RESOLUTION */

    Adc_GroupConvModeType           convMode;
    /**< Operation mode of the group */
    Adc_TriggerSourceType           triggSrc;
    /**< Determines the trigger source (hw or sw trigger).
     *   Note: Only SW trigger is supported */
    Adc_GroupAccessModeType         accessMode;
    /**< Defines the type of the groups buffer */
    Adc_StreamBufferModeType        streamBufMode;
    /**< Use linear or circular stream buffer */
    Adc_HwTriggerSignalType         hwTrigSignal;
    /**< Use rising or falling edge for event pin trigger.
     *   Note: Since HW trigger is not supported, this parameter is not used */
    Adc_HwTriggerTimerType          hwTrigTimer;
    /**< Hardware trigger event.
     *   Note: Since HW trigger is not supported, this parameter is not used.
     *   Set it to 0. */
    Adc_GroupReplacementType        groupReplacement;
    /**< Group replacement logic when priority mechanism is ON - i.e
     *   prioritySupport is not ADC_PRIORITY_NONE. */

    uint32                          highRange;
    /**< Sampled ADC data is compared to this value. If the sampled data is
     *   greater than this value, then interrupt is generated.
     *   Valid values: ADC_MIN_RANGE to ADC_MAX_RANGE.
     *
     *   This check can be enabled/disabled individually for each of the
     *   channel (step) using rangeCheckEnable parameter.
     *
     *   Note: Since there are no standard MCAL API for this feature, this is
     *   not supported by the ADC driver */
    uint32                          lowRange;
    /**< Sampled ADC data is compared to this value. If the sampled data is
     *   lesser than this value, then interrupt is generated.
     *   Valid values: ADC_MIN_RANGE to ADC_MAX_RANGE.
     *
     *   This check can be enabled/disabled individually for each of the
     *   channel (step) using rangeCheckEnable parameter.
     *
     *   Note: Since there are no standard MCAL API for this feature, this is
     *   not supported by the ADC driver */

    uint32                          numChannels;
    /**< Number of channels in this group */
    Adc_ChannelConfigType           channelConfig[ADC_NUM_CHANNEL];
    /**< Channel (HW step) configuration. numChannels elements should be
     *   initialized */
} Adc_GroupConfigType;

/**
 *  \brief ADC Hardware unit configuration structure.
 */
typedef struct
{
    Adc_HWUnitType      hwUnitId;
    /**< ADC HW unit to use */
    uint32              baseAddr;
} Adc_HwUnitConfigType;

/**
 *  \brief ADC config structure
 */
/*
 * Design: MCAL-5716,MCAL-5760,MCAL-5690
 */
typedef struct Adc_ConfigType_s
{
    uint8                   maxGroup;
    /**< Maximum number of group
     *   Should not be more than ADC_MAX_GROUP */
    uint8                   maxHwUnit;
    /**< Maximum number of HW unit
     *   Should not be more than ADC_MAX_HW_UNIT */
    Adc_GroupConfigType     groupCfg[ADC_MAX_GROUP];
    /**< Group configurations */
    Adc_HwUnitConfigType    hwUnitCfg[ADC_MAX_HW_UNIT];
    /**< HW Unit configurations */
} Adc_ConfigType;

#if (STD_ON == ADC_REGISTER_READBACK_API)
/**
 *  \brief ADC register readback structure
 *
 */
typedef struct
{
    /*
     * ADC related registers
     */
    uint32      adcRev;
    /**< IP revision identifier */
    uint32      adcCtrl;
    /**< Control Register */
    uint32      adcMisc;
    /**< Internal Calibration */

} Adc_RegisterReadbackType;
#endif  /* #if (STD_ON == ADC_REGISTER_READBACK_API) */

/* @} */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  \brief Service for ADC initialization.
 *
 *  \verbatim
 *  Service name        : Adc_Init
 *  Syntax              : void Adc_Init( const Adc_ConfigType* CfgPtr )
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]     : 0x00
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Non Reentrant
 *  Parameters (in)     : Pointer to configuration set in Variant PB
 *                        (Variant PC requires a NULL_PTR).
 *  Parameters (inout)  : None
 *  Parameters (out)    : None
 *  Return value        : None
 *  Description         : Initializes the ADC hardware units and driver
 *  \endverbatim
 */
FUNC(void, ADC_CODE) Adc_Init(
                    P2CONST(Adc_ConfigType, AUTOMATIC, ADC_PBCFG) CfgPtr);

#if (STD_ON == ADC_DEINIT_API)
/**
 *  \brief This service sets all ADC HW Units to a state comparable to their
 *  power on reset state
 *
 *  \verbatim
 *  Service name        : Adc_DeInit
 *  Syntax              : void Adc_DeInit(void)
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]     : 0x01
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Non Reentrant
 *  Parameters (in)     : None
 *  Parameters (inout)  : None
 *  Parameters (out)    : None
 *  Return value        : None
 *  Description         : Returns all ADC HW Units to a state comparable to
 *                        their power on reset state
 *  \endverbatim
 */
FUNC(void, ADC_CODE) Adc_DeInit(void);
#endif  /* #if (STD_ON == ADC_DEINIT_API) */

/**
 *  \brief This service initializes ADC driver with the group specific result
 *  buffer start address where the conversion results will be stored.
 *
 *  \verbatim
 *  Service name        : Adc_SetupResultBuffer
 *  Syntax              : Std_ReturnType Adc_SetupResultBuffer(
 *                                          Adc_GroupType Group,
 *                                          Adc_ValueGroupType* DataBufferPtr)
 *  Mode                : User Mode (Non-Privileged Mode)
 *  Service ID[hex]     : 0x0C
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : Group - Numeric ID of requested ADC channel group.
 *                        DataBufferPtr - pointer to result data buffer
 *  Parameters (inout)  : None
 *  Parameters (out)    : None
 *  Return value        : Std_ReturnType
 *                        E_OK: result buffer pointer initialized correctly
 *                        E_NOT_OK: operation failed or development error occured
 *  Description         : Initializes ADC driver with the group specific result
 *                        buffer start address where the conversion results
 *                        will be stored. The application has to ensure that
 *                        the application buffer, where DataBufferPtr points
 *                        to, can hold all the conversion results of the
 *                        specified group. The initialization with
 *                        Adc_SetupResultBuffer is required after reset,
 *                        before a group conversion can be started.
 *  \endverbatim
 */
FUNC(Std_ReturnType, ADC_CODE) Adc_SetupResultBuffer(
                                    Adc_GroupType Group,
                                    const Adc_ValueGroupType *DataBufferPtr);

/**
 *  \brief Returns the conversion status of the requested ADC Channel group.
 *
 *  \verbatim
 *  Service name        : Adc_GetGroupStatus
 *  Syntax              : Adc_StatusType Adc_GetGroupStatus( Adc_GroupType Group )
 *  Mode                : User Mode (Non-Privileged Mode)
 *  Service ID[hex]     : 0x09
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : Group - Numeric ID of requested ADC channel group.
 *  Parameters (inout)  : None
 *  Parameters (out)    : None
 *  Return value        : Adc_StatusType - Conversion status for the requested
 *                        group
 *  Description         : Returns the conversion status of the requested ADC
 *                        Channel group.
 *  \endverbatim
 */
FUNC(Adc_StatusType, ADC_CODE) Adc_GetGroupStatus(Adc_GroupType Group);

/**
 *  \brief Returns the number of valid samples per channel, stored in the
 *  result buffer.
 *
 *  \verbatim
 *  Service name        : Adc_GetStreamLastPointer
 *  Syntax              : Adc_StreamNumSampleType Adc_GetStreamLastPointer(
 *                              Adc_GroupType Group,
 *                              Adc_ValueGroupType** PtrToSamplePtr )
 *  Mode                : User Mode (Non-Privileged Mode)
 *  Service ID[hex]     : 0x0B
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : Group - Numeric ID of requested ADC channel group.
 *  Parameters (inout)  : None
 *  Parameters (out)    : PtrToSamplePtr - Pointer to result buffer pointer.
 *  Return value        : Adc_StreamNumSampleType - Number of valid samples
 *                        per channel
 *  Description         : Returns the number of valid samples per channel,
 *                        stored in the result buffer. Reads a pointer,
 *                        pointing to a position in the group result buffer.
 *                        With the pointer position, the results of all group
 *                        channels of the last completed conversion round can
 *                        be accessed. With the pointer and the return value,
 *                        all valid group conversion results can be accessed
 *                        (the user has to take the layout of the result buffer
 *                        into account).
 *  \endverbatim
 */
FUNC(Adc_StreamNumSampleType, ADC_CODE) Adc_GetStreamLastPointer(
        Adc_GroupType Group,
        Adc_ValueGroupType **PtrToSamplePtr);

#if (STD_ON == ADC_ENABLE_START_STOP_GROUP_API)
/**
 *  \brief Starts the conversion of all channels of the requested ADC
 *  Channel group
 *
 *  \verbatim
 *  Service name        : Adc_StartGroupConversion
 *  Syntax              : void Adc_StartGroupConversion( Adc_GroupType Group )
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]     : 0x02
 *  Sync/Async          : Asynchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : Group - Numeric ID of requested ADC Channel group.
 *  Parameters (inout)  : None
 *  Parameters (out)    : None
 *  Return value        : None
 *  Description         : Starts the conversion of all channels of the
 *                        requested ADC Channel group.
 *  \endverbatim
 */
FUNC(void, ADC_CODE) Adc_StartGroupConversion(Adc_GroupType Group);

/**
 *  \brief Stops the conversion of the requested ADC Channel group
 *
 *  \verbatim
 *  Service name        : Adc_StopGroupConversion
 *  Syntax              : void Adc_StopGroupConversion( Adc_GroupType Group )
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]     : 0x03
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : Group - Numeric ID of requested ADC Channel group.
 *  Parameters (inout)  : None
 *  Parameters (out)    : None
 *  Return value        : None
 *  Description         : Stops the conversion of the requested ADC Channel
 *                        group.
 *  \endverbatim
 */
FUNC(void, ADC_CODE) Adc_StopGroupConversion(Adc_GroupType Group);
#endif  /* #if (STD_ON == ADC_ENABLE_START_STOP_GROUP_API) */

#if (STD_ON == ADC_READ_GROUP_API)
/**
 *  \brief Reads the group conversion result of the last completed conversion
 *  round of the requested group and stores the channel values starting at
 *  the DataBufferPtr address
 *
 *  \verbatim
 *  Service name        : Adc_ReadGroup
 *  Syntax              : Std_ReturnType Adc_ReadGroup(
 *                              Adc_GroupType Group,
 *                              Adc_ValueGroupType* DataBufferPtr )
 *  Mode                : User Mode (Non-Privileged Mode)
 *  Service ID[hex]     : 0x04
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : Group - Numeric ID of requested ADC channel group.
 *                        DataBufferPtr - ADC results of all channels of the
 *                        selected group are stored in the data buffer
 *                        addressed with the pointer
 *  Parameters (inout)  : None
 *  Parameters (out)    : None
 *  Return value        : Std_ReturnType
 *                        E_OK: results are available and written to the data buffer
 *                        E_NOT_OK: no results are available or development error occured
 *  Description         : Reads the group conversion result of the last
 *                        completed conversion round of the requested group
 *                        and stores the channel values starting at the
 *                        DataBufferPtr address. The group channel values are
 *                        stored in ascending channel number order (in contrast
 *                        to the storage layout of the result buffer if
 *                        streaming access is configured).
 *  \endverbatim
 */
FUNC(Std_ReturnType, ADC_CODE) Adc_ReadGroup(
            Adc_GroupType Group,
            Adc_ValueGroupType *DataBufferPtr);
#endif  /* #if (STD_ON == ADC_READ_GROUP_API) */

#if (STD_ON == ADC_GRP_NOTIF_CAPABILITY_API)
/**
 *  \brief Enables the notification mechanism for the requested ADC Channel
 *   group.
 *
 *  \verbatim
 *  Service name        : Adc_EnableGroupNotification
 *  Syntax              : void Adc_EnableGroupNotification( Adc_GroupType Group )
 *  Mode                : User Mode (Non-Privileged Mode)
 *  Service ID[hex]     : 0x07
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : Group - Numeric ID of requested ADC channel group.
 *  Parameters (inout)  : None
 *  Parameters (out)    : None
 *  Return value        : None
 *  Description         : Enables the notification mechanism for the requested
 *                        ADC Channel group.
 *  \endverbatim
 */
FUNC(void, ADC_CODE) Adc_EnableGroupNotification(Adc_GroupType Group);

/**
 *  \brief Disables the notification mechanism for the requested ADC Channel
 *   group.
 *
 *  \verbatim
 *  Service name        : Adc_DisableGroupNotification
 *  Syntax              : void Adc_DisableGroupNotification( Adc_GroupType Group )
 *  Mode                : User Mode (Non-Privileged Mode)
 *  Service ID[hex]     : 0x08
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : Group - Numeric ID of requested ADC channel group.
 *  Parameters (inout)  : None
 *  Parameters (out)    : None
 *  Return value        : None
 *  Description         : Disables the notification mechanism for the requested
 *                        ADC Channel group.
 *  \endverbatim
 */
FUNC(void, ADC_CODE) Adc_DisableGroupNotification(Adc_GroupType Group);
#endif  /* #if (STD_ON == ADC_GRP_NOTIF_CAPABILITY_API) */

#if (STD_ON == ADC_VERSION_INFO_API)
/**
 *  \brief This service returns the version information of this module.
 *
 *  \verbatim
 *  Service name        : Adc_GetVersionInfo
 *  Syntax              : void Adc_GetVersionInfo( Std_VersionInfoType* versioninfo )
 *  Mode                : User Mode (Non-Privileged Mode)
 *  Service ID[hex]     : 0x0A
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : None
 *  Parameters (inout)  : None
 *  Parameters (out)    : versioninfo - Pointer to where to store the version
 *                        information of this module
 *  Return value        : None
 *  Description         : Returns the version information of this module
 *  \endverbatim
 */
FUNC(void, ADC_CODE) Adc_GetVersionInfo(
        P2VAR(Std_VersionInfoType, AUTOMATIC, ADC_APPL_DATA) versioninfo);
#endif  /* #if (STD_ON == ADC_VERSION_INFO_API) */

#if (STD_ON == ADC_REGISTER_READBACK_API)
/**
 *  \brief This function reads the important registers of the hardware unit
 *  and returns the value in the structure.
 *
 *  This API should be called after Adc_Init is called. Otherwise this API will
 *  return E_NOT_OK.
 *
 *  This API could be used to readback the register contents after Adc_Init
 *  and then the readback value could be compared during ADC execution to
 *  check the correctness of the HW unit. Since this API is used for this
 *  purpose, the register returned are the ones which doesn't change after init
 *  based on conv or channel config.
 *
 *  \verbatim
 *  Service name        : Adc_RegisterReadback
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : HWUnit - ADC Hardware microcontroller peripheral
 *                        unit ID. If this is invalid, then the API will
 *                        return E_NOT_OK.
 *  Parameters (inout)  : RegRbPtr - Pointer to where to store the readback
 *                        values. If this pointer is NULL_PTR, then the API
 *                        will return E_NOT_OK.
 *  Return value        : Std_ReturnType
 *                        E_OK: Register read back has been done
 *                        E_NOT_OK: Register read back failed
 *  \endverbatim
 */
FUNC(Std_ReturnType, ADC_CODE) Adc_RegisterReadback(
    Adc_HWUnitType HWUnit,
    P2VAR(Adc_RegisterReadbackType, AUTOMATIC, ADC_APPL_DATA) RegRbPtr);
#endif  /* #if (STD_ON == ADC_REGISTER_READBACK_API) */

#if (STD_ON == ADC_INTERNAL_DIAGNOSTIC_DEBUG_MODE_API)
/**
 *  \brief This function enables the Functional Internal Diagnostic Debug mode
 *  and selects a known voltage source(REFP or VMID) that connects to the AFE.
 *
 *  \verbatim
 *  Service name        : Adc_EnableInternalDiagnostic
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : Group - Numeric ID of requested ADC channel group.
 *                        If this is invalid, then the API will return E_NOT_OK.
 *                        RefSelect - Reference select for functional internal
 *                        diagnostic debug mode.
 *                        0 = VMID
 *                        1 = REFP
 *  Parameters (inout)  : None
 *  Return value        : Std_ReturnType
 *                        E_OK: Functional Internal Diagnostic Debug Mode
 *                        has been enabled Successfully.
 *                        E_NOT_OK: Functional Internal Diagnostic Debug Mode
 *                        enable failed.
 *  \endverbatim
 */
FUNC(Std_ReturnType, ADC_CODE) Adc_EnableInternalDiagnostic(
    Adc_GroupType Group, Adc_RefSelType RefSelect);

/**
 *  \brief This function disables the Functional Internal Diagnostic Debug mode
 *
 *  \verbatim
 *  Service name        : Adc_DisableInternalDiagnostic
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (in)     : Group - Numeric ID of requested ADC channel group.
 *                        If this is invalid, then the API will return E_NOT_OK.
 *  Parameters (inout)  : None
 *  Return value        : Std_ReturnType
 *                        E_OK: Functional Internal Diagnostic Debug Mode
 *                        has been disabled Successfully.
 *                        E_NOT_OK: Functional Internal Diagnostic Debug Mode
 *                        disable failed.
 *  \endverbatim
 */
FUNC(Std_ReturnType, ADC_CODE) Adc_DisableInternalDiagnostic(
    Adc_GroupType Group);
#endif  /* #if (STD_ON == ADC_INTERNAL_DIAGNOSTIC_DEBUG_MODE_API) */

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef ADC_H_ */

/* @} */
