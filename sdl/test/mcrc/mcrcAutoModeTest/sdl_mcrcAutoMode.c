/*
 *  Copyright (C) 2021 Texas Instruments Incorporated
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
 *  \file sdl_mcrcAutoMode.c
 *
 *  \brief Common across test-cases using MCRC Auto mode.
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
/*
 * Application test parameters
 */
 
/** \brief Number of channels */
#define UDMA_TEST_APP_NUM_CH            (2U)
#define MAX_LENGTH_OF_MCRC_CONFIG_PARAMS   (8U)
#define POSITIVE_TEST                       (1U)
#define NEGATIVE_TEST                       (2U)

/*
 * Ring parameters
 */
/** \brief Number of ring entries - we can prime this much memcpy operations */
#define UDMA_TEST_APP_RING_ENTRIES      (1U)
/** \brief Size (in bytes) of each ring entry (Size of pointer - 64-bit) */
#define UDMA_TEST_APP_RING_ENTRY_SIZE   (sizeof(uint64_t))
/** \brief Total ring memory */
#define UDMA_TEST_APP_RING_MEM_SIZE     (UDMA_TEST_APP_RING_ENTRIES * \
                                         UDMA_TEST_APP_RING_ENTRY_SIZE)
/** \brief This ensures every channel memory is aligned */
#define UDMA_TEST_APP_RING_MEM_SIZE_ALIGN ((UDMA_TEST_APP_RING_MEM_SIZE + UDMA_CACHELINE_ALIGNMENT) & ~(UDMA_CACHELINE_ALIGNMENT - 1U))
/**
 *  \brief UDMA TR packet descriptor memory.
 *  This contains the CSL_UdmapCppi5TRPD + Padding to sizeof(CSL_UdmapTR15) +
 *  one Type_15 TR (CSL_UdmapTR15) + one TR response of 4 bytes.
 *  Since CSL_UdmapCppi5TRPD is less than CSL_UdmapTR15, size is just two times
 *  CSL_UdmapTR15 for alignment.
 */
#define UDMA_TEST_APP_TRPD_SIZE_ALIGN         ((sizeof(CSL_UdmapTR15) * 2U) + 4U)
/** \brief This ensures every channel memory is aligned */
#define UDMA_TEST_APP_TRPD_SIZE_ALIGN_ALIGN   ((UDMA_TEST_APP_TRPD_SIZE_ALIGN + UDMA_CACHELINE_ALIGNMENT) & ~(UDMA_CACHELINE_ALIGNMENT - 1U))
/** \brief Number of times to perform the MCRC operation */
#define LOOP_COUNT          (20U)

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
typedef struct
{
    struct Udma_ChObj       chObj;
    Udma_ChHandle           chHandle;
    Udma_DrvHandle          drvHandle;
    /* Semaphore to indicate transfer completion */
	SemaphoreP_Handle       transferDoneSem;
} App_UdmaChObj;

typedef struct
{
    struct Udma_DrvObj      drvObj;
    App_UdmaChObj           appChObj[UDMA_TEST_APP_NUM_CH];
} App_UdmaObj;
/*
 * UDMA Memories
 */
static uint8_t gTxRingMem[UDMA_TEST_APP_NUM_CH][UDMA_TEST_APP_RING_MEM_SIZE_ALIGN] __attribute__((aligned(UDMA_CACHELINE_ALIGNMENT)));
static uint8_t gTxCompRingMem[UDMA_TEST_APP_NUM_CH][UDMA_TEST_APP_RING_MEM_SIZE_ALIGN] __attribute__((aligned(UDMA_CACHELINE_ALIGNMENT)));
static uint8_t gTxTdCompRingMem[UDMA_TEST_APP_NUM_CH][UDMA_TEST_APP_RING_MEM_SIZE_ALIGN] __attribute__((aligned(UDMA_CACHELINE_ALIGNMENT)));
static uint8_t gUdmaTprdMem[UDMA_TEST_APP_NUM_CH][UDMA_TEST_APP_TRPD_SIZE_ALIGN_ALIGN] __attribute__((aligned(UDMA_CACHELINE_ALIGNMENT)));

/*
 * Application Buffers
 */
static uint8_t gUdmaTestSrcBuf[UDMA_TEST_APP_NUM_CH][UDMA_TEST_APP_TRPD_SIZE_ALIGN] __attribute__((aligned(128), section(".benchmark_buffer")));

/*
 * UDMA driver and channel objects
 */
