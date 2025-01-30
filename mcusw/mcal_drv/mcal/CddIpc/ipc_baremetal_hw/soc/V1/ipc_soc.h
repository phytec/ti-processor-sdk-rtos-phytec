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
 *            This is IPC documentation specific to J7 SoC
 *
 *  @{
 */

/**
 *  \file ipc_soc.h
 *
 *  \brief IPC Low Level Driver J7 SOC specific file.
 */
#ifndef IPC_SOC_V1_H_
#define IPC_SOC_V1_H_

#include <CddIpc/ipc_baremetal_hw/include/ipc_config.h>
#include "Cdd_IpcCfg.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * For VLAB toolbox 0.14.5.snapshot2 onward,
 * this must be defined for C66x interrupt to work
 * properly.
 */
#define    SUPPORT_C66X_BIT0

#define    IPC_MAILBOX_USER_CNT                  (4U)
#define    MAIN_NAVSS_MAILBOX_INPUTINTR_MAX    (440U)
#define    MAIN_NAVSS_MAILBOX_OUTPUTINTR_MAX   (512U)

#if defined(SOC_J7200)  ||  defined(SOC_J721E) ||defined (SOC_J721S2) || defined (SOC_J784S4)
/* Refer to J7ES interrupt mapping table */
#ifndef QNX_OS
#define    NAVSS512_MPU1_0_INPUT_MAILBOX_OFFSET             (182U)
#else
#define    NAVSS512_MPU1_0_INPUT_MAILBOX_OFFSET             (112U)
#endif
#define    NAVSS512_MCU1R5F0_INPUT_MAILBOX_OFFSET           (400U)
#define    NAVSS512_MCU1R5F1_INPUT_MAILBOX_OFFSET           (404U)
#define    NAVSS512_MCU2R5F0_INPUT_MAILBOX_OFFSET           (216U)
#define    NAVSS512_MCU2R5F1_INPUT_MAILBOX_OFFSET           (248U)
#define    NAVSS512_MCU3R5F0_INPUT_MAILBOX_OFFSET           (280U)
#define    NAVSS512_MCU3R5F1_INPUT_MAILBOX_OFFSET           (312U)
#define    NAVSS512_C66X1_INPUT_MAILBOX_OFFSET              (344U)  /* C66x_intrRouter_0 : 96 */
#define    NAVSS512_C66X2_INPUT_MAILBOX_OFFSET              (376U)  /* C66x_intrRouter_1 : 96 */
#define    NAVSS512_C7X1_INPUT_MAILBOX_OFFSET               (188U)
#endif

#if defined(SOC_AM62X)
#define    NAVSS512_MCU2R5F0_INPUT_MAILBOX_OFFSET           (216U)
#define    NAVSS512_MCU2R5F1_INPUT_MAILBOX_OFFSET           (248U)
#define    NAVSS512_MCU2M4F0_INPUT_MAILBOX_OFFSET			(216U)
//to revisit and check
// #define    NAVSS512_MCU2M4F0_INPUT_MAILBOX_OFFSET			(248U)
#define    NAVSS512_MCU0A53_0_INPUT_MAILBOX_OFFSET			(280U)
#define    NAVSS512_MCU1A53_0_INPUT_MAILBOX_OFFSET			(312U)
#define    NAVSS512_A53SS_1_INPUT_MAILBOX_OFFSET			(344U)
#define    NAVSS512_A53SS_2_INPUT_MAILBOX_OFFSET			(376U)
#endif

#if defined(SOC_AM62AX) || defined(SOC_AM62PX) || defined(SOC_J722S)
#define    NAVSS512_MCU2R5F0_INPUT_MAILBOX_OFFSET           (216U)
#define    NAVSS512_MCU2R5F1_INPUT_MAILBOX_OFFSET           (248U)
#define    NAVSS512_A53SS0_0_INPUT_MAILBOX_OFFSET			(216U)
#define    NAVSS512_MCU_R5FSS0_0_INPUT_MAILBOX_OFFSET		(248U)
#define    NAVSS512_MCU0A53_0_INPUT_MAILBOX_OFFSET			(280U)
#define    NAVSS512_MCU1A53_0_INPUT_MAILBOX_OFFSET			(312U)
#define    NAVSS512_A53SS_1_INPUT_MAILBOX_OFFSET			(344U)
#define    NAVSS512_A53SS_2_INPUT_MAILBOX_OFFSET			(376U)
#endif

#define IPC_MCU_NAVSS0_INTR0_CFG_BASE    (CSL_NAVSS_MAIN_INTR_ROUTER_CFG_REGS_0_BASE)

/* Could not find the RAT address in CSL */
#define IPC_C66X_RAT_BASE        (0x07ff0030U)

/* Virtual Address used for C66x to access j7_c66_interrupt_router */
#define IPC_C66X_INTR_VA_BASE    (0x18000000U)

/* Sclient does not configure RAT, so it must
 * be done C66x core
 * RAT configuration for C66x_1 and C66x_2 */
#define   IPC_C66X_1_INTR_PA_BASE     (CSL_C66SS0_INTROUTER0_INTR_ROUTER_CFG_BASE)
#define   IPC_C66X_2_INTR_PA_BASE     (CSL_C66SS1_INTROUTER0_INTR_ROUTER_CFG_BASE)
#define   C66X1_MBINTR_INPUT_BASE     (74U)
#define   C66X1_MBINTR_OFFSET         (84U)
#define   C66X1_MBINTR_OUTPUT_BASE    (96U)
#define   C66X2_MBINTR_INPUT_BASE     (74U)
#define   C66X2_MBINTR_OUTPUT_BASE    (96U)
#define   C66X2_MBINTR_OFFSET         (84U)

/* Request Sciclient for available resource in group2 NavSS
 * For C7x, DMSC does not configure or map CLEC router
 * it must be done by C7x software.
 * StartEvent = 672 (CSLR_COMPUTE_CLUSTER0_GIC500SS_SPI_NAVSS0_INTR_ROUTER_0_OUTL_INTR_128)
 * ClecEvent  = 1664 corresponds to 672
 */
#define   C7X_CLEC_BASE_ADDR              (CSL_COMPUTE_CLUSTER0_CLEC_REGS_BASE)

/* Base NAVSS event from group 2 */
#define   IPC_C7X_COMPUTE_CLUSTER_OFFSET  (CSLR_COMPUTE_CLUSTER0_GIC500SS_SPI_NAVSS0_INTR_ROUTER_0_OUTL_INTR_128)

/* CLEC event corresponding to Base NAVSS event */
#define   C7X1_CLEC_BASE_GR2_NAVSS        (1664U)

/* User selected IRQ number */
#define   C7X1_MBINTR_OFFSET              (2U)

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
/* None */

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
#if defined(SOC_AM62X)  ||  defined(SOC_AM62AX) || defined(SOC_AM62PX)
#define TISCI_DEV_R5FSS0_CORE0 121
#define TISCI_DEV_R5FSS1_CORE0 247
#define SCICLIENT_SERVICE_WAIT_FOREVER                    (0xFFFFFFFFU)
#endif

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

#endif /* #ifndef IPC_SOC_V1_H_ */

/* @} */
