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
 *  \file     WdgApp.c
 *
 *  \brief    This file contains the test example for WDG.
 *            RTI Instance selected is MCU RTI0
 *            RTI clock source is selected as 12.5 MHz.
 *            RTI window size is set to 100%.
 *            RTI reaction after expiry is set to interrupt mode.
 *            RTI timeout value is set for 2sec.
 *            RTI Mode selected is WDGIF_SLOW_MODE.
 *            RTI will be serviced 5 times(approx 10 secs)
 *            and then it will expire in 2sec and generates an ESM interrupt.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "Std_Types.h"
#include "Wdg.h"
#include "SchM_Wdg.h"
#include "app_utils.h"
#include "stdint.h"
#include "WdgApp_Startup.h"
#include "WdgApp.h"
#include <hw_include/soc.h>
#include <hw_include/hw_types.h>
#include <hw_include/lld_rti.h>
#include <hw_include/lldr.h>
/* Design : MCAL-5597 */
#include <hw_include/lld_esm.h>
#if defined (SOC_AM62AX) || defined (SOC_AM62PX) || defined (SOC_J722S)
#include <kernel/dpl/ClockP.h>
#endif
/* Interrupt Registrations */
#if defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
#include <ti/osal/osal.h>
#endif
/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */
static void WdgApp_WdgTest(void);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
uint32 WdgApp_TestPassed = E_OK;
/**< WDG App Status Flag */
volatile uint32 WdgApp_IsrFlag = 0U;
/**< WDG App ISR Flag */
uint32 WdgApp_RtiInstance;
/**< RTI INSTANCE */
uint32 WdgApp_UserInput;
/**< User Input selected whether Interrupt mode or External Error Pin */
/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */
#if defined (SOC_AM62X) || defined (SOC_AM62AX) || defined (SOC_AM62PX) || defined (SOC_J722S)
int32_t WDG_delay(uint32_t nTicks)
{
    uint64_t sec = (ClockP_ticksToUsec(nTicks)) / 1000000;

    ClockP_sleep((uint32_t)sec);

    return 0;
}
#endif

int main(void)
{
    uint32 intrStatus;
    WdgApp_TestPassed = E_OK;

#ifdef UART_ENABLED
    AppUtils_Init();
#endif

    AppUtils_LogResult(APP_UTILS_TEST_STATUS_INIT);

    WdgApp_PowerAndClkSrc();
    WdgApp_Startup();


    WdgApp_WdgTest();

    if(WdgApp_TestPassed == E_OK)
    {
        AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
            "WDG Stack Usage : %d bytes\n", AppUtils_GetStackUsage());
        if (AppUtils_WdgCheckStackAndSectionCorruption() != E_OK)
        {
            WdgApp_TestPassed = E_NOT_OK;
            AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
                            "WDG Stack/section corruption!!!\n");
        }
    }

    if (E_OK == WdgApp_TestPassed)
    {
        /* Make sure interrupt is generated only once */
        RTIDwwdGetStatus(WdgApp_RtiInstance, &intrStatus);
        if (intrStatus != 0U)
        {
            WdgApp_TestPassed = E_NOT_OK;
        }
        else
        {
            WdgApp_TestPassed = E_OK;
        }
    }

    if (E_OK == WdgApp_TestPassed)
    {
        AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL, "All tests have passed!!!\n");
        AppUtils_LogResult(APP_UTILS_TEST_STATUS_PASS);
    }
    else
    {
        AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL, "Some tests have Failed!!!\n");
        AppUtils_LogResult(APP_UTILS_TEST_STATUS_FAIL);
    }

    return (0);
}

static void WdgApp_WdgTest()
{
    uint32 serviceCntIndx = 0U, userInput = 0U;
    uint32 timeOutVal;

    /* Initialize Gloabal Variables */
    WdgApp_IsrFlag = 0U;

    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,"Starting WDG test  !!!\n");

#if (WDG_GET_VERSION_INFO_API == STD_ON)
    Std_VersionInfoType versioninfo;
    /* Get and print version */
    Wdg_GetVersionInfo(&versioninfo);
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL," \n");
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,"WDG MCAL Version Info\n");
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,"---------------------\n");
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,"Vendor ID           : %d\n",
        versioninfo.vendorID);
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,"Module ID           : %d\n",
        versioninfo.moduleID);
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,"SW Major Version    : %d\n",
        versioninfo.sw_major_version);
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,"SW Minor Version    : %d\n",
        versioninfo.sw_minor_version);
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,"SW Patch Version    : %d\n",
        versioninfo.sw_patch_version);
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL," \n");
#endif  /* #if (WDG_VERSION_INFO_API == STD_ON) */

    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
    "On Expiry of WDG timeout Please enter :\n"
    "0 To generate an interrupt (via ESM) \n"
#if defined (BUILD_MCU1_0) || defined (BUILD_MCU0_0)
    "1 To drive MCU_SAFETY_ERROR Pin low (via ESM) \n");
#elif defined (BUILD_MCU2_1)
    "1 To drive SOC_SAFETY_ERROR Pin low (via ESM) \n");
#endif

    WdgApp_UserInput = AppUtils_GetNum();
    while((WdgApp_UserInput != 0U) && (WdgApp_UserInput != 1U))
    {
        AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
            "Please enter either 0 or 1 \n");
        WdgApp_UserInput = AppUtils_GetNum();
    }

    if (WdgApp_UserInput == 1U)
    {
        /* Enable ESM Error Pin MCU_SAFETY_ERRORn/SOC_SAFETY_ERRORn */
        WdgApp_EnableESMErrorPin();
#if defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
#if defined (BUILD_MCU1_0)
        AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
        "On J7ES EVM Test Point TP24(SOM Board), "
        "On J7VCL EVM Test Point TP56(SOM Board), "
        "For J7AEP, Test Point TP50 on Common Processor Board, "
        "Test Point TP315, On J7AHP EVM "
        "and it should be Active HIGH before WDG expires\n");
