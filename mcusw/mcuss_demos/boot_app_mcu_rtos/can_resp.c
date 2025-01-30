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
 *  \file     can_profile.c
 *
 *  \brief    This file implements CAN profiling application
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "Std_Types.h"
#include "Det.h"
#include "Dem.h"
#include "Os.h"
#include "Can.h"
#include "CanIf_Cbk.h"
#include "EcuM_Cbk.h"
#include "Dio.h"

#include <ti/csl/arch/csl_arch.h>
#include <ti/csl/soc.h>
#include <ti/csl/cslr.h>

#include "can_resp.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
#define CAN_APP_E_DEM   (E_NOT_OK + 1U)
/**< Error code to indicate DEM */
#define CAN_APP_E_DET   (E_NOT_OK + 2U)
/**< Error code to indicate DEM */

/** \brief Message that would be displayed */
#if (CAN_LOOPBACK_ENABLE == STD_ON)
#define CAN_TX_RX_MSG "Transmit & Receive "
#else
#if (CAN_TX_ONLY_MODE == STD_ON)
#define CAN_TX_RX_MSG "Transmit "
#else
#define CAN_TX_RX_MSG "Receive "
#endif
#endif
/* ========================================================================== */
/*              Internal Function Declarations                                */
/* ========================================================================== */
uint64_t CanProfileGetTimeSpent(uint64_t preTs, uint64_t postTs);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
extern Can_PduType         CanApp_Pdu, CanApp_Pdu1;
/**< Variables which contains Can PDU data used in Can transmission */
extern uint8               CanApp_InputData[64U];
/**< Variable which contains Can SDU data that is transported inside the PDU */
extern volatile PduInfoType        *CanApp_RxPdu;
/**< Pointer received data */
extern uint32              CanApp_DemStatus;
/**< Variable used to track dem notifications */
extern uint32              CanApp_DetStatus;
/**< Variable used to track det notifications */
static Dem_EventIdType     CanApp_EventId;
/**< Variable used to track dem event, reported event id */
static Dem_EventStatusType CanApp_EventStatus;
/**< Variable used to track dem event, reported event status */
static uint16              CanApp_ModuleId;
/**< Variable used to track det event, reported module id */
static uint8               CanApp_InstanceId;
/**< Variable used to track det event, reported instance id */
static uint8               CanApp_ApiId;
/**< Variable used to track det event, reported api id */
static uint8               CanApp_ErrorId;
/**< Variable used to track det event, reported error id */

/* ========================================================================== */
/*                            External Variables                              */
/* ========================================================================== */
/* Global variables used in callback functions */
extern volatile P2VAR(Can_HwType, CANIF_VAR_CLEARED, CANIF_APPL_DATA)
        CanIf_Mailbox;
