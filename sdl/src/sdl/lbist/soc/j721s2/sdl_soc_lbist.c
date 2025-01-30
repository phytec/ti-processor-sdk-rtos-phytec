/**
 * @file  sdl_soc_lbist.c
 *
 * @brief
 *  SDL implementation file for the SoC-specific lbist implementations.
 *
 *  \par
 *  ============================================================================
 *  @n   (C) Copyright 2021, Texas Instruments, Inc.
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
#include <sdl_types.h>
#include <src/sdl/sdl_lbist.h>
#include <src/sdl/lbist/sdl_lbist_priv.h>
#include <soc.h>

/* ========================================================================== */
/*                                Macros                                      */
/* ========================================================================== */
/* Lbist Parameters */
#define LBIST_DC_DEF                   (0x3u)
#define LBIST_DIVIDE_RATIO             (0x02u)
#define LBIST_STATIC_PC_DEF            (0x3ac0u) 
#define LBIST_RESET_PC_DEF             (0x0fu)
#define LBIST_SET_PC_DEF               (0x00u)
#define LBIST_SCAN_PC_DEF              (0x04u)
#define LBIST_PRPG_DEF                 (0x1fffffffffffffu)

/*
* LBIST setup parameters for each core
*/
#define LBIST_MAIN_R5_STATIC_PC_DEF    LBIST_STATIC_PC_DEF
#define LBIST_C7X_STATIC_PC_DEF        (0x3fc0u)
#define LBIST_A72_STATIC_PC_DEF        (0x3fc0u)
#define LBIST_DMPAC_STATIC_PC_DEF      (0x1880u)
#define LBIST_VPAC_STATIC_PC_DEF       (0x3fc0u)

/*
* LBIST expected MISR's (using parameters above)
*/
#define MAIN_R5_MISR_EXP_VAL           (0x71d66f87U)  
#define A72_MISR_EXP_VAL               (0x3544cc7fU)
#define C7X_MISR_EXP_VAL               (0x436d5a8bU)
#define VPAC_MISR_EXP_VAL              (0x4e2054dbU)
#define DMPAC_MISR_EXP_VAL             (0x53e1ef7bU)

/*
 * Note: the order for the below must match order of SDL_LBIST_inst
 */
