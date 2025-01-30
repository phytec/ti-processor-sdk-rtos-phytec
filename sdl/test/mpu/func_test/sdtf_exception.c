/*
 * SDL TEST
 *
 * Software Diagnostics Library
 *
 *  Copyright (c) Texas Instruments Incorporated 2018-2020
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

#include "main.h"
#include <test/r5f_startup/sdl_cache.h>
#include <sdl_exception.h>
#include <test/r5f_startup/interrupt.h>

extern void *_resetvectors;
#define VECTOR_SIZE 0x100


/* This is the list of exception handle and the parameters */
const SDL_R5ExptnHandlers SDTF_R5ExptnHandlers=
{
    .udefExptnHandler = &SDL_EXCEPTION_undefInstructionExptnHandler,
    .swiExptnHandler = &SDL_EXCEPTION_swIntrExptnHandler,
    .pabtExptnHandler = &SDL_EXCEPTION_prefetchAbortExptnHandler,
    .dabtExptnHandler = &SDL_EXCEPTION_dataAbortExptnHandler,
    .udefExptnHandlerArgs = ((void *)0u),
    .swiExptnHandlerArgs = ((void *)0u),
    .pabtExptnHandlerArgs = ((void *)0u),
    .dabtExptnHandlerArgs = ((void *)0u),
};

void SDL_undefInstructionExptnCallback(void)
{
   sdlApp_print("\n Undefined Instruction exception");
}

void SDL_swIntrExptnCallback(void)
{
   sdlApp_print("\n Software interrupt exception");
}

void SDL_prefetchAbortExptnCallback(void)
{
   sdlApp_print("\n Prefetch Abort exception");
}
void SDL_dataAbortExptnCallback(void)
{
   sdlApp_print("\n Data Abort exception");
}

void SDL_exceptionInit(void)
{

    SDL_EXCEPTION_CallbackFunctions_t exceptionCallbackFunctions =
            {
             .udefExptnCallback = SDL_undefInstructionExptnCallback,
             .swiExptnCallback = SDL_swIntrExptnCallback,
             .pabtExptnCallback = SDL_prefetchAbortExptnCallback,
             .dabtExptnCallback = SDL_dataAbortExptnCallback,
     
            };

    /* Initialize SDL exception handler */
    SDL_EXCEPTION_init(&exceptionCallbackFunctions);
    /* Register SDL exception handler */
    Intc_RegisterExptnHandlers(&SDTF_R5ExptnHandlers);

    return;
}

void SDL_copyResetVector(void)
{
    /* Invalidate cache */
   SDL_armR5CacheInv(0, VECTOR_SIZE);

    /* copy teh vectors into 0 location */
    /* Vectors are always assumed to be at 0 for R5 */
    memcpy(0, &_resetvectors, VECTOR_SIZE);

    /* Write back cache */
    SDL_armR5CacheWbInv(0, VECTOR_SIZE);
}
