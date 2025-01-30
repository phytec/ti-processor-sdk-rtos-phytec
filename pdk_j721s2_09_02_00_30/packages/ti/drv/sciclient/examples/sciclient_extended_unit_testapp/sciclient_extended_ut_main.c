/*
 *  Copyright (C) 2024 Texas Instruments Incorporated - http://www.ti.com/
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
  *  \file  sciclient_extended_ut_main.c
  *
  *  \brief Implementation of Sciclient Extended Unit Test application
  *
  */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <string.h>
#include <ti/drv/sciclient/src/sciclient/sciclient_priv.h>
#include <ti/drv/sciclient/examples/common/sci_app_common.h>
#include <ti/drv/sciclient/examples/sciclient_extended_unit_testapp/sciclient_extended_ut_tests.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

extern Sciclient_ServiceHandle_t gSciclientHandle;

/* For SafeRTOS on R5F with FFI Support, task stack should be aligned to the stack size */
#if defined(SAFERTOS) && defined (BUILD_MCU)
static uint8_t  gSciclientAppTskStackMain[32*1024] __attribute__((aligned(32*1024))) = { 0 };
#else
static uint8_t  gSciclientAppTskStackMain[32*1024] __attribute__((aligned(8192)));
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

static void mainTask(void* arg0, void* arg1);
#if defined (BUILD_MCU1_0)
static int32_t SciclientApp_pmMessageTest(void);
#endif
static int32_t SciclientApp_msmcQueryNegTest(void);
static int32_t SciclientApp_otpProcessKeyCfgNegTest(void);
static int32_t SciclientApp_dkekNegTest(void);
static int32_t SciclientApp_firewallNegTest(void);
static int32_t SciclientApp_prepareHeaderNegTest(void);
#if defined (SOC_J784S4)
static int32_t SciclientApp_contextNegTest(void);
#endif
static int32_t SciclientApp_initTest(void);

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

    memset(gSciclientAppTskStackMain, 0xFF, sizeof(gSciclientAppTskStackMain));
    TaskP_Params_init(&taskParams);
    taskParams.priority     = 2;
    taskParams.stack        = gSciclientAppTskStackMain;
    taskParams.stacksize    = sizeof(gSciclientAppTskStackMain);
    task = TaskP_create(&mainTask, &taskParams);
    if(task == NULL)
    {
        OS_stop();
    }

    OS_start();

    return retVal;
}

static void mainTask(void* arg0, void* arg1)
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
    return SCICLIENT_NUM_TESTCASES;
}

int32_t SciApp_testMain(SciApp_TestParams_t *testParams)
{
    switch (testParams->testcaseId)
    {
#if defined (BUILD_MCU1_0)
        case 1:
            testParams->testResult = SciclientApp_pmMessageTest();
            break;
#endif
        case 2:
            testParams->testResult = SciclientApp_msmcQueryNegTest();
            break;
        case 3:
            testParams->testResult = SciclientApp_otpProcessKeyCfgNegTest();
            break;
        case 4:
            testParams->testResult = SciclientApp_dkekNegTest();
            break;
        case 5:
            testParams->testResult = SciclientApp_firewallNegTest();
            break;
        case 6:
            testParams->testResult =  SciclientApp_prepareHeaderNegTest();
            break;
#if defined (SOC_J784S4)
        case 7:
            testParams->testResult =  SciclientApp_contextNegTest();
            break;
#endif
        case 8:
            testParams->testResult =  SciclientApp_initTest();
            break;
        default:
            break;
    }
    
    return 0;
}

/* ========================================================================== */
/*                          Internal Function Definitions                     */
/* ========================================================================== */

