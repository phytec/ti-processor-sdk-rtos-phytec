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
 *  \file     CddIpcApp.c
 *
 *  \brief    This file contains the demo application for CDD IPC
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "CddIpcAppRProcLinux.h"
#include "app_utils.h"
#include "IntRtr_Cfg.h" /* Defines the map / cfg for
                         * interrupt router, for this example
                         */
#include "SchM_Cdd_Ipc.h" /* Exclusive area */

#include <hw_include/soc.h>

#if defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)

#include <hw_include/lld_intr_router.h> /* CSL for interrupt router */

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

/* resource table for Linux */
#include "IpcRscTable.h"
#include <ti/drv/ipc/include/ipc_virtio.h>

#include <ti/csl/soc.h>
#include <ti/csl/cslr.h>

#include <ti/osal/osal.h>
#include <ti/osal/TaskP.h>
#include <ti/board/board.h>

#include <ti/drv/sciclient/sciserver_tirtos.h>

#include <ti/drv/sciclient/src/sciclient/sciclient_priv.h>

#endif

#define System_printf Ipc_Trace_printf
#if defined(SOC_AM62X)  ||  defined(SOC_AM62AX) || defined(SOC_AM62PX) || defined(SOC_J722S)
#include <hw_include/soc.h>
#include <hw_include/hw_types.h>
#include <drivers/sciclient.h>
#include "IpcRscTable.h"
#include <CddIpc/ipc_baremetal_hw/include/ipc_virtio.h>
#endif
#if defined(SOC_AM62X)
#include <stdlib.h>
#include <kernel/dpl/DebugP.h>
#include <kernel/dpl/ClockP.h>
#include "ti_drivers_config.h"
#include "ti_board_config.h"
#include "FreeRTOS.h"
#include "task.h"
#include <drivers/device_manager/sciserver/sciserver_init.h>
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

#define MSG_NORMAL      (APP_UTILS_PRINT_MSG_NORMAL)
/**< Message type */
#define MSG_STATUS      (APP_UTILS_PRINT_MSG_STATUS)
/**< Message type */
#define CDD_IPC_APP_MSG_DATA_SIZE   (256U)
/**< Size of message buffer to be transmitted */
#define CDD_IPC_APP_MSG_TX_RX_CNT   (10U)
/**< Number of times the messages that has to be transmitted and received */
#if ((defined (BUILD_MCU1_0)) && (defined (SOC_J721E) || defined (SOC_J7200)))
#define APP_TSK_STACK_MAIN                  (10U * 1024U)
#define APP_SCISERVER_INIT_TSK_STACK        (32U * 1024U)
#define IPC_INIT_SCISERVER_TASK_PRI         (6)
#define IPC_SETUP_SCISERVER_TASK_PRI_LOW    (4)
#define IPC_SETUP_SCISERVER_TASK_PRI_HIGH   (5)
#endif

#if defined(SOC_AM62X)
#define MAIN_TASK_PRI  (configMAX_PRIORITIES-2)
/**< Main task priority */
#define MAIN_TASK_SIZE (32768U/sizeof(configSTACK_DEPTH_TYPE))
/**< Main task stack size */

#define CDD_IPC_APP_MSG_DATA_SIZE   (256U)
/**< Size of message buffer to be transmitted */
#define System_printf Ipc_Trace_printf

#define LINUX_REMOTE_CORE_COMMUNICATION_ITERATIONS    (20U)
/**< Number of message transmission/reception iterations for remote core linux*/

#define RTOS_REMOTE_CORE_COMMUNICATION_ITERATIONS    (10U)
/**< Number of message transmission/reception iterations for remote core RTOS*/

#endif

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */

#if ((defined (BUILD_MCU1_0)) && (defined (SOC_J721E) || defined (SOC_J7200)))
void ipc_initSciclient(void);
void taskFxn(void* a0, void* a1);
void Ipc_setupSciServer(void *arg0, void *arg1);
void cdd_ipc_Linux(void);
void CddIpcMessageProcess(void);
#endif

