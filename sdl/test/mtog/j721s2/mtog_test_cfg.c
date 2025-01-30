/*
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

 /**
 *  \file     mtog_test_cfg.c
 *
 *  \brief    This file contains MTOG test configuration
 *
 *  \details  MTOG Test Configuration
 **/

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <stdint.h>
#include <string.h>
#include <src/sdl/sdl_types.h>

#include <include/soc.h>
#include <ti/drv/uart/UART_stdio.h>
#include <ti/drv/sciclient/sciclient.h>

/* Osal API header files */
#include <ti/osal/HwiP.h>
#include <ti/osal/TimerP.h>
#include <ti/osal/osal.h>
#include "esm_app.h"
#include "mtog_test_utils.h"
#if defined (SOC_J7200)
    #include <src/ip/sdl_ip_mtog.h>
    #include "test/mtog/j7200/mtog_test_cfg.h"
#endif 

#if defined (SOC_J721S2)
    #include <src/ip/sdl_ip_mtog.h>
    #include "test/mtog/j721s2/mtog_test_cfg.h"
#endif 

/* =======================================1=================================== */
/*                                Macros                                      */
/* ========================================================================== */

/* ========================================================================== */
/*                                Function prototypes                         */
/* ========================================================================== */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

MTOG_TestHandle_t MTOG_TestHandleArray[MTOG_MAX_INSTANCE] =
{
    /* MAIN_MTOG0 */
    {
        .instanceName           = "MAIN MTOG0",
        .handler                = MTOG_eventHandler, /* MTOG event handler */
        .ESMEventNumber         = SDLR_ESM0_ESM_LVL_EVENT_MASTER_SAFETY_GASKET0_TIMED_OUT_0, /* MTOG ESM event number */
        .doneFlag               = false,                 /* Initialize done flag */
    },
    /* MAIN_MTOG1 */
    {
        .instanceName           = "MAIN MTOG1",
        .handler                = MTOG_eventHandler, /* MTOG event handler */
        .ESMEventNumber         = SDLR_ESM0_ESM_LVL_EVENT_MASTER_SAFETY_GASKET1_TIMED_OUT_0, /* MTOG ESM event number */
        .doneFlag               = false,                 /* Initialize done flag */
    },
    /* MAIN_MTOG4 */
    {
        .instanceName           = "MAIN MTOG4",
        .handler                = MTOG_eventHandler, /* MTOG event handler */
        .ESMEventNumber         = SDLR_ESM0_ESM_LVL_EVENT_MASTER_SAFETY_GASKET4_TIMED_OUT_0, /* MTOG ESM event number */
        .doneFlag               = false,                 /* Initialize done flag */
    },
    /* MAIN_MTOG5 */
    {
        .instanceName           = "MAIN MTOG5",
        .handler                = MTOG_eventHandler, /* MTOG event handler */
        .ESMEventNumber         = SDLR_ESM0_ESM_LVL_EVENT_MASTER_SAFETY_GASKET5_TIMED_OUT_0, /* MTOG ESM event number */
        .doneFlag               = false,                 /* Initialize done flag */
    },
    /* MAIN_MTOG14 */
    {
        .instanceName           = "MAIN MTOG14",
        .handler                = MTOG_eventHandler, /* MTOG event handler */
        .ESMEventNumber         = SDLR_ESM0_ESM_LVL_EVENT_NAVSS0_VIRTSS_VIRTSS_FFI_PVU0_DST_TIMED_OUT_0, /* MTOG ESM event number */
        .doneFlag               = false,                 /* Initialize done flag */
    },
    /* MAIN_MTOG24 */ 
    {
        .instanceName           = "MAIN MTOG24",
        .handler                = MTOG_eventHandler, /* MTOG event handler */
        .ESMEventNumber         = SDLR_ESM0_ESM_LVL_EVENT_MASTER_SAFETY_GASKET24_TIMED_OUT_0, /* MTOG ESM event number */
        .doneFlag               = false,                 /* Initialize done flag */
    },
    /* MAIN_MTOG25 */
    {
        .instanceName           = "MAIN MTOG25",
        .handler                = MTOG_eventHandler, /* MTOG event handler */
        .ESMEventNumber         = SDLR_ESM0_ESM_LVL_EVENT_MASTER_SAFETY_GASKET25_TIMED_OUT_0, /* MTOG ESM event number */
        .doneFlag               = false,                 /* Initialize done flag */
    },
    /* MAIN_MTOG32 */
    {
        .instanceName           = "MAIN MTOG32",
        .handler                = MTOG_eventHandler, /* MTOG event handler */
        .ESMEventNumber         = SDLR_ESM0_ESM_LVL_EVENT_MASTER_SAFETY_GASKET32_TIMED_OUT_0, /* MTOG ESM event number */
        .doneFlag               = false,                 /* Initialize done flag */
    },
    /* MAIN_MTOG33 */
    {
        .instanceName           = "MAIN MTOG33",
        .handler                = MTOG_eventHandler, /* MTOG event handler */
        .ESMEventNumber         = SDLR_ESM0_ESM_LVL_EVENT_MASTER_SAFETY_GASKET33_TIMED_OUT_0, /* MTOG ESM event number */
        .doneFlag               = false,                 /* Initialize done flag */
    },
    /* MAIN_MTOG34 */
    {
        .instanceName           = "MAIN MTOG34",
        .handler                = MTOG_eventHandler, /* MTOG event handler */
        .ESMEventNumber         = SDLR_ESM0_ESM_LVL_EVENT_MASTER_SAFETY_GASKET34_TIMED_OUT_0, /* MTOG ESM event number */
        .doneFlag               = false,                 /* Initialize done flag */
    },
    /* MAIN_MTOG35 */
    {
        .instanceName           = "MAIN MTOG35",
        .handler                = MTOG_eventHandler, /* MTOG event handler */
        .ESMEventNumber         = SDLR_ESM0_ESM_LVL_EVENT_MASTER_SAFETY_GASKET35_TIMED_OUT_0, /* MTOG ESM event number */
        .doneFlag               = false,                 /* Initialize done flag */
    },
    /* MAIN_MTOG36 */
    {
        .instanceName           = "MAIN MTOG36",
        .handler                = MTOG_eventHandler, /* MTOG event handler */
        .ESMEventNumber         = SDLR_ESM0_ESM_LVL_EVENT_MASTER_SAFETY_GASKET36_TIMED_OUT_0, /* MTOG ESM event number */
        .doneFlag               = false,                 /* Initialize done flag */
    },
    /* MAIN_MTOG37 */
    {
        .instanceName           = "MAIN MTOG37",
        .handler                = MTOG_eventHandler, /* MTOG event handler */
        .ESMEventNumber         = SDLR_ESM0_ESM_LVL_EVENT_MASTER_SAFETY_GASKET37_TIMED_OUT_0, /* MTOG ESM event number */
        .doneFlag               = false,                 /* Initialize done flag */
    },
    /* MAIN_MTOG38 */
    {
        .instanceName           = "MAIN MTOG38",
        .handler                = MTOG_eventHandler, /* MTOG event handler */
        .ESMEventNumber         = SDLR_ESM0_ESM_LVL_EVENT_MASTER_SAFETY_GASKET38_TIMED_OUT_0, /* MTOG ESM event number */
        .doneFlag               = false,                 /* Initialize done flag */
    },
    /* MAIN_MTOG39 */
    {
        .instanceName           = "MAIN MTOG39",
        .handler                = MTOG_eventHandler, /* MTOG event handler */
        .ESMEventNumber         = SDLR_ESM0_ESM_LVL_EVENT_MASTER_SAFETY_GASKET39_TIMED_OUT_0, /* MTOG ESM event number */
        .doneFlag               = false,                 /* Initialize done flag */
    },
    /* MCU_MTOG0 */
    {
        .instanceName           = "MCU MTOG0",
        .handler                = MTOG_eventHandler, /* MTOG event handler */
        .ESMEventNumber         = SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_MASTER_SAFETY_GASKET0_TIMED_OUT_0, /* MTOG ESM event number */
        .doneFlag               = false,                 /* Initialize done flag */
    },
    /* MCU_MTOG16 */
    {
        .instanceName           = "MCU MTOG16",
        .handler                = MTOG_eventHandler, /* MTOG event handler */
        .ESMEventNumber         = SDLR_MCU_ESM0_ESM_LVL_EVENT_MASTER_SAFETY_GASKET16_TIMED_OUT_0, /* MTOG ESM event number */
        .doneFlag               = false,                 /* Initialize done flag */
    },
    /* MCU_MTOG17 */
    {
        .instanceName           = "MCU MTOG17",
        .handler                = MTOG_eventHandler, /* MTOG event handler */
        .ESMEventNumber         = SDLR_MCU_ESM0_ESM_LVL_EVENT_MASTER_SAFETY_GASKET17_TIMED_OUT_0, /* MTOG ESM event number */
        .doneFlag               = false,                 /* Initialize done flag */
    },
    /* MCU_MTOG18 */
    {
        .instanceName           = "MCU MTOG18",
        .handler                = MTOG_eventHandler, /* MTOG event handler */
        .ESMEventNumber         = SDLR_MCU_ESM0_ESM_LVL_EVENT_MASTER_SAFETY_GASKET18_TIMED_OUT_0, /* MTOG ESM event number */
        .doneFlag               = false,                 /* Initialize done flag */
    },
    /* MCU_MTOG19 */
    {
        .instanceName           = "MCU MTOG19",
        .handler                = MTOG_eventHandler, /* MTOG event handler */
        .ESMEventNumber         = SDLR_MCU_ESM0_ESM_LVL_EVENT_MASTER_SAFETY_GASKET19_TIMED_OUT_0, /* MTOG ESM event number */
        .doneFlag               = false,                 /* Initialize done flag */
    },
    /* MCU_MTOG20 */
    {
        .instanceName           = "MCU MTOG20",
        .handler                = MTOG_eventHandler, /* MTOG event handler */
        .ESMEventNumber         = SDLR_MCU_ESM0_ESM_LVL_EVENT_MASTER_SAFETY_GASKET20_TIMED_OUT_0, /* MTOG ESM event number */
        .doneFlag               = false,                 /* Initialize done flag */
    },
    /* MCU_MTOG21 */
    {
        .instanceName           = "MCU MTOG21",
        .handler                = MTOG_eventHandler, /* MTOG event handler */
        .ESMEventNumber         = SDLR_MCU_ESM0_ESM_LVL_EVENT_MASTER_SAFETY_GASKET21_TIMED_OUT_0, /* MTOG ESM event number */
        .doneFlag               = false,                 /* Initialize done flag */
    },
    /* MCU_MTOG22 */
    {
        .instanceName           = "MCU MTOG22",
        .handler                = MTOG_eventHandler, /* MTOG event handler */
        .ESMEventNumber         = SDLR_MCU_ESM0_ESM_LVL_EVENT_MASTER_SAFETY_GASKET22_TIMED_OUT_0, /* MTOG ESM event number */
        .doneFlag               = false,                 /* Initialize done flag */
    },
    /* MCU_MTOG23 */
    {
        .instanceName           = "MCU MTOG23",
        .handler                = MTOG_eventHandler, /* MTOG event handler */
        .ESMEventNumber         = SDLR_MCU_ESM0_ESM_LVL_EVENT_MASTER_SAFETY_GASKET23_TIMED_OUT_0, /* MTOG ESM event number */
        .doneFlag               = false,                 /* Initialize done flag */
    },
};

/* Nothing past this point */