#if defined (BUILD_MCU1_0)
static int32_t SciclientApp_pmMessageTest(void)
{
    int32_t   status               = CSL_PASS;
    int32_t   sciclientInitStatus  = CSL_PASS;
    int32_t   pmMessageTestStatus  = CSL_PASS;
    uint64_t  reqFreq              = 164UL;
    uint64_t  respFreq             = 0UL;
    uint32_t  clockStatus          = 1U;
#if defined(SOC_J721S2) || defined(SOC_J784S4)
    uint32_t  parentStatus         = 0U;
    uint32_t  numParents           = 0U;
#endif
    uint64_t  freq                 = 0UL;
    uint32_t  moduleState          = 0U;
    uint32_t  resetState           = 0U;
    uint32_t  contextLossState     = 0U;
    Sciclient_ConfigPrms_t  config =
    {
        SCICLIENT_SERVICE_OPERATION_MODE_POLLED,
        NULL,
        1U,
        0U,
        TRUE
    };

    while (gSciclientHandle.initCount != 0)
    {
        status = Sciclient_deinit();
    }
    status = Sciclient_init(&config);
    sciclientInitStatus = status;

    if (status == CSL_PASS)
    {
        SciApp_printf("Sciclient_Init Passed.\n");
        status = Sciclient_pmQueryModuleClkFreq(TISCI_DEV_UART1,
                                                TISCI_DEV_UART1_FCLK_CLK,
                                                reqFreq,
                                                &respFreq,
                                                SCICLIENT_SERVICE_WAIT_FOREVER);
        if (status == CSL_PASS)
        {
            pmMessageTestStatus += CSL_PASS;
            SciApp_printf("Sciclient_pmQueryModuleClkFreq Test Passed.\n");
        }
        else
        {
            pmMessageTestStatus += CSL_EFAIL;
            SciApp_printf("Sciclient_pmQueryModuleClkFreq Test Failed.\n");
        }

        status = Sciclient_pmModuleGetClkStatus(TISCI_DEV_UART1,
                                                TISCI_DEV_UART1_FCLK_CLK,
                                                &clockStatus,
                                                SCICLIENT_SERVICE_WAIT_FOREVER);
        if (status == CSL_PASS)
        {
            pmMessageTestStatus += CSL_PASS;
            SciApp_printf("Sciclient_pmModuleGetClkStatus Test Passed.\n");
        }
        else
        {
            pmMessageTestStatus += CSL_EFAIL;
            SciApp_printf("Sciclient_pmModuleGetClkStatus Test Failed.\n");
        }

        status = Sciclient_pmModuleClkRequest(TISCI_DEV_UART1,
                                              TISCI_DEV_UART1_FCLK_CLK,
                                              TISCI_MSG_VALUE_CLOCK_HW_STATE_READY,
                                              0U,
                                              SCICLIENT_SERVICE_WAIT_FOREVER);
        if (status == CSL_PASS)
        {
            pmMessageTestStatus += CSL_PASS;
            SciApp_printf("Sciclient_pmModuleClkRequest Test Passed.\n");
        }
        else
        {
            pmMessageTestStatus += CSL_EFAIL;
            SciApp_printf("Sciclient_pmModuleClkRequest Test Failed.\n");
        }

#if defined(SOC_J721S2) || defined(SOC_J784S4)
       status = Sciclient_pmSetModuleClkParent(TISCI_DEV_MCSPI1,
                                               TISCI_DEV_MCSPI1_IO_CLKSPII_CLK,
                                               TISCI_DEV_MCSPI1_IO_CLKSPII_CLK_PARENT_BOARD_0_SPI1_CLK_OUT,
                                               SCICLIENT_SERVICE_WAIT_FOREVER);
        if (status == CSL_PASS)
        {
            pmMessageTestStatus += CSL_PASS;
            SciApp_printf("Sciclient_pmSetModuleClkParent Test Passed.\n");
        }
        else
        {
            pmMessageTestStatus += CSL_EFAIL;
            SciApp_printf("Sciclient_pmSetModuleClkParent Test Failed.\n");
        }

        status = Sciclient_pmGetModuleClkParent(TISCI_DEV_MCSPI1,
                                                TISCI_DEV_MCSPI1_IO_CLKSPII_CLK,
                                                &parentStatus,
                                                SCICLIENT_SERVICE_WAIT_FOREVER);
        if ((status == CSL_PASS) && (parentStatus == TISCI_DEV_MCSPI1_IO_CLKSPII_CLK_PARENT_BOARD_0_SPI1_CLK_OUT))
        {
            pmMessageTestStatus += CSL_PASS;
            SciApp_printf("Sciclient_pmGetModuleClkParent Test Passed.\n");
        }
        else
        {
            pmMessageTestStatus += CSL_EFAIL;
            SciApp_printf("Sciclient_pmGetModuleClkParent Test Failed.\n");
        }

        status = Sciclient_pmGetModuleClkNumParent(TISCI_DEV_MCSPI1_IO_CLKSPII_CLK,
                                                  TISCI_DEV_MCSPI1_IO_CLKSPII_CLK_PARENT_BOARD_0_SPI1_CLK_OUT,
                                                  &numParents,
                                                  SCICLIENT_SERVICE_WAIT_FOREVER);
        if (status == CSL_PASS)
        {
            pmMessageTestStatus += CSL_PASS;
            SciApp_printf("Sciclient_pmGetModuleClkNumParent Test Passed.\n");
        }
        else
        {
            pmMessageTestStatus += CSL_EFAIL;
            SciApp_printf("Sciclient_pmGetModuleClkNumParent Test Failed.\n");
        }
#endif

        status = Sciclient_pmSetModuleClkFreq(TISCI_DEV_UART1,
                                              TISCI_DEV_UART1_FCLK_CLK,
                                              reqFreq,
                                              TISCI_MSG_FLAG_CLOCK_ALLOW_FREQ_CHANGE,
                                              SCICLIENT_SERVICE_WAIT_FOREVER);
        if (status == CSL_PASS)
        {
            pmMessageTestStatus += CSL_PASS;
            SciApp_printf("Sciclient_pmSetModuleClkFreq Test Passed.\n");
        }
        else
        {
            pmMessageTestStatus += CSL_EFAIL;
            SciApp_printf("Sciclient_pmSetModuleClkFreq Test Failed.\n");
        }

        status = Sciclient_pmGetModuleClkFreq(TISCI_DEV_UART1,
                                              TISCI_DEV_UART1_FCLK_CLK,
                                              &freq,
                                              SCICLIENT_SERVICE_WAIT_FOREVER);
        if (status == CSL_PASS)
        {
            pmMessageTestStatus += CSL_PASS;
            SciApp_printf("Sciclient_pmGetModuleClkFreq Test Passed.\n");
        }
        else
        {
            pmMessageTestStatus += CSL_EFAIL;
            SciApp_printf("Sciclient_pmGetModuleClkFreq Test Failed.\n");
        }

        status = Sciclient_pmSetModuleState(SCICLIENT_DEV_MCU_R5FSS0_CORE0,
                                            TISCI_MSG_VALUE_DEVICE_SW_STATE_ON,
                                            0U,
                                            SCICLIENT_SERVICE_WAIT_FOREVER);
        if (status == CSL_PASS)
        {
            pmMessageTestStatus += CSL_PASS;
            SciApp_printf("Sciclient_pmSetModuleState: SCICLIENT_DEV_MCU_R5FSS0_CORE0 Test Passed.\n");
        }
        else
        {
            pmMessageTestStatus += CSL_EFAIL;
            SciApp_printf("Sciclient_pmSetModuleState: SCICLIENT_DEV_MCU_R5FSS0_CORE0 Test Failed.\n");
        }

        status = Sciclient_pmSetModuleState(SCICLIENT_DEV_MCU_R5FSS0_CORE1,
                                            TISCI_MSG_VALUE_DEVICE_SW_STATE_ON,
                                            0U,
                                            SCICLIENT_SERVICE_WAIT_FOREVER);
        if (status == CSL_PASS)
        {
            pmMessageTestStatus += CSL_PASS;
            SciApp_printf("Sciclient_pmSetModuleState: SCICLIENT_DEV_MCU_R5FSS0_CORE1 Test Passed.\n");
        }
        else
        {
            pmMessageTestStatus += CSL_EFAIL;
            SciApp_printf("Sciclient_pmSetModuleState: SCICLIENT_DEV_MCU_R5FSS0_CORE1 Test Failed.\n");
        }

        status = Sciclient_pmSetModuleState(TISCI_DEV_BOARD0,
                                            TISCI_MSG_VALUE_DEVICE_SW_STATE_ON,
                                            TISCI_MSG_FLAG_AOP,
                                            SCICLIENT_SERVICE_WAIT_FOREVER);
        if (status == CSL_PASS)
        {
            pmMessageTestStatus += CSL_PASS;
            SciApp_printf("TISCI_DEV_BOARD0 TISCI_MSG_VALUE_DEVICE_SW_STATE_ON Test Passed.\n");
        }
        else
        {
            pmMessageTestStatus += CSL_EFAIL;
            SciApp_printf("TISCI_DEV_BOARD0 TISCI_MSG_VALUE_DEVICE_SW_STATE_ON Test Failed.\n");
        }

        status = Sciclient_pmGetModuleState(SCICLIENT_DEV_MCU_R5FSS0_CORE0,
                                            &moduleState,
                                            &resetState,
                                            &contextLossState,
                                            SCICLIENT_SERVICE_WAIT_FOREVER);
        if (status == CSL_PASS)
        {
            pmMessageTestStatus += CSL_PASS;
            SciApp_printf("SCICLIENT_DEV_MCU_R5FSS0_CORE0 States: \n");
            SciApp_printf("ModuleState: %d\n", moduleState);
            SciApp_printf("ResetState: %d\n", resetState);
            SciApp_printf("ContextLossState: %d\n", contextLossState);
            SciApp_printf("Sciclient_pmGetModuleState Test Passed.\n");
        }
        else
        {
            pmMessageTestStatus += CSL_EFAIL;
            SciApp_printf("Sciclient_pmGetModuleState Test Failed.\n");
        }
    }
    else
    {
        pmMessageTestStatus += CSL_EFAIL;
        SciApp_printf("Sciclient_Init Failed.\n");
    }

    if (sciclientInitStatus == CSL_PASS)
    {
        status = Sciclient_deinit();
        if(status == CSL_PASS)
        {
            pmMessageTestStatus += CSL_PASS;
            SciApp_printf("Sciclient_deinit Passed.\n");
        }
        else
        {
            pmMessageTestStatus += CSL_EFAIL;
            SciApp_printf("Sciclient_deinit Failed.\n");
        }
    }

  return pmMessageTestStatus;
}
#endif

