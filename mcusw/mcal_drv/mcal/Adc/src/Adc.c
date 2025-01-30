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
 *  \file     Adc.c
 *
 *  \brief    This file contains ADC MCAL driver
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "stdint.h"
#include "string.h"
#include "Adc.h"
#include "Adc_Priv.h"
#include <hw_include/lld_adc.h>
/* There are static inline functions in hw_types.h file. Map them as well */
#define ADC_START_SEC_CODE
#include "Adc_MemMap.h"
#include <hw_include/hw_types.h>
#define ADC_STOP_SEC_CODE
#include "Adc_MemMap.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* AUTOSAR version information check has to match definition in header file */
#if ((ADC_AR_RELEASE_MAJOR_VERSION != (4U)) || \
    (ADC_AR_RELEASE_MINOR_VERSION != (3U)) ||  \
    (ADC_AR_RELEASE_REVISION_VERSION != (1U)))
    #error "Adc: AUTOSAR Version Numbers of Adc are different!!"
#endif

/* AUTOSAR version information check has to match definition in header file */
#if ((ADC_SW_MAJOR_VERSION != (9U)) || \
    (ADC_SW_MINOR_VERSION != (0U)) ||  \
    (ADC_SW_PATCH_VERSION != (1U)))

    #error "Adc: Software Version Numbers are inconsistent!!"
#endif

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */
static Std_ReturnType Adc_getStreamPtrCheckDetError(
    Adc_GroupType        Group,
    Adc_ValueGroupType **PtrToSamplePtr);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
#define ADC_START_SEC_VAR_INIT_32
#include "Adc_MemMap.h"
/** \brief ADC driver init status */
static volatile VAR(uint32, ADC_VAR_CLEARED) Adc_DrvIsInit = (uint32) FALSE;
#define ADC_STOP_SEC_VAR_INIT_32
#include "Adc_MemMap.h"

#define ADC_START_SEC_VAR_NO_INIT_UNSPECIFIED
#include "Adc_MemMap.h"
/** \brief ADC driver object */
VAR(Adc_DriverObjType, ADC_VAR_CLEARED) Adc_DrvObj;
#define ADC_STOP_SEC_VAR_NO_INIT_UNSPECIFIED
#include "Adc_MemMap.h"

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

#define ADC_START_SEC_CODE
#include "Adc_MemMap.h"

/*
 *Design: MCAL-5875,MCAL-5859,MCAL-5781,MCAL-5767,MCAL-5854,MCAL-5736,MCAL-5808,MCAL-5717,MCAL-5755,MCAL-5700,MCAL-5884,MCAL-5834,MCAL-5812,MCAL-5797,MCAL-5789,MCAL-5705,MCAL-5890
 */
FUNC(void, ADC_CODE) Adc_Init(
    P2CONST(Adc_ConfigType, AUTOMATIC, ADC_PBCFG) CfgPtr)
{
    uint8 hwUnitIdx;
    const Adc_ConfigType *ConfigPtr = CfgPtr;

#if (STD_ON == ADC_VARIANT_PRE_COMPILE)
    ConfigPtr = &ADC_INIT_CONFIG_PC;
#endif  /* (STD_ON == ADC_PRE_COMPILE_VARIANT) */

#if (STD_ON == ADC_DEV_ERROR_DETECT)
    if (((uint32) FALSE) != Adc_DrvIsInit)
    {
        Adc_reportDetError(ADC_SID_INIT, ADC_E_ALREADY_INITIALIZED);
    }
    else if (NULL_PTR == ConfigPtr)
    {
        Adc_reportDetError(ADC_SID_INIT, ADC_E_PARAM_CONFIG);
    }
    else if ((ConfigPtr->maxGroup > ADC_MAX_GROUP) ||
             (ConfigPtr->maxHwUnit > ADC_MAX_HW_UNIT))
    {
        Adc_reportDetError(ADC_SID_INIT, ADC_E_PARAM_CONFIG);
    }
    else if (((uint32) TRUE) != Adc_checkHwUnitObj(ConfigPtr))
    {
        Adc_reportDetError(ADC_SID_INIT, ADC_E_PARAM_CONFIG);
    }
    else
#endif  /* #if (STD_ON == ADC_DEV_ERROR_DETECT) */
    {
        Adc_resetDrvObj(&Adc_DrvObj);

        /* Copy the configuration */
        Adc_copyConfig(&Adc_DrvObj, ConfigPtr);

        /* Init HW once all config is copied */
        for (hwUnitIdx = 0U; hwUnitIdx < ConfigPtr->maxHwUnit; hwUnitIdx++)
        {
            Adc_HwUnitObjType *hwObj = &(Adc_DrvObj.hwUnitObj[hwUnitIdx]);
            Adc_hwUnitInit(hwObj);
        }

        /* Initialize driver status and object */
        Adc_DrvIsInit = (uint32) TRUE;
    }

    return;
}

