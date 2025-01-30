/*
*
* Copyright (c) 2024 Texas Instruments Incorporated
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
 *  \file     Spi_Priv.h
 *
 *  \brief    This file contains the common declarations and macros used across
 *            all the SPI MCAL driver files.
 *
 */

#ifndef SPI_PRIV_H_
#define SPI_PRIV_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "SchM_Spi.h"
#include "Spi_Utils.h"
#include "Spi_Cfg.h"
#include "Spi/mcspi_hw/V0/mcspi.h"

#if (STD_ON == SPI_DMA_ENABLE)
#include <ti/drv/udma/udma.h>
#endif

#include "Det.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/** \brief Pre-declaration for HW unit object */
typedef struct Spi_HwUnitObjType_t Spi_HwUnitObjType;

/**
 *  \brief McSPI Register Offset for MCSPI_CHxCONF, MCSPI_CHxSTAT,
 *  MCSPI_CHxCTRL, MCSPI_TXx and MCSPI_RXx register set.
 */
#define MCSPI_REG_OFFSET                (0x14U)

/** \brief Bit mask to clear all status bits */
#define MCSPI_IRQSTATUS_CLEAR_ALL       (MCSPI_IRQSTATUS_EOW_MASK |           \
                                         MCSPI_IRQSTATUS_WKS_MASK |           \
                                         MCSPI_IRQSTATUS_RX3_FULL_MASK |      \
                                         MCSPI_IRQSTATUS_TX3_UNDERFLOW_MASK | \
                                         MCSPI_IRQSTATUS_TX3_EMPTY_MASK |     \
                                         MCSPI_IRQSTATUS_RX2_FULL_MASK |      \
                                         MCSPI_IRQSTATUS_TX2_UNDERFLOW_MASK | \
                                         MCSPI_IRQSTATUS_TX2_EMPTY_MASK |     \
                                         MCSPI_IRQSTATUS_RX1_FULL_MASK |      \
                                         MCSPI_IRQSTATUS_TX1_UNDERFLOW_MASK | \
                                         MCSPI_IRQSTATUS_TX1_EMPTY_MASK |     \
                                         MCSPI_IRQSTATUS_RX0_OVERFLOW_MASK |  \
                                         MCSPI_IRQSTATUS_RX0_FULL_MASK |      \
                                         MCSPI_IRQSTATUS_TX0_UNDERFLOW_MASK | \
                                         MCSPI_IRQSTATUS_TX0_EMPTY_MASK)

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/**
 *  \brief SPI Channel object structure.
 */
typedef struct
{
    Spi_ChannelConfigType chCfg;
    /**< SPI channel config passed during init */
    P2CONST(Spi_DataBufferType, AUTOMATIC, SPI_APPL_DATA) txBufPtr;
    /**< TX buffer pointer given by user. This should be persistent */
    P2VAR(Spi_DataBufferType, AUTOMATIC, SPI_APPL_DATA) rxBufPtr;
    /**< RX buffer pointer given by user. This should be persistent */
    Spi_NumberOfDataType  numWordsTxRx;
    /**< Number of words to transfer */

    /*
     * Design: MCAL-6561,MCAL-6677,MCAL-6580,MCAL-6541,MCAL-6723
     */
#if ((SPI_CHANNELBUFFERS == SPI_IB) || (SPI_CHANNELBUFFERS == SPI_IB_EB))
    Spi_DataBufferType    txIb[SPI_IB_MAX_LENGTH];
    /**< Internal TX buffer */
    Spi_DataBufferType    rxIb[SPI_IB_MAX_LENGTH];
    /**< Internal RX buffer */
#endif

    const uint8          *curTxBufPtr;
    /**< Current TX buffer pointer */
    uint8                *curRxBufPtr;
    /**< Current RX buffer pointer */
    Spi_NumberOfDataType  curTxWords;
    /**< Number of words transmitted. We need seperate counters for TX/RX
     *   because when FIFO in enabled, TX writes happen in advance where as
     *   RX will happen on actual received data. */
    Spi_NumberOfDataType  curRxWords;
    /**< Number of words received */
    uint8                 bufWidth;
    /**< Width of buffer in bytes - used for accessing the TX/RX buffer.
     *   When dataWidth < 9,           bufWidth = uint8 (1 bytes)
     *   When dataWidth >= 9  && < 17, bufWidth = uint16 (2 bytes)
     *   When dataWidth >= 17 && < 33, bufWidth = uint32 (4 bytes)
     */
    uint32                effTxFifoDepth;
    /**< Effective TX FIFO depth in words - depends on dataWidth */
    uint32                dataWidthBitMask;
    /**< Data width mask depending on SPI word size */
} Spi_ChannelObjType;

