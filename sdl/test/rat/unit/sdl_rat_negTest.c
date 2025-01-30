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

/* This file contains RAT API negative test code */

#include "rat_test_main.h"

int32_t SDL_RAT_negTest()
{
    int32_t       testStatus = SDL_APP_TEST_PASS, sdlRet;
    uint32_t 	  destId;
	
	SDL_ratTranslationCfgInfo  pTranslationCfg;
	SDL_ratStaticRegs pStaticRegs;
	SDL_ratExceptionInfo pExceptionInfo;
	
	#if defined (SOC_J721E)
	SDL_ratRegs *pRatRegs = (SDL_ratRegs *)SDL_MCU_ARMSS_RAT_CFG_BASE;
	#endif
	
	#if defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
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
        sdlRet = SDL_RAT_configRegionTranslation(NULL, 46, NULL);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("SDL_RAT_configRegionTranslation: failure on line no. %d \n", __LINE__);
        }
     }
	 
	 if (testStatus == SDL_APP_TEST_PASS)
     {
        sdlRet = SDL_RAT_configRegionTranslation(pRatRegs, 46, NULL);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("SDL_RAT_configRegionTranslation: failure on line no. %d \n", __LINE__);
        }
     }
	 
	 if (testStatus == SDL_APP_TEST_PASS)
     {
        sdlRet = SDL_RAT_configRegionTranslation(NULL, 4, NULL);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("SDL_RAT_configRegionTranslation: failure on line no. %d \n", __LINE__);
        }
     }
	 
	 if (testStatus == SDL_APP_TEST_PASS)
     {
        sdlRet = SDL_RAT_configRegionTranslation(NULL, 46, &pTranslationCfg);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("SDL_RAT_configRegionTranslation: failure on line no. %d \n", __LINE__);
        }
     }
	 
	 if (testStatus == SDL_APP_TEST_PASS)
     {
        sdlRet = SDL_RAT_configRegionTranslation(pRatRegs, 4, NULL);

        if (sdlRet == SDL_PASS)
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
        sdlRet = SDL_RAT_verifyConfigRegionTranslation(NULL, 46, NULL);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("SDL_RAT_verifyConfigRegionTranslation: failure on line no. %d \n", __LINE__);
        }
    }
	 
	if (testStatus == SDL_APP_TEST_PASS)
    {
       sdlRet = SDL_RAT_verifyConfigRegionTranslation(pRatRegs, 46, NULL);

       if (sdlRet == SDL_PASS)
       {
           testStatus = SDL_APP_TEST_FAILED;
           UART_printf("SDL_RAT_verifyConfigRegionTranslation: failure on line no. %d \n", __LINE__);
       }
    }
	
	if (testStatus == SDL_APP_TEST_PASS)
    {
       sdlRet = SDL_RAT_verifyConfigRegionTranslation(NULL, 4, NULL);

       if (sdlRet == SDL_PASS)
       {
           testStatus = SDL_APP_TEST_FAILED;
           UART_printf("SDL_RAT_verifyConfigRegionTranslation: failure on line no. %d \n", __LINE__);
       }
    }
	
	if (testStatus == SDL_APP_TEST_PASS)
    {
       sdlRet = SDL_RAT_verifyConfigRegionTranslation(NULL, 46, &pTranslationCfg);

       if (sdlRet == SDL_PASS)
       {
           testStatus = SDL_APP_TEST_FAILED;
           UART_printf("SDL_RAT_verifyConfigRegionTranslation: failure on line no. %d \n", __LINE__);
       }
    }
	
	if (testStatus == SDL_APP_TEST_PASS)
    {
       sdlRet = SDL_RAT_verifyConfigRegionTranslation(pRatRegs, 4, NULL);

       if (sdlRet == SDL_PASS)
       {
           testStatus = SDL_APP_TEST_FAILED;
           UART_printf("SDL_RAT_verifyConfigRegionTranslation: failure on line no. %d \n", __LINE__);
       }
    }
	
	if (testStatus == SDL_APP_TEST_PASS)
    {
		pRatRegs->REGION[4].CTRL = 0;
       sdlRet = SDL_RAT_verifyConfigRegionTranslation(pRatRegs, 4, &pTranslationCfg);

       if (sdlRet == SDL_PASS)
       {
           testStatus = SDL_APP_TEST_FAILED;
           UART_printf("SDL_RAT_verifyConfigRegionTranslation: failure on line no. %d \n", __LINE__);
       }
    }

	if (testStatus == SDL_APP_TEST_PASS)
     {
		pTranslationCfg.translatedAddress = ((uint64_t)&SDL_RATTestArray);
		pTranslationCfg.sizeInBytes		  = sizeof(SDL_RATTestArray);
		pTranslationCfg.baseAddress 	  = (uint32_t)SDL_RAT_SELF_TEST_TRANSLATE_BASE;
		SDL_RAT_configRegionTranslation(pRatRegs, 4, &pTranslationCfg);
		
		pTranslationCfg.translatedAddress = ((uint64_t)&SDL_RATTestArray);
		pTranslationCfg.sizeInBytes		  = 64;
		pTranslationCfg.baseAddress 	  = (uint32_t)SDL_RAT_SELF_TEST_TRANSLATE_BASE;
        sdlRet = SDL_RAT_verifyConfigRegionTranslation(pRatRegs, 4, &pTranslationCfg);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("SDL_RAT_configRegionTranslation: failure on line no. %d \n", __LINE__);
        }
     }
	 
	 if (testStatus == SDL_APP_TEST_PASS)
     {
		pTranslationCfg.translatedAddress = ((uint64_t)&SDL_RATTestArray);
		pTranslationCfg.sizeInBytes		  = sizeof(SDL_RATTestArray);
		pTranslationCfg.baseAddress 	  = (uint32_t)SDL_RAT_SELF_TEST_TRANSLATE_BASE;
		SDL_RAT_configRegionTranslation(pRatRegs, 4, &pTranslationCfg);
		
		pTranslationCfg.translatedAddress = ((uint64_t)&SDL_RATTestArray);
		pTranslationCfg.sizeInBytes		  = sizeof(SDL_RATTestArray);
		pTranslationCfg.baseAddress 	  = (uint32_t)(0x9000000U);
        sdlRet = SDL_RAT_verifyConfigRegionTranslation(pRatRegs, 4, &pTranslationCfg);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("SDL_RAT_configRegionTranslation: failure on line no. %d \n", __LINE__);
        }
     }
	 
	 if (testStatus == SDL_APP_TEST_PASS)
     {
		pTranslationCfg.translatedAddress = ((uint64_t)&SDL_RATTestArray);
		pTranslationCfg.sizeInBytes		  = sizeof(SDL_RATTestArray);
		pTranslationCfg.baseAddress 	  = (uint32_t)SDL_RAT_SELF_TEST_TRANSLATE_BASE;
		SDL_RAT_configRegionTranslation(pRatRegs, 4, &pTranslationCfg);
		
		pTranslationCfg.translatedAddress = ((uint64_t)&destId);
		pTranslationCfg.sizeInBytes		  = sizeof(SDL_RATTestArray);
		pTranslationCfg.baseAddress 	  = (uint32_t)SDL_RAT_SELF_TEST_TRANSLATE_BASE;
        sdlRet = SDL_RAT_verifyConfigRegionTranslation(pRatRegs, 4, &pTranslationCfg);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("SDL_RAT_configRegionTranslation: failure on line no. %d \n", __LINE__);
        }
     }
	 
	 if (testStatus == SDL_APP_TEST_PASS)
     {
		pTranslationCfg.translatedAddress = ((uint32_t)&SDL_RATTestArray);
		pTranslationCfg.sizeInBytes		  = sizeof(SDL_RATTestArray);
		pTranslationCfg.baseAddress 	  = (uint32_t)SDL_RAT_SELF_TEST_TRANSLATE_BASE;
		SDL_RAT_configRegionTranslation(pRatRegs, 4, &pTranslationCfg);
		
		destId = 46;
		
		pTranslationCfg.translatedAddress = ((uint32_t)&destId);
		pTranslationCfg.sizeInBytes		  = sizeof(SDL_RATTestArray);
		pTranslationCfg.baseAddress 	  = (uint32_t)SDL_RAT_SELF_TEST_TRANSLATE_BASE;
        sdlRet = SDL_RAT_verifyConfigRegionTranslation(pRatRegs, 4, &pTranslationCfg);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("SDL_RAT_configRegionTranslation: failure on line no. %d \n", __LINE__);
        }
     }
	 


