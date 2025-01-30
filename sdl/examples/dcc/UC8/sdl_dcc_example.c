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
 *  \file     sdl_dcc_funcTest.c
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
volatile uint32_t isrFlag = 0U;
/**< Flag used to indicate occurrence of the completion interrupt */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */


int32_t SDL_DCC_example(void)
{
    int32_t retVal;
    uint32_t clk0Freq, clk1Freq, refClkRatioNum, testClkRatioNum;
    SDL_DCC_config configParams;
	
	UART_printf("\nINSTANCE: MCU_DCC2\n");
	
	UART_printf("Source clock: RC OSC  \n");
	
	UART_printf("Test clock: HFOSC0 \n\n");

    SDL_DCCAppPrint("\r\n" APP_DCC_STR ": Start UC-8\r\n");
	/* UC-8: Configuring the instance to MCU_DCC2 and keeping the reference clock to RC OSC 
			 and Test clock to HFOSC0. This use case will work in continuous mode and 
			 stopped after some time with no error events*/
    
	/* Step 1 Determine the clock frequencies for the sources */
    retVal = SDL_DCCAppGetClkfreqKHz(APP_DCC_MODULE_INST,
                                     APP_DCC_REF_CLOCK_SRC_0,
                                     APP_DCC_TEST_CLOCK_SRC_1,
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
        configParams.clk0Src = APP_DCC_REF_CLOCK_SRC_0;
        configParams.clk1Src = APP_DCC_TEST_CLOCK_SRC_1;
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
        retVal = SDL_DCC_configure(SDL_DCC_INST_MCU_DCC2, &configParams);
		
		if (SDL_PASS == retVal)
		{
			retVal = SDL_DCC_verifyConfig(SDL_DCC_INST_MCU_DCC2, &configParams);
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
			
			/* Enable ERROR interrupt */
            HW_WR_FIELD32(APP_DCC_MODULE_INST + DCC_DCCGCTRL, DCC_DCCGCTRL_ERRENA,
                          DCC_DCCGCTRL_ERRENA_ENABLE);

            SDL_DCCAppPrint(APP_DCC_STR ": Enabling DCC\n");
            /* Step 5 Enable DCC */
            SDL_DCC_enable(SDL_DCC_INST_MCU_DCC2);

			SDL_DCCAppPrint(APP_DCC_STR ": Waiting for some time to perform Continuous mode \n");
            volatile int32_t i = 0;
			/* Wait for the ESM interrupt to report the error */
			do {
				i++;
				if (i > 0x0FFFFFFF)
				{
					/* Timeout for the wait */
					break;
				}
			} while (isrFlag == DCC_NO_INTERRUPT);
			
			if (isrFlag == DCC_INTERRUPT)
			{
				SDL_DCCAppPrint(APP_DCC_STR ": DCC Generated Error interrupt when it is not expected \n");
				isrFlag = DCC_NO_INTERRUPT;
				retVal = SDL_EFAIL;
			}
			else
			{
				SDL_DCCAppPrint(APP_DCC_STR ": DCC module is disabled \n");
				/* Step 6 Disable DCC */
				SDL_DCC_disable(SDL_DCC_INST_MCU_DCC2);
				retVal = SDL_PASS;
			}
        }
    }
    else
    {
        SDL_DCCAppPrint(APP_DCC_STR ": Error : Could not derive clock "
                        "frequency!!!\r\n");
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
    int32_t retVal = SDL_PASS;
    switch (dccInst)
    {
        case SDL_MCU_DCC2_BASE :
            if (SDL_DCC_CLK0_SRC_CLOCK0_2 == clk0Src)
            {
              *clk0Freq = APP_DCC_REF_CLOCK_FREQ_IN_KHZ;
                retVal = SDL_PASS;
            }
            if ((SDL_DCC_CLK1_SRC_CLOCKSRC6 == clk1Src) &&
                (SDL_PASS == retVal))
            {
              *clk1Freq = APP_DCC_TEST_CLOCK_FREQ_IN_KHZ;
            }
            else
            {
                retVal = SDL_EFAIL;
            }
            if (SDL_PASS != retVal)
            {
                SDL_DCCAppPrint(APP_DCC_STR ": Error : Selected clock sources is"
                        "not supported in this example !!!\r\n");
            }
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
            /* To reduce the seed values so valid range will not be exceeded */
            configParams->clk0Seed = configParams->clk0Seed / 2;
            configParams->clk1Seed = configParams->clk1Seed / 2;
            maxRefCnt = (configParams->clk0Seed * (100U + drfitPer) / 100U);
            minRefCnt = (configParams->clk0Seed * (100U - drfitPer) / 100U);
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
        case SDL_MCU_DCC2_BASE :
		
            dccInt = SDLR_MCU_R5FSS0_CORE0_INTR_MCU_DCC2_INTR_DONE_LEVEL_0;
			
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

static void SDL_DCCAppDoneIntrISR(uintptr_t handle)
{
    SDL_DCC_clearIntr(SDL_DCC_INST_MCU_DCC2, SDL_DCC_INTERRUPT_DONE);
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
