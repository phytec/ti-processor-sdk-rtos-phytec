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
#include <hw_include/soc.h>
/* Interrupts, Clear MCU Timer 0 */
#include <hw_include/arch/lld_arch.h>
#include <hw_include/hw_types.h>
/* SCI Client */
#include <drivers/sciclient.h>
/* Interrupt Registrations */
#include<drivers/sciclient/include/tisci/am62x/tisci_devices.h>
#include "ti_drivers_config.h"
#include "ti_board_config.h"

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

void Cdd_IpcAppMsgFrom_RC_Isr(void *notUsed);
void Cdd_IpcAppMsgFromR5FSS0_0Isr(void *notUsed);



/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */


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
	System_init();
	Sciclient_init(1);
    return;
}

#if defined(CDD_IPC_REMOTE_CORE_M4FSS0_0_USED)
/** \brief Register interrupt handler for new message notification from
 *          core MCU 20
 */
static void Cdd_IpcApp_MbIntRegForM4FSS0_0(void)
{
	HwiP_Params          hwiPrms;
    //Int32 retVal;
    //HwiP_Handle hwiHandle;
	static HwiP_Object          IPC_hwObj;
	int32_t              status = SystemP_SUCCESS;
	/* Register interrupt */
	HwiP_Params_init(&hwiPrms);
	hwiPrms.args          = NULL;
    hwiPrms.callback   =  &Cdd_IpcAppMsgFrom_RC_Isr;
    hwiPrms.priority   = 1U;
    hwiPrms.eventId    = 0U; /* NOT USED ? */
    hwiPrms.intNum = INTRTR_CFG_MBX_CLST1_USR0_MCU_R5FSS0_0_TO_MCU_M4FSS0_0_245;
	status = HwiP_construct(&IPC_hwObj, &hwiPrms);
    if(SystemP_SUCCESS != status)
    {
        AppUtils_Printf(MSG_NORMAL,
                        CDD_IPC_APP ": Error Could not register ISR to receive"
                        " from M4FSS0_0!!!\n");
    }
    return;
}
#endif /* CDD_IPC_REMOTE_CORE_M4FSS0_0_USED */


/** \brief Interrupt from mailbox for all cores registrations */
static void Cdd_IpcApp_InterruptConfig(void)
{

#if defined(CDD_IPC_REMOTE_CORE_M4FSS0_0_USED)
   Cdd_IpcApp_MbIntRegForM4FSS0_0();
#endif
    return;
}

#define CDD_IPC_START_SEC_ISR_CODE
#include "Cdd_Ipc_MemMap.h"

#if defined(CDD_IPC_REMOTE_CORE_M4FSS0_0_USED)
 FUNC(void, CDD_IPC_CODE_FAST)
                                Cdd_IpcAppMsgFrom_RC_Isr(void *notUsed)
{
    /* Invoke M4FSS0_0 Isr handler */
    Cdd_IpcIrqMbxFromRC();
}
#endif

#define CDD_IPC_STOP_SEC_ISR_CODE
#include "Cdd_Ipc_MemMap.h"

/*EoF*/
