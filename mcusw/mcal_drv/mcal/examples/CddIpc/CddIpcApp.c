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
 *  \file     CddIpcApp.c
 *
 *  \brief    This file contains the demo application for CDD IPC
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "CddIpcApp.h"

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

#if defined(SOC_AM62X)
#include "ti_drivers_config.h"
#include "ti_board_config.h"
#include <drivers/sciclient/include/sciclient_pm.h>
#include <kernel/dpl/DebugP.h>
#include <stdio.h>
#include <drivers/soc.h>
#include <kernel/dpl/AddrTranslateP.h>
#include "ti_dpl_config.h"
#endif

/*Update conditional check to #ifdef J7SOC's while validating for Jacinto*/
#if !defined(SOC_AM62X) &&  !defined(SOC_AM62AX)
/* SCI Client */
#include <ti/drv/sciclient/sciclient.h>
/* PM Lib */
#include <ti/drv/pm/include/pm_types.h>
#include <ti/drv/pm/include/dmsc/pmlib_sysconfig.h>
#include <ti/drv/pm/include/dmsc/pmlib_clkrate.h>
/* Interrupt Registrations */
#include <ti/osal/osal.h>
#endif

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
#if !defined(SOC_AM62X)
uint32 Cdd_IpcAppIterCntMcu20, Cdd_IpcAppIterCntMcu21;
#endif
#if defined(SOC_AM62X)
uint32 Cdd_IpcAppIterCntM4FSS0_0, Cdd_IpcAppIterCntR5FSS0_0;
#endif
/**< Counter to track number of messages to be transmitted & received */
#if !defined(SOC_AM62X)
volatile uint32 Cdd_IpcAppNmsgCntMcu20, Cdd_IpcAppNmsgCntMcu21;
#endif
#if defined(SOC_AM62X)
volatile uint32 Cdd_IpcAppNmsgCntM4FSS0_0, Cdd_IpcAppNmsgCntR5FSS0_0;
#endif
/**< Counter to to indicate availability of messages from remote cores */

#if (STD_ON == CDD_IPC_ANNOUNCE_API)
#if !defined(SOC_AM62X)
volatile uint32 Cdd_IpcAppCtrlMsgMcu20, Cdd_IpcAppCtrlMsgMcu21;
#endif
#if defined(SOC_AM62X)
volatile uint32 Cdd_IpcAppCtrlMsgM4FSS0_0, Cdd_IpcAppCtrlMsgR5FSS0_0;
#endif
/**< Flag to indicate which core has announced its availability */
#endif /* CDD_IPC_ANNOUNCE_API */

#if !defined(SOC_AM62X)
#ifdef CDD_IPC_REMOTE_CORE_MCU2_0_USED
static uint8 Cdd_IpcAppMcu20Buffer[CDD_IPC_APP_MSG_DATA_SIZE];
/**< Buffer used to transmit and receive messages, to/from MCU 20 */
#endif /* CDD_IPC_REMOTE_CORE_MCU2_0_USED */
#endif
#if defined(SOC_AM62X)
#ifdef CDD_IPC_REMOTE_CORE_M4FSS0_0_USED
static uint8 Cdd_IpcAppM4FSS0_0Buffer[CDD_IPC_APP_MSG_DATA_SIZE];
/**< Buffer used to transmit and receive messages, to/from M4FSS0_0 */
#endif /* CDD_IPC_REMOTE_CORE_M4FSS0_0_USED */
#endif

#if !defined(SOC_AM62X)
#ifdef CDD_IPC_REMOTE_CORE_MCU2_1_USED
static uint8 Cdd_IpcAppMcu21Buffer[CDD_IPC_APP_MSG_DATA_SIZE];
/**< Buffer used to transmit and receive messages, to/from MCU 21 */
#endif /* CDD_IPC_REMOTE_CORE_MCU2_1_USED */
#endif

