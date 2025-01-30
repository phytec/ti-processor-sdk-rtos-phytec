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
 *  \file     Adc_Priv.c
 *
 *  \brief    This file contains ADC MCAL driver internal functions
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "stdint.h"
#include "string.h"
#include "Adc_Cfg.h"
#include "Adc.h"
#include "Adc_Dbg.h"
#include "Adc_Priv.h"
#include <hw_include/soc.h>
#include "Os.h"
#include "Dem.h"
#include <hw_include/lldr_adc.h>

/* There are static inline functions in hw_types.h file. Map them as well */
#define ADC_START_SEC_CODE
#include "Adc_MemMap.h"
#include <hw_include/hw_types.h>
#define ADC_STOP_SEC_CODE
#include "Adc_MemMap.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/** \brief Total length of each FIFO in words */
#define ADC_FIFO_LENGTH                 (ADC_FIFOTHRESHOLD_FIFO_THR_LEVEL_MAX)

#if ADC_NUM_HW_CHANNEL > (ADC_FIFO_LENGTH >> 1U)
#error " Num of Channels cannot be greater than ADC_FIFO_LENGTH / 2 "
#endif

/** \brief As of now we are using only one FIFO of ADC */
#define ADC_FIFO_NUM                    ((uint32) 0U)

/** \brief Bit mask to clear all status bits */
#define ADC_IRQSTATUS_CLR_ALL       (ADC_IRQSTATUS_END_OF_SEQUENCE_MASK | \
                                     ADC_IRQSTATUS_FIFO0_THR_MASK |       \
                                     ADC_IRQSTATUS_FIFO0_OVERRUN_MASK |   \
                                     ADC_IRQSTATUS_FIFO0_UNDERFLOW_MASK | \
                                     ADC_IRQSTATUS_FIFO1_THR_MASK |       \
                                     ADC_IRQSTATUS_FIFO1_OVERRUN_MASK |   \
                                     ADC_IRQSTATUS_FIFO1_UNDERFLOW_MASK | \
                                     ADC_IRQSTATUS_OUT_OF_RANGE_MASK | \
                                     ADC_IRQSTATUS_AFE_EOC_MISSING_MASK)

/** \brief Bit mask to enable all required interrupts */
#define ADC_IRQENABLE_BITS          (ADC_IRQENABLE_SET_FIFO0_THR_MASK |     \
                                     ADC_IRQENABLE_SET_FIFO0_OVERRUN_MASK | \
                                     ADC_IRQENABLE_SET_FIFO0_UNDERFLOW_MASK)

/** \brief Bit mask to clear all interrupts */
#define ADC_IRQENABLE_CLR_ALL       (ADC_IRQENABLE_CLR_END_OF_SEQUENCE_MASK | \
                                     ADC_IRQENABLE_CLR_FIFO0_THR_MASK |       \
                                     ADC_IRQENABLE_CLR_FIFO0_OVERRUN_MASK |   \
                                     ADC_IRQENABLE_CLR_FIFO0_UNDERFLOW_MASK | \
                                     ADC_IRQENABLE_CLR_FIFO1_THR_MASK |       \
                                     ADC_IRQENABLE_CLR_FIFO1_OVERRUN_MASK |   \
                                     ADC_IRQENABLE_CLR_FIFO1_UNDERFLOW_MASK | \
                                     ADC_IRQENABLE_CLR_OUT_OF_RANGE_MASK | \
                                     ADC_IRQENABLE_CLR_AFE_EOC_MISSING_MASK)

/* Default register values programmed at init time */
#define ADC_IRQENABLE_CLR_DEFAULT       (0x000001FEU)   /* Disable IRQ */
#define ADC_DMAENABLE_CLR_DEFAULT       (0x00000003U)   /* Disable DMA */
#define ADC_CTRL_DEFAULT                (0x00000010U)   /* Power down */
#define ADC_ADCRANGE_DEFAULT            (0x00000000U)
#define ADC_MISC_DEFAULT                (0x00000000U)
#define ADC_STEPENABLE_DEFAULT          (0x00000000U)
#define ADC_STEPCONFIG_DEFAULT          (0x00000000U)
#define ADC_STEPDELAY_DEFAULT           (0x00000000U)
#define ADC_FIFOTHRESHOLD_DEFAULT       (0x00000000U)
#define ADC_DMAREQ_DEFAULT              (0x00000000U)

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */

static void Adc_scheduleGroup(Adc_GroupObjType *groupObj);
static void Adc_checkAndSchedule(Adc_HwUnitObjType *hwUnitObj);

static void Adc_procIsr(Adc_HwUnitObjType *hwUnitObj,
                        Adc_GroupObjType  *groupObj);
static void Adc_resetGroupObjOnResume(Adc_GroupObjType *groupObj);

static void Adc_hwConfig(const Adc_GroupObjType *groupObj, uint32 baseAddr);
static void Adc_hwStart(uint32 baseAddr);
static void Adc_hwStop(uint32 baseAddr);
static void Adc_hwFifoErrRecovery(Adc_GroupObjType *groupObj, uint32 baseAddr);
static void Adc_hwSetDefReg(uint32 baseAddr);
static void Adc_setGroupStatusPostIsr(Adc_HwUnitObjType *hwUnitObj,
                                      Adc_GroupObjType  *groupObj,
                                      uint32             convComplete,
                                      uint32             streamComplete);
static Std_ReturnType Adc_startGroupCheckStatus(
    const Adc_GroupObjType *groupObj);
static void Adc_waitForAfe(uint32 baseAddr);
static void Adc_waitForFsm(uint32 baseAddr);
static void Adc_delayExecution(TickType count);
static void Adc_waitForRamInitDone(uint32 baseAddr);

#if (STD_ON == ADC_DEV_ERROR_DETECT)
static Std_ReturnType Adc_checkGroupCfgRangeParameters(
    const Adc_GroupConfigType *groupCfg);
static void Adc_checkChannelParams(const Adc_ChannelConfigType *chCfg);
static Std_ReturnType Adc_checkGroupParameters(
    const Adc_GroupConfigType *groupCfg, const Adc_ConfigType *cfgPtr);
#endif  /* #if (STD_ON == ADC_DEV_ERROR_DETECT) */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

