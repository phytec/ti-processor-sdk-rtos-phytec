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
 *  \file     Mcu.h
 *
 *  \brief    This file contains interface header for MCU MCAL driver
 *
 */

/**
 *  \defgroup MCAL_MCU_API MCU Driver API
 *
 *
 *  @{
 */

#ifndef MCU_H_
#define MCU_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "Std_Types.h"
#include "Mcu_Cfg.h"
#include "EcuM.h"


#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

#define ARRAYSIZE(array)             ((sizeof (array) / \
                                         sizeof ((array)[0])))


/**
 *  \name MCU Driver Module SW Version Info
 *
 *  Defines for MCU Driver version used for compatibility checks
 *  @{
 */
/** \brief Driver Implementation Major Version */
#define MCU_SW_MAJOR_VERSION            (9U)
/** \brief Driver Implementation Minor Version */
#define MCU_SW_MINOR_VERSION            (1U)
/** \brief Driver Implementation Patch Version */
#define MCU_SW_PATCH_VERSION            (1U)
/* @} */

/**
 *  \name MCU Driver Module AUTOSAR Version Info
 *
 *  Defines for MCU Driver AUTOSAR version used for compatibility checks
 *  @{
 */
/** \brief AUTOSAR Major version specification implemented by MCU Driver */
#define MCU_AR_RELEASE_MAJOR_VERSION    (4U)
/** \brief AUTOSAR Minor version specification implemented by MCU Driver */
#define MCU_AR_RELEASE_MINOR_VERSION    (3U)
/** \brief AUTOSAR Patch version specification implemented by MCU Driver */
#define MCU_AR_RELEASE_REVISION_VERSION (1U)
/* @} */

/**
 *  \name MCU Driver ID Info
 *  @{
 */
/** \brief Texas Instruments Vendor ID */
#define MCU_VENDOR_ID                   ((uint16) 44U)
/** \brief MCU Driver Module ID */
#define MCU_MODULE_ID                   ((uint16) 101U)
/** \brief MCU Driver Instance ID */
#define MCU_INSTANCE_ID                 ((uint8) 0U)
/* @} */

/**
 *  \name MCU Error Codes
 *
 *  Error codes returned by MCU functions
 *  @{
 */

 /*
  * Design: MCAL-8507,MCAL-8489
  */
 #ifndef MCU_E_PARAM_CONFIG
 /** \brief ERROR Description: API service called with wrong configuration parameter. */
 #define MCU_E_PARAM_CONFIG         (0x0AU)
 #endif


 /*
  * Design: MCAL-8507
  */

#ifndef MCU_E_PARAM_CLOCK
/** \brief ERROR Description: API service used with wrong clock value. */
#define MCU_E_PARAM_CLOCK           (0x0BU)
#endif

/*
 * Design: MCAL-8507
 */
#ifndef MCU_E_PARAM_MODE
/** \brief ERROR Description: API service used with wrong mode. */
#define MCU_E_PARAM_MODE            (0x0CU)
#endif

/*
 * Design: MCAL-8507
 */
#ifndef MCU_E_PARAM_RAMSECTION
/** \brief ERROR Description: API service called with invalid/wrong RAM section. */
#define MCU_E_PARAM_RAMSECTION      (0x0DU)
#endif

/*
 * Design: MCAL-8507
 */
#ifndef MCU_E_PLL_NOT_LOCKED
/** \brief ERROR Description: API service is called while PLL is not locked. */
#define MCU_E_PLL_NOT_LOCKED        (0x0EU)
#endif

/*
 * Design: MCAL-8507
 */
#ifndef MCU_E_UNINIT
/** \brief ERROR Description: Mcu Driver API service is called before initialization of MCU Driver. */
#define MCU_E_UNINIT                (0x0FU)
#endif

/*
 * Design: MCAL-8507
 */
#ifndef MCU_E_PARAM_POINTER
/** \brief ERROR Description: API service is called with NULL/Wrong pointer. */
#define MCU_E_PARAM_POINTER         (0x10U)
#endif

/*
 * Design: MCAL-8507
 */
#ifndef MCU_E_INIT_FAILED
/** \brief ERROR Description: Initialization of the MCU Driver Failed. */
#define MCU_E_INIT_FAILED           (0x11U)
#endif

/**
 *  \name MCU Driver State Values
 *
 *  The MCU Driver State Values
 *  @{
 */
 /*
  * Design: MCAL-8507
  */
/** \brief MCU driver Status uninitialized */
#define MCU_STATUS_UNINIT                   ((uint8)(0U))
/** \brief MCU driver Status Initialized */
#define MCU_STATUS_INIT                     ((uint8)(1U))

