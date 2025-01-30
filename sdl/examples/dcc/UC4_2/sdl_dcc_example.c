/*
 *  Copyright (c) 2021 Texas Instruments Incorporated
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
 *  \file     sdl_dcc_example.c
 *
 *  \brief    This example application demonstrates comparison of 2 clock
 *              sources.
 *
 *  \details    Different clock sources are provided to Counter 1 (Test Clock)
 *              and Counter 0 (Reference Clock).
 *              This application configures the DCC module to generate Completion Interrupt when
 *              Counter 0 + Valid Counter & Counter 1 reaches 0 at same time.
 **/

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "main.h"

/*===========================================================================*/
/*                         Declarations                                      */
/*===========================================================================*/
/**
 * \brief   Utility function to print on UART console/CCS console
 */
static void SDL_DCCAppPrint(const char * str);
/**
 * \brief   This function returns clock frequencies
 *
 * \param   dccInst     Instance of DCC
 * \param   clk0Src     Clock source for counter 0
 * \param   clk1Src     Clock source for counter 1
 * \param   clk0Freq    Clock frequency for counter 0 in KHz
 * \param   clk1Freq    Clock frequency for counter 1 in KHz
 *
 * \retval  status      Negative number in case of errors
 */
static int32_t SDL_DCCAppGetClkfreqKHz(uint32_t dccInst,
                                    uint32_t clk0Src, uint32_t clk1Src,
                                    uint32_t *clk0Freq, uint32_t *clk1Freq);
/**
 * \brief   This function waits infinitely for DCC done interrupt
 *
 * \param   dccInst     Instance of DCC
 *
 * \retval  SDL_PASS on occurrence DCC completion and no error.
 *          SDL_EFAIL otherwise.
 */
static int32_t SDL_DCCAppWaitForCompletion(uint32_t dccBaseAddr);
/**
 * \brief   This function returns least integral ratio for given clocks.
 *
 * \param   refClkFreq      Reference clock frequency in KHz.
 * \param   testClkFreq     Test clock frequency in KHz.
 * \param   refClkRatioNum  Reference clock ratio number.
 * \param   testClkRatioNum Test clock ratio number.
 *
 * \retval  None.
 */
static void SDL_DCCAppGetClkRatio(uint32_t  refClkFreq,
                              uint32_t  testClkFreq,
                              uint32_t *refClkRatioNum,
                              uint32_t *testClkRatioNum);

/**
 * \brief   This function returns seed value for COUNT1.
 *
 * \param   refClkFreq      Reference clock frequency in KHz.
 * \param   testClkFreq     Test clock frequency in KHz.
 * \param   refClkRatioNum  Reference clock ratio number.
 * \param   testClkRatioNum Test clock ratio number.
 * \param   drfitPer        Allowed drift in test clock in percentage.
 * \param   configParams    DCC configuration parameters.
 *                          Refer enum #SDL_DCC_config.
 *
 * \retval  None.
 */

static void SDL_DCCAppSetSeedVals(uint32_t           refClkFreq,
                              uint32_t           testClkFreq,
                              uint32_t           refClkRatioNum,
                              uint32_t           testClkRatioNum,
                              uint32_t           drfitPer,
                              SDL_DCC_config  *configParams);

/**
 * \brief   This function register ISR for a given instance of DCC
 *
 *
 * \param   dccInst     Instance of DCC
 *
 * \retval  SDL_PASS on successful interrupt handler registration.
 */
static int32_t SDL_DCCAppRegisterIsr(uint32_t dccInst);

/**
 * \brief   ISR for done interrupt, set a global flag
 *
 * \retval  None
 */
static void SDL_DCCAppDoneIntrISR(uintptr_t handle);
/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
volatile uint32_t isrFlag = 0u;
/**< Flag used to indicate occurrence of the completion interrupt */
/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */


