/*
*
* Copyright (C) 2024 Texas Instruments Incorporated
*
* All rights reserved not granted herein.
*
* Limited License.
*
* Texas Instruments Incorporated grants a world-wide, royalty-free, non-exclusive
* license under copyrights and patents it now or hereafter owns or controls to make,
* have made, use, import, offer to sell and sell ("Utilize") this software subject to the
* terms herein.  With respect to the foregoing patent license, such license is granted
* solely to the extent that any such patent is necessary to Utilize the software alone.
* The patent license shall not apply to any combinations which include this software,
* other than combinations with devices manufactured by or for TI ("TI Devices").
* No hardware patent is licensed hereunder.
*
* Redistributions must preserve existing copyright notices and reproduce this license
* (including the above copyright notice and the disclaimer and (if applicable) source
* code license limitations below) in the documentation and/or other materials provided
* with the distribution
*
* Redistribution and use in binary form, without modification, are permitted provided
* that the following conditions are met:
*
* *       No reverse engineering, decompilation, or disassembly of this software is
* permitted with respect to any software provided in binary form.
*
* *       any redistribution and use are licensed by TI for use only with TI Devices.
*
* *       Nothing shall obligate TI to provide you with source code for the software
* licensed and provided to you in object code.
*
* If software source code is provided to you, modification and redistribution of the
* source code are permitted provided that the following conditions are met:
*
* *       any redistribution and use of the source code, including any resulting derivative
* works, are licensed by TI for use only with TI Devices.
*
* *       any redistribution and use of any object code compiled from the source code
* and any resulting derivative works, are licensed by TI for use only with TI Devices.
*
* Neither the name of Texas Instruments Incorporated nor the names of its suppliers
*
* may be used to endorse or promote products derived from this software without
* specific prior written permission.
*
* DISCLAIMER.
*
* THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY EXPRESS
* OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL TI AND TI'S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
* OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
* OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/

/**
 *  \file Spi_Dma.c
 *
 *  \brief DMA functions implementation file.
 *
 */
/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#if defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
#include <ti/drv/udma/udma.h>
#endif
#include "Spi.h"
#include "Spi_Dbg.h"
#include "Spi_Priv.h"
#include <hw_include/soc.h>
#include <hw_include/lldr_mcspi.h>

#if (STD_ON == SPI_DMA_ENABLE)

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
typedef struct
{
    uint32 dmaEventRx;
    /**< SPI channel DMA Event Rx Number */
    uint32 dmaEventTx;
    /**< SPI channel DMA Event Tx Number */
}Spi_DmaEventObj;

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */
static void Spi_DmaTx(Spi_HwUnitObjType  *hwUnitObj,
                          Spi_JobObjType *jobObj,
                          Spi_ChannelObjType *chObj,
                          uint32 index);
static void Spi_DmaRx(Spi_HwUnitObjType  *hwUnitObj,
                          Spi_JobObjType *jobObj,
                          Spi_ChannelObjType *chObj,
                          uint32 index);
static Std_ReturnType Spi_DmaInitObj(Spi_DriverObjType *drvObj,
    const Spi_ConfigType *cfgPtr);
static void Spi_DmaHpdInit(Udma_ChHandle  chHandle,
                              uint8       *pHpdMem,
                              const void    *bufPtr,
                              uint32       length);
static inline uint32 Spi_DmaIsCacheCoherent(void);
static void Spi_DmaInitiateChunkXfer(
    Spi_DmaObjType      *dmaObj,
    Spi_HwUnitObjType  *hwUnitObj,
    Spi_JobObjType *jobObj,
    Spi_ChannelObjType *chObj,
    uint32              baseAddr);
static void Spi_DmaRxChunk(Spi_HwUnitObjType  *hwUnitObj,
                          Spi_JobObjType *jobObj,
                          Spi_ChannelObjType *chObj,
                          Spi_DmaObjType     *dmaObj);
static void Spi_DmaTxChunk(Spi_HwUnitObjType  *hwUnitObj,
                          Spi_JobObjType *jobObj,
                          Spi_ChannelObjType *chObj,
                          Spi_DmaObjType     *dmaObj);
static sint32 Spi_DmaRegisterEvent(Spi_DriverObjType *drvObj,
                                    Spi_DmaObjType *dmaObj,
                                    uint32 index);
static sint32 Spi_DmaInitTxCh(Spi_DriverObjType *drvObj,
                Spi_DmaObjType *dmaObj, uint32 index);
static sint32 Spi_DmaInitRxCh(Spi_DriverObjType *drvObj,
                Spi_DmaObjType *dmaObj, uint32 index);
/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
#define SPI_START_SEC_CONST_UNSPECIFIED
#include "Spi_MemMap.h"


