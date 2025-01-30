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
 *  \file     IcuApp.c
 *
 *  \brief    This file contains the ICU test example
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "IcuApp.h"
#include "IcuApp_Startup.h"

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
static void IcuApp_mainTest(void);

static void Epwm_Init();
/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */


uint32  IcuApp_DemoStatus = E_OK;
volatile boolean Icu_NotifyIntervalReached = FALSE;
volatile boolean Icu_SignalNotificationReached = FALSE;
volatile uint32 Icu_SignalNotificationNumber = 0U;
/**< Flag used for Demo status */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

int main(void)
{
    IcuApp_DemoStatus = E_OK;
#ifdef UART_ENABLED
    AppUtils_Init();
#endif
    AppUtils_LogResult(APP_UTILS_TEST_STATUS_INIT);

    IcuApp_Startup();

    /* Initialize counters, that would be required for timed operations */
    AppUtils_ProfileInit(0);

    /* Initialize memory sections  */
    AppUtils_IcuSectionInit();
    /* Initialize memory sections  */

#if (!((defined(BUILD_MCU1_0)) && (defined (SOC_J721S2) || defined(SOC_J784S4))))
	AppUtils_PwmSectionInit();
#endif


    AppUtils_Printf(MSG_NORMAL, "\n");
    AppUtils_Printf(MSG_NORMAL, APP_NAME
                                        ": Sample Application - STARTS !!!\n");

    IcuApp_mainTest();

    AppUtils_Printf(MSG_NORMAL, APP_NAME
                                                ": Icu Stack Usage %d bytes\n",
                                                AppUtils_GetStackUsage());
    if (AppUtils_IcuCheckStackAndSectionCorruption() != E_OK)
    {
        IcuApp_DemoStatus = E_NOT_OK;
        AppUtils_Printf(MSG_NORMAL, APP_NAME
                                        ": ICU Stack/section corruption!!!\n");
    }

    if (E_OK == IcuApp_DemoStatus)
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
static void IcuApp_mainTest(void)
{

#if (STD_ON == ICU_SIGNAL_MEASUREMENT_API)
    uint16 elapsedTime = 0U;
    Icu_DutyCycleType dutyCycleApp;
    uint16 periodTimeApp, activeTimeApp = 0U;
#endif
#if (STD_ON == ICU_TIMESTAMP_API)
    Icu_ActivationType activation = ICU_RISING_EDGE;
    uint16 bufferSize = 6;
    Icu_ValueType timestampArray [bufferSize];
#endif
#if (STD_ON == ICU_EDGE_COUNT_API)
    Icu_EdgeNumberType edgecount = 0;
#endif
#if (STD_ON == ICU_EDGE_DETECT_API)
    Icu_InputStateType inputstate;
#endif

#if (((defined(BUILD_MCU1_0)) && (defined (SOC_J721S2) || defined(SOC_J784S4))))
uint32 userInput = 0;
#endif

#if (ICU_GET_VERSION_INFO_API == STD_ON)

    Std_VersionInfoType versioninfo;

    /* Version Info Check*/
    Icu_GetVersionInfo(&versioninfo);
    AppUtils_Printf(MSG_NORMAL, " \n");
    AppUtils_Printf(MSG_NORMAL, APP_NAME ": ICU MCAL Version Info\n");
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

/* ICU Init*/
#if (STD_ON == ICU_PRE_COMPILE_VARIANT)
    AppUtils_Printf(MSG_NORMAL,APP_NAME  ": Variant - Pre Compile being used !!!\n");
#else
    AppUtils_Printf(MSG_NORMAL,APP_NAME  ": Variant - Post Build being used !!!\n");
#endif
    AppUtils_Printf(MSG_NORMAL, " \n");

#if defined(SOC_J721S2) && defined(BUILD_MCU1_0)
    AppUtils_Printf(MSG_NORMAL,APP_NAME  ": Provide EPWM signal with Duty cycle: 60 Percent, 1000Hz as input to ECAP test point TP82 on SOM board for J721S2 EVM\n");
#endif

#if defined(SOC_J784S4) && defined(BUILD_MCU1_0)
    AppUtils_Printf(MSG_NORMAL,APP_NAME  ": Provide EPWM signal with Duty cycle: 60 Percent, 1000Hz as input to ECAP test point TP93 on EVM for J784S4 EVM\n");
#endif

#if (STD_ON == ICU_PRE_COMPILE_VARIANT)
#if (STD_ON == ICU_SIGNAL_MEASUREMENT_API || STD_ON == ICU_EDGE_COUNT_API || STD_ON == ICU_EDGE_DETECT_API || STD_ON == ICU_TIMESTAMP_API)
    const Icu_ConfigType *pCfgPtr = &ICU_INIT_CONFIG_PC;
#endif
    Icu_Init(NULL_PTR);
#else
    const Icu_ConfigType *pCfgPtr = &IcuConfigSet;
    Icu_Init(pCfgPtr);
#endif

    /*Using EPWM as test Input for capture*/
#if (!((defined(BUILD_MCU1_0)) && (defined (SOC_J721S2) || defined(SOC_J784S4))))
    Epwm_Init();
#endif

#if (((defined(BUILD_MCU1_0)) && (defined (SOC_J721S2) || defined(SOC_J784S4))))
	AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,"Please enter 1 after providing signal from function generator as per above defined configuration.\n");
	userInput = AppUtils_GetNum();
	while(userInput != 1U)
	{
		AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
			"Please enter 1 \n");
		userInput = AppUtils_GetNum();
	}
