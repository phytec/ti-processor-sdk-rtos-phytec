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
 *  \file    esm_ecc_trig.c
 *
 *  \brief    This file contains functions that provide input event triggers
 *            for the Error Signaling Module (ESM) application.
 *
 *  \details  ESM Safety Example module tests
 **/

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <stdint.h>
#include <soc.h>
#include <sdl_types.h>
#include <src/sdl/sdl_ecc.h>
#include <ti/drv/uart/UART_stdio.h>
#include <sdl_cache.h>
#include <interrupt.h>
#include <sdl_exception.h>
#include <sdl_arm_r5_pmu.h>
#include "main.h"
#include <sdl_esm.h>

/* ========================================================================== */
/*                                Macros                                      */
/* ========================================================================== */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

#if defined (SOC_J721E) || defined (SOC_J7200)
SDL_ESM_config ECC_Test_esmInitConfig_MAIN =
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
    .priorityBitmap = {0x00000000u, 0xfffefffbu, 0x7fffffffu, 0xffffffffu,
                         0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                         0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                         0xffffffffu, 0xffffffffu, 0xffffffffu, 0x00000000u,
                         0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                         0xffffffffu,
                        },
    /**< All events high priority: except clkstop events for unused clocks
     *   and PCIE events */
    .errorpinBitmap = {0x00000000u, 0xfffefffbu, 0x7fffffffu, 0xffffffffu,
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
                 0xfffbfeffu,
                },
     /**< All events enable: except clkstop events for unused clocks
      *   and PCIE events */
    .priorityBitmap = {0x00000000u, 0xfffeffdbu, 0x7fffffffu, 0xffffffffu,
                         0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                         0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                         0xffffffffu, 0xffffffffu, 0xffffffffu, 0x00000000u,
                         0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                         0xfffbfeffu,
                        },
    /**< All events high priority: except clkstop events for unused clocks
     *   and PCIE events */
    .errorpinBitmap = {0x00000000u, 0xfffeffdbu, 0x7fffffffu, 0xffffffffu,
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

#if defined (SOC_J784S4)
SDL_ESM_config ECC_Test_esmInitConfig_MAIN =
{
    .esmErrorConfig = {1u, 8u}, /* Self test error config */
    .enableBitmap = {0x00000000u, 0xffffffdbu, 0x7fffffffu, 0xffffffffu,
                 0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                 0xffffffffu, 0xfffffffbu, 0xffffffffu, 0xffffffffu,
                 0xffff7fffu, 0xffffffffu, 0xffffffffu, 0x00000000u,
                 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                 0xfffbfeffu,
                },
     /**< All events enable: except clkstop events for unused clocks
      *   and PCIE events */
    .priorityBitmap = {0x00000000u, 0xfffeffdbu, 0x7fffffffu, 0xffffffffu,
                         0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                         0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                         0xffffffffu, 0xffffffffu, 0xffffffffu, 0x00000000u,
                         0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                         0xfffbfeffu,
                        },
    /**< All events high priority: except clkstop events for unused clocks
     *   and PCIE events */
    .errorpinBitmap = {0x00000000u, 0xfffeffdbu, 0x7fffffffu, 0xffffffffu,
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

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/* Defines */
#define MAIN_MSMC_AGGR0_MAX_MEM_SECTIONS (4u)


int32_t ECC_Test_EsmInitHandlerInit(SDL_ESM_Inst esmInstType);
int32_t nESM_ECC_Example_init (void);

#if defined (SOC_J721E)
static SDL_ECC_MemSubType ECC_Test_MAINMSMC_A0subMemTypeList[MAIN_MSMC_AGGR0_MAX_MEM_SECTIONS] =
{
    SDL_COMPUTE_CLUSTER0_MSMC_ECC_AGGR0_MSMC_MMR_BUSECC_RAM_ID,
	SDL_COMPUTE_CLUSTER0_MSMC_ECC_AGGR0_RMW2_CACHE_TAG_PIPE_BUSECC_RAM_ID,
    SDL_COMPUTE_CLUSTER0_MSMC_ECC_AGGR0_CLEC_SRAM_RAMECC_RAM_ID,
	SDL_COMPUTE_CLUSTER0_MSMC_ECC_AGGR0_CLEC_J7ES_CLEC_EDC_CTRL_BUSECC_RAM_ID
};


static SDL_ECC_InitConfig_t ECC_Test_MAINMSMCA0ECCInitConfig =
{
    .numRams = MAIN_MSMC_AGGR0_MAX_MEM_SECTIONS,
    /**< Number of Rams ECC is enabled  */
    .pMemSubTypeList = &(ECC_Test_MAINMSMC_A0subMemTypeList[0]),
    /**< Sub type list  */
};
#endif

#if defined (SOC_J7200)

static SDL_ECC_MemSubType ECC_Test_MAINMSMC_A0subMemTypeList[MAIN_MSMC_AGGR0_MAX_MEM_SECTIONS] =
{
    SDL_COMPUTE_CLUSTER0_MSMC_MMR_BUSECC_RAM_ID,
	SDL_COMPUTE_CLUSTER0_POSTARB_PIPE_CFG_BUSECC_RAM_ID,
    SDL_COMPUTE_CLUSTER0_EN_MSMC_P0_BUSECC_DATA_RAM_ID,
	SDL_COMPUTE_CLUSTER0_EN_MSMC_P0_BUSECC_0_RAM_ID,
};


static SDL_ECC_InitConfig_t ECC_Test_MAINMSMCA0ECCInitConfig =
{
    .numRams = MAIN_MSMC_AGGR0_MAX_MEM_SECTIONS,
    /**< Number of Rams ECC is enabled  */
    .pMemSubTypeList = &(ECC_Test_MAINMSMC_A0subMemTypeList[0]),
    /**< Sub type list  */
};
#endif

#if defined (SOC_J721S2)

static SDL_ECC_MemSubType ECC_Test_MAINMSMC_A0subMemTypeList[MAIN_MSMC_AGGR0_MAX_MEM_SECTIONS] =
{
    SDL_COMPUTE_CLUSTER0_COMPUTE_CLUSTER_J7AE_MSMC_EN_DRU_MSMC_EN_4MB_WRAP_MSMC_4MB_MSMC_WRAP_ECC_AGGR0_MSMC_MMR_BUSECC_RAM_ID,
	SDL_COMPUTE_CLUSTER0_COMPUTE_CLUSTER_J7AE_MSMC_EN_DRU_MSMC_EN_4MB_WRAP_MSMC_4MB_MSMC_WRAP_ECC_AGGR0_POSTARB_PIPE_CFG_BUSECC_RAM_ID,
    SDL_COMPUTE_CLUSTER0_COMPUTE_CLUSTER_J7AE_MSMC_EN_DRU_MSMC_EN_4MB_WRAP_MSMC_4MB_MSMC_WRAP_ECC_AGGR0_EN_MSMC_P0_BUSECC_DATA_RAM_ID,
	SDL_COMPUTE_CLUSTER0_COMPUTE_CLUSTER_J7AE_MSMC_EN_DRU_MSMC_EN_4MB_WRAP_MSMC_4MB_MSMC_WRAP_ECC_AGGR0_EN_MSMC_P0_BUSECC_0_RAM_ID,
};


static SDL_ECC_InitConfig_t ECC_Test_MAINMSMCA0ECCInitConfig =
{
    .numRams = MAIN_MSMC_AGGR0_MAX_MEM_SECTIONS,
    /**< Number of Rams ECC is enabled  */
    .pMemSubTypeList = &(ECC_Test_MAINMSMC_A0subMemTypeList[0]),
    /**< Sub type list  */
};
#endif

#if defined (SOC_J784S4)

static SDL_ECC_MemSubType ECC_Test_MAINMSMC_A0subMemTypeList[MAIN_MSMC_AGGR0_MAX_MEM_SECTIONS] =
{
	SDL_COMPUTE_CLUSTER0_COMPUTE_CLUSTER_J7AHP_MSMC_EN_DRU_MSMC_EN_8MB_WRAP_MSMC_8MB_MSMC_WRAP_ECC_AGGR0_MSMC_LAMBDA3_WRAP_MSMC_QOR_MSMC_CORE_MMR_EDC_CTRL_0_RAM_ID,
	SDL_COMPUTE_CLUSTER0_COMPUTE_CLUSTER_J7AHP_MSMC2_WRAP_ECC_AGGR0_POSTARB_PIPE_CFG_BUSECC_RAM_ID,
    SDL_COMPUTE_CLUSTER0_COMPUTE_CLUSTER_J7AHP_MSMC_EN_DRU_MSMC_EN_8MB_WRAP_MSMC_8MB_MSMC_WRAP_ECC_AGGR0_EN_MSMC_P0_BUSECC_DATA_RAM_ID,
	SDL_COMPUTE_CLUSTER0_COMPUTE_CLUSTER_J7AHP_MSMC_EN_DRU_MSMC_EN_8MB_WRAP_MSMC_8MB_MSMC_WRAP_ECC_AGGR0_EN_MSMC_P0_BUSECC_0_RAM_ID,
};


static SDL_ECC_InitConfig_t ECC_Test_MAINMSMCA0ECCInitConfig =
{
    .numRams = MAIN_MSMC_AGGR0_MAX_MEM_SECTIONS,
    /**< Number of Rams ECC is enabled  */
    .pMemSubTypeList = &(ECC_Test_MAINMSMC_A0subMemTypeList[0]),
    /**< Sub type list  */
};
#endif

static uint32_t arg;
/*********************************************************************
* @fn      nESM_ECC_Example_init
*
* @param   None
*
* @return    0 : Success; < 0 for failures
*/
int32_t nESM_ECC_Example_init (void)
{
    int32_t retValue=0;
    void *ptr = (void *)&arg;
    SDL_ErrType_t result;

    Board_initCfg boardCfg;

    boardCfg = BOARD_INIT_PINMUX_CONFIG |
               BOARD_INIT_UART_STDIO;
    Board_init(boardCfg);

    if (retValue == 0) {
        /* Initialize MAIN ESM module */
        result = SDL_ESM_init(SDL_ESM_INST_MAIN_ESM0, &ECC_Test_esmInitConfig_MAIN, SDL_ESM_applicationCallbackFunction, ptr);
        if (result != SDL_PASS) {
            /* print error and quit */
            UART_printf("ESM_ECC_Example_init: Error initializing MAIN ESM: result = %d\n", result);

            retValue = -1;
        } else {
            UART_printf("\nESM_ECC_Example_init: Init MAIN ESM complete \n");
        }
    }

	if (retValue == 0) {
        /* Initialize ECC */
        result = SDL_ECC_init(SDL_ECC_MEMTYPE_MAIN_MSMC_AGGR0, &ECC_Test_MAINMSMCA0ECCInitConfig);
        if (result != SDL_PASS) {
            /* print error and quit */
             UART_printf("SDTF_init: Error initializing R5F core ECC: result = %d\n", result);

            retValue = -1;
        } else {
            UART_printf("\nSDTF_init: MCU MSMC ECC Init complete \n");
        }
    }	

    if (retValue == 0) {
        /* Initialize ECC callbacks within the Main ESM */
        result = SDL_ECC_initEsm(SDL_ESM_INST_MAIN_ESM0);
        if (result != SDL_PASS) {
            /* print error and quit */
             UART_printf("nESM_ECC_Example_init: Error initializing ECC callback for Main ESM: result = %d\n", result);

            retValue = -1;
        } else {
            UART_printf("\nESM_ECC_Example_init: ECC Callback Init complete for Main ESM \n");
        }
    }
    return retValue;
}


/*********************************************************************
 * @fn    runECC2BitMSMCInjectTest
 *
 * @brief   Execute ECC Doule bit error example test on MSMC ECC aggregator
 *
 * @param   None
 *
 * @return  0 : Success; < 0 for failures
 */
int32_t runECC2BitMMSC_RAMInjectTest(void)
{
    SDL_ErrType_t result;
    int32_t retVal=0;
    uint32_t subType;

    SDL_ECC_InjectErrorConfig_t injectErrorConfig;

    memset(&injectErrorConfig, 0, sizeof(injectErrorConfig));

    UART_printf("\n MSMC Double bit error inject Example test UC-7: starting");

    /* Run one shot test for MSMC 2 bit error */
    /* Note the address is relative to start of ram */
    injectErrorConfig.pErrMem = (uint32_t *)(0x100u);

    injectErrorConfig.flipBitMask = 0x5;
    injectErrorConfig.chkGrp = 0x0;
#if defined (SOC_J721E)
    subType = SDL_COMPUTE_CLUSTER0_MSMC_ECC_AGGR0_MSMC_MMR_BUSECC_RAM_ID;
#elif defined (SOC_J7200)
	subType = SDL_COMPUTE_CLUSTER0_MSMC_MMR_BUSECC_RAM_ID;
#elif defined (SOC_J721S2)
	subType = SDL_COMPUTE_CLUSTER0_COMPUTE_CLUSTER_J7AE_MSMC_EN_DRU_MSMC_EN_4MB_WRAP_MSMC_4MB_MSMC_WRAP_ECC_AGGR0_MSMC_MMR_BUSECC_RAM_ID;
#elif defined (SOC_J784S4)
	subType = SDL_COMPUTE_CLUSTER0_COMPUTE_CLUSTER_J7AHP_MSMC_EN_DRU_MSMC_EN_8MB_WRAP_MSMC_8MB_MSMC_WRAP_ECC_AGGR0_MSMC_LAMBDA3_WRAP_MSMC_QOR_MSMC_CORE_MMR_EDC_CTRL_0_RAM_ID;
#endif
    result = SDL_ECC_injectError(SDL_ECC_MEMTYPE_MAIN_MSMC_AGGR0,
                              subType,
                              SDL_INJECT_ECC_ERROR_FORCING_1BIT_ONCE,
                              &injectErrorConfig);

    if (result != SDL_PASS ) {
        UART_printf("\n MSMC  Double bit error inject test: Subtype %d: test failed",
                    subType);
        retVal = -1;
    } else {
        UART_printf("\n MSMC Double bit error inject test: Subtype 0x%p test complete",
                    subType);
    }

    return retVal;
}

int32_t runECC2BitMMSC_InjectTest(void)
{
    SDL_ErrType_t result;
    int32_t retVal=0;
    uint32_t subType;

    SDL_ECC_InjectErrorConfig_t injectErrorConfig;

    memset(&injectErrorConfig, 0, sizeof(injectErrorConfig));

    UART_printf("\n MSMC Double bit error inject Example test UC-6: starting");

    /* Run one shot test for MSMC 2 bit error */
    /* Note the address is relative to start of ram */
    injectErrorConfig.pErrMem = (uint32_t *)(0u);

    injectErrorConfig.flipBitMask = 0x5;
    injectErrorConfig.chkGrp = 0x0;

#if defined (SOC_J721E)
    subType = SDL_COMPUTE_CLUSTER0_MSMC_ECC_AGGR0_CLEC_J7ES_CLEC_EDC_CTRL_BUSECC_RAM_ID;
#elif defined (SOC_J7200)
	subType = SDL_COMPUTE_CLUSTER0_POSTARB_PIPE_CFG_BUSECC_RAM_ID;
#elif defined (SOC_J721S2)
	subType = SDL_COMPUTE_CLUSTER0_COMPUTE_CLUSTER_J7AE_MSMC_EN_DRU_MSMC_EN_4MB_WRAP_MSMC_4MB_MSMC_WRAP_ECC_AGGR0_POSTARB_PIPE_CFG_BUSECC_RAM_ID;
#elif defined (SOC_J784S4)
	subType = SDL_COMPUTE_CLUSTER0_COMPUTE_CLUSTER_J7AHP_MSMC_EN_DRU_MSMC_EN_8MB_WRAP_MSMC_8MB_MSMC_WRAP_ECC_AGGR0_EN_MSMC_P0_BUSECC_DATA_RAM_ID;
#endif
    result = SDL_ECC_injectError(SDL_ECC_MEMTYPE_MAIN_MSMC_AGGR0,
                              subType,
                              SDL_INJECT_ECC_ERROR_FORCING_2BIT_ONCE,
                              &injectErrorConfig);

    if (result != SDL_PASS ) {
        UART_printf("\n MSMC  Double bit error inject test: Subtype %d: test failed",
                    subType);
        retVal = -1;
    } else {
        UART_printf("\n MSMC Double bit error inject test: Subtype 0x%p test complete",
                    subType);
    }

    return retVal;
}



static int32_t ESM_sdlFuncTest(void)
{
    int32_t result;
    int32_t retVal = 0;

    UART_printf("\n ESM Safety Example tests: starting");

	if (retVal == 0) 
	{
	    result = runECC2BitMMSC_InjectTest();
		
		while(esmError != true)
		{
			/* Wait for Interrupt 
			COMPUTE_CLUSTER0_CLEC_ESM_EVENTS_OUT_LEVEL_0 */			
		}
			 
		if(esmError == true){
			UART_printf("\n UC-6: Got Low priority ESM Interrupt \n");
			esmError = false;
		}
		
	    if (result != SDL_PASS) {
	        retVal = -1;
	        UART_printf("\n ESM_ECC_Example_run: UC-6 has failed...");
			/* UC-6 Low priority MSMC interrupt */
	    }
	}

	if (retVal == 0) 
	{
	    result = runECC2BitMMSC_RAMInjectTest();
	    while(esmError != true)
		{
			/* Wait for Interrupt
			"COMPUTE_CLUSTER0_CLEC_ESM_EVENTS_OUT_LEVEL_1" */
		}
			
		if(esmError == true){
			UART_printf("\n UC-7: Got High priority ESM Interrupt \n");
			esmError = false;
		}
			
	    if (result != SDL_PASS) {
	        retVal = -1;
	        UART_printf("\n ESM_ECC_Example_run: UC-7 has failed...");
			/* UC-7 High priority MSMC interrupt */
	    }
	}
	

    if ( retVal == 0) {
        UART_printf("\n ESM Safety Example tests: success");
    } else {
        UART_printf("\n ESM Safety Example tests: failed");
    }

    return retVal;
}

/* ECC Function module test */
int32_t ESM_funcTest(void)
{
    int32_t testResult;

    testResult = nESM_ECC_Example_init();

    if (testResult != 0)
    {
        UART_printf("\n ESM Safety Example tests: unsuccessful");
        return SDL_EFAIL;
    }

    testResult = ESM_sdlFuncTest();


    return (testResult);
}

/* Nothing past this point */