#if defined(SOC_AM62X)
void freertos_main(void *args);
void cdd_ipc_linux_example(void);
#endif

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

uint32 Cdd_IpcAppIterCntMpu10 = CDD_IPC_APP_MSG_TX_RX_CNT;
/**< Counter to track number of messages to be transmitted & received */

volatile uint32 Cdd_IpcNewMsgCountEP13 = 0;
volatile uint32 Cdd_IpcNewMsgCountEP14 = 0;

static uint8 Cdd_IpcAppMpu10Buffer[CDD_IPC_APP_MSG_DATA_SIZE];
/**< Buffer used to transmit and receive messages, to/from MPU 10 */

uint32 mpu10Bufsize = CDD_IPC_APP_MSG_DATA_SIZE;

#if (STD_ON == CDD_IPC_ANNOUNCE_API)
    const char announceMsg1[32U] = "ti.ipc4.ping-pong";
    const char announceMsg2[32U] = "rpmsg_chrdev";
#endif


#if ((defined (BUILD_MCU1_0)) && (defined (SOC_J721E) || defined (SOC_J7200)))
/* Main application task stack */
static uint8_t  gAppTskStackMain[APP_TSK_STACK_MAIN]
__attribute__ ((aligned(8192)));
/* Secondarary task stack*/
static uint8_t  gSciserverInitTskStack[APP_SCISERVER_INIT_TSK_STACK]
__attribute__ ((aligned(8192)));

extern Sciclient_ServiceHandle_t gSciclientHandle;
#endif

#if defined(SOC_AM62X)
StackType_t gMainTaskStack[MAIN_TASK_SIZE] __attribute__((aligned(32)));
/**< Main task stack implemented using array */
StaticTask_t gMainTaskObj;
/**< Wiil be used as a pointer to statically allocated task object memory */
TaskHandle_t gMainTask;
/**< Variable to store return value of task creation */
static uint8 Cdd_IpcAppM4Buffer[CDD_IPC_APP_MSG_DATA_SIZE];
/**< Variable to receive and transmit messages to M4 */
volatile uint32 Cdd_IpcNewMsgFromM4Count = 0;
#endif

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

