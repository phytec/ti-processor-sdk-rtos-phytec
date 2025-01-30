/*
 * VTM Test Application
 *
 * Voltage and Thermal Monitor (VTM) Test Application
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
 *  \file vtm_main.c
 *
 *  \brief This file contains functions that provide main function
 *         for the Voltage and Thermal Monitor (VTM) application.
 */

/* For Timer functions */
#include <ti/osal/osal.h>

#include "event_trig.h"
#include "sdl_esm.h"
#include <sdl_types.h>
#ifdef UNITY_INCLUDE_CONFIG_H
#include <ti/build/unit-test/Unity/src/unity.h>
#include <ti/build/unit-test/config/unity_config.h>
#include <osal_interface.h>
#include <stdint.h>
#include <string.h>
#include <sdl_types.h>
#include <ti/drv/uart/UART.h>
#include <ti/drv/uart/UART_stdio.h>
#include <ti/board/board.h>
#include <ti/drv/sciclient/sciclient.h>
#endif

#define USE_CASES_RUN         (5)
#define USE_CASES             (5)
#define START_USE_CASE        (0)
#define MAX_ESM_EVENTS_LOGGED (20)
#define SDLR_WKUP_ESM0_ESM_LVL_EVENT_WKUP_VTM0_THERM_LVL_GT_TH1_INTR_0                             (8U)
#define SDLR_WKUP_ESM0_ESM_LVL_EVENT_WKUP_VTM0_THERM_LVL_LT_TH0_INTR_0                             (9U)
#define APP_ARG (1)

/* #define DEBUG */

static uint32_t totalEventsLogged   = 0;
static uint32_t totalHiEventsLogged = 0;
static uint32_t totalLoEventsLogged = 0;
static int32_t  thresholdsReset     = 0;
int32_t apparg = APP_ARG;
/* ESM event log entry */
typedef struct
{
    SDL_ESM_Inst esmInstance;
    SDL_ESM_IntType      intType;
    uint32_t             grpChannel;
    uint32_t             index;
    uint32_t             intSrc;
    uint8_t              useCaseNum;
} ESM_Example_log_entry_t;
static int32_t deactivateTrigger(SDL_ESM_Inst esmInstType,
                                 SDL_ESM_IntType esmIntType,
                                 uint32_t intEsmSrc);

static ESM_Example_log_entry_t esmEventLog[MAX_ESM_EVENTS_LOGGED];


/* State variable for each test case indicating input event trigger
 * has been completed */
volatile uint32_t esmEventInputTrig[USE_CASES] = {TEST_CASE_STATUS_NOT_RUN,
                                                  TEST_CASE_STATUS_NOT_RUN,
                                                  TEST_CASE_STATUS_NOT_RUN,
                                                  TEST_CASE_STATUS_NOT_RUN,
                                                  TEST_CASE_STATUS_NOT_RUN};

/* State variable for each test case indicating the ISR for the test case
 * has been completed */
volatile uint32_t esmOutputResult[USE_CASES] = {TEST_CASE_STATUS_NOT_RUN,
                                                TEST_CASE_STATUS_NOT_RUN,
                                                TEST_CASE_STATUS_NOT_RUN,
                                                TEST_CASE_STATUS_NOT_RUN,
                                                TEST_CASE_STATUS_NOT_RUN};

/* State variable for each test case indicating the pin clearing has been
 * completed by pin clearing ISR (applicable only to test cases only
 * with high priority interrupts routed to the pin directly) */
volatile uint32_t esmPinClearResult[USE_CASES] = {TEST_CASE_STATUS_NOT_RUN,
                                                  TEST_CASE_STATUS_NOT_RUN,
                                                  TEST_CASE_STATUS_NOT_RUN,
                                                  TEST_CASE_STATUS_NOT_RUN,
                                                  TEST_CASE_STATUS_NOT_RUN};

volatile uint8_t currTestCase = START_USE_CASE;

static const char *printTestCaseStepResult(uint32_t result);
void test_sdl_vtm_test_app_runner(void);
void VTM_test_printSummary(void);
int32_t VTM_ESM_init (void);