#if defined(SOC_AM62X)
#ifdef CDD_IPC_REMOTE_CORE_R5FSS0_0_USED
static uint8 Cdd_IpcAppR5FSS0_0Buffer[CDD_IPC_APP_MSG_DATA_SIZE];
/**< Buffer used to transmit and receive messages, to/from R5FSS0_0 */
#endif /* CDD_IPC_REMOTE_CORE_R5FSS0_0_USED */
#endif


#if (STD_ON == CDD_IPC_ANNOUNCE_API)
static uint8 Cdd_IpcAppCtrlMsgBuffer[50U];
/**< Buffer used to receive control message from remote cores */
#endif

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
    uint32 testPassed = E_OK;
#if !defined(SOC_AM62X) &&  !defined(AM62AX)
#if (STD_ON == CDD_IPC_SAFETY_DIAGNOSTIC_API)
    uint32 mboxstatus=0;
#endif  /*(STD_ON == CDD_IPC_SAFETY_DIAGNOSTIC_API)*/
    uint32 mcu20Buflen = 0;
#ifdef CDD_IPC_REMOTE_CORE_MCU2_1_USED	
	uint32 mcu21Buflen = 0;
#endif
#endif
#if (STD_ON == CDD_IPC_ANNOUNCE_API)
    uint32 coreIdx, comId, rCoreId;
#endif


#ifdef CDD_IPC_REMOTE_CORE_MCU2_1_USED
    uint32 mcu21Bufsize = 7U;
#endif

#if defined(SOC_AM62X)
#ifdef CDD_IPC_REMOTE_CORE_M4FSS0_0_USED
    uint32 M4FSS0_0Bufsize = 7U;
#endif

#ifdef CDD_IPC_REMOTE_CORE_R5FSS0_0_USED
    uint32 R5FSS0_0Bufsize = 7U;
#endif
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

#if !defined(SOC_AM62X) &&  !defined(SOC_AM62AX)
    Cdd_IpcAppIterCntMcu20 = CDD_IPC_APP_MSG_TX_RX_CNT;
    Cdd_IpcAppIterCntMcu21 = CDD_IPC_APP_MSG_TX_RX_CNT;
#endif

#if defined(SOC_AM62X)
    Cdd_IpcAppIterCntM4FSS0_0 = CDD_IPC_APP_MSG_TX_RX_CNT;
#endif

#if !defined(SOC_AM62X)
#ifndef CDD_IPC_REMOTE_CORE_MCU2_0_USED
    Cdd_IpcAppIterCntMcu20 = 0U;
#endif
    Cdd_IpcAppNmsgCntMcu20 = 0U;

#ifndef CDD_IPC_REMOTE_CORE_MCU2_1_USED
    Cdd_IpcAppIterCntMcu21 = 0U;
#endif
    Cdd_IpcAppNmsgCntMcu21 = 0U;

#if (STD_ON == CDD_IPC_ANNOUNCE_API)
    Cdd_IpcAppCtrlMsgMcu20 = 0U;
    Cdd_IpcAppCtrlMsgMcu21 = 0U;
#endif /* CDD_IPC_ANNOUNCE_API */
#endif

#if defined(SOC_AM62X)
#ifndef CDD_IPC_REMOTE_CORE_M4FSS0_0_USED
    Cdd_IpcAppIterCntM4FSS0_0 = 0U;
#endif
    Cdd_IpcAppNmsgCntM4FSS0_0 = 0U;

#ifndef CDD_IPC_REMOTE_CORE_R5FSS0_0_USED
    Cdd_IpcAppIterCntR5FSS0_0 = 0U;
#endif
    Cdd_IpcAppNmsgCntR5FSS0_0 = 0U;

#if (STD_ON == CDD_IPC_ANNOUNCE_API)
    Cdd_IpcAppCtrlMsgM4FSS0_0 = 0U;
    Cdd_IpcAppCtrlMsgR5FSS0_0 = 0U;
