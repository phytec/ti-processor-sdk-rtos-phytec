/*
 *  Copyright (c) Texas Instruments Incorporated 2022
 *  All rights reserved.
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
 *  \file main_baremetal.c
 *
 *  \brief Main file for baremetal builds
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <stdio.h>
#include <stdint.h>

#include <ti/drv/uart/UART.h>
#include <ti/drv/uart/UART_stdio.h>

#include <ti/drv/sciclient/sciclient.h>

#include <ti/board/board.h>

#include "uart_utils.h"
#include "main_domain_utils.h"
#include "mcu_timer_read_multicore.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* None */
#define GTC_COUNTER (*((uint64_t *)(CSL_GTC0_GTC_CFG1_BASE + 0x8U)))

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* Prepare core to power down */
void send_core_to_compliance_state(void);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

volatile uint64_t mainStartTimeStamp;
volatile uint64_t mainInitializedTimeStamp;

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

void initSciclient()
{
    Sciclient_ConfigPrms_t        config;

    /* Now reinitialize it as default parameter */
    Sciclient_configPrmsInit(&config);

    Sciclient_init(&config);

    return;
}

void boardInit()
{
#if defined(UART_ENABLED)
    Board_initParams_t initParams;
    UART_Params        params;

    Board_getInitParams(&initParams);
    /* On Jacinto 7, Main Domain apps use the Main Domain UART instance
     * #UART_WRITE_PORT, which is Port 1 by default */
    initParams.uartInst      = UART_WRITE_PORT;
    initParams.uartSocDomain = BOARD_SOC_DOMAIN_MAIN;

    Board_setInitParams(&initParams);

    /* Initialize UART_printf to use UART_WRITE_PORT */
    UART_Params_init(&params);
    params.parityType = UART_PAR_NONE;
    UART_stdioInit2(UART_WRITE_PORT, &params);
#endif

    return;
}

int32_t App_setGTCClk(uint32_t moduleId,
                      uint32_t clkId,
                      uint64_t clkRateHz)
{
    int32_t retVal;
    uint64_t currClkFreqHz;

    retVal = Sciclient_pmGetModuleClkFreq(moduleId,
                                          clkId,
                                          &currClkFreqHz,
                                          SCICLIENT_SERVICE_WAIT_FOREVER);
    if ((retVal == CSL_PASS) &&
        (currClkFreqHz != clkRateHz))
    {
        retVal = Sciclient_pmSetModuleClkFreq(moduleId,
                                              clkId,
                                              clkRateHz,
                                              TISCI_MSG_FLAG_CLOCK_ALLOW_FREQ_CHANGE,
                                              SCICLIENT_SERVICE_WAIT_FOREVER);
    }
    return (retVal);
}

int main(void)
{
    /* Start time stamp */
#if defined(BUILD_MPU)   
    int32_t status;
    status = App_setGTCClk(TISCI_DEV_GTC0,
                           TISCI_DEV_GTC0_GTC_CLK,
                           1000000U);
    
    /* Used to enable the GTC timer */
    HW_WR_REG32(CSL_GTC0_GTC_CFG1_BASE + 0x0U, 0x1);
    
    if (CSL_PASS != status)
    {
        printf("[Error] Configure GTC Timer failed!!\n");
    }
    mainStartTimeStamp = GTC_COUNTER;
#else
    mainStartTimeStamp = get_usec_timestamp();
#endif
    initSciclient();
    boardInit();

    /* Initialized time stamp */
#if defined(BUILD_MPU)   
    mainInitializedTimeStamp = GTC_COUNTER;
#else
    mainInitializedTimeStamp = get_usec_timestamp();
#endif

    volatile int boot_print_delay = BOOT_PRINT_DELAY;

    while (boot_print_delay--);
    UART_printf("%s started running at %d usecs\n\r",
                CORE_NAME, (uint32_t)mainStartTimeStamp);
    UART_printf("%s initialized at %d usecs\n\r",
                CORE_NAME, (uint32_t)mainInitializedTimeStamp);

    UART_printf("%s going to compliance state ... \n\r", CORE_NAME);
    send_core_to_compliance_state();
    
    return(0);
}

void send_core_to_compliance_state(void)
{
#ifdef BUILD_MCU2_0
    __asm(" wfi");
#endif

#ifdef BUILD_MCU2_1
    __asm(" wfi");
#endif

#ifdef BUILD_MCU3_0
    __asm(" wfi");
#endif

#ifdef BUILD_MCU3_1
    __asm(" wfi");
#endif

#ifdef BUILD_C66X_1
    __asm(" IDLE ");
#endif

#ifdef BUILD_C66X_2
    __asm(" IDLE ");
#endif

#ifdef BUILD_C7X_1
    __asm(" IDLE ");
#endif

#ifdef BUILD_MPU1_0
    __asm volatile
    (
     " dsb sy\n"
     " isb\n"
     " wfi\n"
    );
#endif

#ifdef BUILD_MPU1_1
    __asm volatile
    (
     " dsb sy\n"
     " isb\n"
     " wfi\n"
    );
#endif
}
