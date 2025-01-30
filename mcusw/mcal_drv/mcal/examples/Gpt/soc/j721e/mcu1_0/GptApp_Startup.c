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
 *  \file     GptAppStartup.c
 *
 *  \brief    Initializes platform & core specifics to ensure GPT example
 *              can be operated
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
#include "IntRtr_Cfg.h" /* Defines the map / cfg for
                         * interrupt router, for this example
                         */

#include <hw_include/soc.h>
/* Interrupts, Clear MCU Timer 0 */
#include <hw_include/arch/lld_arch.h>
#include <hw_include/hw_types.h>
#include <hw_include/lldr_timer.h>

/* SCI Client */
#include <ti/drv/sciclient/sciclient.h>
/* PM Lib */
#include <ti/drv/pm/include/pm_types.h>
#include <ti/drv/pm/include/dmsc/pmlib_sysconfig.h>
#include <ti/drv/pm/include/dmsc/pmlib_clkrate.h>

/* Interrupt Registrations */
#include <ti/osal/osal.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */
static void GptAppStartup_BuildIntList(void);
static void GptAppStartup_PowerAndClkSrc(void);
static void GptAppStartup_InterruptConfig(void);
static void GptAppStartup_ClearAnyOperationalTimers(void);
static void GptAppStartup_SetClockSource4Timers(void);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
uint32 GptAppStartup_IntNumbers[APP_GPT_TOTAL_TIMER_COUNT];
/**< Stored the interrupt numbers for enabled all timers */
uint32 GptAppStartup_RoutIntNumbers[APP_GPT_TOTAL_TIMER_COUNT];
/**< Flag to indicate if the timer is enabled or not */
GptApp_IsrType GptAppStartup_Isr[APP_GPT_TOTAL_TIMER_COUNT];
/**< Associated ISR */
uint32 GptAppStartup_EnaTimer[APP_GPT_TOTAL_TIMER_COUNT];
/**< Flag to indicate if the timer is enabled or not */
uint32 GptAppStartup_TimerSrcId[APP_GPT_TOTAL_TIMER_COUNT];
/**< Store Module Device Id's used by SciClient for Interrupt Registration */
/** \brief GPT PC Configuration struct declaration */
extern const struct Gpt_ChannelConfigType_PC_s Gpt_ChannelConfig_PC[GPT_MAX_CHANNELS];
/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */


/* ========================================================================== */
/*                        Application Local Functions                         */
/* ========================================================================== */


/** \brief Start up sequence : Program the interrupt muxes / priorities */
void GptApp_Startup(void)
{
    /* Equivalent to EcuM_AL_SetProgrammableInterrupts */

    GptAppStartup_PowerAndClkSrc();

    GptAppStartup_BuildIntList();
    Intc_Init();    /* Interrupt handler initialized, here as other functions
                     * can use API's to clear pending interrupts if any
                     */
    GptAppStartup_ClearAnyOperationalTimers(); /* Clear MCU TIMER 0 only */
    GptAppStartup_SetClockSource4Timers();
    GptAppStartup_InterruptConfig();

}