#endif

/*ICU_SAFETY_DIAGNOSTIC_API*/
#if (STD_ON == ICU_SAFETY_DIAGNOSTIC_API)
Icu_RegisterReadbackType RegRbPtr;
uint32 Channel;
 for (Channel = 0; Channel < ICU_MAX_NUM_CHANNELS; Channel++)
  {
     Std_ReturnType retVal = (Std_ReturnType) E_OK;
     retVal = Icu_RegisterReadback(Channel , &RegRbPtr);
     AppUtils_Printf(MSG_NORMAL, " \n");
     AppUtils_Printf(MSG_NORMAL, APP_NAME
                        "------------------------------------------\n");
   if (retVal != E_NOT_OK)
     {
        AppUtils_Printf(MSG_NORMAL, APP_NAME
                        ": ICU Channel %d configuration register values  \n", (Channel));
        AppUtils_Printf(MSG_NORMAL, APP_NAME " ECAP_CNTPHS        : 0x%x\n",
                        RegRbPtr.ECAP_CNTPHS);
        AppUtils_Printf(MSG_NORMAL, APP_NAME " ECAP_ECCTL         : 0x%x\n",
                        RegRbPtr.ECAP_ECCTL);
        AppUtils_Printf(MSG_NORMAL, APP_NAME " ECAP_ECINT_EN_FLG  : 0x%x\n",
                        RegRbPtr.ECAP_ECINT_EN_FLG);
     }
  else
     {
       AppUtils_Printf(MSG_NORMAL, APP_NAME
                      "ICU Register readback Failed  \n");
       IcuApp_DemoStatus = E_NOT_OK;
     }
 }
#endif

#if (STD_ON == ICU_SIGNAL_MEASUREMENT_API || STD_ON == ICU_EDGE_COUNT_API || STD_ON == ICU_EDGE_DETECT_API || STD_ON == ICU_TIMESTAMP_API)
    Icu_MeasurementModeType mode = pCfgPtr->chCfg[0].measurementMode;
#endif

