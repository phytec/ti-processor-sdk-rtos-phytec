/*
 *   Copyright (c) Texas Instruments Incorporated 2021
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
 *  \file     tog_test_cfg.c
 *
 *  \brief    This file contains TOG test configuration
 *
 *  \details  TOG Test Configuration
 **/

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <stdint.h>
#include <string.h>
#include <src/sdl/sdl_types.h>
#include <src/ip/sdl_ip_tog.h>
#include <include/soc.h>
#include <ti/drv/uart/UART_stdio.h>
#include <ti/drv/sciclient/sciclient.h>
#include <sdlr.h>
#include <sdlr64.h>
#include <src/ip/sdl_ip_rat.h>
#include <src/sdl/sdl_rat.h>
#include <ti/csl/csl_cbass.h>
#include <ti/csl/soc.h>
#include "ti/csl/cslr_cbass.h"
#include <ti/csl/cslr_vpac3.h>
#include <ti/csl/cslr_cp_ace.h>

/* Osal API header files */
#include <ti/osal/HwiP.h>
#include <ti/osal/TimerP.h>
#include <ti/osal/osal.h>
#include "esm_app.h"
#include "tog_test_utils.h"
#include "test/tog/j721s2/tog_test_cfg.h"

#define SDL_CBASS_QOS_EP_MAP_ORDERID_MASK                                      (0x000000F0U)
#define SDL_CBASS_QOS_EP_MAP_ORDERID_SHIFT                                     (0x00000004U)
#define SDL_CBASS_QOS_EP_MAP_ORDERID_MAX                                       (0x0000000FU)
/* ========================================================================== */
/*                                Macros                                      */
/* ========================================================================== */


/* ========================================================================== */
/*                                Function prototypes                         */
/* ========================================================================== */
__attribute((section(".text:TOG_test"))) void TOG_injectWKUP_ESMError(uint32_t instanceIndex);
__attribute((section(".text:TOG_test"))) void TOG_injectTimeoutError(uint32_t instanceIndex);
__attribute((section(".text:TOG_test"))) void TOG_injectESMtimeOutError(uint32_t instanceIndex);
__attribute((section(".text:TOG_test"))) void TOG_injectMCU64BTimeoutError(uint32_t instanceIndex);
__attribute((section(".text:TOG_test"))) void TOG_injectESMError(uint32_t instanceIndex);
__attribute((section(".text:TOG_test"))) void TOG_injectMCU_ESMError(uint32_t instanceIndex);
__attribute((section(".text:TOG_test"))) void TOG_injectQOS_ESMError(uint32_t instanceIndex);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

static volatile uint32_t gTestMSMCLocation[256] __attribute((section(".data:TOG_MSMC_location")));

