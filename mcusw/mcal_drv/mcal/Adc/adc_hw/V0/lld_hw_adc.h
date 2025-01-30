/*
 *   Copyright (c) Texas Instruments Incorporated 2024
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
 *  \ingroup CSL_IP_MODULE
 *  \defgroup CSL_ADC ADC
 *
 *  @{
 */
/**
 *  \file     adc.h
 *
 *  \brief    This file contains the prototypes of the APIs present in the
 *            device abstraction layer file of ADC.
 *            This also contains some related macros.
 */

#ifndef LLD_ADC_H_
#define LLD_ADC_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <hw_include/lldr_adc.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
/**
 * \brief  Macro defines mask for all the interrupts status for ADC.
 */
#define ADC_INTR_STATUS_ALL     (ADC_IRQSTATUS_END_OF_SEQUENCE_MASK | \
                                 ADC_IRQSTATUS_FIFO0_THR_MASK |       \
                                 ADC_IRQSTATUS_FIFO0_OVERRUN_MASK |   \
                                 ADC_IRQSTATUS_FIFO0_UNDERFLOW_MASK | \
                                 ADC_IRQSTATUS_FIFO1_THR_MASK |       \
                                 ADC_IRQSTATUS_FIFO1_OVERRUN_MASK |   \
                                 ADC_IRQSTATUS_FIFO1_UNDERFLOW_MASK | \
                                 ADC_IRQSTATUS_OUT_OF_RANGE_MASK)
/**
 * \brief  Macro defines mask for all the interrupts to enable for ADC.
 */
#define ADC_INTR_ENABLE_ALL     (ADC_IRQENABLE_SET_END_OF_SEQUENCE_MASK | \
                                 ADC_IRQENABLE_SET_FIFO0_THR_MASK |       \
                                 ADC_IRQENABLE_SET_FIFO0_OVERRUN_MASK |   \
                                 ADC_IRQENABLE_SET_FIFO0_UNDERFLOW_MASK | \
                                 ADC_IRQENABLE_SET_FIFO1_THR_MASK |       \
                                 ADC_IRQENABLE_SET_FIFO1_OVERRUN_MASK |   \
                                 ADC_IRQENABLE_SET_FIFO1_UNDERFLOW_MASK | \
                                 ADC_IRQENABLE_SET_OUT_OF_RANGE_MASK)

/**
 * \brief  Macro defines mask for all the interrupts to disable for ADC.
 */
#define ADC_INTR_DISABLE_ALL    (ADC_IRQENABLE_CLR_END_OF_SEQUENCE_MASK | \
                                 ADC_IRQENABLE_CLR_FIFO0_THR_MASK |       \
                                 ADC_IRQENABLE_CLR_FIFO0_OVERRUN_MASK |   \
                                 ADC_IRQENABLE_CLR_FIFO0_UNDERFLOW_MASK | \
                                 ADC_IRQENABLE_CLR_FIFO1_THR_MASK |       \
                                 ADC_IRQENABLE_CLR_FIFO1_OVERRUN_MASK |   \
                                 ADC_IRQENABLE_CLR_FIFO1_UNDERFLOW_MASK | \
                                 ADC_IRQENABLE_CLR_OUT_OF_RANGE_MASK)

/**
 * \brief  Macro defines mask for all the interrupts for ADC.
 */
#define ADC_INTR_ALL             (ADC_INTR_STATUS_ALL)

/**
 * \brief  Macro defines the maximum value of open delay for a step for ADC.
 */
#define ADC_OPENDELAY_MAX               (0x3FFFFU)

/**
 * \brief  Macro defines the maximum value of sample delay for a step for ADC.
 */
#define ADC_SAMPLEDELAY_MAX             (0xFFU)

/**
 * \brief  Macro defines the maximum allowable range for ADC.
 */
#define ADC_RANGE_MAX                   (0x3FFU)

