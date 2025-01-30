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
 *  \file     tog_test_main.c
 *
 *  \brief    This file contains TOG test code for R5 core.
 *
 *  \details  TOG unit tests
 **/

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "tog_test_main.h"
#include <ti/drv/sciclient/sciclient.h>
#include <ti/osal/osal.h>
#include <ti/osal/TimerP.h>

#ifdef UNITY_INCLUDE_CONFIG_H
#include <test/unity/src/unity.h>
#include <test/unity/config/unity_config.h>
#endif

/* #define DEBUG_TEST_CPU_FREQUENCY */

/* ========================================================================== */
/*                                Macros                                      */
/* ========================================================================== */



/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/* ========================================================================== */
/*                 Internal Function Definitions                              */
/* ========================================================================== */
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

#if defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)

#if defined (SOC_J7200)

#define AUX_NUM_DEVICES 2

uint32_t aux_devices[AUX_NUM_DEVICES] =
{
    TISCI_DEV_R5FSS0_CORE0,
    TISCI_DEV_R5FSS0_CORE1
};
#endif
#if defined (SOC_J721S2)

#define AUX_NUM_DEVICES 17

uint32_t aux_devices[AUX_NUM_DEVICES] =
{
	TISCI_DEV_FFI_MAIN_AC_CBASS_VD,
	TISCI_DEV_FFI_MAIN_AC_QM_CBASS_VD,
	TISCI_DEV_FFI_MAIN_RC_CBASS_VD,
	TISCI_DEV_FFI_MAIN_INFRA_CBASS_VD,
	TISCI_DEV_VPAC0,
	TISCI_DEV_WKUP_GPIO1,
	TISCI_DEV_PCIE1,
	TISCI_DEV_RTI15,
	TISCI_DEV_MCU_TIMER0,
	TISCI_DEV_NAVSS0_PVU_0,
	TISCI_DEV_NAVSS0_PVU_1,
	TISCI_DEV_CODEC0,
	TISCI_DEV_TIMER0,
	TISCI_DEV_MCU_FSS0,
	TISCI_DEV_MCU_FSS0_OSPI_1,
    TISCI_DEV_R5FSS0_CORE0,
    TISCI_DEV_R5FSS0_CORE1
};
#endif

#if defined (SOC_J784S4)

#define AUX_NUM_DEVICES 17

uint32_t aux_devices[AUX_NUM_DEVICES] =
{
    TISCI_DEV_NAVSS0_PVU_0,
	TISCI_DEV_FFI_MAIN_AC_CBASS_VD,
	TISCI_DEV_FFI_MAIN_AC_QM_CBASS_VD,
	TISCI_DEV_FFI_MAIN_RC_CBASS_VD,
	TISCI_DEV_FFI_MAIN_INFRA_CBASS_VD,
	TISCI_DEV_VPAC0,
	TISCI_DEV_WKUP_GPIO1,
	TISCI_DEV_PCIE1,
	TISCI_DEV_RTI15,
	TISCI_DEV_MCU_TIMER0,
	TISCI_DEV_NAVSS0_PVU_0,
	TISCI_DEV_NAVSS0_PVU_1,
	TISCI_DEV_CODEC0,
	TISCI_DEV_TIMER0,
	TISCI_DEV_MCU_FSS0,
	TISCI_DEV_MCU_FSS0_OSPI_1,
	TISCI_DEV_MCU_FSS0
};
#endif
static int32_t sdlApp_initDevices(void)
{
    int32_t status = SDL_PASS;
    uint32_t i;

    for (i = 0; i < AUX_NUM_DEVICES; i++)
    {
        /* Power up RTI */
        status = Sciclient_pmSetModuleState(aux_devices[i],
                                            TISCI_MSG_VALUE_DEVICE_SW_STATE_ON,
                                            TISCI_MSG_FLAG_AOP,
                                            SCICLIENT_SERVICE_WAIT_FOREVER);

        if (status != SDL_PASS)
        {
            UART_printf("   Sciclient_pmSetModuleState 0x%x ...FAILED: retValue %d\n",
                        aux_devices[i], status);
        }
    }
    return status;
}
#endif

