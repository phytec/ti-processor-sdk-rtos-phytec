/* Copyright (c) 2021 Texas Instruments Incorporated
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
 *  \file     test_api.c
 *
 *  \brief    This file contains sdl_r5_utils.asm test code.
 *
 *  \details   API tests
 **/

#include <main.h>
#include "unity.h"
#include "unity_config.h"



extern int32_t gNumTempSensors;
extern int32_t gNumCoreVoltageDomains;

int32_t sdl_apiTest(void)
{	
  uint32_t testResult=0;
	if(testResult == 0)
  	{
		testResult=SDL_UTILS_getCFLR();
		
		if (testResult != SDL_PASS)
		{
			UART_printf("\n  SDL_UTILS_getCFLR API test failed on line no: %d \n", __LINE__);
			testResult = -1;
		}
	}
	if(testResult == 0)
  	{
		testResult=SDL_UTILS_getPMOVSR();
		
		if (testResult != SDL_PASS)
		{
			UART_printf("\n  SDL_UTILS_getPMOVSR API test failed on line no: %d \n", __LINE__);
			testResult = -1;
		}
	}
	if(testResult == 0)
  	{
		testResult=SDL_UTILS_getDFSR();
		
		if (testResult != SDL_PASS)
		{
			UART_printf("\n SDL_UTILS_getDFSR API test failed on line no: %d \n", __LINE__);
			testResult = -1;
		}
	}		
	if(testResult == 0)
  	{
		testResult=SDL_UTILS_getDFAR();
		
		if (testResult != SDL_PASS)
		{
			UART_printf("\n SDL_UTILS_getDFAR API test failed on line no: %d \n", __LINE__);
			testResult = -1;
		}
	}	
	if(testResult == 0)
  	{
		testResult=SDL_UTILS_getIFSR();
		
		if (testResult != SDL_PASS)
		{
			UART_printf("\n SDL_UTILS_getIFSR API test failed on line no: %d \n", __LINE__);
			testResult = -1;
		}
	}	
	if(testResult == 0)
  	{
		testResult=SDL_UTILS_getIFAR();
		
		if (testResult != SDL_PASS)
		{
			UART_printf("\n SDL_UTILS_getIFAR API test failed on line no: %d \n", __LINE__);
			testResult = -1;
		}
	}	


   
	return (testResult);
	
}
/* Nothing past this point */