TOG_TestHandle_t TOG_TestHandleArray[TOG_MAX_INSTANCE+1] __attribute((section(".data:TOG_test")))=
{
    /* NAVSS_TO_AC stog_0  */
    {
        .instanceName           = "NAVSS_TO_AC",
        .handler                = TOG_eventHandler,                   /* TOG event handler */
        .injectFunction         = TOG_injectESMtimeOutError,     /* TOG inject error */
        .ESMEventNumber         = SDLR_ESM0_ESM_LVL_EVENT_AM_NAVSS_TO_AC_NON_SAFE_STOG4_TRANS_ERR_LVL_0, /* TOG ESM event number */
        .doneFlag               = false,                 /* Initialize done flag */
        .timeoutValue           = 0x1000000U,              /* timeout value */
        .intSrcBitmap           = SDL_TOG_INTRSRC_ALL,
		.endPointAccess         = 0x4F00000000,
        .flushMode              = true,
    },
    /* AC_CFG_TO_AC stog_1  */
    {
        .instanceName           = "AC_CFG_TO_AC",
        .handler                = TOG_eventHandler,       /* TOG event handler */
        .injectFunction         = TOG_injectESMError,     /* TOG inject ESM error */
        .ESMEventNumber         = SDLR_ESM0_ESM_LVL_EVENT_AM_AC_CFG_TO_AC_CFG_NON_SAFE_STOG9_TRANS_ERR_LVL_0, /* TOG ESM event number */
        .doneFlag               = false,                  /* Initialize done flag */
        .timeoutValue           = 0x10000000U,               /* timeout value */
        .intSrcBitmap           = SDL_TOG_INTRSRC_ALL,
		.endPointAccess         = 0x0F400000 ,
        .flushMode              = true,
    },

    /* AC_CFG stog_2  */
    {
        .instanceName           = "AC_CFG",
        .handler                = TOG_eventHandler,                   /* TOG event handler */
        .injectFunction         = TOG_injectESMError,     /* TOG inject error */
        .ESMEventNumber         = SDLR_ESM0_ESM_LVL_EVENT_AM_AC_CFG_TO_AC_CFG_NON_SAFE_STOG2_TRANS_ERR_LVL_0, /* TOG ESM event number */
        .doneFlag               = false,                 /* Initialize done flag */
        .timeoutValue           = 0x10000000U,              /* timeout value */
        .intSrcBitmap           = SDL_TOG_INTRSRC_ALL,
		.endPointAccess         = 0x04210000 ,
        .flushMode              = true,
    },

    /* RC_TO_HC2_6 stog_3  */
    {
        .instanceName           = "RC_TO_HC2",
        .handler                = TOG_eventHandler,       /* TOG event handler */
        .injectFunction         = TOG_injectESMError,     /* TOG inject ESM error */
        .ESMEventNumber         = SDLR_ESM0_ESM_LVL_EVENT_AM_RC_TO_HC2_STOG6_TRANS_ERR_LVL_0, /* TOG ESM event number */
        .doneFlag               = false,                  /* Initialize done flag */
        .timeoutValue           = 0x10000000U,               /* timeout value */
        .intSrcBitmap           = SDL_TOG_INTRSRC_ALL,
		.endPointAccess         = 0x18000000 ,
        .flushMode              = true,
    },

    /* RC_TO_HC2_7  stog_4  */
    {
        .instanceName           = "RC_TO_HC2_2",
        .handler                = TOG_eventHandler,       /* TOG event handler */
        .injectFunction         = TOG_injectQOS_ESMError,   /* TOG inject ESM error */
        .ESMEventNumber         = SDLR_ESM0_ESM_LVL_EVENT_AM_RC_TO_HC2_STOG7_TRANS_ERR_LVL_0, /* TOG ESM event number */
        .doneFlag               = false,                  /* Initialize done flag */
        .timeoutValue           = 0x10000000U,               /* timeout value */
        .intSrcBitmap           = SDL_TOG_INTRSRC_ALL,
        .endPointAccess         = 0x18000000 ,
    },

    /* HC2_TO_HC stog_5  */
    {
        .instanceName           = "HC2_TO_HC",
        .handler                = TOG_eventHandler,       /* TOG event handler */
        .injectFunction         = TOG_injectESMError, /* TOG inject ESM error */
        .ESMEventNumber         = SDLR_ESM0_ESM_LVL_EVENT_AM_HC2_TO_HC_CFG_STOG5_TRANS_ERR_LVL_0, /* TOG ESM event number */
        .doneFlag               = false,                  /* Initialize done flag */
        .timeoutValue           = 0x10000000U,               /* timeout value */
        .intSrcBitmap           = SDL_TOG_INTRSRC_ALL,
        .endPointAccess         = 0x04104000 ,
        .flushMode              = true,
    },

    /* RC_TO_RC stog_6  */
    {
        .instanceName           = "RC_TO_RC",
        .handler                = TOG_eventHandler,       /* TOG event handler */
        .injectFunction         = TOG_injectESMError,     /* TOG inject ESM error */
        .ESMEventNumber         = SDLR_ESM0_ESM_LVL_EVENT_AM_RC_TO_RC_CFG_STOG3_TRANS_ERR_LVL_0, /* TOG ESM event number */
        .doneFlag               = false,                  /* Initialize done flag */
        .timeoutValue           = 0x10000000U,               /* timeout value */
        .intSrcBitmap           = SDL_TOG_INTRSRC_ALL,
        .endPointAccess         = 0x05380000 ,
        .flushMode              = true,
    },

    /* IPPHY_TO_RTI stog_7  */
    {
        .instanceName           = "IPPHY_TO_RTI",
        .handler                = TOG_eventHandler,       /* TOG event handler */
        .injectFunction         = TOG_injectESMError,     /* TOG inject ESM error */
        .ESMEventNumber         = SDLR_ESM0_ESM_LVL_EVENT_AM_IPPHY_TO_RTI_GPU_STOG8_TRANS_ERR_LVL_0, /* TOG ESM event number */
        .doneFlag               = false,                  /* Initialize done flag */
        .timeoutValue           = 0x10000000U,               /* timeout value */
        .intSrcBitmap           = SDL_TOG_INTRSRC_ALL,
        .endPointAccess         = 0x022F0000 ,
        .flushMode              = true,
    },

    /* IPPHY_TO_IPPHY stog_8  */
    {
        .instanceName           = "IPPHY_TO_IPPHY",
        .handler                = TOG_eventHandler,       /* TOG event handler */
        .injectFunction         = TOG_injectESMError,     /* TOG inject ESM error */
        .ESMEventNumber         = SDLR_ESM0_ESM_LVL_EVENT_AM_IPPHY_TO_IPPHY_STOG1_TRANS_ERR_LVL_0, /* TOG ESM event number */
        .doneFlag               = false,                  /* Initialize done flag */
        .timeoutValue           = 0x10000000U,               /* timeout value */
        .intSrcBitmap           = SDL_TOG_INTRSRC_ALL,
        .endPointAccess         = 0x02200000 ,
        .flushMode              = true,
    },

    /* MAIN_INFRA stog_9  */
    {
        .instanceName           = "MAIN_INFRA",
        .handler                = TOG_eventHandler,       /* TOG event handler */
        .injectFunction         = TOG_injectESMError,     /* TOG inject ESM error */
        .ESMEventNumber         = SDLR_ESM0_ESM_LVL_EVENT_AM_MAIN_INFRA_TO_MAIN_INFRA_STOG0_TRANS_ERR_LVL_0, /* TOG ESM event number */
        .doneFlag               = false,                  /* Initialize done flag */
        .timeoutValue           = 0x10000000U,               /* timeout value */
        .intSrcBitmap           = SDL_TOG_INTRSRC_ALL,
        .endPointAccess         = 0x00A30000 ,
        .flushMode              = true,
    },

	/* PVU0_CFG stog_16 */
    {
        .instanceName           = "PVU0_CFG",
        .handler                = TOG_eventHandler,       /* TOG event handler */
        .injectFunction         = TOG_injectESMError,     /* TOG inject ESM error */
        .ESMEventNumber         = SDLR_ESM0_ESM_LVL_EVENT_NAVSS0_VIRTSS_VIRTSS_FFI_PVU0_CFG_TRANS_ERR_LVL_0, /* TOG ESM event number */
        .doneFlag               = false,                  /* Initialize done flag */
        .timeoutValue           = 0x10000000U,               /* timeout value */
        .intSrcBitmap           = SDL_TOG_INTRSRC_ALL,
		.endPointAccess         = 0x30F80000 ,
        .flushMode              = true,
    },
#if 0	
    /* PVU0_SRC stog_17 */
    {
        .instanceName           = "PVU0_SRC",
        .handler                = TOG_eventHandler,                   /* TOG event handler */
        .injectFunction         = TOG_injectPVUSrc_Error,     /* TOG inject error */
        .ESMEventNumber         = SDLR_ESM0_ESM_LVL_EVENT_NAVSS0_VIRTSS_VIRTSS_FFI_PVU0_SRC_TRANS_ERR_LVL_0, /* TOG ESM event number */
        .doneFlag               = false,                 /* Initialize done flag */
        .timeoutValue           = 0x10000000U,              /* timeout value */
        .intSrcBitmap           = SDL_TOG_INTRSRC_ALL,
    },
#endif	
	/* WKUP_VDC_INFRA stog_10 */
    {
        .instanceName           = "WKUP_VDC_INFRA",
        .handler                = TOG_eventHandler,                   /* TOG event handler */
        .injectFunction         = TOG_injectWKUP_ESMError,     /* TOG inject error */
        .ESMEventNumber         = SDLR_WKUP_ESM0_ESM_LVL_EVENT_WKUP_VDC_INFRA_VBUSP_32B_SRC_SAFEG0_INFRA_SAFEG_TRANS_ERR_LVL_0,/* TOG ESM event number */
        .doneFlag               = false,                 /* Initialize done flag */
        .timeoutValue           = 0x100000U,              /* timeout value */
        .intSrcBitmap           = 0x0U,
		.endPointAccess         = 0x00410000 ,
    },
	
	/* TIMEOUT_64B2 stog_11 */
    {
        .instanceName           = "TIMEOUT_64B2",
        .handler                = TOG_eventHandler,       /* TOG event handler */
        .injectFunction         = TOG_injectTimeoutError,     /* TOG inject ESM error */
        .ESMEventNumber         = SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_TIMEOUT_64B2_TRANS_ERR_LVL_0, /* TOG ESM event number */
        .doneFlag               = false,                  /* Initialize done flag */
        .timeoutValue           = 0x10000000U,               /* timeout value */
        .intSrcBitmap           = SDL_TOG_INTRSRC_ALL,
		.endPointAccess         = 0x02400000 ,
    },
	
	/* TIMEOUT_64B3 stog_12*/
    {
        .instanceName           = "TIMEOUT_64B3",
        .handler                = TOG_eventHandler,       /* TOG event handler */
        .injectFunction         = TOG_injectMCU64BTimeoutError,     /* TOG inject ESM error */
        .ESMEventNumber         = SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_TIMEOUT_64B3_TRANS_ERR_LVL_0, /* TOG ESM event number */
        .doneFlag               = false,                  /* Initialize done flag */
        .timeoutValue           = 0x10000000U,               /* timeout value */
        .intSrcBitmap           = SDL_TOG_INTRSRC_ALL,
		.endPointAccess         = 0x50000000 ,
        .flushMode              = true,
    },

    /* TIMEOUT_64B4 stog_13*/
    {
        .instanceName           = "TIMEOUT_64B4",
        .handler                = TOG_eventHandler,                   /* TOG event handler */
        .injectFunction         = TOG_injectMCU64BTimeoutError,     /* TOG inject error */
        .ESMEventNumber         = SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_TIMEOUT_64B4_TRANS_ERR_LVL_0, /* TOG ESM event number */
        .doneFlag               = false,                 /* Initialize done flag */
        .timeoutValue           = 0x10000000U,              /* timeout value */
        .intSrcBitmap           = SDL_TOG_INTRSRC_ALL,
		.endPointAccess         = 0x58000000 ,
        .flushMode              = true,
    },
	
	/* MCU_VDC_SOC stog_14*/	
    {
        .instanceName           = "MCU FW1",
        .handler                = TOG_eventHandler,       /* TOG event handler */
        .injectFunction         = TOG_injectMCU_ESMError,     /* TOG inject ESM error */
        .ESMEventNumber         = SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_VDC_SOC_FW_VBUSP_32B_SRC_SAFEG1_SOC_FW_SAFEG_TRANS_ERR_LVL_0, /* TOG ESM event number */
        .doneFlag               = false,                  /* Initialize done flag */
        .timeoutValue           = 0x1000000U,               /* timeout value */
        .intSrcBitmap           = 0x0U,
    },

	/* MCU_VDC_INFRA stog_15*/
    {
        .instanceName           = "MCU_VDC_INFRA",
        .handler                = TOG_eventHandler,       /* TOG event handler */
        .injectFunction         = TOG_injectESMError, /* TOG inject ESM error */
        .ESMEventNumber         = SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_VDC_INFRA_VBUSP_32B_SRC_SAFEG0_INFRA_SAFEG_TRANS_ERR_LVL_0, /* TOG ESM event number */
        .doneFlag               = false,                  /* Initialize done flag */
        .timeoutValue           = 0x10000000U,               /* timeout value */
        .intSrcBitmap           = SDL_TOG_INTRSRC_ALL,
		.endPointAccess         = 0x00A90000 ,
        .flushMode              = true,
    },

};

