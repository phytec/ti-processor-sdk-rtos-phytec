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
 *  \file mcrc_semiCPU.c
 *
 *  \brief Common across use-cases using MCRC Semi-CPU mode.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "main.h"
#include <ti/drv/udma/udma.h>
#include <ti/csl/src/ip/udmap/V0/csl_udmap.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
/** \brief Number of times to perform the MCRC operation */
#define LOOP_COUNT          (20U)

/*
 * Ring parameters
 */
/** \brief Number of ring entries - we can prime this much MCRC operations */
#define UDMA_APP_RING_ENTRIES      (1U)
/** \brief Size (in bytes) of each ring entry (Size of pointer - 64-bit) */
#define UDMA_APP_RING_ENTRY_SIZE   (sizeof(uint64_t))
/** \brief Total ring memory */
#define UDMA_APP_RING_MEM_SIZE     (UDMA_APP_RING_ENTRIES * \
                                         UDMA_APP_RING_ENTRY_SIZE)
/**
 *  \brief UDMA TR packet descriptor memory.
 *  This contains the CSL_UdmapCppi5TRPD + Padding to sizeof(CSL_UdmapTR15) +
 *  one Type_15 TR (CSL_UdmapTR15) + one TR response of 4 bytes.
 *  Since CSL_UdmapCppi5TRPD is less than CSL_UdmapTR15, size is just two times
 *  CSL_UdmapTR15 for alignment.
 */
#define UDMA_APP_TRPD_SIZE         ((sizeof(CSL_UdmapTR15) * 2U) + 4U)
#define MAX_LENGTH_OF_MCRC_CONFIG_PARAMS   (9U)
#define POSITIVE_TEST                       (1U)
#define NEGATIVE_TEST                       (2U)

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
/*
 * UDMA driver objects
 */
struct Udma_DrvObj      gUdmaDrvObj;
struct Udma_ChObj       gUdmaChObj;
struct Udma_EventObj    gUdmaCqEventObj;
struct Udma_EventObj    gUdmaTdCqEventObj;

/*
 * UDMA Memories
 */
static uint8_t gTxRingMem[UDMA_APP_RING_MEM_SIZE] __attribute__((aligned(UDMA_CACHELINE_ALIGNMENT)));
static uint8_t gTxCompRingMem[UDMA_APP_RING_MEM_SIZE] __attribute__((aligned(UDMA_CACHELINE_ALIGNMENT)));
static uint8_t gTxTdCompRingMem[UDMA_APP_RING_MEM_SIZE] __attribute__((aligned(UDMA_CACHELINE_ALIGNMENT)));
static uint8_t gUdmaTprdMem[UDMA_APP_TRPD_SIZE] __attribute__((aligned(UDMA_CACHELINE_ALIGNMENT)));

/* Semaphore to indicate transfer completion */
static SemaphoreP_Handle gUdmaAppDoneSem = NULL;


    /** Defines the various MCRC use cases. */
