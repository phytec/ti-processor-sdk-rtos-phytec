/* Copyright (C) 2021 Texas Instruments Incorporated.
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
 *   @file  sdl_pbist_priv.h
 *
 *   @path  $(SDLPATH)/src/sdl/pbist
 *
 *   @desc  This file contains the SDL PBIST internal definitions.
 *
 *   @defgroup SDL_PBIST_API Built-In Test (PBIST)
 *
 *   Provides the internal definitions for PBIST.
 */

#ifndef SDL_PBIST_PRIV_H_
#define SDL_PBIST_PRIV_H_

#ifdef __cplusplus
extern "C" {
#endif

#define PBIST_MAX_NUM_RUNS    2
#define NUM_MAX_PBIST_TEST_ROM_RUNS 13

#define PBIST_NOT_DONE        (0U)
#define PBIST_DONE            (1U)

typedef struct
{
    uint64_t PBISTRegsHiAddress;
    SDL_pbistRegs *pPBISTRegs;
    uint32_t numPBISTRuns;
    SDL_PBIST_config PBISTConfigRun[PBIST_MAX_NUM_RUNS];
    uint32_t clecInputEvt;
    uint32_t interruptNumber;
    volatile uint32_t doneFlag;
    SDL_PBIST_configNeg PBISTNegConfigRun;
    uint32_t numPBISTROMTestRuns;
    SDL_PBIST_romTest   PBISTROMTestConfigRun[NUM_MAX_PBIST_TEST_ROM_RUNS];
    
} SDL_pbistInstInfo;

typedef struct
{
    /** Pointer to PBIST register map */
    SDL_pbistRegs *pPBISTRegs;
} SDL_pbistHWPOSTInstInfo;

SDL_pbistInstInfo * SDL_PBIST_getInstInfo(SDL_PBIST_inst instance);

uint32_t SDL_PBIST_translateClecToIntr(uint32_t outputEvt);

void SDL_PBIST_eventHandler(uint32_t coreIndex);

int32_t SDL_PBIST_getDevicePostStatus(SDL_PBIST_postResult *pResult);

#ifdef __cplusplus
}
#endif

#endif /* SDL_PBIST_PRIV_H_ */
