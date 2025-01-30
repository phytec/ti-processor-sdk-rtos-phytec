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
 *  \file     AdcApp.c
 *
 *  \brief    This file contains the ADC test example
 *            Configured 2 groups, one with One shot mode and HW Unit assigned
 *            is ADC_0 and second group with Continuous mode assigned to ADC_1.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "AdcApp_Startup.h"
#include "AdcApp.h"
#include <hw_include/lldr_adc.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */


/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */

static void AdcApp_Test(void);

static uint32 AdcApp_procImplicitStop(const Adc_ConfigType *Adc_ConfigPtr);
#if defined (SOC_J721E) || defined (SOC_J721S2) || defined (SOC_J784S4)
static uint32 AdcApp_procExplicitStop(const Adc_ConfigType *Adc_ConfigPtr);
#endif  /* #if defined (SOC_J721E) || defined (SOC_J721S2) || defined (SOC_J784S4) */
static void AdcApp_procIsr(uint32 grpIdx);
static void AdcApp_PrintResult(void);
#if (STD_ON == ADC_INTERNAL_DIAGNOSTIC_DEBUG_MODE_API)
static void AdcApp_EnableDiagnosticDebugMode(void);
static void AdcApp_DisableDiagnosticDebugMode(void);
#endif
static void AdcApp_PrintVersionInfo(void);
/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

uint32                            Adc_TestPassed = E_OK;
/**< App Result Status Flag */
uint32                            Adc_AppLoopCount = 5U;
/**< Number of times App runs */
Adc_ValueGroupType                Adc_AppBuffer[ADC_MAX_GROUP][
    ADC_APP_BUF_SIZE_WORD];
/**< SetUp Result Buffer passed to driver */
Adc_ValueGroupType                Adc_AppReadBuffer[ADC_MAX_GROUP][
    ADC_APP_READ_BUF_SIZE_WORD];
/**< Output Result Buffer */
volatile uint32                   Adc_AppRdBufCount[ADC_MAX_GROUP];
/**< No of Samples Counter */
volatile uint32                   Adc_AppGroupDone[ADC_MAX_GROUP];
/**< Group Conversion Completion flag */
extern const Adc_ConfigType *Adc_ConfigPtr;
/**< Pointer to configuration  */
/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

int main(void)
{

#ifdef UART_ENABLED
    AppUtils_Init();
#endif

    AppUtils_Printf(MSG_NORMAL,
                  "ADC_APP: Sample Application - STARTS !!!\n");

    AdcApp_PowerAndClkSrc();
    AdcApp_Startup();
    AdcApp_PlatformInit();

    AdcApp_PrintVersionInfo();
#if (STD_ON == ADC_INTERNAL_DIAGNOSTIC_DEBUG_MODE_API)
    /* The ADC function is validated by measuring a known reference
       voltage source in Functional Internal Diagnostic Debug Mode */
    AppUtils_Printf(MSG_NORMAL,  "ADC_APP: ADC INTERNAL DIAGNOSTIC DEBUG MODE Test\n");
    AdcApp_EnableDiagnosticDebugMode();
    if (E_OK == Adc_TestPassed)
    {
        AdcApp_DisableDiagnosticDebugMode();
    }

    if (E_OK == Adc_TestPassed)
#endif
    {
        AppUtils_Printf(MSG_NORMAL," \n");
        AppUtils_Printf(MSG_NORMAL,  "ADC_APP: ADC Functional Operation Test\n");
        AdcApp_Test();
    }

#if (ADC_DEINIT_API == STD_ON)
    /* DeInit */
    Adc_DeInit();
#endif  /* #if (ADC_DEINIT_API == STD_ON) */

    if (E_OK == Adc_TestPassed)
    {
        AppUtils_Printf(MSG_NORMAL," \n");
        AppUtils_Printf(MSG_NORMAL,
                  "ADC_APP: ADC Stack Usage: %d bytes\n", AppUtils_GetStackUsage());
        if (AppUtils_AdcCheckStackAndSectionCorruption() != E_OK)
        {
            Adc_TestPassed = E_NOT_OK;
            AppUtils_Printf(MSG_NORMAL,  "ADC_APP: ADC Stack/section corruption!!!\n");
        }
    }

    if (E_OK == Adc_TestPassed)
    {
        AppUtils_Printf(MSG_NORMAL,
                  "ADC_APP: All tests have passed!!!\n");
        AppUtils_LogResult(APP_UTILS_STATUS_PASS);
    }
    else
    {
        AppUtils_Printf(MSG_NORMAL,
                  "ADC_APP: Some tests have Failed!!!\n");
        AppUtils_LogResult(APP_UTILS_STATUS_FAIL);
    }

    AppUtils_Printf(MSG_NORMAL,  "ADC_APP: Sample Application - DONE !!!\n");

    return (Adc_TestPassed);
}