int main(void){

#if ((defined (BUILD_MCU2_1)) && (defined (SOC_J721E) || defined (SOC_J7200)))

    /* Variable to keep track of current channel ID */
    uint32 ChannelId = 0;
    /* Variable storing maximum channels configured for IPC communication */
    uint32 MaxChannelsConfigured = 0;
    /* MaxChannelsConfigured variable updated according to demo configurations */
    MaxChannelsConfigured = CddIpcConfiguraions_PC.channelCount;
    /* Pointer to channel configurations */
    const Cdd_IpcChannelType *pCommChCfg = CddIpcConfiguraions_PC.pChCfg;

    /*Initialize SOC*/
    Cdd_IpcApp_Startup();
    /* Load resource table */
    Ipc_loadResourceTable((void*)&ti_ipc_remoteproc_ResourceTable);
    
    /* Wait for Linux VDev ready... */
    for(ChannelId = 0;  ChannelId < MaxChannelsConfigured;  ChannelId++)
    {
        while(!Ipc_isRemoteReady(pCommChCfg[ChannelId].remoteProcId))
        {
            /* Only checking for all channels with A72(Linux) as remote core
             * If more than one channel is configured with both Linux and 
             * Non-Linux cores as remote cores change logic to check and wait
             * only for Linux remote cores
             */
        }
    }
    
    /* Initialize driver */
    Cdd_IpcInit();

    /* Announce that MCAL core is ready for communication with Linux remote core
     * on EP 13, ti.ipc4.ping-pong.
     * on EP 14, rpmsg_chrdev communication
     */
    #if (STD_ON == CDD_IPC_ANNOUNCE_API)
    Cdd_IpcAnnounce((const UCHAR *)announceMsg1,pCommChCfg[CddIpcConf_IpcComChanId_Cdd_IpcMpu10_EP13].id);
    Cdd_IpcAnnounce((const UCHAR *)announceMsg2,pCommChCfg[CddIpcConf_IpcComChanId_Cdd_IpcMpu10_EP14].id);
    #endif

    while(1){
        /* Check if a new message is received from the remote core on EP13*/
        if(0 != Cdd_IpcNewMsgCountEP13){
            /* Receive message */
            Cdd_IpcReceiveMsg(CddIpcConf_IpcComChanId_Cdd_IpcMpu10_EP13, &Cdd_IpcAppMpu10Buffer[0U], &mpu10Bufsize);
            /* Disable global interrupts */
            SchM_Enter_Cdd_Ipc_IPC_EXCLUSIVE_AREA_0();
            /* Decrement message reception variable to indicate message has been received on EP13*/
            Cdd_IpcNewMsgCountEP13--;
            /* Enable global interrupts */
            SchM_Exit_Cdd_Ipc_IPC_EXCLUSIVE_AREA_0();
            /* Respond back with the same received message to the remote core */
            Cdd_IpcSendMsg(CddIpcConf_IpcComChanId_Cdd_IpcMpu10_EP13, &Cdd_IpcAppMpu10Buffer[0U], mpu10Bufsize);
        }
        else if(0 != Cdd_IpcNewMsgCountEP14){
            /* Receive message */
            Cdd_IpcReceiveMsg(CddIpcConf_IpcComChanId_Cdd_IpcMpu10_EP14, &Cdd_IpcAppMpu10Buffer[0U], &mpu10Bufsize);
            /* Disable global interrupts */
            SchM_Enter_Cdd_Ipc_IPC_EXCLUSIVE_AREA_0();
            /* Decrement message reception variable to indicate message has been received on EP13*/
            Cdd_IpcNewMsgCountEP14--;
            /* Enable global interrupts */
            SchM_Exit_Cdd_Ipc_IPC_EXCLUSIVE_AREA_0();
            /* Respond back with the same received message to the remote core */
            Cdd_IpcSendMsg(CddIpcConf_IpcComChanId_Cdd_IpcMpu10_EP14, &Cdd_IpcAppMpu10Buffer[0U], mpu10Bufsize);
        }
    }

#endif

#if ((defined (BUILD_MCU1_0)) && (defined (SOC_J721E) || defined (SOC_J7200)))
    TaskP_Handle task;
    TaskP_Params taskParams;

    void _freertosresetvectors (void);
    memcpy((void *)0x0, (void *)_freertosresetvectors, 0x40);
    
    OS_init();

    /* Initialize the task params */
    TaskP_Params_init(&taskParams);

    /* Set the task priority higher than the default priority (1) */
    taskParams.priority = 2;
    taskParams.stack        = gAppTskStackMain;
    taskParams.stacksize    = sizeof (gAppTskStackMain);

    task = TaskP_create(&taskFxn, &taskParams);
    if(NULL == task)
    {
        OS_stop();
    }
    OS_start();    /* does not return */

    return(0);

#endif

#if defined(SOC_AM62AX) || defined(SOC_AM62PX) || defined(SOC_J722S)

    /* Variable to keep track of current channel ID */
    uint32 ChannelId = 0;
    /* Variable storing maximum channels configured for IPC communication */
    uint32 MaxChannelsConfigured = 0;
    /* MaxChannelsConfigured variable updated according to demo configurations */
    MaxChannelsConfigured = CddIpcConfiguraions_PC.channelCount;
    /* Pointer to channel configurations */
    const Cdd_IpcChannelType *pCommChCfg = CddIpcConfiguraions_PC.pChCfg;

    /* SOC Initialization */
    Cdd_IpcApp_Startup();

    /* If A53 remote core is running Linux OS, then
     * load resource table
     */
    Ipc_loadResourceTable((void*)&ti_ipc_remoteproc_ResourceTable);

    /* Wait for Linux VDev ready... */
    for(ChannelId = 0;  ChannelId < MaxChannelsConfigured;  ChannelId++)
    {
        while(!Ipc_isRemoteReady(pCommChCfg[ChannelId].remoteProcId))
        {
            /* Only checking for all channels with A53(Linux) as remote core
             * If more than one channel is configured with both Linux and 
             * Non-Linux cores as remote cores change logic to check and wait
             * only for Linux remote cores
             */
        }
    }

    /* Initialize driver */
    Cdd_IpcInit();

    /* Announce that MCAL core is ready for communication with Linux remote core
     * on EP 13, ti.ipc4.ping-pong.
     * on EP 14, rpmsg_chrdev communication
     */
    #if (STD_ON == CDD_IPC_ANNOUNCE_API)
    Cdd_IpcAnnounce((const UCHAR *)announceMsg1,pCommChCfg[CddIpcConf_IpcComChanId_Cdd_IpcMpu10_EP13].id);
    Cdd_IpcAnnounce((const UCHAR *)announceMsg2,pCommChCfg[CddIpcConf_IpcComChanId_Cdd_IpcMpu10_EP14].id);
    #endif

    while(1){
        /* Check if a new message is received from the remote core on EP13*/
        if(0 != Cdd_IpcNewMsgCountEP13){
            /* Receive message */
            Cdd_IpcReceiveMsg(CddIpcConf_IpcComChanId_Cdd_IpcMpu10_EP13, &Cdd_IpcAppMpu10Buffer[0U], &mpu10Bufsize);
            /* Disable global interrupts */
            SchM_Enter_Cdd_Ipc_IPC_EXCLUSIVE_AREA_0();
            /* Decrement message reception variable to indicate message has been received on EP13*/
            Cdd_IpcNewMsgCountEP13--;
            /* Enable global interrupts */
            SchM_Exit_Cdd_Ipc_IPC_EXCLUSIVE_AREA_0();
            /* Respond back with the same received message to the remote core */
            Cdd_IpcSendMsg(CddIpcConf_IpcComChanId_Cdd_IpcMpu10_EP13, &Cdd_IpcAppMpu10Buffer[0U], mpu10Bufsize);
        }
        else if(0 != Cdd_IpcNewMsgCountEP14){
            /* Receive message */
            Cdd_IpcReceiveMsg(CddIpcConf_IpcComChanId_Cdd_IpcMpu10_EP14, &Cdd_IpcAppMpu10Buffer[0U], &mpu10Bufsize);
            /* Disable global interrupts */
            SchM_Enter_Cdd_Ipc_IPC_EXCLUSIVE_AREA_0();
            /* Decrement message reception variable to indicate message has been received on EP13*/
            Cdd_IpcNewMsgCountEP14--;
            /* Enable global interrupts */
            SchM_Exit_Cdd_Ipc_IPC_EXCLUSIVE_AREA_0();
            /* Respond back with the same received message to the remote core */
            Cdd_IpcSendMsg(CddIpcConf_IpcComChanId_Cdd_IpcMpu10_EP14, &Cdd_IpcAppMpu10Buffer[0U], mpu10Bufsize);
        }
    }

#endif

#if defined(SOC_AM62X)

    /* init SOC specific modules */
    Cdd_IpcApp_Startup();

    /* This task is created at highest priority, it should create more tasks and then delete itself */
    gMainTask = xTaskCreateStatic( freertos_main,   /* Pointer to the function that implements the task. */
                                  "freertos_main", /* Text name for the task.  This is to facilitate debugging only. */
                                  MAIN_TASK_SIZE,  /* Stack depth in units of StackType_t typically uint32_t on 32b CPUs */
                                  NULL,            /* We are not using the task parameter. */
                                  MAIN_TASK_PRI,   /* task priority, 0 is lowest priority, configMAX_PRIORITIES-1 is highest */
                                  gMainTaskStack,  /* pointer to stack base */
                                  &gMainTaskObj ); /* pointer to statically allocated task object memory */
    configASSERT(gMainTask != NULL);

    /* Start the scheduler to start the tasks executing. */
    vTaskStartScheduler();

    /* The following line should never be reached because vTaskStartScheduler()
    will only return if there was not enough FreeRTOS heap memory available to
    create the Idle and (if configured) Timer tasks.  Heap management, and
    techniques for trapping heap exhaustion, are described in the book text. */
    DebugP_assertNoLog(0);

    return 0;

#endif

}

