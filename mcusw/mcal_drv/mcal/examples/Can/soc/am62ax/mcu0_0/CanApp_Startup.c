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
 *  \file     CanApp_Startup.c
 *
 *  \brief    This file contains the pinmux, interrupt and clock configuration.
 */
/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "CanApp_Startup.h"
#include "app_utils.h"
#include "examples_config/IntRtr_Cfg/soc/am62ax/mcu0_0/IntRtr_Cfg.h"
#include <hw_include/soc.h>
/* Interrupts, Clear MCU Timer 0 */
#include <hw_include/arch/lld_arch.h>
#include <hw_include/hw_types.h>
#include <hw_include/lldr_timer.h>
/* SCI Client */
#include <drivers/sciclient.h>
/* PM Lib */
/* Interrupt Registrations */
#include <hw_include/arch/r5/interrupt.h>
/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
uint32 gRmIrqTreeCount = 0;
uint32 gRmIrqTree = 0;


uint32                      CanApp_IntNumbers[CAN_MAX_CONTROLLER];
/**< Store the interrupt numbers for all enabled CAN instances */
uint32                      CanApp_InstanceSrcId[CAN_MAX_CONTROLLER];
/**< Store the interrupt source id for all enabled CAN instances */
CanApp_IsrType              CanApp_Isr[CAN_MAX_CONTROLLER];
/**< Associated ISR */
uint32                      CanApp_RoutIntNumbers[CAN_MAX_CONTROLLER];
/**< Store the output interrupt number routed */
extern Can_PduType          CanApp_Pdu, CanApp_Pdu1;
extern Can_PduType          CanApp_Pdu2, CanApp_Pdu3, CanApp_Pdu4, CanApp_Pdu5;
/**< Variable used for copying protocol data unit */
extern uint8                CanApp_InputData[64U];
/**< Variables which contains Can PDU data used in Can transmission */
extern const Can_PduType    *PduInfo;
/**< Variable which contains Can PDU data passed during Transmission */
/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */
/** \brief Start up sequence : Program the interrupt muxes / priorities */
void CanApp_Startup(void)
{
    /* Equivalent to EcuM_AL_SetProgrammableInterrupts */
    CanApp_BuildIntList();
    Intc_Init();    /* Interrupt handler initialized, here as other functions
                  //   * can use API's to clear pending interrupts if any
                   //  */
    CanApp_InterruptConfig();

    /* Initialize counters, that would be required for timed operations */
    AppUtils_ProfileInit(0);

    /* Initialize memory sections  */
	AppUtils_CanSectionInit();

    /* Message for CAN-FD(MCU MCAN0 module) */
    /* Setting Up CAN FD Frame*/
    CanApp_Pdu.id           = 0xA0 | 0x40000000U;
    CanApp_Pdu.length       = 64U;
    CanApp_Pdu.swPduHandle  = 1U;
    CanApp_Pdu.sdu          = &CanApp_InputData[0U];

    /* Message for CAN-FD(MCU MCAN1 module) */
    /* Setting Up CAN FD Extended Frame*/
    CanApp_Pdu1.id          = 0xB0 | 0xC0000000U;;
    CanApp_Pdu1.length      = 64U;
    CanApp_Pdu1.swPduHandle = 2U;
    CanApp_Pdu1.sdu         = &CanApp_InputData[0U];

    /* Message for CAN-FD(MAIN MCAN0 module) */
    /* Setting Up CAN FD Frame*/
    CanApp_Pdu2.id          = 0xC0 | 0x40000000U;
    CanApp_Pdu2.length      = 64U;
    CanApp_Pdu2.swPduHandle = 3U;
    CanApp_Pdu2.sdu         = &CanApp_InputData[0U];

}

void CanApp_BuildIntList(void)
{
    uint32 idx=0;
    const Can_ConfigType *Can_ConfigPtr;
    uint32 canMaxControllerCnt = 0U;
    /*
     * 1. Determine the number of timers used
     * 2. Build valid timer list
     * 3. Build isr and interrupt number for enabled timers only
     */
    /* Do Can Init */
#if (STD_ON == CAN_VARIANT_PRE_COMPILE)
    Can_ConfigPtr = &CAN_INIT_CONFIG_PC;
    Can_Init((const Can_ConfigType *) NULL_PTR);
    AppUtils_Printf(MSG_NORMAL,
    "CAN_APP: Variant - Pre Compile being used !!!\n");
#else
    Can_ConfigPtr = &CanConfigSet;
    Can_Init(Can_ConfigPtr);
    AppUtils_Printf(MSG_NORMAL,
    "CAN_APP: Variant - Post Build being used !!!\n");
#endif

    canMaxControllerCnt = Can_ConfigPtr->CanMaxControllerCount;
    for (idx = 0U; idx < canMaxControllerCnt; idx++)
    {
        CanApp_IntNumbers[idx] = 0U;
        CanApp_InstanceSrcId[idx] = 0U;
        CanApp_RoutIntNumbers[idx] = 0U;
    }
  idx=0;
    for (idx = 0U; idx < canMaxControllerCnt; idx++)
    {
       
        if (CanConfigSet_CanController_List_PC[idx]->CanControllerInst == CAN_CONTROLLER_INSTANCE_MCU_MCAN0)
            
        {
            CanApp_IntNumbers[idx] = 43;
            CanApp_Isr[idx] = Can_0_Int0ISR;
            CanApp_InstanceSrcId[idx] = TISCI_DEV_MCU_MCAN0;
            
        }

        if (CanConfigSet_CanController_List_PC[idx]->CanControllerInst  == CAN_CONTROLLER_INSTANCE_MCU_MCAN1)
                
        {
            CanApp_IntNumbers[idx] = 46;
            CanApp_Isr[idx] = Can_1_Int0ISR;
            CanApp_InstanceSrcId[idx] = TISCI_DEV_MCU_MCAN1;
            
        }
    }

    return;
}

