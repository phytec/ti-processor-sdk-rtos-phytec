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
#include "Os.h" 
#include "GptApp.h"
#include "app_utils.h"
#include "IntRtr_Cfg.h"
#include <hw_include/arch/lld_arch.h>
#include <hw_include/hw_types.h>
#include <hw_include/lldr_timer.h>
/* SCI Client */
#include <drivers/sciclient.h>
#include <hw_include/arch/r5/interrupt.h>


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
uint32 gRmIrqTreeCount = 0;
uint32 gRmIrqTree = 0;
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
    Intc_Init();    /*Interrupt handler initialized, here as other functions
                     * can use API's to clear pending interrupts if any
                     */
    GptAppStartup_ClearAnyOperationalTimers(); /* Clear MCU TIMER 0 only */
    GptAppStartup_SetClockSource4Timers();
    GptAppStartup_InterruptConfig();

}

/** \brief Determines the used timers and builds a list of valid timer & ISR */
static void GptAppStartup_BuildIntList(void)
{
    uint32 idx, flag, intNum;
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
        switch(pChCfgPtr->channelId)
        {

#ifdef GptConf_GptChannelConfiguration_WK_TIMER1
            case GPT_CH_GPTIMER1:
                intNum = APP_GPT_WK_0_INT;				
                pIsrHandler = Gpt_Ch1Isr;
                flag = 1U;
            break;
#endif
#ifdef GptConf_GptChannelConfiguration_WK_TIMER2
            case GPT_CH_GPTIMER2:
                intNum = APP_GPT_WK_1_INT;
                pIsrHandler = Gpt_Ch2Isr;
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

    /*
     *  1. Enable SCI Client
     *  2. For each enabled timer
     *  2.1 Enable the module
     *  2.2 Set the clock rate and clock source
     */

    Sciclient_init(1);

    
    return;
}

/** \brief Clear MCU TIMER 0 Timer */
static void GptAppStartup_ClearAnyOperationalTimers(void)
{
    /* WKUP Timer 0 is turned on by RBL and interrupts are enabled. We would
     *  require to disable the interrupt and clear WKUP timer, to ensure no
     *  un-expected interrupts are received
     *  Steps
     *  1. Disable the interrupt in WKUP TIMER 0
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
	Intc_IntClrPend(APP_GPT_WK_0_INT);

    /* Step 6. Reset the clock source also */
    /* Un Lock */
    HW_WR_REG32_RAW(
        (CSL_WKUP_CTRL_MMR0_CFG0_BASE + WKUP_CTRL_MMR0_LOCK2_KICK0),
        0x68EF3490);
    HW_WR_REG32_RAW(
        (CSL_WKUP_CTRL_MMR0_CFG0_BASE + WKUP_CTRL_MMR0_LOCK2_KICK0),
        0xD172BC5A);

    /* Reset Clock source */
    HW_WR_REG32_RAW((CSL_WKUP_CTRL_MMR0_CFG0_BASE +
                        CSL_WKUP_CTRL_MMR_CFG0_WKUP_TIMER0_CLKSEL),
                    0U /* Reset Value */);
}

/** \brief Interrupt registrations. Clears MCU Timer 0 also */
static void GptAppStartup_InterruptConfig(void)
{
    uint32 idx;
	int32_t status = SystemP_SUCCESS;
    HwiP_Params    hwiPrms;
    static HwiP_Object GptHwiObject;
	
    for (idx = 0U; idx < APP_GPT_TOTAL_TIMER_COUNT; idx++)
    {			
       HwiP_Params_init(&hwiPrms);
       hwiPrms.args          = GptAppStartup_Isr[idx];
       hwiPrms.priority     = 1U;
       hwiPrms.eventId = 0U; /* NOT USED ? */
       hwiPrms.callback = &GptApp_GptXIsr;
       hwiPrms.intNum = GptAppStartup_IntNumbers[idx];;
       status = HwiP_construct(&GptHwiObject, &hwiPrms);
       if(SystemP_SUCCESS != status)
		{
			AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
				"Error Could not register ISR !!!\n");
		}
    }
    return;
	

}


static void GptAppStartup_SetClockSource4Timers(void)
{
/* The MMR has been fire walled, use SciClient to configure the clock source.
 *  Use of Sciclient is not demonstrated in MCAL examples at this point
 */

    /* Un Lock MMR for MCU control MMR */
    HW_WR_REG32_RAW(
        (CSL_WKUP_CTRL_MMR0_CFG0_BASE + WKUP_CTRL_MMR0_LOCK2_KICK0),
        0x68EF3490);
    HW_WR_REG32_RAW(
        (CSL_WKUP_CTRL_MMR0_CFG0_BASE + WKUP_CTRL_MMR0_LOCK2_KICK1),
        0xD172BC5A);	

    return;
}


/*EoF*/