static const Spi_DmaEventObj Spi_DmaEventNum[SPI_HW_UNIT_CNT][4U] =
{
    [0U] =
    {
        {
            CSL_PDMA_CH_MCU_MCSPI0_CH0_RX,      /* rxDmaEventNumber */
            CSL_PDMA_CH_MCU_MCSPI0_CH0_TX,      /* txDmaEventNumber */
        },
        {
            CSL_PDMA_CH_MCU_MCSPI0_CH1_RX,      /* rxDmaEventNumber */
            CSL_PDMA_CH_MCU_MCSPI0_CH1_TX,      /* txDmaEventNumber */
        },
        {
            CSL_PDMA_CH_MCU_MCSPI0_CH2_RX,      /* rxDmaEventNumber */
            CSL_PDMA_CH_MCU_MCSPI0_CH2_TX,      /* txDmaEventNumber */
        },
        {
            CSL_PDMA_CH_MCU_MCSPI0_CH3_RX,      /* rxDmaEventNumber */
            CSL_PDMA_CH_MCU_MCSPI0_CH3_TX,      /* txDmaEventNumber */
        },
    },
    [1U] =
    {
        {
            CSL_PDMA_CH_MCU_MCSPI1_CH0_RX,      /* rxDmaEventNumber */
            CSL_PDMA_CH_MCU_MCSPI1_CH0_TX,      /* txDmaEventNumber */
        },
        {
            CSL_PDMA_CH_MCU_MCSPI1_CH1_RX,      /* rxDmaEventNumber */
            CSL_PDMA_CH_MCU_MCSPI1_CH1_TX,      /* txDmaEventNumber */
        },
        {
            CSL_PDMA_CH_MCU_MCSPI1_CH2_RX,      /* rxDmaEventNumber */
            CSL_PDMA_CH_MCU_MCSPI1_CH2_TX,      /* txDmaEventNumber */
        },
        {
            CSL_PDMA_CH_MCU_MCSPI1_CH3_RX,      /* rxDmaEventNumber */
            CSL_PDMA_CH_MCU_MCSPI1_CH3_TX,      /* txDmaEventNumber */
        },
    },
    [2U] =
    {
        {
            CSL_PDMA_CH_MCU_MCSPI2_CH0_RX,      /* rxDmaEventNumber */
            CSL_PDMA_CH_MCU_MCSPI2_CH0_TX,      /* txDmaEventNumber */
        },
        {
            CSL_PDMA_CH_MCU_MCSPI2_CH1_RX,      /* rxDmaEventNumber */
            CSL_PDMA_CH_MCU_MCSPI2_CH1_TX,      /* txDmaEventNumber */
        },
        {
            CSL_PDMA_CH_MCU_MCSPI2_CH2_RX,      /* rxDmaEventNumber */
            CSL_PDMA_CH_MCU_MCSPI2_CH2_TX,      /* txDmaEventNumber */
        },
        {
            CSL_PDMA_CH_MCU_MCSPI2_CH3_RX,      /* rxDmaEventNumber */
            CSL_PDMA_CH_MCU_MCSPI2_CH3_TX,      /* txDmaEventNumber */
        },
    },
    [3U] =
    {
        {
            CSL_PDMA_CH_MAIN_MCSPI0_CH0_RX,      /* rxDmaEventNumber */
            CSL_PDMA_CH_MAIN_MCSPI0_CH0_TX,      /* txDmaEventNumber */
        },
        {
            CSL_PDMA_CH_MAIN_MCSPI0_CH1_RX,      /* rxDmaEventNumber */
            CSL_PDMA_CH_MAIN_MCSPI0_CH1_TX,      /* txDmaEventNumber */
        },
        {
            CSL_PDMA_CH_MAIN_MCSPI0_CH2_RX,      /* rxDmaEventNumber */
            CSL_PDMA_CH_MAIN_MCSPI0_CH2_TX,      /* txDmaEventNumber */
        },
        {
            CSL_PDMA_CH_MAIN_MCSPI0_CH3_RX,      /* rxDmaEventNumber */
            CSL_PDMA_CH_MAIN_MCSPI0_CH3_TX,      /* txDmaEventNumber */
        },
    },
    [4U] =
    {
        {
            CSL_PDMA_CH_MAIN_MCSPI1_CH0_RX,      /* rxDmaEventNumber */
            CSL_PDMA_CH_MAIN_MCSPI1_CH0_TX,      /* txDmaEventNumber */
        },
        {
            CSL_PDMA_CH_MAIN_MCSPI1_CH1_RX,      /* rxDmaEventNumber */
            CSL_PDMA_CH_MAIN_MCSPI1_CH1_TX,      /* txDmaEventNumber */
        },
        {
            CSL_PDMA_CH_MAIN_MCSPI1_CH2_RX,      /* rxDmaEventNumber */
            CSL_PDMA_CH_MAIN_MCSPI1_CH2_TX,      /* txDmaEventNumber */
        },
        {
            CSL_PDMA_CH_MAIN_MCSPI1_CH3_RX,      /* rxDmaEventNumber */
            CSL_PDMA_CH_MAIN_MCSPI1_CH3_TX,      /* txDmaEventNumber */
        },
    },
    [5U] =
    {
        {
            CSL_PDMA_CH_MAIN_MCSPI2_CH0_RX,      /* rxDmaEventNumber */
            CSL_PDMA_CH_MAIN_MCSPI2_CH0_TX,      /* txDmaEventNumber */
        },
        {
            CSL_PDMA_CH_MAIN_MCSPI2_CH1_RX,      /* rxDmaEventNumber */
            CSL_PDMA_CH_MAIN_MCSPI2_CH1_TX,      /* txDmaEventNumber */
        },
        {
            CSL_PDMA_CH_MAIN_MCSPI2_CH2_RX,      /* rxDmaEventNumber */
            CSL_PDMA_CH_MAIN_MCSPI2_CH2_TX,      /* txDmaEventNumber */
        },
        {
            CSL_PDMA_CH_MAIN_MCSPI2_CH3_RX,      /* rxDmaEventNumber */
            CSL_PDMA_CH_MAIN_MCSPI2_CH3_TX,      /* txDmaEventNumber */
        },
    },
    [6U] =
    {
        {
            CSL_PDMA_CH_MAIN_MCSPI3_CH0_RX,      /* rxDmaEventNumber */
            CSL_PDMA_CH_MAIN_MCSPI3_CH0_TX,      /* txDmaEventNumber */
        },
        {
            CSL_PDMA_CH_MAIN_MCSPI3_CH1_RX,      /* rxDmaEventNumber */
            CSL_PDMA_CH_MAIN_MCSPI3_CH1_TX,      /* txDmaEventNumber */
        },
        {
            CSL_PDMA_CH_MAIN_MCSPI3_CH2_RX,      /* rxDmaEventNumber */
            CSL_PDMA_CH_MAIN_MCSPI3_CH2_TX,      /* txDmaEventNumber */
        },
        {
            CSL_PDMA_CH_MAIN_MCSPI3_CH3_RX,      /* rxDmaEventNumber */
            CSL_PDMA_CH_MAIN_MCSPI3_CH3_TX,      /* txDmaEventNumber */
        },
    },
#if defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J784S4)
    [7U] =
    {
        {
            CSL_PDMA_CH_MAIN_MCSPI4_CH0_RX,      /* rxDmaEventNumber */
            CSL_PDMA_CH_MAIN_MCSPI4_CH0_TX,      /* txDmaEventNumber */
        },
        {
            CSL_PDMA_CH_MAIN_MCSPI4_CH1_RX,      /* rxDmaEventNumber */
            CSL_PDMA_CH_MAIN_MCSPI4_CH1_TX,      /* txDmaEventNumber */
        },
        {
            CSL_PDMA_CH_MAIN_MCSPI4_CH2_RX,      /* rxDmaEventNumber */
            CSL_PDMA_CH_MAIN_MCSPI4_CH2_TX,      /* txDmaEventNumber */
        },
        {
            CSL_PDMA_CH_MAIN_MCSPI4_CH3_RX,      /* rxDmaEventNumber */
            CSL_PDMA_CH_MAIN_MCSPI4_CH3_TX,      /* txDmaEventNumber */
        },
    },
    [8U] =
    {
        {
            CSL_PDMA_CH_MAIN_MCSPI5_CH0_RX,      /* rxDmaEventNumber */
            CSL_PDMA_CH_MAIN_MCSPI5_CH0_TX,      /* txDmaEventNumber */
        },
        {
            CSL_PDMA_CH_MAIN_MCSPI5_CH1_RX,      /* rxDmaEventNumber */
            CSL_PDMA_CH_MAIN_MCSPI5_CH1_TX,      /* txDmaEventNumber */
        },
        {
            CSL_PDMA_CH_MAIN_MCSPI5_CH2_RX,      /* rxDmaEventNumber */
            CSL_PDMA_CH_MAIN_MCSPI5_CH2_TX,      /* txDmaEventNumber */
        },
        {
            CSL_PDMA_CH_MAIN_MCSPI5_CH3_RX,      /* rxDmaEventNumber */
            CSL_PDMA_CH_MAIN_MCSPI5_CH3_TX,      /* txDmaEventNumber */
        },
    },
    [9U] =
    {
        {
            CSL_PDMA_CH_MAIN_MCSPI6_CH0_RX,      /* rxDmaEventNumber */
            CSL_PDMA_CH_MAIN_MCSPI6_CH0_TX,      /* txDmaEventNumber */
        },
        {
            CSL_PDMA_CH_MAIN_MCSPI6_CH1_RX,      /* rxDmaEventNumber */
            CSL_PDMA_CH_MAIN_MCSPI6_CH1_TX,      /* txDmaEventNumber */
        },
        {
            CSL_PDMA_CH_MAIN_MCSPI6_CH2_RX,      /* rxDmaEventNumber */
            CSL_PDMA_CH_MAIN_MCSPI6_CH2_TX,      /* txDmaEventNumber */
        },
        {
            CSL_PDMA_CH_MAIN_MCSPI6_CH3_RX,      /* rxDmaEventNumber */
            CSL_PDMA_CH_MAIN_MCSPI6_CH3_TX,      /* txDmaEventNumber */
        },
    },
    [10U] =
    {
        {
            CSL_PDMA_CH_MAIN_MCSPI7_CH0_RX,      /* rxDmaEventNumber */
            CSL_PDMA_CH_MAIN_MCSPI7_CH0_TX,      /* txDmaEventNumber */
        },
        {
            CSL_PDMA_CH_MAIN_MCSPI7_CH1_RX,      /* rxDmaEventNumber */
            CSL_PDMA_CH_MAIN_MCSPI7_CH1_TX,      /* txDmaEventNumber */
        },
        {
            CSL_PDMA_CH_MAIN_MCSPI7_CH2_RX,      /* rxDmaEventNumber */
            CSL_PDMA_CH_MAIN_MCSPI7_CH2_TX,      /* txDmaEventNumber */
        },
        {
            CSL_PDMA_CH_MAIN_MCSPI7_CH3_RX,      /* rxDmaEventNumber */
            CSL_PDMA_CH_MAIN_MCSPI7_CH3_TX,      /* txDmaEventNumber */
        },
    },
#endif
};

#define SPI_STOP_SEC_CONST_UNSPECIFIED
#include "Spi_MemMap.h"

/* RingAcc memories */
#define SPI_START_SEC_VAR_NO_INIT_8
#include <Spi_MemMap.h>

/** \brief Number of ring entries - we can prime this much mcspi instances */
#define UDMA_TEST_APP_RING_ENTRIES      (1U)
/** \brief Size (in bytes) of each ring entry (Size of pointer - 64-bit) */
#define UDMA_TEST_APP_RING_ENTRY_SIZE   (sizeof(uint64))
/** \brief Total ring memory for all DMA Enabled MCSPIInstances */
#define UDMA_TEST_APP_RING_MEM_SIZE     (UDMA_TEST_APP_RING_ENTRIES * \
                                         UDMA_TEST_APP_RING_ENTRY_SIZE)
/** \brief This ensures every channel memory is aligned */
#define UDMA_TEST_APP_RING_MEM_SIZE_ALIGN ((UDMA_TEST_APP_RING_MEM_SIZE + UDMA_CACHELINE_ALIGNMENT) & ~(UDMA_CACHELINE_ALIGNMENT - 1U))
/** \brief Number of ring entries - we can prime this much mcspi instances */
#define UDMA_TEST_APP_NUM_RING_ENTRIES      (UDMA_TEST_APP_RING_MEM_SIZE_ALIGN * SPI_MAX_HW_DMA_UNIT)

