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
 *  \file main_rtos.c
 *
 *  \brief Main file for TI-RTOS build
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <stdio.h>

#include <ti/csl/arch/csl_arch.h>
#include <ti/csl/soc.h>
#include <ti/csl/cslr.h>

#include <ti/osal/osal.h>
#include <ti/osal/TaskP.h>

#include <ti/board/board.h>
#include <ti/drv/uart/UART.h>           /* Prints */
#include <ti/drv/uart/UART_stdio.h>

#include <ti/drv/ipc/ipc.h>

#include "main_rtos.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
#define IPC_REMOTE_APP_SERVICE  "ti.ipc4.ping-pong"
/**< Service name identifier */

#define IPC_REMOTE_APP_TASK_STACK       (0x4000)
/**< Stack required for the stack */
#define IPC_REMOTE_APP_TASK_PRIORITY    (2U)
/**< Stack required for the stack */
#define IPC_REMOTE_APP_TASK_NAME        ("IPC Remote Core App")
/**< Task name */
#define IPC_REMOTE_APP_ITERATION_CNT    (10U)
/**< Number of times to send and receive messages */
#define IPC_REMOTE_APP_TX_MSG_SIZE      (256U)
/**< Number of times to send and receive messages */

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static void IpcRemoteApp_TaskFxn(void* argNotUsed, void* argNotUsed1);
/**< Main task */
static void IpcRemoteApp_TxRx(void);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
static uint8_t IpcRemoteApp_TaskStack[IPC_REMOTE_APP_TASK_STACK] __attribute__((aligned(128U)));
/**< Stack for the task */
static uint8_t IpcRemoteApp_RpMsgCtrlTaskStack[IPC_REMOTE_APP_TASK_STACK] __attribute__((aligned(128U)));
/**< Stack for the task */
static uint8_t IpcRemoteApp_RpMsgRxBuf[IPC_REMOTE_APP_RPMSG_DATA_SIZE];
/**< Buffer to hold, received messages from MCU 1 0/ MCAL CDD IPC */

static uint16_t IpcRemoteApp_DstProc = IPC_MCU2_1;
/**< Destination Processor ID */
static uint32_t IpcRemoteApp_LocalEp, IpcRemoteApp_RemoteEp;
/**< End point to be used */
static RPMessage_Handle IpcRemoteApp_CommChHndl;
/**< Handle for communication channel */
static char IpcRemoteApp_RpMsgName[] = IPC_REMOTE_APP_SERVICE;
/**< Named Service */

#define MSG_ERROR       "Error : "
#define MSG_APP_NAME    "IPC Remote APP :"
#define AppUtils_Printf printf
#define CAN_MAX_CONTROLLER  (2U)

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */
void IpcRemoteApp_Printf(const char *str)
{
    printf("%s", str);
}

int main(void)
{
    Board_initCfg   boardCfg;
    TaskP_Handle     task;
    TaskP_Params     taskParams;

    OS_init();

    boardCfg = BOARD_INIT_UART_STDIO;
    Board_init(boardCfg);

    /* Initialize the task params */
    TaskP_Params_init(&taskParams);
    //taskParams.instance->name = IPC_REMOTE_APP_TASK_NAME;
    /* Set the task priority higher than the default priority (1) */
    taskParams.priority     = IPC_REMOTE_APP_TASK_PRIORITY;
    taskParams.stack        = IpcRemoteApp_TaskStack;
    taskParams.stacksize    = sizeof (IpcRemoteApp_TaskStack);

    task = TaskP_create(&IpcRemoteApp_TaskFxn, &taskParams);
    if(NULL == task)
    {
        IpcRemoteApp_Printf(MSG_APP_NAME MSG_ERROR
                            "Could not create a Task \n");
        OS_stop();
    }
    OS_start();

    return(0);
}