#endif /* CDD_IPC_ANNOUNCE_API */
#endif

    if (testPassed == E_OK)
    {
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
#if !defined(SOC_AM62X)
        while (((0U != Cdd_IpcAppIterCntMcu20)) || (0U != Cdd_IpcAppIterCntMcu21))
#endif
#if defined(SOC_AM62X)
    while (0U != Cdd_IpcAppIterCntM4FSS0_0)
#endif
        {

            /* Step 4 */
    #if (STD_ON == CDD_IPC_ANNOUNCE_API)
            Cdd_IpcApp_WaitCtrlSendCntPing();
    #endif
/*Below section unclear #if (STD_ON == CDD_IPC_SAFETY_DIAGNOSTIC_API)*/
    #if (STD_ON == CDD_IPC_SAFETY_DIAGNOSTIC_API)
        #ifdef CDD_IPC_REMOTE_CORE_MCU2_0_USED

                mboxstatus = Cdd_IpcGetMailboxStatus((uint32) CDD_IPC_CORE_MCU2_0);
                if (mboxstatus != 0 )
                {
                  AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
                          CDD_IPC_APP ": Mailbox status is FULL from remote=%d,but ISR not triggered: ERROR\n",CDD_IPC_CORE_MCU2_0);
                  testPassed = E_NOT_OK;
                  break;
                }
        #endif
        #ifdef CDD_IPC_REMOTE_CORE_MCU2_1_USED

                mboxstatus = Cdd_IpcGetMailboxStatus((uint32) CDD_IPC_CORE_MCU2_1);
                if (mboxstatus != 0 )
                {
                  AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
                          CDD_IPC_APP ": Mailbox status is FULL from remote=%d,but ISR not triggered: ERROR\n",CDD_IPC_CORE_MCU2_1);
                  testPassed = E_NOT_OK;
                  break;
                }
        #endif
    #endif

#if !defined(SOC_AM62X) &&  !defined(AM62AX)
    #ifdef CDD_IPC_REMOTE_CORE_MCU2_0_USED
            /* Step 5 */

            tempVar = Cdd_IpcAppNmsgCntMcu20;
            while ((0U != tempVar) && (0U != Cdd_IpcAppIterCntMcu20))
            {
                Cdd_IpcReceiveMsg(CddIpcConf_IpcComChanId_Cdd_IpcMcu20,
                                    &Cdd_IpcAppMcu20Buffer[0U], &mcu20Buflen);

                SchM_Enter_Cdd_Ipc_IPC_EXCLUSIVE_AREA_0();
                    tempVar--;
                    Cdd_IpcAppNmsgCntMcu20--;
                SchM_Exit_Cdd_Ipc_IPC_EXCLUSIVE_AREA_0();

                AppUtils_Printf(MSG_NORMAL, CDD_IPC_APP
                            " Received %s Iteration %d from MCU 2 0\n",
                            &Cdd_IpcAppMcu20Buffer[0U], Cdd_IpcAppIterCntMcu20);

                Cdd_IpcAppIterCntMcu20--;
                length = snprintf((char *)Cdd_IpcAppMcu20Buffer, 7, "ping %d",
                                    Cdd_IpcAppIterCntMcu20);
                Cdd_IpcAppMcu20Buffer[length++] = '\0';
                Cdd_IpcSendMsg(CddIpcConf_IpcComChanId_Cdd_IpcMcu20,
                                &Cdd_IpcAppMcu20Buffer[0U], length);

            }
    #endif /* CDD_IPC_REMOTE_CORE_MCU2_0_USED */

    #ifdef CDD_IPC_REMOTE_CORE_MCU2_1_USED
            /* Step 5 */
            tempVar = Cdd_IpcAppNmsgCntMcu21;
            while ((0U != tempVar) && (0U != Cdd_IpcAppIterCntMcu21))
            {
                Cdd_IpcReceiveMsg(CddIpcConf_IpcComChanId_Cdd_IpcMcu21,
                                    &Cdd_IpcAppMcu21Buffer[0U], &mcu21Buflen);
                SchM_Enter_Cdd_Ipc_IPC_EXCLUSIVE_AREA_0();
                    tempVar--;
                    Cdd_IpcAppNmsgCntMcu21--;
                SchM_Exit_Cdd_Ipc_IPC_EXCLUSIVE_AREA_0();

                AppUtils_Printf(MSG_NORMAL, CDD_IPC_APP
                            " Received %s Iteration %d from MCU 2 1\n",
                            &Cdd_IpcAppMcu21Buffer[0U], Cdd_IpcAppIterCntMcu21);

                Cdd_IpcAppIterCntMcu21--;
                length = snprintf((char *)Cdd_IpcAppMcu21Buffer, 7, "ping %d",
                                    Cdd_IpcAppIterCntMcu21);
                Cdd_IpcAppMcu21Buffer[length++] = '\0';
                Cdd_IpcSendMsg(CddIpcConf_IpcComChanId_Cdd_IpcMcu21,
                                &Cdd_IpcAppMcu21Buffer[0U], length);

            }
    #endif /* CDD_IPC_REMOTE_CORE_MCU2_1_USED */
