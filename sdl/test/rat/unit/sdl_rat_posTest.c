/*
 *    Copyright (c) 2021 Texas Instruments Incorporated
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

/* This file contains RAT API positive test code */


#include "rat_test_main.h"


int32_t SDL_RAT_posTest(void)
{
    int32_t       testStatus = SDL_APP_TEST_PASS, sdlRet = SDL_PASS;
    uint32_t 	  destId;
	
	SDL_ratTranslationCfgInfo  pTranslationCfg;
	SDL_ratStaticRegs pStaticRegs;
	SDL_ratExceptionInfo pExceptionInfo;
	
	#if defined (SOC_J721E)
	SDL_ratRegs *pRatRegs = (SDL_ratRegs *)SDL_MCU_ARMSS_RAT_CFG_BASE;
	#endif
	
	#if defined (SOC_J7200) || defined (SOC_J721S2)  || defined (SOC_J784S4)
	SDL_ratRegs *pRatRegs = (SDL_ratRegs *)SDL_MCU_R5FSS0_RAT_CFG_BASE;
	#endif 
										   
	pTranslationCfg.translatedAddress = ((uint64_t)&SDL_RATTestArray);
	pTranslationCfg.sizeInBytes		  = sizeof(SDL_RATTestArray);
	pTranslationCfg.baseAddress 	  = (uint32_t)SDL_RAT_SELF_TEST_TRANSLATE_BASE;
	
/*******************************************************************************************
*     Call SDL API SDL_RAT_configRegionTranslation
*******************************************************************************************/

    if (testStatus == SDL_APP_TEST_PASS)
     {
        sdlRet = SDL_RAT_configRegionTranslation(pRatRegs, SDL_RAT_REGION_INDEX, &pTranslationCfg);

        if (sdlRet != SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("SDL_RAT_configRegionTranslation: failure on line no. %d \n", __LINE__);
        }
     }



/*******************************************************************************************
*     Call SDL API SDL_RAT_verifyConfigRegionTranslation
*******************************************************************************************/

    if (testStatus == SDL_APP_TEST_PASS)
     {
        sdlRet = SDL_RAT_verifyConfigRegionTranslation(pRatRegs, SDL_RAT_REGION_INDEX, &pTranslationCfg);

        if (sdlRet != SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("SDL_RAT_verifyConfigRegionTranslation: failure on line no. %d \n", __LINE__);
        }
     }




/*******************************************************************************************
*     Call SDL API SDL_RAT_getStaticRegs
*******************************************************************************************/

     if (testStatus == SDL_APP_TEST_PASS)
     {
        sdlRet = SDL_RAT_getStaticRegs(pRatRegs, SDL_RAT_REGION_INDEX, &pStaticRegs);

        if (sdlRet != SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("SDL_RAT_getStaticRegs: failure on line no. %d \n", __LINE__);
        }
     }



/*******************************************************************************************
*     Call SDL API SDL_RAT_enableRegionTranslation
*******************************************************************************************/

    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_enableRegionTranslation(pRatRegs, SDL_RAT_REGION_INDEX);

        if (sdlRet != SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_enableRegionTranslation positive test failed on line no: %d \n", __LINE__);
        }
    }



/*******************************************************************************************
*     Call SDL API SDL_RAT_disableRegionTranslation
*******************************************************************************************/

    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_disableRegionTranslation(pRatRegs, SDL_RAT_REGION_INDEX);

        if (sdlRet != SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_disableRegionTranslation positive test failed on line no: %d \n", __LINE__);
        }
    }



/*******************************************************************************************
*     Call SDL API SDL_RAT_enableIntr
*******************************************************************************************/

    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_enableIntr(pRatRegs, SDL_RAT_INTR_SRC_EXCEPTION);

        if (sdlRet != SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_enableIntr positive test failed on line no: %d \n", __LINE__);
        }
    }
	
	
	
/*******************************************************************************************
*     Call SDL API SDL_RAT_setErrMsgDestId
*******************************************************************************************/

    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_setErrMsgDestId(pRatRegs, 46);

        if (sdlRet != SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_setErrMsgDestId positive test failed on line no: %d \n", __LINE__);
        }
    }
	
	
	
