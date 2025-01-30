/*
*
* Copyright (c) 2024-2020 Texas Instruments Incorporated
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
#include "can_profile.h"

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
extern Can_PduType          CanApp_Pdu, CanApp_Pdu1;
/**< Variable used for copying protocol data unit */
extern uint8                CanApp_InputData[64U];
/**< Variables which contains Can PDU data used in Can transmission */
extern const Can_PduType    *CanApp_PduInfo;
/**< Variable which contains Can PDU data passed during Transmission */
/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */
/** \brief Start up sequence : Program the interrupt muxes / priorities */
void CanApp_Startup(void)
{
    /* Build Interrupt list for the enabled Can Instances */
    CanApp_BuildIntList();

    /* Interrupt configuration for the enabled Can Instances */
    CanApp_InterruptConfig();

    /* Initialize counters, that would be required for profiling operations */
    AppUtils_ProfileInit(0);

    /* Initialize memory sections, which is used to check memory corruption */
	AppUtils_CanSectionInit();

}

void CanApp_BuildIntList(void)
{
    uint32 idx, flag, intNum;
    const Can_ConfigType *Can_ConfigPtr;
    CanApp_IsrType pIsrHandler = NULL;
    uint32 canMaxControllerCnt = 0U;

    /*
     * 1. Determine the number of Can Instances used
     * 2. Build valid Can Instances list
     * 3. Build isr and interrupt number for enabled Can Instances only
     */
    /* Do Can Init */
#if (STD_ON == CAN_VARIANT_PRE_COMPILE)
    Can_ConfigPtr = &CAN_INIT_CONFIG_PC;
    Can_Init((const Can_ConfigType *) NULL_PTR);
#if (CAN_INITIAL_PRINT_DISABLE_BEFORE_CAN_RESPONSE == STD_OFF)
    AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
                    "Variant - Pre Compile being used !!!\n");
#endif
#else
    Can_ConfigPtr = &CanConfigSet;
    Can_Init(Can_ConfigPtr);
#if (CAN_INITIAL_PRINT_DISABLE_BEFORE_CAN_RESPONSE == STD_OFF)
    AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
                    "Variant - Post Build being used !!!\n");
#endif
#endif

    /* Enable SCI Client */
    Sciclient_init(NULL_PTR);
    canMaxControllerCnt = Can_ConfigPtr->CanMaxControllerCount;
    for (idx = 0U; idx < canMaxControllerCnt; idx++)
    {
        CanApp_IntNumbers[idx] = 0U;
    }

    for (idx = 0U; idx < canMaxControllerCnt; idx++)
    {
        flag = 0U;
        if (CAN_CONTROLLER_INSTANCE_MCU_MCAN0 ==
            CanConfigSet_CanController_List_PC[idx]->CanControllerInst)
        {
            intNum = APP_MCU_MCAN_0_INT0;
            pIsrHandler = Can_0_Int0ISR;
            flag = 1U;
        }
        if (CAN_CONTROLLER_INSTANCE_MCU_MCAN1 ==
            CanConfigSet_CanController_List_PC[idx]->CanControllerInst)
        {
            intNum = APP_MCU_MCAN_1_INT0;
            pIsrHandler = Can_1_Int0ISR;
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
    CSL_REG32_WR(CSL_WKUP_CTRL_MMR0_CFG0_BASE + 0x1D008, 0x68EF3490);
    /* write Partition 7 Lock Key 1 Register */
    CSL_REG32_WR(CSL_WKUP_CTRL_MMR0_CFG0_BASE + 0x1D00C, 0xD172BC5A);
    /* Check for unlock */
    regVal = CSL_REG32_RD(CSL_WKUP_CTRL_MMR0_CFG0_BASE + 0x1D008);
    while ((regVal & 0x1) != 0x1U)
    {
        regVal = CSL_REG32_RD(CSL_WKUP_CTRL_MMR0_CFG0_BASE + 0x1D008);
    }

    /* Unlock lock key registers for Partition 7: IO PAD
       configuration registers in MAIN_CTRL_MMR */
    /* write Partition 7 Lock Key 0 Register */
    CSL_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + 0x1D008, 0x68EF3490);
    /* write Partition 7 Lock Key 1 Register */
    CSL_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + 0x1D00C, 0xD172BC5A);
    /* Check for unlock */
    regVal = CSL_REG32_RD(CSL_CTRL_MMR0_CFG0_BASE + 0x1D008);
    while ((regVal & 0x1) != 0x1U)
    {
        regVal = CSL_REG32_RD(CSL_CTRL_MMR0_CFG0_BASE + 0x1D008);
    }

    /* Unlocking done */
    /* Below code will be replaced by Port module in further releases */
    /* MCU MCAN 0 Tx PAD configuration */
    regVal = 0x60000U;
    CSL_REG32_WR(CSL_WKUP_CTRL_MMR0_CFG0_BASE + 0x1C0B8U, regVal);
    /* MCU MCAN 0 Rx PAD configuration */
    regVal = 0x60000U;
    CSL_REG32_WR(CSL_WKUP_CTRL_MMR0_CFG0_BASE + 0x1C0BCU, regVal);
    /* MCU MCAN 1 Tx PAD configuration */
    regVal = 0x60000U;
    CSL_REG32_WR(CSL_WKUP_CTRL_MMR0_CFG0_BASE + 0x1C0D0U, regVal);
    /* MCU MCAN 1 Rx PAD configuration */
    regVal = 0x60000U;
    CSL_REG32_WR(CSL_WKUP_CTRL_MMR0_CFG0_BASE + 0x1C0D4U, regVal);
    /* MAIN MCAN 0 Tx PAD configuration */
    regVal = 0x60000U;
    CSL_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + 0x1C020U, regVal);
    /* MAIN MCAN 0 Rx PAD configuration */
    regVal = 0x60000U;
    CSL_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + 0x1C024U, regVal);
    /* MAIN MCAN 4 Tx PAD configuration */
    regVal = 0x60006U;
    CSL_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + 0x1C040U, regVal);
    /* MAIN MCAN 4 Rx PAD configuration */
    regVal = 0x60006U;
    CSL_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + 0x1C044U, regVal);
    /* MAIN MCAN 8 Tx PAD configuration */
    regVal = 0x60006U;
    CSL_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + 0x1C060U, regVal);
    /* MAIN MCAN 8 Rx PAD configuration */
    regVal = 0x60006U;
    CSL_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + 0x1C064U, regVal);
    /* MAIN MCAN 10 Tx PAD configuration */
    regVal = 0x60006U;
    CSL_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + 0x1C070U, regVal);
    /* MAIN MCAN 10 Rx PAD configuration */
    regVal = 0x60006U;
    CSL_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + 0x1C074U, regVal);

    /* Take MCAN transceivers out of STB mode i.e NORMAL Mode */
    CanApp_EnableTransceivers();
}

