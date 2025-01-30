/* =============================================================================
 *   Copyright (c) Texas Instruments Incorporated 2018
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
 */

/**
 *  \file vhwa_m2mLdcPriv.h
 *
 *  \brief LDC Private header file
 *
 */

#ifndef VHWA_M2M_LDC_PRIV_H_
#define VHWA_M2M_LDC_PRIV_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/osal/osal.h>

#include <ti/csl/csl_fvid2_dataTypes.h>
#include <ti/drv/fvid2/include/fvid2_drvMgr.h>

#include <ti/drv/vhwa/src/csl/include/csl_hts.h>
#include <ti/drv/vhwa/src/csl/include/csl_lse.h>
#include <ti/drv/vhwa/src/csl/include/csl_vpactop.h>
#include <ti/drv/vhwa/src/csl/include/csl_ldc.h>

#include <ti/drv/vhwa/include/vhwa_m2mLdc.h>

#include <ti/drv/vhwa/include/vhwa_common.h>
#include <ti/drv/vhwa/src/drv/vhwa_cfg.h>
#include <ti/drv/vhwa/src/drv/vhwa_utils.h>

#include <vhwa_vpac_priv.h>

#ifdef __cplusplus
extern "C" {
#endif


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */


/** \brief Minimum number of blocks required in SL2 */
#define VHWA_M2M_LDC_MIN_SL2_BLOCKS           (2u)

#define VHWA_MAX_CH_PER_OUTPUT                ( \
    VHWA_M2M_LDC_OUT_DMA_CH / LDC_MAX_OUTPUT)

#define VHWA_LDC_WAIT_CYCLES                              (1000)
#define VHWA_LDC_MAX_WAIT_LOOP_CNT                        (100)
#define VHWA_LDC_MAX_BUSY_WAIT_LOOP_CNT                   (100000)

/* Maximim number of DMA Channels required for LDC */
#define VHWA_M2M_LDC_OUT_DMA_CH                 (4U)
#define VHWA_M2M_LDC_MAX_DMA_CH                 (VHWA_M2M_LDC_OUT_DMA_CH)

#define VHWA_LDC_FLOW_ID                        (0x3fffU)

/** \brief Maximum loop count - used while dequeuing UDMA */
#define VHWA_LDC_MSC_MAX_WAIT_LOOP_CNT          (1000u)

/** \brief Number of ring entries - there many requests can be enqueued */
#define VHWA_M2M_LDC_UDMA_RING_ENTRIES          (16U)
/** \brief Size (in bytes) of each ring entry (Size of pointer - 64-bit) */
#define VHWA_M2M_LDC_UDMA_RING_ENTRY_SIZE       (sizeof(uint64_t))
/** \brief Total ring memory */
#define VHWA_M2M_LDC_UDMA_RING_MEM_SIZE           \
    (VHWA_M2M_LDC_UDMA_RING_ENTRIES * VHWA_M2M_LDC_UDMA_RING_ENTRY_SIZE)

/** \brief Max number of TR required per region */
#define VHWA_M2M_LDC_MAX_TR_PER_REGION          (3u)

/** \brief Total Transfer Record Descriptors
 *         Three TR descriptor for each regio and total 9 regions,
 *         so total 27 TRs required at max */
#define VHWA_M2M_LDC_UDMA_NUM_TR_DESC             \
    (VHWA_M2M_LDC_MAX_TR_PER_REGION * LDC_MAX_REGIONS)

/**
 *  \brief UDMA TR packet descriptor memory.
 *  This contains the CSL_UdmapCppi5TRPD for header         +
 *     #VHWA_M2M_LDC_UDMA_NUM_TR_DESC number of TRs         +
 *     #VHWA_M2M_LDC_UDMA_NUM_TR_DESC number of TR response +
 *     padding bytes to make total size aligned to #UDMA_CACHELINE_ALIGNMENT
 */
#define VHWA_M2M_LDC_UDMA_TRPD_SIZE             (sizeof(CSL_UdmapTR15) + \
    (sizeof(CSL_UdmapTR15) * VHWA_M2M_LDC_UDMA_NUM_TR_DESC) + \
    (VHWA_M2M_LDC_UDMA_NUM_TR_DESC * 4U) + \
    (UDMA_CACHELINE_ALIGNMENT - (VHWA_M2M_LDC_UDMA_NUM_TR_DESC * 4U)))



/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */



