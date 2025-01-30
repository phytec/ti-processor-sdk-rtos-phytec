/* Copyright (c) 2021 Texas Instruments Incorporated
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
 *  \file     main.h
 *
 *  \brief    This file contains MCRC test code defines.
 *
 **/

/*===========================================================================*/
/*                         Include files                                     */
/*===========================================================================*/
/*===========================================================================*/
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <src/sdl/sdl_types.h>
#include <include/soc.h>
#include <ti/drv/uart/UART.h>
#include <ti/drv/uart/UART_stdio.h>
#include <ti/board/board.h>
#include <sdl_mcrc.h>
#include <sdl_ip_mcrc.h>


#if !defined(TEST_MAIN_H)
#define TEST_MAIN_H

#ifdef __cplusplus
extern "C" {
#endif


/*===========================================================================*/
/*                         Declarations                                      */
/*===========================================================================*/

/* Define the test interface */
typedef struct sdlMCRCTest_s
{
    int32_t  (*testFunction)(void);   /* The code that runs the test */
    char      *name;                  /* The test name */
    int32_t    testStatus;            /* Test Status */
} sdlMCRCTest_t;

/*===========================================================================*/
/*                         Macros                                            */
/*===========================================================================*/
#define SDL_APP_TEST_NOT_RUN        (-(int32_t) (2))
#define SDL_APP_TEST_FAILED         (-(int32_t) (1))
#define SDL_APP_TEST_PASS           ( (int32_t) (0))

/*      Default values      */
#define MCRC_DEF_PATTERN_COUNT           ((uint32_t) 0U)
#define MCRC_DEF_SECTOR_COUNT            ((uint32_t) 1U)
#define MCRC_DEF_WATCHDOG_PRELOAD        ((uint32_t) 0U)
#define MCRC_DEF_BLOCK_PRELOAD           ((uint32_t) 0U)

#define MCRC_DDR_1080P_BUF_SIZE          (4147200U)
#define MCRC_DDR_720P_BUF_SIZE           (1843200U)
#define MCRC_DDR_SMALL_BUF_SIZE          (200000U)
#define MCRC_DDR_BUF_MAX_SIZE            (MCRC_DDR_1080P_BUF_SIZE)
#define MCRC_MSMC_BUF_MAX_SIZE           (1024U)

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/**
 *  \brief MCRC configuration parameter structure.
 */
typedef struct
{
    SDL_MCRC_InstType           instance;
    /**< Base address of MCRC instance. */
    SDL_MCRC_channel_t       mcrcChannelNumber;
    /**< MCRC channel to use. */
    SDL_MCRC_ModeType mcrcMode;
    /**< MCRC operational mode. */
    uint32_t            mcrcPatternSize;
    /**< Pattern size. */
    uint32_t            mcrcPatternCount;
    /**< Pattern count. This is calculated in program.*/
    uint32_t            mcrcSectorCount;
    /**< sector count. */
    uint32_t            mcrcWatchdogPreload;
    /**< Watchdog preload value. */
    uint32_t            mcrcBlockPreload;
    /**< Block preload value. */
    uint32_t            mcrcSignHigh;
    /**< MSB 32 bits Pre-calculated Signature value . */
    uint32_t            mcrcSignLow;
    /**< LSB 32 bits Pre-calculated Signature value . */
	uint32_t              dataSize;
    /**< Specify size of data(in Bytes) on which MCRC is to be performed. */
    uint32_t              sourceMemory;
    /**< Specify memory address on which MCRC is to be performed. */
    uint32_t              testType;
    /**< MCRC test type. */
    uint32_t              dataBitSize;
    /**< MCRC atomic write data bit size. */
} SDL_MCRC_ConfigParams_t;

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/**
 *  \brief global variable for holding data buffer of DDR.
 */
static uint8_t gDdrMCRCSrcBuffer[MCRC_DDR_BUF_MAX_SIZE] __attribute__((aligned(128), section(".benchmark_buffer"))) = {1U};
/*===========================================================================*/
/*                         Internal function declarations                    */
/*===========================================================================*/

/*===========================================================================*/
/*                         External function declarations                    */
/*===========================================================================*/
extern int32_t sdl_mcrcSemiCPU_main(void);
extern int32_t sdl_mcrcFullCPU_main(void);
extern void SDL_Udma_appUtilsCacheInv(const void * addr, int32_t size);
extern void SDL_Udma_appUtilsCacheWb(const void * addr, int32_t size);

/*===========================================================================*/
/*                   Local Function definitions                              */
/*===========================================================================*/

#ifdef __cplusplus
}

#endif /*extern "C" */

#endif /* TEST_MAIN_H */
/* Nothing past this point */