App_UdmaObj gUdmaAppObj;
struct Udma_EventObj    gUdmaCqEventObj[UDMA_TEST_APP_NUM_CH];
struct Udma_EventObj    gUdmaTdCqEventObj[UDMA_TEST_APP_NUM_CH];


/** Defines the various MCRC test cases. */
static	SDL_MCRC_autoConfigParams_t testParams[(uint32_t)MAX_LENGTH_OF_MCRC_CONFIG_PARAMS] =
{
    {/* 32bit atomic write */
        MCRC_MCU_NAVSS,
        (uint32_t) SDL_MCRC_CHANNEL_1,
        (uint32_t) SDL_MCRC_OPERATION_MODE_AUTO,
        4U,
        MCRC_DEF_PATTERN_COUNT,
        MCRC_DEF_SECTOR_COUNT,
        MCRC_DEF_WATCHDOG_PRELOAD,
        MCRC_DEF_BLOCK_PRELOAD,
		0xc60b4fb8U,
        0xd98307f6U,
		MCRC_DDR_SMALL_BUF_SIZE,
        (uint32_t)POSITIVE_TEST,
        (uint32_t)SDL_MCRC_DATA_32_BIT,


	},
	{/* 32bit atomic write */
        NAVSS0_MCRC_0,
        (uint32_t) SDL_MCRC_CHANNEL_1,
        (uint32_t) SDL_MCRC_OPERATION_MODE_AUTO,
        4U,
        MCRC_DEF_PATTERN_COUNT,
        MCRC_DEF_SECTOR_COUNT,
        MCRC_DEF_WATCHDOG_PRELOAD,
        MCRC_DEF_BLOCK_PRELOAD,
		0xc60b4fb8U,
        0xd98307f6U,
		MCRC_DDR_SMALL_BUF_SIZE,
        (uint32_t)POSITIVE_TEST,
        (uint32_t)SDL_MCRC_DATA_32_BIT,
	},
    {/* 32bit atomic write */
        MCRC_MCU_NAVSS,
        (uint32_t) SDL_MCRC_CHANNEL_1,
        (uint32_t) SDL_MCRC_OPERATION_MODE_AUTO,
        4U,
        MCRC_DEF_PATTERN_COUNT,
        MCRC_DEF_SECTOR_COUNT,
        MCRC_DEF_WATCHDOG_PRELOAD,
        MCRC_DEF_BLOCK_PRELOAD,
		0xc60b4fb5U,
        0xd98307f7U,
		MCRC_DDR_SMALL_BUF_SIZE,
        (uint32_t)NEGATIVE_TEST,
        (uint32_t)SDL_MCRC_DATA_32_BIT,

	},
	{/* 32bit atomic write */
        NAVSS0_MCRC_0,
        (uint32_t) SDL_MCRC_CHANNEL_1,
        (uint32_t) SDL_MCRC_OPERATION_MODE_AUTO,
        4U,
        MCRC_DEF_PATTERN_COUNT,
        MCRC_DEF_SECTOR_COUNT,
        MCRC_DEF_WATCHDOG_PRELOAD,
        MCRC_DEF_BLOCK_PRELOAD,
		0xc60b4fb5U,
        0xd98307f7U,
		MCRC_DDR_SMALL_BUF_SIZE,
        (uint32_t)NEGATIVE_TEST,
        (uint32_t)SDL_MCRC_DATA_32_BIT,
	},
    {/* 64bit atomic write */
        MCRC_MCU_NAVSS,
        (uint32_t) SDL_MCRC_CHANNEL_1,
        (uint32_t) SDL_MCRC_OPERATION_MODE_AUTO,
        8U,
        MCRC_DEF_PATTERN_COUNT,
        MCRC_DEF_SECTOR_COUNT,
        MCRC_DEF_WATCHDOG_PRELOAD,
        MCRC_DEF_BLOCK_PRELOAD,
        0x8bacd4feU,
        0x10598f29U,
		MCRC_DDR_720P_BUF_SIZE,
        (uint32_t)POSITIVE_TEST,
        (uint32_t)SDL_MCRC_DATA_64_BIT,


	},
	{/* 64bit atomic write */
        NAVSS0_MCRC_0,
        (uint32_t) SDL_MCRC_CHANNEL_1,
        (uint32_t) SDL_MCRC_OPERATION_MODE_AUTO,
        8U,
        MCRC_DEF_PATTERN_COUNT,
        MCRC_DEF_SECTOR_COUNT,
        MCRC_DEF_WATCHDOG_PRELOAD,
        MCRC_DEF_BLOCK_PRELOAD,
        0x8bacd4feU,
        0x10598f29U,
		MCRC_DDR_720P_BUF_SIZE,
        (uint32_t)POSITIVE_TEST,
        (uint32_t)SDL_MCRC_DATA_64_BIT,
	},
    {/* 64bit atomic write */
        MCRC_MCU_NAVSS,
        (uint32_t) SDL_MCRC_CHANNEL_1,
        (uint32_t) SDL_MCRC_OPERATION_MODE_AUTO,
        8U,
        MCRC_DEF_PATTERN_COUNT,
        MCRC_DEF_SECTOR_COUNT,
        MCRC_DEF_WATCHDOG_PRELOAD,
        MCRC_DEF_BLOCK_PRELOAD,
		0xc60b4fb5U,
        0xd98307f7U,
		MCRC_DDR_720P_BUF_SIZE,
        (uint32_t)NEGATIVE_TEST,
        (uint32_t)SDL_MCRC_DATA_64_BIT,

	},
	{/* 64bit atomic write */
        NAVSS0_MCRC_0,
        (uint32_t) SDL_MCRC_CHANNEL_1,
        (uint32_t) SDL_MCRC_OPERATION_MODE_AUTO,
        8U,
        MCRC_DEF_PATTERN_COUNT,
        MCRC_DEF_SECTOR_COUNT,
        MCRC_DEF_WATCHDOG_PRELOAD,
        MCRC_DEF_BLOCK_PRELOAD,
		0xc60b4fb5U,
        0xd98307f7U,
		MCRC_DDR_720P_BUF_SIZE,
        (uint32_t)NEGATIVE_TEST,
        (uint32_t)SDL_MCRC_DATA_64_BIT,
	}
};

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */
/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static int32_t sdl_mcrcAutoModeTest(App_UdmaObj *appObj,
                                     SDL_MCRC_autoConfigParams_t *testParams, uint32_t testCase);