static int32_t SciclientApp_msmcQueryNegTest(void)
{
    int32_t status                 = CSL_PASS;
    int32_t sciclientInitStatus    = CSL_PASS;
    int32_t msmcQueryTestStatus    = CSL_PASS;
    struct  tisci_query_msmc_resp resp;
    Sciclient_ConfigPrms_t  config =
    {
        SCICLIENT_SERVICE_OPERATION_MODE_INTERRUPT,
        NULL,
        0 /* isSecure = 0 un secured for all cores */
    };

    while (gSciclientHandle.initCount != 0)
    {
        status = Sciclient_deinit();
    }
    status = Sciclient_init(&config);
    sciclientInitStatus = status;

    if(status == CSL_PASS)
    {
        status = Sciclient_msmcQuery(NULL, &resp, SCICLIENT_SERVICE_WAIT_FOREVER);
        if (status == CSL_EFAIL)
        {
           msmcQueryTestStatus += CSL_PASS;
           SciApp_printf("Sciclient_msmcQuery: Negative Arg Test Passed.\n");
        }
        else
        {
           msmcQueryTestStatus += CSL_EFAIL;
           SciApp_printf("Sciclient_msmcQuery: Negative Arg Test Failed.\n");
        }
    }
    else
    {
        msmcQueryTestStatus += CSL_EFAIL;
        SciApp_printf("Sciclient_init FAILED.\n");
    }

    if(sciclientInitStatus == CSL_PASS)
    {
        status = Sciclient_deinit();
        if(status == CSL_PASS)
        {
            msmcQueryTestStatus += CSL_PASS;
            SciApp_printf("Sciclient_deinit PASSED.\n");
        }
        else
        {
            msmcQueryTestStatus += CSL_EFAIL;
            SciApp_printf("Sciclient_deinit FAILED.\n");
        }
    }

  return msmcQueryTestStatus;
}

