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

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
uint32                      CanApp_IntNumbers[CAN_MAX_CONTROLLER];
/**< Store the interrupt numbers for all enabled CAN instances */
uint32                      CanApp_InstanceSrcId[CAN_MAX_CONTROLLER];
/**< Store the interrupt source id for all enabled CAN instances */
CanApp_IsrType              CanApp_Isr[CAN_MAX_CONTROLLER];
/**< Associated ISR */
uint32                      CanApp_RoutIntNumbers[CAN_MAX_CONTROLLER];
/**< Store the output interrupt number routed */
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
                     * can use API's to clear pending interrupts if any
                     */
    CanApp_InterruptConfig();

    /* Initialize counters, that would be required for timed operations */
    AppUtils_ProfileInit(0);

    /* Initialize memory sections  */
	AppUtils_CanSectionInit();

    /* Message for CAN-FD(MAIN MCAN0 module) */
    /* Setting Up CAN FD Frame*/
    CanApp_Pdu2.id          = 0xC0 | 0x40000000U;
    CanApp_Pdu2.length      = 64U;
    CanApp_Pdu2.swPduHandle = 3U;
    CanApp_Pdu2.sdu         = &CanApp_InputData[0U];

    /* Message for CAN-FD(MAIN MCAN4 module) */
    /* Setting Up CAN FD Frame*/
    CanApp_Pdu3.id          = 0xD0 | 0xC0000000U;
    CanApp_Pdu3.length      = 64U;
    CanApp_Pdu3.swPduHandle = 4U;
    CanApp_Pdu3.sdu         = &CanApp_InputData[0U];

    /* Message for CAN-FD(MAIN MCAN9 module) */
    /* Setting Up CAN FD Frame*/
    CanApp_Pdu4.id          = 0xE0 | 0x40000000U;
    CanApp_Pdu4.length      = 64U;
    CanApp_Pdu4.swPduHandle = 5U;
    CanApp_Pdu4.sdu         = &CanApp_InputData[0U];

    /* Message for CAN-FD(MAIN MCAN11 module) */
    /* Setting Up CAN FD Frame*/
    CanApp_Pdu5.id          = 0xF0 | 0xC0000000U;
    CanApp_Pdu5.length      = 64U;
    CanApp_Pdu5.swPduHandle = 6U;
    CanApp_Pdu5.sdu         = &CanApp_InputData[0U];
}

void CanApp_BuildIntList(void)
{
    uint32 idx, flag, intNum;
    const Can_ConfigType *Can_ConfigPtr;
    CanApp_IsrType pIsrHandler = NULL;
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

    /* Enable SCI Client */
    canMaxControllerCnt = Can_ConfigPtr->CanMaxControllerCount;
    for (idx = 0U; idx < canMaxControllerCnt; idx++)
    {
        CanApp_IntNumbers[idx] = 0U;
    }

    for (idx = 0U; idx < canMaxControllerCnt; idx++)
    {
        flag = 0U;
        if (CAN_CONTROLLER_INSTANCE_MCAN0 ==
            CanConfigSet_CanController_List_PC[idx]->CanControllerInst)
        {
            intNum = APP_MCAN_0_INT0;
            pIsrHandler = Can_2_Int0ISR;
            flag = 1U;
        }
        if (CAN_CONTROLLER_INSTANCE_MCAN4 ==
            CanConfigSet_CanController_List_PC[idx]->CanControllerInst)
        {
            intNum = APP_MCAN_4_INT0;
            pIsrHandler = Can_6_Int0ISR;
            flag = 1U;
        }
        if (CAN_CONTROLLER_INSTANCE_MCAN9 ==
            CanConfigSet_CanController_List_PC[idx]->CanControllerInst)
        {
            intNum = APP_MCAN_9_INT0;
            pIsrHandler = Can_11_Int0ISR;
            flag = 1U;
        }
        if (CAN_CONTROLLER_INSTANCE_MCAN11 ==
            CanConfigSet_CanController_List_PC[idx]->CanControllerInst)
        {
            intNum = APP_MCAN_11_INT0;
            pIsrHandler = Can_13_Int0ISR;
            flag = 1U;
        }

        if (0U != flag)
        {
            CanApp_IntNumbers[idx] = intNum;
            CanApp_Isr[idx] = pIsrHandler;
            flag = 0U;
        }
    }

    return;
}

