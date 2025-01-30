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
 *  \file     Mcu.c
 *
 *  \brief    This file contains MCU MCAL driver
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */


#include "Mcu.h"
#include "Mcu_Priv.h"
#include "SchM_Mcu.h"
#include "stdint.h"
#include "string.h"

#if (MCU_DEV_ERROR_DETECT == STD_ON)
#include "Det.h"
#endif /*  (MCU_DEV_ERROR_DETECT == STD_ON) */

/*Design: MCAL-8451 */
#include "Dem.h"
#include "Os.h"

#define MCU_START_SEC_CODE
#include "Mcu_MemMap.h"
#define MCU_STOP_SEC_CODE
#include "Mcu_MemMap.h"




/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* AUTOSAR version information check has to match definition in header file */
#if ((MCU_AR_RELEASE_MAJOR_VERSION != (4U)) || \
    (MCU_AR_RELEASE_MINOR_VERSION != (3U)) ||  \
    (MCU_AR_RELEASE_REVISION_VERSION != (1U)))
    #error "Mcu: AUTOSAR Version Numbers of Mcu are different!!"
#endif

/* AUTOSAR version information check has to match definition in header file */
#if ((MCU_SW_MAJOR_VERSION != (9U)) || \
    (MCU_SW_MINOR_VERSION != (1U)) ||  \
    (MCU_SW_PATCH_VERSION != (1U)))
    #error "Mcu: Software Version Numbers are inconsistent!!"
#endif

/*
*Design: MCAL-8401,MCAL-8407,MCAL-8482,MCAL-8498,MCAL-8446
*/
/* Error Codes in Diagnostic Event Manager */
#ifndef MCU_E_CLOCK_FAILURE
#define MCU_E_CLOCK_FAILURE DemConf_DemEventParameter_MCU_E_CLOCK_FAILURE
#endif


/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/** \brief MCU PC Configuration struct declaration */

/* ========================================================================== */
/*                         Function Declarations                              */
/* ========================================================================== */

#define STATIC static

static Std_ReturnType Mcu_resetDrvObj(Mcu_DriverObjType *drvObj);
static Std_ReturnType Mcu_initDrvObj(Mcu_DriverObjType *drvObj,
                           const Mcu_ConfigType *ConfigPtr);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

#define MCU_START_SEC_VAR_NO_INIT_UNSPECIFIED
#include "Mcu_MemMap.h"

static Mcu_DriverObjType Mcu_DriverObj;

#define MCU_STOP_SEC_VAR_NO_INIT_UNSPECIFIED
#include "Mcu_MemMap.h"

#define MCU_START_SEC_VAR_INIT_8
#include "Mcu_MemMap.h"

/** \brief Mcu driver init status */
volatile VAR(uint8, MCU_VAR_INIT)
    Mcu_DrvStatus = MCU_STATUS_UNINIT;

#define MCU_STOP_SEC_VAR_INIT_8
#include "Mcu_MemMap.h"


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

#define MCU_START_SEC_CODE
#include "Mcu_MemMap.h"

/*******************************************************************************
 * Mcu_GetVersionInfo
 ******************************************************************************/
/*! \brief      This function will return the version for this module
 *
 *  \param[in]  None
 *
 *  \param[out]    Std_VersionInfoType VersionInfo - the structure that will
 *                  contain version info after fxn call.
 *
 *  \context    App
 ******************************************************************************/
 /*
 *Design: MCAL-8397,MCAL-8419,MCAL-8432,MCAL-8479,MCAL-8483,MCAL-8443,MCAL-8451,MCAL-8485
 */
#if (STD_ON == MCU_GET_VERSION_INFO_API)

FUNC(void, MCU_CODE) Mcu_GetVersionInfo(Std_VersionInfoType * versioninfo)
{
#if (MCU_DEV_ERROR_DETECT == STD_ON)
    if (NULL_PTR == versioninfo)
    {
      /* error check implementation*/
      (void) Det_ReportError(MCU_MODULE_ID, MCU_INSTANCE_ID,
                              MCU_SID_GET_VERSION_INFO,
                              MCU_E_PARAM_POINTER);
    }
    else
#endif /* MCU_DEV_ERROR_DETECT == STD_ON */
    {
        versioninfo->vendorID         = (MCU_VENDOR_ID);
        versioninfo->moduleID         = (MCU_MODULE_ID);
        versioninfo->sw_major_version = (uint8) (MCU_SW_MAJOR_VERSION);
        versioninfo->sw_minor_version = (uint8) (MCU_SW_MINOR_VERSION);
        versioninfo->sw_patch_version = (uint8) (MCU_SW_PATCH_VERSION);
    }
}   /* Mcu_GetVersionInfo()*/
#endif  /* STD_ON == MCU_GET_VERSION_INFO_API */

