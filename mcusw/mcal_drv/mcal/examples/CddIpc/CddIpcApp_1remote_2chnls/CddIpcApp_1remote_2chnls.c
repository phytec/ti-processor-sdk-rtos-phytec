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
 *  \file     CddIpcApp_1remote_2chnls.c
 *
 *  \brief    This file contains the demo application for CDD IPC
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "CddIpcCfg_1remote_2chnls.h"
#include "CddIpcApp_1remote_2chnls.h"
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
#include <ti/drv/sciclient/sciclient.h>
/* PM Lib */
#include <ti/drv/pm/include/pm_types.h>
#include <ti/drv/pm/include/dmsc/pmlib_sysconfig.h>
#include <ti/drv/pm/include/dmsc/pmlib_clkrate.h>

/* Interrupt Registrations */
#include <ti/osal/osal.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
#define MSG_NORMAL      (APP_UTILS_PRINT_MSG_NORMAL)
/**< Message type */
#define MSG_STATUS      (APP_UTILS_PRINT_MSG_STATUS)
/**< Message type */
#define CDD_IPC_APP_MSG_DATA_SIZE   (20U)
/**< Size of message buffer to be transmitted */
#define CDD_IPC_APP_MSG_TX_RX_CNT   (10U)
/**< Number of times the messages that has to be transmitted and received */
/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */
static void Cdd_IpcApp_PrintVersion(void);
#if (STD_OFF == CDD_IPC_ANNOUNCE_API)
static void Cdd_IpcApp_SendItrCntAndPingForAllCores(void);
#endif
#if (STD_ON == CDD_IPC_ANNOUNCE_API)
void Cdd_IpcApp_WaitCtrlSendCntPing(void);
#endif

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
uint32  Cdd_IpcAppDemoStatus = E_OK;
/**< Flag used for Demo status */
uint32 Cdd_IpcAppIterCntMcu20_1,Cdd_IpcAppIterCntMcu20_2;
/**< Counter to track number of messages to be transmitted & received */
volatile uint32 Cdd_IpcAppNmsgCntMcu20_1,Cdd_IpcAppNmsgCntMcu20_2;
/**< Counter to to indicate availability of messages from remote cores */

#if (STD_ON == CDD_IPC_ANNOUNCE_API)
volatile uint32 Cdd_IpcAppCtrlMsgMcu20_1, Cdd_IpcAppCtrlMsgMcu20_2;
/**< Flag to indicate which core has announced its availability */
#endif /* CDD_IPC_ANNOUNCE_API */

#ifdef CDD_IPC_REMOTE_CORE_MCU2_0_USED
static uint8 Cdd_IpcAppMcu20_1Buffer[CDD_IPC_APP_MSG_DATA_SIZE];
static uint8 Cdd_IpcAppMcu20_2Buffer[CDD_IPC_APP_MSG_DATA_SIZE];
/**< Buffer used to transmit and receive messages, to/from MCU 20 */
#endif /* CDD_IPC_REMOTE_CORE_MCU2_0_USED */


#if (STD_ON == CDD_IPC_ANNOUNCE_API)
static uint8 Cdd_IpcAppCtrlMsgBuffer[50U];
/**< Buffer used to receive control message from remote cores */
#endif

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */
/**
 * \brief CDD IPC New message available notification
 *
 *  Would be invoked by the driver on reception of message from remote core.
 *  Name of this function is configurable, please refer configurator.
 */
void Cdd_IpcNewMessageNotify(uint32 comId)
{

#ifdef CDD_IPC_REMOTE_CORE_MCU2_0_USED
    if (CddIpcConf_IpcComChanId_Cdd_IpcMcu20_1 == comId)
    {
        Cdd_IpcAppNmsgCntMcu20_1++;
    }

    if (CddIpcConf_IpcComChanId_Cdd_IpcMcu20_2 == comId)
    {
        Cdd_IpcAppNmsgCntMcu20_2++;
    }
#endif /* CDD_IPC_REMOTE_CORE_MCU2_0_USED */


    return;
}