int32_t SDL_DCC_example(void)
{
    int32_t retVal, i;
    uint32_t clk0Freq, clk1Freq, refClkRatioNum, testClkRatioNum, testClk, srcClk;
    SDL_DCC_config configParams;
	SDL_DCC_staticRegs staticRegs;
	
    SDL_DCCAppPrint("\r\n" APP_DCC_STR ": Start UC-4\r\n");
	/* UC-4: Configuring the instance to MCU_DCC0 and keeping the reference clock to HFOSC0
			 and Test clock is looping to three clock sources i.e, i)CBASS, ii) MCAN, iii) ADC.
			 There are no error events which is working in single shot mode */
			 
    /* Steps
    1. Determine the clock frequencies for the sources
    2. Figure out the seed values for successful completion
    3. Configure Interrupts for normal completion
    4. Configure DCC instance
    5. Enable DCC
    6. Wait for normal completion
	7. Disable DCC
    */
	
	UART_printf("\nINSTANCE: MCU_DCC0\n");
	
    UART_printf("\nSource clock: HFOSC0 \n");
	
    for(i=0; i<6; i++)
	{
	
		switch(i)
		{
			case 0:
					testClk = SDL_DCC_CLK1_SRC_CLOCK1;
					srcClk = SDL_DCC_CLK0_SRC_CLOCK0_0;
					UART_printf("\nTest clock source: CBASS \n");
			break;
					
			case 1:
					testClk = SDL_DCC_CLK1_SRC_CLOCKSRC2;
					srcClk = SDL_DCC_CLK0_SRC_CLOCK0_0;
					UART_printf("\nTest clock source: MCAN \n");
			break;
			
			case 2:
					testClk = SDL_DCC_CLK1_SRC_CLOCKSRC1;
					srcClk = SDL_DCC_CLK0_SRC_CLOCK0_0;
					UART_printf("\nTest clock source: ADC \n");
					
			break;
			
	/* UC-2: Configuring the instance to MCU_DCC0 and keeping the reference clock to RC OSC
			 and Test clock is looping to three clock sources i.e, i)CBASS, ii) MCAN, iii) ADC.
			 There are no error events which is working in single shot mode */		
		
			
			case 3:
			
			SDL_DCCAppPrint("\r\n\n\n\n" APP_DCC_STR ": Start UC-2\r\n");
			
			UART_printf("\nINSTANCE: MCU_DCC0\n");
	
			UART_printf("\nSource clock: RC OSC\n");
					testClk = SDL_DCC_CLK1_SRC_CLOCK1;
					srcClk = SDL_DCC_CLK0_SRC_CLOCK0_2;
					UART_printf("\nTest clock source: CBASS \n");
			break;
					
			case 4:
					testClk = SDL_DCC_CLK1_SRC_CLOCKSRC2;
					srcClk = SDL_DCC_CLK0_SRC_CLOCK0_2;
					UART_printf("\nTest clock source: MCAN \n");
			break;
			
			case 5:
					testClk = SDL_DCC_CLK1_SRC_CLOCKSRC1;
					srcClk = SDL_DCC_CLK0_SRC_CLOCK0_2;
					UART_printf("\nTest clock source: ADC \n");
					
			break;
		}
	
		/* Step 1 Determine the clock frequencies for the sources */
		retVal = SDL_DCCAppGetClkfreqKHz(APP_DCC_MODULE_INST,
										 srcClk,
										 testClk,
										 &clk0Freq, &clk1Freq);
	
		if (SDL_PASS == retVal)
		{
			/* Step 2 Figure out the seed values for successful completion */
			/* Get clock ratio */
			SDL_DCCAppGetClkRatio(clk0Freq,
								clk1Freq,
								&refClkRatioNum,
								&testClkRatioNum);
	
			configParams.mode    = APP_DCC_MODE;
			configParams.clk0Src = srcClk;
			configParams.clk1Src = testClk;
			/* Get the seed values for given clock selections and allowed drift */
			SDL_DCCAppSetSeedVals(clk0Freq,
								clk1Freq,
								refClkRatioNum,
								testClkRatioNum,
								APP_DCC_TEST_CLOCK_SRC_1_DRIFT,
								&configParams);
	
			/* Step 3 Configure Interrupts for normal completion */
			SDL_DCCAppRegisterIsr(APP_DCC_MODULE_INST);
	
			/* Step 4 Configure DCC instance */
			retVal = SDL_DCC_configure(SDL_DCC_INST_MCU_DCC0, &configParams);
			
			if (SDL_PASS == retVal)
			{
				retVal = SDL_DCC_verifyConfig(SDL_DCC_INST_MCU_DCC0, &configParams);
			}
			else
			{
				retVal = SDL_EFAIL;
			}
			
			if (SDL_PASS == retVal)
			{
				/* Clear the interrupt flag, SDL_DCCAppWaitForCompletion () will pend
					on this */
				isrFlag = 0U;
	
				SDL_DCCAppPrint(APP_DCC_STR ": DCC configured \n");
				
				SDL_DCC_getStaticRegs(SDL_DCC_INST_MCU_DCC0, &staticRegs);
							
				/* Enable DONE interrupt(only for single shot mode) */
				SDL_DCC_enableIntr(SDL_DCC_INST_MCU_DCC0, SDL_DCC_INTERRUPT_DONE);
	
				SDL_DCCAppPrint(APP_DCC_STR ": Enabling DCC and waiting for "
							"completion interrupt \n");
				/* 5 Enable DCC */
				SDL_DCC_enable(SDL_DCC_INST_MCU_DCC0);
	
				/* Step 6. Wait for normal completion */
				if (SDL_PASS == SDL_DCCAppWaitForCompletion(APP_DCC_MODULE_INST))
				{
					SDL_DCCAppPrint(APP_DCC_STR ": DCC Generated completion interrupt \n");
					SDL_DCCAppPrint(APP_DCC_STR ": No Clock Drift was observed \n");
				}
				else
				{
					SDL_DCCAppPrint(APP_DCC_STR ": Error : DCC Generated error interrupt\n");
					SDL_DCCAppPrint(APP_DCC_STR ": Error interrupt is not expected \n");
					retVal = SDL_EFAIL;
				}
				/* Step 7. Disable DCC */
				SDL_DCC_enable(SDL_DCC_INST_MCU_DCC0);
			}
		}
		else
		{
			SDL_DCCAppPrint(APP_DCC_STR ": Error : Could not derive clock "
							"frequency!!!\r\n");
		}
		
		/* Breaking the loop if any of the clock source Drift change observed */
		if(retVal == SDL_EFAIL)
		{
			break;
		}
	}

    return retVal;
}

