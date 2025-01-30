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
 *  \file     main.c
 *
 *  \brief    This file contains POK example code.
 *
 *  \details  POK example main
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
    {sdlPOK_func,        "POK EXAMPLE UC-1" ,         SDL_APP_TEST_NOT_RUN },
    {sdlPOKInPor_func,   "POR EXAMPLE UC-2" ,         SDL_APP_TEST_NOT_RUN },
    {NULL,               "TERMINATING CONDITION",     SDL_APP_TEST_NOT_RUN }
};

#if defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
SDL_ESM_config POK_Test_esmInitConfig_WKUP =
{
    .esmErrorConfig = {0u, 8u}, /* Self test error config */
    .enableBitmap = {0xffffffffu, 0x00180003u, 0xffffffffu, 0x0000000fu,
                },
     /**< All events enable: except clkstop events for unused clocks */
    .priorityBitmap = {0xffffffffu, 0x00180003u, 0xffffffffu, 0x0000000fu,
                        },
    /**< All events high priority: except clkstop events for unused clocks */
    .errorpinBitmap = {0xffffffffu, 0x00180003u, 0xffffffffu, 0x0000000fu,
                      },
    /**< All events high priority: except clkstop for unused clocks
     *   and selftest error events */
};
#elif defined (SOC_J721E)
SDL_ESM_config POK_Test_esmInitConfig_WKUP =
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
#endif

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
    int32_t    sdlRet;
    void *ptr = (void *)&arg;
    /* Init Board */
    sdlApp_initBoard();

    /* Init Osal */
    sdlApp_osalInit();

    sdlApp_print("\n POK Test Application\r\n");


    /* Unlock the MMR in order to access the POK registers */
    *((uint32_t *)(SDL_WKUP_CTRL_MMR0_CFG0_BASE + SDL_WKUP_CTRL_MMR_CFG0_LOCK6_KICK0)) = KICK0_UNLOCK_VAL;
    *((uint32_t *)(SDL_WKUP_CTRL_MMR0_CFG0_BASE + SDL_WKUP_CTRL_MMR_CFG0_LOCK6_KICK1)) = KICK1_UNLOCK_VAL;

    /* ESM Setup for POK tests */
    /* Initialize WKUP ESM module */
    sdlRet = SDL_ESM_init(SDL_ESM_INST_WKUP_ESM0, &POK_Test_esmInitConfig_WKUP, SDL_ESM_applicationCallbackFunction,ptr);
    if (sdlRet != SDL_PASS) {
        /* print error and quit */
        sdlApp_print("sdlEsmSetupForPOK init: Error initializing WKUP ESM: sdlRet = SDL_EFAIL \n");
         sdlRet = -1;
    } else {
         sdlApp_print("\nsdlEsmSetupForPOK init: Init WKUP ESM complete \n");
    }

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
