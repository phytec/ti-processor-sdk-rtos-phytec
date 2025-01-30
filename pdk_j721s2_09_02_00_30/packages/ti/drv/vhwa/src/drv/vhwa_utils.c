/**
 *   Copyright (c) Texas Instruments Incorporated 2018
 *   All rights reserved.
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
 *  \file vhwa_utils.c
 *
 *  \brief Utility APIs Implementation
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <ti/drv/vhwa/src/drv/vhwa_utils.h>
#include <ti/csl/csl_types.h>
#include <ti/drv/vhwa/include/vhwa_common.h>
#include <ti/drv/vhwa/src/csl/include/csl_lse.h>


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */



/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
uint64_t gDmpacSl2Addr = DMPAC_SL2_START_ADDRESS;
uint64_t gVpacSl2Addr = VPAC_SL2_START_ADDRESS;
uint32_t gDmpacSl2Size = (uint32_t)VPAC_DMPAC_SL2_SIZE;
uint32_t gVpacSl2Size = (uint32_t)VPAC_DMPAC_SL2_SIZE;
uint32_t gVpacDrvierOpen = 0;
uint32_t gDmpacDrvierOpen = 0;

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

uint32_t Vhwa_calcSl2Pitch(uint32_t imgPitch)
{
    uint32_t sl2Pitch;

    /* First convert Img Pitch to Sl2 Aligned pitch */
    sl2Pitch = CSL_lseMakePitchAligned(imgPitch);

    return (sl2Pitch);
}

uint32_t Vhwa_ceil(uint32_t num, uint32_t den)
{
     return ((0u == (num%den)) ? (num/den) : ((num/den) + (uint32_t)1u));
}

uint64_t Vhwa_allocateSl2(uint32_t allocSize, uint32_t sl2Inst)
{
    uint64_t sl2Addr = (uint64_t)NULL;

    if(sl2Inst == (uint32_t)VHWA_SL2_INST_DMPAC)
    {
        if(gDmpacSl2Size >= allocSize)
        {
            sl2Addr = gDmpacSl2Addr;
            gDmpacSl2Addr += allocSize;
            gDmpacSl2Size -= allocSize;
            gDmpacDrvierOpen++;
        }
    }
    else if(sl2Inst == (uint32_t)VHWA_SL2_INST_VPAC)
    {
        if(gVpacSl2Size >= allocSize)
        {
            sl2Addr = gVpacSl2Addr;
            gVpacSl2Addr += allocSize;
            gVpacSl2Size -= allocSize;
            gVpacDrvierOpen++;
        }
    }
    else
    {
        /* Invalid SL2_INST */
    }

    return sl2Addr;
}

void Vhwa_FreeSl2(uint64_t addr, uint32_t sl2Inst)
{
    if(sl2Inst == (uint32_t)VHWA_SL2_INST_DMPAC)
    {
        gDmpacDrvierOpen--;
        if(0U == gDmpacDrvierOpen)
        {
            gDmpacSl2Addr = DMPAC_SL2_START_ADDRESS;
            gDmpacSl2Size = (uint32_t)VPAC_DMPAC_SL2_SIZE;
        }
    }
    else if(sl2Inst == (uint32_t)VHWA_SL2_INST_VPAC)
    {
        gVpacDrvierOpen--;
        if(0U == gVpacDrvierOpen)
        {
            gVpacSl2Addr = VPAC_SL2_START_ADDRESS;
            gVpacSl2Size = (uint32_t)VPAC_DMPAC_SL2_SIZE;
        }
    }
    else
    {
        /* Invalid SL2_INST */
    }
}

uint32_t Vhwa_calcHorzSizeInBytes(uint32_t width, uint32_t ccsf)
{
    uint32_t sizeInBytes;

    switch ((Fvid2_ColorCompStorageFmt)ccsf)
    {
        default:
            sizeInBytes = width;
            break;
        case FVID2_CCSF_BITS8_PACKED:
            sizeInBytes = width;
            break;
        case FVID2_CCSF_BITS12_PACKED:
            sizeInBytes = (width * 3U / 2U);
            break;
        case FVID2_CCSF_BITS8_UNPACKED16_MSB_ALIGNED:
        case FVID2_CCSF_BITS8_UNPACKED16:
            sizeInBytes = width * 2U;
            break;
        case FVID2_CCSF_BITS12_UNPACKED16_MSB_ALIGNED:
        case FVID2_CCSF_BITS12_UNPACKED16:
            sizeInBytes = width * 2U;
            break;
        case FVID2_CCSF_BITS16_PACKED:
            sizeInBytes = width * 2U;
            break;
    }

    return (sizeInBytes);
}