typedef struct
{
    uint32_t                isEnabled;
    /**< Flag to indicate if this region is enabled or not */

    uint32_t                blockWidth;
    /**< Block Width */
    uint32_t                blockWidthInBytes;
    /**< Block Width in terms of bytes */
    uint32_t                blockHeight;
    /**< Block Height */

    uint32_t                width;
    /**< Region Width, stored here so that it can be accessed
     *   easily without going to ldcCfg */
    uint32_t                height;
    /**< Region Height , stored here so that it can be accessed
     *   easily without going to ldcCfg */

    uint32_t                bufAddrOff[VHWA_M2M_LDC_MAX_TR_PER_REGION];
    /**< Buffer Address Offset for each TR in this region */
    uint64_t                sl2Addr[VHWA_M2M_LDC_MAX_TR_PER_REGION];
    /**< SL2 Buffer address for each TR in this region */
    uint32_t                numSl2Blocks;
    /**< Number of SL2 blocks,
         calculated by dividing sl2 block height by LDC block height */

    uint32_t                sicnt2[VHWA_M2M_LDC_MAX_TR_PER_REGION];
    /**< Source Icnt2 */
    uint32_t                sicnt3[VHWA_M2M_LDC_MAX_TR_PER_REGION];
    /**< Source Icnt2 */
    uint32_t                dicnt2[VHWA_M2M_LDC_MAX_TR_PER_REGION];
    /**< Destination Icnt2 */
    uint32_t                dicnt3[VHWA_M2M_LDC_MAX_TR_PER_REGION];
    /**< Destination Icnt2 */

    uint32_t                numTr;
    /**< Number of TR required for this region */
} Vhwa_M2mLdcRegionParams;

typedef struct
{
    uint32_t                isEnabled;
    /**< Flag to indicate if this channel is enabled or not */

    /* Below set of parameters are used in LSE configuration */
    uint32_t                maxSl2BlockPitch;
    /**< Max of all block pitches in multi-region case,
     */
    uint32_t                sl2NumLines;
    /**< SL2 circular buffer size in terms of lines */
    uint32_t                ccsf;
    /**< Color component storage format */
    uint64_t                sl2Addr;
    /**< SL2 start address for this channel */
    uint32_t                sl2Size;
    /**< Size of SL2 memory for this channel */

    uint8_t                *rxTrMem;
    /**< Start address of the TR memory */
    uint8_t                *rxTrRespMem;
    /**< Address of the start of the TR response memory */

    uint32_t                pitch;
    /**< output pitch of the buffer for this channel */

    uint32_t                dataFmt;
    /**< DataFormat for the output channel,
         Only required in case data format is YUV422,
         In that case, YUV422 output in LSE is enabled by
         pairing two channels */

    Vhwa_M2mLdcRegionParams regPrms[LDC_MAX_HORZ_REGIONS][LDC_MAX_VERT_REGIONS];
    /**< Region parameters for this channel */

    uint32_t                minHtsDepth;
    /**< Minimum HTS depth across all regions,
     *   Depth in HTS must be configured with minimum of all regions */

    uint32_t                totalTRs;
    /**< Total number of TRs required for all regions,
     *   Used in setting length in TR Header */

    uint64_t                startAddr;
    /**< start address of the buffer for this channel.
     *   Temporary copying buffer address from FVID2_Frame to this variable,
     *   so that complete FVID2_FrameList need not be passed
     *   across functions */
} Vhwa_M2mLdcChParams;

typedef struct
{
    uint32_t                isUsed;
    /**< Flag to indicate if this object is free or not */

    Vhwa_M2mLdcCreateArgs   createArgs;
    /** Create Params, copied locally */
    Fvid2_DrvCbParams       cbPrms;
    /** Callback parameters, copied locally */

    Vhwa_M2mLdcChParams     chPrms[VHWA_M2M_LDC_MAX_DMA_CH];
    /* Each output color component is treated as a channel and
     * this structure contains channels specific parameters like
     * pointer to TR memory, SL2 pitch, Region parameters etc.. */

    CSL_LseConfig           lseCfg;
    /**< LSE Configuration for this handle */
    Ldc_Config              ldcCfg;
    /**< LDC Configuration for this handle */
    CSL_HtsSchConfig        htsCfg;
    /**< HTS Configuration for this handle */

    uint32_t                hIdx;
    /**< The index of this handle object in the array,
         used in allocating UDMA memory */

    uint32_t                curIterCnt;
    /**< Current iteration count used to process Luma and chroma components */

    volatile uint64_t       perfNum;
    /**< Performance number for the last submitted request,
         Used only if GetTimeStamp function pointer is set in
         Create Args */

    Fvid2UtilsLinkListObj   doneQObj;
    /**< Done Queue, queue containing queue objects,
     *   for which LDC processing is completed */
    Fvid2UtilsLinkListObj  *doneQ;
    /**< Pointer to Done Queue */

    uint32_t                numPendReq;
    /**< Count of pending requests */

    Ldc_ErrEventParams      eePrms;
    /**< Parameters for the Error Events */

    uint32_t                isCfgUpdated;
    /**< Flag to know if config is updated or not. */

} Vhwa_M2mLdcHandleObj;

