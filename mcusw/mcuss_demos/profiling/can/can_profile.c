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

#if (CAN_TX_POLLING == STD_ON) && (CAN_RX_POLLING == STD_ON)
#if (CAN_BATCH_PROCESSING_POLLING == STD_ON)
#define BATCH_SIZE      (8U)
/**< Batch size for batch processing */
#define SLEEP_TIME      (4U)
/**< Polling interval */
#else
#define SLEEP_TIME      (1U)
/**< Polling interval */
#endif
#endif

/** \brief Message that would be displayed */
#if (CAN_EXT_LOOPBACK == STD_ON)
#define CAN_TX_RX_MSG "Transmit & Receive (External-loopback) "
#else
#if (CAN_LOOPBACK_ENABLE == STD_ON)
#define CAN_TX_RX_MSG "Transmit & Receive (Internal-loopback) "
#else
#if (CAN_TX_ONLY_MODE == STD_ON)
#define CAN_TX_RX_MSG "Transmit "
#else
#define CAN_TX_RX_MSG "Receive "
#endif
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
Can_PduType                 CanApp_Pdu2, CanApp_Pdu3, CanApp_Pdu4, CanApp_Pdu5;
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
volatile PduInfoType                 *CanApp_RxPdu = NULL;
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

extern volatile VAR(uint32, CANIF_VAR_CLEARED) CanIf_TxConfirmationCount;
/**< TX Confirmation counter */
extern volatile VAR(uint32, CANIF_VAR_CLEARED) CanIf_RxConfirmationCount;
/**< RX Indication counter */
#if (CAN_TX_POLLING == STD_OFF) && (CAN_RX_POLLING == STD_OFF)
extern SemaphoreP_Handle CanIf_TxConfirmationSemaphore;
/**< TX Confirmation semaphore */
extern SemaphoreP_Handle CanIf_RxConfirmationSemaphore;
/**< Rx Confirmation semaphore */
#endif

extern volatile PduInfoType CanIf_PduInfo;
uint64_t CanProfileCumulativeTx;
/**< Global used to track cumulative time spent for transmission */

/* ========================================================================== */
/*                        PMU setup code for reference                        */
/* ========================================================================== */
#ifdef PMU_PROFILE

#define PMU_EVENT_COUNTER_1 (CSL_ARM_R5_PMU_EVENT_TYPE_CYCLE_CNT)
#define PMU_EVENT_COUNTER_2 (CSL_ARM_R5_PMU_EVENT_TYPE_ICACHE_STALL)
#define PMU_EVENT_COUNTER_3 (CSL_ARM_R5_PMU_EVENT_TYPE_DCACHE_MISS)

/* To use PMU instrumentation, call the config_pmu() once during init time.
 *
 * Reset the counters using CSL_armR5PmuResetCntrs() API.
 * Then use CSL_armR5PmuReadCntr() with argument 0, 1 or 2 to capture value
 * of 1st, 2nd or 3rd PMU counter.
 */

uint32_t config_pmu(void)
{

    CSL_armR5PmuCfg(0, 0 ,1);
    CSL_armR5PmuEnableAllCntrs(1);
    uint32_t num_cnt = CSL_armR5PmuGetNumCntrs();

    CSL_armR5PmuCfgCntr(0, PMU_EVENT_COUNTER_1);
    CSL_armR5PmuCfgCntr(1, PMU_EVENT_COUNTER_2);
    CSL_armR5PmuCfgCntr(2, PMU_EVENT_COUNTER_3);

    CSL_armR5PmuEnableCntrOverflowIntr(0, 0);
    CSL_armR5PmuEnableCntrOverflowIntr(1, 0);
    CSL_armR5PmuEnableCntrOverflowIntr(2, 0);
    CSL_armR5PmuResetCntrs();
    CSL_armR5PmuEnableCntr(0, 1);
    CSL_armR5PmuEnableCntr(1, 1);
    CSL_armR5PmuEnableCntr(2, 1);
    return 0;
}