/**
 *  \name MCU Service Ids
 *
 *  The Service Id is one of the argument to Det_ReportError function and
 *  is used to identify the source of the error
 *  @{
 */

/** \brief Mcu_Init() API Service ID */
#define MCU_SID_INIT                 (0x00U)
/** \brief Mcu_InitRamSection() API Service ID */
#define MCU_SID_INIT_RAM_SECTION     (0x01U)
/** \brief Mcu_InitClock() API Service ID */
#define MCU_SID_INIT_CLOCK           (0x02U)
/** \brief Mcu_DistributePllClock() API Service ID */
#define MCU_SID_DISTRIBUTE_PLL_CLOCK (0x03U)
/** \brief Mcu_GetPllStatus() API Service ID */
#define MCU_SID_GET_PLL_STATUS       (0x04U)
/** \brief Mcu_GetResetReason() API Service ID */
#define MCU_SID_GET_RESET_REASON     (0x05U)
/** \brief Mcu_GetResetRawValue() API Service ID */
#define MCU_SID_GET_RESET_RAW_VALUE  (0x06U)
/** \brief Mcu_PerformReset() API Service ID */
#define MCU_SID_PERFORM_RESET        (0x07U)
/** \brief Mcu_SetMode() API Service ID */
#define MCU_SID_SETMODE              (0x08U)
/** \brief Mcu_GetVersionInfo() API Service ID */
#define MCU_SID_GET_VERSION_INFO     (0x09U)
/** \brief  Mcu_GetRamState() API Service ID */
#define MCU_SID_GET_RAM_STATE        (0x0AU)
/* @} */

#if defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
/**
 *  \name MCU Reset Modes
 *
 *  Reset mode enum which can be used in Mcu_PerformReset()
 *  @{
 */
/** \brief Initiate SW COLD RESET sequence */
#define MCU_PERFORM_RESET_MODE_COLD     ((uint8) 0U)
#endif
/** \brief Initiate SW WARM RESET sequence */
#define MCU_PERFORM_RESET_MODE_WARM     ((uint8) 1U)
/* @} */
#define MCU_E_MODE_FAILURE             0U




/*
 * Design: MCAL-8487,MCAL-8492
 */
/**
 *  \brief This type specifies the identification (ID) for a ClockType
 *  status used by Mcu_InitClock()
 */
typedef uint8 Mcu_ClockType;

/*
 * Design: MCAL-8491,MCAL-8471
 */
/**
 *  \brief This type specifies the identification (ID) for a RAW MCU reset
 *  status returned by Mcu_GetResetRawValue()
 */
typedef uint32 Mcu_RawResetType;

/*
 * Design: MCAL-8406,MCAL-8421
 */
/**
 *  \brief This type specifies the identification (ID) for a RAM section used
 *  in Mcu_InitRamSection()
 */
typedef uint8 Mcu_RamSectionType;

/*
 * Design: MCAL-8490,MCAL-8418
 */
/**
 *  \brief This type specifies the identification (ID) for a MCU mode used
 *  in Mcu_SetMode()
 */
typedef uint8 Mcu_ModeType;

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/*
 * Design: MCAL-8438
 */
/**
 *  \brief Structure for module clock setting
 */
 typedef struct
 {
     Mcu_ClkModuleIdType    Mcu_ClockModuleId;
     /**< ModuleId for which clock setting to be applied */
     Mcu_ClkSourceIdType    Mcu_ClockSourceId;
     /**< Clock source Id for the module */
 	 Mcu_ModuleName       ModuleName;
     /**< Module Name */
     Mcu_DomainType       Domain;
     /**< Operating Domain  */
     boolean                Mcu_InitCfg;
     /**< Apply this configuration at Mcu_init time*/
     uint64                 Mcu_ClockFrequency;
     /**< Clock Frequency for the module*/
 } Mcu_ClockConfigType;



/**
 *  \brief Pointer to Clock Config structure
 */
typedef P2CONST (Mcu_ClockConfigType, AUTOMATIC, MCU_PBCFG)
Mcu_ClockConfigPtrType;

/*
* Design: MCAL-8439,MCAL-8424,MCAL-8405,MCAL-8438,MCAL-8457
*/
/**
 *  \brief MCU CONFIG ROOT structure
 */

 typedef struct Mcu_ConfigType_s
 {
     Mcu_ModeType             Mcu_Mode;
     /**< Mcu mode to be used @sa  MCAL_MCU_API:McuModes */
     uint8                    Mcu_ResetMode;
     /**< Reset mode to be used @sa  MCAL_MCU_API:ResetModes */
     Mcu_ClockConfigPtrType   Mcu_ClockConfig;
     /**< Configuration of Module functional clock source */
     uint8                    Mcu_NumberOfClockConfig;
     /**< Number of entries in Mcu_ClockConfig */
 	 Mcu_CBKFunctionPtrType   Mcu_CBK_Function;
       /**< Callback function pointer for Applications */
 } Mcu_ConfigType;


  /** \brief MCU Configuration struct declaration */
