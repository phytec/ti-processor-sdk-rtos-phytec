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
 *  \file     Wdg_Priv.c
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "Std_Types.h"
#include "stdint.h"
#include "Wdg.h"
#include "Wdg_Priv.h"
#include <hw_include/soc.h>
#define WDG_START_SEC_CODE
#include "Wdg_MemMap.h"
#include <hw_include/hw_types.h>
#define WDG_STOP_SEC_CODE
#include "Wdg_MemMap.h"

#include <hw_include/lldr_rti.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 *  \brief Minimum possible preload value for RTI DWD counter (2^13).
 */
#define WDG_DWD_MIN_PRELOAD_VAL         (0x1FFFU)

/**
 *  \brief Maximum possible preload value for RTI DWD counter(2^25).
 */
#define WDG_DWD_MAX_PRELOAD_VAL         (0x1FFFFFFU)

#define WDG_DWWDPRLD_MULTIPLIER_SHIFT   (13U)

#define WDG_RTIWDKEY_WDKEY_INVALID_WRITE    (0x4444U)

/* Digital Watchdog Control */
#define WDG_CTL_ENABLED                 ((uint32) 0xA98559DAU)
#define WDG_CTL_DISABLED                ((uint32) 0x5312ACEDU)

/* Watchdog Triggers */
#define WDG_TRIGGER_FIRST_KEY           ((uint32) 0xE51AU)
#define WDG_TRIGGER_SECOND_KEY          ((uint32) 0xA35CU)
#define WDG_TRIGGER_RESET_KEY           ((uint32) 0x2345U)

/** \brief Watchdog Max Preload Value */
#define WDG_MAX_PRELOAD_VALUE       (0xFFFU)

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

#if (WDG_DEV_ERROR_DETECT == STD_ON)
static FUNC(uint16, WDG_CODE) Wdg_getNumWdg(void);
#endif
static FUNC(void, WDG_CODE) Wdg_setPreload(uint32 baseAddr,
                                           uint32 dwwdPreloadVal);
static FUNC(void, WDG_CODE) Wdg_setReaction(uint32 baseAddr,
                                            uint32 dwwdReaction);
static FUNC(Std_ReturnType, WDG_CODE) Wdg_windowConfig(uint32 baseAddr,
                                                       uint32 dwwdReaction,
                                                       uint32 timeoutVal,
                                                       uint32 dwwdWindowSize);
static FUNC(void, WDG_CODE) Wdg_clearStatus(uint32 baseAddr);
static FUNC(uint32, WDG_CODE) Wdg_getPreloadValue(uint32 timeoutVal);
static FUNC(void, WDG_CODE) Wdg_setWindowSize(uint32 baseAddr,
                                              uint32 dwwdWindowSize);
/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/*none*/

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */
#define WDG_START_SEC_CODE
#include "Wdg_MemMap.h"

#if (WDG_DEV_ERROR_DETECT == STD_ON)
static FUNC(uint16, WDG_CODE) Wdg_getNumWdg(void)
{
    return ((uint16)WDG_NUM_INSTANCE);
}
#endif

static FUNC(void, WDG_CODE) Wdg_setPreload(uint32 baseAddr,
                                           uint32 dwwdPreloadVal)
{
    /* Initialize DWD Expiration Period */
    HW_WR_FIELD32(baseAddr + RTI_RTIDWDPRLD,
                  RTI_RTIDWDPRLD_DWDPRLD,
                  dwwdPreloadVal);
}

static FUNC(void, WDG_CODE)
    Wdg_setReaction(uint32 baseAddr, uint32 dwwdReaction)
{
    /* Configuring Digital Windowed Watchdog Reaction */
    HW_WR_FIELD32(baseAddr + RTI_RTIDWWDRXNCTRL,
                  RTI_RTIDWWDRXNCTRL_DWWDRXN,
                  dwwdReaction);
}

static FUNC(uint32, WDG_CODE)
    Wdg_getPreloadValue(uint32 timeoutVal)
{
    uint32 timeoutNumCycles = 0U;

    /* Get the clock ticks for given time-out value
     * Divide by 1000 as timeoutVal is in ms) */
    timeoutNumCycles = ((WDG_RTI_FREQUENCY / 1000U) * timeoutVal);
    return timeoutNumCycles;
}

static FUNC(void, WDG_CODE)
    Wdg_setWindowSize(uint32 baseAddr, uint32 dwwdWindowSize)
{
    /* Configure window in which watch-dog should be serviced */
    HW_WR_FIELD32(baseAddr + RTI_RTIDWWDSIZECTRL,
                  RTI_RTIDWWDSIZECTRL_DWWDSIZE,
                  dwwdWindowSize);
}

static FUNC(Std_ReturnType, WDG_CODE)
    Wdg_windowConfig(uint32 baseAddr, uint32 dwwdReaction, uint32 timeoutVal,
    uint32 dwwdWindowSize)
{
    Std_ReturnType configStatus;
    uint32         dwwdPreloadValReg;
    uint32         dwwdPreloadVal;

    /* Get PreLoad Value */
    dwwdPreloadVal = Wdg_getPreloadValue(timeoutVal);

    dwwdPreloadValReg = (dwwdPreloadVal >>
                         WDG_DWWDPRLD_MULTIPLIER_SHIFT);

    if (WDG_MAX_PRELOAD_VALUE > dwwdPreloadValReg)
    {
        /* Configure window in which watch-dog should be serviced */
        Wdg_setWindowSize(baseAddr, dwwdWindowSize);
        /* Initialize DWD Expiration Period */
        Wdg_setPreload(baseAddr, dwwdPreloadValReg);
        /* Configure Digital Windowed Watchdog Reaction */
        Wdg_setReaction(baseAddr, dwwdReaction);
        configStatus = (Std_ReturnType) E_OK;
    }
    else
    {
        configStatus = E_NOT_OK;
    }

    return (configStatus);
}

