/*
 *  Copyright (c) 2021-2023 Texas Instruments Incorporated
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

 /**
 *  \file     sdl_rti_example.c
 *
 *  \brief    This file contains RTI DWWD Function test code.
 *
 **/


#include "main.h"

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

volatile uint32_t isrFlag = RTI_NO_INTERRUPT;
/**< Flag used to indicate interrupt is generated */

/* ========================================================================== */
/*                                Macros                                      */
/* ========================================================================== */
/* define the unlock and lock values */
#define KICK0_UNLOCK_VAL 0x68EF3490
#define KICK1_UNLOCK_VAL 0xD172BC5A
#define KICK_LOCK_VAL    0x00000000

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */
static void RTISetClockSource(uint32_t rtiModuleSelect,
                              uint32_t rtiClockSourceSelect);
static uint32_t RTIGetPreloadValue(uint32_t rtiClkSource, uint32_t timeoutVal);
int32_t RTIDwwdIsClosedWindow(uint32_t baseAddr, uint32_t *pWinStatus);

int32_t SDL_RTI_exampleTest(void)
{
    uint32_t rtiModule;
    uint32_t closedWinStatus;
    int32_t  retVal = SDL_PASS;
    SDL_RTI_configParms     pConfig;
    SDL_RTI_staticRegs         pStaticRegs;

    rtiModule = SDL_MCU_RTI0_CFG_BASE;
    UART_printf("RTI Example code UC-3 started\n");

    /* Register Interrupt */
    isrFlag = RTI_NO_INTERRUPT;
	
    /* Configure RTI parameters */
    pConfig.SDL_RTI_dwwdPreloadVal = RTIGetPreloadValue(RTI_CLOCK_SOURCE_32KHZ, RTI_WDT_TIMEOUT);
    pConfig.SDL_RTI_dwwdWindowSize = RTI_DWWD_WINDOWSIZE_50_PERCENT;
    pConfig.SDL_RTI_dwwdReaction   = RTI_DWWD_REACTION_GENERATE_NMI;

    /* Select RTI module clock source */
    RTISetClockSource(SDL_MCU_RTI0_CFG_BASE, RTI_CLOCK_SOURCE_32KHZ);

    retVal = SDL_RTI_config(SDL_INSTANCE_MCU_RTI0_CFG, &pConfig);

    if (retVal == SDL_EFAIL)
    {
        UART_printf("Error during Window configuration.\n");
    }

    /* Verify the config */
    retVal = SDL_RTI_verifyConfig(SDL_INSTANCE_MCU_RTI0_CFG, &pConfig);

    if (retVal == SDL_EFAIL)
    {
        UART_printf("Error during Window Verify configuration.\n");
    }

    if (retVal == SDL_PASS)
    {
        SDL_RTI_readStaticRegs(SDL_INSTANCE_MCU_RTI0_CFG, &pStaticRegs);

        switch(pStaticRegs.RTI_WWDSIZECTRL)
        {
            case RTI_RTIDWWDSIZECTRL_DWWDSIZE_100_PERCENT:
                 UART_printf("    DWWD configured to 100 percent window size\n");
                 break;
            case RTI_RTIDWWDSIZECTRL_DWWDSIZE_50_PERCENT:
                 UART_printf("    DWWD configured to 50 percent window size\n");
                 break;
            case RTI_RTIDWWDSIZECTRL_DWWDSIZE_25_PERCENT:
                 UART_printf("    DWWD configured to 25 percent window size\n");
                 break;
            case RTI_RTIDWWDSIZECTRL_DWWDSIZE_12_5_PERCENT:
                 UART_printf("    DWWD configured to 6.25 percent window size\n");
                 break;
            case RTI_RTIDWWDSIZECTRL_DWWDSIZE_6_25_PERCENT:
                 UART_printf("    DWWD configured to 3.125 percent window size\n");
                 break;
        }
		/* DWWD is configured to 10 sec timeout */
        UART_printf("    DWWD is configured for %u ms time-out \n", RTI_WDT_TIMEOUT);
	}


    if ((retVal == SDL_PASS) &&
        (pConfig.SDL_RTI_dwwdWindowSize != RTI_DWWD_WINDOWSIZE_100_PERCENT))
    {
        /* RTI is serviced in closed window. Generates DWWD violation.
         * Closed window violation cant be generated for 100% window size.
         */
        volatile uint32_t loopBreak = FALSE;
        isrFlag = RTI_NO_INTERRUPT;
		
	    SDL_RTI_start(SDL_INSTANCE_MCU_RTI0_CFG);
		
        while (loopBreak == FALSE)
        {
			/* UC-3 EARLY TRIGGER: Servicing DWWD before window open generates violation 
			   and generates interrupt to ESM module */
            UART_printf("\n\nRTI DWWD Early Trigger violation test running.\n");
			
            RTIDwwdIsClosedWindow(rtiModule, &closedWinStatus);
			
            if (closedWinStatus == TRUE)
            {
                loopBreak = TRUE;
				
				UART_printf("\n\nServicing before the window open.\n");
				
				/* Servicing the DWWD in closed window */
                SDL_RTI_service(SDL_INSTANCE_MCU_RTI0_CFG);	
				
                while (RTI_NO_INTERRUPT == isrFlag)
                {
                    /* Wait for interrupt */
                }
            }
            else
            {
                /* DWWD is in open window, just service it. */
                SDL_RTI_service(SDL_INSTANCE_MCU_RTI0_CFG);
            }
        }
        if (retVal == SDL_PASS)
        {
            UART_printf("\n\nRTI DWWD Early Trigger violation test successful. \n\n");
        }
        else
        {
            UART_printf("RTI DWWD Early Trigger violation test failed. \n");
        }
    }

    return retVal;
}

