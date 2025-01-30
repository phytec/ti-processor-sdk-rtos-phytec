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
 *  \file     rat_test_func.c
 *
 *  \brief    This file contains RAT SDL Function test code for R5 core.
 *
 *  \details  RAT Memory tests
 **/

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "rat_func.h"
/* Osal API header files */
#include <ti/osal/HwiP.h>
#include <src/sdl/sdl_esm.h>

/* ========================================================================== */
/*                                 Internal function declarations                                      */
/* ========================================================================== */
int32_t RAT_ECC_funcTest(void);
/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
SDL_ESM_config ECC_Test_esmInitConfig_MCU =
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

extern int32_t SDL_ESM_applicationCallbackFunction(SDL_ESM_Inst esmInstType,
                                                   SDL_ESM_IntType esmIntType,
                                                   uint32_t grpChannel,
                                                   uint32_t index,
                                                   uint32_t intSrc,
                                                   void *arg);

/* Defines */
#define MCU_R5F_MAX_MEM_SECTIONS   (8u)

#if defined (SOC_J721E)
static SDL_ECC_MemSubType ECC_Test_R5FCoresubMemTypeList[MCU_R5F_MAX_MEM_SECTIONS] =
{
	SDL_MCU_R5FSS0_CORE0_ECC_AGGR_KSBUS_VBUSM2AXI0_EDC_CTRL_RAM_ID,
	SDL_MCU_R5FSS0_CORE0_ECC_AGGR_MEM_MST0_KSBUS_AXI2VBUSM_W_EDC_CTRL_RAM_ID,
	SDL_MCU_R5FSS0_CORE0_ECC_AGGR_MEM_MST0_KSBUS_AXI2VBUSM_R_EDC_CTRL_RAM_ID,
	SDL_MCU_R5FSS0_CORE0_ECC_AGGR_PERIPH_M_MST0_KSBUS_AXI2VBUSM_W_EDC_CTRL_RAM_ID,
	SDL_MCU_R5FSS0_CORE0_ECC_AGGR_PERIPH_M_MST0_KSBUS_AXI2VBUSM_R_EDC_CTRL_RAM_ID,
	SDL_MCU_R5FSS0_CORE0_ECC_AGGR_PULSAR_AHB2VBUSP_CPU0_EDC_CTRL_RAM_ID,
	SDL_MCU_R5FSS0_CORE0_ECC_AGGR_CPU0_CFG_SCRP_INTERFACE0_GCLK_CLK_EDC_CTRL_RAM_ID,
	SDL_MCU_R5FSS0_CORE0_ECC_AGGR_CPU0_ECC_AGGR_EDC_CTRL_RAM_ID,
};
#endif