static void IpcRemoteApp_TaskFxn(void* argNotUsed, void* argNotUsed1)
{
    int32_t rtnVal;
#if (1U == IPC_REMOTE_APP_BLOCKON_ANNOUNCE)
    uint32_t remoteProcId, remoteEndPt;
#endif
    uint32_t myAllocEp;
    Ipc_VirtIoParams vqParam;
    RPMessage_Params cntrlParam;
    RPMessage_Params comChParam;

    /* Is a 3 Step initialization Sequence
        1. Initialize Processor ID's
        2. Initialize IPC and VirtIO
        3. Initialize Control EP for RP MSG
        3.1. Annouce availability
        Communication can start now
    */
    /* Step 1 */
    rtnVal = Ipc_mpSetConfig(IpcRemoteApp_SelfProcId,
                                    IpcRemoteApp_NumProc,
                                    &IpcRemoteApp_remoteProc[0U]);

    /* Step 2 */
    if (IPC_SOK == rtnVal)
    {
        rtnVal = Ipc_init(NULL);
    }
    /* Init VirtIO */
    if (IPC_SOK == rtnVal)
    {
        vqParam.vqObjBaseAddr = (void*)IpcRemoteApp_SysVqBuf;
        vqParam.vqBufSize     = IpcRemoteApp_NumProc *
                                        Ipc_getVqObjMemoryRequiredPerCore();
        vqParam.vringBaseAddr = (void*)IPC_REMOTE_APP_VRING_BASE_ADDRESS;
        vqParam.vringBufSize  = IPC_REMOTE_APP_VRING_BUFFER_SIZE;
        vqParam.timeoutCnt    = 100;  /* Wait for re transmit counts */
        rtnVal = Ipc_initVirtIO(&vqParam);
    }

    if (IPC_SOK == rtnVal)
    {
        /* Step 3 */
        /* Control EP */
        RPMessageParams_init(&cntrlParam);

        cntrlParam.buf         = &IpcRemoteApp_CtrlTaskMsgBuf[0U];
        cntrlParam.bufSize     = IPC_REMOTE_APP_RPMSG_DATA_SIZE;
        cntrlParam.stackBuffer = &IpcRemoteApp_RpMsgCtrlTaskStack[0U];
        cntrlParam.stackSize   = IPC_REMOTE_APP_TASK_STACK;
        RPMessage_init(&cntrlParam);

        /* Step 3 */
        /* Communication EP */
        if (IPC_MCU1_0 == Ipc_mpGetSelfId())
        {
            IpcRemoteApp_LocalEp = IPC_REMOTE_APP_LOCAL_EP_MCU1_0;
        }
        if (IPC_MCU1_1 == Ipc_mpGetSelfId())
        {
            IpcRemoteApp_LocalEp = IPC_REMOTE_APP_LOCAL_EP_MCU1_1;
        }
        if (IPC_MPU1_0 == Ipc_mpGetSelfId())
        {
            IpcRemoteApp_LocalEp = IPC_REMOTE_APP_LOCAL_EP_MPU1_0;
        }
#if defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
        if (IPC_MCU2_0 == Ipc_mpGetSelfId())
        {
            IpcRemoteApp_LocalEp = IPC_REMOTE_APP_LOCAL_EP_MCU2_0;
        }
        if (IPC_MCU2_1 == Ipc_mpGetSelfId())
        {
            IpcRemoteApp_LocalEp = IPC_REMOTE_APP_LOCAL_EP_MCU2_1;
        }
#endif

        IpcRemoteApp_RemoteEp = IpcRemoteApp_LocalEp;

        RPMessageParams_init(&comChParam);
        comChParam.numBufs = 256U;
        comChParam.buf = IpcRemoteApp_RpMsgRxBuf;
        comChParam.bufSize = IPC_REMOTE_APP_RPMSG_DATA_SIZE;
        comChParam.requestedEndpt = IpcRemoteApp_LocalEp;
        IpcRemoteApp_CommChHndl = RPMessage_create(&comChParam, &myAllocEp);

        if (NULL == IpcRemoteApp_CommChHndl)
        {
            IpcRemoteApp_Printf(MSG_APP_NAME "Error !!!"
                                "Could not create communication channel \n");
        }
        else if (myAllocEp != IpcRemoteApp_LocalEp)
        {
            IpcRemoteApp_Printf(MSG_APP_NAME "Error !!!"
                                    "Could not create required End Point");
        }
        else
        {
            /* Step 3.1 Announce availability */
            rtnVal = RPMessage_announce(IpcRemoteApp_DstProc,
                                        IpcRemoteApp_LocalEp,
                                        (const char*)IpcRemoteApp_RpMsgName);
        }
    }

    if (IPC_SOK == rtnVal)
    {
#if (1U == IPC_REMOTE_APP_BLOCKON_ANNOUNCE)
        /* Wait for Remote EP to active */
        IpcRemoteApp_Printf(MSG_APP_NAME
                            "Waiting for control end point notification \n");
        if (IPC_SOK != RPMessage_getRemoteEndPt(IpcRemoteApp_DstProc,
                                                IPC_REMOTE_APP_SERVICE,
                                                &remoteProcId, &remoteEndPt,
                                                osal_WAIT_FOREVER))
        {
            IpcRemoteApp_Printf(MSG_APP_NAME "Error !!!"
                            "Did not receive any message on control end point");
        }
        else if ((remoteEndPt != IpcRemoteApp_LocalEp) ||
                 (remoteProcId != IpcRemoteApp_DstProc))
        {
            IpcRemoteApp_Printf(MSG_APP_NAME "Error !!!"
                            "Un Expected End point or Processor responded");
        }
        else
#endif
        {
            IpcRemoteApp_TxRx();
        }
    }

    return;
}