void CanApp_PlatformInit()
{
    uint32 regVal = 0U;
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

    /* Take MCAN transceivers out of STB mode i.e NORMAL Mode */
    CanApp_EnableTransceivers();
}

void CanApp_EnableTransceivers(void)
{
    uint32 regVal = 0U;
    Dio_LevelType dioPinLevel[2U];

    /* Configure SOC_PADCONFIG_143 set it to '1' to take
       MCAN transceiver out of STB mode */
    I2C_Params      i2cParams;
    I2C_Handle      handle = NULL;
    uint8           dataToSlave[4];

    /* Pin mux for CAN STB used in GESI board */
    regVal = 0x20007U;
    LLD_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + 0x1C0F4U, regVal);
    /* Take MCAN transceiver out of STB mode for MCAN0 */
    /* Set Pin direction to output */
    regVal = LLD_REG32_RD(CSL_GPIO0_BASE + 0x38U);
    regVal &= (~(1U << 0x1BU));
    LLD_REG32_WR(CSL_GPIO0_BASE + 0x38U, regVal);
    /* Drive Pin to Low */
    Dio_WriteChannel(CAN_TRCV_MAIN_DOMAIN_4_9_11, STD_LOW);
    /* Read Pin level */
    dioPinLevel[0] = Dio_ReadChannel(CAN_TRCV_MAIN_DOMAIN_4_9_11);
    /*Read back the pin levels to ensure transceiver is enabled*/
    if (STD_LOW != dioPinLevel[0U])
    {
        AppUtils_Printf(MSG_NORMAL,
        "CAN_APP: Error in Enabling CAN Transceiver Main Domain Inst 4,9,11!!!\n");
    }
    else
    {
        AppUtils_Printf(MSG_NORMAL,
        "CAN_APP: Successfully Enabled CAN Transceiver Main Domain Inst 4,9,11!!!\n");
    }
    /* Take MCAN transceiver out of STB mode for MCU MCAN0 */
    /* Below code will be replaced by Port module in further releases */
    /* Pin mux configuration of Dio Pin 355 */
    regVal = 0x08050007U;
    LLD_REG32_WR(CSL_WKUP_CTRL_MMR0_CFG0_BASE + CSL_WKUP_CTRL_MMR_CFG0_PADCONFIG44, regVal);
    /* Set Pin direction to output */
    regVal = LLD_REG32_RD(CSL_WKUP_GPIO0_BASE + 0x10U);
    regVal &= (~(1U << 0x0U));
    LLD_REG32_WR(CSL_WKUP_GPIO0_BASE + 0x10U, regVal);
    /* Drive Pin to High */
    Dio_WriteChannel(CAN_TRCV_MCU_MCAN_0, STD_HIGH);
     /* Read Pin level */
    dioPinLevel[1U] = Dio_ReadChannel(CAN_TRCV_MCU_MCAN_0);
    /*Read back the pin levels to ensure transceiver is enabled*/
    if (STD_HIGH != dioPinLevel[1U])
    {
        AppUtils_Printf(MSG_NORMAL,
        "CAN_APP: Error in Enabling CAN Transceiver MCU MCAN0!!!\n");
    }
    else
    {
        regVal = 0x08050007U;
        LLD_REG32_WR(CSL_WKUP_CTRL_MMR0_CFG0_BASE +
            CSL_WKUP_CTRL_MMR_CFG0_PADCONFIG38, regVal);
        /* Set Pin direction to output */
        regVal = LLD_REG32_RD(CSL_WKUP_GPIO0_BASE + 0x38U);
        regVal &= (~(1U << 0x16U));
        LLD_REG32_WR(CSL_WKUP_GPIO0_BASE + 0x38U, regVal);
        /* Drive Pin to High */
        Dio_WriteChannel(54U, STD_HIGH);
         /* Read Pin level */
        dioPinLevel[1U] = Dio_ReadChannel(54U);
        /*Read back the pin levels to ensure transceiver is enabled*/
        if (STD_HIGH != dioPinLevel[1U])
        {
            AppUtils_Printf(MSG_NORMAL,
            "CAN_APP: Error in Enabling CAN Transceiver MCU MCAN0!!!\n");
        }
        else
        {
            AppUtils_Printf(MSG_NORMAL,
            "CAN_APP: Successfully Enabled CAN Transceiver MCU MCAN0!!!\n");
        }
    }
    /* Take MCAN transceiver out of STB mode for MCU MCAN1 */
    /* Below code will be replaced by Port module in further releases */
    /* Pin mux configuration of Dio Pin 2 */
    regVal = 0x08050007U;
    LLD_REG32_WR(CSL_WKUP_CTRL_MMR0_CFG0_BASE + CSL_WKUP_CTRL_MMR_CFG0_PADCONFIG46, regVal);
    /* Set Pin direction to output */
    regVal = LLD_REG32_RD(CSL_WKUP_GPIO0_BASE + 0x10U);
    regVal &= (~(1U << 0x2U));
    LLD_REG32_WR(CSL_WKUP_GPIO0_BASE + 0x10U, regVal);
    /* Drive Pin to Low */
    Dio_WriteChannel(CAN_TRCV_MCU_MCAN_1, STD_LOW);
     /* Read Pin level */
    dioPinLevel[1U] = Dio_ReadChannel(CAN_TRCV_MCU_MCAN_1);
    /*Read back the pin levels to ensure transceiver is enabled*/
    if (STD_LOW != dioPinLevel[1U])
    {
        AppUtils_Printf(MSG_NORMAL,
        "CAN_APP: Error in Enabling CAN Transceiver MCU MCAN1!!!\n");
    }
    else
    {
        AppUtils_Printf(MSG_NORMAL,
        "CAN_APP: Successfully Enabled CAN Transceiver MCU MCAN1!!!\n");
    }
    /*
     * Configuring TCA6424 IO Exp 2 with addr 0x22
     * This io expander is controlled by i2c0
     * For Main MCAN2 P13 and P14 should be set to 0, This should route the MCAN2 STB line to transciver.
     * For Main MCAN0 P06 and P07 should be set to 1.
     */
     /* I2c PinMux */
    /* I2C0_SDL */
    regVal = 0x40000U;
    LLD_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + 0x1C220U, regVal);
    /* I2C0_SDA */
    regVal = 0x40000U;
    LLD_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + 0x1C224U, regVal);
    /* I2C initialization */
    I2C_init();
    I2C_Params_init(&i2cParams);
    i2cParams.transferMode = I2C_MODE_BLOCKING;
    i2cParams.bitRate = I2C_400kHz;
    i2cParams.transferCallbackFxn = NULL;

    handle = I2C_open(0U, &i2cParams);

    dataToSlave[0] = TCA6424_REG_CONFIG0 | TCA6424_CMD_AUTO_INC;
    dataToSlave[1] = 0x0U;
    SetupI2CTransfer(handle, 0x22, &dataToSlave[0], 2, NULL, 0);

    dataToSlave[0] = TCA6424_REG_INPUT0 | TCA6424_CMD_AUTO_INC;
    dataToSlave[1] = 0x0U;
    dataToSlave[2] = 0x0U;
    dataToSlave[3] = 0x0U;
    SetupI2CTransfer(handle, 0x22, &dataToSlave[0], 1, &dataToSlave[1], 3);

    /* Set P06 and P07 to 1.
     * Set P13 and P14 to 0.
     */
    dataToSlave[0] = TCA6424_REG_OUTPUT0 | TCA6424_CMD_AUTO_INC;
    dataToSlave[1] |= 0xC0;
    dataToSlave[2] &= ~(0x18);
    SetupI2CTransfer(handle, 0x22, &dataToSlave[0], 1, &dataToSlave[1], 3);

    return;
}

