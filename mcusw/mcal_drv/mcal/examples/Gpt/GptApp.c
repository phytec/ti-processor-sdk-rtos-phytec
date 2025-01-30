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
 *  \file     GptApp.c
 *
 *  \brief    This file contains the GPT test example
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "Std_Types.h"
#include <stdio.h>
#include "string.h"
#include "Det.h"
#include "Dem.h"
#include "Gpt_Cfg.h"
#include "Gpt.h"
#include "Gpt_Irq.h"
#include "SchM_Gpt.h"

#include "Os.h" /* OS Counters */

#include "GptApp.h"

#include "app_utils.h"
#include <hw_include/lldr_intr_router.h> /* CSL for interrupt router */
#if defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J721S2)  || defined (SOC_J784S4)
	#include <ti/drv/pm/include/pm_types.h>
	#include <ti/drv/pm/include/dmsc/pmlib_sysconfig.h>
	#include <ti/drv/pm/include/dmsc/pmlib_clkrate.h>
	/* SCI Client */
	#include <ti/drv/sciclient/sciclient.h>
	/* PM Lib */
	#include <ti/drv/pm/include/pm_types.h>
	#include <ti/drv/pm/include/dmsc/pmlib_sysconfig.h>
	#include <ti/drv/pm/include/dmsc/pmlib_clkrate.h>
	/* Interrupt Registrations */
	#include <ti/osal/osal.h>
#endif
#if defined (SOC_AM62X) || defined (SOC_AM62AX) || defined (SOC_AM62PX) || defined (SOC_J722S)
	/* SCI Client */
	#include <drivers/sciclient.h>
#endif
#include <hw_include/soc.h>
/* Interrupts, Clear MCU Timer 0 */
#include <hw_include/arch/lld_arch.h>
#include <hw_include/hw_types.h>
#include <hw_include/lldr_timer.h>
#include "IntRtr_Cfg.h"
#if defined (SOC_AM62X)
#include <examples/Gpt/soc/am62x/Generated_Dpl/ti_drivers_config.h>
#include <examples/Gpt/soc/am62x/Generated_Dpl/ti_board_config.h>
#endif
#if defined (SOC_AM62AX) 
#include <examples/Gpt/soc/am62ax/Generated_Dpl/ti_drivers_config.h>
#include <examples/Gpt/soc/am62ax/Generated_Dpl/ti_board_config.h>
#endif
#if defined (SOC_AM62PX)
#include <examples/Gpt/soc/am62px/Generated_Dpl/ti_drivers_config.h>
#include <examples/Gpt/soc/am62px/Generated_Dpl/ti_board_config.h>
#endif
#if defined (SOC_J722S) 
//TODO can these not be added to soc specific startup files?
#include <examples/Gpt/soc/j722s/Generated_Dpl/ti_drivers_config.h>
#include <examples/Gpt/soc/j722s/Generated_Dpl/ti_board_config.h>
#endif
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
#if (STD_ON == GPT_ENABLE_DISABLE_NOTIFICATION_API)
static void AppUtils_delay(uint32 count);
#endif
static void GptApp_mainTest(void);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
volatile uint32  GptApp_NotifiedForInstFlag = 0xFFFF;
/**< Flag used for Notification status */
uint32  GptApp_DemoStatus = E_OK;
/**< Flag used for Demo status */

#if (STD_ON == GPT_REPORT_WAKEUP_SOURCE) && (STD_ON == \
                                             GPT_WAKEUP_FUNCTIONALITY_API)
extern volatile uint8 GptApp_WakeupSrcDetFlag;
/**< Check Wakeup notification flag, indicates wakeup source. Set by EcuM */
#endif

/** \brief GPT PC Configuration struct declaration */
extern const struct Gpt_ChannelConfigType_PC_s Gpt_ChannelConfig_PC[GPT_MAX_CHANNELS];
/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

int main(void)
{
    GptApp_DemoStatus = E_OK;
#if defined (SOC_AM62X) || defined (SOC_AM62AX) || defined (SOC_AM62PX) || defined (SOC_J722S)
	System_init();
#endif
#ifdef UART_ENABLED
    AppUtils_Init();
#endif
    AppUtils_LogResult(APP_UTILS_TEST_STATUS_INIT);

    GptApp_Startup();

    /* Initialize memory sections  */
    AppUtils_GptSectionInit();

    AppUtils_Printf(MSG_NORMAL, "\n");
    AppUtils_Printf(MSG_NORMAL, APP_NAME
                                        ": Sample Application - STARTS !!!\n");

    GptApp_mainTest();

    AppUtils_Printf(MSG_NORMAL, APP_NAME
                                                ": GPT Stack Usage %d bytes\n",
                                                AppUtils_GetStackUsage());

    if (AppUtils_GptCheckStackAndSectionCorruption() != E_OK)
    {
        GptApp_DemoStatus = E_NOT_OK;
        AppUtils_Printf(MSG_NORMAL, APP_NAME
                                        ": GPT Stack/section corruption!!!\n");
    }

    if (E_OK == GptApp_DemoStatus)
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

    return (0);
}