/*******************************************************************************************
*     Call SDL API SDL_RAT_getStaticRegs
*******************************************************************************************/
	 
	if (testStatus == SDL_APP_TEST_PASS)
    {
       sdlRet = SDL_RAT_getStaticRegs(NULL, 46, NULL);

       if (sdlRet == SDL_PASS)
       {
           testStatus = SDL_APP_TEST_FAILED;
           UART_printf("SDL_RAT_getStaticRegs: failure on line no. %d \n", __LINE__);
       }
    }
	
	if (testStatus == SDL_APP_TEST_PASS)
    {
       sdlRet = SDL_RAT_getStaticRegs(pRatRegs, 46, NULL);

       if (sdlRet == SDL_PASS)
       {
           testStatus = SDL_APP_TEST_FAILED;
           UART_printf("SDL_RAT_getStaticRegs: failure on line no. %d \n", __LINE__);
       }
    }
	
	if (testStatus == SDL_APP_TEST_PASS)
    {
       sdlRet = SDL_RAT_getStaticRegs(NULL, 4, NULL);

       if (sdlRet == SDL_PASS)
       {
           testStatus = SDL_APP_TEST_FAILED;
           UART_printf("SDL_RAT_getStaticRegs: failure on line no. %d \n", __LINE__);
       }
    }
	
	if (testStatus == SDL_APP_TEST_PASS)
    {
       sdlRet = SDL_RAT_getStaticRegs(NULL, 46, &pStaticRegs);

       if (sdlRet == SDL_PASS)
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
        sdlRet = SDL_RAT_enableRegionTranslation(NULL, 46);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_enableRegionTranslation negative test failed on line no: %d \n", __LINE__);
        }
    }
	
	if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_enableRegionTranslation(pRatRegs, 46);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_enableRegionTranslation negative test failed on line no: %d \n", __LINE__);
        }
    }
	
	if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_enableRegionTranslation(NULL, 4);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_enableRegionTranslation negative test failed on line no: %d \n", __LINE__);
        }
    }