#endif
/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */
sint32 CanApp_ProfileTest(canAppTaskObj_t *testParams)
{
    uint32              ctlr_cnt;
    Std_ReturnType      status;
#if (CAN_BATCH_PROCESSING_POLLING == STD_ON) && (CAN_TX_POLLING == STD_ON) && (CAN_RX_POLLING == STD_ON)
    Std_ReturnType      writeStatus[BATCH_SIZE];
#endif
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
    /* Message for CAN-FD(MCU MCAN0 module) */
#if ((CAN_TX_ONLY_MODE == STD_ON) || (CAN_EXT_LOOPBACK == STD_ON))
    /* Setting Up CAN FD Extended Frame*/
    CanApp_Pdu.id          = 0xC0 | 0xC0000000U;
    CanApp_Pdu.length      = 64U;
    CanApp_Pdu.swPduHandle = 1U;
    CanApp_Pdu.sdu         = &CanApp_InputData[0U];

    /* Message for CAN-FD(MCU MCAN1 module) */
    /* Setting Up CAN FD Extended Frame*/
    CanApp_Pdu1.id          = 0xB0 | 0xC0000000U;
    CanApp_Pdu1.length      = 64U;
    CanApp_Pdu1.swPduHandle = 2U;
    CanApp_Pdu1.sdu         = &CanApp_InputData[0U];

    /* Message for CAN-FD(MAIN MCAN0 module) */
    /* Setting Up CAN FD Frame*/
    CanApp_Pdu2.id          = 0xC0 | 0xC0000000U;
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
    CanApp_Pdu4.id          = 0xE0 | 0xC0000000U;
    CanApp_Pdu4.length      = 64U;
    CanApp_Pdu4.swPduHandle = 5U;
    CanApp_Pdu4.sdu         = &CanApp_InputData[0U];

    /* Message for CAN-FD(MAIN MCAN11 module) */
    /* Setting Up CAN FD Frame*/
    CanApp_Pdu5.id          = 0xF0 | 0xC0000000U;
    CanApp_Pdu5.length      = 64U;
    CanApp_Pdu5.swPduHandle = 6U;
    CanApp_Pdu5.sdu         = &CanApp_InputData[0U];

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
        CanIf_TxConfirmationCount = 0;
        CanIf_RxConfirmationCount = 0;

        CanApp_ProfileStatus = E_OK;

#if (CAN_TX_ONLY_MODE == STD_ON)
        /* Set Can frame */
        CanApp_SetupCanFrame(ctlr_cnt, &CanApp_Hth, &CanApp_Mask);
#endif

#if (CAN_EXT_LOOPBACK == STD_ON)
        /* Set Can frame */
        CanApp_SetupCanFrameExternalLoopback(&CanApp_Hth, &CanApp_Mask);
        /* Enable Tx and Rx controller for external loopback mode */
        status = CanApp_SetupControllersExternalLoopback(CAN_CS_STARTED);
        if (status != E_OK)
        {
            CanApp_ProfileStatus = E_NOT_OK;
        }
#else
        /* Set Controller Mode to Start */
        status = Can_SetControllerMode(
        CanConfigSet_CanController_List_PC[ctlr_cnt]->ControllerId, CAN_CS_STARTED);
        if (status != E_OK)
        {
            CanApp_ProfileStatus = E_NOT_OK;
        }
#endif

#if (CAN_LOOPBACK_ENABLE == STD_ON)
        /* Enable loopback mode in CAN */
        Can_TestLoopBackModeEnable(
        CanConfigSet_CanController_List_PC[ctlr_cnt]->ControllerId, 0U);
#endif

/* To showcase early CAN response, disable prints before 1st CAN msg is sent */
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
#if (CAN_EXT_LOOPBACK == STD_ON) || (CAN_LOOPBACK_ENABLE == STD_ON)
                /* CORE: MCU2_1
                 * Tx - MCAN4, Rx - MCAN9
                 * External loopback for MCAN4 to MCAN9, so transmit the frame
                 * which can be received by MCAN9 as per it's filter
                 * configuration.
                 *
                 * CORE: MCU1_0
                 * Tx - MCU MCAN0, Rx - MCU MCAN1
                 * External loopback for MCU MCAN0 to MCU MCAN1, so transmit
                 * the frame which can be received by MCAN1 as per it's filter
                 * configuration.
                 *
                 * Frames setup by CanApp_SetupCanFrameExternalLoopback()
                 */

#if (CAN_BATCH_PROCESSING_POLLING == STD_ON) && (CAN_TX_POLLING == STD_ON) && (CAN_RX_POLLING == STD_ON)
                /* Do Can Write to send the CanApp_InputData on CAN bus */
                writeStatus[0] = Can_Write(CanApp_Hth, CanApp_PduInfo);
                writeStatus[1] = Can_Write(CanApp_Hth, CanApp_PduInfo);
                writeStatus[2] = Can_Write(CanApp_Hth, CanApp_PduInfo);
                writeStatus[3] = Can_Write(CanApp_Hth, CanApp_PduInfo);
                writeStatus[4] = Can_Write(CanApp_Hth, CanApp_PduInfo);
                writeStatus[5] = Can_Write(CanApp_Hth, CanApp_PduInfo);
                writeStatus[6] = Can_Write(CanApp_Hth, CanApp_PduInfo);
                writeStatus[7] = Can_Write(CanApp_Hth, CanApp_PduInfo);

                if ((writeStatus[0] == E_OK) && (writeStatus[1] == E_OK) &&
                    (writeStatus[2] == E_OK) && (writeStatus[3] == E_OK) &&
                    (writeStatus[4] == E_OK) && (writeStatus[5] == E_OK) &&
                    (writeStatus[6] == E_OK) && (writeStatus[7] == E_OK)
                   )
                {
#else
                status = Can_Write(CanApp_Hth, CanApp_PduInfo);
                if (status == E_OK)
                {
#endif
#if (CAN_TX_POLLING == STD_ON) && (CAN_RX_POLLING == STD_ON)
                    TaskP_sleep(SLEEP_TIME);
                    Can_MainFunction_Write();
#else
                    SemaphoreP_pend(CanIf_TxConfirmationSemaphore,
                                    SemaphoreP_WAIT_FOREVER);
#endif
                }
                else
                {
                    CanApp_ProfileStatus = E_NOT_OK;
                }

#if (CAN_TX_POLLING == STD_ON) && (CAN_RX_POLLING == STD_ON)
                Can_MainFunction_Read();
#else
                SemaphoreP_pend(CanIf_RxConfirmationSemaphore,
                                SemaphoreP_WAIT_FOREVER);
#endif
#else
/* Do either Tx only or Rx only */
/* Tx Only */
#if (CAN_TX_ONLY_MODE == STD_ON)
                /* Do Can Write to send the CanApp_InputData on CAN bus */
                status = Can_Write(CanApp_Hth, CanApp_PduInfo);
                if (status == E_OK)
                {
#if (CAN_TX_POLLING == STD_ON)
                    TaskP_sleep(SLEEP_TIME);
                    Can_MainFunction_Write();
#else
                    SemaphoreP_pend(CanIf_TxConfirmationSemaphore,
                                    SemaphoreP_WAIT_FOREVER);
#endif
                }
                else
                {
                    CanApp_ProfileStatus = E_NOT_OK;
                }
/* Rx Only */
#else
#if (CAN_RX_POLLING == STD_ON)
                /* Periodic sweep of messages */
                TaskP_sleep(SLEEP_TIME);
                Can_MainFunction_Read();
#else
                SemaphoreP_pend(CanIf_RxConfirmationSemaphore,
                                SemaphoreP_WAIT_FOREVER);
#endif
#endif
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
#if (CAN_TX_POLLING == STD_ON) && (CAN_RX_POLLING == STD_ON)
            AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
                        "NOTE : Operating in polling mode!\n");
#if (CAN_BATCH_PROCESSING_POLLING == STD_ON)
            AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
                        "NOTE : Batch processing enabled!\n");