#endif

#if defined(SOC_AM62X)
#ifdef CDD_IPC_REMOTE_CORE_M4FSS0_0_USED
        /* Step 5 */
        tempVar = Cdd_IpcAppNmsgCntM4FSS0_0;
        while ((0U != tempVar) && (0U != Cdd_IpcAppIterCntM4FSS0_0))
        {
            Cdd_IpcReceiveMsg(CddIpcConf_IpcComChanId_CDD_IPC_CORE_M4FSS0_0 ,
                                &Cdd_IpcAppM4FSS0_0Buffer[0U], &M4FSS0_0Bufsize);
            DebugP_log("inside loop for M4FSS0_0 remote core rec msg");

            SchM_Enter_Cdd_Ipc_IPC_EXCLUSIVE_AREA_0();
                tempVar--;
                Cdd_IpcAppNmsgCntM4FSS0_0--;
            SchM_Exit_Cdd_Ipc_IPC_EXCLUSIVE_AREA_0();

            AppUtils_Printf(MSG_NORMAL, CDD_IPC_APP
                        " Received %s Iteration %d from M4FSS0_0\n",
                        &Cdd_IpcAppM4FSS0_0Buffer[0U], Cdd_IpcAppIterCntM4FSS0_0);         
			

            Cdd_IpcAppIterCntM4FSS0_0--;
            length = snprintf((char *)Cdd_IpcAppM4FSS0_0Buffer, 7, "ping %d",
                                Cdd_IpcAppIterCntM4FSS0_0);
            Cdd_IpcAppM4FSS0_0Buffer[length++] = '\0';
            Cdd_IpcSendMsg(CddIpcConf_IpcComChanId_CDD_IPC_CORE_M4FSS0_0 ,
                            &Cdd_IpcAppM4FSS0_0Buffer[0U], length);
            M4FSS0_0Bufsize = length;
        }
#endif /* CDD_IPC_REMOTE_CORE_M4FSS0_0_USED */
#endif
            /* Step 5 */

        }
      }