/*******************************************************************************************
*     Call SDL API SDL_RAT_disableRegionTranslation
*******************************************************************************************/
	
	if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_disableRegionTranslation(NULL, 46);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_disableRegionTranslation negative test failed on line no: %d \n", __LINE__);
        }
    }
	
	if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_disableRegionTranslation(pRatRegs, 46);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_disableRegionTranslation negative test failed on line no: %d \n", __LINE__);
        }
    }
	
	if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_disableRegionTranslation(NULL, 4);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_disableRegionTranslation negative test failed on line no: %d \n", __LINE__);
        }
    }



/*******************************************************************************************
*     Call SDL API SDL_RAT_enableIntr
*******************************************************************************************/
	
	if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_enableIntr(NULL, SDL_RAT_INTR_SRC_INVALID);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_enableIntr negative test failed on line no: %d \n", __LINE__);
        }
    }
	
	if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_enableIntr(pRatRegs, SDL_RAT_INTR_SRC_INVALID);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_enableIntr negative test failed on line no: %d \n", __LINE__);
        }
    }
	
	if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_enableIntr(NULL, SDL_RAT_INTR_SRC_EXCEPTION);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_enableIntr negative test failed on line no: %d \n", __LINE__);
        }
    }
	
	

/*******************************************************************************************
*     Call SDL API SDL_RAT_setErrMsgDestId
*******************************************************************************************/

    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_setErrMsgDestId(NULL, SDL_RAT_DESTINATION_ID_DEST_ID_MAX);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_setErrMsgDestId negative test failed on line no: %d \n", __LINE__);
        }
    }
	
	if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_setErrMsgDestId(pRatRegs, (SDL_RAT_DESTINATION_ID_DEST_ID_MAX+1));

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_setErrMsgDestId negative test failed on line no: %d \n", __LINE__);
        }
    }
	
	if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_setErrMsgDestId(NULL, 46);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_setErrMsgDestId negative test failed on line no: %d \n", __LINE__);
        }
    }
	
	
	
