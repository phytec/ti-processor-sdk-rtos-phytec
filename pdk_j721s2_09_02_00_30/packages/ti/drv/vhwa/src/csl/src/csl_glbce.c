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
 */


/**
 *  \file csl_glbce.c
 *
 *  \brief GLBCE CSL file, setting up different modules of GLBCE
 *
 */


/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/drv/vhwa/src/csl/include/csl_glbce.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 *  \brief Minimum width supported by GLBCE, there is no register to
 *         read this value, so macro is used.
 */
#define CSL_GLBCE_MIN_WIDTH                     (480U)

/**
 *  \brief Minimum height supported by GLBCE, there is no register to
 *         read this value, so macro is used.
 */
#define CSL_GLBCE_MIN_HEIGHT                    (240U)

/**
 *  \brief Size of the GLBCE Stats memory, used for saving/restoring context
 */
#define CSL_GLBCE_STATS_SIZE (sizeof(CSL_glbce_statmemRegs))

/**
 *  \brief Max width supported by GLBCE, from the register field
 */
#define CSL_GLBCE_MAX_WIDTH                     (                              \
    CSL_GLBCE_FRAME_WIDTH_VAL_MASK >>  CSL_GLBCE_FRAME_WIDTH_VAL_SHIFT)

/**
 *  \brief Max width supported by GLBCE, from the register field
 */
#define CSL_GLBCE_MAX_HEIGHT                    (                              \
    CSL_GLBCE_FRAME_HEIGHT_VAL_MASK >> CSL_GLBCE_FRAME_HEIGHT_VAL_SHIFT)

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/* Function to set GLBCE configuration, mainly frame size */
int32_t CSL_glbceSetConfig(CSL_glbceRegs *gRegs, const CSL_GlbceConfig *cfg)
{
    int32_t  status = FVID2_SOK;

    if ((NULL == cfg) || (NULL == gRegs))
    {
        status = FVID2_EBADARGS;
    }
    else
    {
        /* Check for errors */
        if((CSL_GLBCE_MIN_WIDTH > cfg->width) ||
           (CSL_GLBCE_MIN_HEIGHT > cfg->height) ||
           (cfg->width > CSL_GLBCE_MAX_WIDTH) ||
           (cfg->height > CSL_GLBCE_MAX_HEIGHT))
        {
            status = FVID2_EINVALID_PARAMS;
        }
    }

    if(FVID2_SOK == status)
    {
        /* Set the command parameters like frame size/ format etc */
        CSL_REG32_FINS(&gRegs->FRAME_WIDTH, GLBCE_FRAME_WIDTH_VAL,
            cfg->width);
        CSL_REG32_FINS(&gRegs->FRAME_HEIGHT, GLBCE_FRAME_HEIGHT_VAL,
            cfg->height);

        /* Set the processing mode */
        CSL_REG32_FINS(&gRegs->MODE, GLBCE_MODE_OST, cfg->isOneShotMode);
    }

    return (status);
}

