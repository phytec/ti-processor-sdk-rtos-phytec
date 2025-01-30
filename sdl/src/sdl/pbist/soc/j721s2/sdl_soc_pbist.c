/**
 * @file  sdl_soc_pbist.c
 *
 * @brief
 *  SDL implementation file for the SoC-specific pbist implementations.
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
#include <src/sdl/sdl_pbist.h>
#include <src/sdl/pbist/sdl_pbist_priv.h>
#include <soc.h>

/**
 *  \brief Maximum number of input events supported by CLEC. This is just the
 *  maximum registers supported for programming. The actual event supported
 *  depends on the SOC.
 */
#define SDL_CLEC_MAX_EVT_IN             (2047U)

#define ECR_CLR_VAL                     (1U)
#define PBIST_MCU_SELECTED_MEMORY
/**************************************************************************
* Register Overlay Structure
**************************************************************************/

typedef struct {
    volatile uint32_t PID;                       /* CLEC PID register */
    volatile uint8_t  Resv_1000[4092];
    volatile uint32_t MRR;
    volatile uint8_t  RESV_3000[8188];
    volatile uint32_t ESR;
    volatile uint8_t  Resv_4000[4092];
    volatile uint32_t ECR;
    volatile uint8_t  Resv_C000[32764];
    volatile uint32_t GELRS;                     /* Global Event Lock register for secure claims */
    volatile uint8_t  Resv_D000[4092];
    volatile uint32_t GELRNS;                    /* Global Event Lock register for non-secure claims */
    volatile uint8_t  Resv_12284[12284];
} SDL_PBIST_CLEC_EVTRegs_GLOBAL;

typedef struct {
    SDL_PBIST_CLEC_EVTRegs_GLOBAL CFG[2047]; /* This block will contain all
                                              * registers only CFG[0] will
                                              * contain the PID, GELRS and
                                              * GELRNS registes*/
} SDL_PBIST_CLEC_EVTRegs;

/*
 * Note: the order for the below must match order of SDL_PBIST_inst
 */