static SDL_lbistInstInfo SDL_LBIST_InstInfoArray_ES1_0[SDL_LBIST_NUM_INSTANCES] =
{
 /* Main R5F 0 */
 {
  .pLBISTRegs             = (SDL_lbistRegs *)(SDL_MAIN_R5F0_LBIST_BASE),
  .pLBISTSig              = (uint32_t *)(SDL_MAIN_R5F0_LBIST_SIG),
  .expectedMISR           = MAIN_R5_MISR_EXP_VAL ,    /* Expected signature for main R5 0*/
  .interruptNumber        = SDLR_MCU_R5FSS0_CORE0_INTR_GLUELOGIC_MAIN_PULSAR0_LBIST_GLUE_DFT_LBIST_BIST_DONE_0, /* BIST DONE interrupt number */
  .doneFlag               = LBIST_NOT_DONE,           /* Initialize done flag */
  .LBISTConfig = {
      .dc_def        = LBIST_DC_DEF,
      .divide_ratio  = LBIST_DIVIDE_RATIO,
      .static_pc_def = LBIST_MAIN_R5_STATIC_PC_DEF,
      .set_pc_def    = LBIST_SET_PC_DEF,
      .reset_pc_def  = LBIST_RESET_PC_DEF,
      .scan_pc_def   = LBIST_SCAN_PC_DEF,
      .prpg_def      = LBIST_PRPG_DEF,
  },
 },
 /* Main R5F 1 */
 {
  .pLBISTRegs             = (SDL_lbistRegs *)(SDL_MAIN_R5F1_LBIST_BASE),
  .pLBISTSig              = (uint32_t *)(SDL_MAIN_R5F1_LBIST_SIG),
  .expectedMISR           = MAIN_R5_MISR_EXP_VAL,    /* Expected signature Main R5 1*/
  .interruptNumber        = SDLR_MCU_R5FSS0_CORE0_INTR_GLUELOGIC_MAIN_PULSAR1_LBIST_GLUE_DFT_LBIST_BIST_DONE_0,/* BIST DONE interrupt number */
  .doneFlag               = LBIST_NOT_DONE,          /* Initialize done flag */
  .LBISTConfig = {
      .dc_def        = LBIST_DC_DEF,
      .divide_ratio  = LBIST_DIVIDE_RATIO,
      .static_pc_def = LBIST_MAIN_R5_STATIC_PC_DEF,
      .set_pc_def    = LBIST_SET_PC_DEF,
      .reset_pc_def  = LBIST_RESET_PC_DEF,
      .scan_pc_def   = LBIST_SCAN_PC_DEF,
      .prpg_def      = LBIST_PRPG_DEF,
  },
 },
 /* C7x0 */
 {
  .pLBISTRegs             = (SDL_lbistRegs *)(SDL_C7X0_LBIST_BASE),
  .pLBISTSig              = (uint32_t *)(SDL_C7X0_LBIST_SIG),
  .expectedMISR           = C7X_MISR_EXP_VAL,        /* Expected signature for C7x*/
  .interruptNumber        = SDLR_MCU_R5FSS0_CORE0_INTR_COMPUTE_CLUSTER0_AC71_4_DFT_LBIST_DFT_LBIST_BIST_DONE_0,/* BIST DONE interrupt number */
  .doneFlag               = LBIST_NOT_DONE,          /* Initialize done flag */
  .LBISTConfig = {
      .dc_def        = LBIST_DC_DEF,
      .divide_ratio  = LBIST_DIVIDE_RATIO,
      .static_pc_def = LBIST_C7X_STATIC_PC_DEF,
      .set_pc_def    = LBIST_SET_PC_DEF,
      .reset_pc_def  = LBIST_RESET_PC_DEF,
      .scan_pc_def   = LBIST_SCAN_PC_DEF,
      .prpg_def      = LBIST_PRPG_DEF,
  },
 },
 /* C7x1 */
 {
  .pLBISTRegs             = (SDL_lbistRegs *)(SDL_C7X1_LBIST_BASE),
  .pLBISTSig              = (uint32_t *)(SDL_C7X1_LBIST_SIG),
  .expectedMISR           = C7X_MISR_EXP_VAL,        /* Expected signature for C7x*/
  .interruptNumber        = SDLR_MCU_R5FSS0_CORE0_INTR_COMPUTE_CLUSTER0_AC71_5_DFT_LBIST_DFT_LBIST_BIST_DONE_0,/* BIST DONE interrupt number */
  .doneFlag               = LBIST_NOT_DONE,          /* Initialize done flag */
  .LBISTConfig = {
      .dc_def        = LBIST_DC_DEF,
      .divide_ratio  = LBIST_DIVIDE_RATIO,
      .static_pc_def = LBIST_C7X_STATIC_PC_DEF,
      .set_pc_def    = LBIST_SET_PC_DEF,
      .reset_pc_def  = LBIST_RESET_PC_DEF,
      .scan_pc_def   = LBIST_SCAN_PC_DEF,
      .prpg_def      = LBIST_PRPG_DEF,
  },
 },
 /* VPAC0 */
 {
  .pLBISTRegs             = (SDL_lbistRegs *)(SDL_VPAC0_LBIST_BASE),
  .pLBISTSig              = (uint32_t *)(SDL_VPAC0_LBIST_SIG),
  .expectedMISR           = VPAC_MISR_EXP_VAL,                          /* Expected signature for C6x*/
  .interruptNumber        = SDLR_MCU_R5FSS0_CORE0_INTR_GLUELOGIC_VPAC_LBIST_GLUE_DFT_LBIST_BIST_DONE_0,/* BIST DONE interrupt number */
  .doneFlag               = LBIST_NOT_DONE,                             /* Initialize done flag */
  .LBISTConfig = {
      .dc_def        = LBIST_DC_DEF,
      .divide_ratio  = LBIST_DIVIDE_RATIO,
      .static_pc_def = LBIST_VPAC_STATIC_PC_DEF,
      .set_pc_def    = LBIST_SET_PC_DEF,
      .reset_pc_def  = LBIST_RESET_PC_DEF,
      .scan_pc_def   = LBIST_SCAN_PC_DEF,
      .prpg_def      = LBIST_PRPG_DEF,
  },
 },
 /* DMPAC */
 {
  .pLBISTRegs             = (SDL_lbistRegs *)(SDL_DMPAC_LBIST_BASE),
  .pLBISTSig              = (uint32_t *)(SDL_DMPAC_LBIST_SIG),
  .expectedMISR           = DMPAC_MISR_EXP_VAL,                         /* Expected signature for C6x*/
  .interruptNumber        = SDLR_MCU_R5FSS0_CORE0_INTR_GLUELOGIC_DMPAC_LBIST_GLUE_DFT_LBIST_BIST_DONE_0,/* BIST DONE interrupt number */
  .doneFlag               = LBIST_NOT_DONE,                             /* Initialize done flag */
  .LBISTConfig = {
      .dc_def        = LBIST_DC_DEF,
      .divide_ratio  = LBIST_DIVIDE_RATIO,
      .static_pc_def = LBIST_DMPAC_STATIC_PC_DEF,
      .set_pc_def    = LBIST_SET_PC_DEF,
      .reset_pc_def  = LBIST_RESET_PC_DEF,
      .scan_pc_def   = LBIST_SCAN_PC_DEF,
      .prpg_def      = LBIST_PRPG_DEF,
  },
 },

 /* A72_0 */
 {
  .pLBISTRegs             = (SDL_lbistRegs *)(SDL_A72_0_LBIST_BASE),
  .pLBISTSig              = (uint32_t *)(SDL_A72_0_LBIST_SIG),
  .expectedMISR           = A72_MISR_EXP_VAL,              /* Expected signature for A72 */
  .interruptNumber        = SDLR_MCU_R5FSS0_CORE0_INTR_COMPUTE_CLUSTER0_ARM0_DFT_LBIST_DFT_LBIST_BIST_DONE_0,/* BIST DONE interrupt number */
  .doneFlag               = LBIST_NOT_DONE,                /* Initialize done flag */
  .LBISTConfig = {
      .dc_def        = LBIST_DC_DEF,
      .divide_ratio  = LBIST_DIVIDE_RATIO,
      .static_pc_def = LBIST_A72_STATIC_PC_DEF,
      .set_pc_def    = LBIST_SET_PC_DEF,
      .reset_pc_def  = LBIST_RESET_PC_DEF,
      .scan_pc_def   = LBIST_SCAN_PC_DEF,
      .prpg_def      = LBIST_PRPG_DEF,
  },
 },
};