/*******************************************************************************
 * Mcu_SetMode
 ******************************************************************************/
/*! \brief     This service activates the MCU power modes.
 *
 *  \param[in]  McuMode : Set different MCU power modes configured in
 *                          the configuration set
 *  \param[out] NONE
 *
 *  \context    App
 ******************************************************************************/
 /*
  *Design: MCAL-8396,MCAL-8462,MCAL-8397,MCAL-8419,MCAL-8432,MCAL-8479,MCAL-8483,MCAL-8440,MCAL-8477,MCAL-8443,MCAL-8480,MCAL-8485
  */
FUNC(void, MCU_CODE) Mcu_SetMode(P2CONST(Mcu_ModeType, AUTOMATIC, MCU_APPL_DATA) McuMode)
{


      const Mcu_ModeType *mcumode = McuMode;
      mcumode = &Mcu_DriverObj.Mcu_Mode;
      /* Design: MCAL-8396 */
      if (MCU_STATUS_UNINIT == Mcu_DrvStatus)
      {
          /* Design: MCAL-8397 */
           mcumode = NULL;

  #if (STD_ON == MCU_DEV_ERROR_DETECT)
         if (NULL_PTR == mcumode )
         {
          /* API is being called before calling Mcu_Init */
          (void) Det_ReportError(MCU_MODULE_ID, MCU_INSTANCE_ID,
                                 MCU_SID_SETMODE,
                                 MCU_E_UNINIT);
         }
         else
   #endif /* MCU_DEV_ERROR_DETECT == STD_ON */
         {
           McuMode = mcumode;
         }
       }
      else
      {
          /* Only one mode is supported Mcu_Mode = 0 */
          mcumode = NULL;
      }
}

/*******************************************************************************
 * Mcu_resetDrvObj
 ******************************************************************************/
/*! \brief      This function will reset default  Config structure.
 *
 *  \param[in]  Mcu_DriverObjType drvObj Pointer to Driver Config structure.
 *
 *  \context
 ******************************************************************************/
static Std_ReturnType Mcu_resetDrvObj(Mcu_DriverObjType *drvObj)
{
   Std_ReturnType retVal = E_OK;

    if (MCU_STATUS_UNINIT == Mcu_DrvStatus)
    {

        drvObj->Mcu_ResetMode= 0U;
        drvObj->Mcu_ClockConfig = NULL;
        drvObj->Mcu_NumberOfClockConfig = 0U;
        drvObj->Mcu_CBK_Function = NULL;
    }
    else
    {
      retVal = E_NOT_OK;
    }
  return (retVal);
}


/*******************************************************************************
 * Mcu_initDrvObj
 ******************************************************************************/
/*! \brief      This function will initialize Driver Config structure.
 *
 *  \param[in]  Mcu_DriverObjType drvObj Pointer to Driver Config structure.
 *  \param[in]  Mcu_ConfigType ConfigPtr Pointer to Mcu Config structure.
 *
 *  \context
 ******************************************************************************/
static Std_ReturnType Mcu_initDrvObj(Mcu_DriverObjType *drvObj,
                           const Mcu_ConfigType *ConfigPtr)
{
  Std_ReturnType retVal = E_OK;

   if (MCU_STATUS_UNINIT == Mcu_DrvStatus)
   {

    drvObj->Mcu_ResetMode = ConfigPtr->Mcu_ResetMode;
    drvObj->Mcu_ClockConfig = ConfigPtr->Mcu_ClockConfig;
    drvObj->Mcu_NumberOfClockConfig = ConfigPtr->Mcu_NumberOfClockConfig;
    drvObj->Mcu_CBK_Function = ConfigPtr->Mcu_CBK_Function;

  }
  else
  {
    retVal = E_NOT_OK;
    /* Do Nothing *//* Added only to avoid MISRA violation */
  }
 return (retVal);
}


/*******************************************************************************
 * Mcu_Init
 ******************************************************************************/
/*! \brief     This function initialises the MCU controller
 *
 *              All the required initilisation for the MCU driver and hardware
 *              is performed in this function.
 *
 *  \param[in]  CfgPtr - Pointer to post-build configuration data.
 *
 * \param[out]  NONE
 *
 *  \context    App
 ******************************************************************************/
 /*
 *Design: MCAL-8454,MCAL-8484,MCAL-8426,MCAL-8469,MCAL-8467,MCAL-8389,MCAL-8505,MCAL-8432,MCAL-8479,MCAL-8483,MCAL-8443,MCAL-8485,MCAL-8513
 */
