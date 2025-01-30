/**
 * @file  sdl_vim.c
 *
 * @brief
 *  C implementation file for the VIM module SDL-FL.
 *
 *  Contains the different control command and status query functions definitions
 *
 *  \par
 *  ============================================================================
 *  @n   (C) Copyright 2022, Texas Instruments, Inc.
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
#include <stdint.h>
#include "sdl_vim.h"
#include "sdl_types.h"
#include "sdlr.h"

#define DSB_ASM         asm (" dsb")
#define ISB_ASM         asm (" isb")
#define         SDL_MCU_ARMSS_VIM_NULL_ADDR           ((void *) 0 )

/**
 * Design: PROC_SDL-2123
 */
uint32_t SDL_VIM_getRevision( SDL_vimRegs *pRegs )
{
    void       *pChkRegs = (void *) pRegs;
    uint32_t    regVal = ((uint32_t) (0u));

    if (pChkRegs != SDL_MCU_ARMSS_VIM_NULL_ADDR)
    {
		regVal   = SDL_REG32_RD (&pRegs->PID );
    }
    return (regVal);
}

/**
 * Design: PROC_SDL-2124
 */
uint32_t SDL_VIM_getNumIntrs( SDL_vimRegs *pRegs )
{
    void       *pChkRegs = (void *) pRegs;
    uint32_t    regVal = ((uint32_t) (0u));

    if (pChkRegs != SDL_MCU_ARMSS_VIM_NULL_ADDR)
    {
        regVal =  SDL_REG32_FEXT( &pRegs->INFO, VIM_INFO_INTERRUPTS );
    }
    return (regVal);
}

/**
 * Design: PROC_SDL-2119
 */
int32_t SDL_VIM_cfgIntr( SDL_vimRegs *pRegs, uint32_t intrNum, uint32_t pri, SDL_VimIntrMap intrMap, SDL_VimIntrType intrType, uint32_t vecAddr )
{
    int32_t     retVal = SDL_EFAIL;
    uint32_t    bitNum, groupNum, regVal, regMask;
    void       *pChkRegs = (void *) pRegs;
    uint32_t    maxIntrs = 0, num_groups = 0;

    if (pChkRegs != SDL_MCU_ARMSS_VIM_NULL_ADDR)
    {
        maxIntrs   = pRegs->INFO;
        num_groups = maxIntrs / SDL_VIM_NUM_INTRS_PER_GROUP;
		groupNum = intrNum / SDL_VIM_NUM_INTRS_PER_GROUP;
		/* Condition "(vecAddr - 1U)" is need for THUMB Mode as TI ARM CLANG marks LSB as '1' */
		if( (intrNum  < maxIntrs)                             &&
			(pri <= SDL_VIM_PRI_INT_VAL_MAX)                  &&
			(intrMap <= SDL_VIM_INTR_MAP_FIQ)                 &&
			(intrType <= SDL_VIM_INTR_TYPE_PULSE)             &&
			(((vecAddr & SDL_VIM_VEC_INT_VAL_MASK) == vecAddr) ||
				(((vecAddr - (uint32_t)1U) & SDL_VIM_VEC_INT_VAL_MASK) == (vecAddr - (uint32_t)1U))) )
		{
			bitNum = intrNum & (SDL_VIM_NUM_INTRS_PER_GROUP-1U);
	
			/* Configure INTMAP */
			regMask = (uint32_t)(1U) << bitNum;
			regVal = SDL_REG32_RD( &pRegs->GRP[groupNum].INTMAP );
			regVal &= ~regMask;
			regVal |= intrMap;
			SDL_REG32_WR( &pRegs->GRP[groupNum].INTMAP, regVal );
	
			/* Configure INTTYPE */
			regMask = (uint32_t)(1U) << bitNum;
			regVal = SDL_REG32_RD( &pRegs->GRP[groupNum].INTTYPE );
			regVal &= ~regMask;
			regVal |= intrType << bitNum;
			SDL_REG32_WR( &pRegs->GRP[groupNum].INTTYPE, regVal );
	
			/* Configure PRI */
			SDL_REG32_WR( &pRegs->PRI[intrNum].INT, SDL_FMK( VIM_PRI_INT_VAL, pri ) );
	
			/* Configure VEC */
			SDL_REG32_WR( &pRegs->VEC[intrNum].INT, vecAddr );
				retVal = SDL_PASS;
		}
    }

    return retVal;
}