#if !defined(SOC_AM62X)
/*CDD_IPC_REGISTER_READBACK_API*/
#if (STD_ON == CDD_IPC_REGISTER_READBACK_API)
/* For every remote core, Announce once */
for (coreIdx = 0U; coreIdx < CddIpcConfiguraions_PC.coreIds.numProcs;
     coreIdx++)
{
    rCoreId = CddIpcConfiguraions_PC.coreIds.remoteProcID[coreIdx];
    for (comId = 0U; comId < CddIpcConfiguraions_PC.channelCount; comId++)
    {
        if (rCoreId == CddIpcConfiguraions_PC.pChCfg[comId].remoteProcId)
        {
             Cdd_IpcRegRbValues pRegArgs;
             Std_ReturnType retVal = (Std_ReturnType) E_OK;
             retVal = Cdd_IpcRegisterReadBack(rCoreId , &pRegArgs);
             AppUtils_Printf(MSG_NORMAL, " \n");
             AppUtils_Printf(MSG_NORMAL, CDD_IPC_APP
                                "------------------------------------------\n");
             if (retVal != E_NOT_OK)
               {
                  AppUtils_Printf(MSG_NORMAL, CDD_IPC_APP
                                  ": IPC Channel %d configuration register values  \n", (rCoreId));
                  AppUtils_Printf(MSG_NORMAL, CDD_IPC_APP " MAILBOX_REVISION         : 0x%x\n",
                                  pRegArgs.cddipcRev);
                  AppUtils_Printf(MSG_NORMAL, CDD_IPC_APP " MAILBOX_SYSCONFIG        : 0x%x\n",
                                  pRegArgs.cddipcsyscfg);
                  AppUtils_Printf(MSG_NORMAL, CDD_IPC_APP " MAILBOX_IRQ_EOI          : 0x%x\n",
                                  pRegArgs.cddipceoi);
                  AppUtils_Printf(MSG_NORMAL, CDD_IPC_APP " MAILBOX_MESSAGE          : 0x%x\n",
                                  pRegArgs.cddipcmsg);
                  AppUtils_Printf(MSG_NORMAL, CDD_IPC_APP " MAILBOX_FIFOSTATUS       : 0x%x\n",
                                  pRegArgs.cddipcfifosts);
                  AppUtils_Printf(MSG_NORMAL, CDD_IPC_APP " MAILBOX_MSGSTATUS        : 0x%x\n",
                                  pRegArgs.cddipcmsgsts);
                  AppUtils_Printf(MSG_NORMAL, CDD_IPC_APP " MAILBOX_IRQSTATUS_RAW    : 0x%x\n",
                                  pRegArgs.cddipcirqrawsts);
                  AppUtils_Printf(MSG_NORMAL, CDD_IPC_APP " MAILBOX_IRQSTATUS_CLR    : 0x%x\n",
                                  pRegArgs.cddipcclrsts);
                  AppUtils_Printf(MSG_NORMAL, CDD_IPC_APP " MAILBOX_IRQENABLE_CLR    : 0x%x\n",
                                  pRegArgs.cddipcenbclr);
                  AppUtils_Printf(MSG_NORMAL, CDD_IPC_APP " MAILBOX_IRQENABLE_SET    : 0x%x\n",
                                  pRegArgs.cddipcenbset);
                  testPassed = E_OK;
                 }
             else
                {
                  AppUtils_Printf(MSG_NORMAL, CDD_IPC_APP
                          "IPC Register readback Failed  \n");
                  testPassed = E_NOT_OK;
                }
        }
    }
}

#endif
#endif

    if (E_OK == testPassed)
    {

        AppUtils_Printf(MSG_NORMAL, CDD_IPC_APP
                                        " Transmitted and Received %d times\n",
                                        CDD_IPC_APP_MSG_TX_RX_CNT);
        AppUtils_Printf(MSG_NORMAL, CDD_IPC_APP
                                        " All tests have passed !!!\n");
    }
    else{
        AppUtils_Printf(MSG_NORMAL, CDD_IPC_APP
                                      " Some tests have Failed!!!\n");
    }

    Cdd_IpcAppDemoStatus = E_OK;
    return (0);
}

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 * \brief CDD IPC New message available notification
 *
 *  Would be invoked by the driver on reception of message from remote core.
 *  Name of this function is configurable, please refer configurator.
 */
