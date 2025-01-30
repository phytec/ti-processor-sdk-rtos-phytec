/*
 *   Copyright (c) Texas Instruments Incorporated 2022
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
 *  \file     ccm_func_test.c
 *
 *  \brief    This file contains CCM Functional test code.
 *
 *  \details  CCM Functional tests
 **/

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <stdint.h>
#include <string.h>
#include <sdl_types.h>
#include "ccm_test_main.h"
#include "src/ip/r5/sdl_mcu_armss_ccmr5.h"
#include <sdl_esm.h>
/* For Timer functions */
#include <ti/osal/osal.h>
/* Osal API header files */
#include <ti/osal/HwiP.h>
/* ========================================================================== */
/*                                Macros                                      */
/* ========================================================================== */

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
HwiP_Handle SDTF_EsmHiHwiPHandle;
HwiP_Handle SDTF_EsmLoHwiPHandle;
HwiP_Handle SDTF_EsmCfgHwiPHandle;
extern int32_t SDL_ESM_applicationCallbackFunction(SDL_ESM_Inst esmInstType,
                                                   SDL_ESM_IntType esmIntType,
                                                   uint32_t grpChannel,
                                                   uint32_t index,
                                                   uint32_t intSrc,
                                                   void *arg);
											   
SDL_ESM_config CCM_Test_esmInitConfig_MCU =
{
    .esmErrorConfig = {0u, 3u}, /* Self test error config */

    .enableBitmap = {0xffffffffu, 0xff0fffffu, 0x7fffffffu, 0x0000000fu,
                },
     /**< All events enable: except timer and self test  events, and Main ESM output */
    /* Temporarily disabling vim compare error as well*/
    .priorityBitmap = {0xffffffffu, 0xff0fffffu, 0x7fffffffu, 0x0000000fu,
                        },
    /**< All events high priority: except timer, selftest error events, and Main ESM output */
    .errorpinBitmap = {0xffffffffu, 0xff0fffffu, 0x7fffffffu, 0x0000000fu,
                      },
    /**< All events high priority: except timer, selftest error events, and Main ESM output */
};

SDL_ESM_config CCM_Test_esmInitConfig_WKUP =
{
    .esmErrorConfig = {0u, 8u}, /* Self test error config */
    .enableBitmap = {0xffffffffu, 0x00180003u, 0xffffffffu,
                },
     /**< All events enable: except clkstop events for unused clocks */
    .priorityBitmap = {0xffffffffu, 0x00180003u, 0xffffffffu,
                        },
    /**< All events high priority: except clkstop events for unused clocks */
    .errorpinBitmap = {0xffffffffu, 0x00180003u, 0xffffffffu,
                      },
    /**< All events high priority: except clkstop for unused clocks
     *   and selftest error events */
};