/** \brief UDMA host mode buffer descriptor memory size. */
#define UDMA_TEST_APP_DESC_SIZE         (sizeof(CSL_UdmapCppi5HMPD))
/** \brief This ensures every channel memory is aligned */
#define UDMA_TEST_APP_DESC_SIZE_ALIGN   ((UDMA_TEST_APP_DESC_SIZE + UDMA_CACHELINE_ALIGNMENT) & ~(UDMA_CACHELINE_ALIGNMENT - 1U))
/** \brief Number of desc - we can prime this much mcspi instances */
#define UDMA_TEST_APP_NUM_DESC_ENTRIES  (UDMA_TEST_APP_DESC_SIZE_ALIGN * SPI_MAX_HW_DMA_UNIT)

/* TX RingAcc memories */
VAR(uint8, SPI_VAR_NO_INIT)
    Spi_TxFqRingMem[UDMA_TEST_APP_NUM_RING_ENTRIES] __attribute__((aligned(UDMA_CACHELINE_ALIGNMENT)));
VAR(uint8, SPI_VAR_NO_INIT)
    Spi_TxCqRingMem[UDMA_TEST_APP_NUM_RING_ENTRIES] __attribute__((aligned(UDMA_CACHELINE_ALIGNMENT)));
VAR(uint8, SPI_VAR_NO_INIT)
    Spi_TxHpdMem[UDMA_TEST_APP_NUM_DESC_ENTRIES] __attribute__((aligned(UDMA_CACHELINE_ALIGNMENT)));

/* RX RingAcc memories */
VAR(uint8, SPI_VAR_NO_INIT)
    Spi_RxFqRingMem[UDMA_TEST_APP_NUM_RING_ENTRIES] __attribute__((aligned(UDMA_CACHELINE_ALIGNMENT)));
VAR(uint8, SPI_VAR_NO_INIT)
    Spi_RxCqRingMem[UDMA_TEST_APP_NUM_RING_ENTRIES] __attribute__((aligned(UDMA_CACHELINE_ALIGNMENT)));
VAR(uint8, SPI_VAR_NO_INIT)
    Spi_RxHpdMem[UDMA_TEST_APP_NUM_DESC_ENTRIES] __attribute__((aligned(UDMA_CACHELINE_ALIGNMENT)));

#define SPI_STOP_SEC_VAR_NO_INIT_8
#include <Spi_MemMap.h>

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */
#define SPI_START_SEC_CODE
#include "Spi_MemMap.h"

Std_ReturnType Spi_DmaConfig(Spi_DriverObjType *drvObj,
    const Spi_ConfigType *cfgPtr)
{
    sint32             retVal = UDMA_SOK;
    Std_ReturnType     status = (Std_ReturnType)E_OK;
    Udma_InitPrms      initPrms;
    uint32             instId, index;
    uint32             dmaEnabledHwCnt = (uint32)SPI_MAX_HW_DMA_UNIT;

    /* Initialize DMA Driver Object */
    status = Spi_DmaInitObj(drvObj, cfgPtr);
    if(E_OK == status)
    {
        /* UDMA driver init */
        instId = cfgPtr->udmaInstId;
        UdmaInitPrms_init(instId, &initPrms);
        retVal = Udma_init(&drvObj->udmaDrvObj, &initPrms);
        /* SPI_CoverageGap_17:Dynamic coverage for failed condn with next condn true cannot be
         * achievable as same condn is checked for all remaining statement
         */
        if(UDMA_SOK == retVal)
        {
            /* Initialize UDMA driver jandle */
            drvObj->drvHandle = &drvObj->udmaDrvObj;
            for (index = 0U; index < dmaEnabledHwCnt; index++)
            {
                if(UDMA_SOK == retVal)
                {
                    /* Init Tx Channel Params*/
                    retVal = Spi_DmaInitTxCh(drvObj, &drvObj->dmaObj[index], index);
                }
                if(UDMA_SOK == retVal)
                {
                    /* Init Rx Channel Params*/
                    retVal = Spi_DmaInitRxCh(drvObj, &drvObj->dmaObj[index], index);
                }
                if(UDMA_SOK == retVal)
                {
                    /* Init Rx Channel Params*/
                    retVal = Spi_DmaRegisterEvent(drvObj, &drvObj->dmaObj[index], index);
                }
            }
        }
        if (UDMA_SOK == retVal)
        {
            status = (Std_ReturnType)E_OK;
        }
        else
        {
            status = (Std_ReturnType)E_NOT_OK;
        }
    }
    else
    {
        status = (Std_ReturnType)E_NOT_OK;
    }

    return status;
}

void Spi_DmaTransferStart(Spi_HwUnitObjType  *hwUnitObj,
                          Spi_JobObjType *jobObj,
                          Spi_ChannelObjType *chObj)
{
    uint32                             baseAddr, csNum, index, reminder;
    Spi_McspiExternalDeviceConfigType  *extDevCfg;
    Spi_DmaObjType                     *dmaObj;
    uint32 dmaEnabledHwCnt             = (uint32)SPI_MAX_HW_DMA_UNIT;

    baseAddr  = hwUnitObj->baseAddr;
    csNum     = (uint32)(jobObj->jobCfg_PC.csPin);
    extDevCfg = &jobObj->extDevCfg->mcspi;

    /* Find corresponding Hw Unit idx */
    for (index = 0U; index < dmaEnabledHwCnt; index++)
    {
        dmaObj =  &Spi_DrvObj.dmaObj[index];
        if ((dmaObj->hwUnitId == jobObj->jobCfg.hwUnitId) &&
             (dmaObj->csPin == csNum))
        {
             break;
        }
    }
    /* Clear all previous interrupt status */
    Spi_mcspiClearAllIrqStatus(baseAddr);

    /* Enable TX and RX FIFO */
    CSL_REG32_FINS(
        baseAddr + MCSPI_CHCONF(csNum),
        MCSPI_CH0CONF_FFEW,
        MCSPI_CH0CONF_FFEW_FFENABLED);
    if (extDevCfg->txRxMode == SPI_TX_RX_MODE_BOTH)
    {
        CSL_REG32_FINS(
            baseAddr + MCSPI_CHCONF(csNum),
            MCSPI_CH0CONF_FFER,
            MCSPI_CH0CONF_FFER_FFENABLED);
    }

    /* Set force mode */
    if (SPI_CONTINUOUS == extDevCfg->csMode)
    {
        CSL_REG32_FINS(
            baseAddr + MCSPI_CHCONF(csNum),
            MCSPI_CH0CONF_FORCE,
            MCSPI_CH0CONF_FORCE_ASSERT);
    }


    /* Enable McSPI DMA for transaction
     * MCSPI_CHCONF(csNum)CONF -> DMAR bit for receive DMA
     * MCSPI_CHCONF(csNum)CONF -> DMAW bit for transfer DMA
     */
    CSL_REG32_FINS(
        baseAddr + MCSPI_CHCONF(csNum),
        MCSPI_CH0CONF_DMAR,
        MCSPI_CH0CONF_DMAR_ENABLED);
    CSL_REG32_FINS(
        baseAddr + MCSPI_CHCONF(csNum),
        MCSPI_CH0CONF_DMAW,
        MCSPI_CH0CONF_DMAW_ENABLED);

    /* Enable channel */
    CSL_REG32_FINS(
        baseAddr + MCSPI_CHCTRL(csNum),
        MCSPI_CH0CTRL_EN,
        MCSPI_CH0CTRL_EN_ACT);

    reminder = (((uint32) chObj->numWordsTxRx) & (chObj->effTxFifoDepth - 1U));
    dmaObj->numOfWordsNotEffFifoDepth = reminder;
    if (extDevCfg->txRxMode == SPI_TX_RX_MODE_BOTH)
    {
        if (chObj->numWordsTxRx >= chObj->effTxFifoDepth)
        {
            /* Start transferring multiple of FIFO trigger level */
            (void)Spi_DmaRx(hwUnitObj, jobObj, chObj, index);
            (void)Spi_DmaTx(hwUnitObj, jobObj, chObj, index);
        }
        /* SPI_CoverageGap_18:Dynamic coverage of below code cannot be achievable as it is
         * dependent on the FIFO unaligned data mentioned with tha jira bug id
         * MCAL-5185 as it will be fixed in 1.06 Release
         */
        else
        {
            /* Start transferring if not a multiple of FIFO trigger level */
            (void)Spi_DmaTx(hwUnitObj, jobObj, chObj, index);
            (void)Spi_DmaRx(hwUnitObj, jobObj, chObj, index);
            dmaObj->numOfWordsNotEffFifoDepth = 0U;
        }
    }
    else if (extDevCfg->txRxMode == SPI_TX_RX_MODE_TX_ONLY)
    {
        /* TX_ONLY Mode */
        (void)Spi_DmaTx(hwUnitObj, jobObj, chObj, index);
    }
    else
    {
        /* Do Nothing */
    }

    return;
}

