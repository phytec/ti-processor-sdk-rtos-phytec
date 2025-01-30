/*
 *   Copyright (c) Texas Instruments Incorporated 2020-2021
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
 *  \file     pbist_test_cfg.c
 *
 *  \brief    This file contains PBIST test configuration
 *
 *  \details  PBIST Test Configuration
 **/

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <stdint.h>
#include <sdl_types.h>
#include <soc.h>
#include <src/ip/sdl_ip_pbist.h>
#include <ti/csl/csl_clec.h>
#include <ti/csl/csl_cbass.h>
#include <ti/drv/sciclient/sciclient.h>
#include <ti/drv/uart/UART_stdio.h>
#include <ti/csl/csl_rat.h>
#include <ti/csl/cslr_vpac3.h>
#include <ti/csl/cslr_cp_ace.h>
#include <ti/csl/csl_dss.h>
#include "power_seq.h"
#include "pbist_test_cfg.h"

/* ========================================================================== */
/*                                Macros                                      */
/* ========================================================================== */

/* This macro defined by default to indicate only a select memory section can
 * be run on the MCU memory PBIST instances to ensure test application running
 * on MCU is not corrupted. */
#define PBIST_MCU_SELECTED_MEMORY

/* Macro to determine if test is excluded because of failure.
 * Set to 0 to disable specific sections. */
#define PBIST_DISABLE_BECAUSE_OF_FAILURE 0

/* ========================================================================== */
/*                            Local function prototypes                       */
/* ========================================================================== */
/*
    InitRestore functions : Initialize or Restore based on init flag
    init : TRUE  --> Initialize
    init : FALSE --> Restore
*/
int32_t PBIST_VPACAuxInitRestore(bool init);
int32_t PBIST_HCAuxInitRestore(bool init);
int32_t PBIST_MainInfra0AuxInitRestore(bool init);
int32_t PBIST_NAVSSAuxInitRestore(bool init);
int32_t PBIST_CodecAuxInitRestore(bool init);
int32_t PBIST_GPUAuxInitRestore(bool init);
int32_t PBIST_DSSAuxInitRestore(bool init);
int32_t PBIST_A72AuxInitRestore(bool init);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
uint32_t PBIST_MainInfra0AuxDevList[MAIN_INFRA0_NUM_AUX_DEVICES] =
{
    TISCI_DEV_MMCSD0,
    TISCI_DEV_MMCSD1,
    TISCI_DEV_CPSW1,
    TISCI_DEV_DEBUGSS_WRAP0,
};

uint32_t PBIST_MainInfra1AuxDevList[MAIN_INFRA1_NUM_AUX_DEVICES] =
{
    TISCI_DEV_MCAN0,
    TISCI_DEV_MCAN1,
    TISCI_DEV_MCAN2,
    TISCI_DEV_MCAN3,
    TISCI_DEV_MCAN4,
    TISCI_DEV_MCAN5,
};

uint32_t PBIST_HCAuxDevList[HC_NUM_AUX_DEVICES] =
{
    TISCI_DEV_PCIE1,
    TISCI_DEV_USB0,
    TISCI_DEV_MMCSD0,
    TISCI_DEV_MMCSD1,
    TISCI_DEV_SA2_UL0,
    TISCI_DEV_VUSR_DUAL0,
};

uint32_t PBIST_CodecAuxDevList[CODEC_NUM_AUX_DEVICES] =
{
    TISCI_DEV_CODEC0
};

uint32_t PBIST_GPUAuxDevList[GPU_NUM_AUX_DEVICES] =
{
    TISCI_DEV_J7AEP_GPU_BXS464_WRAP0,
    TISCI_DEV_J7AEP_GPU_BXS464_WRAP0_GPU_SS_0,
    TISCI_DEV_J7AEP_GPU_BXS464_WRAP0_GPUCORE_0,
};

uint32_t PBIST_DSSAuxDevList[DSS_NUM_AUX_DEVICES] =
{
    TISCI_DEV_DSS0,
    TISCI_DEV_DSS_EDP0,
    TISCI_DEV_DSS_DSI0,
    TISCI_DEV_CSI_TX_IF_V2_0,
    TISCI_DEV_CSI_TX_IF_V2_1,
    TISCI_DEV_CSI_RX_IF0,
    TISCI_DEV_CSI_RX_IF1,
};

uint32_t PBIST_C7x_0AuxDevList[C7x_0_NUM_AUX_DEVICES] =
{
    TISCI_DEV_C71X_0_PBIST_VD,
    TISCI_DEV_COMPUTE_CLUSTER0_C71SS0_0,
    TISCI_DEV_COMPUTE_CLUSTER0_C71SS0_MMA_0,
    TISCI_DEV_COMPUTE_CLUSTER0_C71SS0_PBIST_WRAP_0
};

uint32_t PBIST_C7x_1AuxDevList[C7x_1_NUM_AUX_DEVICES] =
{
    TISCI_DEV_C71X_1_PBIST_VD,
    TISCI_DEV_COMPUTE_CLUSTER0_C71SS1_0,
    TISCI_DEV_COMPUTE_CLUSTER0_C71SS1_PBIST_WRAP_0,
};

uint32_t PBIST_A72AuxDevList[A72_NUM_AUX_DEVICES] =
{
    TISCI_DEV_A72SS0_CORE0,
    TISCI_DEV_A72SS0_CORE1,
    TISCI_DEV_A72SS0,
    TISCI_DEV_A72SS0_CORE0_PBIST_WRAP,
};