static int32_t SciclientApp_otpProcessKeyCfgNegTest(void)
{
    int32_t   status                  = CSL_PASS;
    int32_t   sciclientInitStatus     = CSL_PASS;
    int32_t   otpProcessKeyTestStatus = CSL_PASS;
    uint32_t  resp                    = 0U;
    Sciclient_ConfigPrms_t  config    =
    {
       SCICLIENT_SERVICE_OPERATION_MODE_INTERRUPT,
       NULL,
       0 /* isSecure = 0 un secured for all cores */
    };

     while (gSciclientHandle.initCount != 0)
     {
         status = Sciclient_deinit();
     }
     status = Sciclient_init(&config);
     sciclientInitStatus = status;

     if(status == CSL_PASS)
     {
        SciApp_printf("Sciclient_init PASSED.\n");
        status = Sciclient_otpProcessKeyCfg(NULL, SCICLIENT_SERVICE_WAIT_FOREVER, &resp);
        if (status == CSL_EFAIL)
        {
            otpProcessKeyTestStatus += CSL_PASS;
            SciApp_printf("Sciclient_otpProcessKeyCfg: Negative Arg Test Passed.\n");
        }
        else
        {
           otpProcessKeyTestStatus += CSL_EFAIL;
           SciApp_printf("Sciclient_otpProcessKeyCfg: Negative Arg Test Failed.\n");
        }
    }
    else
    {
        otpProcessKeyTestStatus += CSL_EFAIL;
        SciApp_printf("Sciclient_init FAILED.\n");
    }

    if(sciclientInitStatus == CSL_PASS)
    {
        status = Sciclient_deinit();
        if(status == CSL_PASS)
        {
            otpProcessKeyTestStatus += CSL_PASS;
            SciApp_printf("Sciclient_deinit PASSED.\n");
        }
        else
        {
            otpProcessKeyTestStatus += CSL_EFAIL;
            SciApp_printf("Sciclient_deinit FAILED.\n");
        }
    }

  return otpProcessKeyTestStatus;
}

