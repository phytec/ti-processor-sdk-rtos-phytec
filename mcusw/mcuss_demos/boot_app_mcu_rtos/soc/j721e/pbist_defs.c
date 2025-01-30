/*
*
* Copyright (C) 2024 Texas Instruments Incorporated
*
* All rights reserved not granted herein.
*
* Limited License.
*
* Texas Instruments Incorporated grants a world-wide, royalty-free, non-exclusive
* license under copyrights and patents it now or hereafter owns or controls to make,
* have made, use, import, offer to sell and sell ("Utilize") this software subject to the
* terms herein.  With respect to the foregoing patent license, such license is granted
* solely to the extent that any such patent is necessary to Utilize the software alone.
* The patent license shall not apply to any combinations which include this software,
* other than combinations with devices manufactured by or for TI ("TI Devices").
* No hardware patent is licensed hereunder.
*
* Redistributions must preserve existing copyright notices and reproduce this license
* (including the above copyright notice and the disclaimer and (if applicable) source
* code license limitations below) in the documentation and/or other materials provided
* with the distribution
*
* Redistribution and use in binary form, without modification, are permitted provided
* that the following conditions are met:
*
* *       No reverse engineering, decompilation, or disassembly of this software is
* permitted with respect to any software provided in binary form.
*
* *       any redistribution and use are licensed by TI for use only with TI Devices.
*
* *       Nothing shall obligate TI to provide you with source code for the software
* licensed and provided to you in object code.
*
* If software source code is provided to you, modification and redistribution of the
* source code are permitted provided that the following conditions are met:
*
* *       any redistribution and use of the source code, including any resulting derivative
* works, are licensed by TI for use only with TI Devices.
*
* *       any redistribution and use of any object code compiled from the source code
* and any resulting derivative works, are licensed by TI for use only with TI Devices.
*
* Neither the name of Texas Instruments Incorporated nor the names of its suppliers
*
* may be used to endorse or promote products derived from this software without
* specific prior written permission.
*
* DISCLAIMER.
*
* THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY EXPRESS
* OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL TI AND TI'S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
* OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
* OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/

/**
 *  \file pbist_defs.c
 *
 *  \brief PBIST SOC-specific structures and functions
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "Std_Types.h"
#include <stdint.h>
#include <soc.h>
#include <ti/csl/csl_rat.h>
#include <ti/csl/csl_clec.h>
#include <ti/csl/cslr_vpac.h>
#include <ti/csl/csl_cbass.h>
#include <ti/csl/csl_rat.h>
#include <ti/csl/cslr_vpac.h>
#include <ti/csl/cslr_cp_ace.h>
#include <src/sdl/sdl_pbist.h>
#include <src/ip/sdl_ip_pbist.h>

#include <ti/drv/uart/UART.h>
#include <ti/drv/uart/UART_stdio.h>
/* SCI Client */
#include <ti/drv/sciclient/sciclient.h>

#include "pbist_utils.h"
#include "pbist_defs.h"

#include "app_utils.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

#define PBIST_RAT_CFG_BASE                CSL_MCU_ARMSS_RAT_CFG_BASE

#define A72_NUM_AUX_DEVICES               1
#define MSMC_NUM_AUX_DEVICES              3
#define MAIN_INFRA_NUM_AUX_DEVICES        18
#define GPU_NUM_AUX_DEVICES               2
#define DSS_NUM_AUX_DEVICES               10

/* This macro defined by default to indicate only a select memory section can
 * be run on the MCU memory PBIST instances to ensure test application running
 * on MCU is not corrupted. */
#define PBIST_MCU_SELECTED_MEMORY

/* HW POST run status definitions */
#define PBIST_POST_COMPLETED_SUCCESS_STR      "SDL_PBIST_POST_COMPLETED_SUCCESS"
#define PBIST_POST_COMPLETED_FAILURE_STR      "SDL_PBIST_POST_COMPLETED_FAILURE"
#define PBIST_POST_ATTEMPTED_TIMEOUT_STR      "SDL_PBIST_POST_TIMEOUT"
#define PBIST_POST_NOT_RUN_STR                "SDL_PBIST_POST_NOT_RUN"
/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                            Local function prototypes                       */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/*
    InitRestore functions : Initialize or Restore based on init flag
    init : TRUE  --> Initialize
    init : FALSE --> Restore
*/
int32_t PBIST_A72AuxInitRestore(bool init);
int32_t PBIST_VPACAuxInitRestore(bool init);
int32_t PBIST_MainInfraAuxInitRestore(bool init);
int32_t PBIST_decoderAuxInitRestore(bool init);
int32_t PBIST_GPUAuxInitRestore(bool init);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

