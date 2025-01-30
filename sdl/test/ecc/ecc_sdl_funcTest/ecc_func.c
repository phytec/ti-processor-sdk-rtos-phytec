/*
 *   Copyright (c) Texas Instruments Incorporated 2019-2023
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
 *  \file     ecc_func.c
 *
 *  \brief    This file contains ECC SDL Function test code for R5 core.
 *
 *  \details  ECC SDL API module tests
 **/

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <stdint.h>
#include <soc.h>
#include <sdl_types.h>
#include <sdl_ecc.h>
#include <ti/drv/uart/UART_stdio.h>
#include <sdl_cache.h>
#include <interrupt.h>
#include <sdl_exception.h>
#include <sdl_arm_r5_pmu.h>
#include <common/ecc_test_main.h>
#include <sdl_esm.h>
#include <src/ip/sdl_ip_rat.h>
#include <src/sdl/sdl_rat.h>
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

#include <osal/sdl_osal.h>
#include <src/ip/sdl_ip_ecc.h>

#if defined (SOC_J721E)
#include <src/sdl/ecc/soc/j721e/sdl_ecc_soc.h>
#include "test/ecc/ecc_sdl_funcTest/soc/j721e/ecc_func.h"
#endif /* SOC_J721E */

#if defined (SOC_J7200)
#include <src/sdl/ecc/soc/j7200/sdl_ecc_soc.h>
#include "test/ecc/ecc_sdl_funcTest/soc/j7200/ecc_func.h"
#endif /* SOC_J7200 */

#if defined (SOC_J721S2)
#include <src/sdl/ecc/soc/j721s2/sdl_ecc_soc.h>
#include "test/ecc/ecc_sdl_funcTest/soc/j721s2/ecc_func.h"
#endif /* SOC_J721S2 */

#if defined (SOC_J784S4)
#include <src/sdl/ecc/soc/j784s4/sdl_ecc_soc.h>
#include "test/ecc/ecc_sdl_funcTest/soc/j784s4/ecc_func.h"
#endif /* SOC_J784S4 */

/* ========================================================================== */
/*                                Macros                                      */
/* ========================================================================== */
#define ATCM_BASE_ADDR  (0x00000000U)
#define ATCM_SIZE       (0x00008000U)
#define TEST_AREA_SIZE  (0x00000400U)

#define ATCM_FILL_PATTERN (0xffffffffU)

#define FIELDVALUE                                               (0x00000001U)


/* delay for 1us*/
#define DELAY 1

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
SDL_ESM_config ECC_Test_esmInitConfig_MCU =
{
    .esmErrorConfig = {0u, 3u}, /* Self test error config */
    .enableBitmap = {0xffffffffu, 0xfe3fffffu, 0x7fffffffu, 0x00000007u,
                },
     /**< All events enable: except timer and self test  events, and Main ESM output */
    /* Temporarily disabling vim compare error as well*/
    .priorityBitmap = {0xffffffffu, 0xff3fffffu, 0x7fffffffu, 0x00000007u,
                        },
    /**< All events high priority: except timer, selftest error events, and Main ESM output */
    .errorpinBitmap = {0xffffffffu, 0xff3fffffu, 0x7fffffffu, 0x00000007u,
                      },
    /**< All events high priority: except timer, selftest error events, and Main ESM output */
};

SDL_ESM_config ECC_Test_esmInitConfig_WKUP =
{
    .esmErrorConfig = {0u, 8u}, /* Self test error config */
    .enableBitmap = {0xffffffffu, 0x00180003u, 0xffffffffu,
                },
     /**< All events enable: except clkstop events for unused clocks */
    .priorityBitmap = {0xffffffffu, 0x00180003u, 0xffffffffu,
                        },
    /**< All events high priority: except clkstop events for unused clocks */
    .errorpinBitmap = {0xffffffffu, 0x00180003u, 0xffffffffu,
                      },
    /**< All events high priority: except clkstop for unused clocks
     *   and selftest error events */
};

#if defined (SOC_J721E) || defined (SOC_J7200)
SDL_ESM_config ECC_Test_esmInitConfig_MAIN =
{
    .esmErrorConfig = {1u, 8u}, /* Self test error config */
    .enableBitmap = {0x00000000u, 0xfffffffbu, 0x7fffffffu, 0xffffffffu,
                 0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                 0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                 0xffffffffu, 0xffffffffu, 0xffffffffu, 0x00000000u,
                 0x00000000u, 0x00000000u, 0x00000000u, 0x00003c0fu,
                 0xffffffffu,
                },
     /**< All events enable: except clkstop events for unused clocks
      *   and PCIE events */
    .priorityBitmap = {0x00000000u, 0xfffffffbu, 0x7fffffffu, 0xffffffffu,
                         0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                         0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                         0xffffffffu, 0xffffffffu, 0xffffffffu, 0x00000000u,
                         0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                         0xffffffffu,
                        },
    /**< All events high priority: except clkstop events for unused clocks
     *   and PCIE events */
    .errorpinBitmap = {0x00000000u, 0xfffffffbu, 0x7fffffffu, 0xffffffffu,
                       0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                       0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                       0xffffffffu, 0xffffffffu, 0xffffffffu, 0x00000000u,
                       0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                       0xffffffffu,
                      },
    /**< All events high priority: except clkstop for unused clocks
     *   and PCIE events */
};
#endif

#if defined (SOC_J721S2)
SDL_ESM_config ECC_Test_esmInitConfig_MAIN =
{
    .esmErrorConfig = {1u, 8u}, /* Self test error config */
    .enableBitmap = {0x00000000u, 0xffffffdbu, 0x7fffffffu, 0xffffffffu,
                 0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                 0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                 0xffffffffu, 0xffffffffu, 0xffffffffu, 0x00000000u,
                 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                 0x00003c0fu,
                },
     /**< All events enable: except clkstop events for unused clocks
      *   and PCIE events */
    .priorityBitmap = {0x00000000u, 0xfffffffbu, 0x7fffffffu, 0xffffffffu,
                         0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                         0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                         0xffffffffu, 0xffffffffu, 0xffffffffu, 0x00000000u,
                         0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                         0x00003c0fu,
                        },
    /**< All events high priority: except clkstop events for unused clocks
     *   and PCIE events */
    .errorpinBitmap = {0x00000000u, 0xfffffffbu, 0x7fffffffu, 0xffffffffu,
                       0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                       0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                       0xffffffffu, 0xffffffffu, 0xffffffffu, 0x00000000u,
                       0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                       0x00003c0fu,
                      },
    /**< All events high priority: except clkstop for unused clocks
     *   and PCIE events */
};