/*******************************************************************************************
*     Call SDL API SDL_RAT_getErrMsgDestId
*******************************************************************************************/

    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_getErrMsgDestId(NULL, NULL);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_getErrMsgDestId negative test failed on line no: %d \n", __LINE__);
        }
    }
	
	if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_getErrMsgDestId(pRatRegs, NULL);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_getErrMsgDestId negative test failed on line no: %d \n", __LINE__);
        }
    }
	
	if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_getErrMsgDestId(NULL, &destId);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_getErrMsgDestId negative test failed on line no: %d \n", __LINE__);
        }
    }



/*******************************************************************************************
*     Call SDL API SDL_RAT_enableExceptionLog
*******************************************************************************************/

    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_enableExceptionLog(NULL);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_enableExceptionLog negative test failed on line no: %d \n", __LINE__);
        }
    }
	
	

/*******************************************************************************************
*     Call SDL API SDL_RAT_disableExceptionLog
*******************************************************************************************/

    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_disableExceptionLog(NULL);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_disableExceptionLog negative test failed on line no: %d \n", __LINE__);
        }
    }



/*******************************************************************************************
*     Call SDL API SDL_RAT_isIntrPending
*******************************************************************************************/
	
	if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_isIntrPending(NULL, SDL_RAT_INTR_SRC_INVALID);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_isIntrPending negative test failed on line no: %d \n", __LINE__);
        }
    }
	
	if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_isIntrPending(pRatRegs, SDL_RAT_INTR_SRC_INVALID);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_isIntrPending negative test failed on line no: %d \n", __LINE__);
        }
    }
	
	if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_isIntrPending(NULL, SDL_RAT_INTR_SRC_EXCEPTION);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_isIntrPending negative test failed on line no: %d \n", __LINE__);
        }
    }
	
	if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_clrIntrPending(pRatRegs, SDL_RAT_INTR_SRC_EXCEPTION);

        if (sdlRet != SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_clrIntrPending positive test failed on line no: %d \n", __LINE__);
        }
    }	
	
	if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_isIntrPending(pRatRegs, SDL_RAT_INTR_SRC_EXCEPTION);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_isIntrPending negative test failed on line no: %d \n", __LINE__);
        }
    }



/*******************************************************************************************
*     Call SDL API SDL_RAT_setIntrPending
*******************************************************************************************/
	
	if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_setIntrPending(NULL, SDL_RAT_INTR_SRC_INVALID);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_setIntrPending negative test failed on line no: %d \n", __LINE__);
        }
    }
	
	if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_setIntrPending(pRatRegs, SDL_RAT_INTR_SRC_INVALID);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_setIntrPending negative test failed on line no: %d \n", __LINE__);
        }
    }
	
	if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_setIntrPending(NULL, SDL_RAT_INTR_SRC_EXCEPTION);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_setIntrPending negative test failed on line no: %d \n", __LINE__);
        }
    }
	
	