int32_t CSL_glbceSetToneMapConfig(CSL_glbceRegs *gRegs,
    const Glbce_Config *cfg)
{
    int32_t            status = FVID2_EBADARGS;
    uint32_t           cnt;
    volatile uint32_t *regAddr;
    volatile uint32_t  regVal;

    /* Check for null pointers and errors */
    if ((NULL != cfg) && (NULL != gRegs))
    {
        /* Configure Asymmetry LUT */
        regAddr = &gRegs->LUT_FI[0U];
        for(cnt = 0U; cnt < GLBCE_ASYMMETRY_LUT_SIZE; cnt++)
        {
            /* Size of all the Lut entries are same, so using Mask
               and shift of Lut Entry 0 */
            CSL_REG32_FINS(regAddr, GLBCE_LUT_FI_00_VAL, cfg->asymLut[cnt]);

            regAddr ++;
        }

        /* Configure tone mapping slope limit */
        CSL_REG32_FINS(&gRegs->SLOPE_MAX, GLBCE_SLOPE_MAX_SLOPEMAXLIMIT,
            cfg->maxSlopeLimit);
        CSL_REG32_FINS(&gRegs->SLOPE_MIN, GLBCE_SLOPE_MIN_SLOPEMINLIMIT,
            cfg->minSlopeLimit);

        /* Configure Dithering, it will also disable dithering
         * if not required */
        CSL_REG32_FINS(&gRegs->DITHER, GLBCE_DITHER_DITHER,
            cfg->dither);

        /* Configure tone-mapping curve limit */
        regVal  = CSL_REG32_RD(&gRegs->LIMIT_AMPL);
        CSL_FINS(regVal, GLBCE_LIMIT_AMPL_DARKAMPLIFICATIONLIMIT,
            cfg->darkAmplLimit);
        CSL_FINS(regVal, GLBCE_LIMIT_AMPL_BRIGHTAMPLIFICATIONLIMIT,
            cfg->brightAmplLimit);
        CSL_REG32_WR(&gRegs->LIMIT_AMPL, regVal);

        /* Configure Space and Intensity Variance */
        regVal  = CSL_REG32_RD(&gRegs->VARIANCE);
        CSL_FINS(regVal, GLBCE_VARIANCE_VARIANCESPACE,
            cfg->spaceVariance);
        CSL_FINS(regVal, GLBCE_VARIANCE_VARIANCEINTENSITY,
            cfg->intensityVariance);
        CSL_REG32_WR(&gRegs->VARIANCE, regVal);

        /* Configure White and Blank Level */
        CSL_REG32_FINS(&gRegs->BLACK_LEVEL, GLBCE_BLACK_LEVEL_VAL,
            cfg->blackLevel);
        CSL_REG32_FINS(&gRegs->WHITE_LEVEL, GLBCE_WHITE_LEVEL_VAL,
            cfg->whiteLevel);

        /* Configure IR Strength */
        CSL_REG32_FINS(&gRegs->STRENGTH_IR, GLBCE_STRENGTH_IR_VAL,
            cfg->irStrength);

        status = FVID2_SOK;
    }

    return (status);
}

/* Function to set GLBCE Percept Forward/Reverse configuration */
int32_t CSL_glbceSetPerceptConfig(CSL_glbceRegs *gRegs,
    const Glbce_PerceptConfig *cfg, uint32_t perceptDir)
{
    int32_t               status  = FVID2_SOK;
    volatile uint32_t    *regAddr;
    uint32_t              cnt;

    /* Check for null pointers and errors */
    if ((NULL == cfg) || (NULL == gRegs))
    {
        status = FVID2_EBADARGS;
    }
    else
    {
        if ((GLBCE_MODULE_FWD_PERCEPT != perceptDir) &&
            (GLBCE_MODULE_REV_PERCEPT != perceptDir))
        {
            status = FVID2_EINVALID_PARAMS;
        }
    }

    if (FVID2_SOK == status)
    {
        if((uint32_t)UTRUE == cfg->enable)
        {
            if(GLBCE_MODULE_FWD_PERCEPT == perceptDir)
            {
                regAddr = &gRegs->FWD_PERCEPT_LUT[0U];

                CSL_REG32_FINS(&gRegs->PERCEPT_EN, GLBCE_PERCEPT_EN_FWD_EN, 1U);
            }
            else
            {
                regAddr = &gRegs->REV_PERCEPT_LUT[0U];

                CSL_REG32_FINS(&gRegs->PERCEPT_EN, GLBCE_PERCEPT_EN_REV_EN, 1U);
            }

            for(cnt = 0U; cnt < GLBCE_PERCEPT_LUT_SIZE; cnt++)
            {
                /* Since size of the table entry and position in the
                   register is same for Forward and Reverse perpetual module,
                   using mask and shift from forward perpetual table
                   for both */
                CSL_REG32_FINS(regAddr, GLBCE_FWD_PERCEPT_LUT_00_VAL,
                    cfg->table[cnt]);

                regAddr ++;
            }
        }
        else /* Disable/Bypass Forward/Reverse Perpetual Module */
        {
            if(GLBCE_MODULE_FWD_PERCEPT == perceptDir)
            {
                CSL_REG32_FINS(&gRegs->PERCEPT_EN, GLBCE_PERCEPT_EN_FWD_EN, 0U);
            }
            else /* Reverse Perpetual */
            {
                CSL_REG32_FINS(&gRegs->PERCEPT_EN, GLBCE_PERCEPT_EN_REV_EN, 0U);
            }
        }
    }

    return (status);
}