static int32_t mcrcAutoModeTest(App_UdmaObj *appObj,
                           SDL_MCRC_autoConfigParams_t   *testParams,
                           uint32_t testCase);
static void sdl_mcrcAutoMode_udmaEventCb(Udma_EventHandle eventHandle,
                            uint32_t eventType,
                            void *appData);
static void sdl_mcrcAutoMode_udmaTdEventCb(Udma_EventHandle eventHandle,
                            uint32_t eventType,
                            void *appData);

static int32_t sdl_mcrcAutoMode_create(App_UdmaObj *appObj);
static int32_t sdl_mcrcAutoMode_delete(App_UdmaObj *appObj);

static void sdl_mcrcAutoMode_udmaCrcTrpdInit(Udma_ChHandle chHandle,
                                       uint8_t *pTrpdMem,
                                       const void *srcBuf,
                                       const void *destBuf,
                                       uint32_t length,
                                       uint32_t patternSize);
									   
static void sdl_mcrcAutoMode_udmaPSATrpdInit(Udma_ChHandle chHandle,
                                       uint8_t *pTrpdMem,
                                       const void *srcBuf,
                                       const void *destBuf,
                                       uint32_t length,
                                       uint32_t patternSize);
static void App_print(const char *str);

extern void SDL_Udma_appUtilsCacheInv(const void * addr, int32_t size);
extern void SDL_Udma_appUtilsCacheWb(const void * addr, int32_t size);

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */
static int32_t SDL_MCRC_getMCRCSigRegAddr(SDL_MCRC_InstType instance, SDL_MCRC_channel_t channel,
									SDL_MCRC_SignatureRegAddr_t *pMCRCregAddr)
{
    int32_t status = SDL_PASS;
	uint32_t baseAddr;

    if ((SDL_MCRC_getBaseaddr(instance, &baseAddr) != SDL_PASS)   ||
        (pMCRCregAddr == (NULL_PTR)))
    {
        status = SDL_EBADARGS;
    }
    else
    {
        switch (channel)
        {
            case SDL_MCRC_CHANNEL_1:
                pMCRCregAddr->regH = (baseAddr + SDL_MCRC_REGH1);
                pMCRCregAddr->regL = (baseAddr + SDL_MCRC_REGL1);
                break;
            case SDL_MCRC_CHANNEL_2:
                pMCRCregAddr->regH = (baseAddr + SDL_MCRC_REGH2);
                pMCRCregAddr->regL = (baseAddr + SDL_MCRC_REGL2);
                break;
            case SDL_MCRC_CHANNEL_3:
                pMCRCregAddr->regH = (baseAddr + SDL_MCRC_REGH3);
                pMCRCregAddr->regL = (baseAddr + SDL_MCRC_REGL3);
                break;
            case SDL_MCRC_CHANNEL_4:
                pMCRCregAddr->regH = (baseAddr + SDL_MCRC_REGH4);
                pMCRCregAddr->regL = (baseAddr + SDL_MCRC_REGL4);
                break;
            default:
                status = SDL_EBADARGS;
                break;
        }
    }

    return (status);
}