/* Initialization structure for WKUP ESM instance */
static SDL_ESM_config VTM_esmInitConfig_WKUP =
{
    .esmErrorConfig = {0u, 0u}, /* Self test error config */
    .enableBitmap = {0xffffffffu, 0x00180003u, 0x00000000u, 0x00000000u,
                 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                },
     /**< All events enable: except clkstop events for unused clocks */
    .priorityBitmap = {0xfffffcffu, 0x00180003u, 0x00000000u, 0x00000000u,
                         0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                         0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                         0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                         0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                         0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                         0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                         0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                        },
    /**< All events high priority: except clkstop events for unused clocks,
     *   and make VTM WKUP_VTM0_THERM_LVL_GT_TH1_INTR_0 and
     *   WKUP_VTM0_THERM_LVL_LT_TH0_INTR_0 events low priority */
    .errorpinBitmap = {0xfffffcffu, 0x00180003u, 0x00000000u, 0x00000000u,
                       0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                       0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                       0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                       0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                       0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                       0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                       0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                      },
    /**< All events high priority: except clkstop for unused clocks
     *   and make VTM WKUP_VTM0_THERM_LVL_GT_TH1_INTR_0 and
     *   WKUP_VTM0_THERM_LVL_LT_TH0_INTR_0 events not output to pin */
};
static const char *printEsmIntType(SDL_ESM_IntType esmIntType)
{
    char *pStr;

    switch(esmIntType)
    {
        case SDL_ESM_INT_TYPE_HI:
            pStr = "High Priority ESM event";
            break;
        case SDL_ESM_INT_TYPE_LO:
            pStr = "Low Priority ESM event";
            break;
        case SDL_ESM_INT_TYPE_CFG:
            pStr = "Config ESM event";
            break;
        default:
            pStr = NULL;
            break;
    }

    return pStr;
}




int32_t SDL_ESM_applicationCallbackFunction(SDL_ESM_Inst esmInstType,
                                         SDL_ESM_IntType esmIntType,
                                         uint32_t grpChannel,
                                         uint32_t index,
                                         uint32_t intSrc,
                                         void *arg)
{
    /* Log the event */
    esmEventLog[totalEventsLogged].useCaseNum  = currTestCase;
    esmEventLog[totalEventsLogged].esmInstance = esmInstType;
    esmEventLog[totalEventsLogged].intType     = esmIntType;
    esmEventLog[totalEventsLogged].grpChannel  = grpChannel;
    esmEventLog[totalEventsLogged].index       = index;
    esmEventLog[totalEventsLogged].intSrc      = intSrc;

    totalEventsLogged++;
    if (esmIntType == SDL_ESM_INT_TYPE_HI) {
        totalHiEventsLogged++;
    } else if (esmIntType == SDL_ESM_INT_TYPE_LO) {
        totalLoEventsLogged++;
    }

    /* Any additional customer-specific actions to address ESM event
     * can be added here */

    deactivateTrigger(esmInstType, esmIntType, intSrc);

    /* Print information to screen */
    UART_printf("\n  ESM Call back function called : instType 0x%x, intType 0x%x, " \
                "grpChannel 0x%x, index 0x%x, intSrc 0x%x \n",
                esmInstType, esmIntType, grpChannel, index, intSrc);
    UART_printf("  Take action \n");

    UART_printf("  ESM instance #%d, ESM interrupt type = %s\n",
                esmInstType, printEsmIntType(esmIntType));

    esmOutputResult[currTestCase]= TEST_CASE_STATUS_COMPLETED_SUCCESS;

    return SDL_PASS;
}

/*********************************************************************
* @fn      VTM_ESM_init
*
* @brief   Initializes Board, Timers, and ESM module
*
* @param   None
*
* @return    0 : Success; < 0 for failures
*/
int32_t VTM_ESM_init (void)
{
    int32_t retValue=0;
    int32_t result;

    /* Initialize WKUP ESM module */
    result = SDL_ESM_init(SDL_ESM_INST_WKUP_ESM0, &VTM_esmInitConfig_WKUP, SDL_ESM_applicationCallbackFunction, &apparg);
    if (result != SDL_PASS) {
        /* print error and quit */
        UART_printf("VTM_ESM_init: Error initializing WKUP ESM: result = %d\n", result);

        retValue = -1;
    } else {
        UART_printf("\nVTM_ESM_init: Init WKUP ESM complete \n");
    }
    return retValue;
}


