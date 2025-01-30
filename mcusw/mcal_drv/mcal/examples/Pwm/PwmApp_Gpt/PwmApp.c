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
 *  \file     PwmApp.c
 *
 *  \brief    This file contains the PWM test example
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "PwmApp.h"
#include "PwmApp_Startup.h"

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
static void PwmApp_mainTest(void);
/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
uint32  PwmApp_DemoStatus = E_OK;
/**< Flag used for Demo status */
#if (STD_ON == PWM_NOTIFICATION_SUPPORTED)
volatile static uint32  PwmApp_notifycnt = 0U;
#endif

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

int main(void)
{
    PwmApp_DemoStatus = E_OK;
#ifdef UART_ENABLED
    AppUtils_Init();
#endif
    AppUtils_LogResult(APP_UTILS_TEST_STATUS_INIT);

    PwmApp_Startup();

    /* Initialize counters, that would be required for timed operations */
    AppUtils_ProfileInit(0);

    /* Initialize memory sections  */
    AppUtils_PwmSectionInit();

    AppUtils_Printf(MSG_NORMAL, "\n");
    AppUtils_Printf(MSG_NORMAL, APP_NAME
                                        ": Sample Application - STARTS !!!\n");

    PwmApp_mainTest();

    AppUtils_Printf(MSG_NORMAL, APP_NAME
                                                ": Pwm Stack Usage %d bytes\n",
                                                AppUtils_GetStackUsage());
    if (AppUtils_PwmCheckStackAndSectionCorruption() != E_OK)
    {
        PwmApp_DemoStatus = E_NOT_OK;
        AppUtils_Printf(MSG_NORMAL, APP_NAME
                                        ": PWM Stack/section corruption!!!\n");
    }

    if (E_OK == PwmApp_DemoStatus)
    {
        AppUtils_Printf(MSG_NORMAL, APP_NAME
                                        ": All tests have passed!!!\n");
        AppUtils_LogResult(APP_UTILS_TEST_STATUS_PASS);
    }
    else
    {
        AppUtils_Printf(MSG_NORMAL, APP_NAME
                                        ": Some tests have Failed!!!\n");
        AppUtils_LogResult(APP_UTILS_TEST_STATUS_FAIL);
    }

    return 0;
}

/* ========================================================================== */
/*                        Application Local Functions                         */
/* ========================================================================== */
static void PwmApp_mainTest(void)
{
#if (STD_ON == PWM_SET_DUTY_CYCLE_API)
    uint32 inputduty, periodinticks, clkSrc;
#endif
#if (PWM_VERSION_INFO_API == STD_ON)
    Std_VersionInfoType versioninfo;

    /* Version Info Check*/
    Pwm_GetVersionInfo(&versioninfo);
    AppUtils_Printf(MSG_NORMAL, " \n");
    AppUtils_Printf(MSG_NORMAL, APP_NAME ": PWM MCAL Version Info\n");
    AppUtils_Printf(MSG_NORMAL, "---------------------\n");
    AppUtils_Printf(MSG_NORMAL, APP_NAME ": Vendor ID           : %d\n",
                                                        versioninfo.vendorID);
    AppUtils_Printf(MSG_NORMAL, APP_NAME ": Module ID           : %d\n",
                                                        versioninfo.moduleID);
    AppUtils_Printf(MSG_NORMAL, APP_NAME ": SW Major Version    : %d\n",
                                                versioninfo.sw_major_version);
    AppUtils_Printf(MSG_NORMAL, APP_NAME ": SW Minor Version    : %d\n",
                                                versioninfo.sw_minor_version);
    AppUtils_Printf(MSG_NORMAL, APP_NAME ": SW Patch Version    : %d\n",
                                                versioninfo.sw_patch_version);
    AppUtils_Printf(MSG_NORMAL, " \n");
#endif

    /* PWM Init*/
#if (STD_ON == PWM_PRE_COMPILE_VARIANT)
    AppUtils_Printf(MSG_NORMAL,APP_NAME  ": Variant - Pre Compile being used !!!\n");
#else
    AppUtils_Printf(MSG_NORMAL,APP_NAME  ": Variant - Post Build being used !!!\n");
#endif
    AppUtils_Printf(MSG_NORMAL, " \n");
#if (STD_ON == PWM_PRE_COMPILE_VARIANT)
	const Pwm_ConfigType *pCfgPtr = &PWM_INIT_CONFIG_PC;
    Pwm_Init(NULL_PTR);
#else
    const Pwm_ConfigType *pCfgPtr = &PwmChannelConfigSet;
    Pwm_Init(pCfgPtr);
#endif
#if(STD_ON == PWM_REGISTER_READBACK_API)
    Pwm_RegisterReadbackType RegReadback, *RegRbPtr;
    Std_ReturnType      retVal;
    RegRbPtr = &RegReadback;
    retVal = Pwm_RegisterReadback(PWM_CHANNEL, RegRbPtr);
    if (retVal != E_OK)
    {
        AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL, APP_NAME
          ":PWM configuration Register Readback Failed!!\n");
    }
    else
    {
        AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL, APP_NAME
          ": PWM CHANNEL %d configuration Register Readback values  \n",
           (PWM_CHANNEL));
        AppUtils_Printf( APP_UTILS_PRINT_MSG_NORMAL, APP_NAME
          ": PWM_REV 			: 0x%x\n", RegRbPtr->pwmRev);
        AppUtils_Printf( APP_UTILS_PRINT_MSG_NORMAL, APP_NAME
          ": PWM_TTGR 			: 0x%x\n", RegRbPtr->pwmTtgr);
        AppUtils_Printf( APP_UTILS_PRINT_MSG_NORMAL, APP_NAME
          ": PWM_TMSYNCTRL 		: 0x%x\n", RegRbPtr->pwmTimerSynCtrl);
    }