static int32_t SDL_MCRC_getMCRCSectorSig(SDL_MCRC_InstType     instance,
                           SDL_MCRC_channel_t    channel,
                           SDL_MCRC_Signature_t *mcrcSign)
{
    int32_t status = SDL_PASS;
	uint32_t baseAddr;

    if ((SDL_MCRC_getBaseaddr(instance, &baseAddr) != SDL_PASS)    ||
        (mcrcSign == (NULL_PTR)))
    {
        status = SDL_EBADARGS;
    }
    else
    {
        switch (channel)
        {
            case SDL_MCRC_CHANNEL_1:
                mcrcSign->regH = HW_RD_REG32(baseAddr + SDL_MCRC_REGH1);
                mcrcSign->regL = HW_RD_REG32(baseAddr + SDL_MCRC_REGL1);
                break;                      
            case SDL_MCRC_CHANNEL_2:        
                mcrcSign->regH = HW_RD_REG32(baseAddr + SDL_MCRC_REGH2);
                mcrcSign->regL = HW_RD_REG32(baseAddr + SDL_MCRC_REGL2);
                break;                      
            case SDL_MCRC_CHANNEL_3:        
                mcrcSign->regH = HW_RD_REG32(baseAddr + SDL_MCRC_REGH3);
                mcrcSign->regL = HW_RD_REG32(baseAddr + SDL_MCRC_REGL3);
                break;                      
            case SDL_MCRC_CHANNEL_4:        
                mcrcSign->regH = HW_RD_REG32(baseAddr + SDL_MCRC_REGH4);
                mcrcSign->regL = HW_RD_REG32(baseAddr + SDL_MCRC_REGL4);
                break;
            default:
                status = SDL_EBADARGS;
                break;
        }
    }

    return (status);
}

int32_t sdl_UDMAinit(App_UdmaObj *appObj)
{
    int32_t         retVal;
    Udma_InitPrms   initPrms;
    uint32_t        instId,chIdx;
	App_UdmaChObj  *appChObj;
    Udma_DrvHandle  drvHandle = &appObj->drvObj;

#if defined (SOC_J721E) || defined (SOC_J7200)
    /* Use MCU NAVSS for MCU domain cores. Rest all cores uses Main NAVSS */
#if defined (BUILD_MCU1_0) || defined (BUILD_MCU1_1)
    instId = UDMA_INST_ID_MCU_0;
#else
    instId = UDMA_INST_ID_MAIN_0;
#endif
#elif defined (SOC_J721S2) || defined (SOC_J784S4)
    instId = UDMA_INST_ID_MCU_0;
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
	/* Init channel parameters */
    for(chIdx = 0U; chIdx < UDMA_TEST_APP_NUM_CH; chIdx++)
    {
        appChObj                    = &appObj->appChObj[chIdx];
        appChObj->chHandle          = &appChObj->chObj;
        appChObj->drvHandle         = drvHandle;
		appChObj->transferDoneSem   = NULL;
    }

    return (retVal);
}

int32_t sdl_UDMAdeinit(App_UdmaObj *appObj)
{
	int32_t         retVal;
    Udma_DrvHandle  drvHandle = &appObj->drvObj;

    retVal = Udma_deinit(drvHandle);
    if(UDMA_SOK != retVal)
    {
        App_print("[Error] UDMA deinit failed!!\n");
    }

    return (retVal);
}