/**
 *  \brief SPI Sequence object structure.
 */
typedef struct
{
    Spi_SeqConfigType seqCfg;
    /**< SPI sequence config passed during init */
    Spi_SeqConfigType_PC seqCfg_PC;
    /**< SPI PC sequence config passed during init */
    Spi_SeqResultType seqResult;
    /**< SPI sequence result/status */
    uint32            numJobsPending;
    /**< Number of jobs still pending. Used for notifying sequence end */
} Spi_SeqObjType;

/**
 *  \brief SPI Job object structure.
 */
typedef struct
{
    Spi_JobConfigType             jobCfg;
    /**< SPI job config passed during init */
    Spi_JobConfigType_PC          jobCfg_PC;
    /**< SPI PC job config passed during init */
    Spi_ExternalDeviceConfigType *extDevCfg;
    /**< SPI job config passed during init */
    Spi_JobResultType             jobResult;
    /**< SPI job result/status */
    Spi_HwUnitObjType            *hwUnitObj;
    /**< SPI HW unit object as ID and array index may not be same.
     *   This is used for easy de-referencing */
    Spi_SeqObjType               *seqObj;
    /**< SPI sequence object pointer to which the job belongs.
     *   This is used for easy de-referencing */
    Spi_UtilsNode                 nodeObj;
    /**< Node object used for node memory to be used in link list */
    uint32                        curChIdx;
    /**< Current channel index. Used for notifying job end */
    uint32                        txFifoTrigLvl;
    /**< TX FIFO trigger level in bytes */
    uint32                        rxFifoTrigLvl;
    /**< RX FIFO trigger level in bytes */
} Spi_JobObjType;

/**
 *  \brief SPI Hardware unit object structure.
 */
struct Spi_HwUnitObjType_t
{
    Spi_HwUnitConfigType hwUnitCfg;
    /**< SPI HW unit config passed during init */
    Spi_HwUnitResultType hwUnitResult;
    /**< SPI HW unit result/status */
    uint32               baseAddr;
    /**< Base address of the hardware */
    boolean              enabledmaMode;
    /**< Enable SPI DMA Support per instance */
    Spi_UtilsLinkListObj jobList;
    /**< Job link list per hardware */
    Spi_JobObjType      *curJobObj;
    /**< When hardware is busy, this points to the current job that is in
     *   progress. When hardware is idle, this will be NULL */
    Spi_JobType          curConfiguredJobId;
    /**< Currently configured job ID. Used to avoid re-configuring the hardware
     *   again when the same job ID is used back to back */
};

#if (STD_ON == SPI_DMA_ENABLE)
/**
 *  \brief SPI DMA driver object structure.
 */
typedef struct
{
    struct Udma_ChObj       udmaTxChObj;
    /**< UDMA tx channel object */
    struct Udma_ChObj       udmaRxChObj;
    /**< UDMA rx channel object */
    struct Udma_EventObj    udmaTxCqEventObj;
    /**< UDMA tx completion queue object */
    struct Udma_EventObj    udmaRxCqEventObj;
    /**< UDMA rx completion queue object */
    uint32                  dmaEnableHwInstId;
    /**< SPI DMA Callback variable */
    uint32                  numOfWordsNotEffFifoDepth;
    /**< Number of Word count not a multiple of FIFO Depth */
    Spi_HWUnitType          hwUnitId;
    /**< HWUnit associated with this job */
    Spi_CsPinType           csPin;
    /**< Chip select pin to use */
    uint32                  dmaTxChIntrNum;
    /**< DMA TX channel interrupt number */
    uint32                  dmaRxChIntrNum;
    /**< DMA RX channel interrupt number */
}Spi_DmaObjType;
#endif
/**
 *  \brief SPI driver object structure.
 */