/**
 * Design: PROC_SDL-2120
 */
int32_t SDL_VIM_verifyCfgIntr( SDL_vimRegs *pRegs, uint32_t intrNum, uint32_t pri, SDL_VimIntrMap intrMap, SDL_VimIntrType intrType, uint32_t vecAddr )
{
    int32_t  retVal = SDL_EFAIL;
    uint32_t bitNum, groupNum;
    uint32_t intrMapVal, intrTypeVal, priVal, vecVal;
    void       *pChkRegs = (void *) pRegs;
    uint32_t    maxIntrs = 0, num_groups = 0;

    if (pChkRegs != SDL_MCU_ARMSS_VIM_NULL_ADDR)
    {
        maxIntrs   = pRegs->INFO;
        num_groups = maxIntrs / SDL_VIM_NUM_INTRS_PER_GROUP;
        groupNum = intrNum / SDL_VIM_NUM_INTRS_PER_GROUP;
    
		/* Condition "(vecAddr - 1U)" is need for THUMB Mode as TI ARM CLANG marks LSB as '1' */
		if( (intrNum < maxIntrs)                              &&
			(pri <= SDL_VIM_PRI_INT_VAL_MAX)                  &&
			(intrMap <= SDL_VIM_INTR_MAP_FIQ)                 &&
			(intrType <= SDL_VIM_INTR_TYPE_PULSE)             &&
			(((vecAddr & SDL_VIM_VEC_INT_VAL_MASK) == vecAddr) ||
				(((vecAddr - (uint32_t)1U) & SDL_VIM_VEC_INT_VAL_MASK) == (vecAddr - (uint32_t)1U))))
		{
			bitNum = intrNum & (SDL_VIM_NUM_INTRS_PER_GROUP-1U);
	
			/* Read INTMAP */
			intrMapVal  = SDL_REG32_RD( &pRegs->GRP[groupNum].INTMAP );
			/* Get the interrupt map value */
			intrMapVal  = intrMapVal >> bitNum;
			intrMapVal &= (uint32_t)(0x1U);
	
			/* Read INTTYPE */
			intrTypeVal  = SDL_REG32_RD( &pRegs->GRP[groupNum].INTTYPE );
			/* Get the interrupt type value */
			intrTypeVal  = intrTypeVal >> bitNum;
			intrTypeVal &= (uint32_t)(0x1U);
	
			/* Read PRI */
			priVal = SDL_REG32_RD( &pRegs->PRI[intrNum].INT);
	
			/* Read VEC */
			vecVal = SDL_REG32_RD( &pRegs->VEC[intrNum].INT);
				retVal = SDL_PASS;
		}
    }

    if (retVal != SDL_EFAIL)
    {
        /* verify if parameter matches */
        if ((intrMapVal != intrMap) || 
			(intrTypeVal != (uint32_t)intrType) ||
			(priVal != pri) ||
			(vecVal != vecAddr))
        {
            retVal = SDL_EFAIL;
        }
    }

    return retVal;
}

/**
 * Design: PROC_SDL-2126
 */
void SDL_VIM_setDedVectorAddr( SDL_vimRegs *pRegs, uint32_t dedVectorAddr )
{
    void       *pChkRegs = (void *) pRegs;

    if (pChkRegs != SDL_MCU_ARMSS_VIM_NULL_ADDR)
    {
        SDL_REG32_WR( &pRegs->DEDVEC, (dedVectorAddr & SDL_VIM_DEDVEC_ADDR_MASK) );
    }
}