static int32_t SciclientApp_dkekNegTest(void)
{
    int32_t status                = CSL_PASS;
    int32_t sciclientInitStatus   = CSL_PASS;
    int32_t dkekTestStatus        = CSL_PASS;
    Sciclient_ConfigPrms_t config =
    {
       SCICLIENT_SERVICE_OPERATION_MODE_INTERRUPT,
       NULL,
       0 /* isSecure = 0 un secured for all cores */
    };

     while (gSciclientHandle.initCount != 0)
     {
         status = Sciclient_deinit();
     }
     status = Sciclient_init(&config);
     sciclientInitStatus = status;

     if(status == CSL_PASS)
     {
        SciApp_printf("Sciclient_init PASSED.\n");
        status = Sciclient_setDKEK(NULL, NULL, SCICLIENT_SERVICE_WAIT_FOREVER);
        if (status == CSL_EFAIL)
        {
            dkekTestStatus += CSL_PASS;
            SciApp_printf("Sciclient_setDKEK: Negative Arg Test Passed.\n");
        }
        else
        {
           dkekTestStatus += CSL_EFAIL;
           SciApp_printf("Sciclient_setDKEK: Negative Arg Test Failed.\n");
        }

        status = Sciclient_releaseDKEK(NULL, NULL, SCICLIENT_SERVICE_WAIT_FOREVER);
        if (status == CSL_EFAIL)
        {
            dkekTestStatus += CSL_PASS;
            SciApp_printf("Sciclient_releaseDKEK: Negative Arg Test Passed.\n");
        }
        else
        {
           dkekTestStatus += CSL_EFAIL;
           SciApp_printf("Sciclient_releaseDKEK: Negative Arg Test Failed.\n");
        }

        status = Sciclient_getDKEK(NULL, NULL, SCICLIENT_SERVICE_WAIT_FOREVER);
        if (status == CSL_EFAIL)
        {
            dkekTestStatus += CSL_PASS;
            SciApp_printf("Sciclient_getDKEK: Negative Arg Test Passed.\n");
        }
        else
        {
           dkekTestStatus += CSL_EFAIL;
           SciApp_printf("Sciclient_getDKEK: Negative Arg Test Failed.\n");
        }
    }
    else
    {
        dkekTestStatus += CSL_EFAIL;
        SciApp_printf("Sciclient_init FAILED.\n");
    }

    if(sciclientInitStatus == CSL_PASS)
    {
        status = Sciclient_deinit();
        if(status == CSL_PASS)
        {
            dkekTestStatus += CSL_PASS;
            SciApp_printf("Sciclient_deinit PASSED.\n");
        }
        else
        {
            dkekTestStatus += CSL_EFAIL;
            SciApp_printf("Sciclient_deinit FAILED.\n");
        }
    }

  return dkekTestStatus;
}

