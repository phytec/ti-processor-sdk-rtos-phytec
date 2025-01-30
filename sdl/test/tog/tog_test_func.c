/*
 *   Copyright (c) Texas Instruments Incorporated 2021
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
 *  \file     tog_test_func.c
 *
 *  \brief    This file contains TOG functional test code. .
 *
 *  \details  TOG Functional tests
 **/

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <string.h>
#include <stdint.h>
#include <src/sdl/sdl_types.h>
#include <src/sdl/sdl_tog.h>
#include <r5f_startup/interrupt.h>
#include <include/soc.h>
#include <ti/drv/uart/UART_stdio.h>
#include <ti/drv/sciclient/sciclient.h>

/* Osal API header files */
#include <ti/osal/HwiP.h>
#include <ti/osal/TimerP.h>
#include <ti/osal/osal.h>

#include "esm_app.h"

#if defined (SOC_J721E)
#include "test/tog/j721e/tog_test_cfg.h"
#endif /* SOC_J721E */

#if defined (SOC_J7200)
#include "test/tog/j7200/tog_test_cfg.h"
#endif /* SOC_J7200 */

#if defined (SOC_J721S2)
#include "test/tog/j721s2/tog_test_cfg.h"
#endif /* SOC_J721S2 */

#if defined (SOC_J784S4)
#include "test/tog/j784s4/tog_test_cfg.h"
#endif /* SOC_J784S4 */

#ifdef UNITY_INCLUDE_CONFIG_H
#include <test/unity/src/unity.h>
#include <test/unity/config/unity_config.h>
#endif
/* #define DEBUG */
/* ========================================================================== */
/*                                Macros                                      */
/* ========================================================================== */


/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */
void TOG_datAbortExceptionHandler(void *param);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

