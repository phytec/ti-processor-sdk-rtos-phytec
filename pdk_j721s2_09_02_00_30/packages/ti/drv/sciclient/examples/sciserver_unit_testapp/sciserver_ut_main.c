/*
 *  Copyright (C) 2024 Texas Instruments Incorporated
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/**
 *  \file   sciserver_ut_main.c
 *
 *  \brief  Implementation of Sciserver Unit Test application
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <string.h>
#include <ti/drv/sciclient/src/sciclient/sciclient_priv.h>
#include <ti/drv/sciclient/sciserver.h>
#include <ti/drv/sciclient/src/sciserver/sciserver_secproxyTransfer.h>
#include <ti/drv/sciclient/examples/common/sci_app_common.h>
#include <ti/drv/sciclient/examples/sciserver_unit_testapp/sciserver_ut_tests.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* For SafeRTOS on R5F with FFI Support, task stack should be aligned to the stack size */
#if defined(SAFERTOS) && defined (BUILD_MCU)
static uint8_t  gSciserverApp_TskStackMain[32*1024] __attribute__((aligned(32*1024))) = { 0 };
#else
static uint8_t  gSciserverApp_TskStackMain[32*1024] __attribute__((aligned(8192)));
#endif
/* IMPORTANT NOTE: For C7x,
 * - stack size and stack ptr MUST be 8KB aligned
 * - AND min stack size MUST be 32KB
 * - AND stack assigned for task context is "size - 8KB"
 *       - 8KB chunk for the stack area is used for interrupt handling in this task context
 */

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                         Function Declarations                              */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                        Internal Function Declarations                      */
/* ========================================================================== */

void mainTask(void* arg0, void* arg1);
static int32_t SciserverApp_serverFuncNegTest(void);
static int32_t SciserverApp_secProxyTransferNegTest(void);
static int32_t SciserverApp_secproxyRoutingDescriptionNegTest(void);

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

int main(void)
{
    TaskP_Handle task;
    TaskP_Params taskParams;

    uint32_t retVal = CSL_PASS;

    /*  This should be called before any other OS calls (like Task creation, OS_start, etc..) */
    OS_init();

    memset( gSciserverApp_TskStackMain, 0xFF, sizeof( gSciserverApp_TskStackMain ) );
    TaskP_Params_init(&taskParams);
    taskParams.priority   = 2;
    taskParams.stack      = gSciserverApp_TskStackMain;
    taskParams.stacksize  = sizeof (gSciserverApp_TskStackMain);
    task = TaskP_create(&mainTask, &taskParams);
    if(task == NULL)
    {
        OS_stop();
    }

    OS_start();

    return retVal;
}

/* ========================================================================== */
/*                        Internal Function Definitions                       */
/* ========================================================================== */

void mainTask(void* arg0, void* arg1)
{
    /*To suppress unused variable warning*/
    (void)arg0;
    (void)arg1;

    volatile uint32_t loopForever = 1U;

    SciApp_parser();

    while(loopForever);
}

uint32_t SciApp_getNumTests(void)
{
    return SCISERVER_UT_NUM_TESTCASES;
}

int32_t SciApp_testMain(SciApp_TestParams_t *testParams)
{
    switch (testParams->testcaseId)
    {
        case 1:
            testParams->testResult = SciserverApp_serverFuncNegTest();
            break;
        case 2:
            testParams->testResult = SciserverApp_secProxyTransferNegTest();
            break;
        case 3:
            testParams->testResult = SciserverApp_secproxyRoutingDescriptionNegTest();
            break;
        default:
            break;
    }
    
    return 0;
}

