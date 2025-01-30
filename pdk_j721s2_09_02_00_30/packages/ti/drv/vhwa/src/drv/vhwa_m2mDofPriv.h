/**
 *   Copyright (c) Texas Instruments Incorporated 2018
 *   All rights reserved.
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
 *  \file vhwa_m2mDofPriv.h
 *
 *  \brief DOF Private header file
 *
 */

#ifndef VHWA_M2M_DOF_PRIV_H_
#define VHWA_M2M_DOF_PRIV_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/osal/osal.h>
#include <ti/csl/csl_fvid2_dataTypes.h>
#include <ti/drv/fvid2/include/fvid2_drvMgr.h>

#include <ti/drv/vhwa/src/csl/include/csl_dmpac_hts.h>
#include <ti/drv/vhwa/src/csl/include/csl_lse.h>
#include <ti/drv/vhwa/src/csl/include/csl_dmpactop.h>
#include <ti/drv/vhwa/src/csl/include/csl_dof.h>

#include <ti/drv/vhwa/include/vhwa_m2mDof.h>

#include <ti/drv/vhwa/include/vhwa_common.h>
#include <ti/drv/vhwa/src/drv/vhwa_cfg.h>
#include <ti/drv/vhwa/src/drv/vhwa_utils.h>

#include <vhwa_dmpac_priv.h>

#ifdef __cplusplus
extern "C" {
#endif


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
/** \brief Number of FOCO channel required */
#define VHWA_M2M_DOF_FOCO_CH                    (2U)

/** \brief Maximim number of DMA Channels required for DOF */
#define VHWA_M2M_DOF_MAX_DMA_CH                 (8U)

/**
 *  \anchor Vhwa_DofSl2BuffDepth
 *  \name   VHWA DOF SL2 buffer Depfth
 *  \brief  Sl2 Buffer depth macros for Temporal predictor, Pyramidal predictor,
 *          SOF and FOCO
 *
 *  @{
 */
#define DOF_REF_IMG_SL2_DEPTH            (2U)
#define DOF_CURR_IMG_SL2_DEPTH           (2U)
#define DOF_TEMPORAL_PRED_SL2_DEPTH      (2U)
#define DOF_SOF_SL2_DEPTH                (6U)
#define DOF_PYRAM_PRED_SL2_DEPTH         (2U)
#define DOF_FOCO_SL2_DEPTH               (2U)
#define DOF_OUTPUT_SL2_DEPTH             (4U)
 /** @} */

#define VHWA_DOF_FLOW_ID                        (0x3fffU)

/** \brief Maximum loop count - used while dequeuing UDMA */
#define VHWA_DOF_MAX_WAIT_LOOP_CNT          (5000u)

/** \brief Number of ring entries - we can prime this much memcpy operations */
#define VHWA_M2M_DOF_UDMA_RING_ENTRIES      (16U)
/** \brief Size (in bytes) of each ring entry (Size of pointer - 64-bit) */
#define VHWA_M2M_DOF_UDMA_RING_ENTRY_SIZE   (sizeof(uint64_t))
/** \brief Total ring memory */
#define VHWA_M2M_DOF_UDMA_RING_MEM_SIZE     (VHWA_M2M_DOF_UDMA_RING_ENTRIES * \
                                         VHWA_M2M_DOF_UDMA_RING_ENTRY_SIZE)


/** \brief Total Transfer Record Descriptors per channel */
#define VHWA_M2M_DOF_UDMA_NUM_TR_DESC             (1U)

/**
 *  \brief UDMA TR packet descriptor memory.
 *  This contains the CSL_UdmapCppi5TRPD + Padding to sizeof(CSL_UdmapTR15) +
 *  one Type_15 TR (CSL_UdmapTR15) + one TR response of 4 bytes.
 *  Since CSL_UdmapCppi5TRPD is less than CSL_UdmapTR15, size is just two times
 *  CSL_UdmapTR15 for alignment.
 */
#define VHWA_M2M_DOF_UDMA_TRPD_SIZE               (sizeof(CSL_UdmapTR15) + \
    (sizeof(CSL_UdmapTR15) * VHWA_M2M_DOF_UDMA_NUM_TR_DESC) + \
    (VHWA_M2M_DOF_UDMA_NUM_TR_DESC * 4U)+ \
    (UDMA_CACHELINE_ALIGNMENT - (VHWA_M2M_DOF_UDMA_NUM_TR_DESC * 4U)))



/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

typedef struct
{
    uint32_t                isEnabled;
    /**< Flag to indicate if this channel is enabled or not */

    uint8_t                *trMem;
    /**< Start address of the TR memory */
    uint8_t                *trRespMem;
    /**< Address of the start of the TR response memory */

} Vhwa_M2mDofChParams;