static SDL_pbistInstInfo SDL_PBIST_InstInfoArray[SDL_PBIST_NUM_INSTANCES] =
{
    /* VPAC Instance */
    {
        .PBISTRegsHiAddress  = 0u,
        .pPBISTRegs          = (SDL_pbistRegs *)SDL_PBIST8_BASE,
        .numPBISTRuns        = SDL_PBIST8_NUM_TEST_VECTORS,
        .PBISTConfigRun = {
            /* Golden Vector Part 1 */
            {
                .override           = 0x0u,
                /* Override bit set to 0 to use memoryGroupsBitMap & algorithmsBitMap */
                .algorithmsBitMap   = SDL_PBIST8_ALGO_BITMAP_0,   /* Choose recommended Algo bitmap 0 */
                .memoryGroupsBitMap = SDL_PBIST8_MEM_BITMAP_0,    /* Choose recommended mem bitmap 0  */
                .scrambleValue      = 0xFEDCBA9876543210U,    /* Scramble Value */
            },
            /* Golden Vector Part 2 */
            {
                .override           = 0x0u,
                /* Override bit set to 0 to use memoryGroupsBitMap & algorithmsBitMap */
                .algorithmsBitMap   = SDL_PBIST8_ALGO_BITMAP_1,   /* Choose recommended Algo bitmap 1 */
                .memoryGroupsBitMap = SDL_PBIST8_MEM_BITMAP_1,    /* Choose recommended mem bitmap 1  */
                .scrambleValue      = 0xFEDCBA9876543210U,       /* Scramble Value */
            }
        },
        .PBISTNegConfigRun = {
            .CA0   = SDL_PBIST8_FAIL_INSERTION_TEST_VECTOR_CA0,
            .CA1   = SDL_PBIST8_FAIL_INSERTION_TEST_VECTOR_CA1,
            .CA2   = SDL_PBIST8_FAIL_INSERTION_TEST_VECTOR_CA2,
            .CA3   = SDL_PBIST8_FAIL_INSERTION_TEST_VECTOR_CA3,
            .CL0   = SDL_PBIST8_FAIL_INSERTION_TEST_VECTOR_CL0,
            .CL1   = SDL_PBIST8_FAIL_INSERTION_TEST_VECTOR_CL1,
            .CL2   = SDL_PBIST8_FAIL_INSERTION_TEST_VECTOR_CL2,
            .CL3   = SDL_PBIST8_FAIL_INSERTION_TEST_VECTOR_CL3,
            .CMS   = SDL_PBIST8_FAIL_INSERTION_TEST_VECTOR_CMS,
            .CSR   = SDL_PBIST8_FAIL_INSERTION_TEST_VECTOR_CSR,
            .I0    = SDL_PBIST8_FAIL_INSERTION_TEST_VECTOR_I0,
            .I1    = SDL_PBIST8_FAIL_INSERTION_TEST_VECTOR_I1,
            .I2    = SDL_PBIST8_FAIL_INSERTION_TEST_VECTOR_I2,
            .I3    = SDL_PBIST8_FAIL_INSERTION_TEST_VECTOR_I3,
            .RAMT  = SDL_PBIST8_FAIL_INSERTION_TEST_VECTOR_RAMT
        },
        .numPBISTROMTestRuns = 1u,
        .PBISTROMTestConfigRun = 
        {
            {
                .D = 0x65F885E3u,
                .E = 0x65F885E3u,
                .CA2 = 0x7FFFu,
                .CL0 = 0x3FFu,
                .CA3 = 0x0u,
                .I0 = 0x1u,
                .CL1 = 0x1Fu,
                .I3 = 0x0u,
                .I2 = 0xEu,
                .CL2 = 0xEu,
                .CA1 = 0x7FFFu,
                .CA0 = 0x0u,
                .CL3 = 0x7FFFu,
                .I1 = 0x20u,
                .RAMT = 0x20002020u,
                .CSR = 0x00000001u,
                .CMS = 0x01u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            }
        },
        .interruptNumber        = SDLR_MCU_R5FSS0_CORE0_INTR_PBIST8_DFT_PBIST_CPU_0,
        .clecInputEvt           = 0x0u,
        .doneFlag               = PBIST_NOT_DONE
    },
   /* Main Infrastructure Instance */
    {
        .PBISTRegsHiAddress  = 0u,
        .pPBISTRegs          = (SDL_pbistRegs *)SDL_PBIST1_BASE,
        .numPBISTRuns        = SDL_PBIST1_NUM_TEST_VECTORS,
        .PBISTConfigRun = {
            /* Golden Vector Part 1 */
            {
                .override           = 0u,
                /* Override bit set to 0 to use memoryGroupsBitMap & algorithmsBitMap */
                .algorithmsBitMap   = SDL_PBIST1_ALGO_BITMAP_0,   /* Choose recommended Algo bitmap 0 */
                .memoryGroupsBitMap = SDL_PBIST1_MEM_BITMAP_0,    /* Choose recommended mem bitmap 0  */
                .scrambleValue      = 0xFEDCBA9876543210U, /* Scramble Value */
            },
            /* Golden Vector Part 2 */
            {
                .override           = 0u,
                /* Override bit set to 0 to use memoryGroupsBitMap & algorithmsBitMap */
                .algorithmsBitMap   = SDL_PBIST1_ALGO_BITMAP_1,   /* Choose recommended Algo bitmap 1 */
                .memoryGroupsBitMap = SDL_PBIST1_MEM_BITMAP_1,    /* Choose recommended mem bitmap 1 */
                .scrambleValue      = 0xFEDCBA9876543210U, /* Scramble Value */
            }
        },
        .PBISTNegConfigRun = {
            .CA0   = SDL_PBIST1_FAIL_INSERTION_TEST_VECTOR_CA0,
            .CA1   = SDL_PBIST1_FAIL_INSERTION_TEST_VECTOR_CA1,
            .CA2   = SDL_PBIST1_FAIL_INSERTION_TEST_VECTOR_CA2,
            .CA3   = SDL_PBIST1_FAIL_INSERTION_TEST_VECTOR_CA3,
            .CL0   = SDL_PBIST1_FAIL_INSERTION_TEST_VECTOR_CL0,
            .CL1   = SDL_PBIST1_FAIL_INSERTION_TEST_VECTOR_CL1,
            .CL2   = SDL_PBIST1_FAIL_INSERTION_TEST_VECTOR_CL2,
            .CL3   = SDL_PBIST1_FAIL_INSERTION_TEST_VECTOR_CL3,
            .CMS   = SDL_PBIST1_FAIL_INSERTION_TEST_VECTOR_CMS,
            .CSR   = SDL_PBIST1_FAIL_INSERTION_TEST_VECTOR_CSR,
            .I0    = SDL_PBIST1_FAIL_INSERTION_TEST_VECTOR_I0,
            .I1    = SDL_PBIST1_FAIL_INSERTION_TEST_VECTOR_I1,
            .I2    = SDL_PBIST1_FAIL_INSERTION_TEST_VECTOR_I2,
            .I3    = SDL_PBIST1_FAIL_INSERTION_TEST_VECTOR_I3,
            .RAMT  = SDL_PBIST1_FAIL_INSERTION_TEST_VECTOR_RAMT
        },
        .numPBISTROMTestRuns = 1u,
        .PBISTROMTestConfigRun = 
        {
            {
                .D = 0x06EDAC5Fu,
                .E = 0x06EDAC5Fu,
                .CA2 = 0x7FFFu,
                .CL0 = 0x3FFu,
                .CA3 = 0x0u,
                .I0 = 0x1u,
                .CL1 = 0x1Fu,
                .I3 = 0x0u,
                .I2 = 0xEu,
                .CL2 = 0xEu,
                .CA1 = 0x7FFFu,
                .CA0 = 0x0u,
                .CL3 = 0x7FFFu,
                .I1 = 0x20u,
                .RAMT = 0x10002020u,
                .CSR = 0x00000001u,
                .CMS = 0x01u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            }
        },
        .interruptNumber        = SDLR_MCU_R5FSS0_CORE0_INTR_PBIST1_DFT_PBIST_CPU_0,
        .clecInputEvt           = 0x0u,
        .doneFlag               = PBIST_NOT_DONE
    },
    /* DMPAC Instance */
    {
        .PBISTRegsHiAddress  = 0u,
        .pPBISTRegs          = (SDL_pbistRegs *)SDL_PBIST7_BASE,
        .numPBISTRuns        = SDL_PBIST7_NUM_TEST_VECTORS,
        .PBISTConfigRun = {
            /* Golden Vector Part 1 */
            {
                .override           = 0x0u,
                /* Override bit set to 0 to use memoryGroupsBitMap & algorithmsBitMap */
                .algorithmsBitMap   =   SDL_PBIST7_ALGO_BITMAP_0,   /* Choose recommended Algo bitmap 0 */
                .memoryGroupsBitMap = SDL_PBIST7_MEM_BITMAP_0,      /* Choose recommended mem bitmap 0 */
                .scrambleValue      = 0xFEDCBA9876543210U, /* Scramble Value */
            },
            /* Golden Vector Part 2 */
            {
                .override           = 0x0u,
                /* Override bit set to 0 to use memoryGroupsBitMap & algorithmsBitMap */
                .algorithmsBitMap   =   SDL_PBIST7_ALGO_BITMAP_1,   /* Choose recommended Algo bitmap 1 */
                .memoryGroupsBitMap = SDL_PBIST7_MEM_BITMAP_1,      /* Choose recommended mem bitmap 1  */
                .scrambleValue      = 0xFEDCBA9876543210U,   /* Scramble Value */
            },
        },
        .PBISTNegConfigRun = {
            .CA0   = SDL_PBIST7_FAIL_INSERTION_TEST_VECTOR_CA0,
            .CA1   = SDL_PBIST7_FAIL_INSERTION_TEST_VECTOR_CA1,
            .CA2   = SDL_PBIST7_FAIL_INSERTION_TEST_VECTOR_CA2,
            .CA3   = SDL_PBIST7_FAIL_INSERTION_TEST_VECTOR_CA3,
            .CL0   = SDL_PBIST7_FAIL_INSERTION_TEST_VECTOR_CL0,
            .CL1   = SDL_PBIST7_FAIL_INSERTION_TEST_VECTOR_CL1,
            .CL2   = SDL_PBIST7_FAIL_INSERTION_TEST_VECTOR_CL2,
            .CL3   = SDL_PBIST7_FAIL_INSERTION_TEST_VECTOR_CL3,
            .CMS   = SDL_PBIST7_FAIL_INSERTION_TEST_VECTOR_CMS,
            .CSR   = SDL_PBIST7_FAIL_INSERTION_TEST_VECTOR_CSR,
            .I0    = SDL_PBIST7_FAIL_INSERTION_TEST_VECTOR_I0,
            .I1    = SDL_PBIST7_FAIL_INSERTION_TEST_VECTOR_I1,
            .I2    = SDL_PBIST7_FAIL_INSERTION_TEST_VECTOR_I2,
            .I3    = SDL_PBIST7_FAIL_INSERTION_TEST_VECTOR_I3,
            .RAMT  = SDL_PBIST7_FAIL_INSERTION_TEST_VECTOR_RAMT
        },
        .numPBISTROMTestRuns = 1u,
        .PBISTROMTestConfigRun = 
        {
            {
                .D = 0xD6F4BA45u,
                .E = 0xD6F4BA45u,
                .CA2 = 0x7FFFu,
                .CL0 = 0x3FFu,
                .CA3 = 0x0u,
                .I0 = 0x1u,
                .CL1 = 0x1Fu,
                .I3 = 0x0u,
                .I2 = 0xEu,
                .CL2 = 0xEu,
                .CA1 = 0x7FFFu,
                .CA0 = 0x0u,
                .CL3 = 0x7FFFu,
                .I1 = 0x20u,
                .RAMT = 0x20002020u,
                .CSR = 0x00000001u,
                .CMS = 0x01u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            }
        },
        .interruptNumber        = SDLR_MCU_R5FSS0_CORE0_INTR_PBIST7_DFT_PBIST_CPU_0,
        .clecInputEvt           = 0x0u,
        .doneFlag               = PBIST_NOT_DONE
    },
    /* Main Pulsar Instance 0 */
    {
        .PBISTRegsHiAddress  = 0u,
        .pPBISTRegs          = (SDL_pbistRegs *)SDL_PBIST2_BASE, /* PBIST2: Main R5F 0 */
        .numPBISTRuns        = SDL_PBIST2_NUM_TEST_VECTORS,
        .PBISTConfigRun = {
            {
                .override           = 0x0u,
                /* Override bit set to 0 to use memoryGroupsBitMap & algorithmsBitMap */
                /* NOTE: override set to 1 is not functional currently */
                .algorithmsBitMap   = SDL_PBIST2_ALGO_BITMAP_0,   /* Choose recommended Algo bitmap 0 */
                .memoryGroupsBitMap = SDL_PBIST2_MEM_BITMAP_0,    /* Choose recommended mem bitmap 0  */
                .scrambleValue      = 0xFEDCBA9876543210U,        /* Scramble Value */
            },
            {
                .override           = 0x0u,
                .algorithmsBitMap   = 0x0u,
                .memoryGroupsBitMap = 0x0u,
                .scrambleValue      = 0x0u,
            }
        },
        .PBISTNegConfigRun = {
            .CA0   = SDL_PBIST2_FAIL_INSERTION_TEST_VECTOR_CA0,
            .CA1   = SDL_PBIST2_FAIL_INSERTION_TEST_VECTOR_CA1,
            .CA2   = SDL_PBIST2_FAIL_INSERTION_TEST_VECTOR_CA2,
            .CA3   = SDL_PBIST2_FAIL_INSERTION_TEST_VECTOR_CA3,
            .CL0   = SDL_PBIST2_FAIL_INSERTION_TEST_VECTOR_CL0,
            .CL1   = SDL_PBIST2_FAIL_INSERTION_TEST_VECTOR_CL1,
            .CL2   = SDL_PBIST2_FAIL_INSERTION_TEST_VECTOR_CL2,
            .CL3   = SDL_PBIST2_FAIL_INSERTION_TEST_VECTOR_CL3,
            .CMS   = SDL_PBIST2_FAIL_INSERTION_TEST_VECTOR_CMS,
            .CSR   = SDL_PBIST2_FAIL_INSERTION_TEST_VECTOR_CSR,
            .I0    = SDL_PBIST2_FAIL_INSERTION_TEST_VECTOR_I0,
            .I1    = SDL_PBIST2_FAIL_INSERTION_TEST_VECTOR_I1,
            .I2    = SDL_PBIST2_FAIL_INSERTION_TEST_VECTOR_I2,
            .I3    = SDL_PBIST2_FAIL_INSERTION_TEST_VECTOR_I3,
            .RAMT  = SDL_PBIST2_FAIL_INSERTION_TEST_VECTOR_RAMT,
        },
        .numPBISTROMTestRuns = 1u,
        .PBISTROMTestConfigRun = 
        {
            {
                .D = 0xF412605Eu,
                .E = 0xF412605Eu,
                .CA2 = 0x7FFFu,
                .CL0 = 0x3FFu,
                .CA3 = 0x0u,
                .I0 = 0x1u,
                .CL1 = 0x1Fu,
                .I3 = 0x0u,
                .I2 = 0xEu,
                .CL2 = 0xEu,
                .CA1 = 0x7FFFu,
                .CA0 = 0x0u,
                .CL3 = 0x7FFFu,
                .I1 = 0x20u,
                .RAMT = 0x08002020u,
                .CSR = 0x00000001u,
                .CMS = 0x01u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            }
        },

        .interruptNumber        = SDLR_MCU_R5FSS0_CORE0_INTR_PBIST2_DFT_PBIST_CPU_0,
        .clecInputEvt           = 0x0u,
        .doneFlag               = PBIST_NOT_DONE
    },
    /* Main Pulsar Instance 1 */
    {
        .PBISTRegsHiAddress  = 0u,
        .pPBISTRegs          = (SDL_pbistRegs *)SDL_PBIST10_BASE, /* PBIST10: Main MCU 1 */
        .numPBISTRuns        = SDL_PBIST10_NUM_TEST_VECTORS,
        .PBISTConfigRun = {
            {
                .override           = 0x0u,
                /* Override bit set to 0 to use memoryGroupsBitMap & algorithmsBitMap */
                /* NOTE: override set to 1 is not functional currently */
                .algorithmsBitMap   = SDL_PBIST10_ALGO_BITMAP_0,  /* Choose recommended Algo bitmap */
                .memoryGroupsBitMap = SDL_PBIST10_MEM_BITMAP_0,   /* Choose recommended mem bitmap  */
                .scrambleValue      = 0xFEDCBA9876543210U,        /* Scramble Value */
            },
            {
                .override           = 0x0u,
                .algorithmsBitMap   = 0x0u,
                .memoryGroupsBitMap = 0x0u,
                .scrambleValue      = 0x0u,
            }
        },
        .PBISTNegConfigRun = {
            .CA0   = SDL_PBIST10_FAIL_INSERTION_TEST_VECTOR_CA0,
            .CA1   = SDL_PBIST10_FAIL_INSERTION_TEST_VECTOR_CA1,
            .CA2   = SDL_PBIST10_FAIL_INSERTION_TEST_VECTOR_CA2,
            .CA3   = SDL_PBIST10_FAIL_INSERTION_TEST_VECTOR_CA3,
            .CL0   = SDL_PBIST10_FAIL_INSERTION_TEST_VECTOR_CL0,
            .CL1   = SDL_PBIST10_FAIL_INSERTION_TEST_VECTOR_CL1,
            .CL2   = SDL_PBIST10_FAIL_INSERTION_TEST_VECTOR_CL2,
            .CL3   = SDL_PBIST10_FAIL_INSERTION_TEST_VECTOR_CL3,
            .CMS   = SDL_PBIST10_FAIL_INSERTION_TEST_VECTOR_CMS,
            .CSR   = SDL_PBIST10_FAIL_INSERTION_TEST_VECTOR_CSR,
            .I0    = SDL_PBIST10_FAIL_INSERTION_TEST_VECTOR_I0,
            .I1    = SDL_PBIST10_FAIL_INSERTION_TEST_VECTOR_I1,
            .I2    = SDL_PBIST10_FAIL_INSERTION_TEST_VECTOR_I2,
            .I3    = SDL_PBIST10_FAIL_INSERTION_TEST_VECTOR_I3,
            .RAMT  = SDL_PBIST10_FAIL_INSERTION_TEST_VECTOR_RAMT
        },
        .numPBISTROMTestRuns = 1u,
        .PBISTROMTestConfigRun = 
        {
            {
                .D = 0xF412605Eu,
                .E = 0xF412605Eu,
                .CA2 = 0x7FFFu,
                .CL0 = 0x3FFu,
                .CA3 = 0x0u,
                .I0 = 0x1u,
                .CL1 = 0x1Fu,
                .I3 = 0x0u,
                .I2 = 0xEu,
                .CL2 = 0xEu,
                .CA1 = 0x7FFFu,
                .CA0 = 0x0u,
                .CL3 = 0x7FFFu,
                .I1 = 0x20u,
                .RAMT = 0x08002020u,
                .CSR = 0x00000001u,
                .CMS = 0x01u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            }
        },
        .interruptNumber        = SDLR_MCU_R5FSS0_CORE0_INTR_PBIST10_DFT_PBIST_CPU_0,
        .clecInputEvt           = 0x0u,
        .doneFlag               = PBIST_NOT_DONE
    },
    /* Main Infrastructure Instance 0*/
    {
        .PBISTRegsHiAddress  = 0u,
        .pPBISTRegs          = (SDL_pbistRegs *)SDL_PBIST0_BASE,
        .numPBISTRuns        = SDL_PBIST0_NUM_TEST_VECTORS,
        .PBISTConfigRun = {
            /* Golden Vector Part 1 */
            {
                .override           = 0u,
                /* Override bit set to 0 to use memoryGroupsBitMap & algorithmsBitMap */
                .algorithmsBitMap   = SDL_PBIST0_ALGO_BITMAP_0,   /* Choose recommended Algo bitmap 0 */
                .memoryGroupsBitMap = SDL_PBIST0_MEM_BITMAP_0,    /* Choose recommended mem bitmap 0  */
                .scrambleValue      = 0xFEDCBA9876543210U, /* Scramble Value */
            },
            /* Golden Vector Part 2 */
            {
                .override           = 0u,
                /* Override bit set to 0 to use memoryGroupsBitMap & algorithmsBitMap */
                .algorithmsBitMap   = SDL_PBIST0_ALGO_BITMAP_1,   /* Choose recommended Algo bitmap 1 */
                .memoryGroupsBitMap = SDL_PBIST0_MEM_BITMAP_1,    /* Choose recommended mem bitmap 1 */
                .scrambleValue      = 0xFEDCBA9876543210U, /* Scramble Value */
            }
        },
        .PBISTNegConfigRun = {
            .CA0   = SDL_PBIST0_FAIL_INSERTION_TEST_VECTOR_CA0,
            .CA1   = SDL_PBIST0_FAIL_INSERTION_TEST_VECTOR_CA1,
            .CA2   = SDL_PBIST0_FAIL_INSERTION_TEST_VECTOR_CA2,
            .CA3   = SDL_PBIST0_FAIL_INSERTION_TEST_VECTOR_CA3,
            .CL0   = SDL_PBIST0_FAIL_INSERTION_TEST_VECTOR_CL0,
            .CL1   = SDL_PBIST0_FAIL_INSERTION_TEST_VECTOR_CL1,
            .CL2   = SDL_PBIST0_FAIL_INSERTION_TEST_VECTOR_CL2,
            .CL3   = SDL_PBIST0_FAIL_INSERTION_TEST_VECTOR_CL3,
            .CMS   = SDL_PBIST0_FAIL_INSERTION_TEST_VECTOR_CMS,
            .CSR   = SDL_PBIST0_FAIL_INSERTION_TEST_VECTOR_CSR,
            .I0    = SDL_PBIST0_FAIL_INSERTION_TEST_VECTOR_I0,
            .I1    = SDL_PBIST0_FAIL_INSERTION_TEST_VECTOR_I1,
            .I2    = SDL_PBIST0_FAIL_INSERTION_TEST_VECTOR_I2,
            .I3    = SDL_PBIST0_FAIL_INSERTION_TEST_VECTOR_I3,
            .RAMT  = SDL_PBIST0_FAIL_INSERTION_TEST_VECTOR_RAMT
        },
        .numPBISTROMTestRuns = 1u,
        .PBISTROMTestConfigRun = 
        {
            {
                .D = 0xA4E5494Fu,
                .E = 0xA4E5494Fu,
                .CA2 = 0x7FFFu,
                .CL0 = 0x3FFu,
                .CA3 = 0x0u,
                .I0 = 0x1u,
                .CL1 = 0x1Fu,
                .I3 = 0x0u,
                .I2 = 0xEu,
                .CL2 = 0xEu,
                .CA1 = 0x7FFFu,
                .CA0 = 0x0u,
                .CL3 = 0x7FFFu,
                .I1 = 0x20u,
                .RAMT = 0x14002020u,
                .CSR = 0x00000001u,
                .CMS = 0x04u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            }
        },
        .interruptNumber        = SDLR_MCU_R5FSS0_CORE0_INTR_PBIST0_DFT_PBIST_CPU_0,
        .clecInputEvt           = 0x0u,
        .doneFlag               = PBIST_NOT_DONE
    },
    /* HC Instance */
    {
        .PBISTRegsHiAddress  = 0u,
        .pPBISTRegs          = (SDL_pbistRegs *)SDL_PBIST3_BASE,
        .numPBISTRuns        = SDL_PBIST3_NUM_TEST_VECTORS,
        .PBISTConfigRun = {
            /* Golden Vector Part 1 */
            {
                .override           = 0x0u,
                /* Override bit set to 0 to use memoryGroupsBitMap & algorithmsBitMap */
                .algorithmsBitMap   = SDL_PBIST3_ALGO_BITMAP_0,   /* Choose recommended Algo bitmap 0 */
                .memoryGroupsBitMap = SDL_PBIST3_MEM_BITMAP_0,    /* Choose recommended mem bitmap 0  */
                .scrambleValue      = 0xFEDCBA9876543210U,  /* Scramble Value */
            },
            /* Golden Vector Part 2 */
            {
                .override           = 0x0u,
                /* Override bit set to 0 to use memoryGroupsBitMap & algorithmsBitMap */
                .algorithmsBitMap   = SDL_PBIST3_ALGO_BITMAP_1,   /* Choose recommended Algo bitmap 1 */
                .memoryGroupsBitMap = SDL_PBIST3_MEM_BITMAP_1,    /* Choose recommended mem bitmap 1 */
                .scrambleValue      = 0xFEDCBA9876543210U,  /* Scramble Value */
            }
        },
        .PBISTNegConfigRun = {
            .CA0   = SDL_PBIST3_FAIL_INSERTION_TEST_VECTOR_CA0,
            .CA1   = SDL_PBIST3_FAIL_INSERTION_TEST_VECTOR_CA1,
            .CA2   = SDL_PBIST3_FAIL_INSERTION_TEST_VECTOR_CA2,
            .CA3   = SDL_PBIST3_FAIL_INSERTION_TEST_VECTOR_CA3,
            .CL0   = SDL_PBIST3_FAIL_INSERTION_TEST_VECTOR_CL0,
            .CL1   = SDL_PBIST3_FAIL_INSERTION_TEST_VECTOR_CL1,
            .CL2   = SDL_PBIST3_FAIL_INSERTION_TEST_VECTOR_CL2,
            .CL3   = SDL_PBIST3_FAIL_INSERTION_TEST_VECTOR_CL3,
            .CMS   = SDL_PBIST3_FAIL_INSERTION_TEST_VECTOR_CMS,
            .CSR   = SDL_PBIST3_FAIL_INSERTION_TEST_VECTOR_CSR,
            .I0    = SDL_PBIST3_FAIL_INSERTION_TEST_VECTOR_I0,
            .I1    = SDL_PBIST3_FAIL_INSERTION_TEST_VECTOR_I1,
            .I2    = SDL_PBIST3_FAIL_INSERTION_TEST_VECTOR_I2,
            .I3    = SDL_PBIST3_FAIL_INSERTION_TEST_VECTOR_I3,
            .RAMT  = SDL_PBIST3_FAIL_INSERTION_TEST_VECTOR_RAMT
        },
        /*This instance is not supported for PBIST check of ROM*/
        .numPBISTROMTestRuns = 0u,
        .PBISTROMTestConfigRun = 
        { 
            {
                .D = 0xBFE16A0Fu,
                .E = 0xBFE16A0Fu,
                .CA2 = 0x7FFFu,
                .CL0 = 0x3FFu,
                .CA3 = 0x0u,
                .I0 = 0x1u,
                .CL1 = 0x1Fu,
                .I3 = 0x0u,
                .I2 = 0xEu,
                .CL2 = 0xEu,
                .CA1 = 0x7FFFu,
                .CA0 = 0x0u,
                .CL3 = 0x7FFFu,
                .I1 = 0x20u,
                .RAMT = 0x4C002020u,
                .CSR = 0x00000001u,
                .CMS = 0x05u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            }
        },
        .interruptNumber        = SDLR_MCU_R5FSS0_CORE0_INTR_PBIST3_DFT_PBIST_CPU_0,
        .clecInputEvt           = 0x0u,
        .doneFlag               = PBIST_NOT_DONE
    },
    /* NAVSS Instance */
    {
        .PBISTRegsHiAddress  = 0u,
        .pPBISTRegs          = (SDL_pbistRegs *)SDL_PBIST4_BASE,
        .numPBISTRuns        = SDL_PBIST4_NUM_TEST_VECTORS,
        .PBISTConfigRun = {
            /* Golden Vector Part 1 */
            {
                .override           = 0x0u,
                /* Override bit set to 0 to use memoryGroupsBitMap & algorithmsBitMap */
                .algorithmsBitMap   = SDL_PBIST4_ALGO_BITMAP_0,   /* Choose recommended Algo bitmap 0 */
                .memoryGroupsBitMap = SDL_PBIST4_MEM_BITMAP_0,    /* Choose recommended mem bitmap 0  */
                .scrambleValue      = 0xFEDCBA9876543210U,  /* Scramble Value */
            },
            /* Golden Vector Part 2 */
            {
                .override           = 0x0u,
                /* Override bit set to 0 to use memoryGroupsBitMap & algorithmsBitMap */
                .algorithmsBitMap   = SDL_PBIST4_ALGO_BITMAP_1,   /* Choose recommended Algo bitmap 1 */
                .memoryGroupsBitMap = SDL_PBIST4_MEM_BITMAP_1,    /* Choose recommended mem bitmap 1  */
                .scrambleValue      = 0xFEDCBA9876543210U,  /* Scramble Value */
            }
        },
        .PBISTNegConfigRun = {
            .CA0   = SDL_PBIST4_FAIL_INSERTION_TEST_VECTOR_CA0,
            .CA1   = SDL_PBIST4_FAIL_INSERTION_TEST_VECTOR_CA1,
            .CA2   = SDL_PBIST4_FAIL_INSERTION_TEST_VECTOR_CA2,
            .CA3   = SDL_PBIST4_FAIL_INSERTION_TEST_VECTOR_CA3,
            .CL0   = SDL_PBIST4_FAIL_INSERTION_TEST_VECTOR_CL0,
            .CL1   = SDL_PBIST4_FAIL_INSERTION_TEST_VECTOR_CL1,
            .CL2   = SDL_PBIST4_FAIL_INSERTION_TEST_VECTOR_CL2,
            .CL3   = SDL_PBIST4_FAIL_INSERTION_TEST_VECTOR_CL3,
            .CMS   = SDL_PBIST4_FAIL_INSERTION_TEST_VECTOR_CMS,
            .CSR   = SDL_PBIST4_FAIL_INSERTION_TEST_VECTOR_CSR,
            .I0    = SDL_PBIST4_FAIL_INSERTION_TEST_VECTOR_I0,
            .I1    = SDL_PBIST4_FAIL_INSERTION_TEST_VECTOR_I1,
            .I2    = SDL_PBIST4_FAIL_INSERTION_TEST_VECTOR_I2,
            .I3    = SDL_PBIST4_FAIL_INSERTION_TEST_VECTOR_I3,
            .RAMT  = SDL_PBIST4_FAIL_INSERTION_TEST_VECTOR_RAMT
        },
        .numPBISTROMTestRuns = 1u,
        .PBISTROMTestConfigRun = 
        {
            {
                .D = 0xEDE472EBu,
                .E = 0xEDE472EBu,
                .CA2 = 0x7FFFu,
                .CL0 = 0x3FFu,
                .CA3 = 0x0u,
                .I0 = 0x1u,
                .CL1 = 0x1Fu,
                .I3 = 0x0u,
                .I2 = 0xEu,
                .CL2 = 0xEu,
                .CA1 = 0x7FFFu,
                .CA0 = 0x0u,
                .CL3 = 0x7FFFu,
                .I1 = 0x20u,
                .RAMT = 0x50002020u,
                .CSR = 0x00000001u,
                .CMS = 0x01u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            }
        },
        .interruptNumber        = SDLR_MCU_R5FSS0_CORE0_INTR_PBIST4_DFT_PBIST_CPU_0,
        .clecInputEvt           = 0x0u,
        .doneFlag               = PBIST_NOT_DONE
    },
   /* Codec 1 Instance */
    {
        .PBISTRegsHiAddress  = 0u,
        .pPBISTRegs          = (SDL_pbistRegs *)SDL_PBIST11_BASE,
        .numPBISTRuns        = SDL_PBIST11_NUM_TEST_VECTORS,
        .PBISTConfigRun = {
            {
                .override           = 0x0u,
                /* Override bit set to 0 to use memoryGroupsBitMap & algorithmsBitMap */
                .algorithmsBitMap   = SDL_PBIST11_ALGO_BITMAP_0,   /* Choose recommended Algo bitmap 0 */
                .memoryGroupsBitMap = SDL_PBIST11_MEM_BITMAP_0,    /* Choose recommended mem bitmap 0  */
                .scrambleValue      = 0xFEDCBA9876543210U, /* Scramble Value */
            },
            {
                .override           = 0x0u,
                /* Override bit set to 0 to use memoryGroupsBitMap & algorithmsBitMap */
                .algorithmsBitMap   = SDL_PBIST11_ALGO_BITMAP_1,   /* Choose recommended Algo bitmap 1 */
                .memoryGroupsBitMap = SDL_PBIST11_MEM_BITMAP_1,    /* Choose recommended mem bitmap 1  */
                .scrambleValue      = 0xFEDCBA9876543210U, /* Scramble Value */
            },

        },
        .PBISTNegConfigRun = {
            .CA0   = SDL_PBIST11_FAIL_INSERTION_TEST_VECTOR_CA0,
            .CA1   = SDL_PBIST11_FAIL_INSERTION_TEST_VECTOR_CA1,
            .CA2   = SDL_PBIST11_FAIL_INSERTION_TEST_VECTOR_CA2,
            .CA3   = SDL_PBIST11_FAIL_INSERTION_TEST_VECTOR_CA3,
            .CL0   = SDL_PBIST11_FAIL_INSERTION_TEST_VECTOR_CL0,
            .CL1   = SDL_PBIST11_FAIL_INSERTION_TEST_VECTOR_CL1,
            .CL2   = SDL_PBIST11_FAIL_INSERTION_TEST_VECTOR_CL2,
            .CL3   = SDL_PBIST11_FAIL_INSERTION_TEST_VECTOR_CL3,
            .CMS   = SDL_PBIST11_FAIL_INSERTION_TEST_VECTOR_CMS,
            .CSR   = SDL_PBIST11_FAIL_INSERTION_TEST_VECTOR_CSR,
            .I0    = SDL_PBIST11_FAIL_INSERTION_TEST_VECTOR_I0,
            .I1    = SDL_PBIST11_FAIL_INSERTION_TEST_VECTOR_I1,
            .I2    = SDL_PBIST11_FAIL_INSERTION_TEST_VECTOR_I2,
            .I3    = SDL_PBIST11_FAIL_INSERTION_TEST_VECTOR_I3,
            .RAMT  = SDL_PBIST11_FAIL_INSERTION_TEST_VECTOR_RAMT
        },
        .numPBISTROMTestRuns = 1u,
        .PBISTROMTestConfigRun = 
        {
            {
                .D = 0x0412F4CBu,
                .E = 0x0412F4CBu,
                .CA2 = 0x7FFFu,
                .CL0 = 0x3FFu,
                .CA3 = 0x0u,
                .I0 = 0x1u,
                .CL1 = 0x1Fu,
                .I3 = 0x0u,
                .I2 = 0xEu,
                .CL2 = 0xEu,
                .CA1 = 0x7FFFu,
                .CA0 = 0x0u,
                .CL3 = 0x7FFFu,
                .I1 = 0x20u,
                .RAMT = 0x40002020u,
                .CSR = 0x00000001u,
                .CMS = 0x02u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            }
        },
        .interruptNumber        = SDLR_MCU_R5FSS0_CORE0_INTR_PBIST11_DFT_PBIST_CPU_0,
        .clecInputEvt           = 0x0u,
        .doneFlag               = PBIST_NOT_DONE
    },
    /* GPU Instance */
    {
        .PBISTRegsHiAddress  = 0u,
        .pPBISTRegs          = (SDL_pbistRegs *)SDL_AEP_GPU_BXS464_WRAP0_MEM_BASE,
        .numPBISTRuns        = SDL_AEP_GPU_BXS464_WRAP0_DFT_EMBED_PBIST_0_NUM_TEST_VECTORS,
        .PBISTConfigRun = {
            {
                .override           = 0x0u,
                /* Override bit set to 0 to use memoryGroupsBitMap & algorithmsBitMap */
                .algorithmsBitMap   = SDL_AEP_GPU_BXS464_WRAP0_DFT_EMBED_PBIST_0_ALGO_BITMAP_0,   /* Choose recommended Algo bitmap 0 */
                .memoryGroupsBitMap = SDL_AEP_GPU_BXS464_WRAP0_DFT_EMBED_PBIST_0_MEM_BITMAP_0,    /* Choose recommended mem bitmap 0  */
                .scrambleValue      = 0xFEDCBA9876543210U, /* Scramble Value        */
            },
            {
                .override           = 0x0u,
                /* Override bit set to 0 to use memoryGroupsBitMap & algorithmsBitMap */
                .algorithmsBitMap   = SDL_AEP_GPU_BXS464_WRAP0_DFT_EMBED_PBIST_0_ALGO_BITMAP_1,   /* Choose recommended Algo bitmap 1 */
                .memoryGroupsBitMap = SDL_AEP_GPU_BXS464_WRAP0_DFT_EMBED_PBIST_0_MEM_BITMAP_1,    /* Choose recommended mem bitmap 1  */
                .scrambleValue      = 0xFEDCBA9876543210U, /* Scramble Value          */
            },

        },
        .PBISTNegConfigRun = {
            .CA0   = SDL_AEP_GPU_BXS464_WRAP0_DFT_EMBED_PBIST_0_FAIL_INSERTION_TEST_VECTOR_CA0,
            .CA1   = SDL_AEP_GPU_BXS464_WRAP0_DFT_EMBED_PBIST_0_FAIL_INSERTION_TEST_VECTOR_CA1,
            .CA2   = SDL_AEP_GPU_BXS464_WRAP0_DFT_EMBED_PBIST_0_FAIL_INSERTION_TEST_VECTOR_CA2,
            .CA3   = SDL_AEP_GPU_BXS464_WRAP0_DFT_EMBED_PBIST_0_FAIL_INSERTION_TEST_VECTOR_CA3,
            .CL0   = SDL_AEP_GPU_BXS464_WRAP0_DFT_EMBED_PBIST_0_FAIL_INSERTION_TEST_VECTOR_CL0,
            .CL1   = SDL_AEP_GPU_BXS464_WRAP0_DFT_EMBED_PBIST_0_FAIL_INSERTION_TEST_VECTOR_CL1,
            .CL2   = SDL_AEP_GPU_BXS464_WRAP0_DFT_EMBED_PBIST_0_FAIL_INSERTION_TEST_VECTOR_CL2,
            .CL3   = SDL_AEP_GPU_BXS464_WRAP0_DFT_EMBED_PBIST_0_FAIL_INSERTION_TEST_VECTOR_CL3,
            .CMS   = SDL_AEP_GPU_BXS464_WRAP0_DFT_EMBED_PBIST_0_FAIL_INSERTION_TEST_VECTOR_CMS,
            .CSR   = SDL_AEP_GPU_BXS464_WRAP0_DFT_EMBED_PBIST_0_FAIL_INSERTION_TEST_VECTOR_CSR,
            .I0    = SDL_AEP_GPU_BXS464_WRAP0_DFT_EMBED_PBIST_0_FAIL_INSERTION_TEST_VECTOR_I0,
            .I1    = SDL_AEP_GPU_BXS464_WRAP0_DFT_EMBED_PBIST_0_FAIL_INSERTION_TEST_VECTOR_I1,
            .I2    = SDL_AEP_GPU_BXS464_WRAP0_DFT_EMBED_PBIST_0_FAIL_INSERTION_TEST_VECTOR_I2,
            .I3    = SDL_AEP_GPU_BXS464_WRAP0_DFT_EMBED_PBIST_0_FAIL_INSERTION_TEST_VECTOR_I3,
            .RAMT  = SDL_AEP_GPU_BXS464_WRAP0_DFT_EMBED_PBIST_0_FAIL_INSERTION_TEST_VECTOR_RAMT
        },
        .numPBISTROMTestRuns = 1u,
        .PBISTROMTestConfigRun = 
        {
            {
                .D = 0x875F0724u,
                .E = 0x875F0724u,
                .CA2 = 0x7FFFu,
                .CL0 = 0x3FFu,
                .CA3 = 0x0u,
                .I0 = 0x1u,
                .CL1 = 0x1Fu,
                .I3 = 0x0u,
                .I2 = 0xEu,
                .CL2 = 0xEu,
                .CA1 = 0x7FFFu,
                .CA0 = 0x0u,
                .CL3 = 0x7FFFu,
                .I1 = 0x20u,
                .RAMT = 0x60002020u,
                .CSR = 0x00000001u,
                .CMS = 0x01u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            }
        },
        .interruptNumber        = SDLR_MCU_R5FSS0_CORE0_INTR_AEP_GPU_BXS464_WRAP0_DFT_EMBED_PBIST_0_DFT_PBIST_CPU_0,
        .clecInputEvt           = 0x0u,
        .doneFlag               = PBIST_NOT_DONE
    },
	/* C7X_0 Instance */
    {	
	    .PBISTRegsHiAddress  = SDL_COMPUTE_CLUSTER0_DSP0_PBIST_BASE,
        .pPBISTRegs          = (SDL_pbistRegs *)NULL,
        .numPBISTRuns        = SDL_COMPUTE_CLUSTER0_C71SS0_PBIST_WRAP_0_NUM_TEST_VECTORS,
        .PBISTConfigRun = {
            {
                .override           = 0x0u,
                /* Override bit set to 0 to use memoryGroupsBitMap & algorithmsBitMap */
                .algorithmsBitMap   = SDL_COMPUTE_CLUSTER0_C71SS0_PBIST_WRAP_0_ALGO_BITMAP_0,   /* Choose recommended Algo bitmap */
                .memoryGroupsBitMap = SDL_COMPUTE_CLUSTER0_C71SS0_PBIST_WRAP_0_MEM_BITMAP_0,    /* Choose recommended mem bitmap  */
                .scrambleValue      = 0xFEDCBA9876543210U,                  /* Scramble Value */
            },
            {
                .override           = 0x0u,
                /* Override bit set to 0 to use memoryGroupsBitMap & algorithmsBitMap */
                .algorithmsBitMap   = 0x0u,  /* Choose recommended Algo bitmap */
                .memoryGroupsBitMap = 0x0u,   /* Choose recommended mem bitmap  */
                .scrambleValue      = 0x0u, /* Scramble Value */
            }
        },
        .PBISTNegConfigRun = {
            .CA0   = SDL_COMPUTE_CLUSTER0_C71SS0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CA0,
            .CA1   = SDL_COMPUTE_CLUSTER0_C71SS0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CA1,
            .CA2   = SDL_COMPUTE_CLUSTER0_C71SS0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CA2,
            .CA3   = SDL_COMPUTE_CLUSTER0_C71SS0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CA3,
            .CL0   = SDL_COMPUTE_CLUSTER0_C71SS0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CL0,
            .CL1   = SDL_COMPUTE_CLUSTER0_C71SS0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CL1,
            .CL2   = SDL_COMPUTE_CLUSTER0_C71SS0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CL2,
            .CL3   = SDL_COMPUTE_CLUSTER0_C71SS0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CL3,
            .CMS   = SDL_COMPUTE_CLUSTER0_C71SS0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CMS,
            .CSR   = SDL_COMPUTE_CLUSTER0_C71SS0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CSR,
            .I0    = SDL_COMPUTE_CLUSTER0_C71SS0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_I0,
            .I1    = SDL_COMPUTE_CLUSTER0_C71SS0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_I1,
            .I2    = SDL_COMPUTE_CLUSTER0_C71SS0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_I2,
            .I3    = SDL_COMPUTE_CLUSTER0_C71SS0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_I3,
            .RAMT  = SDL_COMPUTE_CLUSTER0_C71SS0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_RAMT
        }, 
        /*This instance is not supported for PBIST check of ROM*/
        .numPBISTROMTestRuns = 0u,
        .PBISTROMTestConfigRun = 
        {
            {
                .D = 0x070FDDAFu,
                .E = 0x070FDDAFu,
                .CA2 = 0x3FFFu,
                .CL0 = 0x3FFu,
                .CA3 = 0x0u,
                .I0 = 0x1u,
                .CL1 = 0xFu,
                .I3 = 0x0u,
                .I2 = 0xDu,
                .CL2 = 0xDu,
                .CA1 = 0x3FFFu,
                .CA0 = 0x0u,
                .CL3 = 0x3FFFu,
                .I1 = 0x10u,
                .RAMT = 0xFF002010u,
                .CSR = 0x00000001u,
                .CMS = 0x01u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            }
        },
        .interruptNumber        = 0x0u,
        .clecInputEvt           = SDLR_COMPUTE_CLUSTER0_CLEC_MSMC_EVENT_IN_COMPUTE_CLUSTER0_CORE_CORE_MSMC_INTR_12,
        .doneFlag               = PBIST_NOT_DONE
    },
	/* C7X_1 Instance */
	{    
        .PBISTRegsHiAddress  = SDL_COMPUTE_CLUSTER0_DSP1_PBIST_BASE,
        .pPBISTRegs          = (SDL_pbistRegs *)NULL,
        .numPBISTRuns        = SDL_COMPUTE_CLUSTER0_C71SS1_PBIST_WRAP_0_NUM_TEST_VECTORS,
        .PBISTConfigRun = {
            {
                .override           = 0x0u,
                /* Override bit set to 0 to use memoryGroupsBitMap & algorithmsBitMap */
                .algorithmsBitMap   = SDL_COMPUTE_CLUSTER0_C71SS1_PBIST_WRAP_0_ALGO_BITMAP_0,   /* Choose recommended Algo bitmap */
                .memoryGroupsBitMap = SDL_COMPUTE_CLUSTER0_C71SS1_PBIST_WRAP_0_MEM_BITMAP_0,    /* Choose recommended mem bitmap  */
                .scrambleValue      = 0xFEDCBA9876543210U,                  /* Scramble Value */
            },
            {
                .override           = 0x0u,
                /* Override bit set to 0 to use memoryGroupsBitMap & algorithmsBitMap */
                .algorithmsBitMap   = 0x0u,  /* Choose recommended Algo bitmap */
                .memoryGroupsBitMap = 0x0u,   /* Choose recommended mem bitmap  */
                .scrambleValue      = 0x0u, /* Scramble Value */
            }
        },
        .PBISTNegConfigRun = {
            .CA0   = SDL_COMPUTE_CLUSTER0_C71SS1_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CA0,
            .CA1   = SDL_COMPUTE_CLUSTER0_C71SS1_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CA1,
            .CA2   = SDL_COMPUTE_CLUSTER0_C71SS1_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CA2,
            .CA3   = SDL_COMPUTE_CLUSTER0_C71SS1_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CA3,
            .CL0   = SDL_COMPUTE_CLUSTER0_C71SS1_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CL0,
            .CL1   = SDL_COMPUTE_CLUSTER0_C71SS1_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CL1,
            .CL2   = SDL_COMPUTE_CLUSTER0_C71SS1_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CL2,
            .CL3   = SDL_COMPUTE_CLUSTER0_C71SS1_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CL3,
            .CMS   = SDL_COMPUTE_CLUSTER0_C71SS1_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CMS,
            .CSR   = SDL_COMPUTE_CLUSTER0_C71SS1_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CSR,
            .I0    = SDL_COMPUTE_CLUSTER0_C71SS1_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_I0,
            .I1    = SDL_COMPUTE_CLUSTER0_C71SS1_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_I1,
            .I2    = SDL_COMPUTE_CLUSTER0_C71SS1_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_I2,
            .I3    = SDL_COMPUTE_CLUSTER0_C71SS1_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_I3,
            .RAMT  = SDL_COMPUTE_CLUSTER0_C71SS1_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_RAMT
        },	
        /*This instance is not supported for PBIST check of ROM*/
        .numPBISTROMTestRuns = 0u,
        .PBISTROMTestConfigRun = 
        {
            {
                .D = 0x070FDDAFu,
                .E = 0x070FDDAFu,
                .CA2 = 0x3FFFu,
                .CL0 = 0x3FFu,
                .CA3 = 0x0u,
                .I0 = 0x1u,
                .CL1 = 0xFu,
                .I3 = 0x0u,
                .I2 = 0xDu,
                .CL2 = 0xDu,
                .CA1 = 0x3FFFu,
                .CA0 = 0x0u,
                .CL3 = 0x3FFFu,
                .I1 = 0x10u,
                .RAMT = 0xFF002010u,
                .CSR = 0x00000001u,
                .CMS = 0x01u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            }
        },
		.interruptNumber        = 0x0u,
        .clecInputEvt           = SDLR_COMPUTE_CLUSTER0_CLEC_MSMC_EVENT_IN_COMPUTE_CLUSTER0_CORE_CORE_MSMC_INTR_13,
        .doneFlag               = PBIST_NOT_DONE
    },		
	/* A72 */
    {
		.PBISTRegsHiAddress  = SDL_COMPUTE_CLUSTER0_MPU_PBIST0_BASE,
        .pPBISTRegs          = (SDL_pbistRegs *)NULL,
        .numPBISTRuns        = SDL_A72SS0_CORE0_PBIST_WRAP_NUM_TEST_VECTORS,
        .PBISTConfigRun = {
            {
                .override           = 0x0u,
                /* Override bit set to 0 to use memoryGroupsBitMap & algorithmsBitMap */
                .algorithmsBitMap   = SDL_A72SS0_CORE0_PBIST_WRAP_ALGO_BITMAP_0,  /* Choose recommended Algo bitmap */
                .memoryGroupsBitMap = SDL_A72SS0_CORE0_PBIST_WRAP_MEM_BITMAP_0,   /* Choose recommended mem bitmap  */
                .scrambleValue      = 0xFEDCBA9876543210U, /* Scramble Value */
            },
            {
                .override           = 0x0u,
                /* Override bit set to 0 to use memoryGroupsBitMap & algorithmsBitMap */
                .algorithmsBitMap   = 0x0u,  /* Choose recommended Algo bitmap */
                .memoryGroupsBitMap = 0x0u,   /* Choose recommended mem bitmap  */
                .scrambleValue      = 0x0u, /* Scramble Value */
            }
        },
        .PBISTNegConfigRun = {
            .CA0   = SDL_A72SS0_CORE0_PBIST_WRAP_FAIL_INSERTION_TEST_VECTOR_CA0,
            .CA1   = SDL_A72SS0_CORE0_PBIST_WRAP_FAIL_INSERTION_TEST_VECTOR_CA1,
            .CA2   = SDL_A72SS0_CORE0_PBIST_WRAP_FAIL_INSERTION_TEST_VECTOR_CA2,
            .CA3   = SDL_A72SS0_CORE0_PBIST_WRAP_FAIL_INSERTION_TEST_VECTOR_CA3,
            .CL0   = SDL_A72SS0_CORE0_PBIST_WRAP_FAIL_INSERTION_TEST_VECTOR_CL0,
            .CL1   = SDL_A72SS0_CORE0_PBIST_WRAP_FAIL_INSERTION_TEST_VECTOR_CL1,
            .CL2   = SDL_A72SS0_CORE0_PBIST_WRAP_FAIL_INSERTION_TEST_VECTOR_CL2,
            .CL3   = SDL_A72SS0_CORE0_PBIST_WRAP_FAIL_INSERTION_TEST_VECTOR_CL3,
            .CMS   = SDL_A72SS0_CORE0_PBIST_WRAP_FAIL_INSERTION_TEST_VECTOR_CMS,
            .CSR   = SDL_A72SS0_CORE0_PBIST_WRAP_FAIL_INSERTION_TEST_VECTOR_CSR,
            .I0    = SDL_A72SS0_CORE0_PBIST_WRAP_FAIL_INSERTION_TEST_VECTOR_I0,
            .I1    = SDL_A72SS0_CORE0_PBIST_WRAP_FAIL_INSERTION_TEST_VECTOR_I1,
            .I2    = SDL_A72SS0_CORE0_PBIST_WRAP_FAIL_INSERTION_TEST_VECTOR_I2,
            .I3    = SDL_A72SS0_CORE0_PBIST_WRAP_FAIL_INSERTION_TEST_VECTOR_I3,
            .RAMT  = SDL_A72SS0_CORE0_PBIST_WRAP_FAIL_INSERTION_TEST_VECTOR_RAMT	
        },
        /*This instance is not supported for PBIST check of ROM*/
        .numPBISTROMTestRuns = 0u,
        .PBISTROMTestConfigRun = 
        {
            {
                .D = 0xAA375C33u,
                .E = 0xAA375C33u,
                .CA2 = 0x3FFFu,
                .CL0 = 0x3FFu,
                .CA3 = 0x0u,
                .I0 = 0x1u,
                .CL1 = 0xFu,
                .I3 = 0x0u,
                .I2 = 0xDu,
                .CL2 = 0xDu,
                .CA1 = 0x3FFFu,
                .CA0 = 0x0u,
                .CL3 = 0x3FFFu,
                .I1 = 0x10u,
                .RAMT = 0xFF002010u,
                .CSR = 0x00000001u,
                .CMS = 0x01u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            }
        },
		.interruptNumber        = 0x0u, 
        .clecInputEvt           = SDLR_COMPUTE_CLUSTER0_CLEC_MSMC_EVENT_IN_COMPUTE_CLUSTER0_CORE_CORE_MSMC_INTR_8,
        .doneFlag               = PBIST_NOT_DONE
    },
	/* MSMC */
    {
		.PBISTRegsHiAddress  = SDL_COMPUTE_CLUSTER0_MSMC_PBIST0_BASE,
        .pPBISTRegs          = (SDL_pbistRegs *)NULL,
        .numPBISTRuns        = SDL_COMPUTE_CLUSTER0_PBIST_WRAP_0_NUM_TEST_VECTORS,
        .PBISTConfigRun = {
            /* Golden Vector Part 1 */
            {
                .override           = 0x0u,
                /* Override bit set to 0 to use memoryGroupsBitMap & algorithmsBitMap */
                .algorithmsBitMap   = SDL_COMPUTE_CLUSTER0_PBIST_WRAP_0_ALGO_BITMAP_0,   /* Choose recommended Algo bitmap 0 */
                .memoryGroupsBitMap = SDL_COMPUTE_CLUSTER0_PBIST_WRAP_0_MEM_BITMAP_0,    /* Choose recommended mem bitmap 0  */
                .scrambleValue      = 0xFEDCBA9876543210U, /* Scramble Value */
            },
            /* Golden Vector Part 2 */
            {
                .override           = 0x0u,
                /* Override bit set to 0 to use memoryGroupsBitMap & algorithmsBitMap */
                .algorithmsBitMap   = SDL_COMPUTE_CLUSTER0_PBIST_WRAP_0_ALGO_BITMAP_1,   /* Choose recommended Algo bitmap 1 */
                .memoryGroupsBitMap = SDL_COMPUTE_CLUSTER0_PBIST_WRAP_0_MEM_BITMAP_1,    /* Choose recommended mem bitmap 1  */
                .scrambleValue      = 0xFEDCBA9876543210U, /* Scramble Value */
            } 
        },
        .PBISTNegConfigRun = {
            .CA0   = SDL_COMPUTE_CLUSTER0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CA0,
            .CA1   = SDL_COMPUTE_CLUSTER0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CA1,
            .CA2   = SDL_COMPUTE_CLUSTER0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CA2,
            .CA3   = SDL_COMPUTE_CLUSTER0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CA3,
            .CL0   = SDL_COMPUTE_CLUSTER0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CL0,
            .CL1   = SDL_COMPUTE_CLUSTER0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CL1,
            .CL2   = SDL_COMPUTE_CLUSTER0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CL2,
            .CL3   = SDL_COMPUTE_CLUSTER0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CL3,
            .CMS   = SDL_COMPUTE_CLUSTER0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CMS,
            .CSR   = SDL_COMPUTE_CLUSTER0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CSR,
            .I0    = SDL_COMPUTE_CLUSTER0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_I0,
            .I1    = SDL_COMPUTE_CLUSTER0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_I1,
            .I2    = SDL_COMPUTE_CLUSTER0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_I2,
            .I3    = SDL_COMPUTE_CLUSTER0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_I3,
            .RAMT  = SDL_COMPUTE_CLUSTER0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_RAMT
        },
        .numPBISTROMTestRuns = 1u,
        .PBISTROMTestConfigRun = 
        {
            {
                .D = 0x4DFF8732u,
                .E = 0x4DFF8732u,
                .CA2 = 0x3FFFu,
                .CL0 = 0x3FFu,
                .CA3 = 0x0u,
                .I0 = 0x1u,
                .CL1 = 0xFu,
                .I3 = 0x0u,
                .I2 = 0xDu,
                .CL2 = 0xDu,
                .CA1 = 0x3FFFu,
                .CA0 = 0x0u,
                .CL3 = 0x3FFFu,
                .I1 = 0x10u,
                .RAMT = 0xFF002010u,
                .CSR = 0x00000001u,
                .CMS = 0x02u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            }
        },
        .interruptNumber        = SDLR_MCU_R5FSS0_CORE0_INTR_COMPUTE_CLUSTER0_PBIST_WRAP_0_DFT_PBIST_CPU_0,
        .clecInputEvt           = 0x0u,
        .doneFlag               = PBIST_NOT_DONE
    },	
    /* MCU Pulsar Instance */
    {
        .PBISTRegsHiAddress  = 0u,
        .pPBISTRegs          = (SDL_pbistRegs *)SDL_MCU_PBIST2_BASE,
#ifdef PBIST_MCU_SELECTED_MEMORY
        .numPBISTRuns        = 1u,
        .PBISTConfigRun = {
            {
                .override           = 0x0u,
                /* Override bit set to 0 to use memoryGroupsBitMap & algorithmsBitMap */
                /* NOTE: As the MCU R5f is running this code override bit cannot be set to enable all memory test */
                .algorithmsBitMap   = 0x00002000u,          /* Choose Algorithm 14 */
                .memoryGroupsBitMap = 0x0080000000000000u,  /* Choose ADC RAM */
                .scrambleValue      = 0xFEDCBA9876543210U,  /* Scramble Value */
            },
            {
                .override           = 0x0u,
                /* Override bit set to 0 to use memoryGroupsBitMap & algorithmsBitMap */
                .algorithmsBitMap   = 0x0u,  /* Choose recommended Algo bitmap */
                .memoryGroupsBitMap = 0x0u,   /* Choose recommended mem bitmap  */
                .scrambleValue      = 0x0u, /* Scramble Value */
            }
        },
   
#else		
        .numPBISTRuns        = SDL_MCU_PBIST2_NUM_TEST_VECTORS,
        .PBISTConfigRun = {	
            {
                .override           = 0x0u,
                /* Override bit set to 0 to use memoryGroupsBitMap & algorithmsBitMap */
                .algorithmsBitMap   = SDL_MCU_PBIST2_ALGO_BITMAP_0,   /* Choose recommended Algo bitmap */
                /* NOTE: As the MCU R5f is running this code override bit cannot be set to enable all memory test */
                .memoryGroupsBitMap = SDL_MCU_PBIST2_MEM_BITMAP_0,    /* Choose recommended mem bitmap  */
                .scrambleValue      = 0xFEDCBA9876543210U,  /* Scramble Value       */
            },
            {
                .override           = 0x0u,
                .algorithmsBitMap   = 0x0u,
                .memoryGroupsBitMap = 0x0u,
                .scrambleValue      = 0x0u,
            }
        },
#endif
        .PBISTNegConfigRun = {
            .CA0   = SDL_MCU_PBIST2_FAIL_INSERTION_TEST_VECTOR_CA0,
            .CA1   = SDL_MCU_PBIST2_FAIL_INSERTION_TEST_VECTOR_CA1,
            .CA2   = SDL_MCU_PBIST2_FAIL_INSERTION_TEST_VECTOR_CA2,
            .CA3   = SDL_MCU_PBIST2_FAIL_INSERTION_TEST_VECTOR_CA3,
            .CL0   = SDL_MCU_PBIST2_FAIL_INSERTION_TEST_VECTOR_CL0,
            .CL1   = SDL_MCU_PBIST2_FAIL_INSERTION_TEST_VECTOR_CL1,
            .CL2   = SDL_MCU_PBIST2_FAIL_INSERTION_TEST_VECTOR_CL2,
            .CL3   = SDL_MCU_PBIST2_FAIL_INSERTION_TEST_VECTOR_CL3,
            .CMS   = SDL_MCU_PBIST2_FAIL_INSERTION_TEST_VECTOR_CMS,
            .CSR   = SDL_MCU_PBIST2_FAIL_INSERTION_TEST_VECTOR_CSR,
            .I0    = SDL_MCU_PBIST2_FAIL_INSERTION_TEST_VECTOR_I0,
            .I1    = SDL_MCU_PBIST2_FAIL_INSERTION_TEST_VECTOR_I1,
            .I2    = SDL_MCU_PBIST2_FAIL_INSERTION_TEST_VECTOR_I2,
            .I3    = SDL_MCU_PBIST2_FAIL_INSERTION_TEST_VECTOR_I3,
            .RAMT  = SDL_MCU_PBIST2_FAIL_INSERTION_TEST_VECTOR_RAMT
        },
        .numPBISTROMTestRuns = 1u,
        .PBISTROMTestConfigRun = 
        {
            {
                .D = 0xCAC5A694u,
                .E = 0xCAC5A694u,
                .CA2 = 0x7FFFu,
                .CL0 = 0x3FFu,
                .CA3 = 0x0u,
                .I0 = 0x1u,
                .CL1 = 0x1Fu,
                .I3 = 0x0u,
                .I2 = 0xEu,
                .CL2 = 0xEu,
                .CA1 = 0x7FFFu,
                .CA0 = 0x0u,
                .CL3 = 0x7FFFu,
                .I1 = 0x20u,
                .RAMT = 0x08002020u,
                .CSR = 0x00000001u,
                .CMS = 0x01u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            }
        },
        .interruptNumber        = SDLR_MCU_R5FSS0_CORE0_INTR_MCU_PBIST2_DFT_PBIST_CPU_0,
        .clecInputEvt           = 0x0u,
        .doneFlag               = PBIST_NOT_DONE
    },
    /* MCU_1 Instance */
    {
        .PBISTRegsHiAddress  = 0u,
        .pPBISTRegs          = (SDL_pbistRegs *)SDL_MCU_PBIST1_BASE,
        /*
         * Golden vectors not currently supported running from MCU core because
         * this will be self destructive. Below select memory which is guaranteed
         * not to be utilized can be run.
         */
        .numPBISTRuns        = 1u,
        .PBISTConfigRun = {
            {
                .override           = 0x0u,
                /* Override bit set to 0 to use memoryGroupsBitMap & algorithmsBitMap */
                /* NOTE: As the MCU R5f is running this code override bit cannot be set to enable all memory test */
                .algorithmsBitMap   = 0x00002000u,          /* Choose Algorithm 14 */
                .memoryGroupsBitMap = 0x0080000000000000u,  /* Choose ADC RAM */
                .scrambleValue      = 0xFEDCBA9876543210U,  /* Scramble Value */
            },
            {
                .override           = 0x0u,
                .algorithmsBitMap   = 0x0u,
                .memoryGroupsBitMap = 0x0u,
                .scrambleValue      = 0x0u,
            }
        },
        .PBISTNegConfigRun =
        {
            .CA0   = SDL_MCU_PBIST0_FAIL_INSERTION_TEST_VECTOR_CA0,
            .CA1   = SDL_MCU_PBIST0_FAIL_INSERTION_TEST_VECTOR_CA1,
            .CA2   = SDL_MCU_PBIST0_FAIL_INSERTION_TEST_VECTOR_CA2,
            .CA3   = SDL_MCU_PBIST0_FAIL_INSERTION_TEST_VECTOR_CA3,
            .CL0   = SDL_MCU_PBIST0_FAIL_INSERTION_TEST_VECTOR_CL0,
            .CL1   = SDL_MCU_PBIST0_FAIL_INSERTION_TEST_VECTOR_CL1,
            .CL2   = SDL_MCU_PBIST0_FAIL_INSERTION_TEST_VECTOR_CL2,
            .CL3   = SDL_MCU_PBIST0_FAIL_INSERTION_TEST_VECTOR_CL3,
            .CMS   = SDL_MCU_PBIST0_FAIL_INSERTION_TEST_VECTOR_CMS,
            .CSR   = SDL_MCU_PBIST0_FAIL_INSERTION_TEST_VECTOR_CSR,
            .I0    = SDL_MCU_PBIST0_FAIL_INSERTION_TEST_VECTOR_I0,
            .I1    = SDL_MCU_PBIST0_FAIL_INSERTION_TEST_VECTOR_I1,
            .I2    = SDL_MCU_PBIST0_FAIL_INSERTION_TEST_VECTOR_I2,
            .I3    = SDL_MCU_PBIST0_FAIL_INSERTION_TEST_VECTOR_I3,
            .RAMT  = SDL_MCU_PBIST0_FAIL_INSERTION_TEST_VECTOR_RAMT
        },
        .numPBISTROMTestRuns = 1u,
        .PBISTROMTestConfigRun = 
        {
            {
                .D = 0xCD031F43u,
                .E = 0xCD031F43u,
                .CA2 = 0x7FFFu,
                .CL0 = 0x3FFu,
                .CA3 = 0x0u,
                .I0 = 0x1u,
                .CL1 = 0x1Fu,
                .I3 = 0x0u,
                .I2 = 0xEu,
                .CL2 = 0xEu,
                .CA1 = 0x7FFFu,
                .CA0 = 0x0u,
                .CL3 = 0x7FFFu,
                .I1 = 0x20u,
                .RAMT = 0x2C002020u,
                .CSR = 0x00000001u,
                .CMS = 0x05u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            }
        },
        .interruptNumber        = SDLR_MCU_R5FSS0_CORE0_INTR_MCU_PBIST1_DFT_PBIST_CPU_0,
        .clecInputEvt           = 0x0u,
        .doneFlag               = PBIST_NOT_DONE
    },

    /* MCU_0 Instance */
    {
        .PBISTRegsHiAddress  = 0u,
        .pPBISTRegs          = (SDL_pbistRegs *)SDL_MCU_PBIST0_BASE,
        /*
         * Golden vectors not currently supported running from MCU core because
         * this will be self destructive. Below select memory which is guaranteed
         * not to be utilized can be run.
         */
        .numPBISTRuns        = 1u,
        .PBISTConfigRun = {
            {
                .override           = 0x0u,
                /* Override bit set to 0 to use memoryGroupsBitMap & algorithmsBitMap */
                /* NOTE: As the MCU R5f is running this code override bit cannot be set to enable all memory test */
                .algorithmsBitMap   = 0x00002000u,          /* Choose Algorithm 14 */
                .memoryGroupsBitMap = 0x0080000000000000u,  /* Choose ADC RAM */
                .scrambleValue      = 0xFEDCBA9876543210U,  /* Scramble Value */
            },
            {
                .override           = 0x0u,
                /* Override bit set to 0 to use memoryGroupsBitMap & algorithmsBitMap */
                .algorithmsBitMap   = 0x0u,  /* Choose recommended Algo bitmap */
                .memoryGroupsBitMap = 0x0u,   /* Choose recommended mem bitmap  */
                .scrambleValue      = 0x0u, /* Scramble Value */
            }
        },
        .PBISTNegConfigRun =
        {
            .CA0   = SDL_MCU_PBIST1_FAIL_INSERTION_TEST_VECTOR_CA0,
            .CA1   = SDL_MCU_PBIST1_FAIL_INSERTION_TEST_VECTOR_CA1,
            .CA2   = SDL_MCU_PBIST1_FAIL_INSERTION_TEST_VECTOR_CA2,
            .CA3   = SDL_MCU_PBIST1_FAIL_INSERTION_TEST_VECTOR_CA3,
            .CL0   = SDL_MCU_PBIST1_FAIL_INSERTION_TEST_VECTOR_CL0,
            .CL1   = SDL_MCU_PBIST1_FAIL_INSERTION_TEST_VECTOR_CL1,
            .CL2   = SDL_MCU_PBIST1_FAIL_INSERTION_TEST_VECTOR_CL2,
            .CL3   = SDL_MCU_PBIST1_FAIL_INSERTION_TEST_VECTOR_CL3,
            .CMS   = SDL_MCU_PBIST1_FAIL_INSERTION_TEST_VECTOR_CMS,
            .CSR   = SDL_MCU_PBIST1_FAIL_INSERTION_TEST_VECTOR_CSR,
            .I0    = SDL_MCU_PBIST1_FAIL_INSERTION_TEST_VECTOR_I0,
            .I1    = SDL_MCU_PBIST1_FAIL_INSERTION_TEST_VECTOR_I1,
            .I2    = SDL_MCU_PBIST1_FAIL_INSERTION_TEST_VECTOR_I2,
            .I3    = SDL_MCU_PBIST1_FAIL_INSERTION_TEST_VECTOR_I3,
            .RAMT  = SDL_MCU_PBIST1_FAIL_INSERTION_TEST_VECTOR_RAMT
        },
        .numPBISTROMTestRuns = 1u,
        .PBISTROMTestConfigRun = 
        {
            {
                /* mcu 0 */
                .D = 0x2936BA4Bu,
                .E = 0x2936BA4Bu,
                .CA2 = 0x7FFFu,
                .CL0 = 0x3FFu,
                .CA3 = 0x0u,
                .I0 = 0x1u,
                .CL1 = 0x1Fu,
                .I3 = 0x0u,
                .I2 = 0xEu,
                .CL2 = 0xEu,
                .CA1 = 0x7FFFu,
                .CA0 = 0x0u,
                .CL3 = 0x7FFFu,
                .I1 = 0x20u,
                .RAMT = 0x10002020u,
                .CSR = 0x00000001u,
                .CMS = 0x03u
            },
            {
                /* MCU PSROM Vector 0 */
                .D = 0x6B41975Cu,
                .E = 0x6B41975Cu,
                .CA2 = 0xFFFu,
                .CL0 = 0xFFu,
                .CA3 = 0x0u,
                .I0 = 0x1u,
                .CL1 = 0xFu,
                .I3 = 0x0u,
                .I2 = 0xBu,
                .CL2 = 0xBu,
                .CA1 = 0xFFFu,
                .CA0 = 0x0u,
                .CL3 = 0xFFFu,
                .I1 = 0x10u,
                .RAMT = 0x00002024u,
                .CSR = 0x00000001u,
                .CMS = 0x01u
            },
            {
                /*This vector not supported for PBIST check of ROM*/
                /* MCU PSROM Vector 1 */
                .D = 0xEA43DEA7u,
                .E = 0xEA43DEA7u,
                .CA2 = 0xFFFu,
                .CL0 = 0xFFu,
                .CA3 = 0x0u,
                .I0 = 0x1u,
                .CL1 = 0xFu,
                .I3 = 0x0u,
                .I2 = 0xBu,
                .CL2 = 0xBu,
                .CA1 = 0xFFFu,
                .CA0 = 0x0u,
                .CL3 = 0xFFFu,
                .I1 = 0x10u,
                .RAMT = 0x00012024u,
                .CSR = 0x00000002u
            },
            {
                /*This vector not supported for PBIST check of ROM*/
                /* MCU PSROM Vector 2 */
                .D = 0xF175E2E7u,
                .E = 0xF175E2E7u,
                .CA2 = 0xFFFu,
                .CL0 = 0xFFu,
                .CA3 = 0x0u,
                .I0 = 0x1u,
                .CL1 = 0xFu,
                .I3 = 0x0u,
                .I2 = 0xBu,
                .CL2 = 0xBu,
                .CA1 = 0xFFFu,
                .CA0 = 0x0u,
                .CL3 = 0xFFFu,
                .I1 = 0x10u,
                .RAMT = 0x00022024u,
                .CSR = 0x00000004u
            },
            {
                /*This vector not supported for PBIST check of ROM*/
                /* MCU PSROM Vector 3 */
                .D = 0x2FAF7E92u,
                .E = 0x2FAF7E92u,
                .CA2 = 0xFFFu,
                .CL0 = 0xFFu,
                .CA3 = 0x0u,
                .I0 = 0x1u,
                .CL1 = 0xFu,
                .I3 = 0x0u,
                .I2 = 0xBu,
                .CL2 = 0xBu,
                .CA1 = 0xFFFu,
                .CA0 = 0x0u,
                .CL3 = 0xFFFu,
                .I1 = 0x10u,
                .RAMT = 0x00032024u,
                .CSR = 0x00000008u
            },
            {
                /*This vector not supported for PBIST check of ROM*/
                /* MCU PSROM Vector 4 */
                .D = 0x6D9AF966u,
                .E = 0x6D9AF966u,
                .CA2 = 0xFFFu,
                .CL0 = 0xFFu,
                .CA3 = 0x0u,
                .I0 = 0x1u,
                .CL1 = 0xFu,
                .I3 = 0x0u,
                .I2 = 0xBu,
                .CL2 = 0xBu,
                .CA1 = 0xFFFu,
                .CA0 = 0x0u,
                .CL3 = 0xFFFu,
                .I1 = 0x10u,
                .RAMT = 0x00042024u,
                .CSR = 0x00000010u
            },
            {
                /*This vector not supported for PBIST check of ROM*/
                /* MCU PSROM Vector 5 */
                .D = 0x98FA4FB9u,
                .E = 0x98FA4FB9u,
                .CA2 = 0xFFFu,
                .CL0 = 0xFFu,
                .CA3 = 0x0u,
                .I0 = 0x1u,
                .CL1 = 0xFu,
                .I3 = 0x0u,
                .I2 = 0xBu,
                .CL2 = 0xBu,
                .CA1 = 0xFFFu,
                .CA0 = 0x0u,
                .CL3 = 0xFFFu,
                .I1 = 0x10u,
                .RAMT = 0x00052024u,
                .CSR = 0x00000020u
            },
            {
                /*This vector not supported for PBIST check of ROM*/
                /* MCU PSROM Vector 6 */
                .D = 0x267CDE89u,
                .E = 0x267CDE89u,
                .CA2 = 0xFFFu,
                .CL0 = 0xFFu,
                .CA3 = 0x0u,
                .I0 = 0x1u,
                .CL1 = 0xFu,
                .I3 = 0x0u,
                .I2 = 0xBu,
                .CL2 = 0xBu,
                .CA1 = 0xFFFu,
                .CA0 = 0x0u,
                .CL3 = 0xFFFu,
                .I1 = 0x10u,
                .RAMT = 0x00062024u,
                .CSR = 0x00000040u
            },
            {
                /*This vector not supported for PBIST check of ROM*/
                /* MCU PSROM Vector 7 */
                .D = 0x50344C09u,
                .E = 0x50344C09u,
                .CA2 = 0xFFFu,
                .CL0 = 0xFFu,
                .CA3 = 0x0u,
                .I0 = 0x1u,
                .CL1 = 0xFu,
                .I3 = 0x0u,
                .I2 = 0xBu,
                .CL2 = 0xBu,
                .CA1 = 0xFFFu,
                .CA0 = 0x0u,
                .CL3 = 0xFFFu,
                .I1 = 0x10u,
                .RAMT = 0x00072024u,
                .CSR = 0x00000080u
            },
            {
                /*This vector not supported for PBIST check of ROM*/
                /* MCU PSROM Vector 8 */
                .D = 0xB5BE0F34u,
                .E = 0xB5BE0F34u,
                .CA2 = 0xFFFu,
                .CL0 = 0xFFu,
                .CA3 = 0x0u,
                .I0 = 0x1u,
                .CL1 = 0xFu,
                .I3 = 0x0u,
                .I2 = 0xBu,
                .CL2 = 0xBu,
                .CA1 = 0xFFFu,
                .CA0 = 0x0u,
                .CL3 = 0xFFFu,
                .I1 = 0x10u,
                .RAMT = 0x00082024u,
                .CSR = 0x00000100u
            },
            {
                /*This vector not supported for PBIST check of ROM*/
                /* MCU PSROM Vector 9 */
                .D = 0x3C45A0EFu,
                .E = 0x3C45A0EFu,
                .CA2 = 0xFFFu,
                .CL0 = 0xFFu,
                .CA3 = 0x0u,
                .I0 = 0x1u,
                .CL1 = 0xFu,
                .I3 = 0x0u,
                .I2 = 0xBu,
                .CL2 = 0xBu,
                .CA1 = 0xFFFu,
                .CA0 = 0x0u,
                .CL3 = 0xFFFu,
                .I1 = 0x10u,
                .RAMT = 0x00092024u,
                .CSR = 0x00000200u
            },
            {
                /*This vector not supported for PBIST check of ROM*/
                /* MCU PSROM Vector 10 */
                .D = 0x502B0D04u,
                .E = 0x502B0D04u,
                .CA2 = 0xFFFu,
                .CL0 = 0xFFu,
                .CA3 = 0x0u,
                .I0 = 0x1u,
                .CL1 = 0xFu,
                .I3 = 0x0u,
                .I2 = 0xBu,
                .CL2 = 0xBu,
                .CA1 = 0xFFFu,
                .CA0 = 0x0u,
                .CL3 = 0xFFFu,
                .I1 = 0x10u,
                .RAMT = 0x000a2024u,
                .CSR = 0x00000400u
            },
            {
                /*This vector not supported for PBIST check of ROM*/
                /* MCU PSROM Vector 11 */
                .D = 0x13B61DC7u,
                .E = 0x13B61DC7u,
                .CA2 = 0xFFFu,
                .CL0 = 0xFFu,
                .CA3 = 0x0u,
                .I0 = 0x1u,
                .CL1 = 0xFu,
                .I3 = 0x0u,
                .I2 = 0xBu,
                .CL2 = 0xBu,
                .CA1 = 0xFFFu,
                .CA0 = 0x0u,
                .CL3 = 0xFFFu,
                .I1 = 0x10u,
                .RAMT = 0x000b2024u,
                .CSR = 0x00000800u
            },
        },
        .interruptNumber        = SDLR_MCU_R5FSS0_CORE0_INTR_MCU_PBIST0_DFT_PBIST_CPU_0,
        .clecInputEvt           = 0x0u,
        .doneFlag               = PBIST_NOT_DONE
    },
    /* DSS Instance */
    {
        .PBISTRegsHiAddress  = 0u,
        .pPBISTRegs          = (SDL_pbistRegs *)SDL_PBIST5_BASE,
        .numPBISTRuns        = SDL_PBIST5_NUM_TEST_VECTORS,
        .PBISTConfigRun = {
            {
                .override           = 0x0u,
                /* Override bit set to 0 to use memoryGroupsBitMap & algorithmsBitMap */
                .algorithmsBitMap   = SDL_PBIST5_ALGO_BITMAP_0,   /* Choose recommended Algo bitmap 0 */
                .memoryGroupsBitMap = SDL_PBIST5_MEM_BITMAP_0,    /* Choose recommended mem bitmap  0 */
                .scrambleValue      = 0xFEDCBA9876543210U, /* Scramble Value */
            },
            {
                .override           = 0x0u,
                /* Override bit set to 0 to use memoryGroupsBitMap & algorithmsBitMap */
                .algorithmsBitMap   = SDL_PBIST5_ALGO_BITMAP_1,   /* Choose recommended Algo bitmap 1 */
                .memoryGroupsBitMap = SDL_PBIST5_MEM_BITMAP_1,    /* Choose recommended mem bitmap  1 */
                .scrambleValue      = 0xFEDCBA9876543210U, /* Scramble Value */
            },

        },
        .PBISTNegConfigRun = {
            .CA0   = SDL_PBIST5_FAIL_INSERTION_TEST_VECTOR_CA0,
            .CA1   = SDL_PBIST5_FAIL_INSERTION_TEST_VECTOR_CA1,
            .CA2   = SDL_PBIST5_FAIL_INSERTION_TEST_VECTOR_CA2,
            .CA3   = SDL_PBIST5_FAIL_INSERTION_TEST_VECTOR_CA3,
            .CL0   = SDL_PBIST5_FAIL_INSERTION_TEST_VECTOR_CL0,
            .CL1   = SDL_PBIST5_FAIL_INSERTION_TEST_VECTOR_CL1,
            .CL2   = SDL_PBIST5_FAIL_INSERTION_TEST_VECTOR_CL2,
            .CL3   = SDL_PBIST5_FAIL_INSERTION_TEST_VECTOR_CL3,
            .CMS   = SDL_PBIST5_FAIL_INSERTION_TEST_VECTOR_CMS,
            .CSR   = SDL_PBIST5_FAIL_INSERTION_TEST_VECTOR_CSR,
            .I0    = SDL_PBIST5_FAIL_INSERTION_TEST_VECTOR_I0,
            .I1    = SDL_PBIST5_FAIL_INSERTION_TEST_VECTOR_I1,
            .I2    = SDL_PBIST5_FAIL_INSERTION_TEST_VECTOR_I2,
            .I3    = SDL_PBIST5_FAIL_INSERTION_TEST_VECTOR_I3,
            .RAMT  = SDL_PBIST5_FAIL_INSERTION_TEST_VECTOR_RAMT
        },
        .numPBISTROMTestRuns = 1u,
        .PBISTROMTestConfigRun = 
        {
            {
                .D = 0x5771A906u,
                .E = 0x5771A906u,
                .CA2 = 0x7FFFu,
                .CL0 = 0x3FFu,
                .CA3 = 0x0u,
                .I0 = 0x1u,
                .CL1 = 0x1Fu,
                .I3 = 0x0u,
                .I2 = 0xEu,
                .CL2 = 0xEu,
                .CA1 = 0x7FFFu,
                .CA0 = 0x0u,
                .CL3 = 0x7FFFu,
                .I1 = 0x20u,
                .RAMT = 0x38002020u,
                .CSR = 0x00000001u,
                .CMS = 0x05u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            },
            {
                .D = 0x0u,
                .E = 0x0u,
                .CA2 = 0x0u,
                .CL0 = 0x0u,
                .CA3 = 0x0u,
                .I0 = 0x0u,
                .CL1 = 0x0u,
                .I3 = 0x0u,
                .I2 = 0x0u,
                .CL2 = 0x0u,
                .CA1 = 0x0u,
                .CA0 = 0x0u,
                .CL3 = 0x0u,
                .I1 = 0x0u,
                .RAMT = 0x0u,
                .CSR = 0x0u,
                .CMS = 0x0u
            }
        },
        .interruptNumber        = SDLR_MCU_R5FSS0_CORE0_INTR_PBIST5_DFT_PBIST_CPU_0,
        .clecInputEvt           = 0x0u,
        .doneFlag               = PBIST_NOT_DONE
    },	
};