static    SDL_MCRC_ConfigParams_t params[(uint32_t)MAX_LENGTH_OF_MCRC_CONFIG_PARAMS] =
{
    { /* 32bit atomic write */
        MCRC_MCU_NAVSS,
        (uint32_t) SDL_MCRC_CHANNEL_2,
        (uint32_t) SDL_MCRC_OPERATION_MODE_SEMICPU,
        4U,
        MCRC_DEF_PATTERN_COUNT,
        MCRC_DEF_SECTOR_COUNT,
        MCRC_DEF_WATCHDOG_PRELOAD,
        MCRC_DEF_BLOCK_PRELOAD,
        0xc60b4fb8U,
        0xd98307f6U,
        MCRC_DDR_SMALL_BUF_SIZE,
        (uint32_t) &gDdrMCRCSrcBuffer[0],
        (uint32_t)POSITIVE_TEST,
        SDL_MCRC_DATA_32_BIT,
    },

    { /* 32bit atomic write */
        NAVSS0_MCRC_0,
        (uint32_t) SDL_MCRC_CHANNEL_1,
        (uint32_t) SDL_MCRC_OPERATION_MODE_SEMICPU,
        4U,
        MCRC_DEF_PATTERN_COUNT,
        MCRC_DEF_SECTOR_COUNT,
        MCRC_DEF_WATCHDOG_PRELOAD,
        MCRC_DEF_BLOCK_PRELOAD,
        0xc60b4fb8U,
        0xd98307f6U,
        MCRC_DDR_SMALL_BUF_SIZE,
        (uint32_t) &gDdrMCRCSrcBuffer[0],
        (uint32_t)POSITIVE_TEST,
        SDL_MCRC_DATA_32_BIT,
    },

    { /* 32bit atomic write */
        MCRC_MCU_NAVSS,
        (uint32_t) SDL_MCRC_CHANNEL_2,
        (uint32_t) SDL_MCRC_OPERATION_MODE_SEMICPU,
        4U,
        MCRC_DEF_PATTERN_COUNT,
        MCRC_DEF_SECTOR_COUNT,
        MCRC_DEF_WATCHDOG_PRELOAD,
        MCRC_DEF_BLOCK_PRELOAD,
        0U,
        0U,
        MCRC_DDR_SMALL_BUF_SIZE,
        (uint32_t) &gDdrMCRCSrcBuffer[0],
        (uint32_t)POSITIVE_TEST,
        SDL_MCRC_DATA_32_BIT,
    },
    { /* 32bit atomic write */
        MCRC_MCU_NAVSS,
        (uint32_t) SDL_MCRC_CHANNEL_2,
        (uint32_t) SDL_MCRC_OPERATION_MODE_SEMICPU,
        4U,
        MCRC_DEF_PATTERN_COUNT,
        MCRC_DEF_SECTOR_COUNT,
        MCRC_DEF_WATCHDOG_PRELOAD,
        MCRC_DEF_BLOCK_PRELOAD,
        0xc60b4f77U,
        0xd9830765U,
        MCRC_DDR_SMALL_BUF_SIZE,
        (uint32_t) &gDdrMCRCSrcBuffer[0],
        (uint32_t)NEGATIVE_TEST,
        SDL_MCRC_DATA_32_BIT,
    },

    { /* 32bit atomic write */
        NAVSS0_MCRC_0,
        (uint32_t) SDL_MCRC_CHANNEL_1,
        (uint32_t) SDL_MCRC_OPERATION_MODE_SEMICPU,
        4U,
        MCRC_DEF_PATTERN_COUNT,
        MCRC_DEF_SECTOR_COUNT,
        MCRC_DEF_WATCHDOG_PRELOAD,
        MCRC_DEF_BLOCK_PRELOAD,
        0xc60b4f35U,
        0xd9830765U,
        MCRC_DDR_SMALL_BUF_SIZE,
        (uint32_t) &gDdrMCRCSrcBuffer[0],
        (uint32_t)NEGATIVE_TEST,
        SDL_MCRC_DATA_32_BIT,
    },

    {/* 64bit atomic write */
        MCRC_MCU_NAVSS,
        (uint32_t) SDL_MCRC_CHANNEL_1,
        (uint32_t) SDL_MCRC_OPERATION_MODE_SEMICPU,
        8U,
        MCRC_DEF_PATTERN_COUNT,
        MCRC_DEF_SECTOR_COUNT,
        MCRC_DEF_WATCHDOG_PRELOAD,
        MCRC_DEF_BLOCK_PRELOAD,
        0x8bacd4feU,
        0x10598f29U,
        MCRC_DDR_720P_BUF_SIZE,
        (uint32_t) &gDdrMCRCSrcBuffer[0],
        (uint32_t)POSITIVE_TEST,
        SDL_MCRC_DATA_64_BIT,
    },
    {/* 64bit atomic write */
        NAVSS0_MCRC_0,
        (uint32_t) SDL_MCRC_CHANNEL_1,
        (uint32_t) SDL_MCRC_OPERATION_MODE_SEMICPU,
        8U,
        MCRC_DEF_PATTERN_COUNT,
        MCRC_DEF_SECTOR_COUNT,
        MCRC_DEF_WATCHDOG_PRELOAD,
        MCRC_DEF_BLOCK_PRELOAD,
        0x8bacd4feU,
        0x10598f29U,
        MCRC_DDR_720P_BUF_SIZE,
        (uint32_t) &gDdrMCRCSrcBuffer[0],
        (uint32_t)POSITIVE_TEST,
        SDL_MCRC_DATA_64_BIT,
    },
    {/* 64bit atomic write */
        MCRC_MCU_NAVSS,
        (uint32_t) SDL_MCRC_CHANNEL_1,
        (uint32_t) SDL_MCRC_OPERATION_MODE_SEMICPU,
        8U,
        MCRC_DEF_PATTERN_COUNT,
        MCRC_DEF_SECTOR_COUNT,
        MCRC_DEF_WATCHDOG_PRELOAD,
        MCRC_DEF_BLOCK_PRELOAD,
        0x8bacd444U,
        0x10598f22U,
        MCRC_DDR_720P_BUF_SIZE,
        (uint32_t) &gDdrMCRCSrcBuffer[0],
        (uint32_t)NEGATIVE_TEST,
        SDL_MCRC_DATA_64_BIT,
    },
    {/* 64bit atomic write */
        NAVSS0_MCRC_0,
        (uint32_t) SDL_MCRC_CHANNEL_1,
        (uint32_t) SDL_MCRC_OPERATION_MODE_SEMICPU,
        8U,
        MCRC_DEF_PATTERN_COUNT,
        MCRC_DEF_SECTOR_COUNT,
        MCRC_DEF_WATCHDOG_PRELOAD,
        MCRC_DEF_BLOCK_PRELOAD,
        0x8bacd4f7U,
        0x10598f24U,
        MCRC_DDR_720P_BUF_SIZE,
        (uint32_t) &gDdrMCRCSrcBuffer[0],
        (uint32_t)NEGATIVE_TEST,
        SDL_MCRC_DATA_64_BIT,
    },
};

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */
/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static int32_t mcrcSemiCPU_mode(Udma_ChHandle chHandle,
                                     SDL_MCRC_ConfigParams_t *params, uint32_t useCase);