uint32_t PBIST_A72AuxDevList[A72_NUM_AUX_DEVICES] =
{
    TISCI_DEV_A72SS0,
};

uint32_t PBIST_MSMCAuxDevList[MSMC_NUM_AUX_DEVICES] =
{
    TISCI_DEV_COMPUTE_CLUSTER0_EN_MSMC_DOMAIN,
    TISCI_DEV_NAVSS0,
    TISCI_DEV_A72SS0,
};

uint32_t PBIST_MainInfraAuxDevList[MAIN_INFRA_NUM_AUX_DEVICES] =
{
    TISCI_DEV_DEBUGSS_WRAP0,
    TISCI_DEV_MCAN0,
    TISCI_DEV_MCAN1,
    TISCI_DEV_MCAN2,
    TISCI_DEV_MCAN3,
    TISCI_DEV_MCAN4,
    TISCI_DEV_MCAN5,
    TISCI_DEV_MCAN6,
    TISCI_DEV_MCAN7,
    TISCI_DEV_MCAN8,
    TISCI_DEV_MCAN9,
    TISCI_DEV_MCAN10,
    TISCI_DEV_MCAN11,
    TISCI_DEV_MCAN12,
    TISCI_DEV_MCAN13,
    TISCI_DEV_I3C0,
    TISCI_DEV_SA2_UL0,
    TISCI_DEV_CPSW0
};

uint32_t PBIST_GPUAuxDevList[GPU_NUM_AUX_DEVICES] =
{
    TISCI_DEV_GPU0,
    TISCI_DEV_GPU0_GPU_0,
};

uint32_t PBIST_DSSAuxDevList[DSS_NUM_AUX_DEVICES] =
{
    TISCI_DEV_DSS0,
    TISCI_DEV_DSS_EDP0,
    TISCI_DEV_CSI_RX_IF0,
    TISCI_DEV_CSI_RX_IF1,
    TISCI_DEV_CSI_TX_IF0,
    TISCI_DEV_CSI_PSILSS0,
    TISCI_DEV_DPHY_RX0,
    TISCI_DEV_DPHY_RX1,
    TISCI_DEV_DPHY_TX0,
    TISCI_DEV_SERDES_10G0,
/* SERDES8MTO*/
};