SDL_pbistInstInfo * SDL_PBIST_getInstInfo(SDL_PBIST_inst instance)
{
    SDL_pbistInstInfo *pInfo = NULL;

    if ((uint32_t)instance < SDL_PBIST_NUM_INSTANCES)
    {
        pInfo = &SDL_PBIST_InstInfoArray[instance];
    }

    return pInfo;
}

#define SDL_PBIST_MAX_CLEC_EVTS 4U

typedef struct {
    uint32_t outputEvt;
    uint32_t intNum;
} sdlClecEvtMap;

sdlClecEvtMap clecLookupTable[SDL_PBIST_MAX_CLEC_EVTS] =
{
    {SDLR_COMPUTE_CLUSTER0_CLEC_MSMC_EVENT_IN_COMPUTE_CLUSTER0_CORE_CORE_MSMC_INTR_8,
     SDLR_MCU_R5FSS0_CORE0_INTR_COMPUTE_CLUSTER0_CLEC_SOC_EVENTS_OUT_LEVEL_8},
    {SDLR_COMPUTE_CLUSTER0_CLEC_MSMC_EVENT_IN_COMPUTE_CLUSTER0_CORE_CORE_MSMC_INTR_9,
     SDLR_MCU_R5FSS0_CORE0_INTR_COMPUTE_CLUSTER0_CLEC_SOC_EVENTS_OUT_LEVEL_9},
    {SDLR_COMPUTE_CLUSTER0_CLEC_MSMC_EVENT_IN_COMPUTE_CLUSTER0_CORE_CORE_MSMC_INTR_12,
     SDLR_MCU_R5FSS0_CORE0_INTR_COMPUTE_CLUSTER0_CLEC_SOC_EVENTS_OUT_LEVEL_12},
    {SDLR_COMPUTE_CLUSTER0_CLEC_MSMC_EVENT_IN_COMPUTE_CLUSTER0_CORE_CORE_MSMC_INTR_13,
     SDLR_MCU_R5FSS0_CORE0_INTR_COMPUTE_CLUSTER0_CLEC_SOC_EVENTS_OUT_LEVEL_13}
};