/**
 * Design: PROC_SDL-2127 
 */
int32_t SDL_VIM_getDedVectorAddr( SDL_vimRegs *pRegs, uint32_t *pDedVectorAddr )
{
    int32_t     retVal = SDL_PASS;
    uint32_t    dedVecAddr;
    void       *pChkDedVecAddr = (void *) pDedVectorAddr;
    void       *pChkRegs = (void *) pRegs;
    if ( (pChkRegs == SDL_MCU_ARMSS_VIM_NULL_ADDR) ||
         (pChkDedVecAddr == SDL_MCU_ARMSS_VIM_NULL_ADDR))
    {
        retVal = SDL_EFAIL;
    }

    if (retVal == SDL_PASS)
    {
        dedVecAddr = SDL_REG32_RD( &pRegs->DEDVEC);
       *pDedVectorAddr = dedVecAddr;
    }
    return (retVal);
}

/**
 * Design: PROC_SDL-2128
 */
int32_t SDL_VIM_getActivePendingIntr( SDL_vimRegs *pRegs, SDL_VimIntrMap intrMap, uint32_t *pIntrNum, uint32_t *pPri )
{
    int32_t  retVal = SDL_PASS;
    uint32_t regVal;
    void       *pChkIntNum = (void *) pIntrNum;
    void       *pChkRegs = (void *) pRegs;
    void       *pChkPri = (void *) pPri;

    if ( pChkRegs == SDL_MCU_ARMSS_VIM_NULL_ADDR )
    {
        retVal = SDL_EFAIL;
    }

    if (retVal == SDL_EFAIL)
    {
       /* No action - API action fails */
    }
    else if( intrMap == SDL_VIM_INTR_MAP_IRQ )
    {
        regVal = SDL_REG32_RD( &pRegs->ACTIRQ );
    }
    else if( intrMap == SDL_VIM_INTR_MAP_FIQ )
    {
        regVal = SDL_REG32_RD( &pRegs->ACTFIQ );
    }
    else
    {
        retVal = SDL_EFAIL;
    }

    if( (retVal == SDL_PASS) && ( SDL_FEXT( regVal, VIM_ACTIRQ_VALID ) == SDL_VIM_ACTIRQ_VALID_VAL_TRUE ))
    {
        /*---------------------------------------------------------------------
         * Note that the SDL FIELD macros are the same for the ACTIRQ and
         * ACTFIQ registers, so we will just use the ACTIRQ ones
         *-------------------------------------------------------------------*/
             if ( (pChkIntNum != SDL_MCU_ARMSS_VIM_NULL_ADDR) )
             {
                 *pIntrNum     = SDL_FEXT( regVal, VIM_ACTIRQ_NUM );
             }
             if ( (pChkPri != SDL_MCU_ARMSS_VIM_NULL_ADDR) )
             {
                 *pPri         = SDL_FEXT( regVal, VIM_ACTIRQ_PRI );
             }
        }
        else
        {
            retVal = SDL_EFAIL;        /* No interrupt is pending */
    }
    return retVal;
}

/**
 * Design: PROC_SDL-2122 
 */
int32_t SDL_VIM_setIntrEnable( SDL_vimRegs *pRegs, uint32_t intrNum, bool bEnable )
{
    int32_t  retVal = SDL_EFAIL;
    uint32_t bitNum, groupNum, mask;
    void       *pChkRegs = (void *) pRegs;
    uint32_t    maxIntrs = 0, num_groups = 0;

    if (pChkRegs != SDL_MCU_ARMSS_VIM_NULL_ADDR)
    {
        maxIntrs   = pRegs->INFO;
        num_groups = maxIntrs / SDL_VIM_NUM_INTRS_PER_GROUP;
		groupNum   = intrNum / SDL_VIM_NUM_INTRS_PER_GROUP;

		if(groupNum < num_groups)
    {
        bitNum = intrNum & (SDL_VIM_NUM_INTRS_PER_GROUP-1U);
        mask   = (((uint32_t)(1U)) << bitNum);
        if( bEnable == (bool) true)
        {
            SDL_REG32_WR(&pRegs->GRP[groupNum].INTR_EN_SET, mask );
        }
        else
        {
            SDL_REG32_WR( &pRegs->GRP[groupNum].INTR_EN_CLR, mask );
        }
		    retVal = SDL_PASS;
    }
    else
    {
        retVal = SDL_EFAIL;
    }
    }
    return retVal;
}

