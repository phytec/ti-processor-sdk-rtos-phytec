/*
 *   Copyright (c) Texas Instruments Incorporated 2019-2021
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
 *  \file     lbist_test_err.c
 *
 *  \brief    This file contains LBIST module error tests.
 *
 *  \details  LBIST Error tests
 **/

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <stdint.h>
#include <sdl_types.h>
#include <src/sdl/sdl_lbist.h>
#include <ti/drv/uart/UART_stdio.h>
#include <src/ip/sdl_ip_lbist.h>
#include <ti/drv/sciclient/sciclient.h>
#include <osal_interface.h>
#include <ti/osal/TimerP.h>

#include <test_osal_interface.h>
#include <src/sdl/lbist/sdl_lbist_priv.h>

#include "lbist_test_cfg.h"

/* ========================================================================== */
/*                                Macros                                      */
/* ========================================================================== */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */
int32_t LBIST_ERR_runTest(uint32_t coreIndex)
{
    int32_t testResult = 0;
    int32_t status;
    uint64_t startTime , testStartTime,  testEndTime, endTime;
    uint64_t prepTime, diffTime, restoreTime;
    bool result = false;

    UART_printf("\n Starting LBIST test on %s, index %d...",
                LBIST_TestHandleArray[coreIndex].coreName,
                coreIndex);
#ifdef DEBUG
    char inputChar;

    UART_printf("\n Press 'n' to skip..Press any key to continue...");
    inputChar = UART_getc();

    if (inputChar == 'n')
    {
        UART_printf("   Skipping this test. on request \n");
        return 0;
    }
#endif

#ifdef DEBUG
    UART_printf("\n  HwiP_Params_init complete \n");
#endif

    /* Get start time of test */
    startTime = TimerP_getTimeInUsecs();

    /*-- Step 2: Configure processor to correct state --*/
    /* SW-initiated LBIST test flow */

    /**--- Step 2a: Request Primary core ---*/
    if (testResult == 0)
    {
        if (LBIST_TestHandleArray[coreIndex].tisciProcId != 0u)
        {
#ifdef DEBUG
            UART_printf("  Primary core: %s: Requesting processor \n",
                        LBIST_TestHandleArray[coreIndex].coreName);
#endif
            /* Request Primary core */
            status = Sciclient_procBootRequestProcessor(LBIST_TestHandleArray[coreIndex].tisciProcId,
                                                        SCICLIENT_SERVICE_WAIT_FOREVER);
            if (status != SDL_PASS)
            {
                UART_printf("   Primary core: Sciclient_procBootRequestProcessor, ProcId 0x%x...FAILED : Status %d\n",
                            LBIST_TestHandleArray[coreIndex].tisciProcId, status);
                testResult = -1;
            }
        }
    }

    /**--- Step 2b: Request Secondary core ---*/
    if (testResult == 0)
    {
        if ((LBIST_TestHandleArray[coreIndex].secondaryCoreNeeded)
            && (LBIST_TestHandleArray[coreIndex].tisciSecProcId != 0u))
        {

#ifdef DEBUG
            UART_printf("  Secondary core: %s: Requesting processor \n",
                        LBIST_TestHandleArray[coreIndex].secCoreName);
#endif
            /* Request secondary core */
            status = Sciclient_procBootRequestProcessor(LBIST_TestHandleArray[coreIndex].tisciSecProcId,
                                                        SCICLIENT_SERVICE_WAIT_FOREVER);
            if (status != SDL_PASS)
            {
                UART_printf("   Secondary core: Sciclient_procBootRequestProcessor, ProcId 0x%x...FAILED \n",
                            LBIST_TestHandleArray[coreIndex].tisciSecProcId);
                testResult = -1;
            }
        }
    }

    /**--- Step 2c: Place all Auxilliary modules needed to run test into module reset ---*/
    if (testResult == 0)
    {
        int i;

        /* Place all Auxilliary modules required for test into module reset */
        for ( i = 0; i < LBIST_TestHandleArray[coreIndex].numAuxDevices; i++)
        {
#ifdef DEBUG
            UART_printf("  Putting into module reset Device number %d Device Id %x\n",
                        i,
                        LBIST_TestHandleArray[coreIndex].auxDeviceIdsP[i]);
#endif

            status = Sciclient_pmSetModuleRst(LBIST_TestHandleArray[coreIndex].auxDeviceIdsP[i],
                                              0x2, /* Module Reset asserted */
                                              SCICLIENT_SERVICE_WAIT_FOREVER);
            if (status != SDL_PASS)
            {
                UART_printf("  Sciclient_pmSetModuleState 0x%x ...FAILED \n",
                            LBIST_TestHandleArray[coreIndex].auxDeviceIdsP[i]);
                testResult = -1;
                break;
            }
        }
    }

    /**--- Step 2d: Put Primary core in module reset and local reset ---*/
    if ((testResult == 0) && (LBIST_TestHandleArray[coreIndex].tisciDeviceId != 0U))
    {
#ifdef DEBUG
        UART_printf("  Primary core: Putting in module and local reset the core %s \n",
                    LBIST_TestHandleArray[coreIndex].coreName);
#endif
        status = Sciclient_pmSetModuleRst(LBIST_TestHandleArray[coreIndex].tisciDeviceId,
                                          0x3, /* Module Reset and Local Reset asserted */
                                          SCICLIENT_SERVICE_WAIT_FOREVER);
        if (status != SDL_PASS)
        {
             UART_printf("  Sciclient_pmSetModuleRst 0x%x ...FAILED \n",
                         LBIST_TestHandleArray[coreIndex].tisciDeviceId);
             testResult = -1;
        }
    }

    /**--- Step 2e: Put Secondary core in module reset and local reset ---*/
    if ((testResult == 0) && (LBIST_TestHandleArray[coreIndex].tisciSecDeviceId != 0U))
    {
#ifdef DEBUG
        UART_printf("  Secondary core: Putting in module and local reset the core %s \n",
                    LBIST_TestHandleArray[coreIndex].secCoreName);
#endif
        status = Sciclient_pmSetModuleRst(LBIST_TestHandleArray[coreIndex].tisciSecDeviceId,
                                          0x3, /* Module Reset and Local Reset asserted */
                                          SCICLIENT_SERVICE_WAIT_FOREVER);
        if (status != SDL_PASS)
        {
             UART_printf("  Sciclient_pmSetModuleRst 0x%x ...FAILED \n",
                         LBIST_TestHandleArray[coreIndex].tisciSecDeviceId);
             testResult = -1;
        }
    }

    /**--- Step 2f: Place all Auxilliary modules needed to run test into retention ---*/
    if (testResult == 0)
    {
        int i;

        /* Place all Auxilliary modules required for test into retention */
        for ( i = 0; i < LBIST_TestHandleArray[coreIndex].numAuxDevices; i++)
        {
#ifdef DEBUG
            UART_printf("  Putting into Retention Device number %d Device Id %x\n",
                        i,
                        LBIST_TestHandleArray[coreIndex].auxDeviceIdsP[i]);
#endif

            status = Sciclient_pmSetModuleState(LBIST_TestHandleArray[coreIndex].auxDeviceIdsP[i],
                                                TISCI_MSG_VALUE_DEVICE_SW_STATE_RETENTION,
                                                TISCI_MSG_FLAG_AOP,
                                                SCICLIENT_SERVICE_WAIT_FOREVER);
            if (status != SDL_PASS)
            {
                UART_printf("  Sciclient_pmSetModuleState 0x%x ...FAILED \n",
                            LBIST_TestHandleArray[coreIndex].auxDeviceIdsP[i]);
                testResult = -1;
                break;
            }
        }
    }

    /**--- Step 2g: Place Primary core into retention ---*/
    if (testResult == 0)
    {
        if (LBIST_TestHandleArray[coreIndex].tisciDeviceId != 0u)
        {
            /* Placing Primary core into Retention */
#ifdef DEBUG
            UART_printf("  Primary core: Putting into Retention %s \n",
                        LBIST_TestHandleArray[coreIndex].coreName);
#endif
            status =  Sciclient_pmSetModuleState(LBIST_TestHandleArray[coreIndex].tisciDeviceId,
                                                 TISCI_MSG_VALUE_DEVICE_SW_STATE_RETENTION,
                                                 TISCI_MSG_FLAG_AOP,
                                                 SCICLIENT_SERVICE_WAIT_FOREVER);
            if (status != SDL_PASS)
            {
                UART_printf("   Primary core: Sciclient_pmSetModuleState...FAILED \n");
                testResult = -1;
            }
        }
    }

    /**--- Step 2h: Place Secondary core into retention ---*/
    if (testResult == 0)
    {
        if (LBIST_TestHandleArray[coreIndex].tisciSecDeviceId != 0u)
        {
            /* Placing Secondary core into Retention */
#ifdef DEBUG
            UART_printf("  Secondary core: Putting into Retention %s \n",
                        LBIST_TestHandleArray[coreIndex].secCoreName);
#endif
            status =  Sciclient_pmSetModuleState(LBIST_TestHandleArray[coreIndex].tisciSecDeviceId,
                                                 TISCI_MSG_VALUE_DEVICE_SW_STATE_RETENTION,
                                                 TISCI_MSG_FLAG_AOP,
                                                 SCICLIENT_SERVICE_WAIT_FOREVER);
            if (status != SDL_PASS)
            {
                UART_printf("   Secondary core: Sciclient_pmSetModuleState...FAILED \n");
                testResult = -1;
            }
        }
    }

    /* Get start time for LBIST test */
    testStartTime = TimerP_getTimeInUsecs();

    /**-- Step 3: Run LBIST test --*/
    if (testResult == 0)
    {
        status = SDL_LBIST_selfTest(LBIST_TestHandleArray[coreIndex].instance, SDL_LBIST_TEST, &result);
        if ((status != SDL_PASS) || (result == false))
        {
            UART_printf("    LBIST selfTest failed \n");
            testResult = -1;
        }
    }

    /* Here LBIST test is complete , get end time of test */
    testEndTime = TimerP_getTimeInUsecs();

    /**-- Step 5: Restore cores --*/
    /* The following sequence is needed to restore core to normal operation */

    /**--- Step 5a: Switch off Secondary core ---*/
    if (testResult == 0)
    {
        if (LBIST_TestHandleArray[coreIndex].secondaryCoreNeeded)
        {
            /* Power off Secondary core */
#ifdef DEBUG
            UART_printf("  Secondary core: Powering off %s \n",
                        LBIST_TestHandleArray[coreIndex].secCoreName);
#endif
            status =  Sciclient_pmSetModuleState(LBIST_TestHandleArray[coreIndex].tisciSecDeviceId,
                                                 TISCI_MSG_VALUE_DEVICE_SW_STATE_AUTO_OFF,
                                                 TISCI_MSG_FLAG_AOP,
                                                 SCICLIENT_SERVICE_WAIT_FOREVER);
            if (status != SDL_PASS)
            {
                UART_printf("   Secondary core: Sciclient_pmSetModuleState:  Power off FAILED \n");
                testResult = -1;
            }
        }
    }

    /**--- Step 5b: Switch off Primary core ---*/
    if (testResult == 0)
    {
        /* Power off Primary core */
#ifdef DEBUG
        UART_printf("  Primary core: Powering off %s \n",
                    LBIST_TestHandleArray[coreIndex].coreName);
#endif
        status =  Sciclient_pmSetModuleState(LBIST_TestHandleArray[coreIndex].tisciDeviceId,
                                             TISCI_MSG_VALUE_DEVICE_SW_STATE_AUTO_OFF,
                                             TISCI_MSG_FLAG_AOP,
                                             SCICLIENT_SERVICE_WAIT_FOREVER);
        if (status != SDL_PASS)
        {
            UART_printf("   Primary core: Sciclient_pmSetModuleState: Power off FAILED \n");
        }
    }

    /**--- Step 5c: Switch off Auxilliary modules ---*/
    if (testResult == 0)
    {
        int i;

        /* Place all Auxilliary modules required for test into retention */
        for ( i = 0; i < LBIST_TestHandleArray[coreIndex].numAuxDevices; i++)
        {
#ifdef DEBUG
            UART_printf("  Powering off Device number %d Device Id %x\n",
                        i,
                        LBIST_TestHandleArray[coreIndex].auxDeviceIdsP[i]);
#endif

            status = Sciclient_pmSetModuleState(LBIST_TestHandleArray[coreIndex].auxDeviceIdsP[i],
                                                TISCI_MSG_VALUE_DEVICE_SW_STATE_AUTO_OFF,
                                                TISCI_MSG_FLAG_AOP,
                                                SCICLIENT_SERVICE_WAIT_FOREVER);
            if (status != SDL_PASS)
            {
                UART_printf("  Sciclient_pmSetModuleState 0x%x ...FAILED \n",
                            LBIST_TestHandleArray[coreIndex].auxDeviceIdsP[i]);
                testResult = -1;
                break;
            }
        }
    }

    /**--- Step 5d: Disable Isolation ---*/
    if (testResult == 0)
    {
#ifdef DEBUG
        UART_printf("  Disabling isolation \n");
#endif
        status = SDL_LBIST_selfTest(LBIST_TestHandleArray[coreIndex].instance, SDL_LBIST_TEST_RELEASE, &result);
        if (status != SDL_PASS)
        {
            UART_printf("   SDL_LBIST_disableIsolation ...FAILED \n");
            testResult = -1;
        }
    }

    /**--- Step 5e: Place all Auxilliary modules into retention ---*/
    if (testResult == 0)
    {
        int i;

        /* Place all Auxilliary modules required for test into retention */
        for ( i = 0; i < LBIST_TestHandleArray[coreIndex].numAuxDevices; i++)
        {
#ifdef DEBUG
            UART_printf("  Putting into Retention Device number %d Device Id %x\n",
                        i,
                        LBIST_TestHandleArray[coreIndex].auxDeviceIdsP[i]);
#endif

            status = Sciclient_pmSetModuleState(LBIST_TestHandleArray[coreIndex].auxDeviceIdsP[i],
                                                TISCI_MSG_VALUE_DEVICE_SW_STATE_RETENTION,
                                                TISCI_MSG_FLAG_AOP,
                                                SCICLIENT_SERVICE_WAIT_FOREVER);
            if (status != SDL_PASS)
            {
                UART_printf("  Sciclient_pmSetModuleState 0x%x ...FAILED \n",
                            LBIST_TestHandleArray[coreIndex].auxDeviceIdsP[i]);
                testResult = -1;
                break;
            }
        }
    }

    /**--- Step 5f: Place Primary core into retention ---*/
    if (testResult == 0)
    {
        /* Placing Primary core into Retention */
#ifdef DEBUG
        UART_printf("  Primary core: Putting into Retention %s \n",
                    LBIST_TestHandleArray[coreIndex].coreName);
#endif
        status = Sciclient_pmSetModuleState(LBIST_TestHandleArray[coreIndex].tisciDeviceId,
                                            TISCI_MSG_VALUE_DEVICE_SW_STATE_RETENTION,
                                            TISCI_MSG_FLAG_AOP,
                                            SCICLIENT_SERVICE_WAIT_FOREVER);

        if (status != SDL_PASS)
        {
            UART_printf("   Primary core: Sciclient_pmSetModuleState 0x%x ...FAILED \n",
                        LBIST_TestHandleArray[coreIndex].tisciDeviceId);
            testResult = -1;
        }
    }

    /**--- Step 5g: Place Secondary core into retention ---*/
    if (testResult == 0)
    {
        if (LBIST_TestHandleArray[coreIndex].secondaryCoreNeeded)
        {
            /* Placing Secondary core into Retention */
#ifdef DEBUG
            UART_printf("  Secondary core: Putting into Retention %s \n",
                        LBIST_TestHandleArray[coreIndex].secCoreName);
#endif
            status = Sciclient_pmSetModuleState(LBIST_TestHandleArray[coreIndex].tisciSecDeviceId,
                                                TISCI_MSG_VALUE_DEVICE_SW_STATE_RETENTION,
                                                TISCI_MSG_FLAG_AOP,
                                                SCICLIENT_SERVICE_WAIT_FOREVER);
            if (status != SDL_PASS)
            {
                UART_printf("   Secondary core: Sciclient_pmSetModuleState 0x%x ...FAILED \n",
                            LBIST_TestHandleArray[coreIndex].tisciSecDeviceId);
                testResult = -1;
                return testResult;
            }
        }
    }

    /**--- Step 5i: Power off Secondary core ---*/
    if (testResult == 0)
    {
        if (LBIST_TestHandleArray[coreIndex].secondaryCoreNeeded)
        {
            /* Power off Secondary core */
#ifdef DEBUG
            UART_printf("  Secondary core: Powering off %s \n",
                        LBIST_TestHandleArray[coreIndex].secCoreName);
#endif
            status =  Sciclient_pmSetModuleState(LBIST_TestHandleArray[coreIndex].tisciSecDeviceId,
                                                 TISCI_MSG_VALUE_DEVICE_SW_STATE_AUTO_OFF,
                                                 TISCI_MSG_FLAG_AOP,
                                                 SCICLIENT_SERVICE_WAIT_FOREVER);
            if (status != SDL_PASS)
            {
                UART_printf("   Secondary core: Sciclient_pmSetModuleState:  Power off FAILED \n");
                testResult = -1;
            }
        }
    }

    /**--- Step 5j: Power off Primary core ---*/
    if (testResult == 0)
    {
        /* Power off Primary core */
#ifdef DEBUG
        UART_printf("  Primary core: Powering off %s \n",
                    LBIST_TestHandleArray[coreIndex].coreName);
#endif
        status =  Sciclient_pmSetModuleState(LBIST_TestHandleArray[coreIndex].tisciDeviceId,
                                             TISCI_MSG_VALUE_DEVICE_SW_STATE_AUTO_OFF,
                                             TISCI_MSG_FLAG_AOP,
                                             SCICLIENT_SERVICE_WAIT_FOREVER);
        if (status != SDL_PASS)
        {
            UART_printf("   Primary core: Sciclient_pmSetModuleState: Power off FAILED \n");
        }
    }

    /**--- Step 5k: Power off Auxilliary modules ---*/
    if (testResult == 0)
    {
        int i;

        /* Place all Auxilliary modules required for test into retention */
        for ( i = 0; i < LBIST_TestHandleArray[coreIndex].numAuxDevices; i++)
        {
#ifdef DEBUG
            UART_printf("  Powering off Device number %d Device Id %x\n",
                        i,
                        LBIST_TestHandleArray[coreIndex].auxDeviceIdsP[i]);
#endif

            status = Sciclient_pmSetModuleState(LBIST_TestHandleArray[coreIndex].auxDeviceIdsP[i],
                                                TISCI_MSG_VALUE_DEVICE_SW_STATE_AUTO_OFF,
                                                TISCI_MSG_FLAG_AOP,
                                                SCICLIENT_SERVICE_WAIT_FOREVER);
            if (status != SDL_PASS)
            {
                UART_printf("  Sciclient_pmSetModuleState 0x%x ...FAILED \n",
                            LBIST_TestHandleArray[coreIndex].auxDeviceIdsP[i]);
                testResult = -1;
                break;
            }
        }
    }

    /**--- Step 5l: Take Primary core out of local reset ---*/
    if ((testResult == 0) && (LBIST_TestHandleArray[coreIndex].tisciDeviceId != 0U))
    {
#ifdef DEBUG
        UART_printf("  Primary core: Taking out of local reset the core %s \n",
                    LBIST_TestHandleArray[coreIndex].coreName);
#endif
        status = Sciclient_pmSetModuleRst(LBIST_TestHandleArray[coreIndex].tisciDeviceId,
                                          0x0,
                                          SCICLIENT_SERVICE_WAIT_FOREVER);
        if (status != SDL_PASS)
        {
            UART_printf("  Sciclient_pmSetModuleRst 0x%x ...FAILED \n",
                        LBIST_TestHandleArray[coreIndex].tisciDeviceId);
            testResult = -1;
        }
    }

    /**--- Step 5m: Take Secondary core out of local reset ---*/
    if ((testResult == 0) && (LBIST_TestHandleArray[coreIndex].tisciSecDeviceId != 0U))
    {
#ifdef DEBUG
        UART_printf("  Secondary core: Taking out of local reset the core %s \n",
                    LBIST_TestHandleArray[coreIndex].secCoreName);
#endif
        status = Sciclient_pmSetModuleRst(LBIST_TestHandleArray[coreIndex].tisciSecDeviceId,
                                          0x0,
                                          SCICLIENT_SERVICE_WAIT_FOREVER);
        if (status != SDL_PASS)
        {
            UART_printf("  Sciclient_pmSetModuleRst 0x%x ...FAILED \n",
                        LBIST_TestHandleArray[coreIndex].tisciSecDeviceId);
            testResult = -1;
        }
    }

    /**--- Step 5n: Take Auxilliary modules out of module reset ---*/
    if (testResult == 0)
    {
        int i;

        /* Place all Auxilliary modules required for test into module reset */
        for ( i = 0; i < LBIST_TestHandleArray[coreIndex].numAuxDevices; i++)
        {
#ifdef DEBUG
            UART_printf("  Putting into module reset Device number %d Device Id %x\n",
                        i,
                        LBIST_TestHandleArray[coreIndex].auxDeviceIdsP[i]);
#endif

            status = Sciclient_pmSetModuleRst(LBIST_TestHandleArray[coreIndex].auxDeviceIdsP[i],
                                              0x0, // Need to keep Local Reset too??
                                              SCICLIENT_SERVICE_WAIT_FOREVER);
            if (status != SDL_PASS)
            {
                UART_printf("  Sciclient_pmSetModuleState 0x%x ...FAILED \n",
                            LBIST_TestHandleArray[coreIndex].auxDeviceIdsP[i]);
                testResult = -1;
                break;
            }
        }
    }

    /**--- Step 5o: Release Primary core ---*/
    if ((testResult == 0) && (LBIST_TestHandleArray[coreIndex].tisciProcId !=0))
    {
        /* release processor Primary core */
#ifdef DEBUG
        UART_printf("  Primary core: Releasing %s \n",
                    LBIST_TestHandleArray[coreIndex].coreName);
#endif

        status = Sciclient_procBootReleaseProcessor(LBIST_TestHandleArray[coreIndex].tisciProcId,
                                                    TISCI_MSG_FLAG_AOP,
                                                    SCICLIENT_SERVICE_WAIT_FOREVER);
        if (status != SDL_PASS)
        {
            UART_printf("   Primary core: Sciclient_procBootReleaseProcessor, ProcId 0x%x...FAILED \n",
                        LBIST_TestHandleArray[coreIndex].tisciProcId);
            testResult = -1;
        }
    }

    /**--- Step 5p: Release Secondary core ---*/
    if (testResult == 0)
    {
        if ((LBIST_TestHandleArray[coreIndex].secondaryCoreNeeded)
            && (LBIST_TestHandleArray[coreIndex].tisciSecDeviceId != 0u))
        {
            /* release processor Secondary core */
#ifdef DEBUG
            UART_printf("  Secondary core: Releasing %s \n",
                        LBIST_TestHandleArray[coreIndex].secCoreName);
#endif
            status = Sciclient_procBootReleaseProcessor(LBIST_TestHandleArray[coreIndex].tisciSecProcId,
                                                        TISCI_MSG_FLAG_AOP,
                                                        SCICLIENT_SERVICE_WAIT_FOREVER);
            if (status != SDL_PASS)
            {
                UART_printf("   Secondary core: Sciclient_procBootReleaseProcessor, ProcId 0x%x...FAILED \n",
                            LBIST_TestHandleArray[coreIndex].tisciSecProcId);
                testResult = -1;
            }
        }
    }

    /* Here LBIST test is complete , get end time of test */
    endTime = TimerP_getTimeInUsecs();

    prepTime = testStartTime - startTime;
    diffTime = testEndTime - testStartTime;
    restoreTime = endTime - testEndTime;
    UART_printf("  Delta Cores prep time in micro secs %d \n", (uint32_t)prepTime );
    UART_printf("  Delta LBIST execution time in micro secs %d \n", (uint32_t)diffTime );
    UART_printf("  Delta Cores restore time in micro secs %d \n", (uint32_t)restoreTime );

    UART_printf("  LBIST complete for %s \n",
                LBIST_TestHandleArray[coreIndex].coreName);

    return (testResult);
}

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