PBIST_TestHandle_t PBIST_TestHandleArray[PBIST_MAX_INSTANCE+1] =
{
    /* VPAC */
    {
        .testName            = "VPAC PBIST",
        .numPostPbistToCheck = 0u,
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
        .numPBISTROMTestRuns = 1,
        .PBISTROMTestConfigRun = 
        {
            {
                .D = 0x65F885E3,
                .E = 0x65F885E3,
                .CA2 = 0x7FFF,
                .CL0 = 0x3FF,
                .CA3 = 0x0,
                .I0 = 0x1,
                .CL1 = 0x1F,
                .I3 = 0x0,
                .I2 = 0xE,
                .CL2 = 0xE,
                .CA1 = 0x7FFF,
                .CA0 = 0x0,
                .CL3 = 0x7FFF,
                .I1 = 0x20,
                .RAMT = 0x20002020,
                .CSR = 0x00000001,
                .CMS = 0x01
            }
        },
        .tisciPBISTDeviceId     = TISCI_DEV_PBIST8,   /* PBIST device id  */
        .pollMode               = false,
        .interruptNumber        = SDLR_MCU_R5FSS0_CORE0_INTR_PBIST8_DFT_PBIST_CPU_0,
        .procRstNeeded          = true,
        .secondaryCoreNeeded    = false,            /* Secondary core needed */
        .coreName               = "VPAC core",      /* Primary core   */
        .tisciProcId            = 0x0u,             /* No ProcId for VPAC */
        .tisciDeviceId          = TISCI_DEV_VPAC0,  /* VPAC core Device Id */
        .coreCustPwrSeqNeeded   = false,
        .numAuxDevices          = 0u,               /* No Aux devices */
        .auxInitRestoreFunction = PBIST_VPACAuxInitRestore,/* Auxilliary init function */
        .doneFlag               = false,            /* Initialize done flag */
    },
    /* Main 1 PBIST */
    {
        .testName            = "Main Infra1 PBIST",
        .numPostPbistToCheck = 0u,
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
       .numPBISTROMTestRuns = 1,
        .PBISTROMTestConfigRun = 
        {
            {
                .D = 0x06EDAC5F,
                .E = 0x06EDAC5F,
                .CA2 = 0x7FFF,
                .CL0 = 0x3FF,
                .CA3 = 0x0,
                .I0 = 0x1,
                .CL1 = 0x1F,
                .I3 = 0x0,
                .I2 = 0xE,
                .CL2 = 0xE,
                .CA1 = 0x7FFF,
                .CA0 = 0x0,
                .CL3 = 0x7FFF,
                .I1 = 0x20,
                .RAMT = 0x10002020,
                .CSR = 0x00000001,
                .CMS = 0x01
            }
        },
        .tisciPBISTDeviceId     = TISCI_DEV_PBIST1,    /* PBIST device id  */
        .pollMode               = false,
        .interruptNumber        = SDLR_MCU_R5FSS0_CORE0_INTR_PBIST1_DFT_PBIST_CPU_0,
        .procRstNeeded          = false,
        .secondaryCoreNeeded    = false,               /* Secondary core needed */
        .coreName               = "",                  /* No coreName   */
        .tisciProcId            = 0x0u,                /* No Proc Id needed for Main Intrastructure */
        .tisciDeviceId          = 0x0u,                /* No Device Id needed for Main infrastructure */
        .coreCustPwrSeqNeeded   = false,
        .numAuxDevices          = MAIN_INFRA1_NUM_AUX_DEVICES,
        .auxDeviceIdsP          = &PBIST_MainInfra1AuxDevList[0], /* Array of Aux device ids */
        .auxInitRestoreFunction = NULL, /* Auxilliary init function */
        .doneFlag               = false,               /* Initialize done flag */
    },
    /* DMPAC */
    {
        .testName            = "DMPAC PBIST",
        .numPostPbistToCheck = 0u,
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
        .numPBISTROMTestRuns = 1,
        .PBISTROMTestConfigRun = 
        {
            {
                .D = 0xD6F4BA45,
                .E = 0xD6F4BA45,
                .CA2 = 0x7FFF,
                .CL0 = 0x3FF,
                .CA3 = 0x0,
                .I0 = 0x1,
                .CL1 = 0x1F,
                .I3 = 0x0,
                .I2 = 0xE,
                .CL2 = 0xE,
                .CA1 = 0x7FFF,
                .CA0 = 0x0,
                .CL3 = 0x7FFF,
                .I1 = 0x20,
                .RAMT = 0x20002020,
                .CSR = 0x00000001,
                .CMS = 0x01
            }
        },
        .tisciPBISTDeviceId     = TISCI_DEV_PBIST7,   /* PBIST device id  */
        .pollMode               = false,
        .interruptNumber        = SDLR_MCU_R5FSS0_CORE0_INTR_PBIST7_DFT_PBIST_CPU_0,
        .procRstNeeded          = true,
        .secondaryCoreNeeded    = true,                       /* Secondary core needed */
        .coreName               = "DMPAC core",               /* Primary core   */
        .secCoreName            = "DMPAC SDE core",           /* Secondary core */
        .tisciProcId            = 0x0u,                       /* No ProcId for DMPAC */
        .tisciSecProcId         = 0x0u,                       /* No Sec ProcId for DMPAC */
        .tisciDeviceId          = TISCI_DEV_DMPAC0,           /* DMPAC Core Device Id */
        .tisciSecDeviceId       = TISCI_DEV_DMPAC0_SDE_0,     /* DMPAC_SDE Device Id */
        .coreCustPwrSeqNeeded   = false,
        .numAuxDevices          = 0u,                  /* No Aux devices */
        .auxInitRestoreFunction = NULL,                /* Auxilliary init function */
        .doneFlag               = false,               /* Initialize done flag */
    },
    /* Main R5F 0 */
    {
        .testName            = "Main R5F 0 PBIST",
        .numPostPbistToCheck = 0u,
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
        .numPBISTROMTestRuns = 1,
        .PBISTROMTestConfigRun = 
        {
            {
                .D = 0xF412605E,
                .E = 0xF412605E,
                .CA2 = 0x7FFF,
                .CL0 = 0x3FF,
                .CA3 = 0x0,
                .I0 = 0x1,
                .CL1 = 0x1F,
                .I3 = 0x0,
                .I2 = 0xE,
                .CL2 = 0xE,
                .CA1 = 0x7FFF,
                .CA0 = 0x0,
                .CL3 = 0x7FFF,
                .I1 = 0x20,
                .RAMT = 0x08002020,
                .CSR = 0x00000001,
                .CMS = 0x01
            }
        },
        .tisciPBISTDeviceId     = TISCI_DEV_PBIST2,    /* PBIST device id  */
        .pollMode               = false,
        .interruptNumber        = SDLR_MCU_R5FSS0_CORE0_INTR_PBIST2_DFT_PBIST_CPU_0,
        .procRstNeeded          = true,
        .secondaryCoreNeeded    = true,                /* Secondary core needed */
        .coreName               = "Main R5F0 core0",   /* Primary core   */
        .secCoreName            = "Main R5F0 core1",   /* Secondary core */
        .tisciProcId            = SCICLIENT_PROC_ID_R5FSS0_CORE0, /* Main R5F core 0 Proc Id */
        .tisciSecProcId         = SCICLIENT_PROC_ID_R5FSS0_CORE1, /* Main R5F core 1 Proc Id */
        .tisciDeviceId          = TISCI_DEV_R5FSS0_CORE0,   /* Main R5F core 0 Device Id */
        .tisciSecDeviceId       = TISCI_DEV_R5FSS0_CORE1,   /* Main R5F core 1 Device Id */
        .coreCustPwrSeqNeeded   = false,
        .numAuxDevices          = 0u,                   /* No Aux devices */
        .auxInitRestoreFunction = NULL,                 /* Auxilliary init function */
        .doneFlag               = false,                /* Initialize done flag  */
    },
    /* Main R5F 1 */
    {
        .testName            = "Main R5F 1 PBIST",
        .numPostPbistToCheck = 0u,
        .PBISTRegsHiAddress  = 0u,
        .pPBISTRegs          = (SDL_pbistRegs *)SDL_PBIST10_BASE,
        .numPBISTRuns        = SDL_PBIST10_NUM_TEST_VECTORS,
        .PBISTConfigRun = {
            {
                .override           = 0x0u,
                /* Override bit set to 0 to use memoryGroupsBitMap & algorithmsBitMap */
                .algorithmsBitMap   = SDL_PBIST10_ALGO_BITMAP_0,   /* Choose recommended Algo bitmap */
                /* NOTE: As the MCU R5f is running this code override bit cannot be set to enable all memory test */
                .memoryGroupsBitMap = SDL_PBIST10_MEM_BITMAP_0,    /* Choose recommended mem bitmap  */
                .scrambleValue      = 0xFEDCBA9876543210U,  /* Scramble Value       */
            },
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
        .numPBISTROMTestRuns = 1,
        .PBISTROMTestConfigRun = 
        {
            {
                .D = 0xF412605E,
                .E = 0xF412605E,
                .CA2 = 0x7FFF,
                .CL0 = 0x3FF,
                .CA3 = 0x0,
                .I0 = 0x1,
                .CL1 = 0x1F,
                .I3 = 0x0,
                .I2 = 0xE,
                .CL2 = 0xE,
                .CA1 = 0x7FFF,
                .CA0 = 0x0,
                .CL3 = 0x7FFF,
                .I1 = 0x20,
                .RAMT = 0x08002020,
                .CSR = 0x00000001,
                .CMS = 0x01
            }
        },
        .tisciPBISTDeviceId     = TISCI_DEV_PBIST10, /* PBIST device id  */
        .interruptNumber        = SDLR_MCU_R5FSS0_CORE0_INTR_PBIST10_DFT_PBIST_CPU_0,
        .procRstNeeded          = true,
        .secondaryCoreNeeded    = true,                /* Secondary core needed */
        .coreName               = "Main R5F0 core0",   /* Primary core   */
        .secCoreName            = "Main R5F0 core1",   /* Secondary core */
        .tisciProcId            = SCICLIENT_PROC_ID_R5FSS0_CORE1, /* Main R5F core 1 Proc Id */
        .tisciSecProcId         = 0,    /* Main R5F core 1 Proc Id */
        .tisciDeviceId          = TISCI_DEV_R5FSS0_CORE1,   /* Main R5F core 1 Device Id */
        .tisciSecDeviceId       = 0,                     /* Secondary core needed */
        .coreCustPwrSeqNeeded   = false,
        .numAuxDevices          = 0u,                   /* No Aux devices */
        .auxInitRestoreFunction = NULL,                 /* Auxilliary init function */
        .doneFlag               = false,                /* Initialize done flag */
    },
    /* Main Infra */
    {
        .testName            = "Main Infra PBIST",
        .numPostPbistToCheck = 0u,
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
        .numPBISTROMTestRuns = 1,
        .PBISTROMTestConfigRun = 
        {
            {
                .D = 0xA4E5494F,
                .E = 0xA4E5494F,
                .CA2 = 0x7FFF,
                .CL0 = 0x3FF,
                .CA3 = 0x0,
                .I0 = 0x1,
                .CL1 = 0x1F,
                .I3 = 0x0,
                .I2 = 0xE,
                .CL2 = 0xE,
                .CA1 = 0x7FFF,
                .CA0 = 0x0,
                .CL3 = 0x7FFF,
                .I1 = 0x20,
                .RAMT = 0x14002020,
                .CSR = 0x00000001,
                .CMS = 0x04
            }
        },
        .tisciPBISTDeviceId     = TISCI_DEV_PBIST0,    /* PBIST device id  */
        .pollMode               = false,
        .interruptNumber        = SDLR_MCU_R5FSS0_CORE0_INTR_PBIST0_DFT_PBIST_CPU_0,
        .procRstNeeded          = false,
        .secondaryCoreNeeded    = false,               /* Secondary core needed */
        .coreName               = "",                  /* No coreName   */
        .tisciProcId            = 0x0u,                /* No Proc Id needed for Main Intrastructure */
        .tisciDeviceId          = 0x0u,                /* No Device Id needed for Main infrastructure */
        .coreCustPwrSeqNeeded   = false,
        .numAuxDevices          = MAIN_INFRA0_NUM_AUX_DEVICES,
        .auxDeviceIdsP          = &PBIST_MainInfra0AuxDevList[0], /* Array of Aux device ids */
        .auxInitRestoreFunction = PBIST_MainInfra0AuxInitRestore, /* Auxilliary init function */
        .doneFlag               = false,               /* Initialize done flag */
    },
    /* HC */
    {
        .testName            = "HC PBIST",
        .numPostPbistToCheck = 0u,
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
        .numPBISTROMTestRuns = 0,
        .PBISTROMTestConfigRun = 
        { 
            {
                .D = 0xBFE16A0F,
                .E = 0xBFE16A0F,
                .CA2 = 0x7FFF,
                .CL0 = 0x3FF,
                .CA3 = 0x0,
                .I0 = 0x1,
                .CL1 = 0x1F,
                .I3 = 0x0,
                .I2 = 0xE,
                .CL2 = 0xE,
                .CA1 = 0x7FFF,
                .CA0 = 0x0,
                .CL3 = 0x7FFF,
                .I1 = 0x20,
                .RAMT = 0x4C002020,
                .CSR = 0x00000001,
                .CMS = 0x05
            }
        },
        .tisciPBISTDeviceId     = TISCI_DEV_PBIST3, /* PBIST device id  */
        .pollMode               = false,
        .interruptNumber        = SDLR_MCU_R5FSS0_CORE0_INTR_PBIST3_DFT_PBIST_CPU_0,
        .procRstNeeded          = false,
        .secondaryCoreNeeded    = false,            /* Secondary core needed */
        .coreName               = "",               /* No coreName   */
        .tisciProcId            = 0x0u,             /* No ProcId for HC */
        .tisciDeviceId          = 0x0u,             /* No Device Id */
        .coreCustPwrSeqNeeded   = false,
        .numAuxDevices          = HC_NUM_AUX_DEVICES,
        .auxDeviceIdsP          = &PBIST_HCAuxDevList[0], /* Array of Aux device ids */
        .auxInitRestoreFunction = PBIST_HCAuxInitRestore,             /* Auxilliary init function */
        .doneFlag               = false,            /* Initialize done flag */
    },
    /* NAVSS */
    {
        .testName            = "NAVSS PBIST",
        .numPostPbistToCheck = 0u,
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
        .numPBISTROMTestRuns = 1,
        .PBISTROMTestConfigRun = 
        {
            {
                .D = 0xEDE472EB,
                .E = 0xEDE472EB,
                .CA2 = 0x7FFF,
                .CL0 = 0x3FF,
                .CA3 = 0x0,
                .I0 = 0x1,
                .CL1 = 0x1F,
                .I3 = 0x0,
                .I2 = 0xE,
                .CL2 = 0xE,
                .CA1 = 0x7FFF,
                .CA0 = 0x0,
                .CL3 = 0x7FFF,
                .I1 = 0x20,
                .RAMT = 0x50002020,
                .CSR = 0x00000001,
                .CMS = 0x01
            }
        },
        .tisciPBISTDeviceId     = TISCI_DEV_PBIST4,   /* PBIST device id  */
        .pollMode               = false,
        .interruptNumber        = SDLR_MCU_R5FSS0_CORE0_INTR_PBIST4_DFT_PBIST_CPU_0,
        .procRstNeeded          = true,
        .secondaryCoreNeeded    = false,                /* Secondary core needed */
        .coreName               = "NAVSS",              /* Primary core   */
        .tisciProcId            = 0x0u,                 /* No ProcId for NAVSS */
        .tisciDeviceId          = TISCI_DEV_NAVSS0,     /* NAVSS Device Id */
        .coreCustPwrSeqNeeded   = false,
        .numAuxDevices          = 0u,                   /* No Aux devices */
        .auxInitRestoreFunction = PBIST_NAVSSAuxInitRestore, /* Auxilliary init function */
        .doneFlag               = false,                /* Initialize done flag */
    },
    /* Codec_1 */
    {
        .testName            = "Codec PBIST",
        .numPostPbistToCheck = 0u,
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
        .numPBISTROMTestRuns = 1,
        .PBISTROMTestConfigRun = 
        {
            {
                .D = 0x0412F4CB,
                .E = 0x0412F4CB,
                .CA2 = 0x7FFF,
                .CL0 = 0x3FF,
                .CA3 = 0x0,
                .I0 = 0x1,
                .CL1 = 0x1F,
                .I3 = 0x0,
                .I2 = 0xE,
                .CL2 = 0xE,
                .CA1 = 0x7FFF,
                .CA0 = 0x0,
                .CL3 = 0x7FFF,
                .I1 = 0x20,
                .RAMT = 0x40002020,
                .CSR = 0x00000001,
                .CMS = 0x02
            }
        },
        .tisciPBISTDeviceId     = TISCI_DEV_PBIST11,   /* PBIST device id  */
        .interruptNumber        = SDLR_MCU_R5FSS0_CORE0_INTR_PBIST11_DFT_PBIST_CPU_0,
        .procRstNeeded          = false,
        .secondaryCoreNeeded    = false,                /* Secondary core needed */
        .coreName               = "Encoder",            /* Primary core          */
        .tisciProcId            = 0x0u,                 /* No ProcId for Encoder */
        .tisciDeviceId          = TISCI_DEV_CODEC0,   /* Encoder Device Id     */
        .coreCustPwrSeqNeeded   = false,
        .numAuxDevices          = CODEC_NUM_AUX_DEVICES,
        .auxDeviceIdsP          = &PBIST_CodecAuxDevList[0], /* Array of Aux device ids */
        .auxInitRestoreFunction = PBIST_CodecAuxInitRestore, /* Auxilliary init function */
        .doneFlag               = false,                /* Initialize done flag  */
    },
        /* GPU */
    {
        .testName            = "GPU PBIST",
        .numPostPbistToCheck = 0u,
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
        .numPBISTROMTestRuns = 1,
        .PBISTROMTestConfigRun = 
        {
            {
                .D = 0x875F0724,
                .E = 0x875F0724,
                .CA2 = 0x7FFF,
                .CL0 = 0x3FF,
                .CA3 = 0x0,
                .I0 = 0x1,
                .CL1 = 0x1F,
                .I3 = 0x0,
                .I2 = 0xE,
                .CL2 = 0xE,
                .CA1 = 0x7FFF,
                .CA0 = 0x0,
                .CL3 = 0x7FFF,
                .I1 = 0x20,
                .RAMT = 0x60002020,
                .CSR = 0x00000001,
                .CMS = 0x01
            }
        },
        .tisciPBISTDeviceId     = TISCI_DEV_J7AEP_GPU_BXS464_WRAP0_DFT_EMBED_PBIST_0,   /* PBIST device id  */
        .interruptNumber        = SDLR_MCU_R5FSS0_CORE0_INTR_AEP_GPU_BXS464_WRAP0_DFT_EMBED_PBIST_0_DFT_PBIST_CPU_0,
        .procRstNeeded          = false,
        .secondaryCoreNeeded    = false,                      /* Secondary core needed */
        .coreName               = "GPU",                      /* Primary core   */
        .tisciProcId            = 0x0u,                       /* No ProcId for GPU */
        .tisciDeviceId          = TISCI_DEV_J7AEP_GPU_BXS464_WRAP0_GPUCORE_0,   /* GPU Device Id */
        .coreCustPwrSeqNeeded   = false,
        .numAuxDevices          = GPU_NUM_AUX_DEVICES,        /* Number of Aux devices   */
        .auxDeviceIdsP          = &PBIST_GPUAuxDevList[0],    /* Array of Aux device ids */
        .auxInitRestoreFunction = PBIST_GPUAuxInitRestore,    /* Auxilliary init function */
        .doneFlag               = false,               /* Initialize done flag */
    },
    /* C7X_0 */
    {
        .testName            = "C7X_0 PBIST",
        .numPostPbistToCheck = 0u,
        .PBISTRegsHiAddress  = SDL_COMPUTE_CLUSTER0_DSP0_PBIST_BASE,
        .pPBISTRegs          = (SDL_pbistRegs *)PBIST_REGION_LOCAL_BASE,
        .numPBISTRuns        = SDL_COMPUTE_CLUSTER0_C71SS0_PBIST_WRAP_0_NUM_TEST_VECTORS,
        .PBISTConfigRun = {
            {
                .override           = 0x0u,
                /* Override bit set to 0 to use memoryGroupsBitMap & algorithmsBitMap */
                .algorithmsBitMap   = SDL_COMPUTE_CLUSTER0_C71SS0_PBIST_WRAP_0_ALGO_BITMAP_0,   /* Choose recommended Algo bitmap */
                .memoryGroupsBitMap = SDL_COMPUTE_CLUSTER0_C71SS0_PBIST_WRAP_0_MEM_BITMAP_0,    /* Choose recommended mem bitmap  */
                .scrambleValue      = 0xFEDCBA9876543210U,                  /* Scramble Value */
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
        .numPBISTROMTestRuns = 0,
        .PBISTROMTestConfigRun = 
        {
            {
                .D = 0x070FDDAF,
                .E = 0x070FDDAF,
                .CA2 = 0x3FFF,
                .CL0 = 0x3FF,
                .CA3 = 0x0,
                .I0 = 0x1,
                .CL1 = 0xF,
                .I3 = 0x0,
                .I2 = 0xD,
                .CL2 = 0xD,
                .CA1 = 0x3FFF,
                .CA0 = 0x0,
                .CL3 = 0x3FFF,
                .I1 = 0x10,
                .RAMT = 0xFF002010,
                .CSR = 0x00000001,
                .CMS = 0x01
            }
        },
        .tisciPBISTDeviceId     = TISCI_DEV_C71X_0_PBIST_VD,   /* PBIST device id  */
        .interruptNumber        = SDLR_MCU_R5FSS0_CORE0_INTR_COMPUTE_CLUSTER0_CLEC_SOC_EVENTS_OUT_LEVEL_12,
        .procRstNeeded          = true,
        .secondaryCoreNeeded    = false,               /* Secondary core needed */
        .coreName               = "C7x 0",
        .tisciProcId            = SCICLIENT_PROC_ID_COMPUTE_CLUSTER0_C71SS0_0,  /* C7x Proc Id */
        .tisciDeviceId          = TISCI_DEV_COMPUTE_CLUSTER0_C71SS0_0,    /* C7x Device Id */
        .coreCustPwrSeqNeeded   = false,
        .numAuxDevices          = C7x_0_NUM_AUX_DEVICES,                  /* Number of Aux devices */
        .auxDeviceIdsP          = &PBIST_C7x_0AuxDevList[0],
        .auxInitRestoreFunction = NULL,                /* Auxilliary init function */
        .doneFlag               = false,               /* Initialize done flag */
    },
    /* C7x_1 */
    {
        .testName            = "C7x_1 PBIST",
        .numPostPbistToCheck = 0u,
        .PBISTRegsHiAddress  = SDL_COMPUTE_CLUSTER0_DSP1_PBIST_BASE,
        .pPBISTRegs          = (SDL_pbistRegs *)PBIST_REGION_LOCAL_BASE,
        .numPBISTRuns        = SDL_COMPUTE_CLUSTER0_C71SS1_PBIST_WRAP_0_NUM_TEST_VECTORS,
        .PBISTConfigRun = {
            {
                .override           = 0x0u,
                /* Override bit set to 0 to use memoryGroupsBitMap & algorithmsBitMap */
                .algorithmsBitMap   = SDL_COMPUTE_CLUSTER0_C71SS1_PBIST_WRAP_0_ALGO_BITMAP_0,   /* Choose recommended Algo bitmap */
                .memoryGroupsBitMap = SDL_COMPUTE_CLUSTER0_C71SS1_PBIST_WRAP_0_MEM_BITMAP_0,    /* Choose recommended mem bitmap  */
                .scrambleValue      = 0xFEDCBA9876543210U,                  /* Scramble Value */
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
        .numPBISTROMTestRuns = 0,
        .PBISTROMTestConfigRun = 
        {
            {
                .D = 0x070FDDAF,
                .E = 0x070FDDAF,
                .CA2 = 0x3FFF,
                .CL0 = 0x3FF,
                .CA3 = 0x0,
                .I0 = 0x1,
                .CL1 = 0xF,
                .I3 = 0x0,
                .I2 = 0xD,
                .CL2 = 0xD,
                .CA1 = 0x3FFF,
                .CA0 = 0x0,
                .CL3 = 0x3FFF,
                .I1 = 0x10,
                .RAMT = 0xFF002010,
                .CSR = 0x00000001,
                .CMS = 0x01
            }
        },
        .tisciPBISTDeviceId     = TISCI_DEV_C71X_1_PBIST_VD,   /* PBIST device id  */
        .interruptNumber        = SDLR_MCU_R5FSS0_CORE0_INTR_COMPUTE_CLUSTER0_CLEC_SOC_EVENTS_OUT_LEVEL_13,
        .procRstNeeded          = true,
        .secondaryCoreNeeded    = false,               /* Secondary core needed */
        .coreName               = "C7x 1",
        .tisciProcId            = SCICLIENT_PROC_ID_COMPUTE_CLUSTER0_C71SS1_0,  /* C7x Proc Id */
        .tisciDeviceId          = TISCI_DEV_COMPUTE_CLUSTER0_C71SS1_0,    /* C7x Device Id */
        .coreCustPwrSeqNeeded   = false,
        .numAuxDevices          = C7x_1_NUM_AUX_DEVICES,                  /* Number of Aux devices */
        .auxDeviceIdsP          = &PBIST_C7x_1AuxDevList[0],
        .auxInitRestoreFunction = NULL,                /* Auxilliary init function */
        .doneFlag               = false,               /* Initialize done flag */
    },
    /* A72 */
    {
        .testName            = "A72 PBIST",
        .numPostPbistToCheck = 0u,
        .PBISTRegsHiAddress  = SDL_COMPUTE_CLUSTER0_MPU_PBIST0_BASE,
        .pPBISTRegs          = (SDL_pbistRegs *)PBIST_REGION_LOCAL_BASE,
        .numPBISTRuns        = SDL_A72SS0_CORE0_PBIST_WRAP_NUM_TEST_VECTORS,
        .PBISTConfigRun = {
            {
                .override           = 0x0u,
                /* Override bit set to 0 to use memoryGroupsBitMap & algorithmsBitMap */
                .algorithmsBitMap   = SDL_A72SS0_CORE0_PBIST_WRAP_ALGO_BITMAP_0,  /* Choose recommended Algo bitmap */
                .memoryGroupsBitMap = SDL_A72SS0_CORE0_PBIST_WRAP_MEM_BITMAP_0,   /* Choose recommended mem bitmap  */
                .scrambleValue      = 0xFEDCBA9876543210U, /* Scramble Value */
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
        .numPBISTROMTestRuns = 0,
        .PBISTROMTestConfigRun = 
        {
            {
                .D = 0xAA375C33,
                .E = 0xAA375C33,
                .CA2 = 0x3FFF,
                .CL0 = 0x3FF,
                .CA3 = 0x0,
                .I0 = 0x1,
                .CL1 = 0xF,
                .I3 = 0x0,
                .I2 = 0xD,
                .CL2 = 0xD,
                .CA1 = 0x3FFF,
                .CA0 = 0x0,
                .CL3 = 0x3FFF,
                .I1 = 0x10,
                .RAMT = 0xFF002010,
                .CSR = 0x00000001,
                .CMS = 0x01
            }
        },
        .tisciPBISTDeviceId     = TISCI_DEV_COMPUTE_CLUSTER0_PBIST_WRAP_0, /* Device Id for A72 PBIST */
        .pollMode               = false,
        .interruptNumber        = SDLR_MCU_R5FSS0_CORE0_INTR_COMPUTE_CLUSTER0_CLEC_SOC_EVENTS_OUT_LEVEL_8,
        .procRstNeeded          = true,
        .secondaryCoreNeeded    = true,                /* Secondary core needed */
        .coreName               = "A72 core 0",        /* Primary core   */
        .secCoreName            = "A72 core 1",        /* Secondary core */
        .tisciProcId            = SCICLIENT_PROC_ID_A72SS0_CORE0,  /* A72 core 0 Proc Id */
        .tisciSecProcId         = SCICLIENT_PROC_ID_A72SS0_CORE1,  /* A72 core 1 Proc Id */
        .tisciDeviceId          = TISCI_DEV_A72SS0_CORE0,  /* A72 core 0 Device Id */
        .tisciSecDeviceId       = TISCI_DEV_A72SS0_CORE1,  /* A72 core 1 Device Id */
        .coreCustPwrSeqNeeded   = true,                    /* A72 needs custom powerdown sequence steps */
        .numAuxDevices          = A72_NUM_AUX_DEVICES,     /* Number of Aux devices   */
        .auxDeviceIdsP          = &PBIST_A72AuxDevList[0], /* Array of Aux device ids */
        .auxInitRestoreFunction = PBIST_A72AuxInitRestore, /* Auxilliary init function */
        .doneFlag               = false,                   /* Initialize done flag */
    },
    /* MSMC */
    {
        .testName            = "MSMC PBIST",
        .numPostPbistToCheck = 0u,
        .PBISTRegsHiAddress  = SDL_COMPUTE_CLUSTER0_MSMC_PBIST0_BASE,
        .pPBISTRegs          = (SDL_pbistRegs *)PBIST_REGION_LOCAL_BASE,
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
            },
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
        .numPBISTROMTestRuns = 1,
        .PBISTROMTestConfigRun = 
        {
            {
                .D = 0x4DFF8732,
                .E = 0x4DFF8732,
                .CA2 = 0x3FFF,
                .CL0 = 0x3FF,
                .CA3 = 0x0,
                .I0 = 0x1,
                .CL1 = 0xF,
                .I3 = 0x0,
                .I2 = 0xD,
                .CL2 = 0xD,
                .CA1 = 0x3FFF,
                .CA0 = 0x0,
                .CL3 = 0x3FFF,
                .I1 = 0x10,
                .RAMT = 0xFF002010,
                .CSR = 0x00000001,
                .CMS = 0x02
            }
        },
        .tisciPBISTDeviceId     = TISCI_DEV_COMPUTE_CLUSTER0_PBIST_WRAP_0,   /* PBIST device id  */
        .pollMode               = false,
        .interruptNumber        = SDLR_MCU_R5FSS0_CORE0_INTR_COMPUTE_CLUSTER0_PBIST_WRAP_0_DFT_PBIST_CPU_0,
        .procRstNeeded          = true,
        .secondaryCoreNeeded    = true,                    /* Secondary core needed */
        .coreName               = "A72 core 0",            /* Primary core   */
        .secCoreName            = "A72 core 1",            /* Secondary core */
        .tisciProcId            = SCICLIENT_PROC_ID_A72SS0_CORE0, /* A72 core 0 Proc Id */
        .tisciSecProcId         = SCICLIENT_PROC_ID_A72SS0_CORE1, /* A72 core 1 Proc Id */
        .tisciDeviceId          = TISCI_DEV_A72SS0_CORE0,         /* A72 core 0 Device Id */
        .tisciSecDeviceId       = TISCI_DEV_A72SS0_CORE1,         /* A72 core 1 Device Id */
        .coreCustPwrSeqNeeded   = true,
        .numAuxDevices          = 0u,                             /* No Aux devices       */
        .auxInitRestoreFunction = NULL,                           /* Auxilliary init function */
        .doneFlag               = false,                          /* Initialize done flag */
    },
    /* MCU PULSAR PBIST */
    {
        .testName            = "MCU PULSAR PBIST",
        .numPostPbistToCheck = 0u,
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
        .numPBISTROMTestRuns = 1,
        .PBISTROMTestConfigRun = 
        {
            {
                .D = 0xCAC5A694,
                .E = 0xCAC5A694,
                .CA2 = 0x7FFF,
                .CL0 = 0x3FF,
                .CA3 = 0x0,
                .I0 = 0x1,
                .CL1 = 0x1F,
                .I3 = 0x0,
                .I2 = 0xE,
                .CL2 = 0xE,
                .CA1 = 0x7FFF,
                .CA0 = 0x0,
                .CL3 = 0x7FFF,
                .I1 = 0x20,
                .RAMT = 0x08002020,
                .CSR = 0x00000001,
                .CMS = 0x01
            }
        },
        .tisciPBISTDeviceId     = TISCI_DEV_MCU_PBIST2, /* PBIST device id  */
        .interruptNumber        = SDLR_MCU_R5FSS0_CORE0_INTR_MCU_PBIST2_DFT_PBIST_CPU_0,
        .procRstNeeded          = false,
        .secondaryCoreNeeded    = false,                /* Secondary core needed */
        .coreCustPwrSeqNeeded   = false,
        .numAuxDevices          = 0u,                   /* No Aux devices        */
        .auxInitRestoreFunction = NULL,                 /* Auxilliary init function */
        .doneFlag               = false,                /* Initialize done flag */
    },

    /* MCU PBIST 0 - Only select memory guaranteed not to be utilized by this test application
     * can be run, since PBIST changes the memory upon which it is run. */
    {
        .testName            = "MCU PBIST",
        .numPostPbistToCheck = 1u,
        .PBISTRegsHiAddress  = 0u,
        .pPBISTRegs          = (SDL_pbistRegs *)SDL_MCU_PBIST0_BASE,
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
            }
        },
#else // Golden Vectors not currently supported running from MCU core as this will be self destructive
        .numPBISTRuns        = SDL_MCU_PBIST0_NUM_TEST_VECTORS,
        .PBISTConfigRun = {
            /* Golden Vector Part 1 */
            {
                .override           = 0x0u,
                /* Override bit set to 0 to use memoryGroupsBitMap & algorithmsBitMap */
                /* NOTE: As the MCU R5f is running this code override bit cannot be set to enable all memory test */
                .algorithmsBitMap   = SDL_MCU_PBIST0_ALGO_BITMAP_0,   /* Choose recommended Algo bitmap 0 */
                .memoryGroupsBitMap = SDL_MCU_PBIST0_MEM_BITMAP_0     /* Choose recommended mem bitmap 0  */
                .scrambleValue      = 0xFEDCBA9876543210U,  /* Scramble Value */
            },
            /* Golden Vector Part 2 */
            {
                .override           = 0x0u,
                /* Override bit set to 0 to use memoryGroupsBitMap & algorithmsBitMap */
                /* NOTE: As the MCU R5f is running this code override bit cannot be set to enable all memory test */
                .algorithmsBitMap   = SDL_MCU_PBIST0_ALGO_BITMAP_1,   /* Choose recommended Algo bitmap 1 */
                .memoryGroupsBitMap = SDL_MCU_PBIST0_MEM_BITMAP_1,    /* Choose recommended mem bitmap 1  */
                .scrambleValue      = 0xFEDCBA9876543210U,  /* Scramble Value */
            }
        },
#endif
        .PBISTNegConfigRun = {
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
        .numPBISTROMTestRuns = 1,
        .PBISTROMTestConfigRun = 
        {
            {
                /* mcu 0 */
                .D = 0x2936BA4B,
                .E = 0x2936BA4B,
                .CA2 = 0x7FFF,
                .CL0 = 0x3FF,
                .CA3 = 0x0,
                .I0 = 0x1,
                .CL1 = 0x1F,
                .I3 = 0x0,
                .I2 = 0xE,
                .CL2 = 0xE,
                .CA1 = 0x7FFF,
                .CA0 = 0x0,
                .CL3 = 0x7FFF,
                .I1 = 0x20,
                .RAMT = 0x10002020,
                .CSR = 0x00000001,
                .CMS = 0x03
            },
            {
                /* MCU PSROM Vector 0 */
                .D = 0x6B41975C,
                .E = 0x6B41975C,
                .CA2 = 0xFFF,
                .CL0 = 0xFF,
                .CA3 = 0x0,
                .I0 = 0x1,
                .CL1 = 0xF,
                .I3 = 0x0,
                .I2 = 0xB,
                .CL2 = 0xB,
                .CA1 = 0xFFF,
                .CA0 = 0x0,
                .CL3 = 0xFFF,
                .I1 = 0x10,
                .RAMT = 0x00002024,
                .CSR = 0x00000001,
                .CMS = 0x01
            },
            {
                /* MCU PSROM Vector 1 */
                .D = 0xEA43DEA7,
                .E = 0xEA43DEA7,
                .CA2 = 0xFFF,
                .CL0 = 0xFF,
                .CA3 = 0x0,
                .I0 = 0x1,
                .CL1 = 0xF,
                .I3 = 0x0,
                .I2 = 0xB,
                .CL2 = 0xB,
                .CA1 = 0xFFF,
                .CA0 = 0x0,
                .CL3 = 0xFFF,
                .I1 = 0x10,
                .RAMT = 0x00012024,
                .CSR = 0x00000002
            },
            {
                /* MCU PSROM Vector 2 */
                .D = 0xF175E2E7,
                .E = 0xF175E2E7,
                .CA2 = 0xFFF,
                .CL0 = 0xFF,
                .CA3 = 0x0,
                .I0 = 0x1,
                .CL1 = 0xF,
                .I3 = 0x0,
                .I2 = 0xB,
                .CL2 = 0xB,
                .CA1 = 0xFFF,
                .CA0 = 0x0,
                .CL3 = 0xFFF,
                .I1 = 0x10,
                .RAMT = 0x00022024,
                .CSR = 0x00000004
            },
            {
                /* MCU PSROM Vector 3 */
                .D = 0x2FAF7E92,
                .E = 0x2FAF7E92,
                .CA2 = 0xFFF,
                .CL0 = 0xFF,
                .CA3 = 0x0,
                .I0 = 0x1,
                .CL1 = 0xF,
                .I3 = 0x0,
                .I2 = 0xB,
                .CL2 = 0xB,
                .CA1 = 0xFFF,
                .CA0 = 0x0,
                .CL3 = 0xFFF,
                .I1 = 0x10,
                .RAMT = 0x00032024,
                .CSR = 0x00000008
            },
            {
                /* MCU PSROM Vector 4 */
                .D = 0x6D9AF966,
                .E = 0x6D9AF966,
                .CA2 = 0xFFF,
                .CL0 = 0xFF,
                .CA3 = 0x0,
                .I0 = 0x1,
                .CL1 = 0xF,
                .I3 = 0x0,
                .I2 = 0xB,
                .CL2 = 0xB,
                .CA1 = 0xFFF,
                .CA0 = 0x0,
                .CL3 = 0xFFF,
                .I1 = 0x10,
                .RAMT = 0x00042024,
                .CSR = 0x00000010
            },
            {
                /* MCU PSROM Vector 5 */
                .D = 0x98FA4FB9,
                .E = 0x98FA4FB9,
                .CA2 = 0xFFF,
                .CL0 = 0xFF,
                .CA3 = 0x0,
                .I0 = 0x1,
                .CL1 = 0xF,
                .I3 = 0x0,
                .I2 = 0xB,
                .CL2 = 0xB,
                .CA1 = 0xFFF,
                .CA0 = 0x0,
                .CL3 = 0xFFF,
                .I1 = 0x10,
                .RAMT = 0x00052024,
                .CSR = 0x00000020
            },
            {
                /* MCU PSROM Vector 6 */
                .D = 0x267CDE89,
                .E = 0x267CDE89,
                .CA2 = 0xFFF,
                .CL0 = 0xFF,
                .CA3 = 0x0,
                .I0 = 0x1,
                .CL1 = 0xF,
                .I3 = 0x0,
                .I2 = 0xB,
                .CL2 = 0xB,
                .CA1 = 0xFFF,
                .CA0 = 0x0,
                .CL3 = 0xFFF,
                .I1 = 0x10,
                .RAMT = 0x00062024,
                .CSR = 0x00000040
            },
            {
                /* MCU PSROM Vector 7 */
                .D = 0x50344C09,
                .E = 0x50344C09,
                .CA2 = 0xFFF,
                .CL0 = 0xFF,
                .CA3 = 0x0,
                .I0 = 0x1,
                .CL1 = 0xF,
                .I3 = 0x0,
                .I2 = 0xB,
                .CL2 = 0xB,
                .CA1 = 0xFFF,
                .CA0 = 0x0,
                .CL3 = 0xFFF,
                .I1 = 0x10,
                .RAMT = 0x00072024,
                .CSR = 0x00000080
            },
            {
                /* MCU PSROM Vector 8 */
                .D = 0xB5BE0F34,
                .E = 0xB5BE0F34,
                .CA2 = 0xFFF,
                .CL0 = 0xFF,
                .CA3 = 0x0,
                .I0 = 0x1,
                .CL1 = 0xF,
                .I3 = 0x0,
                .I2 = 0xB,
                .CL2 = 0xB,
                .CA1 = 0xFFF,
                .CA0 = 0x0,
                .CL3 = 0xFFF,
                .I1 = 0x10,
                .RAMT = 0x00082024,
                .CSR = 0x00000100
            },
            {
                /* MCU PSROM Vector 9 */
                .D = 0x3C45A0EF,
                .E = 0x3C45A0EF,
                .CA2 = 0xFFF,
                .CL0 = 0xFF,
                .CA3 = 0x0,
                .I0 = 0x1,
                .CL1 = 0xF,
                .I3 = 0x0,
                .I2 = 0xB,
                .CL2 = 0xB,
                .CA1 = 0xFFF,
                .CA0 = 0x0,
                .CL3 = 0xFFF,
                .I1 = 0x10,
                .RAMT = 0x00092024,
                .CSR = 0x00000200
            },
            {
                /* MCU PSROM Vector 10 */
                .D = 0x502B0D04,
                .E = 0x502B0D04,
                .CA2 = 0xFFF,
                .CL0 = 0xFF,
                .CA3 = 0x0,
                .I0 = 0x1,
                .CL1 = 0xF,
                .I3 = 0x0,
                .I2 = 0xB,
                .CL2 = 0xB,
                .CA1 = 0xFFF,
                .CA0 = 0x0,
                .CL3 = 0xFFF,
                .I1 = 0x10,
                .RAMT = 0x000a2024,
                .CSR = 0x00000400
            },
            {
                /* MCU PSROM Vector 11 */
                .D = 0x13B61DC7,
                .E = 0x13B61DC7,
                .CA2 = 0xFFF,
                .CL0 = 0xFF,
                .CA3 = 0x0,
                .I0 = 0x1,
                .CL1 = 0xF,
                .I3 = 0x0,
                .I2 = 0xB,
                .CL2 = 0xB,
                .CA1 = 0xFFF,
                .CA0 = 0x0,
                .CL3 = 0xFFF,
                .I1 = 0x10,
                .RAMT = 0x000b2024,
                .CSR = 0x00000800
            },
        },
        .tisciPBISTDeviceId     = TISCI_DEV_MCU_PBIST0, /* PBIST device id  */
        .pollMode               = false,
        .interruptNumber        = SDLR_MCU_R5FSS0_CORE0_INTR_MCU_PBIST0_DFT_PBIST_CPU_0,
        .procRstNeeded          = false,
        .secondaryCoreNeeded    = false,                /* Secondary core needed */
        .coreCustPwrSeqNeeded   = false,
        .numAuxDevices          = 0u,                   /* No Aux devices */
        .auxInitRestoreFunction = NULL,                 /* Auxilliary init function */
        .doneFlag               = false,                /* InitRestoreialize done flag */
    },
    /* MCU PBIST 1 - Only select memory guaranteed not to be utilized by this test application
     * can be run, since PBIST changes the memory upon which it is run. */
    {
        .testName            = "MCU PBIST 1",
        .numPostPbistToCheck = 1u,
        .PBISTRegsHiAddress  = 0u,
        .pPBISTRegs          = (SDL_pbistRegs *)SDL_MCU_PBIST1_BASE,
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
            }
        },
#else // Golden Vectors not currently supported running from MCU core as this will be self destructive
        .numPBISTRuns        = SDL_MCU_PBIST1_NUM_TEST_VECTORS,
        .PBISTConfigRun = {
            /* Golden Vector Part 1 */
            {
                .override           = 0x0u,
                /* Override bit set to 0 to use memoryGroupsBitMap & algorithmsBitMap */
                /* NOTE: As the MCU R5f is running this code override bit cannot be set to enable all memory test */
                .algorithmsBitMap   = SDL_MCU_PBIST1_ALGO_BITMAP_0,   /* Choose recommended Algo bitmap 0 */
                .memoryGroupsBitMap = SDL_MCU_PBIST1_MEM_BITMAP_0     /* Choose recommended mem bitmap 0  */
                .scrambleValue      = 0xFEDCBA9876543210U,  /* Scramble Value */
            },
            /* Golden Vector Part 2 */
            {
                .override           = 0x0u,
                /* Override bit set to 0 to use memoryGroupsBitMap & algorithmsBitMap */
                /* NOTE: As the MCU R5f is running this code override bit cannot be set to enable all memory test */
                .algorithmsBitMap   = SDL_MCU_PBIST1_ALGO_BITMAP_1,   /* Choose recommended Algo bitmap 1 */
                .memoryGroupsBitMap = SDL_MCU_PBIST1_MEM_BITMAP_1,    /* Choose recommended mem bitmap 1  */
                .scrambleValue      = 0xFEDCBA9876543210U,  /* Scramble Value */
            }
        },
#endif
        .PBISTNegConfigRun = {
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
        .numPBISTROMTestRuns = 1,
        .PBISTROMTestConfigRun = 
        {
            {
                .D = 0xCD031F43,
                .E = 0xCD031F43,
                .CA2 = 0x7FFF,
                .CL0 = 0x3FF,
                .CA3 = 0x0,
                .I0 = 0x1,
                .CL1 = 0x1F,
                .I3 = 0x0,
                .I2 = 0xE,
                .CL2 = 0xE,
                .CA1 = 0x7FFF,
                .CA0 = 0x0,
                .CL3 = 0x7FFF,
                .I1 = 0x20,
                .RAMT = 0x2C002020,
                .CSR = 0x00000001,
                .CMS = 0x05
            }
        },
        .tisciPBISTDeviceId     = TISCI_DEV_MCU_PBIST1, /* PBIST device id  */
        .pollMode               = false,
        .interruptNumber        = SDLR_MCU_R5FSS0_CORE0_INTR_MCU_PBIST1_DFT_PBIST_CPU_0,
        .procRstNeeded          = false,
        .secondaryCoreNeeded    = false,                /* Secondary core needed */
        .coreCustPwrSeqNeeded   = false,
        .numAuxDevices          = 0u,                   /* No Aux devices */
        .auxInitRestoreFunction = NULL,                 /* Auxilliary init function */
        .doneFlag               = false,                /* InitRestoreialize done flag */
    },
    /* DSS */
    {
        .testName            = "DSS EDP PBIST",
        .numPostPbistToCheck = 0u,
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
        .numPBISTROMTestRuns = 1,
        .PBISTROMTestConfigRun = 
        {
            {
                .D = 0x5771A906,
                .E = 0x5771A906,
                .CA2 = 0x7FFF,
                .CL0 = 0x3FF,
                .CA3 = 0x0,
                .I0 = 0x1,
                .CL1 = 0x1F,
                .I3 = 0x0,
                .I2 = 0xE,
                .CL2 = 0xE,
                .CA1 = 0x7FFF,
                .CA0 = 0x0,
                .CL3 = 0x7FFF,
                .I1 = 0x20,
                .RAMT = 0x38002020,
                .CSR = 0x00000001,
                .CMS = 0x05
            }
        },
        .tisciPBISTDeviceId     = TISCI_DEV_PBIST5,   /* PBIST device id  */
        .interruptNumber        = SDLR_MCU_R5FSS0_CORE0_INTR_PBIST5_DFT_PBIST_CPU_0,
        .procRstNeeded          = true,
        .secondaryCoreNeeded    = false,                /* Secondary core needed */
        .coreName               = "DSS",               /* Primary core   */
        .tisciProcId            = 0x0u,                 /* No ProcId for MSMC */
        .tisciDeviceId          = TISCI_DEV_DSS_DSI0, /* DSS Device Id */
        .coreCustPwrSeqNeeded   = false,
        .numAuxDevices          = DSS_NUM_AUX_DEVICES,     /* Number of Aux devices   */
        .auxDeviceIdsP          = &PBIST_DSSAuxDevList[0], /* Array of Aux device ids */
        .auxInitRestoreFunction = PBIST_DSSAuxInitRestore, /* Auxilliary init function */
        .doneFlag               = false,               /* Initialize done flag */
    },
};

/* HW POST-related functions */

int32_t PBIST_isPostPbistTimeout(uint32_t postStatMmrRegVal, bool *pIsTimedOut)
{
    int32_t status = SDL_PASS;

    if (pIsTimedOut == NULL)
    {
        status = SDL_EBADARGS;
    }
    else
    {
        *pIsTimedOut = ((postStatMmrRegVal >>
                         SDL_WKUP_CTRL_MMR_CFG0_WKUP_POST_STAT_POST_MCU_PBIST_TIMEOUT_SHIFT) &
                         0x1u) ? TRUE : FALSE;
    }
    return status;
}

int32_t PBIST_isPostPbistDone(uint32_t postStatMmrRegVal, bool *pIsDone)
{
    int32_t status = SDL_PASS;

    if (pIsDone == NULL)
    {
        status = SDL_EBADARGS;
    }
    else
    {
        *pIsDone = ((postStatMmrRegVal >>
                    SDL_WKUP_CTRL_MMR_CFG0_WKUP_POST_STAT_POST_MCU_PBIST_DONE_SHIFT) &
                    0x1u) ? TRUE : FALSE;
    }
    return status;
}

int32_t PBIST_postCheckResult(uint32_t postStatMmrRegVal, bool *pResult)
{
    int32_t cslResult= SDL_PASS;

    if(pResult == NULL)
    {
        cslResult = SDL_EFAIL;
    }
    else
    {
        if ((postStatMmrRegVal &
            SDL_WKUP_CTRL_MMR_CFG0_WKUP_POST_STAT_POST_MCU_PBIST_FAIL_MASK) ==
            ((uint32_t)0x00000000u))
        {
            *pResult = TRUE;
        }
        else
        {
            *pResult = FALSE;
        }
    }

    return  cslResult;
}


/* PBIST_setFirewall: Sets firewall settings to be able to access CLEC registers */
static int32_t PBIST_setFirewall(void)
{
    int32_t retVal = SDL_PASS;
    uint32_t reqFlag = TISCI_MSG_FLAG_AOP | TISCI_MSG_FLAG_DEVICE_EXCLUSIVE;
    uint32_t timeout =  SCICLIENT_SERVICE_WAIT_FOREVER;
    struct  tisci_msg_fwl_set_firewall_region_req request;
    Sciclient_ReqPrm_t reqParam;
    Sciclient_RespPrm_t respParam;

    request.fwl_id       = (uint32_t)CSL_STD_FW_NAVSS0_VIRTSS_NB_SLV0_MEM0_ID;
    request.region = (uint32_t) 1U; /* Pick up any unused region : 1
                                       NOTE: region 0 is used by default by
                                             System firmware for MSMC Memory currently*/
    request.n_permission_regs = CSL_FW_NUM_CBASS_FW_EP_REGION_PERMISSION;
    request.control = (FW_REGION_ENABLE & CSL_CBASS_ISC_EP_REGION_CONTROL_ENABLE_MASK);
    request.permissions[0] = (FW_MCU_R5F0_PRIVID << CSL_CBASS_FW_EP_REGION_PERMISSION_PRIV_ID_SHIFT)
                             | (1U << CSL_CBASS_FW_EP_REGION_PERMISSION_SEC_SUPV_WRITE_SHIFT)
                             | (1U << CSL_CBASS_FW_EP_REGION_PERMISSION_SEC_SUPV_READ_SHIFT)
                             | (0U << CSL_CBASS_FW_EP_REGION_PERMISSION_SEC_SUPV_CACHEABLE_SHIFT)
                             | (1U << CSL_CBASS_FW_EP_REGION_PERMISSION_SEC_SUPV_DEBUG_SHIFT)
                             | (1U << CSL_CBASS_FW_EP_REGION_PERMISSION_SEC_USER_WRITE_SHIFT)
                             | (1U << CSL_CBASS_FW_EP_REGION_PERMISSION_SEC_USER_READ_SHIFT)
                             | (0U << CSL_CBASS_FW_EP_REGION_PERMISSION_SEC_USER_CACHEABLE_SHIFT)
                             | (1U << CSL_CBASS_FW_EP_REGION_PERMISSION_SEC_USER_DEBUG_SHIFT)
                             | (1U << CSL_CBASS_FW_EP_REGION_PERMISSION_NONSEC_SUPV_WRITE_SHIFT)
                             | (1U << CSL_CBASS_FW_EP_REGION_PERMISSION_NONSEC_SUPV_READ_SHIFT)
                             | (0U << CSL_CBASS_FW_EP_REGION_PERMISSION_NONSEC_SUPV_CACHEABLE_SHIFT)
                             | (1U << CSL_CBASS_FW_EP_REGION_PERMISSION_NONSEC_SUPV_DEBUG_SHIFT)
                             | (1U << CSL_CBASS_FW_EP_REGION_PERMISSION_NONSEC_USER_WRITE_SHIFT)
                             | (1U << CSL_CBASS_FW_EP_REGION_PERMISSION_NONSEC_USER_READ_SHIFT)
                             | (0U << CSL_CBASS_FW_EP_REGION_PERMISSION_NONSEC_USER_CACHEABLE_SHIFT)
                             | (1U << CSL_CBASS_FW_EP_REGION_PERMISSION_NONSEC_USER_DEBUG_SHIFT);
    request.permissions[1] = (FW_MCU_R5F0_PRIVID << CSL_CBASS_FW_EP_REGION_PERMISSION_PRIV_ID_SHIFT)
                             | (1U << CSL_CBASS_FW_EP_REGION_PERMISSION_SEC_SUPV_WRITE_SHIFT)
                             | (1U << CSL_CBASS_FW_EP_REGION_PERMISSION_SEC_SUPV_READ_SHIFT)
                             | (0U << CSL_CBASS_FW_EP_REGION_PERMISSION_SEC_SUPV_CACHEABLE_SHIFT)
                             | (1U << CSL_CBASS_FW_EP_REGION_PERMISSION_SEC_SUPV_DEBUG_SHIFT)
                             | (1U << CSL_CBASS_FW_EP_REGION_PERMISSION_SEC_USER_WRITE_SHIFT)
                             | (1U << CSL_CBASS_FW_EP_REGION_PERMISSION_SEC_USER_READ_SHIFT)
                             | (0U << CSL_CBASS_FW_EP_REGION_PERMISSION_SEC_USER_CACHEABLE_SHIFT)
                             | (1U << CSL_CBASS_FW_EP_REGION_PERMISSION_SEC_USER_DEBUG_SHIFT)
                             | (1U << CSL_CBASS_FW_EP_REGION_PERMISSION_NONSEC_SUPV_WRITE_SHIFT)
                             | (1U << CSL_CBASS_FW_EP_REGION_PERMISSION_NONSEC_SUPV_READ_SHIFT)
                             | (0U << CSL_CBASS_FW_EP_REGION_PERMISSION_NONSEC_SUPV_CACHEABLE_SHIFT)
                             | (1U << CSL_CBASS_FW_EP_REGION_PERMISSION_NONSEC_SUPV_DEBUG_SHIFT)
                             | (1U << CSL_CBASS_FW_EP_REGION_PERMISSION_NONSEC_USER_WRITE_SHIFT)
                             | (1U << CSL_CBASS_FW_EP_REGION_PERMISSION_NONSEC_USER_READ_SHIFT)
                             | (0U << CSL_CBASS_FW_EP_REGION_PERMISSION_NONSEC_USER_CACHEABLE_SHIFT)
                             | (1U << CSL_CBASS_FW_EP_REGION_PERMISSION_NONSEC_USER_DEBUG_SHIFT);
    request.permissions[2] = (FW_MCU_R5F0_PRIVID << CSL_CBASS_FW_EP_REGION_PERMISSION_PRIV_ID_SHIFT)
                             | (1U << CSL_CBASS_FW_EP_REGION_PERMISSION_SEC_SUPV_WRITE_SHIFT)
                             | (1U << CSL_CBASS_FW_EP_REGION_PERMISSION_SEC_SUPV_READ_SHIFT)
                             | (0U << CSL_CBASS_FW_EP_REGION_PERMISSION_SEC_SUPV_CACHEABLE_SHIFT)
                             | (1U << CSL_CBASS_FW_EP_REGION_PERMISSION_SEC_SUPV_DEBUG_SHIFT)
                             | (1U << CSL_CBASS_FW_EP_REGION_PERMISSION_SEC_USER_WRITE_SHIFT)
                             | (1U << CSL_CBASS_FW_EP_REGION_PERMISSION_SEC_USER_READ_SHIFT)
                             | (0U << CSL_CBASS_FW_EP_REGION_PERMISSION_SEC_USER_CACHEABLE_SHIFT)
                             | (1U << CSL_CBASS_FW_EP_REGION_PERMISSION_SEC_USER_DEBUG_SHIFT)
                             | (1U << CSL_CBASS_FW_EP_REGION_PERMISSION_NONSEC_SUPV_WRITE_SHIFT)
                             | (1U << CSL_CBASS_FW_EP_REGION_PERMISSION_NONSEC_SUPV_READ_SHIFT)
                             | (0U << CSL_CBASS_FW_EP_REGION_PERMISSION_NONSEC_SUPV_CACHEABLE_SHIFT)
                             | (1U << CSL_CBASS_FW_EP_REGION_PERMISSION_NONSEC_SUPV_DEBUG_SHIFT)
                             | (1U << CSL_CBASS_FW_EP_REGION_PERMISSION_NONSEC_USER_WRITE_SHIFT)
                             | (1U << CSL_CBASS_FW_EP_REGION_PERMISSION_NONSEC_USER_READ_SHIFT)
                             | (0U << CSL_CBASS_FW_EP_REGION_PERMISSION_NONSEC_USER_CACHEABLE_SHIFT)
                             | (1U << CSL_CBASS_FW_EP_REGION_PERMISSION_NONSEC_USER_DEBUG_SHIFT);
    request.start_address = CSL_COMPUTE_CLUSTER0_CLEC_REGS_BASE;
    request.end_address = CSL_COMPUTE_CLUSTER0_CLEC_REGS_BASE + CSL_COMPUTE_CLUSTER0_CLEC_REGS_SIZE;

    reqParam.messageType    = (uint16_t) TISCI_MSG_SET_FWL_REGION;
    reqParam.flags          = (uint32_t) reqFlag;
    reqParam.pReqPayload    = (const uint8_t *) &request;
    reqParam.reqPayloadSize = (uint32_t) sizeof (request);
    reqParam.timeout        = (uint32_t) timeout;

    respParam.flags           = (uint32_t) 0;   /* Populated by the API */
    respParam.pRespPayload    = (uint8_t *) 0;
    respParam.respPayloadSize = (uint32_t) 0;


    if (((reqFlag & TISCI_MSG_FLAG_AOP) != TISCI_MSG_FLAG_AOP)&&
        (reqFlag != 0U))
    {
        retVal = SDL_EFAIL;
    }
    if (retVal == SDL_PASS)
    {
        retVal = Sciclient_service(&reqParam, &respParam);
    }
    if ((retVal != SDL_PASS) ||
        ((reqFlag != 0U) &&
        ((respParam.flags & TISCI_MSG_FLAG_ACK) != TISCI_MSG_FLAG_ACK)))
    {
        retVal = SDL_EFAIL;
    }
    return retVal;
}

bool error_test_flag = false;
bool error_test_flag_2 = false;

int32_t SDL_OSAL_getClecOutputEvent(uint32_t *clecOutputEvt)
{
    int32_t retVal = SDL_PASS;

    *clecOutputEvt = CSLR_COMPUTE_CLUSTER0_CLEC_MSMC_EVENT_IN_COMPUTE_CLUSTER0_CORE_CORE_MSMC_INTR_12;

    if (error_test_flag == true)
    {
        *clecOutputEvt = 0x0u;
    }
    else if (error_test_flag_2 == true)
    {
        retVal = SDL_EFAIL;
    }
    return retVal;
}

int32_t SDL_OSAL_releaseClecOutputEvent(uint32_t clecOutputEvt)
{
    return SDL_PASS;
}

/* Captures common Initialization: currently initializes firewall for gaining access to CLEC registers */
int32_t PBIST_commonInit(void)
{
    SDL_ErrType_t status;
    int32_t retValue = 0;
    CSL_ClecEventConfig evtCfg;

    /* Add firewall entry to gain access to CLEC registers */
    status = PBIST_setFirewall();
    if (status != SDL_PASS)
    {
        UART_printf(" PBIST_setFirewall failed \n");
        retValue = -1;
    }
    if (retValue == 0)
    {
        evtCfg.secureClaimEnable = 1U;
        evtCfg.evtSendEnable = 1U;
        evtCfg.extEvtNum = CSLR_COMPUTE_CLUSTER0_CLEC_MSMC_EVENT_IN_COMPUTE_CLUSTER0_CORE_CORE_MSMC_INTR_12;
        evtCfg.rtMap = 2U;
        evtCfg.c7xEvtNum = 0U;

        /* Configure interrupt router to take care of routing C7x PBIST interrupt event */
        status =  CSL_clecConfigEvent((CSL_CLEC_EVTRegs *)CSL_COMPUTE_CLUSTER0_CLEC_REGS_BASE,
                                      CSLR_COMPUTE_CLUSTER0_CLEC_MSMC_EVENT_IN_COMPUTE_CLUSTER0_CORE_CORE_MSMC_INTR_12,
                                      &evtCfg);
        if (status != CSL_PASS)
        {
            UART_printf(" CSL_clecConfigEvent C7x 0 failed \n");
            retValue = -1;
        }
    }

    if (retValue == 0)
    {
        evtCfg.extEvtNum = CSLR_COMPUTE_CLUSTER0_CLEC_MSMC_EVENT_IN_COMPUTE_CLUSTER0_CORE_CORE_MSMC_INTR_13;
        /* Configure interrupt router to take care of routing C7x PBIST interrupt event */
        status =  CSL_clecConfigEvent((CSL_CLEC_EVTRegs *)CSL_COMPUTE_CLUSTER0_CLEC_REGS_BASE,
                                      CSLR_COMPUTE_CLUSTER0_CLEC_MSMC_EVENT_IN_COMPUTE_CLUSTER0_CORE_CORE_MSMC_INTR_13,
                                      &evtCfg);
        if (status != CSL_PASS)
        {
            UART_printf(" CSL_clecConfigEvent C7x 1 failed \n");
            retValue = -1;
        }
    }

    if (retValue == 0)
    {
        evtCfg.extEvtNum = CSLR_COMPUTE_CLUSTER0_CLEC_MSMC_EVENT_IN_COMPUTE_CLUSTER0_CORE_CORE_MSMC_INTR_8;
        /* Configure interrupt router to take care of routing A72 PBIST interrupt event */
        status =  CSL_clecConfigEvent((CSL_CLEC_EVTRegs *)CSL_COMPUTE_CLUSTER0_CLEC_REGS_BASE,
                                      CSLR_COMPUTE_CLUSTER0_CLEC_MSMC_EVENT_IN_COMPUTE_CLUSTER0_CORE_CORE_MSMC_INTR_8,
                                      &evtCfg);
        if (status != CSL_PASS)
        {
            UART_printf(" CSL_clecConfigEvent A72 failed \n");
            retValue = -1;
        }
    }

    return status;
}

void PBIST_eventHandler( uint32_t instanceId)
{
    if (instanceId == (uint32_t) PBIST_INSTANCE_C7x_0)
    {
        /* Clear C7x_0 PBIST interrupt event in CLEC */
        CSL_clecClearEvent((CSL_CLEC_EVTRegs *)CSL_COMPUTE_CLUSTER0_CLEC_REGS_BASE,
                       CSLR_COMPUTE_CLUSTER0_CLEC_MSMC_EVENT_IN_COMPUTE_CLUSTER0_CORE_CORE_MSMC_INTR_12);
    }
    if (instanceId == (uint32_t) PBIST_INSTANCE_C7x_1)
    {
        /* Clear C7x_1 PBIST interrupt event in CLEC */
        CSL_clecClearEvent((CSL_CLEC_EVTRegs *)CSL_COMPUTE_CLUSTER0_CLEC_REGS_BASE,
                       CSLR_COMPUTE_CLUSTER0_CLEC_MSMC_EVENT_IN_COMPUTE_CLUSTER0_CORE_CORE_MSMC_INTR_13);
    }
    if (instanceId == (uint32_t) PBIST_INSTANCE_A72_0)
    {
        /* Clear A72 PBIST interrupt event in CLEC*/
        CSL_clecClearEvent((CSL_CLEC_EVTRegs *)CSL_COMPUTE_CLUSTER0_CLEC_REGS_BASE,
                       CSLR_COMPUTE_CLUSTER0_CLEC_MSMC_EVENT_IN_COMPUTE_CLUSTER0_CORE_CORE_MSMC_INTR_8);
    }

    PBIST_TestHandleArray[instanceId].doneFlag = true;
    return;
}
/*
    InitRestore functions : Initialize or Restore based on init flag
    init : TRUE  --> Initialize
    init : FALSE --> Restore
*/
int32_t PBIST_A72AuxInitRestore(bool init)
{
    int32_t testResult = 0;
    bool result;

    CSL_RatTranslationCfgInfo translationCfg;
    uint32_t *localP = (uint32_t *)PBIST_REGION2_LOCAL_BASE;

    /* Add RAT configuration to access address > 32bit address range */
    translationCfg.translatedAddress = CSL_COMPUTE_CLUSTER0_CC_BASE;
    translationCfg.sizeInBytes = PBIST_REG_REGION_SIZE;
    translationCfg.baseAddress = (uint32_t)PBIST_REGION2_LOCAL_BASE;

    /* Set up RAT translation */
    result = CSL_ratConfigRegionTranslation((CSL_ratRegs *)PBIST_RAT_CFG_BASE,
                                            PBIST_RAT_REGION2_INDEX, &translationCfg);
    if (result == false) {
        UART_printf("   CSL_ratConfigRegionTranslation...FAILED \n");
        testResult = -1;
    }

    if (testResult == 0)
    {
        if (init)
        {
            *((uint32_t *)(((uint32_t)localP) + 0x100)) = 0x1;
        }
        else
        {
            *((uint32_t *)(((uint32_t)localP) + 0x100)) = 0x0;
        }
    }
    if (testResult == 0)
    {
        /* Disable RAT translation */
        result = CSL_ratDisableRegionTranslation((CSL_ratRegs *)PBIST_RAT_CFG_BASE,
                                                 PBIST_RAT_REGION2_INDEX);
        if (result == false) {
            UART_printf("   CSL_ratDisableRegionTranslation...FAILED \n");
            testResult = -1;
        }
    }

    return testResult;
}

int32_t PBIST_VPACAuxInitRestore(bool init)
{
    int32_t testResult = 0;
    CSL_viss_topRegs *vissTopRegsP;

    vissTopRegsP = (CSL_viss_topRegs *)CSL_VPAC0_PAR_VPAC_VISS0_S_VBUSP_MMR_CFG_VISS_TOP_BASE;
    if (init)
    {
        vissTopRegsP->VISS_CNTL = CSL_VISS_TOP_VISS_CNTL_NSF4V_EN_MASK
                                  | CSL_VISS_TOP_VISS_CNTL_GLBCE_EN_MASK;
    }
    else
    {
        vissTopRegsP->VISS_CNTL &= (~(CSL_VISS_TOP_VISS_CNTL_NSF4V_EN_MASK
                                  | CSL_VISS_TOP_VISS_CNTL_GLBCE_EN_MASK));
    }

    return testResult;
}

/* define the unlock and lock values */
#define KICK0_UNLOCK_VAL 0x68EF3490
#define KICK1_UNLOCK_VAL 0xD172BC5A
#define KICK_LOCK_VAL    0x00000000

int32_t PBIST_HCAuxInitRestore(bool init)
{
    int32_t testResult = 0;

    CSL_Cp_aceRegs *SA2ULRegsP;

    SA2ULRegsP = (CSL_Cp_aceRegs *)CSL_SA2_UL0_BASE;

    *((uint32_t *)(SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_LOCK3_KICK0)) = KICK0_UNLOCK_VAL;
    *((uint32_t *)(SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_LOCK3_KICK1)) = KICK1_UNLOCK_VAL;
    *((uint32_t *)(SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_PBIST_EN)) = 0xFFFFFFFF;

    if (init)
    {
        SA2ULRegsP->UPDATES.ENGINE_ENABLE |= CSL_CP_ACE_UPDATES_ENGINE_ENABLE_PKA_EN_MASK;
    }
    else
    {
        SA2ULRegsP->UPDATES.ENGINE_ENABLE &= (~CSL_CP_ACE_UPDATES_ENGINE_ENABLE_PKA_EN_MASK);
    }

    return testResult;
}

int32_t PBIST_MainInfra0AuxInitRestore(bool init)
{
    int32_t testResult = 0;

    *((uint32_t *)(SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_LOCK3_KICK0)) = KICK0_UNLOCK_VAL;
    *((uint32_t *)(SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_LOCK3_KICK1)) = KICK1_UNLOCK_VAL;
    *((uint32_t *)(SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_PBIST_EN)) = 0xFFFFFFFF;

    return testResult;
}

int32_t PBIST_NAVSSAuxInitRestore(bool init)
{
    *((uint32_t *)(SDL_WKUP_CTRL_MMR0_CFG0_BASE + SDL_WKUP_CTRL_MMR_CFG0_LOCK6_KICK0)) = KICK0_UNLOCK_VAL;
    *((uint32_t *)(SDL_WKUP_CTRL_MMR0_CFG0_BASE + SDL_WKUP_CTRL_MMR_CFG0_LOCK6_KICK1)) = KICK1_UNLOCK_VAL;
    *((uint32_t *)(SDL_WKUP_CTRL_MMR0_CFG0_BASE + SDL_WKUP_CTRL_MMR_CFG0_MAIN_CLKGATE_CTRL1)) = 0xFFFFFFFF;

    return 0;
}

int32_t PBIST_CodecAuxInitRestore(bool init)
{
    int32_t testResult = 0;
    *((uint32_t *)(SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_LOCK3_KICK0)) = KICK0_UNLOCK_VAL;
    *((uint32_t *)(SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_LOCK3_KICK1)) = KICK1_UNLOCK_VAL;
    *((uint32_t *)(SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_PBIST_EN)) = 0xFFFFFFFF;
    return testResult;
}


int32_t PBIST_GPUAuxInitRestore(bool init)
{
    int32_t testResult = 0;
    bool result;

    CSL_RatTranslationCfgInfo translationCfg;

    uint64_t *localP = (uint64_t *)PBIST_REGION2_LOCAL_BASE;

    /* Add RAT configuration to access address > 32bit address range */
    translationCfg.translatedAddress = SDL_AEP_GPU_BXS464_WRAP0_CORE_MMRS_BASE;
    translationCfg.sizeInBytes = PBIST_REG_REGION2_SIZE;
    translationCfg.baseAddress = (uint32_t)PBIST_REGION2_LOCAL_BASE;

    /* Set up RAT translation */
    result = CSL_ratConfigRegionTranslation((CSL_ratRegs *)PBIST_RAT_CFG_BASE,
                                            PBIST_RAT_REGION2_INDEX, &translationCfg);
    if (result == false) {
        UART_printf("   CSL_ratConfigRegionTranslation...FAILED \n");
        testResult = -1;
    }

    if (testResult == 0)
    {
        if (init)
        {
            *((uint64_t *)(((uint32_t)localP) + 0xA100)) = 0x0;

            while(*((uint64_t *)(((uint32_t)localP) + 0xA100)) !=  (uint64_t)0x0);

            /* Force all GPU clocks on */
            *((uint64_t *)(((uint32_t)localP) + 0x00)) = 0x0015550015115555;

#if 0
            /* Force the FW Processor clocks to ON to enable testing of mars memories */
            *((uint32_t *)(((uint32_t)localP) + 0x3090)) = 0x00000001;
            i = 1000;
            while(i--);
            *((uint32_t *)(((uint32_t)localP) + 0x3480)) = 0x00000001;
            while((*((uint32_t *)(((uint32_t)localP) + 0x3488)) & 0x400) != (uint32_t)0x400);
            *((uint32_t *)(((uint32_t)localP) + 0x3480)) = 0x80000001;
            i = 1000;
            while(i--);
            while((*((uint32_t *)(((uint32_t)localP) + 0x3488)) & 0x200) != (uint32_t)0x200);
            *((uint32_t *)(((uint32_t)localP) + 0x3420)) = 0x000001FF;
            *((uint32_t *)(((uint32_t)localP) + 0x34B8)) = 0x002307F8;
            while((*((uint32_t *)(((uint32_t)localP) + 0x34B8)) & 0x1700) != (uint32_t)0x0000);
            i = 1000;
            while(i--);
            *((uint32_t *)(((uint32_t)localP) + 0x3480)) = 0x00;
            i = 1000;
            while(i--);
#endif
        }
        else
        {
          /* TODO: Need to see how to revert */
        }
    }
    if (testResult == 0)
    {
        /* Disable RAT translation */
        result = CSL_ratDisableRegionTranslation((CSL_ratRegs *)PBIST_RAT_CFG_BASE,
                                                 PBIST_RAT_REGION2_INDEX);
        if (result == false) {
            UART_printf("   CSL_ratDisableRegionTranslation...FAILED \n");
            testResult = -1;
        }
    }

    return testResult;
}

int32_t PBIST_DSSAuxInitRestore(bool init)
{
    int32_t testResult = 0;
    uint32_t value;
    static uint8_t dssaux_apicalled = 0;
    uint32_t *common_m = (uint32_t *)(CSL_DSS0_DISPC_0_COMMON_M_BASE);

    uint32_t *vp2 = (uint32_t *)(CSL_DSS0_VP2_BASE);

    uint32_t *ovr2 = (uint32_t *)(SDL_DSS0_OVR2_BASE);

    uint32_t *vid1 = (uint32_t *)(SDL_DSS0_VID1_BASE);
    uint32_t *vid2 = (uint32_t *)(SDL_DSS0_VIDL1_BASE);
    uint32_t *vid3 = (uint32_t *)(SDL_DSS0_VID2_BASE);
    uint32_t *vid4 = (uint32_t *)(SDL_DSS0_VIDL2_BASE);


    if (init==TRUE && dssaux_apicalled == 0)
    {
		dssaux_apicalled = 1;
        *((uint32_t *)(SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_LOCK3_KICK0)) = KICK0_UNLOCK_VAL;
        *((uint32_t *)(SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_LOCK3_KICK1)) = KICK1_UNLOCK_VAL;
        *((uint32_t *)(SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_PBIST_EN)) = 0xFFFFFFFF;

        *((uint32_t *)(SDL_WKUP_CTRL_MMR0_CFG0_BASE + SDL_WKUP_CTRL_MMR_CFG0_LOCK6_KICK0)) = KICK0_UNLOCK_VAL;
        *((uint32_t *)(SDL_WKUP_CTRL_MMR0_CFG0_BASE + SDL_WKUP_CTRL_MMR_CFG0_LOCK6_KICK1)) = KICK1_UNLOCK_VAL;
        *((uint32_t *)(SDL_WKUP_CTRL_MMR0_CFG0_BASE + SDL_WKUP_CTRL_MMR_CFG0_MAIN_CLKGATE_CTRL2)) = 0xFFFFFFFF;

        CSL_REG32_WR(&((CSL_dss_vid1Regs*)(vid1))->FBDC_ATTRIBUTES, 1);
        CSL_REG32_WR(&((CSL_dss_vidl1Regs*)(vid2))->FBDC_ATTRIBUTES, 1);
        CSL_REG32_WR(&((CSL_dss_vid1Regs*)(vid3))->FBDC_ATTRIBUTES, 1);
        CSL_REG32_WR(&((CSL_dss_vidl1Regs*)(vid4))->FBDC_ATTRIBUTES, 1);

        CSL_REG32_FINS(&((CSL_dss_vidl1Regs*)(vid2))->ATTRIBUTES, DSS_VIDL1_ATTRIBUTES_ENABLE, 1);

        value = CSL_FMK(DSS_OVR1_ATTRIBUTES_ENABLE, 1) | CSL_FMK(DSS_OVR1_ATTRIBUTES_CHANNELIN, 1);
        CSL_REG32_WR(&((CSL_dss_ovr1Regs *)(ovr2))->ATTRIBUTES, value);

        {
            uint32_t division = 2400 / 100;

            uint32_t divfactor = 0x8000 | (division-1);

            *((uint32_t *)(SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_LOCK2_KICK0)) = KICK0_UNLOCK_VAL;
            *((uint32_t *)(SDL_CTRL_MMR0_CFG0_BASE + SDL_MAIN_CTRL_MMR_CFG0_LOCK2_KICK1)) = KICK1_UNLOCK_VAL;

            *((uint32_t *)(SDL_CTRL_MMR0_CFG0_BASE + CSL_MAIN_CTRL_MMR_CFG0_DSS_DISPC0_CLKSEL1)) = 0x1;
            *((uint32_t *)(CSL_PLL0_CFG_BASE + CSL_MAIN_PLL_MMR_CFG_PLL19_PID + CSL_MAIN_PLL_MMR_CFG_PLL0_HSDIV_CTRL0)) = divfactor;
        }

        CSL_REG32_FINS(&((CSL_dss_common_mRegs*)(common_m))->DISPC_CONNECTIONS, DSS_COMMON_M_DISPC_CONNECTIONS_DPI_0_CONN, 4);

        CSL_REG32_FINS(&((CSL_dss_vp1Regs*)(vp2))->CONTROL, DSS_VP1_CONTROL_ENABLE, 1);
    }

    return testResult;
}
/* Nothing past this point */
