/* =============================================================================
 *   Copyright (c) Texas Instruments Incorporated 2022
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
#include <string.h>
#include <src/sdl/sdl_types.h>
#include <src/ip/sdl_ip_mtog.h>
#include <include/soc.h>
#include <ti/drv/uart/UART_stdio.h>
#include <ti/drv/sciclient/sciclient.h>

/* Osal API header files */
#include <ti/osal/HwiP.h>
#include <ti/osal/TimerP.h>
#include "esm_app.h"

#include "mtog_utils.h"

/* ========================================================================== */
/*                                Macros                                      */
/* ========================================================================== */

/* static global variables */
HwiP_Handle gEsmHiHwiPHandle;
HwiP_Handle gEsmLoHwiPHandle;
HwiP_Handle gEsmCfgHwiPHandle;

/* ----------------- local Function prototypes ------------------ */
static int32_t sdlAppClearESMIntrStatus(uint32_t baseAddr, int32_t intNum);
static int32_t sdlAppEnableIntr(uint32_t baseAddr, int32_t intNum);
static int32_t sdlAppSetPri(uint32_t baseAddr, uint32_t pri, int32_t intNum);
static int32_t sdlAppEnableEsmGlobalIntr(uint32_t baseAddr);
static int32_t sdlAppEsmSetupHighPriHandler(uint32_t intNum);
static int32_t sdlAppEsmSetupLowPriHandler(uint32_t intNum);
static void    sdlAppEsmHighInterruptHandler(uintptr_t arg);
static void    sdlAppEsmLowInterruptHandler(uintptr_t arg);
static void sdlAppEsmInterruptHandler (uintptr_t baseAddr, esmIntrPriorityLvl_t esmIntrPriorityLvlType,
                                       uintptr_t arg );
static void sdlAppEsmProcessInterruptSource(uintptr_t arg, uint32_t intSrc);
static void sdlAppChkIsExpectedEvent(uint32_t baseAddr,  uint32_t intSrc);

volatile uint32_t gSecTestPass;
volatile uint32_t gDedTestPass;
volatile char     gOption;

typedef struct sdlAppEsmHandlerSet_s
{
    ESM_handlerPtr handle;
    uint32_t       parameter;
} sdlAppEsmHandlerSet_t;

static sdlAppEsmHandlerSet_t sdlAppMainEsmHandlerArray[ESM_MAX_EVENT_MAP_NUM_WORDS*32U];
static sdlAppEsmHandlerSet_t sdlAppMcuEsmHandlerArray[ESM_MAX_EVENT_MAP_NUM_WORDS*32U];    

/* local function implementation */
static void sdlAppEsmProcessInterruptSource(uintptr_t baseAddr, uint32_t intSrc)
{
    /* Check if the Interrupt caused by ESM event expected
     */

    if (intSrc != NO_EVENT_VALUE)
    {
        sdlAppChkIsExpectedEvent(baseAddr, intSrc);
        /* Clear this error */
        (void)SDL_ESM_clearIntrStatus(baseAddr, intSrc);
    }
    return;
}

static void sdlAppEsmInterruptHandler (uint32_t  baseAddr,
                                                                esmIntrPriorityLvl_t esmIntrPriorityLvlType,
                                                                uintptr_t arg )
{
    uint32_t             intSrc1, intSrc2;
    esmGroupIntrStatus_t localEsmGroupIntrStatus;

    /* Check on the highest priority event and handle it */
    do {
        (void)SDL_ESM_getGroupIntrStatus(baseAddr, (uint32_t)esmIntrPriorityLvlType,
                                         &localEsmGroupIntrStatus);
        intSrc1 = localEsmGroupIntrStatus.highestPendPlsIntNum;
        sdlAppEsmProcessInterruptSource(baseAddr, intSrc1);
        intSrc2 = localEsmGroupIntrStatus.highestPendLvlIntNum;
        sdlAppEsmProcessInterruptSource(baseAddr, intSrc2);
    } while ((intSrc1 != (uint32_t)(NO_EVENT_VALUE)) || (intSrc2 != (uint32_t)(NO_EVENT_VALUE)));

    return;
}