void TOG_injectMCU_ESMError(uint32_t instanceIndex)
{
    /* Do transaction that will exercise TOG */
    SDL_ESM_setIntrStatusRAW(MCU_ESM_BASE, TOG_TestHandleArray[instanceIndex].ESMEventNumber);
}

void TOG_injectESMtimeOutError(uint32_t instanceIndex)
{
    SDL_TOG_Inst instance;
    SDL_TOG_config cfg;
    instance = instanceIndex;
    cfg.cfgCtrl = SDL_TOG_CFG_TIMEOUT;
    int32_t result, status;

    /* Injecting error can result in a Data abort, so disable temporarily */
    disableABORT();

    /* Call SDL API to set smaller timeout to trigger error */
    cfg.timeoutVal = 1u;
    status = SDL_TOG_init(instance, &cfg);
    if (status != SDL_PASS)
    {
        UART_printf("   Inject SDL_TOG_init TimeoutVal Failed \n");
        /* Assert */
    }
	
	SDL_ratTranslationCfgInfo translationCfg;
	/* Add RAT configuration to access address > 32bit address range */
	translationCfg.translatedAddress = 0x4F00000000;
	translationCfg.sizeInBytes = 528U;
	translationCfg.baseAddress = (uint32_t)0x60000000U;
	uint32_t index = 0U;
	uint32_t *address;
	
	/* Set up RAT translation */
	result = SDL_RAT_configRegionTranslation((SDL_ratRegs *)SDL_MCU_R5FSS0_RAT_CFG_BASE,
											index, &translationCfg);
	if (result == SDL_PASS) {
		address = (uint32_t *)0x60000000U;
	}
	
    SDL_TOG_setFlushMode(instanceIndex,TOG_TestHandleArray[instanceIndex].flushMode);
	SDL_REG32_RD(address);
	
    /* Call SDL API to set configure back to original timeout value */
    cfg.timeoutVal = TOG_TestHandleArray[instanceIndex].timeoutValue;
    status = SDL_TOG_init(instance, &cfg);
    if (status != SDL_PASS)
    {
        UART_printf("   Configure back SDL_TOG_init TimeoutVal Failed \n");
        /* Assert */
    }

    /* Enable back Abort */
    enableABORT();
}