/*******************************************************************************************
*     Call SDL API SDL_RAT_clrIntrPending
*******************************************************************************************/
	
	if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_clrIntrPending(NULL, SDL_RAT_INTR_SRC_INVALID);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_clrIntrPending negative test failed on line no: %d \n", __LINE__);
        }
    }
	
	if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_clrIntrPending(pRatRegs, SDL_RAT_INTR_SRC_INVALID);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_clrIntrPending negative test failed on line no: %d \n", __LINE__);
        }
    }
	
	if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_clrIntrPending(NULL, SDL_RAT_INTR_SRC_EXCEPTION);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_clrIntrPending negative test failed on line no: %d \n", __LINE__);
        }
    }
	
	

/*******************************************************************************************
*     Call SDL API SDL_RAT_ackIntr
*******************************************************************************************/
	
	if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_ackIntr(NULL, SDL_RAT_INTR_SRC_INVALID);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_ackIntr negative test failed on line no: %d \n", __LINE__);
        }
    }
	
	if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_ackIntr(pRatRegs, SDL_RAT_INTR_SRC_INVALID);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_ackIntr negative test failed on line no: %d \n", __LINE__);
        }
    }
	
	if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_ackIntr(NULL, SDL_RAT_INTR_SRC_EXCEPTION);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_ackIntr negative test failed on line no: %d \n", __LINE__);
        }
    }
	
	

/*******************************************************************************************
*     Call SDL API SDL_RAT_disableIntr
*******************************************************************************************/
	
	if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_disableIntr(NULL, SDL_RAT_INTR_SRC_INVALID);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_disableIntr negative test failed on line no: %d \n", __LINE__);
        }
    }
	
	if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_disableIntr(pRatRegs, SDL_RAT_INTR_SRC_INVALID);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_disableIntr negative test failed on line no: %d \n", __LINE__);
        }
    }
	
	if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_disableIntr(NULL, SDL_RAT_INTR_SRC_EXCEPTION);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_disableIntr negative test failed on line no: %d \n", __LINE__);
        }
    }
		
	

/*******************************************************************************************
*     Call SDL API SDL_RAT_getExceptionInfo
*******************************************************************************************/

    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_getExceptionInfo(NULL, NULL);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_getExceptionInfo negative test failed on line no: %d \n", __LINE__);
        }
    }
	
	if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_getExceptionInfo(pRatRegs, NULL);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_getExceptionInfo negative test failed on line no: %d \n", __LINE__);
        }
    }
	
	if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_getExceptionInfo(NULL, &pExceptionInfo);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_getExceptionInfo negative test failed on line no: %d \n", __LINE__);
        }
    }
	


/*  Below are the low level functions test cases  */
/*******************************************************************************************
*     Call SDL API SDL_RAT_getRegionTranslationConfig
*******************************************************************************************/
	
	if (testStatus == SDL_APP_TEST_PASS)
     {
        sdlRet = SDL_RAT_getRegionTranslationConfig(NULL, 46, NULL);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("SDL_RAT_getRegionTranslationConfig: failure on line no. %d \n", __LINE__);
        }
     }
	 
	 if (testStatus == SDL_APP_TEST_PASS)
     {
        sdlRet = SDL_RAT_getRegionTranslationConfig(pRatRegs, 46, NULL);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("SDL_RAT_getRegionTranslationConfig: failure on line no. %d \n", __LINE__);
        }
     }
	 
	 if (testStatus == SDL_APP_TEST_PASS)
     {
        sdlRet = SDL_RAT_getRegionTranslationConfig(NULL, 4, NULL);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("SDL_RAT_getRegionTranslationConfig: failure on line no. %d \n", __LINE__);
        }
     }
	 
	 if (testStatus == SDL_APP_TEST_PASS)
     {
        sdlRet = SDL_RAT_getRegionTranslationConfig(NULL, 46, &pTranslationCfg);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("SDL_RAT_getRegionTranslationConfig: failure on line no. %d \n", __LINE__);
        }
     }
	 
	 if (testStatus == SDL_APP_TEST_PASS)
     {
        sdlRet = SDL_RAT_getRegionTranslationConfig(pRatRegs, 4, NULL);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("SDL_RAT_getRegionTranslationConfig: failure on line no. %d \n", __LINE__);
        }
     }
	
	