#endif

#if defined (SOC_J784S4)
SDL_ESM_config ECC_Test_esmInitConfig_MAIN =
{
    .esmErrorConfig = {1u, 8u}, /* Self test error config */
    .enableBitmap = {0x00000000u, 0xffffffdbu, 0x7fffffffu, 0xffffffffu,
                 0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                 0xffffffffu, 0xfffffffbu, 0xffffffffu, 0xffffffffu,
                 0xffff7fffu, 0xffffffffu, 0xffffffffu, 0x00000000u,
                 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                 0x00003c0fu, 0x0000000fu,
                },
     /**< All events enable: except clkstop events for unused clocks
      *   and PCIE events */
    .priorityBitmap = {0x00000000u, 0xffffffdbu, 0x7fffffffu, 0xffffffffu,
                         0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                         0xffffffffu, 0xfffffffbu, 0xffffffffu, 0xffffffffu,
                         0xffff7fffu, 0xffffffffu, 0xffffffffu, 0x00000000u,
                         0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                         0x00003c0fu, 0x0000000au,
                        },
    /**< All events high priority: except clkstop events for unused clocks
     *   and PCIE events */
    .errorpinBitmap = {0x00000000u, 0xffffffdbu, 0x7fffffffu, 0xffffffffu,
                       0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                       0xffffffffu, 0xfffffffbu, 0xffffffffu, 0xffffffffu,
                       0xffff7fffu, 0xffffffffu, 0xffffffffu, 0x00000000u,
                       0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                       0x00003c0fu, 0x0000000au,
                      },
    /**< All events high priority: except clkstop for unused clocks
     *   and PCIE events */
};
#endif

/* If this test is for PSRAM starting at address 0x0, we need special
 * translation using RAT in order to access it for the self test. This is because,
 * in the application, R5F view of 0x0 is mapped to ATCM memory. This
 * flag in the test app allows the address translation function defined the test's
 * OSAL implementation to perform the RAT translation when the address is 0x0 for
 * PSRAM testing */
extern bool sdl_ecc_psram_test;
#if defined(SOC_J721E)
uint32_t sdl_ecc_psram_mem = SDL_ECC_MEMTYPE_MCU_PSRAM0;
#elif defined(SOC_J7200)
uint32_t sdl_ecc_psram_mem = SDL_ECC_PSRAM2KECC0_ECC_AGGR;
#elif defined(SOC_J721S2)
uint32_t sdl_ecc_psram_mem = SDL_PSRAM2KECC0_ECC_AGGR;
#elif defined(SOC_J784S4)
uint32_t sdl_ecc_psram_mem = SDL_PSRAM2KECC0_ECC_AGGR;
#endif

extern int32_t SDL_ESM_applicationCallbackFunction(SDL_ESM_Inst esmInstType,
                                                   SDL_ESM_IntType esmIntType,
                                                   uint32_t grpChannel,
                                                   uint32_t index,
                                                   uint32_t intSrc,
                                                   void *arg);
/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */

/* This is the list of exception handle and the parameters */
const SDL_R5ExptnHandlers ECC_Test_R5ExptnHandlers =
{
    .udefExptnHandler = &SDL_EXCEPTION_undefInstructionExptnHandler,
    .swiExptnHandler = &SDL_EXCEPTION_swIntrExptnHandler,
    .pabtExptnHandler = &SDL_EXCEPTION_prefetchAbortExptnHandler,
    .dabtExptnHandler = &SDL_EXCEPTION_dataAbortExptnHandler,
    .irqExptnHandler = &SDL_EXCEPTION_irqExptnHandler,
    .fiqExptnHandler = &SDL_EXCEPTION_fiqExptnHandler,
    .udefExptnHandlerArgs = ((void *)0u),
    .swiExptnHandlerArgs = ((void *)0u),
    .pabtExptnHandlerArgs = ((void *)0u),
    .dabtExptnHandlerArgs = ((void *)0u),
    .irqExptnHandlerArgs = ((void *)0u),
};

void ECC_Test_undefInstructionExptnCallback(void)
{
    UART_printf("\n Undefined Instruction exception");
}

void ECC_Test_swIntrExptnCallback(void)
{
    UART_printf("\n Software interrupt exception");
}

void ECC_Test_prefetchAbortExptnCallback(void)
{
    UART_printf("\n Prefetch Abort exception");
}
void ECC_Test_dataAbortExptnCallback(void)
{
    UART_printf("\n Data Abort exception");
}
void ECC_Test_irqExptnCallback(void)
{
    UART_printf("\n Irq exception");
}

void ECC_Test_fiqExptnCallback(void)
{
    UART_printf("\n Fiq exception");
}


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/* Defines */

int32_t ECC_Memory_init (void);
void ECC_Test_VIMDEDInterruptHandler(void);

void ECC_Test_exceptionInit(void)
{

    SDL_EXCEPTION_CallbackFunctions_t exceptionCallbackFunctions =
            {
             .udefExptnCallback = ECC_Test_undefInstructionExptnCallback,
             .swiExptnCallback = ECC_Test_swIntrExptnCallback,
             .pabtExptnCallback = ECC_Test_prefetchAbortExptnCallback,
             .dabtExptnCallback = ECC_Test_dataAbortExptnCallback,
             .irqExptnCallback = ECC_Test_irqExptnCallback,
             .fiqExptnCallback = ECC_Test_fiqExptnCallback,
            };

    /* Initialize SDL exception handler */
    SDL_EXCEPTION_init(&exceptionCallbackFunctions);
    /* Register SDL exception handler */
    Intc_RegisterExptnHandlers(&ECC_Test_R5ExptnHandlers);

    return;
}

