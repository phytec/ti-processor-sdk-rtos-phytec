/*
*
* Copyright (C) 2024 Texas Instruments Incorporated
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
#include "Pwm_Cfg.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
#define MSG_NORMAL      (APP_UTILS_PRINT_MSG_NORMAL)
/**< Message type */
#define MSG_STATUS      (APP_UTILS_PRINT_MSG_STATUS)
/**< Message type */

#define TEST_EHRPWM /* Enable to test EHRPWM example */
#define ONE_KHZ_FREQ				1000U    /* 1 Khz Frequency */
#define TEN_KHZ_FREQ				10000U   /* 10 Khz Frequency */
#define ONE_TWENTY_FIVE_MHZ_FREQ	1250000U /* 1.25Mhz Frequency */
/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */
static void PwmApp_mainTest(void);
static void IsrCntValidation(uint32 frequency, uint32 DutyCycle);
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
    PwmApp_PowerAndClkSrc();

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
    uint32 clkSrc, inputduty;
#if (PWM_INDEX == 1U) /*GPT*/
    uint32 periodinticks;
    AppUtils_Printf(MSG_NORMAL,APP_NAME  ": GPTimers being used with Channel # %d!!!\n", PWM_CHANNEL);

#elif (PWM_INDEX == 0U) /*EPWM*/
    uint32 inputfreq, period;
    AppUtils_Printf(MSG_NORMAL,APP_NAME  ": EPWM being used with Channel # %d!!!\n", PWM_CHANNEL);
#endif /*EPWM*/
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
    AppUtils_Printf(MSG_NORMAL,APP_NAME  ": PWM Channel Initialized \n");
    AppUtils_Printf(MSG_NORMAL,APP_NAME ": PWM Duty cycle: 60 Percent, 1000Hz \n");
#if(STD_ON == PWM_REGISTER_READBACK_API)
    Pwm_RegisterReadbackType RegReadback, *RegRbPtr;
    Std_ReturnType      retVal;
    RegRbPtr = &RegReadback;
    retVal = Pwm_RegisterReadback(PWM_CHANNEL,RegRbPtr);
    if (retVal != E_OK)
    {
        AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
          APP_NAME ": PWM configuration Register Readback Failed!!\n");
    }
    else
    {
        AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL, APP_NAME
          ": PWM CHANNEL %d configuration Register Readback values  \n",
           (PWM_CHANNEL) );
        AppUtils_Printf( APP_UTILS_PRINT_MSG_NORMAL, APP_NAME
          ": EPWM_TBCTL 		: 0x%x\n", RegRbPtr->pwmTbCtl);
        AppUtils_Printf( APP_UTILS_PRINT_MSG_NORMAL, APP_NAME
          ": EPWM_TBPHS 		: 0x%x\n", RegRbPtr->pwmTbPhs);
        AppUtils_Printf( APP_UTILS_PRINT_MSG_NORMAL, APP_NAME
          ": EPWM_TBCNT 		: 0x%x\n", RegRbPtr->pwmTbCnt);
        AppUtils_Printf( APP_UTILS_PRINT_MSG_NORMAL, APP_NAME
          ": EPWM_AQCTLA 		: 0x%x\n", RegRbPtr->pwmAqCtlA);
        AppUtils_Printf( APP_UTILS_PRINT_MSG_NORMAL, APP_NAME
          ": EPWM_AQCTLB 		: 0x%x\n", RegRbPtr->pwmAqCtlB);
        AppUtils_Printf( APP_UTILS_PRINT_MSG_NORMAL, APP_NAME
          ": EPWM_DBCTL 		: 0x%x\n", RegRbPtr->pwmDbCtl);
        AppUtils_Printf( APP_UTILS_PRINT_MSG_NORMAL, APP_NAME
          ": EPWM_TZSEL 		: 0x%x\n", RegRbPtr->pwmTzSel);
        AppUtils_Printf( APP_UTILS_PRINT_MSG_NORMAL, APP_NAME
          ": EPWM_TZCTL 		: 0x%x\n", RegRbPtr->pwmTzCtl);
        AppUtils_Printf( APP_UTILS_PRINT_MSG_NORMAL, APP_NAME
          ": EPWM_PCCTL 		: 0x%x\n", RegRbPtr->pwmPcCtl);
        AppUtils_Printf( APP_UTILS_PRINT_MSG_NORMAL, APP_NAME
            ": EPWM_TBSTS 		: 0x%x\n", RegRbPtr->pwmTbsts);
        AppUtils_Printf( APP_UTILS_PRINT_MSG_NORMAL, APP_NAME
            ": EPWM_TBPRD 		: 0x%x\n", RegRbPtr->pwmTbprd);
        AppUtils_Printf( APP_UTILS_PRINT_MSG_NORMAL, APP_NAME
            ": EPWM_AQSFRC 		: 0x%x\n", RegRbPtr->pwmAqsfrc);
        AppUtils_Printf( APP_UTILS_PRINT_MSG_NORMAL, APP_NAME
            ": EPWM_AQSCFRC 	 	: 0x%x\n", RegRbPtr->pwmAqcsfrc);
        AppUtils_Printf( APP_UTILS_PRINT_MSG_NORMAL, APP_NAME
            ": EPWM_Aqsfrc 		: 0x%x\n", RegRbPtr->pwmDbred);
        AppUtils_Printf( APP_UTILS_PRINT_MSG_NORMAL, APP_NAME
            ": EPWM_Aqsfrc 		: 0x%x\n", RegRbPtr->pwmDbfed);
        AppUtils_Printf( APP_UTILS_PRINT_MSG_NORMAL, APP_NAME
            ": EPWM_Aqsfrc 		: 0x%x\n", RegRbPtr->pwmEtsel);
}
#endif /* #if STD_ON == PWM_REGISTER_READBACK_API */

