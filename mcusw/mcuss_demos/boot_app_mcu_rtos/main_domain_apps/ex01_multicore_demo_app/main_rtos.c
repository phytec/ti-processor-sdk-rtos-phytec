/*
 *  Copyright (c) Texas Instruments Incorporated 2020 - 2022
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
 *  \file main_rtos.c
 *
 *  \brief Main file for freeRTOS builds
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <stdio.h>
#include <stdint.h>

#include "ti/osal/osal.h"
#include "ti/osal/TaskP.h"

#if defined (__C7100__)
#include<ti/kernel/freertos/portable/TI_CGT/c7x/Mmu.h>
#endif
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

#ifdef BUILD_C7X_1
#    define  TASK_STACKSIZE  0x8000U
#else
#    define  TASK_STACKSIZE  1024U*10U
#endif

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static void taskFxn(void* a0, void* a1);

/* Prepare core to power down */
void send_core_to_compliance_state(void);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/*
 * In the cfg file of R5F, C66x, default heap is 48K which is not
 * enough for 9 task_stack, so creating task_stack on global.
 * C7x cfg has 256k default heap, so no need to put task_stack on global
 */
#if !defined(BUILD_C7X_1)
uint8_t  g_taskStackBuf[TASK_STACKSIZE];
#else
/* IMPORTANT NOTE: For C7x,
 * - stack size and stack ptr MUST be 8KB aligned
 * - AND min stack size MUST be 16KB
 * - AND stack assigned for task context is "size - 8KB"
*       - 8KB chunk for the stack area is used for interrupt handling in this task context
*/
uint8_t g_taskStackBuf[TASK_STACKSIZE]
__attribute__ ((section(".bss:taskStackSection")))
__attribute__ ((aligned(8192)))
    ;
#endif

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

int main(void)
{
    TaskP_Handle task;
    TaskP_Params taskParams;

    /* Start time stamp */
    mainStartTimeStamp = get_usec_timestamp();

    initSciclient();
    boardInit();

    /* Initialized time stamp */
    mainInitializedTimeStamp = get_usec_timestamp();

    OS_init();

    /* Initialize the task params */
    TaskP_Params_init(&taskParams);
    /* Set the task priority higher than the default priority (1) */
    taskParams.priority = 2;
    taskParams.stack = g_taskStackBuf;
    taskParams.stacksize = sizeof(g_taskStackBuf);
    task = TaskP_create(&taskFxn, &taskParams);
    if(NULL == task)
    {
        OS_stop();
    }
    OS_start();    /* does not return */

    return(0);
}

/*
 * This "Task" writes to the UART or SBL log to indicate the
 * core has started
 */
static void taskFxn(void* a0, void* a1)
{
    volatile int boot_print_delay = BOOT_PRINT_DELAY;

    while (boot_print_delay--);
    UART_printf("%s started running at %d usecs\n\r",
                CORE_NAME, (uint32_t)mainStartTimeStamp);
    UART_printf("%s initialized at %d usecs\n\r",
                CORE_NAME, (uint32_t)mainInitializedTimeStamp);

    UART_printf("%s going to compliance state ... \n\r", CORE_NAME);
    send_core_to_compliance_state();

    return;
}

#if defined(BUILD_MPU) || defined (__C7100__)
extern void Osal_initMmuDefault(void);
void InitMmu(void)
{
    Osal_initMmuDefault();
}
#endif

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