static int32_t deactivateTrigger(SDL_ESM_Inst esmInstType,
                                 SDL_ESM_IntType esmIntType,
                                 uint32_t intEsmSrc)
{
    int32_t retVal = 0;

    if ((esmInstType == SDL_ESM_INST_WKUP_ESM0) && (esmIntType == SDL_ESM_INT_TYPE_LO)) {
        /* TC-1: Low Priority interrupt on WKUP ESM -
         * VTM greater than THR1 */
        if (intEsmSrc ==
            SDLR_WKUP_ESM0_ESM_LVL_EVENT_WKUP_VTM0_THERM_LVL_GT_TH1_INTR_0)
        {
            if (currTestCase == 0)
            {
                if (thresholdsReset == 0)
                {
                    /* Simulate thresholds as if temperature is going to be reduced
                     * below lt_Thr0 */
                    vtm_setNormalThresholds();

                    thresholdsReset = 1;
                }
                vtm_GtThr1CrossedUpdateInt();
            } else if ((currTestCase == 1) || (currTestCase == 2))
            {
                if (thresholdsReset == 0)
                {
                    /* Simulate thresholds as if temperature continues to increase
                     * toward gt_Thr2 */
                    vtm_setThresholdsForCriticalTrigger();

                    thresholdsReset = 1;
                }
                vtm_GtThr1CrossedUpdateInt();
            }
        } else if (intEsmSrc ==
                   SDLR_WKUP_ESM0_ESM_LVL_EVENT_WKUP_VTM0_THERM_LVL_LT_TH0_INTR_0)
        {
            vtm_LtThr0CrossedUpdateInt();

            thresholdsReset = 0;
            esmOutputResult[currTestCase] = TEST_CASE_STATUS_COMPLETED_SUCCESS;
            if (currTestCase == 1) {
                /* At end of this test case, clear the Pin that was left on
                 * throughout the test case*/
                Osal_delay(4);
                vtm_startPinClearTimer(true);
            }
        }
    } else if ((esmInstType == SDL_ESM_INST_WKUP_ESM0) &&
               (esmIntType == SDL_ESM_INT_TYPE_HI)) {
        if ((currTestCase == 1) || (currTestCase == 2)) {
            /* TC-2: High Priority interrupt on WKUP ESM -
             * VTM greater than THR2, no clearing of
             * MCU_SAFETY_ERRORn pin */
            /* TC-3: High Priority interrupt on WKUP ESM -
             * VTM greater than THR2 with clearing
             * of MCU_SAFETY_ERRORn pin */
            if (thresholdsReset == 1)
            {
                /* Simulate thresholds as if temperature is going to be reduced
                 * below lt_Thr0 */
                vtm_setNormalThresholds();

                thresholdsReset = 2;
            }
            if (currTestCase == 2) {
                vtm_startPinClearTimer(false);
            }
            vtm_GtThr2CrossedUpdateInt();
        } else {
            retVal = -1;
        }
    } else {
        UART_printf("ERR: Unexpected ESM Instance %d and ESM Interrupt Type %d \n",
                    esmInstType, esmIntType);
        retVal = -1;
    }

   return (retVal); 
}



/*********************************************************************
* @fn      VTM_test_printSummary
*
* @brief   Print summary of all the test cases run
*
* @param   None
*
* @return  None
*/
void VTM_test_printSummary(void)
{
    int32_t i;

    UART_printf("\n\n");
    UART_printf("ESM Example Application summary\n");
    UART_printf("-------------------------------\n");
    UART_printf("Completed %d Test Cases\n", currTestCase);
    UART_printf("Received %d High Priority Interrupts\n", totalHiEventsLogged);
    UART_printf("Received %d Low Priority Interrupts\n", totalLoEventsLogged);

    UART_printf("\nTest Case Event Log\n");
    UART_printf("------------------\n");
    for (i = 0; i < totalEventsLogged; i++) {
        UART_printf("\nTest Case %d: ESM Call back function called : grpChannel 0x%x, " \
                    "index 0x%x, intSrc 0x%x \n",
                    esmEventLog[i].useCaseNum,
                    esmEventLog[i].grpChannel,
                    esmEventLog[i].index,
                    esmEventLog[i].intSrc);
        UART_printf("  ESM instance #%d, ESM interrupt type = %s\n",
                    esmEventLog[i].esmInstance,
                    printEsmIntType(esmEventLog[i].intType));

    }
}



/*****************************************************************************
 * This is the main function for the Voltage and Thermal Monitor (VTM) example
 * application.
 * It runs through 5 test cases to demonstrate usage of the ESM modules
 * for receiving errors and controlling the error pin.
 */
