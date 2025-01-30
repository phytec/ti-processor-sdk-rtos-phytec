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
 *  \file     McspiApp_Startup.c
 *
 *  \brief    This file contains the pinmux, interrupt and clock configuration.
 */
/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "WdgApp_Startup.h"
#include <drivers/sciclient.h>
/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
extern uint32 WdgApp_TestPassed;
/**< WDG App Status Flag */
extern volatile uint32 WdgApp_IsrFlag;
/**< WDG App ISR Flag */
extern uint32 WdgApp_RtiInstance;
/**< RTI INSTANCE */
extern uint32 WdgApp_UserInput;
/**< User Input selected whether Interrupt mode or External Error Pin */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */
void WdgApp_Startup()
{
    /* Initialize ESM */
    WdgApp_ESMInit();
    /* Register ESM interrupt */
    WdgApp_InterruptConfig();
    /* Initialize memory sections  */
    AppUtils_WdgSectionInit();
}

void WdgApp_PowerAndClkSrc()
{
	System_init();
}

void WdgApp_InterruptConfig(void)
{
    int32_t status = SystemP_SUCCESS;
    uint32 EsmInt;
    HwiP_Params    hwiPrms;
    static HwiP_Object GptHwiObject;
    HwiP_Params_init(&hwiPrms);
    hwiPrms.args          = 0;
    hwiPrms.priority     = 1U;
    hwiPrms.eventId = 0U; /* NOT USED ? */
    EsmInt = MCU_ESM_INTID;
    hwiPrms.callback = &WdgApp_ESMIsr;
    hwiPrms.intNum = EsmInt;
    status = HwiP_construct(&GptHwiObject, &hwiPrms);
    if(SystemP_SUCCESS != status)
    {
        AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
            "Error Could not register ISR !!!\n");
    }
}

void WdgApp_ESMInit()
{	
	
    /* ESM Soft Reset */		
    LLD_ESMReset (MCU_ESM_INSTANCE);			

    /* The below function can be changed to force an error for diagnostic
     * reasons. Make sure we're not in force error mode */
    LLD_ESMSetMode(MCU_ESM_INSTANCE, ESM_OPERATION_MODE_NORMAL);

    /* Enable this ESM Error Signal */
    LLD_ESMEnableIntr(MCU_ESM_INSTANCE, MCU_ESM_ERR_SIG);
    LLD_ESMEnableIntr(MCU_ESM_INSTANCE, MCU_ESM_INTID_IN);

    /* Set the output interrupt priority level */
   LLD_ESMSetIntrPriorityLvl(MCU_ESM_INSTANCE, MCU_ESM_ERR_SIG,
                                ESM_INTR_PRIORITY_LEVEL_HIGH);
    LLD_ESMSetIntrPriorityLvl(MCU_ESM_INSTANCE, MCU_ESM_INTID_IN,
                              ESM_INTR_PRIORITY_LEVEL_HIGH);

    /* Enable Error Pin on this ESM Error Signal */
    LLD_ESMSetInfluenceOnErrPin(MCU_ESM_INSTANCE, MCU_ESM_ERR_SIG, TRUE);
    LLD_ESMSetInfluenceOnErrPin(MCU_ESM_INSTANCE, MCU_ESM_INTID_IN, TRUE);

    /* Enable for all ESM Error Signals */
    LLD_ESMEnableGlobalIntr((MCU_ESM_INSTANCE));
	
}

void WdgApp_EnableESMErrorPin(void)
{
    uint32 regVal;

    /* MCU_SAFETY_ERRORn PAD configuration */
    /* Unlock lock key registers for Partition 0: IO PAD
     configuration registers in WKUP_CTRL_MMR */
    /* write Partition 0 Lock Key 0 Register */
    LLD_REG32_WR(LLD_MCU_PADCFG_CTRL0_CFG0_BASE + 0x1008, 0x68EF3490);
    /* write Partition 7 Lock Key 1 Register */
    LLD_REG32_WR(LLD_MCU_PADCFG_CTRL0_CFG0_BASE + 0x100c, 0xD172BC5A);
    /* Check for unlock */
    regVal = LLD_REG32_RD(LLD_MCU_PADCFG_CTRL0_CFG0_BASE + 0x1008);
    while ((regVal & 0x1) != 0x1U)
    {
        regVal = LLD_REG32_RD(LLD_MCU_PADCFG_CTRL0_CFG0_BASE + 0x1008);
    }
    /* Unlocking done */

    regVal = LLD_REG32_RD(LLD_MCU_PADCFG_CTRL0_CFG0_BASE + 0x4060U);
    regVal |= 0x00000U;
    LLD_REG32_WR(LLD_MCU_PADCFG_CTRL0_CFG0_BASE + 0x4060U, regVal);

    return;
}


#define WDG_START_SEC_ISR_CODE
#include "Wdg_MemMap.h"

void WdgApp_ESMIsr(void * handle)
{
    uint32 intrStatus;
    RTIDwwdGetStatus(WdgApp_RtiInstance, &intrStatus);
    if ((HW_GET_FIELD(intrStatus, RTI_RTIWDSTATUS_DWDST) != 1U) ||
        (HW_GET_FIELD(intrStatus, RTI_RTIWDSTATUS_DWWD_ST) != 1U))
    {
        WdgApp_TestPassed = E_NOT_OK;
    }
    else
    {
        WdgApp_TestPassed = E_OK;
    }
    RTIDwwdClearStatus(WdgApp_RtiInstance, intrStatus);
    LLD_ESMClearIntrStatus(MCU_ESM_INSTANCE, MCU_ESM_ERR_SIG);
    LLD_ESMClearIntrStatus(MCU_ESM_INSTANCE, MCU_ESM_INTID_IN);

    /* Reset error pin if interrupt mode */
    if (WdgApp_UserInput == 0U)
    {
        LLD_ESMResetErrPin (MCU_ESM_INSTANCE);
		
    }

    WdgApp_IsrFlag = 1U;
}

#define WDG_STOP_SEC_ISR_CODE
#include "Wdg_MemMap.h"