#if (PWM_INDEX == 0U)/*EPWM*/
#if defined (SOC_J721E) || defined (SOC_J7200)
    AppUtils_Printf(MSG_NORMAL,APP_NAME  ": Probe EPWM in Main domain Test Connector Pin 11 or 2 on GESI Board (A or B) in J721E EVM and J7200 EVM\n");
#elif  defined (SOC_J721S2)
	#if defined(BUILD_MCU1_0)
		AppUtils_Printf(MSG_NORMAL,APP_NAME  ": Validate J721S2 EPWM application through ISR count\n");
	#else
		AppUtils_Printf(MSG_NORMAL,APP_NAME  ": Probe EPWM in Main domain Test Connector J24 Pin 42 on GESI Board in J721S2 EVM\n");
#endif
#elif defined (SOC_J784S4)
	#if defined(BUILD_MCU1_0)
		AppUtils_Printf(MSG_NORMAL,APP_NAME  ": Validate J784S4 EPWM application through ISR count\n");
	#else
		AppUtils_Printf(MSG_NORMAL,APP_NAME  ": Probe EPWM in Main domain Test Pin TP126 on EVM in J784S4 EVM\n");
	#endif
#endif
#endif /*EPWM*/

#if ((defined(BUILD_MCU1_0)) && (defined (SOC_J721S2) || defined(SOC_J784S4)))
    {
        AppUtils_Printf(MSG_NORMAL,APP_NAME  ": This example waits for 30 seconds validating through ISR count\n");
        IsrCntValidation(ONE_KHZ_FREQ, 0x6666U);
    }
#else
    {
        AppUtils_Printf(MSG_NORMAL,APP_NAME  ": This example waits for 30 seconds please probe\n");
        Osal_delay(30000U);
    }
#endif

#if (STD_ON == PWM_SET_DUTY_CYCLE_API)
    /* Duty Cycle 80% */
    AppUtils_Printf(MSG_NORMAL, " \n");
    AppUtils_Printf(MSG_NORMAL,APP_NAME  ": Changing the Duty cycle from to 60 to 80 percent \n\r");
    inputduty = 0x6666U;
    Pwm_SetDutyCycle(PWM_CHANNEL, inputduty);