/* ========================================================================== */
/*                        Application Local Functions                         */
/* ========================================================================== */

#if ((defined (BUILD_MCU1_0)) && (defined (SOC_J721E) || defined (SOC_J7200)))

void taskFxn(void* a0, void* a1)
{
    /* Initialize SCI Client*/
    ipc_initSciclient();

    /*Initialize SOC*/
    Cdd_IpcApp_Startup();

    #if ((defined (BUILD_MCU1_0)) && (defined (SOC_J721E) || defined (SOC_J7200)))
    TaskP_Handle sciserverInitTask;
    TaskP_Params sciserverInitTaskParams;

    /* Initialize SCI Client Server */
    TaskP_Params_init(&sciserverInitTaskParams);
    sciserverInitTaskParams.priority     = IPC_INIT_SCISERVER_TASK_PRI;
    sciserverInitTaskParams.stack        = gSciserverInitTskStack;
    sciserverInitTaskParams.stacksize    = sizeof (gSciserverInitTskStack);

    sciserverInitTask = TaskP_create(&Ipc_setupSciServer, &sciserverInitTaskParams);
    if(NULL == sciserverInitTask)
    {
        OS_stop();
    }
#endif
    cdd_ipc_Linux();
}

void ipc_initSciclient()
{
    int32_t ret = CSL_PASS;
    Sciclient_ConfigPrms_t        config;

    /* Now reinitialize it as default parameter */
    ret = Sciclient_configPrmsInit(&config);
    if (ret != CSL_PASS)
    {
        //AppUtils_Printf(MSG_NORMAL,"Sciclient_configPrmsInit Failed\n");
    }

#if ((defined (BUILD_MCU1_0)) && (defined (SOC_J721E) || defined (SOC_J7200)))
    if (ret == CSL_PASS)
    {
        ret = Sciclient_boardCfgParseHeader(
            (uint8_t *)SCISERVER_COMMON_X509_HEADER_ADDR,
            &config.inPmPrms, &config.inRmPrms);
        if (ret != CSL_PASS)
        {
            //AppUtils_Printf(MSG_NORMAL,"Sciclient_boardCfgParseHeader Failed\n");
        }
    }
#endif

    if (ret == CSL_PASS)
    {
        ret = Sciclient_init(&config);
        if (ret != CSL_PASS)
        {
            //AppUtils_Printf(MSG_NORMAL,"Sciclient_init Failed\n");
        }
#if ((defined (BUILD_MCU1_0)) && (defined (SOC_J721E) || defined (SOC_J7200)))
        if (gSciclientHandle.rmBoardConfigComplete == SCICLIENT_FT_PASS)
        {
            //AppUtils_Printf(MSG_NORMAL,"Sciclient_boardCfgRm init Passed\n");
        }
        else
        {
            //AppUtils_Printf(MSG_NORMAL,"Sciclient_boardCfgRm init FAILED!\n");
        }
#endif
    }
}

