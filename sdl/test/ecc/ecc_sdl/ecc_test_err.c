/*
 *   Copyright (c) Texas Instruments Incorporated 2019-2021
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
 *  \file     ecc_test_err.c
 *
 *  \brief    This file contains ECC Error module test code for R5 core.
 *
 *  \details  ECC Error module tests
 **/

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <stdint.h>
#include <soc.h>
#include <sdl_types.h>
#include <src/sdl/sdl_ecc.h>
#include <common/ecc_test_main.h>
#include <ti/drv/uart/UART_stdio.h>

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

static int32_t ECC_errNegativeTest(void)
{
    int32_t result;
    int32_t retVal = 0;
    SDL_ECC_InjectErrorConfig_t injectErrorConfig;
    SDL_ECC_staticRegs staticRegs;

    UART_printf("\n ECC negative tests: starting");

    /* Negative tests with invalid mem type 1000U */
    result = SDL_ECC_initMemory(1000U,
                                0U);
    if (result == SDL_PASS) {
        UART_printf("\n  Negative test failed on line no: %d \n", __LINE__);
        retVal = -1;
    }

    if (retVal == 0) {
        /* Negative tests with invalid mem subtype 50U */
        result = SDL_ECC_initMemory(SDL_ECC_MEMTYPE_MCU_R5F0_CORE,
                                    50U);
        if (result == SDL_PASS) {
            UART_printf("\n  Negative test failed on line no: %d \n", __LINE__);
            retVal = -1;
        }
    }

    if (retVal == 0) {
        /* Not actual negative tests with valid mem subtype 0U */
        result = SDL_ECC_initMemory(SDL_ECC_MEMTYPE_MCU_R5F0_CORE, 0U);
        if (result != SDL_PASS) {
            UART_printf("\n  Negative test failed on line no: %d \n", __LINE__);
            retVal = -1;
        }
    }

    if (retVal == 0) {
        /* Negative tests with invalid mem subtype 50U */
        result = SDL_ECC_selfTest(SDL_ECC_MEMTYPE_MCU_R5F0_CORE,
                                  SDL_ECC_R5F_MEM_SUBTYPE_ATCM0_BANK0_VECTOR_ID,
                                  SDL_INJECT_ECC_ERROR_FORCING_1BIT_ONCE,
                                  NULL,
                                  1000);
        if (result != SDL_EBADARGS) {
            UART_printf("\n  Negative test failed on line no: %d \n", __LINE__);
            retVal = -1;
        }
    }

    if (retVal == 0) {
        /* Negative tests with invalid mem subtype 50U */
        result = SDL_ECC_injectError(SDL_ECC_MEMTYPE_MCU_R5F0_CORE,
                                  SDL_ECC_R5F_MEM_SUBTYPE_ATCM0_BANK0_VECTOR_ID,
                                  SDL_INJECT_ECC_ERROR_FORCING_1BIT_ONCE,
                                  NULL);
        if (result != SDL_EBADARGS) {
            UART_printf("\n  Negative test failed on line no: %d \n", __LINE__);
            retVal = -1;
        }
    }
    if (retVal == 0) {
        SDL_ECC_InjectErrorConfig_t injectErrorConfig;
        injectErrorConfig.flipBitMask = 0x0;
        /* Negative tests without mask bit */
        result = SDL_ECC_injectError(SDL_ECC_MEMTYPE_MCU_R5F0_CORE,
                                  SDL_ECC_R5F_MEM_SUBTYPE_ATCM0_BANK0_VECTOR_ID,
                                  SDL_INJECT_ECC_ERROR_FORCING_1BIT_ONCE,
                                  &injectErrorConfig);
        if (result != SDL_EFAIL) {
            UART_printf("\n  Negative test failed on line no: %d \n", __LINE__);
            retVal = -1;
        }
    }

    if (retVal == 0) {
        /* Pass invalid error configuration. No bits set */
        injectErrorConfig.flipBitMask = 0x00;
        /* Set Error address */
        injectErrorConfig.pErrMem = (uint32_t *)(0x100);
        /* Negative tests with invalid mem subtype 50U */
        result = SDL_ECC_selfTest(SDL_ECC_MEMTYPE_MCU_R5F0_CORE,
                                  50U,
                                  SDL_INJECT_ECC_ERROR_FORCING_1BIT_ONCE,
                                  &injectErrorConfig,
                                  1000);
        if (result == SDL_PASS) {
            UART_printf("\n  Negative test failed on line no: %d \n", __LINE__);
            retVal = -1;
        }
    }

    if (retVal == 0) {
    /* Run one shot test for BTCM0 1 bit error */
    injectErrorConfig.flipBitMask = 0x20;
    /* Note the address is relative to start of ram */
    injectErrorConfig.pErrMem = (uint32_t *)(0x41010100u);
        /* Negative tests with EEC error */
        result = SDL_ECC_selfTest(SDL_ECC_MEMTYPE_MCU_R5F0_CORE,
                                  SDL_ECC_R5F_MEM_SUBTYPE_ATCM0_BANK0_VECTOR_ID,
                                  SDL_INJECT_ECC_NO_ERROR,
                                  &injectErrorConfig,
                                  1000);
        if (result == SDL_PASS) {
            UART_printf("\n  Negative test failed on line no: %d \n", __LINE__);
            retVal = -1;
        }
    }

    if (retVal == 0) {
        /* Pass invalid error configuration. No bits set */
        injectErrorConfig.flipBitMask = 0x80000000;
        /* Set Error address */
        injectErrorConfig.pErrMem = (uint32_t *)(0x100);
        /* Negative tests with invalid mem subtype 50U */
        result = SDL_ECC_selfTest(SDL_ECC_MEMTYPE_MCU_R5F0_CORE,
                                  SDL_ECC_R5F_MEM_SUBTYPE_ATCM0_BANK0_VECTOR_ID,
                                  SDL_INJECT_ECC_ERROR_FORCING_2BIT_ONCE,
                                  &injectErrorConfig,
                                  1000);
        if (result == SDL_PASS) {
            UART_printf("\n  Negative test failed on line no: %d \n", __LINE__);
            retVal = -1;
        }
    }

    if (retVal == 0) {
        /* Negative tests with invalid mem type 133U */
        result = SDL_ECC_getStaticRegisters(133U, &staticRegs);
        if (result == SDL_PASS) {
            UART_printf("\n  Negative test failed on line no: %d \n", __LINE__);
            retVal = -1;
        }
    }

    if (retVal == 0) {
        /* Negative tests with invalid pointer */
        result = SDL_ECC_getStaticRegisters(SDL_ECC_MEMTYPE_MCU_R5F0_CORE, NULL);
        if (result == SDL_PASS) {
            UART_printf("\n  Negative test failed on line no: %d \n", __LINE__);
            retVal = -1;
        }
    }

    if (retVal == 0) {
        result = SDL_ECC_getStaticRegisters (1000U, &staticRegs);
        if (result == SDL_PASS) {
            UART_printf("\n  Negative test failed on line no: %d \n", __LINE__);
            retVal = -1;
        }
    }

    if (retVal == 0) {
        result = SDL_ECC_getStaticRegisters (10U, NULL);
        if (result == SDL_PASS) {
            UART_printf("\n  Negative test failed on line no: %d \n", __LINE__);
            retVal = -1;
        }
    }

    if (retVal == 0) {
        /* Negative tests with invalid pointer */
        SDL_Ecc_AggrIntrSrc intrSrcType;
        result = SDL_ECC_getESMErrorInfo (SDL_ESM_INST_WKUP_ESM0, 0U, NULL, &intrSrcType);
        if (result == SDL_PASS) {
            UART_printf("\n  Negative test failed on line no: %d \n", __LINE__);
              retVal = -1;
        }
    }

    if (retVal == 0) {
        /* Negative tests with invalid pointer */
        SDL_ECC_MemType eccMemType;
        result = SDL_ECC_getESMErrorInfo (SDL_ESM_INST_WKUP_ESM0, 0U, &eccMemType, NULL);
        if (result == SDL_PASS) {
            UART_printf("\n  Negative test failed on line no: %d \n", __LINE__);
            retVal = -1;
        }
    }

    if (retVal == 0) {
        /* Negative tests with invalid pointer */
        SDL_Ecc_AggrIntrSrc intrSrcType;
        SDL_ECC_MemType eccMemType;
        result = SDL_ECC_getESMErrorInfo (SDL_ESM_INST_WKUP_ESM0, 21U, &eccMemType, &intrSrcType);
        if (result == SDL_PASS) {
            UART_printf("\n  Negative test failed on line no: %d \n", __LINE__);
            retVal = -1;
        }
    }

    if (retVal == 0) {
        /* Negative tests with invalid pointer */
        result = SDL_ECC_getErrorInfo(5U, 1U, NULL);
        if (result == SDL_PASS) {
            UART_printf("\n  Negative test failed on line no: %d \n", __LINE__);
            retVal = -1;
        }
    }

    if (retVal == 0) {
        /* Negative tests with invalid pointer */
        SDL_ECC_ErrorInfo_t eccErrorInfo;
        result = SDL_ECC_getErrorInfo(5U, 0U, &eccErrorInfo);
        if (result == SDL_PASS) {
            UART_printf("\n  Negative test failed on line no: %d \n", __LINE__);
            retVal = -1;
        }
    }

    if (retVal == 0) {
        /* Negative tests with invalid pointer */
        SDL_ECC_ErrorInfo_t eccErrorInfo;
        result = SDL_ECC_getErrorInfo(121U, 1U, &eccErrorInfo);
        if (result == SDL_PASS) {
            UART_printf("\n  Negative test failed on line no: %d \n", __LINE__);
            retVal = -1;
        }
    }

    if (retVal == 0) {
        /* Negative tests with invalid pointer */
        result = SDL_ECC_init(0U, NULL);
        if (result == SDL_PASS) {
            UART_printf("\n  Negative test failed on line no: %d \n", __LINE__);
            retVal = -1;
        }
    }

    if ( retVal == 0) {
        UART_printf("\n ECC negative tests: success");
    } else {
        UART_printf("\n ECC negative tests: failed");
    }

    return retVal;
}

/* ECC Error module test */
int32_t ECC_errTest(void)
{
    int32_t testResult;

    testResult = ECC_errNegativeTest();

    return (testResult);
}

/* Nothing past this point */
