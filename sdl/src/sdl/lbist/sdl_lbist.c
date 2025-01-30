/**
 * @file  sdl_lbist.c
 *
 * @brief
 *  SDL implementation file for the lbist module.
 *
 *  \par
 *  ============================================================================
 *  @n   (C) Copyright 2021, Texas Instruments, Inc.
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

#include <sdl_lbist.h>
#include <sdl_types.h>
#include <src/sdl/lbist/sdl_lbist_priv.h>

#include <osal/sdl_osal.h>

#define LBIST_MAX_TIMEOUT_VALUE           (100000000u)

static int32_t SDL_LBIST_runTest(SDL_lbistRegs *pRegs, SDL_lbistInstInfo *pInfo)
{
    SDL_ErrType_t status = SDL_PASS;
    bool isLBISTRunning = FALSE;
    uint32_t timeoutCount = 0;

    /* Initialize done flag */
    pInfo->doneFlag = LBIST_NOT_DONE;

    /**-- Step 1: Configure LBIST --*/

    status = SDL_LBIST_programConfig(pRegs, &pInfo->LBISTConfig);

    /**-- Step 2: Run LBIST test --*/

    /**--- Step 2a: Enable Isolation ---*/
    if (status == SDL_PASS)
    {
        /* Call SDL API */
        status = SDL_LBIST_enableIsolation(pRegs);
        /**--- Step 2b: reset LBIST ---*/
        if (status == SDL_PASS)
        {
            status = SDL_LBIST_reset(pRegs);
            /**--- Step 2c: Enable Run BIST Mode ---*/
            if (status == SDL_PASS)
            {
                status = SDL_LBIST_enableRunBISTMode(pRegs);
            }
        }
    }

    /**--- Step 2d: Start LBIST ---*/
    if (status == SDL_PASS)
    {
        status = SDL_LBIST_start(pRegs);
        /**--- Step 2e: Check LBIST Running status ---*/
        if (status == SDL_PASS)
        {
            status = SDL_LBIST_isRunning(pRegs, &isLBISTRunning);
        }
    }

    /**--- Step 2f: Wait for LBIST Interrupt ---*/
    if (status == SDL_PASS)
    {
        /* Timeout if exceeds time */
        while ((pInfo->doneFlag == LBIST_NOT_DONE)
               && (timeoutCount < LBIST_MAX_TIMEOUT_VALUE))
        {
            timeoutCount++;
        }

        if (pInfo->doneFlag == LBIST_NOT_DONE)
        {
            status = SDL_ETIMEOUT;
        }
        /* reset Done flag so we can run again */
        pInfo->doneFlag = LBIST_NOT_DONE;
    }

    return status;
}

static int32_t SDL_LBIST_checkResult(SDL_lbistRegs *pRegs, const SDL_lbistInstInfo *pInfo, const uint32_t *pLBISTSig, bool *pResult)
{
    int32_t status = SDL_PASS;
    uint32_t calculatedMISR;
    uint32_t expectedMISR;

    /**--- Step 2g: Get Signature of test ---*/
    status = SDL_LBIST_getMISR(pRegs, &calculatedMISR);

    /**--- Step 2h: Get Expected Signature ---*/
    if (status == SDL_PASS)
    {
        status = SDL_LBIST_getExpectedMISR(pLBISTSig, &expectedMISR);
    }

    /**--- Step 2i: Clear Run BIST Mode ---*/
    if (status == SDL_PASS)
    {
        status = SDL_LBIST_clearRunBISTMode(pRegs);
    }

    /**--- Step 2j: Stop LBIST ---*/
    if (status == SDL_PASS)
    {
        status = SDL_LBIST_stop(pRegs);
    }

    /**--- Step 2k: Reset LBIST ---*/
    if (status == SDL_PASS)
    {
        status = SDL_LBIST_reset(pRegs);
    }

    /**--- Step 3: Check result of LBIST  ---*/
    if (status == SDL_PASS)
    {
        /* TODO: Temporarily hard coding to expected MISR */
        expectedMISR = pInfo->expectedMISR;
        if (calculatedMISR != expectedMISR)
        {
            *pResult = FALSE;
        }
        else
        {
            *pResult = TRUE;
        }
    }

    return status;
}

/**
 * Design: PROC_SDL-1009,PROC_SDL-1010,PROC_SDL-1011,PROC_SDL-1012,PROC_SDL-1015,PROC_SDL-1020
 */
int32_t SDL_LBIST_selfTest(SDL_LBIST_inst instance, SDL_LBIST_testType testType,
                           bool *pResult)
{
#if defined(SOC_J784S4)
    if(testType == SDL_LBIST_TEST)
    {
	    SDL_LBIST_socConfig(instance,TRUE);
    }
#endif	
    SDL_lbistRegs *pRegs;
    SDL_lbistInstInfo *pInfo;
    SDL_ErrType_t status = SDL_PASS;
    uint32_t *pLBISTSig;

    pSDL_OSAL_hwipHandle LBIST_intrHandle = NULL;
    SDL_OSAL_hwipParams intrParams;

    /* Get the LBIST Instance Info */
    pInfo = SDL_LBIST_getInstInfo((uint32_t)instance);

    if ((pResult == NULL) || (pInfo == NULL) ||
        ((testType != SDL_LBIST_TEST) && (testType != SDL_LBIST_TEST_RELEASE)))
    {
        status = SDL_EBADARGS;
    }

    if (status == SDL_PASS)
    {
        /* Disable interrupt */
        (void)SDL_OSAL_disableInterrupt((int32_t)(pInfo->interruptNumber));

        /* Pass core Index as argument to handler*/
        intrParams.intNum = (int32_t)pInfo->interruptNumber;
        intrParams.callback = SDL_LBIST_eventHandler;
        intrParams.callbackArg = (uintptr_t)instance;

        /* Register call back function for PBIST Interrupt */
        status = SDL_OSAL_registerInterrupt(&intrParams, &LBIST_intrHandle);

        if (LBIST_intrHandle == NULL)
        {
            status = SDL_EFAIL;
        }

        /* Get LBIST register space Pointer */
        pRegs = pInfo->pLBISTRegs;
	pLBISTSig = pInfo->pLBISTSig;
    }

    if (status == SDL_PASS)
    {
        if (testType == SDL_LBIST_TEST)
        {
            status = SDL_LBIST_runTest(pRegs, pInfo);
            if (status == SDL_PASS)
            {
                status = SDL_LBIST_checkResult(pRegs, pInfo, pLBISTSig, pResult);
            }
        }
        else
        {
            status = SDL_LBIST_disableIsolation(pRegs);
            #if defined(SOC_J784S4)	
                /*Reset value of MPU_LBIST_SPARE register to take the core out of test mode*/
	            SDL_LBIST_socConfig(instance,FALSE);
            #endif
        }
    }

    /* Destroy the interrupt handler */
    if (LBIST_intrHandle != NULL)
    {
        status = SDL_OSAL_deregisterInterrupt(LBIST_intrHandle);
    }

    return status;
}

/**
 * Design: PROC_SDL-1013,PROC_SDL-1014
 */
int32_t SDL_LBIST_getPOSTStatus(SDL_LBIST_postResult *pResult)
{
    int32_t ret = SDL_PASS;

    if (pResult == NULL)
    {
        ret = SDL_EBADARGS;
    }

    if (ret == SDL_PASS)
    {
        /* HW POST test flow - test already run, checking the result */
        ret = SDL_LBIST_getDevicePostStatus(pResult);
    }

    return ret;
}

/* Nothing past this point */