extern const struct Mcu_ConfigType_s McuModuleConfiguration_0;




/* @} */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  \brief  This service initializes the MCU driver
 *
 *  \verbatim
 *  Service name      : Mcu_Init
 *  Syntax            : void Mcu_Init( const Mcu_ConfigType* ConfigPtr )
 *  Service ID[hex]   : 0x0
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Non Reentrant
 *  Parameters (in)   : ConfigPtr - Pointer to MCU driver configuration set.
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : None
 *  Description       : This service initializes the MCU driver
 *  \endverbatim
 */
FUNC(void, MCU_CODE) Mcu_Init(
    P2CONST(Mcu_ConfigType, AUTOMATIC, MCU_PBCFG) ConfigPtr);

FUNC(Std_ReturnType, MCU_CODE) Mcu_ClockSetSource(
    Mcu_ClkModuleIdType moduleId,
    Mcu_ClkSourceIdType clkSrcId,
    uint8 clkParentId);

#if (STD_ON == MCU_INIT_CLOCK_API)
/**
 *  \brief This service initializes the PLL and other MCU specific clock options
 *
 *  \verbatim
 *  Service name      : Mcu_InitClock
 *  Syntax            : Std_ReturnType Mcu_InitClock(
 *                           Mcu_ClockType ClockSetting )
 *  Service ID[hex]   : 0x02
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Non Reentrant
 *  Parameters (in)   : ClockSetting - Clock setting
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : Std_ReturnType
 *                    - E_OK: Command has been accepted
 *                    - E_NOT_OK: Command has not been accepted
 *  Description       : This service initializes the PLL and other MCU specific
 *                      clock options
 * \endverbatim
 */
FUNC(Std_ReturnType, MCU_CODE) Mcu_InitClock(Mcu_ClockType ClockSetting);
#endif /* MCU_INIT_CLOCK_API */

/**
 *  \brief This service activates the PLL clock to the MCU clock distribution.
 *
 *  \verbatim
 *  Service name      : Mcu_DistributePllClock
 *  Syntax            : Std_ReturnType Mcu_DistributePllClock(void)
 *  Service ID[hex]   : 0x03
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Reentrant
 *  Parameters (in)   : None
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : Std_ReturnType : E_OK: Command has been accepted
 *                       E_NOT_OK: Command has not been accepted
 *  Description       : This service activates the PLL clock to the MCU clock distribution.
 *  \endverbatim
 */
#if(STD_OFF == MCU_NO_PLL)
FUNC(Std_ReturnType, MCU_CODE) Mcu_DistributePllClock(void);

#endif /* STD_OFF == MCU_NO_PLL */

#if (STD_ON == MCU_PERFORM_RESET_API)
/**
 *  \brief The service performs a microcontroller reset
 *
 *  \verbatim
 *  Service name      : Mcu_PerformReset
 *  Syntax            : void Mcu_PerformReset( void )
 *  Service ID[hex]   : 0x07
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Non Reentrant
 *  Parameters (in)   : None
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : None
 *  Description       : The service performs a microcontroller reset
 *  \endverbatim
 */
FUNC(void, MCU_CODE) Mcu_PerformReset(void);

#endif  /* MCU_PERFORM_RESET_API */

#if (STD_ON == MCU_INIT_RAM_API)
/**
 *  \brief This service initializes the RAM section wise
 *
 *  \verbatim
 *  Service name      : Mcu_InitRamSection
 *  Syntax            : Std_ReturnType Mcu_InitRamSection(
 *                                  Mcu_RamSectionType RamSection )
 *  Service ID[hex]   : 0x01
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Non Reentrant
 *  Parameters (in)   : RamSection - Selects RAM memory section provided
 *                      in configuration set
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : Std_ReturnType
 *                    - E_OK: command has been accepted
 *                    - E_NOT_OK: command has not been accepted e.g. due to
 *                      parameter error
 *  Description       : This service initializes the RAM section wise
 *  \endverbatim
 */
FUNC(Std_ReturnType, MCU_CODE) Mcu_InitRamSection(
    Mcu_RamSectionType RamSection);
#endif /* MCU_INIT_RAM_API */


/* Design: MCAL-8391 */

/*    This is a Dummy API     */