void CanApp_PlatformInit()
{
    volatile uint32_t regVal  = 0U;

    /* Unlock lock key registers for PAD CONFIG: IO PAD configuration registers in PADCFG_CTRL */
	
    /* write Partition 1 Lock Key 0 Register */	
    LLD_REG32_WR(0x4080000UL + 0x5008, 0x68EF3490);
	
    /* write Partition 1 Lock Key 1 Register */
    LLD_REG32_WR(0x4080000UL + 0x500C, 0xD172BC5A);
	
    /* Check for unlock */
    regVal = LLD_REG32_RD(0x4080000UL + 0x5008);
    while ((regVal & 0x1) != 0x1U)
    {
        regVal = LLD_REG32_RD(0x4080000UL + 0x5008);
    }

    /* Unlocking done */
    /* Below code will be replaced by Port module in further releases */
    /* MCU MCAN 0 Tx PAD configuration */
    regVal = 0x10000U;
    LLD_REG32_WR(0x4080000UL + 0x4034U, regVal);
    /* MCU MCAN 0 Rx PAD configuration */
    regVal = 0x50000U;
    LLD_REG32_WR(0x4080000UL + 0x4038U, regVal);
    /* MCU MCAN 1 Tx PAD configuration */
    regVal = 0x60000U;
    LLD_REG32_WR(0x4080000UL + 0x403CU, regVal);
    /* MCU MCAN 1 Rx PAD configuration */
    regVal = 0x60000U;
    LLD_REG32_WR(0x4080000UL + 0x4040U, regVal);
	/* MAIN MCAN 0 Tx PAD configuration */
    regVal = 0x60000U;
    LLD_REG32_WR(0x4080000UL + 0x41D8U, regVal);
    /* MAIN MCAN 0 Rx PAD configuration */
    regVal = 0x60000U;
    LLD_REG32_WR(0x4080000UL + 0x41DCU, regVal);
	
#if defined (SOC_J721E) || defined (SOC_J7200)
    /* Unlock lock key registers for Partition 7: IO PAD
       configuration registers in MAIN_CTRL_MMR */
    /* write Partition 7 Lock Key 0 Register */
    LLD_REG32_WR(CSL_WKUP_CTRL_MMR0_CFG0_BASE + 0x1D008, 0x68EF3490);
    /* write Partition 7 Lock Key 1 Register */
    LLD_REG32_WR(CSL_WKUP_CTRL_MMR0_CFG0_BASE + 0x1D00C, 0xD172BC5A);
    /* Check for unlock */
    regVal = LLD_REG32_RD(CSL_WKUP_CTRL_MMR0_CFG0_BASE + 0x1D008);
    while ((regVal & 0x1) != 0x1U)
    {
        regVal = LLD_REG32_RD(CSL_WKUP_CTRL_MMR0_CFG0_BASE + 0x1D008);
    }

    /* Unlock lock key registers for Partition 7: IO PAD
       configuration registers in MAIN_CTRL_MMR */
    /* write Partition 7 Lock Key 0 Register */
    LLD_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + 0x1D008, 0x68EF3490);
    /* write Partition 7 Lock Key 1 Register */
    LLD_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + 0x1D00C, 0xD172BC5A);
    /* Check for unlock */
    regVal = LLD_REG32_RD(CSL_CTRL_MMR0_CFG0_BASE + 0x1D008);
    while ((regVal & 0x1) != 0x1U)
    {
        regVal = LLD_REG32_RD(CSL_CTRL_MMR0_CFG0_BASE + 0x1D008);
    }

    /* Unlocking done */
    /* Below code will be replaced by Port module in further releases */
    /* MCU MCAN 0 Tx PAD configuration */
    regVal = 0x60000U;
    LLD_REG32_WR(CSL_WKUP_CTRL_MMR0_CFG0_BASE + 0x1C0A8U, regVal);
    /* MCU MCAN 0 Rx PAD configuration */
    regVal = 0x60000U;
    LLD_REG32_WR(CSL_WKUP_CTRL_MMR0_CFG0_BASE + 0x1C0ACU, regVal);
    /* MCU MCAN 1 Tx PAD configuration */
    regVal = 0x60000U;
    LLD_REG32_WR(CSL_WKUP_CTRL_MMR0_CFG0_BASE + 0x1C0C0U, regVal);
    /* MCU MCAN 1 Rx PAD configuration */
    regVal = 0x60000U;
    LLD_REG32_WR(CSL_WKUP_CTRL_MMR0_CFG0_BASE + 0x1C0C4U, regVal);
    /* MAIN MCAN 0 Tx PAD configuration */
    regVal = 0x60000U;
    LLD_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + 0x1C20CU, regVal);
    /* MAIN MCAN 0 Rx PAD configuration */
    regVal = 0x60000U;
    LLD_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + 0x1C208U, regVal);

    /* MAIN MCAN 4 Tx PAD configuration */
    regVal = 0x60006U;
    LLD_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + 0x1C020U, regVal);
    /* MAIN MCAN 4 Rx PAD configuration */
    regVal = 0x60006U;
    LLD_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + 0x1C024U, regVal);
    /* MAIN MCAN 9 Tx PAD configuration */
    regVal = 0x60006U;
    LLD_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + 0x1C0CCU, regVal);
    /* MAIN MCAN 9 Rx PAD configuration */
    regVal = 0x60006U;
    LLD_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + 0x1C0D0U, regVal);
    /* MAIN MCAN 11 Tx PAD configuration */
    regVal = 0x60006U;
    LLD_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + 0x1C11CU, regVal);
    /* MAIN MCAN 11 Rx PAD configuration */
    regVal = 0x60006U;
    LLD_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + 0x1C120U, regVal);