void Spi_DmaStop(Spi_HwUnitObjType  *hwUnitObj,
                          Spi_JobObjType *jobObj)
{
    uint32                              baseAddr, csNum;
    uint32                              intrMask, csIntrMask;
    Spi_McspiExternalDeviceConfigType   *extDevCfg;

    baseAddr  = hwUnitObj->baseAddr;
    extDevCfg = &jobObj->extDevCfg->mcspi;
    csNum     = (uint32)(jobObj->jobCfg_PC.csPin);

    /* Deassert CS */
    /*SPI_CoverageGap_19:Dynamic coverage of below code for false condition is cannot be achieved if we Make
     *csmode as SPI_SINGLE the driver is hung need to be covered in Release 1.06
     */
    if (SPI_CONTINUOUS == extDevCfg->csMode)
    {
        CSL_REG32_FINS(
            baseAddr + MCSPI_CHCONF(csNum),
            MCSPI_CH0CONF_FORCE,
            MCSPI_CH0CONF_FORCE_DEASSERT);
    }

    /* Disable channel */
    CSL_REG32_FINS(
        baseAddr + MCSPI_CHCTRL(csNum),
        MCSPI_CH0CTRL_EN,
        MCSPI_CH0CTRL_EN_NACT);

    /* Disable EOW, TX and RX interrupts */
    intrMask   = CSL_REG32_RD(baseAddr + MCSPI_IRQENABLE);
    intrMask  &= ~(MCSPI_IRQSTATUS_EOW_MASK);
    csIntrMask = Spi_mcspiGetCsIntrMask(csNum, extDevCfg->txRxMode);
    intrMask  &= ~(csIntrMask);
    CSL_REG32_WR(baseAddr + MCSPI_IRQENABLE, intrMask);

    /* Disable TX and RX FIFO - This is required so that next CS can
     * use the FIFO - done as per McSPI spec */
    CSL_REG32_FINS(
        baseAddr + MCSPI_CHCONF(csNum),
        MCSPI_CH0CONF_FFEW,
        MCSPI_CH0CONF_FFEW_FFDISABLED);
    CSL_REG32_FINS(
        baseAddr + MCSPI_CHCONF(csNum),
        MCSPI_CH0CONF_FFER,
        MCSPI_CH0CONF_FFER_FFDISABLED);

    /* Disable McSPI DMA for transaction
     * MCSPI_CH(csNum)CONF -> DMAR bit for receive DMA
     * MCSPI_CH(csNum)CONF -> DMAW bit for transfer DMA
     */
    CSL_REG32_FINS(
        baseAddr + MCSPI_CHCONF(csNum),
        MCSPI_CH0CONF_DMAR,
        MCSPI_CH0CONF_DMAR_DISABLED);
    CSL_REG32_FINS(
        baseAddr + MCSPI_CHCONF(csNum),
        MCSPI_CH0CONF_DMAW,
        MCSPI_CH0CONF_DMAW_DISABLED);

    /* Clear all previous interrupt status */
    Spi_mcspiClearAllIrqStatus(baseAddr);

    return;
}

/*SPI_CoverageGap_20:Dynamic coverage of entire function (Spi_DmaTransferReStart) is not covered,
 *because of bug which can be covered in Release 1,06
 */
void Spi_DmaTransferReStart(Spi_HwUnitObjType *hwUnitObj,
                       Spi_JobObjType    *jobObj,
                      Spi_ChannelObjType *chObj)
{
    uint32                              baseAddr, csNum, index, reminder;
    Spi_McspiExternalDeviceConfigType   *extDevCfg;
    Spi_DmaObjType                      *dmaObj;
    uint32 dmaEnabledHwCnt              = (uint32)SPI_MAX_HW_DMA_UNIT;

    baseAddr  = hwUnitObj->baseAddr;
    extDevCfg = &jobObj->extDevCfg->mcspi;
    csNum    = (uint32)(jobObj->jobCfg_PC.csPin);

    /* Find corresponding Hw Unit idx */
    for (index = 0U; index < dmaEnabledHwCnt; index++)
    {
        dmaObj =  &Spi_DrvObj.dmaObj[index];
        if ((dmaObj->hwUnitId == jobObj->jobCfg.hwUnitId) &&
             (dmaObj->csPin == csNum))
        {
             break;
        }
    }

    /* Disable channel and re-enable so that new word count takes effect */
    CSL_REG32_FINS(
        baseAddr + MCSPI_CHCTRL(csNum),
        MCSPI_CH0CTRL_EN,
        MCSPI_CH0CTRL_EN_NACT);
    CSL_REG32_FINS(
        baseAddr + MCSPI_CHCTRL(csNum),
        MCSPI_CH0CTRL_EN,
        MCSPI_CH0CTRL_EN_ACT);

    reminder = (((uint32) chObj->numWordsTxRx) & (chObj->effTxFifoDepth - 1U));
    dmaObj->numOfWordsNotEffFifoDepth = reminder;
    if (extDevCfg->txRxMode == SPI_TX_RX_MODE_BOTH)
    {
        if (chObj->numWordsTxRx >= chObj->effTxFifoDepth)
        {
            /* Start transferring multiple of FIFO trigger level */
            (void)Spi_DmaRx(hwUnitObj, jobObj, chObj, index);
            (void)Spi_DmaTx(hwUnitObj, jobObj, chObj, index);
        }
        else
        {
            /* Start transferring if not a multiple of FIFO trigger level */
            (void)Spi_DmaTx(hwUnitObj, jobObj, chObj, index);
            (void)Spi_DmaRx(hwUnitObj, jobObj, chObj, index);
            dmaObj->numOfWordsNotEffFifoDepth = 0U;
        }
    }
    else if (extDevCfg->txRxMode == SPI_TX_RX_MODE_TX_ONLY)
    {
        /* TX_ONLY Mode */
        (void)Spi_DmaTx(hwUnitObj, jobObj, chObj, index);
    }
    else
    {
        /* Do Nothing */
    }

    return;
}
/*SPI_CoverageGap_21:Dynamic coverage for Spi_DmaFreeChannel cannot be achievable as maxHwunit is
 *set to 0 in Spi_resetDrvObj so this is never be called
 */
void Spi_DmaFreeChannel(Spi_DmaObjType *dmaObj)
{
    /* Close the TX channel */
    (void)Udma_eventUnRegister((Udma_EventHandle)(&dmaObj->udmaTxCqEventObj));
    (void)Udma_chClose((Udma_ChHandle)(&dmaObj->udmaTxChObj));

    /* Close the RX channel */
    (void)Udma_eventUnRegister((Udma_EventHandle)(&dmaObj->udmaRxCqEventObj));
    (void)Udma_chClose((Udma_ChHandle)(&dmaObj->udmaRxChObj));
}

static Std_ReturnType Spi_DmaInitObj(Spi_DriverObjType *drvObj,
    const Spi_ConfigType *cfgPtr)
{
    uint32                   index, dmaEnableHwIndx;
    Std_ReturnType           retVal = (Std_ReturnType)E_OK;
    Spi_HwUnitObjType        *hwUnitObj;
    Spi_DmaObjType           *dmaObj;
    Spi_JobObjType           *jobObj;
    uint32 dmaEnabledHwCnt = (uint32)SPI_MAX_HW_DMA_UNIT;

    if (cfgPtr->maxHwUnit >= dmaEnabledHwCnt)
    {
        /* Init HW unit id and Chip Select Pin for DMA enabled instances */
        for (index = 0U; index < cfgPtr->maxHwUnit; index++)
        {
            hwUnitObj = &drvObj->hwUnitObj[index];
            if (hwUnitObj->enabledmaMode == TRUE)
            {
                dmaEnableHwIndx  = drvObj->dmaEnableHwIndx;
                dmaObj           = &drvObj->dmaObj[dmaEnableHwIndx];
                dmaObj->hwUnitId = hwUnitObj->hwUnitCfg.hwUnitId;
                dmaObj->dmaTxChIntrNum = hwUnitObj->hwUnitCfg.dmaTxChIntrNum;
                dmaObj->dmaRxChIntrNum = hwUnitObj->hwUnitCfg.dmaRxChIntrNum;
                dmaObj->dmaEnableHwInstId = drvObj->dmaEnableHwIndx;
                drvObj->dmaEnableHwIndx++;
            }
        }
        /* Reset DMA instance index to 0 */
        dmaEnableHwIndx = 0U;
        for (index = 0U; index < cfgPtr->maxJobs; index++)
        {
            jobObj = &drvObj->jobObj[index];
            jobObj->hwUnitObj = Spi_getHwUnitObj(jobObj->jobCfg.hwUnitId);
            if (jobObj->hwUnitObj->enabledmaMode == TRUE)
            {
                dmaObj           = &drvObj->dmaObj[dmaEnableHwIndx];
                if (dmaObj->hwUnitId == jobObj->hwUnitObj->hwUnitCfg.hwUnitId)
                {
                    dmaObj->csPin = jobObj->jobCfg_PC.csPin;
                    dmaEnableHwIndx++;
                }
            }
        }
    }
    else
    {
        retVal = (Std_ReturnType)E_NOT_OK;
    }

    return retVal;
}