typedef struct
{
    uint32                       isAsynInProgress;
    /**< Flag to indicate any async transfer is in progress */
    Spi_AsyncModeType            asyncMode;
    /**< Aysnc mode set by user */
    uint8                        maxChannels;
    /**< Maximum number of channels.
     *   Should not be more than SPI_MAX_CHANNELS */
    uint8                        maxJobs;
    /**< Maximum number of jobs
     *   Should not be more than SPI_MAX_JOBS */
    uint8                        maxSeq;
    /**< Maximum number of sequences
     *   Should not be more than SPI_MAX_SEQ */
    uint8                        maxHwUnit;
    /**< Maximum number of HW unit
     *   Should not be more than SPI_MAX_HW_UNIT */
    uint8                        maxExtDevCfg;
    /**< Maximum number of external device configurations
     *   Should not be more than SPI_MAX_EXT_DEV */
#if (STD_ON == SPI_DMA_ENABLE)
    struct Udma_DrvObj           udmaDrvObj;
    /**< SPI DMA Driver objects */
    Spi_DmaObjType               dmaObj[SPI_MAX_HW_DMA_UNIT];
    /**< SPI DMA Channel objects */
    Udma_DrvHandle               drvHandle;
    /**< SPI DMA Driver handle    */
    uint32                       dmaEnableHwIndx;
    /**< Index pointer to MCSPI instance with DMA enabled */
#endif
    Spi_ChannelObjType           channelObj[SPI_MAX_CHANNELS];
    /**< SPI channel objects */
    Spi_JobObjType               jobObj[SPI_MAX_JOBS];
    /**< SPI job objects */
    Spi_SeqObjType               seqObj[SPI_MAX_SEQ];
    /**< SPI sequence objects */
    Spi_HwUnitObjType            hwUnitObj[SPI_MAX_HW_UNIT];
    /**< SPI hw unit objects */
    Spi_ExternalDeviceConfigType extDevCfg[SPI_MAX_EXT_DEV];
    /**< External HW device config */
} Spi_DriverObjType;

extern Spi_DriverObjType Spi_DrvObj;

/* ========================================================================== */
/*                           Typedefs                                         */
/* ========================================================================== */
/** \brief Declaration for channel object pointer type */
typedef Spi_ChannelObjType * Spi_ChannelObjPtrType;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

void Spi_hwUnitInit(Spi_HwUnitObjType *hwUnitObj);
void Spi_hwUnitDeInit(Spi_HwUnitObjType *hwUnitObj);

Std_ReturnType Spi_startSeqAsync(Spi_SeqObjType *seqObj);
Std_ReturnType Spi_startSeqSync(Spi_SeqObjType *seqObj);
#if (STD_ON == SPI_CANCEL_API)
void Spi_cancelSequence(Spi_SeqObjType *seqObj);
#endif  /* #if (STD_ON == SPI_CANCEL_API) */

void Spi_processEvents(Spi_HwUnitObjType *hwUnitObj);

Spi_HwUnitObjType *Spi_getHwUnitObj(Spi_HWUnitType HWUnit);
void Spi_resetDrvObj(Spi_DriverObjType *drvObj);
#if (STD_ON == SPI_DEV_ERROR_DETECT)
Std_ReturnType Spi_checkConfig(const Spi_ConfigType *cfgPtr);
#endif  /* #if (STD_ON == SPI_DEV_ERROR_DETECT) */
void Spi_copyConfig(Spi_DriverObjType *drvObj, const Spi_ConfigType *cfgPtr);
uint32 Spi_getHwUnitBaseAddr(Spi_HWUnitType hwUnitId);
uint32 Spi_getDataWidthBitMask(uint32 dataWidth);