SDL_ESM_config CCM_Test_esmInitConfig_MAIN =
{
    .esmErrorConfig = {1u, 8u}, /* Self test error config */
#if defined (SOC_J721E) || defined (SOC_J7200)
    .enableBitmap = {0x00000000u, 0xfffffffbu, 0x7fffffffu, 0xffffffffu,
                 0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                 0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                 0xffffffffu, 0xffffffffu, 0xffffffffu, 0x00000000u,
                 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                 0xffffffffu,
                },
     /**< All events enable: except clkstop events for unused clocks
      *   and PCIE events */
    .priorityBitmap = {0x00000000u, 0xfffffffbu, 0x7fffffffu, 0xffffffffu,
                         0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                         0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                         0xffffffffu, 0xffffffffu, 0xffffffffu, 0x00000000u,
                         0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                         0xffffffffu,
                        },
    /**< All events high priority: except clkstop events for unused clocks
     *   and PCIE events */
    .errorpinBitmap = {0x00000000u, 0xfffffffbu, 0x7fffffffu, 0xffffffffu,
                       0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                       0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                       0xffffffffu, 0xffffffffu, 0xffffffffu, 0x00000000u,
                       0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                       0xffffffffu,
                      },
#endif

#if defined (SOC_J721S2)
    .enableBitmap = {0x00000000u, 0xffffffdbu, 0x7fffffffu, 0xffffffffu,
                 0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                 0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                 0xffffffffu, 0xffffffffu, 0xffffffffu, 0x00000000u,
                 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                 0xffffffffu,
                },
     /**< All events enable: except clkstop events for unused clocks
      *   and PCIE events */
    .priorityBitmap = {0x00000000u, 0xffffffdbu, 0x7fffffffu, 0xffffffffu,
                         0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                         0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                         0xffffffffu, 0xffffffffu, 0xffffffffu, 0x00000000u,
                         0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                         0xffffffffu,
                        },
    /**< All events high priority: except clkstop events for unused clocks
     *   and PCIE events */
    .errorpinBitmap = {0x00000000u, 0xffffffdbu, 0x7fffffffu, 0xffffffffu,
                       0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                       0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                       0xffffffffu, 0xffffffffu, 0xffffffffu, 0x00000000u,
                       0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                       0xffffffffu,
                      },
#endif  

#if defined (SOC_J784S4)
    .enableBitmap = {0x00000000u, 0xffffffdbu, 0x7fffffffu, 0xffffffffu,
                 0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                 0xffffffffu, 0xfffffffbu, 0xffffffffu, 0xffffffffu,
                 0xffff7fffu, 0xffffffffu, 0xffffffffu, 0x00000000u,
                 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                 0xffffffffu,
                },
     /**< All events enable: except clkstop events for unused clocks
      *   and PCIE events */
    .priorityBitmap = {0x00000000u, 0xffffffdbu, 0x7fffffffu, 0xffffffffu,
                         0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                         0xffffffffu, 0xfffffffbu, 0xffffffffu, 0xffffffffu,
                         0xffff7fffu, 0xffffffffu, 0xffffffffu, 0x00000000u,
                         0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                         0xffffffffu,
                        },
    /**< All events high priority: except clkstop events for unused clocks
     *   and PCIE events */
    .errorpinBitmap = {0x00000000u, 0xffffffdbu, 0x7fffffffu, 0xffffffffu,
                       0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                       0xffffffffu, 0xfffffffbu, 0xffffffffu, 0xffffffffu,
                       0xffff7fffu, 0xffffffffu, 0xffffffffu, 0x00000000u,
                       0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                       0xffffffffu,
                      },
#endif
				  

    /**< All events high priority: except clkstop for unused clocks
     *   and PCIE events */
};
												   
/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

static uint32_t arg;

/*********************************************************************
 * @fn      SDL_TEST_CCMSelfTest
 *
 * @brief   Execute CCM Self test
 *
 * @param   None
 *
 * @return  0 : Success; < 0 for failures
 */
int32_t SDL_TEST_CCMSelfTest(void)
{
    int32_t result;
    int32_t retVal=0;

    UART_printf("\n CCM self test: starting");

    result = SDL_CCM_selfTest(SDL_CCM_MCU_R5F0,
	                          SDL_CCM_MONITOR_TYPE_OUTPUT_COMPARE_BLOCK,
                              SDL_CCM_SELFTEST_TYPE_NORMAL, 0U,
                              10000000);

    if (result != SDL_PASS ) {
        UART_printf("\n CCM self test failed");
        retVal = -1;
    } else {
        UART_printf("\n CCM Self Test complete");
    }

    return retVal;
}


/*********************************************************************
 * @fn      SDL_TEST_CCMSelfTestErrorForce
 *
 * @brief   Execute CCM Self test with error force
 *
 * @param   None
 *
 * @return  0 : Success; < 0 for failures
 */
int32_t SDL_TEST_CCMSelfTestErrorForce(void)
{
    int32_t result;
    int32_t retVal=0;

    UART_printf("\n CCM self test with error forcing: starting");

    result = SDL_CCM_selfTest(SDL_CCM_MCU_R5F0,
	                          SDL_CCM_MONITOR_TYPE_OUTPUT_COMPARE_BLOCK,
                              SDL_CCM_SELFTEST_TYPE_ERROR_FORCING, 0U,
                              10000000);

    if (result != SDL_PASS ) {
        UART_printf("\n CCM self test with error forcing failed");
        retVal = -1;
    } else {
        UART_printf("\n CCM Self Test with error forcing complete");
    }

    return retVal;
}