static int32_t SciclientApp_firewallNegTest(void)
{
    int32_t status                = CSL_PASS;
    int32_t sciclientInitStatus   = CSL_PASS;
    int32_t firewallNegTestStatus = CSL_PASS;
    Sciclient_ConfigPrms_t SciApp_Config =
    {
       SCICLIENT_SERVICE_OPERATION_MODE_INTERRUPT,
       NULL,
       0 /* isSecure = 0 un secured for all cores */
    };

    while (gSciclientHandle.initCount != 0U)
    {
        status = Sciclient_deinit();
    }
    status = Sciclient_init(&SciApp_Config);
    sciclientInitStatus = status;

    if(status == CSL_PASS)
    {
        SciApp_printf ("Sciclient_init PASSED.\n");

        status = Sciclient_firewallChangeOwnerInfo(NULL, NULL, SCICLIENT_SERVICE_WAIT_FOREVER);
        if (status == CSL_EFAIL)
        {
            firewallNegTestStatus += CSL_PASS;
            SciApp_printf ("Sciclient_firewallChangeOwnerInfo: Negative Arg Test Passed.\n");
        }
        else
        {
            firewallNegTestStatus += CSL_EFAIL;
            SciApp_printf ("Sciclient_firewallChangeOwnerInfo: Negative Arg Test Failed.\n");
        }

        status = Sciclient_firewallSetRegion(NULL, NULL, SCICLIENT_SERVICE_WAIT_FOREVER);
        if (status == CSL_EFAIL)
        {
            firewallNegTestStatus += CSL_PASS;
            SciApp_printf ("Sciclient_firewallSetRegion: Negative Arg Test Passed.\n");
        }
        else
        {
            firewallNegTestStatus += CSL_EFAIL;
            SciApp_printf ("Sciclient_firewallSetRegion: Negative Arg Test Failed.\n");
        }

        status = Sciclient_firewallGetRegion(NULL, NULL, SCICLIENT_SERVICE_WAIT_FOREVER);
        if (status == CSL_EFAIL)
        {
            firewallNegTestStatus += CSL_PASS;
            SciApp_printf ("Sciclient_firewallGetRegion: Negative Arg Test Passed.\n");
        }
        else
        {
            firewallNegTestStatus += CSL_EFAIL;
            SciApp_printf ("Sciclient_firewallGetRegion: Negative Arg Test Failed.\n");
        }
     }
     else
     {
         firewallNegTestStatus += CSL_EFAIL;
         SciApp_printf ("Sciclient_init FAILED.\n");
     }

    if(sciclientInitStatus == CSL_PASS)
    {
        status = Sciclient_deinit();
        if(status == CSL_PASS)
        {
            firewallNegTestStatus += CSL_PASS;
            SciApp_printf ("Sciclient_deinit PASSED.\n");
        }
        else
        {
            firewallNegTestStatus += CSL_EFAIL;
            SciApp_printf ("Sciclient_deinit FAILED.\n");
        }
    }

   return firewallNegTestStatus;
}