#if (STD_ON == ADC_DEINIT_API)
/*
 * Design: MCAL-5784,MCAL-5841,MCAL-5748,MCAL-5838,MCAL-5792,MCAL-5847,MCAL-5659,MCAL-5789
 */
FUNC(void, ADC_CODE) Adc_DeInit(void)
{
    uint8 hwUnitIdx;
    uint32 isError = (uint32) FALSE;
	uint8 grpIdx;
    Adc_GroupObjType *groupObj;

#if (STD_ON == ADC_DEV_ERROR_DETECT)
    if (((uint32) FALSE) == Adc_DrvIsInit)
    {
        Adc_reportDetError(ADC_SID_DEINIT, ADC_E_UNINIT);
    }
    else
#endif  /* #if (STD_ON == ADC_DEV_ERROR_DETECT) */
    {
        SchM_Enter_Adc_ADC_EXCLUSIVE_AREA_0();
        for (grpIdx = 0U; grpIdx < Adc_DrvObj.maxGroup; grpIdx++)
        {
            groupObj = &Adc_DrvObj.groupObj[grpIdx];
            if ((ADC_BUSY == groupObj->groupStatus) ||
                (ADC_COMPLETED == groupObj->groupStatus))
            {
                /* These states indicate, that the group is still converting */
                isError = (uint32) TRUE;
            }
            else if (ADC_STREAM_COMPLETED == groupObj->groupStatus)
            {
                /* Check if this group has to be stopped explicitly */
                if (((uint32) TRUE) == groupObj->isExplicitStopMode)
                {
                    /* Group has to be stopped explicitly. State
                     * ADC_STREAM_COMPLETED means, that group is still
                     * converting */
                    isError = (uint32) TRUE;
                }
            }
            else
            {
                /* This group is not converting: continue */
            }

            if (((uint32) TRUE) == isError)
            {
                Adc_reportDetRuntimeError(ADC_SID_DEINIT, ADC_E_BUSY);
            }
        }

        if (((uint32) FALSE) == isError)
        {
            Adc_DrvIsInit = (uint32) FALSE;
            /* Deinit the hardware modules */
            for (hwUnitIdx = 0U; hwUnitIdx < Adc_DrvObj.maxHwUnit; hwUnitIdx++)
            {
                Adc_hwUnitDeInit(&Adc_DrvObj.hwUnitObj[hwUnitIdx]);
            }
            Adc_resetDrvObj(&Adc_DrvObj);
        }
        SchM_Exit_Adc_ADC_EXCLUSIVE_AREA_0();
    }

    return;
}
#endif  /* #if (STD_ON == ADC_DEINIT_API) */


/*
 * Design: MCAL-5827,MCAL-5758,MCAL-5773,MCAL-5786,MCAL-5721,MCAL-5848,MCAL-5754,MCAL-5846,MCAL-5738
 */
FUNC(Std_ReturnType, ADC_CODE) Adc_SetupResultBuffer(
    Adc_GroupType Group,
    const Adc_ValueGroupType * DataBufferPtr)
{
    Std_ReturnType    retVal = (Std_ReturnType) E_OK;
    Adc_GroupObjType *groupObj;

#if (STD_ON == ADC_DEV_ERROR_DETECT)
    if (((uint32) FALSE) == Adc_DrvIsInit)
    {
        Adc_reportDetError(ADC_SID_SETUP_RESULT_BUFFER, ADC_E_UNINIT);
        retVal = (Std_ReturnType) E_NOT_OK;
    }
    else if (Group >= Adc_DrvObj.maxGroup)
    {
        Adc_reportDetError(ADC_SID_SETUP_RESULT_BUFFER, ADC_E_PARAM_GROUP);
        retVal = (Std_ReturnType) E_NOT_OK;
    }
    else if (NULL_PTR == DataBufferPtr)
    {
        Adc_reportDetError(ADC_SID_SETUP_RESULT_BUFFER, ADC_E_PARAM_POINTER);
        retVal = (Std_ReturnType) E_NOT_OK;
    }
    else
#endif  /* #if (STD_ON == ADC_DEV_ERROR_DETECT) */
    {
        groupObj = &Adc_DrvObj.groupObj[Group];

        SchM_Enter_Adc_ADC_EXCLUSIVE_AREA_0();

        /* Check group status */
        if (ADC_IDLE != groupObj->groupStatus)
        {
            Adc_reportDetRuntimeError(ADC_SID_SETUP_RESULT_BUFFER, ADC_E_BUSY);
            retVal = (Std_ReturnType) E_NOT_OK;
        }
        else
        {
            groupObj->resultBufPtr = DataBufferPtr;
        }

        SchM_Exit_Adc_ADC_EXCLUSIVE_AREA_0();
    }

    return (retVal);
}

