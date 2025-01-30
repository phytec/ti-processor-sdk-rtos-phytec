/*
*
* Copyright (c) 2020 Texas Instruments Incorporated
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
 *  \file lbist_defs.c
 *
 *  \brief LBIST SOC-specific structures and functions
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <stdint.h>
#include <string.h>
#include <sdl_types.h>
#include <soc.h>
#include <src/sdl/sdl_lbist.h>
#include <ti/drv/uart/UART_stdio.h>
#include <ti/drv/sciclient/sciclient.h>

#include "lbist_utils.h"
#include "lbist_defs.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

#define A72_NUM_AUX_DEVICES            3

/* HW POST run status definitions */
#define LBIST_POST_COMPLETED_SUCCESS_STR      "SDL_LBIST_POST_COMPLETED_SUCCESS"
#define LBIST_POST_COMPLETED_FAILURE_STR      "SDL_LBIST_POST_COMPLETED_FAILURE"
#define LBIST_POST_ATTEMPTED_TIMEOUT_STR      "SDL_LBIST_POST_TIMEOUT"
#define LBIST_POST_NOT_RUN_STR                "SDL_LBIST_POST_NOT_RUN"

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */
char * LBIST_getPostStatusString(SDL_LBIST_postStatus postStatus);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

uint32_t LBIST_A72AuxDevList[A72_NUM_AUX_DEVICES] =
{
	TISCI_DEV_A72SS0,
	TISCI_DEV_A72SS0_CORE0,
	TISCI_DEV_A72SS0_CORE1
};


