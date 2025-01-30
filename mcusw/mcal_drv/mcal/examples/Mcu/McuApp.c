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
 *  \file     McuApp.c
 *
 *  \brief    This file contains the GPT test example
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */


#include "Std_Types.h"
#include "Mcu.h"
#include "McuApp_Startup.h"

#include "string.h"
#include "app_utils.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
#define MSG_NORMAL      (APP_UTILS_PRINT_MSG_NORMAL)
/**< Message type */
#define MSG_STATUS      (APP_UTILS_PRINT_MSG_STATUS)
/**< Message type */

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

uint32                   McuApp_TestPassed = E_OK;
Std_VersionInfoType      VersionInfo;
sint32 TestState = TRUE;


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */


int main(void)
{
    Mcu_ResetType         resetType;
    Std_ReturnType        retVal = E_NOT_OK;
#if (STD_ON == MCU_INIT_CLOCK_API)
    Mcu_ClockType         clockSetting;

    uint8                 clockCfg;
    clockSetting = 0U;
    clockCfg = McuModuleConfiguration_0.Mcu_NumberOfClockConfig;
#endif /*(STD_ON == MCU_INIT_CLOCK_API)*/

#if (MCU_GET_VERSION_INFO_API == STD_ON)
    Std_VersionInfoType versioninfo;
#endif  /* #if (MCU_GET_VERSION_INFO_API == STD_ON) */

#if defined (SOC_AM62X) || defined (SOC_AM62AX) || defined (SOC_AM62PX) || defined (SOC_J722S)
	System_init();
#endif

    #ifdef UART_ENABLED
        AppUtils_Init();
    #endif

    AppUtils_Printf(MSG_NORMAL, "\nMcuApp: Sample Application - STARTS !!!\n\r");


  #if (MCU_GET_VERSION_INFO_API == STD_ON)
      /* Get and print version */
      Mcu_GetVersionInfo(&versioninfo);
      AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL," \n");
      AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,"MCU MCAL Version Info\n");
      AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,"---------------------\n");
      AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,"Vendor ID           : %d\n", versioninfo.vendorID);
      AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,"Module ID           : %d\n", versioninfo.moduleID);
      AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,"SW Major Version    : %d\n", versioninfo.sw_major_version);
      AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,"SW Minor Version    : %d\n", versioninfo.sw_minor_version);
      AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,"SW Patch Version    : %d\n", versioninfo.sw_patch_version);
      AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL," \n");
  #endif  /* #if (MCU_GET_VERSION_INFO_API == STD_ON) */

    McuApp_Startup();

    Mcu_Init(&McuModuleConfiguration_0);

    #if (STD_ON == MCU_INIT_CLOCK_API)
    
    for(clockSetting=0; clockSetting<clockCfg ; clockSetting++)
    {
        retVal = Mcu_InitClock(clockSetting);       
    }
      
    #endif /* (STD_ON == MCU_INIT_CLOCK_API) */


    resetType = Mcu_GetResetReason();
#if defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
    Mcu_ClearResetReason();

  #if (STD_ON == MCU_PERFORM_RESET_API)
          Mcu_PerformReset();
  #endif
#endif
         AppUtils_Printf(MSG_NORMAL,"MCU Reset Reason:%d \n", resetType);
         AppUtils_Printf(MSG_NORMAL,"MCU Reset Reason Raw Value:0x%x \n",
                         Mcu_GetResetRawValue());
   if (E_OK == retVal)
   {
     AppUtils_Printf(MSG_NORMAL,
        "MCU Stack Usage: %d bytes\n", AppUtils_GetStackUsage());
       if (AppUtils_McuCheckStackAndSectionCorruption() != E_OK)
       {
           retVal = E_NOT_OK;
           AppUtils_Printf(MSG_NORMAL,  "MCU Stack/section corruption!!!\n");
       }
   }
      if (E_OK == retVal)
      {
          AppUtils_Printf(MSG_NORMAL,
          "MCU_APP: All tests have passed!!!\n");
      }
      else
      {
          AppUtils_Printf(MSG_NORMAL,
          "MCU_APP: Some tests have Failed!!!\n");
      }

#if defined (SOC_AM62X) || defined (SOC_AM62AX) || defined (SOC_AM62PX) || defined (SOC_J722S)
#if (STD_ON == MCU_PERFORM_RESET_API)
    	AppUtils_Printf(MSG_NORMAL,"Perform Warm Reset\n");
        Mcu_PerformReset();
#endif
#endif
    return retVal;
}
/* ========================================================================== */
/*                        Application Local Functions                         */
/* ========================================================================== */
