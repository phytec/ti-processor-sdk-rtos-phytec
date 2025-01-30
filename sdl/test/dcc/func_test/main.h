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
 *  \brief    This file contains DCC Function test code declarations
 *
 **/

/*===========================================================================*/
/*                         Include files                                     */
/*===========================================================================*/
/*===========================================================================*/
#include <sdl_dcc.h>
#include <sdl_esm.h>
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


#if !defined(SDL_DCC_TEST_H)
#define SDL_DCC_TEST_H

/* ========================================================================== */
/*                     Dependant macros in sdl_dcc_funcTest.c                  */
/* ========================================================================== */

#define APP_DCC_STR                     "SDL DCC FUNCTION TEST"
/**< Example Common display string */
#define APP_DCC_MODULE_INST             (SDL_DCC0_BASE)
/**< Instance of DCC. While changing the instance, ensure update clock sources*/
#define APP_DCC_MODE                    (SDL_DCC_MODE_CONTINUOUS)
/**< One Shot mode, Stop counting when Counter 1, reaches 0. */
#define APP_DCC_SRC0_MAX_VAL            (0xFFFFFU)
/**< Maximum value that can be held in the COUNT0 register (ref clock) */
#define APP_DCC_SRC0_VALID_MAX_VAL      (0x0FFFFU)
/**< Maximum value that can be held in the VALID0 register (ref clock) */
#define APP_DCC_SRC1_MAX_VAL            (0xFFFFFU)
/**< Maximum value that can be held in the COUNT1 register (test clock) */

/* Defines that control the clock inputs to DCC and allowed variance */
#define APP_DCC_REF_CLOCK_SRC_0         (SDL_DCC_CLK0_SRC_CLOCK0_1)
#define APP_DCC_TEST_CLOCK_SRC_1        (SDL_DCC_CLK1_SRC_CLOCKSRC3)

#define APP_DCC_TEST_CLOCK_SRC_1_DRIFT  (5U)
/**< Allowed drift in percentage (+/-) */

#if defined (SOC_J721E) || defined (SOC_J7200)
#define APP_DCC_REF_CLOCK_FREQ_IN_KHZ   (19200U)
/**< Clock source for Counter 0, 19.2 MHz for J721e/J7200 */
#define APP_DCC_TEST_CLOCK_FREQ_IN_KHZ  (333334U)
/**< Expected test clock frequency in KHz */
/**< Clock source for Counter 1, 333 MHz for DRA80x/AM65x */
#define APP_DCC_TEST_CLOCK_SRC_1_HIGHER (DCC_DCCCLKSRC1_CLKSRC_2)
/**< Clock source for Counter 1, expected to be higher than
        APP_DCC_TEST_CLOCK_SRC_1, in this Test to simulate an error*/
#elif defined (SOC_AM64X)
#define APP_DCC_REF_CLOCK_FREQ_IN_KHZ   (25000U)
/**< Clock source for Counter 0, 25 MHz for AM64x */
#define APP_DCC_TEST_CLOCK_FREQ_IN_KHZ  (200000U)
/**< Expected test clock frequency in KHz */
/**< Clock source for Counter 1, 200 MHz for AM64x */
#define APP_DCC_TEST_CLOCK_SRC_1_HIGHER (DCC_DCCCLKSRC1_CLKSRC_2)
/**< Clock source for Counter 1, expected to be higher than
        APP_DCC_TEST_CLOCK_SRC_1, in this Test to simulate an error*/
#else
#define APP_DCC_REF_CLOCK_FREQ_IN_KHZ   (25000U)
/**< Clock source for Counter 0, 25 MHz for DRA80x/AM65x */
#define APP_DCC_TEST_CLOCK_FREQ_IN_KHZ  (60000U)
/**< Expected test clock frequency in KHz */
/**< Clock source for Counter 1, 60 MHz for DRA80x/AM65x */
#define APP_DCC_TEST_CLOCK_SRC_1_HIGHER (DCC_DCCCLKSRC1_CLKSRC_1)
/**< Clock source for Counter 1, expected to be higher than
        APP_DCC_TEST_CLOCK_SRC_1, in this Test to simulate an error.
        80 MHz for DRA80x/AM65x */
#endif

/*===========================================================================*/
/*                         Declarations                                      */
/*===========================================================================*/

/* Define the test interface */
typedef struct sdlDccTest_s
{
    int32_t  (*testFunction)(void);   /* The code that runs the test */
    char      *name;                  /* The test name */
    int32_t    testStatus;            /* Test Status */
} sdlDccTest_t;


/*===========================================================================*/
/*                         Macros                                            */
/*===========================================================================*/

#define SDL_APP_TEST_NOT_RUN        (-(int32_t) (2))
#define SDL_APP_TEST_FAILED         (-(int32_t) (1))
#define SDL_APP_TEST_PASS           ( (int32_t) (0))

#define DCC_NO_INTERRUPT    		(0u)
#define DCC_INTERRUPT				(1u)

/*===========================================================================*/
/*                         External function declarations                    */
/*===========================================================================*/



int32_t SDL_ESM_applicationCallbackFunction(SDL_ESM_Inst esmInst,
                                            SDL_ESM_IntType esmIntrType,
                                            uint32_t grpChannel,
                                            uint32_t index,
                                            uint32_t intSrc,
                                            void *arg);

extern int32_t SDL_DCC_funcTest(void);

extern int32_t SDL_ESM_applicationCallbackFunction(SDL_ESM_Inst esmInst,
                                            SDL_ESM_IntType esmIntrType,
                                            uint32_t grpChannel,
                                            uint32_t index,
                                            uint32_t intSrc,
                                            void *arg);

/*===========================================================================*/
/*                         Local Function definitions                        */
/*===========================================================================*/

#endif /* SDL_DCC_TEST_H */
/* Nothing past this point */
