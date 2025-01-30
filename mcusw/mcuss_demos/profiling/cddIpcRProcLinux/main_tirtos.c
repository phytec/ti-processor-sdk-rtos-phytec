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
 *  \file main_tirtos.c
 *
 *  \brief Main file for TI-RTOS build, initializes IPC for profiling
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>
/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

#include <ti/csl/arch/csl_arch.h>
#include <ti/csl/soc.h>
#include <ti/csl/cslr.h>

#include <ti/osal/osal.h>
#include <ti/osal/TaskP.h>

#include "cddIpc_linux_rproc_profile.h"

#include "SchM_Cdd_Ipc.h" /* Exclusive area */

#if (defined (BUILD_MCU1_0) && (defined (SOC_J721E) || defined (SOC_J7200)))
#include <ti/drv/sciclient/sciserver_tirtos.h>
#endif

/* resource table for Linux */
#include "IpcRscTable.h"
#include <ti/drv/ipc/include/ipc_virtio.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* Test application stack size */
#define APP_TASK_STACK                  (10U * 1024U)
/**< Stack required for the stack */
#define IPC_PROFILE_DEMO_TASK_NAME      ("CDD IPC PROFILE")
/**< Task name */


/* High Priority for SCI Server - must be higher than Low priority task */
#define IPC_SETUP_SCISERVER_TASK_PRI_HIGH   (5)
/*
 * Low Priority for SCI Server - must be higher than IPC echo test tasks
 * to prevent delay in handling Sciserver requests when test is performing
 * multicore ping/pong.
 */
#define IPC_SETUP_SCISERVER_TASK_PRI_LOW    (4)


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */
static void Cdd_IpcProfile_PrintVersion(void);
static Void Cdd_IpcProfile_TaskFxn(UArg a0, UArg a1);
sint32 SetupSciServer(void);
/**< Initialize SCI Server, to process RM/PM Requests by other cores */
/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
/* application stack */
static uint8_t Cdd_IpcProfile_TaskStack[APP_TASK_STACK] __attribute__((aligned(32)));
/**< Stack for the task */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */


void Ipc_setupSciServer(void)
{
    Sciserver_TirtosCfgPrms_t appPrms;
    int32_t ret = CSL_PASS;

    ret = Sciserver_tirtosInitPrms_Init(&appPrms);

    appPrms.taskPriority[SCISERVER_TASK_USER_LO] =
                                            IPC_SETUP_SCISERVER_TASK_PRI_LOW;
    appPrms.taskPriority[SCISERVER_TASK_USER_HI] =
                                            IPC_SETUP_SCISERVER_TASK_PRI_HIGH;

    if (ret == CSL_PASS)
    {
        ret = Sciserver_tirtosInit(&appPrms);
    }

    if (ret == CSL_PASS)
    {
        System_printf("Starting Sciserver..... PASSED\n");
    }
    else
    {
        System_printf("Starting Sciserver..... FAILED\n");
    }

    return;
}

void ipc_initSciclient()
{
    Sciclient_ConfigPrms_t        config;

    /* Now reinitialize it as default parameter */
    Sciclient_configPrmsInit(&config);

    Sciclient_init(&config);

}

volatile uint8_t lock1 = 10;

int main(void)
{
    Task_Handle task;
    Error_Block eb;
    Task_Params taskParams;

//while(lock1);

    /* It must be called before board init */
    ipc_initSciclient();

    /* Initialize SCI Client Server */
    Ipc_setupSciServer();

#ifdef UART_ENABLED
    AppUtils_Init();
#endif
    AppUtils_LogResult(APP_UTILS_TEST_STATUS_INIT);

    Cdd_IpcProfile_Startup();

    AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME "\n");
    AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
                            " IPC Profile Application - STARTS !!! \n");

    Error_init(&eb);

    /* Initialize the task params */
    Task_Params_init(&taskParams);
    taskParams.instance->name = IPC_PROFILE_DEMO_TASK_NAME;
    /* Set the task priority higher than the default priority (1) */
    taskParams.priority     = 3;
    taskParams.stack        = Cdd_IpcProfile_TaskStack;
    taskParams.stackSize    = sizeof (Cdd_IpcProfile_TaskStack);

    task = Task_create(Cdd_IpcProfile_TaskFxn, &taskParams, &eb);
    if(NULL == task)
    {
        BIOS_exit(0);
    }
    BIOS_start();

    return(0);
}