static
int32_t TOG_appInitBoard(void)
{
    Board_initCfg     boardCfg;
    Board_STATUS      boardStatus;
    int32_t           testResult = SDL_PASS;
    uint64_t          mcuClkFreq;
#ifdef DEBUG_TEST_CPU_FREQUENCY
    volatile uint64_t prevTimeStamp, curTimeStamp, timeStampDiff = 0;
    int i;
#endif

    boardCfg = BOARD_INIT_PINMUX_CONFIG |
               BOARD_INIT_UART_STDIO;
    boardStatus = Board_init(boardCfg);
    if (boardStatus != BOARD_SOK)
    {
        testResult = SDL_EFAIL;
        UART_printf("[Error] Board init failed!!\n");
    }

    /* Following code is needed to set Osal timing */
#if defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
    /* Get the clock frequency */
    testResult = Sciclient_pmGetModuleClkFreq(TISCI_DEV_MCU_R5FSS0_CORE0,
                                              TISCI_DEV_MCU_R5FSS0_CORE0_CPU_CLK,
                                              &mcuClkFreq,
                                              SCICLIENT_SERVICE_WAIT_FOREVER);
#else
#error "SOC NOT supported please check"
#endif
    if (testResult == 0)
    {
        Osal_HwAttrs  hwAttrs;
        uint32_t      ctrlBitmap;

        testResult = Osal_getHwAttrs(&hwAttrs);
        if (testResult == 0)
        {
            /*
             * Change the timer input clock frequency configuration
               based on R5 CPU clock configured
             */
            hwAttrs.cpuFreqKHz = (int32_t)(mcuClkFreq/1000U);
            ctrlBitmap         = OSAL_HWATTR_SET_CPU_FREQ;
            testResult = Osal_setHwAttrs(ctrlBitmap, &hwAttrs);
        }
    }
    UART_printf("mcuClkFreq %d\n", (uint32_t)mcuClkFreq);

#ifdef DEBUG_TEST_CPU_FREQUENCY
    /* Print 10 dots, with 10 seconds elapsing between each dot */
    for (i = 0; i < 10; i++)
    {
        prevTimeStamp = TimerP_getTimeInUsecs();

        do
        {
            curTimeStamp = TimerP_getTimeInUsecs();
            timeStampDiff = curTimeStamp - prevTimeStamp;
        } while(timeStampDiff < 10000000U);
        UART_printf(".");
    }
#endif
    return (testResult);
}

static
int32_t TOG_appTest(uint32_t testId)
{
    int32_t    testResult;

    switch (testId)
    {
        case TOG_FUNC_TEST_ID:
            testResult = TOG_funcTest();
            UART_printf("\n TOG Functionality Test");
            if (testResult == SDL_PASS)
            {
                UART_printf(" Functionality test Passed.\r\n");
            }
            else
            {
                UART_printf(" Functionality test Failed.\r\n");
            }
            break;

        case TOG_API_TEST_ID:
            testResult = TOG_apiTest();
            UART_printf("\n TOG API Module Test");
            if (testResult == SDL_PASS)
            {
                UART_printf(" API test Passed.\r\n");
            }
            else
            {
                UART_printf(" API test Failed.\r\n");
            }
            break;

        case TOG_ERROR_TEST_ID:
            testResult = TOG_errTest();
            UART_printf("\n TOG Error Module Test");
            if (testResult == SDL_PASS)
            {
                UART_printf(" Error test Passed.\r\n");
            }
            else
            {
                UART_printf(" Error test Failed.\r\n");
            }
            break;

        default:
            UART_printf("\n [Error] Invalid TOG test ID.\r\n");
            testResult = SDL_EFAIL;
            break;
    }

    return (testResult);
}

void test_sdl_tog_test_app(void)
{
    /* @description: Run tog tests

    @cores: mcu1_0 */
    uint32_t testId;
    int32_t  testResult;

    for (testId = ((uint32_t)(0U)); testId < TOG_TOTAL_NUM_TESTS; testId++)
    {
        testResult = TOG_appTest(testId);
        if (testResult != SDL_PASS)
        {
            break;
        }
    }

    if (testResult == SDL_PASS)
    {
        UART_printStatus("\r\nAll tests have passed. \r\n");
#ifdef UNITY_INCLUDE_CONFIG_H
        TEST_PASS();
#endif
    }
    else
    {
        UART_printStatus("\r\nSome tests have failed. \r\n");
#ifdef UNITY_INCLUDE_CONFIG_H
        TEST_FAIL();
#endif
    }
}

void test_sdl_tog_test_app_runner(void)
{
    /* @description: Test runner for TOG tests

       @cores: mcu1_0 */

#ifdef UNITY_INCLUDE_CONFIG_H
    UNITY_BEGIN();
    RUN_TEST(test_sdl_tog_test_app);
    UNITY_END();
    /* Function to print results defined in our unity_config.h file */
    print_unityOutputBuffer_usingUARTstdio();
#else
    test_sdl_tog_test_app();
#endif
}

int32_t main(void)
{
    /* Declaration of variables */
    int32_t  testResult;

    /* Init Board */
    testResult = TOG_appInitBoard();
#if defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
    /* Init the modules */
    sdlApp_initDevices();
#endif    
    if (testResult == SDL_PASS)
    {
        UART_printf("\nTOG Test Applications\r\n");
        test_sdl_tog_test_app_runner();
    }

    return (0);
}

/* Nothing past this point */

