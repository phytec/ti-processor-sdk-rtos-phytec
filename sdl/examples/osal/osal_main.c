/**
 * @file  osal_main.c
 *
 * @brief
 *  Example usage of SDL OSAL API.
 *
 *  \par
 *  ============================================================================
 *  @n   (C) Copyright 2021, Texas Instruments, Inc.
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

#include <stdio.h>
#include <ti/osal/osal.h>
#include <ti/osal/HwiP.h>
#include "sdl_osal.h"
#include <ti/board/board.h>

#if defined (FREERTOS) || defined (SAFERTOS)
#define APP_TSK_STACK_MAIN              (32U * 1024U)
/**< Test application stack size */

/* Test application stack */
#if defined (FREERTOS)
static uint8_t  gAppTskStackMain[APP_TSK_STACK_MAIN]
__attribute__ ((aligned(8192)));
#endif
#if defined (SAFERTOS)
static uint8_t  gAppTskStackMain[APP_TSK_STACK_MAIN]
__attribute__((aligned(32*1024))) = { 0 };
#endif
#endif

extern void UART_printf(const char *pcString, ...);

void sdlApp_print(const char * str)
{
    UART_printf(str);
}

int32_t  sdlApp_initBoard(void);

/* initialize the board for the application */
int32_t  sdlApp_initBoard(void)
{
    Board_initCfg boardCfg;
    Board_STATUS  boardStatus;

    boardCfg = BOARD_INIT_PINMUX_CONFIG |
               BOARD_INIT_UART_STDIO;
    boardStatus = Board_init(boardCfg);
    if (boardStatus != BOARD_SOK)
    {
        sdlApp_print("[Error] Board init failed!!\n");
    }
    return (boardStatus);
}

pSDL_OSAL_hwipHandle SDL_TEST_registerInterrupt(SDL_OSAL_hwipParams *pParams)
{
    HwiP_Params hwipParams;
    HwiP_Params_init(&hwipParams);

    hwipParams.arg = pParams->callbackArg;

    return HwiP_create(pParams->intNum, pParams->callback, &hwipParams);
}

int32_t SDL_TEST_enableInterrupt(uint32_t intNum)
{
    HwiP_enableInterrupt(intNum);
    return SDL_PASS;
}

int32_t SDL_TEST_disableInterrupt(uint32_t intNum)
{
    HwiP_disableInterrupt(intNum);
    return SDL_PASS;
}

int32_t SDL_TEST_globalDisableInterrupts(uintptr_t *key)
{
    *key = HwiP_disable();
    return SDL_PASS;
}

int32_t SDL_TEST_globalRestoreInterrupts(uintptr_t key)
{
    HwiP_restore(key);
    return SDL_PASS;
}

int32_t test_osal_delay(uint32_t delay)
{
    return SDL_PASS;
}

int32_t test_address_translate(uint64_t addr, uint32_t size)
{
    return SDL_PASS;
}

SDL_OSAL_Interface osal_interface =
{
    .enableInterrupt = (pSDL_OSAL_interruptFunction) SDL_TEST_enableInterrupt,
    .disableInterrupt = (pSDL_OSAL_interruptFunction) SDL_TEST_disableInterrupt,
    .registerInterrupt = (pSDL_OSAL_registerFunction) SDL_TEST_registerInterrupt,
    .deregisterInterrupt = (pSDL_OSAL_deregisterFunction) HwiP_delete,
    .globalDisableInterrupts = (pSDL_OSAL_globalDisableInterruptsFunction) SDL_TEST_globalDisableInterrupts,
    .globalRestoreInterrupts = (pSDL_OSAL_globalRestoreInterruptsFunction) SDL_TEST_globalRestoreInterrupts,
    .printFxn = (pSDL_OSAL_printFunction) printf,
	.delay = (pSDL_OSAL_delayFunction) test_osal_delay,
	.addrTranslate = (pSDL_OSAL_addrTranslateFunction) test_address_translate,
};

SDL_OSAL_Interface bad_osal_interface =
{
    .enableInterrupt = (pSDL_OSAL_interruptFunction) NULL,
    .disableInterrupt = (pSDL_OSAL_interruptFunction) NULL,
    .registerInterrupt = (pSDL_OSAL_registerFunction) NULL,
    .deregisterInterrupt = (pSDL_OSAL_deregisterFunction) NULL,
    .globalDisableInterrupts = (pSDL_OSAL_globalDisableInterruptsFunction) NULL,
    .globalRestoreInterrupts = (pSDL_OSAL_globalRestoreInterruptsFunction) NULL,
    .printFxn = (pSDL_OSAL_printFunction) NULL,
};