static int32_t mcrcSemiCPU_udmaCrc(Udma_ChHandle  chHandle,
                           SDL_MCRC_ConfigParams_t   *params,
                           SDL_MCRC_Signature_t           refSignVal,
                           uint32_t useCase);
static void mcrcSemiCPU_udmaEventCb(Udma_EventHandle eventHandle,
                            uint32_t eventType,
                            void *appData);

static int32_t mcrcSemiCPU_create(Udma_DrvHandle drvHandle, Udma_ChHandle chHandle);
static int32_t mcrcSemiCPU_delete(Udma_DrvHandle drvHandle, Udma_ChHandle chHandle);

static void mcrcSemiCPU_udmaTrpdInit(Udma_ChHandle chHandle,
                                       uint8_t *pTrpdMem,
                                       const void *srcBuf,
                                       const void *destBuf,
                                       uint32_t length,
                                       uint32_t patternSize);
static void App_print(const char *str);

extern void Udma_appUtilsCacheInv(const void * addr, int32_t size);
extern void Udma_appUtilsCacheWb(const void * addr, int32_t size);

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */
int32_t sdl_UDMA_init(void)
{
    int32_t         retVal;
    Udma_InitPrms   initPrms;
    uint32_t        instId;
    Udma_DrvHandle  drvHandle = &gUdmaDrvObj;

    /* Use MCU NAVSS for MCU domain cores. Rest all cores uses Main NAVSS */
#if defined (BUILD_MCU1_0) || defined (BUILD_MCU1_1)
    instId = UDMA_INST_ID_MCU_0;
#else
    instId = UDMA_INST_ID_MAIN_0;
#endif
    /* UDMA driver init */
    UdmaInitPrms_init(instId, &initPrms);
#if defined (QT_BUILD)
    initPrms.skipRmOverlapCheck = TRUE;
#endif
    initPrms.printFxn = &App_print;
    retVal = Udma_init(drvHandle, &initPrms);
    if(SDL_PASS != retVal)
    {
        UART_printf("[Error] UDMA init failed!!\n");
    }

    return (retVal);
}

int32_t sdl_UDMA_deinit(void)
{
    int32_t         retVal;
    Udma_DrvHandle  drvHandle = &gUdmaDrvObj;

    retVal = Udma_deinit(drvHandle);
    if(SDL_PASS != retVal)
    {
        UART_printf("[Error] UDMA deinit failed!!\n");
    }

    return (retVal);
}


int32_t sdl_mcrcSemiCPU_main(void)
{
    int32_t         retVal = SDL_PASS;
    int32_t         retVal1 = SDL_PASS;
    Udma_DrvHandle  drvHandle = &gUdmaDrvObj;
    Udma_ChHandle   chHandle = &gUdmaChObj;
    uint32_t useCase = 0;
    UART_printf("\n\nMCRC Semi_CPU mode : starting\n\n");

    /* UDMA driver init */
    sdl_UDMA_init();

    for(useCase = 0; useCase <(uint32_t)MAX_LENGTH_OF_MCRC_CONFIG_PARAMS; useCase++)
    {
        if(params[useCase].dataBitSize == SDL_MCRC_DATA_32_BIT)
        {
            UART_printf("\n MCRC Semi CPU mode : starting 32 bit atomic write  for useCase =  %d",useCase);
            
        }
        else
        {
            UART_printf("\n MCRC Semi CPU mode : starting 64 bit atomic write for useCase = %d",useCase);
        }
        /* Configure MCRC channel */
        retVal = SDL_MCRC_init(params[useCase].instance,
                    params[useCase].mcrcChannelNumber,
                    params[useCase].mcrcWatchdogPreload,
                    params[useCase].mcrcBlockPreload);
        if(SDL_PASS != retVal)
        {
             UART_printf("[Error] mcrcSemiCPU channel intialization failed!!\n");
        }

        retVal = mcrcSemiCPU_create(drvHandle, chHandle);
        if(SDL_PASS != retVal)
        {
            UART_printf("[Error] mcrcSemiCPU App create failed!!\n");
        }

        if(SDL_PASS == retVal)
        {
            retVal = mcrcSemiCPU_mode(chHandle, params, useCase);
            if(SDL_PASS != retVal)
            {
                UART_printf("[Error] mcrcSemiCPU App MCRC failed!!\n");
            }
        }

        retVal1 = mcrcSemiCPU_delete(drvHandle, chHandle);
        if(SDL_PASS != retVal1)
        {
            UART_printf("[Error] mcrcSemiCPU App delete failed!!\n");
        }

    }
    sdl_UDMA_deinit();
    return (retVal + retVal1);
}