#if defined (SOC_J7200)
static SDL_ECC_MemSubType ECC_Test_R5FCoresubMemTypeList[MCU_R5F_MAX_MEM_SECTIONS] =
{
    SDL_R5FSS0_PULSAR_SL_KSBUS_VBUSM2AXI0_EDC_CTRL_0_RAM_ID,
    SDL_R5FSS0_CPU0_AXI2VBUSM_MEM_MST_RAMECC_RAM_ID,
    SDL_R5FSS0_PULSAR_SL_MEM_MST0_EDC_CTRL_0_RAM_ID,
    SDL_R5FSS0_CPU0_AXI2VBUSM_PERIPH_MST_RAMECC_RAM_ID,
    SDL_R5FSS0_PULSAR_SL_PERIPH_M_MST0_EDC_CTRL_0_RAM_ID,
    SDL_R5FSS0_CPU0_AHB2VBUSP_EDC_RAM_ID,
    SDL_R5FSS0_PULSAR_SL_CPU0_CFG_SCRP_INTERFACE0_GCLK_CLK_EDC_CTRL_RAM_ID,
    SDL_R5FSS0_PULSAR_SL_CPU0_ECC_AGGR_EDC_CTRL_RAM_ID,
};
#elif defined (SOC_J721S2)
static SDL_ECC_MemSubType ECC_Test_R5FCoresubMemTypeList[MCU_R5F_MAX_MEM_SECTIONS] =
{
    SDL_MCU_R5FSS0_PULSAR_SL_CPU0_ECC_AGGR_PULSAR_SL_KSBUS_VBUSM2AXI0_EDC_CTRL_0_RAM_ID,
    SDL_MCU_R5FSS0_PULSAR_SL_CPU0_ECC_AGGR_CPU0_AXI2VBUSM_MEM_MST_RAMECC_RAM_ID,
    SDL_MCU_R5FSS0_PULSAR_SL_CPU0_ECC_AGGR_PULSAR_SL_MEM_MST0_EDC_CTRL_0_RAM_ID,
    SDL_MCU_R5FSS0_PULSAR_SL_CPU0_ECC_AGGR_CPU0_AXI2VBUSM_PERIPH_MST_RAMECC_RAM_ID,
    SDL_MCU_R5FSS0_PULSAR_SL_CPU0_ECC_AGGR_PULSAR_SL_PERIPH_M_MST0_EDC_CTRL_0_RAM_ID,
    SDL_MCU_R5FSS0_PULSAR_SL_CPU0_ECC_AGGR_CPU0_AHB2VBUSP_EDC_RAM_ID,
    SDL_MCU_R5FSS0_PULSAR_SL_CPU0_ECC_AGGR_PULSAR_SL_CPU0_CFG_SCRP_INTERFACE0_GCLK_CLK_EDC_CTRL_RAM_ID,
    SDL_MCU_R5FSS0_PULSAR_SL_CPU0_ECC_AGGR_PULSAR_SL_CPU0_ECC_AGGR_EDC_CTRL_RAM_ID,
};
#elif defined (SOC_J784S4)
static SDL_ECC_MemSubType ECC_Test_R5FCoresubMemTypeList[MCU_R5F_MAX_MEM_SECTIONS -2] =
{
	SDL_MCU_R5FSS0_PULSAR_SL_CPU0_ECC_AGGR_PULSAR_SL_KSBUS_VBUSM2AXI0_EDC_CTRL_0_RAM_ID,
	SDL_MCU_R5FSS0_PULSAR_SL_CPU0_ECC_AGGR_CPU0_AXI2VBUSM_MEM_MST_RAMECC_RAM_ID,
	SDL_MCU_R5FSS0_PULSAR_SL_CPU0_ECC_AGGR_CPU0_AXI2VBUSM_PERIPH_MST_RAMECC_RAM_ID,
	SDL_MCU_R5FSS0_PULSAR_SL_CPU0_ECC_AGGR_CPU0_AHB2VBUSP_EDC_RAM_ID,
	SDL_MCU_R5FSS0_PULSAR_SL_CPU0_ECC_AGGR_PULSAR_SL_CPU0_CFG_SCRP_INTERFACE0_GCLK_CLK_EDC_CTRL_RAM_ID,
	SDL_MCU_R5FSS0_PULSAR_SL_CPU0_ECC_AGGR_PULSAR_SL_CPU0_ECC_AGGR_EDC_CTRL_RAM_ID,
    
};
#endif

static SDL_ECC_InitConfig_t ECC_Test_R5FCoreECCInitConfig =
{
    .numRams = MCU_R5F_MAX_MEM_SECTIONS -2,
    /**< Number of Rams ECC is enabled  */
    .pMemSubTypeList = &(ECC_Test_R5FCoresubMemTypeList[0]),
    /**< Sub type list  */
};

static uint32_t arg;
/*********************************************************************
* @fn      RAT_Test_init
*
* @brief   Initializes Software Diagostics Test Framework
*
* @param   None
*
* @return    0 : Success; < 0 for failures
*/
int32_t ECC_Test_init (void)
{
    int32_t retValue=0;
    void *ptr = (void *)&arg;
    SDL_ErrType_t result;

    if (retValue == 0) {
        /* Initialize MCU ESM module */
        result = SDL_ESM_init(SDL_ESM_INST_MCU_ESM0, &ECC_Test_esmInitConfig_MCU, SDL_ESM_applicationCallbackFunction, ptr);
        if (result != SDL_PASS) {
            /* print error and quit */
             UART_printf("ECC_Test_init: Error initializing MCU ESM: result = %d\n", result);

            retValue = -1;
        } else {
            UART_printf("\nECC_Test_init: Init MCU ESM complete \n");
        }
    }

   
    if (retValue == 0) {
        /* Initialize ECC */
        result = SDL_ECC_init(SDL_ECC_MEMTYPE_MCU_R5F0_CORE, &ECC_Test_R5FCoreECCInitConfig);
        if (result != SDL_PASS) {
            /* print error and quit */
            UART_printf("ECC_Test_init: Error initializing R5F core ECC: result = %d\n", result);

            retValue = -1;
        } else {
            UART_printf("\nECC_Test_init: R5F Core ECC Init complete \n");
        }
    }

    if (retValue == 0) {
        /* Initialize ECC callbacks within the MCU ESM */
        result = SDL_ECC_initEsm(SDL_ESM_INST_MCU_ESM0);
        if (result != SDL_PASS) {
            /* print error and quit */
             UART_printf("ECC_Test_init: Error initializing ECC callback for MCU ESM: result = %d\n", result);

            retValue = -1;
        } else {
            UART_printf("\nECC_Test_init: ECC Callback Init complete for MCU ESM \n");
        }
    }

    if (retValue == 0) {		
		uint32_t subType;
		
		#if defined (SOC_J721E)
		subType = SDL_MCU_R5FSS0_CORE0_ECC_AGGR_PULSAR_AHB2VBUSP_CPU0_EDC_CTRL_RAM_ID;
		#endif
		
		#if defined (SOC_J7200)
		subType = SDL_R5FSS0_CPU0_AHB2VBUSP_EDC_RAM_ID;
		#endif

        #if defined (SOC_J721S2)
        subType = SDL_MCU_R5FSS0_PULSAR_SL_CPU0_ECC_AGGR_CPU0_AHB2VBUSP_EDC_RAM_ID;
        #endif
	    #if defined (SOC_J784S4)
        subType = SDL_MCU_R5FSS0_PULSAR_SL_CPU0_ECC_AGGR_CPU0_AHB2VBUSP_EDC_RAM_ID;
        #endif
		
        result = SDL_ECC_initMemory(SDL_ECC_MEMTYPE_MCU_R5F0_CORE, subType);
        if (result != SDL_PASS) {
             /* print error and quit */
              UART_printf("ECC_Test_init: Error initializing ECC memory: retValue = %d\n", retValue);
             retValue = -1;
         } else {
             UART_printf("\nECC_Test_init: ECC Memory init complete \n");
         }
    }

    return retValue;
}

