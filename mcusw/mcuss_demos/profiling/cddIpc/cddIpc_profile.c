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
 *  \brief    This file implements CDD IPC profiling application
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "Std_Types.h"
#include "Det.h"
#include "Dem.h"
#include "Os.h"

#include <ti/csl/arch/csl_arch.h>
#include <ti/csl/soc.h>
#include <ti/csl/cslr.h>

#include <ti/osal/osal.h>

#include "cddIpc_profile.h"
#include "SchM_Cdd_Ipc.h" /* Exclusive area */

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
#define CDD_IPC_PROFILE_APP_TX_MSG_BUF_SIZE (64U)
/**< Maximum transmission buffer size */
#define CDD_IPC_PROFILE_APP_RX_MSG_BUF_SIZE (16U)
/**< Maximum reception buffer size */

typedef struct
{
    uint8   *pBuf;
    /**< Pointer to buffer */
    uint32  sizeInBytes;
    /**< Size of the buffer in bytes */

}Cdd_IpcProfileTxBufferType;

/* ========================================================================== */
/*                           Function Prototypes                              */
/* ========================================================================== */

uint64_t Cdd_IpcProfileGetTimeSpent(uint64_t preTs, uint64_t postTs);
void Cdd_IpcProfilePrepareTxBufs(void);
void Cdd_IpcProfilePrintProfiledValues(uint32 itr, uint32 rxLength,
                                        uint64_t cumlativeRxIsrTime);
uint32 Cdd_IpcProfileCore(uint32 commId, SemaphoreP_Handle newIpcRxMsgRcvd);
void Cdd_IpcProfileCheckCtrlMsg(uint32 *pMcu21Flag);
/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
SemaphoreP_Handle Cdd_NewIpcRxMsgRcvd = (SemaphoreP_Handle)NULL;
/**< Semaphore handle to notify reception of a message */
Cdd_IpcProfileTxBufferType Cdd_IpcProfileTxBuf[APP_NUM_ITERATION];
/**< Tx Message buffer containers */
uint8 Cdd_IpcProfileIter0TxData[APP_NUM_ITERATION_0_SIZE];
/**< Buffer that would be transmitted */
uint8 Cdd_IpcProfileIter1TxData[APP_NUM_ITERATION_1_SIZE];
/**< Buffer that would be transmitted */
uint8 Cdd_IpcProfileIter2TxData[APP_NUM_ITERATION_2_SIZE];
/**< Buffer that would be transmitted */
volatile uint64_t Cdd_IpcProfileCumulativeTx;
/**< Global used to track cumulative time spent for transmission */
volatile uint64_t Cdd_IpcProfileCumulativeRx;
/**< Global used to track cumulative time spent for reception */

#if defined (CDD_IPC_REMOTE_CORE_MCU2_1_USED)
uint64_t Cdd_IpcProfileCumulativeRxMcu21Isr;
/**< Global used to track cumulative time spent in reception ISR */
#endif /* CDD_IPC_REMOTE_CORE_MCU2_1_USED */



/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */
void Cdd_IpcNewMessageNotify(uint32 comId)
{

#if defined (SOC_J721E) || defined (SOC_J7200)
    if (CddIpcConf_IpcComChanId_Cdd_IpcMcu21 == comId)
    {
        SemaphoreP_post(Cdd_NewIpcRxMsgRcvd);
    }
#endif
    return;
}

#if (STD_ON == CDD_IPC_ANNOUNCE_API)
void Cdd_IpcNewCtrlMessageNotify(uint32 remoteProcId)
{
    uint32 idx;
    const Cdd_IpcChannelType *pCommChCfg = CddIpcConfiguraions_PC.pChCfg;
    uint32 chanCount = CddIpcConfiguraions_PC.channelCount;

    for (idx = 0U; idx < chanCount; idx++)
    {
        if (remoteProcId == pCommChCfg[idx].remoteProcId)
        {
            break;
        }
    }
    /* Can notify/flag availability of control message */
}

#endif /* (STD_ON == CDD_IPC_ANNOUNCE_API) */