void sdlAppEsmHighInterruptHandler (uintptr_t arg)
{
    uint32_t intNumber = (uint32_t) arg;
    uintptr_t baseAddr;

    switch(intNumber)
    {
        case ESM_HI_INT:
            baseAddr = ESM_CFG_BASE;
            break;

        case MCU_ESM_HI_INT:
            baseAddr = MCU_ESM_CFG_BASE;
            break;
    }

    /* Call common Interrupt handler */
    sdlAppEsmInterruptHandler(baseAddr, ESM_INTR_PRIORITY_LEVEL_HIGH, intNumber);

    return ;
}

void sdlAppEsmLowInterruptHandler (uintptr_t arg)
{
    uint32_t intNumber = (uint32_t) arg;
    uint32_t baseAddr;

    switch(intNumber)
    {
        case ESM_LO_INT:
            baseAddr = ESM_CFG_BASE;
            break;

        case MCU_ESM_LO_INT:
            baseAddr = MCU_ESM_CFG_BASE;
            break;
    }

    /* Call common Interrupt handler */
    sdlAppEsmInterruptHandler(baseAddr, ESM_INTR_PRIORITY_LEVEL_LOW, intNumber);

    return ;
}

static int32_t sdlAppEsmSetupHighPriHandler(uint32_t intNum)
{
    HwiP_Params       hwiParams;

    HwiP_disableInterrupt(intNum);
    HwiP_Params_init(&hwiParams);
    hwiParams.arg = intNum;
    hwiParams.enableIntr = FALSE;
    /* register the call back function for ESM Hi interrupt */
    gEsmHiHwiPHandle = HwiP_create(intNum,
                                   (HwiP_Fxn) sdlAppEsmHighInterruptHandler,
                                   (void *)&hwiParams);
    HwiP_enableInterrupt(intNum);

    return (SDL_PASS);
}

static int32_t sdlAppEsmSetupLowPriHandler(uint32_t intNum)
{
    HwiP_Params       hwiParams;

    HwiP_disableInterrupt(intNum);
    HwiP_Params_init(&hwiParams);
    hwiParams.arg = intNum;
    hwiParams.enableIntr = FALSE;
    /* register the call back function for ESM Lo interrupt */
    gEsmHiHwiPHandle = HwiP_create(intNum,
                                   (HwiP_Fxn) sdlAppEsmLowInterruptHandler,
                                   (void *)&hwiParams);
    HwiP_enableInterrupt(intNum);
    return (SDL_PASS);
}

/* This function clears the ESM interrrupt status */
static int32_t sdlAppClearESMIntrStatus(uint32_t baseAddr, int32_t intNum)
{
    int32_t   sdlResult;
    uint32_t  intStatus;

    /* Clear interrupt status, so that we start with clean state */
    sdlResult = SDL_ESM_clearIntrStatus(baseAddr, intNum);

    if (sdlResult == SDL_PASS)
    {
        sdlResult = SDL_ESM_getIntrStatus(baseAddr, intNum, &intStatus);
    }
    if (sdlResult == SDL_PASS)
    {
        if (intStatus != ((uint32_t)0U))
        {
            sdlResult = SDL_EFAIL;
        }
    }
    return (sdlResult);
}

/* Enable the ESM event */
static int32_t sdlAppEnableIntr(uint32_t baseAddr, int32_t intNum)
{
    int32_t   sdlResult;
    uint32_t  intStatus;

    /* Enable interrupt and verify if interrupt status is enabled */
    sdlResult = SDL_ESM_enableIntr(baseAddr, intNum);

    if (sdlResult == SDL_PASS)
    {
        sdlResult = SDL_ESM_isEnableIntr(baseAddr, intNum, &intStatus);
    }
    if (sdlResult == SDL_PASS)
    {
        if (intStatus != ((uint32_t)1U))
        {
            sdlResult = SDL_EFAIL;
        }
    }
    return (sdlResult);
}