void Ipc_setupSciServer(void *arg0, void *arg1)
{

    Sciserver_TirtosCfgPrms_t appPrms;
    int32_t ret = CSL_PASS;
    //char *version_str = NULL;
    //char *rmpmhal_version_str = NULL;

    ret = Sciserver_tirtosInitPrms_Init(&appPrms);

    appPrms.taskPriority[SCISERVER_TASK_USER_LO] =
                                            IPC_SETUP_SCISERVER_TASK_PRI_LOW;
    appPrms.taskPriority[SCISERVER_TASK_USER_HI] =
                                            IPC_SETUP_SCISERVER_TASK_PRI_HIGH;

    if (ret == CSL_PASS)
    {
        ret = Sciserver_tirtosInit(&appPrms);
    }

    //version_str = Sciserver_getVersionStr();
    //rmpmhal_version_str = Sciserver_getRmPmHalVersionStr();
    //AppUtils_Printf(MSG_NORMAL,"Sciserver Version: %s\n", version_str);
    //AppUtils_Printf(MSG_NORMAL,"RM_PM_HAL Version: %s\n", rmpmhal_version_str);

    if (ret == CSL_PASS)
    {
        //App_printf("Starting Sciserver..... PASSED\n");
    }
    else
    {
        //App_printf("Starting Sciserver..... FAILED\n");
    }

    return;
}