void interruptCallback(uintptr_t args)
{
    SDL_OSAL_printf("Callback Function called...\n");
}

int32_t osalAppAPITest(void)
{
    SDL_ErrType_t ret = SDL_PASS;
    pSDL_OSAL_hwipHandle handle;
    uint32_t intNum = 0;
    SDL_OSAL_hwipParams intrParams;
    uintptr_t key;

    intrParams.intNum = intNum;
    intrParams.callback = interruptCallback;
    intrParams.callbackArg = (uintptr_t)NULL;

    // Test functions without having called SDL_OSAL_init
	ret = SDL_OSAL_delay(0u);
	if (ret == SDL_PASS)
	{
		UART_printf("SDL_OSAL_delay test FAILED\n");
	}
	else
	{
		UART_printf("SDL_OSAL_delay test PASSED\n");
	}
	
	void*  translateAddress;
	
	translateAddress = SDL_OSAL_addrTranslate(0u, 4);
	if (translateAddress == NULL)
	{
		UART_printf("SDL_OSAL_addrTranslate test FAILED\n");
	}
	else
	{
		UART_printf("SDL_OSAL_addrTranslate test PASSED\n");
	}
	
    ret = SDL_OSAL_disableInterrupt(0);
    if (ret == SDL_PASS)
    {
        UART_printf("SDL_OSAL_disableInterrupt with NULL function pointer test FAILED\n");
    }
    else
    {
        UART_printf("SDL_OSAL_disableInterrupt with NULL function pointer test PASSED\n");
    }
    ret = SDL_OSAL_enableInterrupt(intNum);
    if (ret == SDL_PASS)
    {
        UART_printf("SDL_OSAL_enableInterrupt with NULL function pointer test FAILED\n");
    }
    else
    {
        UART_printf("SDL_OSAL_enableInterrupt with NULL function pointer test PASSED\n");
    }
    ret = SDL_OSAL_registerInterrupt(&intrParams, &handle);
    if (ret == SDL_PASS)
    {
        UART_printf("SDL_OSAL_registerInterrupt with NULL function pointer test FAILED\n");
    }
    else
    {
        UART_printf("SDL_OSAL_registerInterrupt with NULL function pointer test PASSED\n");
    }

    ret = SDL_OSAL_deregisterInterrupt(handle);
    if (ret == SDL_PASS)
    {
        UART_printf("SDL_OSAL_deregisterInterrupt with NULL function pointer test FAILED\n");
    }
    else
    {
        UART_printf("SDL_OSAL_deregisterInterrupt with NULL function pointer test PASSED\n");
    }

    ret = SDL_OSAL_globalDisableInterrupts(&key);
    if (ret == SDL_PASS)
    {
        UART_printf("SDL_OSAL_globalDisableInterrupt with NULL function pointer test FAILED\n");
    }
    else
    {
        UART_printf("SDL_OSAL_globalDisableInterrupt with NULL function pointer test PASSED\n");
    }

    ret = SDL_OSAL_globalRestoreInterrupts(key);
    if (ret == SDL_PASS)
    {
        UART_printf("SDL_OSAL_globalRestoreInterrupt with NULL function pointer test FAILED\n");
    }
    else
    {
        UART_printf("SDL_OSAL_globalRestoreInterrupt with NULL function pointer test PASSED\n");
    }

    // try printf. It has no return value but should fail to print
    SDL_OSAL_printf("TEST\n");

    // Test SDL_OSAL_init
    ret = SDL_OSAL_init(NULL);
    if (ret == SDL_PASS)
    {
        UART_printf("SDL_OSAL_init with NULL params test FAILED\n");
    }
    else
    {
        UART_printf("SDL_OSAL_init with NULL params test PASSED\n");
    }

    ret = SDL_OSAL_init(&bad_osal_interface);
    if (ret == SDL_PASS)
    {
        ret = SDL_OSAL_disableInterrupt(intNum);
        if (ret == SDL_PASS)
        {
            UART_printf("SDL_OSAL_disableInterrupt with NULL function pointer test FAILED\n");
        }
        else
        {
            UART_printf("SDL_OSAL_disableInterrupt with NULL function pointer test PASSED\n");
        }
        ret = SDL_OSAL_enableInterrupt(intNum);
        if (ret == SDL_PASS)
        {
            UART_printf("SDL_OSAL_enableInterrupt with NULL function pointer test FAILED\n");
        }
        else
        {
            UART_printf("SDL_OSAL_enableInterrupt with NULL function pointer test PASSED\n");
        }

        ret = SDL_OSAL_registerInterrupt(&intrParams, &handle);
        if (ret == SDL_PASS)
        {
            UART_printf("SDL_OSAL_registerInterrupt with NULL function pointer test FAILED\n");
        }
        else
        {
            UART_printf("SDL_OSAL_registerInterrupt with NULL function pointer test PASSED\n");
        }

        ret = SDL_OSAL_deregisterInterrupt(handle);
        if (ret == SDL_PASS)
        {
            UART_printf("SDL_OSAL_deregisterInterrupt with NULL function pointer test FAILED\n");
        }
        else
        {
            UART_printf("SDL_OSAL_deregisterInterrupt with NULL function pointer test PASSED\n");
        }

        ret = SDL_OSAL_globalDisableInterrupts(&key);
        if (ret == SDL_PASS)
        {
            UART_printf("SDL_OSAL_globalDisableInterrupt with NULL function pointer test FAILED\n");
        }
        else
        {
            UART_printf("SDL_OSAL_globalDisableInterrupt with NULL function pointer test PASSED\n");
        }

        ret = SDL_OSAL_globalRestoreInterrupts(key);
        if (ret == SDL_PASS)
        {
            UART_printf("SDL_OSAL_globalRestoreInterrupt with NULL function pointer test FAILED\n");
        }
        else
        {
            UART_printf("SDL_OSAL_globalRestoreInterrupt with NULL function pointer test PASSED\n");
        }
		
		ret = SDL_OSAL_delay(0u);
		if (ret == SDL_PASS)
		{
			UART_printf("SDL_OSAL_delay test FAILED\n");
		}
		else
		{
			UART_printf("SDL_OSAL_delay test PASSED\n");
		}
		
		void*  translateAddress;
		
		translateAddress = SDL_OSAL_addrTranslate(0u, 4);
		if (translateAddress == NULL)
		{
			UART_printf("SDL_OSAL_addrTranslate test FAILED\n");
		}
		else
		{
			UART_printf("SDL_OSAL_addrTranslate test PASSED\n");
		}

        // try printf. It has no return value but should fail to print
        SDL_OSAL_printf("SDL_OSAL_printf with NULL function pointer test FAILED\n");
    }
    else
    {
        UART_printf("SDL_OSAL_init with bad params test FAILED\n");
    }

    ret = SDL_OSAL_init(&osal_interface);
    if (ret != SDL_PASS)
    {
        UART_printf("SDL_OSAL_init failed to register interface\n");
    }
    else
    {
        ret = SDL_OSAL_registerInterrupt(NULL, &handle);
        if (ret == SDL_PASS)
        {
            UART_printf("SDL_OSAL_registerInterrupt with NULL params test FAILED\n");
        }
        else
        {
            UART_printf("SDL_OSAL_registerInterrupt with NULL params test PASSED\n");
        }

        ret = SDL_OSAL_registerInterrupt(&intrParams, NULL);
        if (ret == SDL_PASS)
        {
            UART_printf("SDL_OSAL_registerInterrupt with NULL handle test FAILED\n");
        }
        else
        {
            UART_printf("SDL_OSAL_registerInterrupt with NULL handle test PASSED\n");
        }

        ret = SDL_OSAL_deregisterInterrupt(NULL);
        if (ret == SDL_PASS)
        {
            UART_printf("SDL_OSAL_deregisterInterrupt with NULL handle test FAILED\n");
        }
        else
        {
            UART_printf("SDL_OSAL_deregisterInterrupt with NULL handle test PASSED\n");
        }

        ret = SDL_OSAL_globalDisableInterrupts(NULL);
        if (ret == SDL_PASS)
        {
            UART_printf("SDL_OSAL_globalDisableInterrupts with NULL key pointer test FAILED\n");
        }
        else
        {
            UART_printf("SDL_OSAL_globalDisableInterrupts with NULL handle test PASSED\n");
        }		
    }

    return ret;
}