int32_t SDL_TEST_CCMSelfTest_Inactivity_ErrorForce(void)
{
    int32_t result;
    int32_t retVal=0;

    UART_printf("\n CCM Inactivity self test with error forcing: starting");

    result = SDL_CCM_selfTest(SDL_CCM_MCU_R5F0,
                              SDL_CCM_MONITOR_TYPE_INACTIVITY_MONITOR,
                              SDL_CCM_SELFTEST_TYPE_ERROR_FORCING, 0U,
                              10000000);

    if (result != SDL_PASS ) {
        UART_printf("\n CCM Inactivity self test with error forcing failed");
        retVal = -1;
    } else {
        UART_printf("\n CCM Inactivity Self Test with error forcing complete");
    }

    return retVal;
}

int32_t SDL_TEST_CCMSelfTest_VIM_ErrorForce(void)
{
    int32_t result;
    int32_t retVal=0;

    UART_printf("\n CCM VIM self test with error forcing: starting");

    result = SDL_CCM_selfTest(SDL_CCM_MCU_R5F0,
                              SDL_CCM_MONITOR_TYPE_VIM,
                              SDL_CCM_SELFTEST_TYPE_ERROR_FORCING, 0U,
                              10000000);

    if (result != SDL_PASS ) {
        UART_printf("\n CCM VIM self test with error forcing failed");
        retVal = -1;
    } else {
        UART_printf("\n CCM VIM Self Test with error forcing complete");
    }

    return retVal;
}

/*********************************************************************
 * @fn      SDL_TEST_CCMInjectError
 *
 * @brief   Execute CCM Inject Error test
 *
 * @param   None
 *
 * @return  0 : Success; < 0 for failures
 */
int32_t SDL_TEST_CCMInjectError(void)
{
    int32_t result;
    int32_t retVal=0;

    UART_printf("\n CCM inject  error: test starting");

    result = SDL_CCM_injectError(SDL_CCM_MCU_R5F0, SDL_CCM_MONITOR_TYPE_OUTPUT_COMPARE_BLOCK);

    if (result != SDL_PASS ) {
        UART_printf("\n CCM inject failed");
       retVal = -1;
    } else {
        UART_printf("\n CCM inject Test complete");
    }

    return retVal;
}

int32_t SDL_TEST_CCMInjectVIMError(void)
{
    int32_t result;
    int32_t retVal=0;

    UART_printf("\n CCM inject VIM error: test starting");

    result = SDL_CCM_injectError(SDL_CCM_MCU_R5F0, SDL_CCM_MONITOR_TYPE_VIM);

    if (result != SDL_PASS ) {
        UART_printf("\n CCM inject VIM failed");
       retVal = -1;
    } else {
        UART_printf("\n CCM inject VIM Test complete");
    }

    return retVal;
}

int32_t SDL_TEST_CCMInjectInactivityError(void)
{
    int32_t result;
    int32_t retVal=0;

    UART_printf("\n CCM inject inactivity monitor error: test starting");

    result = SDL_CCM_injectError(SDL_CCM_MCU_R5F0, SDL_CCM_MONITOR_TYPE_INACTIVITY_MONITOR);

    if (result != SDL_PASS ) {
        UART_printf("\n CCM inject failed");
       retVal = -1;
    } else {
        UART_printf("\n CCM inject inactivity monitor Test complete");
    }

    return retVal;
}

/*********************************************************************
 * @fn      SDL_TEST_CCMSelftestPolarityInvert
 *
 * @brief   Execute CCM with polarity inversion
 *
 * @param   None
 *
 * @return  0 : Success; < 0 for failures
 */
int32_t SDL_TEST_CCMSelftestPolarityInvert(void)
{
    int32_t result;
    int32_t retVal=0;

    UART_printf("\n CCM polarity invert self test: starting");

    result = SDL_CCM_selfTest(SDL_CCM_MCU_R5F0,
	                          SDL_CCM_MONITOR_TYPE_OUTPUT_COMPARE_BLOCK,
                              SDL_CCM_SELFTEST_POLARITY_INVERSION, 0xFFU,
                              10000000);

    if (result != SDL_PASS ) {
        UART_printf("\n CCM polarity invert self test failed");
       retVal = -1;
    } else {
        UART_printf("\n CCM polarity invert self test complete");
    }

    return retVal;
}

/*********************************************************************
 * @fn      SDL_TEST_CCMVIMSelfTest
 *
 * @brief   Execute CCM VIM Self test
 *
 * @param   None
 *
 * @return  0 : Success; < 0 for failures
 */