/*
 * Design: MCAL-5892,MCAL-5802,MCAL-5857,MCAL-5828,MCAL-5860,MCAL-5810,MCAL-5778,MCAL-5704,MCAL-5743,MCAL-5887,MCAL-5741,MCAL-5806
 */
FUNC(Adc_StatusType, ADC_CODE) Adc_GetGroupStatus(Adc_GroupType Group)
{
    Adc_StatusType    groupStatus = ADC_IDLE;
    Adc_GroupObjType *groupObj;

#if (STD_ON == ADC_DEV_ERROR_DETECT)
    if (((uint32) FALSE) == Adc_DrvIsInit)
    {
        Adc_reportDetError(ADC_SID_GET_GROUP_STATUS, ADC_E_UNINIT);
    }
    else if (Group >= Adc_DrvObj.maxGroup)
    {
        Adc_reportDetError(ADC_SID_GET_GROUP_STATUS, ADC_E_PARAM_GROUP);
    }
    else
#endif  /* #if (STD_ON == ADC_DEV_ERROR_DETECT) */
    {
        groupObj = &Adc_DrvObj.groupObj[Group];

        SchM_Enter_Adc_ADC_EXCLUSIVE_AREA_0();

        groupStatus = groupObj->groupStatus;

        SchM_Exit_Adc_ADC_EXCLUSIVE_AREA_0();
    }

    return (groupStatus);
}


/*
 * Design: MCAL-5731,MCAL-5862,MCAL-5783,MCAL-5728,MCAL-5707,MCAL-5853,MCAL-5711,MCAL-5804,MCAL-5730,MCAL-5780,MCAL-5806,MCAL-5719,MCAL-5802,MCAL-5892
 */
FUNC(Adc_StreamNumSampleType, ADC_CODE) Adc_GetStreamLastPointer(
    Adc_GroupType Group,
    Adc_ValueGroupType * *PtrToSamplePtr)
{
    Adc_GroupObjType       *groupObj;
    Adc_StreamNumSampleType numSamples = 0U;
    Adc_ChannelObjType     *chObjLast;
    uint32 offset;
    Std_ReturnType          retVal = (Std_ReturnType) E_OK;

    SchM_Enter_Adc_ADC_EXCLUSIVE_AREA_0();

    retVal = Adc_getStreamPtrCheckDetError(Group, PtrToSamplePtr);
    if (((Std_ReturnType) E_OK) == retVal)
    {
        groupObj = &Adc_DrvObj.groupObj[Group];
        if (ADC_BUSY == groupObj->groupStatus)
        {
            *PtrToSamplePtr = NULL;
        }
        else
        {
            numSamples =
                (Adc_StreamNumSampleType) groupObj->validSampleCount;
            /* Reset ValidSampleCount after read process */
            groupObj->validSampleCount = 0U;

            /* Use the last channel pointers to calculate last result
             * pointer as this represents a completion of a set of all
             * channels. If we use any other channel, we could be half way
             * through and might return pointer one location extra */
            chObjLast =
                &groupObj->chObj[groupObj->groupCfg.numChannels - 1U];
            if ((chObjLast->curResultBufPtr) <= (chObjLast->chResultBufPtr))
            {
                /* Pointer is at first element, so offset is last sample */
                offset = (uint32) groupObj->groupCfg.streamNumSamples - 1U;
            }
            else
            {
                offset =
                    chObjLast->curResultBufPtr - chObjLast->chResultBufPtr;
                /* The next result will be written at position [n],
                 * so the latest result in buffer is at [n-1] */
                offset--;
            }
            /* Let PtrToSamplePtr refer to the latest element in
             * application result buffer. */
            *PtrToSamplePtr = (Adc_ValueGroupType *)
								&groupObj->resultBufPtr[offset];

            /* State transition after getting StreamLastPointer */
            if (ADC_COMPLETED == groupObj->groupStatus)
            {
                /* Conversion has not been finished, when the group
                 * was read. State transition to ADC_BUSY */
                groupObj->groupStatus = ADC_BUSY;
            }
            else
            {
                /*
                 * ADC_STREAM_COMPLETED State
                 */

                /* Conversion has been finished before the group was read.
                 * State transition to ADC_IDLE if the group stops
                 * implicitly or to ADC_BUSY if the group has to be
                 * stopped explicitly.
                 */
                if (((uint32) TRUE) == groupObj->isExplicitStopMode)
                {
                    /* Group has to be stopped explicitly, so conversion is
                     * going on after this read. */
                    groupObj->groupStatus = ADC_BUSY;
                }
                else
                {
                    /* Group stops implicitly when ADC_STREAM_COMPLETED is
                     * reached and the group is read. */
                    groupObj->groupStatus = ADC_IDLE;
                }
            }
        }
    }

    SchM_Exit_Adc_ADC_EXCLUSIVE_AREA_0();

    return (numSamples);
}