/* ========================================================================== */
/*                 Internal Function Definitions                              */
/* ========================================================================== */

static void SDL_DCCAppPrint(const char * str)
{
    UART_printf(str);
}

static int32_t SDL_DCCAppGetClkfreqKHz(uint32_t dccInst,
                                        uint32_t clk0Src, uint32_t clk1Src,
                                        uint32_t *clk0Freq, uint32_t *clk1Freq)
{
    int32_t retVal;
    switch (dccInst)
    {
        case SDL_MCU_DCC0_BASE :
            
                /* 25 MHz */
                *clk0Freq = APP_DCC_REF_CLOCK_FREQ_IN_KHZ;
                retVal = SDL_PASS;
            
                /* 60 MHz */
                *clk1Freq = APP_DCC_TEST_CLOCK_FREQ_IN_KHZ;
				
				retVal = SDL_PASS;
            
        break;

        default :
            SDL_DCCAppPrint(APP_DCC_STR ": Error : DCC Instance not supported in"
                        " this example !!!\r\n");
			retVal = SDL_EFAIL;
        break;
    }
    return (retVal);
}

static void SDL_DCCAppGetClkRatio(uint32_t  refClkFreq,
                              uint32_t  testClkFreq,
                              uint32_t *refClkRatioNum,
                              uint32_t *testClkRatioNum)
{
    uint32_t loopCnt, hcf = 1U;

    for (loopCnt = 1;
         (loopCnt <= refClkFreq) || (loopCnt <= testClkFreq);
         loopCnt++)
    {
        if ((refClkFreq % loopCnt == 0) && (testClkFreq % loopCnt == 0))
        {
            hcf = loopCnt;
        }
    }
    *refClkRatioNum  = (refClkFreq / hcf);
    *testClkRatioNum = (testClkFreq / hcf);
}