#if (STD_ON == ADC_GROUP_LOG)
#define ADC_START_SEC_VAR_NO_INIT_UNSPECIFIED
#include "Adc_MemMap.h"
/** \brief ADC group log object */
VAR(Adc_GroupLogType, ADC_VAR_CLEARED) Adc_GroupLogObj;
#define ADC_STOP_SEC_VAR_NO_INIT_UNSPECIFIED
#include "Adc_MemMap.h"
#endif  /* #if (STD_ON == ADC_GROUP_LOG) */

#if (STD_ON == ADC_FIFO_ERR_LOG)
#define ADC_START_SEC_VAR_NO_INIT_UNSPECIFIED
#include "Adc_MemMap.h"
/** \brief ADC FIFO error log object */
VAR(Adc_FifoErrLogType, ADC_VAR_CLEARED) Adc_FifoErrLogObj;
#define ADC_STOP_SEC_VAR_NO_INIT_UNSPECIFIED
#include "Adc_MemMap.h"
#endif  /* #if (STD_ON == ADC_FIFO_ERR_LOG) */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

#define ADC_START_SEC_CODE
#include "Adc_MemMap.h"

/*
 * Design: MCAL-5797,MCAL-5812,MCAL-5884
 */
void Adc_hwUnitInit(Adc_HwUnitObjType *hwUnitObj)
{
    uint32   baseAddr;
    uint32   spareInVal;
    TickType delayCount =
        (TickType) ADC_TIMEOUT_DURATION;

    /* Assign base address */
    hwUnitObj->baseAddr = hwUnitObj->hwUnitCfg.baseAddr;

    Adc_utilsInitLinkList(&hwUnitObj->groupList);
    /*
     * Init the HW instance
     */
    baseAddr = hwUnitObj->baseAddr;
    Adc_hwSetDefReg(baseAddr);

    /*
     * Power up Analog Front End
     */
    HW_WR_FIELD32(
        baseAddr + ADC_CTRL, ADC_CTRL_POWER_DOWN,
        ADC_CTRL_POWER_DOWN_AFEPOWERUP);

    /* Wait 4us at-least */
    Adc_delayExecution(delayCount);
    /*
     * Start internal calibration
     */
    spareInVal = ((uint32) 1U << ADC_MISC_AFE_SPARE_INPUT_SHIFT);
    HW_WR_FIELD32(baseAddr + ADC_MISC, ADC_MISC_AFE_SPARE_INPUT, spareInVal);

    Adc_waitForAfe(baseAddr);

    /* Set misc setup */
    HW_WR_FIELD32(
        baseAddr + ADC_CTRL,
        ADC_CTRL_STEP_ID_TAG,
        ADC_CTRL_STEP_ID_TAG_WRZERO);

    return;
}

/*
 * Design: MCAL-5792,MCAL-5847
 */
void Adc_hwUnitDeInit(Adc_HwUnitObjType *hwUnitObj)
{
    uint32 baseAddr;

    baseAddr = hwUnitObj->baseAddr;

    /*
     * De-Init the HW instance
     */
    Adc_hwSetDefReg(baseAddr);

    /* Power down analog Front End */
    HW_WR_FIELD32(
        baseAddr + ADC_CTRL, ADC_CTRL_POWER_DOWN,
        ADC_CTRL_POWER_DOWN_AFEPOWERDOWN);

    Adc_utilsDeInitLinkList(&hwUnitObj->groupList);

    return;
}

/*
 * Design: MCAL-5747,MCAL-5752,MCAL-5837,MCAL-5722,MCAL-5819,MCAL-5751,MCAL-5741,MCAL-5882,MCAL-5714,MCAL-5733,MCAL-5768,MCAL-5791,MCAL-5701
 */
Std_ReturnType Adc_startGroup(Adc_GroupObjType *groupObj)
{
    Std_ReturnType        retVal = (Std_ReturnType) E_OK;
    Adc_HwUnitObjType    *hwUnitObj;
    Adc_GroupObjType     *curGroupObj;
    Adc_GroupPriorityType effPriority;

    retVal = Adc_startGroupCheckStatus(groupObj);
    if (((Std_ReturnType) E_OK) == retVal)
    {
        hwUnitObj = groupObj->hwUnitObj;

        /* Check if we have any group already going on in hardware */
        if (NULL_PTR == hwUnitObj->curGroupObj)
        {
            /* HW is free, schedule this group */
            hwUnitObj->curGroupObj = groupObj;
            groupObj->groupStatus = ADC_BUSY;
            Adc_scheduleGroup(groupObj);
        }
        else
        {
            if (STD_OFF == ADC_ENABLE_QUEUING)
            {
                /* Queuing is disabled - return error */
                retVal = (Std_ReturnType) E_NOT_OK;
            }

            if (((Std_ReturnType) E_OK) == retVal)
            {
                if (ADC_PRIORITY_HW_SW == ADC_PRIORITY_IMPLEMENTATION)
                {
                    /* Use priority only when enabled */
                    effPriority = groupObj->groupCfg.groupPriority;
                }
                else
                {
                    /* Use same priority so that request are processed on first
                     * come first serve basis */
                    effPriority = 0U;
                }

                /* Check if this group's priority is more than current group's
                 * priority */
                curGroupObj = hwUnitObj->curGroupObj;
                if (effPriority > curGroupObj->groupCfg.groupPriority)
                {
                    /*
                     * This is high priority group. Stop the current group,
                     * push back to the group queue and schedule the new group
                     */

                    /* Stop current group and push back to hardware queue */
                    Adc_hwStop(hwUnitObj->baseAddr);
                    curGroupObj->isPaused = (uint32) TRUE;
                    curGroupObj->isQueued = (uint32) TRUE;
                    Adc_utilsLinkNodePri(
                        &hwUnitObj->groupList,
                        &curGroupObj->nodeObj,
                        curGroupObj,
                        curGroupObj->groupCfg.groupPriority,
                        curGroupObj->isPaused);

                    /* schedule the new group */
                    groupObj->groupStatus = ADC_BUSY;
                    hwUnitObj->curGroupObj = groupObj;
                    Adc_scheduleGroup(groupObj);
                }
                else
                {
                    /* Some high priority group already on-going - queue this to
                     * hardware queue */
                    groupObj->groupStatus = ADC_BUSY;
                    groupObj->isQueued = (uint32) TRUE;
                    Adc_utilsLinkNodePri(
                        &hwUnitObj->groupList,
                        &groupObj->nodeObj,
                        groupObj,
                        effPriority,
                        FALSE);
                }
            }
        }
    }

    return (retVal);
}

/*
 *Design: MCAL-5763,MCAL-5873,MCAL-5718,MCAL-5746,MCAL-5870,MCAL-5867
 */
