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
 *  \file     main.h
 *
 *  \brief    This file contains J7_clock Function test code declarations
 *
 **/

/*===========================================================================*/
/*                         Include files                                     */
/*===========================================================================*/
/*===========================================================================*/


#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <src/sdl/sdl_types.h>
#include <src/sdl/hw_types.h>
#include <ti/drv/uart/UART.h>
#include <ti/drv/uart/UART_stdio.h>
#include <ti/board/board.h>
#include <ti/osal/osal.h>
#include <src/ip/sdl_ip_esm.h>
#include <src/sdl/sdl_esm.h>

/* Interrupt Registrations */
#ifdef UNITY_INCLUDE_CONFIG_H
#include <ti/build/unit-test/Unity/src/unity.h>
#include <ti/build/unit-test/config/unity_config.h>
#endif

/* ========================================================================== */
/*                     Dependant macros in SDL_j7_clock_test.c                  */
/* ========================================================================== */

#define WKUP_ESM_INSTANCE   SDL_WKUP_ESM0_CFG_BASE
#define MCU_ESM_INSTANCE    SDL_MCU_ESM0_CFG_BASE
#define WKUP_ESM_ERR_SIG    SDLR_WKUP_ESM0_ESM_LVL_EVENT_GLUELOGIC_HFOSC0_CLKLOSS_GLUE_REF_CLK_LOSS_DETECT_OUT_0

#if defined (SOC_J721E)
#define MCU_ESM_ERR_SIG     SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_PLLFRAC2_SSMOD0_LOCKLOSS_IPCFG_0
#endif

#if defined (SOC_J7200)
#define	MCU_ESM_ERR_SIG	 	SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_PLLFRACF_SSMOD0_LOCKLOSS_IPCFG_0
#endif

#if defined (SOC_J721S2)
#define	MCU_ESM_ERR_SIG	 	SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_PLLFRACF2_SSMOD0_LOCKLOSS_IPCFG_0
#endif

#if defined (SOC_J784S4)
#define	MCU_ESM_ERR_SIG	 	SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_PLLFRACF2_SSMOD0_LOCKLOSS_IPCFG_0
#endif

/*
* To change the ESM Instance as desired, use the following definitions:
* - ESM_INTID
* - ESM_ERR_SIG
*/
#define ESM_ERR_SIG_WKUP    WKUP_ESM_ERR_SIG
#define ESM_ERR_SIG_MCU     MCU_ESM_ERR_SIG

/* Print buffer character limit for prints- UART or CCS Console */
#define APP_PRINT_BUFFER_SIZE                       (4000U)

/*===========================================================================*/
/*                         Declarations                                      */
/*===========================================================================*/

/* Define the test interface */
typedef struct sdlJ7_clockTest_s
{
    int32_t  (*testFunction)(void);   /* The code that runs the test */
    char      *name;                  /* The test name */
    int32_t    testStatus;            /* Test Status */
} sdlJ7_clockTest_s;


/*===========================================================================*/
/*                         Macros                                            */
/*===========================================================================*/

#define SDL_APP_TEST_NOT_RUN        (-(int32_t) (2))
#define SDL_APP_TEST_FAILED         (-(int32_t) (1))
#define SDL_APP_TEST_PASS           ( (int32_t) (0))

/*===========================================================================*/
/*                         External function declarations                    */
/*===========================================================================*/



int32_t SDL_ESM_applicationCallbackFunction(SDL_ESM_Inst esmInst,
                                            SDL_ESM_IntType esmIntrType,
                                            uint32_t grpChannel,
                                            uint32_t index,
                                            uint32_t intSrc,
                                            void *arg);

extern int32_t SDL_j7_clock_test(void);

/*===========================================================================*/
/*                         Local Function definitions                        */
/*===========================================================================*/

/* Nothing past this point */