static void SDL_DCCAppSetSeedVals(uint32_t           refClkFreq,
                              uint32_t           testClkFreq,
                              uint32_t           refClkRatioNum,
                              uint32_t           testClkRatioNum,
                              uint32_t           drfitPer,
                              SDL_DCC_config *configParams)
{
    uint32_t maxFreqKHz, maxCntLimit;
    uint32_t maxRefCnt, minRefCnt;
    uint64_t mulVar;

    /* Find maximum frequency between test and reference clock */
    if (refClkFreq > testClkFreq)
    {
        maxFreqKHz  = refClkFreq;
        maxCntLimit = APP_DCC_SRC0_MAX_VAL;
    }
    else
    {
        maxFreqKHz  = testClkFreq;
        maxCntLimit = APP_DCC_SRC1_MAX_VAL;
    }
    /* Calculate seed values for 0% drift */
    if (maxFreqKHz == refClkFreq)
    {
        configParams->clk0Seed = maxCntLimit / refClkRatioNum;
        configParams->clk0Seed = configParams->clk0Seed * refClkRatioNum;
        mulVar = ((uint64_t) (configParams->clk0Seed) *
                  (uint32_t) (testClkRatioNum));
        configParams->clk1Seed   = (uint32_t) (mulVar / refClkRatioNum);
        configParams->clk0ValidSeed = refClkRatioNum;
    }
    else
    {
        configParams->clk1Seed = maxCntLimit / testClkRatioNum;
        configParams->clk1Seed = configParams->clk1Seed * testClkRatioNum;
        mulVar = ((uint64_t) (configParams->clk1Seed) *
                  (uint32_t) (refClkRatioNum));
        configParams->clk0Seed   = (uint32_t) (mulVar / testClkRatioNum);
        configParams->clk0ValidSeed = 1U;
    }
    /* Applying allowed drift */
    if (((APP_DCC_SRC0_MAX_VAL + APP_DCC_SRC0_VALID_MAX_VAL) <
         (configParams->clk0Seed * (100U + drfitPer) / 100U)))
    {
        /* Seed values with drift exceeds maximum range */
        SDL_DCCAppPrint(APP_DCC_STR ": Seed values with drift exceeds"
                        " allowed range\n");
        SDL_DCCAppPrint(APP_DCC_STR ": Application will run with 0% "
                        " allowed drift\n");
    }
    else if (100U < drfitPer)
    {
        /* Error percentage is greater than 100 */
        SDL_DCCAppPrint(APP_DCC_STR ": Warning Wrong drift %,Not applying drift\n");
        SDL_DCCAppPrint(APP_DCC_STR ": Application will run with 0% drift\n");
    }
    else
    {
        maxRefCnt = (configParams->clk0Seed * (100U + drfitPer) / 100U);
        minRefCnt = (configParams->clk0Seed * (100U - drfitPer) / 100U);
        if (APP_DCC_SRC0_VALID_MAX_VAL < (maxRefCnt - minRefCnt))
        {
            SDL_DCCAppPrint(APP_DCC_STR ": Warning Seed value for valid count "
                        "exceeds allowed range.\n");
            SDL_DCCAppPrint(APP_DCC_STR "Application will run with 0% allowed"
                        " drift.\n");
        }
        else
        {
            if (maxRefCnt == minRefCnt)
            {
                configParams->clk0ValidSeed = 1U;
            }
            else
            {
                configParams->clk0Seed   = minRefCnt;
                configParams->clk0ValidSeed = (maxRefCnt - minRefCnt);
            }
        }
    }
    SDL_DCCAppPrint(APP_DCC_STR ": Seed values calculation done.\n");
}