int32_t sdl_mcrcAutoMode_main(void)
{
    int32_t         retVal = SDL_PASS;
    int32_t         retVal1 = SDL_PASS;
    App_UdmaObj    *appObj = &gUdmaAppObj;
	uint32_t testCase = 0U;
	UART_printf("\n\nMCRC AutoMode mode test: starting\n\n");
	
	/* UDMA driver init */
	sdl_UDMAinit(appObj);
	for(testCase = 0; testCase <(uint32_t)MAX_LENGTH_OF_MCRC_CONFIG_PARAMS; testCase++)
	{

        if(testParams[testCase].dataBitSize == SDL_MCRC_DATA_32_BIT)
        {
            UART_printf("\n MCRC auto CPU mode : starting 32 bit atomic write  for test =  %d",testCase);
            
        }
        else
        {
            UART_printf("\n MCRC auto CPU mode : starting 64 bit atomic write for test = %d",testCase);
        }
		/* Configure MCRC channel */
		retVal = SDL_MCRC_init(testParams[testCase].instance,
					testParams[testCase].mcrcChannelNumber,
					testParams[testCase].mcrcWatchdogPreload,
					testParams[testCase].mcrcBlockPreload);
		if(SDL_PASS != retVal)
		{
			UART_printf("[Error] mcrcAutoMode channel intialization failed!!\n");
		}
	
		retVal = sdl_mcrcAutoMode_create(appObj);
		if(SDL_PASS != retVal)
		{
			UART_printf("[Error] mcrcAutoMode App create failed!!\n");
		}
		
		if(SDL_PASS == retVal)
		{
			retVal = sdl_mcrcAutoModeTest(appObj, testParams, testCase);
			if(SDL_PASS != retVal)
			{
				UART_printf("[Error] mcrcAutoMode App MCRC test failed!!\n");
			}
		}
		
		retVal1 = sdl_mcrcAutoMode_delete(appObj);
		if(SDL_PASS != retVal1)
		{
			UART_printf("[Error] mcrcAutoMode App delete failed!!\n");
		}
	}
	sdl_UDMAdeinit(appObj);
	return (retVal + retVal1);
}


/* ========================================================================== */
/*                 Internal Function Definitions                              */
/* ========================================================================== */
static int32_t sdl_mcrcAutoModeTest(App_UdmaObj *appObj,
                                     SDL_MCRC_autoConfigParams_t *testParams, uint32_t testCase)
{

    int32_t         retVal = SDL_PASS;
    uint32_t        loopCnt = 0U;

    while(loopCnt < LOOP_COUNT)
    {
         /* Perform MCRC Operation */
        retVal = mcrcAutoModeTest(appObj,testParams, testCase);
        if(SDL_PASS != retVal)
        {
            break;
        }

        loopCnt++;
    }

    return (retVal);
}