/**
 * Design: PROC_SDL-2125 
 */
int32_t SDL_VIM_getIntrType( SDL_vimRegs *pRegs, uint32_t intrNum)
{
    int32_t  retVal = SDL_EFAIL; 
    uint32_t bitNum, groupNum, regVal;
    void       *pChkRegs = (void *) pRegs;
    uint32_t    maxIntrs = 0, num_groups = 0;

    if (pChkRegs != SDL_MCU_ARMSS_VIM_NULL_ADDR)
    {
        maxIntrs   = pRegs->INFO;
        num_groups = maxIntrs / SDL_VIM_NUM_INTRS_PER_GROUP;
		groupNum = intrNum / SDL_VIM_NUM_INTRS_PER_GROUP;

		if(groupNum < num_groups)
		{
			bitNum = intrNum & (SDL_VIM_NUM_INTRS_PER_GROUP-1U);
			regVal = SDL_REG32_RD( &pRegs->GRP[groupNum].INTTYPE);
			regVal  = (((regVal) >> bitNum) & 1U);
			if (regVal == (uint32_t)1u)
			{
            retVal = ((int32_t)(1));
			}
            else{
				retVal = (0);
			}
		}
    }
    return retVal;
}

/**
 * Design: PROC_SDL-2129 
 */
uint32_t SDL_VIM_getIrqVectorAddress( SDL_vimRegs *pRegs )
{
    void       *pChkRegs = (void *) pRegs;
    uint32_t    regVal = ((uint32_t) (0u));

    if (pChkRegs != SDL_MCU_ARMSS_VIM_NULL_ADDR)
    {
        regVal = (uint32_t)SDL_REG32_RD( &pRegs->IRQVEC );
    }
    return (regVal);
}

/**
 * Design: PROC_SDL-2130 
 */
uint32_t SDL_VIM_getFiqVectorAddress( SDL_vimRegs *pRegs )
{
    void       *pChkRegs = (void *) pRegs;
    uint32_t    regVal = ((uint32_t) (0u));

    if (pChkRegs != SDL_MCU_ARMSS_VIM_NULL_ADDR)
    {
        regVal = (uint32_t)SDL_REG32_RD( &pRegs->FIQVEC );
    }
    return (regVal);
}

/**
 * Design: PROC_SDL-2131
 */
uint32_t SDL_VIM_getGroupsIntrPending( SDL_vimRegs *pRegs, SDL_VimIntrMap intrMap )
{
    uint32_t    groupsStatus = ((uint32_t) (0u));
    void       *pChkRegs = (void *) pRegs;

    if (pChkRegs != SDL_MCU_ARMSS_VIM_NULL_ADDR)
    {
        if( intrMap == SDL_VIM_INTR_MAP_IRQ )
        {
            groupsStatus = SDL_REG32_RD( &pRegs->IRQGSTS );
        }
        else
        {
            groupsStatus = SDL_REG32_RD( &pRegs->FIQGSTS );
        }
    }
    return groupsStatus;
}

/**
 * Design: PROC_SDL-2132 
 */
