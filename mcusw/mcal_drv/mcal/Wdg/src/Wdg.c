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
 *  \file     Wdg.c
 *
 *  \brief    This file contains implementation of the Internal Watchdog MCAL
 *            driver
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "string.h"
#include "stdint.h"
#include "Wdg.h"
#include "SchM_Wdg.h"
#include <hw_include/lldr_rti.h>
#if (STD_ON == WDG_DEV_ERROR_DETECT)
#include "Det.h"
#endif
#include "Dem.h"
#include "Wdg_Priv.h"
/* There are static inline functions in hw_types.h file. Map them as well */
#define WDG_START_SEC_CODE
#include "Wdg_MemMap.h"
#include <hw_include/hw_types.h>
#define WDG_STOP_SEC_CODE
#include "Wdg_MemMap.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* AUTOSAR version information check has to match definition in header file */
#if ((WDG_AR_RELEASE_MAJOR_VERSION != (0x04U)) || \
    (WDG_AR_RELEASE_MINOR_VERSION != (0x03U)) ||  \
    (WDG_AR_RELEASE_REVISION_VERSION != (0x01U)))
  #error "WDG: AUTOSAR Version Numbers of WDG are different"
#endif

/* Software Version Check */
#if ((WDG_SW_MAJOR_VERSION != (9U)) || (WDG_SW_MINOR_VERSION != (0U)))
  #error "Version numbers of Wdg.c and Wdg.h are inconsistent!"
#endif

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */
/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */
static FUNC(void, WDG_CODE) Wdg_resetDrvObj(volatile Wdg_DriverObjType *drvObj);
static FUNC(void, WDG_CODE) Wdg_copyConfig(volatile Wdg_DriverObjType *drvObj,
                                const Wdg_ConfigType *cfgPtr);
/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

#define WDG_START_SEC_VAR_NO_INIT_UNSPECIFIED
#include "Wdg_MemMap.h"


    volatile VAR(Wdg_DriverObjType, WDG_VAR_INIT) Wdg_DrvObj;

#define WDG_STOP_SEC_VAR_NO_INIT_UNSPECIFIED
#include "Wdg_MemMap.h"

#define WDG_START_SEC_VAR_INIT_UNSPECIFIED
#include "Wdg_MemMap.h"
/** \brief WDG driver status */
/*
 * Design: MCAL-5622
 */

    volatile VAR(Wdg_StatusType, WDG_VAR_CLEARED) Wdg_DrvStatus = WDG_UNINIT;
#define WDG_STOP_SEC_VAR_INIT_UNSPECIFIED
#include "Wdg_MemMap.h"

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

#define WDG_START_SEC_CODE
#include "Wdg_MemMap.h"

#if (STD_ON == WDG_GET_VERSION_INFO_API)
/*
 * Design: MCAL-5575
 */
FUNC(void, WDG_CODE)
    Wdg_GetVersionInfo(Std_VersionInfoType * versioninfo)
{
#if (STD_ON == WDG_DEV_ERROR_DETECT)
    if (NULL_PTR == versioninfo)
    {
        (void) Wdg_reportDetError(WDG_API_GET_VERSION_INFO,
                                  WDG_E_PARAM_POINTER);
    }
    else
#endif
    {
        versioninfo->vendorID         = (WDG_VENDOR_ID);
        versioninfo->moduleID         = (WDG_MODULE_ID);
        versioninfo->sw_major_version = (uint8) (WDG_SW_MAJOR_VERSION);
        versioninfo->sw_minor_version = (uint8) (WDG_SW_MINOR_VERSION);
        versioninfo->sw_patch_version = (uint8) (WDG_SW_PATCH_VERSION);
    }

    return;
}
#endif  /* STD_ON == WDG_GET_VERSION_INFO_API */

/*
 * Design: MCAL-5542,MCAL-5621,MCAL-5586,MCAL-5550,MCAL-5618,MCAL-5579,MCAL-5572,MCAL-5544,MCAL-5558,MCAL-5563,MCAL-5548,MCAL-5625
 */