int32_t RAT_SEC_test(void)
{
    SDL_ErrType_t result;
    int32_t retVal=0;
	uint32_t subType;

    SDL_ECC_InjectErrorConfig_t injectErrorConfig;
    volatile uint32_t testLocationValue;

    UART_printf("\n RAT memory Single bit Repeat error inject: test starting");

    /* Note the address is relative to start of ram */
    injectErrorConfig.pErrMem = (uint32_t *)(0x00);

    /* Run one shot test for ATCM 1 bit Repeat error */
    injectErrorConfig.flipBitMask = 0x3;
	injectErrorConfig.chkGrp = 0x1;
	
	#if defined (SOC_J721E)
	subType = SDL_MCU_R5FSS0_CORE0_ECC_AGGR_PULSAR_AHB2VBUSP_CPU0_EDC_CTRL_RAM_ID;
	#endif
	
	#if defined (SOC_J7200)
	subType = SDL_R5FSS0_CPU0_AHB2VBUSP_EDC_RAM_ID;
	#endif

    #if defined (SOC_J721S2)
    subType = SDL_MCU_R5FSS0_PULSAR_SL_CPU0_ECC_AGGR_CPU0_AHB2VBUSP_EDC_RAM_ID;
    #endif	
    #if defined (SOC_J784S4)
    subType = SDL_MCU_R5FSS0_PULSAR_SL_CPU0_ECC_AGGR_CPU0_AHB2VBUSP_EDC_RAM_ID;
    #endif	
	
	result = SDL_ECC_injectError(SDL_ECC_MEMTYPE_MCU_R5F0_CORE,
								 subType,
								 SDL_INJECT_ECC_ERROR_FORCING_1BIT_ONCE,
								 &injectErrorConfig);

    if (result != SDL_PASS ) {
        UART_printf("\n RAT memory Single bit Repeat error inject at pErrMem 0x%p test failed",
                    injectErrorConfig.pErrMem);
        retVal = -1;
    } else {
        /* Access the memory where injection is expected */
        testLocationValue = injectErrorConfig.pErrMem[0];

        UART_printf("\n RAT memory  Single bit Repeat error inject at pErrMem 0x%p: test complete",
                    injectErrorConfig.pErrMem);
    }

    return retVal;
}


int32_t RAT_ECC_funcTest(void)
{
    int32_t result;
    int32_t retVal = 0;
    SDL_ECC_InjectErrorConfig_t injectErrorConfig;
    memset(&injectErrorConfig, 0, sizeof(injectErrorConfig));
    injectErrorConfig.pErrMem = (uint32_t *)(0u);
    injectErrorConfig.flipBitMask = 0x3;

    UART_printf("\n RAT Memory test: starting");

	
    if (retVal == 0) {
        result = RAT_SEC_test();
        if (result != SDL_PASS) {
            retVal = -1;
            UART_printf("\n RAT_SEC_test has failed...");
        }
    }

    return retVal;
}

/* Nothing past this point */
