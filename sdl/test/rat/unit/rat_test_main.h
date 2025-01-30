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
 *  \file     rat_test_main.h
 *
 *  \brief    This file contains RAT UNIT test code declarations.
 *
 **/

/*===========================================================================*/
/*                         Include files                                     */
/*===========================================================================*/
/*===========================================================================*/


#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <src/sdl/sdl_types.h>
#include <src/ip/sdl_ip_rat.h>
#include <sdl_rat.h>
#include <ti/csl/arch/csl_arch.h>
#include <src/sdl/hw_types.h>
#include <ti/drv/uart/UART.h>
#include <ti/drv/uart/UART_stdio.h>
#include <ti/board/board.h>
#include <test/osal/osal_interface.h>

#if defined(SOC_J721E)
#include <include/soc/j721e/sdlr_soc_mcu_r5_baseaddress.h>
#endif

#if defined (SOC_J7200)
#include <include/soc/j7200/sdlr_mcu_r5fss0_baseaddress.h>
#endif

#if defined (SOC_J721S2)
#include <include/soc/j721s2/sdlr_mcu_r5fss0_baseaddress.h>
#endif

#if defined (SOC_J784S4)
#include <include/soc/j784s4/sdlr_mcu_r5fss0_baseaddress.h>
#endif

#if !defined(SDL_RAT_TEST_H)
#define SDL_RAT_TEST_H


/*===========================================================================*/
/*                    Dependant array in test files                			 */
/*===========================================================================*/

/* Choosing unused address space */
#define SDL_RAT_SELF_TEST_TRANSLATE_BASE  (0x10000000U)

__attribute((section(".ecc_test_rat_testsection"))) __attribute__((aligned(32)))static uint32_t SDL_RATTestArray[8];

#define SDL_RAT_REGION_INDEX 0
/*===========================================================================*/
/*                         Declarations                                      */
/*===========================================================================*/

/* Define the test interface */
typedef struct sdlRatTest_s
{
    int32_t  (*testFunction)(void);   /* The code that runs the test */
    char      *name;                  /* The test name */
    int32_t    testStatus;            /* Test Status */
} sdlRatTest_t;



/*===========================================================================*/
/*                         Macros                                            */
/*===========================================================================*/

#define SDL_APP_TEST_NOT_RUN        (-(int32_t) (2))
#define SDL_APP_TEST_FAILED         (-(int32_t) (1))
#define SDL_APP_TEST_PASS           ( (int32_t) (0))



/*===========================================================================*/
/*                         Internal function declarations                    */
/*===========================================================================*/
void sdlApp_print(const char * str);

/*===========================================================================*/
/*                         External function declarations                    */
/*===========================================================================*/

extern int32_t SDL_RAT_posTest(void);
extern int32_t SDL_RAT_negTest(void);



/*===========================================================================*/
/*                         Local Function definitions                        */
/*===========================================================================*/


#endif /* SDL_RAT_TEST_H */
/* Nothing past this point */
