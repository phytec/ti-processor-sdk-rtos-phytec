/*
 *  Copyright (C) 2022 Texas Instruments Incorporated
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
 *  \file mcrc_fullCPU.c
 *
 *  \brief Common across use-cases using MCRC Full-CPU mode.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "main.h"
/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
#define MAX_LOOPCOUNT                       (20U)
#define MAX_LENGTH_OF_MCRC_CONFIG_PARAMS    (8U)
#define POSITIVE_TEST                       (1U)
#define NEGATIVE_TEST                       (2U)

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
/* None */

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */
/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */
/* None */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */
/** \brief Defines the various MCRC use cases. */
static SDL_MCRC_ConfigParams_t params[(uint32_t)MAX_LENGTH_OF_MCRC_CONFIG_PARAMS] =
{
    { /* 32bit atomic write */
        MCRC_MCU_NAVSS,
        (uint32_t) SDL_MCRC_CHANNEL_1,
        (uint32_t) SDL_MCRC_OPERATION_MODE_FULLCPU,
        4U,
        MCRC_DEF_PATTERN_COUNT,
        MCRC_DEF_SECTOR_COUNT,
        MCRC_DEF_WATCHDOG_PRELOAD,
        MCRC_DEF_BLOCK_PRELOAD,
        0xf2c2e9eeU,
        0xebeb19c1U,
        MCRC_DDR_720P_BUF_SIZE,
        (uint32_t) &gDdrMCRCSrcBuffer[0],
        (uint32_t)POSITIVE_TEST,
        (uint32_t)SDL_MCRC_DATA_32_BIT,
    },
    {/* 32bit atomic write */
        NAVSS0_MCRC_0,
        (uint32_t) SDL_MCRC_CHANNEL_1,
        (uint32_t) SDL_MCRC_OPERATION_MODE_FULLCPU,
        4U,
        MCRC_DEF_PATTERN_COUNT,
        MCRC_DEF_SECTOR_COUNT,
        MCRC_DEF_WATCHDOG_PRELOAD,
        MCRC_DEF_BLOCK_PRELOAD,
        0xf2c2e9eeU,
        0xebeb19c1U,
        MCRC_DDR_720P_BUF_SIZE,
        (uint32_t) &gDdrMCRCSrcBuffer[0],
        (uint32_t)POSITIVE_TEST,
        (uint32_t)SDL_MCRC_DATA_32_BIT,
    },
    { /* 32bit atomic write */
        MCRC_MCU_NAVSS,
        (uint32_t) SDL_MCRC_CHANNEL_1,
        (uint32_t) SDL_MCRC_OPERATION_MODE_FULLCPU,
        4U,
        MCRC_DEF_PATTERN_COUNT,
        MCRC_DEF_SECTOR_COUNT,
        MCRC_DEF_WATCHDOG_PRELOAD,
        MCRC_DEF_BLOCK_PRELOAD,
        0xf2c2e9ffU,
        0xebeb19c2U,
        MCRC_DDR_720P_BUF_SIZE,
        (uint32_t) &gDdrMCRCSrcBuffer[0],
        (uint32_t)NEGATIVE_TEST,
        (uint32_t)SDL_MCRC_DATA_32_BIT,
    },
    {/* 32bit atomic write */
        NAVSS0_MCRC_0,
        (uint32_t) SDL_MCRC_CHANNEL_1,
        (uint32_t) SDL_MCRC_OPERATION_MODE_FULLCPU,
        4U,
        MCRC_DEF_PATTERN_COUNT,
        MCRC_DEF_SECTOR_COUNT,
        MCRC_DEF_WATCHDOG_PRELOAD,
        MCRC_DEF_BLOCK_PRELOAD,
        0xf2c2e9ceU,
        0xebeb1911U,
        MCRC_DDR_720P_BUF_SIZE,
        (uint32_t) &gDdrMCRCSrcBuffer[0],
        (uint32_t)NEGATIVE_TEST,
        (uint32_t)SDL_MCRC_DATA_32_BIT,
    },
    {/* 64bit atomic write */
        MCRC_MCU_NAVSS,
        (uint32_t) SDL_MCRC_CHANNEL_1,
        (uint32_t) SDL_MCRC_OPERATION_MODE_FULLCPU,
        4U,
        MCRC_DEF_PATTERN_COUNT,
        MCRC_DEF_SECTOR_COUNT,
        MCRC_DEF_WATCHDOG_PRELOAD,
        MCRC_DEF_BLOCK_PRELOAD,
        0x8bacd4feU,
        0x10598f29U,
        MCRC_DDR_720P_BUF_SIZE,
        (uint32_t) &gDdrMCRCSrcBuffer[0],
        (uint32_t)POSITIVE_TEST,
        (uint32_t)SDL_MCRC_DATA_64_BIT,
    },
    {/* 64bit atomic write */
        NAVSS0_MCRC_0,
        (uint32_t) SDL_MCRC_CHANNEL_1,
        (uint32_t) SDL_MCRC_OPERATION_MODE_FULLCPU,
        4U,
        MCRC_DEF_PATTERN_COUNT,
        MCRC_DEF_SECTOR_COUNT,
        MCRC_DEF_WATCHDOG_PRELOAD,
        MCRC_DEF_BLOCK_PRELOAD,
        0x8bacd4feU,
        0x10598f29U,
        MCRC_DDR_720P_BUF_SIZE,
        (uint32_t) &gDdrMCRCSrcBuffer[0],
        (uint32_t)POSITIVE_TEST,
        (uint32_t)SDL_MCRC_DATA_64_BIT,
    },
    {/* 64bit atomic write */
        MCRC_MCU_NAVSS,
        (uint32_t) SDL_MCRC_CHANNEL_1,
        (uint32_t) SDL_MCRC_OPERATION_MODE_FULLCPU,
        4U,
        MCRC_DEF_PATTERN_COUNT,
        MCRC_DEF_SECTOR_COUNT,
        MCRC_DEF_WATCHDOG_PRELOAD,
        MCRC_DEF_BLOCK_PRELOAD,
        0x8bacd4ffU,
        0x10598f28U,
        MCRC_DDR_720P_BUF_SIZE,
        (uint32_t) &gDdrMCRCSrcBuffer[0],
        (uint32_t)NEGATIVE_TEST,
        (uint32_t)SDL_MCRC_DATA_64_BIT,
    },
    {/* 64bit atomic write */
        NAVSS0_MCRC_0,
        (uint32_t) SDL_MCRC_CHANNEL_1,
        (uint32_t) SDL_MCRC_OPERATION_MODE_FULLCPU,
        4U,
        MCRC_DEF_PATTERN_COUNT,
        MCRC_DEF_SECTOR_COUNT,
        MCRC_DEF_WATCHDOG_PRELOAD,
        MCRC_DEF_BLOCK_PRELOAD,
        0x8bacd480U,
        0x10598f27U,
        MCRC_DDR_720P_BUF_SIZE,
        (uint32_t) &gDdrMCRCSrcBuffer[0],
        (uint32_t)NEGATIVE_TEST,
        (uint32_t)SDL_MCRC_DATA_64_BIT,
    },
};