/** \brief Determines the used timers and builds a list of valid timer & ISR */
static void GptAppStartup_BuildIntList(void)
{
    uint32 idx, flag, intNum, routIntNum, timerDevId;
    const Gpt_ChannelConfigType_PC *pChCfgPtr = NULL;
    GptApp_IsrType pIsrHandler = NULL;

    /*
     * 1. Determine the number of timers used
     * 2. Build valid timer list
     * 3. Build isr and interrupt number for enabled timers only
     */

     AppUtils_Printf(MSG_NORMAL, APP_NAME ": Building Interrupt List !!!\n");
#if (STD_ON == GPT_PRE_COMPILE_VARIANT)
    const Gpt_ConfigType  *pCfgPtr = &GptChannelConfigSet_PC;
    AppUtils_Printf(MSG_NORMAL, APP_NAME ": Variant - Pre Compile being used !!!\n");
#else
    const Gpt_ConfigType  *pCfgPtr = &GptChannelConfigSet;
    AppUtils_Printf(MSG_NORMAL, APP_NAME ": Variant - Post Build being used !!!\n");
#endif

    for (idx = 0U; idx < APP_GPT_TOTAL_TIMER_COUNT; idx++)
    {
        GptAppStartup_EnaTimer[idx] = 0U;
        GptAppStartup_IntNumbers[idx] = 0U;
        GptAppStartup_TimerSrcId[idx] = 0U;
    }

    for (idx = 0U; idx < pCfgPtr->channelCount; idx++)
    {
        pChCfgPtr = &Gpt_ChannelConfig_PC[idx];
        flag = 0U;
        routIntNum = 0U;
        timerDevId = 0U;
        switch(pChCfgPtr->channelId)
        {
#ifdef GptConf_GptChannelConfiguration_MCU_TIMER0
            case GPT_CH_GPTIMER1:
                intNum = APP_GPT_MCU_0_INT;
                pIsrHandler = Gpt_Ch1Isr;
                flag = 1U;
            break;
#endif
#ifdef GptConf_GptChannelConfiguration_MCU_TIMER1
            case GPT_CH_GPTIMER2:
                intNum = APP_GPT_MCU_1_INT;
                pIsrHandler = Gpt_Ch2Isr;
                flag = 1U;
            break;
#endif
#ifdef GptConf_GptChannelConfiguration_MCU_TIMER2
            case GPT_CH_GPTIMER3:
                intNum = APP_GPT_MCU_2_INT;
                pIsrHandler = Gpt_Ch3Isr;
                flag = 1U;
            break;
#endif
#ifdef GptConf_GptChannelConfiguration_MCU_TIMER3
            case GPT_CH_GPTIMER4:
                intNum = APP_GPT_MCU_3_INT;
                pIsrHandler = Gpt_Ch4Isr;
                flag = 1U;
            break;
#endif
#ifdef GptConf_GptChannelConfiguration_MCU_TIMER4
            case GPT_CH_GPTIMER5:
                intNum = APP_GPT_MCU_4_INT;
                pIsrHandler = Gpt_Ch5Isr;
                flag = 1U;
            break;
#endif
#ifdef GptConf_GptChannelConfiguration_MCU_TIMER5
            case GPT_CH_GPTIMER6:
                intNum = APP_GPT_MCU_5_INT;
                pIsrHandler = Gpt_Ch6Isr;
                flag = 1U;
            break;
#endif
#ifdef GptConf_GptChannelConfiguration_MCU_TIMER6
            case GPT_CH_GPTIMER7:
                intNum = APP_GPT_MCU_6_INT;
                pIsrHandler = Gpt_Ch7Isr;
                flag = 1U;
            break;
#endif
#ifdef GptConf_GptChannelConfiguration_MCU_TIMER7
            case GPT_CH_GPTIMER8:
                intNum = APP_GPT_MCU_7_INT;
                pIsrHandler = Gpt_Ch8Isr;
                flag = 1U;
            break;
#endif
#ifdef GptConf_GptChannelConfiguration_MCU_TIMER8
            case GPT_CH_GPTIMER9:
                intNum = APP_GPT_MCU_8_INT;
                pIsrHandler = Gpt_Ch9Isr;
                flag = 1U;
            break;
#endif
#ifdef GptConf_GptChannelConfiguration_MCU_TIMER9
            case GPT_CH_GPTIMER10:
                intNum = APP_GPT_MCU_9_INT;
                pIsrHandler = Gpt_Ch10Isr;
                flag = 1U;
            break;
#endif
#ifdef GptConf_GptChannelConfiguration_TIMER0
            case GPT_CH_GPTIMER11:
                intNum = APP_GPT_0_INT;
                routIntNum = INTRTR_CFG_MAIN_DOMAIN_TIMER_1_OUT_INT_NO +
                                INTRTR_CFG_START_LEVEL_INT_NUMBER;
                pIsrHandler = Gpt_Ch11Isr;
                timerDevId = TISCI_DEV_TIMER0;
                flag = 1U;
            break;
#endif
#ifdef GptConf_GptChannelConfiguration_TIMER1
            case GPT_CH_GPTIMER12:
                intNum = APP_GPT_1_INT;
                routIntNum = INTRTR_CFG_MAIN_DOMAIN_TIMER_2_OUT_INT_NO +
                                INTRTR_CFG_START_LEVEL_INT_NUMBER;
                pIsrHandler = Gpt_Ch12Isr;
                timerDevId = TISCI_DEV_TIMER1;
                flag = 1U;
            break;
#endif
#ifdef GptConf_GptChannelConfiguration_TIMER2
            case GPT_CH_GPTIMER13:
                intNum = APP_GPT_2_INT;
                routIntNum = INTRTR_CFG_MAIN_DOMAIN_TIMER_3_OUT_INT_NO +
                                INTRTR_CFG_START_LEVEL_INT_NUMBER;
                pIsrHandler = Gpt_Ch13Isr;
                timerDevId = TISCI_DEV_TIMER2;
                flag = 1U;
            break;
#endif
#ifdef GptConf_GptChannelConfiguration_TIMER3
            case GPT_CH_GPTIMER14:
                intNum = APP_GPT_3_INT;
                routIntNum = INTRTR_CFG_MAIN_DOMAIN_TIMER_4_OUT_INT_NO +
                                INTRTR_CFG_START_LEVEL_INT_NUMBER;
                pIsrHandler = Gpt_Ch14Isr;
                timerDevId = TISCI_DEV_TIMER3;
                flag = 1U;
            break;
#endif
#ifdef GptConf_GptChannelConfiguration_TIMER4
            case GPT_CH_GPTIMER15:
                intNum = APP_GPT_4_INT;
                routIntNum = INTRTR_CFG_MAIN_DOMAIN_TIMER_5_OUT_INT_NO +
                                INTRTR_CFG_START_LEVEL_INT_NUMBER;
                pIsrHandler = Gpt_Ch15Isr;
                timerDevId = TISCI_DEV_TIMER4;
                flag = 1U;
            break;
#endif
#ifdef GptConf_GptChannelConfiguration_TIMER5
            case GPT_CH_GPTIMER16:
                intNum = APP_GPT_5_INT;
                routIntNum = INTRTR_CFG_MAIN_DOMAIN_TIMER_6_OUT_INT_NO +
                                INTRTR_CFG_START_LEVEL_INT_NUMBER;
                pIsrHandler = Gpt_Ch16Isr;
                timerDevId = TISCI_DEV_TIMER5;
                flag = 1U;
            break;
#endif
#ifdef GptConf_GptChannelConfiguration_TIMER6
            case GPT_CH_GPTIMER17:
                intNum = APP_GPT_6_INT;
                routIntNum = INTRTR_CFG_MAIN_DOMAIN_TIMER_7_OUT_INT_NO +
                                INTRTR_CFG_START_LEVEL_INT_NUMBER;
                pIsrHandler = Gpt_Ch17Isr;
                timerDevId = TISCI_DEV_TIMER6;
                flag = 1U;
            break;
#endif
#ifdef GptConf_GptChannelConfiguration_TIMER7
            case GPT_CH_GPTIMER18:
                intNum = APP_GPT_7_INT;
                routIntNum = INTRTR_CFG_MAIN_DOMAIN_TIMER_8_OUT_INT_NO +
                                INTRTR_CFG_START_LEVEL_INT_NUMBER;
                pIsrHandler = Gpt_Ch18Isr;
                timerDevId = TISCI_DEV_TIMER7;
                flag = 1U;
            break;
#endif
#ifdef GptConf_GptChannelConfiguration_TIMER8
            case GPT_CH_GPTIMER19:
                intNum = APP_GPT_8_INT;
                routIntNum = INTRTR_CFG_MAIN_DOMAIN_TIMER_9_OUT_INT_NO +
                                INTRTR_CFG_START_LEVEL_INT_NUMBER;
                pIsrHandler = Gpt_Ch19Isr;
                timerDevId = TISCI_DEV_TIMER8;
                flag = 1U;
            break;
#endif
#ifdef GptConf_GptChannelConfiguration_TIMER9
            case GPT_CH_GPTIMER20:
                intNum = APP_GPT_9_INT;
                routIntNum = INTRTR_CFG_MAIN_DOMAIN_TIMER_10_OUT_INT_NO +
                                INTRTR_CFG_START_LEVEL_INT_NUMBER;
                pIsrHandler = Gpt_Ch20Isr;
                timerDevId = TISCI_DEV_TIMER9;
                flag = 1U;
            break;
#endif
#ifdef GptConf_GptChannelConfiguration_TIMER10
            case GPT_CH_GPTIMER21:
                intNum = APP_GPT_10_INT;
                routIntNum = INTRTR_CFG_MAIN_DOMAIN_TIMER_11_OUT_INT_NO +
                                INTRTR_CFG_START_LEVEL_INT_NUMBER;
                pIsrHandler = Gpt_Ch21Isr;
                timerDevId = TISCI_DEV_TIMER10;
                flag = 1U;
            break;
#endif
#ifdef GptConf_GptChannelConfiguration_TIMER11
            case GPT_CH_GPTIMER22:
                intNum = APP_GPT_11_INT;
                routIntNum = INTRTR_CFG_MAIN_DOMAIN_TIMER_12_OUT_INT_NO +
                                INTRTR_CFG_START_LEVEL_INT_NUMBER;
                pIsrHandler = Gpt_Ch22Isr;
                timerDevId = TISCI_DEV_TIMER11;
                flag = 1U;
            break;
#endif
#ifdef GptConf_GptChannelConfiguration_TIMER12
            case GPT_CH_GPTIMER23:
                intNum = APP_GPT_12_INT;
                routIntNum = INTRTR_CFG_MAIN_DOMAIN_TIMER_13_OUT_INT_NO +
                                INTRTR_CFG_START_LEVEL_INT_NUMBER;
                pIsrHandler = Gpt_Ch23Isr;
                timerDevId = TISCI_DEV_TIMER12;
                flag = 1U;
            break;
#endif
#ifdef GptConf_GptChannelConfiguration_TIMER13
            case GPT_CH_GPTIMER24:
                intNum = APP_GPT_13_INT;
                routIntNum = INTRTR_CFG_MAIN_DOMAIN_TIMER_14_OUT_INT_NO +
                                INTRTR_CFG_START_LEVEL_INT_NUMBER;
                pIsrHandler = Gpt_Ch24Isr;
                timerDevId = TISCI_DEV_TIMER13;
                flag = 1U;
            break;
#endif
#ifdef GptConf_GptChannelConfiguration_TIMER14
            case GPT_CH_GPTIMER25:
                intNum = APP_GPT_14_INT;
                routIntNum = INTRTR_CFG_MAIN_DOMAIN_TIMER_15_OUT_INT_NO +
                                INTRTR_CFG_START_LEVEL_INT_NUMBER;
                pIsrHandler = Gpt_Ch25Isr;
                timerDevId = TISCI_DEV_TIMER14;
                flag = 1U;
            break;
#endif
#ifdef GptConf_GptChannelConfiguration_TIMER15
            case GPT_CH_GPTIMER26:
                intNum = APP_GPT_15_INT;
                routIntNum = INTRTR_CFG_MAIN_DOMAIN_TIMER_16_OUT_INT_NO +
                                INTRTR_CFG_START_LEVEL_INT_NUMBER;
                pIsrHandler = Gpt_Ch26Isr;
                timerDevId = TISCI_DEV_TIMER15;
                flag = 1U;
            break;
#endif
#ifdef GptConf_GptChannelConfiguration_TIMER16
            case GPT_CH_GPTIMER27:
                intNum = APP_GPT_16_INT;
                routIntNum = INTRTR_CFG_MAIN_DOMAIN_TIMER_17_OUT_INT_NO +
                                INTRTR_CFG_START_LEVEL_INT_NUMBER;
                pIsrHandler = Gpt_Ch27Isr;
                timerDevId = TISCI_DEV_TIMER16;
                flag = 1U;
            break;
#endif
#ifdef GptConf_GptChannelConfiguration_TIMER17
            case GPT_CH_GPTIMER28:
                intNum = APP_GPT_17_INT;
                routIntNum = INTRTR_CFG_MAIN_DOMAIN_TIMER_18_OUT_INT_NO +
                                INTRTR_CFG_START_LEVEL_INT_NUMBER;
                pIsrHandler = Gpt_Ch28Isr;
                timerDevId = TISCI_DEV_TIMER17;
                flag = 1U;
            break;
#endif
#ifdef GptConf_GptChannelConfiguration_TIMER18
            case GPT_CH_GPTIMER29:
                intNum = APP_GPT_18_INT;
                routIntNum = INTRTR_CFG_MAIN_DOMAIN_TIMER_19_OUT_INT_NO +
                                INTRTR_CFG_START_LEVEL_INT_NUMBER;
                pIsrHandler = Gpt_Ch29Isr;
                timerDevId = TISCI_DEV_TIMER18;
                flag = 1U;
            break;
#endif
#ifdef GptConf_GptChannelConfiguration_TIMER19
            case GPT_CH_GPTIMER30:
                intNum = APP_GPT_19_INT;
                routIntNum = INTRTR_CFG_MAIN_DOMAIN_TIMER_20_OUT_INT_NO +
                                INTRTR_CFG_START_LEVEL_INT_NUMBER;
                pIsrHandler = Gpt_Ch30Isr;
                timerDevId = TISCI_DEV_TIMER19;
                flag = 1U;
            break;
#endif
            default:
                AppUtils_Printf(MSG_NORMAL, APP_NAME ": ERROR Interrupt registration is not done for timer ID %d !!!\n", pChCfgPtr->channelId);
                flag = 0U;
            break;
        }
        if (0U != flag)
        {
            GptAppStartup_EnaTimer[pChCfgPtr->channelId] = 1U;
            GptAppStartup_IntNumbers[pChCfgPtr->channelId] = intNum;
            GptAppStartup_RoutIntNumbers[pChCfgPtr->channelId] = routIntNum;
            GptAppStartup_TimerSrcId[pChCfgPtr->channelId] = timerDevId;
            GptAppStartup_Isr[pChCfgPtr->channelId] = pIsrHandler;
            flag = 0U;
        }
    }

    AppUtils_Printf(MSG_NORMAL, APP_NAME ": Interrupt List Completed !!!\n");

    return;
}

