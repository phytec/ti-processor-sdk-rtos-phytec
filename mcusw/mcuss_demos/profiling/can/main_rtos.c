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
 *  \file can_profile.h.c
 *
 *  \brief Main file for TI-RTOS build
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/csl/arch/csl_arch.h>
#include <ti/csl/soc.h>
#include <ti/csl/cslr.h>

#include <ti/osal/osal.h>
#include <ti/osal/TaskP.h>

#include "CanApp_Startup.h"
#include "can_profile.h"
#include "CanIf.h"
#if (defined (BUILD_MCU1_0) && (defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J784S4) || defined (SOC_J721S2)))
#include <ti/drv/sciclient/sciserver_tirtos.h>
#endif
/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* Test application stack size */
#define APP_TASK_STACK                  (10U * 1024U)
/**< Stack required for the stack */
#define CAN_PROFILE_DEMO_TASK_NAME      ("CAN PROFILE")
/**< Task name */

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ==========================================================================*/
/*                          Function Declarations                             */
/* ========================================================================== */

static void CanApp_TaskFxn(void* a0, void* a1);
static void CanApp_Shutdown(void);
sint32 SetupSciServer(void);
/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
/* application stack */
static uint8_t CanApp_TaskStack[APP_TASK_STACK] __attribute__((aligned(32)));
/**< Stack for the task */
static canAppTaskObj_t  CanApp_TestPrms;
/**< Test parameters */
static HwiP_Handle      CanApp_IsrHndls[CAN_MAX_CONTROLLER];
/**< Stores the ISR handles */
extern SemaphoreP_Handle CanIf_TxConfirmationSemaphore;
/**< TX Confirmation semaphore, would be posted when TX completes */
extern SemaphoreP_Handle CanIf_RxConfirmationSemaphore;
/**< Rx Confirmation semaphore, would be posted when TX completes */
/* ========================================================================== */
/*                            External Variables                              */
/* ========================================================================== */
extern uint32 CanIf_DrvStatus;
/**< CAN IF Driver Status, defined in CanIf.c */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

int main(void)
{
    TaskP_Handle task;
    TaskP_Params taskParams;
    sint32 ret = CSL_PASS;

    OS_init();

#ifdef UART_ENABLED
    AppUtils_Init();
#endif

    /* Initialize SCI Client Server */
    ret = SetupSciServer();
    if(ret != CSL_PASS)
    {
        OS_stop();
    }
    CanApp_Startup();
    CanApp_PowerAndClkSrc();

    /* Initialize dummy CAN IF */
    CanIf_Init(NULL);

    if (CANIF_DRV_INITIALIZED == CanIf_DrvStatus)
    {
        /* Initialize the task params */
        TaskP_Params_init(&taskParams);
        //taskParams.instance->name = CAN_PROFILE_DEMO_TASK_NAME;
        /* Set the task priority higher than the default priority (1) */
        taskParams.priority     = 2;
        taskParams.stack        = CanApp_TaskStack;
        taskParams.stacksize    = sizeof (CanApp_TaskStack);

        task = TaskP_create(&CanApp_TaskFxn, &taskParams);
        if(NULL == task)
        {
            OS_stop();
        }
        OS_start();    /* does not return */
    }
    else
    {
        AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
                " Error : CAN IF not initialized !!!\n");
    }

    return(0);
}

static void CanApp_TaskFxn(void* a0, void* a1)
{
    CanApp_PlatformInit();
    Utils_prfInit();

    Utils_prfLoadRegister (TaskP_self(), CAN_PROFILE_DEMO_TASK_NAME);

    CanApp_TestPrms.numTxPackets = APP_NUM_MSG_PER_ITERATION;
    CanApp_TestPrms.numIterations = APP_NUM_ITERATION;

    TaskP_yield();

    CanApp_ProfileTest(&CanApp_TestPrms);

    CanApp_Shutdown();

    Utils_prfLoadUnRegister (TaskP_self());
    Utils_prfDeInit();
    return;
}

/** \brief Application tear down functions */
static void CanApp_Shutdown(void)
{
    uint32 idx;

    for (idx = 0U; idx < CAN_MAX_CONTROLLER; idx++)
    {
        if (NULL != CanApp_IsrHndls[idx])
        {
            if (HwiP_OK != HwiP_delete(CanApp_IsrHndls[idx]))
            {
                AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
                                " Error!!! Could not De register"
                                " the ISR for instance %d!!!\n", idx);
                break;
            }
        }
    }

    if (NULL != CanIf_TxConfirmationSemaphore)
    {
        SemaphoreP_delete(CanIf_TxConfirmationSemaphore);
        if (NULL != CanIf_RxConfirmationSemaphore)
        {
            SemaphoreP_delete(CanIf_RxConfirmationSemaphore);
        }
    }
    return;
}

sint32 SetupSciServer(void)
{

    sint32 ret = CSL_PASS;
#if (defined (BUILD_MCU1_0) && (defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J784S4)))
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