#if (STD_ON == ADC_ENABLE_START_STOP_GROUP_API)
/*
 * Design: MCAL-5833,MCAL-5725,MCAL-5751,MCAL-5713,MCAL-5872,MCAL-5819,MCAL-5720,MCAL-5759,MCAL-5790,MCAL-5891,MCAL-5787,MCAL-5846,MCAL-5825,MCAL-5869,MCAL-5809,MCAL-5738,MCAL-5689,MCAL-5802
 */
FUNC(void, ADC_CODE) Adc_StartGroupConversion(Adc_GroupType Group)
{
    Std_ReturnType    retVal;
    Adc_GroupObjType *groupObj;

#if (STD_ON == ADC_DEV_ERROR_DETECT)
    if (((uint32) FALSE) == Adc_DrvIsInit)
    {
        Adc_reportDetError(ADC_SID_START_GROUP_CONVERSION, ADC_E_UNINIT);
    }
    else if (Group >= Adc_DrvObj.maxGroup)
    {
        Adc_reportDetError(ADC_SID_START_GROUP_CONVERSION, ADC_E_PARAM_GROUP);
    }
    else
#endif  /* #if (STD_ON == ADC_DEV_ERROR_DETECT) */
    {
        groupObj = &Adc_DrvObj.groupObj[Group];

        SchM_Enter_Adc_ADC_EXCLUSIVE_AREA_0();

#if (STD_ON == ADC_DEV_ERROR_DETECT)
        if (NULL_PTR == groupObj->resultBufPtr)
        {
            Adc_reportDetError(
                ADC_SID_START_GROUP_CONVERSION, ADC_E_BUFFER_UNINIT);
        }
        else if (ADC_TRIGG_SRC_HW == groupObj->groupCfg.triggSrc)
        {
            Adc_reportDetError(
                ADC_SID_START_GROUP_CONVERSION, ADC_E_WRONG_TRIGG_SRC);
        }
        else
#endif  /* #if (STD_ON == ADC_DEV_ERROR_DETECT) */
        {
            /* Call the start group API */
            retVal = Adc_startGroup(groupObj);
            if (((Std_ReturnType) E_NOT_OK) == retVal)
            {
                Adc_reportDetRuntimeError(ADC_SID_START_GROUP_CONVERSION, ADC_E_BUSY);
            }
        }

        SchM_Exit_Adc_ADC_EXCLUSIVE_AREA_0();
    }

    return;
}

/*
 * Design: MCAL-5785,MCAL-5842,MCAL-5835,MCAL-5763,MCAL-5718,MCAL-5850,MCAL-5746,MCAL-5794,MCAL-5777,MCAL-5873,MCAL-5867,MCAL-5870,MCAL-5689,MCAL-5892
 */
FUNC(void, ADC_CODE) Adc_StopGroupConversion(Adc_GroupType Group)
{
    Adc_GroupObjType *groupObj;
    Std_ReturnType retVal = (Std_ReturnType) E_OK;

#if (STD_ON == ADC_DEV_ERROR_DETECT)
    if (((uint32) FALSE) == Adc_DrvIsInit)
    {
        Adc_reportDetError(ADC_SID_STOP_GROUP_CONVERSION, ADC_E_UNINIT);
    }
    else if (Group >= Adc_DrvObj.maxGroup)
    {
        Adc_reportDetError(ADC_SID_STOP_GROUP_CONVERSION, ADC_E_PARAM_GROUP);
    }
    else
#endif  /* #if (STD_ON == ADC_DEV_ERROR_DETECT) */
    {
        groupObj = &Adc_DrvObj.groupObj[Group];

        SchM_Enter_Adc_ADC_EXCLUSIVE_AREA_0();

#if (STD_ON == ADC_DEV_ERROR_DETECT)
        if (ADC_TRIGG_SRC_HW == groupObj->groupCfg.triggSrc)
        {
            Adc_reportDetError(
                ADC_SID_STOP_GROUP_CONVERSION, ADC_E_WRONG_TRIGG_SRC);
			retVal = (Std_ReturnType) E_NOT_OK;
        }
#endif  /* #if (STD_ON == ADC_DEV_ERROR_DETECT) */
		if (retVal == (Std_ReturnType) E_OK)
		{
			if (ADC_IDLE == groupObj->groupStatus)
			{
				Adc_reportDetRuntimeError(
					ADC_SID_STOP_GROUP_CONVERSION, ADC_E_IDLE);
			}
			else
			{
				/* Call the stop group API */
				Adc_stopGroup(groupObj);
			}
		}

        SchM_Exit_Adc_ADC_EXCLUSIVE_AREA_0();
    }

    return;
}
#endif  /* #if (STD_ON == ADC_ENABLE_START_STOP_GROUP_API) */