uint32_t SDL_PBIST_translateClecToIntr(uint32_t outputEvt)
{
    uint32_t intNum = 0;
    uint32_t i;

    for (i = 0; i < SDL_PBIST_MAX_CLEC_EVTS; i++)
    {
        if (outputEvt == clecLookupTable[i].outputEvt)
        {
            intNum = clecLookupTable[i].intNum;
            break;
        }
    }

    return intNum;
}

static void SDL_PBIST_clecClearEvent(SDL_PBIST_CLEC_EVTRegs *pRegs, uint32_t evtNum)
{
    uint32_t    regVal;

    /* ECR is a write-only register. When written value of 0x1
     * it will clear the event. So write 0x1. */
    regVal = ECR_CLR_VAL;
    SDL_REG32_WR(&pRegs->CFG[evtNum].ECR, regVal);

    return;
}

void SDL_PBIST_eventHandler( uint32_t instanceId)
{
    if (instanceId == (uint32_t) SDL_PBIST_INST_C7X_0)
    {
        /* Clear C7x_0 PBIST interrupt event in CLEC */
        SDL_PBIST_clecClearEvent((SDL_PBIST_CLEC_EVTRegs *)SDL_COMPUTE_CLUSTER0_CLEC_REGS_BASE,
                       SDLR_COMPUTE_CLUSTER0_CLEC_MSMC_EVENT_IN_COMPUTE_CLUSTER0_CORE_CORE_MSMC_INTR_12);
    }
	if (instanceId == (uint32_t) SDL_PBIST_INST_C7X_1)
    {
        /* Clear C7x_1 PBIST interrupt event in CLEC */
        SDL_PBIST_clecClearEvent((SDL_PBIST_CLEC_EVTRegs *)SDL_COMPUTE_CLUSTER0_CLEC_REGS_BASE,
                       SDLR_COMPUTE_CLUSTER0_CLEC_MSMC_EVENT_IN_COMPUTE_CLUSTER0_CORE_CORE_MSMC_INTR_13);
    }
    if (instanceId == (uint32_t) SDL_PBIST_INST_A72)
    {
        /* Clear A72 PBIST interrupt event in CLEC*/
        SDL_PBIST_clecClearEvent((SDL_PBIST_CLEC_EVTRegs *)SDL_COMPUTE_CLUSTER0_CLEC_REGS_BASE,
                       SDLR_COMPUTE_CLUSTER0_CLEC_MSMC_EVENT_IN_COMPUTE_CLUSTER0_CORE_CORE_MSMC_INTR_8);
    }

    SDL_PBIST_InstInfoArray[instanceId].doneFlag = PBIST_DONE;

    return;
}

