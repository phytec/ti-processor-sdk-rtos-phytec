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
 *  \file   Icu_Priv.c
 *
 *  \brief  This file contains the Internal APIs for ICU.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "Icu_Priv.h"
#include "string.h"
#include "Icu_Cfg.h"
/* ========================================================================== */
/*                                Macros                                      */
/* ========================================================================== */

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */

/*None*/

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

#define ICU_START_SEC_CODE
#include "Icu_MemMap.h"

void Icu_ResetChObj(Icu_ChObjType *chObj)
{
#if (ICU_TIMESTAMP_API == STD_ON)
    chObj->NextTimeStampIndexPtr = (Icu_ValueType *) NULL_PTR;
    chObj->NextTimeStampIndex = 0U;
    chObj->TimeStampBufferSize = 0U;
    chObj->NotifyInterval = 0U;
    chObj->NotificationCounter = 0U;
    chObj->IsActive = FALSE;
#endif
#if (ICU_SIGNAL_MEASUREMENT_API == STD_ON)
    chObj->cap1 = 0U;
    chObj->cap2 = 0U;
    chObj->cap3 = 0U;
    chObj->cap4 = 0U;
    chObj->HighTime = 0U;
    chObj->LowTime = 0U;
    chObj->Period = 0U;
    chObj->DutyCycle.ActiveTime = (Icu_ValueType)0U;
    chObj->DutyCycle.PeriodTime = (Icu_ValueType)0U;
    chObj->PeriodAcquired = FALSE;
    chObj->DutyAcquired = FALSE;
#endif
    chObj->InputState = (Icu_InputStateType)ICU_IDLE;
#if (ICU_EDGE_COUNT_API == STD_ON)
    chObj->EdgeCounter = (Icu_EdgeNumberType) 0U;
#endif
    chObj->activation_edge = ICU_BOTH_EDGES;
    chObj->IsRunning = FALSE;
    chObj->NotificationEnabled = FALSE;
    chObj->baseAddr = 0U;
    chObj->prescaler = 0U;
    chObj->chCfg.defaultStartEdge = ICU_BOTH_EDGES;
    chObj->chCfg.measurementMode = ICU_MODE_SIGNAL_MEASUREMENT;
    chObj->chCfg.signalMeasurementProperty = ICU_PERIOD_TIME;
    chObj->chCfg.notificationHandler = (Icu_NotifyFuncType) NULL_PTR;
    chObj->chCfg.bufferType = ICU_LINEAR_BUFFER;
    chObj->chCfg.instanceClkMHz = 0U;
    chObj->chCfg.prescaler = 0U;
    chObj->chCfg_PC.channelId = 0U;
    return;
}

void Icu_CopyConfig(
    Icu_ChObjType *chObj, const Icu_ChannelConfigType *chCfg,
    const Icu_ChannelConfigType_PC *chCfg_pc)
{
    Icu_ChannelType channelNumber;
    /* Copying the Config stucture to Channel Object*/
    (void)memcpy(&chObj->chCfg, &chCfg[0U], sizeof(Icu_ChannelConfigType));

    /*Copying the pre-compile config structure to Channel Object */
    chObj->chCfg_PC.channelId = chCfg_pc->channelId;
    chObj->activation_edge = chCfg->defaultStartEdge;
    channelNumber = chObj->chCfg_PC.channelId;
    chObj->prescaler = chCfg->prescaler;
    /* Store base address of all the configured channels */
    chObj->baseAddr = Icu_GetBaseAddr(channelNumber);
    return;
}

FUNC(uint32, ICU_CODE) Icu_GetBaseAddr(
    Icu_ChannelType ChannelNumber)
{
    uint32 baseAddr;

#if (ICU_DEV_ERROR_DETECT == STD_ON)
    if (ChannelNumber >= (uint32) ICU_MAX_NUM_CHANNELS)
    {
        (void) Icu_reportDetError(ICU_INIT_ID, ICU_E_PARAM_CHANNEL);
        baseAddr = 0U;
    }
    else
#endif
    {
        baseAddr = Icu_HwUnitBaseAddr[ChannelNumber];
    }
    return (baseAddr);
}