void Adc_stopGroup(Adc_GroupObjType *groupObj)
{
    Adc_HwUnitObjType *hwUnitObj;

    hwUnitObj = groupObj->hwUnitObj;

    /* Check if on-going conversion is same as this group */
    if (groupObj == hwUnitObj->curGroupObj)
    {
        /* Stop ADC */
        Adc_hwStop(hwUnitObj->baseAddr);
        hwUnitObj->curGroupObj = NULL;

        /* Check and schedule any pending groups for this HW */
        Adc_checkAndSchedule(hwUnitObj);
    }
    else
    {
        /* Just remove request from the hardware queue */
        Adc_utilsUnLinkNodePri(&hwUnitObj->groupList, &groupObj->nodeObj);
        groupObj->isQueued = (uint32) FALSE;
    }

    /* Disable group notification */
    groupObj->isNotifyOn = (uint32) FALSE;
    /* Set status to IDLE */
    groupObj->groupStatus = ADC_IDLE;

    return;
}

void Adc_IrqTxRx(Adc_HwUnitObjType *hwUnitObj)
{
    uint32 baseAddr, irqStatus;
    Adc_GroupObjType *groupObj;

    baseAddr = hwUnitObj->baseAddr;
    groupObj = hwUnitObj->curGroupObj;

    /* Clear interrupts immediately before handling events. This should avoid
     * missing the same interrupts again while handling the events */
    irqStatus = HW_RD_REG32(baseAddr + ADC_IRQSTATUS);
    HW_WR_REG32(baseAddr + ADC_IRQSTATUS, irqStatus);

    /* Check if FIFO error interrupts */
    if (((irqStatus & ADC_IRQSTATUS_FIFO0_OVERRUN_MASK) != 0U) ||
        ((irqStatus & ADC_IRQSTATUS_FIFO0_UNDERFLOW_MASK) != 0U))
    {
        /*
         * Recover from FIFO overflow or underflow
         */
        Adc_hwFifoErrRecovery(groupObj, baseAddr);
    }
    /* Check if FIFO threshold interrupt */
    else if ((irqStatus & ADC_IRQSTATUS_FIFO0_THR_MASK) != 0U)
    {
        /*
         * Process the group
         */
        if (NULL_PTR != groupObj)
        {
            Adc_procIsr(hwUnitObj, groupObj);
        }

        /* ADC_CoverageGap_01: Dynamic branch coverage for the above 'if' statement is not covered
         * for False condn because this interrupt is called only when a valid group object
         * is doing the conversion. The 'if' statement is added as an additional safety
         * mechanism.
         */
    }
    else
    {
        /* No other event */
        /*
         * Note: This could happen when we stop ADC and an interrupt
         * happened between interrupt disable and executing ADC stop
         * instruction. After we stop ADC, we are clearing all interrupt
         * status. So no status bits set means, this condition!!
         * Even though interrupts are cleared, we still get the interrupt
         * which was raised to the host processor. So just ignore this
         * interrupt
         */
    }

    /* Set EOI so that ADC can re-evaluate any new interrupts */
    HW_WR_REG32(baseAddr + ADC_IRQ_EOI, ADC_IRQ_EOI_LINE_NUMBER_EOI);

    return;
}

/**
 *  \brief Returns the the HW object for the corresponding HW unit ID
 */
Adc_HwUnitObjType *Adc_getHwUnitObj(Adc_HWUnitType HWUnit)
{
    uint8             hwUnitIdx;
    Adc_HwUnitObjType *hwObj  = (Adc_HwUnitObjType *) NULL_PTR;
    Adc_DriverObjType *drvObj = &Adc_DrvObj;

    /* HW unit ID may not be the index, so search for matching HW unit */
    for (hwUnitIdx = 0U; hwUnitIdx < drvObj->maxHwUnit; hwUnitIdx++)
    {
        if (drvObj->hwUnitObj[hwUnitIdx].hwUnitCfg.hwUnitId == HWUnit)
        {
            hwObj = &drvObj->hwUnitObj[hwUnitIdx];
            break; /* Match found */
        }
    }

    if (hwObj == NULL_PTR)
    {
#if (STD_ON == ADC_DEV_ERROR_DETECT)
        Adc_reportDetError(ADC_SID_INIT, ADC_E_PARAM_CONFIG);
#endif  /* #if (STD_ON == ADC_DEV_ERROR_DETECT) */
    }

    return (hwObj);
}

/*
 * Design: MCAL-5854,MCAL-5808,MCAL-5892
 */
void Adc_resetDrvObj(Adc_DriverObjType *drvObj)
{
    uint8          hwUnitIdx, grpIdx;
    uint32         chIdx;
    Adc_UtilsNode *nodeObj;

    for (grpIdx = 0U; grpIdx < ADC_MAX_GROUP; grpIdx++)
    {
        drvObj->groupObj[grpIdx].groupStatus        = ADC_IDLE;
        drvObj->groupObj[grpIdx].isExplicitStopMode = (uint32) FALSE;
        drvObj->groupObj[grpIdx].isNotifyOn         = (uint32) FALSE;
        drvObj->groupObj[grpIdx].isPaused           = (uint32) FALSE;
        drvObj->groupObj[grpIdx].isQueued           = (uint32) FALSE;
        drvObj->groupObj[grpIdx].validSampleCount   = 0U;
        drvObj->groupObj[grpIdx].curCh        = 0U;
        drvObj->groupObj[grpIdx].resultBufPtr =
            NULL;
        for (chIdx = 0U; chIdx < ADC_NUM_CHANNEL; chIdx++)
        {
            drvObj->groupObj[grpIdx].chObj[chIdx].chResultBufPtr =
                NULL;
            drvObj->groupObj[grpIdx].chObj[chIdx].curResultBufPtr =
                NULL;
            drvObj->groupObj[grpIdx].chObj[chIdx].curNumSamples = 0U;
        }

        nodeObj = &drvObj->groupObj[grpIdx].nodeObj;
        Adc_utilsInitNodeObject(nodeObj);
    }
    for (hwUnitIdx = 0U; hwUnitIdx < ADC_MAX_HW_UNIT; hwUnitIdx++)
    {
        drvObj->hwUnitObj[hwUnitIdx].curGroupObj =
            NULL;
    }
    drvObj->maxGroup  = 0U;
    drvObj->maxHwUnit = 0U;

#if (STD_ON == ADC_GROUP_LOG)
    (void) memset(&Adc_GroupLogObj, 0, sizeof (Adc_GroupLogObj));
#endif  /* #if (STD_ON == ADC_GROUP_LOG) */
#if (STD_ON == ADC_FIFO_ERR_LOG)
    (void) memset(&Adc_FifoErrLogObj, 0, sizeof (Adc_FifoErrLogObj));
#endif  /* #if (STD_ON == ADC_FIFO_ERR_LOG) */

    return;
}