void CanApp_EnableTransceivers(void)
{
    uint32 regVal = 0U;
    Dio_LevelType dioPinLevel[2U];

    /* Take MCAN transceiver out of STB mode for MCU MCAN0 */
    /* Below code will be replaced by Port module in further releases */
    /* Pin mux configuration of Dio Pin 355 */
    regVal = 0x08050007U;
    CSL_REG32_WR(CSL_WKUP_CTRL_MMR0_CFG0_BASE + CSL_WKUP_CTRL_MMR_CFG0_PADCONFIG48, regVal);
    /* Set Pin direction to output */
    regVal = CSL_REG32_RD(CSL_WKUP_GPIO0_BASE + 0x10U);
    regVal &= (~(1U << 0x0U));
    CSL_REG32_WR(CSL_WKUP_GPIO0_BASE + 0x10U, regVal);
    /* Drive Pin to High */
    Dio_WriteChannel(CAN_TRCV_MCU_MCAN_0, STD_HIGH);
     /* Read Pin level */
    dioPinLevel[1U] = Dio_ReadChannel(CAN_TRCV_MCU_MCAN_0);
    /*Read back the pin levels to ensure transceiver is enabled*/
    if (STD_HIGH != dioPinLevel[1U])
    {
        AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
        " Error in Enabling CAN Transceiver MCU MCAN0!!!\n");
    }
    else
    {
        regVal = 0x08050007U;
        CSL_REG32_WR(CSL_WKUP_CTRL_MMR0_CFG0_BASE +
            CSL_WKUP_CTRL_MMR_CFG0_PADCONFIG42, regVal);
        /* Set Pin direction to output */
        regVal = CSL_REG32_RD(CSL_WKUP_GPIO0_BASE + 0x38U);
        regVal &= (~(1U << 0x1AU));
        CSL_REG32_WR(CSL_WKUP_GPIO0_BASE + 0x38U, regVal);
        /* Drive Pin to High */
        Dio_WriteChannel(58U, STD_HIGH);
         /* Read Pin level */
        dioPinLevel[1U] = Dio_ReadChannel(58U);
        /*Read back the pin levels to ensure transceiver is enabled*/
        if (STD_HIGH != dioPinLevel[1U])
        {
            AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
            " Error in Enabling CAN Transceiver MCU MCAN0!!!\n");
        }
        else
        {
            AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
            " Successfully Enabled CAN Transceiver MCU MCAN0!!!\n");
        }
    }
    /* Take MCAN transceiver out of STB mode for MCU MCAN1 */
    /* Below code will be replaced by Port module in further releases */
    /* Pin mux configuration of Dio Pin 2 */
    regVal = 0x08050007U;
    CSL_REG32_WR(CSL_WKUP_CTRL_MMR0_CFG0_BASE + CSL_WKUP_CTRL_MMR_CFG0_PADCONFIG50, regVal);
    /* Set Pin direction to output */
    regVal = CSL_REG32_RD(CSL_WKUP_GPIO0_BASE + 0x10U);
    regVal &= (~(1U << 0x2U));
    CSL_REG32_WR(CSL_WKUP_GPIO0_BASE + 0x10U, regVal);
    /* Drive Pin to Low */
    Dio_WriteChannel(CAN_TRCV_MCU_MCAN_1, STD_LOW);
     /* Read Pin level */
    dioPinLevel[1U] = Dio_ReadChannel(CAN_TRCV_MCU_MCAN_1);
    /*Read back the pin levels to ensure transceiver is enabled*/
    if (STD_LOW != dioPinLevel[1U])
    {
        AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
        " Error in Enabling CAN Transceiver MCU MCAN1!!!\n");
    }
    else
    {
        AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
        " Successfully Enabled CAN Transceiver MCU MCAN1!!!\n");
    }

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
        Osal_RegisterInterrupt_initParams(&intrPrms);
        intrPrms.corepacConfig.arg          = (uintptr_t)CanApp_Isr[idx];
        intrPrms.corepacConfig.isrRoutine   = &CanApp_CanXIsr;
        intrPrms.corepacConfig.priority     = 1U;
        intrPrms.corepacConfig.corepacEventNum = 0U; /* NOT USED ? */
        intrPrms.corepacConfig.intVecNum        = CanApp_IntNumbers[idx];

        osalRetVal = Osal_RegisterInterrupt(&intrPrms, &hwiHandle);
        if(OSAL_INT_SUCCESS != osalRetVal)
        {
            AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
                            " Error!!! Could not register the ISR for instance"
                            "%d!!!\n", idx);
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