FUNC(void, MCU_CODE) Mcu_Init(
    P2CONST(Mcu_ConfigType, AUTOMATIC, MCU_PBCFG) CfgPtr)
{
  Std_ReturnType status = E_NOT_OK;
  const Mcu_ConfigType *ConfigPtr = CfgPtr;

  #if (STD_ON == MCU_VARIANT_PRE_COMPILE)
      if (NULL_PTR == ConfigPtr)
      {
          ConfigPtr = &MCU_INIT_CONFIG_PC;
      }
  #endif /* (STD_ON == MCU_VARIANT_PRE_COMPILE)  */

  #if (MCU_DEV_ERROR_DETECT == STD_ON)
      if (NULL_PTR == ConfigPtr)
      {
          (void) Det_ReportError(MCU_MODULE_ID, MCU_INSTANCE_ID, MCU_SID_INIT,
                                 MCU_E_PARAM_CONFIG);
      }
      else
  #endif /* MCU_DEV_ERROR_DETECT == STD_ON */
      {
      /* Reseting the elements of Mcu_DriverObj  */
          status = Mcu_resetDrvObj(&Mcu_DriverObj);
          if (E_OK == status)
          {
          /* Initializing of the Mcu_DriverObj  */
              status = Mcu_initDrvObj(&Mcu_DriverObj, ConfigPtr);
              if (E_OK == status)
              {
                Mcu_DrvStatus = MCU_STATUS_INIT;
              }
              else
              {
                Mcu_DrvStatus = MCU_STATUS_UNINIT;
              }
          }
      }
  return ;
}

/*******************************************************************************
 * Mcu_InitClock
 ******************************************************************************/
/*! \brief     This service initializes the PLL and other MCU specific clock
 *             options.
 *
 *  \param[in]  ClockSetting - ClockSetting from config.
 *
 * \param[out]  Std_ReturnType -
 *                              E_OK  - successful
 *                              E_NOT_OK - Failed
 *
 *  \context    App
 ******************************************************************************/
 /*
 *Design: MCAL-8442,MCAL-8455,MCAL-8416,MCAL-8420,MCAL-8397,MCAL-8505,MCAL-8432,MCAL-8479,MCAL-8483,MCAL-8392,MCAL-8443,MCAL-8388,MCAL-8413,MCAL-8485
 */
#if (STD_ON == MCU_INIT_CLOCK_API)
FUNC(Std_ReturnType, MCU_CODE) Mcu_InitClock(Mcu_ClockType ClockSetting)
{
    Std_ReturnType initclock_return = E_NOT_OK;


  #if (MCU_DEV_ERROR_DETECT == STD_ON)
    if (MCU_STATUS_UNINIT == Mcu_DrvStatus)
    {
        (void) Det_ReportError(MCU_MODULE_ID, MCU_INSTANCE_ID,
                               MCU_SID_INIT_CLOCK,
                               MCU_E_UNINIT);
    }
    else if (ClockSetting >= Mcu_DriverObj.Mcu_NumberOfClockConfig)
    {
        /* error check implementation*/
        (void) Det_ReportError(MCU_MODULE_ID, MCU_INSTANCE_ID,
                               MCU_SID_INIT_CLOCK,
                               MCU_E_PARAM_CLOCK);
     }
    else
  #endif      /* MCU_DEV_ERROR_DETECT == STD_ON */
    {

      /* Config Clock */
      initclock_return = Mcu_DriverObj.Mcu_CBK_Function(
      Mcu_DriverObj.Mcu_ClockConfig[ClockSetting].Mcu_ClockModuleId,
      Mcu_DriverObj.Mcu_ClockConfig[ClockSetting].Mcu_ClockSourceId,
      Mcu_DriverObj.Mcu_ClockConfig[ClockSetting].Mcu_ClockFrequency
      );

    }

    return (initclock_return);
}  /* end of the Mcu_InitClock */

#endif /* (STD_ON == MCU_INIT_CLOCK_API) */

/*******************************************************************************
 * Mcu_DistributePllClock
 ******************************************************************************/