uint32 Cdd_IpcProfileTest(void)
{
    uint32 isMcu21Profiled, rtnVal;
    uint32 ctrlMsgFromMcu21Valid;
    SemaphoreP_Params   semParams;

#if (STD_ON == CDD_IPC_ANNOUNCE_API)
    uint32 coreIdx, comId, rCoreId;
#endif

    rtnVal = E_OK;
    isMcu21Profiled = 1U;

#if defined (CDD_IPC_REMOTE_CORE_MCU2_1_USED)
    isMcu21Profiled = 0U;
#endif


    ctrlMsgFromMcu21Valid = 0U;

#if (STD_ON == CDD_IPC_ANNOUNCE_API)
    const char announceMsg[CDD_IPC_MAX_CTRL_MSG_LEN] = "ti.ipc4.ping-pong";
#endif

    Cdd_IpcProfilePrepareTxBufs();

    SemaphoreP_Params_init(&semParams);

    /* create the semaphore block */
    Cdd_NewIpcRxMsgRcvd = SemaphoreP_create(0, &semParams);

    if ((SemaphoreP_Handle)NULL != Cdd_NewIpcRxMsgRcvd)
    {

#if (STD_ON == CDD_IPC_ANNOUNCE_API)
    /* For every remote core, Announce once */
    for (coreIdx = 0U; coreIdx < CddIpcConfiguraions_PC.coreIds.numProcs;
         coreIdx++)
    {
        rCoreId = CddIpcConfiguraions_PC.coreIds.remoteProcID[coreIdx];
        for (comId = 0U; comId < CddIpcConfiguraions_PC.channelCount; comId++)
        {
            if (rCoreId == CddIpcConfiguraions_PC.pChCfg[comId].remoteProcId)
            {
                Cdd_IpcAnnounce((const unsigned char *)announceMsg,
                                    CddIpcConfiguraions_PC.pChCfg[comId].id);
            }
        }
    }

#else
        /* No control message expected */
        ctrlMsgFromMcu21Valid = 1U;
#endif

        while (0U == isMcu21Profiled)
        {

            /* Check validity of control message */
            Cdd_IpcProfileCheckCtrlMsg(&ctrlMsgFromMcu21Valid);

#if defined (CDD_IPC_REMOTE_CORE_MCU2_1_USED)
            if ((0U == isMcu21Profiled) && (1U == ctrlMsgFromMcu21Valid))
            {
                AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME "\n");
                AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME " Starting Profiling"
                                                            " for MCU 2 1 \n");
                rtnVal = Cdd_IpcProfileCore(
                                    CddIpcConf_IpcComChanId_Cdd_IpcMcu21,
                                    Cdd_NewIpcRxMsgRcvd);
                AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME "\n");
                isMcu21Profiled = 1U;
            }
#endif /* CDD_IPC_REMOTE_CORE_MCU2_1_USED */

        }
        AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME "\n");
        AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
                    "%d bytes used for stack \n", AppUtils_GetStackUsage());
        AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME "\n");

        if (E_OK == rtnVal)
        {
            AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME "Profiling completes\n");
            AppUtils_LogResult(APP_UTILS_STATUS_PASS);
        }
        else
        {
            AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME "IPC Profiling Failed\n");
            AppUtils_LogResult(APP_UTILS_STATUS_FAIL);
        }

    }
    return (rtnVal);
}

