/*
 *   Copyright (c) Texas Instruments Incorporated 2020
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
 *  \file     pbist_test_func.h
 *
 *  \brief    This file contains PBIST test function structures
 *
 *  \details  PBIST Test function structures
 **/
#ifndef PBIST_TEST_FUNC_H
#define PBIST_TEST_FUNC_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <sdl_pbist.h>

/* ========================================================================== */
/*                                Macros                                      */
/* ========================================================================== */

#define PBIST_INSTANCE_NAME_MAX_LENGTH    20
#define NUM_MAX_PBIST_TEST_ROM_RUNS 13

#if defined (SOC_J721E)
#define PBIST_MAX_NUM_RUNS                3
#endif

#if defined (SOC_J7200)
#define PBIST_MAX_NUM_RUNS                2
#endif

#if defined (SOC_J721S2)
#define PBIST_MAX_NUM_RUNS                3
#endif

#if defined (SOC_J784S4)
#define PBIST_MAX_NUM_RUNS                3
#endif
/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <stdint.h>
#include <string.h>
#include <src/ip/sdl_ip_pbist.h>

/* ========================================================================== */
/*                                Data Structures                             */
/* ========================================================================== */

typedef void (*PBIST_handlerPtr)(uint32_t instanceId);

/*
    InitRestore function : Initialize or Restore based on init flag
    init : TRUE  --> Initialize
    init : FALSE --> Restore
*/
typedef int32_t (*PBIST_auxInitRestoreFunctionPtr)(bool init);

typedef struct PBIST_TestHandle_s
{
    char     testName[PBIST_INSTANCE_NAME_MAX_LENGTH];
    SDL_PBIST_inst pbistInst;
    uint64_t PBISTRegsHiAddress;
    SDL_pbistRegs *pPBISTRegs;
    SDL_PBIST_config PBISTConfigRun[PBIST_MAX_NUM_RUNS];
    uint32_t numPBISTRuns;
    SDL_PBIST_configNeg PBISTNegConfigRun;
     uint32_t numPBISTROMTestRuns;
    SDL_PBIST_romTest   PBISTROMTestConfigRun[NUM_MAX_PBIST_TEST_ROM_RUNS];
    uint32_t tisciPBISTDeviceId;
    bool     pollMode;
    uint32_t interruptNumber;
    bool procRstNeeded;
    bool secondaryCoreNeeded;
    bool thirdCoreNeeded;
    bool fourthCoreNeeded;
    char coreName[16];
    char secCoreName[16];
    char thCoreName[16];
    char foCoreName[16];
    uint32_t tisciProcId;
    uint32_t tisciSecProcId;
    uint32_t tisciThProcId;
    uint32_t tisciFoProcId;
    uint32_t tisciDeviceId;
    uint32_t tisciSecDeviceId;
    uint32_t tisciThDeviceId;
    uint32_t tisciFoDeviceId;
    volatile bool doneFlag;
    bool     coreCustPwrSeqNeeded;
    uint8_t  numPostPbistToCheck;
    uint32_t numAuxDevices;
    uint32_t *auxDeviceIdsP;
    PBIST_auxInitRestoreFunctionPtr auxInitRestoreFunction;
} PBIST_TestHandle_t;


void PBIST_eventHandler(uint32_t coreIndex);

int32_t PBIST_commonInit(void);

#ifdef __cplusplus
}
#endif

#endif /* PBIST_TEST_FUNC_H */

/* Nothing past this point */