int32_t sdl_mcrcFullCPU_main(void)
{
    int32_t result;
    int32_t retVal=0;
    SDL_MCRC_dataConfig_t mcrcData;
    uint32_t i;
    uint32_t *pMCRCData;
    uint32_t useCase;
    for (useCase=0; useCase<(uint32_t)MAX_LENGTH_OF_MCRC_CONFIG_PARAMS; useCase++)
    {
        if(params[useCase].dataBitSize == SDL_MCRC_DATA_32_BIT)
        {
            UART_printf("\n MCRC FULL CPU mode : starting 32 bit atomic write");
            mcrcData.dataBitSize 	= params[useCase].dataBitSize;
        }
        else
        {
            UART_printf("\n MCRC FULL CPU mode : starting 64 bit atomic write");
            mcrcData.dataBitSize 	= params[useCase].dataBitSize;
        }
        mcrcData.pMCRCData       = (uint32_t *)params[useCase].sourceMemory;
        mcrcData.size            = params[useCase].dataSize;
        SDL_MCRC_Signature_t  sectSignVal;

        result = SDL_MCRC_init(params[useCase].instance,params[useCase].mcrcChannelNumber,
                               params[useCase].mcrcWatchdogPreload,params[useCase].mcrcBlockPreload);
        if (result == SDL_PASS)
        {
            result = SDL_MCRC_channelReset(params[useCase].instance,params[useCase].mcrcChannelNumber);
        }
        if (result == SDL_PASS)
        {
            result = SDL_MCRC_config(params[useCase].instance,params[useCase].mcrcChannelNumber,params[useCase].mcrcPatternCount,
                        params[useCase].mcrcSectorCount, params[useCase].mcrcMode);
        }

        pMCRCData = (uint32_t *)mcrcData.pMCRCData;
        for (i = 0; i < (mcrcData.size / 4U); i++)
        {
            pMCRCData[i] = i;
        }

        result = SDL_MCRC_computeSignCPUmode(params[useCase].instance,
                                params[useCase].mcrcChannelNumber,
                                &mcrcData, &sectSignVal);
        if (result == SDL_PASS)
        {
            /*
             * Check if the generated MCRC signature value
             * matches with the reference signaure vaule
             */
             if(params[useCase].testType == (uint32_t)POSITIVE_TEST )
             {
                if ((sectSignVal.regH == params[useCase].mcrcSignHigh) &&
                    (sectSignVal.regL == params[useCase].mcrcSignLow))
                {
                    UART_printf("\n Full_CPU mode MCRC signature verification done successfully for positive test for the usecase = %d \n\n",useCase);
                    result = SDL_PASS;
                }
                else{
                    UART_printf("\n Full_CPU mode MCRC signature verification failed for positive test for the usecase = %d \n\n",useCase);
                    result = SDL_EFAIL;
                }
             }
             else
             { /* To get EFAIL result*/
                if ((sectSignVal.regH == params[useCase].mcrcSignHigh) &&
                    (sectSignVal.regL == params[useCase].mcrcSignLow))
                {
                    UART_printf("\n Full_CPU mode MCRC signature verification failed for negative test for the usecase = %d \n\n",useCase);
                    result = SDL_EFAIL;
                }
                else
                {
                    UART_printf("\n Full_CPU mode MCRC signature verification done successfully for negative test for the usecase = %d \n\n",useCase);
                    result = SDL_PASS;
                }
             }
        }
    }

    return retVal;
}