static Void Cdd_IpcProfile_TaskFxn(UArg a0, UArg a1)
{


    uint32 idx = 0U;
    const Cdd_IpcChannelType *pCommChCfg = CddIpcConfiguraions_PC.pChCfg;
    uint32 chanCount = CddIpcConfiguraions_PC.channelCount;    

Utils_prfInit();

    Utils_prfLoadRegister (TaskP_self(), IPC_PROFILE_DEMO_TASK_NAME);

    TaskP_yield();

    /* If A72 remote core is running Linux OS, then
     * load resource table
     */
    Ipc_loadResourceTable((void*)&ti_ipc_remoteproc_ResourceTable);
    System_printf(" Resource Table loaded, chanCount %d \n",
                        chanCount);

    /* Wait for Linux VDev ready... */
    for(idx = 0; idx < chanCount; idx++)
    {
       while(!Ipc_isRemoteReady(pCommChCfg[idx].remoteProcId))
        {
        }
    }

   System_printf("Linux VDEV ready now .....\n");

    Cdd_IpcInit();
    /* No error checks required, as Det are turned OFF */

    Cdd_IpcProfile_PrintVersion();



    Cdd_IpcProfileTest();

    Utils_prfLoadUnRegister (TaskP_self());
    Utils_prfDeInit();
    return;
}


/** \brief prints of the version of this implementation */
static void Cdd_IpcProfile_PrintVersion(void)
{
#if (STD_ON == CDD_IPC_VERSION_INFO_API)
    Std_VersionInfoType versioninfo;

    Cdd_IpcGetVersionInfo(&versioninfo);
    AppUtils_Printf(MSG_NORMAL, " \n");
    AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME " CDD IPC MCAL Version Info\n");
    AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME "---------------------\n");
    AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME " Vendor ID           : %d\n",
                                                versioninfo.vendorID);
    AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME " Module ID           : %d\n",
                                                versioninfo.moduleID);
    AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME " SW Major Version    : %d\n",
                                                versioninfo.sw_major_version);
    AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME " SW Minor Version    : %d\n",
                                                versioninfo.sw_minor_version);
    AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME " SW Patch Version    : %d\n",
                                                versioninfo.sw_patch_version);
    AppUtils_Printf(MSG_NORMAL, " \n");
#endif

}

sint32 SetupSciServer(void)
{
    sint32 ret = CSL_PASS;
#if (defined (BUILD_MCU1_0) && (defined (SOC_J721E) || defined (SOC_J7200)))
    Sciserver_TirtosCfgPrms_t appPrms;
    Sciclient_ConfigPrms_t clientPrms;

    appPrms.taskPriority[SCISERVER_TASK_USER_LO] = 1;
    appPrms.taskPriority[SCISERVER_TASK_USER_HI] = 4;

    /* Sciclient needs to be initialized before Sciserver. Sciserver depends on
     * Sciclient API to execute message forwarding */
    ret = Sciclient_configPrmsInit(&clientPrms);
    if (ret == CSL_PASS)
    {
        ret = Sciclient_init(&clientPrms);
    }

    if (ret == CSL_PASS)
    {
        ret = Sciserver_tirtosInit(&appPrms);
    }

    if (ret == CSL_PASS)
    {
        AppUtils_Printf(MSG_NORMAL, "Starting Sciserver..... PASSED\n");
    }
    else
    {
        AppUtils_Printf(MSG_NORMAL, "Starting Sciserver..... FAILED\n");
    }

#endif
    return ret;
}

/*EoF*/
