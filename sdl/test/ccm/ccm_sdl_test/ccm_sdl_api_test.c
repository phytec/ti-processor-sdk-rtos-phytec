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
 *  \file     ccm_sdl_api_test.c
 *
 *  \brief    This file contains CCM SDL API test code.
 *
 *  \details  CCM API tests
 **/

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <stdint.h>
#include <string.h>
#include <sdl_types.h>
#include "ccm_test_main.h"
#include "src/ip/r5/sdl_mcu_armss_ccmr5.h"
#include <sdl_esm.h>

/* ========================================================================== */
/*                                Macros                                      */
/* ========================================================================== */

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
								   												   
/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

static int32_t CCM_API_test(uint32_t instanceId)
{
    int32_t       testResult = 0;
    SDL_ErrType_t sdlResult;
    int i;

	sdlResult = SDL_CCM_init(instanceId);
	if (sdlResult != SDL_PASS)
	{
		UART_printf("sdlCcm_apiTest: failure on line no. %d \n", __LINE__);
		testResult = -1;
	}
	if(testResult == 0)
	{
    	sdlResult = SDL_CCM_verifyConfig(instanceId);
    	if (sdlResult != SDL_PASS)
    	{
    		UART_printf("sdlCcm_apiTest: failure on line no. %d \n", __LINE__);
    		testResult = -1;
    	}
	}
	if(testResult == 0)
	{
        for(i = SDL_CCM_MONITOR_TYPE_OUTPUT_COMPARE_BLOCK; i <= 	SDL_CCM_MONITOR_TYPE_INACTIVITY_MONITOR; i++)
    	{
			sdlResult = SDL_CCM_clearError(instanceId, (SDL_CCM_MonitorType)i);
			if (sdlResult != SDL_PASS)
			{
				UART_printf("sdlCcm_apiTest: failure on line no. %d \n", __LINE__);
				testResult = -1;
				break;
			}
    	}
    }

    if (testResult == 0)
    {
        sdlResult = SDL_CCM_selfTest(SDL_CCM_MCU_R5F0, SDL_CCM_MONITOR_TYPE_OUTPUT_COMPARE_BLOCK, \
		                             SDL_CCM_SELFTEST_TYPE_NORMAL, 0x0, 0U);
        if (sdlResult != SDL_PASS)
        {
            UART_printf("sdlCcm_apiTest: failure on line no. %d \n", __LINE__);
            testResult = -1;
        }
    }
	
    return (testResult);
}

/* CCM Functional test */
int32_t CCM_sdlApiTest(void)
{
    int32_t    testResult = 0;

	/* Run the test for diagnostics first */
	testResult = CCM_API_test(SDL_CCM_MCU_R5F0);

    return (testResult);
}
/* Nothing past this point */