void TOG_injectESMError(uint32_t instanceIndex)
{
    SDL_TOG_Inst instance;
    SDL_TOG_config cfg;
    instance = instanceIndex;
    cfg.cfgCtrl = SDL_TOG_CFG_TIMEOUT;
    int32_t status;
    /* Injecting error can result in a Data abort, so disable temporarily */
    disableABORT();

    /* Call SDL API to set smaller timeout to trigger error */
    cfg.timeoutVal = 1u;
    status = SDL_TOG_init(instance, &cfg);
    if (status != SDL_PASS)
    {
        UART_printf("   Inject SDL_TOG_init TimeoutVal Failed \n");
        /* Assert */
    }
	
    SDL_TOG_setFlushMode(instanceIndex,TOG_TestHandleArray[instanceIndex].flushMode);
    SDL_REG32_RD(TOG_TestHandleArray[instanceIndex].endPointAccess);
	
    /* Call SDL API to set configure back to original timeout value */
    cfg.timeoutVal = TOG_TestHandleArray[instanceIndex].timeoutValue;
    status = SDL_TOG_init(instance, &cfg);
    if (status != SDL_PASS)
    {
        UART_printf("   Configure back SDL_TOG_init TimeoutVal Failed \n");
        /* Assert */
    }

    /* Enable back Abort */
    enableABORT();
}