/* ========================================================================== */
/*                 Internal Function Definitions                              */
/* ========================================================================== */
static int32_t mcrcSemiCPU_mode(Udma_ChHandle chHandle,
                                     SDL_MCRC_ConfigParams_t *params, uint32_t useCase)
{
    int32_t     retVal = SDL_PASS;
    uint32_t    i;
    uint32_t    dataSize;
    uint32_t   *mcrcSourceMemory;
    uint32_t    loopCnt = 0U;
    uint64_t    timeStampBeforeCrcConfig, timeStampAfterCrcComplete;
    uint32_t    mcrcCompletionTime;
    Float32     mcrcSemiCPUPerformance;
    uint32_t    mcrcSemiCPUPerformanceInt;
    SDL_MCRC_Signature_t refSignVal;
    uint32_t IntrMask = 0x1U;


    /* Init buffers */
    mcrcSourceMemory = (uint32_t *) params[useCase].sourceMemory;
    dataSize = (params[useCase].dataSize);
    for (i = 0; i < (dataSize / 4); i++)
    {
        mcrcSourceMemory[i] = i;
    }
    UART_printf("\nCopied reference data into memory @%x.",  params[useCase].sourceMemory);

    /* Writeback source buffer */
    Udma_appUtilsCacheWb(mcrcSourceMemory, dataSize);

    /* Get the reference crc sign value. The reference crc sign value is retrieved
       by performing full cpu mode CRC on the same set of data used for semiCPU mode.
       This is done in the test app to get the reference value for the test, but is
       not required for using semi-CPU mode. */
    if ((0U == params[useCase].mcrcSignHigh) &&
        (0U == params[useCase].mcrcSignLow))
    {
        SDL_MCRC_InstType instance = params[useCase].instance;
        SDL_MCRC_channel_t mcrcChannelNumber = params[useCase].mcrcChannelNumber;
        uint32_t mcrcPatternSize       = params[useCase].mcrcPatternSize;
        uint32_t mcrcPatterCount       = (params[useCase].dataSize) / mcrcPatternSize;
        uint32_t mcrcSectorCount       = params[useCase].mcrcSectorCount;
        SDL_MCRC_SignatureRegAddr_t mcrcPSASignatureRegAddress;
        uint32_t forLoopCount;

        /* Configure the MCRC module in Full CPU mode and get reference MCRC. */
        UART_printf("\nCalculating Reference MCRC signature Value.");
        /* MCRC channel RESET before initialization/configuration. */
        retVal = SDL_MCRC_channelReset(instance, mcrcChannelNumber);

        if (retVal == SDL_PASS)
        {
            /* Initialize MCRC channel */
            retVal = SDL_MCRC_config(instance,
                                  mcrcChannelNumber,
                                  mcrcPatterCount,
                                  mcrcSectorCount,
                                  SDL_MCRC_OPERATION_MODE_FULLCPU);
        }

        if (retVal == SDL_PASS)
        {
            retVal = SDL_MCRC_enableIntr(instance, mcrcChannelNumber,IntrMask);
        }

        if (retVal == SDL_PASS)
        {
            /* Get MCRC PSA signature register address */
            retVal = SDL_MCRC_getPSASigRegAddr(instance,
                                         mcrcChannelNumber,
                                         &mcrcPSASignatureRegAddress);
        }

        if (retVal == SDL_PASS)
        {
            for (forLoopCount = 0;
                 forLoopCount < (dataSize / mcrcPatternSize);
                 forLoopCount++)
            {
                HW_WR_REG32(mcrcPSASignatureRegAddress.regL,
                            mcrcSourceMemory[forLoopCount]);
            }
            /* Fetch MCRC signature value       */
            retVal = SDL_MCRC_getPSASectorSig(instance,
                                        mcrcChannelNumber,
                                        &refSignVal);
        }
        if (retVal == SDL_PASS)
        {
            UART_printf("\n MCRC signature value : 0x%x%xU",
                        refSignVal.regH,
                        refSignVal.regL);
        }
    }
    else
    {
        UART_printf("\nUsing Pre-Defined Reference MCRC signature Value.");
        refSignVal.regH = params[useCase].mcrcSignHigh;
        refSignVal.regL = params[useCase].mcrcSignLow;
    }

    if (retVal == SDL_PASS)
    {
        /*      Get time-stamp     */
        timeStampBeforeCrcConfig = TimerP_getTimeInUsecs();
        while(loopCnt < LOOP_COUNT)
        {
            /* Perform UDMA MCRC */
            retVal = mcrcSemiCPU_udmaCrc(chHandle, params, refSignVal, useCase);
            if(SDL_PASS != retVal)
            {
                break;
            }

            loopCnt++;
        }
        /*      get the time-stamp       */
        timeStampAfterCrcComplete = TimerP_getTimeInUsecs();
    }

    if(SDL_PASS == retVal)
    {
        UART_printf("\nMCRC signature verification done successfully.");
        mcrcCompletionTime = (uint32_t) ((timeStampAfterCrcComplete -
                              timeStampBeforeCrcConfig) / 1000U);
        mcrcSemiCPUPerformance =
            (Float32) (((Float32) params[useCase].dataSize) / mcrcCompletionTime) * 1000U ;
        mcrcSemiCPUPerformance = ((Float32) mcrcSemiCPUPerformance) *
                                ((Float32) LOOP_COUNT);
        mcrcSemiCPUPerformance = (Float32) mcrcSemiCPUPerformance /
                                (1024U * 1024U);
        mcrcSemiCPUPerformanceInt = (uint32_t) mcrcSemiCPUPerformance;
        UART_printf("\nProcessed data of size %u Bytes %d times in %u ms."
               , params[useCase].dataSize, LOOP_COUNT,
               mcrcCompletionTime);
        UART_printf("\nMCRC performance: %u MB/s", mcrcSemiCPUPerformanceInt);
        retVal = SDL_PASS;
    }
    else
    {
        UART_printf("\nCRC signature verification failed.");
        retVal = SDL_EFAIL;
    }
    return (retVal);
}