static void AdcApp_Test(void)
{
    uint32              grpIdx;
    uint32              testPassed = E_OK;
    Adc_StatusType      status;
    Std_ReturnType      retVal;

    for (grpIdx = 0U; grpIdx < Adc_ConfigPtr->maxGroup; grpIdx++)
    {
        /* Reset read counter and buffer content */
        Adc_AppRdBufCount[grpIdx] = 0U;
        memset(
            &Adc_AppReadBuffer[grpIdx][0],
            0U,
            (sizeof (Adc_ValueGroupType) * ADC_APP_READ_BUF_SIZE_WORD));

        /* Check group status - it should be idle */
        status = Adc_GetGroupStatus(grpIdx);
        if (status != ADC_IDLE)
        {
            testPassed = E_NOT_OK;
            AppUtils_Printf(MSG_NORMAL, " ADC Group %d is not IDLE!!\n", grpIdx);
        }

        /* Memset result buffer and give to driver */
        memset(
            &Adc_AppBuffer[grpIdx][0],
            0U,
            (sizeof (Adc_ValueGroupType) * ADC_APP_BUF_SIZE_WORD));
        retVal = Adc_SetupResultBuffer(grpIdx, &Adc_AppBuffer[grpIdx][0]);
        if (retVal != E_OK)
        {
            testPassed = E_NOT_OK;
            AppUtils_Printf(MSG_NORMAL, " ADC Group %d setup buffer failed!!\n",
                grpIdx);
        }
    }

    /* One Shot Mode Conversion */
    if ((ADC_CONV_MODE_ONESHOT == Adc_ConfigPtr->groupCfg[ADC_GROUP_ID_0].convMode) ||
        (ADC_STREAM_BUFFER_LINEAR ==
         Adc_ConfigPtr->groupCfg[ADC_GROUP_ID_0].streamBufMode))
    {
        testPassed = AdcApp_procImplicitStop(Adc_ConfigPtr);
    }

#if defined (SOC_J721E) || defined (SOC_J721S2) || (SOC_J784S4)
    /* Continuous Mode Conversion */
    if ((ADC_CONV_MODE_CONTINUOUS == Adc_ConfigPtr->groupCfg[ADC_GROUP_ID_1].convMode) ||
        (ADC_STREAM_BUFFER_CIRCULAR ==
         Adc_ConfigPtr->groupCfg[ADC_GROUP_ID_1].streamBufMode))
    {
        testPassed = AdcApp_procExplicitStop(Adc_ConfigPtr);
    }
#endif  /* #if defined (SOC_J721E) || defined (SOC_J721S2) || (SOC_J784S4) */

    AdcApp_PrintResult();

    if (E_OK == Adc_TestPassed)
    {
        Adc_TestPassed = testPassed;
    }

    return;
}

static uint32 AdcApp_procImplicitStop(const Adc_ConfigType *Adc_ConfigPtr)
{
    uint32         loopCount;
    uint32         testPassed = E_OK, isPending;
    Adc_StatusType status;

    loopCount = 0U;
    while (loopCount < Adc_AppLoopCount)
    {
        /* Reset flags */
        Adc_AppGroupDone[ADC_GROUP_ID_0] = FALSE;

        /* Memset result buffer and give to driver */
        memset(
            &Adc_AppBuffer[ADC_GROUP_ID_0][0],
            0U,
            (sizeof (Adc_ValueGroupType) * ADC_APP_BUF_SIZE_WORD));

#if (ADC_GRP_NOTIF_CAPABILITY_API == STD_ON)
        /* Enable notification */
        Adc_EnableGroupNotification(ADC_GROUP_ID_0);
#endif      /* #if (ADC_GRP_NOTIF_CAPABILITY_API == STD_ON) */

#if (ADC_ENABLE_START_STOP_GROUP_API == STD_ON)
        /* Start conversion */
        Adc_StartGroupConversion(ADC_GROUP_ID_0);
#endif      /* #if (ADC_ENABLE_START_STOP_GROUP_API == STD_ON) */

        /* Wait for all group conversion to get over */
        while (1)
        {
            isPending = FALSE;

            if (Adc_AppGroupDone[ADC_GROUP_ID_0] == FALSE)
            {
                isPending = TRUE;
            }
            if (FALSE == isPending)
            {
                break;
            }
        }

        /* Note: Stop not required as read group API we are calling from
         * ISR callback will move state to idle automatically */
#if (ADC_GRP_NOTIF_CAPABILITY_API == STD_ON)
        Adc_DisableGroupNotification(ADC_GROUP_ID_0);
#endif      /* #if (ADC_GRP_NOTIF_CAPABILITY_API == STD_ON) */

        /* Check group status - it should be idle */
        status = Adc_GetGroupStatus(ADC_GROUP_ID_0);
        if (status != ADC_IDLE)
        {
            testPassed = E_NOT_OK;
            AppUtils_Printf(MSG_NORMAL, " ADC Group %d is not IDLE!!\n",
                  ADC_GROUP_ID_0);
        }

        loopCount++;
    }

    return (testPassed);
}