static void testFxn(void *a0, void* a1)
{
    SDL_ErrType_t ret = SDL_PASS;
    pSDL_OSAL_hwipHandle handle;
    uint32_t intNum = 0;
    SDL_OSAL_hwipParams intrParams;
    uintptr_t key;

    ret = osalAppAPITest();
	
	ret = SDL_OSAL_delay(246);
	if (ret != SDL_PASS)
	{
		UART_printf("SDL_OSAL_delay test FAILED\n");
	}
	else
	{
		UART_printf("SDL_OSAL_delay test PASSED\n");
	}
	
	uint64_t testVar = 0x1000;
	void*  translateAddress;
	
	translateAddress = SDL_OSAL_addrTranslate(testVar, 4);
	if (translateAddress != SDL_PASS)
	{
		UART_printf("SDL_OSAL_addrTranslate test FAILED\n");
	}
	else
	{
		UART_printf("SDL_OSAL_addrTranslate test PASSED\n");
	}

    ret = SDL_OSAL_init(&osal_interface);
    if (ret != SDL_PASS)
    {
        UART_printf("Error: Init Failed\n");
        return;
    }

    SDL_OSAL_printf("SDL_OSAL test start\n");

    ret = SDL_OSAL_disableInterrupt(intNum);
    if (ret != SDL_PASS)
    {
        UART_printf("SDL_OSAL_disableInterrupt test FAILED\n");
    }
    else
    {
        UART_printf("SDL_OSAL_disableInterrupt test PASSED\n");
    }
	
    ret = SDL_OSAL_enableInterrupt(intNum);
    if (ret != SDL_PASS)
    {
        UART_printf("SDL_OSAL_enableInterrupt test FAILED\n");
    }
    else
    {
        UART_printf("SDL_OSAL_enableInterrupt test PASSED\n");
    }

    ret = SDL_OSAL_globalDisableInterrupts(&key);
    if (ret != SDL_PASS)
    {
        UART_printf("SDL_OSAL_globalDisableInterrupts test FAILED\n");
    }
    else
    {
        UART_printf("SDL_OSAL_globalDisableInterrupts test PASSED\n");
    }

    ret = SDL_OSAL_globalRestoreInterrupts(key);
    if (ret != SDL_PASS)
    {
        UART_printf("SDL_OSAL_globalRestoreInterrupts test FAILED\n");
    }
    else
    {
        UART_printf("SDL_OSAL_globalRestoreInterrupts test PASSED\n");
    }

    intrParams.intNum = intNum;
    intrParams.callback = interruptCallback;
    intrParams.callbackArg = (uintptr_t)NULL;
    ret = SDL_OSAL_registerInterrupt(&intrParams, &handle);

    if (ret != SDL_PASS)
    {
        UART_printf("SDL_OSAL_registerInterrupt test FAILED\n");
    }
    else
    {
        UART_printf("SDL_OSAL_registerInterrupt test PASSED\n");
    }
    ret = SDL_OSAL_deregisterInterrupt(handle);
    if (ret != SDL_PASS)
    {
        UART_printf("SDL_OSAL_deregisterInterrupt test FAILED\n");
    }
    else
    {
        UART_printf("SDL_OSAL_deregisterInterrupt test PASSED\n");
    }
	
	
	
	
    SDL_OSAL_printf("SDL_OSAL test end\n");
}

int32_t main(void)
{
#if defined (FREERTOS) || defined (SAFERTOS)
    TaskP_Handle task;
    TaskP_Params taskParams;
	
	/* Init Board */
    sdlApp_initBoard();

    OS_init();

    /* Initialize the task params */
    TaskP_Params_init(&taskParams);
    /* Set the task priority higher than the default priority (1) */
    taskParams.priority = 2;
    taskParams.stack        = gAppTskStackMain;
    taskParams.stacksize    = sizeof (gAppTskStackMain);

    task = TaskP_create(testFxn, &taskParams);
    if(NULL == task)
    {
        OS_stop();
    }
    OS_start();    /* does not return */

    return(0);
#else
		
	/* Init Board */
    sdlApp_initBoard();

    testFxn(NULL, NULL);

    return (0);
#endif
}