int32_t SDL_TEST_CCMVIMSelfTest(void)
{
    int32_t result;
    int32_t retVal=0;

    UART_printf("\n CCM VIM self test: starting");

    result = SDL_CCM_selfTest(SDL_CCM_MCU_R5F0, SDL_CCM_MONITOR_TYPE_VIM,
                              SDL_CCM_SELFTEST_TYPE_NORMAL, 0U,
                              10000000);

    if (result != SDL_PASS ) {
        UART_printf("\n CCM VIM self test failed");
        retVal = -1;
    } else {
        UART_printf("\n CCM VIM Self Test complete");
    }

    return retVal;
}

/*********************************************************************
 * @fn      SDL_TEST_CCMInactivitySelfTest
 *
 * @brief   Execute CCM inactivity monitor Self test
 *
 * @param   None
 *
 * @return  0 : Success; < 0 for failures
 */
int32_t SDL_TEST_CCMInactivitySelfTest(void)
{
    int32_t result;
    int32_t retVal=0;

    UART_printf("\n CCM inactivity monitor self test: starting");

    result = SDL_CCM_selfTest(SDL_CCM_MCU_R5F0, SDL_CCM_MONITOR_TYPE_INACTIVITY_MONITOR,
                              SDL_CCM_SELFTEST_TYPE_NORMAL, 0U,
                              10000000);

    if (result != SDL_PASS ) {
        UART_printf("\n CCM inactivity monitor self test failed");
        retVal = -1;
    } else {
        UART_printf("\n CCM inactivity monitor Self Test complete");
    }

    return retVal;
}

static int32_t CCM_runTest(uint32_t instanceId)
{
    int32_t       testResult = 0;
    SDL_ErrType_t sdlResult;

    if(testResult == 0)
    {
        sdlResult = SDL_TEST_CCMSelfTest();
	if (sdlResult != SDL_PASS)
	{
		UART_printf("sdlCcm_funcTest: failure on line no. %d \n", __LINE__);
		testResult = -1;
	}
    }
	if(testResult == 0)
	{
        sdlResult = SDL_TEST_CCMSelfTestErrorForce();
    	if (sdlResult != SDL_PASS)
    	{
    	 	UART_printf("sdlCcm_funcTest: failure on line no. %d \n", __LINE__);
    		testResult = -1;
    	}
	}
	if(testResult == 0)
	{
        sdlResult = SDL_TEST_CCMInjectError();
        if (sdlResult != SDL_PASS)
    	{
            UART_printf("sdlCcm_funcTest: failure on line no. %d \n", __LINE__);
            testResult = -1;
        }
    }
    if(testResult == 0)
    {
        sdlResult = SDL_TEST_CCMInactivitySelfTest();
			if (sdlResult != SDL_PASS)
			{
				UART_printf("sdlCcm_funcTest: failure on line no. %d \n", __LINE__);
				testResult = -1;
			}
    	}
	if(testResult == 0)
	{
        sdlResult = SDL_TEST_CCMSelfTest_Inactivity_ErrorForce();
    	if (sdlResult != SDL_PASS)
    	{
    		UART_printf("sdlCcm_funcTest: failure on line no. %d \n", __LINE__);
    		testResult = -1;
    	}
	}
	if(testResult == 0)
	{
        sdlResult = SDL_TEST_CCMInjectInactivityError();
    	if (sdlResult != SDL_PASS)
    	{
    		UART_printf("sdlCcm_funcTest: failure on line no. %d \n", __LINE__);
    		testResult = -1;
    	}
	}
	if(testResult == 0)
	{
        sdlResult = SDL_TEST_CCMSelftestPolarityInvert();
    	if (sdlResult != SDL_PASS)
    	{
    		UART_printf("sdlCcm_funcTest: failure on line no. %d \n", __LINE__);
    		testResult = -1;
    	}
	}
	if(testResult == 0)
	{
        sdlResult = SDL_TEST_CCMVIMSelfTest();
    	if (sdlResult != SDL_PASS)
    	{
    		UART_printf("sdlCcm_funcTest: failure on line no. %d \n", __LINE__);
    		testResult = -1;
    	}
	}
	if(testResult == 0)
	{
        sdlResult = SDL_TEST_CCMSelfTest_VIM_ErrorForce();
    	if (sdlResult != SDL_PASS)
    	{
    		UART_printf("sdlCcm_funcTest: failure on line no. %d \n", __LINE__);
    		testResult = -1;
    	}
	}
	if(testResult == 0)
	{
        sdlResult = SDL_TEST_CCMInjectVIMError();
    	if (sdlResult != SDL_PASS)
    	{
    		UART_printf("sdlCcm_funcTest: failure on line no. %d \n", __LINE__);
    		testResult = -1;
    	}
	}
	if(testResult == 0)
	{
        sdlResult = SDL_CCM_injectError(instanceId, SDL_CCM_MONITOR_TYPE_OUTPUT_COMPARE_BLOCK);
    	if (sdlResult != SDL_PASS)
    	{
    		UART_printf("sdlCcm_funcTest: failure on line no. %d \n", __LINE__);
    		testResult = -1;
    	}
	}

    return (testResult);
}