#if (STD_ON == ICU_SIGNAL_MEASUREMENT_API)

    if (mode == ICU_MODE_SIGNAL_MEASUREMENT)
    {
        AppUtils_Printf(MSG_NORMAL,APP_NAME ": Signal Measurement Mode! \n\r");
        Icu_SignalMeasurementPropertyType type = pCfgPtr->chCfg[0].signalMeasurementProperty;

        Icu_StartSignalMeasurement(ICU_CHANNEL);
        Osal_delay(100U);
        Icu_StopSignalMeasurement(ICU_CHANNEL);

        elapsedTime = Icu_GetTimeElapsed(ICU_CHANNEL);
        if (type == ICU_PERIOD_TIME)
            AppUtils_Printf(MSG_NORMAL,APP_NAME ": elapsed time (Period) is  %d us\n\r", elapsedTime);
        else if (type == ICU_HIGH_TIME)
            AppUtils_Printf(MSG_NORMAL,APP_NAME ": elapsed time (High Time) is  %d us\n\r", elapsedTime);
        else if (type == ICU_LOW_TIME)
            AppUtils_Printf(MSG_NORMAL,APP_NAME ": elapsed time (Low Time) is  %d us\n\r", elapsedTime);

        Icu_GetDutyCycleValues(ICU_CHANNEL, &dutyCycleApp);
        activeTimeApp = dutyCycleApp.ActiveTime;
        periodTimeApp = dutyCycleApp.PeriodTime;
        AppUtils_Printf(MSG_NORMAL,APP_NAME ": Active Time is %d \n\r", activeTimeApp);
        AppUtils_Printf(MSG_NORMAL,APP_NAME ": Period Time is %d \n\r", periodTimeApp);

        elapsedTime = Icu_GetTimeElapsed(ICU_CHANNEL);
        AppUtils_Printf(MSG_NORMAL,APP_NAME ": elapsed time read is  %d us\n\r", elapsedTime);

        Icu_StartSignalMeasurement(ICU_CHANNEL);
        Osal_delay(100U);
        Icu_StopSignalMeasurement(ICU_CHANNEL);

        elapsedTime = Icu_GetTimeElapsed(ICU_CHANNEL);
        if (type == ICU_PERIOD_TIME)
            AppUtils_Printf(MSG_NORMAL,APP_NAME ": elapsed time (Period) is  %d us\n\r", elapsedTime);
        else if (type == ICU_HIGH_TIME)
            AppUtils_Printf(MSG_NORMAL,APP_NAME ": elapsed time (High Time) is  %d us\n\r", elapsedTime);
        else if (type == ICU_LOW_TIME)
            AppUtils_Printf(MSG_NORMAL,APP_NAME ": elapsed time (Low Time) is  %d us\n\r", elapsedTime);

        Icu_GetDutyCycleValues(ICU_CHANNEL, &dutyCycleApp);
        activeTimeApp = dutyCycleApp.ActiveTime;
        periodTimeApp = dutyCycleApp.PeriodTime;
        AppUtils_Printf(MSG_NORMAL,APP_NAME ": Active Time is %d \n\r", activeTimeApp);
        AppUtils_Printf(MSG_NORMAL,APP_NAME ": Period Time is %d \n\r", periodTimeApp);

    }

#endif /* ICU_SIGNAL_MEASUREMENT_API*/

#if (STD_ON == ICU_TIMESTAMP_API)

    if (mode == ICU_MODE_TIMESTAMP)
    {
        AppUtils_Printf(MSG_NORMAL,APP_NAME ": Timestamp Mode! \n\r");

        if (activation == ICU_RISING_EDGE)
            AppUtils_Printf(MSG_NORMAL,APP_NAME ": Setting activation edge to RISING. \n\r");
        else if (activation == ICU_FALLING_EDGE)
            AppUtils_Printf(MSG_NORMAL,APP_NAME ": Setting activation edge to FALLING. \n\r");
        else if (activation == ICU_BOTH_EDGES)
            AppUtils_Printf(MSG_NORMAL,APP_NAME ": Setting activation edge to BOTH. \n\r");

        Icu_SetActivationCondition(ICU_CHANNEL, activation);

        Icu_EnableNotification(ICU_CHANNEL);
        Icu_GetTimestampIndex(ICU_CHANNEL);
        Icu_StartTimestamp(ICU_CHANNEL, &timestampArray[0], bufferSize, 6);
        Osal_delay(100);

        while (!Icu_NotifyIntervalReached)
        {
            AppUtils_Printf(MSG_NORMAL,APP_NAME ": still looping in timestamp \n\r");
        }
        Icu_NotifyIntervalReached = FALSE;

        AppUtils_Printf(MSG_NORMAL,APP_NAME ": Array value 0 is %d \n\r", timestampArray[0]);
        AppUtils_Printf(MSG_NORMAL,APP_NAME ": Array value 1 is %d \n\r", timestampArray[1]);
        AppUtils_Printf(MSG_NORMAL,APP_NAME ": Array value 2 is %d \n\r", timestampArray[2]);
        AppUtils_Printf(MSG_NORMAL,APP_NAME ": Array value 3 is %d \n\r", timestampArray[3]);
        AppUtils_Printf(MSG_NORMAL,APP_NAME ": Array value 4 is %d \n\r", timestampArray[4]);
        AppUtils_Printf(MSG_NORMAL,APP_NAME ": Array value 5 is %d \n\r", timestampArray[5]);

        Icu_StopTimestamp(ICU_CHANNEL);

    }