static SDL_lbistInstInfo SDL_LBIST_InstInfoArray_ES1_1[SDL_LBIST_NUM_INSTANCES] =
{
 /* Main R5F 0 */
 {
  .pLBISTRegs             = (SDL_lbistRegs *)(SDL_MAIN_R5F0_LBIST_BASE),
  .pLBISTSig              = (uint32_t *)(SDL_MAIN_R5F0_LBIST_SIG),
  .expectedMISR           = MAIN_R5_MISR_EXP_VAL ,    /* Expected signature for main R5 0*/
  .interruptNumber        = SDLR_MCU_R5FSS0_CORE0_INTR_GLUELOGIC_MAIN_PULSAR0_LBIST_GLUE_DFT_LBIST_BIST_DONE_0, /* BIST DONE interrupt number */
  .doneFlag               = LBIST_NOT_DONE,           /* Initialize done flag */
  .LBISTConfig = {
      .dc_def        = LBIST_DC_DEF,
      .divide_ratio  = LBIST_DIVIDE_RATIO,
      .static_pc_def = LBIST_MAIN_R5_STATIC_PC_DEF,
      .set_pc_def    = LBIST_SET_PC_DEF,
      .reset_pc_def  = LBIST_RESET_PC_DEF,
      .scan_pc_def   = LBIST_SCAN_PC_DEF,
      .prpg_def      = LBIST_PRPG_DEF,
  },
 },
 /* Main R5F 1 */
 {
  .pLBISTRegs             = (SDL_lbistRegs *)(SDL_MAIN_R5F1_LBIST_BASE),
  .pLBISTSig              = (uint32_t *)(SDL_MAIN_R5F1_LBIST_SIG),
  .expectedMISR           = MAIN_R5_MISR_EXP_VAL,    /* Expected signature Main R5 1*/
  .interruptNumber        = SDLR_MCU_R5FSS0_CORE0_INTR_GLUELOGIC_MAIN_PULSAR1_LBIST_GLUE_DFT_LBIST_BIST_DONE_0,/* BIST DONE interrupt number */
  .doneFlag               = LBIST_NOT_DONE,          /* Initialize done flag */
  .LBISTConfig = {
      .dc_def        = LBIST_DC_DEF,
      .divide_ratio  = LBIST_DIVIDE_RATIO,
      .static_pc_def = LBIST_MAIN_R5_STATIC_PC_DEF,
      .set_pc_def    = LBIST_SET_PC_DEF,
      .reset_pc_def  = LBIST_RESET_PC_DEF,
      .scan_pc_def   = LBIST_SCAN_PC_DEF,
      .prpg_def      = LBIST_PRPG_DEF,
  },
 },
 /* C7x0 */
 {
  .pLBISTRegs             = (SDL_lbistRegs *)(SDL_C7X0_LBIST_BASE),
  .pLBISTSig              = (uint32_t *)(SDL_C7X0_LBIST_SIG),
  .expectedMISR           = C7X_MISR_EXP_VAL,        /* Expected signature for C7x*/
  .interruptNumber        = SDLR_MCU_R5FSS0_CORE0_INTR_COMPUTE_CLUSTER0_AC71_4_DFT_LBIST_DFT_LBIST_BIST_DONE_0,/* BIST DONE interrupt number */
  .doneFlag               = LBIST_NOT_DONE,          /* Initialize done flag */
  .LBISTConfig = {
      .dc_def        = LBIST_DC_DEF,
      .divide_ratio  = LBIST_DIVIDE_RATIO,
      .static_pc_def = LBIST_C7X_STATIC_PC_DEF,
      .set_pc_def    = LBIST_SET_PC_DEF,
      .reset_pc_def  = LBIST_RESET_PC_DEF,
      .scan_pc_def   = LBIST_SCAN_PC_DEF,
      .prpg_def      = LBIST_PRPG_DEF,
  },
 },
 /* C7x1 */
 {
  .pLBISTRegs             = (SDL_lbistRegs *)(SDL_C7X1_LBIST_BASE),
  .pLBISTSig              = (uint32_t *)(SDL_C7X1_LBIST_SIG),
  .expectedMISR           = C7X_MISR_EXP_VAL,        /* Expected signature for C7x*/
  .interruptNumber        = SDLR_MCU_R5FSS0_CORE0_INTR_COMPUTE_CLUSTER0_AC71_5_DFT_LBIST_DFT_LBIST_BIST_DONE_0,/* BIST DONE interrupt number */
  .doneFlag               = LBIST_NOT_DONE,          /* Initialize done flag */
  .LBISTConfig = {
      .dc_def        = LBIST_DC_DEF,
      .divide_ratio  = LBIST_DIVIDE_RATIO,
      .static_pc_def = LBIST_C7X_STATIC_PC_DEF,
      .set_pc_def    = LBIST_SET_PC_DEF,
      .reset_pc_def  = LBIST_RESET_PC_DEF,
      .scan_pc_def   = LBIST_SCAN_PC_DEF,
      .prpg_def      = LBIST_PRPG_DEF,
  },
 },
 /* VPAC0 */
 {
  .pLBISTRegs             = (SDL_lbistRegs *)(SDL_VPAC0_LBIST_BASE),
  .pLBISTSig              = (uint32_t *)(SDL_VPAC0_LBIST_SIG),
  .expectedMISR           = VPAC_MISR_EXP_VAL,                          /* Expected signature for C6x*/
  .interruptNumber        = SDLR_MCU_R5FSS0_CORE0_INTR_GLUELOGIC_VPAC_LBIST_GLUE_DFT_LBIST_BIST_DONE_0,/* BIST DONE interrupt number */
  .doneFlag               = LBIST_NOT_DONE,                             /* Initialize done flag */
  .LBISTConfig = {
      .dc_def        = LBIST_DC_DEF,
      .divide_ratio  = LBIST_DIVIDE_RATIO,
      .static_pc_def = LBIST_VPAC_STATIC_PC_DEF,
      .set_pc_def    = LBIST_SET_PC_DEF,
      .reset_pc_def  = LBIST_RESET_PC_DEF,
      .scan_pc_def   = LBIST_SCAN_PC_DEF,
      .prpg_def      = LBIST_PRPG_DEF,
  },
 },
 /* DMPAC */
 {
  .pLBISTRegs             = (SDL_lbistRegs *)(SDL_DMPAC_LBIST_BASE),
  .pLBISTSig              = (uint32_t *)(SDL_DMPAC_LBIST_SIG),
  .expectedMISR           = DMPAC_MISR_EXP_VAL,                         /* Expected signature for C6x*/
  .interruptNumber        = SDLR_MCU_R5FSS0_CORE0_INTR_GLUELOGIC_DMPAC_LBIST_GLUE_DFT_LBIST_BIST_DONE_0,/* BIST DONE interrupt number */
  .doneFlag               = LBIST_NOT_DONE,                             /* Initialize done flag */
  .LBISTConfig = {
      .dc_def        = LBIST_DC_DEF,
      .divide_ratio  = LBIST_DIVIDE_RATIO,
      .static_pc_def = LBIST_DMPAC_STATIC_PC_DEF,
      .set_pc_def    = LBIST_SET_PC_DEF,
      .reset_pc_def  = LBIST_RESET_PC_DEF,
      .scan_pc_def   = LBIST_SCAN_PC_DEF,
      .prpg_def      = LBIST_PRPG_DEF,
  },
 },

 /* A72_0 */
 {
  .pLBISTRegs             = (SDL_lbistRegs *)(SDL_A72_0_LBIST_BASE),
  .pLBISTSig              = (uint32_t *)(SDL_A72_0_LBIST_SIG),
  .expectedMISR           = A72_MISR_EXP_VAL,              /* Expected signature for A72 */
  .interruptNumber        = SDLR_MCU_R5FSS0_CORE0_INTR_COMPUTE_CLUSTER0_ARM0_DFT_LBIST_DFT_LBIST_BIST_DONE_0,/* BIST DONE interrupt number */
  .doneFlag               = LBIST_NOT_DONE,                /* Initialize done flag */
  .LBISTConfig = {
      .dc_def        = LBIST_DC_DEF,
      .divide_ratio  = LBIST_DIVIDE_RATIO,
      .static_pc_def = LBIST_A72_STATIC_PC_DEF,
      .set_pc_def    = LBIST_SET_PC_DEF,
      .reset_pc_def  = LBIST_RESET_PC_DEF,
      .scan_pc_def   = LBIST_SCAN_PC_DEF,
      .prpg_def      = LBIST_PRPG_DEF,
  },
 },
};