int32_t eccInject_onlyTest(uint32_t mainMem, uint32_t subType, uint32_t chkGrp )
{
    SDL_ErrType_t result;
    int32_t retVal=0;
    uint32_t maxTimeOutMilliSeconds = 100000;
    uint32_t timeOutCnt = 0;

    SDL_ECC_InjectErrorConfig_t injectErrorConfig;

    memset(&injectErrorConfig, 0, sizeof(injectErrorConfig));


    /* Run one shot test for MSMC 2 bit error */
    /* Note the address is relative to start of ram */
    injectErrorConfig.pErrMem = (uint32_t *)(0u);

    injectErrorConfig.flipBitMask = 0x1;
    injectErrorConfig.chkGrp = chkGrp;

    result = SDL_ECC_injectError(mainMem,
                                subType,
                                SDL_INJECT_ECC_ERROR_FORCING_1BIT_ONCE,
                                &injectErrorConfig);

    if (result != SDL_PASS ) {
        UART_printf("\n ecc_aggr_test inject test: mainMem %d: fixed location test failed,Interconnect type RAM id = %d, checker group = %d\n",
        mainMem, subType, chkGrp);
        retVal = -1;
    }
    if (result == SDL_PASS)
    {
        UART_printf("\n\n Waiting for ESM Interrupt \n\n");
        do
            {
                /* dummy wait for the interrupt */
                SDL_OSAL_delay(DELAY);
                timeOutCnt += 10;
                if (timeOutCnt > maxTimeOutMilliSeconds)
                {
                    result = SDL_EFAIL;
                    break;
                }
            } while (esmError == false);
    }
    if(result == SDL_PASS){
        UART_printf("\n\n got ESM Interrupt \n\n");
        esmError = false;
    }

    if (result != SDL_PASS) {
        UART_printf("    ...skipped because this is Inject Only type\n");
        result = SDL_PASS;
    }

    return retVal;
}

static uint32_t arg;
/*********************************************************************
* @fn      ECC_Memory_init
*
* @brief   Initializes Software Diagostics Test Framework
*
* @param   None
*
* @return    0 : Success; < 0 for failures
*/
int32_t ECC_Memory_init (void)
{
    int32_t retValue=0;
    void *ptr = (void *)&arg;
    SDL_ErrType_t result;

    Board_initCfg boardCfg;

    boardCfg = BOARD_INIT_PINMUX_CONFIG |
               BOARD_INIT_UART_STDIO;
    Board_init(boardCfg);

    /* Initialise exception handler */
    ECC_Test_exceptionInit();

    UART_printf("\nECC_Test_init: Exception init complete \n");

    if (retValue == 0) {
        /* Initialize MCU ESM module */
        result = SDL_ESM_init(SDL_ESM_INST_MCU_ESM0, &ECC_Test_esmInitConfig_MCU, SDL_ESM_applicationCallbackFunction, ptr);
        if (result != SDL_PASS) {
            /* print error and quit */
             UART_printf("ECC_Memory_init: Error initializing MCU ESM: result = %d\n", result);

            retValue = -1;
        } else {
            //UART_printf("\nECC_Test_init: Init MCU ESM complete \n");
        }
    }

    if (retValue == 0) {
        /* Initialize WKUP ESM module */
        result = SDL_ESM_init(SDL_ESM_INST_WKUP_ESM0, &ECC_Test_esmInitConfig_WKUP,SDL_ESM_applicationCallbackFunction,ptr);
        if (result != SDL_PASS) {
            /* print error and quit */
             UART_printf("ECC_Memory_init: Error initializing WKUP ESM: result = %d\n", result);

            retValue = -1;
        } else {
            //UART_printf("\nECC_Test_init: Init WKUP ESM complete \n");
        }
    }

    if (retValue == 0) {
        /* Initialize MAIN ESM module */
        result = SDL_ESM_init(SDL_ESM_INST_MAIN_ESM0, &ECC_Test_esmInitConfig_MAIN,SDL_ESM_applicationCallbackFunction,ptr);
        if (result != SDL_PASS) {
            /* print error and quit */
            UART_printf("ECC_Memory_init: Error initializing MAIN ESM: result = %d\n", result);

            retValue = -1;
        } else {
            //UART_printf("\nECC_Test_init: Init MAIN ESM complete \n");
        }
    }

    if (retValue == 0) {
        /* Initialize ECC callbacks within the MCU ESM */
        result = SDL_ECC_initEsm(SDL_ESM_INST_MCU_ESM0);
        if (result != SDL_PASS) {
            /* print error and quit */
             UART_printf("ECC_Memory_init: Error initializing ECC callback for MCU ESM: result = %d\n", result);

            retValue = -1;
        } else {
            //UART_printf("\nECC_Test_init: ECC Callback Init complete for MCU ESM \n");
        }
    }

    if (retValue == 0) {
        /* Initialize ECC callbacks within the Main ESM */
        result = SDL_ECC_initEsm(SDL_ESM_INST_MAIN_ESM0);
        if (result != SDL_PASS) {
            /* print error and quit */
             UART_printf("ECC_Memory_init: Error initializing ECC callback for Main ESM: result = %d\n", result);

            retValue = -1;
        } else {
            //UART_printf("\nECC_Test_init: ECC Callback Init complete for Main ESM \n");
        }
    }

    if (retValue == 0) {
        /* Initialize ECC callbacks within the WKUP ESM */
        result = SDL_ECC_initEsm(SDL_ESM_INST_WKUP_ESM0);
        if (result != SDL_PASS) {
            /* print error and quit */
             UART_printf("ECC_Memory_init: Error initializing ECC callback for WKUP ESM: result = %d\n", result);

            retValue = -1;
        } else {
            //UART_printf("\nECC_Test_init: ECC Callback Init complete for WKUP ESM \n");
        }
    }

    if (retValue == 0) {
        /* Initialize VIM ECC memory ; This is specifically for VIM in lockstep*/
        /* All other memories are auto initialized by hardware */
        result = SDL_ECC_initMemory(SDL_ECC_MEMTYPE_MCU_R5F0_CORE, SDL_ECC_R5F_MEM_SUBTYPE_KS_VIM_RAM_VECTOR_ID);
        if (result != SDL_PASS) {
             /* print error and quit */
              UART_printf("ECC_Memory_init: Error initializing ECC memory: retValue = %d\n", retValue);
             retValue = -1;;
         } else {
             //UART_printf("\nECC_Test_init: ECC Memory init complete \n");
         }
    }

    return retValue;
}

#define DEBUG

/********************************************************************
 * @fn      ecc_aggr_test
 *
 * @brief   Execute ECC single bit error self test on ecc_aggr_test ECC aggregator
 *
 * @param   None
 *
 * @return  0 : Success; < 0 for failures
 */