static FUNC(void, WDG_CODE)
    Wdg_clearStatus(uint32 baseAddr)
{
    uint32 intrStatus = HW_RD_REG32(baseAddr + RTI_RTIWDSTATUS);
    HW_WR_REG32(baseAddr + RTI_RTIWDSTATUS, intrStatus);

    return;
}

FUNC(Std_ReturnType, WDG_CODE)
    Wdg_generateSysReset(uint32 baseAddr)
{
    Std_ReturnType retVal = (Std_ReturnType) E_OK;

    /* Generate system reset by writing value other than pre-defined
     *  second write value '0x0000A35C' to RTI_RTIWDKEY */
    HW_WR_FIELD32(baseAddr + RTI_RTIWDKEY,
                  RTI_RTIWDKEY_WDKEY,
                  RTI_RTIWDKEY_WDKEY_FIRST_WRITE);
    HW_WR_FIELD32(baseAddr + RTI_RTIWDKEY,
                  RTI_RTIWDKEY_WDKEY,
                  WDG_RTIWDKEY_WDKEY_INVALID_WRITE);

    return retVal;
}

FUNC(uint32, WDG_CODE) Wdg_getCurrentCounter(
    uint32 baseAddr)
{
    return HW_RD_REG32(baseAddr + RTI_RTIDWDCNTR);
}

/* This function returns value which is loaded into the down counter */
FUNC(uint32, WDG_CODE) Wdg_getReloadValue(uint32 baseAddr)
{
    uint32 dwwdPreloadVal;
    dwwdPreloadVal = HW_RD_FIELD32(baseAddr + RTI_RTIDWDPRLD,
                  RTI_RTIDWDPRLD_DWDPRLD);
    dwwdPreloadVal = (dwwdPreloadVal + 1U) << WDG_DWWDPRLD_MULTIPLIER_SHIFT;
    return(dwwdPreloadVal);
}

FUNC(void, WDG_CODE) Wdg_counterEnable(
    uint32 baseAddr)
{
    /* Enable DWWD by writing pre-defined value '0xA98559DA' to RTIDWDCTRL    */
    HW_WR_REG32(baseAddr + RTI_RTIDWDCTRL,
                RTI_RTIDWDCTRL_DWDCTRL_ENABLE);

    return;
}

FUNC(uint32, WDG_CODE) Wdg_getWdgBaseAddr(uint16 regNum)
{
    uint32 baseAddr;

    if (regNum >= (uint16) WDG_NUM_INSTANCE)
    {
        baseAddr = INIT_VAL_ZERO;
    }
    else
    {
        baseAddr = RTIHwUnit_BaseAddr[regNum];
    }

    return (baseAddr);
}

FUNC(Std_ReturnType, WDG_CODE) Wdg_service(uint32 baseAddr)
{
    Std_ReturnType retVal = (Std_ReturnType) E_OK;

    HW_WR_FIELD32(baseAddr + RTI_RTIWDKEY,
                  RTI_RTIWDKEY_WDKEY,
                  RTI_RTIWDKEY_WDKEY_FIRST_WRITE);
    HW_WR_FIELD32(baseAddr + RTI_RTIWDKEY,
                  RTI_RTIWDKEY_WDKEY,
                  RTI_RTIWDKEY_WDKEY_SECOND_WRITE);

    return retVal;
}

/*
 * Design: MCAL-5627,MCAL-5577
 */
FUNC(Std_ReturnType, WDG_CODE) Wdg_platformInit(
    P2CONST(volatile Wdg_DriverObjType, AUTOMATIC, WDG_APPL_CONST) drvObj)
{
    Std_ReturnType status;
    uint32         baseAddr;

#if (WDG_DEV_ERROR_DETECT == STD_ON)
    if ((drvObj->instanceId >= Wdg_getNumWdg()))
    {
        (void) Wdg_reportDetError(WDG_API_INIT, WDG_E_PARAM_CONFIG);
        status = (Std_ReturnType) E_NOT_OK;
    }
    else
#endif
    {
        baseAddr = Wdg_getWdgBaseAddr(drvObj->instanceId);

        /* Clear any previous violations */
        Wdg_clearStatus(baseAddr);
        if (drvObj->defaultMode == WDGIF_FAST_MODE)
        {
            status = Wdg_windowConfig(baseAddr,
                                        drvObj->fastModeCfg.reaction,
                                        drvObj->fastModeCfg.timeOutVal,
                                        drvObj->fastModeCfg.windowSize);
        }
        else
        {
            /* WDGIF_SLOW_MODE */
            status = Wdg_windowConfig(baseAddr,
                                        drvObj->slowModeCfg.reaction,
                                        drvObj->slowModeCfg.timeOutVal,
                                        drvObj->slowModeCfg.windowSize);
        }
    }

    return (status);
}

#define WDG_STOP_SEC_CODE
#include "Wdg_MemMap.h"