#if defined (SOC_J721E) || defined (SOC_J721S2) || (SOC_J784S4)
static uint32 AdcApp_procExplicitStop(const Adc_ConfigType *Adc_ConfigPtr)
{
    uint32         testPassed = E_OK, isPending;
    Adc_StatusType status;

    /* Reset flags */
    Adc_AppGroupDone[ADC_GROUP_ID_1] = FALSE;

    /* Memset result buffer and give to driver */
    memset(
        &Adc_AppBuffer[ADC_GROUP_ID_1][0],
        0U,
        (sizeof (Adc_ValueGroupType) * ADC_APP_BUF_SIZE_WORD));

#if (ADC_GRP_NOTIF_CAPABILITY_API == STD_ON)
    /* Enable notification */
    Adc_EnableGroupNotification(ADC_GROUP_ID_1);
#endif  /* #if (ADC_GRP_NOTIF_CAPABILITY_API == STD_ON) */

#if (ADC_ENABLE_START_STOP_GROUP_API == STD_ON)
    /* Start conversion */
    Adc_StartGroupConversion(ADC_GROUP_ID_1);
#endif  /* #if (ADC_ENABLE_START_STOP_GROUP_API == STD_ON) */

    /* Wait for all group conversion to get over for all loopcount */
    while (1)
    {
        isPending = FALSE;

        if (Adc_AppGroupDone[ADC_GROUP_ID_1] == FALSE)
        {
            isPending = TRUE;
        }
        if (FALSE == isPending)
        {
            break;
        }
    }

#if (ADC_ENABLE_START_STOP_GROUP_API == STD_ON)
    /* Stop conversion */
    Adc_StopGroupConversion(ADC_GROUP_ID_1);
#endif  /* #if (ADC_ENABLE_START_STOP_GROUP_API == STD_ON) */

#if (ADC_GRP_NOTIF_CAPABILITY_API == STD_ON)
    Adc_DisableGroupNotification(ADC_GROUP_ID_1);
#endif  /* #if (ADC_GRP_NOTIF_CAPABILITY_API == STD_ON) */

    /* Check group status - it should be idle */
    status = Adc_GetGroupStatus(ADC_GROUP_ID_1);
    if (status != ADC_IDLE)
    {
        testPassed = E_NOT_OK;
        AppUtils_Printf(MSG_NORMAL, " ADC Group %d is not IDLE!!\n",
                  ADC_GROUP_ID_1);
    }

    return (testPassed);
}
#endif  /* #if defined (SOC_J721E) || defined (SOC_J721S2) || (SOC_J784S4) */

void AdcApp_HwUnit0_Group0EndNotification(void)
{
    AdcApp_procIsr(ADC_GROUP_ID_0);
}

#if defined (SOC_J721E) || defined (SOC_J721S2) || (SOC_J784S4)
void AdcApp_HwUnit1_Group0EndNotification(void)
{
    AdcApp_procIsr(ADC_GROUP_ID_1);
}
#endif  /* #if defined (SOC_J721E) || defined (SOC_J721S2) || (SOC_J784S4) */