#if (STD_ON == ADC_READ_GROUP_API)
/*
 * Design: MCAL-5710,MCAL-5828,MCAL-5851,MCAL-5796,MCAL-5745,MCAL-5803,MCAL-5824,MCAL-5766,MCAL-5866,MCAL-5868,MCAL-5727,MCAL-5715,MCAL-5649,MCAL-5802,MCAL-5892
 */
FUNC(Std_ReturnType, ADC_CODE) Adc_ReadGroup(
    Adc_GroupType Group,
    Adc_ValueGroupType * DataBufferPtr)
{
    Std_ReturnType retVal = (Std_ReturnType) E_OK;
    Adc_GroupObjType           *groupObj;
    Adc_ChannelObjType         *chObj, *chObjLast;
    uint32         offset, chIdx;

#if (STD_ON == ADC_DEV_ERROR_DETECT)
    if (((uint32) FALSE) == Adc_DrvIsInit)
    {
        Adc_reportDetError(ADC_SID_READ_GROUP, ADC_E_UNINIT);
        retVal = (Std_ReturnType) E_NOT_OK;
    }
    else if (Group >= Adc_DrvObj.maxGroup)
    {
        Adc_reportDetError(ADC_SID_READ_GROUP, ADC_E_PARAM_GROUP);
        retVal = (Std_ReturnType) E_NOT_OK;
    }
    else if (NULL_PTR == DataBufferPtr)
    {
        /* Note: This error return is not mentioned in spec */
        Adc_reportDetError(ADC_SID_READ_GROUP, ADC_E_PARAM_GROUP);
        retVal = (Std_ReturnType) E_NOT_OK;
    }
    else
#endif  /* #if (STD_ON == ADC_DEV_ERROR_DETECT) */
    {
        groupObj = &Adc_DrvObj.groupObj[Group];

        SchM_Enter_Adc_ADC_EXCLUSIVE_AREA_0();

        if (ADC_IDLE == groupObj->groupStatus)
        {
            Adc_reportDetRuntimeError(ADC_SID_READ_GROUP, ADC_E_IDLE);
            retVal = (Std_ReturnType) E_NOT_OK;
        }
        else
        {
            if (ADC_BUSY == groupObj->groupStatus)
            {
                retVal = (Std_ReturnType) E_NOT_OK;
            }
            else
            {
                /*
                 * State equals ADC_COMPLETED or ADC_STREAM_COMPLETED, so
                 * results are available: start reading group
                 */

                /* Use the last channel pointers to calculate last result
                 * pointer as this represents a completion of a set of all
                 * channels. If we use any other channel, we could be half way
                 * through and might return pointer one location extra */
                chObjLast =
                    &groupObj->chObj[groupObj->groupCfg.numChannels - 1U];

                if ((chObjLast->curResultBufPtr) <= (chObjLast->chResultBufPtr))
                {
                    /* Pointer is at first element, so offset is last sample */
                    offset = (uint32) groupObj->groupCfg.streamNumSamples - 1U;
                }
                else
                {
                    offset =
                        chObjLast->curResultBufPtr - chObjLast->chResultBufPtr;
                    /* The next result will be written at position [n],
                     * so the latest result in buffer is at [n-1] */
                    offset--;
                }

                for (chIdx = 0U;
                     chIdx < groupObj->groupCfg.numChannels;
                     chIdx++)
                {
                    chObj = &groupObj->chObj[chIdx];
                    DataBufferPtr[chIdx] = chObj->chResultBufPtr[offset];
                }

                /* Reset ValidSampleCount after read process */
                groupObj->validSampleCount = 0U;

                /* State transition after reading to buffer */
                if (ADC_COMPLETED == groupObj->groupStatus)
                {
                    /* Conversion has not been finished, when the group
                     * was read. State transition to ADC_BUSY */
                    groupObj->groupStatus = ADC_BUSY;
                }
                else
                {
                    /*
                     * ADC_STREAM_COMPLETED State
                     */

                    /* Conversion has been finished before the group was read.
                     * State transition to ADC_IDLE if the group stops
                     * implicitly or to ADC_BUSY if the group has to be
                     * stopped explicitly.
                     */
                    if (((uint32) TRUE) == groupObj->isExplicitStopMode)
                    {
                        /* Group has to be stopped explicitly, so conversion is
                         * going on after this read. */
                        groupObj->groupStatus = ADC_BUSY;
                    }
                    else
                    {
                        /* Group stops implicitly when ADC_STREAM_COMPLETED is
                         * reached and the group is read. */
                        groupObj->groupStatus = ADC_IDLE;
                    }
                }
            }
        }

        SchM_Exit_Adc_ADC_EXCLUSIVE_AREA_0();
    }

    return (retVal);
}
#endif  /* #if (STD_ON == ADC_READ_GROUP_API) */