void Vhwa_enableHtsIntr(CSL_vpac_intd_cfgRegs *intdRegs, uint32_t irqNum,
    uint32_t htsPipelineNum)
{
    volatile uint32_t regVal;

    /* Check for Null pointer */
    GT_assert(VhwaMscTrace, (NULL != intdRegs));

    /* Enable HTS Interrupt in INTD Module */
    regVal = CSL_REG32_RD(&intdRegs->ENABLE_REG_LEVEL_VPAC_OUT[irqNum][3U]);
    regVal |= ((uint32_t)1u << htsPipelineNum);
    CSL_REG32_WR(&intdRegs->ENABLE_REG_LEVEL_VPAC_OUT[irqNum][3U], regVal);
}

void Vhwa_disableHtsIntr(CSL_vpac_intd_cfgRegs *intdRegs, uint32_t irqNum,
    uint32_t htsPipelineNum)
{
    volatile uint32_t regVal;

    /* Check for Null pointer */
    GT_assert(VhwaMscTrace, (NULL != intdRegs));

    /* Disable HTS Interrupt in INTD Module */
    regVal = CSL_REG32_RD(&intdRegs->ENABLE_CLR_REG_LEVEL_VPAC_OUT[irqNum][3U]);
    regVal |= ((uint32_t)1u << htsPipelineNum);
    CSL_REG32_WR(&intdRegs->ENABLE_CLR_REG_LEVEL_VPAC_OUT[irqNum][3U], regVal);

}

void Vhwa_clearHtsIntr(CSL_vpac_intd_cfgRegs *intdRegs, uint32_t irqNum,
    uint32_t htsPipelineNum)
{
    volatile uint32_t     regVal;

    /* Check for Null pointer */
    GT_assert(VhwaMscTrace, (NULL != intdRegs));

    regVal = CSL_REG32_RD(&intdRegs->STATUS_REG_LEVEL_VPAC_OUT[irqNum][3U]);
    if (0u != (regVal & ((uint32_t)1u << htsPipelineNum)))
    {
        regVal = (uint32_t)1u << htsPipelineNum;
        CSL_REG32_WR(&intdRegs->STATUS_CLR_REG_LEVEL_VPAC_OUT[irqNum][3U],
            regVal);
    }
}

void Vhwa_enableDmpacHtsIntr(CSL_dmpac_intd_cfgRegs *intdRegs, uint32_t irqNum,
    uint32_t htsPipelineNum)
{
    volatile uint32_t regVal;

    /* Enable HTS Interrupt in INTD Module */
    regVal = CSL_REG32_RD(&intdRegs->ENABLE_REG_LEVEL_DMPAC_OUT[irqNum][1U]);
    regVal |= ((uint32_t)1u << htsPipelineNum);
    CSL_REG32_WR(&intdRegs->ENABLE_REG_LEVEL_DMPAC_OUT[irqNum][1U], regVal);
}

void Vhwa_disableDmpacHtsIntr(CSL_dmpac_intd_cfgRegs *intdRegs, uint32_t irqNum,
    uint32_t htsPipelineNum)
{
    volatile uint32_t regVal;

    /* Disable HTS Interrupt in INTD Module */
    regVal = CSL_REG32_RD(&intdRegs->ENABLE_CLR_REG_LEVEL_DMPAC_OUT[irqNum][1U]);
    regVal |= ((uint32_t)1u << htsPipelineNum);
    CSL_REG32_WR(&intdRegs->ENABLE_CLR_REG_LEVEL_DMPAC_OUT[irqNum][1U], regVal);

}

void Vhwa_clearDmpacHtsIntr(CSL_dmpac_intd_cfgRegs *intdRegs, uint32_t irqNum,
    uint32_t htsPipelineNum)
{
    volatile uint32_t     regVal;

    regVal = CSL_REG32_RD(&intdRegs->STATUS_REG_LEVEL_DMPAC_OUT[irqNum][1U]);
    if (0u != (regVal & ((uint32_t)1u << htsPipelineNum)))
    {
        regVal = (uint32_t)1u << htsPipelineNum;
        CSL_REG32_WR(&intdRegs->STATUS_CLR_REG_LEVEL_DMPAC_OUT[irqNum][1U],
            regVal);
    }
}

/* ========================================================================== */
/*                           Local Functions                                  */
/* ========================================================================== */