/**
 * \brief  Macro defines the minimum allowable range for ADC.
 */
#define ADC_RANGE_MIN                   (0x0U)

/**
 * \brief  Macro defines the maximum number of samples FIFO0 can store.
 */
#define ADC_FIFO_SIZE                  (64U)

/**
 * \brief  Macro defines the maximum number of steps in ADC.
 */
#define ADC_RESLN_MAX                   (1024U)
/**
 * \brief  Macro defines the maximum number of channels in ADC.
 */
#define ADC_MAX_NUM_CHN                 (uint32)(8U)
/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */
/**
 *  \anchor adcChannel_t
 *  \name ADC Fifo Num
 *  @{
 */
/**
 * \brief  Enum to select the channel for input.
 *
 *         ADC have 8 channels from which it can take input.
 */
typedef uint32 adcChannel_t;

#define ADC_CHANNEL_1                       (ADC_STEPCONFIG_SEL_INM_SWC_CHANNEL_1)
/**< Select channel 1 for input */
#define ADC_CHANNEL_2                       (ADC_STEPCONFIG_SEL_INM_SWC_CHANNEL_2)
/**< Select channel 2 for input */
#define ADC_CHANNEL_3                       (ADC_STEPCONFIG_SEL_INM_SWC_CHANNEL_3)
/**< Select channel 3 for input */
#define ADC_CHANNEL_4                       (ADC_STEPCONFIG_SEL_INM_SWC_CHANNEL_4)
/**< Select channel 4 for input */
#define ADC_CHANNEL_5                       (ADC_STEPCONFIG_SEL_INM_SWC_CHANNEL_5)
/**< Select channel 5 for input */
#define ADC_CHANNEL_6                       (ADC_STEPCONFIG_SEL_INM_SWC_CHANNEL_6)
/**< Select channel 6 for input */
#define ADC_CHANNEL_7                       (ADC_STEPCONFIG_SEL_INM_SWC_CHANNEL_7)
/**< Select channel 7 for input */
#define ADC_CHANNEL_8                       (ADC_STEPCONFIG_SEL_INM_SWC_CHANNEL_8)
/**< Select channel 8 for input */
/* @} */

/**
 *  \anchor adcStepId_t
 *  \name ADC Step Id
 *  @{
 */
/**
 * \brief  Enum to select the step for operation.
 */
typedef uint32 adcStepId_t;

#define ADC_STEP_1                          (ADC_ADCSTAT_STEP_ID_STEP1)
/**< Select step 1 */
#define ADC_STEP_2                          (ADC_ADCSTAT_STEP_ID_STEP2)
/**< Select step 2 */
#define ADC_STEP_3                          (ADC_ADCSTAT_STEP_ID_STEP3)
/**< Select step 3 */
#define ADC_STEP_4                          (ADC_ADCSTAT_STEP_ID_STEP4)
/**< Select step 4 */
#define ADC_STEP_5                          (ADC_ADCSTAT_STEP_ID_STEP5)
/**< Select step 5 */
#define ADC_STEP_6                          (ADC_ADCSTAT_STEP_ID_STEP6)
/**< Select step 6 */
#define ADC_STEP_7                          (ADC_ADCSTAT_STEP_ID_STEP7)
/**< Select step 7 */
#define ADC_STEP_8                          (ADC_ADCSTAT_STEP_ID_STEP8)
/**< Select step 8 */
#define ADC_STEP_9                          (ADC_ADCSTAT_STEP_ID_STEP9)
/**< Select step 9 */
#define ADC_STEP_10                         (ADC_ADCSTAT_STEP_ID_STEP10)
/**< Select step 10 */
#define ADC_STEP_11                         (ADC_ADCSTAT_STEP_ID_STEP11)
/**< Select step 11 */
#define ADC_STEP_12                         (ADC_ADCSTAT_STEP_ID_STEP12)
/**< Select step 12 */
#define ADC_STEP_13                         (ADC_ADCSTAT_STEP_ID_STEP13)
/**< Select step 13 */
#define ADC_STEP_14                         (ADC_ADCSTAT_STEP_ID_STEP14)
/**< Select step 14 */
#define ADC_STEP_15                         (ADC_ADCSTAT_STEP_ID_STEP15)
/**< Select step 15 */
#define ADC_STEP_16                         (ADC_ADCSTAT_STEP_ID_STEP16)
/**< Select step 16 */
/* @} */

