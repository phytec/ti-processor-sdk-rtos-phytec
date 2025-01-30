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
 *  \file     Wdg_priv.h
 *
 *  \brief    This file contains Wdg MCAL driver private include file
 */

#ifndef WDG_PRIV_H_
#define WDG_PRIV_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "Std_Types.h"
#include "Wdg.h"
#if (WDG_DEV_ERROR_DETECT == STD_ON)
#include "Det.h"
#endif
#include "Dem.h"
#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
/* None */

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/**
 *  \brief WDG driver object structure.
 */
typedef struct
{
    uint32 baseAddr;
    /**< Base address of the hardware */
    uint32 timeOutCounter;
    /**< Watchdog timeOutCounter value */
    uint32 counterRef;
    /**< Counter refence value for trigge */
    WdgIf_ModeType   defaultMode;
    /**< Default mode */
    uint16           instanceId;
    /**< Instance ID */
    uint32           initialTimeOut;
    /**< Initial timeout */
    Wdg_ModeInfoType fastModeCfg;
    /**< Fast mode config */
    Wdg_ModeInfoType slowModeCfg;
    /**< Slow mode config */
} Wdg_DriverObjType;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

FUNC(Std_ReturnType, WDG_CODE) Wdg_platformInit(
    P2CONST(volatile Wdg_DriverObjType, AUTOMATIC, WDG_APPL_CONST) drvObj);
FUNC(uint32, WDG_CODE) Wdg_getCurrentCounter(uint32 baseAddr);
FUNC(void, WDG_CODE) Wdg_counterEnable(uint32 baseAddr);
FUNC(Std_ReturnType, WDG_CODE) Wdg_generateSysReset(uint32 baseAddr);
FUNC(Std_ReturnType, WDG_CODE) Wdg_service(uint32 baseAddr);
FUNC(uint32, WDG_CODE) Wdg_getWdgBaseAddr(uint16 regNum);
FUNC(uint32, WDG_CODE) Wdg_getReloadValue(uint32 baseAddr);
#if (STD_ON == WDG_DEV_ERROR_DETECT)
void Wdg_reportDetError(uint8 apiId, uint8 errorId);
#endif
void Wdg_reportDemError(Dem_EventIdType     eventId,
                           Dem_EventStatusType eventStatus);
#ifdef __cplusplus
}
#endif

#endif  /* #ifndef WDG_PRIV_H_ */