#endif /* ICU_TIMESTAMP_API*/

#if (STD_ON == ICU_EDGE_COUNT_API)

    if (mode == ICU_MODE_EDGE_COUNTER)
    {
        AppUtils_Printf(MSG_NORMAL,APP_NAME ": Edge Count Mode! \n\r");

        Icu_EnableEdgeCount(ICU_CHANNEL);
        /* Wait 1 seconds */
        Osal_delay(1000);
        Icu_DisableEdgeCount(ICU_CHANNEL);

        edgecount = Icu_GetEdgeNumbers(ICU_CHANNEL);
        AppUtils_Printf(MSG_NORMAL,APP_NAME ": edgecount in 1 sec is  %d \n\r", edgecount);

        Icu_ResetEdgeCount(ICU_CHANNEL);

        edgecount = Icu_GetEdgeNumbers(ICU_CHANNEL);
        AppUtils_Printf(MSG_NORMAL,APP_NAME ": edgecount after reset is %d \n\r", edgecount);

    }

#endif

#if (STD_ON == ICU_EDGE_DETECT_API)

    if (mode == ICU_MODE_SIGNAL_EDGE_DETECT)
    {
        AppUtils_Printf(MSG_NORMAL,APP_NAME ": Edge Detect Mode! \n\r");

        inputstate = Icu_GetInputState(ICU_CHANNEL);
        if(inputstate == ICU_ACTIVE)
            AppUtils_Printf(MSG_NORMAL,APP_NAME ": input state is ICU_ACTIVE\n\r");
        else if (inputstate == ICU_IDLE)
            AppUtils_Printf(MSG_NORMAL,APP_NAME ": input state is ICU_IDLE \n\r");

        Icu_EnableNotification(ICU_CHANNEL);

        Icu_EnableEdgeDetection(ICU_CHANNEL);

        Osal_delay(1000);

        AppUtils_Printf(MSG_NORMAL,APP_NAME ": SignalNotification for Double Edge Detection Reached in 1 sec: %d \n\r", Icu_SignalNotificationNumber);
        Icu_SignalNotificationNumber = 0U;

        Icu_DisableEdgeDetection(ICU_CHANNEL);

        Icu_SetActivationCondition(ICU_CHANNEL,ICU_RISING_EDGE);

        Icu_EnableEdgeDetection(ICU_CHANNEL);

        Osal_delay(1000);

        AppUtils_Printf(MSG_NORMAL,APP_NAME ": SignalNotification for Single Edge Detection Reached in 1 sec: %d \n\r", Icu_SignalNotificationNumber);
        Icu_SignalNotificationNumber = 0U;

        Icu_DisableEdgeDetection(ICU_CHANNEL);

        inputstate = Icu_GetInputState(ICU_CHANNEL);
        if(inputstate == ICU_ACTIVE)
            AppUtils_Printf(MSG_NORMAL,APP_NAME ": input state is ICU_ACTIVE\n\r");
        else if (inputstate == ICU_IDLE)
            AppUtils_Printf(MSG_NORMAL,APP_NAME ": input state is ICU_IDLE\n\r");

    }

#endif

    AppUtils_Printf(MSG_NORMAL, " \n");
    AppUtils_Printf(MSG_NORMAL,APP_NAME
         ": Changing EPWM Frequency from 1000Hz to 25000Hz and Duty Cycle to 50%%\n\r");