#if (STD_ON == ADC_GRP_NOTIF_CAPABILITY_API)
/*
 * Design: MCAL-5818,MCAL-5775,MCAL-5863,MCAL-5776,MCAL-5729,MCAL-5840
 */
FUNC(void, ADC_CODE) Adc_EnableGroupNotification(Adc_GroupType Group)
{
    Adc_GroupObjType *groupObj;

#if (STD_ON == ADC_DEV_ERROR_DETECT)
    if (((uint32) FALSE) == Adc_DrvIsInit)
    {
        Adc_reportDetError(ADC_SID_ENABLE_GROUP_NOTIFICATION, ADC_E_UNINIT);
    }
    else if (Group >= Adc_DrvObj.maxGroup)
    {
        Adc_reportDetError(
            ADC_SID_ENABLE_GROUP_NOTIFICATION, ADC_E_PARAM_GROUP);
    }
    else
#endif  /* #if (STD_ON == ADC_DEV_ERROR_DETECT) */
    {
        groupObj = &Adc_DrvObj.groupObj[Group];

#if (STD_ON == ADC_DEV_ERROR_DETECT)
        /* Check group status */
        if ((Adc_GroupEndNotifyType)NULL_PTR ==
                groupObj->groupCfg.Adc_GroupEndNotification)
        {
            Adc_reportDetError(
                ADC_SID_ENABLE_GROUP_NOTIFICATION, ADC_E_NOTIF_CAPABILITY);
        }
        else
#endif  /* #if (STD_ON == ADC_DEV_ERROR_DETECT) */
        {
            SchM_Enter_Adc_ADC_EXCLUSIVE_AREA_0();

            groupObj->isNotifyOn = (uint32) TRUE;

            SchM_Exit_Adc_ADC_EXCLUSIVE_AREA_0();
        }
    }

    return;
}

/*
 * Design: MCAL-5737,MCAL-5878,MCAL-5813,MCAL-5877,MCAL-5772,MCAL-5886
 */
FUNC(void, ADC_CODE) Adc_DisableGroupNotification(Adc_GroupType Group)
{
    Adc_GroupObjType *groupObj;

#if (STD_ON == ADC_DEV_ERROR_DETECT)
    if (((uint32) FALSE) == Adc_DrvIsInit)
    {
        Adc_reportDetError(ADC_SID_DISABLE_GROUP_NOTIFICATION, ADC_E_UNINIT);
    }
    else if (Group >= Adc_DrvObj.maxGroup)
    {
        Adc_reportDetError(
            ADC_SID_DISABLE_GROUP_NOTIFICATION, ADC_E_PARAM_GROUP);
    }
    else
#endif  /* #if (STD_ON == ADC_DEV_ERROR_DETECT) */
    {
        groupObj = &Adc_DrvObj.groupObj[Group];

#if (STD_ON == ADC_DEV_ERROR_DETECT)
        /* Check group status */
        if ((Adc_GroupEndNotifyType)NULL_PTR ==
                groupObj->groupCfg.Adc_GroupEndNotification)
        {
            Adc_reportDetError(
                ADC_SID_DISABLE_GROUP_NOTIFICATION, ADC_E_NOTIF_CAPABILITY);
        }
        else
#endif  /* #if (STD_ON == ADC_DEV_ERROR_DETECT) */
        {
            SchM_Enter_Adc_ADC_EXCLUSIVE_AREA_0();

            groupObj->isNotifyOn = (uint32) FALSE;

            SchM_Exit_Adc_ADC_EXCLUSIVE_AREA_0();
        }
    }

    return;
}
#endif  /* #if (STD_ON == ADC_GRP_NOTIF_CAPABILITY_API) */

