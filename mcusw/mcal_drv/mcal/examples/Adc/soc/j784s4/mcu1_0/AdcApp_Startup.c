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
 *  \file     AdcApp_Startup.c
 *
 *  \brief    This file contains the pinmux, interrupt and clock configuration.
 */
/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "AdcApp_Startup.h"
#include "Adc.h"

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
/*None*/
/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */
static void AdcApp_BuildIntList(void);
static void AdcApp_InterruptConfig(void);
/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

uint32                            AdcApp_IntNumbers[ADC_HW_UNIT_CNT];
/**< Stored the interrupt numbers for enabled all timers */
AdcApp_IsrType                    AdcApp_Isr[ADC_HW_UNIT_CNT];
/**< Associated ISR */
const Adc_ConfigType *Adc_ConfigPtr;
/**< Pointer to configuration  */
/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

void AdcApp_Startup(void)
{
    /* Equivalent to EcuM_AL_SetProgrammableInterrupts */
    AdcApp_BuildIntList();
    Intc_Init();    /* Interrupt handler initialized, here as other functions
                     * can use API's to clear pending interrupts if any
                     */
    AdcApp_InterruptConfig();

    /* Initialize counters, that would be required for timed operations */
    AppUtils_ProfileInit(0);

    /* Initialize memory sections  */
	AppUtils_AdcSectionInit();
}

void AdcApp_PlatformInit(void)
{
    return;
}

void AdcApp_PowerAndClkSrc(void)
{
    uint32 regVal;
    /* Unlock lock key registers for Partition 0: IO PAD
       configuration registers in MCU_CTRL_MMR */
    /* write Partition 0 Lock Key 0 Register */
    CSL_REG32_WR(CSL_MCU_CTRL_MMR0_CFG0_BASE +
        CSL_MCU_CTRL_MMR_CFG0_LOCK2_KICK0, 0x68EF3490U);
    /* write Partition 1 Lock Key 1 Register */
    CSL_REG32_WR(CSL_MCU_CTRL_MMR0_CFG0_BASE +
        CSL_MCU_CTRL_MMR_CFG0_LOCK2_KICK1, 0xD172BC5AU);
    /* Check for unlock */
    regVal = CSL_REG32_RD(CSL_MCU_CTRL_MMR0_CFG0_BASE +
        CSL_MCU_CTRL_MMR_CFG0_LOCK2_KICK0);
    while ((regVal & 0x1U) != 0x1U)
    {
		/* Check for unlock */
		regVal = CSL_REG32_RD(CSL_MCU_CTRL_MMR0_CFG0_BASE +
			CSL_MCU_CTRL_MMR_CFG0_LOCK2_KICK0);
    }

    /* Select clock frequency 25 MHZ */
    CSL_REG32_FINS(CSL_MCU_CTRL_MMR0_CFG0_BASE +
                  CSL_MCU_CTRL_MMR_CFG0_MCU_ADC0_CLKSEL,
                  MCU_CTRL_MMR_CFG0_MCU_ADC0_CLKSEL_PROXY_MCU_ADC0_CLKSEL_CLK_SEL_PROXY,
                  0U);
    return;
}

static void AdcApp_BuildIntList(void)
{
    uint32 idx, flag, intNum;
    AdcApp_IsrType pIsrHandler = NULL;
    /*
     * 1. Determine the number of timers used
     * 2. Build valid timer list
     * 3. Build isr and interrupt number for enabled timers only
     */
    /* Do Adc Init */
#if (STD_ON == ADC_VARIANT_PRE_COMPILE)
    Adc_ConfigPtr = &ADC_INIT_CONFIG_PC;
    Adc_Init((const Adc_ConfigType *) NULL_PTR);
    AppUtils_Printf(MSG_NORMAL,
    "ADC_APP: Variant - Pre Compile being used !!!\n");
#else
    Adc_ConfigPtr = &AdcConfigSet;
    Adc_Init(Adc_ConfigPtr);
    AppUtils_Printf(MSG_NORMAL,
    "ADC_APP: Variant - Post Build being used !!!\n");
#endif

    for (idx = 0U; idx < Adc_ConfigPtr->maxHwUnit; idx++)
    {
        AdcApp_IntNumbers[idx] = 0U;
    }

    for (idx = 0U; idx < Adc_ConfigPtr->maxHwUnit; idx++)
    {
        flag = 0U;
        if (ADC_UNIT_0 == Adc_ConfigPtr->hwUnitCfg[idx].hwUnitId)
        {
            intNum = APP_ADC_0_INT0;
            pIsrHandler = Adc_IrqUnit0;
            flag = 1U;
        }
        if (ADC_UNIT_1 == Adc_ConfigPtr->hwUnitCfg[idx].hwUnitId)
        {
        intNum = APP_ADC_1_INT0;
        pIsrHandler = Adc_IrqUnit1;
        flag = 1U;
        }
        if (0U != flag)
        {
            AdcApp_IntNumbers[idx] = intNum;
            AdcApp_Isr[idx] = pIsrHandler;
            flag = 0U;
        }
    }

    return;
}

static void AdcApp_InterruptConfig(void)
{
    uint32 idx;
    OsalRegisterIntrParams_t    intrPrms;
    OsalInterruptRetCode_e      osalRetVal;
    HwiP_Handle hwiHandle;

    for (idx = 0U; idx < ADC_HW_UNIT_CNT; idx++)
    {
        Osal_RegisterInterrupt_initParams(&intrPrms);
        intrPrms.corepacConfig.arg          = (uintptr_t)AdcApp_Isr[idx];
        intrPrms.corepacConfig.isrRoutine   = &AdcApp_AdcXIsr;
        intrPrms.corepacConfig.priority     = 1U;
        intrPrms.corepacConfig.corepacEventNum = 0U; /* NOT USED ? */
        intrPrms.corepacConfig.intVecNum        = AdcApp_IntNumbers[idx];

        osalRetVal = Osal_RegisterInterrupt(&intrPrms, &hwiHandle);
        if(OSAL_INT_SUCCESS != osalRetVal)
        {
            AppUtils_Printf(MSG_NORMAL,
                            "ADC_APP: Error %d !!!\n");
            break;
        }
    }

    return;
}

#define ADC_START_SEC_ISR_CODE
#include "Adc_MemMap.h"

FUNC(void, ADC_CODE_FAST) AdcApp_AdcXIsr(uintptr_t AdcPtr)
{
    AdcApp_IsrType adcChIsr = (AdcApp_IsrType)AdcPtr;

    /* Associated ADC ISR */
    adcChIsr();
}

#define ADC_STOP_SEC_ISR_CODE
#include "Adc_MemMap.h"