/**
 *  \anchor adcOperationMode_t
 *  \name ADC Operation Mode
 *  @{
 */
/**
 * \brief  Enum to select the ADC Operation Mode.
 *
 *         ADC can either operate in software enabled single shot mode or
 *         software enabled continuous mode.
 */
typedef uint32 adcOperationMode_t;

#define ADC_OPERATION_MODE_SINGLE_SHOT      (ADC_STEPCONFIG_MODE_SW_EN_ONESHOT)
/**< Configure ADC operation mode to software enabled single shot */
#define ADC_OPERATION_MODE_CONTINUOUS       (ADC_STEPCONFIG_MODE_SW_EN_CONTINUOUS)
/**< Configure ADC operation mode to software enabled continuous */
/* @} */

/**
 *  \anchor adcAveraging_t
 *  \name ADC Averaging
 *  @{
 */
/**
 * \brief  Enum to number of samplings to average.
 *
 *         ADC allows user to program the number of samplings to average.
 */
typedef uint32 adcAveraging_t;

#define ADC_AVERAGING_NONE                  (ADC_STEPCONFIG_AVERAGING_NOAVG)
/**< Disable averaging */
#define ADC_AVERAGING_2_SAMPLES             (ADC_STEPCONFIG_AVERAGING_2_SAMPLESAVG)
/**< Set number of samplings to average to 2 */
#define ADC_AVERAGING_4_SAMPLES             (ADC_STEPCONFIG_AVERAGING_4_SAMPLESAVG)
/**< Set number of samplings to average to 4 */
#define ADC_AVERAGING_8_SAMPLES             (ADC_STEPCONFIG_AVERAGING_8_SAMPLESAVG)
/**< Set number of samplings to average to 8 */
#define ADC_AVERAGING_16_SAMPLES            (ADC_STEPCONFIG_AVERAGING_16_SAMPLESAV)
/**< Set number of samplings to average to 16 */
/* @} */

/**
 *  \anchor adcFIFONum_t
 *  \name ADC Fifo Num
 *  @{
 */
/**
 * \brief  Enum to select FIFO to store the data.
 */
typedef uint32 adcFIFONum_t;

#define ADC_FIFO_NUM_0                      (0x0U)
/**< Select FIFO 0 to store data */
#define ADC_FIFO_NUM_1                      (0x1U)
/**< Select FIFO 1 to store data */
/* @} */

/**
 *  \anchor adcIntrSrc_t
 *  \name ADC Interrupt source
 *  @{
 */
/**
 * \brief  Enum for ADC interrupts.
 */
typedef uint32 adcIntrSrc_t;