#elif defined (BUILD_MCU2_1)
        AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
        "On J7ES EVM Test Point TP14(SOM Board), "
		"On J7VCL EVM Test Point TP82 (SOM Board), "
        "For J7AEP EVM Test Point TP99 on Common Processor Board, "
        "On J7AHP EVM Test Point to the Resistor R1035 "
        "and it should be Active HIGH before WDG expires\n");
#endif
#elif defined (SOC_AM62X) || defined (SOC_AM62AX) || defined (SOC_AM62PX) || defined (SOC_J722S)
        AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
        "Test Point J9 26, 2(GND) On AM62X/AM62AX/AM62PX EVM Test (SOM Board),  "
        "and it should be Active HIGH before WDG expires\n");
#endif

        AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,"Please enter 1 "
        "after probing on test point \n");
        userInput = AppUtils_GetNum();
        while(userInput != 1U)
        {
            AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
                "Please enter 1 \n");
            userInput = AppUtils_GetNum();
        }
    }

    /* WDG Init */
#if (STD_ON == WDG_VARIANT_PRE_COMPILE)
    const Wdg_ConfigType *pCfgPtr = &WDG_INIT_CONFIG_PC;
    Wdg_Init((const Wdg_ConfigType *) NULL_PTR);
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
        "Variant - Pre Compile being used !!!\n");
#elif (STD_ON == WDG_VARIANT_LINK_TIME)
    const Wdg_ConfigType *pCfgPtr = &WdgSettingsConfig;
    Wdg_Init(pCfgPtr);
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
        "Variant - Link Time being used !!!\n");
#else
    const Wdg_ConfigType *pCfgPtr = &WdgSettingsConfig;
    Wdg_Init(pCfgPtr);
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
        "Variant - Post Build being used !!!\n");
#endif
#if defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
#if defined (BUILD_MCU1_0)
	/* Currently WDG is verified only for MCU1_0 and MCU2_1 Cores */
	WdgApp_RtiInstance = CSL_MCU_RTI0_CFG_BASE;
#elif defined (BUILD_MCU2_1)
	WdgApp_RtiInstance = CSL_RTI29_CFG_BASE;
#endif
#elif defined (SOC_AM62X)
	WdgApp_RtiInstance = CSL_WKUP_RTI0_CFG_BASE;
#elif defined (SOC_AM62AX) || defined (SOC_AM62PX) || defined (SOC_J722S)
	WdgApp_RtiInstance = CSL_MCU_RTI0_CFG_BASE;
#endif
    if (pCfgPtr->defaultMode == WDGIF_FAST_MODE)
    {
        timeOutVal = pCfgPtr->fastModeCfg.timeOutVal;
    }
    else
    {
        timeOutVal = pCfgPtr->slowModeCfg.timeOutVal;
    }

    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,"WDG timeout is configured "
    "for %u millisecs\n", timeOutVal);

    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,"Number of times WDG will be "
    "serviced : %u\n", WDG_SERVICE_CNT);

    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,"WDG ESM Interrupt will be "
    "generated after %u * %u i.e %u millisecs\n",
    timeOutVal, WDG_SERVICE_CNT,
    timeOutVal * WDG_SERVICE_CNT);

    Wdg_SetTriggerCondition(timeOutVal * WDG_SERVICE_CNT);
    do
    {
        /* Service before the WDG expires */
#if defined (SOC_AM62AX) || defined (SOC_AM62X) || defined (SOC_AM62PX) || defined (SOC_J722S)
        WDG_delay((WDG_TRIGGER_PERCENT * timeOutVal)
                                                    / 100);
#else
        Osal_delay((WDG_TRIGGER_PERCENT * timeOutVal)
                                                    / 100);
#endif
        Wdg_Trigger();
        serviceCntIndx++;
    } while (serviceCntIndx < WDG_SERVICE_CNT);

    while (WdgApp_IsrFlag == 0U)
    {
        /* Wait for ESM Interrupt */
    }

    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,"WDG Elapsed and generated an"
    " event to ESM\n");
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,"ESM has generated an Interrupt\n");
    if (WdgApp_UserInput == 1U)
    {
#if defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
#if defined (BUILD_MCU1_0)
        AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
        " On J7ESEVM Test Point TP24(SOM Board)"
        " and it should be Active LOW after ESM interrupt generated\n");
#elif defined (BUILD_MCU2_1)
        AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
        " On J7ESEVM Test Point TP14(SOM Board)"
        " and it should be Active LOW after ESM interrupt generated\n");
#endif
#elif defined (SOC_AM62X) || defined (SOC_AM62AX) || defined (SOC_AM62PX) || defined (SOC_J722S)
        AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
        " On AM62XEVM Test Point pin No 26 and 2 (GND) on J9 Tag (SOM Board)"
        " and it should be Active LOW after ESM interrupt generated\n");
#endif
        AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,"Please enter 1 "
        "after probing for ESM Error on test point \n");
        userInput = AppUtils_GetNum();
        while(userInput != 1U)
        {
            AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
                "Please enter 1 \n");
            userInput = AppUtils_GetNum();
        }
    }

    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
        "WDG App Completed as expected!!!\n");
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
    "Please refer user guide on usage details\n");

    return;
}