/**
 *  \brief Structure defining the queue object used in queue/dequeue operation.
 *         This is used for enqueueing pending request as well as
 *         the completed requested to the drivers request queue
 *         and done queue resepctively. This stores pointer to handle
 *         object, for easy access to configurations and also input
 *         and output frame lists.
 */
typedef struct
{
    Fvid2Utils_QElem        qElem;
    /**< FVID2 Utils queue element used in node addition. */
    Vhwa_M2mLdcHandleObj   *hObj;
    /**< Handle Object reference for this Queue object */
    Fvid2_FrameList         inFrmList;
    /**< FVID2 frame list for store list of input frames. */
    Fvid2_FrameList         outFrmList;
    /**< FVID2 frame list for store list of output frames. */
} Vhwa_M2mLdcQueueObj;

typedef struct
{
    uint32_t                initDone;
    /**< Flag to know if LDC is initialized or not,
         Essentially init is called or not */
    uint32_t                isSl2AllocDone;
    /**< Flag to know if SL2 memory is allocated or not.
     *   Essentially if Vhwa_m2mLdcAllocSl2 is called and is successfully
     *   executed or not.
     *   Driver open is allowed only if this flag is set to UTRUE */
    Vhwa_M2mLdcInitParams   initPrms;
    /**< Init paramters, copied here */

    uint32_t                isRegistered;
    /**< Flag to indicate if this instance object is registered to FVID */

    struct Udma_ChObj       utcChObj[VHWA_M2M_LDC_MAX_DMA_CH];
    /**< UDMA Channel object for input channels */
    Udma_ChHandle           utcChHndl[VHWA_M2M_LDC_MAX_DMA_CH];
    /**< UDMA Channel handles for input channels */
    uint32_t                complRingNum[VHWA_M2M_LDC_MAX_DMA_CH];
    /**< UDMA Completion ring number for input channels */
    Udma_RingHandle         fqRingHndl[VHWA_M2M_LDC_MAX_DMA_CH];
    /**< Forward Queue's Ring Handle */
    Udma_RingHandle         cqRingHndl[VHWA_M2M_LDC_MAX_DMA_CH];
    /**< Completion Queue's Ring Handle */
    uint32_t                utcCh[VHWA_M2M_LDC_MAX_DMA_CH];

    SemaphoreP_Handle       lock;
    /**< Semaphore to protect instance object's variables. */
    SemaphoreP_Handle       hwaLock;
    /**< Semaphore to protect LDC HWA.
     *   Only one processing request can be submitted to the LDC HW.
     *   This lock makes sure that except submitted one, all other
     *   requests are waiting on this semaphore.
     *   There is no longer support for submission from ISR and so
     *   no ISR protection in processRequest. */

    uint32_t                openCnt;
    /**< Keeps track of number of opens,
         used to initialize common configuration only once
         for example, enabling LDC at the top level or
            initializing ring accelerator */

    uint32_t                pipeline;
    /**< HTS Pipeline number,
         Same pipeline is used for all handles,
         Allocated on the first open
         Refer to #CSL_HtsPipeline for valid values. */

    HwiP_Handle             intrHndl;
    /**< Handle to Hardware Interrupt */

    Vhwa_M2mLdcSl2AllocPrms sl2AllocPrms;
    /**< Sl2 Alloc Parameters */

    uint64_t                sl2Addr[LDC_MAX_OUTPUT];
    /**< Address of SL2 used by LDC driver,
         Storing based on the output, so that higher block size can be used
         when format is luma or chroma only */
    uint32_t                sl2Size[LDC_MAX_OUTPUT];
    /**< Sl2 Size allocated to LDC driver */

    Vhwa_M2mLdcQueueObj    *actQObj;
    /**< Pointer points to queue object, whose request is submitted to
     * the HW and not yet completed */
    Vhwa_M2mLdcHandleObj   *lastHndlObj;
    /**< Pointer to the handle object, whose request was submitted
     *   and completed */

    Ldc_SocInfo             socInfo;
    /**< Soc Information, like base address, interrupt number,
     *   INTD irq numbers etc. */
    uint32_t                irqNum;
    /**< Core IRQ Number to be used for LDC,
     *   Used internally for isr registration,
     *   Storing it locally for easy access */
    uint32_t                vhwaIrqNum;
    /**< VHWA IRQ Number,
     *   Refer to #VHWA_IrqNum for valid number.
     *   Used to enable the irq in INTD number,
     *   Storing it locally for easy access */

    Fvid2UtilsLinkListObj   freeQObj;
    /**< Free Queue, queue containing free queue objcts */
    Fvid2UtilsLinkListObj  *freeQ;
    /**< Pointer to Free Queue */
    Fvid2UtilsLinkListObj   reqQObj;
    /**< Request queue, containing list of pending requests */
    Fvid2UtilsLinkListObj  *reqQ;
    /**< Pointer to request Queue */
    Vhwa_M2mLdcQueueObj     ldcQObj[VHWA_M2M_LDC_UDMA_RING_ENTRIES];
    /**< Ldc queue objects */

    uint32_t                totalReqCnt;
    /**< Keeps track of total number of Request submitte */
    uint32_t                totalIntrCnt;
    /**< Keeps track of total number of interrupts,
      *  including error interrupts */
    uint32_t                errIntrInvalidStatus;
    /**< Keeps track of Error interrupt,
      * when pipeline completion status is 0 */
    uint32_t                errIntrRepeateIntr;
    /**< Keeps track of Error interrupt,
      * when pipeline completion status is repeated */
    uint32_t                errInvalidHtsPipelineStatus;
    /**< Keeps track of Invalid HTS Pipline Status Error */

    uint32_t errNoActQObj;
    uint32_t errNoLastHndlObj;
} Vhwa_M2mLdcInstObj;