#if (!((defined(BUILD_MCU1_0)) && (defined (SOC_J721S2) || defined(SOC_J784S4))))
    uint32 clkSrc, inputduty;

    uint32 inputfreq, period;

    inputfreq = 25000U;
    /* 50% duty cycle */
    /* Counter Direction set is - UPDOWN Mode to generate a symmetric PWM
     * PRD = (TBCLK/PWM_FREQ) / 2
     * TBCLK = (SYSCLK / (HSPCLKDIV x CLKDIV) , PWM_FREQ = 1 / TPWM
     * SYSCLK - System Clock(PWM_SYS_CLOCK)
     * HSPCLKDIV and CLKDIV are Prescalers to calculate Time base clock
     * Please refer PWM User guide for more details
     * Make sure period changed should use same prescalers used in init */
    clkSrc = 125000000;
    period = (clkSrc / (1U * 1U)); /* Prescalers */
    period = (period / inputfreq) / 2; /* UPDOWN Mode so / 2 */
    inputduty = 0x4000U;
    #if defined (SOC_J721S2)
    Pwm_SetPeriodAndDuty(PWM_CHANNEL1, period, inputduty);
    #elif defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J784S4)
    Pwm_SetPeriodAndDuty(PWM_CHANNEL2, period, inputduty);
    #endif
#else
	AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,"Please enter 1 after providing signal from function generator as per above defined configuration.\n");
	userInput = AppUtils_GetNum();
	while(userInput != 1U)
	{
		AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
			"Please enter 1 \n");
		userInput = AppUtils_GetNum();
	}
#endif

#if (STD_ON == ICU_SIGNAL_MEASUREMENT_API)

    if (mode == ICU_MODE_SIGNAL_MEASUREMENT)
    {
        AppUtils_Printf(MSG_NORMAL,APP_NAME ": Signal Measurement Mode! \n\r");
        Icu_SignalMeasurementPropertyType type = pCfgPtr->chCfg[0].signalMeasurementProperty;

        Icu_StartSignalMeasurement(ICU_CHANNEL);
        Osal_delay(100U);
        Icu_StopSignalMeasurement(ICU_CHANNEL);

        elapsedTime = Icu_GetTimeElapsed(ICU_CHANNEL);
        if (type == ICU_PERIOD_TIME)
            AppUtils_Printf(MSG_NORMAL,APP_NAME ": elapsed time (Period) is  %d us\n\r", elapsedTime);
        else if (type == ICU_HIGH_TIME)
            AppUtils_Printf(MSG_NORMAL,APP_NAME ": elapsed time (High Time) is  %d us\n\r", elapsedTime);
        else if (type == ICU_LOW_TIME)
            AppUtils_Printf(MSG_NORMAL,APP_NAME ": elapsed time (Low Time) is  %d us\n\r", elapsedTime);

        Icu_GetDutyCycleValues(ICU_CHANNEL, &dutyCycleApp);
        activeTimeApp = dutyCycleApp.ActiveTime;
        periodTimeApp = dutyCycleApp.PeriodTime;
        AppUtils_Printf(MSG_NORMAL,APP_NAME ": Active Time is %d \n\r", activeTimeApp);
        AppUtils_Printf(MSG_NORMAL,APP_NAME ": Period Time is %d \n\r", periodTimeApp);

        elapsedTime = Icu_GetTimeElapsed(ICU_CHANNEL);
        AppUtils_Printf(MSG_NORMAL,APP_NAME ": elapsed time read is  %d us\n\r", elapsedTime);

        Icu_StartSignalMeasurement(ICU_CHANNEL);
        Osal_delay(100U);
        Icu_StopSignalMeasurement(ICU_CHANNEL);

        elapsedTime = Icu_GetTimeElapsed(ICU_CHANNEL);
        if (type == ICU_PERIOD_TIME)
            AppUtils_Printf(MSG_NORMAL,APP_NAME ": elapsed time (Period) is  %d us\n\r", elapsedTime);
        else if (type == ICU_HIGH_TIME)
            AppUtils_Printf(MSG_NORMAL,APP_NAME ": elapsed time (High Time) is  %d us\n\r", elapsedTime);
        else if (type == ICU_LOW_TIME)
            AppUtils_Printf(MSG_NORMAL,APP_NAME ": elapsed time (Low Time) is  %d us\n\r", elapsedTime);

        Icu_GetDutyCycleValues(ICU_CHANNEL, &dutyCycleApp);
        activeTimeApp = dutyCycleApp.ActiveTime;
        periodTimeApp = dutyCycleApp.PeriodTime;
        AppUtils_Printf(MSG_NORMAL,APP_NAME ": Active Time is %d \n\r", activeTimeApp);
        AppUtils_Printf(MSG_NORMAL,APP_NAME ": Period Time is %d \n\r", periodTimeApp);

    }