static int32_t mcrcAutoModeTest(App_UdmaObj *appObj,
                        SDL_MCRC_autoConfigParams_t   *testParams,
                        uint32_t testCase)
{
    int32_t              		i,retVal = SDL_PASS;
    uint32_t             		*pTrResp, trRespStatus;
    uint64_t             		 pDesc = 0;
	uint8_t              		*tprdMem;
    SDL_MCRC_SignatureRegAddr_t psaSignRegAddr,refSignRegAddr;
    uint32_t                    patternCnt, sectCnt,chIdx, intrStatus = 0U;
	uint32_t	                   *src;
	SDL_MCRC_InstType           instance;
    SDL_MCRC_channel_t          mcrcChannel;
	SDL_MCRC_Signature_t        sectSignVal,mcrcSignVal;
	uint32_t                    IntrMask = 0xfU;
	App_UdmaChObj  *appChObj;
    Udma_ChHandle   chHandle;
	
    patternCnt  = testParams[testCase].dataSize / testParams[testCase].mcrcPatternSize;
    sectCnt     = testParams[testCase].mcrcSectorCount;
    instance = testParams[testCase].instance;
    mcrcChannel  = testParams[testCase].mcrcChannelNumber;

    /* Get MCRC PSA signature register address */
    SDL_MCRC_getPSASigRegAddr(instance, mcrcChannel, &psaSignRegAddr);
	SDL_MCRC_getMCRCSigRegAddr(instance, mcrcChannel, &refSignRegAddr);
    SDL_MCRC_channelReset(instance, mcrcChannel);
    retVal = SDL_MCRC_config(instance, mcrcChannel, patternCnt, sectCnt, testParams[testCase].mcrcMode);
	
	if (retVal == SDL_PASS)
    {
		retVal = SDL_MCRC_enableIntr(instance, mcrcChannel,IntrMask);
	}
	for (chIdx = 0; chIdx<2; chIdx++)
	{
		appChObj = &appObj->appChObj[chIdx];
        chHandle = appChObj->chHandle;
		if(chIdx ==0)
		{
			tprdMem = &gUdmaTprdMem[chIdx][0U];
			src = (uint32_t*)&gUdmaTestSrcBuf[chIdx][0U];
			src[0] = testParams[testCase].mcrcSignLow;
			src[1] = testParams[testCase].mcrcSignHigh;
			/* Writeback source buffer */
			SDL_Udma_appUtilsCacheWb(src, 2U);
			
			/* Update TR packet descriptor */
			sdl_mcrcAutoMode_udmaCrcTrpdInit(chHandle,tprdMem,
								(void *) src,
								(void *)(uintptr_t) refSignRegAddr.regL,
								8U,
								testParams[testCase].mcrcPatternSize);
		}
		else{
			tprdMem = &gUdmaTprdMem[chIdx][0U];
			src = (uint32_t*)&gUdmaTestSrcBuf[chIdx][0U];
			/* Update TR packet descriptor */
			for (i = 0; i < ((testParams[testCase].dataSize)/4U); i++)
			{
				src[i] = i;
			}
			/* Writeback source buffer */
			SDL_Udma_appUtilsCacheWb(src, testParams[testCase].dataSize);
			
			sdl_mcrcAutoMode_udmaPSATrpdInit(chHandle,tprdMem,
								(void *) src,
								(void *)(uintptr_t) psaSignRegAddr.regL,
								testParams[testCase].dataSize,
								testParams[testCase].mcrcPatternSize);	
		}


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
			SemaphoreP_pend(appChObj->transferDoneSem, SemaphoreP_WAIT_FOREVER);
	
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
			SDL_Udma_appUtilsCacheInv(&gUdmaTprdMem[chIdx][0U], UDMA_TEST_APP_TRPD_SIZE_ALIGN_ALIGN);
	
			/* check TR response status */
			pTrResp = (uint32_t *) (tprdMem + (sizeof(CSL_UdmapTR15) * 2U));
			trRespStatus = CSL_FEXT(*pTrResp, UDMAP_TR_RESPONSE_STATUS_TYPE);
			if(trRespStatus != CSL_UDMAP_TR_RESPONSE_STATUS_COMPLETE)
			{
				UART_printf("[Error] TR Response not completed!!\n");
				retVal = SDL_EFAIL;
			}
		}
	}
	
	if (SDL_PASS == retVal)
	{
		retVal = SDL_MCRC_getPSASectorSig(instance, mcrcChannel, &sectSignVal);
		retVal = SDL_MCRC_getMCRCSectorSig(instance, mcrcChannel, &mcrcSignVal);
	}
	
	if(SDL_PASS == retVal)
	{		
        /* Compare MCRC signature value against reference MCRC signature */
        if(testParams[testCase].testType == (uint32_t)POSITIVE_TEST)
        {
			retVal = SDL_MCRC_intrStatus(instance, mcrcChannel, &intrStatus);
            UART_printf("\nPOSITIVE TEST");

			UART_printf("\nInterupt status : %d",intrStatus);
			
			if (((sectSignVal.regH == mcrcSignVal.regH) && 
				(sectSignVal.regL == mcrcSignVal.regL) &&
				(intrStatus == 0U)))
			{
				/* Sector signature matches - Passed */
				UART_printf("\nSector signature matched.\n");
			}
			else{
				retVal = SDL_EFAIL;
			}
        }
        else
        {
            UART_printf("\nNEGATIVE TEST");
			while ((intrStatus == 0x0U) && (SDL_PASS == retVal))
			{
				/* Wait here  */
				retVal = SDL_MCRC_intrStatus(instance, mcrcChannel, &intrStatus);
			}
			UART_printf("\nInterupt status : %d",intrStatus);
			
			if((intrStatus != 0x0U))
			{
				UART_printf("\nSector signature does not match.");
				UART_printf("\nExpected MCRC signature value : 0x%x%xU",
							mcrcSignVal.regH,mcrcSignVal.regL);
				UART_printf("\nCalculated MCRC signature value : 0x%08x%08xU",
							sectSignVal.regH,sectSignVal.regL);
				UART_printf("\n Got the interrupt.");
			}
            else{
				retVal = SDL_EFAIL;
			}
        }
		retVal = SDL_MCRC_clearIntr(instance, mcrcChannel, SDL_MCRC_CHANNEL_IRQSTATUS_RAW_MAIN_ALL);
	}
    return (retVal);
}