/* ========================================================================== */
/*                 Internal Function Definitions                              */
/* ========================================================================== */

static void RTISetClockSource(uint32_t rtiModuleSelect,
                              uint32_t rtiClockSourceSelect)
{
#if !defined(SOC_TPR12) && !defined (SOC_AWR294X) /* No need to set clock for TPR12 */
    /* Unlock the MCU_CTRL_MMR before programming the RTI CLKSEL */
    *((uint32_t *)(SDL_MCU_CTRL_MMR0_CFG0_BASE + SDL_MCU_CTRL_MMR_CFG0_LOCK2_KICK0)) = KICK0_UNLOCK_VAL;
    *((uint32_t *)(SDL_MCU_CTRL_MMR0_CFG0_BASE + SDL_MCU_CTRL_MMR_CFG0_LOCK2_KICK1)) = KICK1_UNLOCK_VAL;

    switch (rtiModuleSelect) {
        case SDL_MCU_RTI0_CFG_BASE:
            HW_WR_FIELD32(SDL_MCU_CTRL_MMR0_CFG0_BASE +
                          SDL_MCU_CTRL_MMR_CFG0_MCU_RTI0_CLKSEL,
                          SDL_MCU_CTRL_MMR_CFG0_MCU_RTI0_CLKSEL_CLK_SEL,
                          rtiClockSourceSelect);
            break;
        case SDL_MCU_RTI1_CFG_BASE:
            HW_WR_FIELD32(SDL_MCU_CTRL_MMR0_CFG0_BASE +
                          SDL_MCU_CTRL_MMR_CFG0_MCU_RTI1_CLKSEL,
                          SDL_MCU_CTRL_MMR_CFG0_MCU_RTI1_CLKSEL_CLK_SEL,
                          rtiClockSourceSelect);
            break;
        default:
            UART_printf("Error : RTI Instance not supported "
                "!!!\r\n");
            break;
    }
#endif
}

#if defined(SOC_J721E) || defined (SOC_J7200)
#define SDL_TEST_RTI0_WWD_INTR SDLR_MCU_R5FSS0_CORE0_INTR_MCU_RTI0_INTR_WWD_0
#define SDL_TEST_RTI1_WWD_INTR SDLR_MCU_R5FSS0_CORE0_INTR_MCU_RTI1_INTR_WWD_0
#define SDL_TEST_ESM_INT_HI_LVL SDLR_MCU_R5FSS0_CORE0_INTR_MCU_ESM0_ESM_INT_HI_LVL_0
#endif

#if defined(SOC_TPR12) || defined (SOC_AWR294X)
#define SDL_TEST_ESM_INT_HI_LVL SDL_MSS_INTR_MSS_ESM_HI
#endif

static uint32_t RTIGetPreloadValue(uint32_t rtiClkSource, uint32_t timeoutVal)
{
    uint32_t clkFreqKHz       = (uint32_t) RTI_CLOCK_SOURCE_32KHZ_FREQ_KHZ,
             timeoutNumCycles = 0;

    switch (rtiClkSource)
    {
        case RTI_CLOCK_SOURCE_32KHZ:
            clkFreqKHz = (uint32_t) RTI_CLOCK_SOURCE_32KHZ_FREQ_KHZ;
            break;
        case RTI_CLOCK_SOURCE_200MHZ:
            clkFreqKHz = (uint32_t) RTI_CLOCK_SOURCE_200MHZ_FREQ_KHZ;
            break;
        default:
            break;
    }
    /* Get the clock ticks for given time-out value */
    timeoutNumCycles = timeoutVal * clkFreqKHz;
    return timeoutNumCycles;
}

static void IntrDisable(uint32_t intsrc)
{
    uint32_t intrStatus;

    SDL_RTI_getStatus(SDL_INSTANCE_MCU_RTI0_CFG, &intrStatus);
    SDL_RTI_clearStatus(SDL_INSTANCE_MCU_RTI0_CFG, intrStatus);

    /* Clear ESM registers. */
#if defined(SOC_TPR12) || defined (SOC_AWR294X)

#else
    SDL_ESM_disableIntr(SDL_MCU_ESM0_CFG_BASE, intsrc);
    SDL_ESM_clrNError(SDL_ESM_INST_MCU_ESM0);
#endif
}


int32_t SDL_ESM_applicationCallbackFunction(SDL_ESM_Inst esmInst, SDL_ESM_IntType esmIntrType,
                                            uint32_t grpChannel,  uint32_t index, uint32_t intSrc, void *arg)
{
    int32_t retVal = SDL_PASS;

    UART_printf("\nInterrupt is generated to ESM\n");
    UART_printf("    ESM Call back function called : instType 0x%x, intType 0x%x, " \
                "grpChannel 0x%x, index 0x%x, intSrc 0x%x \n",
                esmInst, esmIntrType, grpChannel, index, intSrc);
    UART_printf("  Take action \n");
	isrFlag = RTI_ESM_INTRPT;
    /* For clearing the interrupt */
    IntrDisable(intSrc);

    return retVal;
}

/********************************* End of file ******************************/