/*******************************************************************************************
*     Call SDL API SDL_RAT_setErrMsgDestId
*******************************************************************************************/

    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_getErrMsgDestId(pRatRegs, &destId);

        if (sdlRet != SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_setErrMsgDestId positive test failed on line no: %d \n", __LINE__);
        }
    }



/*******************************************************************************************
*     Call SDL API SDL_RAT_enableExceptionLog
*******************************************************************************************/

    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_enableExceptionLog(pRatRegs);

        if (sdlRet != SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_enableExceptionLog positive test failed on line no: %d \n", __LINE__);
        }
    }
	
	

/*******************************************************************************************
*     Call SDL API SDL_RAT_disableExceptionLog
*******************************************************************************************/

    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_disableExceptionLog(pRatRegs);

        if (sdlRet != SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_disableExceptionLog positive test failed on line no: %d \n", __LINE__);
        }
    }



/*******************************************************************************************
*     Call SDL API SDL_RAT_setIntrPending
*******************************************************************************************/

    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_setIntrPending(pRatRegs, SDL_RAT_INTR_SRC_EXCEPTION);

        if (sdlRet != SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_setIntrPending positive test failed on line no: %d \n", __LINE__);
        }
    }




/*******************************************************************************************
*     Call SDL API SDL_RAT_isIntrPending
*******************************************************************************************/

    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_isIntrPending(pRatRegs, SDL_RAT_INTR_SRC_EXCEPTION);

        if (sdlRet != SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_isIntrPending positive test failed on line no: %d \n", __LINE__);
        }
    }
	
	

/*******************************************************************************************
*     Call SDL API SDL_RAT_clrIntrPending
*******************************************************************************************/

    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_clrIntrPending(pRatRegs, SDL_RAT_INTR_SRC_EXCEPTION);

        if (sdlRet != SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_clrIntrPending positive test failed on line no: %d \n", __LINE__);
        }
    }
	
	

/*******************************************************************************************
*     Call SDL API SDL_RAT_ackIntr
*******************************************************************************************/

    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_ackIntr(pRatRegs, SDL_RAT_INTR_SRC_EXCEPTION);

        if (sdlRet != SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_ackIntr positive test failed on line no: %d \n", __LINE__);
        }
    }
	
	

/*******************************************************************************************
*     Call SDL API SDL_RAT_disableIntr
*******************************************************************************************/

    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_disableIntr(pRatRegs, SDL_RAT_INTR_SRC_EXCEPTION);

        if (sdlRet != SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_disableIntr positive test failed on line no: %d \n", __LINE__);
        }
    }
		
	

/*******************************************************************************************
*     Call SDL API SDL_RAT_getExceptionInfo
*******************************************************************************************/

    if (testStatus == SDL_APP_TEST_PASS)
    {
		pTranslationCfg.translatedAddress = ((uint64_t)&SDL_RATTestArray);
		pTranslationCfg.sizeInBytes		  = sizeof(SDL_RATTestArray);
		pTranslationCfg.baseAddress 	  = (uint32_t)(0x9000000U);
		SDL_RAT_configRegionTranslation(pRatRegs, SDL_RAT_REGION_INDEX, &pTranslationCfg);
		
        SDL_RAT_enableExceptionLog(pRatRegs);
		
		sdlRet = SDL_RAT_getExceptionInfo(pRatRegs, &pExceptionInfo);
		SDL_RAT_disableRegionTranslation(pRatRegs, SDL_RAT_REGION_INDEX);
		
        if (sdlRet != SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_getExceptionInfo positive test failed on line no: %d \n", __LINE__);
        }
    }
	


/*  Below are the low level functions test cases  */
/*******************************************************************************************
*     Call SDL API SDL_RAT_getRegionTranslationConfig
*******************************************************************************************/

    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_getRegionTranslationConfig(pRatRegs, 4u, &pTranslationCfg);

        if (sdlRet != SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_getRegionTranslationConfig positive test failed on line no: %d \n", __LINE__);
        }
    }
	
	

/*******************************************************************************************
*     Call SDL API SDL_RAT_lmo
*******************************************************************************************/

    if (testStatus == SDL_APP_TEST_PASS)
    {
        SDL_RAT_lmo(46);

        if (sdlRet != SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_lmo positive test failed on line no: %d \n", __LINE__);
        }
    }
	
	

