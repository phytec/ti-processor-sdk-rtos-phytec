/* Copyright (c) 2021 Texas Instruments Incorporated
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
 *  \file     test_main.c
 *
 *  \brief    This file contains POK test code.
 *
 *  \details  POK tests
 **/

/*===========================================================================*/
/*                         Include files                                     */
/*===========================================================================*/
#include "main.h"
#include <osal_interface.h>

#ifdef UNITY_INCLUDE_CONFIG_H
#include <ti/build/unit-test/Unity/src/unity.h>
#include <ti/build/unit-test/config/unity_config.h>
#endif


/*===========================================================================*/
/*                         Declarations                                      */
/*===========================================================================*/
/* None */

/*===========================================================================*/
/*                         Macros                                            */
/*===========================================================================*/
/* define the unlock values */
#define KICK0_UNLOCK_VAL 0x68EF3490
#define KICK1_UNLOCK_VAL 0xD172BC5A

/*===========================================================================*/
/*                         Internal function declarations                    */
/*===========================================================================*/
static int32_t  sdlApp_initBoard(void);
/* Unity functions */
void test_sdl_pok_baremetal_test_app_runner(void);
void test_sdl_pok_baremetal_test_app (void);

/*===========================================================================*/
/*                         Global Variables                                  */
/*===========================================================================*/
sdlPokTest_t  sdlPokTestList[] = {
    {sdlPOK_funcTest,       "POK FUNCTION TEST" ,    SDL_APP_TEST_NOT_RUN },
    {sdlPOKInPor_funcTest,  "POR FUNCTION TEST",     SDL_APP_TEST_NOT_RUN },
    {NULL,                  "TERMINATING CONDITION", SDL_APP_TEST_NOT_RUN }
};

/*===========================================================================*/
/*                   Local Function definitions                              */
/*===========================================================================*/
void sdlApp_print(const char * str) 
{
    UART_printf(str);
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
        sdlApp_print("[Error] Board init failed!!\n");
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

/*===========================================================================*/
/*                         Function definitions                              */
/*===========================================================================*/

void test_sdl_pok_baremetal_test_app (void)
{
    /* Declarations of variables */
    int32_t    testResult = SDL_APP_TEST_PASS;
    int32_t    i;

    /* Init Board */
    sdlApp_initBoard();

    /* Init Osal */
    sdlApp_osalInit();

    sdlApp_print("\n POK Test Application\r\n");

    /* Unlock the MMR in order to access the POK registers */
    *((uint32_t *)(SDL_WKUP_CTRL_MMR0_CFG0_BASE + SDL_WKUP_CTRL_MMR_CFG0_LOCK6_KICK0)) = KICK0_UNLOCK_VAL;
    *((uint32_t *)(SDL_WKUP_CTRL_MMR0_CFG0_BASE + SDL_WKUP_CTRL_MMR_CFG0_LOCK6_KICK1)) = KICK1_UNLOCK_VAL;

    for ( i = 0; sdlPokTestList[i].testFunction != NULL; i++)
    {
        testResult = sdlPokTestList[i].testFunction();
        sdlPokTestList[i].testStatus = testResult;
    }

    testResult = SDL_APP_TEST_PASS;
    for ( i = 0; sdlPokTestList[i].testFunction != NULL; i++)
    {
        if (sdlPokTestList[i].testStatus != SDL_APP_TEST_PASS)
        {
            UART_printf("Test Name: %s  FAILED \n", sdlPokTestList[i].name);
            testResult = SDL_APP_TEST_FAILED;
            break;
        }
        else
        {
            UART_printf("Test Name: %s  PASSED \n", sdlPokTestList[i].name);
        }
    }

    if (testResult == SDL_APP_TEST_PASS)
    {
        UART_printStatus("\n All tests have passed. \n");
    }
    else
    {
        UART_printStatus("\n Few/all tests Failed \n");
    }
#if defined (UNITY_INCLUDE_CONFIG_H)
    TEST_ASSERT_EQUAL_INT32(SDL_APP_TEST_PASS, testResult);
#endif
}

void test_sdl_pok_baremetal_test_app_runner(void)
{
    /* @description:Test runner for POK tests

       @cores: mcu1_0 */

#if defined(UNITY_INCLUDE_CONFIG_H)
    UNITY_BEGIN();
    RUN_TEST (test_sdl_pok_baremetal_test_app);
    UNITY_END();
    /* Function to print results defined in our unity_config.h file */
    print_unityOutputBuffer_usingUARTstdio();
#else
    test_sdl_pok_baremetal_test_app();
#endif
    return;
}

int32_t main(void)
{
    test_sdl_pok_baremetal_test_app_runner();

    /* Stop the test and wait here */
    while (1);
}

/* Nothing past this point */