/** \brief None, SBL/GEL powers up the timers and clock sources */
static void GptAppStartup_PowerAndClkSrc(void)
{
    /* Mcu module, when included will replace this operation */
#if NOT_YET
    volatile uint32 idx, count;
    static pmlibSysConfigPowerStateParams_t
                                        enabledTimer[APP_GPT_TOTAL_TIMER_COUNT];
    static uint32 timerId[APP_GPT_TOTAL_TIMER_COUNT];
    static uint32 timerClkSrc[APP_GPT_TOTAL_TIMER_COUNT];

    pmlibSysConfigErrReturn_t statusRtn;
    /*
     *  1. Enable SCI Client
     *  2. For each enabled timer
     *  2.1 Enable the module
     *  2.2 Set the clock rate and clock source
     */

    Sciclient_init(NULL_PTR);

    count = 0U;
    for (idx = 0U; idx < APP_GPT_TOTAL_TIMER_COUNT; idx++)
    {
        if (0U != GptAppStartup_EnaTimer[idx])
        {
            /* MCU Domain / R5F Timers */
            if (((0U == idx) || (1U == idx)) ||
                ((2U == idx) || (3U == idx)))
            {
                enabledTimer[count].pwrState = PMLIB_SYS_CONFIG_ALWAYS_ENABLED;
                if (0U == idx)
                {
                    enabledTimer[count].modId = TISCI_DEV_MCU_TIMER0;

                    timerId[count] = TISCI_DEV_MCU_TIMER0;
                    timerClkSrc[count] = TISCI_DEV_MCU_TIMER0_BUS_TIMER_TCLK_CLK_PARENT_MX_WAKEUP_GS80_WKUP_0_BUS_WKUP_RCOSC_12P5M_CLK;
                }
                else if (1U == idx)
                {
                    enabledTimer[count].modId = TISCI_DEV_MCU_TIMER1;

                    timerId[count] = TISCI_DEV_MCU_TIMER1;
                    timerClkSrc[count] = TISCI_DEV_MCU_TIMER1_BUS_TIMER_TCLK_CLK_PARENT_MX_WAKEUP_GS80_WKUP_0_BUS_WKUP_RCOSC_12P5M_CLK;
                }
                else if (2U == idx)
                {
                    enabledTimer[count].modId = TISCI_DEV_MCU_TIMER2;

                    timerId[count] = TISCI_DEV_MCU_TIMER2;
                    timerClkSrc[count] = TISCI_DEV_MCU_TIMER2_BUS_TIMER_TCLK_CLK_PARENT_MX_WAKEUP_GS80_WKUP_0_BUS_WKUP_RCOSC_12P5M_CLK;
                }
                else
                {
                    enabledTimer[count].modId = TISCI_DEV_MCU_TIMER3;

                    timerId[count] = TISCI_DEV_MCU_TIMER3;
                    timerClkSrc[count] = TISCI_DEV_MCU_TIMER3_BUS_TIMER_TCLK_CLK_PARENT_MX_WAKEUP_GS80_WKUP_0_BUS_WKUP_RCOSC_12P5M_CLK;
                }
            }
            else if (((4U == idx) || (5U == idx)) ||
                     ((6U == idx) || (7U == idx)))
            {
                enabledTimer[count].pwrState = PMLIB_SYS_CONFIG_ALWAYS_ENABLED;
                if (4U == idx)
                {
                    enabledTimer[count].modId = TISCI_DEV_TIMER0;

                    timerId[count] = TISCI_DEV_TIMER0;
                    timerClkSrc[count] = TISCI_DEV_TIMER0_BUS_TIMER_TCLK_CLK_PARENT_MX_WAKEUP_GS80_WKUP_0_BUS_WKUP_RCOSC_12P5M_CLK;
                }
                else if (5U == idx)
                {
                    enabledTimer[count].modId = TISCI_DEV_TIMER1;

                    timerId[count] = TISCI_DEV_TIMER1;
                    timerClkSrc[count] = TISCI_DEV_TIMER1_BUS_TIMER_TCLK_CLK_PARENT_MX_WAKEUP_GS80_WKUP_0_BUS_WKUP_RCOSC_12P5M_CLK;
                }
                else if (6U == idx)
                {
                    enabledTimer[count].modId = TISCI_DEV_TIMER2;

                    timerId[count] = TISCI_DEV_TIMER2;
                    timerClkSrc[count] = TISCI_DEV_TIMER2_BUS_TIMER_TCLK_CLK_PARENT_MX_WAKEUP_GS80_WKUP_0_BUS_WKUP_RCOSC_12P5M_CLK;
                }
                else
                {
                    enabledTimer[count].modId = TISCI_DEV_TIMER3;

                    timerId[count] = TISCI_DEV_TIMER3;
                    timerClkSrc[count] = TISCI_DEV_TIMER3_BUS_TIMER_TCLK_CLK_PARENT_MX_WAKEUP_GS80_WKUP_0_BUS_WKUP_RCOSC_12P5M_CLK;
                }
            }
            else if (((8U == idx) || (9U == idx)) ||
                     ((10U == idx) || (11U == idx)))
            {
                enabledTimer[count].pwrState = PMLIB_SYS_CONFIG_ALWAYS_ENABLED;
                if (8U == idx)
                {
                    enabledTimer[count].modId = TISCI_DEV_TIMER4;

                    timerId[count] = TISCI_DEV_TIMER4;
                    timerClkSrc[count] = TISCI_DEV_TIMER4_BUS_TIMER_TCLK_CLK_PARENT_MX_WAKEUP_GS80_WKUP_0_BUS_WKUP_RCOSC_12P5M_CLK;
                }
                else if (9U == idx)
                {
                }
                else if (10U == idx)
                {
                }
                else
                {
                }
            }
            else
            {
                enabledTimer[count].pwrState = PMLIB_SYS_CONFIG_ALWAYS_ENABLED;
                if (9U == idx)
                {
                }
                else if (10U == idx)
                {
                }
                else if (11U == idx)
                {
                }
                else
                {
                }
            }
            count++;
        }
    }

    if (count)
    {
        if (PM_SUCCESS == PMLIBSysConfigSetPowerState(&enabledTimer[0U],
                                                        count, 0x5000,
                                                        &statusRtn))
        {
            AppUtils_Printf(MSG_NORMAL,
                APP_NAME ": \nPowered up configured timers !!!\n");
        }
        else
        {
            /* Error */
            AppUtils_Printf(MSG_NORMAL, APP_NAME
                            "Error Could not power up timers !!! \n");
        }
    }

    for (idx = 0U; idx < count; idx++)
    {
        if (PM_SUCCESS == PMLIBClkRateSet(timerId[idx], timerClkSrc[idx],
                                         (uint64_t)(12.5 * (1000U * 1000U))))
        {
            AppUtils_Printf(MSG_NORMAL,
                APP_NAME ": Set the clock source to 12.5Mhz !!!\n");
        }
        else
        {
            AppUtils_Printf(MSG_NORMAL,
                APP_NAME ": Could not set the clock source !!!\n");
        }
    }
#endif
    return;
}

