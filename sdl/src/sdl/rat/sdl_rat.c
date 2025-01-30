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

/**
 *  \file     sdl_rat.c
 *
 *  \brief    This file contains the implementation of the APIs present in the
 *            device abstraction layer file of RAT.
 *            This also contains some related macros.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
/** \brief This is to disable HW_SYNC_BARRIER for register access */
#define MEM_BARRIER_DISABLE

#include <sdl_rat.h>

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/********************************************************************************************************
*   API for configuring the RAT module
*********************************************************************************************************/
/**
 *  Design: PROC_SDL-2235
 */
int32_t SDL_RAT_configRegionTranslation(SDL_ratRegs *pRatRegs, uint32_t regionIndex, 
								        SDL_ratTranslationCfgInfo * pTranslationCfg)
{
    int32_t sdlResult = SDL_EFAIL;

    if ((pRatRegs != NULL)                         &&
        (regionIndex < SDL_RAT_getMaxRegions(pRatRegs)) &&  
        (pTranslationCfg != NULL))
    {
        /*---------------------------------------------------------------------
         * Validate translation configuration parameters
         *-------------------------------------------------------------------*/
        if(SDL_RAT_validateTranslationConfig(pRatRegs, regionIndex, pTranslationCfg) == SDL_PASS )
        {
            /*-----------------------------------------------------------------
             * Write translation configuration parameters into respective
             * registers for this region
             *---------------------------------------------------------------*/
            sdlResult = SDL_RAT_disableRegionTranslation(pRatRegs, regionIndex);
            if (sdlResult == SDL_PASS)
            {
                SDL_REG32_WR( &pRatRegs->REGION[regionIndex].CTRL, SDL_RAT_lmo(pTranslationCfg->sizeInBytes) );
                SDL_REG32_WR( &pRatRegs->REGION[regionIndex].BASE, pTranslationCfg->baseAddress );
                SDL_REG32_WR( &pRatRegs->REGION[regionIndex].TRANS_L, pTranslationCfg->translatedAddress & 0xFFFFFFFFU );
                SDL_REG32_WR( &pRatRegs->REGION[regionIndex].TRANS_U, (pTranslationCfg->translatedAddress >> 32UL) & 0xFFFFFFFFU );
                /*-----------------------------------------------------------------
                 * Enable the region translation
                 *---------------------------------------------------------------*/
                sdlResult = SDL_RAT_enableRegionTranslation(pRatRegs, regionIndex);
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
*   API for verify the configuration of RAT module
*********************************************************************************************************/
/**
 *  Design: PROC_SDL-2236
 */
int32_t SDL_RAT_verifyConfigRegionTranslation(const SDL_ratRegs *pRatRegs, uint32_t regionIndex, 
											  SDL_ratTranslationCfgInfo *pTranslationCfg)
{
    int32_t  sdlResult = SDL_EFAIL;
    uint32_t ctrl, base, trans_l, trans_u;
    uint32_t ctrl_r, base_r, trans_l_r, trans_u_r;

    if ((pRatRegs != NULL)                         &&
        (regionIndex < SDL_RAT_getMaxRegions(pRatRegs)) &&
        (pTranslationCfg != NULL))
    {
        /*---------------------------------------------------------------------
         * Validate translation configuration parameters
         *-------------------------------------------------------------------*/
        if( SDL_RAT_validateTranslationConfig(pRatRegs, regionIndex, pTranslationCfg) == SDL_PASS )
        {
            ctrl    = SDL_RAT_lmo(pTranslationCfg->sizeInBytes);
            base    = pTranslationCfg->baseAddress;
            trans_l = (uint32_t) (pTranslationCfg->translatedAddress & (uint32_t)0xFFFFFFFFU);
            trans_u = (uint32_t) ((pTranslationCfg->translatedAddress >> (uint64_t)32UL) &(uint32_t)0xFFFFFFFFU);

            /* read the programmed values */
            ctrl_r = SDL_REG32_FEXT( &pRatRegs->REGION[regionIndex].CTRL, RAT_REGION_CTRL_SIZE );
            base_r = SDL_REG32_FEXT( &pRatRegs->REGION[regionIndex].BASE, RAT_REGION_BASE_BASE);
            trans_l_r = SDL_REG32_FEXT( &pRatRegs->REGION[regionIndex].TRANS_L, RAT_REGION_TRANS_L_LOWER );
            trans_u_r = SDL_REG32_FEXT( &pRatRegs->REGION[regionIndex].TRANS_U, RAT_REGION_TRANS_U_UPPER);

            /*-----------------------------------------------------------------
             * Check Enable the region translation
             *---------------------------------------------------------------*/
            sdlResult = SDL_RAT_isRegionTranslationEnabled(pRatRegs, regionIndex);
			
            if ( (sdlResult == SDL_PASS) &&
			(ctrl == ctrl_r) &&
			(base == base_r) &&
			(trans_l == trans_l_r) &&
			(trans_u == trans_u_r) )
            {
                sdlResult = SDL_PASS;
            }
			else
			{
				sdlResult = SDL_EFAIL;
			}
        }
    }
	else
	{
		sdlResult = SDL_EBADARGS;
	}
    return (sdlResult);
}



/********************************************************************************************************
*   API to read Static Registers
*********************************************************************************************************/
/**
 *  Design: PROC_SDL-2237
 */
int32_t SDL_RAT_getStaticRegs(SDL_ratRegs *pRatRegs, uint32_t regionIndex, SDL_ratStaticRegs * pStaticRegs)
{
	int32_t sdlResult = SDL_EFAIL;

    if ((pRatRegs != NULL) && (pStaticRegs != NULL))        
    {
		pStaticRegs->RAT_CTRL_k    = SDL_REG32_FEXT( &pRatRegs->REGION[regionIndex].CTRL, RAT_REGION_CTRL_SIZE );
        pStaticRegs->RAT_BASE_k    = SDL_REG32_FEXT( &pRatRegs->REGION[regionIndex].BASE, RAT_REGION_BASE_BASE);
        pStaticRegs->RAT_TRANS_L_k = SDL_REG32_FEXT( &pRatRegs->REGION[regionIndex].TRANS_L, RAT_REGION_TRANS_L_LOWER );
        pStaticRegs->RAT_TRANS_U_k = SDL_REG32_FEXT( &pRatRegs->REGION[regionIndex].TRANS_U, RAT_REGION_TRANS_U_UPPER);
		
		sdlResult = SDL_PASS;
	}
	else
	{
		sdlResult = SDL_EBADARGS;
	}
	return sdlResult;	
}



/********************************************************************************************************
*   API for enabling RAT
*********************************************************************************************************/
/**
 *  Design: PROC_SDL-2238
 */
int32_t SDL_RAT_enableRegionTranslation(SDL_ratRegs *pRatRegs, uint32_t regionIndex )
{
    int32_t sdlResult = SDL_EFAIL;

    if ((pRatRegs != NULL) && (regionIndex < SDL_RAT_getMaxRegions(pRatRegs)))
    {
        SDL_ratTranslationCfgInfo   xlatCfg;
        /*---------------------------------------------------------------------
         * Validate translation configuration parameters
         *-------------------------------------------------------------------*/
        (void) SDL_RAT_getRegionTranslationConfig(pRatRegs, regionIndex, &xlatCfg);
        
        sdlResult = SDL_RAT_validateTranslationConfig(pRatRegs, regionIndex, &xlatCfg);
			
        if( sdlResult == SDL_PASS)
        {
            sdlResult = SDL_RAT_setRegionTranslationEnable(pRatRegs, regionIndex, (bool)true);
        }
    }
	else
	{
		sdlResult = SDL_EBADARGS;
	}
    return sdlResult;
}



/********************************************************************************************************
*   API for disabling RAT
*********************************************************************************************************/
/**
 *  Design: PROC_SDL-2239
 */
int32_t SDL_RAT_disableRegionTranslation(SDL_ratRegs *pRatRegs, uint32_t regionIndex)
{
    int32_t sdlResult = SDL_EFAIL;

    if ((pRatRegs != NULL) && (regionIndex < SDL_RAT_getMaxRegions(pRatRegs)))
    {
        sdlResult = SDL_RAT_setRegionTranslationEnable(pRatRegs, regionIndex, (bool)false);
    }
	else
	{
		sdlResult = SDL_EBADARGS;
	}
    return sdlResult;
}



/********************************************************************************************************
*   API for Enabling the Interrupt
*********************************************************************************************************/
/**
 *  Design: PROC_SDL-2240
 */
int32_t SDL_RAT_enableIntr(SDL_ratRegs *pRatRegs, SDL_RAT_intrSrcTypes intrSrc)
{
    int32_t sdlResult = SDL_EFAIL;

    if ((intrSrc == SDL_RAT_INTR_SRC_EXCEPTION)    &&
        (SDL_RAT_enableExceptionLog(pRatRegs) == SDL_PASS))
    {
        SDL_RAT_setIntEnable(pRatRegs, intrSrc, (bool)true);
        sdlResult = SDL_PASS;
    }
	else
	{
		sdlResult = SDL_EBADARGS;
	}

    return sdlResult;
}



/********************************************************************************************************
*   API for setting the destination ID for error messages.
*********************************************************************************************************/
/**
 *  Design: PROC_SDL-2242
 */
int32_t SDL_RAT_setErrMsgDestId(SDL_ratRegs *pRatRegs, uint32_t destId)
{
    int32_t sdlResult = SDL_EFAIL;

    if ((pRatRegs != NULL) && (destId <= SDL_RAT_DESTINATION_ID_DEST_ID_MAX))
    {
        SDL_REG32_WR(&pRatRegs->DESTINATION_ID, SDL_FMK(RAT_DESTINATION_ID_DEST_ID, destId));
        sdlResult = SDL_PASS;
    }
	else
	{
		sdlResult = SDL_EBADARGS;
	}
    return sdlResult;
}



/********************************************************************************************************
*    API to get the destination ID for error messages.
*********************************************************************************************************/
/**
 *  Design: PROC_SDL-2243
 */
int32_t SDL_RAT_getErrMsgDestId(const SDL_ratRegs *pRatRegs, uint32_t *pDestId)
{
    int32_t sdlResult = SDL_EFAIL;

    if ((pRatRegs != NULL) && (pDestId != NULL))
    {
        *pDestId = SDL_REG32_FEXT(&pRatRegs->DESTINATION_ID, RAT_DESTINATION_ID_DEST_ID);
        sdlResult = SDL_PASS;
    }
	else
	{
		sdlResult = SDL_EBADARGS;
	}
    return sdlResult;
}



/********************************************************************************************************
*   API for enables the exception logging capability
*********************************************************************************************************/
/**
 *  Design: PROC_SDL-2244
 */
int32_t SDL_RAT_enableExceptionLog(SDL_ratRegs *pRatRegs)
{
    int32_t sdlResult = SDL_EFAIL;

    if (pRatRegs != NULL)
    {
        SDL_RAT_setExceptionLoggingEnable(pRatRegs, (bool)true);
        SDL_RAT_setExceptionLoggingIntEnable(pRatRegs, (bool)true);
        sdlResult = SDL_PASS;
    }
	else
	{
		sdlResult = SDL_EBADARGS;
	}
    return sdlResult;
}



/********************************************************************************************************
*   API for disabling the exception logging capability
*********************************************************************************************************/
/**
 *  Design: PROC_SDL-2245
 */
int32_t SDL_RAT_disableExceptionLog(SDL_ratRegs *pRatRegs)
{
    int32_t sdlResult = SDL_EFAIL;

    if (pRatRegs != NULL)
    {
        SDL_RAT_setExceptionLoggingEnable(pRatRegs, (bool)false);
        SDL_RAT_setExceptionLoggingIntEnable(pRatRegs, (bool)false);
        sdlResult = SDL_PASS;
    }
	else
	{
		sdlResult = SDL_EBADARGS;
	}
    return sdlResult;
}



/********************************************************************************************************
*   API for returning the pending interrupt status of the specified interrupt source
*********************************************************************************************************/
/**
 *  Design: PROC_SDL-2246
 */
int32_t SDL_RAT_isIntrPending(const SDL_ratRegs *pRatRegs, SDL_RAT_intrSrcTypes intrSrc)
{
    int32_t sdlResult = SDL_EFAIL;

    if ((pRatRegs != NULL) && (intrSrc == SDL_RAT_INTR_SRC_EXCEPTION))
    {
        sdlResult = (SDL_REG32_FEXT( &pRatRegs->EXCEPTION_PEND_SET, RAT_EXCEPTION_PEND_SET_PEND_SET ) == (uint32_t)1U)? SDL_PASS : SDL_EFAIL;
    }
	else
	{
		sdlResult = SDL_EBADARGS;
	}
    return sdlResult;
}



/********************************************************************************************************
*   API used to forceably set the pending status of the specified interrupt source.
*********************************************************************************************************/
/**
 *  Design: PROC_SDL-2247
 */
int32_t SDL_RAT_setIntrPending(SDL_ratRegs *pRatRegs, SDL_RAT_intrSrcTypes intrSrc)
{
    int32_t sdlResult = SDL_EFAIL;

    if ((pRatRegs != NULL) && (intrSrc == SDL_RAT_INTR_SRC_EXCEPTION))
    {
        SDL_REG32_WR( &pRatRegs->EXCEPTION_PEND_SET, SDL_FMK( RAT_EXCEPTION_PEND_SET_PEND_SET, 1U ));
        sdlResult = SDL_PASS;
    }
	else
	{
		sdlResult = SDL_EBADARGS;
	}
    return sdlResult;
}



/********************************************************************************************************
*   API used to clear the pending status of the specified interrupt source.
*********************************************************************************************************/
/**
 *  Design: PROC_SDL-2248
 */
int32_t SDL_RAT_clrIntrPending(SDL_ratRegs *pRatRegs, SDL_RAT_intrSrcTypes intrSrc)
{
    int32_t sdlResult = SDL_EFAIL;

    if ((pRatRegs != NULL) && (intrSrc == SDL_RAT_INTR_SRC_EXCEPTION))
    {
        SDL_REG32_WR( &pRatRegs->EXCEPTION_PEND_CLEAR, SDL_FMK( RAT_EXCEPTION_PEND_CLEAR_PEND_CLR, 1U ) );
        sdlResult = SDL_PASS;
    }
	else
	{
		sdlResult = SDL_EBADARGS;
	}
    return sdlResult;
}



/********************************************************************************************************
*   API used to acknowledge the specified interrupt source.
*********************************************************************************************************/
/**
 *  Design: PROC_SDL-2249
 */
int32_t SDL_RAT_ackIntr(SDL_ratRegs *pRatRegs, SDL_RAT_intrSrcTypes intrSrc)
{
    int32_t sdlResult = SDL_EFAIL;

    if ((pRatRegs != NULL) && (intrSrc == SDL_RAT_INTR_SRC_EXCEPTION))
    {
        SDL_REG32_WR( &pRatRegs->EOI_REG, 0 );
        sdlResult = SDL_PASS;
    }
	else
	{
		sdlResult = SDL_EBADARGS;
	}
    return sdlResult;
}



/********************************************************************************************************
*   API to disable the specified interrupt source
*********************************************************************************************************/
/**
 *  Design: PROC_SDL-2250
 */
int32_t SDL_RAT_disableIntr(SDL_ratRegs *pRatRegs, SDL_RAT_intrSrcTypes intrSrc)
{
    int32_t sdlResult = SDL_EFAIL;

    if ((pRatRegs != NULL) && (intrSrc == SDL_RAT_INTR_SRC_EXCEPTION))
    {
        SDL_RAT_setIntEnable(pRatRegs, intrSrc, (bool)false);
        sdlResult = SDL_PASS;
    }
	else
	{
		sdlResult = SDL_EBADARGS;
	}
    return sdlResult;
}



/********************************************************************************************************
*   API used to get information for the last boundary crossing
*   transaction exception captured
*********************************************************************************************************/
/**
 *  Design: PROC_SDL-2251
 */
int32_t SDL_RAT_getExceptionInfo(SDL_ratRegs *pRatRegs, SDL_ratExceptionInfo *pExceptionInfo)
{
    uint32_t val, val2;
    int32_t sdlResult = SDL_EFAIL;

    if ((pRatRegs != NULL) && (pExceptionInfo != NULL))
    {
        val = SDL_REG32_RD(&pRatRegs->EXCEPTION_LOGGING_HEADER0);
        pExceptionInfo->type        = SDL_FEXT(val, RAT_EXCEPTION_LOGGING_HEADER0_TYPE_F);
        pExceptionInfo->destId      = SDL_FEXT(val, RAT_EXCEPTION_LOGGING_HEADER0_DEST_ID);
        pExceptionInfo->srcId       = SDL_FEXT(val, RAT_EXCEPTION_LOGGING_HEADER0_SRC_ID);

        val = SDL_REG32_RD(&pRatRegs->EXCEPTION_LOGGING_HEADER1);
        pExceptionInfo->code        = SDL_FEXT(val, RAT_EXCEPTION_LOGGING_HEADER1_CODE);
        pExceptionInfo->group       = SDL_FEXT(val, RAT_EXCEPTION_LOGGING_HEADER1_GROUP);

        val = SDL_REG32_FEXT(&pRatRegs->EXCEPTION_LOGGING_DATA0, RAT_EXCEPTION_LOGGING_DATA0_ADDR_L);
        val2= SDL_REG32_FEXT(&pRatRegs->EXCEPTION_LOGGING_DATA1, RAT_EXCEPTION_LOGGING_DATA1_ADDR_H);
        pExceptionInfo->address     = (((uint64_t)val2) << 32) | (uint64_t)val;

        val = SDL_REG32_RD(&pRatRegs->EXCEPTION_LOGGING_DATA2);
        pExceptionInfo->routeId     = SDL_FEXT(val, RAT_EXCEPTION_LOGGING_DATA2_ROUTEID);
        pExceptionInfo->privId      = SDL_FEXT(val, RAT_EXCEPTION_LOGGING_DATA2_PRIV_ID);

        pExceptionInfo->bWrite      = (SDL_FEXT(val, RAT_EXCEPTION_LOGGING_DATA2_WRITE) == 0U) ?        FALSE : TRUE;
        pExceptionInfo->bRead       = (SDL_FEXT(val, RAT_EXCEPTION_LOGGING_DATA2_READ) == 0U) ?         FALSE : TRUE;
        pExceptionInfo->bDebug      = (SDL_FEXT(val, RAT_EXCEPTION_LOGGING_DATA2_DEBUG) == 0U) ?        FALSE : TRUE;
        pExceptionInfo->bCacheable  = (SDL_FEXT(val, RAT_EXCEPTION_LOGGING_DATA2_CACHEABLE) == 0U) ?    FALSE : TRUE;
        pExceptionInfo->bPriv       = (SDL_FEXT(val, RAT_EXCEPTION_LOGGING_DATA2_PRIV) == 0U) ?         FALSE : TRUE;
        pExceptionInfo->bSecure     = (SDL_FEXT(val, RAT_EXCEPTION_LOGGING_DATA2_SECURE) == 0U) ?       FALSE : TRUE;

        val = SDL_REG32_RD(&pRatRegs->EXCEPTION_LOGGING_DATA3);
        pExceptionInfo->byteCnt     = SDL_FEXT(val, RAT_EXCEPTION_LOGGING_DATA3_BYTECNT);

        /* Clear the interrupt status bit just to make sure the log is cleared for the next exception */
        sdlResult = SDL_RAT_clrIntrPending(pRatRegs, SDL_RAT_INTR_SRC_EXCEPTION);
    }
	else
	{
		sdlResult = SDL_EBADARGS;
	}
    return sdlResult;
}