typedef struct
{
    uint64_t    sl2Addr[VHWA_M2M_DOF_MAX_DMA_CH];
    /**< SL2 Write address for all 4 channels */

    uint32_t    sl2NumLines[VHWA_M2M_DOF_MAX_DMA_CH];
    /**< SL2 circular buffer size in terms of lines */
    uint32_t    sl2RefCurPitch;
    /**< SL2 pitch for current and reference frames */
} Vhwa_M2mDofSl2Prms;

typedef struct
{
    uint32_t                 isUsed;
    /**< Flag to indicate if this object is free or not */

    uint32_t                isFocoUsed[DOF_MAX_PYR_LVL_SUPPORTED];
    /**< Flag to indicate if FOCO is used */

    uint32_t                isTempPred;
    /**< FLAG to indicate if temporal predictor is enabled */

    uint32_t                isPyrPred[DOF_MAX_PYR_LVL_SUPPORTED];
    /**< FLAG to indicate if Pyramidal predictor is enabled */

    uint32_t                isSof;
    /**< FLAG to indicate if Sparse Optical flow is enabled */

    Vhwa_M2mDofCreateArgs    createArgs;
    /** Create Arguments */
    Fvid2_DrvCbParams       cbPrms;
    /** Callback parameters, copied locally */
    Dof_ConfScoreParam       confScoreCfg;
    /**< DOF confidence score configuration parameters */
    CSL_DofBufParams         dofBuffPrams;
    /**< DOF Buffer configuration parameters */

    Vhwa_M2mDofChParams     chPrms[DOF_MAX_PYR_LVL_SUPPORTED]
                                    [VHWA_M2M_DOF_MAX_DMA_CH];
    /**< Pointer to TR Response Descr Memory */

    Vhwa_M2mDofSl2Prms      sl2Prms;
    /**< Sl2 prameters for this handle Memory */

    Dmpac_FocoConfig        focoCfg[DOF_MAX_PYR_LVL_SUPPORTED]
                                    [VHWA_M2M_DOF_FOCO_CH];
    /**< Foco configuration for Current and reference image */

    CSL_LseConfig           lseCfg[DOF_MAX_PYR_LVL_SUPPORTED];
    /**< LSE Configuration for this handle */
    Vhwa_M2mDofPrms         dofPrms;
    /**< DOF Input Configuration for this handle */
    Dof_Config              dofCfg[DOF_MAX_PYR_LVL_SUPPORTED];
    /**< DOF Core Configuration for this handle */
    CSL_DmpacHtsSchConfig        htsCfg[DOF_MAX_PYR_LVL_SUPPORTED];
    /**< HTS Configuration for this handle */
    CSL_DmpacHtsSchConfig        focoHtsCfg[DOF_MAX_PYR_LVL_SUPPORTED];
    /**< FOCO HTS Configuration for this handle */

    uint32_t                hIdx;
    /**< The index of this handle object in the array,
         used in allocating UDMA memory */

    uint32_t                curPyrLvl;
    /**< Current pyramid level being processed */

    uint32_t                nextPyrLvl;
    /**< Next Pyramid level to be processed */

    volatile uint64_t       perfNum;
    /**< Performance number for the last submitted request,
         Used only if GetTimeStamp function pointer is set in
         Create Args */

    Fvid2UtilsLinkListObj   doneQObj;
    /**< Done Queue, queue containing queue objects,
     *   for which DOF processing is completed */
    Fvid2UtilsLinkListObj  *doneQ;
    /**< Pointer to Done Queue */

    uint32_t                numPendReq;
    /**< Count of pending requests */

    Dof_ErrEventParams      eePrms;
    /**< Parameters for the Error Events */
} Vhwa_M2mDofHandleObj;

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
    uint32_t                pyrLvl;
    /**< Pyramid level processed */
    Vhwa_M2mDofHandleObj   *hObj;
    /**< Handle Object reference for this Queue object */
    Fvid2_FrameList         inFrmList;
    /**< FVID2 frame list for store list of input frames. */
    Fvid2_FrameList         outFrmList;
    /**< FVID2 frame list for store list of output frames. */
} Vhwa_M2mDofQueueObj;