uint32_t SDL_VIM_getGroupIntrPending(
                                                  SDL_vimRegs   *pRegs,
                                                  SDL_VimIntrMap intrMap,
                                                  uint32_t       groupNum,
                                                  bool           bMasked
)
{
    uint32_t    groupStatus = ((uint32_t) (0u));
    void       *pChkRegs = (void *) pRegs;
    uint32_t    maxIntrs = 0, num_groups = 0;

    if (pChkRegs != SDL_MCU_ARMSS_VIM_NULL_ADDR)
    {
        maxIntrs   = pRegs->INFO;
        num_groups = maxIntrs / SDL_VIM_NUM_INTRS_PER_GROUP;
    }

    if ( (pChkRegs != SDL_MCU_ARMSS_VIM_NULL_ADDR) &&
         (groupNum < num_groups) )
    {
        if( bMasked == (bool) true)
        {
            if( intrMap == SDL_VIM_INTR_MAP_IRQ )
            {
                groupStatus = SDL_REG32_RD( &pRegs->GRP[groupNum].IRQSTS );
            }
            else if( intrMap == SDL_VIM_INTR_MAP_FIQ )
            {
                groupStatus = SDL_REG32_RD( &pRegs->GRP[groupNum].FIQSTS );
            }
            else
            {
                groupStatus = SDL_REG32_RD( &pRegs->GRP[groupNum].STS );
            }
        }
        else
        {
            groupStatus = SDL_REG32_RD( &pRegs->GRP[groupNum].RAW );
        }
    }
    return groupStatus;
}

/**
 * Design: PROC_SDL-2133
 */
int32_t SDL_VIM_setIntrPending( SDL_vimRegs *pRegs, uint32_t intrNum )
{
    int32_t  retVal = SDL_PASS;
    uint32_t bitNum, groupNum, mask;
    void       *pChkRegs = (void *) pRegs;
    uint32_t    maxIntrs = 0, num_groups = 0;

    if (pChkRegs != SDL_MCU_ARMSS_VIM_NULL_ADDR)
    {
        maxIntrs   = pRegs->INFO;
        num_groups = maxIntrs / SDL_VIM_NUM_INTRS_PER_GROUP;
    }

    groupNum = intrNum / SDL_VIM_NUM_INTRS_PER_GROUP;

    if ( (pChkRegs != SDL_MCU_ARMSS_VIM_NULL_ADDR) &&
         (groupNum < num_groups) )
    {
        bitNum = intrNum & (SDL_VIM_NUM_INTRS_PER_GROUP-1U);
        mask   = (((uint32_t)(1U)) << bitNum);
        SDL_REG32_WR( &pRegs->GRP[groupNum].RAW, mask );
        /*
         * Add delay to insure posted interrupt are triggered before function
         * returns.
         */
        DSB_ASM;
        ISB_ASM;
    }
    else
    {
        retVal = SDL_EFAIL;
    }
    return retVal;
}

/**
 * Design: PROC_SDL-2134
 */
int32_t SDL_VIM_clrIntrPending( SDL_vimRegs *pRegs, uint32_t intrNum )
{
    int32_t  retVal = SDL_PASS;
    uint32_t bitNum, groupNum, mask;
    void       *pChkRegs = (void *) pRegs;
    uint32_t    maxIntrs = 0, num_groups = 0;

    if (pChkRegs != SDL_MCU_ARMSS_VIM_NULL_ADDR)
    {
        maxIntrs   = pRegs->INFO;
        num_groups = maxIntrs / SDL_VIM_NUM_INTRS_PER_GROUP;
    }

    groupNum = intrNum / SDL_VIM_NUM_INTRS_PER_GROUP;
    bitNum   = intrNum & (SDL_VIM_NUM_INTRS_PER_GROUP-1U);
    mask   = (((uint32_t)(1U)) << bitNum);

    if ( (pChkRegs != SDL_MCU_ARMSS_VIM_NULL_ADDR) &&
         (groupNum < num_groups) )
    {
        SDL_REG32_WR( &pRegs->GRP[groupNum].STS, mask );
    }
    else
    {
        retVal = SDL_EFAIL;
    }
    return retVal;
}