/* Enable the ESM event */
static int32_t sdlAppDisableIntr(uint32_t baseAddr, int32_t intNum)
{
    int32_t   sdlResult;
    uint32_t  intStatus;

    /* Enable interrupt and verify if interrupt status is enabled */
    sdlResult = SDL_ESM_disableIntr(baseAddr, intNum);

    if (sdlResult == SDL_PASS)
    {
        sdlResult = SDL_ESM_isEnableIntr(baseAddr, intNum, &intStatus);
    }
    if (sdlResult == SDL_PASS)
    {
        if (intStatus != ((uint32_t)0U))
        {
            sdlResult = SDL_EFAIL;
        }
    }
    return (sdlResult);
}
/* Set the ESM Pri for that event */
static int32_t sdlAppSetPri(uint32_t baseAddr, uint32_t pri, int32_t intNum)
{
    int32_t    sdlResult;
    esmIntrPriorityLvl_t intrPriorityLvlWr, intrPriorityLvlRd;

    intrPriorityLvlWr = pri;

    sdlResult = SDL_ESM_setIntrPriorityLvl(baseAddr, intNum, intrPriorityLvlWr);

    if (sdlResult == SDL_PASS)
    {
        sdlResult = SDL_ESM_getIntrPriorityLvl(baseAddr,
                                               intNum,
                                               &intrPriorityLvlRd);
    }
    if (sdlResult == SDL_PASS)
    {
        if (intrPriorityLvlWr != intrPriorityLvlRd)
        {
            sdlResult = SDL_EFAIL;
        }
    }
    return (sdlResult);
}

/* Enable the global interrupt */
static int32_t sdlAppEnableEsmGlobalIntr(uint32_t baseAddr)
{
    int32_t     sdlResult;
    uint32_t    intStatus;

    /* Enable Global interrupt and verify if global interrupt is enabled for ESM */
    sdlResult = SDL_ESM_enableGlobalIntr(baseAddr);

    if (sdlResult == SDL_PASS)
    {
        sdlResult = SDL_ESM_getGlobalIntrEnabledStatus(baseAddr, &intStatus);
    }
    if (sdlResult == SDL_PASS)
    {
        if (intStatus != SDL_TEST_ESM_EN_KEY_ENBALE_VAL)
        {
            sdlResult = SDL_EFAIL;
        }
    }
    return (sdlResult);
}

/* ---------------------------------------------------- */
static void sdlAppChkIsExpectedEvent(uint32_t baseAddr,  uint32_t intSrc)
{
    switch(baseAddr)
    {
        case ESM_CFG_BASE:
            if (sdlAppMainEsmHandlerArray[intSrc].handle != NULL_PTR)
                sdlAppMainEsmHandlerArray[intSrc].handle(sdlAppMainEsmHandlerArray[intSrc].parameter);
            break;

        case MCU_ESM_CFG_BASE:
            if (sdlAppMcuEsmHandlerArray[intSrc].handle != NULL_PTR)
                sdlAppMcuEsmHandlerArray[intSrc].handle(sdlAppMcuEsmHandlerArray[intSrc].parameter);
            break;
    }

    return;
}

/* function initializes ESM */
int32_t sdlAppEsmSetupCommon(uint32_t  baseAddr, uint32_t hiIntNum, uint32_t loIntNum)
{
    int32_t     sdlResult;

    /* ESM reset and configure */
    sdlResult = SDL_ESM_reset(baseAddr);
    if ( sdlResult != SDL_PASS)
    {
        UART_printf( "\r\nESM reset failed...");
    }

    if (sdlResult == SDL_PASS)
    {
        sdlResult = sdlAppEnableEsmGlobalIntr(baseAddr);
        if ( sdlResult != SDL_PASS)
        {
            UART_printf("\r\nESM Enable Global Interrupt Failed...");
        }
    }

    if (sdlResult == SDL_PASS)
    {
        if ( hiIntNum != SDL_APP_INT_NUM_INVALID)
        {
            sdlResult = sdlAppEsmSetupHighPriHandler(hiIntNum);
            if ( sdlResult != SDL_PASS)
            {
                UART_printf("\r\nESM High Priority handler setup Failed...");
            }
        }
    }

    if (sdlResult == SDL_PASS)
    {
        if ( loIntNum != SDL_APP_INT_NUM_INVALID)
        {
            sdlResult = sdlAppEsmSetupLowPriHandler(loIntNum);
            if ( sdlResult != SDL_PASS)
            {
                UART_printf("\r\nESM Lo Priority handler setup Failed...");
            }
        }
    }

    if (sdlResult == SDL_PASS)
    {
        UART_printf("\r\n sdlAppEsmSetupCommon...Done");
    }
    return (sdlResult);
}