/* Function to set GLBCE Percept Forward/Reverse configuration */
int32_t CSL_glbceSetWdrConfig(CSL_glbceRegs *gRegs,
    const Glbce_WdrConfig *cfg)
{
    int32_t           status = FVID2_SOK;
    volatile uint32_t *regAddr;
    uint32_t          cnt;

    if ((NULL == cfg) || (NULL == gRegs))
    {
        status = FVID2_EBADARGS;
    }
    else
    {
        if((uint32_t)UTRUE == cfg->enable)
        {
            /* Set the WDR LUT */
            regAddr = &gRegs->WDR_GAMMA_LUT[0U];
            for(cnt = 0U; cnt < GLBCE_WDR_LUT_SIZE; cnt++)
            {
                /* ALL Tables entries are of same size, so using
                   macros of LUT_00 */
                CSL_REG32_FINS(regAddr, GLBCE_WDR_GAMMA_LUT_00_VAL,
                    cfg->table[cnt]);

                regAddr ++;
            }

            CSL_REG32_FINS(&gRegs->WDR_GAMMA_EN, GLBCE_WDR_GAMMA_EN_EN, 1U);
        }
        else
        {
            CSL_REG32_FINS(&gRegs->WDR_GAMMA_EN, GLBCE_WDR_GAMMA_EN_EN, 0U);
        }
    }

    return (status);
}

/* Function to start the GLBCE AF module either in one shot or in free
 * running mode */
int32_t CSL_glbceStart(CSL_glbceRegs *gRegs)
{
    int32_t               status  = FVID2_SOK;

    if (NULL == gRegs)
    {
        status = FVID2_EBADARGS;
    }
    else
    {
        /* Enable Glbce engine, all the parameters must be set before
         * enabling this */
        gRegs->CONTROL0 = 0x0;
        /* TODO: Provide interface for this control register */
        gRegs->CONTROL1 = 0x6;
        CSL_REG32_FINS(&gRegs->CONTROL0, GLBCE_CONTROL0_ONOFF, 1U);
    }

    return (status);
}

/* Function to start the GLBCE AF module either in one shot or in free
 * running mode */
int32_t CSL_glbceStop(CSL_glbceRegs *gRegs)
{
    int32_t               status  = FVID2_SOK;

    if (NULL == gRegs)
    {
        status = FVID2_EBADARGS;
    }
    else
    {
        /* Enable Glbce engine, all the parameters must be set before
         * enabling this */
        CSL_REG32_FINS(&gRegs->PERCEPT_EN, GLBCE_PERCEPT_EN_FWD_EN, 0U);
        CSL_REG32_FINS(&gRegs->PERCEPT_EN, GLBCE_PERCEPT_EN_REV_EN, 0U);
        CSL_REG32_FINS(&gRegs->WDR_GAMMA_EN, GLBCE_WDR_GAMMA_EN_EN, 0U);
        CSL_REG32_FINS(&gRegs->CONTROL0, GLBCE_CONTROL0_ONOFF, 0U);
        CSL_REG32_FINS(&gRegs->STRENGTH_IR, GLBCE_STRENGTH_IR_VAL,
            0u);
    }

    return (status);
}

int32_t CSL_glbceGetStatsInfo(const CSL_glbce_statmemRegs *glbceStatsMem,
    Glbce_StatsInfo *stats)
{
    int32_t status = FVID2_EBADARGS;

    if (NULL != stats)
    {
        /* Assuming System Address is same as local R5F
         * address for GLBCE */
        stats->addr = (uint64_t)glbceStatsMem;
        stats->size = CSL_GLBCE_STATS_SIZE;
        status = FVID2_SOK;
    }

    return (status);
}

/* -------------------------------------------------------------------------- */
/*                 Internal Function Definitions                              */
/* -------------------------------------------------------------------------- */

/* None */