/** \brief Clear MCU TIMER 0 Timer */
static void GptAppStartup_ClearAnyOperationalTimers(void)
{
    /* MCU Timer 0 is turned on by RBL and interrupts are enabled. We would
     *  require to disable the interrupt and clear MCU timer, to ensure no
     *  un-expected interrupts are received
     *  Steps
     *  1. Disable the interrupt in MCU TIMER 0
     *  2. Stop the timer
     *  3. Enable Soft Reset
     *  4. Reset the timer
     *  5. Clear the interrupt in VIM of R5F
     *  6. Reset the clock source
     */
    /* Steps 1 to 2 */
    HW_WR_REG32_RAW((CSL_MCU_TIMER0_CFG_BASE + TIMER_IRQENABLE_CLR),
                    (TIMER_IRQENABLE_CLR_TCAR_EN_FLAG_MASK  |
                        TIMER_IRQENABLE_CLR_OVF_EN_FLAG_MASK   |
                        TIMER_IRQENABLE_CLR_MAT_EN_FLAG_MASK));
    HW_WR_REG32_RAW((CSL_MCU_TIMER0_CFG_BASE + TIMER_TCLR), 0x0U);
    /* Steps 3 to 4 */
    HW_WR_REG32_RAW((CSL_MCU_TIMER0_CFG_BASE + TIMER_TSICR),
                    TIMER_TSICR_SFT_MASK);
    HW_WR_REG32_RAW((CSL_MCU_TIMER0_CFG_BASE + TIMER_TIOCP_CFG),
                    TIMER_TIOCP_CFG_SOFTRESET_MASK);

    /* Step 5 */
    Intc_IntClrPend(APP_GPT_MCU_0_INT);

    /* Step 6. Reset the clock source also */
    /* Un Lock */
    HW_WR_REG32_RAW(
        (CSL_MCU_CTRL_MMR0_CFG0_BASE + CSL_MCU_CTRL_MMR_CFG0_LOCK0_KICK0),
        0x68EF3490);
    HW_WR_REG32_RAW(
        (CSL_MCU_CTRL_MMR0_CFG0_BASE + CSL_MCU_CTRL_MMR_CFG0_LOCK0_KICK1),
        0xD172BC5A);

    /* Reset Clock source */
    HW_WR_REG32_RAW((CSL_MCU_CTRL_MMR0_CFG0_BASE +
                        CSL_MCU_CTRL_MMR_CFG0_MCU_TIMER0_CLKSEL),
                    0U /* Reset Value */);
}