void Adc_copyConfig(Adc_DriverObjType *drvObj, const Adc_ConfigType *cfgPtr)
{
    uint8 hwUnitIdx, grpIdx;
    const Adc_HwUnitConfigType *hwCfg;
    Adc_GroupObjType           *groupObj;
    const Adc_GroupConfigType  *groupCfg;
#if (STD_ON == ADC_DEV_ERROR_DETECT)
    uint32         chIdx;
    const Adc_ChannelConfigType *chCfg;
    Std_ReturnType retVal;
#endif  /* #if (STD_ON == ADC_DEV_ERROR_DETECT) */

    drvObj->maxGroup  = cfgPtr->maxGroup;
    drvObj->maxHwUnit = cfgPtr->maxHwUnit;
    for (hwUnitIdx = 0U; hwUnitIdx < cfgPtr->maxHwUnit; hwUnitIdx++)
    {
        hwCfg = &cfgPtr->hwUnitCfg[hwUnitIdx];

        /* Copy configuration */
        (void) memcpy(
            &drvObj->hwUnitObj[hwUnitIdx].hwUnitCfg,
            hwCfg,
            sizeof (Adc_HwUnitConfigType));
    }

    for (grpIdx = 0U; grpIdx < cfgPtr->maxGroup; grpIdx++)
    {
        groupCfg = &cfgPtr->groupCfg[grpIdx];
        groupObj = &drvObj->groupObj[grpIdx];
#if (STD_ON == ADC_DEV_ERROR_DETECT)
        retVal = Adc_checkGroupParameters(groupCfg, cfgPtr);
        if (((Std_ReturnType) E_OK) == retVal)
#endif      /* #if (STD_ON == ADC_DEV_ERROR_DETECT) */
        {
            /* Copy configuration */
                (void) memcpy(
                &groupObj->groupCfg,
                groupCfg,
                sizeof (Adc_GroupConfigType));

            /* Store HW object pointer for easy reference */
            groupObj->hwUnitObj = &Adc_DrvObj.hwUnitObj[groupCfg->hwUnitId];

            /* All modes except single shot and linear streaming needs to be
             * stopped explicitly */
            groupObj->isExplicitStopMode = (uint32) TRUE;
            if ((ADC_CONV_MODE_ONESHOT == groupCfg->convMode) ||
                ((ADC_ACCESS_MODE_STREAMING == groupCfg->accessMode) &&
                 (ADC_STREAM_BUFFER_LINEAR == groupCfg->streamBufMode)))
            {
                groupObj->isExplicitStopMode = (uint32) FALSE;
            }

#if (STD_ON == ADC_DEV_ERROR_DETECT)
            /* Check channel configuration */
            for (chIdx = 0U; chIdx < groupCfg->numChannels; chIdx++)
            {
                chCfg = &groupCfg->channelConfig[chIdx];
                Adc_checkChannelParams(chCfg);
            }
#endif      /* #if (STD_ON == ADC_DEV_ERROR_DETECT) */
        }
    }

    return;
}

uint32 Adc_checkHwUnitObj(const Adc_ConfigType *cfgPtr)
{
    uint8 hwUnitIdx = 0U;
    const Adc_HwUnitConfigType *hwCfg;
    uint32 found = (uint32) FALSE;

    for (hwUnitIdx = 0U; hwUnitIdx < cfgPtr->maxHwUnit; hwUnitIdx++)
    {
        hwCfg = &cfgPtr->hwUnitCfg[hwUnitIdx];
        if (hwCfg->hwUnitId < ADC_MAX_HW_UNIT)
        {
            found = (uint32) TRUE;
            break;      /* Match found */
        }
    }
    return (found);
}

/*
 * Design:  MCAL-5759,MCAL-5713,MCAL-5720
 */
static Std_ReturnType Adc_startGroupCheckStatus(
    const Adc_GroupObjType *groupObj)
{
    Std_ReturnType retVal = (Std_ReturnType) E_OK;

    if (((uint32) TRUE) == groupObj->isExplicitStopMode)
    {
        /* For explicit stop mode, group in idle status can be started */
        if (ADC_IDLE != groupObj->groupStatus)
        {
            /* Group is already in progress - return error */
            retVal = (Std_ReturnType) E_NOT_OK;
        }
    }
    else
    {
        /* For implicit stop mode, group in idle or stream completed status
         * can be started */
        if ((ADC_IDLE != groupObj->groupStatus) &&
            (ADC_STREAM_COMPLETED != groupObj->groupStatus))
        {
            /* Group is already in progress - return error */
            retVal = (Std_ReturnType) E_NOT_OK;
        }
    }

    return retVal;
}

/* check channel parameters */
#if (STD_ON == ADC_DEV_ERROR_DETECT)
static void Adc_checkChannelParams(const Adc_ChannelConfigType *chCfg)
{
    if ((chCfg->hwChannelId > ADC_MAX_HW_CHANNEL_ID) ||
        (chCfg->openDelay > ADC_MAX_OPEN_DELAY) ||
        (chCfg->sampleDelay > ADC_MAX_SAMPLE_DELAY) ||
        (chCfg->averagingMode > ADC_AVERAGING_MODE_16_SAMPLES))
    {
        Adc_reportDetError(ADC_SID_INIT, ADC_E_PARAM_CONFIG);
    }

    return;
}

#endif

