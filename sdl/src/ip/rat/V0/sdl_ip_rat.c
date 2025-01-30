/*
 *   Copyright (c) Texas Instruments Incorporated 2021
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

#include <sdl_ip_rat.h>



/********************************************************************************************************
*   API for getting the RAT config
*********************************************************************************************************/
/**
 *  Design: PROC_SDL-2432
 */
int32_t SDL_RAT_getRegionTranslationConfig(const SDL_ratRegs *pRatRegs, uint32_t regionIndex, 
											   SDL_ratTranslationCfgInfo *pTranslationCfg)
{
    int32_t sdlResult = SDL_EFAIL;

    if ((pRatRegs != NULL)                         &&
        (regionIndex < SDL_RAT_getMaxRegions(pRatRegs)) &&  
        (pTranslationCfg != NULL))
    {
        uint32_t regVal, regVal1;
        /*---------------------------------------------------------------------
         * Read translation configuration parameters from registers into their
         * respective structure elements for this region
         *-------------------------------------------------------------------*/
        pTranslationCfg->sizeInBytes = (uint64_t)1 << SDL_REG32_FEXT( &pRatRegs->REGION[regionIndex].CTRL, RAT_REGION_CTRL_SIZE );
        pTranslationCfg->baseAddress = SDL_REG32_RD( &pRatRegs->REGION[regionIndex].BASE );
        regVal  = SDL_REG32_RD( &pRatRegs->REGION[regionIndex].TRANS_L );
        regVal1 = SDL_REG32_RD( &pRatRegs->REGION[regionIndex].TRANS_U );
        pTranslationCfg->translatedAddress  = (((uint64_t)regVal1 & 0x0000FFFFU) << 32U) | (uint64_t)regVal;
        sdlResult = SDL_PASS;
    }
	else
	{
		sdlResult = SDL_EBADARGS;
	}
    return sdlResult;
}



/********************************************************************************************************
*   API converts value into a logarithmic value
*********************************************************************************************************/
/**
 *  Design: PROC_SDL-2433
 */
uint32_t SDL_RAT_lmo(uint64_t v)
{
    uint32_t r = 0U;  /* r will be lg(v) */
    uint64_t local_v = v;

    local_v = local_v >>(uint64_t) 1UL;
    while (local_v != (uint64_t)0UL)
    {
      r++;
      local_v = local_v >> (uint64_t)1UL;
    }
    return r;
}



/************************************************************************************************
*   API to set/enable the RAT
************************************************************************************************/
/**
 *  Design: PROC_SDL-2434
 */
int32_t SDL_RAT_setRegionTranslationEnable(SDL_ratRegs *pRatRegs, uint32_t regionIndex, bool bEnable)
{
    int32_t sdlResult = SDL_EFAIL;

    if ((pRatRegs != NULL) && (regionIndex < SDL_RAT_getMaxRegions(pRatRegs)))
    {
        uint32_t regVal;

        if( bEnable == (bool)true )
        {
            regVal = ((uint32_t)1U);
        }
        else
        {
            regVal = ((uint32_t)0U);
        }
        SDL_REG32_FINS( &pRatRegs->REGION[regionIndex].CTRL, RAT_REGION_CTRL_EN, regVal );
        sdlResult = SDL_PASS;
    }
	else
	{
		sdlResult = SDL_EBADARGS;
	}

    return sdlResult;
}



/********************************************************************************************************
*   API to set the Interrupt
*********************************************************************************************************/
/**
 *  Design: PROC_SDL-2435
 */
void SDL_RAT_setIntEnable(SDL_ratRegs *pRatRegs, SDL_RAT_intrSrcTypes intrSrc, bool bEnable)
{
    if((pRatRegs != NULL) && (intrSrc == SDL_RAT_INTR_SRC_EXCEPTION))
    {
        if(bEnable == (bool)true)
        {
            SDL_REG32_WR( &pRatRegs->EXCEPTION_ENABLE_SET, SDL_FMK( RAT_EXCEPTION_ENABLE_SET_ENABLE_SET, (uint32_t)1 ) );
        }
        else
        {
            SDL_REG32_WR( &pRatRegs->EXCEPTION_ENABLE_CLEAR, SDL_FMK( RAT_EXCEPTION_ENABLE_CLEAR_ENABLE_CLR, (uint32_t)1 ) );
        }
    }
}



/********************************************************************************************************
*   API to set the Exception logging enable
*********************************************************************************************************/
/**
 *  Design: PROC_SDL-2436
 */
void SDL_RAT_setExceptionLoggingEnable(SDL_ratRegs *pRatRegs, bool bEnable)
{
    if(pRatRegs != NULL)
	{
		SDL_REG32_FINS(&pRatRegs->EXCEPTION_LOGGING_CONTROL, RAT_EXCEPTION_LOGGING_CONTROL_DISABLE_F, (bEnable ? 0U : 1U) );
	}
}



/********************************************************************************************************
*   API to set the Exception logging Interrupt enable
*********************************************************************************************************/
/**
 *  Design: PROC_SDL-2437
 */
void SDL_RAT_setExceptionLoggingIntEnable(SDL_ratRegs *pRatRegs, bool bEnable)
{
	if(pRatRegs != NULL)
	{
		SDL_REG32_FINS(&pRatRegs->EXCEPTION_LOGGING_CONTROL, RAT_EXCEPTION_LOGGING_CONTROL_DISABLE_INTR, (bEnable ? 0U : 1U));
	}
}



/********************************************************************************************************
*   API for checking the value is Power of 2
*********************************************************************************************************/
/**
 *  Design: PROC_SDL-2438
 */