#endif /* #if STD_ON == PWM_REGISTER_READBACK_API */

    AppUtils_Printf(MSG_NORMAL,APP_NAME  ": PWM Channel Initialized \n");
    AppUtils_Printf(MSG_NORMAL,APP_NAME ": PWM Duty cycle: 50 Percent, PWM Period: 1 sec\n");

#if defined (SOC_J721E)
    AppUtils_Printf(MSG_NORMAL,APP_NAME  ": Probe TIMER in Main domain(Test Point TP97)");
    AppUtils_Printf(MSG_NORMAL,"in J721E EVM\n");
#elif defined (SOC_J7200)
    AppUtils_Printf(MSG_NORMAL,APP_NAME  ": Probe TIMER in Main domain(J5F Pin F15)");
    AppUtils_Printf(MSG_NORMAL,"in J7200 EVM\n");
#elif defined (SOC_J721S2)
    AppUtils_Printf(MSG_NORMAL,APP_NAME  ": Probe TIMER in Main domain(Test Point TP82)");
    AppUtils_Printf(MSG_NORMAL,"in J721S2 EVM\n");
#elif defined (SOC_J784S4)
    AppUtils_Printf(MSG_NORMAL,APP_NAME  ": Probe TIMER in Main domain(Test Pin TP93)");
    AppUtils_Printf(MSG_NORMAL,"in J784S4 EVM\n");
#endif

    AppUtils_Printf(MSG_NORMAL,APP_NAME  ": This example waits for 10 seconds please probe\n");
    Osal_delay(10000U);


#if (STD_ON == PWM_SET_DUTY_CYCLE_API)
    /* Duty Cycle 80% */
    AppUtils_Printf(MSG_NORMAL, " \n");
    AppUtils_Printf(MSG_NORMAL,APP_NAME  ": Changing the Duty cycle from 50 Percent to 80 Percent \n\r");
    inputduty = 0x6666U;
    Pwm_SetDutyCycle(PWM_CHANNEL, inputduty);
    AppUtils_Printf(MSG_NORMAL,APP_NAME  ": This example waits for 10 seconds Please probe\n");
    Osal_delay(1000U);
#endif

#if (STD_ON == PWM_SET_OUTPUT_TO_IDLE_API)
    /* Output to Idle */
    AppUtils_Printf(MSG_NORMAL, " \n");
    AppUtils_Printf(MSG_NORMAL,APP_NAME  ": Setting Output to Idle state \n\r");
    Pwm_SetOutputToIdle(PWM_CHANNEL);
    AppUtils_Printf(MSG_NORMAL,APP_NAME  ": This example waits for 10 seconds Please probe\n");
    Osal_delay(10000U);
#endif

#if (STD_ON == PWM_SET_PERIOD_AND_DUTY_API)
    /* Update period and duty*/
    AppUtils_Printf(MSG_NORMAL, " \n");
    AppUtils_Printf(MSG_NORMAL,APP_NAME  ": Changing PWM Period from 1s to 500 ms and Duty Cycle to 50%%\n\r");
    inputduty = 0x4000U;
    clkSrc = pCfgPtr->chCfg[0].instanceClkHz;
    periodinticks = (uint32)((clkSrc / 1000U) * 500U);
    Pwm_SetPeriodAndDuty(PWM_CHANNEL, periodinticks , inputduty);
    AppUtils_Printf(MSG_NORMAL,APP_NAME  ": This example waits for 10 seconds Please probe\n");
    Osal_delay(10000U);
#endif
#if (STD_ON == PWM_NOTIFICATION_SUPPORTED)
    /* Enable Notifications */
    AppUtils_Printf(MSG_NORMAL, " \n");
    AppUtils_Printf(MSG_NORMAL,APP_NAME  ": Enabling Notifications for PWM channel\r");
    Pwm_EnableNotification(PWM_CHANNEL, PWM_FALLING_EDGE);
    AppUtils_Printf(MSG_NORMAL,APP_NAME  ": This example waits for 10 seconds please probe\n");
    Osal_delay(10000U);
    /* Wait for ISR count */
    AppUtils_Printf(MSG_NORMAL,APP_NAME
              ": Pwm Isr Count: %u \n\r", PwmApp_notifycnt);

  while (PwmApp_notifycnt < 20U);
  if (PwmApp_notifycnt < 20U)
  {
    PwmApp_DemoStatus = E_NOT_OK;
  }
#endif

#if (STD_ON == PWM_NOTIFICATION_SUPPORTED)
    /* Disable Notifications */
    AppUtils_Printf(MSG_NORMAL, " \n");
    AppUtils_Printf(MSG_NORMAL,APP_NAME  ": Disabling Notifications for PWM channel\r");
    Pwm_DisableNotification(PWM_CHANNEL);
#endif

#if (STD_ON == PWM_DEINIT_API)
    /* De-init */
    Pwm_DeInit();
    AppUtils_Printf(MSG_NORMAL, " \n");
    AppUtils_Printf(MSG_NORMAL,APP_NAME  ": PWM De-initialized\r\n");
#endif
}

#if (STD_ON == PWM_NOTIFICATION_SUPPORTED)
void Pwm_Notification_Channel1(void)
{
    PwmApp_notifycnt++;
}
#endif
/*EoF*/