const SDL_R5ExptnHandlers TOG_R5ExptnHandlers =
{
    .udefExptnHandler = NULL,
    .swiExptnHandler = NULL,
    .pabtExptnHandler = NULL,
    .dabtExptnHandler = &TOG_datAbortExceptionHandler,
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
__attribute__((section(".text:TOG_test"))) void TOG_eventHandler( uint32_t instanceIndex );
__attribute__((section(".text:TOG_test"))) int32_t TOG_runTest(uint32_t instanceIndex);


void TOG_eventHandler( uint32_t instanceIndex )
{
    int32_t status = SDL_PASS;
    uint32_t pendInts;
    uint32_t intCount;
    SDL_TOG_errInfo errInfo;
    SDL_TOG_config cfg;
    SDL_TOG_Inst instance;

    instance = instanceIndex;
    cfg.cfgCtrl = SDL_TOG_CFG_TIMEOUT;

    if (TOG_TestHandleArray[instanceIndex].intSrcBitmap != 0U)
    {
        /* Read error info */
        status = SDL_TOG_getErrInfo(instance, &errInfo);
        TEST_ASSERT_EQUAL_INT32(SDL_PASS, status);
    }

    if (TOG_TestHandleArray[instanceIndex].intSrcBitmap
            & SDL_TOG_INTRSRC_TRANSACTION_TIMEOUT)
    {
        /* Get Transaction timeout interrupt count */
        if (status == SDL_PASS)
        {
            status = SDL_TOG_getIntrCount(instance, SDL_TOG_INTRSRC_TRANSACTION_TIMEOUT, &intCount);
            TEST_ASSERT_EQUAL_INT32(SDL_PASS, status);
        }

        /* Clear Transaction timeout interrupt events */
        if ((status == SDL_PASS) && (intCount != 0))
        {
            status = SDL_TOG_ackIntr(instance, SDL_TOG_INTRSRC_TRANSACTION_TIMEOUT, intCount);
            TEST_ASSERT_EQUAL_INT32(SDL_PASS, status);
        }
    }

    if (TOG_TestHandleArray[instanceIndex].intSrcBitmap
            & SDL_TOG_INTRSRC_UNEXPECTED_RESPONSE)
    {
        /* Get Unexpected Response interrupt count */
        if (status == SDL_PASS)
        {
            status = SDL_TOG_getIntrCount(instance, SDL_TOG_INTRSRC_UNEXPECTED_RESPONSE, &intCount);
            TEST_ASSERT_EQUAL_INT32(SDL_PASS, status);
        }

        /* Clear Unexpected response interrupt events */
        if ((status == SDL_PASS) && (intCount != 0))
        {
            status = SDL_TOG_ackIntr(instance, SDL_TOG_INTRSRC_UNEXPECTED_RESPONSE, intCount);
            TEST_ASSERT_EQUAL_INT32(SDL_PASS, status);
        }
    }

    /* Get Pending interrupt count */
    if (status == SDL_PASS)
    {
        status = SDL_TOG_getIntrPending(instance, &pendInts );
        TEST_ASSERT_EQUAL_INT32(SDL_PASS, status);
    }

    /* Clear Pending interrupt */
    if (status == SDL_PASS)
    {
        status = SDL_TOG_clrIntrPending(instance, SDL_TOG_INTRSRC_TRANSACTION_TIMEOUT);
        TEST_ASSERT_EQUAL_INT32(SDL_PASS, status);
    }

    if (status == SDL_PASS)
    {
        status = SDL_TOG_clrIntrPending(instance, SDL_TOG_INTRSRC_UNEXPECTED_RESPONSE);
        TEST_ASSERT_EQUAL_INT32(SDL_PASS, status);
    }
    if (status == SDL_PASS)
    {
        TOG_TestHandleArray[instanceIndex].doneFlag = true;
        /* Call SDL API to configure back Timeout Gasket */
        cfg.timeoutVal = TOG_TestHandleArray[instanceIndex].timeoutValue;
        status = SDL_TOG_init(instance, &cfg);
        TEST_ASSERT_EQUAL_INT32(SDL_PASS, status);

        /* Stop the Timeout Gasket */
        SDL_TOG_stop( instance );

        /* Reset the Timeout gasket */
        SDL_TOG_reset( instance );
    }
    return;
}


int32_t TOG_runTest(uint32_t instanceIndex)
{
    int32_t testResult = 0;
    int32_t status;
    uint64_t startTime , testStartTime,  testEndTime, endTime;
    uint64_t prepTime, diffTime, restoreTime;
    volatile uint32_t timeoutCount = 0;
    SDL_TOG_Inst instance;
    SDL_TOG_config cfg;
    instance = instanceIndex;
    cfg.cfgCtrl = SDL_TOG_CFG_TIMEOUT;

    UART_printf("\n Starting TOG test on %s, index %d...",
                TOG_TestHandleArray[instanceIndex].instanceName,
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
    TOG_TestHandleArray[instanceIndex].doneFlag = false;

    /* Get start time of test */
    startTime = TimerP_getTimeInUsecs();

    /*-- Step 1: Configure ESM handler --*/
#if defined (SOC_J721E)
        status = sdlAppEsmConfig(MCU_ESM_BASE,
                                 TOG_TestHandleArray[instanceIndex].ESMEventNumber,
                                 ESM_INTR_PRIORITY_LEVEL_HIGH,
                                 TOG_TestHandleArray[instanceIndex].handler,
                                 instanceIndex);
#elif (SOC_J7200)
    if(instanceIndex < SDL_TOG_INSTANCE_TIMEOUT0)
    {
            status = sdlAppEsmConfig(MCU_ESM_BASE,
                             TOG_TestHandleArray[instanceIndex].ESMEventNumber,
                             ESM_INTR_PRIORITY_LEVEL_HIGH,
                             TOG_TestHandleArray[instanceIndex].handler,
                             instanceIndex);
    }
    else{
            status = sdlAppEsmConfig(MAIN_ESM_BASE,
                                 TOG_TestHandleArray[instanceIndex].ESMEventNumber,
                                 ESM_INTR_PRIORITY_LEVEL_HIGH,
                                 TOG_TestHandleArray[instanceIndex].handler,
                                 instanceIndex);
    }
#elif (SOC_J721S2)
        if(instanceIndex < SDL_TOG_MAIN_INSTANCE)
    {
            status = sdlAppEsmConfig(MAIN_ESM_BASE,
                             TOG_TestHandleArray[instanceIndex].ESMEventNumber,
                             ESM_INTR_PRIORITY_LEVEL_HIGH,
                             TOG_TestHandleArray[instanceIndex].handler,
                             instanceIndex);
    }
    else{
            status = sdlAppEsmConfig(MCU_ESM_BASE,
                                 TOG_TestHandleArray[instanceIndex].ESMEventNumber,
                                 ESM_INTR_PRIORITY_LEVEL_HIGH,
                                 TOG_TestHandleArray[instanceIndex].handler,
                                 instanceIndex);
    }
#elif (SOC_J784S4)
        if(instanceIndex < SDL_TOG_MAIN_INSTANCE)
    {
            status = sdlAppEsmConfig(MAIN_ESM_BASE,
                             TOG_TestHandleArray[instanceIndex].ESMEventNumber,
                             ESM_INTR_PRIORITY_LEVEL_HIGH,
                             TOG_TestHandleArray[instanceIndex].handler,
                             instanceIndex);
    }
    else{
            status = sdlAppEsmConfig(MCU_ESM_BASE,
                                 TOG_TestHandleArray[instanceIndex].ESMEventNumber,
                                 ESM_INTR_PRIORITY_LEVEL_HIGH,
                                 TOG_TestHandleArray[instanceIndex].handler,
                                 instanceIndex);
    }	
#endif

    if (status != SDL_PASS)
    {
        UART_printf("   sdlAppEsmConfig Failed \n");
        testResult = -1;
    }

    if (testResult == 0)
    {
        /* Enable interrupts */
        status = SDL_TOG_setIntrEnable(instance, SDL_TOG_INTRSRC_ALL, true);
        if (status != SDL_PASS)
        {
            UART_printf("   SDL_TOG_setIntrEnable Failed\n");
            testResult = -1;
        }
    }

    /** Step 2: Configure and enable Timeout Gasket */
    if (testResult == 0)
    {
        /* Call SDL API to configure Timeout Gasket */
        cfg.timeoutVal = TOG_TestHandleArray[instanceIndex].timeoutValue;
        status = SDL_TOG_init(instance, &cfg);
        if (status != SDL_PASS)
        {
            UART_printf("   SDL_TOG_init timeout Failed \n");
            testResult = -1;
        }
    }

    if (testResult == 0)
    {
        cfg.timeoutVal = TOG_TestHandleArray[instanceIndex].timeoutValue;
        status = SDL_TOG_verifyConfig(instance, &cfg);
        if (status != SDL_PASS)
        {
            UART_printf("   SDL_TOG_VerifyTimeoutVal Failed \n");
            testResult = -1;
        }

    }

    if (testResult == 0)
    {
        /* Call SDL API to enable Timeout Gasket */
        status = SDL_TOG_start(instance);
        if (status != SDL_PASS)
        {
            UART_printf("   SDL_TOG_start Failed \n");
            testResult = -1;
        }
    }
    /* Get start time of test */
    testStartTime = TimerP_getTimeInUsecs();

    /* Step 3: Inject timeout error */
    if (testResult == 0)
    {
        TOG_TestHandleArray[instanceIndex].injectFunction(instanceIndex);
    }

    /**--- Step 3: Wait for TOG Interrupt ---*/
    if (testResult == 0)
    {
        /* Timeout if exceeds time */
        while ((!TOG_TestHandleArray[instanceIndex].doneFlag)
               && (timeoutCount++ < TOG_MAX_TEST_TIMEOUT_VALUE))
        {
            /* Interrupt handler available */
            if (TOG_TestHandleArray[instanceIndex].handler == NULL)
            {
                /* Use Polling */
                TOG_eventHandler(instanceIndex);
            }
        }

        if (!(TOG_TestHandleArray[instanceIndex].doneFlag))
        {
            SDL_TOG_stop( instance );
            UART_printf("   TOG test timed out \n");
            testResult = -1;
        }

/* flushMode feature is not supported for J721e per this errata ID i2122 */
#if defined (FLUSH_MODE_SUPPORTED)
        SDL_TOG_setFlushMode(instanceIndex,false);
#endif
        /* reset Done flag so we can run again */
        TOG_TestHandleArray[instanceIndex].doneFlag = false;
    }

    /* Get end time of test */
    testEndTime = TimerP_getTimeInUsecs();

    /**--- Step 4: Disable ESM ---*/
    if (testResult == 0)
    {
#if defined (SOC_J721E)
        status = sdlAppEsmDisable(MCU_ESM_BASE,
                            TOG_TestHandleArray[instanceIndex].ESMEventNumber);
#elif (SOC_J7200)
        if(instanceIndex < SDL_TOG_INSTANCE_TIMEOUT0)
        {
        status = sdlAppEsmDisable(MCU_ESM_BASE,
                            TOG_TestHandleArray[instanceIndex].ESMEventNumber);
        }
        else
        {
            status = sdlAppEsmDisable(MAIN_ESM_BASE,
                                      TOG_TestHandleArray[instanceIndex].ESMEventNumber);
        }
#elif (SOC_J721S2)
        if(instanceIndex < SDL_TOG_MAIN_INSTANCE)
        {
            status = sdlAppEsmDisable(MAIN_ESM_BASE,
                                TOG_TestHandleArray[instanceIndex].ESMEventNumber);
        }
        else
        {
            status = sdlAppEsmDisable(MCU_ESM_BASE,
                                      TOG_TestHandleArray[instanceIndex].ESMEventNumber);
        }
#elif (SOC_J784S4)
        if(instanceIndex < SDL_TOG_MAIN_INSTANCE)
        {
            status = sdlAppEsmDisable(MAIN_ESM_BASE,
                                TOG_TestHandleArray[instanceIndex].ESMEventNumber);
        }
        else
        {
            status = sdlAppEsmDisable(MCU_ESM_BASE,
                                      TOG_TestHandleArray[instanceIndex].ESMEventNumber);
        }		
#endif
        if (status != SDL_PASS)
        {
            UART_printf("   sdlAppEsmDisable Failed \n");
            testResult = -1;
        }
    }

    /* Here TOG test is complete , get end time of test */
    endTime = TimerP_getTimeInUsecs();

    prepTime = testStartTime - startTime;
    diffTime = testEndTime - testStartTime;
    restoreTime = endTime - testEndTime;
    UART_printf("\n  Delta TOG prep time in micro secs %d \n", (uint32_t)prepTime );
    UART_printf("  Delta TOG execution time in micro secs %d \n", (uint32_t)diffTime );
    UART_printf("  Delta TOG restore time in micro secs %d \n", (uint32_t)restoreTime );

    UART_printf("  TOG complete for %s \n",
                TOG_TestHandleArray[instanceIndex].instanceName);

    return (testResult);
}


void TOG_datAbortExceptionHandler(void *param)
{
    /* This is a fake exception so return */
}

/* TOG prepare for test */
int32_t TOG_PrepareForTest(void)
{
    int32_t sdlResult = SDL_PASS;

    sdlResult = sdlAppEsmSetup();

    if (sdlResult == SDL_PASS)
    {
        /* Register exception handler */
        /* This is needed to handle data abort that can happen in the process of injecting the error */
        Intc_RegisterExptnHandlers(&TOG_R5ExptnHandlers);
    }

    return sdlResult;

}

/* TOG Functional test */
int32_t TOG_funcTest(void)
{
    int32_t    testResult = 0;
    int i;

    testResult = TOG_PrepareForTest();
    if (testResult != 0)
    {
        UART_printf("   TOG_PrepareForTest failed \n");
    }

    if (testResult == 0)
    {
        for ( i = 0; i <= TOG_MAX_INSTANCE; i++)
        {   
            #if defined (SOC_J784S4)
			Osal_delay(200);
			#endif
            testResult = TOG_runTest(i);
            if (testResult != 0)
            {
                UART_printf("   TOG functional test failed %d\n", i);
                break;
            }
        }
    }

    return (testResult);
}

/* Nothing past this point */