static int32_t SciclientApp_prepareHeaderNegTest(void)
{
    int32_t  status              = CSL_PASS;
    int32_t  sciclientInitStatus = CSL_PASS;
    int32_t  sciclientTestStatus = CSL_PASS;
    const Sciclient_ReqPrm_t SciApp_ReqPrm = {};
    Sciclient_ConfigPrms_t   SciApp_Config =
    {
       SCICLIENT_SERVICE_OPERATION_MODE_INTERRUPT,
       NULL,
       0 /* isSecure = 0 un secured for all cores */
    };

     while (gSciclientHandle.initCount != 0U)
     {
         status = Sciclient_deinit();
     }
     status = Sciclient_init(&SciApp_Config);
     sciclientInitStatus = status;

     if(status == CSL_PASS)
     {
          SciApp_printf("Sciclient_init PASSED.\n");
          status = Sciclient_servicePrepareHeader(NULL, NULL, SCICLIENT_CONTEXT_R5_NONSEC_0, NULL);
          if (status == CSL_EBADARGS)
          {
               sciclientTestStatus += CSL_PASS;
               SciApp_printf("Sciclient_servicePrepareHeader: Negative Arg Test Passed.\n");
          }
          else
          {
               sciclientTestStatus += CSL_EFAIL;
               SciApp_printf("Sciclient_servicePrepareHeader: Negative Arg Test Failed.\n");
          }

          status = Sciclient_servicePrepareHeader(&SciApp_ReqPrm, NULL, SCICLIENT_CONTEXT_MAX_NUM, NULL);
          if (status == CSL_EBADARGS)
          {
               sciclientTestStatus += CSL_PASS;
               SciApp_printf("Sciclient_servicePrepareHeader: Negative Arg Test Passed.\n");
          }
          else
          {
               sciclientTestStatus += CSL_EFAIL;
               SciApp_printf("Sciclient_servicePrepareHeader: Negative Arg Test Failed.\n");
          }
    }
    else
    {
        sciclientInitStatus += CSL_EFAIL;
        SciApp_printf ("Sciclient_init FAILED.\n");
    }

    if(sciclientInitStatus == CSL_PASS)
    {
        status = Sciclient_deinit();
        if(status == CSL_PASS)
        {
            sciclientTestStatus += CSL_PASS;
            SciApp_printf("Sciclient_deinit PASSED.\n");
        }
        else
        {
            sciclientTestStatus += CSL_EFAIL;
            SciApp_printf("Sciclient_deinit FAILED.\n");
        }
    }

  return sciclientTestStatus;
}

#if defined (SOC_J784S4)
static int32_t SciclientApp_contextNegTest(void)
{
    int32_t  status              = CSL_PASS;
    int32_t  sciclientInitStatus = CSL_PASS;
    int32_t  sciclientTestStatus = CSL_PASS;
    uint32_t IntrNum1            = CSLR_MCU_R5FSS0_CORE0_INTR_MCU_NAVSS0_INTR_ROUTER_0_OUTL_INTR_1;
    uint32_t IntrNum2            = CSLR_MCU_R5FSS0_CORE0_INTR_MCU_MCAN0_MCANSS_MCAN_LVL_INT_0;
    uint32_t IntrNum3            = CSLR_MCU_R5FSS0_CORE0_INTR_MCU_CPSW0_EVNT_PEND_0;
    uint16_t messagetype[4]      = {TISCI_MSG_BOOT_NOTIFICATION, 
                                    TISCI_MSG_BOARD_CONFIG, 
                                    TISCI_MSG_BOARD_CONFIG_SECURITY
                                    };
    int8_t   num;
    Sciclient_ConfigPrms_t SciApp_Config =
    {
       SCICLIENT_SERVICE_OPERATION_MODE_INTERRUPT,
       NULL,
       0 /* isSecure = 0 un secured for all cores */
    };

     while (gSciclientHandle.initCount != 0U)
     {
         status = Sciclient_deinit();
     }
     status = Sciclient_init(&SciApp_Config);
     sciclientInitStatus = status;

     if(status == CSL_PASS)
     {
          SciApp_printf("Sciclient_init PASSED.\n");
        /* Passing different interrupt numbers to check proxy map context id for 'gSciclientMap' */
          status = Sciclient_contextIdFromIntrNum(IntrNum1);
          if (status == 0U)
          {
               sciclientTestStatus += CSL_PASS;
               SciApp_printf("Sciclient_contextIdFromIntrNum initial condition Test Passed.\n");
          }
          else
          {
               sciclientTestStatus += CSL_EFAIL;
               SciApp_printf("Sciclient_contextIdFromIntrNum initial condition Test Failed.\n");
          }

          status = Sciclient_contextIdFromIntrNum(IntrNum2);
          if (status != CSL_EFAIL)
          {
               sciclientTestStatus += CSL_PASS;
               SciApp_printf("Sciclient_contextIdFromIntrNum: IntrNum2 Arg Test Passed.\n");
          }
          else
          {
               sciclientTestStatus += CSL_EFAIL;
               SciApp_printf("Sciclient_contextIdFromIntrNum: IntrNum2 Arg Test Failed.\n");
          }

          status = Sciclient_contextIdFromIntrNum(IntrNum3);
          if (status == CSL_EFAIL)
          {
               sciclientTestStatus += CSL_PASS;
               SciApp_printf("Sciclient_contextIdFromIntrNum: Negative Arg Test Passed.\n");
          }
          else
          {
               sciclientTestStatus += CSL_EFAIL;
               SciApp_printf("Sciclient_contextIdFromIntrNum: Negative Arg Test Failed.\n");
          }

          /* Passing different message types to determine the which context to be used.*/
          for(num = 0; num < 3; num++)
          {
            status = Sciclient_getCurrentContext(messagetype[num]);
            if (status == SCICLIENT_CONTEXT_SEC)
            {
                sciclientTestStatus += CSL_PASS;
                SciApp_printf("Sciclient_getCurrentContext: Secure Arg Test Passed.\n");
            }
            else
            {
                sciclientTestStatus += CSL_EFAIL;
                SciApp_printf("Sciclient_getCurrentContext: Secure Arg Test Failed.\n");
            }
          }
    }
    else
    {
        sciclientInitStatus += CSL_EFAIL;
        SciApp_printf ("Sciclient_init FAILED.\n");
    }

    if(sciclientInitStatus == CSL_PASS)
    {
        status = Sciclient_deinit();
        if(status == CSL_PASS)
        {
            sciclientTestStatus += CSL_PASS;
            SciApp_printf("Sciclient_deinit PASSED.\n");
        }
        else
        {
            sciclientTestStatus += CSL_EFAIL;
            SciApp_printf("Sciclient_deinit FAILED.\n");
        }
    }

  return sciclientTestStatus;
}
#endif

