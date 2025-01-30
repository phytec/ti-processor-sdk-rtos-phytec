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
 *  \file     main.c
 *
 *  \brief    This file contains DCC Example test code.
 *
 *  \details  DCC tests
 **/

/*===========================================================================*/
/*                         Include files                                     */
/*===========================================================================*/
#include "main.h"

#include <ti/drv/sciclient/sciclient.h>
#include <test/osal/osal_interface.h>



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
/* None */

/*===========================================================================*/
/*                         Internal function declarations                    */
/*===========================================================================*/

static void sdlApp_print(const char * str);
static int32_t  sdlApp_initBoard(void);
/* Unity functions */
void test_sdl_dcc_baremetal_test_app_runner(void);
void test_sdl_dcc_baremetal_test_app (void);
/*===========================================================================*/
/*                         Global Variables                                  */
/*===========================================================================*/
sdlDccTest_t  sdlDccTestList[] = {
    {SDL_DCC_example, "DCC EXAMPLE TEST" ,     SDL_APP_TEST_NOT_RUN },
    {NULL,             "TERMINATING CONDITION",  SDL_APP_TEST_NOT_RUN }
};

SDL_ESM_config DCC_Test_esmInitConfig_MCU =
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

extern int32_t SDL_ESM_applicationCallbackFunction(SDL_ESM_Inst esmInstType,
                                                   SDL_ESM_IntType esmIntType,
                                                   uint32_t grpChannel,
                                                   uint32_t index,
                                                   uint32_t intSrc,
                                                   void *arg);
static uint32_t arg;
/*===========================================================================*/
/*                   Local Function definitions                              */
/*===========================================================================*/
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

static void sdlApp_print(const char * str)
{
    UART_printf(str);
}

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

void test_sdl_dcc_baremetal_test_app (void)
{
    /* Declarations of variables */
    int32_t    testResult = SDL_APP_TEST_PASS;
    int32_t    i, result;
    void *ptr = (void *)&arg;

    /* Init Board */
    sdlApp_initBoard();

    sdlApp_print("\n DCC Example Test Application\r\n");

    /* Init Osal */
    sdlApp_osalInit();

    /* Initialize MCU DCC module */
    result = SDL_ESM_init(SDL_ESM_INST_MCU_ESM0, &DCC_Test_esmInitConfig_MCU, SDL_ESM_applicationCallbackFunction, ptr);

    if (result != SDL_PASS)
    {
        /* print error and quit */
         UART_printf("DCC_Test_init: Error initializing MCU ESM: result = %d\n", result);
    }
    else
    {
        UART_printf("\nDCC_Test_init: Init MCU ESM complete \n\n");
    }

    for ( i = 0; sdlDccTestList[i].testFunction != NULL; i++)
    {
        testResult = sdlDccTestList[i].testFunction();
        sdlDccTestList[i].testStatus = testResult;
    }

    testResult = SDL_APP_TEST_PASS;
    for ( i = 0; sdlDccTestList[i].testFunction != NULL; i++)
    {
        if (sdlDccTestList[i].testStatus != SDL_APP_TEST_PASS)
        {
            UART_printf("\nTest Name: %s  FAILED \n", sdlDccTestList[i].name);
            testResult = SDL_APP_TEST_FAILED;
            break;
        }
        else
        {
            UART_printf("\nTest Name: %s  PASSED \n", sdlDccTestList[i].name);
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


void test_sdl_dcc_baremetal_test_app_runner(void)
{
    /* @description:Test runner for DCC tests

       @cores: mcu1_0 */

#if defined(UNITY_INCLUDE_CONFIG_H)
    UNITY_BEGIN();
    RUN_TEST (test_sdl_dcc_baremetal_test_app);
    UNITY_END();
    /* Function to print results defined in our unity_config.h file */
    print_unityOutputBuffer_usingUARTstdio();
#else
    test_sdl_dcc_baremetal_test_app();
#endif
    return;
}

int32_t main(void)
{
    test_sdl_dcc_baremetal_test_app_runner();
    /* Stop the test and wait here */
    while (1);
}

/* Nothing past this point */