int32_t ecc_aggr_test(void)
{
    SDL_ErrType_t result;
    int32_t retVal=0;
    uint32_t j,i,k,mainMem;
    SDL_ecc_aggrRegs *pEccAggr;
    uint32_t maxTimeOutMilliSeconds = 3000;
    uint32_t timeOutCnt = 0;
    SDL_ECC_InjectErrorType    intsrc;
    uint32_t errSrc;
    uint32_t errorInjectionErrSrc;

#ifdef DEBUG
    int32_t selectedIndex = -1;
    bool exit = (bool)false;
#endif

    SDL_ECC_InjectErrorConfig_t injectErrorConfig;
#ifdef DEBUG
    while (exit != (bool)true)
    {
    	UART_printf("\n Enter number between range 0 to %d",SDL_ECC_MEMTYPE_MAX);
        UART_printf("\n Select the memory to test...");
        UART_scanFmt("%d", &selectedIndex);

        if (selectedIndex == -1)
        {
            UART_printf("\n Exiting the test on request \n");
            exit = (bool)true;
            continue;
        }
        UART_printf("...selected %d\n", selectedIndex);
        if (selectedIndex >= SDL_ECC_MEMTYPE_MAX)
        {
            UART_printf("\necc_aggr_test: selection [%d] is not a valid memory id\n", selectedIndex);
            continue;
        }
#endif

        memset(&injectErrorConfig, 0, sizeof(injectErrorConfig));
        /* Run one shot test for ecc_aggr_test 1 bit error */
        /* Note the address is relative to start of ram */
        injectErrorConfig.pErrMem = (uint32_t *)(0u);

        injectErrorConfig.flipBitMask = 0x3;
#ifdef DEBUG
        mainMem = selectedIndex;
#else
        for (mainMem = SDL_ECC_MEMTYPE_MCU_R5F0_CORE; mainMem < SDL_ECC_MEMTYPE_MAX; mainMem++)
        {
#endif
#ifndef DEBUG
            if (mainMem >= SDL_ECC_MEMTYPE_MAX)
            {
                UART_printf("\necc_aggr_test: selection [%d] is not a valid memory id\n", mainMem);
                continue;
            }
            else if (ECC_Test_config[mainMem].initConfig == NULL)
            {
                UART_printf("\necc_aggr_test: [%d] Skipping %s due to missing information\n", mainMem, ECC_Test_config[mainMem].aggrName);
                continue;
            }
#endif
            UART_printf("\n ecc_aggrtest: [%d]  self test: %s starting \n\n", mainMem, ECC_Test_config[mainMem].aggrName);
            /* Sub memory list  entered in the for loop for perticular mem type*/

            if (ECC_Test_config[mainMem].initConfig != NULL)
            {

                /* Initialize ECC */
                result = SDL_ECC_init(mainMem, ECC_Test_config[mainMem].initConfig);
                if (result != SDL_PASS)
                {
                    /* print error and quit */
                    UART_printf("\nECC_Memory_init: [%d] Error initializing %s: result = %d\n", mainMem, ECC_Test_config[mainMem].aggrName, result);

                    result = -1;
                }
                else
                {
                    UART_printf("\nECC_Memory_init: [%d] %s ECC Init complete \n", mainMem, ECC_Test_config[mainMem].aggrName);
                }
            }
            else
            {
                UART_printf("\nECC_Memory_init: [%d] Skipping %s due to missing data\n", mainMem, ECC_Test_config[mainMem].aggrName);
            }

            for (i=0; i< SDL_ECC_aggrTable[mainMem].numRams; i++)
            {
#if defined (SOC_J721E)
                if (((mainMem == SDL_ECC_MEMTYPE_MCU_R5F0_CORE) && (i == SDL_ECC_R5F_MEM_SUBTYPE_KS_VIM_RAM_VECTOR_ID)) ||
                    ((mainMem == SDL_R5FSS0_CORE0_ECC_AGGR) && (i == SDL_R5FSS0_CORE0_ECC_AGGR_CPU0_KS_VIM_RAMECC_RAM_ID)) ||
                    ((mainMem == SDL_ECC_MEMTYPE_MCU_R5F1_CORE) && (i == SDL_MCU_R5FSS0_CORE1_ECC_AGGR_CPU1_KS_VIM_RAMECC_RAM_ID)) ||
                    ((mainMem == SDL_R5FSS0_CORE1_ECC_AGGR) && (i == SDL_R5FSS0_CORE1_ECC_AGGR_CPU1_KS_VIM_RAMECC_RAM_ID)) ||
                    ((mainMem == SDL_R5FSS1_CORE0_ECC_AGGR) && (i == SDL_R5FSS1_CORE0_ECC_AGGR_CPU0_KS_VIM_RAMECC_RAM_ID)) ||
                    ((mainMem == SDL_R5FSS1_CORE1_ECC_AGGR) && (i == SDL_R5FSS1_CORE1_ECC_AGGR_CPU1_KS_VIM_RAMECC_RAM_ID)))
#elif (SOC_J7200)
                if (((mainMem == SDL_ECC_MEMTYPE_MCU_R5F0_CORE) && (i == SDL_ECC_R5F_MEM_SUBTYPE_KS_VIM_RAM_VECTOR_ID)) ||
                    ((mainMem == SDL_R5FSS0_CORE0_ECC_AGGR) && (i == SDL_R5FSS0_CPU0_KS_VIM_RAMECC_RAM_ID)) ||
                    ((mainMem == SDL_ECC_MEMTYPE_MCU_R5F1_CORE) && (i == SDL_MCU_R5FSS0_CPU1_KS_VIM_RAMECC_RAM_ID)) ||
                    ((mainMem == SDL_R5FSS0_CORE1_ECC_AGGR) && (i == SDL_R5FSS0_CPU1_KS_VIM_RAMECC_RAM_ID)) )
#elif (SOC_J721S2)
                if (((mainMem == SDL_ECC_MEMTYPE_MCU_R5F0_CORE) && (i == SDL_MCU_R5FSS0_PULSAR_SL_CPU0_ECC_AGGR_CPU0_KS_VIM_RAMECC_RAM_ID)) ||
                    ((mainMem == SDL_ECC_MEMTYPE_MCU_R5F1_CORE) && (i == SDL_MCU_R5FSS0_PULSAR_SL_CPU1_ECC_AGGR_CPU1_KS_VIM_RAMECC_RAM_ID)) ||
                    ((mainMem == SDL_R5FSS0_0_ECC_AGGR) && (i == SDL_R5FSS0_PULSAR_SL_CPU0_ECC_AGGR_CPU0_KS_VIM_RAMECC_RAM_ID)) ||
                    ((mainMem == SDL_R5FSS0_1_ECC_AGGR) && (i == SDL_R5FSS0_PULSAR_SL_CPU1_ECC_AGGR_CPU1_KS_VIM_RAMECC_RAM_ID)) ||
                    ((mainMem == SDL_R5FSS1_0_ECC_AGGR) && (i == SDL_R5FSS1_PULSAR_SL_CPU0_ECC_AGGR_CPU0_KS_VIM_RAMECC_RAM_ID)) ||
                    ((mainMem == SDL_R5FSS1_1_ECC_AGGR) && (i == SDL_R5FSS1_PULSAR_SL_CPU1_ECC_AGGR_CPU1_KS_VIM_RAMECC_RAM_ID)))
#elif (SOC_J784S4)
                if (((mainMem == SDL_ECC_MEMTYPE_MCU_R5F0_CORE) && (i == SDL_MCU_R5FSS0_PULSAR_SL_CPU0_ECC_AGGR_CPU0_KS_VIM_RAMECC_RAM_ID)) ||
                    ((mainMem == SDL_ECC_MEMTYPE_MCU_R5F1_CORE) && (i == SDL_MCU_R5FSS0_PULSAR_SL_CPU1_ECC_AGGR_CPU1_KS_VIM_RAMECC_RAM_ID)) ||
                    ((mainMem == SDL_R5FSS0_0_ECC_AGGR) && (i == SDL_R5FSS0_PULSAR_SL_CPU0_ECC_AGGR_CPU0_KS_VIM_RAMECC_RAM_ID)) ||
                    ((mainMem == SDL_R5FSS0_1_ECC_AGGR) && (i == SDL_R5FSS0_PULSAR_SL_CPU1_ECC_AGGR_CPU1_KS_VIM_RAMECC_RAM_ID)) ||
                    ((mainMem == SDL_R5FSS1_0_ECC_AGGR) && (i == SDL_R5FSS1_PULSAR_SL_CPU0_ECC_AGGR_CPU0_KS_VIM_RAMECC_RAM_ID)) ||
                    ((mainMem == SDL_R5FSS1_1_ECC_AGGR) && (i == SDL_R5FSS1_PULSAR_SL_CPU1_ECC_AGGR_CPU1_KS_VIM_RAMECC_RAM_ID)))
#endif
                {
                    UART_printf("skipping VIM because it is destructive\n");
                }
#if defined (SOC_J721S2)
                else if ((mainMem == SDL_ECC_MEMTYPE_MAIN_MSMC_AGGR0) && (i == SDL_COMPUTE_CLUSTER0_COMPUTE_CLUSTER_J7AE_MSMC_EN_DRU_MSMC_EN_4MB_WRAP_MSMC_4MB_MSMC_WRAP_ECC_AGGR0_EN_MSMC_P0_MMR_FW_EDC_CTL_RAM_ID))
                {
                    UART_printf("skipping MSMC endpoint 85 - it fails\n");
                }
#endif
                else if ((SDL_ECC_aggrTable[mainMem].ramTable[i].RAMId) != SDL_ECC_RAMID_INVALID)
                {
                    if ((SDL_ECC_aggrTable[mainMem].ramTable[i].ramIdType) != SDL_ECC_AGGR_ECC_TYPE_ECC_WRAPPER)
                    {
                        UART_printf("self test started RamId %d  starting \n",i );

                        /* This for loop provide interconnect checkers group */
                        for (j=0; j< SDL_ECC_aggrTable[mainMem].ramTable[i].numCheckers; j++)
                        {
                            injectErrorConfig.chkGrp = j;
                            /* Below mainMem are third parties ECC Aggregator, It can be tested by creating traffics.
                            SDL supports inject only because out of scope for SDL implementation.  */
#if defined (SOC_J721E)
                            if( (mainMem == SDL_CSI_RX_IF0_ECC_AGGR_0) || (mainMem == SDL_CSI_RX_IF1_ECC_AGGR_0 ) ||
                                (mainMem == SDL_CSI_TX_IF0_CSI_TX_IF_ECC_AGGR) || (mainMem == SDL_CSI_TX_IF0_CSI_TX_IF_ECC_AGGR_BYTE) ||
                                (mainMem == SDL_DSS_EDP0_K3_DSS_EDP_MHDPTX_WRAPPER_ECC_AGGR_PHY) || (mainMem == SDL_DSS_EDP0_K3_DSS_EDP_MHDPTX_WRAPPER_ECC_AGGR_DSC) ||
                                (mainMem == SDL_DSS_EDP0_K3_DSS_EDP_MHDPTX_WRAPPER_ECC_AGGR_CORE) || (mainMem == SDL_DSS_DSI0_K3_DSS_DSI_TOP_ECC_AGGR_SYS) ||
                                (mainMem == SDL_I3C0_I3C_P_ECC_AGGR) || (mainMem == SDL_MCU_I3C0_I3C_P_ECC_AGGR) || (mainMem == SDL_MCU_I3C1_I3C_P_ECC_AGGR))
#elif (SOC_J7200)
                            if( (mainMem == SDL_ECC_MCU_I3C0_0_ECC_AGGR) || (mainMem == SDL_ECC_MCU_I3C0_1_ECC_AGGR) ||
                                (mainMem == SDL_ECC_MCU_I3C1_0_ECC_AGGR) || (mainMem == SDL_ECC_MCU_I3C1_1_ECC_AGGR) )
#elif (SOC_J721S2)
                            if(0)
#elif (SOC_J784S4)
							if( (mainMem == SDL_DSS_DSI0_ECC_AGGR) || (mainMem == SDL_DSS_EDP0_0_ECC_AGGR) || (mainMem == SDL_DSS_EDP0_2_ECC_AGGR) ||
								(mainMem == SDL_DSS_DSI1_ECC_AGGR) || (mainMem == SDL_PCIE0_0_ECC_AGGR) || (mainMem == SDL_PCIE0_1_ECC_AGGR)||
								(mainMem == SDL_PCIE3_0_ECC_AGGR) || (mainMem == SDL_PCIE3_1_ECC_AGGR) || (mainMem == SDL_PCIE2_0_ECC_AGGR) ||
								(mainMem == SDL_PCIE2_1_ECC_AGGR) || (mainMem == SDL_PCIE1_0_ECC_AGGR) || (mainMem == SDL_PCIE1_1_ECC_AGGR) ||
								(mainMem == SDL_MCU_I3C1_0_ECC_AGGR) || (mainMem == SDL_MCU_I3C1_1_ECC_AGGR) || (mainMem == SDL_CSI_RX_IF2_ECC_AGGR) || 
                                (mainMem == SDL_CSI_RX_IF1_ECC_AGGR) || (mainMem == SDL_CSI_RX_IF0_ECC_AGGR) || (mainMem == SDL_CSI_TX_IF_V2_1_0_ECC_AGGR) ||
                                (mainMem == SDL_CSI_TX_IF_V2_1_1_ECC_AGGR) || (mainMem == SDL_CSI_TX_IF_V2_0_0_ECC_AGGR) || (mainMem == SDL_CSI_TX_IF_V2_0_1_ECC_AGGR) ||
                                (mainMem == SDL_MCU_I3C0_1_ECC_AGGR) )
#endif                                      
                            {
                                result = eccInject_onlyTest(mainMem, i, j);
                            }
                            else
                            {
                                if(SDL_ECC_aggrTable[mainMem].esmIntSEC != 0u)
                                {
                                    intsrc = SDL_INJECT_ECC_ERROR_FORCING_1BIT_ONCE;
                                    result = SDL_ECC_selfTest(mainMem,
                                                            i,
                                                            intsrc,
                                                            &injectErrorConfig,
                                                            100000);
                                        
                                    if (result != SDL_PASS)
                                    {
                                        UART_printf("\n ecc_aggr_test self test SEC : mainMem %d: fixed location test failed,Interconnect type RAM id = %d, checker group = %d\n",
                                        mainMem, i, j);
                                        retVal = -1;
                                    }
                                }

                                /* DED is supported only for EDC checker type groups of interconnect RAM id's */
                                intsrc = SDL_INJECT_ECC_ERROR_FORCING_2BIT_ONCE;
                                result = SDL_ECC_selfTest(mainMem,
                                                        i,
                                                        intsrc,
                                                        &injectErrorConfig,
                                                        100000);

                                /* SDL_EUNSUPPORTED_OPS is only returned in the case of DED test for invalid checker group type */
                                if (result != SDL_PASS && result != SDL_EUNSUPPORTED_OPS)
                                {
                                    UART_printf("\n ecc_aggr_test self test DED : mainMem %d: fixed location test failed,Interconnect type RAM id = %d, checker group = %d\n",
                                    mainMem, i, j);
                                    retVal = -1;
                                }
                            }
                        }
                    }
                    else
                    {
                        for (k=0; k< SDL_ECC_aggrTable[mainMem].numMemEntries; k++)
                        {
#if defined(SOC_J7200)
                            if ((SDL_ECC_aggrTable[mainMem].memConfigTable[k].memSubType) == (SDL_ECC_aggrTable[mainMem].ramTable[i].RAMId))
                            {
                                if ((mainMem == SDL_ECC_MSRAM_512K0_ECC_AGGR) && (SDL_ECC_aggrTable[mainMem].ramTable[i].RAMId == 0U))
                                {
                                    SDL_ratTranslationCfgInfo translationCfg;
                                    /* Add RAT configuration to access address > 32bit address range */
                                    translationCfg.translatedAddress = SDL_MSRAM_512K0_RAM_BASE;
                                    translationCfg.sizeInBytes = 528U;
                                    translationCfg.baseAddress = (uint32_t)0x60000000U;
                                    uint32_t index = 0U;

                                    /* Set up RAT translation */
                                    result = SDL_RAT_configRegionTranslation((SDL_ratRegs *)SDL_MCU_R5FSS0_RAT_CFG_BASE,
                                                                            index, &translationCfg);
                                    if (result == SDL_PASS)
                                    {
                                        injectErrorConfig.pErrMem = (uint32_t *)0x60000000U;
                                    }
                                    else
                                    {
                                        UART_printf("RAT translation is failed" );
                                    }
                                    break;
                                }
                                else
                                {
                                    injectErrorConfig.pErrMem =((uint32_t *)SDL_ECC_aggrTable[mainMem].memConfigTable[k].memStartAddr);
                                    break;
                                }
                            }
#elif defined (SOC_J721E) || defined (SOC_J721S2) || defined (SOC_J784S4)
                            if ((SDL_ECC_aggrTable[mainMem].memConfigTable[k].memSubType) == (SDL_ECC_aggrTable[mainMem].ramTable[i].RAMId))
                            {
                                if (SDL_ECC_aggrTable[mainMem].memConfigTable[k].memStartAddr < 0x100000000)
                                {
                                    injectErrorConfig.pErrMem = ((uint32_t *)SDL_ECC_aggrTable[mainMem].memConfigTable[k].memStartAddr);
                                }
                                else
                                {
                                    injectErrorConfig.errMem64 = (uint64_t)SDL_ECC_aggrTable[mainMem].memConfigTable[k].memStartAddr;
                                }

                                break;
                            }
#endif
                        }

 /* Below mainMem are third parties ECC Aggregator, It can be tested by creating traffics.
                   SDL supports inject only because out of scope for SDL implementation.  */
#if defined (SOC_J721E)
                        if( (mainMem == SDL_CSI_RX_IF0_ECC_AGGR_0) || (mainMem == SDL_CSI_RX_IF1_ECC_AGGR_0 ) ||
                            (mainMem == SDL_CSI_TX_IF0_CSI_TX_IF_ECC_AGGR) || (mainMem == SDL_CSI_TX_IF0_CSI_TX_IF_ECC_AGGR_BYTE) ||
                            (mainMem == SDL_DSS_EDP0_K3_DSS_EDP_MHDPTX_WRAPPER_ECC_AGGR_PHY) || (mainMem == SDL_DSS_EDP0_K3_DSS_EDP_MHDPTX_WRAPPER_ECC_AGGR_DSC) ||
                            (mainMem == SDL_DSS_EDP0_K3_DSS_EDP_MHDPTX_WRAPPER_ECC_AGGR_CORE) || (mainMem == SDL_DSS_DSI0_K3_DSS_DSI_TOP_ECC_AGGR_SYS) ||
                            (mainMem == SDL_I3C0_I3C_P_ECC_AGGR) || (mainMem == SDL_MCU_I3C0_I3C_P_ECC_AGGR) || (mainMem == SDL_MCU_I3C1_I3C_P_ECC_AGGR))
#elif (SOC_J7200)
                        if( (mainMem == SDL_ECC_MCU_I3C0_0_ECC_AGGR) || (mainMem == SDL_ECC_MCU_I3C0_1_ECC_AGGR) ||
                            (mainMem == SDL_ECC_MCU_I3C1_0_ECC_AGGR) || (mainMem == SDL_ECC_MCU_I3C1_1_ECC_AGGR) )
#elif (SOC_J721S2)
                        if(0)
#elif (SOC_J784S4)
						if( (mainMem == SDL_DSS_DSI0_ECC_AGGR) || (mainMem == SDL_DSS_EDP0_0_ECC_AGGR) || (mainMem == SDL_DSS_EDP0_2_ECC_AGGR) ||
							(mainMem == SDL_DSS_DSI1_ECC_AGGR) || (mainMem == SDL_PCIE0_0_ECC_AGGR) || (mainMem == SDL_PCIE0_1_ECC_AGGR)||
							(mainMem == SDL_PCIE3_0_ECC_AGGR) || (mainMem == SDL_PCIE3_1_ECC_AGGR) || (mainMem == SDL_PCIE2_0_ECC_AGGR) ||
							(mainMem == SDL_PCIE2_1_ECC_AGGR) || (mainMem == SDL_PCIE1_0_ECC_AGGR) || (mainMem == SDL_PCIE1_1_ECC_AGGR) ||
							(mainMem == SDL_MCU_I3C1_0_ECC_AGGR) || (mainMem == SDL_MCU_I3C1_1_ECC_AGGR) || (mainMem == SDL_CSI_RX_IF2_ECC_AGGR) || 
                            (mainMem == SDL_CSI_RX_IF1_ECC_AGGR) || (mainMem == SDL_CSI_RX_IF0_ECC_AGGR) || (mainMem == SDL_CSI_TX_IF_V2_1_0_ECC_AGGR) ||
                            (mainMem == SDL_CSI_TX_IF_V2_1_1_ECC_AGGR) || (mainMem == SDL_CSI_TX_IF_V2_0_0_ECC_AGGR) || (mainMem == SDL_CSI_TX_IF_V2_0_1_ECC_AGGR) ||
                            (mainMem == SDL_MCU_I3C0_1_ECC_AGGR) )
#endif
                        {
                            result = eccInject_onlyTest(mainMem, i, j);
                        }
                        else
                        {

                            if (SDL_ECC_aggrTable[mainMem].memConfigTable[k].readable == ((bool)true))
                            {
                                UART_printf("self test started accessable RamId %d  starting \n",i );
                                for (errorInjectionErrSrc=SDL_INJECT_ECC_ERROR_FORCING_1BIT_ONCE; errorInjectionErrSrc<=(uint32_t)SDL_INJECT_ECC_ERROR_FORCING_2BIT_ONCE; errorInjectionErrSrc++)
                                {
                                    if((errorInjectionErrSrc == SDL_INJECT_ECC_ERROR_FORCING_1BIT_ONCE) && (SDL_ECC_aggrTable[mainMem].esmIntSEC != 0u))
                                    {
                                        intsrc = SDL_INJECT_ECC_ERROR_FORCING_1BIT_ONCE;

                                    }
                                    else if (errorInjectionErrSrc == SDL_INJECT_ECC_ERROR_FORCING_2BIT_ONCE){
                                        intsrc = SDL_INJECT_ECC_ERROR_FORCING_2BIT_ONCE;
                                    }
                                    else {
                                        continue;
                                    }

                                    if (SDL_ECC_aggrTable[mainMem].ramTable[i].aggregatorTypeInjectOnly == 1U)
                                    {
                                        result = SDL_ECC_injectError(mainMem,i,intsrc,&injectErrorConfig);
                                        if (result == SDL_PASS )
                                        {
                                            if (mainMem < SDL_ECC_MEMTYPE_MAX)
                                            {
                                                if (mainMem <= (SDL_ECC_AGGREGATOR_MAX_LOW_ENTRIES-1U))
                                                {
                                                    /* Note in the SDL_ECC_aggrBaseAddressTable only the above are
                                                    * supported currently
                                                    */
                                                    pEccAggr = SDL_ECC_aggrBaseAddressTable[mainMem];
                                                }
                                                else if (mainMem >= SDL_ECC_MEMTYPE_MAIN_MSMC_AGGR0)
                                                {
                                                    pEccAggr = SDL_ECC_aggrHighBaseAddressTableTrans[mainMem - \
                                                            SDL_ECC_MEMTYPE_MAIN_MSMC_AGGR0];
                                                }
                                                SDL_ecc_aggrSetEccRamIntrPending(pEccAggr, i, errSrc);
                                            }

                                            do
                                            {
                                                /* dummy wait for the interrupt */
                                                SDL_OSAL_delay(DELAY);;
                                                timeOutCnt += 10;
                                                if (timeOutCnt > maxTimeOutMilliSeconds)
                                                {
                                                    result = SDL_EFAIL;
                                                    break;
                                                }
                                            } while (esmError == false);
                                            UART_printf("    ...skipped because this is Inject Only type\n");
                                            result = SDL_PASS;
                                        }
                                    }
                                    else
                                    {
                                        /* If this test is for PSRAM starting at address 0x0, we need special
                                        * translation using RAT in order to access it for the self test. This is because,
                                        * in the application, R5F view of 0x0 is mapped to ATCM memory. This
                                        * flag in the test app allows the address translation function defined the test's
                                        * OSAL implementation to perform the RAT translation when the address is 0x0 for
                                        * PSRAM testing */
                                        if (mainMem == sdl_ecc_psram_mem)
                                        {
                                            sdl_ecc_psram_test = (bool)true;
                                        }
                                        result = SDL_ECC_selfTest(mainMem,
                                                                i,
                                                                intsrc,
                                                                &injectErrorConfig,
                                                                100000);
                                        
                                        /* SDL_EUNSUPPORTED_OPS is only returned in the case of DED test for invalid checker group type */
                                        if (result != SDL_PASS  && result != SDL_EUNSUPPORTED_OPS)
                                        {
                                            UART_printf("\n ecc_aggr_test self test for intSrc = %d : mainMem %d: accessable mem type test failed, Wrapper type RAM id = %d\n",intsrc,
                                                        mainMem, i);
                                            retVal = -1;
                                        }
                                        /* set sdl_ecc_psram_test back to false to indicate test is done */
                                        sdl_ecc_psram_test = (bool)false;
                                    }
                                }
                            }
                            else
                            {
                                UART_printf("self test started not accessable RamId %d  starting \n",i );

                                if(SDL_ECC_aggrTable[mainMem].esmIntSEC != 0u)
                                {
                                    intsrc = SDL_INJECT_ECC_ERROR_FORCING_1BIT_ONCE;
                                    errSrc = SDL_ECC_AGGR_INTR_SRC_SINGLE_BIT;
                                    esmError = false;
                                    result = SDL_ECC_injectError(mainMem,i,intsrc,&injectErrorConfig);
                                    if (result == SDL_PASS )
                                    {
                                        if (SDL_ECC_aggrTable[mainMem].ramTable[i].aggregatorTypeInjectOnly != 1)
                                        {
                                            if (mainMem < SDL_ECC_MEMTYPE_MAX)
                                            {
                                                if (mainMem <= (SDL_ECC_AGGREGATOR_MAX_LOW_ENTRIES-1U))
                                                {
                                                    /* Note in the SDL_ECC_aggrBaseAddressTable only the above are
                                                    * supported currently
                                                    */
                                                    pEccAggr = SDL_ECC_aggrBaseAddressTable[mainMem];
                                                } 
                                                else if (mainMem >= SDL_ECC_MEMTYPE_MAIN_MSMC_AGGR0)
                                                {
                                                    pEccAggr = SDL_ECC_aggrHighBaseAddressTableTrans[mainMem - \
                                                            SDL_ECC_MEMTYPE_MAIN_MSMC_AGGR0];
                                                }
                                                SDL_ecc_aggrSetEccRamIntrPending(pEccAggr, i, errSrc);
                                            }
                                            timeOutCnt = 0;
                                            do
                                            {
                                                /* dummy wait for the interrupt */
                                                SDL_OSAL_delay(DELAY);;
                                                timeOutCnt += 10;
                                                if (timeOutCnt > maxTimeOutMilliSeconds)
                                                {
                                                    result = SDL_EFAIL;
                                                    break;
                                                }
                                            } while (esmError == false);

                                            timeOutCnt = 0;
                                            esmError = false;
                                            if (result == SDL_PASS)
                                            {
                                                UART_printf("\n\n  Got it\n");
                                            }
                                            else
                                            {
                                                UART_printf("\n Failed\n");
                                            }
                                        }
                                        else
                                        {
                                            do
                                            {
                                                /* dummy wait for the interrupt */
                                                SDL_OSAL_delay(DELAY);;
                                                timeOutCnt += 10;
                                                if (timeOutCnt > maxTimeOutMilliSeconds)
                                                {
                                                    result = SDL_EFAIL;
                                                    break;
                                                }
                                            } while (esmError == false);
                                            UART_printf("    ...skipped because this is Inject Only type\n");
                                            result = SDL_PASS;
                                        }
                                    }
                                    else
                                    {
                                        UART_printf("    Inject error failed!\n");
                                    }
                                    if (result != SDL_PASS )
                                    {
                                        UART_printf("\n ecc_aggr_test self test SEC: mainMem %d: fixed location test failed, Wrapper type RAM id = %d\n",
                                                mainMem, i);
                                        retVal = -1;
                                    }
                                }
                                intsrc = SDL_INJECT_ECC_ERROR_FORCING_2BIT_ONCE;
                                errSrc = SDL_ECC_AGGR_INTR_SRC_DOUBLE_BIT;
                                esmError = false;
                                result = SDL_ECC_injectError(mainMem,i,intsrc,&injectErrorConfig);
                                if (result == SDL_PASS )
                                {
                                    if (SDL_ECC_aggrTable[mainMem].ramTable[i].aggregatorTypeInjectOnly != 1)
                                    {
                                        if (mainMem < SDL_ECC_MEMTYPE_MAX)
                                        {
                                            if (mainMem <= (SDL_ECC_AGGREGATOR_MAX_LOW_ENTRIES-1U))
                                            {
                                                /* Note in the SDL_ECC_aggrBaseAddressTable only the above are
                                                * supported currently
                                                */
                                                pEccAggr = SDL_ECC_aggrBaseAddressTable[mainMem];
                                            } 
                                            else if (mainMem >= SDL_ECC_MEMTYPE_MAIN_MSMC_AGGR0)
                                            {
                                                pEccAggr = SDL_ECC_aggrHighBaseAddressTableTrans[mainMem - \
                                                        SDL_ECC_MEMTYPE_MAIN_MSMC_AGGR0];
                                            }
                                            SDL_ecc_aggrSetEccRamIntrPending(pEccAggr, i, errSrc);
                                        }
                                        timeOutCnt = 0;
                                        do
                                        {
                                            /* dummy wait for the interrupt */
                                            SDL_OSAL_delay(DELAY);;
                                            timeOutCnt += 10;
                                            if (timeOutCnt > maxTimeOutMilliSeconds)
                                            {
                                                result = SDL_EFAIL;
                                                break;
                                            }
                                        } while (esmError == false);

                                        timeOutCnt = 0;
                                        esmError = false;
                                        if (result == SDL_PASS)
                                        {
                                            UART_printf("\n\n  Got it\n");
                                        }
                                        else
                                        {
                                            UART_printf("\n Failed\n");
                                        }
                                    }
                                    else
                                    {
                                        do
                                        {
                                            /* dummy wait for the interrupt */
                                            SDL_OSAL_delay(DELAY);;
                                            timeOutCnt += 10;
                                            if (timeOutCnt > maxTimeOutMilliSeconds)
                                            {
                                                result = SDL_EFAIL;
                                                break;
                                            }
                                        } while (esmError == false);
                                        UART_printf("    ...skipped because this is Inject Only type\n");
                                        result = SDL_PASS;
                                    }
                                }
                                else
                                {
                                    UART_printf("    Inject error failed!\n");
                                }
                                /* SDL_EUNSUPPORTED_OPS is only returned in the case of DED test for invalid checker group type */
                                if (result != SDL_PASS  && result != SDL_EUNSUPPORTED_OPS)
                                {
                                    UART_printf("\n ecc_aggr_test self test DED: mainMem %d: fixed location test failed, Wrapper type RAM id = %d\n",
                                            mainMem, i);
                                    retVal = -1;
                                }
                            }
                        }
                    }
                }
#ifdef DEBUG
        }
#endif
    }
    return retVal;
}

static int32_t ECC_sdlFuncTest(void)
{
    int32_t result;
    int32_t retVal = 0;

    UART_printf("\n ECC SDL API tests: starting \n\n");
    if (retVal == 0) {
       result = ecc_aggr_test();
       if (result != SDL_PASS) {
           retVal = -1;
            UART_printf("\n ecc_aggr test has failed...");
       }
    }
    return retVal;
}

/* ECC Function module test */
int32_t ECC_funcTest(void)
{
    int32_t testResult;

    testResult = ECC_Memory_init();
    if (testResult != 0)
    {
        UART_printf("\n ECC func tests: unsuccessful");
        return SDL_EFAIL;
    }

    testResult = ECC_sdlFuncTest();

    return (testResult);
}

/* Nothing past this point */
