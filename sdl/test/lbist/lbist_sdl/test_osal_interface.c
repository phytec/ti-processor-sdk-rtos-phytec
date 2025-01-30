/**
 * @file  test_osal_interface.c
 *
 * @brief
 *  Example implementation of SDL OSAL API.
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

#include <stdio.h>
#include <soc.h>
#include <ti/osal/osal.h>
#include <osal/sdl_osal.h>
#include <ti/csl/csl_rat.h>
#include <ti/drv/uart/UART.h>
#include <ti/drv/uart/UART_stdio.h>

pSDL_OSAL_hwipHandle LBIST_TEST_registerInterrupt_error(SDL_OSAL_hwipParams *pParams)
{
    return NULL;
}

void LBIST_TEST_eventHandler( uint32_t instanceId)
{
    return;
}

pSDL_OSAL_hwipHandle LBIST_TEST_registerInterrupt_timeout(SDL_OSAL_hwipParams *pParams)
{
    HwiP_Params hwipParams;
    HwiP_Params_init(&hwipParams);

    hwipParams.arg = pParams->callbackArg;

    return HwiP_create(pParams->intNum, LBIST_TEST_eventHandler, &hwipParams);
}

int32_t LBIST_TEST_enableInterrupt(uint32_t intNum)
{
    HwiP_enableInterrupt(intNum);
    return SDL_PASS;
}

int32_t LBIST_TEST_disableInterrupt(uint32_t intNum)
{
    HwiP_disableInterrupt(intNum);
    return SDL_PASS;
}

int32_t LBIST_TEST_globalDisableInterrupts(uintptr_t *key)
{
    *key = HwiP_disable();
    return SDL_PASS;
}

int32_t LBIST_TEST_globalRestoreInterrupts(uintptr_t key)
{
    HwiP_restore(key);
    return SDL_PASS;
}

void* LBIST_TEST_addrTranslate(uint64_t addr, uint32_t size)
{
    return (void *)0xa0000000;
}

SDL_OSAL_Interface test_osal_interface1 =
{
    .enableInterrupt = (pSDL_OSAL_interruptFunction) LBIST_TEST_enableInterrupt,
    .disableInterrupt = (pSDL_OSAL_interruptFunction) LBIST_TEST_disableInterrupt,
    .registerInterrupt = (pSDL_OSAL_registerFunction) LBIST_TEST_registerInterrupt_error,
    .deregisterInterrupt = (pSDL_OSAL_deregisterFunction) HwiP_delete,
    .globalDisableInterrupts = (pSDL_OSAL_globalDisableInterruptsFunction) LBIST_TEST_globalDisableInterrupts,
    .globalRestoreInterrupts = (pSDL_OSAL_globalRestoreInterruptsFunction) LBIST_TEST_globalRestoreInterrupts,
    .printFxn = (pSDL_OSAL_printFunction) UART_printf,
    .delay = (pSDL_OSAL_delayFunction) Osal_delay,
    .addrTranslate = (pSDL_OSAL_addrTranslateFunction) LBIST_TEST_addrTranslate
};

int32_t LBIST_TEST1_osalInit(void)
{
    SDL_ErrType_t ret = SDL_PASS;

    ret = SDL_OSAL_init(&test_osal_interface1);

    return ret;
}

SDL_OSAL_Interface test_osal_interface2 =
{
    .enableInterrupt = (pSDL_OSAL_interruptFunction) LBIST_TEST_enableInterrupt,
    .disableInterrupt = (pSDL_OSAL_interruptFunction) LBIST_TEST_disableInterrupt,
    .registerInterrupt = (pSDL_OSAL_registerFunction) LBIST_TEST_registerInterrupt_timeout,
    .deregisterInterrupt = (pSDL_OSAL_deregisterFunction) HwiP_delete,
    .globalDisableInterrupts = (pSDL_OSAL_globalDisableInterruptsFunction) LBIST_TEST_globalDisableInterrupts,
    .globalRestoreInterrupts = (pSDL_OSAL_globalRestoreInterruptsFunction) LBIST_TEST_globalRestoreInterrupts,
    .printFxn = (pSDL_OSAL_printFunction) UART_printf,
    .delay = (pSDL_OSAL_delayFunction) Osal_delay,
    .addrTranslate = (pSDL_OSAL_addrTranslateFunction) LBIST_TEST_addrTranslate
};

int32_t LBIST_TEST2_osalInit(void)
{
    SDL_ErrType_t ret = SDL_PASS;

    ret = SDL_OSAL_init(&test_osal_interface2);

    return ret;
}