#endif /* ICU_SIGNAL_MEASUREMENT_API*/

#if (STD_ON == ICU_TIMESTAMP_API)

    if (mode == ICU_MODE_TIMESTAMP)
    {
        AppUtils_Printf(MSG_NORMAL,APP_NAME ": Timestamp Mode! \n\r");

        if (activation == ICU_RISING_EDGE)
            AppUtils_Printf(MSG_NORMAL,APP_NAME ": Setting activation edge to RISING. \n\r");
        else if (activation == ICU_FALLING_EDGE)
            AppUtils_Printf(MSG_NORMAL,APP_NAME ": Setting activation edge to FALLING. \n\r");
        else if (activation == ICU_BOTH_EDGES)
            AppUtils_Printf(MSG_NORMAL,APP_NAME ": Setting activation edge to BOTH. \n\r");

        Icu_SetActivationCondition(ICU_CHANNEL, activation);

        Icu_EnableNotification(ICU_CHANNEL);
        Icu_StartTimestamp(ICU_CHANNEL, &timestampArray[0], bufferSize, 6);
        Osal_delay(100);

        while (!Icu_NotifyIntervalReached)
        {
            AppUtils_Printf(MSG_NORMAL,APP_NAME ": still looping in timestamp \n\r");
        }
        Icu_NotifyIntervalReached = FALSE;

        AppUtils_Printf(MSG_NORMAL,APP_NAME ": Array value 0 is %d \n\r", timestampArray[0]);
        AppUtils_Printf(MSG_NORMAL,APP_NAME ": Array value 1 is %d \n\r", timestampArray[1]);
        AppUtils_Printf(MSG_NORMAL,APP_NAME ": Array value 2 is %d \n\r", timestampArray[2]);
        AppUtils_Printf(MSG_NORMAL,APP_NAME ": Array value 3 is %d \n\r", timestampArray[3]);
        AppUtils_Printf(MSG_NORMAL,APP_NAME ": Array value 4 is %d \n\r", timestampArray[4]);
        AppUtils_Printf(MSG_NORMAL,APP_NAME ": Array value 5 is %d \n\r", timestampArray[5]);

        Icu_StopTimestamp(ICU_CHANNEL);

    }

#endif /* ICU_TIMESTAMP_API*/

#if (STD_ON == ICU_EDGE_COUNT_API)

    if (mode == ICU_MODE_EDGE_COUNTER)
    {
        AppUtils_Printf(MSG_NORMAL,APP_NAME ": Edge Count Mode! \n\r");

        Icu_EnableEdgeCount(ICU_CHANNEL);
        /* Wait 1 seconds */
        Osal_delay(1000);
        Icu_DisableEdgeCount(ICU_CHANNEL);

        edgecount = Icu_GetEdgeNumbers(ICU_CHANNEL);
        AppUtils_Printf(MSG_NORMAL,APP_NAME ": edgecount in 1 sec is  %d \n\r", edgecount);

        Icu_ResetEdgeCount(ICU_CHANNEL);

        edgecount = Icu_GetEdgeNumbers(ICU_CHANNEL);
        AppUtils_Printf(MSG_NORMAL,APP_NAME ": edgecount after reset is %d \n\r", edgecount);

    }

#endif

