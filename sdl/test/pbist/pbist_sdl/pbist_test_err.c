/*
 *   Copyright (c) Texas Instruments Incorporated 2021
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
 *  \file     sdl_pbist_test_err.c
 *
 *  \brief    This file contains PBIST Error module test code for R5 core.
 *
 *  \details  PBIST Error module tests
 **/

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <stdint.h>
#include <soc.h>
#include <sdl_types.h>
#include <src/sdl/sdl_pbist.h>
#include <ti/drv/uart/UART_stdio.h>
#include <ti/drv/sciclient/sciclient.h>

#include <ti/drv/uart/UART_stdio.h>

/* Osal API header files */
#include <ti/osal/HwiP.h>
#include <ti/osal/TimerP.h>

#include <pbist_test_cfg.h>

#include "power_seq.h"

/* This is to power up the cores before test and power down afterwards */
#define POWERUP_CORES_BEFORE_TEST

#include <osal_interface.h>
#include <test_osal_interface.h>
#include <src/sdl/pbist/sdl_pbist_priv.h>

/* ========================================================================== */
/*                                Macros                                      */
/* ========================================================================== */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
extern bool error_test_flag;
extern bool error_test_flag_2;

#if defined (SOC_J721E)
#define SDL_PCONFIG_TEST_INSTANCE  PBIST_INSTANCE_VPAC
#elif defined (SOC_J7200)
#define SDL_PCONFIG_TEST_INSTANCE  PBIST_INSTANCE_MSMC
#elif defined (SOC_J784S4)
#define SDL_PCONFIG_TEST_INSTANCE  PBIST_INSTANCE_VPAC_0
#endif

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */
int32_t PBIST_ERROR_runTest(uint32_t instanceId, bool runNegTest)
{
    int32_t testResult = 0;
    SDL_ErrType_t status;
    bool PBISTResult;
    SDL_PBIST_testType testType;

    uint64_t startTime , testStartTime,  testEndTime, endTime;
    uint64_t prepTime, diffTime, restoreTime;
    int i;
#ifdef DEBUG
    char inputChar;
#endif
    uint32_t moduleState = TISCI_MSG_VALUE_DEVICE_HW_STATE_OFF;
    uint32_t resetState = 0U;
    uint32_t contextLossState = 0U;
#ifdef PBIST_POST_CORE_MAX
    uint8_t  postStatus = PBIST_POST_COMPLETED_SUCCESS;
#endif
    SDL_PBIST_config pbistConfig;

    pbistConfig.override = 0x0u;
#if defined (SOC_J721E)
    pbistConfig.algorithmsBitMap = SDL_A72SS0_CORE0_PBIST_WRAP_ALGO_BITMAP_0;
    pbistConfig.memoryGroupsBitMap = SDL_A72SS0_CORE0_PBIST_WRAP_MEM_BITMAP_0;
#elif defined (SOC_J7200)
    pbistConfig.algorithmsBitMap = SDL_COMPUTE_CLUSTER0_ARM0_DFT_EMBED_PBIST_0_ALGO_BITMAP_0;
    pbistConfig.memoryGroupsBitMap = SDL_COMPUTE_CLUSTER0_ARM0_DFT_EMBED_PBIST_0_MEM_BITMAP_0;
#elif defined (SOC_J784S4)
    pbistConfig.algorithmsBitMap = SDL_COMPUTE_CLUSTER0_ARM0_DFT_EMBED_PBIST_0_ALGO_BITMAP_0;
    pbistConfig.memoryGroupsBitMap = SDL_COMPUTE_CLUSTER0_ARM0_DFT_EMBED_PBIST_0_MEM_BITMAP_0;
#endif
    pbistConfig.scrambleValue = 0xFEDCBA9876543210U;

    if (runNegTest == true)
    {
        UART_printf("\n Starting PBIST failure insertion test on %s, index %d...\n",
                    PBIST_TestHandleArray[instanceId].testName,
                    instanceId);
        testType = SDL_PBIST_NEG_TEST;
    }
    else
    {
        UART_printf("\n Starting PBIST test on %s, index %d...\n",
                    PBIST_TestHandleArray[instanceId].testName,
                    instanceId);
        testType = SDL_PBIST_TEST;
    }

#ifdef DEBUG
    UART_printf("\n Press any key to continue...");
    inputChar = UART_getc();

    if (inputChar == 'n')
    {
        UART_printf("\n Skipping this test. on request \n");
        return 0;
    }
#endif

    /* Get start time of test */
    startTime = TimerP_getTimeInUsecs();

#ifdef PBIST_POST_CORE_MAX

    if ((testResult == 0) &&
        (PBIST_TestHandleArray[instanceId].numPostPbistToCheck > 0))
    {
        uint32_t postRegVal;

#ifdef DEBUG
        UART_printf("  HW POST: Running test on HW POST, %d Instances \n",
                    PBIST_TestHandleArray[instanceId].numPostPbistToCheck);
#endif
        postRegVal = CSL_REG32_RD(CSL_WKUP_CTRL_MMR0_CFG0_BASE +
                                  CSL_WKUP_CTRL_MMR_CFG0_WKUP_POST_STAT);

        /* Check if HW POST PBIST was performed */
        status = PBIST_isPostPbistDone(postRegVal, &PBISTResult);
        if (status != SDL_PASS)
        {
            UART_printf("   HW POST: PBIST_isPostPbistDone failed\n");
            testResult = -1;
        }
        else
        {
            if (PBISTResult != true)
            {
                /* HW POST: PBIST not completed, check if it timed out */
                status = PBIST_isPostPbistTimeout(postRegVal, &PBISTResult);
                if (PBISTResult != true)
                {
                    /* HW POST: PBIST was not performed at all on this device */
                    postStatus = PBIST_POST_NOT_RUN;
                }
                else
                {
                    /* HW POST: PBIST was attempted but timed out */
                    postStatus = PBIST_POST_ATTEMPTED_TIMEOUT;
                    testResult = -1;
                }
            }
            else
            {
                /* HW POST: PBIST was completed on this device, check the result */
                status = PBIST_postCheckResult(postRegVal, &PBISTResult);
                if (PBISTResult != true)
                {
                    /* HW POST: PBIST was completed, but the test failed */
                    postStatus = PBIST_POST_COMPLETED_FAILURE;
                    testResult = -1;
                    UART_printf("\n HW POST: PBIST test failed\n");
                }
            } /* if (PBISTResult != true) */
        } /* if (status != SDL_PASS) */
    }
#endif  /* PBIST_POST_CORE_MAX */
    if ((testResult == 0) && (PBIST_TestHandleArray[instanceId].procRstNeeded))
    {
        if (PBIST_TestHandleArray[instanceId].tisciProcId != 0u)
        {
#ifdef DEBUG
            UART_printf("  Primary core: %s: Requesting processor \n",
                        PBIST_TestHandleArray[instanceId].coreName);
#endif
            /* Request Primary core */
            status = Sciclient_procBootRequestProcessor(PBIST_TestHandleArray[instanceId].tisciProcId,
                                                        SCICLIENT_SERVICE_WAIT_FOREVER);
            if (status != SDL_PASS)
            {
                UART_printf("   Primary core: Sciclient_procBootRequestProcessor, ProcId 0x%x...FAILED \n",
                            PBIST_TestHandleArray[instanceId].tisciProcId);
                testResult = -1;
            }
        }
    }

    if ((testResult == 0) && (PBIST_TestHandleArray[instanceId].procRstNeeded))
    {
        if ((PBIST_TestHandleArray[instanceId].secondaryCoreNeeded)
            && (PBIST_TestHandleArray[instanceId].tisciSecProcId != 0u))
        {

#ifdef DEBUG
            UART_printf("  Secondary core: %s: Requesting processor \n",
                    PBIST_TestHandleArray[instanceId].secCoreName);
#endif
            /* Request secondary core */
            status = Sciclient_procBootRequestProcessor(PBIST_TestHandleArray[instanceId].tisciSecProcId,
                                                        SCICLIENT_SERVICE_WAIT_FOREVER);
            if (status != SDL_PASS)
            {
                UART_printf("   Secondary core: Sciclient_procBootRequestProcessor, ProcId 0x%x...FAILED \n",
                            PBIST_TestHandleArray[instanceId].tisciSecProcId);
                testResult = -1;
            }
        }
    }

    if ((testResult == 0) && (PBIST_TestHandleArray[instanceId].procRstNeeded))
    {
        if (PBIST_TestHandleArray[instanceId].tisciDeviceId != 0u)
        {
            /* Set Local reset for Primary core */
#ifdef DEBUG
            UART_printf("  %s: Primary core: Set module reset \n",
                        PBIST_TestHandleArray[instanceId].coreName);
#endif
            status =  Sciclient_pmSetModuleRst(PBIST_TestHandleArray[instanceId].tisciDeviceId,
                                               0x1, /* Local Reset asserted */
                                               SCICLIENT_SERVICE_WAIT_FOREVER);
            if (status != SDL_PASS)
            {
                UART_printf("   Primary core: Sciclient_pmSetModuleRst...FAILED \n");
                testResult = -1;
            }
        }
    }

    if ((testResult == 0) && (PBIST_TestHandleArray[instanceId].procRstNeeded))
    {
        if ((PBIST_TestHandleArray[instanceId].secondaryCoreNeeded)
            && (PBIST_TestHandleArray[instanceId].tisciSecDeviceId != 0u))
        {
            /* Set Local reset for Secondary core */
#ifdef DEBUG
            UART_printf("  %s: Secondary core: Set Module reset \n",
                        PBIST_TestHandleArray[instanceId].secCoreName);
#endif
            status =  Sciclient_pmSetModuleRst(PBIST_TestHandleArray[instanceId].tisciSecDeviceId,
                                               0x1, /* Local Reset asserted */
                                               SCICLIENT_SERVICE_WAIT_FOREVER);
            if (status != SDL_PASS)
            {
                UART_printf("   Secondary core: Sciclient_pmSetModuleRst...FAILED \n");
                testResult = -1;
            }
        }
    }

#ifdef POWERUP_CORES_BEFORE_TEST
    /* Custom core power restore sequence - needed to allow core to be powered
     * up later by Secondary Bootloader (SBL) */
    if ((testResult == 0) &&
        (PBIST_TestHandleArray[instanceId].coreCustPwrSeqNeeded) &&
        (PBIST_TestHandleArray[instanceId].tisciProcId != 0u))
    {
        status = customPrepareForPowerUpSequence(PBIST_TestHandleArray[instanceId].tisciProcId);
        if (status != SDL_PASS)
        {
            UART_printf("  Custom core power restore sequence, ProcId 0x%x ...FAILED \n",
                        PBIST_TestHandleArray[instanceId].tisciProcId);
            testResult = -1;
        }
    }

    /* Power up of Auxilliary modules needed to run test */
    if (testResult == 0)
    {
        /* Power all modules required for test */
        for ( i = 0; i < PBIST_TestHandleArray[instanceId].numAuxDevices; i++)
        {
#ifdef DEBUG
            UART_printf("  Powering on Device number %d Device Id %x\n",
                        i, PBIST_TestHandleArray[instanceId].auxDeviceIdsP[i]);
#endif

            status = Sciclient_pmSetModuleState(PBIST_TestHandleArray[instanceId].auxDeviceIdsP[i],
                                                TISCI_MSG_VALUE_DEVICE_SW_STATE_ON,
                                                TISCI_MSG_FLAG_AOP,
                                                SCICLIENT_SERVICE_WAIT_FOREVER);
            if (status != SDL_PASS)
            {
                UART_printf("  Sciclient_pmSetModuleState 0x%x ...FAILED \n",
                            PBIST_TestHandleArray[instanceId].auxDeviceIdsP[i]);
                testResult = -1;
                break;
            }
        }
    }

    if ((testResult == 0) && (PBIST_TestHandleArray[instanceId].procRstNeeded)
                          && (PBIST_TestHandleArray[instanceId].tisciDeviceId != 0U))
    {
        /* power on Primary core*/
#ifdef DEBUG
        UART_printf("  Primary core: Powering on %s \n",
                    PBIST_TestHandleArray[instanceId].coreName);
#endif
        status = Sciclient_pmSetModuleState(PBIST_TestHandleArray[instanceId].tisciDeviceId,
                                            TISCI_MSG_VALUE_DEVICE_SW_STATE_ON,
                                            TISCI_MSG_FLAG_AOP,
                                            SCICLIENT_SERVICE_WAIT_FOREVER);

        if (status != SDL_PASS)
        {
            UART_printf("   Primary core: Sciclient_pmSetModuleState 0x%x ...FAILED \n",
                        PBIST_TestHandleArray[instanceId].tisciDeviceId);
            testResult = -1;
        }
    }

    if ((testResult == 0) && (PBIST_TestHandleArray[instanceId].procRstNeeded)
                          && (PBIST_TestHandleArray[instanceId].tisciSecDeviceId != 0U))
    {
        if (PBIST_TestHandleArray[instanceId].secondaryCoreNeeded)
        {
            /* power on Secondary core*/
#ifdef DEBUG
            UART_printf("  Secondary core: Powering on %s \n",
                        PBIST_TestHandleArray[instanceId].secCoreName);
#endif
            status = Sciclient_pmSetModuleState(PBIST_TestHandleArray[instanceId].tisciSecDeviceId,
                                                TISCI_MSG_VALUE_DEVICE_SW_STATE_ON,
                                                TISCI_MSG_FLAG_AOP,
                                                SCICLIENT_SERVICE_WAIT_FOREVER);
            if (status != SDL_PASS)
            {
                UART_printf("   Secondary core: Sciclient_pmSetModuleState 0x%x ...FAILED \n",
                            PBIST_TestHandleArray[instanceId].tisciSecDeviceId);
                testResult = -1;
                return testResult;
            }
        }
    }
#endif /* #ifdef POWERUP_CORES_BEFORE_TEST */
    /* Double check the Power up of Auxilliary modules needed to run test and wait until they
     * are powered up */
    if (testResult == 0)
    {
        /* Wait for all modules required for test to be powered up */
        for ( i = 0; i < PBIST_TestHandleArray[instanceId].numAuxDevices; i++)
        {
#ifdef DEBUG
        UART_printf(
                        "  Double checking Powering on Device number %d Device Id %x\n",
                        i, PBIST_TestHandleArray[instanceId].auxDeviceIdsP[i]);
#endif
            do
            {
                status = Sciclient_pmGetModuleState(PBIST_TestHandleArray[instanceId].auxDeviceIdsP[i],
                                                    &moduleState,
                                                    &resetState,
                                                    &contextLossState,
                                                    SCICLIENT_SERVICE_WAIT_FOREVER);
                if (status != SDL_PASS)
                {
                    UART_printf("  Sciclient_pmGetModuleState 0x%x ...FAILED \n",
                                PBIST_TestHandleArray[instanceId].auxDeviceIdsP[i]);
                    testResult = -1;
                    break;
                }
            } while (moduleState != TISCI_MSG_VALUE_DEVICE_HW_STATE_ON);
        }
    }

    if ((testResult == 0) && (PBIST_TestHandleArray[instanceId].procRstNeeded)
                          && (PBIST_TestHandleArray[instanceId].tisciDeviceId != 0U))
    {
        /* Double check power on Primary core*/
#ifdef DEBUG
        UART_printf(
                        "  Primary core: Double checking Powering on %s \n",
                        PBIST_TestHandleArray[instanceId].coreName);
#endif
        do
        {
            status = Sciclient_pmGetModuleState(PBIST_TestHandleArray[instanceId].tisciDeviceId,
                                                &moduleState,
                                                &resetState,
                                                &contextLossState,
                                                SCICLIENT_SERVICE_WAIT_FOREVER);
            if (status != SDL_PASS)
            {
                UART_printf("   Primary core: Sciclient_pmGetModuleState 0x%x ...FAILED \n",
                            PBIST_TestHandleArray[instanceId].tisciDeviceId);
                testResult = -1;
                break;
            }
        } while (moduleState != TISCI_MSG_VALUE_DEVICE_HW_STATE_ON);
    }

    if ((testResult == 0) && (PBIST_TestHandleArray[instanceId].procRstNeeded)
                          && (PBIST_TestHandleArray[instanceId].tisciSecDeviceId != 0U))
    {
        if (PBIST_TestHandleArray[instanceId].secondaryCoreNeeded)
        {
            /* Double check power on Secondary core*/
#ifdef DEBUG
            UART_printf(
                            "  Secondary core: Double checking Powering on %s \n",
                            PBIST_TestHandleArray[instanceId].coreName);
#endif
            do
            {
                status = Sciclient_pmGetModuleState(PBIST_TestHandleArray[instanceId].tisciSecDeviceId,
                                                    &moduleState,
                                                    &resetState,
                                                    &contextLossState,
                                                    SCICLIENT_SERVICE_WAIT_FOREVER);
                if (status != SDL_PASS)
                {
                    UART_printf("   Secondary core: Sciclient_pmGetModuleState 0x%x ...FAILED \n",
                                PBIST_TestHandleArray[instanceId].tisciSecDeviceId);
                    testResult = -1;
                    break;
                }
            } while (moduleState != TISCI_MSG_VALUE_DEVICE_HW_STATE_ON);
        }
    }

    /* Power up PBIST */
    if ((testResult == 0) && (PBIST_TestHandleArray[instanceId].tisciPBISTDeviceId != 0u))
    {
#ifdef DEBUG
        UART_printf("  Powering on PBIST %d \n",
                    PBIST_TestHandleArray[instanceId].tisciPBISTDeviceId);
#endif
        status = Sciclient_pmSetModuleState(PBIST_TestHandleArray[instanceId].tisciPBISTDeviceId,
                                            TISCI_MSG_VALUE_DEVICE_SW_STATE_ON,
                                            TISCI_MSG_FLAG_AOP,
                                            SCICLIENT_SERVICE_WAIT_FOREVER);

        if (status != SDL_PASS)
        {
            UART_printf("   PBIST Sciclient_pmSetModuleState 0x%x ...FAILED: retValue %d\n",
                        PBIST_TestHandleArray[instanceId].tisciPBISTDeviceId, status);
            testResult = -1;
        }
    }

    /* Execute Auxilliary init function */
    if (testResult == 0)
    {

        if (PBIST_TestHandleArray[instanceId].auxInitRestoreFunction != 0)
        {
            status = PBIST_TestHandleArray[instanceId].auxInitRestoreFunction(TRUE);
            if (status != SDL_PASS)
            {
                testResult = -1;
            }
        }

    }

    /* Get start time for PBIST test */
    testStartTime = TimerP_getTimeInUsecs();

    status = SDL_PBIST_selfTest((SDL_PBIST_inst)PBIST_TestHandleArray[instanceId].pbistInst, testType, &pbistConfig, &PBISTResult);
    if ((status != SDL_PASS) || (PBISTResult == false))
    {
        testResult = -1;
    }

    /* Record test end time */
    testEndTime = TimerP_getTimeInUsecs();

    /* Execute Auxilliary restore function */
    if (testResult == 0)
    {

        if (PBIST_TestHandleArray[instanceId].auxInitRestoreFunction != 0)
        {
            status = PBIST_TestHandleArray[instanceId].auxInitRestoreFunction(FALSE);
            if (status != SDL_PASS)
            {
                testResult = -1;
            }
        }

    }

    /* The following sequence is needed to restore core to normal operation */
    /* Power off PBIST */
    if ((testResult == 0) && (PBIST_TestHandleArray[instanceId].tisciPBISTDeviceId != 0u))
    {
#ifdef DEBUG
        UART_printf("  Powering off PBIST %d \n",
                    PBIST_TestHandleArray[instanceId].tisciPBISTDeviceId);
#endif
        status = Sciclient_pmSetModuleState(PBIST_TestHandleArray[instanceId].tisciPBISTDeviceId,
                                            TISCI_MSG_VALUE_DEVICE_SW_STATE_AUTO_OFF,
                                            TISCI_MSG_FLAG_AOP,
                                            SCICLIENT_SERVICE_WAIT_FOREVER);

        if (status != SDL_PASS)
        {
            UART_printf("   PBIST Sciclient_pmSetModuleState 0x%x ...FAILED \n",
                            PBIST_TestHandleArray[instanceId].tisciPBISTDeviceId);
            testResult = -1;
        }
    }

#ifdef POWERUP_CORES_BEFORE_TEST
    if ((testResult == 0) && (PBIST_TestHandleArray[instanceId].procRstNeeded)
                          && (PBIST_TestHandleArray[instanceId].tisciSecDeviceId != 0U))
    {
        if (PBIST_TestHandleArray[instanceId].secondaryCoreNeeded)
        {
            /* power off Secondary core*/
#ifdef DEBUG
            UART_printf("  Secondary core: Powering off %s \n",
                        PBIST_TestHandleArray[instanceId].secCoreName);
#endif
            status = Sciclient_pmSetModuleState(PBIST_TestHandleArray[instanceId].tisciSecDeviceId,
                                                TISCI_MSG_VALUE_DEVICE_SW_STATE_AUTO_OFF,
                                                TISCI_MSG_FLAG_AOP,
                                                SCICLIENT_SERVICE_WAIT_FOREVER);
            if (status != SDL_PASS)
            {
                UART_printf("   Secondary core: Sciclient_pmSetModuleState Power off 0x%x ...FAILED \n",
                            PBIST_TestHandleArray[instanceId].tisciSecDeviceId);
                testResult = -1;
                return testResult;
            }
        }
    }

    /* Custom core power down sequence */
    if ((testResult == 0) &&
        (PBIST_TestHandleArray[instanceId].coreCustPwrSeqNeeded) &&
        (PBIST_TestHandleArray[instanceId].tisciProcId != 0u))
    {
        status = customPowerDownSequence(PBIST_TestHandleArray[instanceId].tisciProcId);
        if (status != SDL_PASS)
        {
            UART_printf("  Custom core power down sequence, ProcId 0x%x ...FAILED \n",
                        PBIST_TestHandleArray[instanceId].tisciProcId);
            testResult = -1;
        }
    }

    if ((testResult == 0) && (PBIST_TestHandleArray[instanceId].tisciProcId != 0u)
                    && (PBIST_TestHandleArray[instanceId].procRstNeeded))
    {
        /* power off Primary core*/
#ifdef DEBUG
        UART_printf("  Primary core: Powering off %s \n",
                    PBIST_TestHandleArray[instanceId].coreName);
#endif
        status = Sciclient_pmSetModuleState(PBIST_TestHandleArray[instanceId].tisciDeviceId,
                                            TISCI_MSG_VALUE_DEVICE_SW_STATE_AUTO_OFF,
                                            TISCI_MSG_FLAG_AOP,
                                            SCICLIENT_SERVICE_WAIT_FOREVER);

        if (status != SDL_PASS)
        {
            UART_printf("   Primary core: Sciclient_pmSetModuleState Power off 0x%x ...FAILED \n",
                        PBIST_TestHandleArray[instanceId].tisciDeviceId);
            testResult = -1;
        }
    }

    /* Power off of Auxilliary modules needed to run test */
    if (testResult == 0)
    {
        /* Power all modules required for test */
        for ( i = 0; i < PBIST_TestHandleArray[instanceId].numAuxDevices; i++)
        {
#ifdef DEBUG
            UART_printf("  Powering off Device number %d Device Id %x\n",
                        i, PBIST_TestHandleArray[instanceId].auxDeviceIdsP[i]);
#endif
            status = Sciclient_pmSetModuleState(PBIST_TestHandleArray[instanceId].auxDeviceIdsP[i],
                                                TISCI_MSG_VALUE_DEVICE_SW_STATE_AUTO_OFF,
                                                TISCI_MSG_FLAG_AOP,
                                                SCICLIENT_SERVICE_WAIT_FOREVER);
            if (status != SDL_PASS)
            {
                UART_printf("  Sciclient_pmSetModuleState 0x%x ...FAILED \n",
                            PBIST_TestHandleArray[instanceId].auxDeviceIdsP[i]);
                testResult = -1;
                break;
            }
        }
    }

    /* Custom core power restore sequence - needed to allow core to be powered
     * up properly later */
    if ((testResult == 0) &&
        (PBIST_TestHandleArray[instanceId].coreCustPwrSeqNeeded) &&
        (PBIST_TestHandleArray[instanceId].tisciProcId != 0u))
    {
        status = customPrepareForPowerUpSequence(PBIST_TestHandleArray[instanceId].tisciProcId);
        if (status != SDL_PASS)
        {
            UART_printf("  Custom core power restore sequence, ProcId 0x%x ...FAILED \n",
                        PBIST_TestHandleArray[instanceId].tisciProcId);
            testResult = -1;
        }
    }

    /* Take Primary core out of local reset */
    if ((testResult == 0) && (PBIST_TestHandleArray[instanceId].procRstNeeded)
                          && (PBIST_TestHandleArray[instanceId].tisciDeviceId != 0U))
    {
#ifdef DEBUG
        UART_printf("  Primary core: Taking out of local reset the core %s \n",
                    PBIST_TestHandleArray[instanceId].coreName);
#endif
        status = Sciclient_pmSetModuleRst(PBIST_TestHandleArray[instanceId].tisciDeviceId,
                                          0x0, /* Local Reset de-asserted */
                                          SCICLIENT_SERVICE_WAIT_FOREVER);
        if (status != SDL_PASS)
        {
             UART_printf("  Sciclient_pmSetModuleRst 0x%x ...FAILED \n",
                         PBIST_TestHandleArray[instanceId].tisciDeviceId);
             testResult = -1;
        }
    }

    /* Take Secondary core out of local reset */
    if ((testResult == 0) && (PBIST_TestHandleArray[instanceId].procRstNeeded)
                          && (PBIST_TestHandleArray[instanceId].tisciSecDeviceId != 0U))
    {
#ifdef DEBUG
        UART_printf("  Secondary core: Taking out of local reset the core %s \n",
                    PBIST_TestHandleArray[instanceId].secCoreName);
#endif
        status = Sciclient_pmSetModuleRst(PBIST_TestHandleArray[instanceId].tisciSecProcId,
                                          0x0, /* Local Reset de-asserted */
                                          SCICLIENT_SERVICE_WAIT_FOREVER);
        if (status != SDL_PASS)
        {
             UART_printf("  Sciclient_pmSetModuleRst 0x%x ...FAILED \n",
                         PBIST_TestHandleArray[instanceId].tisciSecDeviceId);
             testResult = -1;
        }
    }
#endif /* #ifdef POWERUP_CORES_BEFORE_TEST */

    /* Ensure that cores have been turned off */
    if ((testResult == 0) && (PBIST_TestHandleArray[instanceId].procRstNeeded))
    {
        if (PBIST_TestHandleArray[instanceId].tisciDeviceId != 0u)
        {
            /* Set Software Reset Disable State for Primary core */
#ifdef DEBUG
            UART_printf("  %s: Primary core: Put in Software Reset Disable \n",
                        PBIST_TestHandleArray[instanceId].coreName);
#endif
            status =  Sciclient_pmSetModuleState(PBIST_TestHandleArray[instanceId].tisciDeviceId,
                                                 TISCI_MSG_VALUE_DEVICE_SW_STATE_AUTO_OFF,
                                                 TISCI_MSG_FLAG_AOP,
                                                 SCICLIENT_SERVICE_WAIT_FOREVER);
            if (status != SDL_PASS)
            {
                UART_printf("   Primary core: Sciclient_pmSetModuleState...FAILED \n");
                testResult = -1;
            }
        }
    }

    if ((testResult == 0) && (PBIST_TestHandleArray[instanceId].procRstNeeded))
    {
        if ((PBIST_TestHandleArray[instanceId].secondaryCoreNeeded)
            && (PBIST_TestHandleArray[instanceId].tisciSecDeviceId != 0u))
        {
            /* Set Software Reset Disable State for Secondary core */
#ifdef DEBUG
            UART_printf("  %s: Secondary Core Put in Software Reset Disable \n",
                        PBIST_TestHandleArray[instanceId].secCoreName);
#endif
            status =  Sciclient_pmSetModuleState(PBIST_TestHandleArray[instanceId].tisciSecDeviceId,
                                                 TISCI_MSG_VALUE_DEVICE_SW_STATE_AUTO_OFF,
                                                 TISCI_MSG_FLAG_AOP,
                                                 SCICLIENT_SERVICE_WAIT_FOREVER);
            if (status != SDL_PASS)
            {
                UART_printf("   Secondary core Sciclient_pmSetModuleState...FAILED \n");
                testResult = -1;
            }
        }
    }

    if ((testResult == 0) && (PBIST_TestHandleArray[instanceId].tisciProcId != 0u)
            && (PBIST_TestHandleArray[instanceId].procRstNeeded))
    {
        /* release processor Primary core */
#ifdef DEBUG
        UART_printf("  Primary core: Releasing %s \n",
                    PBIST_TestHandleArray[instanceId].coreName);
#endif

        status = Sciclient_procBootReleaseProcessor(PBIST_TestHandleArray[instanceId].tisciProcId,
                                                    TISCI_MSG_FLAG_AOP,
                                                    SCICLIENT_SERVICE_WAIT_FOREVER);
        if (status != SDL_PASS)
        {
            UART_printf("   Primary core: Sciclient_procBootReleaseProcessor, ProcId 0x%x...FAILED \n",
                        PBIST_TestHandleArray[instanceId].tisciProcId);
            testResult = -1;
        }
    }

    if (testResult == 0)
    {
        if ((PBIST_TestHandleArray[instanceId].secondaryCoreNeeded)
            && (PBIST_TestHandleArray[instanceId].tisciSecProcId != 0u)
            && (PBIST_TestHandleArray[instanceId].procRstNeeded))
        {
            /* release processor Secondary core */
#ifdef DEBUG
            UART_printf("  Secondary core: Releasing %s \n",
                        PBIST_TestHandleArray[instanceId].secCoreName);
#endif
            status = Sciclient_procBootReleaseProcessor(PBIST_TestHandleArray[instanceId].tisciSecProcId,
                                                        TISCI_MSG_FLAG_AOP,
                                                        SCICLIENT_SERVICE_WAIT_FOREVER);
            if (status != SDL_PASS)
            {
                UART_printf("   Secondary core: Sciclient_procBootReleaseProcessor, ProcId 0x%x...FAILED \n",
                            PBIST_TestHandleArray[instanceId].tisciSecProcId);
                testResult = -1;
            }
        }
    }

    /* Record end time */
    endTime = TimerP_getTimeInUsecs();

    prepTime = testStartTime - startTime;
    diffTime = testEndTime - testStartTime;
    restoreTime = endTime - testEndTime;
    UART_printf("  Delta Cores prep time in micro secs %d \n", (uint32_t)prepTime );
    UART_printf("  Delta PBIST execution time in micro secs %d \n", (uint32_t)diffTime );
    UART_printf("  Delta Cores restore time in micro secs %d \n", (uint32_t)restoreTime );
    UART_printf(" PBIST complete %s, test index %d\n",
                PBIST_TestHandleArray[instanceId].testName,
                instanceId);
#ifdef PBIST_POST_CORE_MAX
    if (PBIST_TestHandleArray[instanceId].numPostPbistToCheck > 0)
    {
        switch(postStatus)
        {
            case PBIST_POST_COMPLETED_FAILURE:
                UART_printf("\n HW POST: PBIST test failed\n");
                break;

            case PBIST_POST_ATTEMPTED_TIMEOUT:
                UART_printf("\n HW POST: PBIST was attempted but timed out\n");
                break;

            case PBIST_POST_NOT_RUN:
                UART_printf("\n HW POST: PBIST was not performed on this device\n");
                break;

            case PBIST_POST_COMPLETED_SUCCESS:
            default:
                UART_printf("\n HW POST: PBIST ran and succeeded\n");
                break;
        }
    }
#endif /* PBIST_POST_CORE_MAX */
    return (testResult);
}

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