#if (STD_ON == ADC_VERSION_INFO_API)
/*
 * Design: MCAL-5830,MCAL-5691,MCAL-5789
 */
FUNC(void, ADC_CODE) Adc_GetVersionInfo(
    P2VAR(Std_VersionInfoType, AUTOMATIC, ADC_APPL_DATA) versioninfo)
{
#if (STD_ON == ADC_DEV_ERROR_DETECT)
    if (NULL_PTR == versioninfo)
    {
        Adc_reportDetError(ADC_SID_GET_VERSION_INFO, ADC_E_PARAM_POINTER);
    }
    else
#endif  /* #if (STD_ON == ADC_DEV_ERROR_DETECT) */
    {
        versioninfo->vendorID         = ADC_VENDOR_ID;
        versioninfo->moduleID         = ADC_MODULE_ID;
        versioninfo->sw_major_version = (uint8) ADC_SW_MAJOR_VERSION;
        versioninfo->sw_minor_version = (uint8) ADC_SW_MINOR_VERSION;
        versioninfo->sw_patch_version = (uint8) ADC_SW_PATCH_VERSION;
    }

    return;
}
#endif  /* #if (STD_ON == ADC_VERSION_INFO_API) */

static Std_ReturnType Adc_getStreamPtrCheckDetError(
    Adc_GroupType        Group,
    Adc_ValueGroupType **PtrToSamplePtr)
{
    Std_ReturnType    retVal = (Std_ReturnType) E_OK;
    Adc_GroupObjType *groupObj;

#if (STD_ON == ADC_DEV_ERROR_DETECT)
    if ((((uint32) FALSE)) == Adc_DrvIsInit)
    {
        if (NULL_PTR != PtrToSamplePtr)
        {
            *PtrToSamplePtr = NULL;
        }
        retVal = (Std_ReturnType) E_NOT_OK;
        Adc_reportDetError(ADC_SID_GET_STREAM_LAST_POINTER, ADC_E_UNINIT);
    }
    else if (Group >= Adc_DrvObj.maxGroup)
    {
        if (NULL_PTR != PtrToSamplePtr)
        {
            *PtrToSamplePtr = NULL;
        }
        retVal = (Std_ReturnType) E_NOT_OK;
        Adc_reportDetError(ADC_SID_GET_STREAM_LAST_POINTER, ADC_E_PARAM_GROUP);
    }
    else if (NULL_PTR == PtrToSamplePtr)
    {
        /* Note: This error return is not mentioned in spec */
        retVal = (Std_ReturnType) E_NOT_OK;
        Adc_reportDetError(
            ADC_SID_GET_STREAM_LAST_POINTER, ADC_E_PARAM_GROUP);
    }
    else
#endif  /* #if (STD_ON == ADC_DEV_ERROR_DETECT) */
    {
        groupObj = &Adc_DrvObj.groupObj[Group];
        if (ADC_IDLE == groupObj->groupStatus)
        {
            *PtrToSamplePtr = NULL;
            retVal          = (Std_ReturnType) E_NOT_OK;
            Adc_reportDetRuntimeError(
                ADC_SID_GET_STREAM_LAST_POINTER, ADC_E_IDLE);
        }
    }


    return retVal;
}

/*
 * Design: MCAL-5864,MCAL-5771,MCAL-5671,MCAL-5671
 */
#if (STD_ON == ADC_REGISTER_READBACK_API)
FUNC(Std_ReturnType, ADC_CODE) Adc_RegisterReadback(
    Adc_HWUnitType HWUnit,
    P2VAR(Adc_RegisterReadbackType, AUTOMATIC, ADC_APPL_DATA) RegRbPtr)
{
    Adc_HwUnitObjType *hwUnitObj;
    uint32 baseAddr;
    Std_ReturnType     retVal = ((Std_ReturnType) E_OK);

    if (((uint32) FALSE == Adc_DrvIsInit) || (NULL_PTR == RegRbPtr))
    {
        retVal = (Std_ReturnType) E_NOT_OK;
    }
    else
    {
        (void) memset(RegRbPtr, 0, sizeof (Adc_RegisterReadbackType));

        /* HW unit ID may not be the index, so search for matching HW unit */
        hwUnitObj = &Adc_DrvObj.hwUnitObj[HWUnit];

        baseAddr = hwUnitObj->baseAddr;
        RegRbPtr->adcRev    = HW_RD_REG32(baseAddr + ADC_REVISION);
        RegRbPtr->adcCtrl   = HW_RD_REG32(baseAddr + ADC_CTRL);
        RegRbPtr->adcMisc   = HW_RD_REG32(baseAddr + ADC_MISC);
    }

    return (retVal);
}
#endif  /* #if (STD_ON == ADC_REGISTER_READBACK_API) */