#if (STD_ON == ADC_DEV_ERROR_DETECT)
static Std_ReturnType Adc_checkGroupParameters(
    const Adc_GroupConfigType *groupCfg, const Adc_ConfigType *cfgPtr)
{
    Std_ReturnType retVal = (Std_ReturnType) E_OK;
    /* ID is used as index, can't exceed array size */
    if (groupCfg->groupId >= cfgPtr->maxGroup)
    {
        retVal = (Std_ReturnType) E_NOT_OK;
        Adc_reportDetError(ADC_SID_INIT, ADC_E_PARAM_CONFIG);
    }
    /* check group parameters */
    else if ((groupCfg->streamNumSamples == 0U) ||
             ((ADC_ACCESS_MODE_SINGLE == groupCfg->accessMode) &&
              (groupCfg->streamNumSamples != 1U)) ||
             ((ADC_CONV_MODE_ONESHOT == groupCfg->convMode) &&
              (groupCfg->accessMode != ADC_ACCESS_MODE_SINGLE)))
    {
        retVal = (Std_ReturnType) E_NOT_OK;
        Adc_reportDetError(ADC_SID_INIT, ADC_E_PARAM_CONFIG);

        /* ADC_CoverageGap_02: Dynamic MC/DC coverage for the above 'if' statement is not covered 100%
         * because MC/DC cannot be achieved for C5. The only combination to achieve
		     * 100% coverage is when both C2 and C5 are either True or False and this
		     * is not possible as the condition C2 and C5 cannot be True or False
		     * at the same time.
         */
    }
    else if (NULL_PTR == Adc_getHwUnitObj(groupCfg->hwUnitId))
    {
        /* DET already reported by Adc_getHwUnitObj() */
        retVal = (Std_ReturnType) E_NOT_OK;
    }
    else
    {
        retVal = Adc_checkGroupCfgRangeParameters(groupCfg);
    }

    return retVal;
}

#endif  /* #if (STD_ON == ADC_DEV_ERROR_DETECT) */

#if (STD_ON == ADC_DEV_ERROR_DETECT)
static Std_ReturnType Adc_checkGroupCfgRangeParameters(
    const Adc_GroupConfigType *groupCfg)
{
    Std_ReturnType retVal = (Std_ReturnType) E_OK;

    if ((0U == groupCfg->numChannels) ||
        (groupCfg->numChannels > ADC_NUM_CHANNEL) ||
        (groupCfg->resolution != ADC_DEF_CHANNEL_RESOLUTION) ||
        (groupCfg->highRange > ADC_MAX_RANGE) ||
        (groupCfg->lowRange > ADC_MAX_RANGE) ||
        (groupCfg->lowRange > groupCfg->highRange))
    {
        retVal = (Std_ReturnType) E_NOT_OK;
        Adc_reportDetError(ADC_SID_INIT, ADC_E_PARAM_CONFIG);
    }

    return retVal;
}

#endif  /* #if (STD_ON == ADC_DEV_ERROR_DETECT) */

/*
 * Design: MCAL-5738,MCAL-5802,MCAL-5831
 */
static void Adc_scheduleGroup(Adc_GroupObjType *groupObj)
{
    uint32 chIdx;
    Adc_HwUnitObjType     *hwUnitObj;
    Adc_ValueGroupType    *tempResultBufPtr;

#if (STD_ON == ADC_GROUP_LOG)
    Adc_GroupLogEntryType *logEntry;
    uint32 timeStamp;
    StatusType status;
#endif  /* #if (STD_ON == ADC_GROUP_LOG) */

    hwUnitObj = groupObj->hwUnitObj;

    if (((uint32) TRUE) == groupObj->isPaused)
    {
        /* For both type we always start with first channel and discard any
         * intermediate results */
        if ((ADC_GROUP_REPL_ABORT_RESTART ==
             groupObj->groupCfg.groupReplacement) ||
            (ADC_GROUP_REPL_SUSPEND_RESUME ==
             groupObj->groupCfg.groupReplacement))
        {
            Adc_resetGroupObjOnResume(groupObj);
        }
    }
    else
    {
        /* Set the states for normal start */
        groupObj->validSampleCount = 0U;
        groupObj->curCh  = 0U;
        tempResultBufPtr = (Adc_ValueGroupType *)groupObj->resultBufPtr;
        for (chIdx = 0U; chIdx < groupObj->groupCfg.numChannels; chIdx++)
        {
            groupObj->chObj[chIdx].chResultBufPtr  = tempResultBufPtr;
            groupObj->chObj[chIdx].curResultBufPtr = tempResultBufPtr;
            groupObj->chObj[chIdx].curNumSamples   = 0U;

            /* Move to next channel buffer */
            tempResultBufPtr += groupObj->groupCfg.streamNumSamples;
        }
    }

#if (STD_ON == ADC_GROUP_LOG)
    /* Log the group */
    logEntry =
        &Adc_GroupLogObj.logEntry[Adc_GroupLogObj.logIndex];

    status = GetCounterValue(ADC_OS_COUNTER_ID, &timeStamp);
    if (((StatusType) E_OK) == status)
    {
        logEntry->timeStamp = timeStamp;
        logEntry->groupId   = groupObj->groupCfg.groupId;
        logEntry->hwUnitId      = hwUnitObj->hwUnitCfg.hwUnitId;
        logEntry->isReScheduled = groupObj->isPaused;

        /* Move to next entry */
        Adc_GroupLogObj.logIndex++;
        Adc_GroupLogObj.totalLog++;
        if (Adc_GroupLogObj.logIndex >= ADC_MAX_GROUP_LOG)
        {
            Adc_GroupLogObj.logIndex = 0U;
        }
    }
#endif  /* #if (STD_ON == ADC_GROUP_LOG) */

        groupObj->isPaused = (uint32) FALSE;    /* Reset flag after log */

        /* Configure ADC steps and start conversion */
        Adc_hwConfig(groupObj, hwUnitObj->baseAddr);
        Adc_hwStart(hwUnitObj->baseAddr);

    return;
}

static void Adc_checkAndSchedule(Adc_HwUnitObjType *hwUnitObj)
{
    Adc_UtilsNode    *headNodeObj;
    Adc_GroupObjType *nextGroupObj;

    /* Check if any new group is pending */
    headNodeObj = Adc_utilsGetHeadNode(&hwUnitObj->groupList);
    if (NULL_PTR != headNodeObj)
    {
        /* Some group pending - schedule it */
        nextGroupObj = (Adc_GroupObjType *) headNodeObj->data;
        Adc_utilsUnLinkNodePri((&(hwUnitObj->groupList)), headNodeObj);
        nextGroupObj->isQueued = (uint32) FALSE;
        hwUnitObj->curGroupObj = nextGroupObj;
        Adc_scheduleGroup(nextGroupObj);
    }

    return;
}

/*
 * Design: MCAL-5888,MCAL-5761,MCAL-5857,MCAL-5860,MCAL-5719,MCAL-5753,MCAL-3234,MCAL-5762
 */