#if (STD_ON == CDD_IPC_ANNOUNCE_API)
void Cdd_IpcNewCtrlMessageNotify(uint32 remoteProcId)
{
    uint32 idx, commId = 0U;
    const Cdd_IpcChannelType *pCommChCfg = CddIpcConfiguraions_PC.pChCfg;
    uint32 chanCount = CddIpcConfiguraions_PC.channelCount;

    for (idx = 0U; idx < chanCount; idx++)
    {
        if (remoteProcId == pCommChCfg[idx].remoteProcId)
        {
            commId = pCommChCfg[idx].id;
        }


      #ifdef CDD_IPC_REMOTE_CORE_MCU2_0_USED
          if (CddIpcConf_IpcComChanId_Cdd_IpcMcu20_1 == commId)
          {
              Cdd_IpcAppCtrlMsgMcu20_1 = 1U;
          }

          if (CddIpcConf_IpcComChanId_Cdd_IpcMcu20_2 == commId)
          {
              Cdd_IpcAppCtrlMsgMcu20_2 = 1U;
          }
      #endif

    }
}

#endif /* (STD_ON == CDD_IPC_ANNOUNCE_API) */

/** \brief Main Entry point for demo application
 *
 *  Application flow
 *      1. Register interrupt and other common steps
 *      2. Initialize IPC
 *      3. If Announcement is disabled
 *          Send a message "Ping x" to all cores, where x is iteration
 *      4. If announcement is enabled
 *          Wait for control message and then send "Ping x"
 *      5. Wait infinitely for "Ping x" from all remote cores
 *      6. Print the received message CAUTION Received message should be NULL
 *          terminated
 */
int main(void)
{
    uint32 length;
    volatile uint32 tempVar;
    uint32 mcu20_1Buflen,mcu20_2Buflen;
    const Cdd_IpcChannelType *pChanCfg = CddIpcConfiguraions_PC.pChCfg;
#if (STD_ON == CDD_IPC_ANNOUNCE_API)
    uint32 coreIdx, comId, rCoreId;
#endif

#ifdef CDD_IPC_REMOTE_CORE_MCU2_0_USED
    uint32 mcu20Bufsize = 7U;
#endif


#if (STD_ON == CDD_IPC_ANNOUNCE_API)
    const char announceMsg[32U] = "ti.ipc4.ping-pong";
#endif

    /* Step 1 */
    Cdd_IpcAppDemoStatus = E_NOT_OK;
#ifdef UART_ENABLED
    AppUtils_Init();
#endif
    AppUtils_LogResult(APP_UTILS_TEST_STATUS_INIT);

    Cdd_IpcApp_PrintVersion();

    Cdd_IpcApp_Startup();

    AppUtils_Printf(MSG_NORMAL, CDD_IPC_APP "\n");
    AppUtils_Printf(MSG_NORMAL, CDD_IPC_APP
                                        " Sample Application - STARTS !!! \n");

    Cdd_IpcAppIterCntMcu20_1 = CDD_IPC_APP_MSG_TX_RX_CNT;
    Cdd_IpcAppIterCntMcu20_2 = CDD_IPC_APP_MSG_TX_RX_CNT;

#ifndef CDD_IPC_REMOTE_CORE_MCU2_0_USED
    Cdd_IpcAppIterCntMcu20_1 = 0U;
    Cdd_IpcAppIterCntMcu20_2 = 0U;
#endif
    Cdd_IpcAppNmsgCntMcu20_1 = 0U;
    Cdd_IpcAppNmsgCntMcu20_2 = 0U;


#if (STD_ON == CDD_IPC_ANNOUNCE_API)
    Cdd_IpcAppCtrlMsgMcu20_1 = 0U;
    Cdd_IpcAppCtrlMsgMcu20_2 = 0U;
#endif /* CDD_IPC_ANNOUNCE_API */

    /* Step 2 */
    Cdd_IpcInit();

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
                Cdd_IpcAnnounce((const UCHAR *)announceMsg,
                                    CddIpcConfiguraions_PC.pChCfg[comId].id);
            }
        }
    }
#else
    /* Step 3 */
    Cdd_IpcApp_SendItrCntAndPingForAllCores();