PBIST_TestHandle_t PBIST_TestHandleArray[PBIST_MAX_INSTANCE+1] =
{
    /* HW POST - Used to check the status of HW POST MCU PBIST */
    {
        .testName              = "PBIST HWPOST MCU",
        .pbistInst             = SDL_PBIST_INST_MCUR5F0,
        .numPostPbistToCheck   = 1u,
    },
    /* MCU PBIST - Not run as part of BootApp since PBIST changes the memory upon which it is run. */
    {
        .testName               = "MCU PBIST",
        .pbistInst              = SDL_PBIST_INST_MCUR5F0,
        .numPostPbistToCheck    = 0u,
        .tisciPBISTDeviceId     = TISCI_DEV_MCU_PBIST0, /* PBIST device id  */
        .procRstNeeded          = false,
        .secondaryCoreNeeded    = false,                /* Secondary core needed */
        .coreCustPwrSeqNeeded   = false,
        .numAuxDevices          = 0u,                   /* No Aux devices */
        .auxInitRestoreFunction = NULL,                 /* Auxilliary init/restore function */
    },
    /* Main R5F 0 */
    {
        .testName               = "Main R5F 0 PBIST",
        .pbistInst              = SDL_PBIST_INST_MAINR5F0,
        .numPostPbistToCheck    = 0u,
        .tisciPBISTDeviceId     = TISCI_DEV_PBIST9,               /* PBIST device id  */
        .procRstNeeded          = true,
        .secondaryCoreNeeded    = true,                           /* Secondary core needed */
        .coreName               = "Main R5F0 core0",              /* Primary core   */
        .secCoreName            = "Main R5F0 core1",              /* Secondary core */
        .tisciProcId            = SCICLIENT_PROC_ID_R5FSS0_CORE0, /* Main R5F core 0 Proc Id */
        .tisciSecProcId         = SCICLIENT_PROC_ID_R5FSS0_CORE1, /* Main R5F core 1 Proc Id */
        .tisciDeviceId          = TISCI_DEV_R5FSS0_CORE0,         /* Main R5F core 0 Device Id */
        .tisciSecDeviceId       = TISCI_DEV_R5FSS0_CORE1,         /* Main R5F core 1 Device Id */
        .coreCustPwrSeqNeeded   = false,
        .numAuxDevices          = 0u,                             /* No Aux devices */
        .auxInitRestoreFunction = NULL,                           /* Auxilliary init/restore function */
    },
    /* Main R5F 1 */
    {
        .testName               = "Main R5F 1 PBIST",
        .pbistInst              = SDL_PBIST_INST_MAINR5F1,
        .numPostPbistToCheck    = 0u,
        .tisciPBISTDeviceId     = TISCI_DEV_PBIST10,               /* PBIST device id  */
        .procRstNeeded          = true,
        .secondaryCoreNeeded    = true,                            /* Secondary core needed */
        .coreName               = "Main R5F1 core0",               /* Primary core   */
        .secCoreName            = "Main R5F1 core1",               /* Secondary core */
        .tisciProcId            = SCICLIENT_PROC_ID_R5FSS1_CORE0,  /* Main R5F core 0 Proc Id */
        .tisciSecProcId         = SCICLIENT_PROC_ID_R5FSS1_CORE1,  /* Main R5F core 1 Proc Id */
        .tisciDeviceId          = TISCI_DEV_R5FSS1_CORE0,          /* Main R5F core 0 Device id */
        .tisciSecDeviceId       = TISCI_DEV_R5FSS1_CORE1,          /* Main R5F core 1 Device id */
        .coreCustPwrSeqNeeded   = false,
        .numAuxDevices          = 0u,                              /* No Aux devices */
        .auxInitRestoreFunction = NULL,                            /* Auxilliary init/restore function */
    },
    /* C7X */
    {
        .testName               = "C7X PBIST",
        .pbistInst              = SDL_PBIST_INST_C7X,
        .numPostPbistToCheck    = 0u,
        .tisciPBISTDeviceId     = TISCI_DEV_C71X_0_PBIST_VD,       /* PBIST device id  */
        .procRstNeeded          = true,
        .secondaryCoreNeeded    = false,                           /* Secondary core needed */
        .coreName               = "C7x ",
        .tisciProcId            = SCICLIENT_PROC_ID_C71SS0,        /* C7x Proc Id */
        .tisciDeviceId          = TISCI_DEV_C71SS0,                /* C7x Device Id */
        .coreCustPwrSeqNeeded   = false,
        .numAuxDevices          = 0u,                              /* No Aux devices */
        .auxInitRestoreFunction = NULL,                            /* Auxilliary init/restore function */
    },
    /* A72 */
    {
        .testName               = "A72 PBIST",
        .pbistInst              = SDL_PBIST_INST_A72,
        .tisciPBISTDeviceId     = TISCI_DEV_COMPUTE_CLUSTER0_PBIST_WRAP, /* Device Id for A72 PBIST */
        .procRstNeeded          = true,
        .secondaryCoreNeeded    = true,                           /* Secondary core needed */
        .coreName               = "A72 core 0",                   /* Primary core   */
        .secCoreName            = "A72 core 1",                   /* Secondary core */
        .tisciProcId            = SCICLIENT_PROC_ID_A72SS0_CORE0, /* A72 core 0 Proc Id */
        .tisciSecProcId         = SCICLIENT_PROC_ID_A72SS0_CORE1, /* A72 core 1 Proc Id */
        .tisciDeviceId          = TISCI_DEV_A72SS0_CORE0,         /* A72 core 0 Device Id */
        .tisciSecDeviceId       = TISCI_DEV_A72SS0_CORE1,         /* A72 core 1 Device Id */
        .coreCustPwrSeqNeeded   = true,                           /* A72 needs custom powerdown sequence steps */
        .numAuxDevices          = A72_NUM_AUX_DEVICES,            /* Number of Aux devices */
        .auxDeviceIdsP          = &PBIST_A72AuxDevList[0],        /* Array of Aux device ids */
        .auxInitRestoreFunction = PBIST_A72AuxInitRestore,        /* Auxilliary init/restore function */
    },
    /* VPAC */
    {
        .testName               = "VPAC PBIST",
        .pbistInst              = SDL_PBIST_INST_VPAC,
        .numPostPbistToCheck    = 0u,
        .tisciPBISTDeviceId     = TISCI_DEV_PBIST3,          /* PBIST device id  */
        .procRstNeeded          = true,
        .secondaryCoreNeeded    = false,                     /* Secondary core needed */
        .coreName               = "VPAC core",               /* Primary core   */
        .tisciProcId            = 0x0u,                      /* No Proc Id for VPAC */
        .tisciDeviceId          = TISCI_DEV_VPAC0,           /* VPAC core Device Id */
        .coreCustPwrSeqNeeded   = false,
        .numAuxDevices          = 0u,                        /* No Aux devices */
        .auxInitRestoreFunction = PBIST_VPACAuxInitRestore,  /* Auxilliary init/restore function */
    },
    /* DMPAC */
    {
        .testName               = "DMPAC PBIST",
        .pbistInst              = SDL_PBIST_INST_DMPAC,
        .numPostPbistToCheck    = 0u,
        .tisciPBISTDeviceId     = TISCI_DEV_PBIST1,           /* PBIST device id  */
        .procRstNeeded          = true,
        .secondaryCoreNeeded    = true,                       /* Secondary core needed */
        .coreName               = "DMPAC core",               /* Primary core   */
        .secCoreName            = "DMPAC SDE core",           /* Secondary core */
        .tisciProcId            = 0x0u,                       /* No ProcId for DMPAC */
        .tisciSecProcId         = 0x0u,                       /* No Sec ProcId for DMPAC */
        .tisciDeviceId          = TISCI_DEV_DMPAC0,           /* DMPAC Core Device Id */
        .tisciSecDeviceId       = TISCI_DEV_DMPAC0_SDE_0,     /* DMPAC_SDE Device Id */
        .coreCustPwrSeqNeeded   = false,
        .numAuxDevices          = 0u,                         /* No Aux devices */
        .auxInitRestoreFunction = NULL,                       /* Auxilliary init/restore function */
    },
    /* NAVSS - Not supported in BootApp */
    {
        .testName               = "NAVSS PBIST",
        .pbistInst              = SDL_PBIST_INST_NAVSS,
        .numPostPbistToCheck    = 0u,
        .tisciPBISTDeviceId     = TISCI_DEV_PBIST7,         /* PBIST device id  */
        .procRstNeeded          = true,
        .secondaryCoreNeeded    = false,                    /* Secondary core needed */
        .coreName               = "NAVSS",                  /* Primary core   */
        .tisciProcId            = 0x0u,                     /* No Proc Id for NAVSS */
        .tisciDeviceId          = TISCI_DEV_NAVSS0,         /* NAVSS Device Id */
        .coreCustPwrSeqNeeded   = false,
        .numAuxDevices          = 0u,                       /* No Aux devices */
        .auxInitRestoreFunction = NULL,                     /* Auxilliary init/restore function */
    },
    /* HC */
    {
        .testName               = "HC PBIST",
        .pbistInst              = SDL_PBIST_INST_HC,
        .numPostPbistToCheck    = 0u,
        .tisciPBISTDeviceId     = TISCI_DEV_PBIST5,         /* PBIST device id  */
        .procRstNeeded          = false,
        .secondaryCoreNeeded    = false,                    /* Secondary core needed */
        .coreName               = "HC",                     /* Primary core */
        .tisciProcId            = 0x0u,                     /* No ProcId for HC */
        .tisciDeviceId          = 0x0u,                     /* Assumes MLB (memory for Memory Groups 8-11) is powered up already */
        .coreCustPwrSeqNeeded   = false,
        .numAuxDevices          = 0u,                       /* No Aux devices */
        .auxInitRestoreFunction = NULL,                     /* Auxilliary init/restore function */
    },
    /* C6X core 0 */
    {
        .testName               = "C6x core 0 PBIST",
        .pbistInst              = SDL_PBIST_INST_C66X0,
        .numPostPbistToCheck    = 0u,
        .tisciPBISTDeviceId     = TISCI_DEV_C66SS0_PBIST0,   /* PBIST device id  */
        .procRstNeeded          = true,
        .secondaryCoreNeeded    = false,                           /* Secondary core needed */
        .coreName               = "C6x core 0",                    /* Primary core   */
        .tisciProcId            = SCICLIENT_PROC_ID_C66SS0_CORE0,  /* C6x core0 Proc Id */
        .tisciDeviceId          = TISCI_DEV_C66SS0_CORE0,          /* C6x Core 0 Device Id */
        .coreCustPwrSeqNeeded   = false,
        .numAuxDevices          = 0u,                              /* No Aux devices */
        .auxInitRestoreFunction = NULL,                            /* Auxilliary init/restore function */
    },
    /* C6X core 1 */
    {
        .testName               = "C6x core 1 PBIST",
        .pbistInst              = SDL_PBIST_INST_C66X1,
        .numPostPbistToCheck    = 0u,
        .tisciPBISTDeviceId     = TISCI_DEV_C66SS1_PBIST0,   /* PBIST device id  */
        .procRstNeeded          = true,                            /* Initialize done flag */
        .secondaryCoreNeeded    = false,                           /* Secondary core needed */
        .coreName               = "C6x core 1",                    /* Primary core   */
        .tisciProcId            = SCICLIENT_PROC_ID_C66SS1_CORE0,  /* C6x Core 1 Proc Id */
        .tisciDeviceId          = TISCI_DEV_C66SS1_CORE0,          /* C6x Core 1 Device Id */
        .coreCustPwrSeqNeeded   = false,
        .numAuxDevices          = 0u,                              /* No Aux devices */
        .auxInitRestoreFunction = NULL,                            /* Auxilliary init/restore function */
    },
    /* Main Infra */
    {
        .testName               = "Main Infra PBIST",
        .pbistInst              = SDL_PBIST_INST_MAININFRA,
        .numPostPbistToCheck    = 0u,
        .tisciPBISTDeviceId     = TISCI_DEV_PBIST6,        /* PBIST device id  */
        .procRstNeeded          = false,
        .secondaryCoreNeeded    = false,                   /* Secondary core needed */
        .coreName               = "",                      /* No coreName   */
        .tisciProcId            = 0x0u,                    /* No Proc Id needed for Main Intrastructure */
        .tisciDeviceId          = 0x0u,                    /* No Device Id needed for Main infrastructure */
        .coreCustPwrSeqNeeded   = false,
        .numAuxDevices          = MAIN_INFRA_NUM_AUX_DEVICES,    /* No Aux devices       */
        .auxDeviceIdsP          = &PBIST_MainInfraAuxDevList[0], /* Array of Aux device ids */
        .auxInitRestoreFunction = PBIST_MainInfraAuxInitRestore, /* Auxilliary init/restore function */
    },
    /* MSMC */
    {
        .testName               = "MSMC PBIST",
        .pbistInst              = SDL_PBIST_INST_MSMC,
        .numPostPbistToCheck    = 0u,
        .tisciPBISTDeviceId     = TISCI_DEV_COMPUTE_CLUSTER0_PBIST_WRAP, /* PBIST device id  */
        .procRstNeeded          = true,
        .secondaryCoreNeeded    = true,                    /* Secondary core needed */
        .coreName               = "A72 core 0",            /* Primary core   */
        .secCoreName            = "A72 core 1",            /* Secondary core */
        .tisciProcId            = SCICLIENT_PROC_ID_A72SS0_CORE0, /* A72 core 0 Proc Id */
        .tisciSecProcId         = SCICLIENT_PROC_ID_A72SS0_CORE1, /* A72 core 1 Proc Id */
        .tisciDeviceId          = TISCI_DEV_A72SS0_CORE0,         /* A72 core 0 Device Id */
        .tisciSecDeviceId       = TISCI_DEV_A72SS0_CORE1,         /* A72 core 1 Device Id */
        .coreCustPwrSeqNeeded   = true,
        .numAuxDevices          = MSMC_NUM_AUX_DEVICES,           /* No Aux devices       */
        .auxDeviceIdsP          = &PBIST_MSMCAuxDevList[0],       /* Array of Aux device ids */
        .auxInitRestoreFunction = NULL,                           /* Auxilliary init/restore function */
    },
    /* Encoder */
    {
        .testName               = "Encoder PBIST",
        .pbistInst              = SDL_PBIST_INST_ENCODER,
        .numPostPbistToCheck    = 0u,
        .tisciPBISTDeviceId     = TISCI_DEV_PBIST2,   /* PBIST device id  */
        .procRstNeeded          = false,
        .secondaryCoreNeeded    = false,                /* Secondary core needed */
        .coreName               = "Encoder",            /* Primary core          */
        .tisciProcId            = 0x0u,                 /* No ProcId for Encoder */
        .tisciDeviceId          = TISCI_DEV_ENCODER0,   /* Encoder Device Id     */
        .coreCustPwrSeqNeeded   = false,
        .numAuxDevices          = 0u,                   /* No Aux devices        */
        .auxInitRestoreFunction = NULL,                 /* Auxilliary init/restore function */
    },

    /* Decoder */
    {
        .testName               = "Decoder PBIST",
        .pbistInst              = SDL_PBIST_INST_DECODER,
        .numPostPbistToCheck    = 0u,
        .tisciPBISTDeviceId     = TISCI_DEV_PBIST0,            /* PBIST device id  */
        .procRstNeeded          = false,
        .secondaryCoreNeeded    = false,                       /* Secondary core needed */
        .coreName               = "Decoder",                   /* Primary core          */
        .tisciProcId            = 0x0u,                        /* No ProcId for Decoder */
        .tisciDeviceId          = TISCI_DEV_DECODER0,          /* Device Id             */
        .coreCustPwrSeqNeeded   = false,
        .numAuxDevices          = 0u,                          /* No Aux devices        */
        .auxInitRestoreFunction = PBIST_decoderAuxInitRestore, /* Auxilliary init/restore function */
    },
    /* Remaining PBIST sections are not currently supported in BootApp */
#if 0
    /* GPU */
    {
        .testName               = "GPU PBIST",
        .pbistInst              = SDL_PBIST_INST_GPU,
        .numPostPbistToCheck    = 0u,
        .tisciPBISTDeviceId     = TISCI_DEV_GPU0_DFT_PBIST_0, /* PBIST device id  */
        .procRstNeeded          = false,
        .secondaryCoreNeeded    = false,                    /* Secondary core needed */
        .coreName               = "GPU",                    /* Primary core   */
        .tisciProcId            = 0x0u,                     /* No ProcId for GPU */
        .tisciDeviceId          = TISCI_DEV_GPU0_GPUCORE_0, /* GPU Device Id */
        .coreCustPwrSeqNeeded   = false,
        .numAuxDevices          = GPU_NUM_AUX_DEVICES,      /* Number of Aux devices   */
        .auxDeviceIdsP          = &PBIST_GPUAuxDevList[0],  /* Array of Aux device ids */
        .auxInitRestoreFunction = PBIST_GPUAuxInitRestore,  /* Auxilliary init/restore function */
    },
    /* DSS EDP DSI */
    {
        .testName               = "DSS EDP PBIST",
        .pbistInst              = SDL_PBIST_INST_DSS,
        .numPostPbistToCheck    = 0u,
        .tisciPBISTDeviceId     = TISCI_DEV_PBIST4,        /* PBIST device id  */
        .procRstNeeded          = false,
        .secondaryCoreNeeded    = false,                   /* Secondary core needed */
        .coreName               = "DSS",                   /* Primary core   */
        .tisciProcId            = 0x0u,                    /* No ProcId for MSMC */
        .tisciDeviceId          = TISCI_DEV_DSS_DSI0,      /* DSS Device Id */
        .coreCustPwrSeqNeeded   = false,
        .numAuxDevices          = DSS_NUM_AUX_DEVICES,     /* Number of Aux devices   */
        .auxDeviceIdsP          = &PBIST_DSSAuxDevList[0], /* Array of Aux device ids */
        .auxInitRestoreFunction = NULL,                    /* Auxilliary init/restore function */
    },
    /* MCU PULSAR PBIST */
    {
        .testName               = "MCU PULSAR PBIST",
        .pbistInst              = SDL_PBIST_INST_MCUR5F1,
        .numPostPbistToCheck    = 0u,
        .tisciPBISTDeviceId     = TISCI_DEV_MCU_PBIST1,       /* PBIST device id */
        .procRstNeeded          = false,
        .secondaryCoreNeeded    = false,                   /* Secondary core needed */
        .coreCustPwrSeqNeeded   = false,
        .numAuxDevices          = 0u,                      /* No Aux devices */
        .auxInitRestoreFunction = NULL,                    /* Auxilliary init/restore function */
    },
#endif
};

