/*
 * TOG Usecases
 *
 * Timeout Gasket (TOG) Example Application
 *
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
 */
/**
 *  \file tog_usecase.c
 *
 *  \brief This file triggers input for the Timeout Gasket (TOG) example
 */

#include "tog_main.h"
#include <ti/drv/uart/UART_stdio.h>
#include <ti/drv/sciclient/sciclient.h>

/* ========================================================================== */
/*                                Macros                                      */
/* ========================================================================== */
#if defined (SOC_J721E)
    #include <include/soc/j721e/sdlr_soc_baseaddress.h>
#endif
#if defined (SOC_J7200)
    #include <include/soc/j7200/sdlr_soc_baseaddress.h>
#endif
#if defined (SOC_J721S2)
    #include <include/soc/j721s2/sdlr_soc_baseaddress.h>
#endif
#if defined (SOC_J784S4)
    #include <include/soc/j784s4/sdlr_soc_baseaddress.h>
#endif
#define ESM_CFG_BASE                (SDL_ESM0_CFG_BASE)
#define TOG_INSTANCE_MCU_INFRA0_CFG         0

__attribute((section(".text:TOG_test"))) int32_t tog_minTimeout(uint32_t instanceIndex);
__attribute((section(".text:TOG_test"))) void TOG_injectMCUINFRATimeoutError(uint32_t instanceIndex);
__attribute((section(".text:TOG_test"))) void TOG_injectESMError(uint32_t instanceIndex);

__attribute((section(".text:ESMApp_Handlers"))) int32_t SDL_ESM_applicationCallbackFunction(SDL_ESM_Inst esmInst,
                                                            SDL_ESM_IntType esmIntrType, uint32_t grpChannel,
                                                            uint32_t index, uint32_t intSrc, void *arg);

__attribute((section(".text:TOG_test"))) void TOG_eventHandler(uint32_t instanceIndex);


#define TOG_TEST_TIMEOUTVAL 0x10000U
static uint32_t arg;

volatile bool handlerFlag __attribute__((section(".data:TOG_test"))) = false;

SDL_ESM_config TOG_Test_esmInitConfig_MCU =
{
    .esmErrorConfig = {0u, 3u}, /* Self test error config */
    .enableBitmap = {0xffffffffu, 0xff0fffffu, 0x7fffffffu, 0x00000007u,
                },
     /**< All events enable: except timer and self test  events, and Main ESM output */
    /* Temporarily disabling vim compare error as well*/
    .priorityBitmap = {0xffffffffu, 0xff0fffffu, 0x7fffffffu, 0x00000007u,
                        },
    /**< All events high priority: except timer, selftest error events, and Main ESM output */
    .errorpinBitmap = {0xffffffffu, 0xff0fffffu, 0x7fffffffu, 0x00000007u,
                      },
    /**< All events high priority: except timer, selftest error events, and Main ESM output */
};

int32_t SDL_ESM_applicationCallbackFunction(SDL_ESM_Inst esmInst,
                                            SDL_ESM_IntType esmIntrType,
                                            uint32_t grpChannel,
                                            uint32_t index,
                                            uint32_t intSrc,
                                            void *arg)
{
    int32_t retVal = SDL_PASS;

#if defined (SOC_J721E) || defined (SOC_J7200)
    TOG_eventHandler(0);
#elif defined (SOC_J721S2)
    TOG_eventHandler(16);
#elif defined (SOC_J784S4)
        TOG_eventHandler(16);
#endif
    UART_printf("\n  ESM Call back function called : instType 0x%x, intType 0x%x, " \
                "grpChannel 0x%x, index 0x%x, intSrc 0x%x \n",
                esmInst, esmIntrType, grpChannel, index, intSrc);
    UART_printf("  Take action \n");

    /* Any additional customer specific actions can be added here */

    return retVal;
}

void TOG_eventHandler( uint32_t instanceIndex )
{
    int32_t status = SDL_PASS;
    uint32_t pendInts;
    uint32_t intCount;
    SDL_TOG_errInfo errInfo;
    SDL_TOG_config cfg;
    SDL_TOG_Inst instance;
    SDL_TOG_IntrSrc intSrc;
    intSrc = (SDL_TOG_INTRSRC_UNEXPECTED_RESPONSE |
              SDL_TOG_INTRSRC_TRANSACTION_TIMEOUT);

    instance = instanceIndex;
    cfg.cfgCtrl = SDL_TOG_CFG_TIMEOUT;

    UART_printf(" \n  TOG_eventHandler  \n");
    if (intSrc != 0U)
    {
        /* Read error info */
        status = SDL_TOG_getErrInfo(instance, &errInfo);
        TEST_ASSERT_EQUAL_INT32(SDL_PASS, status);
    }

    if (intSrc & SDL_TOG_INTRSRC_TRANSACTION_TIMEOUT)
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

    if (intSrc & SDL_TOG_INTRSRC_UNEXPECTED_RESPONSE)
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
        handlerFlag = true;
        /* Call SDL API to configure back Timeout Gasket */
        cfg.timeoutVal = TOG_TEST_TIMEOUTVAL;
        status = SDL_TOG_init(instance, &cfg);
        TEST_ASSERT_EQUAL_INT32(SDL_PASS, status);

        /* Stop the Timeout Gasket */
        SDL_TOG_stop( instance );

        /* Reset the Timeout gasket */
        SDL_TOG_reset( instance );
    }
    return;
}

#if defined (SOC_J721E) || defined (SOC_J7200)