static int32_t SciserverApp_serverFuncNegTest(void)
{
    int32_t status                  = CSL_PASS;
    int32_t sciserverFuncTestStatus = CSL_PASS;
    uint8_t setProcessState         = 1U;
    uint8_t getProcessState         = 0U;
    Sciserver_CfgPrms_t prms;

    /* Trying to deinit sciserver even if it was not init before to achieve fail condition */
    status = Sciserver_deinit();
    if(status == CSL_EFAIL)
    {
        sciserverFuncTestStatus += CSL_PASS;
        SciApp_printf(" Sciserver was not init \n");
    }
    else
    {
        sciserverFuncTestStatus += CSL_EFAIL;
        SciApp_printf(" Sciserver_deinit \n");
    }

    /* Passing NULL parameter to Sciserver_init*/
    status = Sciserver_init(NULL);
    if(status == CSL_EBADARGS)
    {
        sciserverFuncTestStatus += CSL_PASS;
        SciApp_printf(" Sciserver_init NULL Arg Test PASSED \n");
    }
    else
    {
        sciserverFuncTestStatus += CSL_EFAIL;
        SciApp_printf(" Sciserver_init NULL Arg Test FAILED \n");
    }

    /* Passing valid parameters to Sciserver_init */
    status = Sciserver_init(&prms);
    if(status == CSL_PASS)
    {
        sciserverFuncTestStatus += CSL_PASS;
        SciApp_printf(" Sciserver_init PASSED \n");
    }
    else
    {
        sciserverFuncTestStatus += CSL_EFAIL;
        SciApp_printf(" Sciserver_init FAILED \n");
    }

    /* Sciserver_init already done, trying to initialize it again to achieve fail condition */
    status = Sciserver_init(&prms);
    if(status == CSL_EFAIL)
    {
        sciserverFuncTestStatus += CSL_PASS;
        SciApp_printf(" Sciserver_init Already Done \n");
    }
    else
    {
        sciserverFuncTestStatus += CSL_EFAIL;
        SciApp_printf(" Sciserver_init Done \n");
    }

    Sciserver_setProcessState(setProcessState);
    getProcessState = Sciserver_getProcessState();
    if(getProcessState == setProcessState)
    {
        sciserverFuncTestStatus += CSL_PASS;
        SciApp_printf(" Sciserver_setProcessState, Sciserver_getProcessState Tests PASSED \n");
    }
    else
    {
        sciserverFuncTestStatus += CSL_EFAIL;
        SciApp_printf(" Sciserver_setProcessState, Sciserver_getProcessState Tests FAILED \n");
    }

    status = Sciserver_deinit();
    if(status == CSL_PASS)
    {
        sciserverFuncTestStatus += CSL_PASS;
        SciApp_printf(" Sciserver_deinit PASSED \n");
    }
    else
    {
        sciserverFuncTestStatus += CSL_EFAIL;
        SciApp_printf(" Sciserver_deinit FAILED \n");
    }

    status = Sciserver_interruptHandler(NULL, NULL);
    if(status == CSL_EFAIL)
    {
        sciserverFuncTestStatus += CSL_PASS;
        SciApp_printf(" Sciserver_interruptHandler Null Arg Test PASSED \n");
    }
    else
    {
        sciserverFuncTestStatus += CSL_EFAIL;
        SciApp_printf(" Sciserver_interruptHandler Null Arg Test FAILED \n");
    }

    return sciserverFuncTestStatus;
}

static int32_t SciserverApp_secProxyTransferNegTest(void)
{
  int32_t   status               = CSL_PASS;
  int32_t   sciserverInitStatus  = CSL_PASS;
  int32_t   secProxyTestStatus   = CSL_PASS;
  uint16_t  txConfigId           = 0U;
  uint32_t  *txBuff              = NULL;
  uint32_t  txNumWords           = 0x0FU;
  uint32_t  offset               = 0U;
  uint16_t  rxConfigId           = 0x55U;
  uint32_t  rxNumWords           = 0x0FU;
  Sciserver_CfgPrms_t prms;
  
   status = Sciserver_init(&prms);
   sciserverInitStatus = status;

   if(status == CSL_PASS)
   {
        SciApp_printf ("Sciserver_init PASSED.\n");
        status = Sciserver_SproxyMsgGetSenderHostId(0U, NULL);
        if (status == CSL_EFAIL)
        {
            secProxyTestStatus += CSL_PASS;
            SciApp_printf ("Sciserver_SproxyMsgGetSenderHostId: Negative Arg Test Passed.\n");
        }
        else
        {
           secProxyTestStatus += CSL_EFAIL;
           SciApp_printf ("Sciserver_SproxyMsgGetSenderHostId: Negative Arg Test Failed.\n");
        }

        status = Sciserver_SproxyMsgSendAtOffset(txConfigId, txBuff, txNumWords, offset);
        if (status == CSL_EFAIL)
        {
            secProxyTestStatus += CSL_PASS;
            SciApp_printf ("Sciserver_SproxyMsgSendAtOffset: txNumWords Negative Arg Test Passed.\n");
        }
        else
        {
           secProxyTestStatus += CSL_EFAIL;
           SciApp_printf ("Sciserver_SproxyMsgSendAtOffset: txNumWords Negative Arg Test Failed.\n");
        }

        txNumWords = 0U;
        status = Sciserver_SproxyMsgSendAtOffset(txConfigId, txBuff, txNumWords, offset);
        if (status == CSL_EFAIL)
        {
            secProxyTestStatus += CSL_PASS;
            SciApp_printf ("Sciserver_SproxyMsgSendAtOffset: txNumWords=0 Negative Arg Test Passed.\n");
        }
        else
        {
           secProxyTestStatus += CSL_EFAIL;
           SciApp_printf ("Sciserver_SproxyMsgSendAtOffset: txNumWords=0 Negative Arg Test Failed.\n");
        }

        txConfigId = 0xB2U;
        status = Sciserver_SproxyCanSend(txConfigId);
        if (status == CSL_EFAIL)
        {
            secProxyTestStatus += CSL_PASS;
            SciApp_printf ("Sciserver_SproxyTxIsReady: Negative Arg Test Passed.\n");
        }
        else
        {
           secProxyTestStatus += CSL_EFAIL;
           SciApp_printf ("Sciserver_SproxyTxIsReady: Negative Arg Test Failed.\n");
        }

        status = Sciserver_SproxyMsgIsPending(rxConfigId);
        if (status == CSL_EFAIL)
        {
            secProxyTestStatus += CSL_PASS;
            SciApp_printf ("Sciserver_SproxyRxIsReady: Negative Arg Test Passed.\n");
        }
        else
        {
           secProxyTestStatus += CSL_EFAIL;
           SciApp_printf ("Sciserver_SproxyRxIsReady: Negative Arg Test Failed.\n");
        }

        status = Sciserver_SproxyMsgReadAtOffset(rxConfigId, NULL, rxNumWords, offset);
        if (status == CSL_EFAIL)
        {
            secProxyTestStatus += CSL_PASS;
            SciApp_printf ("Sciserver_SproxyMsgReadAtOffset: Negative Arg Test Passed.\n");
        }
        else
        {
           secProxyTestStatus += CSL_EFAIL;
           SciApp_printf ("Sciserver_SproxyMsgReadAtOffset: Negative Arg Test Failed.\n");
        }
  }
  else
  {
      secProxyTestStatus += CSL_EFAIL;
      SciApp_printf ("Sciserver_init FAILED.\n");
  }

  if(sciserverInitStatus == CSL_PASS)
  {
      status = Sciserver_deinit();
      if(status == CSL_PASS)
      {
          secProxyTestStatus += CSL_PASS;
          SciApp_printf ("Sciserver_deinit PASSED.\n");
      }
      else
      {
          secProxyTestStatus += CSL_EFAIL;
          SciApp_printf ("Sciserver_deinit FAILED.\n");
      }
  }

  return secProxyTestStatus;
}