/** \brief Interrupt registrations. Clears MCU Timer 0 also */
static void GptAppStartup_InterruptConfig(void)
{
    uint32 idx;
    OsalRegisterIntrParams_t    intrPrms;
    OsalInterruptRetCode_e      osalRetVal;
    HwiP_Handle hwiHandle;
    struct tisci_msg_rm_irq_set_req     rmIrqReq;
    struct tisci_msg_rm_irq_set_resp    rmIrqResp;
    Int32 retVal;

    for (idx = 0U; idx < APP_GPT_TOTAL_TIMER_COUNT; idx++)
    {
        if (0U != GptAppStartup_EnaTimer[idx])
        {
            if (0U != GptAppStartup_RoutIntNumbers[idx])
            {
                /* We will have to use the routed interrupt number
                    subsequently */
                GptAppStartup_IntNumbers[idx] = GptAppStartup_RoutIntNumbers[idx];

                /* If the timer is not in MCU domain, the interrupt router will
                    have to be configured */
                /* Set the destination interrupt */
                rmIrqReq.valid_params           = TISCI_MSG_VALUE_RM_DST_ID_VALID;
                rmIrqReq.valid_params          |= TISCI_MSG_VALUE_RM_DST_HOST_IRQ_VALID;
                rmIrqReq.src_id                 = GptAppStartup_TimerSrcId[idx];
                rmIrqReq.global_event           = 0U;
                rmIrqReq.src_index              = 0U;
                rmIrqReq.dst_id                 = TISCI_DEV_MCU_R5FSS0_CORE0;
                rmIrqReq.dst_host_irq           = GptAppStartup_RoutIntNumbers[idx];
                rmIrqReq.ia_id                  = 0U;
                rmIrqReq.vint                   = 0U;
                rmIrqReq.vint_status_bit_index  = 0U;
                rmIrqReq.secondary_host         = TISCI_MSG_VALUE_RM_UNUSED_SECONDARY_HOST;
                retVal = Sciclient_rmIrqSet(
                             &rmIrqReq, &rmIrqResp, APP_SCICLIENT_TIMEOUT);
                if(LLD_PASS != retVal)
                {
                    AppUtils_Printf(MSG_NORMAL,APP_NAME
                    ": Error in SciClient Interrupt Params Configuration!!!\n");
                }
            }
            Osal_RegisterInterrupt_initParams(&intrPrms);
            intrPrms.corepacConfig.arg          = (uintptr_t)GptAppStartup_Isr[idx];
            intrPrms.corepacConfig.isrRoutine   = &GptApp_GptXIsr;
            intrPrms.corepacConfig.priority     = 1U;
            intrPrms.corepacConfig.corepacEventNum = 0U; /* NOT USED ? */
            intrPrms.corepacConfig.intVecNum    = GptAppStartup_IntNumbers[idx];

            osalRetVal = Osal_RegisterInterrupt(&intrPrms, &hwiHandle);
            if(OSAL_INT_SUCCESS != osalRetVal)
            {
                AppUtils_Printf(MSG_NORMAL,APP_NAME
                    ": Error in OSAL Interrupt Registration !!!\n");
                break;
            }
        }
    }

    return;
}


