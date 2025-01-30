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
 *              This application configures the DCC module to generate
 *				Error Interrupt when Counter 1 reaches 0 
 *				before Counter 0 reaches 0
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
	
	UART_printf("\nINSTANCE: DCC0\n");
	
	UART_printf("Source clock: HFOSC0  \n");
	
	UART_printf("Test clock: MCAN \n\n");

    SDL_DCCAppPrint("\r\n" APP_DCC_STR ": Start UC-6\r\n");
	/* UC-6: Configuring the instance to DCC0 and keeping the reference clock to HFOSC0 
			 and Test clock to MCAN. Error event is generated to ESM to take action by triggering 
			 failure which is working in continuous mode */
    
    retVal = SDL_DCCAppGetClkfreqKHz(APP_DCC_MODULE_INST,
                                     APP_DCC_REF_CLOCK_SRC_0,
                                     APP_DCC_TEST_CLOCK_SRC_1,
                                     &clk0Freq, &clk1Freq);
    if (SDL_PASS == retVal)
    {
        
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

        
        SDL_DCCAppRegisterIsr(APP_DCC_MODULE_INST);

        
        retVal = SDL_DCC_configure(SDL_DCC_INST_DCC0, &configParams);
		
		if (SDL_PASS == retVal)
		{
			retVal = SDL_DCC_verifyConfig(SDL_DCC_INST_DCC0, &configParams);
		}
		else
		{
			retVal = SDL_EFAIL;
		}
         
        if (retVal == SDL_PASS)
        {
            SDL_DCCAppPrint(APP_DCC_STR ": Changing the clock ratios \n");
			
            /* Deliberately change the Reference Clock to 3times to
             * introduce the error in the clock ratio
             */
            SDL_DCCAppGetClkRatio(clk0Freq*3U,
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
            retVal = SDL_DCC_configure(SDL_DCC_INST_DCC0, &configParams);
            if (retVal != SDL_PASS)
            {
                SDL_DCCAppPrint(APP_DCC_STR ": Error in setting the DCC Seed Config \n");
			#if defined (UNITY_INCLUDE_CONFIG_H)
                TEST_FAIL();
			#endif
                while (1);
            }
			
			/* Enable ERROR interrupt */
            SDL_DCC_enableIntr(SDL_DCC_INST_DCC0, SDL_DCC_INTERRUPT_ERR);
			
            SDL_DCCAppPrint(APP_DCC_STR ": Enabling DCC and waiting for "
                        "Error interrupt \n");
            SDL_DCC_enable(SDL_DCC_INST_DCC0);

            /* Wait for error notification */
			
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
				SDL_DCCAppPrint(APP_DCC_STR ": DCC Generated Error interrupt \n");
				SDL_DCCAppPrint(APP_DCC_STR ": Indicating clock drift/change \n");
				isrFlag = DCC_NO_INTERRUPT;
			}
			else
			{
				SDL_DCCAppPrint(APP_DCC_STR ": Could not generate Error interrupt \n");
				retVal = SDL_EFAIL;
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
    int32_t retVal = SDL_EFAIL;
    switch (dccInst)
    {
        case SDL_DCC0_BASE :
            if (SDL_DCC_CLK0_SRC_CLOCK0_0 == clk0Src)
            {
                /* 25 MHz */
                *clk0Freq = APP_DCC_REF_CLOCK_FREQ_IN_KHZ;
                retVal = SDL_PASS;
            }
            if ((SDL_DCC_CLK1_SRC_CLOCKSRC3 == clk1Src) &&
                (SDL_PASS == retVal))
            {
                /* 60 MHz */
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
        case SDL_DCC0_BASE :
		
            dccInt = SDLR_ESM0_ESM_LVL_EVENT_DCC0_INTR_ERR_LEVEL_0;
			
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
    SDL_DCC_clearIntr(SDL_DCC_INST_DCC0, SDL_DCC_INTERRUPT_DONE);
    isrFlag  = 1U;
}

static void SDL_DCCIntrDisable(uint32_t intsrc)
{
    /* Clear ESM registers. */
    SDL_ESM_disableIntr(SDL_ESM0_CFG_BASE, intsrc);
    SDL_ESM_clrNError(SDL_ESM_INST_MAIN_ESM0);
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
