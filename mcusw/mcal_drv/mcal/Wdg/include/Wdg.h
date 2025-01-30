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
 *  \file     Wdg.h
 *
 *  \brief    This file contains interface header for WDG MCAL driver
 *
 */

/**
 *  \defgroup MCAL_WDG_API WDG Driver API
 *
 *  The WDG driver provides services for handlign the watch dog timer on the
 *  SOC.
 *
 *  \verbatim
 *        Digital Windowed Watchdog(DWWD) Overview :
 *        The Digital Watchdog Timer(DWT) generates reset after a programmable
 *        period, if not serviced within that period. In DWT, time-out
 *        boundary is configurable.
 *        In DWWD, along with configurable time-out boundary, the start time
 *        boundary is also configurable. The DWWD can generate Reset or
 *        Interrupt, if not serviced within window(Open Window) defined by
 *        start time and time-out boundary. Also the DWWD can generate Reset or
 *        Interrupt if serviced outside Open Window (within Closed Window).
 *        Generation of Reset or Interrupt depends on the DWWD Reaction
 *        configuration.
 *
 *        DWWD Down Counter Overview :
 *        Upper 12 bit part of the down counter is configurable and
 *        remaining 13 bit are always 1.
 *        Minimum possible time-out value is 2^13 RTI clock cycles.
 *        Maximum possible time-out value is 2^25 RTI clock cycles.
 *        Example :
 *        RTI frequency : 32kHz
 *        12 bit preload value : 0x004
 *        25 bit preload value : 0x0009FFF
 *        time-out value(in RTI clock cycles) : 40959.
 *        time-out value(in seconds) : 40959 + 1 / 32000 = 1.28 seconds.
 *
 *
 *        DWWD Window Sizes Overview :
 *         time-out value...........................................3 2 1 0
 *   (100%)|___________________________Open Window________________________|
 *    (50%)|         Closed Window         |________Open Window___________|
 *    (25%)|            Closed Window                      |___OW_________|
 *  (12.5%)|                 Closed Window                         |_OW___|
 *  (6.25%)|                     Closed Window                        |OW |
 * (3.125%)|                         Closed Window                     |OW|
 *        For time-out value calculation refer DWWD Down Counter Overview.
 *
 *
 *  \endverbatim
 *
 *  @sa MCAL_WDG_CFG
 *  @{
 */

/*
 * Below are the global design requirements which are met by this WDG
 * which can't be mapped to a particular source ID
 */
/*
 * Design: MCAL-5599,MCAL-5558,MCAL-5603,MCAL-5559,MCAL-5560,MCAL-5568,MCAL-5564,MCAL-5547
 */

#ifndef WDG_H_
#define WDG_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

/*
 * Design: MCAL-5557
 */
#include "WdgIf_Types.h"
/*
 * Design: MCAL-5573
 */
