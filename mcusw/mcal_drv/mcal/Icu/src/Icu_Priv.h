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
 *  \file     Icu_Priv.h
 *
 *  \brief    This file contains the common declarations and macros used across
 *            all the ICU MCAL driver files.
 *
 */

#ifndef ICU_PRIV_H_
#define ICU_PRIV_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "Icu.h"

#include <hw_include/lldr_ecap.h>
#include <hw_include/soc.h>
#include <hw_include/lld_ecap.h>
#include <hw_include/hw_types.h>
#include <hw_include/lldr.h>
#include "Det.h"
#ifdef __cplusplus
extern "C"
{
#endif
/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

#define ICUTIMER_MAX_RESOLUTION    (0xFFFFFFFFU)
#define ICUTICKMAXVALUE            (0xF0000000U)

#define ECAP_INT_ALL                    (ECAP_CEVT1_INT  | \
                                         ECAP_CEVT2_INT  | \
                                         ECAP_CEVT3_INT  | \
                                         ECAP_CEVT4_INT  | \
                                         ECAP_CNTOVF_INT | \
                                         ECAP_PRDEQ_INT  | \
                                         ECAP_CMPEQ_INT)
#define ECAP_INT_ALLCAPS                (ECAP_CEVT1_INT  | \
                                         ECAP_CEVT2_INT  | \
                                         ECAP_CEVT3_INT  | \
                                         ECAP_CEVT4_INT)

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/** \brief Icu configuration structure internal to driver */
typedef struct
{
#if (ICU_TIMESTAMP_API == STD_ON)
    Icu_ValueType* NextTimeStampIndexPtr;
    /**< Next time stamp index Pointer for timestamp API */
    uint32 NextTimeStampIndex;
    /**< Next time stamp index for timestamp API */
    uint16 TimeStampBufferSize;
    /**< Time Stamp buffer size as input from user */
    uint16 NotifyInterval;
    /**< Notify Iterval number as input from user */
    uint16 NotificationCounter;
    /**< Notification counter to compare with NotifyInterval */
    boolean IsActive;
    /**< Set to true when timestamp API is executing */
#endif
#if (ICU_SIGNAL_MEASUREMENT_API == STD_ON)
    uint32 cap1;
    /**< Value from capture register 1 */
    uint32 cap2;
    /**< Value from capture register 2 */
    uint32 cap3;
    /**< Value from capture register 3 */
    uint32 cap4;
    /**< Value from capture register 4 */
    Icu_DutyCycleType DutyCycle;
    /**< Duty Cycle */
    Icu_ValueType HighTime;
    /**< Signal High Time */
    Icu_ValueType LowTime;
    /**< Signal Low Time */
    Icu_ValueType Period;
    /**< Signal period */
    boolean DutyAcquired;
    /**< Set to true when Duty cycle values have been acquired */
    boolean PeriodAcquired;
    /**< Set to true when Period values have been acquired */
#endif
    Icu_InputStateType InputState;
    /**< Variable for input state of module */
#if (ICU_EDGE_COUNT_API == STD_ON)
    Icu_EdgeNumberType EdgeCounter;
    /**< Counter for edge count */
#endif
    Icu_ActivationType activation_edge;
    /**< Activation edge type for module*/
    boolean IsRunning;
    /**< Set to true when operation in in progress */
    boolean NotificationEnabled;
    /**< Set to true when notification are enabled */
    uint32                baseAddr;
    /**< Base address of the ICU Channel being used*/
    Icu_ChannelPrescalerType          prescaler;
    /**< Prescaler value */
    Icu_ChannelConfigType chCfg;
    /**< Channel configurations */
    Icu_ChannelConfigType_PC chCfg_PC;
    /**< ChannelId configuration in PC */
} Icu_ChObjType;

/**
 *  \brief This type defines Counter Reset Mode
 */
typedef enum
{
   ICU_ABSOLUTE_MODE,
   /**< Don't Reset counter upon counter event*/
   ICU_DELTA_MODE
   /**< Reset upon counter capture event */
} Icu_CounterRstType;