uint32 Cdd_IpcProfileCore(uint32 commId, SemaphoreP_Handle newIpcRxMsgRcvd)
{
    uint32              numMsg, itr, *pTxBuf32b, length;
    volatile uint64_t   preTimeStamp, postTimeStamp;
    uint8               rxBuf[CDD_IPC_PROFILE_APP_TX_MSG_BUF_SIZE];
    uint8               txBuf[CDD_IPC_PROFILE_APP_TX_MSG_BUF_SIZE];
    uint32 rtnVal = E_OK;

    /* Configure the number of messages that would be sent */
    pTxBuf32b = (uint32 *)txBuf;
    *pTxBuf32b = (APP_NUM_MSG_PER_ITERATION * APP_NUM_ITERATION);
    length = 4U;
    Cdd_IpcSendMsg(commId, &txBuf[0U], length);

    for (itr = 0U; itr < APP_NUM_ITERATION; itr++)
    {
        Cdd_IpcProfileCumulativeTx = 0U;
        Cdd_IpcProfileCumulativeRx = 0U;

#if defined (CDD_IPC_REMOTE_CORE_MCU2_1_USED)
        Cdd_IpcProfileCumulativeRxMcu21Isr = 0U;
#endif /* CDD_IPC_REMOTE_CORE_MCU2_1_USED */


        Utils_prfLoadCalcReset();
        Utils_prfLoadCalcStart();

        for (numMsg = 0U; ((numMsg < APP_NUM_MSG_PER_ITERATION) &&
                            (E_OK == rtnVal)); numMsg++)
        {
            preTimeStamp = TimerP_getTimeInUsecs();

                Cdd_IpcSendMsg(commId,
                                Cdd_IpcProfileTxBuf[itr].pBuf,
                                Cdd_IpcProfileTxBuf[itr].sizeInBytes);

            postTimeStamp = TimerP_getTimeInUsecs();

            Cdd_IpcProfileCumulativeTx +=
                        Cdd_IpcProfileGetTimeSpent(preTimeStamp, postTimeStamp);

            length = CDD_IPC_PROFILE_APP_TX_MSG_BUF_SIZE-2U;
            SemaphoreP_pend(Cdd_NewIpcRxMsgRcvd, SemaphoreP_WAIT_FOREVER);

            preTimeStamp = TimerP_getTimeInUsecs();

                rtnVal = Cdd_IpcReceiveMsg(commId, &rxBuf[0U], &length);

            postTimeStamp = TimerP_getTimeInUsecs();

            Cdd_IpcProfileCumulativeRx +=
                        Cdd_IpcProfileGetTimeSpent(preTimeStamp, postTimeStamp);
        }
        Utils_prfLoadCalcStop ();

        if (E_OK == rtnVal)
        {
            AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME "\n");

#if defined (CDD_IPC_REMOTE_CORE_MCU2_1_USED)
            if (CddIpcConf_IpcComChanId_Cdd_IpcMcu21 == commId)
            {
                Cdd_IpcProfilePrintProfiledValues(itr, length,
                                        Cdd_IpcProfileCumulativeRxMcu21Isr);
            }
#endif /* CDD_IPC_REMOTE_CORE_MCU2_1_USED */

        }
    }

    return rtnVal;
}

void Cdd_IpcProfileCheckCtrlMsg(uint32 *pMcu21Flag)
{
#if (STD_ON == CDD_IPC_ANNOUNCE_API)
    uint8 rxBuf[40U];
    uint32 length, aRemoteProcId, aRemoteEndPt, numCh, rtnVal;
    const Cdd_IpcChannelType *pComChCfg;
    extern const Cdd_IpcConfigType CddIpcConfiguraions_PC;

    rtnVal = Cdd_IpcReceiveCtrlMsg(&aRemoteProcId,
                                    &aRemoteEndPt,
                                    &rxBuf[0U],
                                    &length);
    if (E_OK == rtnVal)
    {
        rtnVal = E_NOT_OK;
        for (numCh = 0U; numCh < CddIpcConfiguraions_PC.channelCount; numCh++)
        {
            pComChCfg = CddIpcConfiguraions_PC.pChCfg;
            if ((aRemoteProcId == pComChCfg[numCh].remoteProcId) &&
                (aRemoteEndPt == pComChCfg[numCh].remoteEp))
            {
                rtnVal = E_OK;
            }

#if defined (CDD_IPC_REMOTE_CORE_MCU2_1_USED)
            if ((CddIpcConf_IpcComChanId_Cdd_IpcMcu21 == pComChCfg[numCh].id) &&
                (E_OK == rtnVal))
            {
                /* Can add additional checks for message validity */
                *pMcu21Flag = 1U;
                AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
                    " Received %s as ctrl MSG from MCU 2 1\n",
                    &rxBuf[0U]);
                rtnVal = E_NOT_OK;
            }
#endif
        }
    }
#endif
    return;
}

