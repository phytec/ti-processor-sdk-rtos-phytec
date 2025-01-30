/*
*
* Copyright (c) 2024 Texas Instruments Incorporated
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
 *  \file     CddIpcAppStartup.c
 *
 *  \brief    This file implements the start up code specific to SoC / core that
 *              application is hosted on.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "CddIpcApp.h"

#include "app_utils.h"
#include <hw_include/lld_intr_router.h> /* CSL for interrupt router */
#include "IntRtr_Cfg.h" /* Defines the map / cfg for
                         * interrupt router, for this example
                         */
#include "SchM_Cdd_Ipc.h" /* Exclusive area */
#include <hw_include/lld_types.h>
#include <hw_include/soc.h>
/* Interrupts, Clear MCU Timer 0 */
#include <hw_include/arch/lld_arch.h>
#include <hw_include/hw_types.h>

/* SCI Client */
#include <ti/drv/sciclient/sciclient.h>
/* PM Lib */
#include <ti/drv/pm/include/pm_types.h>
#include <ti/drv/pm/include/dmsc/pmlib_sysconfig.h>
#include <ti/drv/pm/include/dmsc/pmlib_clkrate.h>

/* Interrupt Registrations */
#include <ti/osal/osal.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
#define MSG_NORMAL      (APP_UTILS_PRINT_MSG_NORMAL)
/**< Message type */
#define MSG_STATUS      (APP_UTILS_PRINT_MSG_STATUS)
/**< Message type */
/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */
static void Cdd_IpcApp_BuildIntList(void);
static void Cdd_IpcApp_PowerAndClkSrc(void);
static void Cdd_IpcApp_InterruptConfig(void);

void Cdd_IpcAppMsgFromMcu20Isr(uintptr_t notUsed);
void Cdd_IpcAppMsgFromMcu21Isr(uintptr_t notUsed);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

#if defined (USE_GENERIC_INT_REG)
/**< Indexed list of dst ids */
const int32_t Cdd_IpcAppSciClientDstCoreIDs[] =
{
  TISCI_DEV_COMPUTE_CLUSTER0_GIC500SS,
  TISCI_DEV_MCU_R5FSS0_CORE0,
  TISCI_DEV_MCU_R5FSS0_CORE1,
  TISCI_DEV_R5FSS0_CORE0,
  TISCI_DEV_R5FSS0_CORE1,
  TISCI_DEV_R5FSS1_CORE0,
  TISCI_DEV_R5FSS1_CORE1,
  TISCI_DEV_R5FSS2_CORE0,
  TISCI_DEV_R5FSS2_CORE1,
  TISCI_DEV_COMPUTE_CLUSTER0_CLEC,
  TISCI_DEV_COMPUTE_CLUSTER0_CLEC,
  TISCI_DEV_COMPUTE_CLUSTER0_CLEC,
  TISCI_DEV_COMPUTE_CLUSTER0_CLEC
};

/**< Indexed list of host ids */
const uint16_t Cdd_IpcAppSciClientHostIDs[] =
{
    TISCI_HOST_ID_A72_0,
    TISCI_HOST_ID_MCU_0_R5_0,
    TISCI_HOST_ID_MCU_0_R5_2,
    TISCI_HOST_ID_MAIN_0_R5_0,
    TISCI_HOST_ID_MAIN_0_R5_2,
    TISCI_HOST_ID_MAIN_1_R5_0,
    TISCI_HOST_ID_MAIN_1_R5_2,
    TISCI_HOST_ID_MAIN_2_R5_0,
    TISCI_HOST_ID_MAIN_2_R5_2,
    TISCI_HOST_ID_C7X_1,
    TISCI_HOST_ID_C7X_2,
    TISCI_HOST_ID_C7X_3,
    TISCI_HOST_ID_C7X_4
};

