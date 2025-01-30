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
 *  \file     CanApp.c
 *
 *  \brief    This file contains the Can test example
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "CanApp_Startup.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
/* None */

/* ========================================================================== */
/*              Internal Function Declarations                                */
/* ========================================================================== */
static void CanApp_LoopbackTest(void);
#if (STD_ON == CAN_SAFETY_DIAGNOSTIC)
static void CanApp_SafetyApiTest(void);
#endif

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
uint32                      CanApp_TestPassed = E_OK;
/**< Variable used for Demo status */
Can_PduType                 CanApp_Pdu, CanApp_Pdu1;
Can_PduType                 CanApp_Pdu2, CanApp_Pdu3, CanApp_Pdu4, CanApp_Pdu5;

/**< Variables which contains Can PDU data used in Can transmission */
uint8                       CanApp_InputData[64U] =
{0xA1, 0x1A, 0xFF, 0xFF, 0xC1, 0x1C, 0xB1, 0x1B,
0xA2, 0x2A, 0xFF, 0xFF, 0xC2, 0x2C, 0xB2, 0x2B,
0xA3, 0x3A, 0xFF, 0xFF, 0xC3, 0x3C, 0xB3, 0x3B,
0xA4, 0x4A, 0xFF, 0xFF, 0xC4, 0x4C, 0xB4, 0x4B,
0xA5, 0x5A, 0xFF, 0xFF, 0xC5, 0x5C, 0xB5, 0x5B,
0xA6, 0x6A, 0xFF, 0xFF, 0xC6, 0x6C, 0xB6, 0x6B,
0xA7, 0x7A, 0xFF, 0xFF, 0xC7, 0x7C, 0xB7, 0x7B,
0xA8, 0x8A, 0xFF, 0xFF, 0xC8, 0x8C, 0xB8, 0x8B};
/**< Variable which contains Can SDU data that is transported inside the PDU */
const Can_PduType    *PduInfo;
/**< Variable which contains Can PDU data passed during Transmission */
/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
/* Global variables used in callback functions */
extern volatile P2VAR(Can_HwType, CANIF_VAR_CLEARED, CANIF_APPL_DATA)
        CanIf_Mailbox;
extern volatile VAR(uint8, CANIF_VAR_CLEARED) CanIf_RxConfirmationCount;
extern volatile VAR(uint8, CANIF_VAR_CLEARED) CanIf_TxConfirmationCount;
extern volatile P2CONST(PduInfoType, CANIF_VAR_CLEARED, CANIF_APPL_DATA)
															CanIf_PduInfoPtr;
/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */
sint32 main(void)
{
    CanApp_TestPassed = E_OK;

#if defined (SOC_AM62X) || defined (SOC_AM62AX) || defined (SOC_AM62PX) || defined (SOC_J722S)
	System_init();
#endif

#ifdef UART_ENABLED
    AppUtils_Init();
#endif

    AppUtils_Printf(MSG_NORMAL,
    "CAN_APP: Sample Application - STARTS !!!\n");

    CanApp_Startup();
    CanApp_PowerAndClkSrc();
    CanApp_PlatformInit();

    CanApp_LoopbackTest();
#if (STD_ON == CAN_SAFETY_DIAGNOSTIC)
    CanApp_SafetyApiTest();
#endif

    AppUtils_Printf(MSG_NORMAL,
        "CAN Stack Usage: %d bytes\n", AppUtils_GetStackUsage());
    if (AppUtils_CanCheckStackAndSectionCorruption() != E_OK)
    {
        CanApp_TestPassed = E_NOT_OK;
        AppUtils_Printf(MSG_NORMAL,  "CAN Stack/section corruption!!!\n");
    }

    if (E_OK == CanApp_TestPassed)
    {
        AppUtils_Printf(MSG_NORMAL,
        "CAN_APP: All tests have passed!!!\n");
        AppUtils_LogResult(APP_UTILS_STATUS_PASS);
    }
    else
    {
        AppUtils_Printf(MSG_NORMAL,
        "CAN_APP: Some tests have Failed!!!\n");
        AppUtils_LogResult(APP_UTILS_STATUS_FAIL);
    }
    return (CanApp_TestPassed);
}