static void AdcApp_procIsr(uint32 grpIdx)
{
    Adc_StatusType status;
    const Adc_GroupConfigType *grpCfg;
#if (ADC_READ_GROUP_API == STD_ON)
    uint32         offset;
    Std_ReturnType retVal;
#endif  /* #if (ADC_READ_GROUP_API == STD_ON) */

    grpCfg = &Adc_ConfigPtr->groupCfg[grpIdx];
    /* Check group status - it should be COMPLETED state */
    status = Adc_GetGroupStatus(grpIdx);
    if ((status == ADC_COMPLETED) || (status == ADC_STREAM_COMPLETED))
    {
#if (ADC_READ_GROUP_API == STD_ON)
        /* Read and store the result */
        offset = Adc_AppRdBufCount[grpIdx] * grpCfg->numChannels;
        retVal = Adc_ReadGroup(grpIdx, &Adc_AppReadBuffer[grpIdx][offset]);
        if (retVal != E_OK)
        {
            Adc_TestPassed = E_NOT_OK;
            AppUtils_Printf(MSG_NORMAL, "ADC Group %d read buffer failed!!\n",
                  grpIdx);

        }

        /* Move to next set of samples */
        Adc_AppRdBufCount[grpIdx]++;
        if (Adc_AppRdBufCount[grpIdx] > ADC_APP_MAX_READ_SAMPLES)
        {
            /* Keep reading to the last buffer set incase of buffer overflow */
            Adc_AppRdBufCount[grpIdx] = ADC_APP_MAX_READ_SAMPLES - 1U;
        }
#endif  /* #if (ADC_READ_GROUP_API == STD_ON) */
    }
    else
    {
        Adc_TestPassed = E_NOT_OK;
        AppUtils_Printf(MSG_NORMAL, "ADC Group %d is not in read state!!\n",
                  grpIdx);
    }

    if ((ADC_CONV_MODE_ONESHOT == grpCfg->convMode) ||
        (ADC_STREAM_BUFFER_LINEAR == grpCfg->streamBufMode))
    {
        /* post after all streams are completed */
        if ((Adc_AppRdBufCount[grpIdx] % grpCfg->streamNumSamples) == 0U)
        {
            Adc_AppGroupDone[grpIdx] = TRUE;
        }
    }
    else
    {
        /* post after loopcount of all streams */
        if (Adc_AppRdBufCount[grpIdx] >=
            (grpCfg->streamNumSamples * Adc_AppLoopCount))
        {
            Adc_AppGroupDone[grpIdx] = TRUE;
        }
    }

    return;
}

