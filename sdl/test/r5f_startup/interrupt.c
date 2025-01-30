/*
 *  Copyright (C) 2017-2018 Texas Instruments Incorporated - http://www.ti.com/
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
 *  \file   interrupt.c
 *
 *  \brief  Interrupt related common APIs for Nonos and FreeRTOS.
 **/

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <stdint.h>
#include <stddef.h>
#include "interrupt.h"
#include "sdl_arm_r5.h"
#include "sdl_vim.h"

/* ========================================================================== */
/*                                Macros                                      */
/* ========================================================================== */
/* None */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
/** \brief Base address of VIM */
uint32_t gVimBaseAddr;
void       *argArray[R5_VIM_INTR_NUM]       = {0};
IntrFuncPtr fxnArray[R5_VIM_INTR_NUM]       = {0};
uint32_t    intrSrcType[R5_VIM_INTR_NUM]    = {(uint32_t)SDL_VIM_INTR_TYPE_LEVEL};
uint16_t    intrPri[R5_VIM_INTR_NUM]        = {0};
uint8_t     intrMap[R5_VIM_INTR_NUM]        = {0};
SDL_R5ExptnHandlers gExptnHandlers          = {NULL};

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

void Intc_IntClrPend(uint16_t intrNum)
{
    SDL_VIM_clrIntrPending( (SDL_vimRegs *)(uintptr_t)gVimBaseAddr, intrNum );
    SDL_VIM_ackIntr( (SDL_vimRegs *)(uintptr_t)gVimBaseAddr, (SDL_VimIntrMap)intrMap[intrNum] );
}

void Intc_Init(void)
{
    uint32_t loopCnt;
    SDL_ArmR5CPUInfo info;

    SDL_armR5GetCpuID(&info);
#if defined(SDL_MAIN_DOMAIN_VIM_BASE_ADDR0) && defined(SDL_MAIN_DOMAIN_VIM_BASE_ADDR1)
    if (info.grpId == (uint32_t)SDL_ARM_R5_CLUSTER_GROUP_ID_0)
    {
        /* MCU SS Pulsar R5 SS */
        gVimBaseAddr = (info.cpuID == SDL_ARM_R5_CPU_ID_0)?
                                      SDL_MCU_DOMAIN_VIM_BASE_ADDR0:
                                      SDL_MCU_DOMAIN_VIM_BASE_ADDR1;
    }
    else
    {
        /* MAIN SS Pulsar R5 SS */
        gVimBaseAddr = (info.cpuID == SDL_ARM_R5_CPU_ID_0)?
                                      SDL_MAIN_DOMAIN_VIM_BASE_ADDR0:
                                      SDL_MAIN_DOMAIN_VIM_BASE_ADDR1;
    }
#else
    /* MCU SS Pulsar R5 SS */
    gVimBaseAddr = (info.cpuID == SDL_ARM_R5_CPU_ID_0)?
                                  SDL_MCU_DOMAIN_VIM_BASE_ADDR0:
                                  SDL_MCU_DOMAIN_VIM_BASE_ADDR1;
#endif
    /* Initialize interrupt source type for each interrupt */
    for (loopCnt = 0U ; loopCnt < R5_VIM_INTR_NUM ; loopCnt++)
    {
        intrSrcType[loopCnt] = (uint32_t)SDL_VIM_INTR_TYPE_LEVEL;
        intrPri[loopCnt] = 0;
        intrMap[loopCnt] = 0;
    }
}

void Intc_IntSetSrcType(uint16_t intrNum, uint32_t type)
{
    if( intrNum < R5_VIM_INTR_NUM )
    {
        intrSrcType[intrNum] = type;
    }
}

void Intc_IntPrioritySet(uint16_t intrNum, uint16_t priority, uint8_t hostIntRoute)
{
    if( intrNum < R5_VIM_INTR_NUM )
    {
        intrPri[intrNum] = priority;
        intrMap[intrNum] = hostIntRoute;
    }
}

void Intc_IntEnable(uint16_t intrNum)
{
    if( intrNum < R5_VIM_INTR_NUM )
    {
        SDL_VIM_setIntrEnable( (SDL_vimRegs *)(uintptr_t)gVimBaseAddr, intrNum, true );    /* Enable interrupt in vim */
    }
}

void Intc_IntDisable(uint16_t intrNum)
{
    if( intrNum < R5_VIM_INTR_NUM )
    {
        SDL_VIM_setIntrEnable( (SDL_vimRegs *)(uintptr_t)gVimBaseAddr, intrNum, false );   /* Disable interrupt in vim */
    }
}


uintptr_t Intc_SystemDisable(void)
{
    uintptr_t cookie;

    cookie = SDL_armR5GetCpsrRegVal();
    /* Disable IRQ and FIQ */
    SDL_armR5DisableIrqFiq();

    return cookie;
}

void Intc_SystemRestore(uintptr_t cookie)
{
    SDL_armR5EnableIrqFiq(cookie);
}

void Intc_SystemEnable(void)
{
    SDL_armR5IntrEnableFiq(1);  /* Enable FIQ interrupt in R5 */
    SDL_armR5IntrEnableIrq(1);  /* Enable IRQ interrupt in R5 */
}


void Intc_InitExptnHandlers(SDL_R5ExptnHandlers *handlers)
{
    handlers->udefExptnHandler = NULL;
    handlers->swiExptnHandler = NULL;
    handlers->pabtExptnHandler = NULL;
    handlers->dabtExptnHandler = NULL;
    handlers->irqExptnHandler = NULL;
    handlers->fiqExptnHandler = NULL;
    handlers->udefExptnHandlerArgs = NULL;
    handlers->swiExptnHandlerArgs = NULL;
    handlers->pabtExptnHandlerArgs = NULL;
    handlers->dabtExptnHandlerArgs = NULL;
    handlers->irqExptnHandlerArgs = NULL;
    handlers->fiqExptnHandlerArgs = NULL;
}

void Intc_RegisterExptnHandlers(const SDL_R5ExptnHandlers *handlers)
{
    /* Copying exception handlers */
    gExptnHandlers.udefExptnHandler = handlers->udefExptnHandler;
    gExptnHandlers.swiExptnHandler = handlers->swiExptnHandler;
    gExptnHandlers.pabtExptnHandler = handlers->pabtExptnHandler;
    gExptnHandlers.dabtExptnHandler = handlers->dabtExptnHandler;
    gExptnHandlers.irqExptnHandler = handlers->irqExptnHandler;
    gExptnHandlers.fiqExptnHandler = handlers->fiqExptnHandler;
    gExptnHandlers.udefExptnHandlerArgs = handlers->udefExptnHandlerArgs;
    gExptnHandlers.swiExptnHandlerArgs = handlers->swiExptnHandlerArgs;
    gExptnHandlers.pabtExptnHandlerArgs = handlers->pabtExptnHandlerArgs;
    gExptnHandlers.dabtExptnHandlerArgs = handlers->dabtExptnHandlerArgs;
    gExptnHandlers.irqExptnHandlerArgs = handlers->irqExptnHandlerArgs;
    gExptnHandlers.fiqExptnHandlerArgs = handlers->fiqExptnHandlerArgs;
}


/********************************* End of file ******************************/