LBIST_TestHandle_t LBIST_TestHandleArray[LBIST_MAX_CORE_INDEX+1] =
{
 /* HW POST - SMS - Checks MISR results only */
 
 {
  .coreName               = "HWPOST - SMS",
  .numAuxDevices          = 0u,                       /* No Aux devices */
  .hwPostCoreCheck        = true,
  .hwPostCoreNum          = LBIST_POST_CORE_SMS,
 },
 /* HW POST - MCU - Checks MISR results only */
 {
  .coreName               = "HWPOST - MCU",
  .numAuxDevices          = 0u,                       /* No Aux devices */
  .hwPostCoreCheck        = true,
  .hwPostCoreNum          = LBIST_POST_CORE_MCU,
 },                  /* LBIST event handler */

 /* Main R5F 0 */
 {
  .coreName               = "Main R5F0-0",
  .instance               = SDL_LBIST_INST_MAINR5F0,
  .secondaryCoreNeeded    = true,             /* Secondary core needed */
  .wfiCheckNeeded         = false,            /* wfi check needed */
  .secCoreName            = "Main R5F1-0",    /* Secondary core */
  .cpuStatusFlagMask      = TISCI_MSG_VAL_PROC_BOOT_STATUS_FLAG_R5_WFI, /* Expected boot status value for wfi */
  .tisciProcId            = SCICLIENT_PROC_ID_R5FSS0_CORE0, /* Main R5F core 0 Proc Id */
  .tisciSecProcId         = SCICLIENT_PROC_ID_R5FSS0_CORE1, /* Main R5F core 1 Proc Id */
  .tisciDeviceId          = TISCI_DEV_R5FSS0_CORE0,   /* Main R5F core 0 Device Id */
  .tisciSecDeviceId       = TISCI_DEV_R5FSS0_CORE1,   /* Main R5F core 1 Device Id */
  .numAuxDevices          = 0u,                       /* No Aux devices */
 },
 /* Main R5F 1 */
 {
  .coreName               = "Main R5F1-0",
  .instance               = SDL_LBIST_INST_MAINR5F1,
  .secondaryCoreNeeded    = true,            /* Secondary core needed */
  .wfiCheckNeeded         = false,           /* wfi check needed */
  .secCoreName            = "Main R5F1-1",   /* Secondary core */
  .cpuStatusFlagMask      = TISCI_MSG_VAL_PROC_BOOT_STATUS_FLAG_R5_WFI, /* Expected boot status value for wfi */
  .tisciProcId            = SCICLIENT_PROC_ID_R5FSS1_CORE0, /* Main R5F core 0 Proc Id */
  .tisciSecProcId         = SCICLIENT_PROC_ID_R5FSS1_CORE1, /* Main R5F core 1 Proc Id */
  .tisciDeviceId          = TISCI_DEV_R5FSS1_CORE0,  /* Main R5F core 0 Device id */
  .tisciSecDeviceId       = TISCI_DEV_R5FSS1_CORE1,  /* Main R5F core 1 Device id */
  .numAuxDevices          = 0u,                      /* No Aux devices */
 },
 /* C7x0 */
 {
  .coreName               = "C7x0 ",
  .instance               = SDL_LBIST_INST_C7X0,
  .secondaryCoreNeeded    = false,  /* Secondary core needed */
  .wfiCheckNeeded         = false,  /* wfi check needed */
  .secCoreName            = "None",   /* Secondary core */
  .cpuStatusFlagMask      = TISCI_MSG_VAL_PROC_BOOT_STATUS_FLAG_R5_WFI, /* Expected boot status value for wfi */
  .tisciProcId            = SCICLIENT_PROC_ID_COMPUTE_CLUSTER0_C71SS0_0,  /* C7x Proc Id */
  .tisciSecProcId         = SCICLIENT_PROC_ID_COMPUTE_CLUSTER0_C71SS1_0,
  .tisciDeviceId          = TISCI_DEV_COMPUTE_CLUSTER0_C71SS0_0,          /* C7x Device Id */
  .tisciSecDeviceId       = TISCI_DEV_COMPUTE_CLUSTER0_C71SS1_0,
  .numAuxDevices          = 0u,                        /* No Aux devices */
 },
 /* C7x1 */
 {
  .coreName               = "C7x1 ",
  .instance               = SDL_LBIST_INST_C7X1,
  .secondaryCoreNeeded    = false,  /* Secondary core needed */
  .wfiCheckNeeded         = false,  /* wfi check needed */
  .secCoreName            = "None",   /* Secondary core */
  .cpuStatusFlagMask      = TISCI_MSG_VAL_PROC_BOOT_STATUS_FLAG_R5_WFI, /* Expected boot status value for wfi */
  .tisciProcId            = SCICLIENT_PROC_ID_COMPUTE_CLUSTER0_C71SS0_0,  /* C7x Proc Id */
  .tisciSecProcId         = SCICLIENT_PROC_ID_COMPUTE_CLUSTER0_C71SS1_0,
  .tisciDeviceId          = TISCI_DEV_COMPUTE_CLUSTER0_C71SS0_0,          /* C7x Device Id */
  .tisciSecDeviceId       = TISCI_DEV_COMPUTE_CLUSTER0_C71SS1_0,
  .numAuxDevices          = 0u,                        /* No Aux devices */
 },
 /* VPAC0 */
 {
  .coreName               = "VPAC0",
  .instance               = SDL_LBIST_INST_VPAC0,
  .secondaryCoreNeeded    = false,           /* Secondary core needed */
  .wfiCheckNeeded         = false,           /* wfi check needed */
  .secCoreName            = "None",          /* Secondary core */
  .cpuStatusFlagMask      = TISCI_MSG_VAL_PROC_BOOT_STATUS_FLAG_R5_WFI, /* Expected boot status value for wfi */
  .tisciProcId            = 0,  /* No proc id */
  .tisciSecProcId         = 0,  /* No Proc Id */
  .tisciDeviceId          = TISCI_DEV_VPAC0,                       /* VPAC Device Id */
  .tisciSecDeviceId       = 0,
  .numAuxDevices          = 0u,                                    /* No Aux devices */
 },

 /* DMPAC */
 {
  .coreName               = "DMPAC",
  .instance               = SDL_LBIST_INST_DMPAC,
  .secondaryCoreNeeded    = false,           /* Secondary core needed */
  .wfiCheckNeeded         = false,           /* wfi check needed */
  .secCoreName            = "None",          /* Secondary core */
  .cpuStatusFlagMask      = TISCI_MSG_VAL_PROC_BOOT_STATUS_FLAG_R5_WFI, /* Expected boot status value for wfi */
  .tisciProcId            = 0,  /* No proc id */
  .tisciSecProcId         = 0,  /* No Proc Id */
  .tisciDeviceId          = TISCI_DEV_DMPAC0,                       /* DMPAC Device Id */
  .tisciSecDeviceId       = 0,
  .numAuxDevices          = 0u,                                     /* No Aux devices */
 },

 /* A72_0*/
 {
  .coreName               = "A72 core 0",
  .instance               = SDL_LBIST_INST_A72,
  .secondaryCoreNeeded    = true,           /* Secondary core needed */
  .wfiCheckNeeded         = false,          /* wfi check needed */
  .secCoreName            = "A72 core 1",   /* Secondary core */
  .cpuStatusFlagMask      = TISCI_MSG_VAL_PROC_BOOT_STATUS_FLAG_R5_WFI, /* Expected boot status value for wfi */
  .tisciProcId            = SCICLIENT_PROC_ID_A72SS0_CORE0, /* A72 core 0 Proc Id */
  .tisciSecProcId         = SCICLIENT_PROC_ID_A72SS0_CORE1, /* A72 core 1 Proc Id */
  .tisciDeviceId          = TISCI_DEV_A72SS0_CORE0,  /* A72 core 0 Device Id */
  .tisciSecDeviceId       = TISCI_DEV_A72SS0_CORE1,  /* A72 core 1 Device Id */
  .numAuxDevices          = A72_NUM_AUX_DEVICES,     /* Number of Aux devices */
  .auxDeviceIdsP          = &LBIST_A72AuxDevList[0], /* Array of Aux device ids */
 },


};

/* ========================================================================== */
/*                            External Variables                              */
/* ========================================================================== */


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

char * LBIST_getPostStatusString(SDL_LBIST_postStatus postStatus)
{
    if (postStatus == SDL_LBIST_POST_NOT_RUN)
    {
        return LBIST_POST_NOT_RUN_STR;
    }
    else if (postStatus == SDL_LBIST_POST_TIMEOUT)
    {
        return LBIST_POST_ATTEMPTED_TIMEOUT_STR;
    }
    else if (postStatus == SDL_LBIST_POST_COMPLETED_FAILURE)
    {
        return LBIST_POST_COMPLETED_FAILURE_STR;
    }
    else if (postStatus == SDL_LBIST_POST_COMPLETED_SUCCESS)
    {
        return LBIST_POST_COMPLETED_SUCCESS_STR;
    }
    return NULL;
}

void LBIST_printPostStatus(SDL_LBIST_postResult *result)
{
    UART_printf("    HW POST MCU Status : %s\n", (LBIST_getPostStatusString(result->mcuPostStatus)) ? : "Invalid");
    UART_printf("    HW POST DMSC Status : %s\n", (LBIST_getPostStatusString(result->smsPostStatus)) ? : "Invalid");

    return;
}