static sint32 Spi_DmaRegisterEvent(Spi_DriverObjType *drvObj,
                                    Spi_DmaObjType *dmaObj,
                                    uint32 index)
{
    sint32                              retVal = UDMA_SOK;
    Spi_McspiExternalDeviceConfigType   *extDevCfg;
    Spi_JobObjType                      *jobObj;
    Udma_EventPrms                      eventPrms;
    Udma_EventHandle                    eventHandle;

     /* Register TX event callback */
    dmaObj     = &drvObj->dmaObj[index];
    eventHandle = (Udma_EventHandle)(&dmaObj->udmaTxCqEventObj);

    UdmaEventPrms_init(&eventPrms);
    eventPrms.eventType          = UDMA_EVENT_TYPE_DMA_COMPLETION;
    eventPrms.eventMode          = UDMA_EVENT_MODE_SHARED;
    eventPrms.chHandle           = (Udma_ChHandle)(&dmaObj->udmaTxChObj);
    eventPrms.masterEventHandle  = NULL;
    eventPrms.osalRegisterDisable  = TRUE;
    eventPrms.preferredCoreIntrNum = dmaObj->dmaTxChIntrNum;
    eventPrms.eventCb            = NULL;
    eventPrms.appData            = NULL;
    retVal = Udma_eventRegister(drvObj->drvHandle, eventHandle, &eventPrms);
    /* Upon a successful event registration, the UDMA driver returns the
     * granted interrupt number, the clear and status registers as well as
     * the bit mask */

    if (UDMA_SOK == retVal)
    {
        if (eventPrms.coreIntrNum == eventPrms.preferredCoreIntrNum)
        {
            retVal = UDMA_SOK;
        }
        /* SPI_CoverageGap_22:Dynamic coverage for below code cannot be possible the system hungs
         *for wrong interupt value for both Tx and Rx
         */
        else
        {
            /* Failure to allocate the exact interrupt being requested
             * is treated as an allocation error */
            Udma_eventUnRegister((Udma_EventHandle)(&dmaObj->udmaTxCqEventObj));
            retVal = UDMA_EFAIL;
        }
    }

    /* Do not register RX event if transfer mode is TX only. */
    if(UDMA_SOK == retVal)
    {
        jobObj = &drvObj->jobObj[index];
        extDevCfg = &jobObj->extDevCfg->mcspi;
        if (extDevCfg->txRxMode == SPI_TX_RX_MODE_BOTH)
        {
            /* Register RX event callback */
            dmaObj     = &drvObj->dmaObj[index];
            eventHandle = (Udma_EventHandle)(&dmaObj->udmaRxCqEventObj);
            UdmaEventPrms_init(&eventPrms);
            eventPrms.eventType         = UDMA_EVENT_TYPE_DMA_COMPLETION;
            eventPrms.eventMode         = UDMA_EVENT_MODE_SHARED;
            eventPrms.chHandle          = (Udma_ChHandle)(&dmaObj->udmaRxChObj);
            eventPrms.masterEventHandle  = NULL;
            eventPrms.osalRegisterDisable  = TRUE;
            eventPrms.preferredCoreIntrNum = dmaObj->dmaRxChIntrNum;
            eventPrms.eventCb            = NULL;
            eventPrms.appData            = NULL;
            retVal = Udma_eventRegister(drvObj->drvHandle, eventHandle, &eventPrms);
            if (UDMA_SOK == retVal)
            {
                if (eventPrms.coreIntrNum == eventPrms.preferredCoreIntrNum)
                {
                    retVal = UDMA_SOK;
                }
                /* SPI_CoverageGap_23:Dynamic coverage for below code cannot be possible the
                 * system hungs for wrong interupt value for both Tx and Rx
                 */
                else
                {
                    /* Failure to allocate the exact interrupt being requested
                     * is treated as an allocation error */
                    Udma_eventUnRegister((Udma_EventHandle)(&dmaObj->udmaRxCqEventObj));
                    retVal = UDMA_EFAIL;
                }
            }
        }
    }

    return retVal;
}

static sint32 Spi_DmaInitTxCh(Spi_DriverObjType *drvObj,
                Spi_DmaObjType *dmaObj, uint32 index)
{
    sint32                  retVal;
    uint32                  csNum;
    Udma_DrvHandle          drvHandle;
    uint32                  chType;
    Udma_ChPrms             chPrms;
    Udma_ChHandle           txChHandle;
    Spi_HWUnitType          hwUnitId;
    Udma_ChTxPrms           txPrms;

    /* Get the pointer to the object and hwAttrs */
    drvHandle  = (Udma_DrvHandle)(drvObj->drvHandle);
    txChHandle = (Udma_ChHandle)(&dmaObj->udmaTxChObj);
    csNum      = dmaObj->csPin;
    hwUnitId   = dmaObj->hwUnitId;

    /* Init TX channel parameters */
    chType = UDMA_CH_TYPE_PDMA_TX;
    UdmaChPrms_init(&chPrms, chType);
    chPrms.peerChNum            =  Spi_DmaEventNum[hwUnitId][csNum].dmaEventTx;
    chPrms.fqRingPrms.ringMem   = &Spi_TxFqRingMem[index * UDMA_TEST_APP_RING_MEM_SIZE_ALIGN];
    chPrms.cqRingPrms.ringMem   = &Spi_TxCqRingMem[index * UDMA_TEST_APP_RING_MEM_SIZE_ALIGN];
    chPrms.fqRingPrms.elemCnt   = 1U;
    chPrms.cqRingPrms.elemCnt   = 1U;

    /* Open TX channel for transmit */
    retVal = Udma_chOpen(drvHandle, txChHandle, chType, &chPrms);
    if(UDMA_SOK == retVal)
    {
        UdmaChTxPrms_init(&txPrms, chType);
        retVal = Udma_chConfigTx(txChHandle, &txPrms);
    }

    return retVal;
}

static sint32 Spi_DmaInitRxCh(Spi_DriverObjType *drvObj,
                Spi_DmaObjType *dmaObj, uint32 index)
{
    sint32                  retVal;
    uint32                  csNum;
    Udma_DrvHandle          drvHandle;
    uint32                  chType;
    Udma_ChPrms             chPrms;
    Udma_ChHandle           rxChHandle;
    Spi_HWUnitType          hwUnitId;
    Udma_ChRxPrms           rxPrms;

    /* Get the pointer to the object and hwAttrs */
    drvHandle  = (Udma_DrvHandle)(drvObj->drvHandle);
    csNum      = dmaObj->csPin;
    hwUnitId   = dmaObj->hwUnitId;
    rxChHandle = (Udma_ChHandle)(&dmaObj->udmaRxChObj);

    /* Init RX channel parameters */
    chType = UDMA_CH_TYPE_PDMA_RX;
    UdmaChPrms_init(&chPrms, chType);
    chPrms.peerChNum            =  Spi_DmaEventNum[hwUnitId][csNum].dmaEventRx;
    chPrms.fqRingPrms.ringMem   = &Spi_RxFqRingMem[index * UDMA_TEST_APP_RING_MEM_SIZE_ALIGN];
    chPrms.cqRingPrms.ringMem   = &Spi_RxCqRingMem[index * UDMA_TEST_APP_RING_MEM_SIZE_ALIGN];
    chPrms.fqRingPrms.elemCnt   = 1U;
    chPrms.cqRingPrms.elemCnt   = 1U;

    /* Open RX channel for transmit */
    retVal = Udma_chOpen(drvHandle, rxChHandle, chType, &chPrms);
    if(UDMA_SOK == retVal)
    {
        UdmaChRxPrms_init(&rxPrms, chType);
        retVal = Udma_chConfigRx(rxChHandle, &rxPrms);
    }

    return retVal;
}