void CanApp_InterruptConfig(void)
{
    uint32 idx;
    OsalRegisterIntrParams_t    intrPrms;
    OsalInterruptRetCode_e      osalRetVal;
    HwiP_Handle hwiHandle;
    const Can_ConfigType *Can_ConfigPtr;

#if (STD_ON == CAN_VARIANT_PRE_COMPILE)
    Can_ConfigPtr = &CAN_INIT_CONFIG_PC;
#else
    Can_ConfigPtr = &CanConfigSet;
#endif

    for (idx = 0U; idx < Can_ConfigPtr->CanMaxControllerCount; idx++)
    {
        /* If the CAN instance is not in MCU domain, the interrupt router will
            have to be configured */
        /* Set the destination interrupt */
        Osal_RegisterInterrupt_initParams(&intrPrms);
        intrPrms.corepacConfig.arg          = (uintptr_t)CanApp_Isr[idx];
        intrPrms.corepacConfig.isrRoutine   = &CanApp_CanXIsr;
        intrPrms.corepacConfig.priority     = 1U;
        intrPrms.corepacConfig.corepacEventNum = 0U; /* NOT USED ? */
        intrPrms.corepacConfig.intVecNum        = CanApp_IntNumbers[idx];

        osalRetVal = Osal_RegisterInterrupt(&intrPrms, &hwiHandle);
        if(OSAL_INT_SUCCESS != osalRetVal)
        {
            AppUtils_Printf(MSG_NORMAL,
                            "CAN_APP: Error %d !!!\n");
            break;
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

void SetupI2CTransfer(I2C_Handle handle,  uint32 slaveAddr,
                      uint8 *writeData, uint32 numWriteBytes,
                      uint8 *readData,  uint32 numReadBytes)
{
    bool status;
    I2C_Transaction i2cTransaction;

    I2C_transactionInit(&i2cTransaction);
    i2cTransaction.slaveAddress = slaveAddr;
    i2cTransaction.writeBuf = (uint8 *)&writeData[0];
    i2cTransaction.writeCount = numWriteBytes;
    i2cTransaction.readBuf = (uint8 *)&readData[0];
    i2cTransaction.readCount = numReadBytes;
    status = I2C_transfer(handle, &i2cTransaction);
    if(FALSE == status)
    {
        AppUtils_Printf(MSG_NORMAL, "\n Data Transfer failed. \n");
    }
}

void CanApp_SetupCanFrame(uint32 ctlr_cnt, uint8 *hth, uint32 *mask)
{
	if (CAN_CONTROLLER_INSTANCE_MCAN0 ==
		CanConfigSet_CanController_List_PC[ctlr_cnt]->CanControllerInst)
	{
		/* CAN Protocol Data Unit */
		PduInfo = &CanApp_Pdu2;
		/* Hardware Transmit Handle assigned to MCAN0 */
		*hth     = CAN_HTRH_0;
		/* mask is need for checking id of received message with
		 * sent message as 30th bit the of sent message will be set
		 * for CAN FD frame */
		*mask    = 0x40000000U;
	}

	if (CAN_CONTROLLER_INSTANCE_MCAN4 ==
		CanConfigSet_CanController_List_PC[ctlr_cnt]->CanControllerInst)
	{
		/* CAN Protocol Data Unit */
		PduInfo = &CanApp_Pdu3;
		/* Hardware Transmit Handle assigned to MCAN1 */
		*hth     = CAN_HTRH_2;
		/* mask is need for checking id of received message with
		 * sent message as 30th bit the of sent message will be set
		 * for CAN FD frame */
		*mask = 0xC0000000U;
	}

	if (CAN_CONTROLLER_INSTANCE_MCAN9 ==
		CanConfigSet_CanController_List_PC[ctlr_cnt]->CanControllerInst)
	{
		/* CAN Protocol Data Unit */
		PduInfo = &CanApp_Pdu4;
		/* Hardware Transmit Handle assigned to MCAN0 */
		*hth     = CAN_HTRH_4;
		/* mask is need for checking id of received message with
		 * sent message as 30th bit the of sent message will be set
		 * for CAN FD frame */
		*mask    = 0x40000000U;
	}

	if (CAN_CONTROLLER_INSTANCE_MCAN11 ==
		CanConfigSet_CanController_List_PC[ctlr_cnt]->CanControllerInst)
	{
		/* CAN Protocol Data Unit */
		PduInfo = &CanApp_Pdu5;
		/* Hardware Transmit Handle assigned to MCAN1 */
		*hth     = CAN_HTRH_6;
		/* mask is need for checking id of received message with
		 * sent message as 30th bit the of sent message will be set
		 * for CAN FD frame */
		*mask = 0xC0000000U;
	}
}