/*******************************************************************************************
*     Call SDL API SDL_RAT_setRegionTranslationEnable
*******************************************************************************************/

    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_setRegionTranslationEnable(pRatRegs, 4u, TRUE);

        if (sdlRet != SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_setRegionTranslationEnable positive test failed on line no: %d \n", __LINE__);
        }
    }
	
	

/*******************************************************************************************
*     Call SDL API SDL_RAT_setIntEnable
*******************************************************************************************/

    if (testStatus == SDL_APP_TEST_PASS)
    {
        SDL_RAT_setIntEnable(pRatRegs, SDL_RAT_INTR_SRC_EXCEPTION, TRUE);

        if (sdlRet != SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_setIntEnable positive test failed on line no: %d \n", __LINE__);
        }
    }
	
	

/*******************************************************************************************
*     Call SDL API SDL_RAT_setExceptionLoggingEnable
*******************************************************************************************/

    if (testStatus == SDL_APP_TEST_PASS)
    {
        SDL_RAT_setExceptionLoggingEnable(pRatRegs, TRUE);

        if (sdlRet != SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_setExceptionLoggingEnable positive test failed on line no: %d \n", __LINE__);
        }
    }
	
	

/*******************************************************************************************
*     Call SDL API SDL_RAT_setExceptionLoggingIntEnable
*******************************************************************************************/

    if (testStatus == SDL_APP_TEST_PASS)
    {
        SDL_RAT_setExceptionLoggingIntEnable(pRatRegs, TRUE);

        if (sdlRet != SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_setExceptionLoggingIntEnable positive test failed on line no: %d \n", __LINE__);
        }

    }
	
	

/*******************************************************************************************
*     Call SDL API SDL_RAT_isPowerOf2
*******************************************************************************************/

    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_isPowerOf2(16);

        if (sdlRet != SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_isPowerOf2 positive test failed on line no: %d \n", __LINE__);
        }
    }
	
	

/*******************************************************************************************
*     Call SDL API SDL_RAT_isRegionOverlap
*******************************************************************************************/

    if (testStatus == SDL_APP_TEST_PASS)
    {
		SDL_RAT_setRegionTranslationEnable(pRatRegs, 4u, TRUE);
        sdlRet = SDL_RAT_isRegionOverlap(pRatRegs, 4u, &pTranslationCfg);

        if (sdlRet != SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_isRegionOverlap positive test failed on line no: %d \n", __LINE__);
        }
    }
	
	

/*******************************************************************************************
*     Call SDL API SDL_RAT_isRegionTranslationEnabled
*******************************************************************************************/

    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_isRegionTranslationEnabled(pRatRegs, 4u);

        if (sdlRet != SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_isRegionTranslationEnabled positive test failed on line no: %d \n", __LINE__);
        }
    }
	
	

/*******************************************************************************************
*     Call SDL API SDL_RAT_validateTranslationConfig
*******************************************************************************************/

    if (testStatus == SDL_APP_TEST_PASS)
    {
		pTranslationCfg.translatedAddress = ((uint64_t)&SDL_RATTestArray);
		pTranslationCfg.sizeInBytes		  = sizeof(SDL_RATTestArray);
		pTranslationCfg.baseAddress 	  = (uint32_t)SDL_RAT_SELF_TEST_TRANSLATE_BASE;
		
        sdlRet = SDL_RAT_validateTranslationConfig(pRatRegs, 4u, &pTranslationCfg);

        if (sdlRet != SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_validateTranslationConfig positive test failed on line no: %d \n", __LINE__);
        }
    }
	
	

/*******************************************************************************************
*     Call SDL API SDL_RAT_getMaxRegions
*******************************************************************************************/

    if (testStatus == SDL_APP_TEST_PASS)
    {
        SDL_RAT_getMaxRegions(pRatRegs);

        if (sdlRet != SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_getMaxRegions positive test failed on line no: %d \n", __LINE__);
        }
    }
	
	if (testStatus == SDL_APP_TEST_PASS)
    {
		SDL_ratRegs ratRegs;
		
        SDL_RAT_getMaxRegions(&ratRegs);

        if (sdlRet != SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_getMaxRegions positive test failed on line no: %d \n", __LINE__);
        }
    }
	

return (testStatus);
}
