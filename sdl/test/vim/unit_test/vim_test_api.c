/*
 *    Copyright (c) 2022 Texas Instruments Incorporated
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
 *  A PAVIMCULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/* This file contains VIM API positive test code */

#include "vim_test_main.h"
#include <test/r5f_startup/interrupt.h>
#include <test/r5f_startup/interrupt_priv.h>
#include "soc.h"
#include <sdl_esm.h>
#include "src/ip/r5/sdl_mcu_armss_ccmr5.h"
#include <src/sdl/sdl_ccm.h>

#include <sdl_arm_r5_pmu.h>
#include <sdl_exception.h>
#include <test/r5f_startup/sdl_cache.h>

#if defined (SOC_J721E)
#include <src/sdl/r5/j721e/sdl_soc_ccm.h>
#endif 

#if defined (SOC_J7200)
#include <src/sdl/r5/j7200/sdl_soc_ccm.h>
#endif 

#if defined (SOC_J721S2)
#include <src/sdl/r5/j721s2/sdl_soc_ccm.h>
#endif 

/* For Timer functions */
#include <ti/osal/osal.h>
/* Osal API header files */
#include <ti/osal/HwiP.h>


#define TEST_VIM_BASE_ADDR    SDL_MAIN_DOMAIN_VIM_BASE_ADDR

#define ATCM_BASE_ADDR  (0x00000000U)
#define ATCM_SIZE       (0x00008000U)
#define TEST_AREA_SIZE  (0x00000400U)

#define ATCM_FILL_PATTERN (0xffffffffU)

#define VECTOR_SIZE 0x100

#define MCU_R5F_MAX_MEM_SECTIONS   (8u)
#define MCU_CBASS_MAX_MEM_SECTIONS (3u)
#define MAIN_MSMC_AGGR0_MAX_MEM_SECTIONS (4u)

void testVimIsr_0 (void)
{
	UART_printf("\n  VIM ISR 0");
}
void testVimIsr_1 (void)
{
	UART_printf("\n  VIM ISR 1");
}

uintptr_t vimIsrArray[2] =
{
    (uintptr_t)&testVimIsr_0,
    (uintptr_t)&testVimIsr_1,
};