#endif

    while (0U != Cdd_IpcAppIterCntMcu20_1)
    {

        /* Step 4 */
#if (STD_ON == CDD_IPC_ANNOUNCE_API)
        Cdd_IpcApp_WaitCtrlSendCntPing();
#endif

#ifdef CDD_IPC_REMOTE_CORE_MCU2_0_USED
        /* Step 5 */
        tempVar = Cdd_IpcAppNmsgCntMcu20_1;
        while ((0U != tempVar) && (0U != Cdd_IpcAppIterCntMcu20_1))
        {
            Cdd_IpcReceiveMsg(CddIpcConf_IpcComChanId_Cdd_IpcMcu20_1,
                                &Cdd_IpcAppMcu20_1Buffer[0U], &mcu20_1Buflen);

            SchM_Enter_Cdd_Ipc_IPC_EXCLUSIVE_AREA_0();
                tempVar--;
                Cdd_IpcAppNmsgCntMcu20_1--;
            SchM_Exit_Cdd_Ipc_IPC_EXCLUSIVE_AREA_0();

            AppUtils_Printf(MSG_NORMAL, CDD_IPC_APP
                        " Received %s Iteration %d from MCU 2 0 channel-1 , , EP[%d], ProcId[%d] \n",
                        &Cdd_IpcAppMcu20_1Buffer[0U], Cdd_IpcAppIterCntMcu20_1,
                              pChanCfg[CddIpcConf_IpcComChanId_Cdd_IpcMcu20_1].remoteEp, \
                              pChanCfg[CddIpcConf_IpcComChanId_Cdd_IpcMcu20_1].remoteProcId);

            Cdd_IpcAppIterCntMcu20_1--;
            length = snprintf((char *)Cdd_IpcAppMcu20_1Buffer, 7, "ping %d",
                                Cdd_IpcAppIterCntMcu20_1);
            Cdd_IpcAppMcu20_1Buffer[length++] = '\0';
            Cdd_IpcSendMsg(CddIpcConf_IpcComChanId_Cdd_IpcMcu20_1,
                            &Cdd_IpcAppMcu20_1Buffer[0U], length);
        }
    }
     while (0U != Cdd_IpcAppIterCntMcu20_2)
    {
                /* Step 4 */
#if (STD_ON == CDD_IPC_ANNOUNCE_API)
        Cdd_IpcApp_WaitCtrlSendCntPing();
#endif

        tempVar = Cdd_IpcAppNmsgCntMcu20_2;
        while ((0U != tempVar) && (0U != Cdd_IpcAppIterCntMcu20_2))
        {
            Cdd_IpcReceiveMsg(CddIpcConf_IpcComChanId_Cdd_IpcMcu20_2,
                                &Cdd_IpcAppMcu20_2Buffer[0U], &mcu20_2Buflen);

            SchM_Enter_Cdd_Ipc_IPC_EXCLUSIVE_AREA_0();
                tempVar--;
                Cdd_IpcAppNmsgCntMcu20_2--;
            SchM_Exit_Cdd_Ipc_IPC_EXCLUSIVE_AREA_0();

            AppUtils_Printf(MSG_NORMAL, CDD_IPC_APP
                        " Received %s Iteration %d from MCU 2 0 channel-2, , EP[%d], ProcId[%d] \n",
                        &Cdd_IpcAppMcu20_2Buffer[0U], Cdd_IpcAppIterCntMcu20_2,
                              pChanCfg[CddIpcConf_IpcComChanId_Cdd_IpcMcu20_2].remoteEp, \
                              pChanCfg[CddIpcConf_IpcComChanId_Cdd_IpcMcu20_2].remoteProcId);

            Cdd_IpcAppIterCntMcu20_2--;
            length = snprintf((char *)Cdd_IpcAppMcu20_2Buffer, 7, "ping %d",
                                Cdd_IpcAppIterCntMcu20_2);
            Cdd_IpcAppMcu20_2Buffer[length++] = '\0';
            Cdd_IpcSendMsg(CddIpcConf_IpcComChanId_Cdd_IpcMcu20_2,
                            &Cdd_IpcAppMcu20_2Buffer[0U], length);
        }
#endif /* CDD_IPC_REMOTE_CORE_MCU2_0_USED */


        /* Step 5 */

    }

    AppUtils_Printf(MSG_NORMAL, CDD_IPC_APP
                                    " Transmitted and Received %d times\n",
                                    CDD_IPC_APP_MSG_TX_RX_CNT);
    AppUtils_Printf(MSG_NORMAL, CDD_IPC_APP
                                    " Sample Application - Completes !!!\n");

    Cdd_IpcAppDemoStatus = E_OK;
    return (0);
}