void Icu_HwUnitInit(Icu_ChObjType *chObj)
{
    uint32 baseAddr;
    baseAddr = chObj->baseAddr;

    /*Disable and Clear Interrupts*/
    ECAPIntDisable(baseAddr, ECAP_INT_ALL);
    ECAPIntStatusClear(baseAddr, ECAP_INT_ALL);

    /* Disable CAP1-CAP4 register loads */
    ECAPCaptureLoadingDisable(baseAddr);

    /* Configure eCAP
     *    Enable capture mode.
     *    Continous, stop capture at event 4.
     *    Set polarity based on default start edge configuration.
     *    Set capture in time difference mode.
     *    Enable eCAP module.
     */

    ECAPCounterControl(baseAddr, ECAP_COUNTER_STOP);
    ECAPOperatingModeSelect(baseAddr, ECAP_CAPTURE_MODE);

    /*Configure initial parameters:*/
    ECAPContinousModeConfig(baseAddr);

    if (chObj->activation_edge == ICU_RISING_EDGE)
    {
        ECAPCapeEvtPolarityConfig(baseAddr, ICU_RISING, ICU_RISING, ICU_RISING, ICU_RISING);
    }
    else if (chObj->activation_edge == ICU_FALLING_EDGE)
    {
        ECAPCapeEvtPolarityConfig(baseAddr, ICU_FALLING, ICU_FALLING, ICU_FALLING, ICU_FALLING);
    }
    else if (chObj->activation_edge == ICU_BOTH_EDGES)
    {
        ECAPCapeEvtPolarityConfig(baseAddr, ICU_FALLING, ICU_RISING, ICU_FALLING, ICU_RISING);
    }
    else
    {
        /*MISRA-C*/
    }

    ECAPCaptureEvtCntrRstConfig(baseAddr, 1, 1, 1, 1); /*Delta Mode*/

    ECAPPrescaleConfig(baseAddr, chObj->prescaler);

    ECAPSyncInOutSelect(baseAddr, ECAP_ENABLE_Counter, ECAP_SYNC_IN);
    ECAPCounterControl(baseAddr, ECAP_COUNTER_FREE_RUNNING);
    ECAPCaptureLoadingEnable(baseAddr);
}

void Icu_ConfigEcap(uint32 baseAddr,
        Icu_ActivationType activation, Icu_CounterRstType cntRst,
        boolean interruptEnable)
{
    ECAPIntDisable(baseAddr, ECAP_INT_ALL);
    ECAPIntStatusClear(baseAddr, ECAP_INT_ALL);

    /* Disable CAP1-CAP4 register loads */
    ECAPCaptureLoadingDisable(baseAddr);

    ECAPCounterControl(baseAddr, ECAP_COUNTER_STOP);
    ECAPOperatingModeSelect(baseAddr, ECAP_CAPTURE_MODE);
    ECAPContinousModeConfig(baseAddr);

    if (activation == ICU_RISING_EDGE)
    {
        ECAPCapeEvtPolarityConfig(baseAddr, ICU_RISING, ICU_RISING, ICU_RISING, ICU_RISING);
    }
    else if (activation == ICU_FALLING_EDGE)
    {
        ECAPCapeEvtPolarityConfig(baseAddr, ICU_FALLING, ICU_FALLING, ICU_FALLING, ICU_FALLING);
    }
    else if (activation == ICU_BOTH_EDGES)
    {
        ECAPCapeEvtPolarityConfig(baseAddr, ICU_FALLING, ICU_RISING, ICU_FALLING, ICU_RISING);
    }
    else
    {
        /*MISRA-C*/
    }

    if (cntRst == ICU_ABSOLUTE_MODE)
    {
        ECAPCaptureEvtCntrRstConfig(baseAddr, 0, 0, 0, 0); /*using absolute mode*/
    }
    else if (cntRst == ICU_DELTA_MODE)
    {
        ECAPCaptureEvtCntrRstConfig(baseAddr, 1, 1, 1, 1); /*using delta mode*/
    }
    else
    {
        /*MISRA-C*/
    }

    ECAPSyncInOutSelect(baseAddr, ECAP_ENABLE_Counter, ECAP_SYNC_IN);
    ECAPCounterControl(baseAddr, ECAP_COUNTER_FREE_RUNNING);
    /* Enable CAP1-CAP4 register loads */
    ECAPCaptureLoadingEnable(baseAddr);
    if (interruptEnable == TRUE)
    {
        /*Cause interrupts at every capture event*/
        ECAPIntEnable(baseAddr, ECAP_INT_ALLCAPS);
    }
}

#if (STD_ON == ICU_TIMESTAMP_API)

void Icu_TimeStamp_Init(Icu_ChannelType Channel, Icu_ValueType* StartPtr, uint16 Size, uint16 NotifyInterval)
{
    Icu_ChObj[Channel].NextTimeStampIndexPtr = StartPtr;
    Icu_ChObj[Channel].TimeStampBufferSize = Size;
    Icu_ChObj[Channel].NextTimeStampIndex = 0U;
    Icu_ChObj[Channel].NotifyInterval = NotifyInterval;
    Icu_ChObj[Channel].NotificationCounter = 0U;
}

void Icu_TimeStamp_Clear(Icu_ChannelType Channel)
{
    Icu_ChObj[Channel].NextTimeStampIndexPtr = NULL;
    Icu_ChObj[Channel].TimeStampBufferSize = 0U;
    Icu_ChObj[Channel].NextTimeStampIndex = 0U;
    Icu_ChObj[Channel].NotifyInterval = 0U;
    Icu_ChObj[Channel].NotificationCounter = 0U;
}