static void Adc_procIsr(Adc_HwUnitObjType *hwUnitObj,
                        Adc_GroupObjType  *groupObj)
{
    uint32 baseAddr;
    uint32 convComplete   = (uint32) FALSE;
    uint32 streamComplete = (uint32) FALSE;
    uint32 numSamples, data;
    Adc_ChannelObjType *chObj;

    baseAddr = hwUnitObj->baseAddr;

    /* Get FIFO count */
    numSamples = HW_RD_REG32(baseAddr + ADC_FIFOCOUNT(ADC_FIFO_NUM));

    /* Read all samples in the FIFO */
    while (numSamples > 0U)
    {
        data = HW_RD_REG32(baseAddr + ADC_FIFODATA(ADC_FIFO_NUM));

        chObj = &groupObj->chObj[groupObj->curCh];
        *chObj->curResultBufPtr = (data & ADC_FIFODATA_ADCDATA_MASK);

        /* Move to next buffer pointer for the channel */
        chObj->curNumSamples++;
        chObj->curResultBufPtr++;
        if ((chObj->curResultBufPtr) >=
            (chObj->chResultBufPtr + (groupObj->groupCfg.streamNumSamples)))
        {
            chObj->curNumSamples   = 0U;
            chObj->curResultBufPtr = chObj->chResultBufPtr;
        }

        /* Move to next channel */
        groupObj->curCh++;
        if (groupObj->curCh >= groupObj->groupCfg.numChannels)
        {
            /* All channel conversion completed */
            groupObj->curCh = 0U;
            /* Increment valid count and limit to stream max samples */
            groupObj->validSampleCount++;
            if (groupObj->validSampleCount >
            ((uint32) groupObj->groupCfg.streamNumSamples))
            {
                /* This happens if app fails to read data faster than driver
                 * generates */
                groupObj->validSampleCount =
                    (uint32) groupObj->groupCfg.streamNumSamples;
            }

            /* Check if all streams are completed - check last channel pointer.
             * If it is at start then stream completed */
            convComplete = (uint32) TRUE;
            if (chObj->curResultBufPtr == chObj->chResultBufPtr)
            {
                streamComplete = (uint32) TRUE;
                /* Incase of one-shot/linear stream, stop conversion and
                 * don't update the buffers - break from reading the FIFO and
                 * storing data */
                if (((uint32) FALSE) == groupObj->isExplicitStopMode)
                {
                    break;
                }
            }
        }

        numSamples--;
    }

    /* Set Group Status and Call group end notification */
    Adc_setGroupStatusPostIsr(hwUnitObj, groupObj, convComplete, streamComplete);

    return;
}

static void Adc_setGroupStatusPostIsr(Adc_HwUnitObjType *hwUnitObj,
                                      Adc_GroupObjType  *groupObj,
                                      uint32             convComplete,
                                      uint32             streamComplete)
{
    Adc_GroupEndNotifyType groupEndNotification = NULL;
    if (((uint32) TRUE) == convComplete)
    {
        /* Set group status */
        if (((uint32) TRUE) == streamComplete)
        {
            /* Conversion over for all channels and all streams */
            groupObj->groupStatus = ADC_STREAM_COMPLETED;
        }
        else
        {
            /* Atleast one set of conversion over for all channels */
            groupObj->groupStatus = ADC_COMPLETED;
        }

        /* Call group end notification */
        if ((((uint32) TRUE) == groupObj->isNotifyOn) &&
            ((Adc_GroupEndNotifyType)NULL_PTR !=
                groupObj->groupCfg.Adc_GroupEndNotification))
        {
            groupEndNotification =
                groupObj->groupCfg.Adc_GroupEndNotification;
            groupEndNotification();

            /* ADC_CoverageGap_03: Dynamic MC/DC coverage for the above 'if' statement is not covered 100%
             * because MC/DC cannot be achieved for C2. The only combination to achieve
		         * 100% coverage is when C1 is True and C2 is False and this is not possible
			       * because if isNotifyOn is true then groupCfg.Adc_GroupEndNotification is
		         * assigned to a callback fxn pointer.
             */
        }

        /* Stop ADC in case of one-shot/linear stream */
        if ((((uint32) TRUE) == streamComplete) &&
            (((uint32) FALSE) == groupObj->isExplicitStopMode))
        {
            Adc_hwStop(hwUnitObj->baseAddr);
            hwUnitObj->curGroupObj = NULL;

            /* Check and schedule any pending groups for this HW */
            Adc_checkAndSchedule(hwUnitObj);
        }
    }

    return;
}

static void Adc_resetGroupObjOnResume(Adc_GroupObjType *groupObj)
{
    uint32 chIdx;

    /* When we re-start, the ADC will start conversion from 0th chan, so
     * un-do previous conversion if half-way through */
    for (chIdx = 0U; chIdx < groupObj->curCh; chIdx++)
    {
        if (groupObj->chObj[chIdx].curNumSamples > 0U)
        {
            groupObj->chObj[chIdx].curNumSamples--;
            groupObj->chObj[chIdx].curResultBufPtr--;

            /* Handle wrap around */

            if ((groupObj->chObj[chIdx].curResultBufPtr) <
                (groupObj->chObj[chIdx].chResultBufPtr))
            {
                /* We were at start pointer, so move to last pointer */
                groupObj->chObj[chIdx].curResultBufPtr =
                    groupObj->chObj[chIdx].chResultBufPtr +
                    (groupObj->groupCfg.streamNumSamples - 1U);

                /* ADC_CoverageGap_04: Dynamic Code coverage for this statement is not covered
                 * because curResultBufPtr cannot be less than chResultBufPtr.
    				     * This 'if statement' is added as an additional safety mechanism.
                 */
            }
        }
    }

    /* Set the Current channel to 0 */
    groupObj->curCh = 0U;

    return;
}