void Cdd_IpcNewMessageNotify(void)
{
#if defined(SOC_AM62X)
#ifdef CDD_IPC_REMOTE_CORE_M4FSS0_0_USED
    Cdd_IpcAppNmsgCntM4FSS0_0++;
#endif /* CDD_IPC_REMOTE_CORE_M4FSS0_0_USED */
#endif

#if !defined(SOC_AM62X) &&  !defined(AM62AX)
#ifdef CDD_IPC_REMOTE_CORE_MCU2_0_USED
    Cdd_IpcAppNmsgCntMcu20++;
#endif /* CDD_IPC_REMOTE_CORE_MCU2_0_USED */
#endif

    return;
}

#if (STD_ON == CDD_IPC_ANNOUNCE_API)

void Cdd_IpcNewCtrlMessageNotify(uint32 remoteProcId)
{
    /* Variable to store maximum number of remote processors 
     * configured for communication 
     */
    uint32 MaxRemoteProcs = 0;
    
    /* Loop Counter */
    uint32 i = 0;

    /* Updating the remote processors from the configurations */
    MaxRemoteProcs = CddIpcConfiguraions_PC.coreIds.numProcs;

    /* Loop to iterate through all configured remote processors */
    for(i = 0; i<MaxRemoteProcs; i++){
        /* Check if remote processor passed as argument is enabled
         * as a remote core in the configuration
         */
        if(remoteProcId == CddIpcConfiguraions_PC.coreIds.remoteProcID[i]){

            /* If remote processor matches the configuration
             * increment the control message process variable 
             * and process the actual control message in the 
             * application using the Cdd_IpcReceiveCtrlMsg API
             */
            #if !defined(SOC_AM62X) && !defined(SOC_AM62AX)
            
            #ifdef CDD_IPC_REMOTE_CORE_MCU2_0_USED
            Cdd_IpcAppCtrlMsgMcu20++;
            #endif /* CDD_IPC_REMOTE_CORE_MCU2_0_USED */
            
            #ifdef CDD_IPC_REMOTE_CORE_MCU2_1_USED
            Cdd_IpcAppCtrlMsgMcu21++;
            #endif /* CDD_IPC_REMOTE_CORE_MCU2_1_USED */

            #endif /* !defined(SOC_AM62X) && !defined(SOC_AM62AX) */

            #if defined(SOC_AM62X)
            
            #ifdef CDD_IPC_REMOTE_CORE_M4FSS0_0_USED
            Cdd_IpcAppCtrlMsgM4FSS0_0++;
            #endif /* CDD_IPC_REMOTE_CORE_M4FSS0_0_USED */
            
            #endif /* defined(SOC_AM62X) */
            
            break;
        }
    }
    return;

}

#endif /* (STD_ON == CDD_IPC_ANNOUNCE_API) */