void cdd_ipc_Linux(void){

    /* Pointer to channel configurations */
    const Cdd_IpcChannelType *pCommChCfg = CddIpcConfiguraions_PC.pChCfg;

    /* Load resource table */
    Ipc_loadResourceTable((void*)&ti_ipc_remoteproc_ResourceTable);
    
    /* Wait for Linux VDev ready... */
    while(!Ipc_isRemoteReady(pCommChCfg[0].remoteProcId))
    {
        /* Only checking for one channel as demo configurations
         * for mcu1_0 have only one channel configured for Linux
         * communication
         */
    }
    /* Initialize driver */
    Cdd_IpcInit();

    /* Announce that MCAL core is ready for communication with Linux remote core
     * on EP 13, ti.ipc4.ping-pong.
     * on EP 14, rpmsg_chrdev communication
     */
    #if (STD_ON == CDD_IPC_ANNOUNCE_API)
    Cdd_IpcAnnounce((const UCHAR *)announceMsg1,pCommChCfg[CddIpcConf_IpcComChanId_Cdd_IpcMpu10_EP13].id);
    Cdd_IpcAnnounce((const UCHAR *)announceMsg2,pCommChCfg[CddIpcConf_IpcComChanId_Cdd_IpcMpu10_EP14].id);
    #endif

    while(1){
    /* Check if a new message is received from the remote core on EP13*/
        if(0 != Cdd_IpcNewMsgCountEP13){
            /* Receive message */
            Cdd_IpcReceiveMsg(CddIpcConf_IpcComChanId_Cdd_IpcMpu10_EP13, &Cdd_IpcAppMpu10Buffer[0U], &mpu10Bufsize);
            /* Disable global interrupts */
            SchM_Enter_Cdd_Ipc_IPC_EXCLUSIVE_AREA_0();
            /* Decrement message reception variable to indicate message has been received on EP13*/
            Cdd_IpcNewMsgCountEP13--;
            /* Enable global interrupts */
            SchM_Exit_Cdd_Ipc_IPC_EXCLUSIVE_AREA_0();
            /* Respond back with the same received message to the remote core */
            Cdd_IpcSendMsg(CddIpcConf_IpcComChanId_Cdd_IpcMpu10_EP13, &Cdd_IpcAppMpu10Buffer[0U], mpu10Bufsize);
        }
        else if(0 != Cdd_IpcNewMsgCountEP14){
            /* Receive message */
            Cdd_IpcReceiveMsg(CddIpcConf_IpcComChanId_Cdd_IpcMpu10_EP14, &Cdd_IpcAppMpu10Buffer[0U], &mpu10Bufsize);
            /* Disable global interrupts */
            SchM_Enter_Cdd_Ipc_IPC_EXCLUSIVE_AREA_0();
            /* Decrement message reception variable to indicate message has been received on EP13*/
            Cdd_IpcNewMsgCountEP14--;
            /* Enable global interrupts */
            SchM_Exit_Cdd_Ipc_IPC_EXCLUSIVE_AREA_0();
            /* Respond back with the same received message to the remote core */
            Cdd_IpcSendMsg(CddIpcConf_IpcComChanId_Cdd_IpcMpu10_EP14, &Cdd_IpcAppMpu10Buffer[0U], mpu10Bufsize);
        }
    }
}

#endif

#if defined(SOC_AM62X)

void freertos_main(void *args)
{
    sciServer_init();
    cdd_ipc_linux_example();
    while (1)
    {
        ClockP_usleep(1000);

    }
    vTaskDelete(NULL);
}