static void SDL_PBIST_postCheckResult(uint32_t postStatMmrRegVal,
                                      SDL_PBIST_hwpostInst instance,
                                      bool *pResult)
{
    uint32_t mask = 0xFFFFFFFFu;
    (void)instance;

    /* Only one instance supported for J721E: SDL_PBIST_HWPOST_INST_MCU */
    mask = SDL_WKUP_CTRL_MMR_CFG0_WKUP_POST_STAT_POST_MCU_PBIST_FAIL_MASK;

    if ((postStatMmrRegVal & mask) == ((uint32_t)0x00000000u))
    {
        *pResult = (bool)true;
    }
    else
    {
        *pResult = (bool)false;
    }

    return;
}

static void SDL_PBIST_isPostPbistTimeout(uint32_t postStatMmrRegVal,
                                         SDL_PBIST_hwpostInst instance,
                                         bool *pIsTimedOut)
{
    uint32_t shift;
    (void)instance;

    /* Only one instance supported for J721E: SDL_PBIST_HWPOST_INST_MCU */
    shift = SDL_WKUP_CTRL_MMR_CFG0_WKUP_POST_STAT_POST_MCU_PBIST_TIMEOUT_SHIFT;

    *pIsTimedOut = (((postStatMmrRegVal >> shift) & 0x1u) == 0x1u) ? (bool)true : (bool)false;

    return;
}