/* ========================================================================== */
/*                        Application Local Functions                         */
/* ========================================================================== */
#if (STD_OFF == CDD_IPC_ANNOUNCE_API)
void Cdd_IpcApp_SendItrCntAndPingForAllCores(void)
{
    uint32 length, *numIterationCnt;

#if !defined(SOC_AM62X)
#ifdef CDD_IPC_REMOTE_CORE_MCU2_0_USED
    /* Send the number of iterations, of transmission and reception */
    numIterationCnt = (uint32 *)Cdd_IpcAppMcu20Buffer;
    *numIterationCnt = Cdd_IpcAppIterCntMcu20;
    length = 4U;
    Cdd_IpcSendMsg(CddIpcConf_IpcComChanId_Cdd_IpcMcu20,
                    &Cdd_IpcAppMcu20Buffer[0U], length);

    /* First Ping */
    length = snprintf((char *)Cdd_IpcAppMcu20Buffer, 7, "ping %d",
                        Cdd_IpcAppIterCntMcu20);
    Cdd_IpcAppMcu20Buffer[length++] = '\0';
    Cdd_IpcSendMsg(CddIpcConf_IpcComChanId_Cdd_IpcMcu20,
                    &Cdd_IpcAppMcu20Buffer[0U], length);
#endif /* CDD_IPC_REMOTE_CORE_MCU2_0_USED */

#ifdef CDD_IPC_REMOTE_CORE_MCU2_1_USED
    /* Send the number of iterations, of transmission and reception */
    numIterationCnt = (uint32 *)Cdd_IpcAppMcu21Buffer;
    *numIterationCnt = Cdd_IpcAppIterCntMcu21;
    length = 4U;
    Cdd_IpcSendMsg(CddIpcConf_IpcComChanId_Cdd_IpcMcu21,
                    &Cdd_IpcAppMcu21Buffer[0U], length);

    /* First Ping */
    length = snprintf((char *)Cdd_IpcAppMcu21Buffer, 7, "ping %d",
                        Cdd_IpcAppIterCntMcu21);
    Cdd_IpcAppMcu21Buffer[length++] = '\0';
    Cdd_IpcSendMsg(CddIpcConf_IpcComChanId_Cdd_IpcMcu21,
                    &Cdd_IpcAppMcu21Buffer[0U], length);
#endif /* CDD_IPC_REMOTE_CORE_MCU2_1_USED */
#endif

#if defined(SOC_AM62X)
#ifdef CDD_IPC_REMOTE_CORE_M4FSS0_0_USED
    /* Send the number of iterations, of transmission and reception */
    numIterationCnt = (uint32 *)Cdd_IpcAppM4FSS0_0Buffer;
    *numIterationCnt = Cdd_IpcAppIterCntM4FSS0_0;
    length = 4U;
    Cdd_IpcSendMsg(CddIpcConf_IpcComChanId_CDD_IPC_CORE_M4FSS0_0 ,
                    &Cdd_IpcAppM4FSS0_0Buffer[0U], length);

    /* First Ping */
    length = snprintf((char *)Cdd_IpcAppM4FSS0_0Buffer, 7, "ping %d",
                        Cdd_IpcAppIterCntM4FSS0_0);
    DebugP_log("first ping sent to all cores");

    Cdd_IpcAppM4FSS0_0Buffer[length++] = '\0';
    Cdd_IpcSendMsg(CddIpcConf_IpcComChanId_CDD_IPC_CORE_M4FSS0_0 ,
                    &Cdd_IpcAppM4FSS0_0Buffer[0U], length);
    DebugP_log("ping sent to all cores");

#endif /* CDD_IPC_REMOTE_CORE_M4FSS0_0_USED */
#endif

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

#if !defined(SOC_AM62X)
#ifdef CDD_IPC_REMOTE_CORE_MCU2_0_USED
    if (((Std_ReturnType)E_OK == rtnVal) && (0U != Cdd_IpcAppCtrlMsgMcu20))
    {
        /* One could add checks to confirm the control message here */
        AppUtils_Printf(MSG_NORMAL, CDD_IPC_APP
                    " Received %s as ctrl MSG from MCU 2 0\n",
                    &Cdd_IpcAppCtrlMsgBuffer[0U]);

        /* Send the number of iterations, of transmission and reception */
        numIterationCnt = (uint32 *)Cdd_IpcAppMcu20Buffer;
        *numIterationCnt = Cdd_IpcAppIterCntMcu20;
        length = 4U;
        Cdd_IpcSendMsg(CddIpcConf_IpcComChanId_Cdd_IpcMcu20,
                        &Cdd_IpcAppMcu20Buffer[0U], length);

        /* First Ping */
        length = snprintf((char *)Cdd_IpcAppMcu20Buffer, 8, "ping %d",
                            Cdd_IpcAppIterCntMcu20);
        Cdd_IpcAppMcu20Buffer[length++] = '\0';
        Cdd_IpcSendMsg(CddIpcConf_IpcComChanId_Cdd_IpcMcu20,
                        &Cdd_IpcAppMcu20Buffer[0U], length);

        /* This variable is shared between ISR and app, care should be taken
            to ensure, ISR or interrutps are disabled while updating this
            variable */
        Cdd_IpcAppCtrlMsgMcu20 = 0U;
    }
#endif /* CDD_IPC_REMOTE_CORE_MCU2_0_USED */

#ifdef CDD_IPC_REMOTE_CORE_MCU2_1_USED
    if (((Std_ReturnType)E_OK == rtnVal) && (0U != Cdd_IpcAppCtrlMsgMcu21))
    {
        /* One could add checks to confirm the control message here */
        AppUtils_Printf(MSG_NORMAL, CDD_IPC_APP
                    " Received %s as ctrl MSG from MCU 2 1\n",
                    &Cdd_IpcAppCtrlMsgBuffer[0U]);

        /* Send the number of iterations, of transmission and reception */
        numIterationCnt = (uint32 *)Cdd_IpcAppMcu21Buffer;
        *numIterationCnt = Cdd_IpcAppIterCntMcu21;
        length = 4U;
        Cdd_IpcSendMsg(CddIpcConf_IpcComChanId_Cdd_IpcMcu21,
                        &Cdd_IpcAppMcu21Buffer[0U], length);

        /* First Ping */
        length = snprintf((char *)Cdd_IpcAppMcu21Buffer, 7, "ping %d",
                            Cdd_IpcAppIterCntMcu21);
        Cdd_IpcAppMcu21Buffer[length++] = '\0';
        Cdd_IpcSendMsg(CddIpcConf_IpcComChanId_Cdd_IpcMcu21,
                        &Cdd_IpcAppMcu21Buffer[0U], length);

        /* This variable is shared between ISR and app, care should be taken
            to ensure, ISR or interrutps are disabled while updating this
            variable */
        Cdd_IpcAppCtrlMsgMcu21 = 0U;
    }
#endif /* CDD_IPC_REMOTE_CORE_MCU2_1_USED */
#endif

#if defined(SOC_AM62X)
#ifdef CDD_IPC_REMOTE_CORE_M4FSS0_0_USED
    if (((Std_ReturnType)E_OK == rtnVal) && (0U != Cdd_IpcAppCtrlMsgM4FSS0_0))
    {
        /* One could add checks to confirm the control message here */
        AppUtils_Printf(MSG_NORMAL, CDD_IPC_APP
                    " Received %s as ctrl MSG from M4FSS0_0\n",
                    &Cdd_IpcAppCtrlMsgBuffer[0U]);
                 
        /* Send the number of iterations, of transmission and reception */
        numIterationCnt = (uint32 *)Cdd_IpcAppM4FSS0_0Buffer;
        *numIterationCnt = Cdd_IpcAppIterCntM4FSS0_0;
        length = 4U;
        Cdd_IpcSendMsg(CddIpcConf_IpcComChanId_CDD_IPC_CORE_M4FSS0_0 ,
                        &Cdd_IpcAppM4FSS0_0Buffer[0U], length);

        /* First Ping */
        length = snprintf((char *)Cdd_IpcAppM4FSS0_0Buffer, 8, "ping %d",
                            Cdd_IpcAppIterCntM4FSS0_0);
        Cdd_IpcAppM4FSS0_0Buffer[length++] = '\0';
        Cdd_IpcSendMsg(CddIpcConf_IpcComChanId_CDD_IPC_CORE_M4FSS0_0,
                        &Cdd_IpcAppM4FSS0_0Buffer[0U], length);

        /* This variable is shared between ISR and app, care should be taken
            to ensure, ISR or interrutps are disabled while updating this
            variable */
        Cdd_IpcAppCtrlMsgM4FSS0_0 = 0U;
    }
#endif /* CDD_IPC_REMOTE_CORE_M4FSS0_0_USED */
#endif

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