SDL_lbistInstInfo * SDL_LBIST_getInstInfo(uint32_t index)
{
    SDL_lbistInstInfo *handle;
    SDL_lbistInstInfo *pInfo = NULL;
    uint32_t siliconRev = 0U;

    siliconRev = SDL_REG32_FEXT((SDL_WKUP_CTRL_MMR0_CFG0_BASE +
                                SDL_WKUP_CTRL_MMR_CFG0_JTAGID),
                                WKUP_CTRL_MMR_CFG0_JTAGID_VARIANT);
    if ((uint32_t)0U != siliconRev)
    {
        handle = SDL_LBIST_InstInfoArray_ES1_1;
    }
    else
    {
        handle = SDL_LBIST_InstInfoArray_ES1_0;
    }

    if (index < (uint32_t)SDL_LBIST_NUM_INSTANCES)
    {
        pInfo = &handle[index];
    }

    return pInfo;
}

void SDL_LBIST_eventHandler( uint32_t instanceId )
{
    int32_t status;
    bool isLBISTDone = FALSE;
    SDL_lbistInstInfo *pInstInfo = SDL_LBIST_getInstInfo(instanceId);
    SDL_lbistRegs *pLBISTRegs;

    if (pInstInfo != NULL)
    {
        pLBISTRegs = pInstInfo->pLBISTRegs;
        /* Double check if the LBIST done flag is set */
        status = SDL_LBIST_isDone(pLBISTRegs, &isLBISTDone);
        if ((status == SDL_PASS) && (isLBISTDone == TRUE))
        {
            pInstInfo->doneFlag = LBIST_DONE;
            /* Need to pull run down to low to clear the done interrupt */
            (void)SDL_LBIST_stop( pLBISTRegs );
        }
    }
    return;

}