#if (STD_ON == ADC_INTERNAL_DIAGNOSTIC_DEBUG_MODE_API)
/*
 * Design: MCAL-5793,MCAL-5726
 */
FUNC(Std_ReturnType, ADC_CODE) Adc_EnableInternalDiagnostic(
    Adc_GroupType Group, Adc_RefSelType RefSelect)
{
    Std_ReturnType     retVal = ((Std_ReturnType) E_OK);
    Adc_GroupObjType *groupObj;
    Adc_HwUnitObjType    *hwUnitObj;
    uint32   baseAddr;

    if ((uint32) FALSE == Adc_DrvIsInit)
    {
        retVal = (Std_ReturnType) E_NOT_OK;
    }
    else if (Group >= Adc_DrvObj.maxGroup)
    {
        retVal = (Std_ReturnType) E_NOT_OK;
    }
    else
    {
        SchM_Enter_Adc_ADC_EXCLUSIVE_AREA_0();
			  groupObj = &Adc_DrvObj.groupObj[Group];
	      hwUnitObj = groupObj->hwUnitObj;
	      baseAddr = hwUnitObj->baseAddr;
        if (ADC_IDLE != groupObj->groupStatus)
        {
            /* This indicate, that the group is still converting */
            retVal = (Std_ReturnType) E_NOT_OK;
        }
      	else
      	{
            /* Enable Functional internal diagnostic debug mode */
            HW_WR_FIELD32(baseAddr + ADC_CTRL, ADC_CTRL_HW_MID_EN,
                  ADC_CTRL_HW_MID_EN_ENABLE);
            /* Reference select for functional internal diagnostic debug mode */
            if(RefSelect != ((Adc_RefSelType)ADC_CTRL_HW_MID_SEL_DISABLE))
            {
                /* REFP selected as Reference select */
                HW_WR_FIELD32(baseAddr + ADC_CTRL, ADC_CTRL_HW_MID_SEL,
                  ADC_CTRL_HW_MID_SEL_ENABLE);
            }
            else
            {
                /* VMID selected as Reference select */
                HW_WR_FIELD32(baseAddr + ADC_CTRL, ADC_CTRL_HW_MID_SEL,
                  ADC_CTRL_HW_MID_SEL_DISABLE);
            }
        }
        SchM_Exit_Adc_ADC_EXCLUSIVE_AREA_0();
    }

    return (retVal);
}

/*
 * Design: MCAL-5793,MCAL-5726
 */
FUNC(Std_ReturnType, ADC_CODE) Adc_DisableInternalDiagnostic(Adc_GroupType Group)
{
    Std_ReturnType     retVal = ((Std_ReturnType) E_OK);
    Adc_GroupObjType *groupObj;
    Adc_HwUnitObjType    *hwUnitObj;
    uint32   baseAddr;

    if ((uint32) FALSE == Adc_DrvIsInit)
    {
        retVal = (Std_ReturnType) E_NOT_OK;
    }
    else if (Group >= Adc_DrvObj.maxGroup)
    {
        retVal = (Std_ReturnType) E_NOT_OK;
    }
    else
    {
        SchM_Enter_Adc_ADC_EXCLUSIVE_AREA_0();
        groupObj = &Adc_DrvObj.groupObj[Group];
        hwUnitObj = groupObj->hwUnitObj;
        baseAddr = hwUnitObj->baseAddr;
        if (ADC_IDLE != groupObj->groupStatus)
        {
            /* This indicate, that the group is still converting */
            retVal = (Std_ReturnType) E_NOT_OK;
        }
        else
        {
            /* Disable Functional internal diagnostic debug mode */
            HW_WR_FIELD32(baseAddr + ADC_CTRL, ADC_CTRL_HW_MID_EN,
                  ADC_CTRL_HW_MID_EN_DISABLE);
            /* Reference select set to default */
            HW_WR_FIELD32(baseAddr + ADC_CTRL, ADC_CTRL_HW_MID_SEL,
                  ADC_CTRL_HW_MID_SEL_DISABLE);
        }
        SchM_Exit_Adc_ADC_EXCLUSIVE_AREA_0();
    }

    return (retVal);
}
#endif 	/* #if (STD_ON == ADC_INTERNAL_DIAGNOSTIC_DEBUG_MODE_API) */

#define ADC_STOP_SEC_CODE
#include "Adc_MemMap.h"