static void CanApp_LoopbackTest(void)
{
    uint32               ctlr_cnt, mask = 0x00000000U, canMaxControllerCnt;
    uint8                loopCnt = 0U, hth = 0U;
    Std_ReturnType       status;
    const Can_ConfigType *Can_ConfigPtr;

#if (STD_ON == CAN_VARIANT_PRE_COMPILE)
    Can_ConfigPtr = &CAN_INIT_CONFIG_PC;
#else
    Can_ConfigPtr = &CanConfigSet;
#endif

    canMaxControllerCnt = Can_ConfigPtr->CanMaxControllerCount;
    for (ctlr_cnt = 0U; ctlr_cnt < canMaxControllerCnt;
        ctlr_cnt++)
    {
        /* Set Can frame */
        CanApp_SetupCanFrame(ctlr_cnt, &hth, &mask);

        /* Set Controller Mode to Start */
        status = Can_SetControllerMode(
        CanConfigSet_CanController_List_PC[ctlr_cnt]->ControllerId, CAN_CS_STARTED);
        if (status != E_OK)
        {
            CanApp_TestPassed = E_NOT_OK;
        }

#if (CAN_LOOPBACK_ENABLE == STD_ON)
        /* Enable loopback mode in CAN */
        Can_TestLoopBackModeEnable(
        CanConfigSet_CanController_List_PC[ctlr_cnt]->ControllerId, 0U);
#endif

        /* Do Can Write to send the CanApp_InputData on CAN bus */
        status = Can_Write(hth, PduInfo);

#if defined (SOC_AM62X) || defined (SOC_AM62AX) || defined (SOC_AM62PX) || defined (SOC_J722S)
        Can_MainFunction_Write();
        for(int cnt=0; cnt<100000;cnt++)
          {
                       ;
          }
#endif

        if (status != E_OK)
        {
            AppUtils_Printf(MSG_NORMAL,
            "CAN_APP: Error In Can_Write.\n");
            CanApp_TestPassed = E_NOT_OK;
        }

        while ((CanIf_TxConfirmationCount != 1U))
        {
            /* Wait for Tx Confirmation */
#if defined (SOC_AM62X) || defined (SOC_AM62AX) || defined (SOC_AM62PX) || defined (SOC_J722S)
            Can_MainFunction_Write();
            for(int cnt=0; cnt<10000;cnt++)
            {
                ;
            }
#endif

        }

#if defined (SOC_AM62X) || defined (SOC_AM62AX) || defined (SOC_AM62PX) || defined (SOC_J722S)
        Can_MainFunction_Read();
#endif

        if (E_OK == CanApp_TestPassed)
        {
            if ((1U == CanIf_TxConfirmationCount) &&
                        (1U == CanIf_RxConfirmationCount))
            {
                CanIf_TxConfirmationCount = 0U;
                CanIf_RxConfirmationCount = 0U;
            }
            else
            {
                CanApp_TestPassed = E_NOT_OK;
                break;
            }

            /* Set Controller Mode to Stop*/
            status = Can_SetControllerMode(
                CanConfigSet_CanController_List_PC[ctlr_cnt]->ControllerId,
                CAN_CS_STOPPED);
            if (status != E_OK)
            {
                CanApp_TestPassed = E_NOT_OK;
            }

#if (CAN_LOOPBACK_ENABLE == STD_ON)
            Can_TestLoopBackModeDisable(
            CanConfigSet_CanController_List_PC[ctlr_cnt]->ControllerId, 0U);
#endif

            if ((PduInfo->length == CanIf_PduInfoPtr->SduLength) &&
                (PduInfo->id == (CanIf_Mailbox->CanId | mask)))
            {
#if defined (SOC_AM62X) || defined (SOC_AM62AX) || defined (SOC_AM62PX) || defined (SOC_J722S)
                AppUtils_Printf(MSG_NORMAL,
                "CAN_APP: Message Id Received %x Message Length is %u\t \n",
                CanIf_Mailbox->CanId, CanIf_PduInfoPtr->SduLength);
                for (loopCnt = 0U; loopCnt < CanIf_PduInfoPtr->SduLength;
                        loopCnt++)
                {
                    if (CanIf_PduInfoPtr->SduDataPtr[loopCnt] !=
                        CanApp_InputData[loopCnt])
                    {
                        CanApp_TestPassed = E_NOT_OK;
                        AppUtils_Printf(MSG_NORMAL,
                        "CAN_APP: Transmitted and Received Data miss-match.\n");
                        break;
                    }
                    else
                    {
                        CanApp_TestPassed = E_OK;
                    }
                }
                AppUtils_Printf(MSG_NORMAL,
                "CAN_APP: Can Controller Instance MCAN %u \
Internal LoopBack Mode Test Passed\n", ctlr_cnt);
            }
            else
            {
                CanApp_TestPassed = E_NOT_OK;
                break;
            }

#if (CAN_LOOPBACK_ENABLE == STD_ON)
            Can_TestLoopBackModeDisable(
            CanConfigSet_CanController_List_PC[ctlr_cnt]->ControllerId, 0U);
#endif
            if ((PduInfo->length == CanIf_PduInfoPtr->SduLength) &&
                (PduInfo->id == (CanIf_Mailbox->CanId | mask)))
            {
#endif /* if defined SOC_AM62X) */


                AppUtils_Printf(MSG_NORMAL,
                "CAN_APP: Message Id Received %x Message Length is %u\t \n",
                CanIf_Mailbox->CanId, CanIf_PduInfoPtr->SduLength);
                for (loopCnt = 0U; loopCnt < CanIf_PduInfoPtr->SduLength;
                        loopCnt++)
                {
                    if (CanIf_PduInfoPtr->SduDataPtr[loopCnt] !=
                        CanApp_InputData[loopCnt])
                    {
                        CanApp_TestPassed = E_NOT_OK;
                        AppUtils_Printf(MSG_NORMAL,
                        "CAN_APP: Transmitted and Received Data miss-match.\n");
                        break;
                    }
                    else
                    {
                        CanApp_TestPassed = E_OK;
                    }
                }
                AppUtils_Printf(MSG_NORMAL,
                  "CAN_APP: Can Controller Instance MCAN %u \
                  Internal LoopBack Mode Test Passed\n", ctlr_cnt);
            }
            else
            {
                CanApp_TestPassed = E_NOT_OK;
                break;
            }
        }
    }
}

