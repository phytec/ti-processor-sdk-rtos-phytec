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

/* This file contains VIM API negative test code */

#include "vim_test_main.h"
#include <test/r5f_startup/interrupt.h>
#include <test/r5f_startup/interrupt_priv.h>

#if defined (SOC_J721E)
#include "src/sdl/r5/j721e/sdl_soc_r5.h"
#endif 

#if defined (SOC_J7200)
#include "src/sdl/r5/j7200/sdl_soc_r5.h"
#endif 

#if defined (SOC_J721S2)
#include "src/sdl/r5/j721s2/sdl_soc_r5.h"
#endif 

#if defined (SOC_J784S4)
#include "src/sdl/r5/j784s4/sdl_soc_r5.h"
#endif

int32_t VIM_sdlErrTest()
{
    int32_t     testStatus = SDL_APP_TEST_PASS, sdlRet;
    SDL_vimRegs *pRegs;
    /* initialize the address */
    pRegs        = (SDL_vimRegs *)(SDL_MCU_VIC_CFG_BASE);
    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_getRevision(NULL);
		/* If returns 0U, its invalid pointer */
        if (sdlRet != 0U)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }
    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_getNumIntrs(NULL);
		/* If returns 0U, its invalid pointer */
        if (sdlRet != 0U)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }
    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_cfgIntr(NULL, 0xFFU, 0xFU, 0x1U, 0x1U, 0xF0U);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }
    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_cfgIntr(pRegs, 0xFFFFU, 0xFU, 0x1U, 0x1U, 0xF0U);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }
    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_cfgIntr(pRegs, 0xFFFFU, 0xFU, 0x1U, 0x1U, 0xFFFFU);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }
    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_cfgIntr(pRegs, 0xFFFFU, 0xFU, 0x1U, 0x1U, 0xFFFEU);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_cfgIntr(pRegs, 0xFFFFU, 0xFU, 0x1U, 0x1U, 0xFFFCU);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }
    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_cfgIntr(NULL, 0xFFFFU, 0xFU, 0x1U, 0x1U, 0xF0U);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }	
    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_cfgIntr(pRegs, 0xFFU, 0xFFU, 0x1U, 0x1U, 0xF0U);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }
    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_cfgIntr(pRegs, 0xFFU, 0xFU, 0x2U, 0x1U, 0xF0U);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }
    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_cfgIntr(pRegs, 0xFFU, 0xFU, 0x1U, 0x2U, 0xF0U);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }
    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_cfgIntr(pRegs, 0xFFU, 0xFU, 0x1U, 0x1U, 0xFFU);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }
    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_cfgIntr(pRegs, 0xFFFFU, 0xFU, 0x1U, 0x1U, 0xFFU);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }
    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_cfgIntr(pRegs, 0xFFFFU, 0xFU, 0x1U, 0x1U, 0xFFFFFFFFU);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }
    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_cfgIntr(pRegs, 0xFFFFU, 0xFU, 0x1U, 0x1U, 0xFFFFFFFEU);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }
    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_cfgIntr(pRegs, 0xFFU, 0xFU, 0x1U, 0x1U, 0x00FFFFFFU);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }
    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_cfgIntr(pRegs, 0x200U, 0xFU, 0x1U, 0x1U, 0x00FFFFF0U);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }
    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_cfgIntr(NULL, 0xFFU, 0xFU, 0x1U, 0x1U, 0x00FFFFF0U);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }
    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_verifyCfgIntr(NULL, 0xFFU, 0xFU, 0x1U, 0x1U, 0xF0U);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }
    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_verifyCfgIntr(NULL, 0xFFFFU, 0xFU, 0x1U, 0x1U, 0xF0U);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }
    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_verifyCfgIntr(pRegs, 0xFFFFU, 0xFU, 0x1U, 0x1U, 0xFFFEU);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }
    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_verifyCfgIntr(pRegs, 0xFFFFU, 0xFU, 0x1U, 0x1U, 0xFFFCU);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }
    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_verifyCfgIntr(pRegs, 0xFFFFU, 0xFU, 0x1U, 0x1U, 0xF0U);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }
    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_verifyCfgIntr(pRegs, 0xFFU, 0xFFU, 0x1U, 0x1U, 0xF0U);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }
    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_verifyCfgIntr(pRegs, 0xFFU, 0xFU, 0x2U, 0x1U, 0xF0U);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }
    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_verifyCfgIntr(pRegs, 0xFFU, 0xFU, 0x1U, 0x2U, 0xF0U);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }
    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_verifyCfgIntr(pRegs, 0xFFU, 0xFU, 0x1U, 0x1U, 0xFFU);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }
		
	SDL_VIM_cfgIntr(pRegs, 11U, 0xFU, 0x0U, 0x1U, (uint32_t)&SDL_masterIsr);
	
    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_verifyCfgIntr(pRegs, 11U, 0xFU, 0x1U, 0x1U, (uint32_t)&SDL_masterIsr);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }
    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_verifyCfgIntr(pRegs, 11U, 0xFU, 0x0U, 0x0U, (uint32_t)&SDL_masterIsr);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }
    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_verifyCfgIntr(pRegs, 11U, 0x0U, 0x0U, 0x1U, (uint32_t)&SDL_masterIsr);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }
    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_verifyCfgIntr(pRegs, 11U, 0xFU, 0x0U, 0x1U, 0x00FFFFF0U);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }
    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_verifyCfgIntr(pRegs, 0xFFU, 0xFU, 0x1U, 0x1U, 0xFFFFFFFDU);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }
    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_verifyCfgIntr(pRegs, 0x200U, 0xFU, 0x1U, 0x1U, 0x00FFFFF0U);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }
    if (testStatus == SDL_APP_TEST_PASS)
    {
       (void)SDL_VIM_setDedVectorAddr(pRegs, 0xFFU);
       (void)SDL_VIM_setDedVectorAddr(NULL, 0xFFU);
        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }	
    if (testStatus == SDL_APP_TEST_PASS)
    {
		uint32_t vectorAddr;
        sdlRet = SDL_VIM_getDedVectorAddr(NULL, &vectorAddr);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }	
    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_getDedVectorAddr(pRegs, NULL);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }	
    if (testStatus == SDL_APP_TEST_PASS)
    {
		uint32_t intrNum; 
		uint32_t priority;
        sdlRet = SDL_VIM_getActivePendingIntr(NULL, 2U, &intrNum, &priority);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }    if (testStatus == SDL_APP_TEST_PASS)
    {
		uint32_t intrNum; 
		uint32_t priority;
        sdlRet = SDL_VIM_getActivePendingIntr(pRegs, 2U, &intrNum, &priority);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }
    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_setIntrEnable(NULL, 0xFF, (bool) true);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }	
    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_setIntrEnable(pRegs, 0xFFFF, (bool) true);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }	    if (testStatus == SDL_APP_TEST_PASS)
    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_getIntrType(NULL, 0xFF);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }	
    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_getIntrType(pRegs, 0xFFFF);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }
    
    if (testStatus == SDL_APP_TEST_PASS)
    {
		/* If returns 0U, its invalid pointer */
        sdlRet = SDL_VIM_getIrqVectorAddress(NULL);

        if (sdlRet != 0U)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }	
    if (testStatus == SDL_APP_TEST_PASS)
    {
		/* If returns 0U, its invalid pointer */
        sdlRet = SDL_VIM_getFiqVectorAddress(NULL);

        if (sdlRet != 0U)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }	
    if (testStatus == SDL_APP_TEST_PASS)
    {
		/* If returns 0U, its invalid pointer */
        sdlRet = SDL_VIM_getGroupsIntrPending(NULL, 0U);

        if (sdlRet != 0U)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }	
    if (testStatus == SDL_APP_TEST_PASS)
    {
		/* If returns 0U, its invalid pointer */
        sdlRet = SDL_VIM_getGroupIntrPending(NULL, 2U, 1U, (bool) true);

        if (sdlRet != 0U)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }	
    if (testStatus == SDL_APP_TEST_PASS)
    {
		/* If returns 0U, its invalid group number */
        sdlRet = SDL_VIM_getGroupIntrPending(pRegs, 2U, 0xFFU, (bool) true);

        if (sdlRet != 0U)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }
    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_setIntrPending(NULL, 0xFFU);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }	
    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_setIntrPending(pRegs, 0xFFFFU);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }
    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_clrIntrPending(NULL, 0xFFU);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }
    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_clrIntrPending(pRegs, 0xFFFFU);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }	
    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_ackIntr(NULL, 2U);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }
    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_ackIntr(pRegs, 2U);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }	
    if (testStatus == SDL_APP_TEST_PASS)
    {
		SDL_vimStaticRegs staticRegs;
        sdlRet = SDL_VIM_getStaticRegs(NULL, &staticRegs);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }
    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_VIM_getStaticRegs(pRegs, NULL);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  VIM Error test failed on line no: %d \n", __LINE__);
        }
    }
    /* Returning the test status  */
    return (testStatus);

}
