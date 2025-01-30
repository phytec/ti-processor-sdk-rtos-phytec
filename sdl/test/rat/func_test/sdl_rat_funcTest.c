/*
 * SDL RAT
 *
 * Software Diagnostics Library Test for RAT module
 *
 *  Copyright (c) Texas Instruments Incorporated 2021
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

#include "main.h"
#include "disable_cache.h"

/* Choosing unused address space */
#define SDL_RAT_SELF_TEST_TRANSLATE_BASE  (0x10000000U)

__attribute((section(".sdl_rat_testsection"))) __attribute__((aligned(32))) uint32_t SDL_RATTestVar;

#define SDL_RAT_REGION_INDEX     0

/*********************************************************************
 *
 * \brief Test RAT module
 *
 *
 * \return SDL_PASS if test pass, SDL_EFAIL in case of failure
 */
int32_t SDL_RAT_funcTest(void)
{
    int32_t retValue=SDL_PASS;
    SDL_ratTranslationCfgInfo translationCfg;

    UART_printf("\n RAT Test: starting");

    translationCfg.translatedAddress = ((uint64_t)&SDL_RATTestVar);
    translationCfg.sizeInBytes = sizeof(SDL_RATTestVar);
    translationCfg.baseAddress = (uint32_t)SDL_RAT_SELF_TEST_TRANSLATE_BASE;

    retValue = SDL_RATSelfTest(SDL_RAT_REGION_INDEX, &translationCfg, &SDL_RATTestVar);

    return retValue;

}
/*********************************************************************
 *
 * \brief Self test for RAT module
 * Note: This function assumes both the mapped and translated addresses are accessible directly
 * for the self test
 * Note: The values in the region may be overwritten
 *
 * \param regionIndex: RAT region index to be used for the test
 * \param pTranslateCfg: Pointer to translation configuration for
 *                       the RAT mapping
 * \param pTestMemArea: Test memory area for doing the RAT region mapping
 *
 * \return SDL_PASS if test pass, SDL_EFAIL in case of failure
 */
/* Note this function assumes both the mapped and translated addresses are accessible directly
 * for the self test
 * Note: The values in the region may be overwritten
 */
int32_t SDL_RATSelfTest(uint32_t regionIndex, SDL_ratTranslationCfgInfo *pTranslationCfg,
                         uint32_t *pTestMemArea)
{
    int32_t retValue=SDL_PASS;
    int32_t sdlStatus;
    bool result;
	uint32_t *pMappedBase;
	SDL_ratStaticRegs pStaticRegs;
	SDL_ratExceptionInfo exceptionInfo;
	
	#if defined (SOC_J721E)
	SDL_ratRegs *pRatRegs = (SDL_ratRegs *)SDL_MCU_ARMSS_RAT_CFG_BASE;
	#endif
	
	#if defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
	SDL_ratRegs *pRatRegs = (SDL_ratRegs *)SDL_MCU_R5FSS0_RAT_CFG_BASE;
	#endif
	
    /* The assumption is that the tranlated Address is accessible directly as well
     * for Self test to work.
     */
	 
    if ((pTranslationCfg->translatedAddress > (uint64_t)0xffffffffu)
            && (pTranslationCfg->translatedAddress != (uint64_t)pTestMemArea)){
        retValue = SDL_EFAIL;
    }

    if (retValue == SDL_PASS) {

        /* Set up RAT translation */
        result = SDL_RAT_configRegionTranslation(pRatRegs, regionIndex, pTranslationCfg);
        if(result != SDL_PASS) {
            retValue = SDL_EFAIL;
        }
    }

    if (retValue == SDL_PASS) {
        /* Verify RAT configuration */
        sdlStatus = SDL_RAT_verifyConfigRegionTranslation(pRatRegs, regionIndex, pTranslationCfg);
        if (sdlStatus != SDL_PASS) {
            retValue = SDL_EFAIL;
        }
    }

    if (retValue == SDL_PASS) {
        /* Now write another value to test area */
        SDL_RATTestVar = 0xaaaa5555;

        pMappedBase = ((uint32_t *)(SDL_RAT_SELF_TEST_TRANSLATE_BASE));

        /* Check the first written value */
        if (*pMappedBase != SDL_RATTestVar) {
            retValue = SDL_EFAIL;
        }
    }
	
	SDL_RAT_getStaticRegs(pRatRegs, SDL_RAT_REGION_INDEX, &pStaticRegs);
	UART_printf("\n pStaticRegs RAT_EXCEPTION_ENABLE_SET value %d", pStaticRegs.RAT_EXCEPTION_ENABLE_SET);
	
    /* Disable RAT region */
    SDL_RAT_disableRegionTranslation(pRatRegs, regionIndex);	
	
	/*This part of code is for error injecting when illegal boundary transaction occurs */
	
	pTranslationCfg->translatedAddress = ((uint64_t)&SDL_RATTestVar);
	pTranslationCfg->sizeInBytes = 1u;
	pTranslationCfg->baseAddress = (uint32_t)SDL_RAT_SELF_TEST_TRANSLATE_BASE;
	
	UART_printf("\n\n TEST for error injecting when illegal boundary transaction occurs.\n\n");
	
	if (retValue == SDL_PASS) {
		
		/* disabling the cache */
		disable_cache();
		
        UART_printf(" 	Enabling the Exception Log");
		/* Enabling the interrupt */
		SDL_RAT_enableIntr(pRatRegs, SDL_RAT_INTR_SRC_EXCEPTION);

		UART_printf("\n 	Configuring the RAT region");
        /* Set up RAT translation */
        retValue = SDL_RAT_configRegionTranslation(pRatRegs, SDL_RAT_REGION_INDEX, pTranslationCfg);		
    }
	
	if(retValue == SDL_PASS)
	{
		/* Verify RAT configuration */
		retValue = SDL_RAT_verifyConfigRegionTranslation(pRatRegs, SDL_RAT_REGION_INDEX, pTranslationCfg);
	}
	
	if(retValue == SDL_PASS)
	{
		pMappedBase = ((uint32_t *)(SDL_RAT_SELF_TEST_TRANSLATE_BASE));
		
                
        /* Writing 4 byte value to RAT region to make it cross the boundary */
        *pMappedBase = (uint32_t)0xaaaa5555;
		UART_printf("\n 	Writing 0xaaaa5555 to RAT region to cross the boundary");
		/* Capturing the Exception log */
		retValue = SDL_RAT_getExceptionInfo(pRatRegs, &exceptionInfo);
	}
	
	/* if code value is 1 then boundary crossing error has been detected */
	if(exceptionInfo.code == 1)
	{		
		UART_printf("\n 	Exception info has been logged.\n\n");
	}
	else
	{
		retValue = SDL_EFAIL;
	}
	
	/* Disabling the interrupt */
	SDL_RAT_disableIntr(pRatRegs, SDL_RAT_INTR_SRC_EXCEPTION);
	
	/* Disable RAT region */
    SDL_RAT_disableRegionTranslation(pRatRegs, SDL_RAT_REGION_INDEX);
	
    return retValue;
}