static void Adc_hwConfig(const Adc_GroupObjType *groupObj, uint32 baseAddr)
{
    uint32 stepId, stepEnable;
    uint32 threshold;
    const Adc_GroupConfigType   *groupCfg;
    const Adc_ChannelConfigType *chCfg;

    groupCfg = &groupObj->groupCfg;

    /* Set range value */
    HW_WR_FIELD32(
        baseAddr + ADC_ADCRANGE,
        ADC_ADCRANGE_HIGH_RANGE_DATA,
        groupCfg->highRange);
    HW_WR_FIELD32(
        baseAddr + ADC_ADCRANGE,
        ADC_ADCRANGE_LOW_RANGE_DATA,
        groupCfg->lowRange);

    /* Set threshold to number of channels so that we get interrupt after
     * every conversion of all channels */
    threshold = groupCfg->numChannels;

    /* Program one minus */
    HW_WR_FIELD32(
        baseAddr + ADC_FIFOTHRESHOLD(ADC_FIFO_NUM),
        ADC_FIFOTHRESHOLD_FIFO_THR_LEVEL,
        (threshold - 1U));

    /* Configure all steps */
    stepEnable = 0U;                /* Disable all steps */
    for (stepId = 0U; stepId < groupCfg->numChannels; stepId++)
    {
        chCfg = &groupCfg->channelConfig[stepId];

        stepEnable |= ((uint32) 0x1U << (stepId + ADC_STEPENABLE_STEP1_SHIFT));

        if (ADC_CONV_MODE_ONESHOT == groupCfg->convMode)
        {
            HW_WR_FIELD32(
                baseAddr + ADC_STEPCONFIG(stepId),
                ADC_STEPCONFIG_MODE,
                ADC_STEPCONFIG_MODE_SW_EN_ONESHOT);
        }
        else
        {
            HW_WR_FIELD32(
                baseAddr + ADC_STEPCONFIG(stepId),
                ADC_STEPCONFIG_MODE,
                ADC_STEPCONFIG_MODE_SW_EN_CONTINUOUS);
        }
        HW_WR_FIELD32(
            baseAddr + ADC_STEPCONFIG(stepId),
            ADC_STEPCONFIG_SEL_INP_SWC,
            chCfg->hwChannelId);
        HW_WR_FIELD32(
            baseAddr + ADC_STEPCONFIG(stepId),
            ADC_STEPCONFIG_AVERAGING,
            (uint32) chCfg->averagingMode);
        HW_WR_FIELD32(
            baseAddr + ADC_STEPCONFIG(stepId),
            ADC_STEPCONFIG_RANGE_CHECK,
            chCfg->rangeCheckEnable);
        HW_WR_FIELD32(
            baseAddr + ADC_STEPCONFIG(stepId),
            ADC_STEPCONFIG_FIFO_SELECT,
            ADC_FIFO_NUM);

        HW_WR_FIELD32(
            baseAddr + ADC_STEPDELAY(stepId),
            ADC_STEPDELAY_OPENDELAY,
            chCfg->openDelay);
        HW_WR_FIELD32(
            baseAddr + ADC_STEPDELAY(stepId),
            ADC_STEPDELAY_SAMPLEDELAY,
            chCfg->sampleDelay);
    }

    /* Enable the required steps */
    HW_WR_REG32(baseAddr + ADC_STEPENABLE, stepEnable);

    return;
}

static void Adc_hwStart(uint32 baseAddr)
{
    /* Clear all previous interrupt status */
    HW_WR_REG32(baseAddr + ADC_IRQSTATUS, ADC_IRQSTATUS_CLR_ALL);

    /* Enable required interrupts */
    HW_WR_REG32(baseAddr + ADC_IRQENABLE_SET, ADC_IRQENABLE_BITS);

    /* Wait for RAM Init done */
    Adc_waitForRamInitDone(baseAddr);

    /* Start ADC */
    HW_WR_FIELD32(baseAddr + ADC_CTRL, ADC_CTRL_EN, ADC_CTRL_EN_ENABLE);

    /*
     * Wait for interrupts!!
     */

    return;
}

static void Adc_hwStop(uint32 baseAddr)
{
    /* Clear all steps */
    HW_WR_REG32(baseAddr + ADC_STEPENABLE, 0x00U);

    /* Wait for FSM to become idle */
    Adc_waitForFsm(baseAddr);

    /* Stop ADC */
    HW_WR_FIELD32(baseAddr + ADC_CTRL, ADC_CTRL_EN, ADC_CTRL_EN_DISABLE);

    /* Wait for FSM to become idle */
    Adc_waitForFsm(baseAddr);

    /* Disable all enabled interrupts */
    HW_WR_REG32(baseAddr + ADC_IRQENABLE_CLR, ADC_IRQENABLE_CLR_ALL);

    /* Clear all pending interrupt status */
    HW_WR_REG32(baseAddr + ADC_IRQSTATUS, ADC_IRQSTATUS_CLR_ALL);

    return;
}

static void Adc_hwFifoErrRecovery(Adc_GroupObjType *groupObj, uint32 baseAddr)
{
#if (STD_ON == ADC_FIFO_ERR_LOG)
    Adc_FifoErrLogEntryType *logEntry;
    uint32 timeStamp;
    StatusType status;
#endif  /* #if (STD_ON == ADC_FIFO_ERR_LOG) */

    /* Stop ADC - this will clear the FIFO */
    Adc_hwStop(baseAddr);

    /* if no active group, clear only the HW event, no SW update needed */
    if (NULL_PTR != groupObj)
    {
#if (STD_ON == ADC_FIFO_ERR_LOG)
        /* Log the group */
        logEntry =
            &Adc_FifoErrLogObj.logEntry[Adc_FifoErrLogObj.logIndex];
    status = GetCounterValue(ADC_OS_COUNTER_ID, &timeStamp);
    if (((StatusType) E_OK) == status)
    {
        logEntry->timeStamp = timeStamp;
        logEntry->groupId   = groupObj->groupCfg.groupId;
        logEntry->hwUnitId  = groupObj->hwUnitObj->hwUnitCfg.hwUnitId;

        /* Move to next entry */
        Adc_FifoErrLogObj.logIndex++;
        Adc_FifoErrLogObj.totalLog++;
        if (Adc_FifoErrLogObj.logIndex >= ADC_MAX_FIFO_ERR_LOG)
        {
            Adc_FifoErrLogObj.logIndex = 0U;
        }
    }
#endif  /* #if (STD_ON == ADC_FIFO_ERR_LOG) */

        Adc_resetGroupObjOnResume(groupObj);

        /* Configure ADC steps and start conversion */
        Adc_hwConfig(groupObj, baseAddr);
        /* Start ADC - this will start from channel 0 */
        Adc_hwStart(baseAddr);
    }

    return;
}