static void GptApp_mainTest(void)
{
    uint32                 countVal;
    uint32                 j;
    Gpt_ChannelConfigType *pChannelCfg;
    Gpt_ChannelType        chId;
#if (STD_ON == GPT_REGISTER_READBACK_API)
    Gpt_RegisterReadbackType Gpt_RegRb;
    Std_ReturnType retVal = ((Std_ReturnType) E_OK);
#endif

#if (STD_ON == GPT_TIME_ELAPSED_API) || (STD_ON == GPT_TIME_REMAINING_API)
    uint32                 currCountval;
#endif

#if (GPT_VERSION_INFO_API == STD_ON)
    Std_VersionInfoType    versioninfo;
#endif  /* #if (GPT_VERSION_INFO_API == STD_ON) */

#if (STD_ON == GPT_PRE_COMPILE_VARIANT)
    const Gpt_ConfigType  *pCfgPtr = &GptChannelConfigSet_PC;
    AppUtils_Printf(MSG_NORMAL, APP_NAME ": Variant - Pre Compile being used !!!\n");
#else
    const Gpt_ConfigType  *pCfgPtr = &GptChannelConfigSet;
    AppUtils_Printf(MSG_NORMAL, APP_NAME ": Variant - Post Build being used !!!\n");
#endif

#if (GPT_VERSION_INFO_API == STD_ON)
    /* Get and print version */
    Gpt_GetVersionInfo(&versioninfo);
    AppUtils_Printf(MSG_NORMAL, " \n");
    AppUtils_Printf(MSG_NORMAL, APP_NAME " GPT MCAL Version Info\n");
    AppUtils_Printf(MSG_NORMAL, APP_NAME "---------------------\n");
    AppUtils_Printf(MSG_NORMAL, APP_NAME " Vendor ID           : %d\n",
                                                versioninfo.vendorID);
    AppUtils_Printf(MSG_NORMAL, APP_NAME " Module ID           : %d\n",
                                                versioninfo.moduleID);
    AppUtils_Printf(MSG_NORMAL, APP_NAME " SW Major Version    : %d\n",
                                                versioninfo.sw_major_version);
    AppUtils_Printf(MSG_NORMAL, APP_NAME " SW Minor Version    : %d\n",
                                                versioninfo.sw_minor_version);
    AppUtils_Printf(MSG_NORMAL, APP_NAME " SW Patch Version    : %d\n",
                                                versioninfo.sw_patch_version);
    AppUtils_Printf(MSG_NORMAL, " \n");
#endif

#if (STD_ON == GPT_PRE_COMPILE_VARIANT)
    Gpt_Init(NULL_PTR);
#else
    Gpt_Init(pCfgPtr);
#endif


    for (j = 0U; j < pCfgPtr->channelCount; j++)
    {
        /* Take each channel configuration in configuration set */
        pChannelCfg = (Gpt_ChannelConfigType *)(pCfgPtr->ChannelCfgPtr + j);
        chId = Gpt_ChannelConfig_PC[j].channelId;

#if (STD_ON == GPT_REGISTER_READBACK_API)
        retVal = Gpt_RegisterReadback(chId , &Gpt_RegRb);
        AppUtils_Printf(MSG_NORMAL, " \n");
        AppUtils_Printf(MSG_NORMAL, APP_NAME
                                "------------------------------------------\n");
        if (retVal != E_NOT_OK)
        {
            AppUtils_Printf(MSG_NORMAL, APP_NAME
                                ": GPT Channel %d configuration register values  \n", (chId));
            AppUtils_Printf(MSG_NORMAL, APP_NAME " TIMER_TIDR         : 0x%x\n",
                                Gpt_RegRb.gptRev);
            AppUtils_Printf(MSG_NORMAL, APP_NAME " TIMER_TTGR         : 0x%x\n",
                                Gpt_RegRb.gptTtgr);
            AppUtils_Printf(MSG_NORMAL, APP_NAME " TIMER_TSICR        : 0x%x\n",
                                Gpt_RegRb.gptTimerSynCtrl);
            AppUtils_Printf(MSG_NORMAL, APP_NAME " TIMER_TIOCP_CFG    : 0x%x\n",
                                Gpt_RegRb.gptTiocpCfg);
            AppUtils_Printf(MSG_NORMAL, APP_NAME " TIMER_TCLR         : 0x%x\n",
                                Gpt_RegRb.gptTclr);
        }
        else
        {
            AppUtils_Printf(MSG_NORMAL, APP_NAME
                              "GPT Register readback Failed  \n");
            GptApp_DemoStatus = E_NOT_OK;
        }
#endif
        AppUtils_Printf(MSG_NORMAL,
                                "------------------------------------------\n");
        AppUtils_Printf(MSG_NORMAL, APP_NAME
                                ": Running GPT Test for channel %d\n", (chId));
        AppUtils_Printf(MSG_NORMAL,
                                "------------------------------------------\n");

#if defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J721S2)  || defined (SOC_J784S4)
 	countVal = GPTIMER_COUNT_VALUE_6_SEC_25_MHZ_INPUT;
	if (GPT_CH_GPTIMER4 < chId)
        {
            countVal = GPTIMER_COUNT_VALUE_6_SEC_25_MHZ_INPUT;
        }
#elif defined (SOC_AM62X) || defined (SOC_AM62AX) || defined (SOC_AM62PX) || defined (SOC_J722S)
            countVal = GPTIMER_COUNT_VALUE_6_SEC_25_MHZ_INPUT;
#endif
        #if (STD_ON == GPT_ENABLE_DISABLE_NOTIFICATION_API)
        Gpt_EnableNotification(chId);
        AppUtils_Printf(MSG_NORMAL,
            APP_NAME ": Enabled notification for channel [%d]\n", chId);
        #endif

        Gpt_StartTimer(chId, countVal);
        AppUtils_Printf(MSG_NORMAL, APP_NAME
                                        ": Started Timer Channel [%d]\n", chId);

        #if (STD_ON == GPT_TIME_ELAPSED_API)
        currCountval = Gpt_GetTimeElapsed(chId);
        AppUtils_Printf(MSG_NORMAL, APP_NAME
                                        ": Elapsed Time Value = %x\n",
                        currCountval);
        #endif

        #if (STD_ON == GPT_ENABLE_DISABLE_NOTIFICATION_API)
        AppUtils_Printf(MSG_NORMAL,
            APP_NAME ": Wait for notification(approx. 6 seconds)\n");
        /* Wait till you get timer notifications */
        while (chId != GptApp_NotifiedForInstFlag)
        {
            AppUtils_delay(1000U);
        }
        AppUtils_Printf(MSG_NORMAL, "\n");
        GptApp_NotifiedForInstFlag = 0xFFFF;
        #endif

        #if (STD_ON == GPT_ENABLE_DISABLE_NOTIFICATION_API)
        AppUtils_Printf(MSG_NORMAL,
            APP_NAME ": Disable channel notification for this channel\n");
        Gpt_DisableNotification(chId);
        GptApp_NotifiedForInstFlag = 0xFFFF;
        #endif

        /* If not in one shot mode */
        if (GPT_CH_MODE_ONESHOT != pChannelCfg->channelMode)
        {
            AppUtils_Printf(MSG_NORMAL, APP_NAME
                                    ": Wait till timer overflows, no "
                                     "notification should be received\n");
            #if (STD_ON == GPT_TIME_ELAPSED_API)
            currCountval = Gpt_GetTimeElapsed(chId);
            AppUtils_Printf(MSG_NORMAL, APP_NAME
                                ": Time Elapsed Value = 0x%x\n", currCountval);
            #endif
            #if (STD_ON == GPT_TIME_REMAINING_API)
            currCountval = Gpt_GetTimeRemaining(chId);
            AppUtils_Printf(MSG_NORMAL, APP_NAME
                            ": Time Remaining Value = 0x%x\n", currCountval);
            #endif

            #if (STD_ON == GPT_TIME_REMAINING_API)
            AppUtils_Printf(MSG_NORMAL, APP_NAME
                                        ": Waiting for timer to overflow\n");
            /* Wait till timer overflows, notification is not expected to be
             * received */
            while (currCountval)
            {
                static uint32 prevCountVal = 0U;
                prevCountVal = currCountval;
                currCountval = Gpt_GetTimeRemaining(chId);
                if (prevCountVal > currCountval)
                {
                    break;
                }
            }
            if (0xFFFF == GptApp_NotifiedForInstFlag)
            {
                AppUtils_Printf(MSG_NORMAL, APP_NAME
                        ": Overflow happened no notification received\n");
            }
            else
            {
                AppUtils_Printf(MSG_NORMAL, APP_NAME
                                                            ": Error !!!\n");
                AppUtils_Printf(MSG_NORMAL, APP_NAME
                                            ": Received notification, while "
                                            "notification was disabled\n");
                GptApp_DemoStatus = E_NOT_OK;
            }
            #endif /* (STD_ON == GPT_TIME_REMAINING_API) */
        }

        AppUtils_Printf(MSG_NORMAL, "\n");

#if (STD_ON == GPT_REPORT_WAKEUP_SOURCE) && (STD_ON == \
                                             GPT_WAKEUP_FUNCTIONALITY_API)

        Gpt_StopTimer(chId);
        AppUtils_Printf(MSG_NORMAL, APP_NAME
                                            ": Stopped for channel %d\n", chId);
        AppUtils_Printf(MSG_NORMAL, APP_NAME
                                        ": Enable wakeup for this channel\n");
        Gpt_EnableWakeup(chId);

        Gpt_StartTimer(chId, countVal);
        AppUtils_Printf(MSG_NORMAL, APP_NAME
                                        ": Started timer channel [%d]\n", chId);
        /* Enable sleep mode as wakeup only allowed in sleep mode
         * \Note: set mode to sleep should be called after enabling wakeup
         *        for timer channel, else timer will be stopped
         */
        Gpt_SetMode(GPT_MODE_SLEEP);
        /* Gpt_CheckWakeup will check if wakeup capable timer channel is
         * source for a wakeup event and call EcuM_SetWakeupEvent to
         * indicate the valid timer wakeup event to the EcuM. */
        AppUtils_Printf(MSG_NORMAL,
            APP_NAME
            ": check if this channel is wakeup source for any wakeup event\n");
        Gpt_CheckWakeup(pChannelCfg->wakeupSourceRef);
        /* Wait for call to EcuM_SetWakeupEvent as wakeup is mapped to
         * current channel */
        while ((uint8) TRUE != GptApp_WakeupSrcDetFlag)
        {
            AppUtils_delay(1000U);
        }
        AppUtils_Printf(MSG_NORMAL, APP_NAME
                                        ": Woken up for channel [%d]\n",chId);
        GptApp_WakeupSrcDetFlag = (uint8) FALSE;
        Gpt_DisableWakeup(chId);
#endif

        AppUtils_Printf(MSG_NORMAL, APP_NAME
                                                            ": Stop timer \n");
        Gpt_StopTimer(chId);

        AppUtils_Printf(MSG_NORMAL, APP_NAME
                        ": GPT example passed for channel =%d !!!\n", (chId));
    }

    /* Deinit current configuration */
