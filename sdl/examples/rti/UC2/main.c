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
 *  \brief    This file contains RTI Example test code.
 *
 *  \details  RTI tests
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
static int32_t  sdlApp_initBoard(void);
/* Unity functions */
void test_sdl_rti_baremetal_test_app_runner(void);
void test_sdl_rti_baremetal_test_app (void);
/*===========================================================================*/
/*                         Global Variables                                  */
/*===========================================================================*/
sdlRtiTest_t  sdlRtiTestList[] = {
    {SDL_RTI_exampleTest, "RTI EXAMPLE TEST" ,     SDL_APP_TEST_NOT_RUN },
    {NULL,             "TERMINATING CONDITION",  SDL_APP_TEST_NOT_RUN }
};

#if defined (SOC_J721E)
SDL_ESM_config RTI_Test_esmInitConfig_MCU =
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
#endif

#if defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)   
SDL_ESM_config RTI_Test_esmInitConfig_MCU =
{
    .esmErrorConfig = {0u, 3u}, /* Self test error config */
    .enableBitmap = {0xffffffffu, 0xff0fffffu, 0x7fffffffu, 0x00000300u,
                },
     /**< All events enable: except timer and self test  events, and Main ESM output */
    /* Temporarily disabling vim compare error as well*/
    .priorityBitmap = {0xffffffffu, 0xff0fffffu, 0x7fffffffu, 0x00000300u,
                        },
    /**< All events high priority: except timer, selftest error events, and Main ESM output */
    .errorpinBitmap = {0xffffffffu, 0xff0fffffu, 0x7fffffffu, 0x00000300u,
                      },
    /**< All events high priority: except timer, selftest error events, and Main ESM output */
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

/*===========================================================================*/
/*                         Local Function definitions                        */
/*===========================================================================*/

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

#define RTI_NUM_DEVICES SDL_RTI_MAX_INSTANCE+1

#if defined (SOC_J721E)
uint32_t RTI_devices[RTI_NUM_DEVICES] =
{
    TISCI_DEV_MCU_RTI0,
    TISCI_DEV_MCU_RTI1,
    TISCI_DEV_RTI0,
    TISCI_DEV_RTI1,
    TISCI_DEV_RTI15,
    TISCI_DEV_RTI16,
    TISCI_DEV_RTI24,
    TISCI_DEV_RTI25,
    TISCI_DEV_RTI28,
    TISCI_DEV_RTI29,
    TISCI_DEV_RTI30,
    TISCI_DEV_RTI31
};

static int32_t sdlApp_initRTI(void)
{
    int32_t status = SDL_PASS;
    uint32_t i;

    for (i = 0; i < RTI_NUM_DEVICES; i++)
    {
        /* Power up RTI */
        status = Sciclient_pmSetModuleState(RTI_devices[i],
                                            TISCI_MSG_VALUE_DEVICE_SW_STATE_ON,
                                            TISCI_MSG_FLAG_AOP,
                                            SCICLIENT_SERVICE_WAIT_FOREVER);

        if (status != SDL_PASS)
        {
            UART_printf("   RTI Sciclient_pmSetModuleState 0x%x ...FAILED: retValue %d\n",
                        RTI_devices[i], status);
        }
    }

    return status;
}
#endif

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

void test_sdl_rti_baremetal_test_app (void)
{
    /* Declarations of variables */
    int32_t    testResult = SDL_APP_TEST_PASS;
    int32_t    i, result;
    void *ptr = (void *)&arg;

    /* Init Board */
    sdlApp_initBoard();

    sdlApp_print("\n RTI Example Test Application\r\n");

	#if defined (SOC_J721E)
    /* Init the RTI modules */
    sdlApp_initRTI();
	#endif

    /* Init Osal */
    sdlApp_osalInit();

    /* Initialize MCU RTI module */
    result = SDL_ESM_init(SDL_ESM_INST_MCU_ESM0, &RTI_Test_esmInitConfig_MCU, SDL_ESM_applicationCallbackFunction, ptr);

    if (result != SDL_PASS)
    {
        /* print error and quit */
         UART_printf("RTI_Test_init: Error initializing MCU ESM: result = %d\n", result);
    }
    else
    {
        UART_printf("\nRTI_Test_init: Init MCU ESM complete \n\n");
    }

    for ( i = 0; sdlRtiTestList[i].testFunction != NULL; i++)
    {
        testResult = sdlRtiTestList[i].testFunction();
        sdlRtiTestList[i].testStatus = testResult;
    }

    testResult = SDL_APP_TEST_PASS;
    for ( i = 0; sdlRtiTestList[i].testFunction != NULL; i++)
    {
        if (sdlRtiTestList[i].testStatus != SDL_APP_TEST_PASS)
        {
            UART_printf("Test Name: %s  FAILED \n", sdlRtiTestList[i].name);
            testResult = SDL_APP_TEST_FAILED;
            break;
        }
        else
        {
            UART_printf("Test Name: %s  PASSED \n", sdlRtiTestList[i].name);
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


void test_sdl_rti_baremetal_test_app_runner(void)
{
    /* @description:Test runner for RTI tests

       @cores: mcu1_0 */

#if defined(UNITY_INCLUDE_CONFIG_H)
    UNITY_BEGIN();
    RUN_TEST (test_sdl_rti_baremetal_test_app);
    UNITY_END();
    /* Function to print results defined in our unity_config.h file */
    print_unityOutputBuffer_usingUARTstdio();
#else
    test_sdl_rti_baremetal_test_app();
#endif
    return;
}

int32_t main(void)
{
    test_sdl_rti_baremetal_test_app_runner();
	
	return 0;
}

/* Nothing past this point */