/*******************************************************************************************
*     Call SDL API SDL_RAT_lmo
*******************************************************************************************/

    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_lmo(46);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_lmo negative test failed on line no: %d \n", __LINE__);
        }
    }
	
	

/*******************************************************************************************
*     Call SDL API SDL_RAT_setRegionTranslationEnable
*******************************************************************************************/
	
	if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_setRegionTranslationEnable(NULL, 46, TRUE);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_setRegionTranslationEnable negative test failed on line no: %d \n", __LINE__);
        }
    }
	
	if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_setRegionTranslationEnable(pRatRegs, 46, TRUE);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_setRegionTranslationEnable negative test failed on line no: %d \n", __LINE__);
        }
    }
	
	if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_setRegionTranslationEnable(NULL, 4u, TRUE);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_setRegionTranslationEnable negative test failed on line no: %d \n", __LINE__);
        }
    }
	
	

/*******************************************************************************************
*     Call SDL API SDL_RAT_setIntEnable
*******************************************************************************************/

    if (testStatus == SDL_APP_TEST_PASS)
    {
        SDL_RAT_setIntEnable(NULL, SDL_RAT_INTR_SRC_INVALID, TRUE);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_setIntEnable negative test failed on line no: %d \n", __LINE__);
        }
    }
	
	if (testStatus == SDL_APP_TEST_PASS)
    {
        SDL_RAT_setIntEnable(pRatRegs, SDL_RAT_INTR_SRC_INVALID, TRUE);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_setIntEnable negative test failed on line no: %d \n", __LINE__);
        }
    }
	
	if (testStatus == SDL_APP_TEST_PASS)
    {
        SDL_RAT_setIntEnable(NULL, SDL_RAT_INTR_SRC_EXCEPTION, TRUE);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_setIntEnable negative test failed on line no: %d \n", __LINE__);
        }
    }
	
	

/*******************************************************************************************
*     Call SDL API SDL_RAT_setExceptionLoggingEnable
*******************************************************************************************/

    if (testStatus == SDL_APP_TEST_PASS)
    {
        SDL_RAT_setExceptionLoggingEnable(NULL, TRUE);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_setExceptionLoggingEnable negative test failed on line no: %d \n", __LINE__);
        }
    }
	
	

/*******************************************************************************************
*     Call SDL API SDL_RAT_setExceptionLoggingIntEnable
*******************************************************************************************/

    if (testStatus == SDL_APP_TEST_PASS)
    {
        SDL_RAT_setExceptionLoggingIntEnable(NULL, TRUE);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_setExceptionLoggingIntEnable negative test failed on line no: %d \n", __LINE__);
        }

    }
	
	

/*******************************************************************************************
*     Call SDL API SDL_RAT_isPowerOf2
*******************************************************************************************/

    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_isPowerOf2(46);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_isPowerOf2 negative test failed on line no: %d \n", __LINE__);
        }
    }
	
	if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_isPowerOf2(0);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_isPowerOf2 negative test failed on line no: %d \n", __LINE__);
        }
    }
	
	