static void AdcApp_PrintResult()
{
    uint32 grpIdx, chIdx;
    uint32 strmIdx, strmOffset;
    uint32 setIdx, setOffset;
    const Adc_GroupConfigType   *grpCfg;
    const Adc_ChannelConfigType *chCfg;
    const Adc_ValueGroupType    *tempResultPtr;
    uint32 voltage;
    uint32 readErr;
    Adc_ValueGroupType lowerLimit, upperLimit;

    AppUtils_Printf(MSG_NORMAL," \n");
    AppUtils_Printf(MSG_NORMAL," Result Buffer Content\n");
    AppUtils_Printf(MSG_NORMAL," ---------------------\n");

    for (grpIdx = 0U; grpIdx < Adc_ConfigPtr->maxGroup; grpIdx++)
    {
        grpCfg = &Adc_ConfigPtr->groupCfg[grpIdx];

        AppUtils_Printf(MSG_NORMAL, " ADC Group %d Log:\n",
                  grpIdx);
        AppUtils_Printf(MSG_NORMAL," ----------------\n");
        AppUtils_Printf(MSG_NORMAL,
                  " Channel    Stream    HW_CH       ADC Value    Volt\n");
        AppUtils_Printf(MSG_NORMAL,
                  " ----------------------------------------------------\n");

        for (chIdx = 0U; chIdx < grpCfg->numChannels; chIdx++)
        {
            chCfg = &grpCfg->channelConfig[chIdx];
            for (strmIdx = 0U; strmIdx < grpCfg->streamNumSamples;
                 strmIdx++)
            {
                strmOffset =
                    (grpCfg->streamNumSamples * chIdx) + strmIdx;
                tempResultPtr = &Adc_AppBuffer[grpIdx][strmOffset];
                voltage       = AppUtils_GetAdcVoltage(*tempResultPtr);

                AppUtils_Printf(MSG_NORMAL,
                    " %-4d       %-4d      ADC_IN%-3d   0x%08x  %4d mV\n",
                    chIdx, strmIdx, chCfg->hwChannelId,
                    *tempResultPtr,
                    voltage);
            }
        }
    }

    AppUtils_Printf(MSG_NORMAL," \n");
    AppUtils_Printf(MSG_NORMAL," Read Buffer Content\n");
    AppUtils_Printf(MSG_NORMAL," -------------------\n");
    for (grpIdx = 0U; grpIdx < Adc_ConfigPtr->maxGroup; grpIdx++)
    {
        readErr = FALSE;
        grpCfg  = &Adc_ConfigPtr->groupCfg[grpIdx];

        AppUtils_Printf(MSG_NORMAL," ADC Group %d Log:\n",
                  grpIdx);
        AppUtils_Printf(MSG_NORMAL," ----------------\n");
        AppUtils_Printf(MSG_NORMAL,
                  " Channel    Set Idx   HW_CH       ADC Value    Volt\n");
        AppUtils_Printf(MSG_NORMAL,
                  " ----------------------------------------------------\n");

        for (chIdx = 0U; chIdx < grpCfg->numChannels; chIdx++)
        {
            chCfg = &grpCfg->channelConfig[chIdx];
            for (setIdx = 0U; setIdx < Adc_AppRdBufCount[grpIdx]; setIdx++)
            {
                setOffset     = (grpCfg->numChannels * setIdx) + chIdx;
                tempResultPtr = &Adc_AppReadBuffer[grpIdx][setOffset];
                voltage       = AppUtils_GetAdcVoltage(*tempResultPtr);

                AppUtils_Printf(MSG_NORMAL,
                    " %-4d       %-4d      ADC_IN%-3d   0x%08x  %4d mV\n",
                    chIdx, setIdx, chCfg->hwChannelId, *tempResultPtr,
                    voltage);

                /* Check for value within range - use first sample value
                 * as the reference and keeping 5% variance i.e appx 200mv */
                if (0U == setIdx)
                {
                    lowerLimit = 0U;
                    if (*tempResultPtr > 200U)
                    {
                        lowerLimit = *tempResultPtr - 200U;
                    }
                    upperLimit = *tempResultPtr + 200U;
                }
                else
                {
                    if ((*tempResultPtr < lowerLimit) ||
                        (*tempResultPtr > upperLimit))
                    {
                        readErr = TRUE;
                    }
                }
            }
        }

        if (TRUE == readErr)
        {
            AppUtils_Printf(MSG_NORMAL,
                      " Warning: ADC Group %d values are out of range!!\n",
                      grpIdx);
        }
    }

    return;
}

#if (STD_ON == ADC_INTERNAL_DIAGNOSTIC_DEBUG_MODE_API)
static void AdcApp_EnableDiagnosticDebugMode(void)
{
    Std_ReturnType      retVal;
    Adc_StatusType      status;
    uint32              testPassed = E_OK;

    /* Check group status - it should be idle */
    status = Adc_GetGroupStatus(ADC_GROUP_ID_0);
    if (status != ADC_IDLE)
    {
        Adc_StopGroupConversion(ADC_GROUP_ID_0);
    }

    retVal = Adc_EnableInternalDiagnostic(ADC_GROUP_ID_0, ADC_CTRL_HW_MID_SEL_ENABLE);
    if(retVal != E_NOT_OK)
    {
        AppUtils_Printf(MSG_NORMAL, "ADC_APP: ADC_UNIT_0 Internal Diagnostic Debug Mode - Enabled\n");
    }
    else
    {
        AppUtils_Printf(MSG_NORMAL, "ADC_APP: ADC_UNIT_0 Failed to enter Internal Diagnostic Debug Mode\n");
        testPassed = E_NOT_OK;
    }

#if defined (SOC_J721E) || defined (SOC_J721S2) || (SOC_J784S4)
    status = Adc_GetGroupStatus(ADC_GROUP_ID_1);
    if (status != ADC_IDLE)
    {
        Adc_StopGroupConversion(ADC_GROUP_ID_1);
    }

    retVal = Adc_EnableInternalDiagnostic(ADC_GROUP_ID_1, ADC_CTRL_HW_MID_SEL_DISABLE);
	  if(retVal != E_NOT_OK)
    {
        AppUtils_Printf(MSG_NORMAL, "ADC_APP: ADC_UNIT_1 Internal Diagnostic Debug Mode - Enabled\n");
    }
    else
    {
        AppUtils_Printf(MSG_NORMAL, "ADC_APP: ADC_UNIT_1 Failed to enter Internal Diagnostic Debug Mode\n");
        testPassed = E_NOT_OK;
    }
#endif
    AdcApp_Test();

    if (E_OK == Adc_TestPassed)
    {
        Adc_TestPassed = testPassed;
    }
}