static int32_t mcrcSemiCPU_udmaCrc(Udma_ChHandle chHandle,
                           SDL_MCRC_ConfigParams_t *params,
                           SDL_MCRC_Signature_t refSignVal,
                           uint32_t useCase)
{
    int32_t               retVal = SDL_PASS;
    uint32_t             *pTrResp, trRespStatus;
    uint64_t              pDesc = 0;
    uint8_t              *tprdMem = &gUdmaTprdMem[0U];
    SDL_MCRC_Signature_t        sectSignVal;
    SDL_MCRC_SignatureRegAddr_t psaSignRegAddr;
    uint32_t              patternCnt, sectCnt;
    SDL_MCRC_InstType instance;
    SDL_MCRC_channel_t          mcrcChannel;
    uint32_t IntrMask = 2U;

    sectSignVal.regL = 0U;
    sectSignVal.regH = 0U;
    patternCnt  = params[useCase].dataSize / params[useCase].mcrcPatternSize;
    sectCnt     = params[useCase].mcrcSectorCount;
    instance = params[useCase].instance;
    mcrcChannel  = params[useCase].mcrcChannelNumber;

    /* Get MCRC PSA signature register address */
    SDL_MCRC_getPSASigRegAddr(instance, mcrcChannel, &psaSignRegAddr);
    SDL_MCRC_channelReset(instance, mcrcChannel);
    retVal = SDL_MCRC_config(instance, mcrcChannel, patternCnt, sectCnt, params[useCase].mcrcMode);

    if (retVal == SDL_PASS)
    {
        retVal = SDL_MCRC_enableIntr(instance, mcrcChannel,IntrMask);
    }

    /* Update TR packet descriptor */
    mcrcSemiCPU_udmaTrpdInit(chHandle,tprdMem,
                                (void *) params[useCase].sourceMemory,
                                (void *)(uintptr_t) psaSignRegAddr.regL,
                                params[useCase].dataSize,
                                params[useCase].mcrcPatternSize);

    /* Submit TRPD to channel */
    retVal = Udma_ringQueueRaw(Udma_chGetFqRingHandle(chHandle), (uint64_t) tprdMem);
    if(SDL_PASS != retVal)
    {
        UART_printf("[Error] Channel queue failed!!\n");
    }

    if(SDL_PASS == retVal)
    {
        /* Wait for return descriptor in completion ring - this marks the
         * transfer completion */
        SemaphoreP_pend(gUdmaAppDoneSem, SemaphoreP_WAIT_FOREVER);

        /* Response received in completion queue */
        retVal = Udma_ringDequeueRaw(Udma_chGetCqRingHandle(chHandle), &pDesc);
        if(SDL_PASS != retVal)
        {
            UART_printf("[Error] No descriptor after callback!!\n");
            retVal = SDL_EFAIL;
        }
    }

    if(SDL_PASS == retVal)
    {
        /*
         * Sanity check
         */
        /* Check returned descriptor pointer */
        if(pDesc != ((uint64_t) tprdMem))
        {
            UART_printf("[Error] TR descriptor pointer returned doesn't "
                   "match the submitted address!!\n");
            retVal = SDL_EFAIL;
        }
    }

    if(SDL_PASS == retVal)
    {
        /* Invalidate cache */
        Udma_appUtilsCacheInv(&gUdmaTprdMem[0U], UDMA_APP_TRPD_SIZE);

        /* check TR response status */
        pTrResp = (uint32_t *) (tprdMem + (sizeof(CSL_UdmapTR15) * 2U));
        trRespStatus = CSL_FEXT(*pTrResp, UDMAP_TR_RESPONSE_STATUS_TYPE);
        if(trRespStatus != CSL_UDMAP_TR_RESPONSE_STATUS_COMPLETE)
        {
            UART_printf("[Error] TR Response not completed!!\n");
            retVal = SDL_EFAIL;
        }
    }

    if(SDL_PASS == retVal)
    {
        uint32_t intrStatus = 0U;

        while ((intrStatus == 0x0U) && (SDL_PASS == retVal))
        {
            /* Wait here till MCRC compression complete is set. */
            retVal = SDL_MCRC_intrStatus(instance, mcrcChannel, &intrStatus);
            UART_printf("MCRC Semi CPU intrrupt status : %d\n", intrStatus);
        }
    }

    if (SDL_PASS == retVal)
    {
        retVal = SDL_MCRC_getPSASectorSig(instance, mcrcChannel, &sectSignVal);
    }

    if (SDL_PASS == retVal)
    {
        if(params[useCase].testType == (uint32_t)POSITIVE_TEST)
        {/* "Positive test, expected that signature will match" */
            UART_printf("\nPositive test, expected that signature will match.");
            /* Compare MCRC signature value against reference MCRC signature */
            if((sectSignVal.regH == refSignVal.regH) && (sectSignVal.regL == refSignVal.regL))
            {
                /* Sector signature matches - Passed */
                UART_printf("\nExpected MCRC signature value : 0x%x%xU",
                    refSignVal.regH,
                    refSignVal.regL);
                UART_printf("\nCalculated MCRC signature value : 0x%08x%08xU",
                    sectSignVal.regH,
                    sectSignVal.regL);

            }
            else
            {
                retVal = SDL_EFAIL;
                UART_printf("\nSector signature does not match.");
                UART_printf("\nExpected MCRC signature value : 0x%x%xU",
                    refSignVal.regH,
                    refSignVal.regL);
                UART_printf("\nCalculated MCRC signature value : 0x%08x%08xU",
                    sectSignVal.regH,
                    sectSignVal.regL);
            }
        }
        else
        { /* "Negative test, expected that signature will not match" */
            /* Compare MCRC signature value against reference MCRC signature */
            if((sectSignVal.regH == refSignVal.regH) && (sectSignVal.regL == refSignVal.regL))
            {
                retVal = SDL_EFAIL;
            }
            else
            {
                retVal = SDL_PASS;
                UART_printf("\nNegative test, expected that signature will not match.");
                UART_printf("\nSector signature does not match.");
                UART_printf("\nExpected MCRC signature value : 0x%x%xU",
                    refSignVal.regH,
                    refSignVal.regL);
                UART_printf("\nCalculated MCRC signature value : 0x%08x%08xU",
                    sectSignVal.regH,
                    sectSignVal.regL);
            }
        }
        if (retVal == SDL_PASS)
        {
            retVal = SDL_MCRC_clearIntr(instance, mcrcChannel, SDL_MCRC_CHANNEL_IRQSTATUS_RAW_MAIN_ALL);
        }
    }

    return (retVal);
}