#endif /*ICU_TIMESTAMP_API*/

#if (STD_ON == ICU_SIGNAL_MEASUREMENT_API)

/*Reset Signal measurement runtime struct*/
void Icu_SignalMeasurement_Init(Icu_ChannelType Channel)
{
    Icu_ChObj[Channel].cap1 = 0U;
    Icu_ChObj[Channel].cap2 = 0U;
    Icu_ChObj[Channel].cap3 = 0U;
    Icu_ChObj[Channel].cap4 = 0U;
    Icu_ChObj[Channel].HighTime = 0U;
    Icu_ChObj[Channel].LowTime = 0U;
    Icu_ChObj[Channel].Period = 0U;
    Icu_ChObj[Channel].DutyCycle.PeriodTime = 0U;
    Icu_ChObj[Channel].DutyCycle.ActiveTime = 0U;
    Icu_ChObj[Channel].DutyAcquired = FALSE;
    Icu_ChObj[Channel].PeriodAcquired = FALSE;
}

#endif

#if (STD_ON == ICU_DEV_ERROR_DETECT)
void Icu_reportDetError(uint8 apiId, uint8 errorId)
{
    (void) Det_ReportError(ICU_MODULE_ID, ICU_INSTANCE_ID, apiId, errorId);
    return;
}
#endif  /* #if (STD_ON == ICU_DEV_ERROR_DETECT) */

void Icu_reportDetRuntimeError(uint8 apiId, uint8 errorId)
{
    (void) Det_ReportRuntimeError(ICU_MODULE_ID, ICU_INSTANCE_ID, apiId, errorId);
    return;
}

#define ICU_STOP_SEC_CODE
#include "Icu_MemMap.h"

#define ICU_START_SEC_ISR_CODE
/*TI_INSPECTED 338 S : MISRAC_2012_D_20_1
 * "Reason - Required to comply with AUTOSAR memmap spec " */
#include "Icu_MemMap.h"

#if (STD_ON == ICU_TIMESTAMP_API)

FUNC(void, ICU_CODE) Icu_Timestamp_ISR (
    Icu_ChannelType Channel)
{
    uint32 baseAddr;
    baseAddr = Icu_ChObj[Channel].baseAddr;
    uint32 timestampRead;

    /*BufferPtr null check*/
    if (Icu_ChObj[Channel].NextTimeStampIndexPtr != NULL)
    {
        /*Check which capregister called the interrupt*/
        if (ECAPIntStatus(baseAddr,ECAP_CEVT1_INT)!=(uint32)0)
        {
            /*interrupt occured at CAP1, get the current timestamp from CAP1*/
            timestampRead = ECAPTimeStampRead(baseAddr, ECAP_CAPTURE_EVENT_1);
            Icu_ChObj[Channel].NextTimeStampIndexPtr[Icu_ChObj[Channel].NextTimeStampIndex] = timestampRead;
            ECAPIntStatusClear(baseAddr, ECAP_CEVT1_INT);
        }

        /* ICU_CoverageGap_10: Dynamic coverage for the below "else if” 
         * statement and branch is dependent on hardware register input. 
         * Thus it cannot be covered.
         */
        
        else if (ECAPIntStatus(baseAddr,ECAP_CEVT2_INT)!=(uint32)0)
        {
            /*interrupt occured at CAP1, get the current timestamp from CAP1*/
            timestampRead = ECAPTimeStampRead(baseAddr, ECAP_CAPTURE_EVENT_2);
            Icu_ChObj[Channel].NextTimeStampIndexPtr[Icu_ChObj[Channel].NextTimeStampIndex] = timestampRead;
            ECAPIntStatusClear(baseAddr, ECAP_CEVT2_INT);

        }

        /* ICU_CoverageGap_10: Dynamic coverage for the below "else if” 
         * statement and branch is dependent on hardware register input. 
         * Thus it cannot be covered.
         */
        else if (ECAPIntStatus(baseAddr,ECAP_CEVT3_INT)!=(uint32)0)
        {
            /*interrupt occured at CAP1, get the current timestamp from CAP1*/
            timestampRead = ECAPTimeStampRead(baseAddr, ECAP_CAPTURE_EVENT_3);
            Icu_ChObj[Channel].NextTimeStampIndexPtr[Icu_ChObj[Channel].NextTimeStampIndex] = timestampRead;
            ECAPIntStatusClear(baseAddr, ECAP_CEVT3_INT);
        }

        /* ICU_CoverageGap_10: Dynamic coverage for the below "else if” 
         * statement and branch is dependent on hardware register input. 
         * Thus it cannot be covered.
         */
        else if (ECAPIntStatus(baseAddr,ECAP_CEVT4_INT)!=(uint32)0)
        {
            /*interrupt occured at CAP1, get the current timestamp from CAP1*/
            timestampRead = ECAPTimeStampRead(baseAddr, ECAP_CAPTURE_EVENT_4);
            Icu_ChObj[Channel].NextTimeStampIndexPtr[Icu_ChObj[Channel].NextTimeStampIndex] = timestampRead;
            ECAPIntStatusClear(baseAddr, ECAP_CEVT4_INT);
        }
        else
        {
            /* no other values possible*/
        }

        /* Enable all global interrupts*/
        ECAPGlobalIntEnable(baseAddr);

        Icu_ChObj[Channel].NextTimeStampIndex++;

        /*Handling for end of buffer*/
        if (Icu_ChObj[Channel].NextTimeStampIndex >= Icu_ChObj[Channel].TimeStampBufferSize)
        {
         
        /* ICU_CoverageGap_11: Dynamic coverage for the below "switch” statements
         * is dependent on hardware register input. Thus it cannot be covered.
         */
         
            switch (Icu_ChObj[Channel].chCfg.bufferType)
            {
                case ICU_LINEAR_BUFFER:
                    /*Stop capturing values and disable interrupts */

                    /*Stop capturing timestamps, but take the NextTimeStampIndex value*/

                    ECAPIntDisable(baseAddr, ECAP_INT_ALL);
                    Icu_ChObj[Channel].IsRunning = FALSE;
                    break;
                case ICU_CIRCULAR_BUFFER:
                    /*Next timestamp writes over the first item, and continous capturing timestamps. */
                    Icu_ChObj[Channel].NextTimeStampIndex = 0;
                    break;
                default:
                    break;
            }
        }

    }
    else
    {
        /*Buffer Null error*/
    }

    return;
}