#define ADC_INTR_SRC_END_OF_SEQUENCE        (ADC_IRQENABLE_SET_END_OF_SEQUENCE_MASK)
/**< End of sequence interrupt */
#define ADC_INTR_SRC_FIFO0_THRESHOLD        (ADC_IRQENABLE_SET_FIFO0_THR_MASK)
/**< FIFO0 threshold interrupt */
#define ADC_INTR_SRC_FIFO0_OVERRUN          (ADC_IRQENABLE_SET_FIFO0_OVERRUN_MASK)
/**< FIFO0 overrun interrupt */
#define ADC_INTR_SRC_FIFO0_UNDERFLOW        (ADC_IRQENABLE_SET_FIFO0_UNDERFLOW_MASK)
/**< FIFO0 underrun interrupt */
#define ADC_INTR_SRC_FIFO1_THRESHOLD        (ADC_IRQENABLE_SET_FIFO1_THR_MASK)
/**< FIFO1 threshold interrupt */
#define ADC_INTR_SRC_FIFO1_OVERRUN          (ADC_IRQSTATUS_FIFO1_OVERRUN_MASK)
/**< FIFO1 overrun interrupt */
#define ADC_INTR_SRC_FIFO1_UNDERFLOW        (ADC_IRQENABLE_SET_FIFO1_UNDERFLOW_MASK)
/**< FIFO1 underrun interrupt */
#define ADC_INTR_SRC_OUT_OF_RANGE           (ADC_IRQENABLE_SET_OUT_OF_RANGE_MASK)
/**< Out of interrupt */
/* @} */

/**
 *  \anchor adcIdleMode_t
 *  \name ADC Idle mode
 *  @{
 */
/**
 * \brief  Enum to configure ADC idle mode.Applicable for TDA3XX Only.
 */
typedef uint32 adcIdleMode_t;

#define ADC_IDLE_MODE_FORCE_IDLE            (ADC_SYSCONFIG_IDLEMODE_FORCE)
/**< Force idle mode */
#define ADC_IDLE_MODE_NO_IDLE               (ADC_SYSCONFIG_IDLEMODE_NO_IDLE)
/**< No idle mode */
#define ADC_IDLE_MODE_SMART_IDLE            (ADC_SYSCONFIG_IDLEMODE_SMART_IDLE)
/**< Smart idle mode */
/* @} */

/**
 * \brief  Structure for accessing Revision ID of ADC module.
 */
typedef struct adcRevisionId
{
    uint32 scheme;
    /**< Scheme */
    uint32 func;
    /**< Functional number */
    uint32 rtlRev;
    /**< RTL revision */
    uint32 major;
    /**< Major revision */
    uint32 custom;
    /**< Custom revision */
    uint32 minor;
    /**< Minor revision */
}adcRevisionId_t;

/**
 * \brief  Structure containing parameters for ADC step configuration.
 */
typedef struct adcStepConfig
{
    uint32 mode;
    /**< Mode of operation.
     *  Refer enum #adcOperationMode_t.
     */
    uint32 channel;
    /**< Channel number from which input is given.
     *  Refer enum #adcChannel_t.
     */
    uint32 openDelay;
    /**< Delay between driving the inputs and the time
     *  to send the start of conversion signal.
     *  It is number of clock cycles of ADC clock and can take values from 0 to
     *  ADC_OPENDELAY_MAX.
     */
    uint32 sampleDelay;
    /**< Sampling time/width of the start of conversion signal.
     *  It is number of clock cycles of ADC clock and can take values from 1 to
     *  ADC_SAMPLEDELAY_MAX.
     */
    uint32 rangeCheckEnable;
    /**< Option to enable range check.
     *   Enabled if it is TRUE
     *   Disabled if it is FALSE.
     */
    uint32 averaging;
    /**< Option for averaging the sampled data.
     *  Refer enum #adcAveraging_t.
     */
    uint32 fifoNum;
    /**< FIFO to be used to store the data.
     *  Refer enum #adcFIFONum_t.
     */
}adcStepConfig_t;

/**
 * \brief  Structure for reporting ADC sequencer status.
 */
typedef struct adcSequencerStatus
{
    uint32 afeBusy;
    /**< AFE status
     *  0 - Idle
     *  1 - Busy
     */
    uint32 fsmBusy;
    /**< FSM status
     *  0 - Idle
     *  1 - Busy(conversion in progress) */
    uint32 stepId;
    /**< Step ID in progress
     *  Refer enum #adcStepId_t.
     */
}adcSequencerStatus_t;

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */

#ifdef __cplusplus
}

#endif /*extern "C" */

#endif
 /** @} */