#if ((defined(BUILD_MCU1_0)) && (defined (SOC_J721S2) || defined(SOC_J784S4)))
    AppUtils_Printf(MSG_NORMAL,APP_NAME  ": This example waits for 30 seconds validating through ISR count\n");
    IsrCntValidation(ONE_KHZ_FREQ, inputduty);
#else
    AppUtils_Printf(MSG_NORMAL,APP_NAME  ": This example waits for 30 seconds Please probe\n");
    Osal_delay(30000U);
#endif
#endif
#if (STD_ON == PWM_SET_DUTY_CYCLE_API)
    /* Duty Cycle 100% */
    AppUtils_Printf(MSG_NORMAL, " \n");
    AppUtils_Printf(MSG_NORMAL,APP_NAME  ": Changing the Duty cycle to 100 Percent \n\r");
    inputduty = 0x8000U;
    Pwm_SetDutyCycle(PWM_CHANNEL, inputduty);
#if ((defined(BUILD_MCU1_0)) && (defined (SOC_J721S2) || defined(SOC_J784S4)))
    AppUtils_Printf(MSG_NORMAL,APP_NAME  ": This example waits for 10 seconds validating through ISR count\n");
    IsrCntValidation(ONE_KHZ_FREQ,inputduty);
#else
    AppUtils_Printf(MSG_NORMAL,APP_NAME  ": This example waits for 10 seconds Please probe\n");
    Osal_delay(10000U);
#endif
#endif

#if (STD_ON == PWM_SET_OUTPUT_TO_IDLE_API)
    /* Output to Idle */
    AppUtils_Printf(MSG_NORMAL, " \n");
    AppUtils_Printf(MSG_NORMAL,APP_NAME  ": Setting Output to Idle state \n\r");
    Pwm_SetOutputToIdle(PWM_CHANNEL);
#if ((defined(BUILD_MCU1_0)) && (defined (SOC_J721S2) || defined(SOC_J784S4)))
    AppUtils_Printf(MSG_NORMAL,APP_NAME  ": This example  waits for 10 seconds validating through ISR count\n");
    IsrCntValidation(ONE_KHZ_FREQ, inputduty);
#else
	AppUtils_Printf(MSG_NORMAL,APP_NAME  ": This example waits for 10 seconds Please probe\n");
    Osal_delay(10000U);
#endif
#endif

#if (STD_ON == PWM_SET_PERIOD_AND_DUTY_API)
    AppUtils_Printf(MSG_NORMAL, " \n");
    AppUtils_Printf(MSG_NORMAL,APP_NAME
         ": Changing Frequency from 1kHz to 10KHz and Duty Cycle to 50%%\n\r");

    inputfreq = 10000U;
    /* 50% duty cycle */
    /* Counter Direction set is - UPDOWN Mode to generate a symmetric PWM
     * PRD = (TBCLK/PWM_FREQ) / 2
     * TBCLK = (SYSCLK / (HSPCLKDIV x CLKDIV) , PWM_FREQ = 1 / TPWM
     * SYSCLK - System Clock(PWM_SYS_CLOCK)
     * HSPCLKDIV and CLKDIV are Prescalers to calculate Time base clock
     * Please refer PWM User guide for more details
     * Make sure period changed should use same prescalers used in init */
    clkSrc = pCfgPtr->chCfg[0].instanceClkHz;
    period = (clkSrc / (1U * 1U)); /* Prescalers */
    period = (period / inputfreq) / 2; /* UPDOWN Mode so / 2 */

    AppUtils_Printf(MSG_NORMAL,APP_NAME ": period is set to %d\n\r", period);
    inputduty = 0x4000U;
    Pwm_SetPeriodAndDuty(PWM_CHANNEL, period, inputduty);
    AppUtils_Printf(MSG_NORMAL,APP_NAME  ": This example validating through ISR count\n");
    IsrCntValidation(TEN_KHZ_FREQ,inputduty);