void TOG_injectMCUINFRATimeoutError(uint32_t instanceIndex)
{
    SDL_TOG_Inst instance;
    SDL_TOG_config cfg;
    instance = instanceIndex;
    cfg.cfgCtrl = SDL_TOG_CFG_TIMEOUT;
    int32_t status;
    volatile esmRevisionId_t esmRevisionId;

    /* Injecting error can result in a Data abort, so disable temporarily */
    disableABORT();

    /* Call SDL API to set smaller timeout to trigger error */
    cfg.timeoutVal = 1u;
    status = SDL_TOG_init(instance, &cfg);
    if (status != SDL_PASS)
    {
        UART_printf("   Inject SDL_TOG_init TimeoutVal Failed \n");
        /* Assert */
    }

    /* Access Main ESM to trigger transaction through the Gasket */
    (void)SDL_ESM_getRevisionId(ESM_CFG_BASE, (esmRevisionId_t *)&esmRevisionId);

    /* Call SDL API to set configure back to original timeout value */
    cfg.timeoutVal = TOG_TEST_TIMEOUTVAL;
    status = SDL_TOG_init(instance, &cfg);
    if (status != SDL_PASS)
    {
        UART_printf("\n  Configure back SDL_TOG_init TimeoutVal Failed \n");
        /* Assert */
    }

    /* Enable back Abort */
    enableABORT();
}
#endif

#if defined (SOC_J721S2) || defined (SOC_J784S4)

#define END_POINT_ACCESS 0x00A90000U

void TOG_injectESMError(uint32_t instanceIndex)
{
    SDL_TOG_Inst instance;
    SDL_TOG_config cfg;
    instance = instanceIndex;
    cfg.cfgCtrl = SDL_TOG_CFG_TIMEOUT;
    int32_t status;
    /* Injecting error can result in a Data abort, so disable temporarily */
    disableABORT();

    /* Call SDL API to set smaller timeout to trigger error */
    cfg.timeoutVal = 1u;
    status = SDL_TOG_init(instance, &cfg);
    if (status != SDL_PASS)
    {
        UART_printf("   Inject SDL_TOG_init TimeoutVal Failed \n");
        /* Assert */
    }
	
    SDL_REG32_RD(END_POINT_ACCESS);
	
    /* Call SDL API to set configure back to original timeout value */
    cfg.timeoutVal = TOG_TEST_TIMEOUTVAL;
    status = SDL_TOG_init(instance, &cfg);
    if (status != SDL_PASS)
    {
        UART_printf("   Configure back SDL_TOG_init TimeoutVal Failed \n");
        /* Assert */
    }

    /* Enable back Abort */
    enableABORT();
}

#endif

int32_t tog_minTimeout(uint32_t instanceIndex)
{
    SDL_TOG_Inst instance;
    SDL_TOG_config cfg;
    void *ptr = (void *)&arg;
    int32_t status = SDL_PASS;
    int32_t result = 0;
    volatile uint32_t timeoutCount = 0;
    instance = instanceIndex;
    cfg.cfgCtrl = SDL_TOG_CFG_TIMEOUT;

    /* Initialize done flag */
    handlerFlag = false;

    UART_printf("\n Demo for minimum value timeout \n");

    /* Initialize MCU ESM module */
    status = SDL_ESM_init(SDL_ESM_INST_MCU_ESM0, &TOG_Test_esmInitConfig_MCU, SDL_ESM_applicationCallbackFunction, ptr);
    if (status != SDL_PASS) {
        /* print error and quit */
        UART_printf("TOG_App_init: Error initializing MCU ESM: result = %d\n", result);

        result = -1;
    } else {
        UART_printf("\nECC_Test_init: Init MCU ESM complete \n");
    }

    if (result == 0)
    {
        /* Enable interrupts */
        status = SDL_TOG_setIntrEnable(instance, SDL_TOG_INTRSRC_ALL, true);
        if (status != SDL_PASS)
        {
            UART_printf("   SDL_TOG_setIntrEnable Failed \n");
            result = -1;
        } else {
            UART_printf("\nSDL_TOG_setIntrEnable complete \n");
        }

    }

    /** Step 2: Configure and enable Timeout Gasket */
    if (result == 0)
    {
        /* Call SDL API to configure Timeout Gasket */
        cfg.timeoutVal = TOG_TEST_TIMEOUTVAL;
        status = SDL_TOG_init(instance, &cfg);
        if (status != SDL_PASS)
        {
            UART_printf("   SDL_TOG_init timeout Failed \n");
            result = -1;
        } else {
            UART_printf("\nSDL_TOG_init.timeout complete \n");
        }
    }

    if (result == 0)
    {
        /* Call SDL API to enable Timeout Gasket */
        status = SDL_TOG_start(instance);
        if (status != SDL_PASS)
        {
            UART_printf("   SDL_TOG_start Failed \n");
            result = -1;
        } else {
            UART_printf("\nSDL_TOG_start complete \n");
        }
    }

    /* Step 3: Inject timeout error */
    if (result == 0)
    {
#if defined (SOC_J721E) || defined (SOC_J7200)
        TOG_injectMCUINFRATimeoutError(instance);
#elif defined (SOC_J721S2) || defined (SOC_J784S4)
        TOG_injectESMError(instance);
#endif
    }

    /**--- Step 3: Wait for TOG Interrupt ---*/
    if (result == 0)
    {
        /* Timeout if exceeds time */
        while ((!handlerFlag)
               && (timeoutCount < TOG_MAX_TEST_TIMEOUT_VALUE))
        {
            timeoutCount++;
        }

        if (!(handlerFlag))
        {
            SDL_TOG_stop( instance );
            UART_printf("   TOG test timed out \n");
            result = -1;
        } else {
            UART_printf("\nSDL_TOG_stop complete \n");
        }
        /* reset Done flag so we can run again */
        handlerFlag = false;
    }

    return (result);
}