#if (STD_ON == ICU_EDGE_DETECT_API)

    if (mode == ICU_MODE_SIGNAL_EDGE_DETECT)
    {
        AppUtils_Printf(MSG_NORMAL,APP_NAME ": Edge Detect Mode! \n\r");

        inputstate = Icu_GetInputState(ICU_CHANNEL);
        if(inputstate == ICU_ACTIVE)
            AppUtils_Printf(MSG_NORMAL,APP_NAME ": input state is ICU_ACTIVE\n\r");
        else if (inputstate == ICU_IDLE)
            AppUtils_Printf(MSG_NORMAL,APP_NAME ": input state is ICU_IDLE \n\r");

        Icu_SetActivationCondition(ICU_CHANNEL,ICU_RISING_EDGE);

        Icu_EnableNotification(ICU_CHANNEL);

        Icu_EnableEdgeDetection(ICU_CHANNEL);

        Osal_delay(1000);

        AppUtils_Printf(MSG_NORMAL,APP_NAME ": SignalNotification for Single Edge Detection Reached in 1 sec: %d \n\r", Icu_SignalNotificationNumber);
        Icu_SignalNotificationNumber = 0U;

        Icu_DisableEdgeDetection(ICU_CHANNEL);

        Icu_DisableNotification(ICU_CHANNEL);
        inputstate = Icu_GetInputState(ICU_CHANNEL);
        if(inputstate == ICU_ACTIVE)
            AppUtils_Printf(MSG_NORMAL,APP_NAME ": input state is ICU_ACTIVE\n\r");
        else if (inputstate == ICU_IDLE)
            AppUtils_Printf(MSG_NORMAL,APP_NAME ": input state is ICU_IDLE\n\r");

        Icu_EnableEdgeDetection(ICU_CHANNEL);

        Osal_delay(1000);

        AppUtils_Printf(MSG_NORMAL,APP_NAME ": SignalNotification for after disabling notification Edge Detection Reached: %d \n\r", Icu_SignalNotificationNumber);
        Icu_SignalNotificationNumber = 0U;

        Icu_DisableEdgeDetection(ICU_CHANNEL);
#if defined (SOC_J721E)
        AppUtils_Printf(MSG_NORMAL,APP_NAME
                ": Icu Isr Count: %u \n\r", Icu_ch1Notify);
#endif
#if defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
        AppUtils_Printf(MSG_NORMAL,APP_NAME
                 ": Icu Isr Count: %u \n\r", Icu_ch2Notify);
#endif

    }

#endif

#if (STD_ON == ICU_DE_INIT_API)

    AppUtils_Printf(MSG_NORMAL,APP_NAME ": Calling Deinit \n\r");

    Icu_DeInit();

#endif


}

static void Epwm_Init()
{
 
    PwmApp_Startup();
    PwmApp_PowerAndClkSrc();

#if (STD_ON == PWM_PRE_COMPILE_VARIANT)

    Pwm_Init(NULL_PTR);

#else
    Pwm_Init(pCfgPtr);
#endif

    AppUtils_Printf(MSG_NORMAL,APP_NAME  ": EPWM Channel Initialized \n");
    AppUtils_Printf(MSG_NORMAL,APP_NAME ": EPWM Duty cycle: 60 Percent, 1000Hz \n");
#if defined (SOC_J721E)
    AppUtils_Printf(MSG_NORMAL,APP_NAME  ": Use EPWM Pin 2/11 in J22 on GESI Board as input to ECAP test point TP97 on base board\n");
#elif defined (SOC_J7200)
    AppUtils_Printf(MSG_NORMAL,APP_NAME  ": Use EPWM Pin 2/11 in J22 on GESI Board as input to ECAP test point TP30 on SOM board\n");
#elif defined (SOC_J721S2)
    AppUtils_Printf(MSG_NORMAL,APP_NAME  ": Use EPWM Pin 42 in J24 on GESI Board as input to ECAP test point TP82 on SOM board\n");
#elif defined (SOC_J784S4)
    AppUtils_Printf(MSG_NORMAL,APP_NAME  ": Use EPWM test point TP126 as input to ECAP test point TP93 on EVM\n");
#endif


}

void Icu_SignalNotification_Channel2(void)
{
    Icu_SignalNotificationNumber++;
}

void Icu_TimeStampNotification_Channel2(void)
{
    Icu_NotifyIntervalReached = TRUE;
}

void Icu_SignalNotification_Channel1(void)
{
    Icu_SignalNotificationNumber++;
}

void Icu_TimeStampNotification_Channel1(void)
{
    Icu_NotifyIntervalReached = TRUE;
}


#if (STD_ON == PWM_NOTIFICATION_SUPPORTED)
void Pwm_Notification_Channel1(void)
{
    /* None*/
}
#endif

/*EoF*/