const uint16_t map_src_id[] =
{
    TISCI_DEV_NAVSS0_MAILBOX_0,
    TISCI_DEV_NAVSS0_MAILBOX_1,
    TISCI_DEV_NAVSS0_MAILBOX_2,
    TISCI_DEV_NAVSS0_MAILBOX_3,
    TISCI_DEV_NAVSS0_MAILBOX_4,
    TISCI_DEV_NAVSS0_MAILBOX_5,
    TISCI_DEV_NAVSS0_MAILBOX_6,
    TISCI_DEV_NAVSS0_MAILBOX_7,
    TISCI_DEV_NAVSS0_MAILBOX_8,
    TISCI_DEV_NAVSS0_MAILBOX_9,
    TISCI_DEV_NAVSS0_MAILBOX_10,
    TISCI_DEV_NAVSS0_MAILBOX_11,
    TISCI_DEV_NAVSS0_MAILBOX1_0,
    TISCI_DEV_NAVSS0_MAILBOX1_1,
    TISCI_DEV_NAVSS0_MAILBOX1_2,
    TISCI_DEV_NAVSS0_MAILBOX1_3,
    TISCI_DEV_NAVSS0_MAILBOX1_4,
    TISCI_DEV_NAVSS0_MAILBOX1_5
};

/**< Maps source core to destination core interrupt number, for a given core */
const uint32 Cdd_IpcAppMbxIntNumToUse[CDD_IPC_CORE_MAX_PROCS] =
{
    INTRTR_CFG_MBX_CLST0_USR1_437_MPU_1_0_TO_MCU_1_0,
    0, 0, 0, 0, 0, 0, 0, 0, 0
};

