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
 *  \file     mtog_test_func.c
 *
 *  \brief    This file contains MTOG functional test code. .
 *
 *  \details  MTOG Functional tests
 **/

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <string.h>
#include <stdint.h>
#include <src/sdl/sdl_types.h>
#include <src/sdl/sdl_mtog.h>
#include <r5f_startup/interrupt.h>
#include <include/soc.h>
#include <ti/drv/uart/UART_stdio.h>
#include <ti/drv/sciclient/sciclient.h>

/* Osal API header files */
#include <ti/osal/HwiP.h>
#include <ti/osal/TimerP.h>

#include "esm_app.h"

#if defined (SOC_J7200)
#include "test/mtog/j7200/mtog_test_cfg.h"
#endif 

#if defined (SOC_J721S2)
#include "test/mtog/j721s2/mtog_test_cfg.h"
#endif 

#if defined (SOC_J784S4)
#include "test/mtog/j784s4/mtog_test_cfg.h"
#endif 

#ifdef UNITY_INCLUDE_CONFIG_H
#include <test/unity/src/unity.h>
#include <test/unity/config/unity_config.h>
#endif

/* ========================================================================== */
/*                                Macros                                      */
/* ========================================================================== */

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */
void MTOG_datAbortExceptionHandler(void *param);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

const SDL_R5ExptnHandlers MTOG_R5ExptnHandlers =
{
    .udefExptnHandler = NULL,
    .swiExptnHandler = NULL,
    .pabtExptnHandler = NULL,
    .dabtExptnHandler = &MTOG_datAbortExceptionHandler,
    .irqExptnHandler = NULL,
    .fiqExptnHandler = NULL,
    .udefExptnHandlerArgs = ((void *)0u),
    .swiExptnHandlerArgs = ((void *)0u),
    .pabtExptnHandlerArgs = ((void *)0u),
    .dabtExptnHandlerArgs = ((void *)0u),
    .irqExptnHandlerArgs = ((void *)0u),
};

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */
void MTOG_eventHandler( uint32_t instanceIndex );
int32_t MTOG_runTest(uint32_t instanceIndex);

void MTOG_eventHandler( uint32_t instanceIndex )
{
    int32_t status = SDL_PASS;
	
    /* Reset the Timeout gasket */
    status = SDL_MTOG_reset( instanceIndex );
		
    if (status == SDL_PASS)
    {
        UART_printf("\n MTOG instance - %d reset passed",instanceIndex);
    }
	else{
        UART_printf("\n MTOG instance - %d reset failed",instanceIndex);
	}
	MTOG_TestHandleArray[instanceIndex].doneFlag = true;
	
    return;
}