static int32_t PBIST_errNegativeTest(void)
{
    int32_t                    testResult = SDL_PASS;
    SDL_ErrType_t sdlRet;
    SDL_PBIST_config PBISTConfig;
    bool PBISTresult;

    sdlRet = SDL_PBIST_getPOSTStatus(NULL);
    if (sdlRet == SDL_PASS)
    {
        UART_printf("\n  SDL_PBIST_getPOSTStatus negative test failed \n");
        testResult = -1;
    }

    if (testResult == 0)
    {
        sdlRet = SDL_PBIST_selfTest((SDL_PBIST_inst)SDL_PBIST_NUM_INSTANCES, SDL_PBIST_TEST, &PBISTConfig, &PBISTresult);
        if (sdlRet == SDL_PASS)
        {
            UART_printf("\n  SDL_PBIST_selfTest negative test failed \n");
            testResult = -1;
        }
    }

#if defined (SOC_J721E) || defined (SOC_J7200)
    if (testResult == 0)
    {
        #if defined (SOC_J721E) || defined (SOC_J7200)
        sdlRet = SDL_PBIST_selfTest(SDL_PBIST_INST_A72, (SDL_PBIST_testType)(SDL_PBIST_TEST_OF_ROM + 1), &PBISTConfig, &PBISTresult);
        #endif
        if (sdlRet == SDL_PASS)
        {
            UART_printf("\n  SDL_PBIST_selfTest negative test failed \n");
            testResult = -1;
        }
    }

    if (testResult == 0)
    {
	/* Note: config may be NULL */
        sdlRet = SDL_PBIST_selfTest(SDL_PBIST_INST_A72, SDL_PBIST_TEST, NULL, NULL);
        if (sdlRet == SDL_PASS)
        {
            UART_printf("\n  SDL_PBIST_selfTest negative test failed \n");
            testResult = -1;
        }
    }
#endif
#if defined (SOC_J784S4)
    if (testResult == 0)
    {
        sdlRet = SDL_PBIST_selfTest(SDL_PBIST_INST_A72_0_0, (SDL_PBIST_testType)(SDL_PBIST_NEG_TEST + 2), &PBISTConfig, &PBISTresult);
        if (sdlRet == SDL_PASS)
        {
            UART_printf("\n  SDL_PBIST_selfTest negative test failed \n");
            testResult = -1;
        }
    }

    if (testResult == 0)
    {
	/* Note: config may be NULL */
        sdlRet = SDL_PBIST_selfTest(SDL_PBIST_INST_A72_0_0, SDL_PBIST_TEST, NULL, NULL);
        if (sdlRet == SDL_PASS)
        {
            UART_printf("\n  SDL_PBIST_selfTest negative test failed \n");
            testResult = -1;
        }
    }
#endif
    if (testResult == 0)
    {
        sdlRet = SDL_PBIST_selfTest((SDL_PBIST_inst)SDL_PBIST_NUM_INSTANCES, SDL_PBIST_NEG_TEST, NULL, &PBISTresult);
        if (sdlRet == SDL_PASS)
        {
            UART_printf("\n  SDL_PBIST_selfTest negative test failed \n");
            testResult = -1;
        }
    }
#if defined (SOC_J721E) || defined (SOC_J7200)
    if (testResult == 0)
    {
        sdlRet = SDL_PBIST_selfTest(SDL_PBIST_INST_A72, SDL_PBIST_NEG_TEST, NULL, NULL);
        if (sdlRet == SDL_PASS)
        {
            UART_printf("\n  SDL_PBIST_selfTest negative test failed \n");
            testResult = -1;
        }
    }

    /* Tests with invalid OSALs */
    if (testResult == 0)
    {
        PBIST_TEST1_osalInit();
        sdlRet = SDL_PBIST_selfTest(SDL_PBIST_INST_A72, SDL_PBIST_TEST, NULL, &PBISTresult);
        if (sdlRet == SDL_PASS)
        {
            UART_printf("\n  SDL_PBIST_selfTest negative test failed \n");
            testResult = -1;
        }
    }

    if (testResult == 0)
    {
        PBIST_TEST2_osalInit();
        sdlRet = SDL_PBIST_selfTest(SDL_PBIST_INST_A72, SDL_PBIST_TEST, NULL, &PBISTresult);
        if (sdlRet == SDL_PASS)
        {
            UART_printf("\n  SDL_PBIST_selfTest negative test failed \n");
            testResult = -1;
        }
    }

    if (testResult == 0)
    {
        PBIST_TEST3_osalInit();
        sdlRet = SDL_PBIST_selfTest(SDL_PBIST_INST_A72, SDL_PBIST_TEST, NULL, &PBISTresult);
        if (sdlRet == SDL_PASS)
        {
            UART_printf("\n  SDL_PBIST_selfTest negative test failed \n");
            testResult = -1;
        }
    }
#endif	
#if defined (SOC_J784S4)
    if (testResult == 0)
    {
        sdlRet = SDL_PBIST_selfTest(SDL_PBIST_INST_A72_0_0, SDL_PBIST_NEG_TEST, NULL, NULL);
        if (sdlRet == SDL_PASS)
        {
            UART_printf("\n  SDL_PBIST_selfTest negative test failed \n");
            testResult = -1;
        }
    }

    /* Tests with invalid OSALs */
    if (testResult == 0)
    {
        PBIST_TEST1_osalInit();
        sdlRet = SDL_PBIST_selfTest(SDL_PBIST_INST_A72_0_0, SDL_PBIST_TEST, NULL, &PBISTresult);
        if (sdlRet == SDL_PASS)
        {
            UART_printf("\n  SDL_PBIST_selfTest negative test failed \n");
            testResult = -1;
        }
    }

    if (testResult == 0)
    {
        PBIST_TEST2_osalInit();
        sdlRet = SDL_PBIST_selfTest(SDL_PBIST_INST_A72_0_0, SDL_PBIST_TEST, NULL, &PBISTresult);
        if (sdlRet == SDL_PASS)
        {
            UART_printf("\n  SDL_PBIST_selfTest negative test failed \n");
            testResult = -1;
        }
    }

    if (testResult == 0)
    {
        PBIST_TEST3_osalInit();
        sdlRet = SDL_PBIST_selfTest(SDL_PBIST_INST_A72_0_0, SDL_PBIST_TEST, NULL, &PBISTresult);
        if (sdlRet == SDL_PASS)
        {
            UART_printf("\n  SDL_PBIST_selfTest negative test failed \n");
            testResult = -1;
        }
    }
#endif	
    if (testResult == 0)
    {
        sdlRet = SDL_PBIST_translateClecToIntr(0x0u);
        if (sdlRet != 0u)
        {
            UART_printf("\n SDL_PBIST_translateClecToIntr test failed \n");
            testResult = -1;
        }
    }
#if defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J784S4)
    if (testResult == 0)
    {
        UART_printf("\n Running pConfig test\n");
        SDL_TEST_osalInit();
        PBIST_ERROR_runTest(SDL_PCONFIG_TEST_INSTANCE, false);
	// don't check the result, this is for test only
    }

#if defined (SOC_J721E)
    /* These tests are only for when the SOC uses Clec programming */
    error_test_flag = true;
    sdlRet = SDL_PBIST_selfTest(SDL_PBIST_INST_A72, SDL_PBIST_TEST, NULL, &PBISTresult);
    error_test_flag = false;

    error_test_flag_2 = true;
    sdlRet = SDL_PBIST_selfTest(SDL_PBIST_INST_A72, SDL_PBIST_TEST, NULL, &PBISTresult);
    error_test_flag_2 = false;
#endif
#if defined (SOC_J784S4)
    /* These tests are only for when the SOC uses Clec programming */
    error_test_flag = true;
    sdlRet = SDL_PBIST_selfTest(SDL_PBIST_INST_A72_0_0, SDL_PBIST_TEST, NULL, &PBISTresult);
    error_test_flag = false;

    error_test_flag_2 = true;
    sdlRet = SDL_PBIST_selfTest(SDL_PBIST_INST_A72_0_0, SDL_PBIST_TEST, NULL, &PBISTresult);
    error_test_flag_2 = false;
#endif
#endif

    // override the interrupt handler to force "timeout"
    PBIST_TEST4_osalInit();
#if defined (SOC_J721E) || defined (SOC_J7200)	
    sdlRet = SDL_PBIST_selfTest(SDL_PBIST_INST_A72, SDL_PBIST_TEST, NULL, &PBISTresult);
#endif	
#if defined (SOC_J784S4)
    sdlRet = SDL_PBIST_selfTest(SDL_PBIST_INST_A72_0_0, SDL_PBIST_TEST, NULL, &PBISTresult);
#endif	
    return (testResult);
}

/* PBIST Error module test */
int32_t PBIST_errTest(void)
{
    int32_t    testResult;

    testResult = PBIST_errNegativeTest();

    return (testResult);
}

/* Nothing past this point */