static void SDL_PBIST_isPostPbistDone(uint32_t postStatMmrRegVal,
                                      SDL_PBIST_hwpostInst instance,
                                      bool *pIsDone)
{
    uint32_t shift;
    (void)instance;

    /* Only one instance supported for J721E: SDL_PBIST_HWPOST_INST_MCU */
    shift = SDL_WKUP_CTRL_MMR_CFG0_WKUP_POST_STAT_POST_MCU_PBIST_DONE_SHIFT;

    *pIsDone = (((postStatMmrRegVal >> shift) & 0x1u) == 0x1u) ? (bool)true : (bool)false;

    return;
}

static void SDL_PBIST_runPostPbistCheck(SDL_PBIST_hwpostInst instance, SDL_PBIST_postStatus *pPostStatus)
{
    uint32_t postRegVal;
    bool     PBISTResult;

    /* Get PBIST register space Pointer */
    *pPostStatus = SDL_PBIST_POST_COMPLETED_SUCCESS;

    /* Read HW POST status register */
    postRegVal = SDL_REG32_RD(SDL_WKUP_CTRL_MMR0_CFG0_BASE +
                              SDL_WKUP_CTRL_MMR_CFG0_WKUP_POST_STAT);

    /* Check if HW POST PBIST was performed */
    SDL_PBIST_isPostPbistDone(postRegVal, instance, &PBISTResult);

    if (PBISTResult != (bool) true)
    {
        /* HW POST: PBIST not completed, check if it timed out */
        SDL_PBIST_isPostPbistTimeout(postRegVal,
                                     instance,
                                     &PBISTResult);
        if (PBISTResult != (bool) true)
        {
            /* HW POST: PBIST was not performed at all on this device
             * for this core */
            *pPostStatus = SDL_PBIST_POST_NOT_RUN;
        }
        else
        {
            /* HW POST: PBIST was attempted but timed out for this section */
            *pPostStatus = SDL_PBIST_POST_TIMEOUT;
        }
    }
    else
    {
        /* HW POST: PBIST was completed on this device, check the result */
        SDL_PBIST_postCheckResult(postRegVal, instance, &PBISTResult);
        if (PBISTResult != (bool) true)
        {
            /* HW POST: PBIST was completed, but the test failed */
            *pPostStatus = SDL_PBIST_POST_COMPLETED_FAILURE;
        }
    } /* if (PBISTResult != (bool) true) */

    return;
}

int32_t SDL_PBIST_getDevicePostStatus(SDL_PBIST_postResult *pResult)
{
    int32_t ret = SDL_PASS;

    /* Get status for MCU */
    SDL_PBIST_runPostPbistCheck(SDL_PBIST_HWPOST_INST_MCU, &pResult->mcuPostStatus);

    return ret;
}