static int32_t SciserverApp_secproxyRoutingDescriptionNegTest(void)
{
    int32_t   status                = CSL_PASS;
    int32_t   sciserverInitStatus   = CSL_PASS;
    int32_t   routingDesTestStatus  = CSL_PASS;
    uint16_t  hostId                = 0xFFU;
    uint16_t  txConfigId            = 0U;
    Sciserver_CfgPrms_t prms;
  
     status = Sciserver_init(&prms);
     sciserverInitStatus = status;

     if(status == CSL_PASS)
     {
          SciApp_printf ("Sciserver_init PASSED.\n");
          status = Sciserver_SproxyGetResponseConfId(hostId, &txConfigId);
          if (status == CSL_EFAIL)
          {
              routingDesTestStatus += CSL_PASS;
              SciApp_printf ("Sciserver_SproxyConfLookup: Negative Arg Test Passed.\n");
          }
          else
          {
             routingDesTestStatus += CSL_EFAIL;
             SciApp_printf ("Sciserver_SproxyConfLookup: Negative Arg Test Failed.\n");
          }

          hostId = 0xFEU;
          status = Sciserver_SproxyGetNotifyConfId(hostId, &txConfigId);
          if (status == CSL_EFAIL)
          {
              routingDesTestStatus += CSL_PASS;
              SciApp_printf ("Sciserver_SproxyConfLookup: Negative Arg Test Passed.\n");
          }
          else
          {
             routingDesTestStatus += CSL_EFAIL;
             SciApp_printf ("Sciserver_SproxyConfLookup: Negative Arg Test Failed.\n");
          }

          hostId = 3U;
          status = Sciserver_SproxyGetNotifyConfId(hostId, &txConfigId);
          if (status == CSL_PASS)
          {
              routingDesTestStatus += CSL_PASS;
              SciApp_printf ("Sciserver_SproxyGetNotifyConfId: Test Passed.\n");
          }
          else
          {
             routingDesTestStatus += CSL_EFAIL;
             SciApp_printf ("Sciserver_SproxyGetNotifyConfId: Test Failed.\n");
          }
    }
    else
    {
        routingDesTestStatus += CSL_EFAIL;
        SciApp_printf ("Sciserver_init FAILED.\n");
    }

    if(sciserverInitStatus == CSL_PASS)
    {
        status = Sciserver_deinit();
        if(status == CSL_PASS)
        {
            routingDesTestStatus += CSL_PASS;
            SciApp_printf ("Sciserver_deinit PASSED.\n");
        }
        else
        {
            routingDesTestStatus += CSL_EFAIL;
            SciApp_printf ("Sciserver_deinit FAILED.\n");
        }
    }

  return routingDesTestStatus;
}