static void Spi_DmaTx(Spi_HwUnitObjType  *hwUnitObj,
                          Spi_JobObjType *jobObj,
                          Spi_ChannelObjType *chObj, uint32 index)
{
    sint32              retVal;
    uint32              length = 0U, shiftVal;
    Udma_ChPdmaPrms     pdmaPrms;
    uint8               *pHpdMem;
    Udma_ChHandle       txChHandle;
    Spi_DriverObjType   *drvObj = &Spi_DrvObj;
    Spi_DmaObjType      *dmaObj;


    UdmaChPdmaPrms_init(&pdmaPrms);
    /* bufWidth is 1 or 2 (8/16 bit),
        length is the total number of bytes to be transferred */
    if (chObj->bufWidth <= 2U)
    {
        pdmaPrms.elemSize = chObj->bufWidth - 1U;
        shiftVal = chObj->bufWidth - 1U;
        length = (chObj->numWordsTxRx << shiftVal);
    }
    else
    {
        /* bufWidth is 4(32 bit) */
        pdmaPrms.elemSize = UDMA_PDMA_ES_32BITS;
        shiftVal = 2U;
        length = (chObj->numWordsTxRx << shiftVal);
    }

    /* Number of words transferred in each transfer */
    /* SPI_CoverageGap_24:Dynamic coverage can be achievable for fifo unaligned data mentioned
     * in bug MCAL-5185,can be fixed in the 1.06 Release
     */
    if (length > jobObj->txFifoTrigLvl)
    {
        pdmaPrms.elemCnt = (uint32)(chObj->effTxFifoDepth);
    }
    else
    {
        pdmaPrms.elemCnt = (uint32)(length >> shiftVal);
    }

    /* Don't care for write */
    pdmaPrms.fifoCnt = 0U;

    dmaObj = &drvObj->dmaObj[index];
    txChHandle = (Udma_ChHandle)(&dmaObj->udmaTxChObj);

    retVal = Udma_chConfigPdma(txChHandle, &pdmaPrms);
    if(UDMA_SOK == retVal)
    {
        retVal = Udma_chEnable(txChHandle);
    }

    if(UDMA_SOK == retVal)
    {
        /* Update Source Buffer Pointer */
#if ((SPI_CHANNELBUFFERS == SPI_EB) || (SPI_CHANNELBUFFERS == SPI_IB_EB))
        if (SPI_EB == chObj->chCfg.channelBufType)
        {
            chObj->curTxBufPtr = chObj->txBufPtr;
        }
#endif
#if ((SPI_CHANNELBUFFERS == SPI_IB) || (SPI_CHANNELBUFFERS == SPI_IB_EB))
        if (SPI_IB == chObj->chCfg.channelBufType)
        {
            chObj->curTxBufPtr = (const uint8 *) &chObj->txIb[0U];
        }
#endif
        /* Update host packet descriptor */
        pHpdMem = (uint8 *) &Spi_TxHpdMem[index * UDMA_TEST_APP_DESC_SIZE_ALIGN];
        Spi_DmaHpdInit(txChHandle, pHpdMem, chObj->curTxBufPtr, length);

        /* Submit HPD to channel */
        retVal = Udma_ringQueueRaw(Udma_chGetFqRingHandle(txChHandle), (uint64) pHpdMem);
    }

    return;

}

static void Spi_DmaRx(Spi_HwUnitObjType  *hwUnitObj,
                          Spi_JobObjType *jobObj,
                          Spi_ChannelObjType *chObj, uint32 index)
{
    sint32              retVal;
    uint32              length = 0U, shiftVal;
    Udma_ChPdmaPrms     pdmaPrms;
    uint8               *pHpdMem;
    Udma_ChHandle       rxChHandle;
    Spi_DriverObjType   *drvObj = &Spi_DrvObj;
    Spi_DmaObjType      *dmaObj;


    UdmaChPdmaPrms_init(&pdmaPrms);
    /* bufWidth is 1 or 2 (8/16 bit),
    length is the total number of bytes to be transferred */
    /* SPI_CoverageGap_25:Dynamic coverage  for below code cannot be achievable for fifo unaligned
     * data mentioned in bug MCAL-5185,can be fixed in the 1.06 Release
     */
    if (chObj->bufWidth <= 2U)
    {
        pdmaPrms.elemSize = chObj->bufWidth - 1U;
        shiftVal = chObj->bufWidth - 1U;
        length = (chObj->numWordsTxRx << shiftVal);
    }
    else
    {
        /* bufWidth is 4(32 bit) */
        pdmaPrms.elemSize = UDMA_PDMA_ES_32BITS;
        shiftVal = 2U;
        length = (chObj->numWordsTxRx << shiftVal);
    }

    /* Number of words received in each transfer */
    if (length > jobObj->rxFifoTrigLvl)
    {
        pdmaPrms.elemCnt = (uint32)(jobObj->rxFifoTrigLvl >> shiftVal);
        pdmaPrms.fifoCnt = length / jobObj->rxFifoTrigLvl;
    }
    else
    {
        pdmaPrms.elemCnt = (uint32)(length >> shiftVal);
        pdmaPrms.fifoCnt = 1U;
    }

    dmaObj = &drvObj->dmaObj[index];
    rxChHandle = (Udma_ChHandle)(&dmaObj->udmaRxChObj);
    retVal = Udma_chConfigPdma(rxChHandle, &pdmaPrms);
    if(UDMA_SOK == retVal)
    {
        retVal = Udma_chEnable(rxChHandle);
    }

    if(UDMA_SOK == retVal)
    {
         /* Update Destination Buffer Pointer */
#if ((SPI_CHANNELBUFFERS == SPI_EB) || (SPI_CHANNELBUFFERS == SPI_IB_EB))
        if (SPI_EB == chObj->chCfg.channelBufType)
        {
            chObj->curRxBufPtr = chObj->rxBufPtr;
        }
#endif
#if ((SPI_CHANNELBUFFERS == SPI_IB) || (SPI_CHANNELBUFFERS == SPI_IB_EB))
        if (SPI_IB == chObj->chCfg.channelBufType)
        {
            chObj->curRxBufPtr = (uint8 *) &chObj->rxIb[0U];
        }
#endif
        /* Update host packet descriptor */
        pHpdMem = (uint8 *) &Spi_RxHpdMem[index * UDMA_TEST_APP_DESC_SIZE_ALIGN];
        Spi_DmaHpdInit(rxChHandle, pHpdMem, chObj->curRxBufPtr, length);

        /* Submit HPD to channel */
        retVal = Udma_ringQueueRaw(Udma_chGetFqRingHandle(rxChHandle), (uint64) pHpdMem);
    }

    return;
}

static void Spi_DmaInitiateChunkXfer(
    Spi_DmaObjType      *dmaObj,
    Spi_HwUnitObjType   *hwUnitObj,
    Spi_JobObjType      *jobObj,
    Spi_ChannelObjType  *chObj,
    uint32              baseAddr)
{
    uint32                            reminder, regVal;
    uint32                            csNum;
    Spi_McspiExternalDeviceConfigType *extDevCfg;

    csNum = (uint32)(dmaObj->csPin);

    /* Disable channel so that new settings takes effect */
    CSL_REG32_FINS(
        baseAddr + MCSPI_CHCTRL(csNum),
        MCSPI_CH0CTRL_EN,
        MCSPI_CH0CTRL_EN_NACT);

    /* Set FIFO trigger level and word count to be equal to the
     * reminder word. Otherwise the HW doesn't generating TX
     * empty interrupt if WCNT is less than FIFO trigger level */
    reminder = ((uint32) dmaObj->numOfWordsNotEffFifoDepth);

    regVal   = CSL_REG32_RD(baseAddr + MCSPI_XFERLEVEL);
    regVal  &= ~(MCSPI_XFERLEVEL_AFL_MASK | MCSPI_XFERLEVEL_AEL_MASK);
    if (jobObj->rxFifoTrigLvl != 1U)
    {
        regVal |= (((reminder - 1U) << MCSPI_XFERLEVEL_AFL_SHIFT) &
                   MCSPI_XFERLEVEL_AFL_MASK);
    }
    regVal |= (((reminder - 1U) << MCSPI_XFERLEVEL_AEL_SHIFT) &
               MCSPI_XFERLEVEL_AEL_MASK);
    regVal &= ~MCSPI_XFERLEVEL_WCNT_MASK;
    regVal |= ((reminder << MCSPI_XFERLEVEL_WCNT_SHIFT) &
               MCSPI_XFERLEVEL_WCNT_MASK);
    CSL_REG32_WR(baseAddr + MCSPI_XFERLEVEL, regVal);

    /* Enable channel */
    CSL_REG32_FINS(
        baseAddr + MCSPI_CHCTRL(csNum),
        MCSPI_CH0CTRL_EN,
        MCSPI_CH0CTRL_EN_ACT);

    extDevCfg = &jobObj->extDevCfg->mcspi;
    /* SPI_CoverageGap_26:Dynamic coverage  for below code cannot be achievable for fifo unaligned
     * data mentioned in bug MCAL-5185,can be fixed in the 1.06 Release
     */
    if (extDevCfg->txRxMode == SPI_TX_RX_MODE_BOTH)
    {
        (void)Spi_DmaTxChunk(hwUnitObj, jobObj, chObj, dmaObj);
        (void)Spi_DmaRxChunk(hwUnitObj, jobObj, chObj, dmaObj);
    }
    else if (extDevCfg->txRxMode == SPI_TX_RX_MODE_TX_ONLY)
    {
        /* TX_ONLY Mode */
        (void)Spi_DmaTxChunk(hwUnitObj, jobObj, chObj, dmaObj);
    }
    else
    {
        /* Do Nothing */
    }

    return;
}