#if (STD_ON == GPT_DEINIT_API)
    Gpt_DeInit();
#endif

    AppUtils_Printf(MSG_NORMAL, APP_NAME
                                            ": GPT example Completed !!!\n");
}

void Gpt_Channel_Notify1(void)
{
    AppUtils_Printf(MSG_NORMAL,
        APP_NAME ": GPT Notification received for channel 0 !!!\n");
    GptApp_NotifiedForInstFlag = 0U; /* Set it to channel number */
}

void Gpt_Channel_Notify2(void)
{
    AppUtils_Printf(MSG_NORMAL,
        APP_NAME ": GPT Notification received for channel 1 !!!\n");
    GptApp_NotifiedForInstFlag = 1U; /* Set it to channel number */
}
#if defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4) || defined (SOC_AM62AX) || defined (SOC_AM62PX) || defined (SOC_J722S)
void Gpt_Channel_Notify3(void)
{
    AppUtils_Printf(MSG_NORMAL,
        APP_NAME ": GPT Notification received for channel 2 !!!\n");
    GptApp_NotifiedForInstFlag = 2U; /* Set it to channel number */
}

void Gpt_Channel_Notify4(void)
{
    AppUtils_Printf(MSG_NORMAL,
        APP_NAME ": GPT Notification received for channel 3 !!!\n");
    GptApp_NotifiedForInstFlag = 3U; /* Set it to channel number */
}