FUNC(void, WDG_CODE) Wdg_Init(
    P2CONST(Wdg_ConfigType, AUTOMATIC, WDG_APPL_CONST) CfgPtr)
{
    Std_ReturnType retVal;
    const Wdg_ConfigType *ConfigPtr = CfgPtr;

#if (STD_ON == WDG_VARIANT_PRE_COMPILE)
   if (NULL_PTR == ConfigPtr)
{
    ConfigPtr = &WDG_INIT_CONFIG_PC;

}
#endif /*(STD_ON == WDG_VARIANT_PRE_COMPILE)*/

#if (STD_ON == WDG_DEV_ERROR_DETECT)
    if (NULL_PTR == ConfigPtr)
    {
        (void) Wdg_reportDetError(WDG_API_INIT, WDG_E_PARAM_POINTER);
    }
    else
#endif
    {
#if (STD_OFF == WDG_DISABLE_ALLOWED)
        if (WDGIF_OFF_MODE == ConfigPtr->defaultMode)
        {
            /* Disable Watchdog Not Allowed */
            Wdg_reportDemError(WDG_E_DISABLE_REJECTED, DEM_EVENT_STATUS_FAILED);
        }
        else
#endif
        {
#if (STD_OFF == WDG_DISABLE_ALLOWED)
            /* Disable Watchdog Not Failed */
            Wdg_reportDemError(WDG_E_DISABLE_REJECTED, DEM_EVENT_STATUS_PASSED);
#endif
            /* Copy the configuration */
            Wdg_copyConfig(&Wdg_DrvObj, ConfigPtr);
            retVal = Wdg_platformInit(&Wdg_DrvObj);
            if (((Std_ReturnType) E_OK) != retVal)
            {
                /* Mode setting failed */
                Wdg_reportDemError(WDG_E_MODE_FAILED, DEM_EVENT_STATUS_FAILED);
            }
            else
            {
                /* Mode setting passed */
                Wdg_reportDemError(WDG_E_MODE_FAILED, DEM_EVENT_STATUS_PASSED);
                Wdg_DrvObj.baseAddr = Wdg_getWdgBaseAddr(
                    Wdg_DrvObj.instanceId);
                if (0U != Wdg_DrvObj.baseAddr)
                {
                    /* Set Initial Trigger Timeout */
                    Wdg_DrvObj.timeOutCounter = Wdg_DrvObj.initialTimeOut;
                    /* Save initial watchdog counter value */
                    Wdg_DrvObj.counterRef =
                        Wdg_getCurrentCounter(Wdg_DrvObj.baseAddr);
                    /* Enable Counter */
                    Wdg_counterEnable(Wdg_DrvObj.baseAddr);
                }
#if (STD_ON == WDG_DEV_ERROR_DETECT)
                else
                {
                    /* WDG_CoverageGap_01: Dynamic Code coverage for this statement is not covered
                     * because this part of code alredy covered in above Wdg_platformInit()
                     */
                    (void) Wdg_reportDetError(WDG_API_INIT, WDG_E_PARAM_CONFIG);
                    retVal = (Std_ReturnType)E_NOT_OK;
                }
#endif

#if (STD_ON == WDG_DEV_ERROR_DETECT)
                if ((Std_ReturnType) E_OK == retVal)
                {
                    Wdg_DrvStatus = WDG_IDLE;
                }
#endif
            }
        }
    }

    return;
}

/*
 * Design: MCAL-5592,MCAL-5593,MCAL-5590,MCAL-5571,MCAL-5555
 */
FUNC(void, WDG_CODE) Wdg_SetTriggerCondition(
    uint16 timeout)
{
#if (STD_ON == WDG_DEV_ERROR_DETECT)
    if ((uint16) WDG_MAX_TIMEOUT < timeout)
    {
        /* API is being called with an invalid timeout value */
        (void) Wdg_reportDetError(WDG_SET_TRIGGER_CONDITION,
                                                WDG_E_PARAM_TIMEOUT);
    }
    else
#endif
    {
        /* Update trigger condition */
        Wdg_DrvObj.timeOutCounter = (uint32) timeout;
        if (0U == timeout)
        {
            /* Cause a WD reset if timeout is set as 0 */
            (void)Wdg_generateSysReset(Wdg_DrvObj.baseAddr);
        }
        else
        {
            /* Save current watchdog counter value */
            Wdg_DrvObj.counterRef =
                Wdg_getCurrentCounter(Wdg_DrvObj.baseAddr);
        }
    }

    return;
}