void vtm_test_app(void)
{
    int32_t testErrCount = 0;
    int32_t retValue;
    uint8_t i;

    /* Initialize the ESM instances and handlers */
    retValue = VTM_ESM_init();

    if (retValue < 0) {
        /* print and exit */
        UART_printf("\nERR: VTM_ESM_init failed");
        testErrCount++;
    }

    UART_printf("\nESM example init complete");

    /* Initialize the Timers for all the Test Cases */
    retValue = vtm_timerInit();

    if (retValue < 0) {
        /* print and exit */
        UART_printf("\nERR: Timer initialization failed");
        testErrCount++;
    }
    /* Trigger each Test Case */
    for (i = START_USE_CASE; i < 3; i++) {
        retValue = vtm_runTestCaseTrigger(i);

        if (retValue != 0) {
            UART_printf("\nERR: Test Case Trigger for Test Case %d failed \n",
                        retValue);
            break;
        }

        while((esmEventInputTrig[i] == TEST_CASE_STATUS_NOT_RUN) ||
              (esmOutputResult[i] == TEST_CASE_STATUS_NOT_RUN) ||
              (esmPinClearResult[i] == TEST_CASE_STATUS_NOT_RUN))
        {
#ifdef DEBUG
            UART_printf("InputTrig = %d, OutputResult = %d, ClearResult = %d\n",
                        esmEventInputTrig[i],
                        esmOutputResult[i],
                        esmPinClearResult[i]);
#endif
        }

        UART_printf("\nTest Case %d completed: Input Event Trigger = %s, \n" \
                    "                       Event Handler Complete = %s, \n" \
                    "                       MCU_SAFETY_ERRORn Pin Clear = %s\n",
                    i,
                    printTestCaseStepResult(esmEventInputTrig[i]),
                    printTestCaseStepResult(esmOutputResult[i]),
                    printTestCaseStepResult(esmPinClearResult[i]));
        currTestCase++;
    }

    /* Check results of all the tests */
    for (i = 0; i < 3; i++) {
        if ((esmEventInputTrig[i] != TEST_CASE_STATUS_COMPLETED_SUCCESS) ||
            (esmOutputResult[i] != TEST_CASE_STATUS_COMPLETED_SUCCESS) ||
            (esmPinClearResult[i] != TEST_CASE_STATUS_COMPLETED_SUCCESS)) {
            testErrCount++;
        }
    }

    retValue = vtm_timersDelete();

    if (retValue != 0) {
        UART_printf("\nFailed to delete used timers\n");
    }

    /* Print results and logs of the Test Cases */
    VTM_test_printSummary();
    UART_printf("\nESM Example Application: Complete");

    if (testErrCount == 0)
    {
        UART_printf("\n All tests have passed. \n");
    }
    else
    {
        UART_printf("\n ESM Example app failed. \n");
    }
    return;
}

static const char *printTestCaseStepResult(uint32_t result)
{
    char *pStr;

    switch(result)
    {
        case TEST_CASE_STATUS_NOT_RUN:
            pStr = "Step Not yet run";
            break;
        case TEST_CASE_STATUS_COMPLETED_SUCCESS:
            pStr = "Step completed successfully";
            break;
        case TEST_CASE_STATUS_COMPLETED_FAILURE:
            pStr = "Step completed with failure";
            break;
        default:
            pStr = NULL;
            break;
    }

    return pStr;
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

static int32_t sdlApp_initBoard(void)
{
    Board_initCfg boardCfg;
    Board_STATUS  boardStatus;
    int32_t       testResult = SDL_PASS;
    uint64_t mcuClkFreq=0;

    boardCfg = BOARD_INIT_PINMUX_CONFIG |
               BOARD_INIT_UART_STDIO;
    boardStatus = Board_init(boardCfg);
    if (boardStatus != BOARD_SOK)
    {
        testResult = SDL_EFAIL;
        UART_printf("[Error] Board init failed!!\n");
    }

    /* Following code is needed to set Osal timing */
#if (defined SOC_J721E) || (defined SOC_J7200) || (defined SOC_J721S2) || (defined SOC_J784S4)
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

    return (testResult);
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

void test_sdl_vtm_test_app_runner(void)
{
#if defined(UNITY_INCLUDE_CONFIG_H)
    UNITY_BEGIN();
    RUN_TEST (vtm_test_app);
    UNITY_END();
    /* Function to print results defined in our unity_config.h file */
    print_unityOutputBuffer_usingUARTstdio();
#else
    vtm_test_app();
#endif
    return;
}

int32_t main(void)
{
    int32_t  testResult;
    /* Init Board */
    testResult = sdlApp_initBoard();

    if (testResult == SDL_PASS)
    {
        sdlApp_osalInit();

        UART_printf("\nVTM Test Application\r\n");
        (void)test_sdl_vtm_test_app_runner();
    }
    else
    {
        UART_printStatus("\r\nBoard Init failed. Exiting the app.\r\n");
    }
    return 0;
}

/* Nothing past this point */
