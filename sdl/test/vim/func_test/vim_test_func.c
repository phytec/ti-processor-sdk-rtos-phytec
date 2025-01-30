/*
 *   Copyright (c) Texas Instruments Incorporated 2022
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
 *  \file     vim_test_func.c
 *
 *  \brief    This file contains VIM SDL Function test code for R5 core.
 *
 *  \details  VIM SDL API module tests
 **/

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <stdint.h>
#include <soc.h>
#include <sdl_types.h>
#include <src/sdl/sdl_ecc.h>
#include <ti/drv/uart/UART_stdio.h>
#include <test/r5f_startup/interrupt.h>
#include <sdl_exception.h>
#include <sdl_arm_r5_pmu.h>
#include <common/vim_test_main.h>
#include <sdl_esm.h>
#include <test/r5f_startup/sdl_cache.h>
#include <test/r5f_startup/interrupt_priv.h>
#include "src/ip/r5/sdl_mcu_armss_ccmr5.h"
#include <src/sdl/sdl_ccm.h>
#include <ti/osal/TimerP.h>

#if defined (SOC_J721E)
#include <src/sdl/r5/j721e/sdl_soc_ccm.h>
#endif

#if defined (SOC_J7200)
#include <src/sdl/r5/j7200/sdl_soc_ccm.h>
#endif

#if defined (SOC_J721S2)
#include <src/sdl/r5/j721s2/sdl_soc_ccm.h>
#endif

/* Osal API header files */
#include <ti/osal/HwiP.h>


/* ========================================================================== */
/*                                Macros                                      */
/* ========================================================================== */
#define ATCM_BASE_ADDR  (0x00000000U)
#define ATCM_SIZE       (0x00008000U)
#define TEST_AREA_SIZE  (0x00000400U)

#define ATCM_FILL_PATTERN (0xffffffffU)

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
SDL_ESM_config VIM_Test_esmInitConfig_MCU =
{
    .esmErrorConfig = {0u, 3u}, /* Self test error config */
    .enableBitmap = {0xffffffffu, 0xff0fffffu, 0x7fffffffu, 0x00000007u,
                },
     /**< All events enable: except timer and self test  events, and Main ESM output */
    /* Temporarily disabling vim compare error as well*/
    .priorityBitmap = {0xffffffffu, 0xff0fffffu, 0x7fffffffu, 0x00000007u,
                        },
    /**< All events high priority: except timer, selftest error events, and Main ESM output */
    .errorpinBitmap = {0xffffffffu, 0xff0fffffu, 0x7fffffffu, 0x00000007u,
                      },
    /**< All events high priority: except timer, selftest error events, and Main ESM output */
};