static void AdcApp_DisableDiagnosticDebugMode(void)
{
    Std_ReturnType      retVal;
    Adc_StatusType      status;
    uint32              testPassed = E_OK;

    status = Adc_GetGroupStatus(ADC_GROUP_ID_0);
    if (status != ADC_IDLE)
    {
        Adc_StopGroupConversion(ADC_GROUP_ID_0);
    }
    retVal = Adc_DisableInternalDiagnostic(ADC_GROUP_ID_0);
    AppUtils_Printf(MSG_NORMAL," \n");
    if(retVal != E_NOT_OK)
    {
        AppUtils_Printf(MSG_NORMAL, "ADC_APP: ADC_UNIT_0 Internal Diagnostic Debug Mode - Disabled \n");
    }
    else
    {
        AppUtils_Printf(MSG_NORMAL, "ADC_APP: ADC_UNIT_0 Failed to disable Internal Diagnostic Debug Mode\n");
        testPassed = E_NOT_OK;
    }
#if defined (SOC_J721E) || defined (SOC_J721S2) || (SOC_J784S4)
    status = Adc_GetGroupStatus(ADC_GROUP_ID_1);
    if (status != ADC_IDLE)
    {
        Adc_StopGroupConversion(ADC_GROUP_ID_1);
    }
    retVal = Adc_DisableInternalDiagnostic(ADC_GROUP_ID_1);
    if(retVal != E_NOT_OK)
    {
        AppUtils_Printf(MSG_NORMAL, "ADC_APP: ADC_UNIT_1 Internal Diagnostic Debug Mode - Disabled\n");
    }
    else
    {
        AppUtils_Printf(MSG_NORMAL, "ADC_APP: ADC_UNIT_1 Failed to disable Internal Diagnostic Debug Mode\n");
        testPassed = E_NOT_OK;
    }
#endif

    if (E_OK == Adc_TestPassed)
    {
        Adc_TestPassed = testPassed;
    }
}
#endif

static void AdcApp_PrintVersionInfo(void)
{
#if (ADC_VERSION_INFO_API == STD_ON)
    Std_VersionInfoType versioninfo;
#endif  /* #if (ADC_VERSION_INFO_API == STD_ON) */

    /* Print test case information */
    AppUtils_Printf(MSG_NORMAL, " \n");
    AppUtils_Printf(MSG_NORMAL,
        "ADC_APP: GROUPs %d: HWUNIT %d: LOOP COUNT %d: NUM STREAMS %d:!!!\n",
        Adc_ConfigPtr->maxGroup,
        Adc_ConfigPtr->maxHwUnit,
        Adc_AppLoopCount,
        Adc_ConfigPtr->groupCfg[0U].streamNumSamples);

#if (ADC_VERSION_INFO_API == STD_ON)
    /* Get and print version */
    Adc_GetVersionInfo(&versioninfo);
    AppUtils_Printf(MSG_NORMAL, " \n");
    AppUtils_Printf(MSG_NORMAL, "ADC_APP:  ADC MCAL Version Info\n");
    AppUtils_Printf(MSG_NORMAL, "ADC_APP: ---------------------\n");
    AppUtils_Printf(MSG_NORMAL, "ADC_APP:  Vendor ID           : %d\n",
                                                versioninfo.vendorID);
    AppUtils_Printf(MSG_NORMAL, "ADC_APP:  Module ID           : %d\n",
                                                versioninfo.moduleID);
    AppUtils_Printf(MSG_NORMAL, "ADC_APP:  SW Major Version    : %d\n",
                                                versioninfo.sw_major_version);
    AppUtils_Printf(MSG_NORMAL, "ADC_APP:  SW Minor Version    : %d\n",
                                                versioninfo.sw_minor_version);
    AppUtils_Printf(MSG_NORMAL, "ADC_APP:  SW Patch Version    : %d\n",
                                                versioninfo.sw_patch_version);
    AppUtils_Printf(MSG_NORMAL, " \n");
#endif  /* #if (ADC_VERSION_INFO_API == STD_ON) */
}
