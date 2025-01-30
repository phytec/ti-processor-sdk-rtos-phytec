/*
 * TOG Example
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
 *  \file tog_main.c
 *
 *  \brief This file demonstrates using the Timeout Gasket (TOG)
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <osal_interface.h>
#include <ti/drv/sciclient/sciclient.h>
#include <test/r5f_startup/interrupt.h>
#include <ti/drv/uart/UART_stdio.h>
#include <ti/board/board.h>
#include "tog_main.h"

#ifdef UNITY_INCLUDE_CONFIG_H
#include <test/unity/src/unity.h>
#include <test/unity/config/unity_config.h>
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* Unity functions */
void test_sdl_tog_example_app_runner(void);
void tog_example_app(void);
static int32_t  sdlApp_initBoard(void);
extern int32_t tog_minTimeout(uint32_t instanceIndex);

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
/*                            External Variables                              */
/* ========================================================================== */



/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */


/*****************************************************************************
 * This is the main function for the Timeout Gasket (TOG) example
 * application.
 * It runs through the use case to demonstrate usage of the TOG modules
 */


/* initialize the board for the application */
static int32_t  sdlApp_initBoard(void)
{
    Board_initCfg boardCfg;
    Board_STATUS  boardStatus;

    boardCfg = BOARD_INIT_PINMUX_CONFIG |
               BOARD_INIT_UART_STDIO;
    boardStatus = Board_init(boardCfg);
    if (boardStatus != BOARD_SOK)
    {
        UART_printf("[Error] Board init failed!!\n");
    }
    return (boardStatus);
}

static int32_t sdlApp_osalInit(void)
{
    SDL_ErrType_t ret = SDL_PASS;

    ret = SDL_TEST_osalInit();
    if (ret != SDL_PASS)
    {
        UART_printf("Error: Init Failed\n");
    }

    return ret;
}


void tog_example_app(void)
{
    int32_t    result = 0;

    if (result == 0)
    {
#if defined (SOC_J721E) || defined (SOC_J7200)
        result = tog_minTimeout(0);
#elif defined (SOC_J721S2)
        result = tog_minTimeout(16);	
#elif defined (SOC_J784S4)
        result = tog_minTimeout(16);	
#endif
        if (result != 0)
        {
            UART_printf(" \n  TOG example failed \n");
        }
        else
        {
            UART_printStatus("\n All tests have passed. \n");
        }
    }

    return;
}

#ifdef UNITY_INCLUDE_CONFIG_H
/*
 *  ======== Unity set up and tear down ========
 */
void setUp(void)
{
    /* Do nothing */
}

void tearDown(void)
{
    /* Do nothing */
}
#endif

void test_sdl_tog_example_app_runner(void)
{
#if defined(UNITY_INCLUDE_CONFIG_H)
    UNITY_BEGIN();
    RUN_TEST (tog_example_app);
    UNITY_END();
    /* Function to print results defined in our unity_config.h file */
    print_unityOutputBuffer_usingUARTstdio();
#else
    tog_example_app();
#endif
    return;
}

void TOG_datAbortExceptionHandler(void *param)
{
    /* This is a fake exception so return */
}

int32_t main(void)
{
    /* Init Board */
    sdlApp_initBoard();

    /* Init Osal */
    sdlApp_osalInit();

    /* Register exception handler */
    /* This is needed to handle data abort that can happen in the process of injecting the error */
    Intc_RegisterExptnHandlers(&TOG_R5ExptnHandlers);

    UART_printf("\nTOG Sample Example \r\n");
    test_sdl_tog_example_app_runner();

    return (0);
}