void Cdd_IpcProfilePrintProfiledValues(uint32 itr, uint32 rxLength,
                                        uint64_t cumlativeRxIsrTime)
{
    Utils_PrfLoad       computedLoad;
    volatile uint64_t   avgRxTime, cumuRxTime;

    Utils_prfLoadGetTaskLoad(TaskP_self(), &computedLoad);

    AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
        "Transmitted Message of size %d bytes, %d times in %lld usecs\n",
        Cdd_IpcProfileTxBuf[itr].sizeInBytes,
        APP_NUM_MSG_PER_ITERATION, Cdd_IpcProfileCumulativeTx);

    AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
            "Average of %lld.%02d usecs per message \n",
            Cdd_IpcProfileCumulativeTx / APP_NUM_MSG_PER_ITERATION,
            Cdd_IpcProfileCumulativeTx % APP_NUM_MSG_PER_ITERATION);


    avgRxTime = (Cdd_IpcProfileCumulativeRx + cumlativeRxIsrTime) /
                                        APP_NUM_MSG_PER_ITERATION;

    /* The last message from remote core, would be for the next
        iteration */
    if (APP_NUM_ITERATION - 1U != itr)
    {
        cumuRxTime = Cdd_IpcProfileCumulativeRx - avgRxTime +
                                                        cumlativeRxIsrTime;
        AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
            "Received Message of size %d bytes, %d times in %lld "
            "usecs\n", rxLength, APP_NUM_MSG_PER_ITERATION,
            cumuRxTime);

        cumlativeRxIsrTime = avgRxTime;

    }
    else
    {
        cumuRxTime = (Cdd_IpcProfileCumulativeRx + cumlativeRxIsrTime);
        AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
            "Received Message of size %d bytes, %d times in %lld "
            "usecs\n", rxLength, APP_NUM_MSG_PER_ITERATION,
            cumuRxTime);

        cumlativeRxIsrTime = 0U;
    }

    AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
            "Average of %lld.%02d usecs per message \n",
            cumuRxTime / APP_NUM_MSG_PER_ITERATION,
            cumuRxTime % APP_NUM_MSG_PER_ITERATION);

    AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
                    "Measured Load: Total CPU: "
                    "%f%%, TSK: %f%%\r\n",
                    computedLoad.cpuLoad,
                    computedLoad.tskLoad);

    return;
}

uint64_t Cdd_IpcProfileGetTimeSpent(uint64_t preTs, uint64_t postTs)
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

void Cdd_IpcProfilePrepareTxBufs(void)
{
    uint32 idx;

    Cdd_IpcProfileTxBuf[0U].pBuf = Cdd_IpcProfileIter0TxData;
    Cdd_IpcProfileTxBuf[0U].sizeInBytes = APP_NUM_ITERATION_0_SIZE;

    Cdd_IpcProfileTxBuf[1U].pBuf = Cdd_IpcProfileIter1TxData;
    Cdd_IpcProfileTxBuf[1U].sizeInBytes = APP_NUM_ITERATION_1_SIZE;

    Cdd_IpcProfileTxBuf[2U].pBuf = Cdd_IpcProfileIter2TxData;
    Cdd_IpcProfileTxBuf[2U].sizeInBytes = APP_NUM_ITERATION_2_SIZE;

    for (idx = 0U; idx < APP_NUM_ITERATION_0_SIZE; idx++)
    {
        Cdd_IpcProfileIter0TxData[idx] = idx;
    }
    for (idx = 0U; idx < APP_NUM_ITERATION_1_SIZE; idx++)
    {
        Cdd_IpcProfileIter1TxData[idx] = idx + APP_NUM_ITERATION_0_SIZE;
    }
    for (idx = 0U; idx < APP_NUM_ITERATION_2_SIZE; idx++)
    {
        Cdd_IpcProfileIter2TxData[idx] = idx + APP_NUM_ITERATION_0_SIZE +
                                            APP_NUM_ITERATION_1_SIZE;
    }
}

#define CDD_IPC_START_SEC_ISR_CODE
#include "Cdd_Ipc_MemMap.h"

#if defined (CDD_IPC_REMOTE_CORE_MCU2_1_USED)
CDD_IPC_ISR_TEXT_SECTION FUNC(void, CDD_IPC_CODE_FAST)
                            Cdd_IpcProfile_MsgFromMcu21Isr(uintptr_t notUsed)
{
    volatile uint64_t   preTimeStamp, postTimeStamp;

    preTimeStamp = TimerP_getTimeInUsecs();

    Cdd_IpcIrqMbxFromMcu_21();

    postTimeStamp = TimerP_getTimeInUsecs();

    Cdd_IpcProfileCumulativeRxMcu21Isr +=
        Cdd_IpcProfileGetTimeSpent(preTimeStamp, postTimeStamp);
}

#endif /* CDD_IPC_REMOTE_CORE_MCU2_1_USED */


#define CDD_IPC_STOP_SEC_ISR_CODE
#include "Cdd_Ipc_MemMap.h"

/* EoF */