/* ========================================================================== */
/*                           Function Declarations                            */
/* ========================================================================== */

/**
 * \brief   Function to initialize and allocate UTC/UDMA(External) Channels
 *
 **/
int32_t Vhwa_m2mLdcUdmaInit(Vhwa_M2mLdcInstObj *instObj,
    Vhwa_M2mLdcInitParams *initPrms);
int32_t Vhwa_m2mLdcUdmaDeInit(const Vhwa_M2mLdcInstObj *instObj);

void Vhwa_m2mLdcSetTrDesc(const Vhwa_M2mLdcInstObj *instObj,
    Vhwa_M2mLdcHandleObj *hObj);
int32_t Vhwa_m2mLdcStartCh(const Vhwa_M2mLdcInstObj *instObj);
int32_t Vhwa_m2mLdcStopCh(const Vhwa_M2mLdcInstObj *instObj);
int32_t Vhwa_m2mLdcSubmitRing(Vhwa_M2mLdcInstObj *instObj,
    Vhwa_M2mLdcHandleObj *hObj);
int32_t Vhwa_m2mLdcAllocUdmaMem(Vhwa_M2mLdcHandleObj *hObj);
/**
 * \brief   Function to set the buffer addresses in TRs
 *          The buffer address is locally stored in the channel parameter.
 *          It uses this buffer address and adds the region offset
 *          into it and set it in the appropriate TR.
 *
 * \param   hObj    Handle Object, from which channel object is taken and then
 *                  for each enabled channel object, Addresses are
 *                  populated in TR.
 *
 **/
void Vhwa_m2mLdcSetOutputAddress(Vhwa_M2mLdcHandleObj *hObj);

int32_t Vhwa_m2mLdcPopRings(Vhwa_M2mLdcInstObj *instObj,
    Vhwa_M2mLdcHandleObj *hObj);
uint32_t Vhwa_m2mLdcCalcHorzSizeInBytes(uint32_t width, uint32_t sfcc,
    uint32_t dataFmt);

void Vhwa_m2mLdcUnregisterIsr(Vhwa_M2mLdcInstObj *instObj);
int32_t Vhwa_m2mLdcRegisterIsr(Vhwa_M2mLdcInstObj *instObj);

Vhwa_M2mLdcHandleObj *Vhwa_m2mLdcGetHandleObj(uint32_t cnt);

#ifdef __cplusplus
}
#endif

#endif  /* VHWA_M2M_LDC_PRIV_H_ */