/**
 * Design: PROC_SDL-2135 
 */
int32_t SDL_VIM_ackIntr( SDL_vimRegs *pRegs, SDL_VimIntrMap intrMap )
{
    int32_t  retVal = SDL_PASS;
    void    *pChkRegs = (void *) pRegs;

    if (pChkRegs == SDL_MCU_ARMSS_VIM_NULL_ADDR)
    {
        /* No actions - API fails functionality */
        retVal = SDL_EFAIL;
    }
    else if( intrMap == SDL_VIM_INTR_MAP_IRQ )
    {
        SDL_REG32_WR( &pRegs->IRQVEC, ((uint32_t) (0u)) );
    }
    else if( intrMap == SDL_VIM_INTR_MAP_FIQ )
    {
        SDL_REG32_WR( &pRegs->FIQVEC, ((uint32_t) (0u)) );
    }
    else
    {
        retVal = SDL_EFAIL;
    }
    return retVal;
}

/**
 * Design: PROC_SDL-2121
 */
int32_t SDL_VIM_getStaticRegs( SDL_vimRegs *pRegs, SDL_vimStaticRegs *pStaticRegs)
{
    int32_t  retVal = SDL_PASS;
    uint32_t i, maxIntrs, num_groups;
    void    *pChkRegs = (void *) pRegs;

    if ( (pChkRegs == SDL_MCU_ARMSS_VIM_NULL_ADDR) ||
         (pStaticRegs == SDL_MCU_ARMSS_VIM_NULL_ADDR) )
    {
        /* No actions - API fails to read back */
        retVal = SDL_EFAIL;
    }

    if (retVal == SDL_PASS)
    {
        pStaticRegs->PID  = SDL_REG32_RD(&pRegs->PID);
        pStaticRegs->INFO = SDL_REG32_RD(&pRegs->INFO);

        maxIntrs = pRegs->INFO;
        num_groups = maxIntrs / SDL_VIM_NUM_INTRS_PER_GROUP;

        for (i = ((uint32_t) (0u)); i < num_groups; i++)
        {
            pStaticRegs->GRP[i].INTMAP      = SDL_REG32_RD(&pRegs->GRP[i].INTMAP);
            pStaticRegs->GRP[i].INTR_EN_CLR = SDL_REG32_RD(&pRegs->GRP[i].INTR_EN_CLR);
            pStaticRegs->GRP[i].INTR_EN_SET = SDL_REG32_RD(&pRegs->GRP[i].INTR_EN_SET);
            pStaticRegs->GRP[i].INTTYPE     = SDL_REG32_RD(&pRegs->GRP[i].INTTYPE);
        }

        for (i = ((uint32_t) (0u)); i < maxIntrs; i++)
        {
            pStaticRegs->PRI[i].INT         = SDL_REG32_RD(&pRegs->PRI[i].INT);
            pStaticRegs->VEC[i].INT         = SDL_REG32_RD(&pRegs->VEC[i].INT);
        }
        /* Remaining values zero it */
        for (i = num_groups; i < SDL_VIM_MAX_INTR_GROUPS; i++)
        {
            pStaticRegs->GRP[i].INTMAP      = (uint32_t)0u;
            pStaticRegs->GRP[i].INTR_EN_CLR = (uint32_t)0u;
            pStaticRegs->GRP[i].INTR_EN_SET = (uint32_t)0u;
            pStaticRegs->GRP[i].INTTYPE     = (uint32_t)0u;
        }
        for (i = maxIntrs; i < (SDL_VIM_MAX_INTR_GROUPS * SDL_VIM_NUM_INTRS_PER_GROUP); i++)
        {
            pStaticRegs->PRI[i].INT         = (uint32_t)0u;
            pStaticRegs->VEC[i].INT         = (uint32_t)0u;
        }

    }

    return (retVal);
}

/* nothing past this point */