/*! \brief     This service activates the PLL clock to the MCU clock distribution.
 *
 *
 *  \param[in]  NONE
 *  \param[out]  Std_ReturnType E_OK: Command has been accepted
 *                             E_NOT_OK: Command has not been accepted
 *
 *  \context    App
 ******************************************************************************/
 /*
 *Design: MCAL-8508,MCAL-8470,MCAL-8445,MCAL-8475,MCAL-8405,MCAL-8419,MCAL-8432,MCAL-8479,MCAL-8483,MCAL-8434,MCAL-8443,MCAL-8485
 */
#if(STD_OFF == MCU_NO_PLL)

FUNC(Std_ReturnType, MCU_CODE) Mcu_DistributePllClock(void)
{

  /* No actions pereformed. Dummy API.*/
  return E_OK;

} /*end of the Mcu_DistributePllClock()*/
#endif /* STD_OFF == MCU_NO_PLL */

/*******************************************************************************
 * Mcu_InitRamSection
 ******************************************************************************/
/*! \brief     This service initializes the RAM section wise
 *
 *
 *  \param[in]  RamSection : Selects RAM memory section provided in
 *                             configuration set
 *  \param[out]  Std_ReturnType E_OK: Command has been accepted
 *                             E_NOT_OK: Command has not been accepted
 *  \context    App
 ******************************************************************************/
 /*
 *Design: MCAL-8497,MCAL-8430,MCAL-8419,MCAL-8432,MCAL-8479,MCAL-8483,MCAL-8443,MCAL-8485,MCAL-8486
 */
#if (STD_ON == MCU_INIT_RAM_API)

FUNC(Std_ReturnType, MCU_CODE) Mcu_InitRamSection(Mcu_RamSectionType RamSection)
{
    /* No actions performed. Dummy API. always returns E_OK  */
    return E_OK;
}
#endif /*(STD_ON == MCU_INIT_RAM_API)  */


/*******************************************************************************
 * Mcu_GetPllStatus
 ******************************************************************************/
/*! \brief     This service provides the lock status of the PLL.
 *
 *
 *  \param[in]  NONE
 *  \param[out] NONE
 *
 *  \context    App
 ******************************************************************************/
 /*
 *Design: MCAL-8402,MCAL-8503,MCAL-8466,MCAL-8419,MCAL-8432,MCAL-8479,MCAL-8483,MCAL-8443,MCAL-8485,MCAL-8387
 */
FUNC(Mcu_PllStatusType, MCU_CODE) Mcu_GetPllStatus(void)
{
	
  /* No actions performed. Dummy API. */
  return (Mcu_PllStatusType) MCU_PLL_STATUS_UNDEFINED;
  
} /* end of Mcu_GetPllStatus */


/*******************************************************************************
 * Mcu_GetResetRawValue
 ******************************************************************************/
/*! \brief     The service reads the reset type from the hardware register,
 *             if supported
 *
 *
 *  \param[in]  NONE
 *  \param[out] NONE
 *
 *  \context    App
 ******************************************************************************/
 /*
 *Design: MCAL-8393,MCAL-8458,MCAL-8397,MCAL-8410,MCAL-8422,MCAL-8419,MCAL-8432,MCAL-8479,MCAL-8483,MCAL-8443,MCAL-8485
 */
FUNC(Mcu_RawResetType, MCU_CODE) Mcu_GetResetRawValue(void)
{

    Mcu_RawResetType reset_raw_value;

    if (MCU_STATUS_UNINIT == Mcu_DrvStatus)
    {
        reset_raw_value = MCU_ERRORRST_MASK;
#if (MCU_DEV_ERROR_DETECT == STD_ON)
     if (MCU_ERRORRST_MASK == reset_raw_value)
     {
        /* API is being called before calling Mcu_Init */
        (void) Det_ReportError(MCU_MODULE_ID, MCU_INSTANCE_ID,
                               MCU_SID_GET_RESET_RAW_VALUE,
                               MCU_E_UNINIT);
     }
     else
#endif /* MCU_DEV_ERROR_DETECT == STD_ON */
     {
       /*  Do nothing. */
     }
   }
    else
    {
        reset_raw_value = (Mcu_RawResetType) Mcu_GetSystemRstCause();
    }
    return (reset_raw_value);
}  /*end of the Mcu_ResetRawValue()*/



/*******************************************************************************
 * Mcu_GetResetReason
 ******************************************************************************/
/*! \brief     The service reads the reset type from the hardware, if supported
 *
 *
 *  \param[in]  NONE
 *  \param[out] NONE
 *
 *  \context    App
 ******************************************************************************/
 /*
 *Design: MCAL-8493,MCAL-8425,MCAL-8397,MCAL-8410,MCAL-8422,MCAL-8419,MCAL-8432,MCAL-8479,MCAL-8483,MCAL-8499,MCAL-8443,MCAL-8485
 */