SDL_ESM_config VIM_Test_esmInitConfig_WKUP =
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
SDL_ESM_config VIM_Test_esmInitConfig_MAIN =
{
    .esmErrorConfig = {1u, 8u}, /* Self test error config */
    .enableBitmap = {0x00000000u, 0xfffffffbu, 0x7fffffffu, 0xffffffffu,
                 0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                 0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                 0xffffffffu, 0xffffffffu, 0xffffffffu, 0x00000000u,
                 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
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
#if defined (SOC_J721S2) || defined (SOC_J784S4)
SDL_ESM_config VIM_Test_esmInitConfig_MAIN =
{
    .esmErrorConfig = {1u, 8u}, /* Self test error config */
    .enableBitmap = {0x00000000u, 0xffffff0bu, 0x7fffffffu, 0xffffffffu,
                 0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                 0xffffffffu, 0xfffffffbu, 0xffffffffu, 0xffffffffu,
                 0xffff7fffu, 0xffffffffu, 0xffffffffu, 0x00000000u,
                 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                 0xfffbfeffu,
                },
     /**< All events enable: except clkstop events for unused clocks
      *   and PCIE events */
    .priorityBitmap = {0x00000000u, 0xffffff0bu, 0x7fffffffu, 0xffffffffu,
                         0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                         0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                         0xffffffffu, 0xffffffffu, 0xffffffffu, 0x00000000u,
                         0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                         0xfffbfeffu,
                        },
    /**< All events high priority: except clkstop events for unused clocks
     *   and PCIE events */
    .errorpinBitmap = {0x00000000u, 0xffffff0bu, 0x7fffffffu, 0xffffffffu,
                       0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                       0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                       0xffffffffu, 0xffffffffu, 0xffffffffu, 0x00000000u,
                       0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                       0xfffbfeffu,
                      },
    /**< All events high priority: except clkstop for unused clocks
     *   and PCIE events */
};
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
extern int32_t ECC_Test_UTILS_TCMEnable(void);

void ECC_Test_UTILS_initATCM(void)
{
    memset(ATCM_BASE_ADDR, ATCM_FILL_PATTERN, (ATCM_SIZE-TEST_AREA_SIZE));
}

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/* Function prototypes */
void VIM_Test_copyResetVector(void);
int32_t VIM_Test_EsmInitHandlerInit(SDL_ESM_Inst esmInstType);
int32_t VIM_ECC_Test_init (void);
void VIM_Test_VIMDEDInterruptHandler(void);

/* Defines */
#define MCU_R5F_MAX_MEM_SECTIONS   (8u)

static SDL_ECC_MemSubType VIM_Test_R5FCoresubMemTypeList[MCU_R5F_MAX_MEM_SECTIONS] =
{
    SDL_ECC_R5F_MEM_SUBTYPE_ATCM0_BANK0_VECTOR_ID,
    SDL_ECC_R5F_MEM_SUBTYPE_ATCM0_BANK1_VECTOR_ID,
    SDL_ECC_R5F_MEM_SUBTYPE_KS_VIM_RAM_VECTOR_ID,
    SDL_ECC_R5F_MEM_SUBTYPE_B0TCM0_BANK0_VECTOR_ID,
    SDL_ECC_R5F_MEM_SUBTYPE_B0TCM0_BANK1_VECTOR_ID,
    SDL_ECC_R5F_MEM_SUBTYPE_B1TCM0_BANK0_VECTOR_ID,
    SDL_ECC_R5F_MEM_SUBTYPE_B1TCM0_BANK1_VECTOR_ID,
};

static SDL_ECC_InitConfig_t VIM_Test_R5FCoreECCInitConfig =
{
    .numRams = MCU_R5F_MAX_MEM_SECTIONS,
    /**< Number of Rams ECC is enabled  */
    .pMemSubTypeList = &(VIM_Test_R5FCoresubMemTypeList[0]),
    /**< Sub type list  */
};

#ifdef SOC_J721E

#define MCU_CBASS_MAX_MEM_SECTIONS (3u)
#define MAIN_MSMC_AGGR0_MAX_MEM_SECTIONS (4u)

static SDL_ECC_MemSubType VIM_Test_MCUCBASSsubMemTypeList[MCU_CBASS_MAX_MEM_SECTIONS] =
{
    SDL_MCU_CBASS_ECC_AGGR0_IMCU_COR_FW_VBUSP_32B_SOC_FW_SAFEG_WR_RAMECC_RAM_ID,
    SDL_MCU_CBASS_ECC_AGGR0_IMCU_COR_FW_VBUSP_32B_SOC_FW_SAFEG_RD_RAMECC_RAM_ID,
    SDL_MCU_CBASS_ECC_AGGR0_IMCU_COR_FW_VBUSP_32B_SOC_FW_SAFEG_EDC_CTRL_RAM_ID
};

static SDL_ECC_InitConfig_t VIM_Test_MCUCBASSECCInitConfig =
{
    .numRams = MCU_CBASS_MAX_MEM_SECTIONS,
    /**< Number of Rams ECC is enabled  */
    .pMemSubTypeList = &(VIM_Test_MCUCBASSsubMemTypeList[0]),
    /**< Sub type list  */
};

static SDL_ECC_MemSubType VIM_Test_MAINMSMC_A0subMemTypeList[MAIN_MSMC_AGGR0_MAX_MEM_SECTIONS] =
{
    SDL_COMPUTE_CLUSTER0_MSMC_ECC_AGGR0_MSMC_MMR_BUSECC_RAM_ID,
    SDL_COMPUTE_CLUSTER0_MSMC_ECC_AGGR0_RMW2_CACHE_TAG_PIPE_BUSECC_RAM_ID,
    SDL_COMPUTE_CLUSTER0_MSMC_ECC_AGGR0_CLEC_SRAM_RAMECC_RAM_ID,
	SDL_COMPUTE_CLUSTER0_MSMC_ECC_AGGR0_CLEC_J7ES_CLEC_EDC_CTRL_BUSECC_RAM_ID,
};

static SDL_ECC_InitConfig_t VIM_Test_MAINMSMCA0ECCInitConfig =
{
    .numRams = MAIN_MSMC_AGGR0_MAX_MEM_SECTIONS,
    /**< Number of Rams ECC is enabled  */
    .pMemSubTypeList = &(VIM_Test_MAINMSMC_A0subMemTypeList[0]),
    /**< Sub type list  */
};
#endif

#ifdef SOC_J7200

#define MCU_NAVSS0_MAX_MEM_SECTIONS (1u)
#define MAIN_MSMC_AGGR0_MAX_MEM_SECTIONS (1u)
#define SDL_MMCSD0_MAX_MEM_SECTIONS (1u)

static SDL_ECC_MemSubType ECC_Test_SDL_MMCSD0subMemTypeList[ SDL_MMCSD0_MAX_MEM_SECTIONS] =
{
    SDL_MMCSD0_EMMC8SS_16FFC_SDHC_WRAP_TXMEM_RAM_ID,
};

static SDL_ECC_InitConfig_t ECC_Test_SDL_MMCSD0ECCInitConfig =
{
    .numRams =  SDL_MMCSD0_MAX_MEM_SECTIONS,
    /**< Number of Rams ECC is enabled  */
    .pMemSubTypeList = &(ECC_Test_SDL_MMCSD0subMemTypeList[0]),
    /**< Sub type list  */
};

static SDL_ECC_MemSubType ECC_Test_MCU_NAVSS0subMemTypeList[MCU_NAVSS0_MAX_MEM_SECTIONS] =
{
    SDL_VC_MAIN_HC_ECC_AGGR5_J7VC_HC2_CBASS_J7VC_RC_TO_HC2_VBUSM_M2M_BRIDGE_J7VC_HC2_CBASS_J7VC_RC_TO_HC2_VBUSM_M2M_BRIDGE_SRC_EDC_CTRL_BUSECC_RAM_ID,
};


static SDL_ECC_InitConfig_t ECC_Test_MCU_NAVSS0ECCInitConfig =
{
    .numRams = MCU_NAVSS0_MAX_MEM_SECTIONS,
    /**< Number of Rams ECC is enabled  */
    .pMemSubTypeList = &(ECC_Test_MCU_NAVSS0subMemTypeList[0]),
    /**< Sub type list  */
};

#endif

HwiP_Handle VIM_Test_EsmHiHwiPHandle;
HwiP_Handle VIM_Test_EsmLoHwiPHandle;
HwiP_Handle VIM_Test_EsmCfgHwiPHandle;

extern void *_resetvectors;
#define VECTOR_SIZE 0x100
#define APP_MAX_PROFILE_SIZE 16

void VIM_Test_cpuCacheInv(const void * addr, int32_t size)
{
    SDL_armR5CacheInv(addr, size);
}

void VIM_Test_cpuCacheWbInv(const void * addr, int32_t size)
{
    SDL_armR5CacheWbInv(addr, size);
}

void VIM_Test_copyResetVector(void)
{
    /* Invalidate cache */
    VIM_Test_cpuCacheInv(0, VECTOR_SIZE);

    /* copy the vectors into 0 location */
    /* Vectors are always assumed to be at 0 for R5 */
    memcpy(0, &_resetvectors, VECTOR_SIZE);

    /* Write back cache */
    VIM_Test_cpuCacheWbInv(0, VECTOR_SIZE);
}

/* This is the list of exception handle and the parameters */
const SDL_R5ExptnHandlers VIM_Test_R5ExptnHandlers =
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

void VIM_Test_undefInstructionExptnCallback(void)
{
    UART_printf("\n Undefined Instruction exception");
}

void VIM_Test_swIntrExptnCallback(void)
{
    UART_printf("\n Software interrupt exception");
}

void VIM_Test_prefetchAbortExptnCallback(void)
{
    UART_printf("\n Prefetch Abort exception");
}
void VIM_Test_dataAbortExptnCallback(void)
{
    UART_printf("\n Data Abort exception");
}
void VIM_Test_irqExptnCallback(void)
{
    UART_printf("\n Irq exception");
}

void VIM_Test_fiqExptnCallback(void)
{
    UART_printf("\n Fiq exception");
}

void VIM_Test_exceptionInit(void)
{

    SDL_EXCEPTION_CallbackFunctions_t exceptionCallbackFunctions =
            {
             .udefExptnCallback = VIM_Test_undefInstructionExptnCallback,
             .swiExptnCallback = VIM_Test_swIntrExptnCallback,
             .pabtExptnCallback = VIM_Test_prefetchAbortExptnCallback,
             .dabtExptnCallback = VIM_Test_dataAbortExptnCallback,
             .irqExptnCallback = VIM_Test_irqExptnCallback,
             .fiqExptnCallback = VIM_Test_fiqExptnCallback,
            };

    /* Initialize SDL exception handler */
    SDL_EXCEPTION_init(&exceptionCallbackFunctions);
    /* Register SDL exception handler */
    Intc_RegisterExptnHandlers(&VIM_Test_R5ExptnHandlers);

    return;
}
static uint32_t arg;
/*********************************************************************
* @fn      VIM_ECC_Test_init
*
* @brief   Initializes Software Diagostics Test Framework
*
* @param   None
*
* @return    0 : Success; < 0 for failures
*/
int32_t VIM_ECC_Test_init (void)
{
    int32_t retValue=0;
    void *ptr = (void *)&arg;
    SDL_ErrType_t result;
    SDL_ECC_staticRegs staticRegs;

    /* Enable ATCM and BTCM */
    ECC_Test_UTILS_TCMEnable();

    /* Initialize ATCM */
    ECC_Test_UTILS_initATCM();

    Board_initCfg boardCfg;

    boardCfg = BOARD_INIT_PINMUX_CONFIG |
               BOARD_INIT_UART_STDIO;
    Board_init(boardCfg);

    UART_printf("\nVIM_ECC_Test_init: UART ready to print, proceeding with ECC_Test init \n");

    UART_printf("\nVIM_ECC_Test_init: Profile init complete \n");

    /* Call Init for ECC_Test software structures */

    /* Copy reset vector to 0 location */
    VIM_Test_copyResetVector();

    UART_printf("\nVIM_ECC_Test_init: Copy Reset vector complete \n");

    /* Initialise exception handler */
    VIM_Test_exceptionInit();

    UART_printf("\nVIM_ECC_Test_init: Exception init complete \n");

    /* Register VIM DED interrupt handler */
    SDL_ECC_registerVIMDEDHandler(&VIM_Test_VIMDEDInterruptHandler);

    UART_printf("\nVIM_ECC_Test_init: register VIM DED Handler complete \n");

    if (retValue == 0) {
        /* Initialize MCU ESM module */
        result = SDL_ESM_init(SDL_ESM_INST_MCU_ESM0, &VIM_Test_esmInitConfig_MCU, SDL_ESM_applicationCallbackFunction, ptr);
        if (result != SDL_PASS) {
            /* print error and quit */
             UART_printf("VIM_ECC_Test_init: Error initializing MCU ESM: result = %d\n", result);

            retValue = -1;
        } else {
            UART_printf("\nVIM_ECC_Test_init: Init MCU ESM complete \n");
        }
    }

    if (retValue == 0) {
        /* Initialize WKUP ESM module */
        result = SDL_ESM_init(SDL_ESM_INST_WKUP_ESM0, &VIM_Test_esmInitConfig_WKUP,SDL_ESM_applicationCallbackFunction,ptr);
        if (result != SDL_PASS) {
            /* print error and quit */
             UART_printf("VIM_ECC_Test_init: Error initializing WKUP ESM: result = %d\n", result);

            retValue = -1;
        } else {
            UART_printf("\nVIM_ECC_Test_init: Init WKUP ESM complete \n");
        }
    }

    if (retValue == 0) {
        /* Initialize MAIN ESM module */
        result = SDL_ESM_init(SDL_ESM_INST_MAIN_ESM0, &VIM_Test_esmInitConfig_MAIN,SDL_ESM_applicationCallbackFunction,ptr);
        if (result != SDL_PASS) {
            /* print error and quit */
            UART_printf("VIM_ECC_Test_init: Error initializing MAIN ESM: result = %d\n", result);

            retValue = -1;
        } else {
            UART_printf("\nVIM_ECC_Test_init: Init MAIN ESM complete \n");
        }
    }

    if (retValue == 0) {
        /* Initialize ECC */
        result = SDL_ECC_init(SDL_ECC_MEMTYPE_MCU_R5F0_CORE, &VIM_Test_R5FCoreECCInitConfig);
        if (result != SDL_PASS) {
            /* print error and quit */
            UART_printf("VIM_ECC_Test_init: Error initializing R5F core ECC: result = %d\n", result);

            retValue = -1;
        } else {
            UART_printf("\nVIM_ECC_Test_init: R5F Core ECC Init complete \n");
        }
    }

    if (retValue == 0) {
        /* Read back the static registers */
        result = SDL_ECC_getStaticRegisters(SDL_ECC_MEMTYPE_MCU_R5F0_CORE, &staticRegs);
        if (result != SDL_PASS) {
            /* print error and quit */
            UART_printf("VIM_ECC_Test_init: Error reading the static registers: result = %d\n");

            retValue = -1;
        } else {
            UART_printf("\nVIM_ECC_Test_init: R5F Memtype Register Readback successful \n");
        }
    }
#if defined (SOC_J721E)
    if (retValue == 0) {
        /* Initialize ECC */
        result = SDL_ECC_init(SDL_ECC_MEMTYPE_MCU_CBASS_ECC_AGGR0, &VIM_Test_MCUCBASSECCInitConfig);
        if (result != SDL_PASS) {
            /* print error and quit */
             UART_printf("VIM_ECC_Test_init: Error initializing R5F core ECC: result = %d\n", result);

            retValue = -1;
        } else {
            UART_printf("\nVIM_ECC_Test_init: MCU CBASS ECC Init complete \n");
        }
    }

    if (retValue == 0) {
        /* Read back the static registers */
        result = SDL_ECC_getStaticRegisters(SDL_ECC_MEMTYPE_MCU_CBASS_ECC_AGGR0, &staticRegs);
        if (result != SDL_PASS) {
            /* print error and quit */
            UART_printf("VIM_ECC_Test_init: Error reading the static registers: result = %d\n");

            retValue = -1;
        } else {
            UART_printf("\nVIM_ECC_Test_init: R5F Memtype Register Readback successful \n");
        }
    }
#endif

#if defined (SOC_J7200)
    if (retValue == 0) {
        /* Initialize ECC */
        result = SDL_ECC_init(SDL_ECC_VC_MAIN_HC_ECC_AGGR5_ECC_AGGR, &ECC_Test_MCU_NAVSS0ECCInitConfig);
        if (result != SDL_PASS) {
            /* print error and quit */
             UART_printf("ECC_Test_init: Error initializing MCU_NAVSS0 ECC: result = %d\n", result);

            retValue = -1;
        } else {
            UART_printf("\nECC_Test_init: MCU_NAVSS0 ECC Init complete \n");
        }
    }

    if (retValue == 0) {
        /* Initialize ECC */
        result = SDL_ECC_init(SDL_ECC_MMCSD0_0_ECC_AGGR, &ECC_Test_SDL_MMCSD0ECCInitConfig);
        if (result != SDL_PASS) {
            /* print error and quit */
             UART_printf("ECC_Test_init: Error initializing MCU_NAVSS0 ECC: result = %d\n", result);

            retValue = -1;
        } else {
            UART_printf("\nECC_Test_init: MCU_NAVSS0 ECC Init complete \n");
        }
    }

    if (retValue == 0) {
        /* Read back the static registers */
        result = SDL_ECC_getStaticRegisters(SDL_ECC_VC_MAIN_HC_ECC_AGGR5_ECC_AGGR, &staticRegs);
        if (result != SDL_PASS) {
            /* print error and quit */
            UART_printf("ECC_Test_init: Error reading the MCU_NAVSS0 ECC static registers: result = %d\n");

            retValue = -1;
        } else {
            UART_printf("\nECC_Test_init: MCU_NAVSS0 Memtype Register Readback successful \n");
        }
    }
#endif

#if defined (SOC_J721E)
    /* Initialize an ECC aggregator type that requires mapping.
     * This example only shows MSMC_AGGR0 instance.*/
    if (retValue == 0) {
        /* Initialize ECC */
        result = SDL_ECC_init(SDL_ECC_MEMTYPE_MAIN_MSMC_AGGR0, &VIM_Test_MAINMSMCA0ECCInitConfig);
        if (result != SDL_PASS) {
            /* print error and quit */
             UART_printf("VIM_ECC_Test_init: Error initializing R5F core ECC: result = %d\n", result);

            retValue = -1;
        } else {
            UART_printf("\nVIM_ECC_Test_init: MCU CBASS ECC Init complete \n");
        }
    }

    /* Test the path for MSMC mapping */
    if (retValue == 0) {
        /* Read back the static registers */
        result = SDL_ECC_getStaticRegisters(SDL_ECC_MEMTYPE_MAIN_MSMC_AGGR0, &staticRegs);
        if (result != SDL_PASS) {
            /* print error and quit */
            UART_printf("VIM_ECC_Test_init: Error reading the static registers: result = %d\n");

            retValue = -1;
        } else {
            UART_printf("\nVIM_ECC_Test_init: R5F Memtype Register Readback successful \n");
        }
    }
#endif
    if (retValue == 0) {
        /* Initialize ECC callbacks within the MCU ESM */
        result = SDL_ECC_initEsm(SDL_ESM_INST_MCU_ESM0);
        if (result != SDL_PASS) {
            /* print error and quit */
             UART_printf("VIM_ECC_Test_init: Error initializing ECC callback for MCU ESM: result = %d\n", result);

            retValue = -1;
        } else {
            UART_printf("\nVIM_ECC_Test_init: ECC Callback Init complete for MCU ESM \n");
        }
    }

    if (retValue == 0) {
        /* Initialize ECC callbacks within the Main ESM */
        result = SDL_ECC_initEsm(SDL_ESM_INST_MAIN_ESM0);
        if (result != SDL_PASS) {
            /* print error and quit */
             UART_printf("VIM_ECC_Test_init: Error initializing ECC callback for Main ESM: result = %d\n", result);

            retValue = -1;
        } else {
            UART_printf("\nVIM_ECC_Test_init: ECC Callback Init complete for Main ESM \n");
        }
    }

    if (retValue == 0) {
        /* Initialize ECC callbacks within the WKUP ESM */
        result = SDL_ECC_initEsm(SDL_ESM_INST_WKUP_ESM0);
        if (result != SDL_PASS) {
            /* print error and quit */
             UART_printf("VIM_ECC_Test_init: Error initializing ECC callback for WKUP ESM: result = %d\n", result);

            retValue = -1;
        } else {
            UART_printf("\nVIM_ECC_Test_init: ECC Callback Init complete for WKUP ESM \n");
        }
    }

    if (retValue == 0) {
        /* Initialize VIM ECC memory ; This is specifically for VIM in lockstep*/
        /* All other memories are auto initialized by hardware */
        result = SDL_ECC_initMemory(SDL_ECC_MEMTYPE_MCU_R5F0_CORE, SDL_ECC_R5F_MEM_SUBTYPE_KS_VIM_RAM_VECTOR_ID);
        if (result != SDL_PASS) {
             /* print error and quit */
              UART_printf("VIM_ECC_Test_init: Error initializing ECC memory: retValue = %d\n", retValue);
             retValue = -1;
         } else {
             UART_printf("\nVIM_ECC_Test_init: ECC Memory init complete \n");
         }
    }

    if (retValue == 0) {
#if defined (SOC_J721E)
        result = SDL_ECC_initMemory(SDL_ECC_MEMTYPE_MCU_R5F0_CORE, SDL_MCU_R5FSS0_CORE0_ECC_AGGR_CPU0_ECC_AGGR_EDC_CTRL_ECC_TYPE);
#elif (SOC_J7200)
        result = SDL_ECC_initMemory(SDL_ECC_MEMTYPE_MCU_R5F0_CORE, SDL_MCU_R5FSS0_CPU0_ITAG_RAM1_RAM_ID);
#elif (SOC_J721S2)
        result = SDL_ECC_initMemory(SDL_ECC_MEMTYPE_MCU_R5F0_CORE, SDL_MCU_R5FSS0_PULSAR_SL_CPU0_ECC_AGGR_CPU0_ITAG_RAM1_RAM_ID);
#endif
        if (result != SDL_PASS) {
             /* print error and quit */
              UART_printf("VIM_ECC_Test_init: Error initializing ECC memory: retValue = %d\n", retValue);
             retValue = -1;
         } else {
             UART_printf("\nVIM_ECC_Test_init: ECC Memory init complete \n");
         }
    }

    if (retValue == 0) {
        result = SDL_ECC_initMemory(SDL_ECC_MEMTYPE_MCU_R5F0_CORE, SDL_MCU_R5F0_SUB_MEMORY);
        if (result != SDL_PASS) {
             /* print error and quit */
              UART_printf("VIM_ECC_Test_init: Error initializing ECC memory: retValue = %d\n", retValue);
             retValue = -1;
         } else {
             UART_printf("\nVIM_ECC_Test_init: ECC Memory init complete \n");
         }
    }

    return retValue;
}

uint32_t SDL_getTime(void)
{
    return(SDL_R5PMU_readCntr(SDL_ARM_R5_PMU_CYCLE_COUNTER_NUM));
}

/* Local functions */
void VIM_Test_readVIMRAM(uint32_t offset);
int32_t VIM_Test_ECC_waitDEDTrigger(uint64_t timeCount);

void VIM_Test_ECC_dummyFunction(void);


static bool VIM_Test_ECC_DEDTriggerFlag = false;

/*********************************************************************
 *
 * @brief   indicate DED trigger
 *
 *
 * @return  0 : Success; < 0 for failures
 */
void VIM_Test_ECC_indicateDEDTrigger(void)
{
    VIM_Test_ECC_DEDTriggerFlag = true;
}


int32_t VIM_Test_ECC_waitDEDTrigger(uint64_t timeCount)
{
    int32_t retVal = 0;
    uint32_t startTimeStamp, curTimeStamp, lastTimeStamp, deltaTime;
    uint64_t elapsedTime = 0u;

    startTimeStamp = SDL_getTime();
    lastTimeStamp = startTimeStamp;
    while(VIM_Test_ECC_DEDTriggerFlag != true) {
        curTimeStamp = SDL_getTime();
        deltaTime = curTimeStamp-lastTimeStamp;
        lastTimeStamp = curTimeStamp;
        elapsedTime += deltaTime;
        if (elapsedTime > timeCount) {
            break;
        }
    }

    if (VIM_Test_ECC_DEDTriggerFlag == true ) {
        VIM_Test_ECC_DEDTriggerFlag = false;
    } else {
        retVal = -1;
    }
    return retVal;
}

__attribute__((section(".VIM_Test_ecc_testcodesection"))) void VIM_Test_ECC_dummyFunction(void)
{
    volatile int i;

    i = 10;
    return;
}

/*********************************************************************
 * @fn      VIM_Test_runECC2BitVIMRAMDEDvector
 *
 * @brief   Execute ECC 2 bit Inject test with VIM RAM,but expect to
 *          trigger VIM DED vector
 *
 * @param   None
 *
 * @return  0 : Success; < 0 for failures
 */
int32_t VIM_Test_runECC2BitVIMRAMDEDvector(void)
{
    SDL_ErrType_t result = 0;
    int32_t retVal=0;

    SDL_ECC_InjectErrorConfig_t injectErrorConfig;

    UART_printf("\n VIM RAM Double bit error inject: starting");

    /* Run one shot test for VIM RAM 2 bit error */
    /* Inject the error into the interrupt associated with MCU Timer 0 */
    /* Timer 0 interrupt needs to happen to get the VIM DED error to trigger */
    /* This is expected to trigger the DED vector */
    injectErrorConfig.pErrMem = (uint32_t *)(0x40F82098u);
    injectErrorConfig.flipBitMask = 0x101;

    result = SDL_ECC_injectError(SDL_ECC_MEMTYPE_MCU_R5F0_CORE,
                                 SDL_ECC_R5F_MEM_SUBTYPE_KS_VIM_RAM_VECTOR_ID,
                                 SDL_INJECT_ECC_ERROR_FORCING_2BIT_ONCE,
                                 &injectErrorConfig);
    if (result != SDL_PASS ) {
        UART_printf("\n VIM RAM Double bit error inject: at pErrMem 0x%p: fixed location once test failed",
                    injectErrorConfig.pErrMem);
       retVal = -1;
    } else {
        UART_printf("\n VIM RAM Double bit error inject: pErrMem 0x%p fixed location once test complete",
                    injectErrorConfig.pErrMem);
    }
    UART_printf("\n waiting for DED Vector Trigger");
    if (retVal == 0) {
        retVal = VIM_Test_ECC_waitDEDTrigger(0x100000000ull);
    }
    if (retVal == 0) {
        UART_printf("\n DED Vector Trigger: Success");
    } else {
        UART_printf("\n DED Vector Trigger: Fail");
    }
    return retVal;
}

__attribute__((interrupt, section(".text.hwi"),target("arm"), aligned(32))) void VIM_Test_VIMDEDInterruptHandler(void);


/*********************************************************************
* @fn      VIM_Test_VIMDEDInterruptHandler
*
* @brief   Sample DED Interrupt handler
*
* @param   None
*
* @return   None
*/
void VIM_Test_VIMDEDInterruptHandler(void)
{
    /* Action to be taken by customer */
    UART_printf("\n  VIM DED error happened \n");
    UART_printf("  Take action \n");

    /* clear appropriate interrupt events */

    VIM_Test_ECC_indicateDEDTrigger();
}

__attribute__((interrupt("IRQ"), target("arm")))    void vimIsr(void);
void vimIsr(void)
{
    IntrFuncPtr        fxnPtr;
    volatile uint32_t  intNum, pri;
    UART_printf("\n Interrupt occured");
    /* Process a pending FIQ interrupt before a pending IRQ interrupt */
    if( ( SDL_VIM_getActivePendingIntr( (SDL_vimRegs *)(uintptr_t)gVimBaseAddr, SDL_VIM_INTR_MAP_FIQ, (uint32_t *)&intNum, (uint32_t *)&pri ) == 0 )       ||
        ( SDL_VIM_getActivePendingIntr( (SDL_vimRegs *)(uintptr_t)gVimBaseAddr, SDL_VIM_INTR_MAP_IRQ, (uint32_t *)&intNum, (uint32_t *)&pri ) == 0 ) )
    {
        /* Clear pulse-type interrupt before calling ISR */
        if( intrSrcType[intNum] == (uint32_t)SDL_VIM_INTR_TYPE_PULSE )
        {
            SDL_VIM_clrIntrPending( (SDL_vimRegs *)(uintptr_t)gVimBaseAddr, intNum );
            UART_printf("\n Pulse-type interrupt cleared");
        }
        /* Call ISR */
        if( fxnArray[intNum] != NULL )
        {
            fxnPtr = fxnArray[intNum];
            fxnPtr(argArray[intNum]);
        }
        /* Clear level-type interrupt after calling ISR */
        if( intrSrcType[intNum] == (uint32_t)SDL_VIM_INTR_TYPE_LEVEL )
        {
            SDL_VIM_clrIntrPending( (SDL_vimRegs *)(uintptr_t)gVimBaseAddr, intNum );
            UART_printf("\n Level-type interrupt cleared");
        }
        /* Acknowledge interrupt servicing */
        SDL_VIM_ackIntr( (SDL_vimRegs *)(uintptr_t)gVimBaseAddr, (SDL_VimIntrMap)intrMap[intNum] );
    }
}

static void timerFxn(uintptr_t arg)
{
    return;
}

static void VIM_Test_startTimer(void)
{
    TimerP_Params  timerParams;
    TimerP_Handle  timerHandle;

    /* Initialize the parameters */
    TimerP_Params_init(&timerParams);
    timerParams.startMode = (uint32_t)TimerP_StartMode_USER;
    timerParams.periodType = (uint32_t)TimerP_PeriodType_MICROSECS;
    timerParams.period    = 1000000u;
    timerHandle = TimerP_create(TimerP_ANY, (TimerP_Fxn)&timerFxn, &timerParams);

    if ( timerHandle != NULL_PTR)
    {
        /* start the timer */
        (void)TimerP_start(timerHandle);
    }

}

static int32_t VIM_sdlFuncTest(void)
{
    int32_t result;
    int32_t retVal = 0;

    SDL_vimRegs *pRegs;
    uint32_t intrNum = SDLR_R5FSS0_CORE0_INTR_ESM0_ESM_INT_HI_LVL_0;
    /* initialize the address */
    pRegs        = (SDL_vimRegs *)(uintptr_t)SDL_MCU_DOMAIN_VIM_BASE_ADDR;

    UART_printf("\n VIM SDL API tests: starting");

    /* Timer is used to trigger the interrupt vector used for VIM DED Handler error */
    VIM_Test_startTimer();

    /* This needs to be last as it is destructive */
    if (retVal == 0) {
        result = VIM_Test_runECC2BitVIMRAMDEDvector();
        if (result != SDL_PASS) {
            retVal = -1;
            UART_printf("\n VIM_Test_runECC2BitVIMRAMDEDvector has failed...");
        }
    }
    if ( retVal == 0) {
        UART_printf("\n VIM SDL API tests: success");
    } else {
        UART_printf("\n VIM SDL API tests: failed");
    }

    if (retVal == SDL_APP_TEST_PASS)
    {
        result = SDL_VIM_cfgIntr(pRegs, intrNum, 0xFU, 0x0U, 0x1U, (uint32_t)&vimIsr);

        if (result != SDL_PASS)
        {
            retVal = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM API test failed on line no: %d \n", __LINE__);
        }
    }
    if (retVal == SDL_APP_TEST_PASS)
    {
        result = SDL_VIM_verifyCfgIntr(pRegs, intrNum, 0xFU, 0x0U, 0x1U, (uint32_t)&vimIsr);

        if (result != SDL_PASS)
        {
            retVal = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM API test failed on line no: %d \n", __LINE__);
        }
    }
    if (retVal == SDL_APP_TEST_PASS)
    {
        result = SDL_VIM_setIntrPending(pRegs, intrNum);

        if (result != SDL_PASS)
        {
            retVal = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM API test failed on line no: %d \n", __LINE__);
        }
    }
    if (retVal == SDL_APP_TEST_PASS)
    {
        result = SDL_VIM_setIntrEnable(pRegs, intrNum, (bool) true);

        if (result != SDL_PASS)
        {
            retVal = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM API test failed on line no: %d \n", __LINE__);
        }
    }
    if (retVal == SDL_APP_TEST_PASS)
    {
        SDL_vimStaticRegs staticRegs;
        result = SDL_VIM_getStaticRegs(pRegs, &staticRegs);

        if (result != SDL_PASS)
        {
            retVal = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM API test failed on line no: %d \n", __LINE__);
        }
    }

    return retVal;
}

/* VIM Function module test */
int32_t VIM_funcTest(void)
{
    int32_t testResult;

    testResult = VIM_ECC_Test_init();

    if (testResult != 0)
    {
        UART_printf("\n VIM SDL API tests: unsuccessful");
        return SDL_EFAIL;
    }

    testResult = VIM_sdlFuncTest();


    return (testResult);
}

/* Nothing past this point */