#if (STD_ON == CAN_SAFETY_DIAGNOSTIC)
static void CanApp_SafetyApiTest(void)
{
    uint32               ctlr_cnt, canMaxControllerCnt;
    Std_ReturnType       status;
    Can_IrqStatusType    canIntrstatus;
    const Can_ConfigType *Can_ConfigPtr;
#if (STD_ON == CAN_REGISTER_READBACK_API)
    Can_RegisterReadbackType Can_RegRb;
#endif

#if (STD_ON == CAN_VARIANT_PRE_COMPILE)
    Can_ConfigPtr = &CAN_INIT_CONFIG_PC;
#else
    Can_ConfigPtr = &CanConfigSet;
#endif

    canMaxControllerCnt = Can_ConfigPtr->CanMaxControllerCount;
    for (ctlr_cnt = 0U; ctlr_cnt < canMaxControllerCnt;
        ctlr_cnt++)
    {
        AppUtils_Printf(MSG_NORMAL,
            "\nCAN_APP: Safety Diagnostic API functionality test for controller MCAN %d \n",
            CanConfigSet_CanController_List_PC[ctlr_cnt]->ControllerId);
        AppUtils_Printf(MSG_NORMAL,"CAN_APP: Can_EnableIntr() API test\n");
        status = Can_EnableIntr(
            CanConfigSet_CanController_List_PC[ctlr_cnt]->ControllerId,
            MCAN_ERR_TYPE_TOO_ERROR);
        if (status != E_OK)
        {
            AppUtils_Printf(MSG_NORMAL,
            "CAN_APP: Failed to Enable TOO interrupt \n");
            CanApp_TestPassed = E_NOT_OK;
        }
        else
        {
            AppUtils_Printf(MSG_NORMAL,
            "CAN_APP: Successfully Enabled TOO interrupt \n");
            CanApp_TestPassed = E_OK;
        }
        status = Can_EnableIntr(
            CanConfigSet_CanController_List_PC[ctlr_cnt]->ControllerId,
            MCAN_ERR_TYPE_PEA_ERROR);
        if (status != E_OK)
        {
            AppUtils_Printf(MSG_NORMAL,
            "CAN_APP: Failed to Enable PEA interrupt \n");
            CanApp_TestPassed = E_NOT_OK;
        }
        else
        {
            AppUtils_Printf(MSG_NORMAL,
            "CAN_APP: Successfully Enabled PEA interrupt \n");
            CanApp_TestPassed = E_OK;
        }

        AppUtils_Printf(MSG_NORMAL,"CAN_APP: Can_GetIntrStatus() API test\n");
        canIntrstatus = Can_GetIntrStatus(
                    CanConfigSet_CanController_List_PC[ctlr_cnt]->ControllerId);
        switch(canIntrstatus)
        {
            case CAN_IRQ_NO_INTR_EVENT:
                AppUtils_Printf(MSG_NORMAL,
                    "CAN_APP: No error event occured\n");
                CanApp_TestPassed = E_OK;
                break;
            case CAN_IRQ_INTR_STUFF_EVENT:
                AppUtils_Printf(MSG_NORMAL,
                    "CAN_APP: Stuff error event occured\n");
                CanApp_TestPassed = E_OK;
                break;
            case CAN_IRQ_INTR_FORM_EVENT:
                AppUtils_Printf(MSG_NORMAL,
                    "CAN_APP: Form error event occured\n");
                CanApp_TestPassed = E_OK;
                break;
            case CAN_IRQ_INTR_ACK_EVENT:
                AppUtils_Printf(MSG_NORMAL,
                    "CAN_APP: ACK error event occured\n");
                CanApp_TestPassed = E_OK;
                break;
            case CAN_IRQ_INTR_BIT1_EVENT:
                AppUtils_Printf(MSG_NORMAL,
                    "CAN_APP: Bit 1 error event occured\n");
                CanApp_TestPassed = E_OK;
                break;
            case CAN_IRQ_INTR_BIT0_EVENT:
                AppUtils_Printf(MSG_NORMAL,
                    "CAN_APP: Bit 0 error event occured\n");
                CanApp_TestPassed = E_OK;
                break;
            case CAN_IRQ_INTR_CRC_EVENT:
                AppUtils_Printf(MSG_NORMAL,
                    "CAN_APP: CRC error event occured\n");
                CanApp_TestPassed = E_OK;
                break;
            case CAN_IRQ_INTR_TOO_EVENT:
                AppUtils_Printf(MSG_NORMAL,
                    "CAN_APP: TOO error event occured\n");
                CanApp_TestPassed = E_OK;
                break;
            case CAN_IRQ_STATUS_READ_FAIL:
                AppUtils_Printf(MSG_NORMAL,
                    "CAN_APP: Interrupt Status reading failed\n");
                CanApp_TestPassed = E_NOT_OK;
                break;
        }

        AppUtils_Printf(MSG_NORMAL,"CAN_APP: Can_ClearIntrStatus() API test\n");
        status = Can_ClearIntrStatus(
                    CanConfigSet_CanController_List_PC[ctlr_cnt]->ControllerId);
        if (status != E_OK)
        {
            AppUtils_Printf(MSG_NORMAL,
                "CAN_APP: Interrupt Status Clear Fail\n");
            CanApp_TestPassed = E_NOT_OK;
        }
        else
        {
            AppUtils_Printf(MSG_NORMAL,
                "CAN_APP: Interrupt Status Clear Successful\n");
            CanApp_TestPassed = E_OK;
        }

        AppUtils_Printf(MSG_NORMAL,"CAN_APP: Can_DisableIntr() API test\n");
        status = Can_DisableIntr(
            CanConfigSet_CanController_List_PC[ctlr_cnt]->ControllerId,
            MCAN_ERR_TYPE_TOO_ERROR);
        if (status != E_OK)
        {
            AppUtils_Printf(MSG_NORMAL,
            "CAN_APP: Failed to Disable TOO interrupt\n");
            CanApp_TestPassed = E_NOT_OK;
        }
        else
        {
            AppUtils_Printf(MSG_NORMAL,
            "CAN_APP: Successfully Disable TOO interrupt\n");
            CanApp_TestPassed = E_OK;
        }
        status = Can_DisableIntr(
            CanConfigSet_CanController_List_PC[ctlr_cnt]->ControllerId,
            MCAN_ERR_TYPE_PEA_ERROR);
        if (status != E_OK)
        {
            AppUtils_Printf(MSG_NORMAL,
            "CAN_APP: Failed to Disable PEA interrupt\n");
            CanApp_TestPassed = E_NOT_OK;
        }
        else
        {
            AppUtils_Printf(MSG_NORMAL,
            "CAN_APP: Successfully Disable PEA interrupt\n");
            CanApp_TestPassed = E_OK;
        }

        AppUtils_Printf(MSG_NORMAL,"CAN_APP: Can_RegisterReadback() API test\n");
        status = Can_RegisterReadback(
            CanConfigSet_CanController_List_PC[ctlr_cnt]->ControllerId,
            &Can_RegRb);
        if (status != E_OK)
        {
            AppUtils_Printf(MSG_NORMAL,
                "CAN_APP: Can_RegisterReadback Failed.\n");
            CanApp_TestPassed = E_NOT_OK;
        }
        else
        {
            AppUtils_Printf(MSG_NORMAL,
                "CAN_APP: Can_RegisterReadback Successful\n");
            CanApp_TestPassed = E_OK;
        }
    }
}
#endif /* #if (STD_ON == CAN_SAFETY_DIAGNOSTIC)*/

#define CAN_START_SEC_ISR_CODE
#include "Can_MemMap.h"

#if defined (SOC_AM62X) || defined (SOC_AM62AX) || defined (SOC_AM62PX) || defined (SOC_J722S)
FUNC(void, CAN_CODE_FAST) CanApp_CanXIsr(void *CanPtr)
{
    CanApp_IsrType canChIsr = (CanApp_IsrType)CanPtr;

    /* Associated CAN ISR */
    canChIsr();
}
#else /* ISR for Jacinto */
FUNC(void, CAN_CODE_FAST) CanApp_CanXIsr(uintptr_t CanPtr)
{
    CanApp_IsrType canChIsr = (CanApp_IsrType)CanPtr;

    /* Associated CAN ISR */
    canChIsr();
}
#endif

#define CAN_STOP_SEC_ISR_CODE
#include "Can_MemMap.h"