/* function initializes ESM */
int32_t sdlAppEsmConfig(uint32_t  baseAddr, uint32_t esmEventNumber,
                        uint32_t priority, ESM_handlerPtr pESMHandler,
                        uint32_t parameter)
{
    int32_t     sdlResult;

    sdlResult = sdlAppClearESMIntrStatus(baseAddr, esmEventNumber);
    if ( sdlResult != SDL_PASS)
    {
        UART_printf("\r\n sdlAppClearESMIntrStatus hi pri event failed...");
    }

     /* Enable interrupt and verify if interrupt status is enabled */
     if (sdlResult == SDL_PASS)
    {
        sdlResult = sdlAppEnableIntr(baseAddr, esmEventNumber);
        if ( sdlResult != SDL_PASS)
        {
            UART_printf("\r\nError in ESM Intr Enable for hi pri Event...");
        }
    }

    /* Assign the priority for the events
     * as single bit errors are corrected, can be assigned to lo pri
     * and double bit errors are not corrected, can be assigned to hi pri
     */

    if (sdlResult == SDL_PASS)
    {
        sdlResult = sdlAppSetPri(baseAddr, priority, esmEventNumber);
        if ( sdlResult != SDL_PASS)
        {
            UART_printf("\r\nError in setting Pri for hi pri Event...");
        }
    }

    if (sdlResult == SDL_PASS)
    {
        if (baseAddr == MCU_ESM_CFG_BASE)
        {
            sdlAppMcuEsmHandlerArray[esmEventNumber].handle = pESMHandler;
            sdlAppMcuEsmHandlerArray[esmEventNumber].parameter = parameter;
        } else if (baseAddr == ESM_CFG_BASE)
        {
            sdlAppMainEsmHandlerArray[esmEventNumber].handle = pESMHandler;
            sdlAppMainEsmHandlerArray[esmEventNumber].parameter = parameter;
        }
    }

    if (sdlResult == SDL_PASS)
    {
        UART_printf("\r\n sdlAppEsmConfig...Done");
    }
    return (sdlResult);
}

int32_t sdlAppEsmDisable(uint32_t  baseAddr, uint32_t esmEventNumber)
{
    int32_t     sdlResult;

    sdlResult = sdlAppClearESMIntrStatus(baseAddr, esmEventNumber);
    if ( sdlResult != SDL_PASS)
    {
        UART_printf("\r\n sdlAppClearESMIntrStatus hi pri event failed...");
    }

     /* Enable interrupt and verify if interrupt status is enabled */
     if (sdlResult == SDL_PASS)
    {
        sdlResult = sdlAppDisableIntr(baseAddr, esmEventNumber);
        if ( sdlResult != SDL_PASS)
        {
            UART_printf("\r\nError in ESM Intr Enable for hi pri Event...");
        }
    }

    /* Disable priority
     */

    if (sdlResult == SDL_PASS)
    {
        sdlResult = sdlAppSetPri(baseAddr, 0U, esmEventNumber);
        if ( sdlResult != SDL_PASS)
        {
            UART_printf("\r\nError in setting Pri for hi pri Event...");
        }
    }

    if (sdlResult == SDL_PASS)
    {
        if (baseAddr == MCU_ESM_CFG_BASE)
        {
            sdlAppMcuEsmHandlerArray[esmEventNumber].handle = NULL;
            sdlAppMcuEsmHandlerArray[esmEventNumber].parameter = 0U;
        } else if (baseAddr == ESM_CFG_BASE)
        {
            sdlAppMainEsmHandlerArray[esmEventNumber].handle = NULL;
            sdlAppMainEsmHandlerArray[esmEventNumber].parameter = 0U;
        }
    }

    if (sdlResult == SDL_PASS)
    {
        UART_printf("\r\n sdlAppEsmDisable...Done");
    }
    return (sdlResult);
}

int32_t sdlAppEsmSetup(void)
{
    int32_t     sdlResult;

    memset(sdlAppMcuEsmHandlerArray, 0, sizeof(sdlAppMcuEsmHandlerArray));
    memset(sdlAppMainEsmHandlerArray, 0, sizeof(sdlAppMainEsmHandlerArray));

    sdlResult = sdlAppEsmSetupCommon(MCU_ESM_CFG_BASE,
                                  ESM_HI_INT,
                                  ESM_LO_INT);

    if (sdlResult == SDL_PASS)
    {
         sdlResult = sdlAppEsmSetupCommon(ESM_CFG_BASE,
                                       MCU_ESM_HI_INT,
                                       MCU_ESM_LO_INT);
    }

    if (sdlResult == SDL_PASS)
    {
        UART_printf("\r\n sdlAppEsmSetup...Done");
    }
    return (sdlResult);
}

/* Nothing past this point */