/**
 *  \brief This service provides the lock status of the PLL.
 *
 *  \verbatim
 *  Service name      : Mcu_GetPllStatus
 *  Syntax            : Mcu_PllStatusType Mcu_GetPllStatus( void )
 *  Service ID[hex]   : 0x04
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Reentrant
 *  Parameters (in)   : None
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : Mcu_PllStatusType - PLL Status
 *  Description       : This service provides the lock status of the PLL.
 *  \endverbatim
 */
FUNC(Mcu_PllStatusType, MCU_CODE) Mcu_GetPllStatus(void);

/**
 *  \brief The service reads the reset type from the hardware, if supported
 *
 *  \verbatim
 *  Service name      : Mcu_GetResetReason
 *  Syntax            : Mcu_ResetType Mcu_GetResetReason( void )
 *  Service ID[hex]   : 0x05
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Reentrant
 *  Parameters (in)   : None
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : Mcu_ResetType
 *  Description       : The service reads the reset type from the hardware, if
 *                      supported
 *  \endverbatim
 */
FUNC(Mcu_ResetType, MCU_CODE) Mcu_GetResetReason(void);

/**
 *  \brief The service reads the reset type from the hardware register, if
 *  supported
 *
 *  \verbatim
 *  Service name      : Mcu_GetResetRawValue
 *  Syntax            : Mcu_RawResetType Mcu_GetResetRawValue( void )
 *  Service ID[hex]   : 0x06
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Reentrant
 *  Parameters (in)   : nONE
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : Mcu_RawResetType - Reset raw value
 *  Description       : The service reads the reset type from the hardware
 *                      register, if supported
 *  \endverbatim
 */
FUNC(Mcu_RawResetType, MCU_CODE) Mcu_GetResetRawValue(void);

#if defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
/**
 *  \brief Clear the Mcu Reset reason by clearing the reset reason register
 *
 *  \verbatim
 *  Service name      : Mcu_ClearResetReason
 *  Syntax            : void Mcu_ClearResetReason(void)
 *  Service ID[hex]   : N/A
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Reentrant
 *  Parameters (in)   : None
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : None
 *  Description       : Clear the Mcu Reset reason by clearing the reset reason
 *                      register
 *  \endverbatim
 */
FUNC(void, MCU_CODE) Mcu_ClearResetReason(void);
#endif

#if (STD_ON == MCU_GET_RAM_STATE_API)

/*    This is a Dummy API
*/
/**
 *  \brief This service provides the actual status of the microcontroller Ram.
 *  (if supported)
 *
 *  \verbatim
 *  Service name      : Mcu_GetRamState
 *  Syntax            : Mcu_RamStateType Mcu_GetRamState(void)
 *  Service ID[hex]   : 0x0a
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Reentrant
 *  Parameters (in)   : None
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : Mcu_RamStateType - Status of the Ram Content
 *  Description       : This service provides the actual status of the
 *                      microcontroller Ram. (if supported)
 *  \endverbatim
 */
FUNC(Mcu_RamStateType, MCU_CODE) Mcu_GetRamState(void);
#endif  /* MCU_GET_RAM_STATE_API */

#if (STD_ON == MCU_GET_VERSION_INFO_API)
/**
 *  \brief This service returns the version information of this module
 *
 *  \verbatim
 *  Service name      : Mcu_GetVersionInfo
 *  Syntax            : void Mcu_GetVersionInfo(Std_VersionInfoType*
 *                              versioninfo)
 *  Service ID[hex]   : 0x09
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Reentrant
 *  Parameters (in)   : None
 *  Parameters (inout): None
 *  Parameters (out)  : versioninfo - Pointer to where to store the
 *                      version information of this module.
 *  Return value      : None
 *  Description       : This service returns the version information of this
 *                      module
 *  \endverbatim
 */




FUNC(void, MCU_CODE) Mcu_GetVersionInfo(
    P2VAR(Std_VersionInfoType, AUTOMATIC, MCU_APPL_DATA) versioninfo);
#endif  /* MCU_GET_VERSION_INFO_API */



/**
 *  \brief This service activates the MCU power modes.
 *
 *  \verbatim
 *  Service name      : Mcu_SetMode
 *  Syntax            : void Mcu_SetMode(Mcu_ModeType McuMode)
 *  Service ID[hex]   : 0x08
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Reentrant
 *  Parameters (in)   : McuMode : Set different MCU power modes configured in the configuration set
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : None
 *  Description       : This service activates the MCU power modes.
 *  \endverbatim
 */

FUNC(void, MCU_CODE) Mcu_SetMode(P2CONST(Mcu_ModeType, AUTOMATIC, MCU_APPL_DATA) McuMode);




#ifdef __cplusplus
}
#endif

#endif  /* #ifndef MCU_H_ */

/* @} */