extern Icu_ChObjType Icu_ChObj[ICU_MAX_NUM_CHANNELS];
/* Icu_ChObj is not static storage class, as this requires to be accessed by
    debug interface provided.  */
/* ========================================================================== */
/*                         GLOBAL VARIABLES                                   */
/* ========================================================================== */

extern volatile uint8 Icu_DrvStatus;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 * \brief   This API will reset ICU s/w channel object.
 *
 * \param   chObj       Pointer to channel object.
 *
 * \return  None.
 *
 **/
void Icu_ResetChObj(Icu_ChObjType *chObj);

/**
 * \brief   This API will copy all channel configuration into local ICU Object
 *
 * \param   chObj      Pointer to driver object.
 * \param   CfgPtr     Pointer to configuration object sent by application.
 * \param   CfgPtr_PC  Pointer to configuration object Pre_Comp sent by application.
 *
 * \return  None.
 *
 **/
void Icu_CopyConfig(Icu_ChObjType *chObj, const Icu_ChannelConfigType * chCfg,
                    const Icu_ChannelConfigType_PC * chCfg_pc);

/**
 * \brief   This API will initialize ICU channel object associated with logical
 *          channel.
 *
 * \param   chObj      Pointer to channel object.
 *
 * \return  None.
 *
 **/
void Icu_HwUnitInit(Icu_ChObjType *chObj);

#if (STD_ON == ICU_SIGNAL_MEASUREMENT_API)
/**
 * \brief   This API will Clears the Signal Measurment API Structure
 *
 * \param   Channel      ICU Channel in Use
 *
 * \return  None.
 *
 **/
void Icu_SignalMeasurement_Init(Icu_ChannelType Channel);
#endif

#if (STD_ON == ICU_TIMESTAMP_API)
/**
 * \brief   This API will Init the Timestamp API Structure
 *
 * \param   Channel         ICU Channel in Use
 * \param   StartPtr        Start of the Buffer ptr
 * \param   Size            Size of the Buffer to use
 * \param   NotifyInterval  Notify Inerval
 *
 * \return  None.
 *
 **/
void Icu_TimeStamp_Init(Icu_ChannelType Channel, Icu_ValueType* StartPtr, uint16 Size, uint16 NotifyInterval);
/**
 * \brief   This API will be called by ISR for timestamp API
 *
 * \param   Channel         ICU Channel in Use
 *
 * \return  None.
 *
 **/
void Icu_Timestamp_ISR (Icu_ChannelType Channel);
/**
 * \brief   This API will clear the timestamp structure
 *
 * \param   Channel         ICU Channel in Use
 *
 * \return  None.
 *
 **/
void Icu_TimeStamp_Clear(Icu_ChannelType Channel);
#endif

/**
 * \brief   This API will get the base address of the ICU .
 *
 * \param   Channel       Channel number
 *
 * \return  None.
 *
 **/
uint32 Icu_GetBaseAddr(Icu_ChannelType Channel);

/**
 * \brief   This API will configure the ECAP module
 *
 * \param   baseAddr      Baseaddress of channel
 * \param   activation    Activation Edge type
 *
 * \return  None.
 *
 **/
void Icu_ConfigEcap(uint32 baseAddr, Icu_ActivationType activation, Icu_CounterRstType cntRst, boolean interruptEnable);


/**
 * \brief   This API will report Det Error
 *
 * \param   apiId       api id
            errorId     error id
 *
 * \return
 *
 **/
#if (STD_ON == ICU_DEV_ERROR_DETECT)
void Icu_reportDetError(uint8 apiId, uint8 errorId);
#endif /*ICU_DEV_ERROR_DETECT*/

/**
 * \brief   This API will report Det Runtime Error
 *
 * \param   apiId       api id
            errorId     error id
 *
 * \return
 *
 **/
void Icu_reportDetRuntimeError(uint8 apiId, uint8 errorId);

/**
 * \brief   This is the Channel ISR
 *
 * \param   Channel       Channel number
 *
 * \return
 *
 **/
void Icu_ChannelISR(Icu_ChannelType ChannelNumber);

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef ICU_PRIV_H_ */