static int32_t SciclientApp_initTest(void)
{
    int32_t status              = CSL_PASS;
    int32_t sciclientInitStatus = CSL_PASS;
    Sciclient_ConfigPrms_t SciApp_Config =
    {
       2,
       NULL,
       0 /* isSecure = 0 un secured for all cores */
    };

    Sciclient_ConfigPrms_t SciApp_ConfigIntrMode =
    {
       SCICLIENT_SERVICE_OPERATION_MODE_INTERRUPT,
       NULL,
       2 /* isSecure = 0 un secured for all cores */
    };

    status = Sciclient_init(&SciApp_Config);
    if (status == CSL_EBADARGS)
    {
        sciclientInitStatus += CSL_PASS;
        SciApp_printf("Sciclient_init: Negative Arg Test Passed.\n");
    }
    else
    {
        sciclientInitStatus += CSL_EFAIL;
        SciApp_printf("Sciclient_init: Negative Arg Test Failed.\n");
    }

    Sciclient_deinit();
    status = Sciclient_init(&SciApp_ConfigIntrMode);
    if (status == CSL_EBADARGS)
    {
        sciclientInitStatus += CSL_PASS;
        SciApp_printf("Sciclient_init: Negative Arg Test Passed.\n");
    }
    else
    {
        sciclientInitStatus += CSL_EFAIL;
        SciApp_printf("Sciclient_init: Negative Arg Test Failed.\n");
    }

    status = Sciclient_deinit();
    if(status == CSL_PASS)
    {
        sciclientInitStatus += CSL_PASS;
        SciApp_printf("Sciclient_deinit PASSED.\n");
    }
    else
    {
        sciclientInitStatus += CSL_EFAIL;
        SciApp_printf("Sciclient_deinit FAILED.\n");
    }

    return sciclientInitStatus;
}

#if defined(BUILD_MPU) || defined (BUILD_C7X)
extern void Osal_initMmuDefault(void);
void InitMmu(void)
{
    Osal_initMmuDefault();
}
#endif