int32_t MTOG_runTest(uint32_t instanceIndex)
{
    int32_t testResult = 0;
    int32_t status;
    uint64_t startTime , testStartTime,  testEndTime, endTime;
    uint64_t prepTime, diffTime, restoreTime;
    volatile uint32_t timeoutCount = 0;
    SDL_MTOG_config config;
	config.timeOut = SDL_MTOG_VAL_1K;
    SDL_MTOG_staticRegs staticRegs;
    UART_printf("\n Starting MTOG test on %s, index %d...",
                MTOG_TestHandleArray[instanceIndex].instanceName,
                instanceIndex);

#ifdef DEBUG
    char inputChar;

    UART_printf("\n Press 'n' to skip..Press any key to continue...");
    inputChar = UART_getc();

    if (inputChar == 'n')
    {
        UART_printf("   Skipping this test. on request \n");
        return 0;
    }
    UART_printf("\n  HwiP_Params_init complete \n");
#endif

    /* Initialize done flag */
    MTOG_TestHandleArray[instanceIndex].doneFlag = false;

    /* Get start time of test */
    startTime = TimerP_getTimeInUsecs();

    /*-- Step 1: Configure ESM handler --*/
    if(instanceIndex < MTOG_MAX_MAIN_INSTANCE)
    {
        status = sdlAppEsmConfig(MAIN_ESM_BASE,
                                 MTOG_TestHandleArray[instanceIndex].ESMEventNumber,
                                 ESM_INTR_PRIORITY_LEVEL_HIGH,
                                 MTOG_TestHandleArray[instanceIndex].handler,
                                 instanceIndex);
    }
    else
	{
        status = sdlAppEsmConfig(MCU_ESM_BASE,
                                 MTOG_TestHandleArray[instanceIndex].ESMEventNumber,
                                 ESM_INTR_PRIORITY_LEVEL_HIGH,
                                 MTOG_TestHandleArray[instanceIndex].handler,
                                 instanceIndex);
    }
    if (status != SDL_PASS)
    {
        UART_printf("   sdlAppEsmConfig Failed \n");
        testResult = -1;
    }

    /** Step 2: Configure and start Master Timeout Gasket */
    if (testResult == 0)
    {
        SDL_MTOG_reset(instanceIndex);
        testResult = SDL_MTOG_init(instanceIndex, &config);
        if (status != SDL_PASS)
        {
            UART_printf("   SDL_MTOG_init Failed \n");
            testResult = -1;
        }
    }
    if (testResult == 0)
    {
        testResult = SDL_MTOG_verifyConfig(instanceIndex, &config);
        if (status != SDL_PASS)
        {
            UART_printf("   SDL_MTOG_verifyConfig Failed \n");
            testResult = -1;
        }
    }
    if (testResult == 0)
    {
        testResult = SDL_MTOG_getStaticRegisters(instanceIndex, &staticRegs);
        if (status != SDL_PASS)
        {
            UART_printf("   SDL_MTOG_getStaticRegisters Failed \n");
            testResult = -1;
        }
    }
    if (testResult == 0)
    {
        /* Call SDL API to enable Timeout Gasket */
        status = SDL_MTOG_start(instanceIndex);
        if (status != SDL_PASS)
        {
            UART_printf("   SDL_MTOG_start Failed \n");
            testResult = -1;
        }
    }
    /* Get start time of test */
    testStartTime = TimerP_getTimeInUsecs();
    /* Step 3: Inject master timeout error */
    if (testResult == 0)
    {
        status = SDL_MTOG_forceTimeout(instanceIndex);
        if (status != SDL_PASS)
        {
            UART_printf("   SDL_MTOG_forceTimeout Failed \n");
            testResult = -1;
        }
    }
    /**--- Step 3: Wait for MTOG Interrupt ---*/
    if (testResult == 0)
    {
        /* Timeout if exceeds time */
        while ((!MTOG_TestHandleArray[instanceIndex].doneFlag)
               && (timeoutCount++ < MTOG_MAX_TEST_TIMEOUT_VALUE))
        {
            /* Interrupt handler available */
            if (MTOG_TestHandleArray[instanceIndex].handler == NULL)
            {
                /* Use Polling */
                MTOG_eventHandler(instanceIndex);
            }
        }
        if(timeoutCount >= MTOG_MAX_TEST_TIMEOUT_VALUE)
        {
            UART_printf("\n Instance-%d Timed out  \n", instanceIndex);
            testResult = -1;
        }
	}
    
    /* Get end time of test */
    testEndTime = TimerP_getTimeInUsecs();

    /**--- Step 4: Disable ESM ---*/
    if (testResult == 0)
    {
        if(instanceIndex < MTOG_MAX_MAIN_INSTANCE)
        {
            status = sdlAppEsmDisable(MAIN_ESM_BASE,
                                    MTOG_TestHandleArray[instanceIndex].ESMEventNumber);
        }
        else{
        status = sdlAppEsmDisable(MCU_ESM_BASE,
        						MTOG_TestHandleArray[instanceIndex].ESMEventNumber);
        }
        if (status != SDL_PASS)
        {
            UART_printf("   sdlAppEsmDisable Failed \n");
            testResult = -1;
        }
    }

    /* Here MTOG test is complete , get end time of test */
    endTime = TimerP_getTimeInUsecs();

    prepTime = testStartTime - startTime;
    diffTime = testEndTime - testStartTime;
    restoreTime = endTime - testEndTime;
    UART_printf("  Delta MTOG prep time in micro secs %d \n", (uint32_t)prepTime );
    UART_printf("  Delta MTOG execution time in micro secs %d \n", (uint32_t)diffTime );
    UART_printf("  Delta MTOG restore time in micro secs %d \n", (uint32_t)restoreTime );

    UART_printf("  MTOG complete for %s \n",
                MTOG_TestHandleArray[instanceIndex].instanceName);

    return (testResult);
}

void MTOG_datAbortExceptionHandler(void *param)
{
    /* This is a fake exception so return */
}

/* MTOG prepare for test */
int32_t MTOG_PrepareForTest(void)
{
    int32_t sdlResult = SDL_PASS;

    sdlResult = sdlAppEsmSetup();

    if (sdlResult == SDL_PASS)
    {
        /* Register exception handler */
        /* This is needed to handle data abort that can happen in the process of injecting the error */
        Intc_RegisterExptnHandlers(&MTOG_R5ExptnHandlers);
    }
    return sdlResult;
}

/* MTOG Functional test */
int32_t MTOG_funcTest(void)
{
    int32_t    testResult = 0;
    uint32_t i=0;
    testResult = MTOG_PrepareForTest();
    if (testResult != 0)
    {
        UART_printf("   MTOG_PrepareForTest failed \n");
    }
    if (testResult == 0)
    { 
        for ( i = 0; i < MTOG_MAX_INSTANCE; i++)
        {
            testResult = MTOG_runTest(i);
            if (testResult != 0)
            {
                UART_printf("   MTOG functional test instance %d failed \n", i);
				break;
            }
            else{
                UART_printf("   MTOG functional test instance %d passed \n", i);
            }
            }
    }
    return (testResult);
}
/* Nothing past this point */