/*
 * Design: MCAL-5603,MCAL-5623,MCAL-5604,MCAL-5543,MCAL-5594,MCAL-5569,MCAL-5612,MCAL-5551,MCAL-5614,MCAL-5598
 */
FUNC(void, WDG_CODE) Wdg_Trigger(void)
{
    uint32 elapsedTime;
    uint32 elapsedClkCnt;

#if (STD_ON == WDG_DEV_ERROR_DETECT)
    /* Check if driver already initialized */
    if (WDG_IDLE != Wdg_DrvStatus)
    {
        (void) Wdg_reportDetError(WDG_API_TRIGGER, WDG_E_DRIVER_STATE);
    }
    else
#endif
    {
#if (STD_ON == WDG_DEV_ERROR_DETECT)
        /* Set driver status as busy */
        Wdg_DrvStatus = WDG_BUSY;
#endif

        /* If Timeout counter hasn't expired, continue trigger routine */
        if (0U < Wdg_DrvObj.timeOutCounter)
        {
            elapsedClkCnt =
                Wdg_getCurrentCounter(Wdg_DrvObj.baseAddr);
            /* Counter reference shall not be smaller than current WDW
             * counter as it is Down Counter */
            if (Wdg_DrvObj.counterRef > elapsedClkCnt)
            {
                /* Calculate elapsed time since last trigger invocation */
                elapsedClkCnt =
                    Wdg_DrvObj.counterRef - elapsedClkCnt;
                /* Convert elapsed time into milliseconds */
                elapsedTime =
                    ((elapsedClkCnt  * 1000U) / WDG_RTI_FREQUENCY);

                /* Elapsed time is smaller than the last received trigger
                 * condition */
                if (Wdg_DrvObj.timeOutCounter > elapsedTime)
                {
                    /* Write key sequence to reload the DWD */
                    (void)Wdg_service(Wdg_DrvObj.baseAddr);
                    /* Decrement Counter */
                    Wdg_DrvObj.timeOutCounter =
                        (Wdg_DrvObj.timeOutCounter - elapsedTime);
                    /* update the counter reference */
                    Wdg_DrvObj.counterRef =
                        Wdg_getReloadValue(Wdg_DrvObj.baseAddr);
                }
                /* WDG_CoverageGap_02: Dynamic Code coverage for this statement 
                 * is not covered for Wdg_Trigger() as Elapsed time is smaller than received trigger.
                 */
                else
                {
                    /* Elapsed time is larger than last received trigger
                     * condition, stop servicing watchdog */
                    Wdg_DrvObj.timeOutCounter = INIT_VAL_ZERO;
                }
            }
        }
#if (STD_ON == WDG_DEV_ERROR_DETECT)
        /* Set driver status to idle */
        Wdg_DrvStatus = WDG_IDLE;
#endif
    }
}

/* Dummy Function, just to return E_OK if called from WDG IF layer */
FUNC(Std_ReturnType, WDG_CODE) Wdg_SetMode(
    WdgIf_ModeType Mode)
{

    return (E_OK);
}

/*
 * Design: MCAL-5553,MCAL-5619
 */
#if (STD_ON == WDG_REGISTER_READBACK_API)
FUNC(Std_ReturnType, WDG_CODE) Wdg_RegisterReadback
            (P2VAR(Wdg_RegisterReadbackType, AUTOMATIC,
                         WDG_APPL_DATA) regRbPtr)
{
    Std_ReturnType retVal = (Std_ReturnType) E_NOT_OK;
    uint32 baseAddr;

#if (STD_ON == WDG_DEV_ERROR_DETECT)
    if ((WDG_IDLE != Wdg_DrvStatus) || (NULL_PTR == regRbPtr))
    {
        retVal = (Std_ReturnType) E_NOT_OK;
    }
    else
#endif
    {
        (void) memset(regRbPtr, 0, sizeof (Wdg_RegisterReadbackType));
        baseAddr = Wdg_DrvObj.baseAddr;
        regRbPtr->rtiDwdCtrl     = HW_RD_REG32(baseAddr + RTI_RTIDWDCTRL);
        regRbPtr->rtiDwdprld     = HW_RD_REG32(baseAddr + RTI_RTIDWDPRLD);
        regRbPtr->rtiWdStatus    = HW_RD_REG32(baseAddr + RTI_RTIWDSTATUS);
        regRbPtr->rtiWdKey       = HW_RD_REG32(baseAddr + RTI_RTIWDKEY);
        regRbPtr->rtiWwdRxnCtrl  = HW_RD_REG32(baseAddr + RTI_RTIDWWDRXNCTRL);
        regRbPtr->rtiWwdSizeCtrl = HW_RD_REG32(baseAddr + RTI_RTIDWWDSIZECTRL);

        retVal = (Std_ReturnType) E_OK;
    }

    return(retVal);
}
#endif /*STD_ON == WDG_REGISTER_READBACK_API*/