static int32_t SDL_DCCAppRegisterIsr(uint32_t dccInst)
{
    int32_t retVal = SDL_EFAIL;
    uint32_t dccInt;
    OsalRegisterIntrParams_t intrPrms;
    OsalInterruptRetCode_e osalRetVal;
    HwiP_Handle hwiHandle;

    Osal_RegisterInterrupt_initParams(&intrPrms);
    intrPrms.corepacConfig.arg          = (uintptr_t)0;
    intrPrms.corepacConfig.priority     = 1U;
    intrPrms.corepacConfig.corepacEventNum = 0U; /* NOT USED ? */

    switch (dccInst)
    {
        case SDL_MCU_DCC0_BASE :
		
            dccInt = SDLR_MCU_R5FSS0_CORE0_INTR_MCU_DCC0_INTR_DONE_LEVEL_0;
			
            intrPrms.corepacConfig.isrRoutine   = &SDL_DCCAppDoneIntrISR;

            retVal = SDL_PASS;
        break;

        default :
            SDL_DCCAppPrint(APP_DCC_STR ": Error : DCC Instance not supported in"
                        " this example !!!\r\n");
        break;
    }
    if (SDL_PASS == retVal)
    {
        intrPrms.corepacConfig.intVecNum = dccInt;

        osalRetVal = Osal_RegisterInterrupt(&intrPrms, &hwiHandle);
        if(OSAL_INT_SUCCESS != osalRetVal)
        {
            retVal = SDL_EFAIL;
            SDL_DCCAppPrint(APP_DCC_STR ": Error Could not register ISR !!!\n");
        }
    }
    return (retVal);
}

static int32_t SDL_DCCAppWaitForCompletion(uint32_t dccBaseAddr)
{
    int32_t retVal;
    volatile uint32_t intStatus = 0U;

    /* Wait for completion interrupt / or error flag*/
    while ((0U == isrFlag) &&
           (DCC_DCCSTAT_ERRFLG_MASK != (intStatus & DCC_DCCSTAT_ERRFLG_MASK)))
    {
        intStatus = HW_RD_REG32(dccBaseAddr + DCC_DCCSTAT);
    }

    /* Ensure no error */
    if (DCC_DCCSTAT_ERRFLG_MASK == (intStatus & DCC_DCCSTAT_ERRFLG_MASK))
    {
        retVal = SDL_EFAIL;
    }
    else
    {
        retVal = SDL_PASS;
    }
    return (retVal);
}

static void SDL_DCCAppDoneIntrISR(uintptr_t handle)
{
    SDL_DCC_clearIntr(SDL_DCC_INST_MCU_DCC0, SDL_DCC_INTERRUPT_DONE);
    isrFlag  = 1U;
}

static void SDL_DCCIntrDisable(uint32_t intsrc)
{
    /* Clear ESM registers. */
    SDL_ESM_disableIntr(SDL_MCU_ESM0_CFG_BASE, intsrc);
    SDL_ESM_clrNError(SDL_ESM_INST_MCU_ESM0);
}

int32_t SDL_ESM_applicationCallbackFunction(SDL_ESM_Inst esmInst, SDL_ESM_IntType esmIntrType,
                                            uint32_t grpChannel,  uint32_t index, uint32_t intSrc, void *arg)
{
    int32_t retVal = SDL_PASS;

    UART_printf("\nInterrupt is generated to ESM\n");
    UART_printf("    ESM Call back function called : instType 0x%x, intType 0x%x, " \
                "grpChannel 0x%x, index 0x%x, intSrc 0x%x \n",
                esmInst, esmIntrType, grpChannel, index, intSrc);
    UART_printf("    Take action \n\n");

	isrFlag = DCC_INTERRUPT;
	
    /* For clearing the interrupt */
    SDL_DCCIntrDisable(intSrc);
	
    return retVal;
}
/********************************* End of file ******************************/