FUNC(Mcu_ResetType, MCU_CODE) Mcu_GetResetReason(void)
{
    Mcu_ResetType reset_reason;
    Mcu_RawResetType reset_raw;
    if (MCU_STATUS_UNINIT == Mcu_DrvStatus)
    {
        reset_reason = MCU_RESET_UNDEFINED;
#if (MCU_DEV_ERROR_DETECT == STD_ON)
     if (MCU_RESET_UNDEFINED == reset_reason)
     {
        /* API is being called before calling Mcu_Init */
        (void) Det_ReportError(MCU_MODULE_ID, MCU_INSTANCE_ID,
                               MCU_SID_GET_RESET_REASON,
                               MCU_E_UNINIT);
     }
     else
#endif /* MCU_DEV_ERROR_DETECT == STD_ON */
     {
       /* Do nothing */
     }
   }
    else
    {
        /* Get raw reset reason */
        reset_raw = Mcu_GetResetRawValue();
        reset_reason = Mcu_InterpretRawResetStatus(reset_raw);
    }
    return (reset_reason);
} /* end of Mcu_GetResetReason() */



/*******************************************************************************
 * Mcu_PerformReset
 ******************************************************************************/
/*! \brief     The service performs a microcontroller reset
 *
 *
 *  \param[in]  NONE
 *  \param[out] NONE
 *
 *  \context    App
 ******************************************************************************/
 /*
 *Design: MCAL-8444,MCAL-8452,MCAL-8448,MCAL-8423,MCAL-8397,MCAL-8419,MCAL-8432,MCAL-8479,MCAL-8483,MCAL-8449,MCAL-8443,MCAL-8485
 */

#if (STD_ON == MCU_PERFORM_RESET_API)
FUNC(void, MCU_CODE) Mcu_PerformReset(void)
{
 #if (MCU_DEV_ERROR_DETECT == STD_ON)
    if (MCU_STATUS_UNINIT == Mcu_DrvStatus)
    {
        /* API is being called before calling Mcu_Init */
        (void) Det_ReportError(MCU_MODULE_ID, MCU_INSTANCE_ID,
                               MCU_SID_PERFORM_RESET,
                               MCU_E_UNINIT);

    }
    else
#endif /* MCU_DEV_ERROR_DETECT == STD_ON */
    {
        uint8  mcu_reset_mode;
        uint32 resetVal = 0U;

        /* Perform a sofware reset */
        mcu_reset_mode = Mcu_DriverObj.Mcu_ResetMode;
        if (MCU_PERFORM_RESET_MODE_WARM == mcu_reset_mode)
        {
#if defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J721S2)  || defined (SOC_J784S4)
            resetVal = MCU_POWER_ON_RESET;
#elif defined (SOC_AM62X) || defined (SOC_AM62AX) || defined (SOC_AM62PX)	|| defined (SOC_J722S)		
			resetVal = MCU_POWER_ON_RESET_MAIN_DOMAIN;
#endif
        }
        SchM_Enter_Mcu_MCU_EXCLUSIVE_AREA_0();
        Mcu_PerformSoftSysReset(resetVal);
        SchM_Exit_Mcu_MCU_EXCLUSIVE_AREA_0();
    }
} /*end of the Mcu_PerformReset()*/
#endif /* STD_ON == MCU_PERFORM_RESET_API */


/*******************************************************************************
 * Mcu_GetRamState
 ******************************************************************************/
/*! \brief     This service provides the actual status of the microcontroller
 *             Ram. (if supported)
 *
 *  \param[in]  NONE
 *  \param[out] NONE
 *
 *  \context    App
 ******************************************************************************/
 /*
 *Design: MCAL-8395,MCAL-8447,MCAL-8435,MCAL-8419,MCAL-8432,MCAL-8479,MCAL-8483,MCAL-8443,MCAL-8485
 */

#if (STD_ON == MCU_GET_RAM_STATE_API)

FUNC(Mcu_RamStateType, MCU_CODE) Mcu_GetRamState(void)
{
  /* No actions performed. Always return E_OK */

  return E_OK;
}  /*end of the Mcu_GetRamState()*/
#endif /*STD_ON == MCU_GET_RAM_STATE_API */


#define MCU_STOP_SEC_CODE
#include "Mcu_MemMap.h"

/*******************************************************************************
 *  End of File: Mcu.c
 ******************************************************************************/