#include "Wdg_Cfg.h"
#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 *  \name WDG Driver Module SW Version Info
 *
 *  Defines for WDG Driver version used for compatibility checks
 *  @{
 */
/** \brief Driver Implementation Major Version */
#define WDG_SW_MAJOR_VERSION            (9U)
/** \brief Driver Implementation Minor Version */
#define WDG_SW_MINOR_VERSION            (0U)
/** \brief Driver Implementation Patch Version */
#define WDG_SW_PATCH_VERSION            (0U)
/* @} */

/**
 *  \name WDG Driver Module AUTOSAR Version Info
 *
 *  Defines for WDG Driver AUTOSAR version used for compatibility checks
 *  @{
 */
/** \brief AUTOSAR Major version specification implemented by WDG Driver */
#define WDG_AR_RELEASE_MAJOR_VERSION    (4U)
/** \brief AUTOSAR Minor  version specification implemented by WDG Driver */
#define WDG_AR_RELEASE_MINOR_VERSION    (3U)
/** \brief AUTOSAR Patch version specification implemented by WDG Driver */
#define WDG_AR_RELEASE_REVISION_VERSION (1U)
/* @} */

/**
 *  \name WDG Driver ID Info
 *  @{
 */
/** \brief Texas Instruments Vendor ID */
#define WDG_VENDOR_ID                   ((uint16) 44U)
/** \brief WDG Driver Module ID */
#define WDG_MODULE_ID                   ((uint16) 102U)
/** \brief WDG Driver Instance ID */
#define WDG_INSTANCE_ID                 ((uint8) 0U)
/* @} */

/*
 * Design: MCAL-5591
 */
/**
 *  \name WDG Error Codes
 *
 *  Error codes returned by Driver functions
 *  @{
 */
/** \brief ERROR:Invalid driver state */
#define WDG_E_DRIVER_STATE              ((uint8) 0x10U)
/** \brief ERROR:Invalid mode param */
#define WDG_E_PARAM_MODE                ((uint8) 0x11U)
/** \brief ERROR:Invalid config param */
#define WDG_E_PARAM_CONFIG              ((uint8) 0x12U)
/** \brief ERROR:Invalid timeout value */
#define WDG_E_PARAM_TIMEOUT             ((uint8) 0x13U)
/** \brief ERROR:Invalid NULL ptr param */
#define WDG_E_PARAM_POINTER             ((uint8) 0x14U)
/** \brief ERROR:Invalid configuration set selection */
#define WDG_E_INIT_FAILED               ((uint8) 0x15U)
/* @} */

/*
 * Design: MCAL-5604
 */
/**
 *  \name WDG Service Ids
 *
 *  The Service Id is one of the argument to Det_ReportError function and is
 *  used to identify the source of the error
 *  @{
 */
/** \brief Wdg_Init() */
#define WDG_API_INIT                   ((uint8) 0x00U)
/** \brief Wdg_SetMode() */
#define WDG_API_SET_MODE               ((uint8) 0x01U)
/** \brief Wdg_SetTriggerCondition() */
#define WDG_SET_TRIGGER_CONDITION      ((uint8) 0x03U)
/** \brief Wdg_GetVersionInfo() */
#define WDG_API_GET_VERSION_INFO       ((uint8) 0x04U)
/** \brief Wdg_Trigger() */
#define WDG_API_TRIGGER                ((uint8) 0x05U)
/** \brief Wdg_RegisterReadback() */
#define WDG_API_REGISTER_READBACK      ((uint8) 0x06U)
/* @} */

/**
 *  \name Watchdog Window Size
 *
 *  Windowed watchdog trigger window size define
 *  @{
 */
/** \brief Window open 100% of interval */
#define WDG_WINDOW_100_PERCENT              ((uint32) 0x00000005U)
/** \brief Window open 50%  of interval */
#define WDG_WINDOW_50_PERCENT               ((uint32) 0x00000050U)
/** \brief Window open 25%  of interval */
#define WDG_WINDOW_25_PERCENT               ((uint32) 0x00000500U)
/** \brief Window open 12.5% of interval */
#define WDG_WINDOW_12P5_PERCENT             ((uint32) 0x00005000U)
/** \brief Window open 6.25% of interval */
#define WDG_WINDOW_6P25_PERCENT             ((uint32) 0x00050000U)
/** \brief Window open 3.125% of interval */
#define WDG_WINDOW_3P125_PERCENT             ((uint32) 0x00500000U)
/* @} */

/** \brief For assigning use macro name instead of value 0U to avoid MISRAC violation */
#define INIT_VAL_ZERO                   (0U)

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */
/**
 *  \addtogroup MCAL_WDG_CFG WDG Configuration
 *  @{
 */

/**
 *  \brief  Watchdog driver state
 *
 *  The WDG state is used internally to check that Watchdog trigger happens
 *  only in IDLE state.Also additional check to check Wdg_Init is invoked
 *  before any other Wdg API is called.
 */
typedef enum
{
    WDG_UNINIT,
    /**< Watchdog driver is uninitialized. State on power on before
     *   invoking Wdg_Init() */
    WDG_IDLE,
    /**< Watchdog driver is in idle state. Default state when WDG
     *   timer regs are not being programmed */
    WDG_BUSY
    /**< Watchdog driver is reprogramming the WD to trigger the
     *   watchdog */
} Wdg_StatusType;

/** \brief Mode info type */
typedef struct
{
    uint32 reaction;
    /**< Reaction type
     *   0x5 - This value causes a reset if the watchdog is serviced
     *   outside the time window defined by the configuration,
     *   or if the watchdog is not serviced at all.
     *   0xA - The windowed watchdog will generate a non-maskable
     *   interrupt to the CPU if the watchdog is serviced outside the
     *   time window defined by the configuration,
     *   or if the watchdog is not serviced at all. */
    uint32 windowSize;
    /**< Different Window Sizes
     *   Refer Watchdog Window Size section above for different Window sizes */
    uint32 timeOutVal;
    /**< Watchdog timeout value in milli seconds */
} Wdg_ModeInfoType;

/*
 * Design: MCAL-5587
 */
/** \brief WDG config structure */
typedef struct Wdg_ConfigType_s
{
    WdgIf_ModeType   defaultMode;
    /**< Default mode */
    Wdg_ModeInfoType fastModeCfg;
    /**< Fast mode config */
    Wdg_ModeInfoType slowModeCfg;
    /**< Slow mode config */
} Wdg_ConfigType;

/*
 * Design: MCAL-5609,MCAL-5556
 */
/** \brief WDG pre-compile config structure */
typedef struct Wdg_ConfigType_PC_s
{
    uint16           instanceId;
    /**< Hardware Instance ID */
    uint32           initialTimeOut;
    /**< The initial timeout (seconds) for the trigger condition to be
      *  initialized during Init function. It shall not be > WdgMaxTimeout. */
} Wdg_ConfigType_PC;

#if (STD_ON == WDG_REGISTER_READBACK_API)
typedef struct
{
   uint32 rtiDwdCtrl;
    /**< Digital Watchdog Control, To Enable DWD */
   uint32 rtiDwdprld;
    /**< Digital Watchdog Preload Value */
   uint32 rtiWdStatus;
    /**< Digital Watchdog Status Register */
   uint32 rtiWdKey;
    /**< Digital Watchdog Key */
   uint32 rtiWwdRxnCtrl;
    /**< Digital Windowed Watchdog Reaction */
   uint32 rtiWwdSizeCtrl;
    /**< Digital Windowed Watchdog Window Size */
}Wdg_RegisterReadbackType;
#endif /* STD_ON == WDG_REGISTER_READBACK_API */

/* @} */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

#if (STD_ON == WDG_GET_VERSION_INFO_API)
/**
 *  \brief This service returns the version information of this module
 *
 *  \verbatim
 *  Service name      : Wdg_GetVersionInfo
 *  Syntax            : void Wdg_GetVersionInfo(Std_VersionInfoType*
 *                                  versioninfo)
 *  Mode              : User Mode (Non-Privileged Mode)
 *  Service ID[hex]   : 0x04U
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Reentrant
 *  Parameters (in)   : None
 *  Parameters (inout): None
 *  Parameters (out)  : versioninfo - Pointer to where to store the version
 *                      information of this module.
 *  Return value      : None
 *  Description       : This service returns the version information of this
 *                      module
 *  \endverbatim
 */
FUNC(void, WDG_CODE) Wdg_GetVersionInfo(Std_VersionInfoType * versioninfo);
#endif

/**
 *  \brief  This service initializes the WDG driver
 *
 *  \verbatim
 *  Service name      : Wdg_Init
 *  Syntax            : void Wdg_Init(const Wdg_ConfigType* CfgPtr)
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   : 0x0
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Non Reentrant
 *  Parameters (in)   : CfgPtr - Pointer to WDG driver configuration set.
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : None
 *  Description       : This service initializes the WDG driver
 *  \endverbatim
 */
FUNC(void, WDG_CODE) Wdg_Init(
    P2CONST(Wdg_ConfigType, AUTOMATIC, WDG_APPL_CONST) CfgPtr);
/**
 *  \brief  Sets the timeout value for the trigger counter
 *
 *  \verbatim
 *  Service name      : Wdg_SetTriggerCondition
 *  Syntax            : void Wdg_SetTriggerCondition( uint16 timeout )
 *  Mode              : Supervisor Mode (Privileged Mode)
 *  Service ID[hex]   : 0x03
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Non Reentrant
 *  Parameters (in)   : timeout - Timeout value (milliseconds) for setting the
 *                      trigger counter
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : None
 *  Description       : Sets the timeout value for the trigger counter
 *  \endverbatim
 */
FUNC(void, WDG_CODE) Wdg_SetTriggerCondition(uint16 timeout);
/**
 *  \brief This function triggers the servicing of the watchdog
 *
 *  \verbatim
 *   Function name     : Wdg_Trigger
 *   Syntax            : void Wdg_Trigger( void )
 *   Mode              : Supervisor Mode (Privileged Mode)
 *   Service ID[hex]   : 0x05
 *   Sync/Async        : Synchronous
 *   Reentrancy        : Non-Reentrant
 *   Parameters (in)   : None
 *   Parameters (inout): None
 *   Parameters (out)  : None
 *   Return value      : None
 *   Description       : This function is the watchdog trigger
 *                       This function is invoked from WdgIsr
 *  \endverbatim
 */
FUNC(void, WDG_CODE) Wdg_Trigger(void);
/**
 *  \brief This function triggers the servicing of the watchdog
 *
 *  \verbatim
 *   Function name     : Wdg_SetMode
 *   Syntax            : Std_ReturnType Wdg_SetMode(WdgIf_ModeType Mode)
 *   Mode              : User Mode (Non-Privileged Mode)
 *   Service ID[hex]   : 0x01
 *   Sync/Async        : Synchronous
 *   Reentrancy        : Non-Reentrant
 *   Parameters (in)   : Mode
 *   Parameters (inout): None
 *   Parameters (out)  : None
 *   Return value      : E_OK
 *   Description       : This function is not supported.
 *                       Always returns E_OK, Wdg_SetMode is made available because
 *                       WDG Interface layer expects this function to be present.
 *  \endverbatim
 */
FUNC(Std_ReturnType, WDG_CODE) Wdg_SetMode(WdgIf_ModeType Mode);
#if (STD_ON == WDG_REGISTER_READBACK_API)
/**
 *  \brief This function reads the important registers of the hardware unit
 *  and returns the value in the structure.
 *
 *  This API should be called after Wdg_Init is called. Otherwise this API will
 *  return E_NOT_OK.
 *
 *  This API could be used to readback the register contents after Wdg_Init
 *  and then the readback value could be compared during WDG execution to
 *  check the correctness of the HW unit. Since this API is used for this
 *  purpose, register returned are the ones which doesn't change after init.
 *
 *  \verbatim
 *  Service name        : Wdg_RegisterReadback
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Sync/Async          : Synchronous
 *  Service ID[hex]     : 0x06
 *  Reentrancy          : Reentrant
 *  Parameters (inout)  : regRbPtr - Pointer to where to store the readback
 *                        values. If this pointer is NULL_PTR, then the API
 *                        will return E_NOT_OK.
 *  Return value        : Std_ReturnType
 *                        E_OK: Register read back has been done
 *                        E_NOT_OK: Register read back failed
 *  \endverbatim
 */
FUNC(Std_ReturnType, WDG_CODE) Wdg_RegisterReadback
                            (P2VAR(Wdg_RegisterReadbackType, AUTOMATIC,
                                                WDG_APPL_DATA) regRbPtr);
#endif
#ifdef __cplusplus
}
#endif

#endif  /* #ifndef WDG_H_ */

/* @} */