static void Spi_DmaTxChunk(Spi_HwUnitObjType  *hwUnitObj,
                          Spi_JobObjType *jobObj,
                          Spi_ChannelObjType *chObj,
                          Spi_DmaObjType     *dmaObj)
{
    sint32              retVal;
    uint32              length = 0U, shiftVal;
    Udma_ChPdmaPrms     pdmaPrms;
    uint8               *pHpdMem;
    Udma_ChHandle       txChHandle;

    UdmaChPdmaPrms_init(&pdmaPrms);
    /* bufWidth is 1 or 2 (8/16 bit),
        length is the total number of bytes to be transferred */
    if (chObj->bufWidth <= 2U)
    {
        pdmaPrms.elemSize = chObj->bufWidth - 1U;
        shiftVal = chObj->bufWidth - 1U;
        length = (dmaObj->numOfWordsNotEffFifoDepth << shiftVal);
    }
    else
    {
        /* bufWidth is 4(32 bit) */
        pdmaPrms.elemSize = UDMA_PDMA_ES_32BITS;
        shiftVal = 2U;
        length = (dmaObj->numOfWordsNotEffFifoDepth << shiftVal);
    }

    /* Number of words transmitted in each transfer */
    if (length > jobObj->txFifoTrigLvl)
    {
        pdmaPrms.elemCnt = (uint32)(chObj->effTxFifoDepth);
    }
    else
    {
        pdmaPrms.elemCnt = (uint32)(length >> shiftVal);
    }

    /* Don't care for write */
    pdmaPrms.fifoCnt    = 0U;
    txChHandle =        (Udma_ChHandle)(&dmaObj->udmaTxChObj);
    retVal = Udma_chConfigPdma(txChHandle, &pdmaPrms);
    if(UDMA_SOK == retVal)
    {
        retVal = Udma_chEnable(txChHandle);
    }

    if(UDMA_SOK == retVal)
    {
        /* Update Source Buffer Pointer */
#if ((SPI_CHANNELBUFFERS == SPI_EB) || (SPI_CHANNELBUFFERS == SPI_IB_EB))
        if (SPI_EB == chObj->chCfg.channelBufType)
        {
            chObj->curTxBufPtr = chObj->txBufPtr;
        }
#endif
#if ((SPI_CHANNELBUFFERS == SPI_IB) || (SPI_CHANNELBUFFERS == SPI_IB_EB))
        if (SPI_IB == chObj->chCfg.channelBufType)
        {
            chObj->curTxBufPtr = (const uint8 *) &chObj->txIb[0U];
        }
#endif
        /* Update host packet descriptor */
        pHpdMem = (uint8 *) &Spi_TxHpdMem[dmaObj->dmaEnableHwInstId * UDMA_TEST_APP_DESC_SIZE_ALIGN];
        /* Update the source buffer to the start of the Chunk data */
        Spi_DmaHpdInit(txChHandle, pHpdMem,
            (chObj->curTxBufPtr +
                ((chObj->numWordsTxRx - dmaObj->numOfWordsNotEffFifoDepth) << shiftVal)), length);

        /* Submit HPD to channel */
        retVal = Udma_ringQueueRaw(Udma_chGetFqRingHandle(txChHandle), (uint64) pHpdMem);
    }

    return;

}

static void Spi_DmaRxChunk(Spi_HwUnitObjType  *hwUnitObj,
                          Spi_JobObjType *jobObj,
                          Spi_ChannelObjType *chObj,
                          Spi_DmaObjType     *dmaObj)
{
    sint32              retVal;
    uint32              length = 0U, shiftVal;
    Udma_ChPdmaPrms     pdmaPrms;
    uint8               *pHpdMem;
    Udma_ChHandle       rxChHandle;

    UdmaChPdmaPrms_init(&pdmaPrms);
    if (chObj->bufWidth <= 2U)
    {
        pdmaPrms.elemSize = chObj->bufWidth - 1U;
        shiftVal = chObj->bufWidth - 1U;/* bufWidth is 1 or 2 (8/16 bit) */
        length = (dmaObj->numOfWordsNotEffFifoDepth << shiftVal);
    }
    else
    {
        pdmaPrms.elemSize = UDMA_PDMA_ES_32BITS;
        shiftVal = 2U; /* bufWidth is 4(32 bit) */
        length = (dmaObj->numOfWordsNotEffFifoDepth << shiftVal);
    }

    if (length > jobObj->rxFifoTrigLvl)
    {
        pdmaPrms.elemCnt = (uint32)(jobObj->rxFifoTrigLvl >> shiftVal);
        pdmaPrms.fifoCnt = length / jobObj->rxFifoTrigLvl;
    }
    else
    {
        pdmaPrms.elemCnt = (uint32)(length >> shiftVal);
        pdmaPrms.fifoCnt = 1U;
    }

    rxChHandle = (Udma_ChHandle)(&dmaObj->udmaRxChObj);
    retVal = Udma_chConfigPdma(rxChHandle, &pdmaPrms);
    if(UDMA_SOK == retVal)
    {
        retVal = Udma_chEnable(rxChHandle);
    }

    if(UDMA_SOK == retVal)
    {
        /* Update Destination Buffer Pointer */
#if ((SPI_CHANNELBUFFERS == SPI_EB) || (SPI_CHANNELBUFFERS == SPI_IB_EB))
        if (SPI_EB == chObj->chCfg.channelBufType)
        {
            chObj->curRxBufPtr = chObj->rxBufPtr;
        }
#endif
#if ((SPI_CHANNELBUFFERS == SPI_IB) || (SPI_CHANNELBUFFERS == SPI_IB_EB))
        if (SPI_IB == chObj->chCfg.channelBufType)
        {
            chObj->curRxBufPtr = (uint8 *) &chObj->rxIb[0U];
        }
#endif
        /* Update host packet descriptor */
        pHpdMem = (uint8 *) &Spi_RxHpdMem[dmaObj->dmaEnableHwInstId * UDMA_TEST_APP_DESC_SIZE_ALIGN];
         /* Update the Destination buffer to the start of the Chunk */
        Spi_DmaHpdInit(rxChHandle, pHpdMem, (chObj->curRxBufPtr + ((chObj->numWordsTxRx - dmaObj->numOfWordsNotEffFifoDepth) << shiftVal)), length);

        /* Submit HPD to channel */
        retVal = Udma_ringQueueRaw(Udma_chGetFqRingHandle(rxChHandle), (uint64) pHpdMem);
    }

    return;
}

static void Spi_DmaHpdInit(Udma_ChHandle  chHandle,
                              uint8       *pHpdMem,
                              const void    *bufPtr,
                              uint32       length)
{
    CSL_UdmapCppi5HMPD  *pHpd = (CSL_UdmapCppi5HMPD *) pHpdMem;
    uint32              descType = (uint32)CSL_UDMAP_CPPI5_PD_DESCINFO_DTYPE_VAL_HOST;
    uint32              cqRingNum = Udma_chGetCqRingNum(chHandle);

    /* Setup descriptor */
    CSL_udmapCppi5SetDescType(pHpd, descType);
    CSL_udmapCppi5SetEpiDataPresent(pHpd, (bool)false);
    CSL_udmapCppi5SetPsDataLoc(pHpd, 0U);
    CSL_udmapCppi5SetPsDataLen(pHpd, 0U);
    CSL_udmapCppi5SetPktLen(pHpd, descType, length);
    CSL_udmapCppi5SetPsFlags(pHpd, 0U);
    CSL_udmapCppi5SetIds(pHpd, descType, 0x321, UDMA_DEFAULT_FLOW_ID);
    CSL_udmapCppi5SetSrcTag(pHpd, 0x0000);     /* Not used */
    CSL_udmapCppi5SetDstTag(pHpd, 0x0000);     /* Not used */
    CSL_udmapCppi5SetReturnPolicy(
        pHpd,
        descType,
        CSL_UDMAP_CPPI5_PD_PKTINFO2_RETPOLICY_VAL_ENTIRE_PKT,
        CSL_UDMAP_CPPI5_PD_PKTINFO2_EARLYRET_VAL_NO,
        CSL_UDMAP_CPPI5_PD_PKTINFO2_RETPUSHPOLICY_VAL_TO_TAIL,
        cqRingNum);
    CSL_udmapCppi5LinkDesc(pHpd, 0U);
    CSL_udmapCppi5SetBufferAddr(pHpd, (uint64) bufPtr);
    CSL_udmapCppi5SetBufferLen(pHpd, length);
    CSL_udmapCppi5SetOrgBufferAddr(pHpd, (uint64) bufPtr);
    CSL_udmapCppi5SetOrgBufferLen(pHpd, length);
    /* SPI_CoverageGap_26:Dynamic coverage for false condn cannot be achievable as its dependent
     * on MPU_1_0
     */
    if(Spi_DmaIsCacheCoherent() != TRUE)
    {
        SpiApp_wbInvCache((uint8 *)pHpd, (sint32)sizeof(CSL_UdmapCppi5HMPD));
    }
    return;
}