static int32_t LBIST_errNegativeTest(void)
{
    int32_t testResult = 0;
    bool result = false;
    int32_t sdlRet;
    SDL_lbistInstInfo *pInfo;
    uint32_t expectedMISR;

    sdlRet = SDL_LBIST_selfTest(SDL_LBIST_INST_MAINR5F0, SDL_LBIST_TEST, NULL);
    if (sdlRet == SDL_PASS)
    {
        UART_printf("\n  SDL_LBIST_selfTest negative test with NULL pointer failed \n");
        testResult = -1;
    }
#if defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J721S2)
    if (testResult == 0)
    {
        sdlRet = SDL_LBIST_selfTest((SDL_LBIST_inst)(SDL_LBIST_INST_A72+1), SDL_LBIST_TEST, &result);
        if (sdlRet == SDL_PASS)
        {
            UART_printf("\n  SDL_LBIST_selfTest negative test with invalid instance failed \n");
            testResult = -1;
        }
    }
#endif
#if defined (SOC_J784S4) 
    if (testResult == 0)
    {
        sdlRet = SDL_LBIST_selfTest((SDL_LBIST_inst)(LBIST_INST_MAINR5F2_INDEX+1), SDL_LBIST_TEST, &result);
        if (sdlRet == SDL_PASS)
        {
            UART_printf("\n  SDL_LBIST_selfTest negative test with invalid instance failed \n");
            testResult = -1;
        }
    }
#endif
    if (testResult == 0)
    {
        sdlRet = SDL_LBIST_selfTest(SDL_LBIST_INST_MAINR5F0, (SDL_LBIST_testType)(SDL_LBIST_TEST_RELEASE+1), &result);
        if (sdlRet == SDL_PASS)
        {
            UART_printf("\n  SDL_LBIST_reset negative test with invalid test type failed \n");
            testResult = -1;
        }
    }

    if (testResult == 0)
    {
        sdlRet = SDL_LBIST_getPOSTStatus(NULL);
        if (sdlRet == SDL_PASS)
        {
            UART_printf("\n  SDL_LBIST_getPOSTStatus negative test with invalid status failed \n");
            testResult = -1;
        }
    }

    if (testResult == 0)
    {
        sdlRet = SDL_LBIST_getDevicePostStatus(NULL);
        if (sdlRet == SDL_PASS)
        {
            UART_printf("\n  SDL_LBIST_getDevicePostStatus negative test with invalid status failed \n");
            testResult = -1;
        }
    }

    if (testResult == 0)
    {
        UART_printf("\n SDL_LBIST_eventHandler with invalid instance\n");
        SDL_LBIST_eventHandler(0xFF);
        UART_printf("\n SDL_LBIST_eventHandler with valid instance but no test started\n");
        SDL_LBIST_eventHandler(SDL_LBIST_INST_MAINR5F0);
    }

        /* Tests with invalid OSALs */
    if (testResult == 0)
    {
        LBIST_TEST1_osalInit();
        sdlRet = SDL_LBIST_selfTest(SDL_LBIST_INST_MAINR5F0, SDL_LBIST_TEST, &result);
        if (sdlRet == SDL_PASS)
        {
            UART_printf("\n  SDL_LBIST_selfTest negative test failed \n");
            testResult = -1;
        }
    }

    // override the interrupt handler to force "timeout"
    LBIST_TEST2_osalInit();
    sdlRet = SDL_LBIST_selfTest(SDL_LBIST_INST_MAINR5F0, SDL_LBIST_TEST, &result);

    SDL_TEST_osalInit();
    pInfo = SDL_LBIST_getInstInfo((uint32_t)SDL_LBIST_INST_MAINR5F0);
    // override the expected MISR
    expectedMISR = pInfo->expectedMISR;
    pInfo->expectedMISR = 0x0;
    // run the test
    LBIST_ERR_runTest(0);// core id 0 is Main R5F0
    // restore expectedMISR
    pInfo->expectedMISR = expectedMISR;
    return (testResult);
}


/* LBIST Error module test */
int32_t LBIST_errTest(void)
{
    int32_t testResult;

    testResult = LBIST_errNegativeTest();

    return (testResult);
}

/* Nothing past this point */
