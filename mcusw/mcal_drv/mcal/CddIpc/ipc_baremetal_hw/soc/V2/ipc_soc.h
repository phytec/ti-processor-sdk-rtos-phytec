/*
 *  Copyright (c) Texas Instruments Incorporated 2024
 *  All rights reserved.
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
 *  \ingroup DRV_IPC_MODULE
 *  \defgroup DRV_IPC_SOC_MODULE IPC SoC Config
 *            This is IPC documentation specific to J721S2 SoC
 *
 *  @{
 */

/**
 *  \file ipc_soc.h
 *
 *  \brief IPC Low Level Driver J7 SOC specific file.
 */
#ifndef IPC_SOC_V2_H_
#define IPC_SOC_V2_H_

#include <CddIpc/ipc_baremetal_hw/include/ipc_config.h>
#include "Cdd_IpcCfg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief VRing Buffer Size required for all core
 * combinations.
 * TBD: This number can be smaller, if needed, for # cores on J721S2
 */

#define    IPC_MAILBOX_USER_CNT                  (4U)
#define    MAIN_NAVSS_MAILBOX_INPUTINTR_MAX    (440U)
#define    MAIN_NAVSS_MAILBOX_OUTPUTINTR_MAX   (512U)

/* Refer to J721S2 interrupt mapping table */
#ifndef QNX_OS
#define    NAVSS512_MPU1_0_INPUT_MAILBOX_OFFSET             (105U)
#else
#define    NAVSS512_MPU1_0_INPUT_MAILBOX_OFFSET             (112U)
#endif
#define    NAVSS512_MCU1R5F0_INPUT_MAILBOX_OFFSET           (400U)
#define    NAVSS512_MCU1R5F1_INPUT_MAILBOX_OFFSET           (404U)
#define    NAVSS512_MCU2R5F0_INPUT_MAILBOX_OFFSET           (219U)
#define    NAVSS512_MCU2R5F1_INPUT_MAILBOX_OFFSET           (251U)
#define    NAVSS512_MCU3R5F0_INPUT_MAILBOX_OFFSET           (283U)
#define    NAVSS512_MCU3R5F1_INPUT_MAILBOX_OFFSET           (315U)
#define    NAVSS512_C7X1_INPUT_MAILBOX_OFFSET               (158U)
#define    NAVSS512_C7X2_INPUT_MAILBOX_OFFSET               (179U)

#define IPC_MCU_NAVSS0_INTR0_CFG_BASE    (CSL_NAVSS0_INTR0_INTR_ROUTER_CFG_BASE)

/* For C7x, DMSC does not configure or map CLEC router
 * it must be done by C7x software.
 */
#define   C7X_CLEC_BASE_ADDR              (CSL_COMPUTE_CLUSTER0_CLEC_REGS_BASE)

#define   C7X_CLEC_OFFSET                 (1024U - 32U)
/* CLEC event corresponding to Base NAVSS event */
#define   C7X1_CLEC_BASE_GR2_NAVSS        (1664U)

/* User selected IRQ number */
#define   C7X1_MBINTR_OFFSET            (59U)

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
/* None */

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
/* None */

/* @} */
/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */
/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */
int32_t Ipc_getIntNumRange(uint32 coreIndex, uint16 *rangeStartP,
        uint16 *rangeNumP);

/* ========================================================================== */
/*                       Static Function Definitions                          */
/* ========================================================================== */

/* None */

#ifdef __cplusplus
}
#endif

#endif /* #ifndef IPC_SOC_V2_H_ */

/* @} */