/* ========================================================================== */
/*                            External Variables                              */
/* ========================================================================== */


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

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
    translationCfg.translatedAddress = CSL_COMPUTE_CLUSTER0_CC_REGS_BASE;
    translationCfg.sizeInBytes = PBIST_REG_REGION_SIZE;
    translationCfg.baseAddress = (uint32_t)PBIST_REGION2_LOCAL_BASE;

    /* Set up RAT translation */
    result = CSL_ratConfigRegionTranslation((CSL_ratRegs *)PBIST_RAT_CFG_BASE,
                                            PBIST_RAT_REGION2_INDEX, &translationCfg);
    if (result == false) {
        AppUtils_Printf(MSG_NORMAL,
                        "   CSL_ratConfigRegionTranslation...FAILED \n");
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
            AppUtils_Printf(MSG_NORMAL,
                            "   CSL_ratDisableRegionTranslation...FAILED \n");
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

int32_t PBIST_MainInfraAuxInitRestore(bool init)
{
    int32_t testResult = 0;
    CSL_Cp_aceRegs *SA2ULRegsP;

    SA2ULRegsP = (CSL_Cp_aceRegs *)CSL_SA2_UL0_BASE;

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

int32_t PBIST_decoderAuxInitRestore(bool init)
{
    int32_t testResult = 0;
    uint32_t *localP = (uint32_t *)CSL_DECODER0_MTX_CORE_BASE;

    if (init)
    {
        /* Values imported from the SDL Directory */
        *((uint32_t *)(((uint32_t)localP) + 0x440)) = 0xffffffff;
        *((uint32_t *)(((uint32_t)localP) + 0x444)) = 0xffffffff;
        *((uint32_t *)(((uint32_t)localP) + 0x24020)) = 0xffffffff;
        *((uint32_t *)(((uint32_t)localP) + 0x4020)) = 0xffffffff;
        *((uint32_t *)(((uint32_t)localP) + 0x3020)) = 0xffffffff;
    }
    else
    {
        /* TODO: Need to see how to restore */
    }
    return testResult;
}

int32_t PBIST_GPUAuxInitRestore(bool init)
{
    int32_t testResult = 0;
    bool result;

    CSL_RatTranslationCfgInfo translationCfg;

    uint64_t *localP = (uint64_t *)PBIST_REGION2_LOCAL_BASE;

    /* Add RAT configuration to access address > 32bit address range */
    translationCfg.translatedAddress = CSL_GPU0_CORE_MMRS_BASE;
    translationCfg.sizeInBytes = PBIST_REG_REGION2_SIZE;
    translationCfg.baseAddress = (uint32_t)PBIST_REGION2_LOCAL_BASE;

    /* Set up RAT translation */
    result = CSL_ratConfigRegionTranslation((CSL_ratRegs *)PBIST_RAT_CFG_BASE,
                                            PBIST_RAT_REGION2_INDEX, &translationCfg);
    if (result == false) {
        AppUtils_Printf(MSG_NORMAL,
                        "   CSL_ratConfigRegionTranslation...FAILED \n");
        testResult = -1;
    }

    if (testResult == 0)
    {
        if (init)
        {
            *((uint64_t *)(((uint32_t)localP) + 0xA100)) = 0xffff0113;

            while(*((uint64_t *)(((uint32_t)localP) + 0xA100)) !=  (uint64_t)0x0);
            *((uint64_t *)(((uint32_t)localP) + 0x00)) = 0x0555550015155555;
            *((uint64_t *)(((uint32_t)localP) + 0x80)) = 0x0000000400000000;
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
            AppUtils_Printf(MSG_NORMAL,
                            "   CSL_ratDisableRegionTranslation...FAILED \n");
            testResult = -1;
        }
    }

    return testResult;
}

char * PBIST_getPostStatusString(SDL_PBIST_postStatus postStatus)
{
    if (postStatus == SDL_PBIST_POST_NOT_RUN)
    {
        return PBIST_POST_NOT_RUN_STR;
    }
    else if (postStatus == SDL_PBIST_POST_TIMEOUT)
    {
        return PBIST_POST_ATTEMPTED_TIMEOUT_STR;
    }
    else if (postStatus == SDL_PBIST_POST_COMPLETED_FAILURE)
    {
        return PBIST_POST_COMPLETED_FAILURE_STR;
    }
    else if (postStatus == SDL_PBIST_POST_COMPLETED_SUCCESS)
    {
        return PBIST_POST_COMPLETED_SUCCESS_STR;
    }
    return NULL;
}

void PBIST_printPostStatus(SDL_PBIST_postResult *result)
{
    UART_printf("    HW POST MCU Status : %s\n", (PBIST_getPostStatusString(result->mcuPostStatus)) ? : "Invalid");

    return;
}