void TOG_injectWKUP_ESMError(uint32_t instanceIndex)
{
    /* Do transaction that will exercise TOG */
    SDL_ESM_setIntrStatusRAW(MCU_ESM_BASE, TOG_TestHandleArray[instanceIndex].ESMEventNumber);
}
#if 0

#define QOS_R5FSS0_CORE0_MEM_WR                     SDL_QOS_MCU_R5FSS0_CPU0_WMST_MMR_BASE
#define QOS_R5FSS0_CORE0_MEM_WR_CBASS_MAP(i)        (QOS_R5FSS0_CORE0_MEM_WR + 0x100 + (i) * 4)

void TOG_injectPVUSrc_Error(uint32_t instanceIndex)
{
    /* Do transaction that will exercise TOG */
    SDL_TOG_config cfg;
    SDL_TOG_Inst instance;
    instance = instanceIndex;
    cfg.cfgCtrl = SDL_TOG_CFG_TIMEOUT;
    int32_t status;

    /* Injecting error can result in a Data abort, so disable temporarily */
    disableABORT();

    /* Call SDL API to set smaller timeout to trigger error */
    cfg.timeoutVal = 1u;
    status = SDL_TOG_init(instance, &cfg);
    if (status != SDL_PASS)
    {
        UART_printf("   Inject SDL_TOG_init TimeoutVal Failed \n");
        /* Assert */
    }

    /* This TOG can be triggered by PVU SRC access. Trigger the access by
     * briefly setting up QoS */
    /* setup qos for channel 0 for atype PVU (0x1) */
    CSL_REG32_WR(QOS_R5FSS0_CORE0_MEM_WR_CBASS_MAP(0x0), (0x1 << 28));
    /* setup qos for channel 0 for atype "not translated" (0x0) */
	//CSL_REG32_WR(QOS_R5FSS0_CORE0_MEM_WR_CBASS_MAP(0x0), (0x0 << 28));

    /* Call SDL API to set configure back to original timeout value */
    cfg.timeoutVal = TOG_TestHandleArray[instanceIndex].timeoutValue;
    status = SDL_TOG_init(instance, &cfg);
    if (status != SDL_PASS)
    {
        UART_printf("   Configure back SDL_TOG_init TimeoutVal Failed \n");
        /* Assert */
    }

    /* Enable back Abort */
    enableABORT();
}
#endif