void CanApp_SetupCanFrame(uint32 ctlr_cnt, uint8 *hth, uint32 *mask)
{
#if (CAN_TX_ONLY_MODE == STD_ON)
        if (CAN_CONTROLLER_INSTANCE_MCU_MCAN0 ==
            CanConfigSet_CanController_List_PC[ctlr_cnt]->CanControllerInst)
        {
            /* CAN Protocol Data Unit */
            CanApp_PduInfo = &CanApp_Pdu;
            /* Hardware Transmit Handle assigned to MCAN0 */
            *hth     = CAN_HTRH_0;
            /* mask is need for checking id of received message with
             * sent message as 30th bit the of sent message will be set
             * for CAN FD frame */
#if (CAN_LOOPBACK_ENABLE == STD_ON) || (CAN_EXT_LOOPBACK == STD_ON)
            *mask    = 0xC0000000U;
#endif
        }

        if (CAN_CONTROLLER_INSTANCE_MCU_MCAN1 ==
            CanConfigSet_CanController_List_PC[ctlr_cnt]->CanControllerInst)
        {
            /* CAN Protocol Data Unit */
            CanApp_PduInfo = &CanApp_Pdu1;
            /* Hardware Transmit Handle assigned to MCAN1 */
            *hth     = CAN_HTRH_2;
            /* mask is need for checking id of received message with
             * sent message as 30th bit the of sent message will be set
             * for CAN FD frame */
#if (CAN_LOOPBACK_ENABLE == STD_ON) || (CAN_EXT_LOOPBACK == STD_ON)
            *mask = 0xC0000000U;
#endif
        }
#endif
}

#if (CAN_EXT_LOOPBACK == STD_ON)
Std_ReturnType CanApp_SetupControllersExternalLoopback(uint32 CanControllerTransition)
{ 
    Std_ReturnType      statusTxCan, statusRxCan;
    Std_ReturnType      status = E_OK;

    if (CanControllerTransition == CAN_CS_STARTED)
    {
        statusTxCan = Can_SetControllerMode(
        CanConfigSet_CanController_List_PC[APP_MCU_MCAN0_IDX]->ControllerId, CAN_CS_STARTED);
        statusRxCan = Can_SetControllerMode(
        CanConfigSet_CanController_List_PC[APP_MCU_MCAN1_IDX]->ControllerId, CAN_CS_STARTED);
        if ((statusTxCan != E_OK) && (statusRxCan != E_OK))
        {
            status = E_NOT_OK;
        }
    }

    if (CanControllerTransition == CAN_CS_STOPPED)
    {
        statusTxCan = Can_SetControllerMode(
        CanConfigSet_CanController_List_PC[APP_MCU_MCAN0_IDX]->ControllerId, CAN_CS_STOPPED);
        statusRxCan = Can_SetControllerMode(
        CanConfigSet_CanController_List_PC[APP_MCU_MCAN1_IDX]->ControllerId, CAN_CS_STOPPED);
        if ((statusTxCan != E_OK) && (statusRxCan != E_OK))
        {
            status = E_NOT_OK;
        }
    }

    return status;
}

void CanApp_SetupCanFrameExternalLoopback(uint8 *hth, uint32 *mask)
{
    /* Setup Can frame, reception happens on MCAN9 */
    /* CAN Protocol Data Unit */
    CanApp_PduInfo = &CanApp_Pdu1;
    /* Hardware Transmit Handle assigned to MCAN4 */
    *hth     = CAN_HTRH_0;
    /* mask is need for checking id of received message with
     * sent message as 30th bit the of sent message will be set
     * for CAN FD frame */
    *mask    = 0xC0000000U;
}
#endif