static void mcrcSemiCPU_udmaEventCb(Udma_EventHandle eventHandle,
                            uint32_t eventType,
                            void *appData)
{
    int32_t         retVal;
    CSL_UdmapTdResponse tdResp;

    UART_printf("\nudmaEventCb occured. eventype %d \n.", eventType);

    if(UDMA_EVENT_TYPE_DMA_COMPLETION == eventType)
    {
        SemaphoreP_post(gUdmaAppDoneSem);
    }
    if(UDMA_EVENT_TYPE_TEARDOWN_PACKET == eventType)
    {
        /* Response received in Teardown completion queue */
        retVal = Udma_chDequeueTdResponse(&gUdmaChObj, &tdResp);
        if(SDL_PASS != retVal)
        {
            /* [Error] No TD response after callback!! */
        }
    }

    return;
}

static int32_t mcrcSemiCPU_create(Udma_DrvHandle drvHandle, Udma_ChHandle chHandle)
{
    int32_t             retVal = SDL_PASS;
    uint32_t            chType;
    Udma_ChPrms         chPrms;
    Udma_ChTxPrms       txPrms;
    Udma_ChRxPrms       rxPrms;
    Udma_EventHandle    eventHandle;
    Udma_EventPrms      eventPrms;
    SemaphoreP_Params   semPrms;

    SemaphoreP_Params_init(&semPrms);
    gUdmaAppDoneSem = SemaphoreP_create(0, &semPrms);
    if(NULL == gUdmaAppDoneSem)
    {
        UART_printf("[Error] Sem create failed!!\n");
        retVal = SDL_EFAIL;
    }

    if(SDL_PASS == retVal)
    {
        /* Init channel parameters */
        chType = UDMA_CH_TYPE_TR_BLK_COPY;
        UdmaChPrms_init(&chPrms, chType);
        chPrms.fqRingPrms.ringMem   = &gTxRingMem[0U];
        chPrms.cqRingPrms.ringMem   = &gTxCompRingMem[0U];
        chPrms.tdCqRingPrms.ringMem = &gTxTdCompRingMem[0U];
        chPrms.fqRingPrms.ringMemSize   = UDMA_APP_RING_MEM_SIZE;
        chPrms.cqRingPrms.ringMemSize   = UDMA_APP_RING_MEM_SIZE;
        chPrms.tdCqRingPrms.ringMemSize = UDMA_APP_RING_MEM_SIZE;
        chPrms.fqRingPrms.elemCnt   = UDMA_APP_RING_ENTRIES;
        chPrms.cqRingPrms.elemCnt   = UDMA_APP_RING_ENTRIES;
        chPrms.tdCqRingPrms.elemCnt = UDMA_APP_RING_ENTRIES;

        /* Open channel for block copy */
        retVal = Udma_chOpen(drvHandle, chHandle, chType, &chPrms);
        if(SDL_PASS != retVal)
        {
            UART_printf("[Error] UDMA channel open failed!!\n");
        }
    }

    if(SDL_PASS == retVal)
    {
        /* Config TX channel */
        UdmaChTxPrms_init(&txPrms, chType);
        retVal = Udma_chConfigTx(chHandle, &txPrms);
        if(SDL_PASS != retVal)
        {
            UART_printf("[Error] UDMA TX channel config failed!!\n");
        }
    }

    if(SDL_PASS == retVal)
    {
        /* Config RX channel - which is implicitly paired to TX channel in
         * block copy mode */
        UdmaChRxPrms_init(&rxPrms, chType);
        retVal = Udma_chConfigRx(chHandle, &rxPrms);
        if(SDL_PASS != retVal)
        {
            UART_printf("[Error] UDMA RX channel config failed!!\n");
        }
    }

    if(SDL_PASS == retVal)
    {
        /* Register ring completion callback */
        eventHandle = &gUdmaCqEventObj;
        UdmaEventPrms_init(&eventPrms);
        eventPrms.eventType         = UDMA_EVENT_TYPE_DMA_COMPLETION;
        eventPrms.eventMode         = UDMA_EVENT_MODE_SHARED;
        eventPrms.chHandle          = chHandle;
        eventPrms.masterEventHandle = NULL;
        eventPrms.eventCb           = &mcrcSemiCPU_udmaEventCb;
        retVal = Udma_eventRegister(drvHandle, eventHandle, &eventPrms);
        if(SDL_PASS != retVal)
        {
            UART_printf("[Error] UDMA CQ event register failed!!\n");
        }
    }

    if(SDL_PASS == retVal)
    {
        /* Register teardown ring completion callback */
        eventHandle = &gUdmaTdCqEventObj;
        UdmaEventPrms_init(&eventPrms);
        eventPrms.eventType         = UDMA_EVENT_TYPE_TEARDOWN_PACKET;
        eventPrms.eventMode         = UDMA_EVENT_MODE_SHARED;
        eventPrms.chHandle          = chHandle;
        eventPrms.masterEventHandle = &gUdmaCqEventObj;
        eventPrms.eventCb           = &mcrcSemiCPU_udmaEventCb;
        retVal = Udma_eventRegister(drvHandle, eventHandle, &eventPrms);
        if(SDL_PASS != retVal)
        {
            UART_printf("[Error] UDMA Teardown CQ event register failed!!\n");
        }
    }

    if(SDL_PASS == retVal)
    {
        /* Channel enable */
        retVal = Udma_chEnable(chHandle);
        if(SDL_PASS != retVal)
        {
            UART_printf("[Error] UDMA channel enable failed!!\n");
        }
    }

    return (retVal);
}