static SDL_lbistHWPOSTInstInfo SDL_LBIST_HWPOSTInstInfoArray[SDL_LBIST_HWPOST_NUM_INSTANCES] =
{
 /* HW POST - DMSC - Checks MISR results only */
 {
  .pLBISTRegs             = (SDL_lbistRegs *)(SDL_SMS_LBIST_BASE),
  .pLBISTSig              = (uint32_t *)(SDL_SMS_LBIST_SIG),
 },
 /* HW POST - MCU - Checks MISR results only */
 {
  .pLBISTRegs             = (SDL_lbistRegs *)(SDL_MCU_LBIST_BASE),
  .pLBISTSig              = (uint32_t *)(SDL_MCU_LBIST_SIG),
 },
};

static SDL_lbistHWPOSTInstInfo * SDL_LBIST_getHWPOSTInstInfo(uint32_t instance)
{
    SDL_lbistHWPOSTInstInfo *pInfo = NULL;

    pInfo = &SDL_LBIST_HWPOSTInstInfoArray[instance];

    return pInfo;
}

static int32_t SDL_LBIST_isPostLbistTimeout(uint32_t postStatMmrRegVal,
                                            SDL_LBIST_hwpostInst instance,
                                            bool *pIsTimedOut)
{
    int32_t status = SDL_PASS;
    uint32_t shift;

    if (pIsTimedOut == NULL)
    {
        status = SDL_EBADARGS;
    }
    else
    {
        if (instance == SDL_LBIST_HWPOST_INST_SMS)
        {
            shift = SDL_WKUP_CTRL_MMR_CFG0_WKUP_POST_STAT_POST_SMS_LBIST_TIMEOUT_SHIFT;
        }
        else
        {
            shift = SDL_WKUP_CTRL_MMR_CFG0_WKUP_POST_STAT_POST_MCU_LBIST_TIMEOUT_SHIFT;
        }

        if (((postStatMmrRegVal >> shift) & 0x1u) == 0x0u)
        {
            *pIsTimedOut = FALSE;
        }
        else
        {
            *pIsTimedOut = TRUE;
        }
    }

    return status;
}