typedef struct
{
    uint32_t                initDone;
    /**< Flag to know if DOF is initialized or not,
         Essentially init is called or not */
    uint32_t                isSl2AllocDone;
    /**< Flag to know if SL2 memory is allocated or not.
     *   Essentially if Vhwa_m2mDofAllocSl2 is called and is successfully
     *   executed or not.
     *   Driver open is allowed only if this flag is set to UTRUE */
    Vhwa_M2mDofInitParams   initPrms;
    /**< Init paramters, copied here */

    uint32_t                isRegistered;
    /**< Flag to indicate if this instance object is registered to FVID */

    struct Udma_ChObj       utcChObj[VHWA_M2M_DOF_MAX_DMA_CH];
    /**< UDMA Channel object for input channels */
    Udma_ChHandle           utcChHndl[VHWA_M2M_DOF_MAX_DMA_CH];
    /**< UDMA Channel handles for input channels */
    uint32_t                complRingNum[VHWA_M2M_DOF_MAX_DMA_CH];
    /**< UDMA Completion ring number for input channels */
    Udma_RingHandle         fqRingHndl[VHWA_M2M_DOF_MAX_DMA_CH];
    /**< Forward Queue's Ring Handle */
    Udma_RingHandle         cqRingHndl[VHWA_M2M_DOF_MAX_DMA_CH];
    /**< Completion Queue's Ring Handle */
    uint32_t                utcCh[VHWA_M2M_DOF_MAX_DMA_CH];

    SemaphoreP_Handle       lock;
    /**< Semaphore to protect instance object's variables. */
    SemaphoreP_Handle       hwaLock;
    /**< Semaphore to protect DOF HWA.
     *   Only one processing request can be submitted to the DOF HW.
     *   This lock makes sure that except submitted one, all other
     *   requests are waiting on this semaphore.
     *   There is no longer support for submission from ISR and so
     *   no ISR protection in processRequest. */

    uint32_t                openCnt;
    /**< Keeps track of number of opens,
         used to initialize common configuration only once
         for example, enabling DOF at the top level or
            initializing ring accelerator */

    uint32_t                pipeline;
    /**< HTS Pipeline number,
         Same pipeline is used for all handles,
         Allocated on the first open
         Refer to #CSL_HtsPipeline for valid values. */

    HwiP_Handle             intrHndl;
    /**< Handle to Hardware Interrupt */

    Vhwa_M2mDofSl2AllocPrms sl2AllocPrms;
    /**< Sl2 Alloc Parameters */

    Vhwa_M2mDofQueueObj    *actQObj;
    /**< Pointer points to queue object, whose request is submitted to
     * the HW and not yet completed */
    Vhwa_M2mDofHandleObj   *lastHndlObj;
    /**< Pointer to the handle object, whose request was submitted
     *   and completed */

    Dof_SocInfo             socInfo;
    /**< Soc Information, like base address, interrupt number,
     *   INTD irq numbers etc. */
    uint32_t                irqNum;
    /**< Core IRQ Number to be used for DOF,
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
    Vhwa_M2mDofQueueObj     dofQObj[VHWA_M2M_DOF_UDMA_RING_ENTRIES];
    /**< Dof queue objects */

    uint64_t                startAddr;
    /**< start address of the buffer for Dof in SL2 */
    uint32_t                sl2Size;
    /**< Size of SL2 memory allocated to DOF */


    uint32_t                    totalReqCnt;
    /**< Keeps track of total number of Request submitte */
    uint32_t                    totalIntrCnt;
    /**< Keeps track of total number of interrupts,
      *  including error interrupts */
    uint32_t                    errIntrInvalidStatus;
    /**< Keeps track of Error interrupt,
      * when pipeline completion status is 0 */
    uint32_t                    errIntrRepeateIntr;
    /**< Keeps track of Error interrupt,
      * when pipeline completion status is repeated */
    uint32_t                    errInvalidHtsPipelineStatus;
    /**< Keeps track of Invalid HTS Pipline Status Error */
} Vhwa_M2mDofInstObj;


/* ========================================================================== */
/*                           Function Declarations                            */
/* ========================================================================== */

/**
 * \brief   Function to initialize and allocate UTC/UDMA(External) Channels
 *
 **/
int32_t Vhwa_m2mDofUdmaInit(Vhwa_M2mDofInstObj *instObj,
    Vhwa_M2mDofInitParams *initPrms);
int32_t Vhwa_m2mDofUdmaDeInit(const Vhwa_M2mDofInstObj *instObj);

void Vhwa_m2mDofSetTrDesc(const Vhwa_M2mDofInstObj *instObj,
    Vhwa_M2mDofHandleObj *hObj);
int32_t Vhwa_m2mDofStartCh(const Vhwa_M2mDofInstObj *instObj);
int32_t Vhwa_m2mDofStopCh(const Vhwa_M2mDofInstObj *instObj);
int32_t Vhwa_m2mDofSubmitRing(Vhwa_M2mDofInstObj *instObj,
    Vhwa_M2mDofHandleObj *hObj);
int32_t Vhwa_m2mDofAllocUdmaMem(Vhwa_M2mDofHandleObj *hObj);
void vhwaM2mDofSetTRAddress(Vhwa_M2mDofHandleObj *hObj,
                            const Fvid2_FrameList *inFrmList,
                            uint64_t outAddr);

int32_t Vhwa_m2mDofPopRings(Vhwa_M2mDofInstObj *instObj,
    Vhwa_M2mDofHandleObj *hObj, uint32_t pyrLvl);

void Vhwa_m2mDofUnregisterIsr(Vhwa_M2mDofInstObj *instObj);
int32_t Vhwa_m2mDofRegisterIsr(Vhwa_M2mDofInstObj *instObj);

Vhwa_M2mDofHandleObj *Vhwa_m2mDofGetHandleObj(uint32_t cnt);

#ifdef __cplusplus
}
#endif

#endif  /* VHWA_M2M_DOF_PRIV_H_ */