int32_t SDL_RAT_isPowerOf2(uint64_t v)
{
    return ((v != (uint64_t)0UL) && ((v & (v - (uint64_t)1UL)) == (uint64_t)0UL))? (SDL_PASS) : (SDL_EFAIL);
}



/********************************************************************************************************
*   API for checking overlap regions
*********************************************************************************************************/
/**
 *  Design: PROC_SDL-2439
 */
int32_t SDL_RAT_isRegionOverlap(const SDL_ratRegs *pRatRegs, uint32_t thisRegionIndex, 
									const SDL_ratTranslationCfgInfo *pTranslationCfg)
{
    uint32_t                    regionIndex;
    SDL_ratTranslationCfgInfo   xlatCfg;
    int32_t                     sdlResult = SDL_PASS;
	uint64_t 					r0l, r0h, r1l, r1h;

	if ((pRatRegs != NULL)                         &&
        (thisRegionIndex < SDL_RAT_getMaxRegions(pRatRegs)) &&  
        (pTranslationCfg != NULL))
	{
		for( regionIndex = ((uint32_t)0U); regionIndex < SDL_RAT_getMaxRegions(pRatRegs); regionIndex++ )
		{
			if( (regionIndex != thisRegionIndex) && (SDL_RAT_isRegionTranslationEnabled( pRatRegs, regionIndex ) == SDL_PASS) )
			{
			    (void)SDL_RAT_getRegionTranslationConfig(pRatRegs, regionIndex, &xlatCfg);
	
				r0l = pTranslationCfg->baseAddress;
				r0h = r0l + pTranslationCfg->sizeInBytes;
				r1l = xlatCfg.baseAddress;
				r1h = r1l + xlatCfg.sizeInBytes;
				if( !((r0l >= r1h) || (r1l >= r0h)) )
				{
					sdlResult = SDL_EFAIL;
				}
				
			}
		}
	}
	else
	{
		sdlResult = SDL_EBADARGS;
	}
    return sdlResult;
}



/********************************************************************************************************
*   API for checking RAT is enabled or not
*********************************************************************************************************/
/**
 *  Design: PROC_SDL-2440
 */
int32_t SDL_RAT_isRegionTranslationEnabled(const SDL_ratRegs *pRatRegs, uint32_t regionIndex)
{
    int32_t sdlResult = SDL_EFAIL;

    if (pRatRegs != NULL)
    {
        if( (regionIndex < SDL_RAT_getMaxRegions(pRatRegs))  &&
            (SDL_REG32_FEXT(&pRatRegs->REGION[regionIndex].CTRL, RAT_REGION_CTRL_EN) != 0U) )
        {
            sdlResult = SDL_PASS;
        }
    }
    return sdlResult;
}



/************************************************************************************************
*   API to validate RAT configuration
************************************************************************************************/
/**
 *  Design: PROC_SDL-2462
 */
int32_t SDL_RAT_validateTranslationConfig(const SDL_ratRegs *pRatRegs, uint32_t regionIndex, 
											  SDL_ratTranslationCfgInfo *pTranslationCfg)
{
    uint64_t  regionSize;
    int32_t   sdlResult  = SDL_PASS;

    /*-------------------------------------------------------------------------
     * If the region size is not a power-of-2, round-it-up to the next
     * power-of-2 size.
     *-----------------------------------------------------------------------*/
    if ((pRatRegs != NULL)                         &&
        (regionIndex < SDL_RAT_getMaxRegions(pRatRegs)) &&  
        (pTranslationCfg != NULL))
    {
		regionSize = pTranslationCfg->sizeInBytes;
		if(SDL_RAT_isPowerOf2(regionSize) == SDL_EFAIL )
		{
			regionSize = (uint64_t)1 << (SDL_RAT_lmo(regionSize)+1U);
			pTranslationCfg->sizeInBytes = regionSize;
		}
		/*-------------------------------------------------------------------------
		* The region base address and translated base address must be aligned to
		* the defined region size. So if the region size is defined as 64KB, then
		* the two addresses must be 64KB aligned. The module does not check for
		* this so it is SW's responsibility. Regions that are not aligned will
		* have unpredictable results.
		*
		* Multiple region definitions must not overlap in their covered address
		* space. The module does not check for this so it is SW's responsibility.
		* If they do overlap, then there will be unpredictable results.
		*-----------------------------------------------------------------------*/
		if( (((uint64_t)pTranslationCfg->baseAddress & (uint64_t)(regionSize-1UL)) != 0UL)  ||
			((pTranslationCfg->translatedAddress     & (uint64_t)(regionSize-1UL)) != 0UL)  ||
			(SDL_RAT_isRegionOverlap(pRatRegs, regionIndex, pTranslationCfg ) != SDL_PASS) )
		{
			sdlResult = SDL_EFAIL;
		}
	}
	else
	{
		sdlResult = SDL_EBADARGS;
	}
	
    return sdlResult;
}



/********************************************************************************************************
*   API to get Maximum number of refions for RAT
*********************************************************************************************************/
/**
 *  Design: PROC_SDL-2463
 */
uint32_t SDL_RAT_getMaxRegions(const SDL_ratRegs *pRatRegs)
{
    uint32_t   rtlRev;
    uint32_t   maxRegions = 0U;

    if (pRatRegs != NULL)
    {
        rtlRev = SDL_REG32_FEXT( &pRatRegs->PID, RAT_PID_RTL );
        /* Read max regions from CONFIG register only if rtlRev > 1 */
        if( rtlRev > 1U )
        {
            maxRegions = SDL_REG32_FEXT( &pRatRegs->CONFIG, RAT_CONFIG_REGIONS );
        }
        else
        {
            maxRegions = ((uint32_t)16U);
        }
    }
    return maxRegions;
}