/** \brief Transmission and reception */
static void IpcRemoteApp_TxRx(void)
{
    uint32_t idx, remoteProc, remoteEp, cfgNumIteration;
    uint16_t len;
    static char msgBuffer[IPC_REMOTE_APP_TX_MSG_SIZE];
    int32_t rtnVal = IPC_SOK;

    cfgNumIteration = 0U;

    /* Get the number of iterations of transmission and reception to be
        performed */
    rtnVal = RPMessage_recv(IpcRemoteApp_CommChHndl, (Ptr)msgBuffer,
                            &len, &remoteEp,
                            &remoteProc, IPC_RPMESSAGE_TIMEOUT_FOREVER);

    if ((IPC_SOK == rtnVal) && (4U == len))
    {
        cfgNumIteration = *(uint32_t *)msgBuffer;

        printf(MSG_APP_NAME " Configured for %d iterations \n", cfgNumIteration);
    }

    for (idx = 0U; ((idx < cfgNumIteration) &&
                    (IPC_SOK == rtnVal)); idx++)
    {
        len = snprintf(msgBuffer, IPC_REMOTE_APP_TX_MSG_SIZE-1U, "ping %d", idx);
        msgBuffer[len++] = '\0';
        rtnVal = RPMessage_send(IpcRemoteApp_CommChHndl,
                                IpcRemoteApp_DstProc, IpcRemoteApp_RemoteEp,
                                IpcRemoteApp_LocalEp, (Ptr)msgBuffer, len);

        if (IPC_SOK == rtnVal)
        {
            rtnVal = RPMessage_recv(IpcRemoteApp_CommChHndl, (Ptr)msgBuffer,
                                    &len, &remoteEp,
                                    &remoteProc, IPC_RPMESSAGE_TIMEOUT_FOREVER);
            if(IPC_SOK == rtnVal)
            {
                if (len >= IPC_REMOTE_APP_TX_MSG_SIZE)
                {
                    len = IPC_REMOTE_APP_TX_MSG_SIZE - 1U;
                }
                msgBuffer[len] = '\0';

                if ((remoteProc != IpcRemoteApp_DstProc) ||
                    (remoteEp != IpcRemoteApp_RemoteEp))
                {
                    IpcRemoteApp_Printf(MSG_APP_NAME "Error !!!"
                        "Un Expected End point or Processor sent us a message");
                }
            }
        }
    }

    return;
}