/**< Maps source core to destination core User ID of Mailbox */
const uint32 CddIpcAppIntToMbxUsrIdMap[CDD_IPC_CORE_MAX_PROCS] =
{
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

#endif /* (USE_GENERIC_INT_REG) */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/** \brief Start up sequence : Program the interrupt muxes / priorities */
void Cdd_IpcApp_Startup(void)
{
    /* CLK and power to the device */
    Cdd_IpcApp_PowerAndClkSrc();

    /* Equivalent to EcuM_AL_SetProgrammableInterrupts */

    Cdd_IpcApp_BuildIntList();
#ifndef UART_ENABLED
    Intc_Init();    /* Interrupt handler initialized, here as other functions
                     * can use API's to clear pending interrupts if any
                     */
    /* The board initialization, initializes interrupt. Not required here */
#endif

    Cdd_IpcApp_InterruptConfig();
    /* Initialize counters, that would be required for timed operations */
    AppUtils_ProfileInit(0);

    /* Initialize memory sections  */
    AppUtils_CddIpcSectionInit();

}

/** \brief Build interrupt list */
static void Cdd_IpcApp_BuildIntList(void)
{
    /* None at this point */
    return;
}

/** \brief None, SBL/GEL powers up the timers and clock sources */
static void Cdd_IpcApp_PowerAndClkSrc(void)
{
    return;
}

#if defined(CDD_IPC_REMOTE_CORE_MCU2_0_USED)
/** \brief Register interrupt handler for new message notification from
 *          core MCU 20
 */
static void Cdd_IpcApp_MbIntRegForMcu20(void)
{
    struct tisci_msg_rm_irq_set_req     rmIrqReq;
    struct tisci_msg_rm_irq_set_resp    rmIrqResp;
    struct tisci_msg_rm_irq_release_req rmIrqRel;
    OsalRegisterIntrParams_t    intrPrms;
    OsalInterruptRetCode_e      osalRetVal;
    Int32 retVal;
    HwiP_Handle hwiHandle;

    /* Since the same mailbox, user but different FIFO is being used to receive
        data from multiple cores, we can at most have one ISR only.
        The ISR implementation will process all FIFOs */
    rmIrqRel.ia_id                  = 0U;
    rmIrqRel.vint                   = 0U;
    rmIrqRel.global_event           = 0U;
    rmIrqRel.vint_status_bit_index  = 0U;

    rmIrqRel.valid_params   = TISCI_MSG_VALUE_RM_DST_ID_VALID |
                              TISCI_MSG_VALUE_RM_DST_HOST_IRQ_VALID |
                              TISCI_MSG_VALUE_RM_SECONDARY_HOST_VALID;
    rmIrqRel.src_id         = TISCI_DEV_NAVSS0_MAILBOX_1;
    rmIrqRel.src_index      = 2U;
    rmIrqRel.dst_id         = TISCI_DEV_R5FSS0_CORE1;
    rmIrqRel.secondary_host = TISCI_HOST_ID_MAIN_0_R5_2;
    rmIrqRel.dst_host_irq   = INTRTR_CFG_MBX_CLST1_USR2_434_MCU_2_0_TO_MCU_2_1_250;

    retVal = Sciclient_rmIrqRelease(&rmIrqRel, SCICLIENT_SERVICE_WAIT_FOREVER);

    rmIrqReq.valid_params           = TISCI_MSG_VALUE_RM_DST_ID_VALID;
    rmIrqReq.valid_params          |= TISCI_MSG_VALUE_RM_DST_HOST_IRQ_VALID;
    rmIrqReq.valid_params          |= TISCI_MSG_VALUE_RM_SECONDARY_HOST_VALID;

    rmIrqReq.src_id                 = TISCI_DEV_NAVSS0_MAILBOX_1;
    rmIrqReq.src_index              = 2U; /* 0 for User 0, 1 for user 1... */
    rmIrqReq.dst_id                 = TISCI_DEV_R5FSS0_CORE1;
    rmIrqReq.secondary_host         = TISCI_HOST_ID_MAIN_0_R5_2;
    rmIrqReq.dst_host_irq           = INTRTR_CFG_MBX_CLST1_USR2_434_MCU_2_0_TO_MCU_2_1_250;

    retVal = Sciclient_rmIrqSet(
                 &rmIrqReq, &rmIrqResp, APP_SCICLIENT_TIMEOUT);
    if(LLD_PASS != retVal)
    {
        AppUtils_Printf(MSG_NORMAL,CDD_IPC_APP
        ": Error in SciClient Interrupt Params Configuration!!!\n");
    }

    /* Interrupt hookup */
    Osal_RegisterInterrupt_initParams(&intrPrms);
    intrPrms.corepacConfig.arg          = (uintptr_t)NULL;
    intrPrms.corepacConfig.isrRoutine   = &Cdd_IpcAppMsgFromMcu20Isr;
    intrPrms.corepacConfig.priority     = 1U;
    intrPrms.corepacConfig.corepacEventNum = 0U;
    intrPrms.corepacConfig.intVecNum    = INTRTR_CFG_MBX_CLST1_USR2_434_MCU_2_0_TO_MCU_2_1_250;

    osalRetVal = Osal_RegisterInterrupt(&intrPrms, &hwiHandle);
    if(OSAL_INT_SUCCESS != osalRetVal)
    {
        AppUtils_Printf(MSG_NORMAL,
                        CDD_IPC_APP ": Error Could not register ISR to receive"
                        " from MCU 2 0 !!!\n");
    }
    return;
}
#endif /* CDD_IPC_REMOTE_CORE_MCU2_0_USED */


/** \brief Interrupt from mailbox for all cores registrations */
static void Cdd_IpcApp_InterruptConfig(void)
{

#if defined(CDD_IPC_REMOTE_CORE_MCU2_0_USED)
    Cdd_IpcApp_MbIntRegForMcu20 ();
#endif

    return;
}

#define CDD_IPC_START_SEC_ISR_CODE
#include "Cdd_Ipc_MemMap.h"

#if defined(CDD_IPC_REMOTE_CORE_MCU2_0_USED)
FUNC(void, CDD_IPC_CODE_FAST)
                                Cdd_IpcAppMsgFromMcu20Isr(uintptr_t notUsed)
{
    /* Invoke MCU 20 Isr handler */
    Cdd_IpcIrqMbxFromMcu_20();
}
#endif

#if defined(CDD_IPC_REMOTE_CORE_MCU2_1_USED)
FUNC(void, CDD_IPC_CODE_FAST)
                                Cdd_IpcAppMsgFromMcu21Isr(uintptr_t notUsed)
{
    /* Invoke MCU 21 Isr handler */
    Cdd_IpcIrqMbxFromMcu_21();
}
#endif

#define CDD_IPC_STOP_SEC_ISR_CODE
#include "Cdd_Ipc_MemMap.h"

/*EoF*/