/* ========================================================================== */
/*                        Application Local Functions                         */
/* ========================================================================== */
#if (STD_OFF == CDD_IPC_ANNOUNCE_API)
void Cdd_IpcApp_SendItrCntAndPingForAllCores(void)
{
    uint32 length, *numIterationCnt;

#ifdef CDD_IPC_REMOTE_CORE_MCU2_0_USED
    /* Send the number of iterations, of transmission and reception */
    numIterationCnt = (uint32 *)Cdd_IpcAppMcu20_1Buffer;
    *numIterationCnt = Cdd_IpcAppIterCntMcu20_1;
    length = 4U;
    Cdd_IpcSendMsg(CddIpcConf_IpcComChanId_Cdd_IpcMcu20_1,
                    &Cdd_IpcAppMcu20_1Buffer[0U], length);

    /* First Ping */
    length = snprintf((char *)Cdd_IpcAppMcu20_1Buffer, 7, "ping %d",
                        Cdd_IpcAppIterCntMcu20_1);
    Cdd_IpcAppMcu20_1Buffer[length++] = '\0';
    Cdd_IpcSendMsg(CddIpcConf_IpcComChanId_Cdd_IpcMcu20_1,
                    &Cdd_IpcAppMcu20_1Buffer[0U], length);

    /* Send the number of iterations, of transmission and reception */
    numIterationCnt = (uint32 *)Cdd_IpcAppMcu20_2Buffer;
    *numIterationCnt = Cdd_IpcAppIterCntMcu20_2;
    length = 4U;
    Cdd_IpcSendMsg(CddIpcConf_IpcComChanId_Cdd_IpcMcu20_2,
                    &Cdd_IpcAppMcu20_2Buffer[0U], length);

    /* First Ping */
    length = snprintf((char *)Cdd_IpcAppMcu20_2Buffer, 7, "ping %d",
                        Cdd_IpcAppIterCntMcu20_2);
    Cdd_IpcAppMcu20_2Buffer[length++] = '\0';
    Cdd_IpcSendMsg(CddIpcConf_IpcComChanId_Cdd_IpcMcu20_2,
                    &Cdd_IpcAppMcu20_2Buffer[0U], length);
#endif /* CDD_IPC_REMOTE_CORE_MCU2_0_USED */


    return;
}
#endif /*#if (STD_OFF == CDD_IPC_ANNOUNCE_API)*/

