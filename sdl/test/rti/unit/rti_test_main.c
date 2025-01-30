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
 *  \file     rti_test_main.c
 *
 *  \brief    This file contains RTI UNIT test code.
 *
 *  \details  RTI tests
 **/

/*===========================================================================*/
/*                         Include files                                     */
/*===========================================================================*/
#include <rti_test_main.h>
#include <ti/drv/sciclient/sciclient.h>

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
    {SDL_RTI_posTest,    "RTI POSITIVE TEST" ,        SDL_APP_TEST_NOT_RUN },
    {SDL_RTI_negTest,    "RTI NEGATIVE TEST" ,        SDL_APP_TEST_NOT_RUN },
    {NULL,               "TERMINATING CONDITION",     SDL_APP_TEST_NOT_RUN }
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
#if defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J721S2)
#define RTI_NUM_DEVICES SDL_RTI_MAX_INSTANCE+1
#endif

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
#endif

#if defined (SOC_J7200)
uint32_t RTI_devices[RTI_NUM_DEVICES] =
{
    TISCI_DEV_MCU_RTI0,
    TISCI_DEV_MCU_RTI1,
    TISCI_DEV_RTI0,
    TISCI_DEV_RTI1,
    TISCI_DEV_RTI28,
    TISCI_DEV_RTI29,
};
#endif

#if defined (SOC_J721S2)
uint32_t RTI_devices[RTI_NUM_DEVICES] =
{
    TISCI_DEV_MCU_RTI0,
    TISCI_DEV_MCU_RTI1,
    TISCI_DEV_RTI0,
    TISCI_DEV_RTI1,
    TISCI_DEV_RTI15,
    TISCI_DEV_RTI16,
    TISCI_DEV_RTI17,
    TISCI_DEV_RTI28,
    TISCI_DEV_RTI29,
    TISCI_DEV_RTI30,
    TISCI_DEV_RTI31
};
#endif

#if defined (SOC_J784S4)
#define RTI_NUM_DEVICES 25
uint32_t RTI_devices[RTI_NUM_DEVICES] =
{
    TISCI_DEV_MCU_RTI0,
    TISCI_DEV_MCU_RTI1,
    TISCI_DEV_RTI0,
    TISCI_DEV_RTI1, 
    TISCI_DEV_RTI2, 
    TISCI_DEV_RTI3,
    TISCI_DEV_RTI4, 
    TISCI_DEV_RTI5, 
    TISCI_DEV_RTI6,
    TISCI_DEV_RTI7,
    TISCI_DEV_RTI16,
    TISCI_DEV_RTI17,
    TISCI_DEV_RTI18,
    TISCI_DEV_RTI19,
    TISCI_DEV_RTI15,
    TISCI_DEV_RTI28,
    TISCI_DEV_RTI29,
    TISCI_DEV_RTI30,
    TISCI_DEV_RTI31,
    TISCI_DEV_RTI32,
    TISCI_DEV_RTI33,
	/* Using these below devices to powering on RTI_devices */
    /* C71SS0 Powering ON RTI16*/
    TISCI_DEV_COMPUTE_CLUSTER0_C71SS0,
	/* C71SS0 Powering ON RTI17*/
    TISCI_DEV_COMPUTE_CLUSTER0_C71SS1,
	/* C71SS0 Powering ON RTI18*/
    TISCI_DEV_COMPUTE_CLUSTER0_C71SS2,
	/* C71SS0 Powering ON RTI19*/
    TISCI_DEV_COMPUTE_CLUSTER0_C71SS3
};
#endif

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

/*===========================================================================*/
/*                         Function definitions                              */
/*===========================================================================*/

void test_sdl_rti_baremetal_test_app (void)
{
    /* Declarations of variables */
    int32_t    testResult = SDL_APP_TEST_PASS;
    int32_t    i;

    /* Init Board */
    sdlApp_initBoard();

    sdlApp_print("\n RTI Test Application\r\n");

    /* Init the RTI modules */
    sdlApp_initRTI();

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