void TOG_injectTimeoutError(uint32_t instanceIndex)
{
    SDL_TOG_config cfg;
    SDL_TOG_Inst instance;
    instance = instanceIndex;
    cfg.cfgCtrl = SDL_TOG_CFG_TIMEOUT;
    int32_t status;
    int i;

    /* Injecting error can result in a Data abort, so disable temporarily */
    disableABORT();

    /* Call SDL API to set smaller timeout to trigger error */
    cfg.timeoutVal = 1u;
    status = SDL_TOG_init(instance, &cfg);
    if (status != SDL_PASS)
    {
        UART_printf("   Inject SDL_TOG_init TimeoutVal Failed \n");
        /* Assert */
    }

    /* Do something to trigger transaction through the Gasket */
    for (i=0; i< 256; i++)
    {
        gTestMSMCLocation[i] = 1U;
    }
    /* Call SDL API to set configure back to original timeout value */
    cfg.timeoutVal = TOG_TestHandleArray[instanceIndex].timeoutValue;
    status = SDL_TOG_init(instance, &cfg);
    if (status != SDL_PASS)
    {
        UART_printf("   Configure back SDL_TOG_init TimeoutVal Failed \n");
        /* Assert */
    }

    /* Enable back Abort */
    enableABORT();
}

void TOG_injectMCU64BTimeoutError(uint32_t instanceIndex)
{
    SDL_TOG_config cfg;
    SDL_TOG_Inst instance;
    instance = instanceIndex;
    cfg.cfgCtrl = SDL_TOG_CFG_TIMEOUT;
    int32_t status;
    int i;

    /* Injecting error can result in a Data abort, so disable temporarily */
    disableABORT();

    /* Call SDL API to set smaller timeout to trigger error */
    cfg.timeoutVal = 1u;
    status = SDL_TOG_init(instance, &cfg);
    if (status != SDL_PASS)
    {
        UART_printf("   Inject SDL_TOG_init TimeoutVal Failed \n");
        /* Assert */
    }

    SDL_TOG_setFlushMode(instanceIndex,TOG_TestHandleArray[instanceIndex].flushMode);
    /* Do something to trigger transaction through the Gasket */
    for (i=0; i< 256; i++)
    {
        gTestMSMCLocation[i] = 1U;
        SDL_REG64_RD(TOG_TestHandleArray[instanceIndex].endPointAccess);
        Osal_delay(20);
    }
    /* Call SDL API to set configure back to original timeout value */
    cfg.timeoutVal = TOG_TestHandleArray[instanceIndex].timeoutValue;
    status = SDL_TOG_init(instance, &cfg);
    if (status != SDL_PASS)
    {
        UART_printf("   Configure back SDL_TOG_init TimeoutVal Failed \n");
        /* Assert */
    }

    /* Enable back Abort */
    enableABORT();
}

void TOG_injectQOS_ESMError(uint32_t instanceIndex)
{
    /* Do transaction that will exercise TOG */
    SDL_TOG_config cfg;
    SDL_TOG_Inst instance;
    instance = instanceIndex;
    cfg.cfgCtrl = SDL_TOG_CFG_TIMEOUT;
    int32_t status;
    /* Injecting error can result in a Data abort, so disable temporarily */
    disableABORT();

    /* Call SDL API to set smaller timeout to trigger error */
    cfg.timeoutVal = 1u;
    status = SDL_TOG_init(instance, &cfg);
    if (status != SDL_PASS)
    {
        UART_printf("   Inject SDL_TOG_init TimeoutVal Failed \n");
        /* Assert */
    }
    SDL_REG32_FINS( (SDL_MCU_CBASS0_QOS_BASE + 0x00000100ull), CBASS_QOS_EP_MAP_ORDERID, 0xF );
    SDL_REG32_RD(TOG_TestHandleArray[instanceIndex].endPointAccess);
	
    /* Call SDL API to set configure back to original timeout value */
    cfg.timeoutVal = TOG_TestHandleArray[instanceIndex].timeoutValue;
    status = SDL_TOG_init(instance, &cfg);
    if (status != SDL_PASS)
    {
        UART_printf("   Configure back SDL_TOG_init TimeoutVal Failed \n");
        /* Assert */
    }

    /* Enable back Abort */
    enableABORT();
}

/* Nothing past this point */
