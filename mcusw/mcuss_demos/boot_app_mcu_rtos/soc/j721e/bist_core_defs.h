/********************************************************************
 * Copyright (C) 2024 Texas Instruments Incorporated.
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
 *  Name        : bist_core_defs.h
*/
#ifndef BIST_CORE_DEFS_H_
#define BIST_CORE_DEFS_H_

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "boot_stage_defs.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
/* Defines total number of Boot Stages for the Main Domain
 * multi-stage boot sequence for J721E SOC */
#define NUM_BOOT_STAGES     3

#define PBIST_HWPOST_MCU_INDEX            (0U)
#define PBIST_INSTANCE_MCU                (1U) // Not supported in example
#define PBIST_INSTANCE_MAIN_PULSAR_0      (2U)
#define PBIST_INSTANCE_MAIN_PULSAR_1      (3U)
#define PBIST_INSTANCE_C7X                (4U)
#define PBIST_INSTANCE_A72                (5U)
#define PBIST_INSTANCE_VPAC               (6U)
#define PBIST_INSTANCE_DMPAC              (7U)
#define PBIST_INSTANCE_MAIN_NAVSS         (8U)
#define PBIST_INSTANCE_HC                 (9U)
#define PBIST_INSTANCE_C66X_0             (10U)
#define PBIST_INSTANCE_C66X_1             (11U)
#define PBIST_INSTANCE_MAIN_INFRA         (12U)
#define PBIST_INSTANCE_MSMC               (13U)
#define PBIST_INSTANCE_ENCODER            (14U)
#define PBIST_INSTANCE_DECODER            (15U)
#define PBIST_INSTANCE_GPU                (16U) // Not supported in BootApp
#define PBIST_INSTANCE_DSS_EDP_DSI        (17U) // Not supported in BootApp
#define PBIST_INSTANCE_MCU_PULSAR         (18U) // Not supported in BootApp

#define PBIST_MAX_INSTANCE                (PBIST_INSTANCE_MCU_PULSAR+1U)

#define PBIST_INSTANCE_NAME_MAX_LENGTH    16

#define LBIST_HWPOST_DMSC_INDEX       0
#define LBIST_HWPOST_MCU_INDEX        1
#define LBIST_MAIN_MCU0_INDEX         2
#define LBIST_MAIN_MCU1_INDEX         3
#define LBIST_C7X_CORE_INDEX          4
#define LBIST_VPAC_INDEX              5
#define LBIST_DMPAC_INDEX             6
#define LBIST_A72_CORE_INDEX          7

#define LBIST_MAX_CORE_INDEX          (LBIST_DMPAC_INDEX+1)

/* HW POST core definitions */
#define LBIST_POST_CORE_DMSC  LBIST_HWPOST_DMSC_INDEX
#define LBIST_POST_CORE_MCU   LBIST_HWPOST_MCU_INDEX
#define LBIST_POST_CORE_MAX   LBIST_HWPOST_MCU_INDEX

/* ========================================================================== */
/*                          External Dependencies                             */
/* ========================================================================== */

/* Number of PBIST sections run in the pre-boot stage, i.e. before any Main Domain
 * cores are booted */
extern uint8_t num_pbists_pre_boot;

/* Defines PBIST sections run in the pre-boot stage, i.e. before any Main Domain
 * cores are booted */
extern int pbist_pre_boot_stage[];

/* Structure to save the status of the PBIST tests run as part of the pre-boot stage */
extern int pbist_pre_boot_stage_status[];

/* Structure to save the status of the PBIST negative tests run as part of the pre-boot stage */
extern int pbist_pre_boot_stage_neg_status[];

/* Number of LBIST tests run in the pre-boot stage, i.e. before any Main Domain
 * cores are booted */
extern int lbist_pre_boot_stage[];

/* Defines LBIST tests run in the pre-boot stage, i.e. before any Main Domain
 * cores are booted */
extern int lbist_pre_boot_stage_status[];

/* Points to PBIST arrays that are run right before each of the boot stages */
extern int *pbist_array_stage[NUM_BOOT_STAGES];

/* Points to LBIST arrays that are run right before each of the boot stages */
extern int *lbist_array_stage[NUM_BOOT_STAGES];

/* Number of LBIST sections run in the pre-boot stage, i.e. before any Main Domain
 * cores are booted */
extern uint8_t num_lbists_pre_boot;

/* Number of LBIST sections run before each of the boot stages */
extern uint8_t num_lbists_per_boot_stage[NUM_BOOT_STAGES];

/* Number of PBIST sections run before each of the boot stages */
extern uint8_t num_pbists_per_boot_stage[NUM_BOOT_STAGES];

/* Function to return PBIST section name based on PBIST ID number */
const char *pbistName(uint32_t pbistID);

/* Function to return LBIST section name based on LBIST ID number */
const char *lbistName(uint32_t lbistID);

#ifdef __cplusplus
}
#endif
#endif /* BIST_CORE_DEFS_H_ */
