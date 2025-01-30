/*
 *    Copyright (c) 2021 Texas Instruments Incorporated
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
 *  \file     sdl_exception_posTest.c
 *
 *  \brief    This file contains EXCEPTION UNIT test code.
 *
 *  \details  EXCEPTION tests
 **/

/*===========================================================================*/
/*                         Include files                                     */
/*===========================================================================*/

#include "exception_test_main.h"
#include <ti/drv/uart/UART.h>
#include <ti/drv/uart/UART_stdio.h>
#include <test/r5f_startup/interrupt.h>

/*===========================================================================*/
/*                   Local Function definitions                              */
/*===========================================================================*/

void SDL_ECC_callBackFunction (uint32_t errorSrc, uint32_t errorAddr,
                                      uint32_t ramId, uint64_t bitErrorOffset,
                                      uint32_t bitErrorGroup)
{

}

/* This is the list of exception handle and the parameters */
const SDL_R5ExptnHandlers ECC_Test_R5ExptnHandlers =
{
    .udefExptnHandler = &SDL_EXCEPTION_undefInstructionExptnHandler,
    .swiExptnHandler = &SDL_EXCEPTION_swIntrExptnHandler,
    .pabtExptnHandler = &SDL_EXCEPTION_prefetchAbortExptnHandler,
    .dabtExptnHandler = &SDL_EXCEPTION_dataAbortExptnHandler,
    .irqExptnHandler = &SDL_EXCEPTION_irqExptnHandler,
    .fiqExptnHandler = &SDL_EXCEPTION_fiqExptnHandler,
    .udefExptnHandlerArgs = ((void *)0u),
    .swiExptnHandlerArgs = ((void *)0u),
    .pabtExptnHandlerArgs = ((void *)0u),
    .dabtExptnHandlerArgs = ((void *)0u),
    .irqExptnHandlerArgs = ((void *)0u),
};

void ECC_Test_undefInstructionExptnCallback(void)
{
    UART_printf("\n Undefined Instruction exception");
}

void ECC_Test_swIntrExptnCallback(void)
{
    UART_printf("\n Software interrupt exception");
}

void ECC_Test_prefetchAbortExptnCallback(void)
{
    UART_printf("\n Prefetch Abort exception");
}
void ECC_Test_dataAbortExptnCallback(void)
{
    UART_printf("\n Data Abort exception");
}
void ECC_Test_irqExptnCallback(void)
{
    UART_printf("\n Irq exception");
}

void ECC_Test_fiqExptnCallback(void)
{
    UART_printf("\n Fiq exception");
}

void ECC_Test_exceptionInit(void)
{

    SDL_EXCEPTION_CallbackFunctions_t exceptionCallbackFunctions =
            {
             .udefExptnCallback = ECC_Test_undefInstructionExptnCallback,
             .swiExptnCallback = ECC_Test_swIntrExptnCallback,
             .pabtExptnCallback = ECC_Test_prefetchAbortExptnCallback,
             .dabtExptnCallback = ECC_Test_dataAbortExptnCallback,
             .irqExptnCallback = ECC_Test_irqExptnCallback,
             .fiqExptnCallback = ECC_Test_fiqExptnCallback,
            };

    /* Initialize SDL exception handler */
    SDL_EXCEPTION_init(&exceptionCallbackFunctions);
    /* Register SDL exception handler */
    Intc_RegisterExptnHandlers(&ECC_Test_R5ExptnHandlers);

    return;
}


/*===========================================================================*/
/*                           UNIT test codes			                     */
/*===========================================================================*/

int32_t sdl_exception_posTest(void)
{
    int32_t   testStatus = SDL_APP_TEST_PASS;
	
    SDL_EXCEPTION_CallbackFunctions_t exceptionCallbackFunctions =
            {
             .udefExptnCallback = NULL,
             .swiExptnCallback = NULL,
             .pabtExptnCallback = NULL,
             .dabtExptnCallback = NULL,
             .irqExptnCallback = NULL,
             .fiqExptnCallback = NULL,
            };
	
/*******************************************************************************************
*     Call SDL API SDL_EXCEPTION_init
*******************************************************************************************/

    if (testStatus == SDL_APP_TEST_PASS)
    {
		SDL_EXCEPTION_init((void *)0u);
		SDL_EXCEPTION_init(&exceptionCallbackFunctions);
    }
		SDL_EXCEPTION_registerECCHandler((void *)0u);
		SDL_EXCEPTION_undefInstructionExptnHandler((void *)0u);
		SDL_EXCEPTION_swIntrExptnHandler((void *)0u);
		SDL_EXCEPTION_prefetchAbortExptnHandler((void *)0u);
		SDL_EXCEPTION_dataAbortExptnHandler((void *)0u);
		SDL_EXCEPTION_irqExptnHandler((void *)0u);
		SDL_EXCEPTION_fiqExptnHandler((void *)0u);
		 
/*******************************************************************************************
*     Call SDL API SDL_EXCEPTION_registerECCHandler
*******************************************************************************************/

	ECC_Test_exceptionInit();

    if (testStatus == SDL_APP_TEST_PASS)
	{
		SDL_EXCEPTION_registerECCHandler((void *)0u);
        SDL_EXCEPTION_registerECCHandler(&SDL_ECC_callBackFunction);        
     }

/*******************************************************************************************
*     Call SDL API SDL_EXCEPTION_undefInstructionExptnHandler
*******************************************************************************************/

     if (testStatus == SDL_APP_TEST_PASS)
     {
		SDL_EXCEPTION_undefInstructionExptnHandler((void *)0u);
     }

/*******************************************************************************************
*     Call SDL API SDL_EXCEPTION_swIntrExptnHandler
*******************************************************************************************/

    if (testStatus == SDL_APP_TEST_PASS)
    {
		SDL_EXCEPTION_swIntrExptnHandler((void *)0u);
    }

/*******************************************************************************************
*     Call SDL API SDL_EXCEPTION_prefetchAbortExptnHandler
*******************************************************************************************/

    if (testStatus == SDL_APP_TEST_PASS)
    {
		SDL_EXCEPTION_prefetchAbortExptnHandler((void *)0u);
    }

/*******************************************************************************************
*     Call SDL API SDL_EXCEPTION_dataAbortExptnHandler
*******************************************************************************************/

    if (testStatus == SDL_APP_TEST_PASS)
    {
		SDL_EXCEPTION_dataAbortExptnHandler((void *)0u);
    }

/*******************************************************************************************
*     Call SDL API SDL_EXCEPTION_irqExptnHandler
*******************************************************************************************/

    if (testStatus == SDL_APP_TEST_PASS)
    {
		SDL_EXCEPTION_irqExptnHandler((void *)0u);
    }

/*******************************************************************************************
*     Call SDL API SDL_EXCEPTION_fiqExptnHandler
*******************************************************************************************/

    if (testStatus == SDL_APP_TEST_PASS)
    {
		SDL_EXCEPTION_fiqExptnHandler((void *)0u);
    }

return (testStatus);
}