#endif /*PWM_SET_PERIOD_AND_DUTY_API */

#if defined TEST_EHRPWM

#if (STD_ON == PWM_SET_PERIOD_AND_DUTY_API)
    AppUtils_Printf(MSG_NORMAL, " \n");
    AppUtils_Printf(MSG_NORMAL,APP_NAME
         ": Changing Frequency from 10kHz to 1.25MHz and Duty Cycle to 40.5%%\n\r");

    inputfreq = 1250000U;
    /* 50% duty cycle */
    /* Counter Direction set is - UPDOWN Mode to generate a symmetric PWM
     * PRD = (TBCLK/PWM_FREQ) / 2
     * TBCLK = (SYSCLK / (HSPCLKDIV x CLKDIV) , PWM_FREQ = 1 / TPWM
     * SYSCLK - System Clock(PWM_SYS_CLOCK)
     * HSPCLKDIV and CLKDIV are Prescalers to calculate Time base clock
     * Please refer PWM User guide for more details
     * Make sure period changed should use same prescalers used in init */
    clkSrc = pCfgPtr->chCfg[0].instanceClkHz;
    period = (clkSrc / (1U * 1U)); /* Prescalers */
    period = (period / inputfreq) / 2; /* UPDOWN Mode so / 2 */

    AppUtils_Printf(MSG_NORMAL,APP_NAME ": period is set to %d\n\r", period);
    inputduty = 0x33d7U;
    Pwm_SetPeriodAndDuty(PWM_CHANNEL, period, inputduty);
#endif /*PWM_SET_PERIOD_AND_DUTY_API */
    AppUtils_Printf(MSG_NORMAL,APP_NAME ": This app again waits for 30 seconds please probe\n");
    Osal_delay(30000);

#endif /* TEST_EHRPWM */
}

#if (STD_ON == PWM_NOTIFICATION_SUPPORTED)
void Pwm_Notification_Channel1(void)
{
    PwmApp_notifycnt++;
}
#endif

void IsrCntValidation( uint32 frequency, uint32 DutyCycle)
{
#if (STD_ON == PWM_NOTIFICATION_SUPPORTED)
    uint32 startTime = 0U, endTime = 0U, elapsed = 0U;
#endif
#if (STD_ON == PWM_NOTIFICATION_SUPPORTED)
    Pwm_EnableNotification(PWM_CHANNEL, PWM_FALLING_EDGE);

    startTime = TimerP_getTimeInUsecs();

    /* Wait for ISR count */
#if (STD_ON == PWM_SET_DUTY_CYCLE_API) || \
        (STD_ON == PWM_SET_PERIOD_AND_DUTY_API)
    if ((DutyCycle == PWM_DUTY_0_PERCENT) ||
            (DutyCycle == PWM_DUTY_100_PERCENT))
    {
        Osal_delay(10000);
    }
    else
#endif /*PWM_SET_DUTY_CYCLE_API*/
    {
        while (PwmApp_notifycnt < (frequency * 30U)); /*Multiplied with 30 for 30 seconds run */
    }

    endTime = TimerP_getTimeInUsecs();
    AppUtils_Printf(MSG_NORMAL,APP_NAME
                    ": Pwm Isr Count: %u \n\r", PwmApp_notifycnt);
    elapsed = endTime - startTime;
    AppUtils_Printf(MSG_NORMAL,APP_NAME
                    ": App Run time: %u micro secs \n", elapsed);
#endif /*PWM_NOTIFICATION_SUPPORTED*/

#if (STD_ON == PWM_NOTIFICATION_SUPPORTED)
    /* Disable Notifications */
    AppUtils_Printf(MSG_NORMAL, " \n");
    AppUtils_Printf(MSG_NORMAL,APP_NAME  ": Disabling Notifications for PWM channel");
    Pwm_DisableNotification(PWM_CHANNEL);
	PwmApp_notifycnt = 0;
#endif
}
/*EoF*/