#endif

/*******************************************************************************
 *   Function Name : Icu_ChannelISR
 ******************************************************************************/
/*! \Description: Interrupt Subroutine for Notification
 *
 *  \context    Function is called from interrupt level
 ******************************************************************************/
FUNC(void, ICU_CODE) Icu_ChannelISR(
    Icu_ChannelType Channel)
{
#if (STD_ON == ICU_TIMESTAMP_API)
    if (Icu_ChObj[Channel].chCfg.measurementMode == ICU_MODE_TIMESTAMP)
    {
        Icu_Timestamp_ISR (Channel);
        
        /* ICU_CoverageGap_12: Dynamic coverage for the "if” statement/branch/(MC/DC) gives a false positive.
         * Thus it cannot be covered. 
         */
        
        if ((++Icu_ChObj[Channel].NotificationCounter >= Icu_ChObj[Channel].NotifyInterval) &&
            (Icu_ChObj[Channel].NotificationEnabled != FALSE))
        {
            /* Call configured notification function if defined */
            if (Icu_ChObj[Channel].chCfg.notificationHandler != NULL)
            {
                Icu_ChObj[Channel].chCfg.notificationHandler();
            }
        }
    }
#endif
#if (STD_ON == ICU_EDGE_COUNT_API)
    if (Icu_ChObj[Channel].chCfg.measurementMode == ICU_MODE_EDGE_COUNTER)
    {
        uint32 baseAddr;
        baseAddr = Icu_ChObj[Channel].baseAddr;

        Icu_ChObj[Channel].InputState = ICU_ACTIVE;
        Icu_ChObj[Channel].EdgeCounter++;

        /* Enable all global interrupts*/
        ECAPIntStatusClear(baseAddr, ECAP_INT_ALL);
        ECAPGlobalIntEnable(baseAddr);
    }
#endif
#if (STD_ON == ICU_EDGE_DETECT_API)
    if (Icu_ChObj[Channel].chCfg.measurementMode == ICU_MODE_SIGNAL_EDGE_DETECT)
    {
        uint32 baseAddr;
        baseAddr = Icu_ChObj[Channel].baseAddr;

        Icu_ChObj[Channel].InputState = ICU_ACTIVE;
        
        /* ICU_CoverageGap_12: Dynamic coverage for the "if” statement/branch/(MC/DC) gives a false positive.
         * Thus it cannot be covered. 
         */
        
        if ((Icu_ChObj[Channel].NotificationEnabled == TRUE) &&
            (Icu_ChObj[Channel].chCfg.notificationHandler != NULL))
            {
                Icu_ChObj[Channel].chCfg.notificationHandler();

                /* Enable all global interrupts*/
                ECAPIntStatusClear(baseAddr, ECAP_INT_ALL);
                ECAPGlobalIntEnable(baseAddr);

            }
            else
            {
                /*MISRA*/
            }
    }
#endif
}

#define ICU_STOP_SEC_ISR_CODE
#include "Icu_MemMap.h"