void cdd_ipc_linux_example(void){

    /* Variable to keep track of current channel ID */
    uint32 ChannelId = 0;
    /* Variable storing maximum channels configured for IPC communication */
    uint32 MaxChannelsConfigured = 0;
    /* MaxChannelsConfigured variable updated according to demo configurations */
    MaxChannelsConfigured = CddIpcConfiguraions_PC.channelCount;
    /* Maximum number of messages to be exchanged with Linux core */
    uint32 MaxNumOfMsgIterationsRemoteCoreLinux = LINUX_REMOTE_CORE_COMMUNICATION_ITERATIONS;
    /* Pointer to channel configurations */
    const Cdd_IpcChannelType *pCommChCfg = CddIpcConfiguraions_PC.pChCfg;

    /* If A53 remote core is running Linux OS, then
     * load resource table
     */
    Ipc_loadResourceTable((void*)&ti_ipc_remoteproc_ResourceTable);

    /* Wait for Linux VDev ready... */
    for(ChannelId = 0;  ChannelId < MaxChannelsConfigured;  ChannelId++)
    {
        while(!Ipc_isRemoteReady(pCommChCfg[ChannelId].remoteProcId))
        {
            /* Only checking for all channels with A53(Linux) as remote core
             * If more than one channel is configured with both Linux and 
             * Non-Linux cores as remote cores change logic to check and wait
             * only for Linux remote cores
             */
            ClockP_usleep(1000);
        }
    }

    /* Initialize driver */
    Cdd_IpcInit();

    /* Announce that MCAL core is ready for communication with Linux remote core
     * on EP 13, ti.ipc4.ping-pong and EP14, rpmsg_chrdev
     */
    #if (STD_ON == CDD_IPC_ANNOUNCE_API)
        Cdd_IpcAnnounce((const UCHAR *)announceMsg1,pCommChCfg[CddIpcConf_IpcComChanId_Cdd_IpcMpu10_EP13].id);
        Cdd_IpcAnnounce((const UCHAR *)announceMsg2,pCommChCfg[CddIpcConf_IpcComChanId_Cdd_IpcMpu10_EP14].id);
    #endif

/* Loop to check if atleast 20 messages have been exchanged with Linux remote core */
while(0 != MaxNumOfMsgIterationsRemoteCoreLinux){
    /* Check if a new message is received from the remote core on EP13*/
        if(0 != Cdd_IpcNewMsgCountEP13){
            /* Receive message */
            Cdd_IpcReceiveMsg(CddIpcConf_IpcComChanId_Cdd_IpcMpu10_EP13, &Cdd_IpcAppMpu10Buffer[0U], &mpu10Bufsize);
            /* Disable global interrupts */
            SchM_Enter_Cdd_Ipc_IPC_EXCLUSIVE_AREA_0();
            /* Decrement message reception variable to indicate message has been received on EP13*/
            Cdd_IpcNewMsgCountEP13--;
            /* Decrement Toatal messages left to receive from remote core Linux so that 
             * communication with remote RTOS core can be set up
             */
            MaxNumOfMsgIterationsRemoteCoreLinux--;
            /* Enable global interrupts */
            SchM_Exit_Cdd_Ipc_IPC_EXCLUSIVE_AREA_0();
            /* Respond back with the same received message to the remote core */
            Cdd_IpcSendMsg(CddIpcConf_IpcComChanId_Cdd_IpcMpu10_EP13, &Cdd_IpcAppMpu10Buffer[0U], mpu10Bufsize);
        }
        else if(0 != Cdd_IpcNewMsgCountEP14){
            /* Receive message */
            Cdd_IpcReceiveMsg(CddIpcConf_IpcComChanId_Cdd_IpcMpu10_EP14, &Cdd_IpcAppMpu10Buffer[0U], &mpu10Bufsize);
            /* Disable global interrupts */
            SchM_Enter_Cdd_Ipc_IPC_EXCLUSIVE_AREA_0();
            /* Decrement message reception variable to indicate message has been received on EP13*/
            Cdd_IpcNewMsgCountEP14--;
            /* Decrement Toatal messages left to receive from remote core Linux so that 
             * communication with remote RTOS core can be set up
             */
            MaxNumOfMsgIterationsRemoteCoreLinux--;
            /* Enable global interrupts */
            SchM_Exit_Cdd_Ipc_IPC_EXCLUSIVE_AREA_0();
            /* Respond back with the same received message to the remote core */
            Cdd_IpcSendMsg(CddIpcConf_IpcComChanId_Cdd_IpcMpu10_EP14, &Cdd_IpcAppMpu10Buffer[0U], mpu10Bufsize);
        }
    ClockP_usleep(1000);
}

/* Copy message to send to M4 in array */
strcpy((char *)Cdd_IpcAppM4Buffer, "Hello World");
/* Send message */
Cdd_IpcSendMsg(CddIpcConf_IpcComChanId_CDD_IPC_CORE_M4FSS0_0, &Cdd_IpcAppM4Buffer[0U], mpu10Bufsize);
MaxNumOfMsgIterationsRemoteCoreLinux = 10;

/* Communication with M4 core */
while(1){
    if((0 != Cdd_IpcNewMsgFromM4Count) && (0 != MaxNumOfMsgIterationsRemoteCoreLinux)){
        /* Receive message */
        Cdd_IpcReceiveMsg(CddIpcConf_IpcComChanId_CDD_IPC_CORE_M4FSS0_0, &Cdd_IpcAppM4Buffer[0U], &mpu10Bufsize);
        /* Disable global interrupts */
        SchM_Enter_Cdd_Ipc_IPC_EXCLUSIVE_AREA_0();
        /* Decrement message reception variable to indicate message has been received on EP13 from M4 core*/
        Cdd_IpcNewMsgFromM4Count--;
        MaxNumOfMsgIterationsRemoteCoreLinux--;
        /* Enable global interrupts */
        SchM_Exit_Cdd_Ipc_IPC_EXCLUSIVE_AREA_0();
        /* Respond back with the same received message to the remote core */
        Cdd_IpcSendMsg(CddIpcConf_IpcComChanId_CDD_IPC_CORE_M4FSS0_0, &Cdd_IpcAppM4Buffer[0U], mpu10Bufsize);   
    }
    ClockP_usleep(1000);    
}

}

