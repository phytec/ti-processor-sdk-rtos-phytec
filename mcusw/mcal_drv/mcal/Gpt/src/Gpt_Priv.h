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
 *  \file     Gpt_Priv.h
 *
 *  \brief    This file contains the common declarations and macros used across
 *            all the GPT MCAL driver files.
 *
 */

#ifndef GPT_PRIV_H_
#define GPT_PRIV_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "string.h"

#include "Gpt.h"
#include "Gpt_Irq.h"
#include "SchM_Gpt.h"
#include "Det.h"

#ifdef __cplusplus
extern "C"
{
#endif
/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**< \brief maximum timer resolution */
#define GPTIMER_MAX_RESOLUTION    0xFFFFFFFFU

/**< \brief Index for ISR function array
 *   This will be used to call notify or wakeup function from ISR function
 *   table Gpt_IsrFxn
 *   Note: Using function table instead of direct function table in case more
 *         functions are added in future.
 */
#define GPT_CH_MODE_NOTIFY    0U
#define GPT_CH_MODE_WAKEUP    1U

/**< \brief Driver status UN INITIALIZED */
#define GPT_DRV_UNINTIALIZED    ((uint32) 0U)
/**< \brief Driver status UN INITIALIZED */
#define GPT_DRV_INTIALIZED      ((uint32) 1U)

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/** \brief  List of process modes  */
typedef enum
{
    GPT_TIMERTYPE_GPTIMER,
    /**< GPT general purpose timer */
    GPT_TIMERTYPE_INVALID,
    /**< GPT Invalid timer */
}Gpt_TimerType;

/**
 *  \brief GPT driver object structure.
 */
typedef struct
{
    Gpt_ChannelConfigType channelCfg;
    /**<  HW Channel Configuration passed during init */
    Gpt_ChannelConfigType_PC channelCfg_PC;
    /**<  HW Channel Pre Compile Configuration passed during init */
    uint32                baseAddr;
    /**< Base address of the hardware */
    Gpt_TimerType         timerType;
    /**< Timer type of channel */
    uint32                channelIsrFnIdx;
    /**<  Variable to store Isr function pointer(wakeup function or notify
     * function)
     */
} Gpt_ChannelHwObjType;

/**
 *  \brief GPT driver object structure.
 */
typedef struct
{
    Gpt_ChannelHwObjType channelObj[GPT_MAX_CHANNELS];
    /**<  GPT hw unit objects */
#if (STD_ON == GPT_WAKEUP_FUNCTIONALITY_API)
    Gpt_ModeType         sleepMode;
    /**< Variable to indicate the operation mode of the driver */
#endif
    Gpt_ChannelStateType channelState[GPT_MAX_CHANNELS];
    /**< Variable to indicate the state of the channel */
    uint32               channelNotifyActiveFlag[GPT_MAX_CHANNELS];
    /**<  Variable indicate the notification status for each channel */
    uint32               channelWakeupEnFlag[GPT_MAX_CHANNELS];
    /**<  Variable indicate the wakeup status for each channel */
    uint32               channelCount;
    /**<  Number of channels configured  */
    uint32               channelIdMap[GPT_CH_GPTIMER_MAX];
    /**< Maps channelId to instance of timer, used for debug only */
} Gpt_DriverObjType;

/* ========================================================================== */
/*                         GLOBAL VARIABLES                                   */
/* ========================================================================== */

/** \brief The Index for the ISR_Function Array*/
extern CONST(Gpt_IsrRefType, GPT_CONST) Gpt_IsrFxn[2U];
extern volatile uint32 Gpt_DrvStatus;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

#if (STD_ON == GPT_DEV_ERROR_DETECT)
void Gpt_reportDetError(uint8 apiId, uint8 errorId);
#endif  /* #if (STD_ON == GPT_DEV_ERROR_DETECT) */
void Gpt_reportRuntimeError(uint8 apiId, uint8 errorId);
Gpt_ChannelHwObjType *Gpt_getHwUnitObj(Gpt_ChannelType gptChannel,
                                       uint32         *chIdx);

void Gpt_gptimerInit(uint32 gptChannel, Gpt_ChannelHwObjType *pHwUnitObj);
void Gpt_gptimerDeInit(const Gpt_ChannelHwObjType *pHwUnitObj);
uint32 Gpt_gptimerCounterGet(uint32 baseAddr);
uint32 Gpt_gptimerReloadGet(uint32 baseAddr);
void Gpt_gptimerStart(const Gpt_ChannelHwObjType *pHwUnitObj,
                        uint32 countValue,
                        uint32 enableInt);
void Gpt_gptimerStop(const Gpt_ChannelHwObjType *pHwUnitObj);
void Gpt_gptimerEnableNotify(const Gpt_ChannelHwObjType *pHwUnitObj);
void Gpt_gptimerEnableWakeup(const Gpt_ChannelHwObjType *pHwUnitObj);
void Gpt_gptimerDisableWakeup(const Gpt_ChannelHwObjType *pHwUnitObj);
void Gpt_gptimerDisableNotify(const Gpt_ChannelHwObjType *pHwUnitObj);
void Gpt_gptimerIntrClear(Gpt_ChannelType gptChannel);
void Gpt_notifyIsr(uint32 chIdx);
void Gpt_wakeupIsr(uint32 chIdx);
void Gpt_gptimerIntDisableCh(Gpt_ChannelType gptChannel);
/* ========================================================================== */
/*                       Static Function Definitions                          */
/* ========================================================================== */

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef GPT_PRIV_H_ */
