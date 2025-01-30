/*
*
* Copyright (C) 2024 Texas Instruments Incorporated
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

#include "can_profile.h"

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
uint32                      CanApp_ProfileStatus = E_OK;
/**< Variable used for Demo status */
Can_PduType                 CanApp_Pdu, CanApp_Pdu1;
/**< Variables which contains Can PDU data used in Can transmission */
uint8                       CanApp_InputData[64U] =
{0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x22, 0x44,
0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x22, 0x44,
0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x22, 0x44,
0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x22, 0x44,
0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x22, 0x44,
0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x22, 0x44,
0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x22, 0x44,
0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x22, 0x44,
};
/**< Variable which contains Can SDU data that is transported inside the PDU */
volatile PduInfoType        *CanApp_RxPdu = NULL;
/**< Pointer received data */
uint32                      CanApp_DemStatus = E_OK;
/**< Variable used to track dem notifications */
uint32                      CanApp_DetStatus = E_OK;
/**< Variable used to track det notifications */
static Dem_EventIdType     CanApp_EventId;
/**< Variable used to track dem event, reported event id */
static Dem_EventStatusType CanApp_EventStatus;
/**< Variable used to track dem event, reported event status */
static uint16 CanApp_ModuleId;
/**< Variable used to track det event, reported module id */
static uint8 CanApp_InstanceId;
/**< Variable used to track det event, reported instance id */
static uint8 CanApp_ApiId;
/**< Variable used to track det event, reported api id */
static uint8 CanApp_ErrorId;
/**< Variable used to track det event, reported error id */
#if (CAN_TX_ONLY_MODE == STD_ON)
const Can_PduType   *CanApp_PduInfo;
/**< Pointer to Can PDU used in Can transmission  */
uint8               CanApp_Hth = 0U;
/**< Hardware Mailbox Number configured to trigger transmission */
uint32              CanApp_Mask = 0x00000000U;
/**< Mask is used for checking received message id with transmitted message */
#endif
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
uint64_t CanProfileCumulativeTx;
/**< Global used to track cumulative time spent for transmission */
/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */
sint32 CanApp_ProfileTest(canAppTaskObj_t *testParams)
{
    uint32              ctlr_cnt;
    Std_ReturnType      status;
    uint32              loopCnt, itr, totalCanMsgPerSec;
    float totalCpuload;
    float               totalTimePerMsg;
    Utils_PrfLoad       computedLoad;
    uint64_t            preTimeStamp, postTimeStamp;

#if (APP_INSTANCE_1_INST_IN_CFG_ONLY == STD_OFF)
    const Can_ConfigType *Can_ConfigPtr;
#if (STD_ON == CAN_VARIANT_PRE_COMPILE)
    Can_ConfigPtr = &CAN_INIT_CONFIG_PC;
#else
    Can_ConfigPtr = &CanConfigSet;
#endif
#endif

    CanApp_RxPdu = &CanIf_PduInfo;
    /* Message for CAN-FD(MCAN0 module) */
#if (CAN_TX_ONLY_MODE == STD_ON)
    CanApp_PduInfo         = &CanApp_Pdu;
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
    uint32      canMaxControllerCnt;
    canMaxControllerCnt = Can_ConfigPtr->CanMaxControllerCount;
    for (ctlr_cnt = 0U; ctlr_cnt < canMaxControllerCnt; ctlr_cnt++)
#else
    ctlr_cnt = 0U;
#endif
    {
        CanApp_ProfileStatus = E_OK;

#if (CAN_TX_ONLY_MODE == STD_ON)
        /* Set Can frame */
		CanApp_SetupCanFrame(ctlr_cnt, &CanApp_Hth, &CanApp_Mask);
#endif

        /* Set Controller Mode to Start */
        status = Can_SetControllerMode(
        CanConfigSet_CanController_List_PC[ctlr_cnt]->ControllerId, CAN_CS_STARTED);
        if (status != E_OK)
        {
            CanApp_ProfileStatus = E_NOT_OK;
        }

#if (CAN_LOOPBACK_ENABLE == STD_ON)
        /* Enable loopback mode in CAN */
        Can_TestLoopBackModeEnable(
        CanConfigSet_CanController_List_PC[ctlr_cnt]->ControllerId, 0U);
#endif

#if (CAN_INITIAL_PRINT_DISABLE_BEFORE_CAN_RESPONSE == STD_OFF)
        AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
                        "Will " CAN_TX_RX_MSG "%d Messages, %d times\n",
                        testParams->numTxPackets, testParams->numIterations);
#endif

        CanProfileCumulativeTx = 0U;

        /* Start Time stamp */
        preTimeStamp = TimerP_getTimeInUsecs();

        Utils_prfLoadCalcReset();
        Utils_prfLoadCalcStart();
        for (itr = 0U; itr < testParams->numIterations; itr++)
        {
            for (loopCnt = 0U; loopCnt < testParams->numTxPackets; loopCnt++)
            {
#if (CAN_LOOPBACK_ENABLE == STD_OFF)

/* Do either Tx only or Rx only */
/* Tx Only */
#if (CAN_TX_ONLY_MODE == STD_ON)
                /* Do Can Write to send the CanApp_InputData on CAN bus */
                status = Can_Write(CanApp_Hth, CanApp_PduInfo);
                if (status == E_OK)
                {
                    SemaphoreP_pend(CanIf_TxConfirmationSemaphore,
                                    SemaphoreP_WAIT_FOREVER);
                }
                else
                {
                    CanApp_ProfileStatus = E_NOT_OK;
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
                status = Can_Write(CanApp_Hth, CanApp_PduInfo);
                if (status == E_OK)
                {
                    SemaphoreP_pend(CanIf_TxConfirmationSemaphore,
                                    SemaphoreP_WAIT_FOREVER);
                }
                else
                {
                    CanApp_ProfileStatus = E_NOT_OK;
                }

                /* Pend on the semaphore posted in Rx indication */
                SemaphoreP_pend(CanIf_RxConfirmationSemaphore,
                                SemaphoreP_WAIT_FOREVER);
#endif
            }
        }
        Utils_prfLoadCalcStop ();

        /* End Time stamp */
        postTimeStamp = TimerP_getTimeInUsecs();

        /* Time for transmission of numIterations*numTxPackets messages */
        CanProfileCumulativeTx =
                        CanProfileGetTimeSpent(preTimeStamp, postTimeStamp);

        if (E_OK == CanApp_ProfileStatus)
        {
            Utils_prfLoadGetTaskLoad(TaskP_self(), &computedLoad);
#if (CAN_LOOPBACK_ENABLE == STD_ON)
            AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
                        "NOTE : Operating in internal loop-back mode\n");
#endif

            AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
                        CAN_TX_RX_MSG " %u packets %d times\n",
                        testParams->numTxPackets, testParams->numIterations);

            /* Number of packets transmitted * number of iterations *
                1 or 2 (2 in case of tx & rx) transmitted in msecs (computed
                above)
            */
            AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
                    "Average of %lld.%02d usecs per packet \n",
                    CanProfileCumulativeTx / (testParams->numTxPackets *
                                  testParams->numIterations),
                    CanProfileCumulativeTx % (testParams->numTxPackets *
                                  testParams->numIterations));
            totalTimePerMsg = ((float)CanProfileCumulativeTx / ((float)testParams->numTxPackets *
                                  (float)testParams->numIterations));
            totalCanMsgPerSec = (1000000.0f / totalTimePerMsg);

            /* Hardware interrupt handler load + Software interrupt handler +
                task load */
            totalCpuload = computedLoad.hwiLoad + computedLoad.swiLoad + 
                            computedLoad.tskLoad;

            AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
                    "Average of %u packets in 1 second with CPU Load %f%% \r\n",
                    totalCanMsgPerSec, totalCpuload);

            AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
                            "Measured Load: Total CPU: "
                            "%f%%, HWI: %f%%, SWI:%f%% TSK: %f%%\r\n",
                            computedLoad.cpuLoad,
                            computedLoad.hwiLoad,
                            computedLoad.swiLoad,
                            computedLoad.tskLoad);

            /* Set Controller Mode to Stop*/
            status = Can_SetControllerMode(
                CanConfigSet_CanController_List_PC[ctlr_cnt]->ControllerId,
                CAN_CS_STOPPED);
            if (status != E_OK)
            {
                CanApp_ProfileStatus = E_NOT_OK;
            }