static void sdl_mcrcAutoMode_udmaEventCb(Udma_EventHandle eventHandle,
                            uint32_t eventType,
                            void *appData)
{
    App_UdmaChObj *appChObj = (App_UdmaChObj *) appData;
	int32_t         retVal;
	UART_printf("\n udma EventCb occured \n");

    if(appChObj != NULL)
    {
        if(UDMA_EVENT_TYPE_DMA_COMPLETION == eventType)
        {
            SemaphoreP_post(appChObj->transferDoneSem);
        }
        else
        {
            retVal = SDL_EFAIL;
        }
    }
    else
    {
        retVal = SDL_EFAIL;
    }

    return;
}


static void sdl_mcrcAutoMode_udmaTdEventCb(Udma_EventHandle eventHandle,
                            uint32_t eventType,
                            void *appData)
{
    int32_t             retVal;
    CSL_UdmapTdResponse tdResp;
    App_UdmaChObj      *appChObj = (App_UdmaChObj *) appData;

	UART_printf("\n teardown callback occured\n");

    if(appChObj != NULL)
    {
        if(UDMA_EVENT_TYPE_TEARDOWN_PACKET == eventType)
        {
            /* Response received in Teardown completion queue */
            retVal = Udma_chDequeueTdResponse(appChObj->chHandle, &tdResp);
            if(UDMA_SOK != retVal)
            {
                /* [Error] No TD response after callback!! */
                retVal = SDL_EFAIL;
            }
        }
        else
        {
            retVal = SDL_EFAIL;
        }
    }
    else
    {
        retVal = SDL_EFAIL;
    }

    return;
}