#if (STD_ON == WDG_DEV_ERROR_DETECT)
void Wdg_reportDetError(uint8 apiId, uint8 errorId)
{
    (void) Det_ReportError(WDG_MODULE_ID, WDG_INSTANCE_ID, apiId, errorId);
    return;
}
#endif  /* #if (STD_ON == WDG_DEV_ERROR_DETECT) */

void Wdg_reportDemError(Dem_EventIdType     eventId,
                           Dem_EventStatusType eventStatus)
{

    if(eventId == (uint16)WDG_E_DISABLE_REJECTED)
    {
#if (WDG_E_DISABLE_REJECTED != WDG_DEM_NO_EVENT)
        /* WDG can't be disabled */
        (void)Dem_SetEventStatus(eventId, eventStatus);
#else
        /* Disable Rejected error not assigned to any DEM event */
#endif
    }

    if(eventId == (uint16)WDG_E_MODE_FAILED)
    {
#if (WDG_E_MODE_FAILED != WDG_DEM_NO_EVENT)
        /* WDG can't be disabled */
        (void)Dem_SetEventStatus(eventId, eventStatus);
#else
        /* Disable mode failed error not assigned to any DEM event */
#endif
    }

    return;
}

static FUNC(void, WDG_CODE) Wdg_resetDrvObj(
    volatile Wdg_DriverObjType *drvObj)
{

    drvObj->baseAddr = INIT_VAL_ZERO;
    drvObj->timeOutCounter = INIT_VAL_ZERO;
    drvObj->counterRef = INIT_VAL_ZERO;

    drvObj->defaultMode = WDGIF_FAST_MODE;
    drvObj->instanceId = INIT_VAL_ZERO;
    drvObj->initialTimeOut = INIT_VAL_ZERO;

    drvObj->fastModeCfg.reaction = INIT_VAL_ZERO;
    drvObj->fastModeCfg.windowSize = WDG_WINDOW_100_PERCENT;
    drvObj->fastModeCfg.timeOutVal = INIT_VAL_ZERO;

    drvObj->slowModeCfg.reaction = INIT_VAL_ZERO;
    drvObj->slowModeCfg.windowSize = WDG_WINDOW_100_PERCENT;
    drvObj->slowModeCfg.timeOutVal = INIT_VAL_ZERO;

    return;
}

static FUNC(void, WDG_CODE) Wdg_copyConfig(
    volatile Wdg_DriverObjType *drvObj, const Wdg_ConfigType *cfgPtr)
{

    /* Reset Driver Object */
    Wdg_resetDrvObj(drvObj);

    /* Initialize Driver object */
    drvObj->defaultMode = cfgPtr->defaultMode;

    /* Initialize Fast mode config */
    drvObj->fastModeCfg.reaction = cfgPtr->fastModeCfg.reaction;
    drvObj->fastModeCfg.windowSize = cfgPtr->fastModeCfg.windowSize;
    drvObj->fastModeCfg.timeOutVal = cfgPtr->fastModeCfg.timeOutVal;

    /* Initialize Slow mode config */
    drvObj->slowModeCfg.reaction = cfgPtr->slowModeCfg.reaction;
    drvObj->slowModeCfg.windowSize = cfgPtr->slowModeCfg.windowSize;
    drvObj->slowModeCfg.timeOutVal = cfgPtr->slowModeCfg.timeOutVal;

    /* Initialize Pre compile config params */
    drvObj->instanceId = WdgSettingsConfig_PC.instanceId;
    drvObj->initialTimeOut = WdgSettingsConfig_PC.initialTimeOut;
    return;
}

#define WDG_STOP_SEC_CODE
#include "Wdg_MemMap.h"