static void GptAppStartup_SetClockSource4Timers(void)
{
#ifdef NOT_YET
/* The MMR has been fire walled, use SciClient to configure the clock source.
 *  Use of Sciclient is not demonstrated in MCAL examples at this point
 */

    uint32 idx;

    /* Un Lock MMR for MCU control MMR */
    HW_WR_REG32_RAW(
        (CSL_MCU_CTRL_MMR0_CFG0_BASE + CSL_MCU_CTRL_MMR_CFG0_LOCK0_KICK0),
        0x68EF3490);
    HW_WR_REG32_RAW(
        (CSL_MCU_CTRL_MMR0_CFG0_BASE + CSL_MCU_CTRL_MMR_CFG0_LOCK0_KICK1),
        0xD172BC5A);

    for (idx = 0U; idx < APP_GPT_TOTAL_TIMER_COUNT; idx++)
    {
        if (0U != GptAppStartup_EnaTimer[idx])
        {
            if (idx < GPT_CH_GPTIMER5)
            {
                /* MCU Timers, 12.5 MHz clock to be sourced */
                HW_WR_REG32_RAW((CSL_MCU_CTRL_MMR0_CFG0_BASE +
                                CSL_MCU_CTRL_MMR_CFG0_MCU_TIMER0_CLKSEL +
                                (idx * 0x04)),
                                2U);
            }
        }
    }

#endif

    return;
}

/*EoF*/