int32_t CCM_Test_init (void)
{
    int32_t result, retValue=0;
    void *ptr = (void *)&arg;
	
    if (retValue == 0) {
        /* Initialize MCU ESM module */
        result = SDL_ESM_init(SDL_ESM_INST_MCU_ESM0, &CCM_Test_esmInitConfig_MCU, SDL_ESM_applicationCallbackFunction, ptr);
        if (result != SDL_PASS) {
            /* print error and quit */
             UART_printf("\n CM_Test_init: Error initializing MCU ESM: result = %d\n", result);

            retValue = -1;
        } else {
            UART_printf("\n CCM_Test_init: Init MCU ESM complete \n");
        }
    }

    if (retValue == 0) {
        /* Initialize MAIN ESM module */
        result = SDL_ESM_init(SDL_ESM_INST_MAIN_ESM0, &CCM_Test_esmInitConfig_MAIN,SDL_ESM_applicationCallbackFunction,ptr);
        if (result != SDL_PASS) {
            /* print error and quit */
            UART_printf("CCM_Test_init: Error initializing MAIN ESM: result = %d\n", result);

            retValue = -1;
        } else {
            UART_printf("\nCCM_Test_init: Init MAIN ESM complete \n");
        }
    }
	
    if (retValue == 0) {
        /* Initialize WKUP ESM module */
        result = SDL_ESM_init(SDL_ESM_INST_WKUP_ESM0, &CCM_Test_esmInitConfig_WKUP, SDL_ESM_applicationCallbackFunction,ptr);
        if (result != SDL_PASS) {
            /* print error and quit */
             UART_printf("CCM_Test_init: Error initializing WKUP ESM: result = %d\n", result);

            retValue = -1;
        } else {
            UART_printf("\n CCM_Test_init: Init WKUP ESM complete \n");
        }
    }

    if (retValue == 0) {
        /* Wait for any self test running to settle */
        Osal_delay(2000);
        /* Initialize CCM */
        result = SDL_CCM_init(0);
        if (result != SDL_PASS) {
            /* print error and quit */
            UART_printf("CCM_Test_init: Error result = %d\n", result);

            retValue = -1;
        } else {
            UART_printf("\nCCM_Test_init: CCM Init complete \n");
        }
		Osal_delay(2000);
		        /* Initialize CCM */
        result = SDL_CCM_verifyConfig(0);
        if (result != SDL_PASS) {
            /* print error and quit */
            UART_printf("CCM_Test_init: Error result = %d\n", result);

            retValue = -1;
        } else {
            UART_printf("\nCCM_Test_init: CCM Init complete \n");
        }
    }

    return retValue;
}

/* CCM Functional test */
int32_t CCM_funcTest(void)
{
    int32_t    testResult = 0;

    testResult = CCM_Test_init();

    if (testResult != 0)
    {
        UART_printf("\n CCM SDL API tests: unsuccessful");
        return SDL_EFAIL;
    }
	
	if (testResult == SDL_PASS)
	{
		UART_printf("\nCCM Functional Test \r\n");
	    /* Run the test for diagnostics first */
		testResult = CCM_runTest(SDL_CCM_MCU_R5F0);
	}
	else
	{
		UART_printf("\r\nCCM Init failed. Exiting the app.\r\n");
	}


    return (testResult);
}
/* Nothing past this point */