static int32_t mcrcSemiCPU_delete(Udma_DrvHandle drvHandle, Udma_ChHandle chHandle)
{
    int32_t             retVal;
    Udma_EventHandle    eventHandle;

    retVal = Udma_chDisable(chHandle, UDMA_DEFAULT_CH_DISABLE_TIMEOUT);
    if(SDL_PASS != retVal)
    {
        UART_printf("[Error] UDMA channel disable failed!!\n");
    }

    /* Unregister master event at the end */
    eventHandle = &gUdmaTdCqEventObj;
    retVal += Udma_eventUnRegister(eventHandle);
    eventHandle = &gUdmaCqEventObj;
    retVal += Udma_eventUnRegister(eventHandle);
    if(SDL_PASS != retVal)
    {
        UART_printf("[Error] UDMA event unregister failed!!\n");
    }

    retVal += Udma_chClose(chHandle);
    if(SDL_PASS != retVal)
    {
        UART_printf("[Error] UDMA channel close failed!!\n");
    }

    if(gUdmaAppDoneSem != NULL)
    {
        SemaphoreP_delete(gUdmaAppDoneSem);
        gUdmaAppDoneSem = NULL;
    }

    return (retVal);
}

static void mcrcSemiCPU_udmaTrpdInit(Udma_ChHandle chHandle,
                                       uint8_t *pTrpdMem,
                                       const void *srcBuf,
                                       const void *destBuf,
                                       uint32_t length,
                                       uint32_t patternSize)
{
    CSL_UdmapCppi5TRPD *pTrpd = (CSL_UdmapCppi5TRPD *) pTrpdMem;
    CSL_UdmapTR15 *pTr = (CSL_UdmapTR15 *)(pTrpdMem + sizeof(CSL_UdmapTR15));
    uint32_t *pTrResp = (uint32_t *) (pTrpdMem + (sizeof(CSL_UdmapTR15) * 2U));
    uint32_t cqRingNum = Udma_chGetCqRingNum(chHandle);
    uint32_t cCnt;

    /* Make TRPD */
    UdmaUtils_makeTrpd(pTrpd, UDMA_TR_TYPE_15, 1U, cqRingNum);

    /* Setup TR */
    cCnt = 1;
    while ((length / cCnt) > 0x7FFFU)
    {
        cCnt = cCnt * 2;
    }
    pTr->flags    = CSL_FMK(UDMAP_TR_FLAGS_TYPE, 15)                                            |
                    CSL_FMK(UDMAP_TR_FLAGS_STATIC, 0U)                                          |
                    CSL_FMK(UDMAP_TR_FLAGS_EOL, 0U)                                             |   /* NA */
                    CSL_FMK(UDMAP_TR_FLAGS_EVENT_SIZE, CSL_UDMAP_TR_FLAGS_EVENT_SIZE_COMPLETION)|
                    CSL_FMK(UDMAP_TR_FLAGS_TRIGGER0, CSL_UDMAP_TR_FLAGS_TRIGGER_NONE)           |
                    CSL_FMK(UDMAP_TR_FLAGS_TRIGGER0_TYPE, CSL_UDMAP_TR_FLAGS_TRIGGER_TYPE_ALL)  |
                    CSL_FMK(UDMAP_TR_FLAGS_TRIGGER1, CSL_UDMAP_TR_FLAGS_TRIGGER_NONE)           |
                    CSL_FMK(UDMAP_TR_FLAGS_TRIGGER1_TYPE, CSL_UDMAP_TR_FLAGS_TRIGGER_TYPE_ALL)  |
                    CSL_FMK(UDMAP_TR_FLAGS_CMD_ID, 0x25U)                                       |   /* This will come back in TR response */
                    CSL_FMK(UDMAP_TR_FLAGS_SA_INDIRECT, 0U)                                     |
                    CSL_FMK(UDMAP_TR_FLAGS_DA_INDIRECT, 0U)                                     |
                    CSL_FMK(UDMAP_TR_FLAGS_EOP, 1U);
    pTr->icnt0    = patternSize;
    pTr->icnt1    = (length / patternSize) / cCnt;
    pTr->icnt2    = cCnt;
    pTr->icnt3    = 1U;
    pTr->dim1     = pTr->icnt0;
    pTr->dim2     = (pTr->icnt0 * pTr->icnt1);
    pTr->dim3     = (pTr->icnt0 * pTr->icnt1 * pTr->icnt2);
    pTr->addr     = (uint64_t) srcBuf;
    pTr->fmtflags = 0x00000000U;        /* Linear addressing, 1 byte per elem.
                                           Replace with CSL-FL API */
    pTr->dicnt0   = patternSize;
    pTr->dicnt1   = (length / pTr->dicnt0) / cCnt;
    pTr->dicnt2   = cCnt;
    pTr->dicnt3   = 1U;
    pTr->ddim1    = 0U;
    pTr->ddim2    = 0U;
    pTr->ddim3    = 0U;
    pTr->daddr    = (uint64_t) destBuf;

    /* Clear TR response memory */
    *pTrResp = 0xFFFFFFFFU;

    /* Writeback cache */
    Udma_appUtilsCacheWb(pTrpdMem, UDMA_APP_TRPD_SIZE);
    return;
}

static void App_print(const char *str)
{
    UART_printf("%s", str);
    return;
}