void Gpt_Channel_Notify5(void)
{
    AppUtils_Printf(MSG_NORMAL,
        APP_NAME ": GPT Notification received for channel 4 !!!\n");
    GptApp_NotifiedForInstFlag = 4U; /* Set it to channel number */
}

void Gpt_Channel_Notify6(void)
{
    AppUtils_Printf(MSG_NORMAL,
        APP_NAME ": GPT Notification received for channel 5 !!!\n");
    GptApp_NotifiedForInstFlag = 5U; /* Set it to channel number */
}

void Gpt_Channel_Notify7(void)
{
    AppUtils_Printf(MSG_NORMAL,
        APP_NAME ": GPT Notification received for channel 6 !!!\n");
    GptApp_NotifiedForInstFlag = 6U; /* Set it to channel number */
}

void Gpt_Channel_Notify8(void)
{
    AppUtils_Printf(MSG_NORMAL,
        APP_NAME ": GPT Notification received for channel 7 !!!\n");
    GptApp_NotifiedForInstFlag = 7U; /* Set it to channel number */
}


void Gpt_Channel_Notify9(void)
{
    AppUtils_Printf(MSG_NORMAL,
        APP_NAME ": GPT Notification received for channel 8 !!!\n");
    GptApp_NotifiedForInstFlag = 8U; /* Set it to channel number */
}