static inline uint32 Spi_DmaIsCacheCoherent(void)
{
    uint32 isCacheCoherent;

#if defined (BUILD_MPU1_0)
    isCacheCoherent = (uint32)TRUE;
#else
    isCacheCoherent = (uint32)FALSE;
#endif

    return (isCacheCoherent);
}

#define SPI_STOP_SEC_CODE
#include "Spi_MemMap.h"

#define SPI_START_SEC_ISR_CODE
#include "Spi_MemMap.h"

void Spi_DmaTxIsrHandler(Spi_DmaObjType  *dmaObj)
{
    Udma_ChHandle       txChHandle;
    uint64              pDesc = 0;
    sint32              status = UDMA_SOK;
    Spi_HWUnitType      hwUnitId;
    Spi_HwUnitObjType   *hwUnitObj;
    uint32              baseAddr, intrStatus, chId;
    Spi_ChannelObjType  *chObj;
    Spi_JobObjType      *jobObj;
    Spi_McspiExternalDeviceConfigType *extDevCfg;
    /* Dynamic coverage for false condn cannot be achievable as its dependent
     on MPU_1_0*/
    if(Spi_DmaIsCacheCoherent() != TRUE)
    {
        SpiApp_invCache(&Spi_TxCqRingMem[dmaObj->dmaEnableHwInstId * UDMA_TEST_APP_RING_MEM_SIZE_ALIGN], (sint32)(sizeof(void *)));
    }

    /*
     * Dequeue the descriptor from the TX completion queue
     * to be re-used for the next transfer
     */
    txChHandle = (Udma_ChHandle)(&dmaObj->udmaTxChObj);
    status = Udma_ringDequeueRaw(Udma_chGetCqRingHandle(txChHandle), &pDesc);
    if(UDMA_SOK != status)
    {
#if (SPI_E_HARDWARE_ERROR != SPI_DEM_NO_EVENT)
        (void)Dem_SetEventStatus(SPI_E_HARDWARE_ERROR, DEM_EVENT_STATUS_FAILED);
#endif
    }
    else
    {
        /* Close the TX channel */
        (void)Udma_chDisable((Udma_ChHandle)(txChHandle), UDMA_DEFAULT_CH_DISABLE_TIMEOUT);

        /* Process next Job/Ch */
        hwUnitId = dmaObj->hwUnitId;
        hwUnitObj = Spi_getHwUnitObj(hwUnitId);
        jobObj = hwUnitObj->curJobObj;
        extDevCfg = &jobObj->extDevCfg->mcspi;
        if (extDevCfg->txRxMode == SPI_TX_RX_MODE_TX_ONLY)
        {
            baseAddr = Spi_getHwUnitBaseAddr(hwUnitId);
            /* Get interrupt status */
            intrStatus = CSL_REG32_RD(baseAddr + MCSPI_IRQSTATUS);
            /* clear interrupt status */
            CSL_REG32_WR(baseAddr + MCSPI_IRQSTATUS, intrStatus);
            if ((intrStatus & MCSPI_IRQSTATUS_EOW_MASK) == MCSPI_IRQSTATUS_EOW_MASK)
            {
                chId  = jobObj->jobCfg.channelList[jobObj->curChIdx];
                chObj = Spi_getCurrChannelObj(chId);
                if (dmaObj->numOfWordsNotEffFifoDepth == 0U)
                {
                    Spi_processChCompletion(hwUnitObj, SPI_JOB_OK);
#if (SPI_E_HARDWARE_ERROR != SPI_DEM_NO_EVENT)
                    (void)Dem_SetEventStatus(SPI_E_HARDWARE_ERROR, DEM_EVENT_STATUS_PASSED);
#endif
                }
                /* Dynamic coverage  for below code cannot be achievable for
                fifo unaligned data mentioned in bug MCAL-5185,can be fixed in
                the 1.06 Release*/
                else
                {
                    Spi_DmaInitiateChunkXfer(dmaObj, hwUnitObj, jobObj, chObj, baseAddr);
                    /* Reset to 0 so that it should not initiate chunk transfer again */
                    dmaObj->numOfWordsNotEffFifoDepth = 0U;
                }
            }
        }
    }
}

void Spi_DmaRxIsrHandler(Spi_DmaObjType  *dmaObj)
{
    Udma_ChHandle       rxChHandle;
    uint64              pDesc = 0;
    Spi_HWUnitType      hwUnitId;
    Spi_HwUnitObjType   *hwUnitObj;
    uint32              baseAddr, intrStatus, chId;
    sint32              status = UDMA_SOK;
    Spi_ChannelObjType  *chObj;
    Spi_JobObjType      *jobObj;

    /* Channel completed */

    if(Spi_DmaIsCacheCoherent() != TRUE)
    {
        SpiApp_invCache(&Spi_RxCqRingMem[dmaObj->dmaEnableHwInstId * UDMA_TEST_APP_RING_MEM_SIZE_ALIGN], (sint32)(sizeof(void *)));
    }
    /*
     * Dequeue the descriptor from the TX completion queue
     * to be re-used for the next transfer
     */
    rxChHandle = (Udma_ChHandle)(&dmaObj->udmaRxChObj);
    status = Udma_ringDequeueRaw(Udma_chGetCqRingHandle(rxChHandle), &pDesc);
    if(UDMA_SOK != status)
    {
#if (SPI_E_HARDWARE_ERROR != SPI_DEM_NO_EVENT)
        (void)Dem_SetEventStatus(SPI_E_HARDWARE_ERROR, DEM_EVENT_STATUS_FAILED);
#endif
    }
    else
    {
        /* Close the RX channel */
        (void)Udma_chDisable((Udma_ChHandle)(rxChHandle), UDMA_DEFAULT_CH_DISABLE_TIMEOUT);
        /* Process next Job/Ch */
        hwUnitId = dmaObj->hwUnitId;
        baseAddr = Spi_getHwUnitBaseAddr(hwUnitId);
        /* Get interrupt status */
        intrStatus = CSL_REG32_RD(baseAddr + MCSPI_IRQSTATUS);
        /* clear interrupt status */
        CSL_REG32_WR(baseAddr + MCSPI_IRQSTATUS, intrStatus);
        if ((intrStatus & MCSPI_IRQSTATUS_EOW_MASK) == MCSPI_IRQSTATUS_EOW_MASK)
        {
            hwUnitObj = Spi_getHwUnitObj(hwUnitId);
            jobObj = hwUnitObj->curJobObj;
            chId  = jobObj->jobCfg.channelList[jobObj->curChIdx];
            chObj = Spi_getCurrChannelObj(chId);
            if (dmaObj->numOfWordsNotEffFifoDepth == 0U)
            {
                Spi_processChCompletion(hwUnitObj, SPI_JOB_OK);
#if (SPI_E_HARDWARE_ERROR != SPI_DEM_NO_EVENT)
                (void)Dem_SetEventStatus(SPI_E_HARDWARE_ERROR, DEM_EVENT_STATUS_PASSED);
#endif
            }
            /* SPI_CoverageGap_27:Dynamic coverage  for below code cannot be achievable for fifo
             * unaligned data mentioned in bug MCAL-5185,can be fixed in
             * the 1.06 Release
             */
            else
            {
                Spi_DmaInitiateChunkXfer(dmaObj, hwUnitObj, jobObj, chObj, baseAddr);
                /* Reset to 0 so that it should not initiate chunk transfer again */
                dmaObj->numOfWordsNotEffFifoDepth = 0U;
            }
        }
    }
}

#define SPI_STOP_SEC_ISR_CODE
#include "Spi_MemMap.h"

#endif /* #if (STD_ON == SPI_DMA_ENABLE) */
