/*
 *
 * BIST Example
 *
 * PBIST and LBIST Example Application
 *
 *  Copyright (c) 2022 Texas Instruments Incorporated
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
 *  \file main.c
 *
 *  \brief This file demonstrates using the PBIST and LBIST SDL APIs to check the
 *         status of POST and execute the self test diagnostics for a particular
 *         controller. The example can be expanded to include other controllers
 *         as well.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <stdint.h>
#include <string.h>
#include <sdl_types.h>
#include <ti/drv/uart/UART.h>
#include <ti/drv/uart/UART_stdio.h>
#include <ti/board/board.h>
#include <ti/drv/sciclient/sciclient.h>
#include <bist.h>
#include <soc.h>

/* For Timer functions */
#include <ti/osal/osal.h>
#include <osal_interface.h>

#ifdef UNITY_INCLUDE_CONFIG_H
#include <ti/build/unit-test/Unity/src/unity.h>
#include <ti/build/unit-test/config/unity_config.h>
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* Unity functions */
void sdl_ex_bist_example_app_runner(void);
void bist_example_app(void);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* ========================================================================== */
/*                            External Variables                              */
/* ========================================================================== */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

void bist_example_app(void)
{
    bist_TaskFxn(); 
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

/* define the unlock and lock values */
#define KICK0_UNLOCK_VAL 0x68EF3490
#define KICK1_UNLOCK_VAL 0xD172BC5A
#define KICK_LOCK_VAL    0x00000000

static int32_t sdlApp_initBoard(void)
{
    Board_initCfg boardCfg;
    Board_STATUS  boardStatus;
    int32_t       testResult = SDL_PASS;
    uint64_t mcuClkFreq;

    boardCfg = BOARD_INIT_PINMUX_CONFIG |
               BOARD_INIT_UART_STDIO;
    boardStatus = Board_init(boardCfg);
    if (boardStatus != BOARD_SOK)
    {
        testResult = SDL_EFAIL;
        UART_printf("[Error] Board init failed!!\n");
    }

    *((uint32_t *)(SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_LOCK3_KICK0)) = KICK0_UNLOCK_VAL;
    *((uint32_t *)(SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_LOCK3_KICK1)) = KICK1_UNLOCK_VAL;

    *((uint32_t *)(SDL_MCU_CTRL_MMR0_CFG0_BASE + SDL_MCU_CTRL_MMR_CFG0_LOCK3_KICK0)) = KICK0_UNLOCK_VAL;
    *((uint32_t *)(SDL_MCU_CTRL_MMR0_CFG0_BASE + SDL_MCU_CTRL_MMR_CFG0_LOCK3_KICK1)) = KICK1_UNLOCK_VAL;

    *((uint32_t *)(SDL_WKUP_CTRL_MMR0_CFG0_BASE + SDL_WKUP_CTRL_MMR_CFG0_LOCK3_KICK0)) = KICK0_UNLOCK_VAL;
    *((uint32_t *)(SDL_WKUP_CTRL_MMR0_CFG0_BASE + SDL_WKUP_CTRL_MMR_CFG0_LOCK3_KICK1)) = KICK1_UNLOCK_VAL;

    /* Following code is needed to set Osal timing */
#if defined (SOC_J721E) || defined (SOC_J7200)|| defined (SOC_J721S2) || defined (SOC_J784S4)
    /* Get the clock frequency */
    testResult = Sciclient_pmGetModuleClkFreq(TISCI_DEV_MCU_R5FSS0_CORE0,
                                              TISCI_DEV_MCU_R5FSS0_CORE0_CPU_CLK,
                                              &mcuClkFreq,
                                              SCICLIENT_SERVICE_WAIT_FOREVER);
#elif
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

void sdl_ex_bist_example_app_runner(void)
{
#ifdef UNITY_INCLUDE_CONFIG_H
    UNITY_BEGIN();
    RUN_TEST(bist_example_app);
    UNITY_END();
    /* Function to print results defined in our unity_config.h file */
    print_unityOutputBuffer_usingUARTstdio();
#else
    bist_example_app();
#endif

}

int32_t main(void)
{
    /* Declaration of variables */
    int32_t  testResult;

    /* Init Board */
    testResult = sdlApp_initBoard();

    if (testResult == SDL_PASS)
    {
        sdlApp_osalInit();

        UART_printf("\nBIST Example Application\r\n");
        sdl_ex_bist_example_app_runner();

    }
    else
    {
        UART_printStatus("\r\nBoard Init failed. Exiting the app.\r\n");
    }

    return 0;
}