#endif
 
}


void CanApp_InterruptConfig(void)
{
	
	HwiP_Params          hwiPrms;
    const Can_ConfigType *Can_ConfigPtr;
    uint32 idx;
	static HwiP_Object          CAN_hwObj;
	int32_t              status = SystemP_SUCCESS;

#if (STD_ON == CAN_VARIANT_PRE_COMPILE)
    Can_ConfigPtr = &CAN_INIT_CONFIG_PC;
#else
    Can_ConfigPtr = &CanConfigSet;
#endif

    for (idx = 0U; idx < Can_ConfigPtr->CanMaxControllerCount; idx++)
    {
			/* Register interrupt */
			HwiP_Params_init(&hwiPrms);
            hwiPrms.args          = CanApp_Isr[idx];
            hwiPrms.callback   =  &CanApp_CanXIsr;
            hwiPrms.priority   = 1U;
            hwiPrms.eventId    = 0U; /* NOT USED ? */
            hwiPrms.intNum     = CanApp_IntNumbers[idx];
            status = HwiP_construct(&CAN_hwObj, &hwiPrms);
            if(SystemP_SUCCESS != status)
            {
                AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL, "CAN_APP"
                                  ": Error in Interrupt Registration %d !!!\n");
            }
               
    }

    return;
}

/** \brief None, SBL/GEL powers up the timers and clock sources */
void CanApp_PowerAndClkSrc(void)
{
    /* Mcu module, when included will replace this operation */
    return;
}


void CanApp_SetupCanFrame(uint32 ctlr_cnt, uint8 *hth, uint32 *mask)
{
	if (CAN_CONTROLLER_INSTANCE_MCU_MCAN0 ==
		CanConfigSet_CanController_List_PC[ctlr_cnt]->CanControllerInst)
	{
		/* CAN Protocol Data Unit */
		PduInfo = &CanApp_Pdu;
		/* Hardware Transmit Handle assigned to MCAN0 */
		*hth     = CAN_HTRH_0;
		/* mask is need for checking id of received message with
		 * sent message as 30th bit the of sent message will be set
		 * for CAN FD frame */
		*mask    = 0x40000000U;
	}

    if (CAN_CONTROLLER_INSTANCE_MCU_MCAN1 ==
        CanConfigSet_CanController_List_PC[ctlr_cnt]->CanControllerInst)
    {
        /* CAN Protocol Data Unit */
        PduInfo = &CanApp_Pdu2;
        /* Hardware Transmit Handle assigned to MCAN0 */
        *hth     = CAN_HTRH_2;
        /* mask is need for checking id of received message with
         * sent message as 30th bit the of sent message will be set
         * for CAN FD frame */
        *mask    = 0x40000000U;
    }
    
}

void Can_delay (uint32 count)
{
    volatile uint32 x = count;
    while (x)
        x--;
}