/* New message notify function for end point 13
 * Contains user implementation for acknowledgment
 * that a message is available through a particular
 * channel from M4 core
 */
void CddIpcNewMsgFromM4(void){
    Cdd_IpcNewMsgFromM4Count++;
}

#endif

/* User implemented funtion to process a control message
 * in case of reception of a control message, usually an
 * announcement message from the remote core at end point 
 * number 53
 */

#if (STD_ON == CDD_IPC_ANNOUNCE_API)
void Cdd_IpcNewCtrlMessageNotify(uint32 remoteProcId){

  #if defined (SOC_AM62X)
    if(remoteProcId == CDD_IPC_CORE_A53SS_1){
        /* Custom user implementation in case 
         * control message is received from 
         * A53 for AM62X
         */
    }
    #endif

    #if defined (SOC_AM62AX)
    if(remoteProcId == CDD_IPC_CORE_A53SS0_0){
        /* Custom user implementation in case 
         * control message is received from 
         * A53 for AM62AX
         */
    }
    #endif

    #if defined (SOC_J721E) || defined (SOC_J7200)
    if(remoteProcId == CDD_IPC_CORE_MPU1_0){
        /* Custom user implementation in case 
         * control message is received from 
         * A72 for J721E/J7200
         */
    }
    #endif
}
#endif /* (STD_ON == CDD_IPC_ANNOUNCE_API) */

/* New message notify function for end point 13
 * Contains user implementation for acknowledgment
 * that a message is available through a particular
 * channel
 */
void CddIpcNewMsgEp13(void){
    Cdd_IpcNewMsgCountEP13++;
}

/* New message notify function for end point 14
 * Contains user implementation for acknowledgment
 * that a message is available through a particular
 * channel
 */
void CddIpcNewMsgEp14(void){
    Cdd_IpcNewMsgCountEP14++;
}

/* EoF */