static void Adc_hwSetDefReg(uint32 baseAddr)
{
    uint32 index;

    /*
     * Init all registers to default value
     */
    HW_WR_REG32(baseAddr + ADC_CTRL, ADC_CTRL_DEFAULT);
    HW_WR_REG32(baseAddr + ADC_IRQENABLE_CLR, ADC_IRQENABLE_CLR_DEFAULT);
    HW_WR_REG32(baseAddr + ADC_DMAENABLE_CLR, ADC_DMAENABLE_CLR_DEFAULT);
    HW_WR_REG32(baseAddr + ADC_ADCRANGE, ADC_ADCRANGE_DEFAULT);
    HW_WR_REG32(baseAddr + ADC_MISC, ADC_MISC_DEFAULT);
    HW_WR_REG32(baseAddr + ADC_STEPENABLE, ADC_STEPENABLE_DEFAULT);
    for (index = 0U; index < ADC_STEPCONFIG_NUM_ELEMS; index++)
    {
        HW_WR_REG32(baseAddr + ADC_STEPCONFIG(index), ADC_STEPCONFIG_DEFAULT);
        HW_WR_REG32(baseAddr + ADC_STEPDELAY(index), ADC_STEPDELAY_DEFAULT);
    }
    for (index = 0U; index < ADC_FIFOCOUNT_NUM_ELEMS; index++)
    {
        HW_WR_REG32(
            baseAddr + ADC_FIFOTHRESHOLD(index), ADC_FIFOTHRESHOLD_DEFAULT);
    }
    for (index = 0U; index < ADC_DMAREQ_NUM_ELEMS; index++)
    {
        HW_WR_REG32(baseAddr + ADC_DMAREQ(index), ADC_DMAREQ_DEFAULT);
    }
    /* Clear all previous interrupt status */
    HW_WR_REG32(baseAddr + ADC_IRQSTATUS, ADC_IRQSTATUS_CLR_ALL);

    return;
}

static void Adc_waitForAfe(uint32 baseAddr)
{
    uint32     afeBusy;
    StatusType status;
    TickType   startCount = 0U, tempCount = 0U, elaspsedCount = 0U;

    status = GetCounterValue(ADC_OS_COUNTER_ID, &startCount);
    if (((StatusType) E_OK) == status)
    {
        /* Wait for calibration to end */
        do
        {
            /* Below API can change start time, so use temp variable */
            tempCount = startCount;
            status    = GetElapsedValue(
                ADC_OS_COUNTER_ID,
                &tempCount,
                &elaspsedCount);
            if ((((StatusType) E_OK) != status) ||
                (elaspsedCount >= ADC_TIMEOUT_DURATION))
            {
#if (ADC_E_HARDWARE_ERROR != ADC_DEM_NO_EVENT)
                (void)Dem_SetEventStatus(ADC_E_HARDWARE_ERROR,
                    DEM_EVENT_STATUS_FAILED);
#endif
                break;
            }
            afeBusy =
                HW_RD_FIELD32(baseAddr + ADC_ADCSTAT, ADC_ADCSTAT_AFE_BUSY);
        } while (ADC_ADCSTAT_AFE_BUSY_IDLE != afeBusy);  /* Calibration done */
        /* ADC_CoverageGap_05: Dynamic branch coverage for True condn of while statement is not covered
         * because the hardware Calibration ends before checking while condition.
         */
    }

    return;
}

static void Adc_waitForFsm(uint32 baseAddr)
{
    uint32     fsmBusy;
    StatusType status;
    TickType   startCount = 0U, tempCount = 0U, elaspsedCount = 0U;

    status = GetCounterValue(ADC_OS_COUNTER_ID, &startCount);
    if (((StatusType) E_OK) == status)
    {
        /* Wait for FSM to become idle */
        do
        {
            /* Below API can change start time, so use temp variable */
            tempCount = startCount;
            status    = GetElapsedValue(
                ADC_OS_COUNTER_ID,
                &tempCount,
                &elaspsedCount);
            if ((((StatusType) E_OK) != status) ||
                (elaspsedCount >= ADC_TIMEOUT_DURATION))
            {
                /* timeout */
#if (ADC_E_HARDWARE_ERROR != ADC_DEM_NO_EVENT)
                (void)Dem_SetEventStatus(ADC_E_HARDWARE_ERROR,
                    DEM_EVENT_STATUS_FAILED);
#endif
                break;
            }
            fsmBusy =
                HW_RD_FIELD32(baseAddr + ADC_ADCSTAT, ADC_ADCSTAT_FSM_BUSY);
        } while (ADC_ADCSTAT_FSM_BUSY_IDLE != fsmBusy);  /* FSM Idle */
    }

    return;
}

static void Adc_waitForRamInitDone(uint32 baseAddr)
{
    uint32     memInitDone;
    StatusType status;
    TickType   startCount = 0U, tempCount = 0U, elaspsedCount = 0U;

    status = GetCounterValue(ADC_OS_COUNTER_ID, &startCount);
    if (((StatusType) E_OK) == status)
    {
        /* Wait for calibration to end */
        do
        {
            /* Below API can change start time, so use temp variable */
            tempCount = startCount;
            status    = GetElapsedValue(
                ADC_OS_COUNTER_ID,
                &tempCount,
                &elaspsedCount);
            if ((((StatusType) E_OK) != status) ||
                (elaspsedCount >= ADC_TIMEOUT_DURATION))
            {
#if (ADC_E_HARDWARE_ERROR != ADC_DEM_NO_EVENT)
                (void)Dem_SetEventStatus(ADC_E_HARDWARE_ERROR,
                    DEM_EVENT_STATUS_FAILED);
#endif
                break;
            }
            memInitDone =
                HW_RD_FIELD32(baseAddr + ADC_ADCSTAT, ADC_ADCSTAT_MEM_INIT_DONE);
        } while (ADC_ADCSTAT_MEM_INIT_DONE != memInitDone);  /* Ram Init done */
        /* ADC_CoverageGap_06: Dynamic branch coverage for True condn of while statement is not covered
         * because the hardware Calibration ends before checking while condition.
         */
    }

    return;
}

static void Adc_delayExecution(TickType count)
{
    volatile TickType lpCnt = 0U;

    while (lpCnt < count)
    {
        lpCnt++;
    }
}

#if (STD_ON == ADC_DEV_ERROR_DETECT)
void Adc_reportDetError(uint8 apiId, uint8 errorId)
{
    (void) Det_ReportError(ADC_MODULE_ID, ADC_INSTANCE_ID, apiId, errorId);
    return;
}

#endif  /* #if (STD_ON == ADC_DEV_ERROR_DETECT) */

void Adc_reportDetRuntimeError(uint8 apiId, uint8 errorId)
{
    (void) Det_ReportRuntimeError(ADC_MODULE_ID, ADC_INSTANCE_ID, apiId, errorId);
    return;
}

#define ADC_STOP_SEC_CODE
#include "Adc_MemMap.h"