#if (CAN_LOOPBACK_ENABLE == STD_ON)
            Can_TestLoopBackModeDisable(
            CanConfigSet_CanController_List_PC[ctlr_cnt]->ControllerId, 0U);
#endif

#if (CAN_LOOPBACK_ENABLE == STD_ON)
            if ((CanApp_PduInfo->length == CanApp_RxPdu->SduLength) &&
                (CanApp_PduInfo->id == (CanIf_Mailbox->CanId | CanApp_Mask)))
            {
                AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
                "Message Id Received %x Message Length is %u\t \n",
                CanIf_Mailbox->CanId, CanApp_RxPdu->SduLength);
                for (loopCnt = 0U; 
                     ((loopCnt < CanApp_RxPdu->SduLength)&&(loopCnt < 10U));
                    loopCnt++)
                {
                    if (CanApp_RxPdu->SduDataPtr[loopCnt] !=
                        CanApp_InputData[loopCnt])
                    {
                        CanApp_ProfileStatus = E_NOT_OK;
                        AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
                                "Transmitted and Received Data miss-match.\n");
                        break;
                    }
                    else
                    {
                        CanApp_ProfileStatus = E_OK;
                    }
                }
                AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
                            "Test completed for %u instance \n\n", ctlr_cnt);
            }
            else
            {
                CanApp_ProfileStatus = E_NOT_OK;
            }
#endif
        }
    }
    if (E_OK == CanApp_ProfileStatus)
    {
        AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME "Profiling completes sucessfully!!!\n");
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

    return (CanApp_ProfileStatus);
}


uint64_t CanProfileGetTimeSpent(uint64_t preTs, uint64_t postTs)
{
    uint64_t rtnTs;

    if (postTs >= preTs)
    {
        rtnTs = postTs - preTs;
    }
    else
    {
        rtnTs = postTs + (0xFFFFFFFFFFFFFFFF - preTs);
    }

    return (rtnTs);
}

#define CAN_START_SEC_ISR_CODE
#include "Can_MemMap.h"

FUNC(void, CAN_CODE_FAST) CanApp_CanXIsr (
                                                            uintptr_t CanPtr)
{
    volatile uint64_t   preTimeStamp, postTimeStamp;

    preTimeStamp = TimerP_getTimeInUsecs();
    CanApp_IsrType canChIsr = (CanApp_IsrType)CanPtr;
    /* Associated CAN ISR */
    canChIsr();
    postTimeStamp = TimerP_getTimeInUsecs();
    CanProfileCumulativeTx +=
                CanProfileGetTimeSpent(preTimeStamp, postTimeStamp);
}

#define CAN_STOP_SEC_ISR_CODE
#include "Can_MemMap.h"