#if (STD_ON == CDD_IPC_ANNOUNCE_API)
void Cdd_IpcApp_WaitCtrlSendCntPing(void)
{
    Std_ReturnType rtnVal = E_NOT_OK;
    /**< Buffer used to transmit and receive messages, to/from MCU 20 */
    uint32 wRemoteProcId, wRemoteEndPt;
    uint32 length, *numIterationCnt;

    rtnVal = Cdd_IpcReceiveCtrlMsg(&wRemoteProcId,
                                    &wRemoteEndPt,
                                    Cdd_IpcAppCtrlMsgBuffer,
                                    &length);

#ifdef CDD_IPC_REMOTE_CORE_MCU2_0_USED
    if (((Std_ReturnType)E_OK == rtnVal) && (0U != Cdd_IpcAppCtrlMsgMcu20_1))
    {
        /* One could add checks to confirm the control message here */
        AppUtils_Printf(MSG_NORMAL, CDD_IPC_APP
                    " Received %s as ctrl MSG from MCU 2 0 channel-1\n",
                    &Cdd_IpcAppCtrlMsgBuffer[0U]);

        /* Send the number of iterations, of transmission and reception */
        numIterationCnt = (uint32 *)Cdd_IpcAppMcu20_1Buffer;
        *numIterationCnt = Cdd_IpcAppIterCntMcu20_1;
        length = 4U;
        Cdd_IpcSendMsg(CddIpcConf_IpcComChanId_Cdd_IpcMcu20_1,
                        &Cdd_IpcAppMcu20_1Buffer[0U], length);

        /* First Ping */
        length = snprintf((char *)Cdd_IpcAppMcu20_1Buffer, 8, "ping %d",
                            Cdd_IpcAppIterCntMcu20_1);
        Cdd_IpcAppMcu20_1Buffer[length++] = '\0';
        Cdd_IpcSendMsg(CddIpcConf_IpcComChanId_Cdd_IpcMcu20_1,
                        &Cdd_IpcAppMcu20_1Buffer[0U], length);

        /* This variable is shared between ISR and app, care should be taken
            to ensure, ISR or interrutps are disabled while updating this
            variable */
        Cdd_IpcAppCtrlMsgMcu20_1 = 0U;
    }


    if (((Std_ReturnType)E_OK == rtnVal) && (0U != Cdd_IpcAppCtrlMsgMcu20_2))
    {
        /* One could add checks to confirm the control message here */
        AppUtils_Printf(MSG_NORMAL, CDD_IPC_APP
                    " Received %s as ctrl MSG from MCU 2 0 channel-2\n",
                    &Cdd_IpcAppCtrlMsgBuffer[0U]);

        /* Send the number of iterations, of transmission and reception */
        numIterationCnt = (uint32 *)Cdd_IpcAppMcu20_2Buffer;
        *numIterationCnt = Cdd_IpcAppIterCntMcu20_2;
        length = 4U;
        Cdd_IpcSendMsg(CddIpcConf_IpcComChanId_Cdd_IpcMcu20_2,
                        &Cdd_IpcAppMcu20_2Buffer[0U], length);

        /* First Ping */
        length = snprintf((char *)Cdd_IpcAppMcu20_2Buffer, 8, "ping %d",
                            Cdd_IpcAppIterCntMcu20_2);
        Cdd_IpcAppMcu20_2Buffer[length++] = '\0';
        Cdd_IpcSendMsg(CddIpcConf_IpcComChanId_Cdd_IpcMcu20_2,
                        &Cdd_IpcAppMcu20_2Buffer[0U], length);

        /* This variable is shared between ISR and app, care should be taken
            to ensure, ISR or interrutps are disabled while updating this
            variable */
        Cdd_IpcAppCtrlMsgMcu20_2 = 0U;
    }
#endif /* CDD_IPC_REMOTE_CORE_MCU2_0_USED */



    return;
}
#endif

/** \brief prints of the version of this implementation */
static void Cdd_IpcApp_PrintVersion(void)
{
#if (STD_ON == CDD_IPC_VERSION_INFO_API)
    Std_VersionInfoType versioninfo;

    Cdd_IpcGetVersionInfo(&versioninfo);
    AppUtils_Printf(MSG_NORMAL, " \n");
    AppUtils_Printf(MSG_NORMAL, CDD_IPC_APP " CDD IPC MCAL Version Info\n");
    AppUtils_Printf(MSG_NORMAL, CDD_IPC_APP "---------------------\n");
    AppUtils_Printf(MSG_NORMAL, CDD_IPC_APP " Vendor ID           : %d\n",
                                                versioninfo.vendorID);
    AppUtils_Printf(MSG_NORMAL, CDD_IPC_APP " Module ID           : %d\n",
                                                versioninfo.moduleID);
    AppUtils_Printf(MSG_NORMAL, CDD_IPC_APP " SW Major Version    : %d\n",
                                                versioninfo.sw_major_version);
    AppUtils_Printf(MSG_NORMAL, CDD_IPC_APP " SW Minor Version    : %d\n",
                                                versioninfo.sw_minor_version);
    AppUtils_Printf(MSG_NORMAL, CDD_IPC_APP " SW Patch Version    : %d\n",
                                                versioninfo.sw_patch_version);
    AppUtils_Printf(MSG_NORMAL, " \n");
#endif

}

/*EoF*/