extern volatile PduInfoType CanIf_PduInfo;
extern SemaphoreP_Handle CanIf_TxConfirmationSemaphore;
/**< TX Confirmation semaphore */
extern SemaphoreP_Handle CanIf_RxConfirmationSemaphore;
/**< Rx Confirmation semaphore */
extern uint64_t CanProfileCumulativeTx;
/**< Global used to track cumulative time spent for transmission */
/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */
sint32 CanApp_ResponseTest(void)
{
    uint32              CanApp_ResponseStatus = E_OK;
#if (CAN_LOOPBACK_ENABLE == STD_ON)
    uint32              mask = 0x00000000U;
#endif
    uint32              ctlr_cnt;
#if (CAN_TX_ONLY_MODE == STD_ON)
    const Can_PduType   *PduInfo;
    uint8               hth = 0U;
#endif
    Std_ReturnType      status;

    CanApp_RxPdu = &CanIf_PduInfo;
    /* Message for CAN-FD(MCAN0 module) */
#if (CAN_TX_ONLY_MODE == STD_ON)
    PduInfo         = &CanApp_Pdu;
    /* Setting Up CAN FD Extended Frame*/
    CanApp_Pdu.id          = 0xC0 | 0xC0000000U;
    CanApp_Pdu.length      = 64U;
    CanApp_Pdu.swPduHandle = 1U;
    CanApp_Pdu.sdu         = &CanApp_InputData[0U];

    /* Message for CAN-FD(MCAN1 module) */
    /* Setting Up CAN FD Extended Frame*/
    CanApp_Pdu1.id          = 0xB0 | 0xC0000000U;
    CanApp_Pdu1.length      = 64U;
    CanApp_Pdu1.swPduHandle = 2U;
    CanApp_Pdu1.sdu         = &CanApp_InputData[0U];

    CanApp_DemStatus = E_OK;
    CanApp_DetStatus = E_OK;
#endif
    
#if (APP_INSTANCE_1_INST_IN_CFG_ONLY == STD_OFF)
    for (ctlr_cnt = 0U; ctlr_cnt < CAN_NUM_CONTROLLER; ctlr_cnt++)
#else
    ctlr_cnt = 0U;
#endif
    {
        CanApp_ResponseStatus = E_OK;

#if (CAN_TX_ONLY_MODE == STD_ON)
        if (CAN_CONTROLLER_INSTANCE_MCU_MCAN0 ==
            CanConfigSet_CanController_List_PC[ctlr_cnt]->CanControllerInst)
        {
            /* CAN Protocol Data Unit */
            PduInfo = &CanApp_Pdu;
            /* Hardware Transmit Handle assigned to MCAN0 */
            hth     = CAN_HTRH_0;
            /* mask is need for checking id of received message with
             * sent message as 30th bit the of sent message will be set
             * for CAN FD frame */
#if (CAN_LOOPBACK_ENABLE == STD_ON)
            mask    = 0xC0000000U;
#endif
        }

        if (CAN_CONTROLLER_INSTANCE_MCU_MCAN1 ==
            CanConfigSet_CanController_List_PC[ctlr_cnt]->CanControllerInst)
        {
            /* CAN Protocol Data Unit */
            PduInfo = &CanApp_Pdu1;
            /* Hardware Transmit Handle assigned to MCAN1 */
            hth     = CAN_HTRH_2;
            /* mask is need for checking id of received message with
             * sent message as 30th bit the of sent message will be set
             * for CAN FD frame */
#if (CAN_LOOPBACK_ENABLE == STD_ON)
            mask = 0xC0000000U;
#endif
        }
#endif

        /* Set Controller Mode to Start */
        status = Can_SetControllerMode(
        CanConfigSet_CanController_List_PC[ctlr_cnt]->ControllerId, CAN_CS_STARTED);
        if (status != E_OK)
        {
            CanApp_ResponseStatus = E_NOT_OK;
        }

#if (CAN_LOOPBACK_ENABLE == STD_ON)
        /* Enable loopback mode in CAN */
        Can_TestLoopBackModeEnable(
        CanConfigSet_CanController_List_PC[ctlr_cnt]->ControllerId, 0U);
#endif

#if (CAN_LOOPBACK_ENABLE == STD_OFF)

/* Do either Tx only or Rx only */
/* Tx Only */
#if (CAN_TX_ONLY_MODE == STD_ON)
        /* Do Can Write to send the CanApp_InputData on CAN bus */
        status = Can_Write(hth, PduInfo);
        if (status == E_OK)
        {
            SemaphoreP_pend(CanIf_TxConfirmationSemaphore,
                            SemaphoreP_WAIT_FOREVER);
        }
        else
        {
            CanApp_ResponseStatus = E_NOT_OK;
        }
/* Rx Only */
#else
        /* Pend on the semaphore posted in Rx indication */
        SemaphoreP_pend(CanIf_RxConfirmationSemaphore,
                        SemaphoreP_WAIT_FOREVER);
#endif

/* Internal loopback */
#else
        /* Do Can Write to send the CanApp_InputData on CAN bus */
        status = Can_Write(hth, PduInfo);
        if (status == E_OK)
        {
            SemaphoreP_pend(CanIf_TxConfirmationSemaphore,
                            SemaphoreP_WAIT_FOREVER);
        }
        else
        {
            CanApp_ResponseStatus = E_NOT_OK;
        }

        /* Pend on the semaphore posted in Rx indication */
        SemaphoreP_pend(CanIf_RxConfirmationSemaphore,
                        SemaphoreP_WAIT_FOREVER);
#endif
        if (E_OK == CanApp_ResponseStatus)
        {
#if (CAN_LOOPBACK_ENABLE == STD_ON)
            AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
                        "NOTE : Operating in internal loop-back mode\n");
#endif

            /* Set Controller Mode to Stop*/
            status = Can_SetControllerMode(
                CanConfigSet_CanController_List_PC[ctlr_cnt]->ControllerId,
                CAN_CS_STOPPED);
            if (status != E_OK)
            {
                CanApp_ResponseStatus = E_NOT_OK;
            }

#if (CAN_LOOPBACK_ENABLE == STD_ON)
            Can_TestLoopBackModeDisable(
            CanConfigSet_CanController_List_PC[ctlr_cnt]->ControllerId, 0U);
#endif

#if (CAN_LOOPBACK_ENABLE == STD_ON)
            if ((PduInfo->length == CanApp_RxPdu->SduLength) &&
                (PduInfo->id == (CanIf_Mailbox->CanId | mask)))
            {
                AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
                "Message Id Received %x Message Length is %u\t \n",
                CanIf_Mailbox->CanId, CanApp_RxPdu->SduLength);
                if (CanApp_RxPdu->SduDataPtr[0] !=
                    CanApp_InputData[0])
                {
                    CanApp_ResponseStatus = E_NOT_OK;
                    AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
                                "Transmitted and Received Data miss-match.\n");
                    break;
                }
                else
                {
                    CanApp_ResponseStatus = E_OK;
                }
                AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
                            "Test completed for %u instance \n\n", ctlr_cnt);
            }
            else
            {
                CanApp_ResponseStatus = E_NOT_OK;
            }
#endif
        }
    }
    AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
                    " %d bytes used for stack \n", AppUtils_GetStackUsage());
    if (E_OK == CanApp_ResponseStatus)
    {
        //AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME "Profiling completes!!!\n");
        AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME "Early CAN completed!!!\n");
        AppUtils_LogResult(APP_UTILS_STATUS_PASS);
    }
    else
    {
        if (CAN_APP_E_DEM == CanApp_DemStatus)
        {
            AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
                    "Dem error was reported, eventId = %d, eventStatus = %d\n",
                    CanApp_EventId, CanApp_EventStatus);
            CanApp_DemStatus = E_OK;
        }
        if (CAN_APP_E_DET == CanApp_DetStatus)
        {
            AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
                                                    "Det error was reported\n");
            AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
                                    "With Module ID as %d\n", CanApp_ModuleId);
            AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
                                "With Module ID as %d\n", CanApp_InstanceId);
            AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
                                "With Module ID as %d\n", CanApp_ApiId);
            AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
                                "With Module ID as %d\n", CanApp_ErrorId);
            CanApp_DetStatus = E_OK;
        }
        AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME "CAN Test Failed!!!\n");
        AppUtils_LogResult(APP_UTILS_STATUS_FAIL);
    }

    return (CanApp_ResponseStatus);
}