/* McSPI related functions */
void Spi_mcspiInit(const Spi_HwUnitObjType *hwUnitObj);
void Spi_mcspiDeInit(const Spi_HwUnitObjType *hwUnitObj);
void Spi_mcspiConfigJob(const Spi_HwUnitObjType *hwUnitObj,
                        Spi_JobObjType          *jobObj);
void Spi_mcspiConfigCh(const Spi_HwUnitObjType *hwUnitObj,
                       const Spi_JobObjType    *jobObj,
                       Spi_ChannelObjType      *chObj);
void Spi_mcspiStart(const Spi_HwUnitObjType *hwUnitObj,
                    const Spi_JobObjType    *jobObj,
                    uint32                   isIntrMode);
void Spi_mcspiReStart(const Spi_HwUnitObjType *hwUnitObj,
                      const Spi_JobObjType    *jobObj);
Spi_JobResultType Spi_mcspiContinueTxRx(const Spi_HwUnitObjType *hwUnitObj,
                                        const Spi_JobObjType    *jobObj,
                                        Spi_ChannelObjType      *chObj);
Spi_JobResultType Spi_mcspiXferJob(const Spi_HwUnitObjType *hwUnitObj,
                                   Spi_JobObjType          *jobObj);
void Spi_mcspiStop(const Spi_HwUnitObjType *hwUnitObj,
                   const Spi_JobObjType    *jobObj);
void Spi_mcspiClearAllIrqStatus(uint32 baseAddr);
void Spi_mcspiDisableAllIntr(uint32 baseAddr);
#if (STD_ON == SPI_REGISTER_READBACK_API)
void Spi_mcspiRegReadback(const Spi_HwUnitObjType *hwUnitObj,
                          Spi_RegisterReadbackType *RegRbPtr);
#endif  /* #if (STD_ON == SPI_REGISTER_READBACK_API) */

#if (STD_ON == SPI_DEV_ERROR_DETECT)
void Spi_reportDetError(uint8 apiId, uint8 errorId);
#endif  /* #if (STD_ON == SPI_DEV_ERROR_DETECT) */
void Spi_reportDetRuntimeError(uint8 apiId, uint8 errorId);

Spi_ChannelObjPtrType Spi_getCurrChannelObj(Spi_ChannelType chId);
void Spi_processChCompletion(Spi_HwUnitObjType *hwUnitObj,
                                    Spi_JobResultType  jobResult);
uint32 Spi_mcspiGetCsIntrMask(uint32 csNum, Spi_TxRxMode txRxMode);
#if (STD_ON == SPI_DMA_ENABLE)
Std_ReturnType Spi_DmaConfig(Spi_DriverObjType *drvObj,
    const Spi_ConfigType *cfgPtr);
void Spi_DmaTransferStart(Spi_HwUnitObjType  *hwUnitObj,
                          Spi_JobObjType *jobObj,
                          Spi_ChannelObjType *chObj);
void Spi_DmaFreeChannel(Spi_DmaObjType *dmaObj);
void Spi_DmaTransferReStart(Spi_HwUnitObjType *hwUnitObj,
                       Spi_JobObjType    *jobObj,
                      Spi_ChannelObjType *chObj);
void Spi_DmaStop(Spi_HwUnitObjType  *hwUnitObj,
                          Spi_JobObjType *jobObj);
void Spi_DmaTxIsrHandler(Spi_DmaObjType  *dmaObj);
void Spi_DmaRxIsrHandler(Spi_DmaObjType  *dmaObj);
#endif


/* ========================================================================== */
/*                       Static Function Definitions                          */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/** \brief SPI driver status */
extern volatile Spi_StatusType Spi_DrvStatus;

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef SPI_PRIV_H_ */