#endif
#else
            AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
                        "NOTE : Operating in interrupt mode!\n");
#endif

            AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
                        CAN_TX_RX_MSG " %u packets %d times\n",
                        testParams->numTxPackets , testParams->numIterations);

            AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
                    "Average of %lld.%02d usecs per packet \n",
                    CanProfileCumulativeTx / (CanIf_TxConfirmationCount + CanIf_RxConfirmationCount),
                    CanProfileCumulativeTx % (CanIf_TxConfirmationCount + CanIf_RxConfirmationCount));
            totalTimePerMsg = ((float)CanProfileCumulativeTx / (float)(CanIf_TxConfirmationCount + CanIf_RxConfirmationCount));

            totalCanMsgPerSec = (1000000.0f / totalTimePerMsg);

            /* Hardware interrupt handler load + Software interrupt handler +
                task load */
            totalCpuload = computedLoad.tskLoad;

            AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
                    "Average of %u packets in 1 second with CPU Load %f%% \r\n",
                    totalCanMsgPerSec, totalCpuload);

            AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
                    "Packets sent: %u, Packets recv: %u in total time: %llu us\r\n",
                    CanIf_TxConfirmationCount, CanIf_RxConfirmationCount, CanProfileCumulativeTx);

            AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
                            "Measured Load: Total CPU: "
                            "%f%%, TSK: %f%%\r\n",
                            computedLoad.cpuLoad,
                            computedLoad.tskLoad);

#if (CAN_EXT_LOOPBACK == STD_ON)
            /* Stop Tx and Rx controller for external loopback mode */
            status = CanApp_SetupControllersExternalLoopback(CAN_CS_STOPPED);
            if (status != E_OK)
            {
                CanApp_ProfileStatus = E_NOT_OK;
            }
#else
            /* Set Controller Mode to Stop*/
            status = Can_SetControllerMode(
                CanConfigSet_CanController_List_PC[ctlr_cnt]->ControllerId,
                CAN_CS_STOPPED);
            if (status != E_OK)
            {
                CanApp_ProfileStatus = E_NOT_OK;
            }
#endif

#if (CAN_LOOPBACK_ENABLE == STD_ON)
            Can_TestLoopBackModeDisable(
            CanConfigSet_CanController_List_PC[ctlr_cnt]->ControllerId, 0U);
#endif

#if ((CAN_LOOPBACK_ENABLE == STD_ON) || (CAN_EXT_LOOPBACK == STD_ON))
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
    AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
                    " %d bytes used for stack \n", AppUtils_GetStackUsage());
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