int32_t VIM_sdlApiTest(void)
{
    int32_t     testStatus = SDL_APP_TEST_PASS, sdlRet;
    SDL_vimRegs *pRegs; 
	uint32_t intrNum = SDLR_R5FSS0_CORE0_INTR_ESM0_ESM_INT_HI_LVL_0;
    /* initialize the address */
	pRegs        = (SDL_vimRegs *)(uintptr_t)SDL_MCU_DOMAIN_VIM_BASE_ADDR;

    Intc_Init();	
    if (testStatus == SDL_APP_TEST_PASS)
    {
		/* Its read only register. So not validated*/
        (void)SDL_VIM_getRevision(pRegs);
    }
    if (testStatus == SDL_APP_TEST_PASS)
    {
		/* Its read only register. So not validated*/
        (void)SDL_VIM_getNumIntrs(pRegs);
    }	
	if (testStatus == SDL_APP_TEST_PASS)
	{
		sdlRet = SDL_VIM_cfgIntr(pRegs, intrNum, 0xFU, 0x1U, 0x1U, (uint32_t)&SDL_masterIsr);
		if (sdlRet != SDL_PASS)
		{
			testStatus = SDL_APP_TEST_FAILED;
			UART_printf("\n  VIM API test failed on line no: %d \n", __LINE__);
		}
	}
	if (testStatus == SDL_APP_TEST_PASS)
	{
		sdlRet = SDL_VIM_cfgIntr(pRegs, intrNum, 0xFU, 0x0U, 0x1U, (uint32_t)&SDL_masterIsr);
		if (sdlRet != SDL_PASS)
		{
			testStatus = SDL_APP_TEST_FAILED;
			UART_printf("\n  VIM API test failed on line no: %d \n", __LINE__);
		}
	}
    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_cfgIntr(pRegs, 0xFFU, 0xFU, 0x1U, 0x1U, 0xFFFFFFFDU);

        if (sdlRet != SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM API test failed on line no: %d \n", __LINE__);
        }
    }
	if (testStatus == SDL_APP_TEST_PASS)
	{
		sdlRet = SDL_VIM_verifyCfgIntr(pRegs, intrNum, 0xFU, 0x0U, 0x1U, (uint32_t)&SDL_masterIsr);
		if (sdlRet != SDL_PASS)
		{
			testStatus = SDL_APP_TEST_FAILED;
			UART_printf("\n  VIM API test failed on line no: %d \n", __LINE__);
		}
	}
    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_setIntrPending(pRegs, intrNum);

        if (sdlRet != SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM API test failed on line no: %d \n", __LINE__);
        }
    }
    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_setIntrEnable(pRegs, intrNum, (bool) true);

        if (sdlRet != SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM API test failed on line no: %d \n", __LINE__);
        }
    }
    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_getIntrType(pRegs, intrNum);

        if ((sdlRet != SDL_VIM_INTR_TYPE_LEVEL) && (sdlRet != SDL_VIM_INTR_TYPE_PULSE))
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM API test failed on line no: %d \n", __LINE__);
        }
		sdlRet = 0;
		/*If return value 0, invalid address*/
        SDL_VIM_getIrqVectorAddress(pRegs);
		/*If return value 0, invalid address*/
        SDL_VIM_getFiqVectorAddress(pRegs);
    }	

    if (testStatus == SDL_APP_TEST_PASS)
    {
        SDL_VIM_cfgIntr(pRegs, 11U, 0xFU, 0x0U, 0x0U, (uint32_t)&SDL_masterIsr);
        sdlRet = SDL_VIM_getIntrType(pRegs, intrNum);

        if (sdlRet != SDL_VIM_INTR_TYPE_LEVEL)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM API test failed on line no: %d \n", __LINE__);
        }
    }
    if (testStatus == SDL_APP_TEST_PASS)
    {
		/* Returns the groupsStatus value and not result*/
        SDL_VIM_getGroupsIntrPending(pRegs, 0U);
        SDL_VIM_getGroupsIntrPending(pRegs, 1U);
        if (sdlRet != SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM API test failed on line no: %d \n", __LINE__);
        }
    }	
    if (testStatus == SDL_APP_TEST_PASS)
    {
		/* Returns the groupsStatus value and not result*/
        SDL_VIM_getGroupIntrPending(pRegs, 0U, 0U, (bool) true);
		/* Returns the groupsStatus value and not result*/
        SDL_VIM_getGroupIntrPending(pRegs, 1U, 0U, (bool) true);
		/* Returns the groupsStatus value and not result*/
        SDL_VIM_getGroupIntrPending(pRegs, 2U, 0U, (bool) true);
        if (sdlRet != SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM API test failed on line no: %d \n", __LINE__);
        }
    }	
    if (testStatus == SDL_APP_TEST_PASS)
    {
		/* Returns the groupsStatus value and not result*/
        SDL_VIM_getGroupIntrPending(pRegs, 0U, 0U, (bool) false);

        if (sdlRet != SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM API test failed on line no: %d \n", __LINE__);
        }
    }	
    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_clrIntrPending(pRegs, intrNum);

        if (sdlRet != SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM API test failed on line no: %d \n", __LINE__);
        }
    }	
    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_ackIntr(pRegs, 1U);

        if (sdlRet != SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM API test failed on line no: %d \n", __LINE__);
        }
    }
    if (testStatus == SDL_APP_TEST_PASS)
    {
		/*No fail*/
       (void)SDL_VIM_setDedVectorAddr(pRegs, vimIsrArray[0]);

        if (sdlRet != SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM API test failed on line no: %d \n", __LINE__);
        }
    }	
    if (testStatus == SDL_APP_TEST_PASS)
    {
		uint32_t vectorAddr;
        sdlRet = SDL_VIM_getDedVectorAddr(pRegs, &vectorAddr);

        if (sdlRet != SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM API test failed on line no: %d \n", __LINE__);
        }
    }
    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_setIntrEnable(pRegs, intrNum, (bool) false);

        if (sdlRet != SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM API test failed on line no: %d \n", __LINE__);
        }
    }	
    if (testStatus == SDL_APP_TEST_PASS)
    {
		SDL_vimStaticRegs staticRegs;
        sdlRet = SDL_VIM_getStaticRegs(pRegs, &staticRegs);

        if (sdlRet != SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM API test failed on line no: %d \n", __LINE__);
        }
    }

return (testStatus);
}

/* VIM Function module API test */
int32_t VIM_apiTest(void)
{
    int32_t testResult;

    testResult = VIM_sdlApiTest();

    return (testResult);
}

/* Nothing past this point */