void Gpt_Channel_Notify10(void)
{
    AppUtils_Printf(MSG_NORMAL,
        APP_NAME ": GPT Notification received for channel 9 !!!\n");
    GptApp_NotifiedForInstFlag = 9U; /* Set it to channel number */
}

void Gpt_Channel_Notify11(void)
{
    AppUtils_Printf(MSG_NORMAL,
        APP_NAME ": GPT Notification received for channel 10 !!!\n");
    GptApp_NotifiedForInstFlag = 10U; /* Set it to channel number */
}

void Gpt_Channel_Notify12(void)
{
    AppUtils_Printf(MSG_NORMAL,
        APP_NAME ": GPT Notification received for channel 11 !!!\n");
    GptApp_NotifiedForInstFlag = 11U; /* Set it to channel number */
}

void Gpt_Channel_Notify13(void)
{
    AppUtils_Printf(MSG_NORMAL,
        APP_NAME ": GPT Notification received for channel 12 !!!\n");
    GptApp_NotifiedForInstFlag = 12U; /* Set it to channel number */
}

void Gpt_Channel_Notify14(void)
{
    AppUtils_Printf(MSG_NORMAL,
        APP_NAME ": GPT Notification received for channel 13 !!!\n");
    GptApp_NotifiedForInstFlag = 13U; /* Set it to channel number */
}

void Gpt_Channel_Notify15(void)
{
    AppUtils_Printf(MSG_NORMAL,
        APP_NAME ": GPT Notification received for channel 14 !!!\n");
    GptApp_NotifiedForInstFlag = 14U; /* Set it to channel number */
}

void Gpt_Channel_Notify16(void)
{
    AppUtils_Printf(MSG_NORMAL,
        APP_NAME ": GPT Notification received for channel 15 !!!\n");
    GptApp_NotifiedForInstFlag = 15U; /* Set it to channel number */
}

#endif

#if defined (SOC_J721E)
void Gpt_Channel_Notify30(void)
{
    AppUtils_Printf(MSG_NORMAL,
        APP_NAME ": GPT Notification received for channel 30 !!!\n");
    GptApp_NotifiedForInstFlag = 29U; /* Set it to channel number */
}
#endif

#if defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
void Gpt_Channel_Notify18(void)
{
    AppUtils_Printf(MSG_NORMAL,
        APP_NAME ": GPT Notification received for channel 18 !!!\n");
    GptApp_NotifiedForInstFlag = 17U; /* Set it to channel number */
}
#endif

/* ========================================================================== */
/*                        Application Local Functions                         */
/* ========================================================================== */
#if (STD_ON == GPT_ENABLE_DISABLE_NOTIFICATION_API)
/** \brief Local delay function */
void AppUtils_delay (uint32 count)
{
    volatile uint32 x = count;
    while (x)
        x--;
}
#endif

#define GPT_START_SEC_ISR_CODE
#include "Gpt_MemMap.h"

#if defined (SOC_J721E) || (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
FUNC(void, GPT_CODE_FAST) GptApp_GptXIsr(uintptr_t GptPtr)
{
    GptApp_IsrType gptChIsr = (GptApp_IsrType)GptPtr;

    /* Associated GPT ISR */
    gptChIsr();
}
#elif defined (SOC_AM62X) || defined (SOC_AM62AX) || defined (SOC_AM62PX) || defined (SOC_J722S)
FUNC(void, GPT_CODE_FAST) GptApp_GptXIsr(void * GptPtr)
{
    GptApp_IsrType gptChIsr = (GptApp_IsrType)GptPtr;

    /* Associated GPT ISR */
    gptChIsr();
}

#endif

#define GPT_STOP_SEC_ISR_CODE
#include "Gpt_MemMap.h"

/*EoF*/