static int32_t sdl_mcrcAutoMode_create(App_UdmaObj *appObj)
{
    int32_t             retVal = SDL_PASS;
    uint32_t            chType, chIdx;
    Udma_ChPrms         chPrms;
    Udma_ChTxPrms       txPrms;
    Udma_ChRxPrms       rxPrms;
    Udma_EventHandle    eventHandle;
    Udma_EventPrms      eventPrms;
    SemaphoreP_Params   semPrms;
	App_UdmaChObj      *appChObj;
    Udma_ChHandle       chHandle;
    Udma_DrvHandle      drvHandle = &appObj->drvObj;
	
	for(chIdx = 0U; chIdx < UDMA_TEST_APP_NUM_CH; chIdx++)
    {
		appChObj = &appObj->appChObj[chIdx];
        chHandle = appChObj->chHandle;
		SemaphoreP_Params_init(&semPrms);
		appChObj->transferDoneSem = SemaphoreP_create(0, &semPrms);
		if(NULL == appChObj->transferDoneSem)
		{
			UART_printf("[Error] Sem create failed!!\n");
			retVal = SDL_EFAIL;
		}
	
		if(SDL_PASS == retVal)
		{
			/* Init channel parameters */
			chType = UDMA_CH_TYPE_TR_BLK_COPY;
			UdmaChPrms_init(&chPrms, chType);
			chPrms.fqRingPrms.ringMem   = &gTxRingMem[chIdx][0U];
			chPrms.cqRingPrms.ringMem   = &gTxCompRingMem[chIdx][0U];
			chPrms.tdCqRingPrms.ringMem = &gTxTdCompRingMem[chIdx][0U];
			chPrms.fqRingPrms.ringMemSize   = UDMA_TEST_APP_RING_MEM_SIZE;
			chPrms.cqRingPrms.ringMemSize   = UDMA_TEST_APP_RING_MEM_SIZE;
			chPrms.tdCqRingPrms.ringMemSize = UDMA_TEST_APP_RING_MEM_SIZE;
			chPrms.fqRingPrms.elemCnt   = UDMA_TEST_APP_RING_ENTRIES;
			chPrms.cqRingPrms.elemCnt   = UDMA_TEST_APP_RING_ENTRIES;
			chPrms.tdCqRingPrms.elemCnt = UDMA_TEST_APP_RING_ENTRIES;
	
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
			eventHandle = &gUdmaCqEventObj[chIdx];
			UdmaEventPrms_init(&eventPrms);
			eventPrms.eventType         = UDMA_EVENT_TYPE_DMA_COMPLETION;
			eventPrms.eventMode         = UDMA_EVENT_MODE_SHARED;
			eventPrms.chHandle          = chHandle;
			eventPrms.masterEventHandle = NULL;
			eventPrms.appData = &appObj->appChObj[chIdx];
			eventPrms.eventCb           = &sdl_mcrcAutoMode_udmaEventCb;
			retVal = Udma_eventRegister(drvHandle, eventHandle, &eventPrms);
			if(SDL_PASS != retVal)
			{
				UART_printf("[Error] UDMA CQ event register failed!!\n");
			}
		}
	
		if(SDL_PASS == retVal)
		{
			/* Register teardown ring completion callback */
			eventHandle = &gUdmaTdCqEventObj[chIdx];
			UdmaEventPrms_init(&eventPrms);
			eventPrms.eventType         = UDMA_EVENT_TYPE_TEARDOWN_PACKET;
			eventPrms.eventMode         = UDMA_EVENT_MODE_SHARED;
			eventPrms.chHandle          = chHandle;
			eventPrms.masterEventHandle = &gUdmaCqEventObj[chIdx];
			eventPrms.appData = &appObj->appChObj[chIdx];
			eventPrms.eventCb           = &sdl_mcrcAutoMode_udmaTdEventCb;
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
	}
    return (retVal);
}

static int32_t sdl_mcrcAutoMode_delete(App_UdmaObj *appObj)
{
    int32_t         retVal, tempRetVal;
    uint64_t        pDesc;
    int32_t         chIdx;
    App_UdmaChObj  *appChObj;
    Udma_ChHandle   chHandle;

    for(chIdx = 0U; chIdx < UDMA_TEST_APP_NUM_CH; chIdx++)
    {
        appChObj = &appObj->appChObj[chIdx];
        chHandle = appChObj->chHandle;

        retVal = Udma_chDisable(chHandle, UDMA_DEFAULT_CH_DISABLE_TIMEOUT);
        if(UDMA_SOK != retVal)
        {
            App_print("[Error] UDMA channel disable failed!!\n");
        }

        /* Flush any pending request from the free queue */
        while(1)
        {
            tempRetVal = Udma_ringFlushRaw(
                             Udma_chGetFqRingHandle(chHandle), &pDesc);
            if(UDMA_ETIMEOUT == tempRetVal)
            {
                break;
            }
        }
    }

	for(chIdx = UDMA_TEST_APP_NUM_CH - 1U; chIdx >=0 ; chIdx--)
    {
        appChObj = &appObj->appChObj[chIdx];
        chHandle = appChObj->chHandle;
		Udma_EventHandle    eventHandle;
		
		/* Unregister master event at the end */
		eventHandle = &gUdmaTdCqEventObj[chIdx];
		retVal += Udma_eventUnRegister(eventHandle);
		eventHandle = &gUdmaCqEventObj[chIdx];
		retVal += Udma_eventUnRegister(eventHandle);
		if(SDL_PASS != retVal)
		{
			UART_printf("[Error] UDMA event unregister failed!!\n");
		}

        retVal += Udma_chClose(chHandle);
        if(UDMA_SOK != retVal)
        {
            App_print("[Error] UDMA channel close failed!!\n");
        }

		if(appChObj->transferDoneSem != NULL)
		{
			SemaphoreP_delete(appChObj->transferDoneSem);
			appChObj->transferDoneSem = NULL;
		}
	}
    return (retVal);
}

static void sdl_mcrcAutoMode_udmaCrcTrpdInit(Udma_ChHandle chHandle,
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
    pTr->dim2     = 0U;
    pTr->dim3     = 0U;
    pTr->addr     = (uint64_t) srcBuf;
    pTr->fmtflags = 0x00000000U;        /* Linear addressing, 1 byte per elem.
                                           Replace with CSL-FL API */
    pTr->dicnt0   = patternSize;
    pTr->dicnt1   = (length / pTr->dicnt0) / cCnt;
    pTr->dicnt2   = cCnt;
    pTr->dicnt3   = 1U;
    pTr->ddim1    = pTr->icnt0;
    pTr->ddim2    = 0U;
    pTr->ddim3    = 0U;
    pTr->daddr    = (uint64_t) destBuf;

    /* Clear TR response memory */
    *pTrResp = 0xFFFFFFFFU;

    /* Writeback cache */
    SDL_Udma_appUtilsCacheWb(pTrpdMem, UDMA_TEST_APP_TRPD_SIZE_ALIGN_ALIGN);
    return;
}

static void sdl_mcrcAutoMode_udmaPSATrpdInit(Udma_ChHandle chHandle,
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
    SDL_Udma_appUtilsCacheWb(pTrpdMem, UDMA_TEST_APP_TRPD_SIZE_ALIGN_ALIGN);
    return;
}

static void App_print(const char *str)
{
    UART_printf("%s", str);
    return;
}