/*******************************************************************************************
*     Call SDL API SDL_RAT_isRegionOverlap
*******************************************************************************************/
	
	if (testStatus == SDL_APP_TEST_PASS)
     {
        sdlRet = SDL_RAT_isRegionOverlap(NULL, 46, NULL);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("SDL_RAT_isRegionOverlap: failure on line no. %d \n", __LINE__);
        }
     }
	 
	 if (testStatus == SDL_APP_TEST_PASS)
     {
        sdlRet = SDL_RAT_isRegionOverlap(pRatRegs, 46, NULL);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("SDL_RAT_isRegionOverlap: failure on line no. %d \n", __LINE__);
        }
     }
	 
	 if (testStatus == SDL_APP_TEST_PASS)
     {
        sdlRet = SDL_RAT_isRegionOverlap(NULL, 4, NULL);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("SDL_RAT_isRegionOverlap: failure on line no. %d \n", __LINE__);
        }
     }
	 
	 if (testStatus == SDL_APP_TEST_PASS)
     {
        sdlRet = SDL_RAT_isRegionOverlap(NULL, 46, &pTranslationCfg);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("SDL_RAT_isRegionOverlap: failure on line no. %d \n", __LINE__);
        }
     }
	 
	 if (testStatus == SDL_APP_TEST_PASS)
     {
        sdlRet = SDL_RAT_isRegionOverlap(pRatRegs, 4, NULL);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("SDL_RAT_isRegionOverlap: failure on line no. %d \n", __LINE__);
        }
     }
	
	

/*******************************************************************************************
*     Call SDL API SDL_RAT_isRegionTranslationEnabled
*******************************************************************************************/

    if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_isRegionTranslationEnabled(NULL, 46u);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_isRegionTranslationEnabled negative test failed on line no: %d \n", __LINE__);
        }
    }
	
	if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_isRegionTranslationEnabled(pRatRegs, 46u);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_isRegionTranslationEnabled negative test failed on line no: %d \n", __LINE__);
        }
    }
	
	if (testStatus == SDL_APP_TEST_PASS)
    {
        sdlRet = SDL_RAT_isRegionTranslationEnabled(NULL, 4u);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_isRegionTranslationEnabled negative test failed on line no: %d \n", __LINE__);
        }
    }
	
	

/*******************************************************************************************
*     Call SDL API SDL_RAT_validateTranslationConfig
*******************************************************************************************/
	
	if (testStatus == SDL_APP_TEST_PASS)
     {
        sdlRet = SDL_RAT_validateTranslationConfig(NULL, 46, NULL);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("SDL_RAT_validateTranslationConfig: failure on line no. %d \n", __LINE__);
        }
     }
	 
	 if (testStatus == SDL_APP_TEST_PASS)
     {
        sdlRet = SDL_RAT_validateTranslationConfig(pRatRegs, 46, NULL);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("SDL_RAT_validateTranslationConfig: failure on line no. %d \n", __LINE__);
        }
     }
	 
	 if (testStatus == SDL_APP_TEST_PASS)
     {
        sdlRet = SDL_RAT_validateTranslationConfig(NULL, 4, NULL);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("SDL_RAT_validateTranslationConfig: failure on line no. %d \n", __LINE__);
        }
     }
	 
	 if (testStatus == SDL_APP_TEST_PASS)
     {
        sdlRet = SDL_RAT_validateTranslationConfig(NULL, 46, &pTranslationCfg);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("SDL_RAT_validateTranslationConfig: failure on line no. %d \n", __LINE__);
        }
     }
	 
	 if (testStatus == SDL_APP_TEST_PASS)
     {
        sdlRet = SDL_RAT_validateTranslationConfig(pRatRegs, 4, NULL);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("SDL_RAT_validateTranslationConfig: failure on line no. %d \n", __LINE__);
        }
     }
	
	

/*******************************************************************************************
*     Call SDL API SDL_RAT_getMaxRegions
*******************************************************************************************/

    if (testStatus == SDL_APP_TEST_PASS)
    {
        SDL_RAT_getMaxRegions(NULL);

        if (sdlRet == SDL_PASS)
        {
            testStatus = SDL_APP_TEST_FAILED;
            UART_printf("\n  SDL_RAT_getMaxRegions negative test failed on line no: %d \n", __LINE__);
        }
    }
	

	return (testStatus);
}
