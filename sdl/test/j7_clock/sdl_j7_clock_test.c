/*
 *  Copyright (c) 2021 Texas Instruments Incorporated
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
 *  \file     sdl_j7_clock_test.c
 *
 *  \brief    This file contains ESM test application code for detecting error
 *            signals/interrupts such as clock loss
 **/

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "main.h"

/* ========================================================================== */
/*                                Macros                                      */
/* ========================================================================== */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

volatile bool ESM_Error = false;

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */

/**
 * \brief   This API will print on UART/CCS Console.
 *
 * \param   pcString        string to be printed.
 *
 * \return  None.
 */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */
int32_t SDL_j7_clock_test(void)
{
	int32_t retVal = SDL_PASS;
	
    /*Initialize UART*/
    Board_initCfg boardCfg;
    Board_STATUS  boardStatus;
    boardCfg = BOARD_INIT_PINMUX_CONFIG |
               BOARD_INIT_UART_STDIO;
    boardStatus = Board_init(boardCfg);
#if defined (UNITY_INCLUDE_CONFIG_H)
    TEST_ASSERT_EQUAL_INT32(BOARD_SOK, boardStatus);
#else
    if (boardStatus != BOARD_SOK)
    {
        UART_printf("[Error] Board init failed!!\n");
		retVal = SDL_EFAIL;
    }
#endif

	if(retVal == SDL_PASS)
	{
		UART_printf("\n\n Starting Application...\n");
		
		
		
		for(int useCase=0; useCase < 2; useCase++)
		{ 
			switch (useCase)
			{
			case 0:
				UART_printf("\n***** WKUP R5F Interrupt + J7 Clock loss Testing *****\n");
				/*intentionally trip the ESM*/
				UART_printf("    Intentionally setting the ESM error for Clock loss interrupt.\n");
				SDL_ESM_setIntrStatusRAW(WKUP_ESM_INSTANCE, ESM_ERR_SIG_WKUP);
			break;
			
			case 1:
				UART_printf("\n\n***** MCU R5F Interrupt + PLL event Testing *****\n");
				/*intentionally trip the ESM*/
				UART_printf("    Intentionally setting the ESM error for PLL event interrupt.\n");
				SDL_ESM_setIntrStatusRAW(MCU_ESM_INSTANCE, ESM_ERR_SIG_MCU);
			break;
			}
			
			/*See what happens. The ISR should fire off*/
			while(!ESM_Error)
			{
				UART_printf("Waiting for ESM to receive error\n");
			}
			/*Once Error is recieved by ESM*/
			UART_printf("ESM has received the error.\n");
		}
			
		UART_printf("\nAll tests have passed.\n");
		#if defined (UNITY_INCLUDE_CONFIG_H)
			TEST_ASSERT_TRUE(ESM_Error);
		#endif
		
	}
	else
	{
		retVal = SDL_EFAIL;
	}

    return retVal;
}

/* ========================================================================== */
/*                 Internal Function Definitions                              */
/* ========================================================================== */


int32_t SDL_ESM_applicationCallbackFunction(SDL_ESM_Inst esmInst, SDL_ESM_IntType esmIntrType,
                                            uint32_t grpChannel,  uint32_t index, uint32_t intSrc, void *arg)
{
    int32_t retVal = SDL_PASS;

    UART_printf("\nInterrupt is generated to ESM\n");
    UART_printf("    ESM Call back function called : instType 0x%x, intType 0x%x, " \
                "grpChannel 0x%x, index 0x%x, intSrc 0x%x \n",
                esmInst, esmIntrType, grpChannel, index, intSrc);
    UART_printf("    Take action \n\n");
	
	if(intSrc == 0x18U)
	{
		SDL_ESM_clearIntrStatus(WKUP_ESM_INSTANCE, ESM_ERR_SIG_WKUP);
		SDL_ESM_resetErrPin(WKUP_ESM_INSTANCE);
	}
	else
	{
		SDL_ESM_clearIntrStatus(MCU_ESM_INSTANCE, ESM_ERR_SIG_MCU);
		SDL_ESM_resetErrPin(MCU_ESM_INSTANCE);
	}
	
	ESM_Error = true;
	
    return retVal;
}

/********************************* End of file ******************************/
