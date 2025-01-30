/**
 * @file  sdl_osal.c
 *
 * @brief
 *  SDL implementation file for the osal module.
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

#include <stdint.h>
#include "sdl_osal.h"
#include "sdl_types.h"

/**
 * Design: PROC_SDL-1149
 */
SDL_OSAL_Interface *gOSAL_Interface = NULL_PTR;

/**
 * Design: PROC_SDL-1148
 */
int32_t SDL_OSAL_init(SDL_OSAL_Interface *osalInterface)
{
    SDL_ErrType_t ret = SDL_PASS;

    if (osalInterface == NULL_PTR)
    {
        ret = SDL_EINVALID_PARAMS;
    }
    else
    {
        gOSAL_Interface = osalInterface;
    }

    return ret;
}

/**
 * Design: PROC_SDL-1135,PROC_SDL-1136,PROC_SDL-1137
 */
int32_t SDL_OSAL_enableInterrupt(int32_t intNum)
{
    SDL_ErrType_t ret = SDL_PASS;

    // Check HwiP function is valid
    if ((gOSAL_Interface == NULL_PTR) || (gOSAL_Interface->enableInterrupt == NULL_PTR))
    {
        ret = SDL_EINVALID_PARAMS;
    }
    else
    {
        ret = gOSAL_Interface->enableInterrupt(intNum);
    }

    return ret;
}

/**
 * Design: PROC_SDL-1138,PROC_SDL-1139,PROC_SDL-1140
 */
int32_t SDL_OSAL_disableInterrupt(int32_t intNum)
{
    SDL_ErrType_t ret = SDL_PASS;

    // Check HwiP function is valid
    if ((gOSAL_Interface == NULL_PTR) || (gOSAL_Interface->disableInterrupt == NULL_PTR))
    {
        ret = SDL_EINVALID_PARAMS;
    }
    else
    {
        ret = gOSAL_Interface->disableInterrupt(intNum);
    }

    return ret;
}

/**
 * Design: PROC_SDL-1145,PROC_SDL-1146
 */
int32_t SDL_OSAL_registerInterrupt(SDL_OSAL_hwipParams *pParams, pSDL_OSAL_hwipHandle *handle)
{
    SDL_ErrType_t ret = SDL_PASS;

    // Check HwiP function is valid
    if ((handle == NULL_PTR) || (pParams == NULL_PTR))
    {
        ret = SDL_EINVALID_PARAMS;
    }
    else if ((gOSAL_Interface == NULL_PTR) || (gOSAL_Interface->registerInterrupt == NULL_PTR))
    {
        ret = SDL_EINVALID_PARAMS;
    }
    else
    {
        *handle = gOSAL_Interface->registerInterrupt(pParams);
    }

    return ret;
}

/**
 * Design: PROC_SDL-1147
 */
int32_t SDL_OSAL_deregisterInterrupt(pSDL_OSAL_hwipHandle handle)
{
    SDL_ErrType_t ret = SDL_PASS;

    // Check HwiP function is valid
    if (handle == NULL_PTR)
    {
        ret = SDL_EINVALID_PARAMS;
    }
    else if ((gOSAL_Interface == NULL_PTR) || (gOSAL_Interface->deregisterInterrupt == NULL_PTR))
    {
        ret = SDL_EINVALID_PARAMS;
    }
    else
    {
        ret = gOSAL_Interface->deregisterInterrupt(handle);
    }

    return ret;
}

/**
 * Design: PROC_SDL-1143,PROC_SDL-1144
 */
int32_t SDL_OSAL_globalDisableInterrupts(uintptr_t *key)
{
    SDL_ErrType_t ret = SDL_PASS;

    if (key == NULL_PTR)
    {
        ret = SDL_EINVALID_PARAMS;
    }
    // Check HwiP function is valid
    else if ((gOSAL_Interface == NULL_PTR) || (gOSAL_Interface->globalDisableInterrupts == NULL_PTR))
    {
        ret = SDL_EINVALID_PARAMS;
    }
    else
    {
        ret = gOSAL_Interface->globalDisableInterrupts(key);
    }

    return ret;
}

/**
 * Design: PROC_SDL-1141,PROC_SDL-1142
 */
int32_t SDL_OSAL_globalRestoreInterrupts(uintptr_t key)
{
    SDL_ErrType_t ret = SDL_PASS;

    // Check HwiP function is valid
    if ((gOSAL_Interface == NULL_PTR) || (gOSAL_Interface->globalRestoreInterrupts == NULL_PTR))
    {
        ret = SDL_EINVALID_PARAMS;
    }
    else
    {
        ret = gOSAL_Interface->globalRestoreInterrupts(key);
    }

    return ret;
}

/**
 * Design: PROC_SDL-1516
 */
void SDL_OSAL_printf(const char* traceBuf)
{
    // Check that function is valid
    if ((gOSAL_Interface != NULL_PTR) && (gOSAL_Interface->printFxn != NULL_PTR))
    {
        gOSAL_Interface->printFxn(traceBuf);
    }

    return;
}

int32_t SDL_OSAL_delay(int32_t ndelay)
{
    SDL_ErrType_t ret = SDL_PASS;

    // Check if delay function is valid
    if ((gOSAL_Interface == NULL_PTR) || (gOSAL_Interface->delay == NULL_PTR))
    {
        ret = SDL_EINVALID_PARAMS;
    }
    else
    {
        ret = gOSAL_Interface->delay(ndelay);
    }

    return ret;
}

void* SDL_OSAL_addrTranslate(uint64_t addr, uint32_t size)
{
    void *ret = (void *)(-1);

    if ((gOSAL_Interface != NULL_PTR) && (gOSAL_Interface->addrTranslate != NULL_PTR))
    {
        ret = gOSAL_Interface->addrTranslate(addr, size);
    }

    return ret;
}