static void SDL_LBIST_isPostLbistDone(uint32_t postStatMmrRegVal,
                                      SDL_LBIST_hwpostInst instance,
                                      bool *pIsDone)
{
    uint32_t shift;

    if (instance == SDL_LBIST_HWPOST_INST_SMS)
    {
        shift = SDL_WKUP_CTRL_MMR_CFG0_WKUP_POST_STAT_POST_SMS_LBIST_DONE_SHIFT;
    }
    else
    {
        shift = SDL_WKUP_CTRL_MMR_CFG0_WKUP_POST_STAT_POST_MCU_LBIST_DONE_SHIFT;
    }

    if (((postStatMmrRegVal >> shift) & 0x1u) == 0x0u)
    {
        *pIsDone = FALSE;
    }
    else
    {
        *pIsDone = TRUE;
    }

    return;
}

static int32_t SDL_LBIST_runPostLbistCheck(SDL_LBIST_hwpostInst instance, SDL_LBIST_postStatus *pPostStatus)
{
    int32_t  status = SDL_PASS;
    SDL_lbistHWPOSTInstInfo *pInfo = NULL;
    SDL_lbistRegs *pRegs;
    uint32_t *pLBISTSig;
    uint32_t calculatedMISR;
    uint32_t expectedMISR = 0;
    uint32_t postRegVal;
    bool     LBISTResult;

    pInfo = SDL_LBIST_getHWPOSTInstInfo((uint32_t)instance);
    if (pPostStatus == NULL)
    {
        status = SDL_EBADARGS;
    }

    if (status == SDL_PASS)
    {
        /* Get LBIST register space Pointer */
        pRegs = pInfo->pLBISTRegs;
        pLBISTSig = pInfo->pLBISTSig;
        *pPostStatus = SDL_LBIST_POST_NOT_RUN;

        /* Read HW POST status register */
        postRegVal = SDL_REG32_RD(SDL_WKUP_CTRL_MMR0_CFG0_BASE +
                                  SDL_WKUP_CTRL_MMR_CFG0_WKUP_POST_STAT);

        /* Check if HW POST LBIST was performed */
        SDL_LBIST_isPostLbistDone(postRegVal, instance, &LBISTResult);
        if (LBISTResult != (bool)true)
        {
            /* HW POST: LBIST not completed, check if it timed out */
            status = SDL_LBIST_isPostLbistTimeout(postRegVal,
                                                  instance,
                                                  &LBISTResult);
            if (LBISTResult != (bool)true)
            {
                /* HW POST: LBIST was not performed at all on this device
                 * for this core */
                *pPostStatus = SDL_LBIST_POST_NOT_RUN;
            }
            else
            {
                /* HW POST: LBIST was attempted but timed out for this section */
                *pPostStatus = SDL_LBIST_POST_TIMEOUT;
            }
        }
        else
        {
            /* Get the output MISR and the expected MISR */
            status = SDL_LBIST_getMISR(pRegs, &calculatedMISR);

            if (status == SDL_PASS)
            {
                status = SDL_LBIST_getExpectedMISR(pLBISTSig, &expectedMISR);
            }

            /* Compare the results, and set failure if they do not match */
            if (calculatedMISR != expectedMISR)
            {
                /* HW POST: LBIST was completed, but the test failed for this
                 * core */
                *pPostStatus = SDL_LBIST_POST_COMPLETED_FAILURE;
            }
            else if ((calculatedMISR == (uint32_t)0U) && (expectedMISR == (uint32_t)0U))
            {
                *pPostStatus = SDL_LBIST_POST_NOT_RUN;
            }
            else
            {
                *pPostStatus = SDL_LBIST_POST_COMPLETED_SUCCESS;
            }
        } /* if (LBISTResult != true) */
    }

    return status;
}

int32_t SDL_LBIST_getDevicePostStatus(SDL_LBIST_postResult *pResult)
{
    int32_t ret = SDL_PASS;
    int32_t status = SDL_PASS;

    if (pResult == NULL)
    {
        status = SDL_EBADARGS;
    }
    else
    {
        /* Get status for MCU */
        status = SDL_LBIST_runPostLbistCheck(SDL_LBIST_HWPOST_INST_MCU, &pResult->mcuPostStatus);

        if (status != SDL_PASS)
        {
            ret = SDL_EFAIL;
        }

        status = SDL_LBIST_runPostLbistCheck(SDL_LBIST_HWPOST_INST_SMS, &pResult->smsPostStatus);
    }

    if (status != SDL_PASS)
    {
        ret = SDL_EFAIL;
    }

    return ret;
}
